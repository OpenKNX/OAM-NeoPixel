#include "NeoPixelModule.h"
#include "ColorManagement.h"
#include "EffectConfiguration.h"
#include "HclCurve.h"
#include "SegmentController.h"
#include "StripConfiguration.h"
#include "HardwareMappingLogic.h"

// Include generated effect parameters if available
#if defined(__has_include)
    #if __has_include("EffectParameterMapping.h")
        #define EFFECT_PARAMETER_MAPPING_GENERATED
        #include "EffectParameterMapping.h"
    #endif
#else
    // Fallback for older compilers: can define via build flags
    #ifdef EFFECT_PARAMETER_MAPPING_GENERATED
        #include "EffectParameterMapping.h"
    #endif
#endif

#include "NeoPixelFlashPersistence.h"
#include "OpenKNX.h"
#include "PhysicalStripConfig.h" // For SpiStripConfig
#include "colorhelper.h"
#include "knxprod.h"
#include <algorithm>
#include <vector>
// #define NEOPIXEL_MODULE_TEST_ENV
#ifdef NEOPIXEL_MODULE_TEST_ENV
    #include "test/test.h"
#endif

// DPT 3.007 step code to delta mapping (exponential)
// Standard KNX/ETS: stepCode 1 = largest (100%), 7 = smallest (~2%), 0 = stop
static inline int16_t dpt3_007_delta(uint8_t stepCode)
{
    static const uint8_t factors[8] = {0, 64, 32, 16, 8, 4, 2, 1};
    return (int16_t)((factors[stepCode & 0x07] * 255u + 32u) / 64u);
}

// Helper: Start/stop dimming for a segment channel
static void startStopDimming(NeoPixelBusModule::SegmentConfig& config,
                             NeoPixelBusModule::SegmentConfig::DimmingChannel channel,
                             uint8_t rel, uint8_t segmentIndex)
{
    // Decode ETS values: 1-7=start decreasing, 8-9=stop, 10-16=start increasing
    if (rel >= 1 && rel <= 16)
    {
        if (rel == 8 || rel == 9)
        {
            // Stop: stepCode 0
            config.activeDimming = NeoPixelBusModule::SegmentConfig::NONE;
            config.dimmingStepCode = 0;
        }
        else if (rel >= 10 && rel <= 16)
        {
            // Start/continue increasing
            uint8_t stepCode = rel - 9; // 10→1, 11→2, ..., 16→7
            config.activeDimming = channel;
            config.dimmingIncrease = true;
            config.dimmingStepCode = stepCode;
            config.dimmingLastUpdate = millis();
            config.dimmingNextStep = millis();
        }
        else
        { // 1-7
            // Start/continue decreasing
            config.activeDimming = channel;
            config.dimmingIncrease = false;
            config.dimmingStepCode = rel; // 1→100%, 7→~2%
            config.dimmingLastUpdate = millis();
            config.dimmingNextStep = millis();
        }
    }
}

NeoPixelBusModule openknxNeoPixelModule;

NeoPixelBusModule::NeoPixelBusModule()
    : _flashPersistence(nullptr), _effectConfiguration(nullptr), _colorManagement(nullptr), _segmentController(nullptr), _hclCurve(nullptr)
{
    _flashPersistence = new NeoPixelFlashPersistence(this);
    _effectConfiguration = new EffectConfiguration(this);
    _colorManagement = new ColorManagement(this);
    _segmentController = new SegmentController(this);
    _hclCurve = new HclCurve();
}

NeoPixelBusModule::~NeoPixelBusModule()
{
    delete _flashPersistence;
    delete _effectConfiguration;
    delete _colorManagement;
    delete _segmentController;
    delete _hclCurve;
}

void NeoPixelBusModule::setup(bool configured)
{
#ifdef NEOPIXEL_MODULE_TEST_ENV
    // Test mode: Initialize core module first, then setup test environment
    if (!_neoPixel.isInitialized())
    {
        _neoPixel.init();
        _neoPixel.setup(true); // Must call setup() to set _initialized flag
    }
    setup_test_environment(_neoPixel);
    _initialized = true;
#else
    if (configured)
    {
        // Initialize NeoPixel library first (creates manager)
        if (!_neoPixel.isInitialized())
        {
            _neoPixel.init();
        }

        // Configure strips, segments and effects from ETS parameters
        configureFromETS();
        _initialized = true;
        // Initialize all physical strips (PIO, DMA, etc.) - MUST be after configureFromETS()
        // which creates the PhysicalStrip objects, but BEFORE any show() calls
        _neoPixel.setup(configured);
        // Now that hardware is initialized, clear LEDs if requested during configuration
        if (_clearLedsAfterSetup && _virtualStrip)
        {
            _virtualStrip->show();
            logInfoP("LEDs cleared - will restore saved state after startup delay");
            _clearLedsAfterSetup = false;
        }

        // Setup HCL curve for automatic Kelvin scheduling
        if (_hclCurve)
        {
            logInfoP("Initializing HCL curve for automatic Kelvin scheduling");
            _hclCurve->setup(0); // Index 0 for global HCL
        }
    }
#endif
}

void NeoPixelBusModule::loop(bool configured)
{
    if (!configured || !_initialized) return;

    // Check for hardware configuration mismatch and warn periodically
    if (_hwConfigMismatch) {
        unsigned long now = millis();
        if (now - _lastHwMismatchWarning >= 5000) { // Every 5 seconds
            _lastHwMismatchWarning = now;
            #ifdef DEVICE_HW_ID
                #ifdef ParamNEO_NEO_NeoPixelHardwareSelect
                    uint16_t selectedHwIndex = (uint16_t)ParamNEO_NEO_NeoPixelHardwareSelect;
                    
                    if (selectedHwIndex == 255) {
                        // ETS has dummy value - user hasn't configured hardware yet
                        uint8_t compiledHwIndex = HardwareMapping::mapDeviceHwIdToIndex(DEVICE_HW_ID);
                        const char* compiledHwName = HardwareMapping::getHardwareName(compiledHwIndex);
                        logErrorP("*** No hardware configured! Use 'Hardware auto-detect' button in ETS or manually select: %s (ID: 0x%04X)",
                                  compiledHwName ? compiledHwName : "Unknown", DEVICE_HW_ID);
                    } else {
                        // Hardware configured but mismatched - compare indices directly
                        uint8_t compiledHwIndex = HardwareMapping::mapDeviceHwIdToIndex(DEVICE_HW_ID);
                        uint8_t etsHwIndex = selectedHwIndex;  // Parameter contains index directly now
                        const char* compiledHwName = HardwareMapping::getHardwareName(compiledHwIndex);
                        const char* etsHwName = HardwareMapping::getHardwareName(etsHwIndex);
                        logErrorP("!!! HW MISMATCH: Firmware=%s (Index %d) vs ETS=%s (Index %d) - Update ETS config!",
                                  compiledHwName ? compiledHwName : "Unknown", compiledHwIndex,
                                  etsHwName ? etsHwName : "Unknown", etsHwIndex);
                    }
                #endif
            #endif
        }
    }

    // Process HCL curve scheduling (updates Kelvin target based on sun position or time)
    // This runs even when power is OFF to maintain the curve state
    if (_hclCurve)
    {
        _hclCurve->loop();
    }

    // If global power is OFF, skip all effect processing and pixel updates
    if (!_globalPowerOn) return;

    // Process active DPT 3.007 start/stop dimming
    processActiveDimming();

    // Call library loop() for auto-update timer and effect processing
    _neoPixel.loop(configured);

    // Apply HCL post-processing to rendered pixels (color temperature correction)
    // This must happen AFTER effects are rendered but BEFORE pixels are sent to hardware
    if (_colorManagement)
    {
        _colorManagement->applyHclPostProcess();
    }
}

void NeoPixelBusModule::processBeforeRestart()
{
    // Check if LED shutdown is enabled in ETS
    if (!ParamNEO_NEOTurnOffBeforeRestart)
    {
        logInfoP("LED shutdown before restart is disabled in ETS");
        return;
    }

    // Turn off all LEDs before ETS programming or device restart
    logInfoP("Turning off all LEDs before restart/programming");

    if (!_initialized) return;

    // Stop all effects and clear all LEDs
    for (auto& segConfig : _segments)
    {
        if (segConfig.segment && segConfig.segment->hasEffect())
        {
            segConfig.segment->clearEffect(); // Remove effect (stops further updates)
        }
    }

    // Use library method to clear and show all LEDs
    _neoPixel.clearAll();  // Clear all segments
    _neoPixel.updateAll(); // Send to hardware

    // Wait for DMA completion before restart (max 100ms timeout)
    if (_virtualStrip)
    {
        _virtualStrip->waitForCompletion(100);
    }
}

// ============================================================================
// Flash State Persistence - Delegate to NeoPixelFlashPersistence
// ============================================================================

uint16_t NeoPixelBusModule::flashSize()
{
    return _flashPersistence ? _flashPersistence->calculateFlashSize() : 0;
}

void NeoPixelBusModule::writeFlash()
{
    if (_flashPersistence)
    {
        _flashPersistence->writeToFlash();
    }
}

void NeoPixelBusModule::readFlash(const uint8_t* data, const uint16_t size)
{
    if (_flashPersistence)
    {
        _flashPersistence->readFromFlash(data, size);
    }
}

void NeoPixelBusModule::processAfterStartupDelay()
{
    if (_flashPersistence)
    {
        _flashPersistence->restoreStatesAfterStartup();
    }
}

// Process active start/stop dimming for all segments
void NeoPixelBusModule::processActiveDimming()
{
    _segmentController->processActiveDimming();
}

void NeoPixelBusModule::processInputKo(GroupObject& ko)
{
    // Get the KO number for routing to correct channel/segment
    uint16_t koNumber = ko.asap();
    logInfoP("processInputKo: KO Number %d", koNumber);
    // Global NeoPixel KOs
    if (koNumber == NEO_KoPower)
    {
        bool powerState = ko.value(DPT_Switch);
        logInfoP("Global Power KO: %s", powerState ? "ON" : "OFF");

        // Update global power state flag
        _globalPowerOn = powerState;

        if (powerState == false)
        {
            // Power off - turn off all LEDs immediately by clearing all segments
            if (!_segments.empty() && _virtualStrip)
            {
                // Clear each segment individually
                for (auto& segConfig : _segments)
                {
                    if (segConfig.segment)
                    {
                        segConfig.segment->clearAll(); // Clear RGBW pixels
                    }
                }
                _virtualStrip->show();
            }
        }

        // Send state feedback
        bool changed = KoNEO_PowerState.valueNoSendCompare(powerState, DPT_Switch);
        if (changed) KoNEO_PowerState.objectWritten();
        return;
    }

    if (koNumber == NEO_KoBrightness)
    {
        // DPT 5.001 sends 0-100 (percentage), need to scale to 0-255 for internal use
        uint8_t brightnessPercent = ko.value(DPT_Scaling);
        uint8_t brightness = (brightnessPercent * 255) / 100;
        logInfoP("Global Brightness KO: %d%% (scaled to %d/255)", brightnessPercent, brightness);

        // Apply brightness to all segments
        applyGlobalBrightness(brightness);

        // Send state feedback (send back percentage)
        bool changed = KoNEO_BrightnessState.valueNoSendCompare(brightnessPercent, DPT_Scaling);
        if (changed) KoNEO_BrightnessState.objectWritten();
        return;
    }

    if (koNumber == NEO_KoHCLState)
    {
        uint16_t kelvinValue = ko.value(Dpt(7, 600)); // DPT 7.600 = Color Temperature (Kelvin)
        logInfoP("HCL State KO received: %dK color temperature", kelvinValue);

        // Special case: 0K = disable HCL mode
        if (kelvinValue == 0)
        {
            logInfoP("HCL disable command received (0K)");
            disableHclMode();
        }
        else
        {
            // Apply the color temperature globally if valid range
            applyHclColorTemperature(kelvinValue);
        }
        return;
    }

    // Channel-specific KOs (segment-based) - Calculate which channel this KO belongs to
    int channel = NEO_KoCalcChannel(koNumber);
    if (channel >= 0)
    {
        uint8_t oldChannelIndex = _channelIndex;
        _channelIndex = channel;

        int koIndex = NEO_KoCalcIndex(koNumber);

        // IMPORTANT: "channel" here refers to segment index, not physical strip index
        // The KNX channels correspond to logical segments that users can control
        // Physical strips are hardware configuration and not directly controlled via KOs

        logInfoP("Segment Channel %d KO Index %d received (_segments.size=%d, _numberOfSegments=%d)",
                 channel, koIndex, (int)_segments.size(), _numberOfSegments);

        // Validate channel against segment count, not physical strip count
        if (channel >= _segments.size())
        {
            logWarningP("KO for channel %d ignored - only %d segments configured (_numberOfSegments=%d)",
                        channel, (int)_segments.size(), _numberOfSegments);
            _channelIndex = oldChannelIndex;
            return;
        }

        // Get the target segment for this channel
        Segment* targetSegment = (channel < _segments.size()) ? _segments[channel].segment : nullptr;
        if (!targetSegment)
        {
            logWarningP("Channel %d has no valid segment, ignoring KO", channel);
            _channelIndex = oldChannelIndex;
            return;
        }

        switch (koIndex)
        {
            case NEO_KoR:
            {
                uint8_t red = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d Red: %d", channel, red);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore)
                cfg.pendingSolidR = red;
                cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
                cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
                cfg.pendingSolidWW = cfg.savedValid ? cfg.savedWW : 0;

                // Update saved values - preserve existing G, B, W
                cfg.savedR = red;
                if (!cfg.savedValid)
                {
                    cfg.savedG = 0;
                    cfg.savedB = 0;
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                {
                    uint8_t g = cfg.savedG;
                    uint8_t b = cfg.savedB;
                    targetSegment->setPrimaryColor(red, g, b, 0);
                    logInfoP("Segment %d: Updated primary color (Red=%d)", channel, red);
                }

                // Send combined RGB/RGBW status feedback
                _channelIndex = channel;
                uint8_t r, g, b;
                r = cfg.pendingSolidR;
                g = cfg.pendingSolidG;
                b = cfg.pendingSolidB;
                // Check if virtual strip has white channel (4 bytes per LED)
                if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4)
                {
                    // Has white channel - send RGBW status
                    uint8_t w = cfg.pendingSolidWW;
                    uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
                    bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
                    if (changed) KoNEO_RGBWState.objectWritten();
                }
                else
                {
                    // No white channel - send RGB status
                    uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b; // RGB
                    bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                    if (changed) KoNEO_RGBState.objectWritten();
                }
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoG:
            {
                uint8_t green = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d Green: %d", channel, green);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore)
                cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
                cfg.pendingSolidG = green;
                cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
                cfg.pendingSolidWW = cfg.savedValid ? cfg.savedWW : 0;

                // Update saved values - preserve existing R, B, W
                if (!cfg.savedValid)
                {
                    cfg.savedR = 0;
                    cfg.savedB = 0;
                    cfg.savedWW = 0;
                }
                cfg.savedG = green;
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                {
                    uint8_t r = cfg.savedR;
                    uint8_t b = cfg.savedB;
                    targetSegment->setPrimaryColor(r, green, b, 0);
                    logInfoP("Segment %d: Updated primary color (Green=%d)", channel, green);
                }

                // Send combined RGB/RGBW status feedback
                _channelIndex = channel;
                uint8_t r, g, b;
                r = cfg.pendingSolidR;
                g = cfg.pendingSolidG;
                b = cfg.pendingSolidB;
                // Check if virtual strip has white channel (4 bytes per LED)
                if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4)
                {
                    // Has white channel - send RGBW status
                    uint8_t w = cfg.pendingSolidWW;
                    uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
                    bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
                    if (changed) KoNEO_RGBWState.objectWritten();
                }
                else
                {
                    // No white channel - send RGB status
                    uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b; // RGB
                    bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                    if (changed) KoNEO_RGBState.objectWritten();
                }
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoB:
            {
                uint8_t blue = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d Blue: %d", channel, blue);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore)
                cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
                cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
                cfg.pendingSolidB = blue;
                cfg.pendingSolidWW = cfg.savedValid ? cfg.savedWW : 0;

                // Update saved values - preserve existing R, G, W
                if (!cfg.savedValid)
                {
                    cfg.savedR = 0;
                    cfg.savedG = 0;
                    cfg.savedWW = 0;
                }
                cfg.savedB = blue;
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                {
                    uint8_t r = cfg.savedR;
                    uint8_t g = cfg.savedG;
                    targetSegment->setPrimaryColor(r, g, blue, 0);
                    logInfoP("Segment %d: Updated primary color (Blue=%d)", channel, blue);
                }

                // Send combined RGB/RGBW status feedback
                _channelIndex = channel;
                uint8_t r, g, b;
                r = cfg.pendingSolidR;
                g = cfg.pendingSolidG;
                b = cfg.pendingSolidB;
                // Check if virtual strip has white channel (4 bytes per LED)
                if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4)
                {
                    // Has white channel - send RGBW status
                    uint8_t w = cfg.pendingSolidWW;
                    uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
                    bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
                    if (changed) KoNEO_RGBWState.objectWritten();
                }
                else
                {
                    // No white channel - send RGB status
                    uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b; // RGB
                    bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                    if (changed) KoNEO_RGBState.objectWritten();
                }
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoW:
            {
                uint8_t white = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d White: %d", channel, white);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore)
                cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
                cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
                cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
                cfg.pendingSolidWW = white;

                // Update saved values - preserve existing R, G, B
                if (!cfg.savedValid)
                {
                    cfg.savedR = 0;
                    cfg.savedG = 0;
                    cfg.savedB = 0;
                }
                cfg.savedWW = white;
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                {
                    uint8_t r = cfg.savedR;
                    uint8_t g = cfg.savedG;
                    uint8_t b = cfg.savedB;
                    targetSegment->setPrimaryColor(r, g, b, white);
                    logInfoP("Segment %d: Updated primary color (White=%d)", channel, white);
                }

                // Send RGBW status feedback (white channel update)
                _channelIndex = channel;
                uint8_t r, g, b;
                r = cfg.pendingSolidR;
                g = cfg.pendingSolidG;
                b = cfg.pendingSolidB;
                uint8_t w = white;
                uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w; // RGBW
                bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
                if (changed) KoNEO_RGBWState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoCCT:
            {
                uint16_t cct = ko.value(DPT_Value_Temp);
                logInfoP("Segment %d CCT: %dK", channel, cct);

                SegmentConfig& cfg = _segments[channel];

                // Apply color temperature to segment - convert Kelvin to RGB
                uint8_t r, g, b;
                ColorHelper::kelvinToRGB(cct, r, g, b);
                targetSegment->setPrimaryColor(r, g, b, 0);

                // Save color for effect restore
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                cfg.savedWW = 0;
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;
                cfg.savedLastWasEffect = false;

                // Send status feedback
                _channelIndex = channel;
                bool changed = KoNEO_CCTState.valueNoSendCompare(cct, DPT_Value_Temp);
                if (changed) KoNEO_CCTState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoH:
            {
                // DPT 5.003 is 1 byte on the wire: 0..255  <->  0..360°
                uint8_t raw = ko.value(DPT_Value_1_Ucount); // read raw byte (no scaling)

                // optional: treat 360° (=255) as 0° for hue wrap
                uint8_t hue = (raw == 255) ? 0 : raw;

                // log degrees (use >=16-bit!)
                uint16_t deg = (uint32_t(raw) * 360u + 127u) / 255u;
                logInfoP("Hue: %u° (raw=%u) -> HSV hue=%u", (unsigned)deg, (unsigned)raw, (unsigned)hue);

                // Use persistent S and V from config
                SegmentConfig& cfg = _segments[channel];
                uint8_t s = cfg.currentS;
                uint8_t v = cfg.currentV;
                // Preserve S and V - don't reset them when H changes
                // If V is 0, default to full brightness to make color visible
                if (v == 0)
                {
                    v = 255;
                }

                // Convert HSV to RGB
                uint8_t r, g, b;
                ColorHelper::hsvToRGB(hue, s, v, r, g, b);

                // Store as pending (for effect restore) - preserve W if strip has white channel
                cfg.pendingSolidR = r;
                cfg.pendingSolidG = g;
                cfg.pendingSolidB = b;
                if (!cfg.savedValid || (_virtualStrip && _virtualStrip->getBytesPerLed() != 4))
                {
                    cfg.pendingSolidWW = 0;
                }

                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, 0);
                logInfoP("Segment %d: Updated primary color (Hue)", channel);

                // Store and persist HSV values
                cfg.currentH = hue;
                cfg.currentS = s;
                cfg.currentV = v;

                // Send HSV status feedback - convert to RGB using actual V value
                _channelIndex = channel;
                ColorHelper::hsvToRGB(cfg.currentH, cfg.currentS, cfg.currentV, r, g, b);
                uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                bool changed = KoNEO_HSVState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                if (changed) KoNEO_HSVState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoS:
            {
                // DPT 5.001 sends 0-100 (percentage), need to scale to 0-255 for internal use
                uint8_t saturationPercent = ko.value(DPT_Scaling);
                uint8_t saturation = (saturationPercent * 255) / 100;
                logInfoP("Segment %d Saturation: %d%% (scaled to %d/255)", channel, saturationPercent, saturation);

                // Use persistent H and V from config
                SegmentConfig& cfg = _segments[channel];
                uint8_t h = cfg.currentH;
                uint8_t v = cfg.currentV;
                // Preserve H and V - don't reset them when S changes
                // If V is 0, default to full brightness to make color visible
                if (v == 0)
                {
                    v = 255;
                }

                // Convert HSV to RGB
                uint8_t r, g, b;
                ColorHelper::hsvToRGB(h, saturation, v, r, g, b);

                // Store as pending (for effect restore) - preserve W if strip has white channel
                cfg.pendingSolidR = r;
                cfg.pendingSolidG = g;
                cfg.pendingSolidB = b;
                if (!cfg.savedValid || (_virtualStrip && _virtualStrip->getBytesPerLed() != 4))
                {
                    cfg.pendingSolidWW = 0;
                }

                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, 0);
                logInfoP("Segment %d: Updated primary color (Saturation)", channel);

                // Store HSV values
                cfg.currentH = h;
                cfg.currentS = saturation;
                cfg.currentV = v;

                // Send HSV status feedback - convert to RGB using actual V value
                _channelIndex = channel;
                ColorHelper::hsvToRGB(cfg.currentH, cfg.currentS, cfg.currentV, r, g, b);
                uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                bool changed = KoNEO_HSVState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                if (changed) KoNEO_HSVState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoV:
            {
                // DPT 5.001 sends 0-100 (percentage), need to scale to 0-255 for internal use
                uint8_t valuePercent = ko.value(DPT_Scaling);
                uint8_t value = (valuePercent * 255) / 100;
                logInfoP("Segment %d Value: %d%% (scaled to %d/255)", channel, valuePercent, value);

                // Use persistent H and S from config
                SegmentConfig& cfg = _segments[channel];
                uint8_t h = cfg.currentH;
                uint8_t s = cfg.currentS;
                // Preserve H and S - don't reset them when V changes

                // Convert HSV to RGB
                uint8_t r, g, b;
                ColorHelper::hsvToRGB(h, s, value, r, g, b);

                // Store as pending (for effect restore) - preserve W if strip has white channel
                cfg.pendingSolidR = r;
                cfg.pendingSolidG = g;
                cfg.pendingSolidB = b;
                if (!cfg.savedValid || (_virtualStrip && _virtualStrip->getBytesPerLed() != 4))
                {
                    cfg.pendingSolidWW = 0;
                }

                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, 0);
                logInfoP("Segment %d: Updated primary color (Value)", channel);

                // Store HSV values
                cfg.currentH = h;
                cfg.currentS = s;
                cfg.currentV = value;

                // Send HSV status feedback - convert to RGB using actual V value
                _channelIndex = channel;
                ColorHelper::hsvToRGB(cfg.currentH, cfg.currentS, cfg.currentV, r, g, b);
                uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                bool changed = KoNEO_HSVState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                if (changed) KoNEO_HSVState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoRGB:
            {
                uint32_t rgb = ko.value(DPT_Colour_RGB);
                uint8_t r = (rgb >> 16) & 0xFF;
                uint8_t g = (rgb >> 8) & 0xFF;
                uint8_t b = rgb & 0xFF;
                logInfoP("Segment %d RGB: R=%d G=%d B=%d", channel, r, g, b);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore) - preserve W if strip has white channel
                cfg.pendingSolidR = r;
                cfg.pendingSolidG = g;
                cfg.pendingSolidB = b;
                if (!cfg.savedValid || (_virtualStrip && _virtualStrip->getBytesPerLed() != 4))
                {
                    cfg.pendingSolidWW = 0;
                }

                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, 0);
                logInfoP("Segment %d: Updated primary color (RGB)", channel);

                // Send status feedback
                _channelIndex = channel;
                uint32_t feedbackRgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                bool changed = KoNEO_RGBState.valueNoSendCompare(feedbackRgb, DPT_Colour_RGB);
                if (changed) KoNEO_RGBState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoHSV:
            {
                uint32_t hsv = ko.value(DPT_Colour_RGB); // Using same DPT for now
                logInfoP("Segment %d HSV: 0x%08X", channel, hsv);

                // Extract HSV components from the 32-bit value
                // Assuming format: 0x00HHSSVV (H=hue, S=saturation, V=value)
                uint8_t h = (hsv >> 16) & 0xFF; // Hue
                uint8_t s = (hsv >> 8) & 0xFF;  // Saturation
                uint8_t v = hsv & 0xFF;         // Value/Brightness

                // Convert HSV to RGB
                uint8_t r, g, b;
                ColorHelper::hsvToRGB(h, s, v, r, g, b);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore) - preserve W if strip has white channel
                cfg.pendingSolidR = r;
                cfg.pendingSolidG = g;
                cfg.pendingSolidB = b;
                if (!cfg.savedValid || (_virtualStrip && _virtualStrip->getBytesPerLed() != 4))
                {
                    cfg.pendingSolidWW = 0;
                }

                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, 0);
                logInfoP("Segment %d: Updated primary color (HSV)", channel);

                // Send status feedback
                _channelIndex = channel;
                uint32_t feedbackHsv = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                bool changed = KoNEO_HSVState.valueNoSendCompare(feedbackHsv, DPT_Colour_RGB);
                if (changed) KoNEO_HSVState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoFx:
            {
                uint8_t effect = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d Effect: %d", channel, effect);

                // Remember selected effect for restore
                SegmentConfig& cfg = _segments[channel];

                // Apply effect to segment dynamically FIRST (this clears the pixel buffer)
                applyEffectToSegment(targetSegment, effect);

                // THEN restore primary color for ALL effects (after the clear)
                // Effects like Wipe, Rainbow, etc. use the primary color from EffectConfig
                // This must happen AFTER applyEffectToSegment to avoid the clear wiping our color

                // Determine which color to use
                bool hasPendingColor = (cfg.pendingSolidR > 0 || cfg.pendingSolidG > 0 || cfg.pendingSolidB > 0 || cfg.pendingSolidWW > 0);

                if (hasPendingColor)
                {
                    // User set colors during the previous effect - use those
                    targetSegment->setPrimaryColor(cfg.pendingSolidR, cfg.pendingSolidG, cfg.pendingSolidB, cfg.pendingSolidWW);
                    logInfoP("Segment %d: Applied pending color (R=%d G=%d B=%d W=%d) to effect %d",
                             channel, cfg.pendingSolidR, cfg.pendingSolidG, cfg.pendingSolidB, cfg.pendingSolidWW, effect);
                    // Clear pending after applying
                    cfg.pendingSolidR = 0;
                    cfg.pendingSolidG = 0;
                    cfg.pendingSolidB = 0;
                    cfg.pendingSolidWW = 0;
                }
                else if (cfg.savedValid)
                {
                    // No pending colors - use the saved color
                    targetSegment->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW);
                    if (effect == 0)
                    {
                        // For Solid effect, also restore brightness
                        targetSegment->setBrightness(cfg.savedBrightness);
                    }
                    logInfoP("Segment %d: Applied saved color (R=%d G=%d B=%d W=%d) to effect %d",
                             channel, cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, effect);
                }
                else
                {
                    // No saved color - use a default (white for effects, off for Solid)
                    if (effect == 0)
                    {
                        targetSegment->setPrimaryColor(0, 0, 0, 0);
                        logInfoP("Segment %d: No saved color, Solid effect off", channel);
                    }
                    else
                    {
                        targetSegment->setPrimaryColor(255, 255, 255, 0);
                        logInfoP("Segment %d: No saved color, using white for effect %d", channel, effect);
                    }
                }
                // Auto-update will render the change on next cycle
                cfg.savedEffectType = effect;
                cfg.savedEffectValid = (effect > 0);
                // Note: Effect-specific parameters are now stored in ETS, no need to snapshot

                // Send status feedback
                _channelIndex = channel;
                bool changed = KoNEO_FxState.valueNoSendCompare(effect, DPT_SceneNumber);
                if (changed) KoNEO_FxState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoPreset:
            {
                uint8_t preset = ko.value(DPT_SceneNumber);
                logInfoP("Segment %d Preset: %d", channel, preset);

                // Apply preset configuration to segment (predefined color/effect combinations)
                switch (preset)
                {
                    case 1: // Red
                        targetSegment->setPrimaryColor(255, 0, 0, 0);
                        targetSegment->setBrightness(255);
                        break;
                    case 2: // Green
                        targetSegment->setPrimaryColor(0, 255, 0, 0);
                        targetSegment->setBrightness(255);
                        break;
                    case 3: // Blue
                        targetSegment->setPrimaryColor(0, 0, 255, 0);
                        targetSegment->setBrightness(255);
                        break;
                    case 4: // White
                        targetSegment->setPrimaryColor(255, 255, 255, 0);
                        targetSegment->setBrightness(255);
                        break;
                    case 5: // Warm White (using color temperature)
                    {
                        uint8_t r, g, b;
                        ColorHelper::kelvinToRGB(2700, r, g, b); // Warm white ~2700K
                        targetSegment->setPrimaryColor(r, g, b, 0);
                        targetSegment->setBrightness(255);
                    }
                    break;
                    case 6: // Cool White
                    {
                        uint8_t r, g, b;
                        ColorHelper::kelvinToRGB(6500, r, g, b); // Cool white ~6500K
                        targetSegment->setPrimaryColor(r, g, b, 0);
                        targetSegment->setBrightness(255);
                    }
                    break;
                    case 7:                                     // Rainbow Effect
                        applyEffectToSegment(targetSegment, 1); // Rainbow effect
                        targetSegment->setBrightness(200);
                        break;
                    case 8: // Off
                        targetSegment->setPrimaryColor(0, 0, 0, 0);
                        targetSegment->setBrightness(0);
                        break;
                    default:
                        logWarningP("Unknown preset %d for segment %d", preset, channel);
                        break;
                }
                break;
            }

            case NEO_KoSegmentPower:
            {
                bool power = ko.value(DPT_Switch);
                logDebugP("Segment %d Power: %s", channel, power ? "ON" : "OFF");

                SegmentConfig& cfg = _segments[channel];

                if (power)
                {
                    logDebugP("Segment %d Power ON: savedValid=%d, savedLastWasEffect=%d, savedEffectValid=%d, type=%d, savedRGB=(%d,%d,%d), savedBri=%d",
                              channel, cfg.savedValid, cfg.savedLastWasEffect, cfg.savedEffectValid, cfg.savedEffectType,
                              cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedBrightness);

                    // Update power state for flash persistence
                    cfg.savedPower = 1;

                    if (cfg.savedLastWasEffect && cfg.savedEffectValid && cfg.savedEffectType > 0)
                    {
                        // Restore effect (parameters are loaded from ETS automatically)
                        applyEffectToSegment(targetSegment, cfg.savedEffectType);
                        _effectConfiguration->setupEffectConfiguration(targetSegment);
                        targetSegment->setBrightness(cfg.savedBrightness == 0 ? 255 : cfg.savedBrightness);
                    }
                    else if (cfg.savedValid)
                    {
                        // IMPORTANT: Set brightness FIRST before setPrimaryColor() to ensure pixels are visible
                        targetSegment->setBrightness(cfg.savedBrightness == 0 ? 255 : cfg.savedBrightness);
                        // savedWW is always 0 for RGB strips, contains actual value for RGBW strips
                        targetSegment->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW);
                    }
                    else
                    {
                        if (targetSegment->getBrightness() == 0)
                        {
                            targetSegment->setBrightness(255);
                        }
                    }
                }
                else
                {
                    // Snapshot BEFORE clearing
                    uint8_t r = 0, g = 0, b = 0;
                    if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4)
                    {
                        uint8_t w = 0;
                        targetSegment->getPixel(0, r, g, b, w);
                        cfg.savedWW = w;
                    }
                    else
                    {
                        targetSegment->getPixel(0, r, g, b);
                    }
                    cfg.savedR = r;
                    cfg.savedG = g;
                    cfg.savedB = b;
                    cfg.savedBrightness = targetSegment->getBrightness();
                    cfg.savedValid = true;

                    // Update power state for flash persistence
                    cfg.savedPower = 0;

                    // Track if effect was active (effect type already stored in cfg.savedEffectType)
                    cfg.savedLastWasEffect = (targetSegment->getEffect() != nullptr);

                    logDebugP("Segment %d Power OFF: snapshot RGB=(%d,%d,%d), W=%d, Bri=%d, effectType=%d",
                              channel, cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedBrightness, cfg.savedEffectType);

                    targetSegment->setBrightness(0);
                    if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4)
                    {
                        targetSegment->setPrimaryColor(0, 0, 0, 0);
                    }
                    else
                    {
                        targetSegment->setPrimaryColor(0, 0, 0, 0);
                    }
                }

                _channelIndex = channel;
                bool changed = KoNEO_SegmentPowerState.valueNoSendCompare(power, DPT_Switch);
                if (changed) KoNEO_SegmentPowerState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoSegmentBrightness:
            {
                // DPT 5.001 sends 0-100 (percentage), need to scale to 0-255 for internal use
                uint8_t brightnessPercent = ko.value(DPT_Scaling);
                uint8_t brightness = (brightnessPercent * 255) / 100;
                logInfoP("Segment %d Brightness: %d%% (scaled to %d/255)", channel, brightnessPercent, brightness);

                // Get segment config to access saved colors
                SegmentConfig& cfgB = _segments[channel];

                // Persist desired brightness for power-restore (before global scaling)
                cfgB.savedBrightness = brightness;

                // Apply global brightness scaling if active (< 255)
                uint8_t effectiveBrightness = brightness;
                if (_globalBrightness < 255)
                {
                    effectiveBrightness = (brightness * _globalBrightness) / 255;
                    logDebugP("Segment %d: desired=%d%%, global=%d%%, effective=%d%%",
                              channel,
                              (brightness * 100 + 127) / 255,
                              (_globalBrightness * 100 + 127) / 255,
                              (effectiveBrightness * 100 + 127) / 255);
                }

                // Set the effective brightness level
                targetSegment->setBrightness(effectiveBrightness);

                // Send status feedback (send back percentage)
                _channelIndex = channel;
                bool changed = KoNEO_SegmentBrightnessState.valueNoSendCompare(brightnessPercent, DPT_Scaling);
                if (changed) KoNEO_SegmentBrightnessState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            // Warm White and Cool White control
            case NEO_KoWW:
            {
                uint8_t ww = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d Warm White: %d", channel, ww);

                // Store warm white in primaryWW, preserve cool white (5-channel RGBCCT)
                uint8_t r = targetSegment->getConfig().r();
                uint8_t g = targetSegment->getConfig().g();
                uint8_t b = targetSegment->getConfig().b();
                uint8_t cw = targetSegment->getConfig().cw();
                targetSegment->setPrimaryColor(r, g, b, ww, cw);
                break;
            }

            case NEO_KoCW:
            {
                uint8_t cw = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d Cool White: %d", channel, cw);

                // Store cool white in primaryCW, preserve warm white (5-channel RGBCCT)
                uint8_t r = targetSegment->getConfig().r();
                uint8_t g = targetSegment->getConfig().g();
                uint8_t b = targetSegment->getConfig().b();
                uint8_t ww = targetSegment->getConfig().ww();
                targetSegment->setPrimaryColor(r, g, b, ww, cw);
                break;
            }

            // RGBW combined control
            case NEO_KoRGBW:
            {
                uint32_t rgbw = ko.value(DPT_Colour_RGBW);
                uint8_t r = (rgbw >> 24) & 0xFF; // RGBW
                uint8_t g = (rgbw >> 16) & 0xFF;
                uint8_t b = (rgbw >> 8) & 0xFF;
                uint8_t w = rgbw & 0xFF;
                logInfoP("Segment %d RGBW: R=%d G=%d B=%d W=%d", channel, r, g, b, w);

                // Store in config and apply
                targetSegment->setPrimaryColor(r, g, b, w);

                // Persist desired static color for power-restore
                {
                    SegmentConfig& cfg = _segments[channel];
                    cfg.savedR = r;
                    cfg.savedG = g;
                    cfg.savedB = b;
                    cfg.savedWW = w;
                    cfg.savedBrightness = targetSegment->getBrightness();
                    cfg.savedValid = true;
                    cfg.savedLastWasEffect = false;
                }

                // Send status feedback
                _channelIndex = channel;
                bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
                if (changed) KoNEO_RGBWState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            // Relative controls (DPT 3.007 - relative dimming)
            case NEO_KoBriRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::BRIGHTNESS, rel, channel);
                break;
            }

            case NEO_KoRRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::RED, rel, channel);
                break;
            }

            case NEO_KoGRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::GREEN, rel, channel);
                break;
            }

            case NEO_KoBRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::BLUE, rel, channel);
                break;
            }

            case NEO_KoWRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::WHITE, rel, channel);
                break;
            }

            case NEO_KoWWRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::WARM_WHITE, rel, channel);
                break;
            }

            case NEO_KoCWRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::COOL_WHITE, rel, channel);
                break;
            }

            case NEO_KoHRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::HUE, rel, channel);
                break;
            }

            case NEO_KoSRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::SATURATION, rel, channel);
                break;
            }

            case NEO_KoVRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::VALUE, rel, channel);
                break;
            }

            case NEO_KoRGBRel:
            {
                // RGB relative control - expects 3-byte RGB value with relative changes
                uint32_t rgbRel = ko.value(DPT_Colour_RGB);
                uint8_t rRel = (rgbRel >> 16) & 0xFF;
                uint8_t gRel = (rgbRel >> 8) & 0xFF;
                uint8_t bRel = rgbRel & 0xFF;

                // Get current RGB values
                uint8_t currentR = targetSegment->getConfig().r();
                uint8_t currentG = targetSegment->getConfig().g();
                uint8_t currentB = targetSegment->getConfig().b();
                uint8_t currentW = targetSegment->getConfig().w();

                // Apply relative changes (treating 128 as no change, <128 as decrease, >128 as increase)
                int16_t newR = currentR + ((int16_t)rRel - 128);
                int16_t newG = currentG + ((int16_t)gRel - 128);
                int16_t newB = currentB + ((int16_t)bRel - 128);

                // Constrain to valid range
                newR = constrain(newR, 0, 255);
                newG = constrain(newG, 0, 255);
                newB = constrain(newB, 0, 255);

                logInfoP("Segment %d RGB Relative: R=%d->%d, G=%d->%d, B=%d->%d", channel,
                         currentR, newR, currentG, newG, currentB, newB);

                // Update config and apply
                targetSegment->setPrimaryColor((uint8_t)newR, (uint8_t)newG, (uint8_t)newB, currentW);
                break;
            }

            case NEO_KoHSVRel:
            {
                // HSV relative control - expects 3-byte HSV value with relative changes
                uint32_t hsvRel = ko.value(DPT_Colour_RGB); // Using RGB DPT for 3-byte HSV data
                uint8_t hRel = (hsvRel >> 16) & 0xFF;       // Hue relative (scaled to 0-255)
                uint8_t sRel = (hsvRel >> 8) & 0xFF;        // Saturation relative
                uint8_t vRel = hsvRel & 0xFF;               // Value relative

                // Get current RGB values and convert to HSV
                uint8_t currentR = targetSegment->getConfig().r();
                uint8_t currentG = targetSegment->getConfig().g();
                uint8_t currentB = targetSegment->getConfig().b();
                uint8_t currentW = targetSegment->getConfig().w();

                uint8_t currentH, currentS, currentV;
                ColorHelper::rgbToHSV(currentR, currentG, currentB, currentH, currentS, currentV);

                // Apply relative changes (treating 128 as no change, <128 as decrease, >128 as increase)
                int16_t newH = currentH + ((int16_t)hRel - 128);
                int16_t newS = currentS + ((int16_t)sRel - 128);
                int16_t newV = currentV + ((int16_t)vRel - 128);

                // Constrain values to valid ranges
                if (newH < 0) newH += 256;
                else if (newH >= 256)
                    newH -= 256; // Hue wraps around in 0-255 range
                newS = constrain(newS, 0, 255);
                newV = constrain(newV, 0, 255);

                logInfoP("Segment %d HSV Relative: H=%d->%d, S=%d->%d, V=%d->%d", channel,
                         currentH, newH, currentS, newS, currentV, newV);

                // Convert back to RGB and apply
                uint8_t newR, newG, newB;
                ColorHelper::hsvToRGB((uint8_t)newH, (uint8_t)newS, (uint8_t)newV, newR, newG, newB);

                // Update config and apply
                targetSegment->setPrimaryColor(newR, newG, newB, currentW);
                break;
            }

            case NEO_KoRGBWRel:
            {
                // RGBW relative control - expects 4-byte RGBW value with relative changes
                uint32_t rgbwRel = ko.value(DPT_Colour_RGBW);
                uint8_t rRel = (rgbwRel >> 24) & 0xFF;
                uint8_t gRel = (rgbwRel >> 16) & 0xFF;
                uint8_t bRel = (rgbwRel >> 8) & 0xFF;
                uint8_t wRel = rgbwRel & 0xFF;

                // Get current RGBW values
                uint8_t currentR = targetSegment->getConfig().r();
                uint8_t currentG = targetSegment->getConfig().g();
                uint8_t currentB = targetSegment->getConfig().b();
                uint8_t currentW = targetSegment->getConfig().w();

                // Apply relative changes (treating 128 as no change, <128 as decrease, >128 as increase)
                int16_t newR = currentR + ((int16_t)rRel - 128);
                int16_t newG = currentG + ((int16_t)gRel - 128);
                int16_t newB = currentB + ((int16_t)bRel - 128);
                int16_t newW = currentW + ((int16_t)wRel - 128);

                // Constrain to valid range
                newR = constrain(newR, 0, 255);
                newG = constrain(newG, 0, 255);
                newB = constrain(newB, 0, 255);
                newW = constrain(newW, 0, 255);

                logInfoP("Segment %d RGBW Relative: R=%d->%d, G=%d->%d, B=%d->%d, W=%d->%d", channel,
                         currentR, newR, currentG, newG, currentB, newB, currentW, newW);

                // Update config and apply
                targetSegment->setPrimaryColor((uint8_t)newR, (uint8_t)newG, (uint8_t)newB, (uint8_t)newW);
                break;
            }

            // Status-only KOs (prevent "Unhandled KO Index" warnings)
            case NEO_KoSegmentPowerState:
            case NEO_KoSegmentBrightnessState:
            case NEO_KoCCTState:
            case NEO_KoFxState:
            case NEO_KoPresetState:
            case NEO_KoRGBState:
            case NEO_KoHSVState:
            case NEO_KoRGBWState:
                // These are status/output-only KOs, ignore incoming writes
                break;

            default:
                logInfoP("Segment %d Unhandled KO Index: %d", channel, koIndex);
                break;
        }

        // Restore channel index
        _channelIndex = oldChannelIndex;

        // Auto-update cycle will render changes
    }
}

// Console: show help entries by delegating to core NeoPixel module
void NeoPixelBusModule::showHelp()
{
    // Print the 'neo' command group header via core module
    _neoPixel.showHelp();
    
    #ifdef OPENKNX_DEBUG
    // Add NeoPixel-specific debug commands
    openknx.console.printHelpLine("neoa", "Show NeoPixel ETS configuration analysis");
    #endif
}

// Console: process commands by delegating 'neo' prefixed commands
bool NeoPixelBusModule::processCommand(const std::string command, bool diagnose)
{
    #ifdef OPENKNX_DEBUG
    // Handle NeoPixel-specific debug commands
    if (command == "neoa")
    {
        debugShowConfiguration();
        return true;
    }
    #endif
    
    // Forward to core module's console handler
    return _neoPixel.processCommand(command, diagnose);
}

// FunctionProperty: Handle ETS online functions (hardware detection)
bool NeoPixelBusModule::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    // Object 158 = NeoPixel Module, Property 10 = Hardware Detection
    if (objectIndex != 158 || propertyId != 10)
        return false;

    if (!knx.configured())
    {
        resultData[0] = 1; // Error: device not configured
        resultLength = 1;
        return false;
    }

    // Read hardware ID from compile-time define
    #ifdef DEVICE_HW_ID
        uint16_t hwId = DEVICE_HW_ID;
        
        // Return format: [0, hwId_high, hwId_low]
        resultData[0] = 0;              // Success
        resultData[1] = (hwId >> 8);    // High byte
        resultData[2] = (hwId & 0xFF);  // Low byte
        resultLength = 3;
        
        logInfoP("Hardware detection: DEVICE_HW_ID = 0x%04X", hwId);
        return true;
    #else
        // Hardware ID not defined in build
        resultData[0] = 2; // Error: hardware ID not available
        resultLength = 1;
        logErrorP("Hardware detection failed: DEVICE_HW_ID not defined");
        return false;
    #endif
}

void NeoPixelBusModule::configureFromETS()
{
    // Initialize the OFM-NeoPixel module first
    _neoPixel.init();

    // Log hardware configuration and check for mismatch
    #ifdef DEVICE_HW_ID
        uint8_t hwIndex = getCurrentHardwareIndex();
        const char* hwName = HardwareMapping::getHardwareName(hwIndex);
        logInfoP("Hardware: %s (ID: 0x%04X, Index: %d) - Compile-time mode", 
                 hwName ? hwName : "Unknown", DEVICE_HW_ID, hwIndex);
        
        // Check if ETS configuration matches compiled hardware
        #ifdef ParamNEO_NEO_NeoPixelHardwareSelect
            uint16_t selectedHwIndex = (uint16_t)ParamNEO_NEO_NeoPixelHardwareSelect;  // Parameter contains index now
            uint8_t compiledHwIndex = HardwareMapping::mapDeviceHwIdToIndex(DEVICE_HW_ID);  // Convert HW_ID to index
            if (selectedHwIndex != compiledHwIndex) {
                _hwConfigMismatch = true;
                const char* compiledHwName = HardwareMapping::getHardwareName(compiledHwIndex);
                const char* etsHwName = HardwareMapping::getHardwareName(selectedHwIndex);
                logErrorP("!!!! HARDWARE MISMATCH !!!!");
                logErrorP("  Compiled for:  %s (Index: %d, ID: 0x%04X)", 
                         compiledHwName ? compiledHwName : "Unknown", compiledHwIndex, DEVICE_HW_ID);
                logErrorP("  ETS configured: %s (Index: %d)", 
                         etsHwName ? etsHwName : "Unknown", selectedHwIndex);
                logErrorP("  GPIO Port configuration from ETS will be IGNORED, to prevent damage and unexpected behaviors!");
                logErrorP("  Please Choose the correct Hardware in ETS or use correct firmware for this device.");
                logErrorP("!!!! HARDWARE MISMATCH !!!!");
                
                // Set Prog LED to red + error code blinking (Prio 2 - overrides prog mode)
                openknx.ledFunctions.get(OPENKNX_LEDFUNC_BASE_PROG)->color(OpenKNX::Led::Color::Red);
                openknx.ledFunctions.get(OPENKNX_LEDFUNC_BASE_PROG)->errorCode(1); // 1x blink = HW mismatch
            } else {
                _hwConfigMismatch = false;
            }
        #endif
    #else
        #ifdef ParamNEO_NEO_NeoPixelHardwareSelect
            uint16_t selectedHwIndex = (uint16_t)ParamNEO_NEO_NeoPixelHardwareSelect;  // Parameter contains index now
            const char* hwName = HardwareMapping::getHardwareName(selectedHwIndex);
            uint16_t selectedHwId = HardwareMapping::mapIndexToDeviceHwId(selectedHwIndex);  // Convert index to HW_ID for logging
            logInfoP("Hardware: %s (ID: 0x%04X, Index: %d) - ETS runtime selection", 
                     hwName ? hwName : "Unknown", selectedHwId, selectedHwIndex);
            _hwConfigMismatch = false; // No mismatch in runtime mode
        #else
            logWarningP("Hardware selection not available - using default configuration");
            _hwConfigMismatch = false;
        #endif
    #endif

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
    _totalLeds = 0;

    // CRITICAL: Delete old PhysicalStrip objects before clearing vector!
    // This prevents memory leaks and deregisters old DMA handlers
    for (auto* strip : _physicalStrips)
    {
        if (strip) delete strip;
    }
    _physicalStrips.clear();

    // Clear virtual strip configuration
    _virtualStripConfiguration.clear();

    // PRE-SCAN: Mark all manually configured GPIO pins as used BEFORE auto-allocation starts
    // This prevents auto-allocation from using pins that are manually configured on other strips
    logInfoP("Pre-scanning %d strips for manual GPIO configurations...", maxStrips);
    for (uint8_t i = 0; i < maxStrips; ++i)
    {
        _channelIndex = i;
        logInfoP("PRE-SCAN Strip %d: Set _channelIndex = %d", i, _channelIndex);

        const uint16_t pixels = (uint16_t)ParamNEOSTRIP_NEOLength;
        logInfoP("PRE-SCAN Strip %d: Read pixels = %d (from offset %d)", i, pixels, NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize + NEOSTRIP_NEOLength);
        if (pixels == 0) continue; // Skip strips with 0 LEDs

        const uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const LedProtocol proto = StripConfiguration::mapProtocol(ledTypeParam);
        const bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;

        if (gpioManualConfig)
        {
            if (isSpiProtocol(proto))
            {
                // SPI protocols need 2 pins - read from manual GPIO parameters
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
                // 1-Wire protocols need 1 pin - read from manual GPIO parameter
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
        else
        {
            // Hardware-assisted mode: Get GPIO from hardware-specific port selection
            // CRITICAL: Ignore hardware port config if there's a hardware mismatch!
            if (_hwConfigMismatch) {
                logErrorP("Strip %d: Hardware mismatch detected - ignoring hardware port selection", i);
                // Don't pre-mark any hardware ports, force manual configuration
            } else {
                uint8_t dataPortIndex = getGpioDataPortForHw(i);
                
                if (isHardwarePortSelected(dataPortIndex))
                {
                    uint8_t dataGpio = mapPortIndexToGpio(dataPortIndex);
                    
                    if (dataGpio != 255) // Valid GPIO
                    {
                        if (!isPinUsed(dataGpio))
                        {
                            usedPins.push_back(dataGpio);
                            logInfoP("Strip %d: Pre-marked hardware Data GPIO %d (port %d) as used", i, dataGpio, dataPortIndex);
                        }
                        else
                        {
                            logErrorP("Strip %d: Hardware Data GPIO %d (port %d) already marked as used!", i, dataGpio, dataPortIndex);
                        }
                        
                        // For SPI protocols, also mark clock GPIO
                        if (isSpiProtocol(proto))
                        {
                            uint8_t clockPortIndex = getGpioClockPortForHw(i);
                            
                            if (isHardwarePortSelected(clockPortIndex))
                            {
                                uint8_t sckGpio = mapPortIndexToGpio(clockPortIndex);
                                
                                if (sckGpio != 255) // Valid GPIO
                                {
                                    if (!isPinUsed(sckGpio))
                                    {
                                        usedPins.push_back(sckGpio);
                                        logInfoP("Strip %d: Pre-marked hardware SCK GPIO %d (port %d) as used", i, sckGpio, clockPortIndex);
                                    }
                                    else
                                    {
                                        logErrorP("Strip %d: Hardware SCK GPIO %d (port %d) already marked as used!", i, sckGpio, clockPortIndex);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    logInfoP("Pre-scan complete: %d manual GPIO pins marked as used", (int)usedPins.size());

    // 2) Create all physical strips in configuration order (auto-allocation will skip pre-marked pins)
    for (uint8_t i = 0; i < maxStrips; ++i)
    {
        _channelIndex = i;

        const uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const LedProtocol proto = StripConfiguration::mapProtocol(ledTypeParam);

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
            order = StripConfiguration::mapColorOrder((uint8_t)ParamNEOSTRIP_NEOColourOrder);
        }

        const uint8_t ledType = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const uint8_t dataGpio = (uint8_t)ParamNEOSTRIP_NEODataGPIO;
        const uint16_t pixels = (uint16_t)ParamNEOSTRIP_NEOLength;
        
        // DEBUG: Log all parameters for this strip
        logInfoP("Strip %d: LEDType=%d, pixels=%d (channelIndex=%d)", i, ledType, pixels, _channelIndex);
        logInfoP("  -> Offset calculation: base=%d + (ch=%d * size=%d) = %d", 
                 NEOSTRIP_ParamBlockOffset, _channelIndex, NEOSTRIP_ParamBlockSize,
                 NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize);
        
        // DEBUG: SPI-specific parameters
        if (isSpiProtocol(proto))
        {
            const uint8_t mosiGpio = (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO;
            const uint8_t sckGpio = (uint8_t)ParamNEOSTRIP_NEOClockGPIO;
            logInfoP("  -> SPI: MOSI=%d (offset %d+%d=%d), SCK=%d (offset %d+%d=%d)", 
                     mosiGpio, 
                     NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize, NEOSTRIP_NEOSPIMOSIGPIO,
                     NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize + NEOSTRIP_NEOSPIMOSIGPIO,
                     sckGpio,
                     NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize, NEOSTRIP_NEOClockGPIO,
                     NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize + NEOSTRIP_NEOClockGPIO);
        }

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
                // Hardware-assisted mode: Use hardware-specific port mapping
                // CRITICAL: Block hardware port usage if there's a hardware mismatch!
                if (_hwConfigMismatch) {
                    logErrorP("Strip %d: Hardware mismatch - cannot use hardware ports! Please configure GPIO manually.", i);
                    continue; // Skip this strip completely
                }
                
                uint8_t dataPortIndex = getGpioDataPortForHw(i);
                uint8_t clockPortIndex = getGpioClockPortForHw(i);
                
                if (isHardwarePortSelected(dataPortIndex) && isHardwarePortSelected(clockPortIndex))
                {
                    // Map port indices to actual GPIO numbers
                    mosiGpio = mapPortIndexToGpio(dataPortIndex);
                    sckGpio = mapPortIndexToGpio(clockPortIndex);
                    
                    if (mosiGpio == 255 || sckGpio == 255)
                    {
                        logErrorP("Strip %d: Invalid hardware port mapping (data:%d->%d, clock:%d->%d)", 
                                  i, dataPortIndex, mosiGpio, clockPortIndex, sckGpio);
                        // Fallback to automatic allocation
                        mosiGpio = 19;
                        sckGpio = 18;
                    }
                    else
                    {
                        logInfoP("Strip %d: Using hardware ports (MOSI:%d, SCK:%d)", i, mosiGpio, sckGpio);
                    }
                }
                else
                {
                    // Fallback to automatic pin allocation if hardware ports not selected
                    logWarningP("Strip %d: No hardware ports selected, using automatic allocation", i);
                    
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

            // Use manager directly for frequency parameter (NeoPixel wrapper doesn't expose this overload yet)
            auto mgr = _neoPixel.getManager();
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
                     i, pixels, mosiGpio, sckGpio, getProtocolName(proto), getColorOrderName(order),
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
                // Hardware-assisted mode: Use hardware-specific port mapping
                // CRITICAL: Block hardware port usage if there's a hardware mismatch!
                if (_hwConfigMismatch) {
                    logErrorP("Strip %d: Hardware mismatch - cannot use hardware ports! Please configure GPIO manually.", i);
                    continue; // Skip this strip completely
                }
                
                uint8_t dataPortIndex = getGpioDataPortForHw(i);
                
                if (isHardwarePortSelected(dataPortIndex))
                {
                    // Map port index to actual GPIO number
                    dataGpioPin = mapPortIndexToGpio(dataPortIndex);
                    
                    if (dataGpioPin == 255)
                    {
                        logErrorP("Strip %d: Invalid hardware port mapping (data:%d->%d)", 
                                  i, dataPortIndex, dataGpioPin);
                        // Fallback to automatic allocation
                        dataGpioPin = dataGpio;
                    }
                    else
                    {
                        logInfoP("Strip %d: Using hardware port (Data:%d)", i, dataGpioPin);
                    }
                }
                else
                {
                    // Fallback to automatic pin allocation if hardware port not selected
                    logWarningP("Strip %d: No hardware port selected, using automatic allocation", i);
                    
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
            }

            phys = _neoPixel.addStrip(dataGpioPin, pixels, proto, order);
            logInfoP("1-Wire Strip %d: %d LEDs, GPIO=%d, Protocol=%s, ColorOrder=%s%s",
                     i, pixels, dataGpioPin, getProtocolName(proto), getColorOrderName(order),
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
            _totalLeds += pixels;
            _physicalStrips.push_back(phys);

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
                configureColorCorrection();
                updateColorCorrection(); // Set correction parameters on VirtualStrip once

                auto* cfg = phys->getConfig();
                if (cfg)
                {
                    // Configure gamma correction in the physical strip config
                    if (_gammaCorrectionEnabled)
                    {
                        cfg->setGammaCorrection(_gammaValue);
                        logInfoP("Strip %d: Gamma correction enabled (value=%.2f)", i, _gammaValue);
                    }

                    // Configure white balance in the physical strip config
                    if (_whiteBalanceEnabled)
                    {
                        cfg->setWhiteBalance(_whiteBalanceRed, _whiteBalanceGreen, _whiteBalanceBlue);
                        logInfoP("Strip %d: White balance enabled (R:%d G:%d B:%d)", i, _whiteBalanceRed, _whiteBalanceGreen, _whiteBalanceBlue);
                    }
                }

                logInfoP("Strip %d: Color correction enabled - Gamma=%s(%.1f), WhiteBalance=%s(R:%d G:%d B:%d)",
                         i,
                         _gammaCorrectionEnabled ? "ON" : "OFF", _gammaValue,
                         _whiteBalanceEnabled ? "ON" : "OFF", _whiteBalanceRed, _whiteBalanceGreen, _whiteBalanceBlue);
            }

            // Configure strip options (swap mode and skip LEDs)
            configureStripOptions();
            if (_swapMode > 0 || _skipFirstLeds > 0)
            {
                logInfoP("Strip %d: Strip options - SwapMode=%d, SkipFirstLEDs=%d",
                         i, _swapMode, _skipFirstLeds);
            }
        }
        else
        {
            logErrorP("Failed to create strip %d", i);
        }
    }

    logInfoP("Created %d physical strips with total %d LEDs", maxStrips, _totalLeds);

    // 3) Configure virtual strip order from ETS parameters
    configureVirtualStripOrder();

    // 4) Configure segments once, now that we know the virtual strip layout
    // Segments are defined on the virtual strip
    if (_totalLeds > 0)
    {
        configureSegments();
        if (_numberOfSegments > 0)
        {
            logInfoP("Configured %d segments for virtual strip (%d total LEDs)", _numberOfSegments, _totalLeds);
        }
    }

    // 5) Create virtual strip using configured order and apply segments
    if (!_physicalStrips.empty() && _totalLeds > 0)
    {
        createVirtualStripWithOrder();

        // Configure effects for segments (now that virtual strip and segments exist)
        if (_numberOfSegments > 0)
        {
            _effectConfiguration->configureEffects();
            logInfoP("Applied effects to %d segments", _numberOfSegments);

            // Turn off all segments after configuration to prevent showing ETS defaults
            // They will be restored to correct state in processAfterStartupDelay()
            // Note: show() is called later in setup() after _neoPixel.setup() initializes hardware
            for (auto& segConfig : _segments)
            {
                if (segConfig.segment)
                {
                    segConfig.segment->setBrightness(0);
                    segConfig.segment->clearAll();
                }
            }
            _clearLedsAfterSetup = true; // Flag to call show() after hardware init
            logInfoP("LEDs prepared for clearing - will show after hardware init");
        }

        // Configure power management using OFM PowerManager
        configurePowerManagement();

        // Apply initial color correction to ensure ETS settings take effect immediately
        if (_gammaCorrectionEnabled || _whiteBalanceEnabled || _swapMode > 0)
        {
            forceColorCorrectionUpdate();
            logInfoP("Applied initial color correction settings");
        }
    }

    logInfoP("Configuration complete: %d physical strips, %d total LEDs, %d segments",
             maxStrips, _totalLeds, _numberOfSegments);
}

// ============================================================================
// helpers
// ============================================================================

void NeoPixelBusModule::configureVirtualStripOrder()
{
    _virtualStripConfiguration.clear();

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
        if (physStripIndex == 0 || physStripIndex > _physicalStrips.size())
        {
            continue;
        }

        // Convert from 1-based to 0-based indexing
        physStripIndex -= 1;

        // Get the LED count for this physical strip
        if (physStripIndex < _physicalStrips.size() && _physicalStrips[physStripIndex])
        {
            uint16_t ledCount = _physicalStrips[physStripIndex]->getLedCount();
            uint16_t virtualStart = startPositions[pos];

            // Convert from 1-based (ETS user-friendly) to 0-based (internal indexing)
            // ETS shows "Start LED = 1" but internally we need offset 0
            uint16_t virtualStartZeroBased = (virtualStart > 0) ? (virtualStart - 1) : 0;

            _virtualStripConfiguration.emplace_back(physStripIndex, virtualStartZeroBased, ledCount);

            logInfoP("Virtual position %d: Physical strip %d (%d LEDs) starts at virtual position %d (ETS: %d)",
                     pos + 1, physStripIndex + 1, ledCount, virtualStartZeroBased, virtualStart);
        }
        else
        {
            logErrorP("Invalid physical strip index %d at virtual position %d", physStripIndex + 1, pos + 1);
        }
    }

    // Validate the configuration
    if (_virtualStripConfiguration.empty())
    {
        logWarningP("No valid virtual strip configuration found - using default order");
        createDefaultVirtualStripOrder();
    }
    else
    {
        logInfoP("Virtual strip configuration complete: %d physical strips mapped",
                 static_cast<int>(_virtualStripConfiguration.size()));
    }
}

void NeoPixelBusModule::createVirtualStripWithOrder()
{
    if (_physicalStrips.empty() || _totalLeds == 0 || _virtualStripConfiguration.empty())
    {
        logErrorP("Cannot create virtual strip: insufficient configuration");
        return;
    }

    // Determine if any physical strip requires RGBW (4 bytes) or RGBCCT (5 bytes per LED)
    bool needsRGBW = false;
    bool needsRGBCCT = false;
    for (const auto* phys : _physicalStrips)
    {
        if (phys)
        {
            ColorOrder order = phys->getColorOrder();
            // Check for 5-channel first (RGBCCT, GRBCCT, RGBCTW, GRBCTW)
            if (order == ColorOrder::RGBCCT || order == ColorOrder::GRBCCT ||
                order == ColorOrder::RGBCTW || order == ColorOrder::GRBCTW)
            {
                needsRGBCCT = true;
                break; // 5-channel is the maximum, no need to check further
            }
            // Check for 4-channel (RGBW, GRBW)
            else if (order == ColorOrder::RGBW || order == ColorOrder::GRBW)
            {
                needsRGBW = true;
            }
        }
    }

    // Create virtual strip: RGBCCT if any 5-channel, RGBW if any 4-channel, otherwise RGB
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

    _virtualStrip = _neoPixel.addVirtualStrip(_totalLeds, virtualOrder);
    if (!_virtualStrip)
    {
        logErrorP("Failed to create virtual strip");
        return;
    }

    if (needsRGBCCT)
    {
        logInfoP("VirtualStrip created with RGBCCT support (5 bytes/LED) for 5-channel physical strips");
    }
    else if (needsRGBW)
    {
        logInfoP("VirtualStrip created with RGBW support (4 bytes/LED) for RGBW physical strips");
    }

    auto mgr = _neoPixel.getManager();
    if (!mgr)
    {
        logErrorP("NeoPixelManager not available for virtual strip creation");
        return;
    }

    // Attach physical strips to virtual strip according to the configured order
    for (const auto& config : _virtualStripConfiguration)
    {
        if (config.physicalStripIndex >= _physicalStrips.size())
        {
            logErrorP("Invalid physical strip index %d in virtual configuration", config.physicalStripIndex);
            continue;
        }

        PhysicalStrip* phys = _physicalStrips[config.physicalStripIndex];
        if (!phys)
        {
            logErrorP("Physical strip %d is null", config.physicalStripIndex);
            continue;
        }

        // Apply skip offset if configured for this strip
        uint8_t oldChannelIndex = _channelIndex;
        _channelIndex = config.physicalStripIndex;
        uint16_t skipLeds = ParamNEOSTRIP_NEOSkipFirstLEDs;
        _channelIndex = oldChannelIndex;

        uint16_t effectiveLeds = config.ledCount;
        uint16_t attachOffset = config.virtualStartPosition;

        // Apply skip offset if configured
        if (skipLeds > 0 && skipLeds < effectiveLeds)
        {
            logDebugP("Strip %d: Skipping first %d LEDs, effective range: %d-%d",
                      config.physicalStripIndex, skipLeds, skipLeds, effectiveLeds - 1);
        }

        if (mgr->attachPhysicalToVirtual(_virtualStrip, phys, attachOffset))
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
             _totalLeds, static_cast<int>(_virtualStripConfiguration.size()));
    logInfoP("ColorOrder Design: VirtualStrip=RGB (internal), PhysicalStrips=hardware-specific");

    // Configure color correction parameters on VirtualStrip
    // (These are applied during rendering, NOT in-place!)
    // NOTE: setColorCorrection removed from VirtualStrip - color correction deactivated for now
    /*
    if (_virtualStrip) {
      _virtualStrip->setColorCorrection(
        _gammaCorrectionEnabled, _gammaValue,
        _whiteBalanceEnabled, _whiteBalanceRed, _whiteBalanceGreen, _whiteBalanceBlue,
        _swapMode
      );
      logInfoP("VirtualStrip color correction configured: Gamma=%s(%.1f), WB=%s, Swap=%d",
               _gammaCorrectionEnabled ? "ON" : "OFF", _gammaValue,
               _whiteBalanceEnabled ? "ON" : "OFF",
               _swapMode);
    }
    */

    // Create segments after virtual strip is ready
    if (_numberOfSegments > 0)
    {
        createSegments();
        applySegmentConfiguration();
        logInfoP("Created %d segments on virtual strip", _numberOfSegments);
    }
}

void NeoPixelBusModule::createDefaultVirtualStripOrder()
{
    _virtualStripConfiguration.clear();

    // Create simple sequential order: Strip 1, Strip 2, Strip 3, etc.
    uint16_t currentStart = 0;
    for (size_t i = 0; i < _physicalStrips.size(); i++)
    {
        if (_physicalStrips[i])
        {
            uint16_t ledCount = _physicalStrips[i]->getLedCount();
            _virtualStripConfiguration.emplace_back(static_cast<uint8_t>(i), currentStart, ledCount);

            logInfoP("Default order: Physical strip %d (%d LEDs) starts at virtual position %d",
                     static_cast<int>(i), ledCount, currentStart);

            currentStart += ledCount;
        }
    }
}

bool NeoPixelBusModule::isSpiProtocol(LedProtocol protocol)
{
    switch (protocol)
    {
        case LedProtocol::APA102:
        case LedProtocol::APA102_CLONE:
        case LedProtocol::SK9822:
        case LedProtocol::WS2801:
        case LedProtocol::LPD8806:
            return true;
        default:
            return false;
    }
}

const char* NeoPixelBusModule::getColorOrderName(ColorOrder order)
{
    switch (order)
    {
        case ColorOrder::RGB: return "RGB";
        case ColorOrder::RBG: return "RBG";
        case ColorOrder::GRB: return "GRB";
        case ColorOrder::GBR: return "GBR";
        case ColorOrder::BGR: return "BGR";
        case ColorOrder::BRG: return "BRG";
        case ColorOrder::RGBW: return "RGBW";
        case ColorOrder::GRBW: return "GRBW";
        // 5-Channel Color Orders
        case ColorOrder::RGBCCT: return "RGBCCT";
        case ColorOrder::GRBCCT: return "GRBCCT";
        case ColorOrder::RGBCTW: return "RGBCTW";
        case ColorOrder::GRBCTW: return "GRBCTW";
        default: return "UNKNOWN";
    }
}

const char* NeoPixelBusModule::getProtocolName(LedProtocol protocol)
{
    switch (protocol)
    {
        case LedProtocol::WS2812: return "WS2812";
        case LedProtocol::WS2812B: return "WS2812B";
        case LedProtocol::WS2813: return "WS2813";
        case LedProtocol::WS2815: return "WS2815";
        case LedProtocol::WS2811: return "WS2811";
        case LedProtocol::SK6812: return "SK6812";
        case LedProtocol::SK6805: return "SK6805";
        case LedProtocol::WS2814: return "WS2814";
        case LedProtocol::TM1814: return "TM1814";
        case LedProtocol::GS8208: return "GS8208";
        case LedProtocol::APA102: return "APA102";
        case LedProtocol::SK9822: return "SK9822";
        case LedProtocol::WS2801: return "WS2801";
        case LedProtocol::LPD8806: return "LPD8806";
        // 5-Channel RGBCCT Protocols
        case LedProtocol::SK6812_RGBCCT: return "SK6812_RGBCCT";
        case LedProtocol::WS2814_RGBCCT: return "WS2814_RGBCCT";
        case LedProtocol::WS2805_RGBCCT: return "WS2805_RGBCCT";
        default: return "UNKNOWN";
    }
}

// ============================================================================
// Power Management Implementation
// ============================================================================

void NeoPixelBusModule::configurePowerManagement()
{
    auto mgr = _neoPixel.getManager();
    if (!mgr)
    {
        logErrorP("NeoPixelManager not available for power management configuration");
        return;
    }

    auto* powerManager = mgr->getPowerManager();
    if (!powerManager)
    {
        logErrorP("PowerManager not available");
        return;
    }

    // Read GLOBAL power parameters
    bool powerLimitEnabled = (bool)ParamNEO_NEOpowerLimitEnabled;

    // Configure power management in OFM
    powerManager->setEnabled(powerLimitEnabled);

    if (powerLimitEnabled)
    {
        // Read all ETS parameters
        uint8_t powerLimitType = (uint8_t)ParamNEO_NEOpowerLimitType; // 0=Global, 1=PerChannel, 2=PerLED
        uint16_t powerLimitGlobal = (uint16_t)ParamNEO_NEOpowerLimitGlobal;
        uint16_t currentPerChannel = (uint16_t)ParamNEO_NEOcurrentPerChannel;
        uint8_t currentPerLed = (uint8_t)ParamNEO_NEOcurrentPerLED;
        uint8_t autoBrightnessLimit = (uint8_t)ParamNEO_NEOautoBrightnessLimit;
        uint8_t powerLimitThreshold = (uint8_t)ParamNEO_NEOpowerLimitThreshold;
        uint8_t ablSlewRate = (uint8_t)ParamNEO_NEOablSlewRatePercent;

        // Set power limit mode
        PowerLimitMode mode;
        switch (powerLimitType)
        {
            case 0: mode = PowerLimitMode::GLOBAL; break;
            case 1: mode = PowerLimitMode::PER_CHANNEL; break;
            case 2: mode = PowerLimitMode::PER_LED; break;
            default: mode = PowerLimitMode::GLOBAL; break;
        }
        powerManager->setPowerLimitMode(mode);

        // Configure limits based on mode
        switch (mode)
        {
            case PowerLimitMode::GLOBAL:
                powerManager->setMaxCurrent(powerLimitGlobal);
                break;

            case PowerLimitMode::PER_CHANNEL:
                powerManager->setMaxCurrentPerChannel(currentPerChannel);
                break;

            case PowerLimitMode::PER_LED:
                powerManager->setMaxCurrentPerLed(currentPerLed);
                break;
        }

        // Set soft-limiting threshold (Phase 2)
        powerManager->setThresholdPercent(powerLimitThreshold);

        // Set auto brightness limit (Phase 2)
        powerManager->setMaxBrightnessPercent(autoBrightnessLimit);

        // Set brightness slew rate (Phase 3)
        powerManager->setBrightnessSlewRate(ablSlewRate);

        // Configure LED current profile based on ETS settings and detected strip type
        LedCurrentProfile profile;

        // Check if we have any RGBW (4-channel) or RGBCCT (5-channel) strips
        bool hasRgbwStrip = false;
        bool hasRgbcctStrip = false;
        for (const auto& strip : _physicalStrips)
        {
            if (strip)
            {
                ColorOrder order = strip->getColorOrder();
                // 5-channel: RGBCCT, GRBCCT, RGBCTW, GRBCTW
                if (order == ColorOrder::RGBCCT || order == ColorOrder::GRBCCT ||
                    order == ColorOrder::RGBCTW || order == ColorOrder::GRBCTW)
                {
                    hasRgbcctStrip = true;
                    break; // 5-channel is the maximum
                }
                // 4-channel: RGBW, GRBW
                else if (order == ColorOrder::RGBW || order == ColorOrder::GRBW)
                {
                    hasRgbwStrip = true;
                }
            }
        }

        if (currentPerLed > 0 && mode != PowerLimitMode::PER_LED)
        {
            // User specified custom current per LED
            // For RGB: divide by 3, RGBW: divide by 4, RGBCCT: divide by 5
            if (hasRgbcctStrip)
            {
                uint8_t perChannel = currentPerLed / 5;
                profile = LedCurrentProfile(perChannel, perChannel, perChannel, perChannel, perChannel);
            }
            else if (hasRgbwStrip)
            {
                uint8_t perChannel = currentPerLed / 4;
                profile = LedCurrentProfile(perChannel, perChannel, perChannel, perChannel);
            }
            else
            {
                uint8_t perChannel = currentPerLed / 3;
                profile = LedCurrentProfile(perChannel, perChannel, perChannel, 0);
            }
        }
        else
        {
            // Use default profile based on strip type
            if (hasRgbcctStrip)
            {
                profile = LedProfiles::SK6812_RGBCCT;
            }
            else if (hasRgbwStrip)
            {
                profile = LedProfiles::SK6812_RGBW;
            }
            else
            {
                profile = LedProfiles::WS2812B;
            }
        }

        powerManager->setLedProfile(profile);

        // Log configuration
        const char* modeNames[] = {"GLOBAL", "PER_CHANNEL", "PER_LED"};
        switch (mode)
        {
            case PowerLimitMode::GLOBAL:
                logInfoP("Power Management: %s mode - Limit=%dmA, Profile=R:%dmA G:%dmA B:%dmA WW:%dmA CW:%dmA",
                         modeNames[0], powerLimitGlobal, profile.redMA, profile.greenMA, profile.blueMA, profile.warmWhiteMA, profile.coolWhiteMA);
                break;
            case PowerLimitMode::PER_CHANNEL:
                logInfoP("Power Management: %s mode - Limit=%dmA per channel, Profile=R:%dmA G:%dmA B:%dmA WW:%dmA CW:%dmA",
                         modeNames[1], currentPerChannel, profile.redMA, profile.greenMA, profile.blueMA, profile.warmWhiteMA, profile.coolWhiteMA);
                break;
            case PowerLimitMode::PER_LED:
                logInfoP("Power Management: %s mode - Limit=%dmA per LED",
                         modeNames[2], currentPerLed);
                break;
        }

        logInfoP("  Threshold=%d%%, MaxBrightness=%d%%, SlewRate=%d%%/s",
                 powerLimitThreshold, autoBrightnessLimit, ablSlewRate);
    }
    else
    {
        logInfoP("Power Management: DISABLED");
    }
}

// ============================================================================
// Global Brightness Control Implementation (delegated to ColorManagement)
// ============================================================================

void NeoPixelBusModule::applyGlobalBrightness(uint8_t brightness)
{
    _colorManagement->applyGlobalBrightness(brightness);
}

void NeoPixelBusModule::restoreOriginalBrightness()
{
    _colorManagement->restoreOriginalBrightness();
}

// ============================================================================
// HCL Color Temperature Control Implementation (delegated to ColorManagement)
// ============================================================================

void NeoPixelBusModule::applyHclColorTemperature(uint16_t kelvin)
{
    _colorManagement->applyHclColorTemperature(kelvin);
}

void NeoPixelBusModule::disableHclMode()
{
    _colorManagement->disableHclMode();
}

// ============================================================================
// Color Correction Implementation (delegated to ColorManagement)
// ============================================================================

void NeoPixelBusModule::configureColorCorrection()
{
    _colorManagement->configureColorCorrection();
}

void NeoPixelBusModule::updateColorCorrection()
{
    _colorManagement->updateColorCorrection();
}

void NeoPixelBusModule::forceColorCorrectionUpdate()
{
    _colorManagement->forceColorCorrectionUpdate();
}

// ============================================================================
// Strip Options Implementation
// ============================================================================

// Configure strip options from ETS parameters
void NeoPixelBusModule::configureStripOptions()
{
    // Swap mode configuration
    _swapMode = mapSwapMode(ParamNEOSTRIP_NEOSwap);
    logDebugP("Swap mode configured: %d", _swapMode);

    // Skip first LEDs - now handled automatically in VirtualStrip::syncToPhysical()
    _skipFirstLeds = ParamNEOSTRIP_NEOSkipFirstLEDs;
    if (_skipFirstLeds > 0)
    {
        logInfoP("Skip first LEDs configured: %d LEDs (handled by OFM-NeoPixel)", _skipFirstLeds);
    }
}

// Map ETS swap parameter to swap mode
uint8_t NeoPixelBusModule::mapSwapMode(uint8_t paramValue)
{
    // 3-bit parameter: 0-7 possible values
    // 0 = No swap
    // 1 = R<->G swap
    // 2 = R<->B swap
    // 3 = G<->B swap
    // 4 = R->B->G->R rotate
    // 5 = R->G->B->R rotate
    // 6-7 = Reserved for future use
    return paramValue & 0x07; // Mask to ensure 3-bit value
}

// Helper for channel swapping
void NeoPixelBusModule::swapChannels(uint8_t& ch1, uint8_t& ch2)
{
    uint8_t temp = ch1;
    ch1 = ch2;
    ch2 = temp;
}

// ============================================================================
// Segment Configuration Implementation
// ============================================================================

// Configure segments from ETS parameters
void NeoPixelBusModule::configureSegments()
{
    // Read number of segments from ETS
    _numberOfSegments = ParamNEO_NEONumberOfSegments;

    logInfoP("configureSegments: Reading _numberOfSegments = %d from ETS", _numberOfSegments);

    if (_numberOfSegments == 0)
    {
        logDebugP("No segments configured");
        return;
    }

    // Clear existing segments
    _segments.clear();
    _segments.reserve(_numberOfSegments);

    // Configure each segment from ETS parameters
    for (uint8_t i = 0; i < _numberOfSegments; i++)
    {
        SegmentConfig config = createSegmentConfig(i);
        _segments.push_back(config);

        logInfoP("Segment %d: Start=%d, End=%d, Offset=%d, Group=%d, Spacing=%d, Reverse=%s, Mirror=%s",
                 i, config.startLed, config.endLed, config.offset, config.grouping, config.spacing,
                 config.reverseDirection ? "YES" : "NO", config.mirrorEffect ? "YES" : "NO");
    }

    logInfoP("Configured %d unique segments (each with individual ETS parameters), _segments.size() = %d",
             _numberOfSegments, (int)_segments.size());
}

// Create segment configuration from ETS parameters
NeoPixelBusModule::SegmentConfig NeoPixelBusModule::createSegmentConfig(uint8_t segmentIndex)
{
    SegmentConfig config;

    // Store current channel index and set to segment index for parameter access
    // This is crucial: NEO_ParamCalcIndex uses _channelIndex to calculate per-segment parameters
    // Without this, all segments would read identical parameter values from index 0
    uint8_t oldChannelIndex = _channelIndex;
    _channelIndex = segmentIndex;

    // Read segment parameters from ETS (these are now truly per-segment parameters)
    uint16_t etsStartLed = ParamNEO_NEOSegmentStart;
    uint16_t etsEndLed = ParamNEO_NEOSegmentEnd;

    // Convert from 1-based (ETS user-friendly) to 0-based (internal indexing)
    // ETS shows "Start LED = 1" but internally we need index 0
    config.startLed = (etsStartLed > 0) ? (etsStartLed - 1) : 0;
    config.endLed = (etsEndLed > 0) ? (etsEndLed - 1) : 0;
    config.offset = ParamNEO_NEOSegmentOffset;
    config.grouping = ParamNEO_NEOSegmentGrouping;
    config.spacing = ParamNEO_NEOSegmentSpacing;
    config.reverseDirection = ParamNEO_NEOSegmentReverseDirection;
    config.mirrorEffect = ParamNEO_NEOSegmentMirrorEffect;

    logDebugP("Segment %d: Read ETS params - Start=%d, End=%d, Offset=%d, Group=%d, Spacing=%d (ETS Start=%d, End=%d)",
              segmentIndex, config.startLed, config.endLed, config.offset, config.grouping, config.spacing,
              etsStartLed, etsEndLed);

    // Validate segment bounds
    if (config.startLed > config.endLed)
    {
        logErrorP("Segment %d: Invalid range - Start(%d) > End(%d)",
                  segmentIndex, config.startLed, config.endLed);
        // Swap them
        uint16_t temp = config.startLed;
        config.startLed = config.endLed;
        config.endLed = temp;
    }

    // Ensure segment fits within total LED count
    if (config.startLed >= _totalLeds)
    {
        logWarningP("Segment %d: Start LED(%d) exceeds total LEDs(%d), clamping to 0",
                    segmentIndex, config.startLed, _totalLeds);
        config.startLed = 0;
    }

    if (config.endLed >= _totalLeds)
    {
        logWarningP("Segment %d: End LED(%d) exceeds total LEDs(%d), adjusting",
                    segmentIndex, config.endLed, _totalLeds);
        config.endLed = _totalLeds - 1;
    }

    // Restore original channel index
    _channelIndex = oldChannelIndex;

    return config;
}

// Create segments on virtual strip
void NeoPixelBusModule::createSegments()
{
    if (!_virtualStrip || _segments.empty())
    {
        return;
    }

    auto mgr = _neoPixel.getManager();
    if (!mgr)
    {
        logErrorP("NeoPixelManager not available for segment creation");
        return;
    }

    // Create actual segments
    for (size_t i = 0; i < _segments.size(); i++)
    {
        SegmentConfig& config = _segments[i];

        // Calculate effective start/end considering offset
        uint16_t effectiveStart = config.startLed + config.offset;
        uint16_t effectiveEnd = config.endLed + config.offset;

        // Ensure bounds are within virtual strip
        if (effectiveEnd >= _virtualStrip->getLedCount())
        {
            effectiveEnd = _virtualStrip->getLedCount() - 1;
            logWarningP("Segment %zu: Adjusted end to %d (within virtual strip bounds)", i, effectiveEnd);
        }

        if (effectiveStart <= effectiveEnd)
        {
            config.segment = mgr->addSegment(_virtualStrip, effectiveStart, effectiveEnd);

            if (config.segment)
            {
                logInfoP("Created segment %zu: LEDs %d-%d (length %d)",
                         i, effectiveStart, effectiveEnd, effectiveEnd - effectiveStart + 1);
            }
            else
            {
                logErrorP("Failed to create segment %zu", i);
            }
        }
        else
        {
            logErrorP("Segment %zu: Invalid effective range %d-%d", i, effectiveStart, effectiveEnd);
        }
    }
}

// Apply segment-specific configuration
void NeoPixelBusModule::applySegmentConfiguration()
{
    for (size_t i = 0; i < _segments.size(); i++)
    {
        SegmentConfig& config = _segments[i];
        if (!config.segment) continue;

        // Apply grouping and spacing via Segment API
        if (config.grouping > 1 || config.spacing > 0)
        {
            config.segment->setGrouping(config.grouping);
            config.segment->setSpacing(config.spacing);
            logInfoP("Segment %zu: Applied grouping=%d, spacing=%d (virtual length=%d)",
                     i, config.grouping, config.spacing, config.segment->getLength());
        }

        // Apply offset (shift effect start position)
        if (config.offset > 0)
        {
            config.segment->setOffset(config.offset);
            logInfoP("Segment %zu: Offset=%d LEDs", i, config.offset);
        }

        // Apply reverse direction
        if (config.reverseDirection)
        {
            config.segment->setReverse(true);
            logInfoP("Segment %zu: Reverse direction enabled", i);
        }

        // Apply mirror effect
        if (config.mirrorEffect)
        {
            config.segment->setMirror(true);
            logInfoP("Segment %zu: Mirror effect enabled", i);
        }

        logDebugP("Applied configuration to segment %zu", i);
    }
}

// Get segment by index
Segment* NeoPixelBusModule::getSegment(uint8_t index) const
{
    if (index < _segments.size())
    {
        return _segments[index].segment;
    }
    return nullptr;
}

// =============================================================================
// Debug: Configuration Analysis (OPENKNX_DEBUG only)
// =============================================================================

#ifdef OPENKNX_DEBUG
void NeoPixelBusModule::debugShowConfiguration()
{
    logInfoP("════════════════════════════════════════════════════════════════════════");
    logInfoP("  NeoPixel ETS Configuration Analysis");
    logInfoP("════════════════════════════════════════════════════════════════════════");
    logInfoP("");
    
    // ========================================================================
    // Hardware Configuration
    // ========================================================================
    logInfoP("▶ Hardware Configuration:");
    logInfoP("────────────────────────────────────────────────────────────────────────");
    
    #ifdef DEVICE_HW_ID
        uint8_t hwIndex = getCurrentHardwareIndex();
        const char* hwName = HardwareMapping::getHardwareName(hwIndex);
        logInfoP("  Mode:             Compile-time (DEVICE_HW_ID defined)");
        logInfoP("  Device HW ID:     0x%04X (%d decimal)", DEVICE_HW_ID, DEVICE_HW_ID);
        logInfoP("  Hardware Index:   %d", hwIndex);
        logInfoP("  Hardware Name:    %s", hwName ? hwName : "Unknown");
    #else
        #ifdef ParamNEO_NEO_NeoPixelHardwareSelect
            uint16_t selectedHwId = (uint16_t)ParamNEO_NEO_NeoPixelHardwareSelect;
            uint8_t hwIndex = getCurrentHardwareIndex();
            const char* hwName = HardwareMapping::getHardwareName(hwIndex);
            logInfoP("  Mode:             ETS Runtime Selection");
            logInfoP("  Selected HW ID:   0x%04X (%d decimal)", selectedHwId, selectedHwId);
            logInfoP("  Hardware Index:   %d", hwIndex);
            logInfoP("  Hardware Name:    %s", hwName ? hwName : "Unknown");
        #else
            logInfoP("  Mode:             Not configured (no hardware selection)");
        #endif
    #endif
    logInfoP("");
    
    // ========================================================================
    // Strip Configuration
    // ========================================================================
    logInfoP("▶ Strip Configuration:");
    logInfoP("────────────────────────────────────────────────────────────────────────");
    logInfoP("  Physical Strips:  %d", _physicalStrips.size());
    logInfoP("  Total LEDs:       %d", _totalLeds);
    logInfoP("  Skip First LEDs:  %d (Global)", _skipFirstLeds);
    logInfoP("  Swap Mode:        %d (Global)", _swapMode);
    logInfoP("");
    
    // Show each physical strip with ETS and actual values
    for (size_t i = 0; i < _physicalStrips.size(); i++)
    {
        PhysicalStrip* strip = _physicalStrips[i];
        if (!strip) continue;
        
        // Set _channelIndex to read ETS parameters for this strip
        uint8_t _channelIndex = i;
        
        logInfoP("  ┌─ Strip %d:", i + 1);
        logInfoP("  │");
        
        // === ETS Configuration (Raw Parameters) ===
        logInfoP("  │  📋 ETS Configuration:");
        logInfoP("  │    LED Count:        %d", (uint16_t)ParamNEOSTRIP_NEOLength);
        
        uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const char* ledTypeNames[] = {
            "WS2811/WS2812/WS2812B (GRB)", 
            "WS2813 (GRB)", 
            "WS2815 (GRB)",
            "SK6812 (GRB)", 
            "TM1804 (RGB)", 
            "TM1829 (RGB/WRGB)",
            "APA106 (RGB)", 
            "UCS1904 (RGB)",
            "SK6812/WS2814 (RGBW)",
            "LPD6803 (RGB SPI)",
            "LPD8806 (GRB SPI)",
            "WS2801 (RGB SPI)",
            "P9813 (RGB SPI)",
            "APA102 (RGB SPI)",
            "SK9822 (RGB SPI)"
        };
        const char* ledTypeName = (ledTypeParam < 15) ? ledTypeNames[ledTypeParam] : "Unknown";
        logInfoP("  │    LED Type:         %d (%s)", ledTypeParam, ledTypeName);
        
        uint8_t colorOrderParam = (uint8_t)ParamNEOSTRIP_NEOColourOrder;
        const char* colorOrderNames[] = {"RGB", "RBG", "GRB", "GBR", "BRG", "BGR", "WRGB", "WRBG", "WGRB", "WGBR", "WBRG", "WBGR", "RGBW", "RBGW", "GRBW", "GBRW", "BRGW", "BGRW", "RWGB", "RWBG", "GWRB", "GWBR", "BWRG", "BWGR", "RGBW", "RBWG", "GRBW", "GBWR", "BRWG", "BGWR"};
        const char* colorOrderName = (colorOrderParam < 30) ? colorOrderNames[colorOrderParam] : "Unknown";
        logInfoP("  │    Color Order:      %d (%s)", colorOrderParam, colorOrderName);
        
        // GPIO Configuration Mode
        bool gpioManual = (bool)ParamNEOSTRIP_NEOGPIOManual;
        logInfoP("  │    GPIO Mode:        %s", gpioManual ? "Manual" : "Hardware-assisted");
        
        if (gpioManual)
        {
            // Manual mode: show configured pins
            if (isSpiProtocol(strip->getProtocol()))
            {
                logInfoP("  │      MOSI GPIO:      %d", (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO);
                logInfoP("  │      SCK GPIO:       %d", (uint8_t)ParamNEOSTRIP_NEOClockGPIO);
                
                // SPI Frequency
                bool spiClkManual = (ParamNEOSTRIP_NEOSPICLKMode == 1);
                if (spiClkManual)
                {
                    uint8_t spiClkValue = (uint8_t)ParamNEOSTRIP_NEOSPICLK;
                    const char* spiFreqNames[] = {"1 MHz", "2 MHz", "4 MHz", "8 MHz", "12 MHz", "16 MHz", "20 MHz"};
                    const char* spiFreqName = (spiClkValue < 7) ? spiFreqNames[spiClkValue] : "Unknown";
                    logInfoP("  │      SPI Frequency:  %d (%s)", spiClkValue, spiFreqName);
                }
                else
                {
                    logInfoP("  │      SPI Frequency:  Auto (10 MHz)");
                }
            }
            else
            {
                logInfoP("  │      Data GPIO:      %d", (uint8_t)ParamNEOSTRIP_NEODataGPIO);
            }
        }
        else
        {
            // Hardware-assisted mode: show port selection
            uint8_t hwIndex = getCurrentHardwareIndex();
            uint8_t dataPortIndex = getGpioDataPortForHw(i, hwIndex);
            
            if (isHardwarePortSelected(dataPortIndex))
            {
                logInfoP("  │      Data Port:      %d (ETS selection)", dataPortIndex);
            }
            else
            {
                logInfoP("  │      Data Port:      Auto (not selected in ETS)");
            }
            
            if (isSpiProtocol(strip->getProtocol()))
            {
                uint8_t clockPortIndex = getGpioClockPortForHw(i, hwIndex);
                if (isHardwarePortSelected(clockPortIndex))
                {
                    logInfoP("  │      Clock Port:     %d (ETS selection)", clockPortIndex);
                }
                else
                {
                    logInfoP("  │      Clock Port:     Auto (not selected in ETS)");
                }
            }
        }
        
        // Timing Mode
        uint8_t timingMode = (uint8_t)ParamNEOSTRIP_NEOTiming;
        const char* timingModes[] = {
            "AUTO", "AUTO_LEGACY",
            "SLOW_20%", "SLOW_15%", "SLOW_10%", "SLOW_5%",
            "FAST_5%", "FAST_10%", "FAST_15%", "FAST_20%", "FAST_25%"
        };
        const char* timingName = (timingMode < 11) ? timingModes[timingMode] : "UNKNOWN";
        logInfoP("  │    Timing Mode:      %d (%s)", timingMode, timingName);
        
        // Skip First LEDs
        uint16_t skipLeds = (uint16_t)ParamNEOSTRIP_NEOSkipFirstLEDs;
        logInfoP("  │    Skip First LEDs:  %d", skipLeds);
        
        // Color Correction
        bool gammaEnabled = (bool)ParamNEOSTRIP_NEOGammaCorrection;
        bool whiteBalEnabled = (bool)ParamNEOSTRIP_NEOWhiteBalanceCorrection;
        logInfoP("  │    Gamma Corr:       %s", gammaEnabled ? "Yes" : "No");
        logInfoP("  │    White Balance:    %s", whiteBalEnabled ? "Yes" : "No");
        
        logInfoP("  │");
        
        // === Actual Hardware (What's running) ===
        logInfoP("  │  ⚙️  Actual Hardware:");
        logInfoP("  │    LEDs:             %d", strip->getLedCount());
        logInfoP("  │    Protocol:         %s", getProtocolName(strip->getProtocol()));
        logInfoP("  │    Color Order:      %s", getColorOrderName(strip->getColorOrder()));
        logInfoP("  │    Data GPIO:        %d", strip->getDataPin());
        if (isSpiProtocol(strip->getProtocol()))
        {
            logInfoP("  │    Clock GPIO:       %d", strip->getClockPin());
        }
        
        logInfoP("  └──");
    }
    logInfoP("");
    
    // ========================================================================
    // Segment Configuration
    // ========================================================================
    logInfoP("▶ Segment Configuration:");
    logInfoP("────────────────────────────────────────────────────────────────────────");
    logInfoP("  Number of Segments: %d", _numberOfSegments);
    logInfoP("");
    
    for (size_t i = 0; i < _segments.size(); i++)
    {
        const auto& seg = _segments[i];
        logInfoP("  ┌─ Segment %d:", i + 1);
        logInfoP("  │  LED Range:     %d - %d (%d LEDs)", 
                                          seg.startLed, seg.endLed, seg.endLed - seg.startLed + 1);
        logInfoP("  │  Offset:        %d", seg.offset);
        logInfoP("  │  Grouping:      %d", seg.grouping);
        logInfoP("  │  Spacing:       %d", seg.spacing);
        logInfoP("  │  Reverse:       %s", seg.reverseDirection ? "Yes" : "No");
        logInfoP("  │  Mirror:        %s", seg.mirrorEffect ? "Yes" : "No");
        logInfoP("  │  Effect Type:   %d", seg.savedEffectType);
        
        if (seg.savedValid)
        {
            logInfoP("  │  Saved State:   R=%d G=%d B=%d WW=%d CW=%d Br=%d", 
                                              seg.savedR, seg.savedG, seg.savedB, seg.savedWW, seg.savedCW, seg.savedBrightness);
        }
        
        logInfoP("  └──");
    }
    logInfoP("");
    
    // ========================================================================
    // Color Correction
    // ========================================================================
    logInfoP("▶ Color Correction:");
    logInfoP("────────────────────────────────────────────────────────────────────────");
    logInfoP("  Gamma Correction:   %s", _gammaCorrectionEnabled ? "Enabled" : "Disabled");
    if (_gammaCorrectionEnabled)
    {
        logInfoP("  Gamma Value:        %.2f", _gammaValue);
    }
    logInfoP("  White Balance:      %s", _whiteBalanceEnabled ? "Enabled" : "Disabled");
    if (_whiteBalanceEnabled)
    {
        logInfoP("  WB Red:             %d", _whiteBalanceRed);
        logInfoP("  WB Green:           %d", _whiteBalanceGreen);
        logInfoP("  WB Blue:            %d", _whiteBalanceBlue);
    }
    logInfoP("  Global Brightness:  %d", _globalBrightness);
    logInfoP("  HCL Mode:           %s", _hclModeEnabled ? "Active" : "Inactive");
    if (_hclModeEnabled)
    {
        logInfoP("  HCL Temperature:    %d K", _hclAppliedKelvin);
    }
    logInfoP("");
    
    // ========================================================================
    // Effects Configuration
    // ========================================================================
    logInfoP("▶ Effects System:");
    logInfoP("────────────────────────────────────────────────────────────────────────");
    logInfoP("  Effects Enabled:    %s", _effectsEnabled ? "Yes" : "No");
    logInfoP("");
    
    logInfoP("════════════════════════════════════════════════════════════════════════");
}
#endif

// =============================================================================
// Effects Implementation
// =============================================================================

void NeoPixelBusModule::configureEffects()
{
    // Delegate to EffectConfiguration sub-module
    _effectConfiguration->configureEffects();
}

void NeoPixelBusModule::applyEffectToSegment(Segment* segment, uint8_t effectType)
{
    // Delegate to EffectConfiguration sub-module
    _effectConfiguration->applyEffectToSegment(segment, effectType);
}

void NeoPixelBusModule::setupEffectConfiguration(Segment* segment)
{
    // Delegate to EffectConfiguration sub-module
    _effectConfiguration->setupEffectConfiguration(segment);
}