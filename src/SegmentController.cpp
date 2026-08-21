#include "SegmentController.h"
#include "NeoPixelModule.h"
#include "OpenKNX.h"
#include "colorhelper.h"
#include "knxprod.h"

// Constructor
SegmentController::SegmentController(NeoPixelBusModule* module)
    : _module(module)
{
}

// Destructor
SegmentController::~SegmentController()
{
}

// dpt3_007_delta() removed: the start-stop dim ramp is now time-based (see
// processActiveDimming). The old exponential mapping also returned 0 for
// stepCode 7 (integer /4 of 3) -> that channel never dimmed.

// Process active start/stop dimming for all segments
void SegmentController::processActiveDimming()
{
    uint32_t now = millis();
    const uint32_t DIMMING_TIMEOUT = 2000; // Stop if no telegram for 2 seconds

    for (auto& segConfig : _module->_segments)
    {
        if (segConfig.activeDimming == NeoPixelBusModule::SegmentConfig::NONE) continue;

        Segment* seg = segConfig.segment;
        if (!seg) continue;

        // Check for timeout
        if (now - segConfig.dimmingLastUpdate > DIMMING_TIMEOUT)
        {
            segConfig.activeDimming = NeoPixelBusModule::SegmentConfig::NONE;
            segConfig.dimmingStepCode = 0;
            continue;
        }

        // Check if it's time for next step
        if (now < segConfig.dimmingNextStep) continue;

        // Time-based start-stop ramp: a full 0..255 dim takes ~DIM_RAMP_FULL_MS,
        // independent of the sender's step code. MDT & most pushbuttons send
        // stepCode 1 (100%) start-stop and expect the DIMMER to define the speed.
        // The old code applied the step percentage per 40ms tick, so stepCode 1
        // ramped in ~0.2s -> looked like an instant off.
        // TODO(B): expose DIM_RAMP_FULL_MS as an ETS "Dimmzeit" parameter per segment.
        static constexpr int32_t DIM_RAMP_FULL_MS = 3000; // 0<->255 in ~3s
        const uint32_t interval = 40; // fixed tick for a smooth ramp
        segConfig.dimmingNextStep = now + interval;

        int16_t delta = (int16_t)(((int32_t)255 * (int32_t)interval) / DIM_RAMP_FULL_MS);
        if (delta < 1) delta = 1; // guarantee visible progress
        if (!segConfig.dimmingIncrease) delta = -delta;

        // Apply dimming based on active channel
        uint8_t r, g, b;
        switch (segConfig.activeDimming)
        {
            case NeoPixelBusModule::SegmentConfig::BRIGHTNESS:
            {
                // Ratchet the MASTER (intent) brightness, mirroring the absolute SegmentBrightness KO
                // (NeoPixelModule.cpp:1812-1830): keep savedBrightness (the pre-global baseline) in sync
                // and re-derive the render brightness from master. Writing only the render layer (old
                // behavior) lost the dim on the next cue switch or global-brightness recompute, which
                // broke the master cascade (P1) and the "live dim survives EM-stop" rule (conflict#1=b).
                int16_t newDesired = constrain((int16_t)segConfig.savedBrightness + delta, 0, 255);
                segConfig.savedBrightness = (uint8_t)newDesired;

                uint8_t globalB = _module->getGlobalBrightness();
                uint8_t effective = (globalB < 255) ? (uint8_t)((newDesired * globalB) / 255) : (uint8_t)newDesired;

                seg->setMasterBrightness(effective);
                if (segConfig.emController.isRunning())
                    // EM owns the render brightness — re-derive it from the new master immediately.
                    segConfig.emController.reapplyMasterBrightness(seg);
                else
                    seg->setBrightness(effective);
                break;
            }

            case NeoPixelBusModule::SegmentConfig::RED:
                applyColorChannelDimming(seg, 0, delta); // 0 = Red channel
                break;

            case NeoPixelBusModule::SegmentConfig::GREEN:
                applyColorChannelDimming(seg, 1, delta); // 1 = Green channel
                break;

            case NeoPixelBusModule::SegmentConfig::BLUE:
                applyColorChannelDimming(seg, 2, delta); // 2 = Blue channel
                break;

            case NeoPixelBusModule::SegmentConfig::WHITE:
            case NeoPixelBusModule::SegmentConfig::WARM_WHITE:
            case NeoPixelBusModule::SegmentConfig::COOL_WHITE:
            {
                uint8_t bytesPerLed = seg->getVirtualStrip()->getBytesPerLed();
                if (bytesPerLed == 4)
                {
                    // 4-channel RGBW: all white channels dim the single W
                    uint8_t w;
                    if (seg->getPixel(0, r, g, b, w))
                    {
                        int16_t newW = w + delta;
                        newW = constrain(newW, 0, 255);
                        seg->setPrimaryColor(r, g, b, (uint8_t)newW);
                    }
                }
                else if (bytesPerLed == 5)
                {
                    // 5-channel RGBCCT: separate WW and CW
                    uint8_t ww, cw;
                    if (seg->getPixel(0, r, g, b, ww, cw))
                    {
                        if (segConfig.activeDimming == NeoPixelBusModule::SegmentConfig::COOL_WHITE)
                        {
                            int16_t newCW = cw + delta;
                            newCW = constrain(newCW, 0, 255);
                            seg->setPrimaryColor(r, g, b, ww, (uint8_t)newCW);
                        }
                        else
                        {
                            // WHITE and WARM_WHITE both dim the WW channel
                            int16_t newWW = ww + delta;
                            newWW = constrain(newWW, 0, 255);
                            seg->setPrimaryColor(r, g, b, (uint8_t)newWW, cw);
                        }
                    }
                }
                break;
            }

            case NeoPixelBusModule::SegmentConfig::HUE:
            case NeoPixelBusModule::SegmentConfig::SATURATION:
            case NeoPixelBusModule::SegmentConfig::VALUE:
            {
                if (seg->getPixel(0, r, g, b))
                {
                    uint8_t h, s, v;
                    ColorHelper::rgbToHSV(r, g, b, h, s, v);

                    if (segConfig.activeDimming == NeoPixelBusModule::SegmentConfig::HUE)
                    {
                        h = (uint8_t)(h + delta); // Wraps automatically
                    }
                    else if (segConfig.activeDimming == NeoPixelBusModule::SegmentConfig::SATURATION)
                    {
                        int16_t newS = s + delta;
                        s = (uint8_t)constrain(newS, 0, 255);
                    }
                    else
                    { // VALUE
                        int16_t newV = v + delta;
                        v = (uint8_t)constrain(newV, 0, 255);
                    }

                    uint8_t newR, newG, newB;
                    ColorHelper::hsvToRGB(h, s, v, newR, newG, newB);

                    // Preserve white channel(s) when changing RGB
                    uint8_t bytesPerLed = seg->getVirtualStrip()->getBytesPerLed();
                    if (bytesPerLed == 5)
                    {
                        uint8_t ww, cw;
                        seg->getPixel(0, r, g, b, ww, cw);
                        seg->setPrimaryColor(newR, newG, newB, ww, cw);
                    }
                    else if (bytesPerLed == 4)
                    {
                        uint8_t w;
                        seg->getPixel(0, r, g, b, w);
                        seg->setPrimaryColor(newR, newG, newB, w);
                    }
                    else
                    {
                        seg->setPrimaryColor(newR, newG, newB, 0);
                    }
                }
                break;
            }

            default:
                break;
        }
    }
}

// Segment KO processor wrapper
bool SegmentController::processSegmentKo(GroupObject& ko, uint16_t koNumber, uint8_t channel)
{
    // Set _channelIndex for NEO_KoCalcIndex macro
    uint8_t _channelIndex = channel;

    // Calculate KO index for this channel
    int koIndex = NEO_KoCalcIndex(koNumber);

    // Dispatch to appropriate handler
    switch (koIndex)
    {
        case NEO_KoR:
            processRedKo(channel, ko);
            return true;

        case NEO_KoG:
            processGreenKo(channel, ko);
            return true;

        case NEO_KoB:
            processBlueKo(channel, ko);
            return true;

        case NEO_KoW:
            processWhiteKo(channel, ko);
            return true;

        case NEO_KoCCT:
            processCctKo(channel, ko);
            return true;

        case NEO_KoH:
            processHueKo(channel, ko);
            return true;

        case NEO_KoS:
            processSaturationKo(channel, ko);
            return true;

        case NEO_KoV:
            processValueKo(channel, ko);
            return true;

        case NEO_KoHSV:
            processHsvKo(channel, ko);
            return true;

        default:
            return false; // KO not handled by segment controller
    }
}

// Individual KO handlers (implementations moved from NeoPixelModule.cpp)
void SegmentController::processRedKo(uint8_t channel, GroupObject& ko)
{
    uint8_t red = ko.value(DPT_Value_1_Ucount);
    processColorChannelKo(channel, ko, red, 0, "Red");
}

void SegmentController::processGreenKo(uint8_t channel, GroupObject& ko)
{
    uint8_t green = ko.value(DPT_Value_1_Ucount);
    processColorChannelKo(channel, ko, green, 1, "Green");
}

void SegmentController::processBlueKo(uint8_t channel, GroupObject& ko)
{
    uint8_t blue = ko.value(DPT_Value_1_Ucount);
    processColorChannelKo(channel, ko, blue, 2, "Blue");
}

void SegmentController::processWhiteKo(uint8_t channel, GroupObject& ko)
{
    uint8_t white = ko.value(DPT_Value_1_Ucount);
    processColorChannelKo(channel, ko, white, 3, "White");
}

void SegmentController::processCctKo(uint8_t channel, GroupObject& ko)
{
    uint16_t cct = ko.value(DPT_Value_Temp);
    logInfoP("Segment %d CCT: %dK", channel, cct);

    auto& cfg = _module->_segments[channel];
    Segment* targetSegment = cfg.segment;

    // Convert Kelvin to RGB
    uint8_t r, g, b;
    ColorHelper::kelvinToRGB(cct, r, g, b);
    targetSegment->setPrimaryColor(r, g, b, 0, 0);

    // Save color for effect restore
    cfg.savedR = r;
    cfg.savedG = g;
    cfg.savedB = b;
    cfg.savedWW = 0;
    cfg.savedCW = 0;
    cfg.savedBrightness = targetSegment->getBrightness();
    cfg.savedValid = true;
    cfg.savedLastWasEffect = false;

    // Send status feedback
    uint8_t _channelIndex = _module->getChannelIndex();
    _module->_channelIndex = channel;
    bool changed = KoNEO_CCTState.valueNoSendCompare(cct, DPT_Value_Temp);
    if (changed) KoNEO_CCTState.objectWritten();
    _module->_channelIndex = _channelIndex;
}

void SegmentController::processHueKo(uint8_t channel, GroupObject& ko)
{
    uint8_t hue = ko.value(DPT_Value_1_Ucount);
    processHsvChannelKo(channel, ko, hue, 0, "Hue");
}

void SegmentController::processSaturationKo(uint8_t channel, GroupObject& ko)
{
    uint8_t saturation = ko.value(DPT_Value_1_Ucount);
    processHsvChannelKo(channel, ko, saturation, 1, "Saturation");
}

void SegmentController::processValueKo(uint8_t channel, GroupObject& ko)
{
    uint8_t value = ko.value(DPT_Value_1_Ucount);
    processHsvChannelKo(channel, ko, value, 2, "Value");
}

void SegmentController::processHsvKo(uint8_t channel, GroupObject& ko)
{
    // Combined HSV KO - 3-byte value (H, S, V)
    uint32_t hsvValue = ko.value(DPT_Colour_RGB); // Using RGB DPT for 3-byte HSV data
    uint8_t h = (hsvValue >> 16) & 0xFF;
    uint8_t s = (hsvValue >> 8) & 0xFF;
    uint8_t v = hsvValue & 0xFF;

    logInfoP("Segment %d: HSV = H:%d S:%d V:%d", channel, h, s, v);

    auto& cfg = _module->_segments[channel];
    Segment* targetSegment = cfg.segment;

    // Update HSV state
    cfg.currentH = h;
    cfg.currentS = s;
    cfg.currentV = v;

    // Convert HSV to RGB
    uint8_t r, g, b;
    ColorHelper::hsvToRGB(h, s, v, r, g, b);

    // Update pending and saved RGB
    cfg.pendingSolidR = r;
    cfg.pendingSolidG = g;
    cfg.pendingSolidB = b;
    cfg.savedR = r;
    cfg.savedG = g;
    cfg.savedB = b;
    cfg.savedBrightness = targetSegment->getBrightness();
    cfg.savedValid = true;

    // If Solid effect is running, update immediately
    if (targetSegment->getEffect() == EffectPool::getSolid())
    {
        targetSegment->setPrimaryColor(r, g, b, cfg.savedWW, cfg.savedCW);
        logInfoP("Segment %d: Updated Solid effect with HSV->RGB (%d,%d,%d)", channel, r, g, b);
    }
    else
    {
        logInfoP("Segment %d: Stored pending HSV->RGB (%d,%d,%d)", channel, r, g, b);
    }

    // Send status feedback
    sendColorStatusFeedback(channel);
}

void SegmentController::startStopDimming(uint8_t channel, uint8_t dimmingChannel, uint8_t rel)
{
    auto& cfg = _module->_segments[channel];

    bool increase = (rel & 0x08) != 0; // Bit 3 = direction
    uint8_t stepCode = rel & 0x07;     // Bits 0-2 = step code

    if (stepCode == 0)
    {
        // Stop dimming
        cfg.activeDimming = NeoPixelBusModule::SegmentConfig::NONE;
        cfg.dimmingStepCode = 0;
        logInfoP("Segment %d: Stop dimming", channel);
    }
    else
    {
        // Start dimming
        cfg.activeDimming = static_cast<NeoPixelBusModule::SegmentConfig::DimmingChannel>(dimmingChannel);
        cfg.dimmingIncrease = increase;
        cfg.dimmingStepCode = stepCode;
        cfg.dimmingLastUpdate = millis();
        cfg.dimmingNextStep = millis();
        logInfoP("Segment %d: Start dimming channel=%d, increase=%d, stepCode=%d",
                 channel, dimmingChannel, increase, stepCode);
    }
}

// ========== Helper Functions for Code Deduplication ==========

void SegmentController::applyColorChannelDimming(Segment* seg, uint8_t colorChannel, int16_t delta)
{
    uint8_t r, g, b;
    uint8_t bytesPerLed = seg->getVirtualStrip()->getBytesPerLed();

    if (bytesPerLed == 5) // RGBCCT
    {
        uint8_t ww, cw;
        if (seg->getPixel(0, r, g, b, ww, cw))
        {
            // Update the specified color channel
            uint8_t* channels[] = {&r, &g, &b};
            int16_t newValue = *channels[colorChannel] + delta;
            *channels[colorChannel] = (uint8_t)constrain(newValue, 0, 255);
            seg->setPrimaryColor(r, g, b, ww, cw);
        }
    }
    else if (bytesPerLed == 4) // RGBW
    {
        uint8_t w;
        if (seg->getPixel(0, r, g, b, w))
        {
            // Update the specified color channel
            uint8_t* channels[] = {&r, &g, &b};
            int16_t newValue = *channels[colorChannel] + delta;
            *channels[colorChannel] = (uint8_t)constrain(newValue, 0, 255);
            seg->setPrimaryColor(r, g, b, w);
        }
    }
    else // RGB
    {
        if (seg->getPixel(0, r, g, b))
        {
            // Update the specified color channel
            uint8_t* channels[] = {&r, &g, &b};
            int16_t newValue = *channels[colorChannel] + delta;
            *channels[colorChannel] = (uint8_t)constrain(newValue, 0, 255);
            seg->setPrimaryColor(r, g, b, 0);
        }
    }
}

void SegmentController::processColorChannelKo(uint8_t channel, GroupObject& ko, uint8_t colorValue, uint8_t colorChannel, const char* channelName)
{
    logInfoP("Segment %d %s: %d", channel, channelName, colorValue);

    auto& cfg = _module->_segments[channel];
    Segment* targetSegment = cfg.segment;

    // Update saved and pending values based on channel
    if (colorChannel == 0) // Red
    {
        cfg.pendingSolidR = colorValue;
        cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
        cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
        cfg.pendingSolidWW = cfg.savedValid ? cfg.savedWW : 0;
        cfg.savedR = colorValue;
        if (!cfg.savedValid)
        {
            cfg.savedG = 0;
            cfg.savedB = 0;
            cfg.savedWW = 0;
            cfg.savedCW = 0;
        }
    }
    else if (colorChannel == 1) // Green
    {
        cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
        cfg.pendingSolidG = colorValue;
        cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
        cfg.pendingSolidWW = cfg.savedValid ? cfg.savedWW : 0;
        cfg.savedG = colorValue;
        if (!cfg.savedValid)
        {
            cfg.savedR = 0;
            cfg.savedB = 0;
            cfg.savedWW = 0;
            cfg.savedCW = 0;
        }
    }
    else if (colorChannel == 2) // Blue
    {
        cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
        cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
        cfg.pendingSolidB = colorValue;
        cfg.pendingSolidWW = cfg.savedValid ? cfg.savedWW : 0;
        cfg.savedB = colorValue;
        if (!cfg.savedValid)
        {
            cfg.savedR = 0;
            cfg.savedG = 0;
            cfg.savedWW = 0;
            cfg.savedCW = 0;
        }
    }
    else if (colorChannel == 3) // White
    {
        cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
        cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
        cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
        cfg.pendingSolidWW = colorValue;
        cfg.savedWW = colorValue;
        if (!cfg.savedValid)
        {
            cfg.savedR = 0;
            cfg.savedG = 0;
            cfg.savedB = 0;
        }
    }

    cfg.savedBrightness = targetSegment->getBrightness();
    cfg.savedValid = true;

    // If Solid effect is running, update immediately
    if (targetSegment->getEffect() == EffectPool::getSolid())
    {
        if (colorChannel == 3) // White channel for RGBW
            targetSegment->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, colorValue, cfg.savedCW);
        else // RGB channels
            targetSegment->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedCW);
        logInfoP("Segment %d: Updated Solid effect color (%s=%d)", channel, channelName, colorValue);
    }
    else
    {
        logInfoP("Segment %d: Stored pending %s=%d (will apply when effect stops)", channel, channelName, colorValue);
    }

    // Send status feedback
    sendColorStatusFeedback(channel);
}

void SegmentController::processHsvChannelKo(uint8_t channel, GroupObject& ko, uint8_t hsvValue, uint8_t hsvChannel, const char* channelName)
{
    logInfoP("Segment %d %s: %d", channel, channelName, hsvValue);

    auto& cfg = _module->_segments[channel];
    Segment* targetSegment = cfg.segment;

    // Update HSV state based on channel
    if (hsvChannel == 0) // Hue
    {
        cfg.currentH = hsvValue;
    }
    else if (hsvChannel == 1) // Saturation
    {
        cfg.currentS = hsvValue;
    }
    else if (hsvChannel == 2) // Value
    {
        cfg.currentV = hsvValue;
    }

    // Convert current HSV to RGB
    uint8_t r, g, b;
    ColorHelper::hsvToRGB(cfg.currentH, cfg.currentS, cfg.currentV, r, g, b);

    // Update pending and saved RGB
    cfg.pendingSolidR = r;
    cfg.pendingSolidG = g;
    cfg.pendingSolidB = b;
    cfg.savedR = r;
    cfg.savedG = g;
    cfg.savedB = b;
    cfg.savedBrightness = targetSegment->getBrightness();
    cfg.savedValid = true;

    // If Solid effect is running, update immediately
    if (targetSegment->getEffect() == EffectPool::getSolid())
    {
        targetSegment->setPrimaryColor(r, g, b, cfg.savedWW, cfg.savedCW);
        logInfoP("Segment %d: Updated Solid effect with %s=%d -> RGB(%d,%d,%d)",
                 channel, channelName, hsvValue, r, g, b);
    }
    else
    {
        logInfoP("Segment %d: Stored pending %s=%d -> RGB(%d,%d,%d) (will apply when effect stops)",
                 channel, channelName, hsvValue, r, g, b);
    }

    // Send status feedback
    sendColorStatusFeedback(channel);
}

void SegmentController::sendColorStatusFeedback(uint8_t channel)
{
    auto& cfg = _module->_segments[channel];

    // Store original channel index and switch to target channel
    uint8_t originalChannelIndex = _module->getChannelIndex();
    _module->_channelIndex = channel;
    uint8_t _channelIndex = channel; // Local variable for KO macro expansion

    uint8_t r = cfg.pendingSolidR;
    uint8_t g = cfg.pendingSolidG;
    uint8_t b = cfg.pendingSolidB;

    // Send appropriate status feedback based on strip type
    if (_module->_virtualStrip && _module->_virtualStrip->hasWhiteChannel())
    {
        // RGBW feedback uses warm white as the single KNX white component.
        uint8_t w = cfg.pendingSolidWW;
        uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
        bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
        if (changed) KoNEO_RGBWState.objectWritten();
    }
    else
    {
        // 3-channel RGB feedback
        uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
        if (changed) KoNEO_RGBState.objectWritten();
    }

    // Restore original channel index
    _module->_channelIndex = originalChannelIndex;
}
