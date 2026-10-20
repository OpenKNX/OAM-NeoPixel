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

// The start-stop dim ramp is time-based (see processActiveDimming); the step code only
// selects "until stop" (code 1) or a finite step of 1/2^(code-1) of the range.

// Process active start/stop dimming for all segments
void SegmentController::processActiveDimming()
{
    uint32_t now = millis();
    // Time-based ramp: a full 0..255 dim takes ~DIM_RAMP_FULL_MS, independent of the sender's
    // step code. MDT & most pushbuttons send stepCode 1 (100%) start-stop and expect the DIMMER
    // to define the speed.
    // TODO: expose DIM_RAMP_FULL_MS as an ETS "Dimmzeit" parameter per segment.
    static constexpr int32_t DIM_RAMP_FULL_MS = 3000; // 0<->255 in 3 s
    // Safety stop when the stop telegram is lost, counted from the start telegram; a full ramp
    // fits with margin. Hue has no end stop and halts here.
    const uint32_t DIMMING_TIMEOUT = DIM_RAMP_FULL_MS + 2000;

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
            // No stop telegram arrived: report the dimmed result
            _module->queueStatus(static_cast<uint8_t>(1 + (&segConfig - _module->_segments.data())), false);
            continue;
        }

        // Check if it's time for next step
        if ((int32_t)(now - segConfig.dimmingNextStep) < 0) continue; // wrap-safe

        const uint32_t interval = 40; // tick for a smooth ramp
        segConfig.dimmingNextStep = now + interval;

        // Step size from the elapsed time, so slow loop passes do not stretch the ramp; the remainder
        // carries over, so the steps add up to 255 per DIM_RAMP_FULL_MS.
        const uint32_t elapsed = std::min<uint32_t>(now - segConfig.dimmingLastStepMs, DIM_RAMP_FULL_MS);
        segConfig.dimmingLastStepMs = now;
        const uint32_t ramp = (uint32_t)255 * elapsed + segConfig.dimmingStepRest;
        int16_t delta = (int16_t)std::min<uint32_t>(255, ramp / DIM_RAMP_FULL_MS);
        segConfig.dimmingStepRest = (uint16_t)(ramp % DIM_RAMP_FULL_MS);
        if (delta < 1)
        {
            delta = 1; // first tick of a ramp: respond at once
            segConfig.dimmingStepRest = 0;
        }
        bool stepDone = false;
        if (segConfig.dimmingRemaining >= 0)
        {
            // Finite step (codes 2..7): stop once the interval is covered.
            if (delta >= segConfig.dimmingRemaining)
            {
                delta = segConfig.dimmingRemaining;
                stepDone = true;
            }
            segConfig.dimmingRemaining -= delta;
        }
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
                    // Base: the configured primary colour (pixel 0 is brightness-scaled and black when off)
                    const EffectConfig& pc = seg->getConfig();
                    int16_t newW = constrain((int16_t)pc.w() + delta, 0, 255);
                    seg->setPrimaryColor(pc.r(), pc.g(), pc.b(), (uint8_t)newW);
                }
                else if (bytesPerLed == 5)
                {
                    // 5-channel RGBCCT: separate WW and CW
                    const EffectConfig& pc = seg->getConfig();
                    if (segConfig.activeDimming == NeoPixelBusModule::SegmentConfig::COOL_WHITE)
                    {
                        int16_t newCW = constrain((int16_t)pc.cw() + delta, 0, 255);
                        seg->setPrimaryColor(pc.r(), pc.g(), pc.b(), pc.ww(), (uint8_t)newCW);
                    }
                    else if (segConfig.activeDimming == NeoPixelBusModule::SegmentConfig::WARM_WHITE)
                    {
                        int16_t newWW = constrain((int16_t)pc.ww() + delta, 0, 255);
                        seg->setPrimaryColor(pc.r(), pc.g(), pc.b(), (uint8_t)newWW, pc.cw());
                    }
                    else
                    {
                        // Generic white = WW + CW (value of the W KO and the W status). The WW share and
                        // the range are taken at the ramp start (or after another command changed WW/CW),
                        // so rounding does not shift the colour temperature; from black the split matches
                        // the W KO (ww = w/2). Upward the ramp ends at 255 like the W KO; a ramp that
                        // starts above 255 runs up to the larger channel at 255.
                        const int16_t ww = pc.ww(), cw = pc.cw(), sum = ww + cw;
                        if (ww != segConfig.dimmingWhiteWW || cw != segConfig.dimmingWhiteCW)
                        {
                            segConfig.dimmingWhiteShare = sum ? (uint16_t)(((uint32_t)ww << 15) / sum) : 0x4000;
                            segConfig.dimmingWhiteScale = (sum > 255) ? 2 : 1; // 0..510 also in DIM_RAMP_FULL_MS
                        }
                        const int16_t scale = segConfig.dimmingWhiteScale;
                        // Upper end: no channel clips at this share
                        const uint32_t maxShare = std::max<uint32_t>(segConfig.dimmingWhiteShare, 0x8000u - segConfig.dimmingWhiteShare);
                        const int16_t limit = (int16_t)std::min<uint32_t>(255u * scale, (255u * 0x8000u) / maxShare);
                        const int16_t newSum = constrain((int16_t)(sum + delta * scale), 0, std::max<int16_t>(limit, sum));
                        const int16_t newWW = std::min<int16_t>(255, (int16_t)(((uint32_t)newSum * segConfig.dimmingWhiteShare + 0x3FFF) >> 15));
                        const int16_t newCW = std::min<int16_t>(255, (int16_t)(newSum - newWW));
                        segConfig.dimmingWhiteWW = newWW;
                        segConfig.dimmingWhiteCW = newCW;
                        seg->setPrimaryColor(pc.r(), pc.g(), pc.b(), (uint8_t)newWW, (uint8_t)newCW);
                    }
                }
                break;
            }

            case NeoPixelBusModule::SegmentConfig::HUE:
            case NeoPixelBusModule::SegmentConfig::SATURATION:
            case NeoPixelBusModule::SegmentConfig::VALUE:
            {
                {
                    const EffectConfig& pc = seg->getConfig();
                    r = pc.r();
                    g = pc.g();
                    b = pc.b();
                    // Base on the stored H/S/V while it still describes the primary colour: deriving
                    // HSV from RGB loses hue and saturation at V = 0 and stalls the hue at low V.
                    uint8_t h = segConfig.currentH, s = segConfig.currentS, v = segConfig.currentV;
                    uint8_t cr, cg, cb;
                    ColorHelper::hsvToRGB(h, s, v, cr, cg, cb);
                    if (!segConfig.hsvTracked || cr != r || cg != g || cb != b) ColorHelper::rgbToHSV(r, g, b, h, s, v);

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
                    segConfig.currentH = h;
                    segConfig.currentS = s;
                    segConfig.currentV = v;
                    segConfig.hsvTracked = true;

                    // Preserve white channel(s) when changing RGB
                    uint8_t bytesPerLed = seg->getVirtualStrip()->getBytesPerLed();
                    if (bytesPerLed == 5)
                        seg->setPrimaryColor(newR, newG, newB, pc.ww(), pc.cw());
                    else if (bytesPerLed == 4)
                        seg->setPrimaryColor(newR, newG, newB, pc.w());
                    else
                        seg->setPrimaryColor(newR, newG, newB, pc.w());
                }
                break;
            }

            default:
                break;
        }
        // Colour steps: saved* follows the live colour (base for Fx and the absolute colour KOs)
        if (segConfig.activeDimming != NeoPixelBusModule::SegmentConfig::BRIGHTNESS)
            _module->captureDirectColor((size_t)(&segConfig - _module->_segments.data()));

        if (stepDone)
        {
            segConfig.activeDimming = NeoPixelBusModule::SegmentConfig::NONE;
            segConfig.dimmingStepCode = 0;
            segConfig.dimmingRemaining = -1;
            _module->queueStatus(static_cast<uint8_t>(1 + (&segConfig - _module->_segments.data())), false);
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
    targetSegment->setPrimaryColor(r, g, b, 0);

    // Save color for effect restore
    cfg.savedR = r;
    cfg.savedG = g;
    cfg.savedB = b;
    cfg.savedWW = 0;
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
    cfg.savedR = r;
    cfg.savedG = g;
    cfg.savedB = b;
    cfg.savedValid = true;

    // If Solid effect is running, update immediately
    if (targetSegment->getEffect() == EffectPool::getSolid())
    {
        targetSegment->setPrimaryColor(r, g, b, cfg.savedWW);
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
        // Start dimming. KNX 3/7/2 3.3.1: step code n divides 0..100 % into 2^(n-1) intervals.
        // Code 1 (100 %) is the usual start/stop telegram: ramp until the stop telegram or a
        // limit. Codes 2..7 move by one interval (50 % .. 1.6 %) and stop by themselves.
        cfg.activeDimming = static_cast<NeoPixelBusModule::SegmentConfig::DimmingChannel>(dimmingChannel);
        cfg.dimmingIncrease = increase;
        cfg.dimmingStepCode = stepCode;
        // Rounded up: 2^(n-1) steps reach the end of the range (255 >> (n-1) left 1..63 behind).
        const int16_t intervals = (int16_t)(1 << (stepCode - 1));
        cfg.dimmingRemaining = (stepCode == 1) ? -1 : (int16_t)((255 + intervals - 1) / intervals);
        cfg.dimmingLastUpdate = millis();
        cfg.dimmingNextStep = millis();
        cfg.dimmingLastStepMs = millis();
        cfg.dimmingStepRest = 0;
        cfg.dimmingWhiteWW = -1;
        logInfoP("Segment %d: Start dimming channel=%d, increase=%d, stepCode=%d",
                 channel, dimmingChannel, increase, stepCode);
    }
}

// ========== Helper Functions for Code Deduplication ==========

void SegmentController::applyColorChannelDimming(Segment* seg, uint8_t colorChannel, int16_t delta)
{
    // Base: the configured primary colour (pixel 0 is brightness-scaled and black when off)
    const EffectConfig& pc = seg->getConfig();
    uint8_t rgb[3] = {pc.r(), pc.g(), pc.b()};
    rgb[colorChannel] = (uint8_t)constrain((int16_t)rgb[colorChannel] + delta, 0, 255);

    const uint8_t bytesPerLed = seg->getVirtualStrip()->getBytesPerLed();
    if (bytesPerLed == 5) // RGBCCT
        seg->setPrimaryColor(rgb[0], rgb[1], rgb[2], pc.ww(), pc.cw());
    else if (bytesPerLed == 4) // RGBW
        seg->setPrimaryColor(rgb[0], rgb[1], rgb[2], pc.w());
    else // RGB: W is not shown, but kept like the absolute colour KOs do (sync byte 6)
        seg->setPrimaryColor(rgb[0], rgb[1], rgb[2], pc.w());
}

void SegmentController::processColorChannelKo(uint8_t channel, GroupObject& ko, uint8_t colorValue, uint8_t colorChannel, const char* channelName)
{
    logInfoP("Segment %d %s: %d", channel, channelName, colorValue);

    auto& cfg = _module->_segments[channel];
    Segment* targetSegment = cfg.segment;

    // Update saved values based on channel
    if (colorChannel == 0) // Red
    {
        cfg.savedR = colorValue;
        if (!cfg.savedValid)
        {
            cfg.savedG = 0;
            cfg.savedB = 0;
            cfg.savedWW = 0;
        }
    }
    else if (colorChannel == 1) // Green
    {
        cfg.savedG = colorValue;
        if (!cfg.savedValid)
        {
            cfg.savedR = 0;
            cfg.savedB = 0;
            cfg.savedWW = 0;
        }
    }
    else if (colorChannel == 2) // Blue
    {
        cfg.savedB = colorValue;
        if (!cfg.savedValid)
        {
            cfg.savedR = 0;
            cfg.savedG = 0;
            cfg.savedWW = 0;
        }
    }
    else if (colorChannel == 3) // White
    {
        cfg.savedWW = colorValue;
        if (!cfg.savedValid)
        {
            cfg.savedR = 0;
            cfg.savedG = 0;
            cfg.savedB = 0;
        }
    }

    cfg.savedValid = true;

    // If Solid effect is running, update immediately
    if (targetSegment->getEffect() == EffectPool::getSolid())
    {
        if (colorChannel == 3) // White channel for RGBW
            targetSegment->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, colorValue);
        else // RGB channels
            targetSegment->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW);
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
    cfg.savedR = r;
    cfg.savedG = g;
    cfg.savedB = b;
    cfg.savedValid = true;

    // If Solid effect is running, update immediately
    if (targetSegment->getEffect() == EffectPool::getSolid())
    {
        targetSegment->setPrimaryColor(r, g, b, cfg.savedWW);
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

    uint8_t r = cfg.savedR;
    uint8_t g = cfg.savedG;
    uint8_t b = cfg.savedB;

    // Send appropriate status feedback based on strip type
    if (_module->_virtualStrip && _module->_virtualStrip->getBytesPerLed() == 4)
    {
        // 4-channel RGBW feedback
        uint8_t w = cfg.savedWW;
        uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
        bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
        if (changed) KoNEO_RGBWState.objectWritten();
    }
    else
    {
        // 3-channel RGB feedback (also used for 5-channel RGBCCT)
        uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
        if (changed) KoNEO_RGBState.objectWritten();
    }

    // Restore original channel index
    _module->_channelIndex = originalChannelIndex;
}
