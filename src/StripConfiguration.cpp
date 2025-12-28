#include "StripConfiguration.h"
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

// Available GPIO pins in order (PIN1-PIN8)
#if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
    const uint8_t availablePins[] = {
        KNXIAO_RP2040_PIN1, KNXIAO_RP2040_PIN2, KNXIAO_RP2040_PIN3, KNXIAO_RP2040_PIN4,
        KNXIAO_RP2040_PIN5, KNXIAO_RP2040_PIN6, KNXIAO_RP2040_PIN7, KNXIAO_RP2040_PIN8};
#elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
    const uint8_t availablePins[] = {
        KNXIAO_ESP32S3_PIN1, KNXIAO_ESP32S3_PIN2, KNXIAO_ESP32S3_PIN3, KNXIAO_ESP32S3_PIN4,
        KNXIAO_ESP32S3_PIN5, KNXIAO_ESP32S3_PIN6, KNXIAO_ESP32S3_PIN7, KNXIAO_ESP32S3_PIN8};
#else
    const uint8_t availablePins[] = {27, 28, 29, 6, 7, 2, 4, 3}; // Fallback
#endif
    const uint8_t numAvailablePins = sizeof(availablePins) / sizeof(availablePins[0]);
    uint8_t nextPinIndex = 0; // Index into availablePins array

    // Helper lambda to get next available pin(s)
    auto getNextPin = [&](uint8_t count = 1) -> bool {
        // Check if we have enough pins available
        if (nextPinIndex + count > numAvailablePins)
        {
            return false; // Not enough pins
        }
        nextPinIndex += count;
        return true;
    };

    // Helper lambda to check if a pin is already used
    auto isPinUsed = [&usedPins](uint8_t pin) {
        return std::find(usedPins.begin(), usedPins.end(), pin) != usedPins.end();
    };

    // 1) Determine number of strips from ETS (max 6 strips supported in virtual configuration)
    const uint8_t maxStrips = std::max<uint8_t>(1, std::min<uint8_t>(6, ParamNEO_NEONumberOfLEDStrips));
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
    logInfoP("Pre-scanning %d strips for manual GPIO configurations...", maxStrips);
    for (uint8_t i = 0; i < maxStrips; ++i)
    {
        _module->setChannelIndex(i);
        uint8_t _channelIndex = i; // Local variable for ETS parameter macros

        const uint16_t pixels = (uint16_t)ParamNEOSTRIP_NEOLength;
        if (pixels == 0) continue; // Skip strips with 0 LEDs

        const uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const LedProtocol proto = mapProtocol(ledTypeParam);
        const bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;

        if (gpioManualConfig)
        {
            if (isSpiProtocol(proto))
            {
                // SPI protocols need 2 pins
                uint8_t mosiGpio = (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO;
                uint8_t sckGpio = (uint8_t)ParamNEOSTRIP_NEOClockGPIO;

                if (!isPinUsed(sckGpio))
                {
                    usedPins.push_back(sckGpio);
                    logInfoP("Strip %d: Pre-marked manual SCK GPIO %d as used", i, sckGpio);
                }
                else
                {
                    logWarningP("Strip %d: Manual SCK GPIO %d already marked as used!", i, sckGpio);
                }

                if (!isPinUsed(mosiGpio))
                {
                    usedPins.push_back(mosiGpio);
                    logInfoP("Strip %d: Pre-marked manual MOSI GPIO %d as used", i, mosiGpio);
                }
                else
                {
                    logWarningP("Strip %d: Manual MOSI GPIO %d already marked as used!", i, mosiGpio);
                }
            }
            else
            {
                // 1-Wire protocols need 1 pin
                const uint8_t dataGpio = (uint8_t)ParamNEOSTRIP_NEODataGPIO;

                if (!isPinUsed(dataGpio))
                {
                    usedPins.push_back(dataGpio);
                    logInfoP("Strip %d: Pre-marked manual Data GPIO %d as used", i, dataGpio);
                }
                else
                {
                    logWarningP("Strip %d: Manual Data GPIO %d already marked as used!", i, dataGpio);
                }
            }
        }
    }
    logInfoP("Pre-scan complete: %d manual GPIO pins marked as used", (int)usedPins.size());

    // 2) Create all physical strips in configuration order (auto-allocation will skip pre-marked pins)
    for (uint8_t i = 0; i < maxStrips; ++i)
    {
        _module->setChannelIndex(i);
        uint8_t _channelIndex = i; // Local variable for ETS parameter macros

        const uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const LedProtocol proto = mapProtocol(ledTypeParam);

        // Get color order: use GRBW for RGBW protocols, or user-selected for others
        ColorOrder order;
        if (ledTypeParam == 8)
        { // SK6812/WS2814 (RGBW) - special ETS value
            // RGBW capable protocols: use GRBW by default (white channel on back)
            // User can adjust white position with Swap parameter if needed
            order = ColorOrder::GRBW;
            logInfoP("Strip %d: RGBW protocol detected (ETS type 8), using GRBW color order", i);
        }
        else
        {
            // Regular RGB protocols: use user-selected color order
            order = mapColorOrder((uint8_t)ParamNEOSTRIP_NEOColourOrder);
        }

        const uint8_t dataGpio = (uint8_t)ParamNEOSTRIP_NEODataGPIO;
        const uint16_t pixels = (uint16_t)ParamNEOSTRIP_NEOLength;

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

            // Check if manual GPIO configuration is enabled
            bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;

            if (gpioManualConfig)
            {
                // Use ETS configured GPIO pins (already pre-marked as used during pre-scan)
                mosiGpio = (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO;
                sckGpio = (uint8_t)ParamNEOSTRIP_NEOClockGPIO;
            }
            else
            {
                // Automatic pin allocation: SPI needs 2 consecutive pins
                // Skip to next available pair of pins that aren't used
                bool foundPins = false;
                while (nextPinIndex + 1 < numAvailablePins)
                {
                    sckGpio = availablePins[nextPinIndex];
                    mosiGpio = availablePins[nextPinIndex + 1];

                    // Check if both pins are free
                    if (!isPinUsed(sckGpio) && !isPinUsed(mosiGpio))
                    {
                        foundPins = true;
                        // Mark pins as used immediately
                        usedPins.push_back(sckGpio);
                        usedPins.push_back(mosiGpio);
                        nextPinIndex += 2; // Consume both pins
                        break;
                    }
                    // If conflict, skip to next pin and try again
                    nextPinIndex++;
                }

                if (!foundPins)
                {
                    logErrorP("Strip %d: No available GPIO pins for SPI strip!", i);
                    // Fallback to last resort pins
                    sckGpio = 18;
                    mosiGpio = 19;
                    // Mark fallback pins as used
                    usedPins.push_back(sckGpio);
                    usedPins.push_back(mosiGpio);
                }
            }

            // Read SPI frequency from ETS (if manual config enabled)
            uint32_t spiFrequency = 10000000; // Default: 10 MHz
            bool spiClkManual = (bool)ParamNEOSTRIP_NEOSPICLKManual;

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
                }
                else
                {
                    logErrorP("SPI Strip %d: Failed to cast config to SpiStripConfig!", i);
                }
            }
            else
            {
                logErrorP("SPI Strip %d: addSpiStrip returned nullptr!", i);
            }

            logInfoP("SPI Strip %d: %d LEDs, MOSI=%d, SCK=%d, Protocol=%s, ColorOrder=%s, Freq=%d Hz%s",
                     i, pixels, mosiGpio, sckGpio, NeoPixelBusModule::getProtocolName(proto), NeoPixelBusModule::getColorOrderName(order),
                     spiFrequency, gpioManualConfig ? " (Manual)" : " (Auto)");
        }
        else
        {
            // 1-Wire protocols use Data GPIO
            uint8_t dataGpioPin;

            // Check if manual GPIO configuration is enabled
            bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;

            if (gpioManualConfig)
            {
                // Use ETS configured GPIO pin (already pre-marked as used during pre-scan)
                dataGpioPin = dataGpio;
            }
            else
            {
                // Automatic pin allocation: 1-Wire needs 1 pin
                // Find next available unused pin
                bool foundPin = false;
                while (nextPinIndex < numAvailablePins)
                {
                    dataGpioPin = availablePins[nextPinIndex];

                    // Check if pin is free
                    if (!isPinUsed(dataGpioPin))
                    {
                        foundPin = true;
                        // Mark pin as used immediately
                        usedPins.push_back(dataGpioPin);
                        nextPinIndex++; // Consume this pin
                        break;
                    }
                    // If conflict, skip to next pin
                    nextPinIndex++;
                }

                if (!foundPin)
                {
                    logErrorP("Strip %d: No available GPIO pins for 1-Wire strip!", i);
                    // Fallback to ETS parameter
                    dataGpioPin = dataGpio;
                    usedPins.push_back(dataGpioPin);
                }
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
                }
            }
        }

        if (phys)
        {
            _module->_totalLeds += pixels;
            _module->_physicalStrips.push_back(phys);

            // Configure timing for this physical strip
            uint8_t timingMode = (uint8_t)ParamNEOSTRIP_NEOTiming;
            if (timingMode <= 10)
            {
                // Map timing mode parameter (0-10) to TimingMode enum
                TimingMode mode = static_cast<TimingMode>(timingMode);
                phys->setTimingMode(mode);

                const char* timingModes[] = {
                    "AUTO", "AUTO_LEGACY",
                    "SLOW_20%", "SLOW_15%", "SLOW_10%", "SLOW_5%",
                    "FAST_5%", "FAST_10%", "FAST_15%", "FAST_20%", "FAST_25%"};
                const char* timingName = (timingMode < 11) ? timingModes[timingMode] : "UNKNOWN";
                logInfoP("Strip %d: Timing mode=%d (%s)", i, timingMode, timingName);
            }

            // Configure color correction for this strip
            if ((bool)ParamNEOSTRIP_NEOGammaCorrection || (bool)ParamNEOSTRIP_NEOWhiteBalanceCorrection)
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
    // Based on NEOLedType enumeration from NeoPixelBus.share.xml
    switch (p)
    {
        case 0: return LedProtocol::WS2812B;  // WS2812B
        case 1: return LedProtocol::WS2805;   // WS2805
        case 2: return LedProtocol::WS2811;   // WS2811
        case 3: return LedProtocol::WS2813;   // WS2813
        case 4: return LedProtocol::SK6812;   // SK6812
        case 5: return LedProtocol::APA102;   // APA102
        case 6: return LedProtocol::SK9822;   // SK9822
        case 7: return LedProtocol::WS2812B;  // WS281x (mapped to WS2812B)
        case 8: return LedProtocol::SK6812;   // SK6812/WS2814 (RGBW)
        case 9: return LedProtocol::TM1814;   // TM1814
        case 10: return LedProtocol::WS2811;  // WS2812_400kHz (mapped to WS2811)
        case 21: return LedProtocol::WS2801;  // WS2801
        case 22: return LedProtocol::LPD8806; // LPD8806
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
        case 7: return ColorOrder::GRBW; // GRBW (4-channel, SK6812 standard)
        default: return ColorOrder::GRB; // Default to WS2812/SK6812 standard
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

    // Read the virtual strip position parameters (1-6 positions for 1-6 strips)
    // Value 0 = not used, Values 1-6 = physical strip index (1-based)
    uint8_t positions[6] = {
        static_cast<uint8_t>(ParamNEO_VirtualStripPos1),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos2),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos3),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos4),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos5),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos6)};

    // Read the start positions (calculated by ETS JavaScript)
    uint16_t startPositions[6] = {
        static_cast<uint16_t>(ParamNEO_VirtualStripStart1),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart2),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart3),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart4),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart5),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart6)};

    // Build the virtual strip configuration based on positions
    for (uint8_t pos = 0; pos < 6; pos++)
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
                     pos + 1, physStripIndex + 1, ledCount, virtualStartZeroBased, virtualStart);
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
        return;
    }

    // Determine if any physical strip requires RGBW (4 bytes per LED)
    bool needsRGBW = false;
    for (const auto* phys : _module->_physicalStrips)
    {
        if (phys && phys->getColorOrder() >= ColorOrder::RGBW)
        {
            needsRGBW = true;
            break;
        }
    }

    // Create virtual strip: RGBW if any strip needs it, otherwise RGB
    ColorOrder virtualOrder = needsRGBW ? ColorOrder::RGBW : ColorOrder::RGB;
    _module->_virtualStrip = _module->_neoPixel.addVirtualStrip(_module->_totalLeds, virtualOrder);
    if (!_module->_virtualStrip)
    {
        logErrorP("Failed to create virtual strip");
        return;
    }

    if (needsRGBW)
    {
        logInfoP("VirtualStrip created with RGBW support (4 bytes/LED) for RGBW physical strips");
    }

    auto mgr = _module->_neoPixel.getManager();
    if (!mgr)
    {
        logErrorP("NeoPixelManager not available for virtual strip creation");
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