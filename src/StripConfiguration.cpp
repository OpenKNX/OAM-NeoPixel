#include "StripConfiguration.h"
#include "SerialTimingProfile.h"
#include "HardwareMappingLogic.h"
#include "NeoPixelModule.h"
#include "OpenKNX.h"
#include "knxprod.h"
#include <PhysicalStripConfig.h>
#include <algorithm>

// Constructor
StripConfiguration::StripConfiguration(NeoPixelBusModule* module)
    : _module(module)
{
}

void StripConfiguration::configureFromETS()
{
    // Initialize the OFM-NeoPixel module first
    _module->_neoPixel.init();

    // Track used GPIO pins to avoid conflicts
    std::vector<uint8_t> usedPins;

    // Helper lambda to check if a pin is already used
    auto isPinUsed = [&usedPins](uint8_t pin) {
        return std::find(usedPins.begin(), usedPins.end(), pin) != usedPins.end();
    };

    // Pins reserved by the KNX/BCU subsystem must NEVER be driven by a LED strip.
    auto isReservedPin = [](uint8_t pin) -> bool {
#if defined(KNX_UART_TX_PIN)
        if (pin == (uint8_t)KNX_UART_TX_PIN) return true;
#endif
#if defined(KNX_UART_RX_PIN)
        if (pin == (uint8_t)KNX_UART_RX_PIN) return true;
#endif
#if defined(SAVE_INTERRUPT_PIN)
        if (pin == (uint8_t)SAVE_INTERRUPT_PIN) return true;
#endif
        return false;
    };

    // 1) Determine number of strips from ETS (match the runtime strip limit)
    const uint8_t maxStrips = std::max<uint8_t>(1, std::min<uint8_t>(NEOPIXEL_MAX_PHYSICAL_STRIPS, ParamNEO_NEONumberOfLEDStrips));
    _module->_totalLeds = 0;

    // CRITICAL: Delete old PhysicalStrip objects before clearing vector!
    // This prevents memory leaks and deregisters old DMA handlers
    for (auto* strip : _module->_physicalStrips)
    {
        if (strip) delete strip;
    }
    _module->_physicalStrips.clear();

    // Clear virtual strip configuration
    _module->_virtualStripConfiguration.clear();

    // PRE-SCAN: Mark all manually configured GPIO pins as used BEFORE auto-allocation starts
    // This prevents auto-allocation from using pins that are manually configured on other strips
    logInfoP("Pre-scanning %d strips for GPIO pin conflicts...", maxStrips);
    std::vector<uint8_t> conflictedStrips; // Track strips with conflicts

    for (uint8_t i = 0; i < maxStrips; ++i)
    {
        _module->setChannelIndex(i);
        uint8_t& _channelIndex = _module->_channelIndex; // Reference to module member for ETS parameter macros

        const uint16_t pixels = (uint16_t)ParamNEOSTRIP_NEOLength;
        if (pixels == 0) continue; // Skip strips with 0 LEDs

        const uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const LedProtocol proto = mapProtocol(ledTypeParam);
        const bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;

        bool hasConflict = false;

        if (isSpiProtocol(proto))
        {
            uint8_t mosiGpio, sckGpio;

            if (gpioManualConfig)
            {
                // Manual: User-configured GPIOs
                mosiGpio = (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO;
                sckGpio = (uint8_t)ParamNEOSTRIP_NEOClockGPIO;
            }
            else
            {
                // Hardware: Get from hardware port mapping
                mosiGpio = getActualDataGpio(i, (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO, false);
                sckGpio = getActualClockGpio(i, (uint8_t)ParamNEOSTRIP_NEOClockGPIO, false);
            }

            // Skip if no valid pins (255 = not configured)
            if (mosiGpio == 255 || sckGpio == 255) continue;

            // Check SCK conflict
            if (isPinUsed(sckGpio))
            {
                logErrorP("Strip %d: SCK GPIO %d CONFLICT - already used by another strip!", i, sckGpio);
                hasConflict = true;
            }
            else
            {
                usedPins.push_back(sckGpio);
                logInfoP("Strip %d: Registered SCK GPIO %d%s", i, sckGpio, gpioManualConfig ? " (Manual)" : " (HW)");
            }

            // Check MOSI conflict
            if (isPinUsed(mosiGpio))
            {
                logErrorP("Strip %d: MOSI GPIO %d CONFLICT - already used by another strip!", i, mosiGpio);
                hasConflict = true;
            }
            else
            {
                usedPins.push_back(mosiGpio);
                logInfoP("Strip %d: Registered MOSI GPIO %d%s", i, mosiGpio, gpioManualConfig ? " (Manual)" : " (HW)");
            }
        }
        else
        {
            // 1-Wire: Data GPIO only
            uint8_t dataGpio;

            if (gpioManualConfig)
            {
                dataGpio = (uint8_t)ParamNEOSTRIP_NEODataGPIO;
            }
            else
            {
                dataGpio = getActualDataGpio(i, (uint8_t)ParamNEOSTRIP_NEODataGPIO, false);
            }

            // Skip if not configured
            if (dataGpio == 255) continue;

            // Check conflict
            if (isPinUsed(dataGpio))
            {
                logErrorP("Strip %d: Data GPIO %d CONFLICT - already used by another strip!", i, dataGpio);
                hasConflict = true;
            }
            else
            {
                usedPins.push_back(dataGpio);
                logInfoP("Strip %d: Registered Data GPIO %d%s", i, dataGpio, gpioManualConfig ? " (Manual)" : " (HW)");
            }
        }

        // Mark this strip as conflicted
        if (hasConflict)
        {
            conflictedStrips.push_back(i);
        }
    }

    if (!conflictedStrips.empty())
    {
        logErrorP("GPIO PIN CONFLICTS detected! %d strip(s) will be SKIPPED:", (int)conflictedStrips.size());
        for (auto stripIdx : conflictedStrips)
        {
            logErrorP("  - Strip %d (GPIO conflict)", stripIdx);
        }
        _module->setErrorBlink(NEO_ERROR_GPIO_CONFLICT); // 3× blink
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // PRE-SCAN: Relay GPIO Conflict Detection
    // Check external relay GPIOs to prevent conflicts with strips
    // ═══════════════════════════════════════════════════════════════════════════
    uint8_t relayCount = std::min<uint8_t>(4, ParamNEO_NEOExternalRelayCount);
    logInfoP("Relay pre-scan: Checking %d relay(s) for GPIO conflicts...", relayCount);

    for (uint8_t r = 0; r < relayCount; ++r)
    {
        uint8_t portIndex;
        switch (r)
        {
            case 0: portIndex = (uint8_t)ParamNEO_NEOExternalRelay1Port; break;
            case 1: portIndex = (uint8_t)ParamNEO_NEOExternalRelay2Port; break;
            case 2: portIndex = (uint8_t)ParamNEO_NEOExternalRelay3Port; break;
            case 3: portIndex = (uint8_t)ParamNEO_NEOExternalRelay4Port; break;
            default: portIndex = 15; break;
        }

        // Check if hardware port is selected (not 15=Dummy, not 10=Manual)
        if (!isHardwarePortSelected(portIndex))
        {
            logInfoP("Relay %d: No valid port selected (port=%d), skipping", r + 1, portIndex);
            continue;
        }

        // Map port index to actual GPIO
        uint8_t relayGpio = mapPortIndexToGpio(portIndex);

        // Skip if invalid GPIO
        if (relayGpio == 255)
        {
            logWarningP("Relay %d: Invalid GPIO mapping for port %d", r + 1, portIndex);
            continue;
        }

        // Check for conflict
        if (isPinUsed(relayGpio))
        {
            logErrorP("Relay %d: GPIO %d CONFLICT - already used by strip!", r + 1, relayGpio);
            // Don't add to conflictedStrips since relays aren't strips
            // But mark this as a critical error
        }
        else
        {
            usedPins.push_back(relayGpio);
            logInfoP("Relay %d: Registered GPIO %d (port %d)", r + 1, relayGpio, portIndex);
        }
    }

    logInfoP("Pre-scan complete: %d pins registered (%d strip conflicts)", (int)usedPins.size(), (int)conflictedStrips.size());

    // 2) Create all physical strips in configuration order (auto-allocation will skip pre-marked pins)
    for (uint8_t i = 0; i < maxStrips; ++i)
    {
        _module->setChannelIndex(i);
        uint8_t& _channelIndex = _module->_channelIndex; // Reference to module member for ETS parameter macros

        // Skip strips with GPIO conflicts
        if (std::find(conflictedStrips.begin(), conflictedStrips.end(), i) != conflictedStrips.end())
        {
            logErrorP("Strip %d: SKIPPED due to GPIO conflict", i);
            continue;
        }

        const uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const LedProtocol proto = mapProtocol(ledTypeParam);

        // Get color order from ETS configuration
        ColorOrder order = mapColorOrder((uint8_t)ParamNEOSTRIP_NEOColourOrder);

        // Get voltage from ETS configuration (0=5V, 1=12V, 2=24V)
        uint8_t voltageParam = (uint8_t)ParamNEOSTRIP_NEOVoltage;
        uint8_t voltage = (voltageParam == 0) ? 5 : (voltageParam == 1) ? 12
                                                                        : 24;

        const uint16_t pixels = (uint16_t)ParamNEOSTRIP_NEOLength;

        // DEBUG: Log LED count for each strip
        logInfoP("Strip %d: Reading ParamNEOSTRIP_NEOLength = %d (channelIndex=%d)", i, pixels, _channelIndex);

        // Skip strips with 0 LEDs configured
        if (pixels == 0)
        {
            logInfoP("Strip %d: Skipped (0 LEDs configured)", i);
            continue;
        }

        // Determine if this is an SPI protocol and get appropriate GPIO pins
        PhysicalStrip* phys = nullptr;
        if (isSpiProtocol(proto))
        {
            uint8_t mosiGpio, sckGpio;
            bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;

            if (gpioManualConfig)
            {
                // Manual: Use ETS configured GPIO pins
                mosiGpio = (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO;
                sckGpio = (uint8_t)ParamNEOSTRIP_NEOClockGPIO;
            }
            else
            {
                // Hardware: Get from hardware port mapping
                mosiGpio = getActualDataGpio(i, (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO, false);
                sckGpio = getActualClockGpio(i, (uint8_t)ParamNEOSTRIP_NEOClockGPIO, false);

                // Skip if hardware port not configured (255 = dummy/not selected)
                if (mosiGpio == 255 || sckGpio == 255)
                {
                    logInfoP("SPI Strip %d: Skipped (hardware port not configured)", i);
                    continue;
                }
            }

            // Read SPI frequency from ETS (if manual config enabled)
            uint32_t spiFrequency = 10000000; // Default: 10 MHz
            bool spiClkManual = (ParamNEOSTRIP_NEOSPICLKMode == 1);

            if (spiClkManual)
            {
                // Map ETS enum value to frequency in Hz
                uint8_t spiClkValue = (uint8_t)ParamNEOSTRIP_NEOSPICLK;
                switch (spiClkValue)
                {
                    case 0: spiFrequency = 1000000; break;   // 1 MHz
                    case 1: spiFrequency = 2000000; break;   // 2 MHz
                    case 2: spiFrequency = 4000000; break;   // 4 MHz
                    case 3: spiFrequency = 8000000; break;   // 8 MHz
                    case 4: spiFrequency = 12000000; break;  // 12 MHz
                    case 5: spiFrequency = 16000000; break;  // 16 MHz
                    case 6: spiFrequency = 20000000; break;  // 20 MHz
                    default: spiFrequency = 10000000; break; // Fallback: 10 MHz
                }
            }

            // Pins reserved by the KNX/BCU subsystem must NEVER be driven by a LED strip.
            if (isReservedPin(mosiGpio) || isReservedPin(sckGpio))
            {
                logErrorP("Strip %d: REFUSED - SPI GPIO(s) collide with reserved KNX/system pins (MOSI=%d, SCK=%d). Strip disabled to protect KNX bus.",
                          i, mosiGpio, sckGpio);
                continue; // Skip this strip completely
            }

            // Use manager directly for frequency parameter (NeoPixel wrapper doesn't expose this overload yet)
            auto mgr = _module->_neoPixel.getManager();
            phys = mgr->addSpiStrip(mosiGpio, sckGpio, pixels, proto, order, spiFrequency);

            // Configure SPI strip-specific settings BEFORE init()
            // The config is created in the PhysicalStrip constructor
            if (phys)
            {
                auto* cfg = phys->getConfig();
                SpiStripConfig* spiCfg = cfg && cfg->isSpiConfig() ? static_cast<SpiStripConfig*>(cfg) : nullptr;

                if (spiCfg)
                {
                    // Read "Überspringe erste LEDs" parameter for this strip
                    uint16_t skipLeds = ParamNEOSTRIP_NEOSkipFirstLEDs;

                    // Store skipFirstLeds in config (VirtualStrip will force them to black)
                    spiCfg->setSkipFirstLeds(skipLeds);

                    // Set dummy LED mode based on skipLeds parameter:
                    // - skipLeds=0: setDummyLedMode(0) - all LEDs active
                    // - skipLeds>0: setDummyLedMode(1) - sacrifice LED#0, then (skipLeds-1) LEDs forced black
                    if (skipLeds > 0)
                    {
                        spiCfg->setDummyLedMode(1); // Physical dummy LED (sacrifice LED#0)
                        logInfoP("SPI Strip %d: Skip mode enabled - %d LEDs will stay black (1 dummy + %d forced)", i, skipLeds, skipLeds - 1);
                    }
                    else
                    {
                        spiCfg->setDummyLedMode(0); // No dummy LED
                        logInfoP("SPI Strip %d: All LEDs active (no skipping)", i);
                    }

                    // Apply the configuration (prepares for init)
                    phys->applyConfig();
                    logInfoP("SPI Strip %d: Configuration applied successfully", i);

                    // Configure power limiting for this strip
                    // Level 1: Main mode (0=Disabled, 1=UseGlobal, 2=Custom)
                    uint8_t powerMainMode = (uint8_t)ParamNEOSTRIP_NEOpowerLimitCombined;

                    if (powerMainMode == 0)
                    {
                        // Disabled
                        spiCfg->setPowerLimitMode(0);
                        logInfoP("SPI Strip %d: Power limiting disabled", i);
                    }
                    else if (powerMainMode == 1)
                    {
                        // Use Global
                        spiCfg->setPowerLimitMode(1);
                        logInfoP("SPI Strip %d: Using global power limiting settings", i);
                    }
                    else if (powerMainMode == 2)
                    {
                        // Mode 2: Custom (Fixed value)
                        spiCfg->setPowerLimitMode(2);
                        uint16_t maxCurrent = ParamNEOSTRIP_NEOcurrentPerChannel;
                        spiCfg->setMaxCurrentMa(maxCurrent);
                        logInfoP("SPI Strip %d: Custom power limit = %d mA (fixed)", i, maxCurrent);
                    }
                    else if (powerMainMode == 3)
                    {
                        // Mode 3: Custom (Per LED)
                        spiCfg->setPowerLimitMode(3);
                        uint8_t currentPerLed = ParamNEOSTRIP_NEOcurrentPerLED;
                        spiCfg->setCurrentPerLedMa(currentPerLed);
                        logInfoP("SPI Strip %d: Custom power limit = %d mA/LED (calculated: %d mA total)",
                                 i, currentPerLed, currentPerLed * pixels);
                    }

                    // Load ABL parameters (for Mode 2 or 3)
                    if (powerMainMode == 2 || powerMainMode == 3)
                    {
                        uint8_t autoBrLimit = (uint8_t)ParamNEOSTRIP_NEOautoBrightnessLimit;
                        uint8_t threshold = (uint8_t)ParamNEOSTRIP_NEOpowerLimitThreshold;
                        uint8_t slewRate = (uint8_t)ParamNEOSTRIP_NEOablSlewRatePercent;
                        spiCfg->setAutoBrightnessLimit(autoBrLimit);
                        spiCfg->setPowerLimitThreshold(threshold);
                        spiCfg->setAblSlewRate(slewRate);
                        logInfoP("SPI Strip %d: ABL settings = autoBrLimit:%d%%, threshold:%d%%, slew:%d%%",
                                 i, autoBrLimit, threshold, slewRate);
                    }
                }
                else
                {
                    logErrorP("SPI Strip %d: Failed to cast config to SpiStripConfig!", i);
                    _module->setErrorBlink(NEO_ERROR_STRIP_FAILED); // 4× blink
                }
            }
            else
            {
                logErrorP("SPI Strip %d: addSpiStrip returned nullptr!", i);
                _module->setErrorBlink(NEO_ERROR_STRIP_FAILED); // 4× blink
            }

            logInfoP("SPI Strip %d: %d LEDs, MOSI=%d, SCK=%d, Protocol=%s, ColorOrder=%s, Freq=%d Hz%s",
                     i, pixels, mosiGpio, sckGpio, NeoPixelBusModule::getProtocolName(proto), NeoPixelBusModule::getColorOrderName(order),
                     spiFrequency, gpioManualConfig ? " (Manual)" : " (Auto)");
        }
        else
        {
            // 1-Wire protocols use Data GPIO
            uint8_t dataGpioPin;
            bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;

            if (gpioManualConfig)
            {
                // Manual: Use ETS configured GPIO pin
                dataGpioPin = (uint8_t)ParamNEOSTRIP_NEODataGPIO;
            }
            else
            {
                // Hardware: Get from hardware port mapping
                dataGpioPin = getActualDataGpio(i, (uint8_t)ParamNEOSTRIP_NEODataGPIO, false);

                // Skip if hardware port not configured (255 = dummy/not selected)
                if (dataGpioPin == 255)
                {
                    logInfoP("1-Wire Strip %d: Skipped (hardware port not configured)", i);
                    continue;
                }
            }

            // Pins reserved by the KNX/BCU subsystem must NEVER be driven by a LED strip.
            if (isReservedPin(dataGpioPin))
            {
                logErrorP("Strip %d: REFUSED - Data GPIO %d collides with reserved KNX/system pin. Strip disabled to protect KNX bus.",
                          i, dataGpioPin);
                continue; // Skip this strip completely
            }

            phys = _module->_neoPixel.addStrip(dataGpioPin, pixels, proto, order);
            logInfoP("1-Wire Strip %d: %d LEDs, GPIO=%d, Protocol=%s, ColorOrder=%s%s",
                     i, pixels, dataGpioPin, NeoPixelBusModule::getProtocolName(proto), NeoPixelBusModule::getColorOrderName(order),
                     gpioManualConfig ? " (Manual)" : " (Auto)");

            // Configure skipFirstLeds for serial strips (all strip types support this now)
            if (phys)
            {
                auto* cfg = phys->getConfig();
                if (cfg)
                {
                    uint16_t skipLeds = ParamNEOSTRIP_NEOSkipFirstLEDs;
                    cfg->setSkipFirstLeds(skipLeds);

                    if (skipLeds > 0)
                    {
                        logInfoP("1-Wire Strip %d: Skip mode enabled - first %d LEDs will stay black", i, skipLeds);
                    }

                    // Configure power limiting for this strip
                    // Level 1: Main mode (0=Disabled, 1=UseGlobal, 2=Custom)
                    uint8_t powerMainMode = (uint8_t)ParamNEOSTRIP_NEOpowerLimitCombined;

                    if (powerMainMode == 0)
                    {
                        // Disabled
                        cfg->setPowerLimitMode(0);
                        logInfoP("1-Wire Strip %d: Power limiting disabled", i);
                    }
                    else if (powerMainMode == 1)
                    {
                        // Use Global
                        cfg->setPowerLimitMode(1);
                        logInfoP("1-Wire Strip %d: Using global power limiting settings", i);
                    }
                    else if (powerMainMode == 2)
                    {
                        // Mode 2: Custom (Fixed value)
                        cfg->setPowerLimitMode(2);
                        uint16_t maxCurrent = ParamNEOSTRIP_NEOcurrentPerChannel;
                        cfg->setMaxCurrentMa(maxCurrent);
                        logInfoP("1-Wire Strip %d: Custom power limit = %d mA (fixed)", i, maxCurrent);
                    }
                    else if (powerMainMode == 3)
                    {
                        // Mode 3: Custom (Per LED)
                        cfg->setPowerLimitMode(3);
                        uint8_t currentPerLed = ParamNEOSTRIP_NEOcurrentPerLED;
                        cfg->setCurrentPerLedMa(currentPerLed);
                        logInfoP("1-Wire Strip %d: Custom power limit = %d mA/LED (calculated: %d mA total)",
                                 i, currentPerLed, currentPerLed * pixels);
                    }

                    // Load ABL parameters (for Mode 2 or 3)
                    if (powerMainMode == 2 || powerMainMode == 3)
                    {
                        uint8_t autoBrLimit = (uint8_t)ParamNEOSTRIP_NEOautoBrightnessLimit;
                        uint8_t threshold = (uint8_t)ParamNEOSTRIP_NEOpowerLimitThreshold;
                        uint8_t slewRate = (uint8_t)ParamNEOSTRIP_NEOablSlewRatePercent;
                        cfg->setAutoBrightnessLimit(autoBrLimit);
                        cfg->setPowerLimitThreshold(threshold);
                        cfg->setAblSlewRate(slewRate);
                        logInfoP("1-Wire Strip %d: ABL settings = autoBrLimit:%d%%, threshold:%d%%, slew:%d%%",
                                 i, autoBrLimit, threshold, slewRate);
                    }
                }
            }
        }

        if (phys)
        {
            // Set voltage for power calculation (must be done BEFORE init)
            phys->setVoltage(voltage);
            logInfoP("Strip %d: Voltage set to %dV (for power calculation)", i, voltage);

            _module->_totalLeds += pixels;
            _module->_physicalStrips.push_back(phys);

            // Kept in step with the active twin in NeoPixelModule.cpp: value 0 means "use
            // the chip profile", any other value scales that profile to the chosen bit rate.
            static const uint16_t timingFreqTable[16] = {
                800, 960, 640, 680, 720, 760, 840, 880, 920, 750, 765, 770, 775, 780, 785, 790};
            const uint8_t timingSel = (uint8_t)ParamNEOSTRIP_NEOTiming & 0x0F;

            if (timingSel != 0)
            {
                const uint16_t freqKhz = timingFreqTable[timingSel];
                SerialTiming::Profile base = SerialTiming::profileFor(proto);
                if (base.t1h == 0) base = SerialTiming::profileFor(LedProtocol::WS2812B);
                const SerialTiming::Profile bent = SerialTiming::scaledTo(base, (uint32_t)freqKhz * 1000UL);
                if (phys->setCustomTiming(bent.t0h, bent.t0l, bent.t1h, bent.t1l, bent.resetUs))
                    logInfoP("Strip %d: Timing override = %u kHz (T0H=%u ns, T1H=%u ns)",
                             i, freqKhz, bent.t0h, bent.t1h);
            }

            // Configure color correction for this strip (only if Master-Checkbox is ON)
            bool colorCalibrationMaster = (bool)ParamNEOSTRIP_NEOColorCalibrationMaster;
            if (colorCalibrationMaster && ((bool)ParamNEOSTRIP_NEOGammaCorrection || true))
            {
                _module->configureColorCorrection();
                _module->updateColorCorrection(); // Set correction parameters on VirtualStrip once

                auto* cfg = phys->getConfig();
                if (cfg)
                {
                    // Configure gamma correction in the physical strip config
                    if (_module->_gammaCorrectionEnabled)
                    {
                        cfg->setGammaCorrection(_module->_gammaValue);
                        logInfoP("Strip %d: Gamma correction enabled (value=%.2f)", i, _module->_gammaValue);
                    }

                    // Configure white balance in the physical strip config
                    if (_module->_whiteBalanceEnabled)
                    {
                        cfg->setWhiteBalance(_module->_whiteBalanceRed, _module->_whiteBalanceGreen, _module->_whiteBalanceBlue);
                        logInfoP("Strip %d: White balance enabled (R:%d G:%d B:%d)", i, _module->_whiteBalanceRed, _module->_whiteBalanceGreen, _module->_whiteBalanceBlue);
                    }
                }

                logInfoP("Strip %d: Color correction enabled - Gamma=%s(%.1f), WhiteBalance=%s(R:%d G:%d B:%d)",
                         i,
                         _module->_gammaCorrectionEnabled ? "ON" : "OFF", _module->_gammaValue,
                         _module->_whiteBalanceEnabled ? "ON" : "OFF", _module->_whiteBalanceRed, _module->_whiteBalanceGreen, _module->_whiteBalanceBlue);
            }

            // Configure strip options (swap mode and skip LEDs)
            _module->configureStripOptions();
            if (_module->_swapMode > 0 || _module->_skipFirstLeds > 0)
            {
                logInfoP("Strip %d: Strip options - SwapMode=%d, SkipFirstLEDs=%d",
                         i, _module->_swapMode, _module->_skipFirstLeds);
            }
        }
        else
        {
            logErrorP("Failed to create strip %d", i);
            _module->setErrorBlink(NEO_ERROR_STRIP_FAILED); // 4× blink
        }
    }

    logInfoP("Created %d physical strips with total %d LEDs", maxStrips, _module->_totalLeds);

    // 3) Configure virtual strip order from ETS parameters
    configureVirtualStripOrder();

    // 4) Configure segments once, now that we know the virtual strip layout
    // Segments are defined on the virtual strip
    if (_module->_totalLeds > 0)
    {
        _module->configureSegments();
        if (_module->_numberOfSegments > 0)
        {
            logInfoP("Configured %d segments for virtual strip (%d total LEDs)", _module->_numberOfSegments, _module->_totalLeds);
        }
    }

    // 5) Create virtual strip using configured order and apply segments
    if (!_module->_physicalStrips.empty() && _module->_totalLeds > 0)
    {
        createVirtualStripWithOrder();

        // Configure effects for segments (now that virtual strip and segments exist)
        if (_module->_numberOfSegments > 0)
        {
            _module->configureEffects();
            logInfoP("Applied effects to %d segments", _module->_numberOfSegments);
        }

        // Configure power management using OFM PowerManager
        _module->configurePowerManagement();

        // Apply initial color correction to ensure ETS settings take effect immediately
        if (_module->_gammaCorrectionEnabled || _module->_whiteBalanceEnabled || _module->_swapMode > 0)
        {
            _module->forceColorCorrectionUpdate();
            logInfoP("Applied initial color correction settings");
        }
    }

    logInfoP("Configuration complete: %d physical strips, %d total LEDs, %d segments",
             maxStrips, _module->_totalLeds, _module->_numberOfSegments);
}
// ============================================================================
// Helper functions
// ============================================================================

LedProtocol StripConfiguration::mapProtocol(uint8_t p)
{
    // Map ETS LED type enum to OFM's LedProtocol enum
    // Based on NEOLedType enumeration from NeoPixel.share.xml
    switch (p)
    {
        case 0: return LedProtocol::WS2812B;        // WS2812B
        case 1: return LedProtocol::WS2805_RGBCCT;  // WS2805 RGBCCT (5-channel)
        case 2: return LedProtocol::WS2811;         // WS2811
        case 3: return LedProtocol::WS2813;         // WS2813
        case 4: return LedProtocol::SK6812;         // SK6812
        case 5: return LedProtocol::APA102;         // APA102
        case 6: return LedProtocol::SK9822;         // SK9822
        case 7: return LedProtocol::WS2812B;        // WS281x (generic, mapped to WS2812B)
        case 8: return LedProtocol::SK6812;         // SK6812/WS2814 (RGBW)
        case 9: return LedProtocol::TM1814;         // TM1814
        case 10: return LedProtocol::WS2811;        // WS2812_400kHz (mapped to WS2811)
        case 11: return LedProtocol::TM1829;        // TM1829
        case 12: return LedProtocol::UCS8903;       // UCS8903 (16 bit/channel)
        case 13: return LedProtocol::APA106;        // APA106/PL9823
        case 14: return LedProtocol::TM1914;        // TM1914
        case 15: return LedProtocol::FW1906;        // FW1906 (6 channels)
        case 16: return LedProtocol::UCS8904;       // UCS8904 (RGBW, 16 bit/channel)
        case 17: return LedProtocol::WS2805_RGBCCT; // WS2805_RGBCW (same as RGBCCT)
        case 18: return LedProtocol::SM16825;       // SM16825 (5ch, 16 bit/channel)
        case 19: return LedProtocol::WS2811;        // WS2811_WHITE (single channel, mapped to WS2811)
        case 20: return LedProtocol::WS2812B;       // WS281x_WWA (warm/cool white, mapped to WS2812B)
        case 21: return LedProtocol::WS2801;        // WS2801
        case 22: return LedProtocol::LPD8806;       // LPD8806
        case 23: return LedProtocol::LPD6803;       // LPD6803
        case 24: return LedProtocol::P9813;         // P9813
        case 25: return LedProtocol::APA102_CLONE;  // APA102-Clone
        case 30: return LedProtocol::SK6812_RGBCCT; // SK6812 RGBCCT (5ch)
        case 31: return LedProtocol::WS2814_RGBCCT; // WS2814 RGBCCT (5ch)
        case 99: return LedProtocol::WS2812B;       // CUSTOM (default to WS2812B)

        default: return LedProtocol::WS2812B; // Default to most common
    }
}

ColorOrder StripConfiguration::mapColorOrder(uint8_t c)
{
    // Map ETS ColorOrder enum to OFM's ColorOrder enum
    // Based on NEOColourOrder enumeration from NeoPixelBus.share.xml
    switch (c)
    {
        case 0: return ColorOrder::GRB;  // GRB (WS2812/SK6812 standard)
        case 1: return ColorOrder::RGB;  // RGB (standard RGB order)
        case 2: return ColorOrder::BRG;  // BRG (rare configuration)
        case 3: return ColorOrder::RBG;  // RBG (some LED clones)
        case 4: return ColorOrder::BGR;  // BGR (APA102/SK9822 standard)
        case 5: return ColorOrder::GBR;  // GBR (some WS2812B clones)
        case 6: return ColorOrder::RGBW; // RGBW (4-channel, RGB+White)
        case 7:
            return ColorOrder::GRBW; // GRBW (4-channel, SK6812 standard)
        // 5-Channel Color Orders (RGBCCT = RGB + Warm White + Cool White)
        case 8: return ColorOrder::RGBCCT;  // RGBCCT (5-channel)
        case 9: return ColorOrder::GRBCCT;  // GRBCCT (5-channel, likely standard)
        case 10: return ColorOrder::RGBCTW; // RGBCTW (5-channel, CW first)
        case 11: return ColorOrder::GRBCTW; // GRBCTW (5-channel, CW first)
        default: return ColorOrder::GRB;    // Default to WS2812/SK6812 standard
    }
}

void StripConfiguration::configureVirtualStripOrder()
{
    _module->_virtualStripConfiguration.clear();

    // Check if we have duplicate configuration (user may have made mistakes)
    bool hasDuplicates = ParamNEO_VirtualStripHasDuplicates;
    if (hasDuplicates)
    {
        logWarningP("Virtual strip configuration has duplicates - using default order (simple sequential order)");
        // Fall back to simple sequential order
        createDefaultVirtualStripOrder();
        return;
    }

    logInfoP("Configuring virtual strip order from ETS parameters");

    constexpr size_t kMaxVirtualStripPositions = NEOPIXEL_MAX_PHYSICAL_STRIPS;

    // Read the virtual strip position parameters (1-8 positions for 1-8 strips)
    // Value 0 = not used, Values 1-8 = physical strip index (1-based)
    uint8_t positions[kMaxVirtualStripPositions] = {
        static_cast<uint8_t>(ParamNEO_VirtualStripPos1),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos2),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos3),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos4),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos5),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos6),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos7),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos8)};

    // Read the start positions (calculated by ETS JavaScript)
    uint16_t startPositions[kMaxVirtualStripPositions] = {
        static_cast<uint16_t>(ParamNEO_VirtualStripStart1),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart2),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart3),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart4),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart5),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart6),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart7),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart8)};

    // Build the virtual strip configuration based on positions
    for (size_t pos = 0; pos < kMaxVirtualStripPositions; pos++)
    {
        uint8_t physStripIndex = positions[pos];

        // Skip unused positions (value 0) or invalid indices
        if (physStripIndex == 0 || physStripIndex > _module->_physicalStrips.size())
        {
            continue;
        }

        // Convert from 1-based to 0-based indexing
        physStripIndex -= 1;

        // Get the LED count for this physical strip
        if (physStripIndex < _module->_physicalStrips.size() && _module->_physicalStrips[physStripIndex])
        {
            uint16_t ledCount = _module->_physicalStrips[physStripIndex]->getLedCount();
            uint16_t virtualStart = startPositions[pos];

            // Convert from 1-based (ETS user-friendly) to 0-based (internal indexing)
            // ETS shows "Start LED = 1" but internally we need offset 0
            uint16_t virtualStartZeroBased = (virtualStart > 0) ? (virtualStart - 1) : 0;

            _module->_virtualStripConfiguration.emplace_back(physStripIndex, virtualStartZeroBased, ledCount);

            logInfoP("Virtual position %d: Physical strip %d (%d LEDs) starts at virtual position %d (ETS: %d)",
                     static_cast<int>(pos + 1), physStripIndex + 1, ledCount, virtualStartZeroBased, virtualStart);
        }
        else
        {
            logErrorP("Invalid physical strip index %d at virtual position %d", physStripIndex + 1, pos + 1);
        }
    }

    // Validate the configuration
    if (_module->_virtualStripConfiguration.empty())
    {
        logWarningP("No valid virtual strip configuration found - using default order");
        createDefaultVirtualStripOrder();
    }
    else
    {
        logInfoP("Virtual strip configuration complete: %d physical strips mapped",
                 static_cast<int>(_module->_virtualStripConfiguration.size()));
    }
}

void StripConfiguration::createVirtualStripWithOrder()
{
    if (_module->_physicalStrips.empty() || _module->_totalLeds == 0 || _module->_virtualStripConfiguration.empty())
    {
        logErrorP("Cannot create virtual strip: insufficient configuration");
        _module->setErrorBlink(NEO_ERROR_VSTRIP_FAILED); // 5× blink
        return;
    }

    // Determine if any physical strip requires RGBW (4 bytes) or RGBCCT (5 bytes per LED)
    bool needsRGBW = false;
    bool needsRGBCCT = false;
    for (const auto* phys : _module->_physicalStrips)
    {
        if (phys)
        {
            ColorOrder order = phys->getColorOrder();
            // Check for 5-channel first (RGBCCT, GRBCCT, RGBCTW, GRBCTW)
            if (order == ColorOrder::RGBCCT || order == ColorOrder::GRBCCT ||
                order == ColorOrder::RGBCTW || order == ColorOrder::GRBCTW)
            {
                needsRGBCCT = true;
                break; // 5-channel takes priority
            }
            // Check for 4-channel (RGBW, GRBW, etc.)
            if (order >= ColorOrder::RGBW)
            {
                needsRGBW = true;
            }
        }
    }

    // Create virtual strip: RGBCCT (5 bytes) > RGBW (4 bytes) > RGB (3 bytes)
    ColorOrder virtualOrder;
    if (needsRGBCCT)
    {
        virtualOrder = ColorOrder::GRBCCT; // Standard 5-channel order
    }
    else if (needsRGBW)
    {
        virtualOrder = ColorOrder::RGBW;
    }
    else
    {
        virtualOrder = ColorOrder::RGB;
    }

    _module->_virtualStrip = _module->_neoPixel.addVirtualStrip(_module->_totalLeds, virtualOrder);
    if (!_module->_virtualStrip)
    {
        logErrorP("Failed to create virtual strip");
        _module->setErrorBlink(NEO_ERROR_VSTRIP_FAILED); // 5× blink
        return;
    }

    if (needsRGBCCT)
    {
        logInfoP("VirtualStrip created with RGBCCT support (5 bytes/LED) for WS2805/RGBCCT physical strips");
    }
    else if (needsRGBW)
    {
        logInfoP("VirtualStrip created with RGBW support (4 bytes/LED) for RGBW physical strips");
    }

    auto mgr = _module->_neoPixel.getManager();
    if (!mgr)
    {
        logErrorP("NeoPixelManager not available for virtual strip creation");
        _module->setErrorBlink(NEO_ERROR_VSTRIP_FAILED); // 5× blink
        return;
    }

    // Attach physical strips to virtual strip according to the configured order
    for (const auto& config : _module->_virtualStripConfiguration)
    {
        if (config.physicalStripIndex >= _module->_physicalStrips.size())
        {
            logErrorP("Invalid physical strip index %d in virtual configuration", config.physicalStripIndex);
            continue;
        }

        PhysicalStrip* phys = _module->_physicalStrips[config.physicalStripIndex];
        if (!phys)
        {
            logErrorP("Physical strip %d is null", config.physicalStripIndex);
            continue;
        }

        // Apply skip offset if configured for this strip
        uint8_t oldChannelIndex = _module->getChannelIndex();
        _module->setChannelIndex(config.physicalStripIndex);
        uint8_t _channelIndex = config.physicalStripIndex; // Local variable for ETS parameter macros
        uint16_t skipLeds = ParamNEOSTRIP_NEOSkipFirstLEDs;
        _module->setChannelIndex(oldChannelIndex);

        uint16_t effectiveLeds = config.ledCount;
        uint16_t attachOffset = config.virtualStartPosition;

        // Apply skip offset if configured
        if (skipLeds > 0 && skipLeds < effectiveLeds)
        {
            logDebugP("Strip %d: Skipping first %d LEDs, effective range: %d-%d",
                      config.physicalStripIndex, skipLeds, skipLeds, effectiveLeds - 1);
        }

        if (mgr->attachPhysicalToVirtual(_module->_virtualStrip, phys, attachOffset))
        {
            logInfoP("Attached Physical Strip %d to Virtual Strip at position %d (length %d, skip %d)",
                     config.physicalStripIndex, attachOffset, effectiveLeds, skipLeds);
        }
        else
        {
            logErrorP("Failed to attach Physical Strip %d to Virtual Strip", config.physicalStripIndex);
        }
    }

    logInfoP("Created Virtual Strip with user-defined order: %d LEDs, %d physical strips",
             _module->_totalLeds, static_cast<int>(_module->_virtualStripConfiguration.size()));
    logInfoP("ColorOrder Design: VirtualStrip=RGB (internal), PhysicalStrips=hardware-specific");

    // Configure color correction parameters on VirtualStrip
    // (These are applied during rendering, NOT in-place!)
    // NOTE: setColorCorrection removed from VirtualStrip - color correction deactivated for now
    /*
    if (_module->_virtualStrip) {
      _module->_virtualStrip->setColorCorrection(
        _module->_gammaCorrectionEnabled, _module->_gammaValue,
        _module->_whiteBalanceEnabled, _module->_whiteBalanceRed, _module->_whiteBalanceGreen, _module->_whiteBalanceBlue,
        _module->_swapMode
      );
      logInfoP("VirtualStrip color correction configured: Gamma=%s(%.1f), WB=%s, Swap=%d",
               _module->_gammaCorrectionEnabled ? "ON" : "OFF", _module->_gammaValue,
               _module->_whiteBalanceEnabled ? "ON" : "OFF",
               _module->_swapMode);
    }
    */

    // Create segments after virtual strip is ready
    if (_module->_numberOfSegments > 0)
    {
        _module->createSegments();
        _module->applySegmentConfiguration();
        logInfoP("Created %d segments on virtual strip", _module->_numberOfSegments);
    }
}

void StripConfiguration::createDefaultVirtualStripOrder()
{
    _module->_virtualStripConfiguration.clear();

    // Create simple sequential order: Strip 1, Strip 2, Strip 3, etc.
    uint16_t currentStart = 0;
    for (size_t i = 0; i < _module->_physicalStrips.size(); i++)
    {
        if (_module->_physicalStrips[i])
        {
            uint16_t ledCount = _module->_physicalStrips[i]->getLedCount();
            _module->_virtualStripConfiguration.emplace_back(static_cast<uint8_t>(i), currentStart, ledCount);

            logInfoP("Default order: Physical strip %d (%d LEDs) starts at virtual position %d",
                     static_cast<int>(i), ledCount, currentStart);

            currentStart += ledCount;
        }
    }
}

bool StripConfiguration::isSpiProtocol(LedProtocol protocol)
{
    switch (protocol)
    {
        case LedProtocol::APA102:
        case LedProtocol::SK9822:
        case LedProtocol::WS2801:
        case LedProtocol::LPD8806:
            return true;
        default:
            return false;
    }
}