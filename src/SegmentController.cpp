#include "SegmentController.h"
#include "NeoPixelModule.h"
#include "colorhelper.h"
#include "knxprod.h"
#include "OpenKNX.h"

// Constructor
SegmentController::SegmentController(NeoPixelBusModule* module)
    : _module(module)
{
}

// Destructor
SegmentController::~SegmentController()
{
}

// Calculate DPT3.007 delta for dimming
int16_t SegmentController::dpt3_007_delta(uint8_t stepCode)
{
    // stepCode 1-7 for DPT 3.007: 1=100%, 2=50%, 3=25%, 4=12%, 5=6%, 6=3%, 7=1%
    int16_t delta = 255; // Start with 100%
    
    for (uint8_t i = 1; i < stepCode && i < 7; i++)
    {
        delta = delta / 2; // Halve for each step
    }
    
    // Additional reduction for step 7
    if (stepCode >= 7)
    {
        delta = delta / 4; // ~1-2%
    }
    
    return delta;
}

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

        // Calculate step interval based on stepCode (faster = more frequent updates)
        // stepCode 1 (100%) = ~40ms, stepCode 7 (~2%) = ~250ms
        uint32_t interval = 40 + (segConfig.dimmingStepCode - 1) * 35;
        segConfig.dimmingNextStep = now + interval;

        // Calculate delta for this step
        int16_t delta = dpt3_007_delta(segConfig.dimmingStepCode);
        delta = delta / 6; // Divide by ~6 to make continuous dimming smoother
        if (!segConfig.dimmingIncrease) delta = -delta;

        // Apply dimming based on active channel
        uint8_t r, g, b;
        switch (segConfig.activeDimming)
        {
            case NeoPixelBusModule::SegmentConfig::BRIGHTNESS:
            {
                uint8_t bri = seg->getBrightness();
                int16_t newBri = bri + delta;
                newBri = constrain(newBri, 0, 255);
                seg->setBrightness((uint8_t)newBri);
                break;
            }

            case NeoPixelBusModule::SegmentConfig::RED:
                if (seg->getPixel(0, r, g, b))
                {
                    int16_t newR = r + delta;
                    newR = constrain(newR, 0, 255);
                    seg->setPrimaryColor((uint8_t)newR, g, b, 0);
                }
                break;

            case NeoPixelBusModule::SegmentConfig::GREEN:
                if (seg->getPixel(0, r, g, b))
                {
                    int16_t newG = g + delta;
                    newG = constrain(newG, 0, 255);
                    seg->setPrimaryColor(r, (uint8_t)newG, b, 0);
                }
                break;

            case NeoPixelBusModule::SegmentConfig::BLUE:
                if (seg->getPixel(0, r, g, b))
                {
                    int16_t newB = b + delta;
                    newB = constrain(newB, 0, 255);
                    seg->setPrimaryColor(r, g, (uint8_t)newB, 0);
                }
                break;

            case NeoPixelBusModule::SegmentConfig::WHITE:
            case NeoPixelBusModule::SegmentConfig::WARM_WHITE:
            case NeoPixelBusModule::SegmentConfig::COOL_WHITE:
                if (seg->getVirtualStrip()->getBytesPerLed() == 4)
                {
                    uint8_t w;
                    if (seg->getPixel(0, r, g, b, w))
                    {
                        int16_t newW = w + delta;
                        newW = constrain(newW, 0, 255);
                        seg->setPrimaryColor(r, g, b, (uint8_t)newW);
                    }
                }
                break;

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
                    seg->setPrimaryColor(newR, newG, newB, 0);

                    if (seg->getVirtualStrip()->getBytesPerLed() == 4)
                    {
                        uint8_t w;
                        seg->getPixel(0, r, g, b, w);
                        seg->setPrimaryColor(newR, newG, newB, w);
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
        case NEO_KoS:
        case NEO_KoV:
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
    logInfoP("Segment %d Red: %d", channel, red);
    
    auto& cfg = _module->_segments[channel];
    Segment* targetSegment = cfg.segment;
    
    // Store as pending
    cfg.pendingSolidR = red;
    cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
    cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
    cfg.pendingSolidW = cfg.savedValid ? cfg.savedW : 0;
    
    // Update saved values
    cfg.savedR = red;
    if (!cfg.savedValid)
    {
        cfg.savedG = 0;
        cfg.savedB = 0;
        cfg.savedW = 0;
    }
    cfg.savedBrightness = targetSegment->getBrightness();
    cfg.savedValid = true;
    
    // If Solid effect is running, update immediately
    if (targetSegment->getEffect() == EffectPool::getSolid())
    {
        targetSegment->setPrimaryColor(red, cfg.savedG, cfg.savedB, 0);
        logInfoP("Segment %d: Updated Solid effect color (Red=%d)", channel, red);
    }
    else
    {
        logInfoP("Segment %d: Stored pending Red=%d (will apply when effect stops)", channel, red);
    }
    
    // Send status feedback
    uint8_t _channelIndex = _module->getChannelIndex();
    _module->_channelIndex = channel;
    
    uint8_t r = cfg.pendingSolidR;
    uint8_t g = cfg.pendingSolidG;
    uint8_t b = cfg.pendingSolidB;
    
    if (_module->_virtualStrip && _module->_virtualStrip->getBytesPerLed() == 4)
    {
        uint8_t w = cfg.pendingSolidW;
        uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
        bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
        if (changed) KoNEO_RGBWState.objectWritten();
    }
    else
    {
        uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
        if (changed) KoNEO_RGBState.objectWritten();
    }
    
    _module->_channelIndex = _channelIndex;
}

void SegmentController::processGreenKo(uint8_t channel, GroupObject& ko)
{
    uint8_t green = ko.value(DPT_Value_1_Ucount);
    logInfoP("Segment %d Green: %d", channel, green);
    
    auto& cfg = _module->_segments[channel];
    Segment* targetSegment = cfg.segment;
    
    // Store as pending
    cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
    cfg.pendingSolidG = green;
    cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
    cfg.pendingSolidW = cfg.savedValid ? cfg.savedW : 0;
    
    // Update saved values
    if (!cfg.savedValid)
    {
        cfg.savedR = 0;
        cfg.savedB = 0;
        cfg.savedW = 0;
    }
    cfg.savedG = green;
    cfg.savedBrightness = targetSegment->getBrightness();
    cfg.savedValid = true;
    
    // If Solid effect is running, update immediately
    if (targetSegment->getEffect() == EffectPool::getSolid())
    {
        targetSegment->setPrimaryColor(cfg.savedR, green, cfg.savedB, 0);
        logInfoP("Segment %d: Updated Solid effect color (Green=%d)", channel, green);
    }
    else
    {
        logInfoP("Segment %d: Stored pending Green=%d (will apply when effect stops)", channel, green);
    }
    
    // Send status feedback (same pattern as Red)
    uint8_t _channelIndex = _module->getChannelIndex();
    _module->_channelIndex = channel;
    
    uint8_t r = cfg.pendingSolidR;
    uint8_t g = cfg.pendingSolidG;
    uint8_t b = cfg.pendingSolidB;
    
    if (_module->_virtualStrip && _module->_virtualStrip->getBytesPerLed() == 4)
    {
        uint8_t w = cfg.pendingSolidW;
        uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
        bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
        if (changed) KoNEO_RGBWState.objectWritten();
    }
    else
    {
        uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
        if (changed) KoNEO_RGBState.objectWritten();
    }
    
    _module->_channelIndex = _channelIndex;
}

void SegmentController::processBlueKo(uint8_t channel, GroupObject& ko)
{
    uint8_t blue = ko.value(DPT_Value_1_Ucount);
    logInfoP("Segment %d Blue: %d", channel, blue);
    
    auto& cfg = _module->_segments[channel];
    Segment* targetSegment = cfg.segment;
    
    // Store as pending
    cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
    cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
    cfg.pendingSolidB = blue;
    cfg.pendingSolidW = cfg.savedValid ? cfg.savedW : 0;
    
    // Update saved values
    if (!cfg.savedValid)
    {
        cfg.savedR = 0;
        cfg.savedG = 0;
        cfg.savedW = 0;
    }
    cfg.savedB = blue;
    cfg.savedBrightness = targetSegment->getBrightness();
    cfg.savedValid = true;
    
    // If Solid effect is running, update immediately
    if (targetSegment->getEffect() == EffectPool::getSolid())
    {
        targetSegment->setPrimaryColor(cfg.savedR, cfg.savedG, blue, 0);
        logInfoP("Segment %d: Updated Solid effect color (Blue=%d)", channel, blue);
    }
    else
    {
        logInfoP("Segment %d: Stored pending Blue=%d (will apply when effect stops)", channel, blue);
    }
    
    // Send status feedback (same pattern as Red/Green)
    uint8_t _channelIndex = _module->getChannelIndex();
    _module->_channelIndex = channel;
    
    uint8_t r = cfg.pendingSolidR;
    uint8_t g = cfg.pendingSolidG;
    uint8_t b = cfg.pendingSolidB;
    
    if (_module->_virtualStrip && _module->_virtualStrip->getBytesPerLed() == 4)
    {
        uint8_t w = cfg.pendingSolidW;
        uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
        bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
        if (changed) KoNEO_RGBWState.objectWritten();
    }
    else
    {
        uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
        if (changed) KoNEO_RGBState.objectWritten();
    }
    
    _module->_channelIndex = _channelIndex;
}

void SegmentController::processWhiteKo(uint8_t channel, GroupObject& ko)
{
    uint8_t white = ko.value(DPT_Value_1_Ucount);
    logInfoP("Segment %d White: %d", channel, white);
    
    auto& cfg = _module->_segments[channel];
    Segment* targetSegment = cfg.segment;
    
    // Store as pending
    cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
    cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
    cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
    cfg.pendingSolidW = white;
    
    // Update saved values
    if (!cfg.savedValid)
    {
        cfg.savedR = 0;
        cfg.savedG = 0;
        cfg.savedB = 0;
    }
    cfg.savedW = white;
    cfg.savedBrightness = targetSegment->getBrightness();
    cfg.savedValid = true;
    
    // If Solid effect is running, update immediately
    if (targetSegment->getEffect() == EffectPool::getSolid())
    {
        targetSegment->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, white);
        logInfoP("Segment %d: Updated Solid effect color (White=%d)", channel, white);
    }
    else
    {
        logInfoP("Segment %d: Stored pending White=%d (will apply when effect stops)", channel, white);
    }
    
    // Send RGBW status feedback
    uint8_t _channelIndex = _module->getChannelIndex();
    _module->_channelIndex = channel;
    
    uint8_t r = cfg.pendingSolidR;
    uint8_t g = cfg.pendingSolidG;
    uint8_t b = cfg.pendingSolidB;
    uint8_t w = white;
    uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
    bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
    if (changed) KoNEO_RGBWState.objectWritten();
    
    _module->_channelIndex = _channelIndex;
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
    cfg.savedW = 0;
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

void SegmentController::processHsvKo(uint8_t channel, GroupObject& ko)
{
    // HSV handling - simplified version, can be expanded
    logInfoP("Segment %d: HSV KO received", channel);
    // TODO: Implement full HSV handling similar to processRedKo/Green/Blue
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
