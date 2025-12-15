#include "NeoPixelModule.h"
#include "colorhelper.h"
#include "OpenKNX.h"
#include "knxprod.h"
#include <algorithm>
#include <vector>

NeoPixelBusModule openknxNeoPixelModule;
extern NeoPixel neoPixelModule;

void NeoPixelBusModule::setup(bool configured)
{
  if (configured)
  {
    configureFromETS();
    _initialized = true;
    neoPixelModule.setup(configured);
  }
}

void NeoPixelBusModule::loop(bool configured)
{
  if (!configured || !_initialized) return;
  neoPixelModule.loop(configured);
}

void NeoPixelBusModule::processInputKo(GroupObject& ko)
{
  // Get the KO number for routing to correct channel/segment
  uint16_t koNumber = ko.asap();
  logInfoP("processInputKo: KO Number %d", koNumber);
  // Global NeoPixel KOs
  if (koNumber == NEO_KoPower) {
    bool powerState = ko.value(DPT_Switch);
    logInfoP("Global Power KO: %s", powerState ? "ON" : "OFF");
    
    if (powerState) {
      // Power on - restore previous state or apply default
      neoPixelModule.updateAll();
    } else {
      // Power off - turn off all LEDs  
      neoPixelModule.clearAll();
    }
    
    // Send state feedback
    bool changed = KoNEO_PowerState.valueNoSendCompare(powerState, DPT_Switch);
    if (changed) KoNEO_PowerState.objectWritten();
    return;
  }
  
  if (koNumber == NEO_KoBrightness) {
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
  
  if (koNumber == NEO_KoHCLState) {
    uint16_t kelvinValue = ko.value(Dpt(7, 600)); // DPT 7.600 = Color Temperature (Kelvin)
    logInfoP("HCL State KO received: %dK color temperature", kelvinValue);
    
    // Special case: 0K = disable HCL mode
    if (kelvinValue == 0) {
      logInfoP("HCL disable command received (0K)");
      disableHclMode();
    } else {
      // Apply the color temperature globally if valid range
      applyHclColorTemperature(kelvinValue);
    }
    return;
  }
  
  // Channel-specific KOs (segment-based)
  // Calculate which channel this KO belongs to
  int channel = NEO_KoCalcChannel(koNumber);
  if (channel >= 0) {
    uint8_t oldChannelIndex = _channelIndex;
    _channelIndex = channel;
    
    int koIndex = NEO_KoCalcIndex(koNumber);
    
    // IMPORTANT: "channel" here refers to segment index, not physical strip index
    // The KNX channels correspond to logical segments that users can control
    // Physical strips are hardware configuration and not directly controlled via KOs
    
    logInfoP("Segment Channel %d KO Index %d received (_segments.size=%d, _numberOfSegments=%d)", 
             channel, koIndex, (int)_segments.size(), _numberOfSegments);
    
    // Validate channel against segment count, not physical strip count
    if (channel >= _segments.size()) {
      logWarningP("KO for channel %d ignored - only %d segments configured (_numberOfSegments=%d)", 
                  channel, (int)_segments.size(), _numberOfSegments);
      _channelIndex = oldChannelIndex;
      return;
    }
    
    // Get the target segment for this channel
    Segment* targetSegment = (channel < _segments.size()) ? _segments[channel].segment : nullptr;
    if (!targetSegment) {
      logWarningP("Channel %d has no valid segment, ignoring KO", channel);
      _channelIndex = oldChannelIndex;
      return;
    }
    
    switch (koIndex) {
      case NEO_KoR: {
        uint8_t red = ko.value(DPT_Value_1_Ucount);   // 5.010
        logInfoP("Segment %d Red: %d", channel, red);
        
        // Apply red value to segment - get current RGB and update red channel
        uint8_t r, g, b;
        if (targetSegment->getPixel(0, r, g, b)) {
          targetSegment->setAll(red, g, b);
        } else {
          // Fallback if getPixel fails
          targetSegment->setAll(red, 0, 0);
        }
        
        // Send combined RGB/RGBW status feedback
        _channelIndex = channel;
        targetSegment->getPixel(0, r, g, b);
        // Check if virtual strip has white channel (4 bytes per LED)
        if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4) {
          // Has white channel - send RGBW status
          uint8_t w;
          targetSegment->getPixel(0, r, g, b, w);
          uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
          bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
          if (changed) KoNEO_RGBWState.objectWritten();
        } else {
          // No white channel - send RGB status
          uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;   // RGB
          bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
          if (changed) KoNEO_RGBState.objectWritten();
        }
        _channelIndex = oldChannelIndex;
        break;
      }
      
      case NEO_KoG: {
        uint8_t green = ko.value(DPT_Value_1_Ucount);   // 5.010
        logInfoP("Segment %d Green: %d", channel, green);
        
        // Apply green value to segment - get current RGB and update green channel
        uint8_t r, g, b;
        if (targetSegment->getPixel(0, r, g, b)) {
          targetSegment->setAll(r, green, b);
        } else {
          // Fallback if getPixel fails
          targetSegment->setAll(0, green, 0);
        }
        
        // Send combined RGB/RGBW status feedback
        _channelIndex = channel;
        targetSegment->getPixel(0, r, g, b);
        // Check if virtual strip has white channel (4 bytes per LED)
        if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4) {
          // Has white channel - send RGBW status
          uint8_t w;
          targetSegment->getPixel(0, r, g, b, w);
          uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
          bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
          if (changed) KoNEO_RGBWState.objectWritten();
        } else {
          // No white channel - send RGB status
          uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;   // RGB
          bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
          if (changed) KoNEO_RGBState.objectWritten();
        }
        _channelIndex = oldChannelIndex;
        break;
      }
      
      case NEO_KoB: {
        uint8_t blue = ko.value(DPT_Value_1_Ucount);   // 5.010
        logInfoP("Segment %d Blue: %d", channel, blue);
        
        // Apply blue value to segment - get current RGB and update blue channel
        uint8_t r, g, b;
        if (targetSegment->getPixel(0, r, g, b)) {
          targetSegment->setAll(r, g, blue);
        } else {
          // Fallback if getPixel fails
          targetSegment->setAll(0, 0, blue);
        }
        
        // Send combined RGB/RGBW status feedback
        _channelIndex = channel;
        targetSegment->getPixel(0, r, g, b);
        // Check if virtual strip has white channel (4 bytes per LED)
        if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4) {
          // Has white channel - send RGBW status
          uint8_t w;
          targetSegment->getPixel(0, r, g, b, w);
          uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
          bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
          if (changed) KoNEO_RGBWState.objectWritten();
        } else {
          // No white channel - send RGB status
          uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;   // RGB
          bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
          if (changed) KoNEO_RGBState.objectWritten();
        }
        _channelIndex = oldChannelIndex;
        break;
      }
      
      case NEO_KoW: {
        uint8_t white = ko.value(DPT_Value_1_Ucount);   // 5.010
        logInfoP("Segment %d White: %d", channel, white);
        
        // Apply white value to segment - get current RGB and update with white channel
        uint8_t r, g, b;
        if (targetSegment->getPixel(0, r, g, b)) {
          targetSegment->setAll(r, g, b, white);
        } else {
          // Fallback if getPixel fails - pure white
          targetSegment->setAll(0, 0, 0, white);
        }
        
        // Send RGBW status feedback (white channel update)
        _channelIndex = channel;
        targetSegment->getPixel(0, r, g, b);
        uint8_t w = white;
        uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;   // RGBW
        bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
        if (changed) KoNEO_RGBWState.objectWritten();
        _channelIndex = oldChannelIndex;
        break;
      }
      
      case NEO_KoCCT: {
        uint16_t cct = ko.value(DPT_Value_Temp);
        logInfoP("Segment %d CCT: %dK", channel, cct);
        
        // Apply color temperature to segment - convert Kelvin to RGB
        uint8_t r, g, b;
        ColorHelper::kelvinToRGB(cct, r, g, b);
        targetSegment->setAll(r, g, b);
        
        // Send status feedback
        _channelIndex = channel;
        bool changed = KoNEO_CCTState.valueNoSendCompare(cct, DPT_Value_Temp);
        if (changed) KoNEO_CCTState.objectWritten();
        _channelIndex = oldChannelIndex;
        break;
      }
      
      case NEO_KoH: {
        uint16_t hue = ko.value(DPT_Angle);
        logInfoP("Segment %d Hue: %d°", channel, hue);
        
        // Apply hue to HSV on segment - convert angle to 0-255 range
        // DPT_Angle uses 0-360° as 0-65535, so scale to 0-255 for HSV
        uint8_t h = (hue * 255) / 360;
        
        // Use full saturation and current brightness or default
        uint8_t s = 255;  // Full saturation
        uint8_t v = targetSegment->getBrightness();
        if (v == 0) v = 255;  // Default brightness if not set
        
        uint8_t r, g, b;
        ColorHelper::hsvToRGB(h, s, v, r, g, b);
        targetSegment->setAll(r, g, b);
        
        // Send HSV status feedback with all current HSV values
        _channelIndex = channel;
        targetSegment->getPixel(0, r, g, b);
        ColorHelper::rgbToHSV(r, g, b, h, s, v);
        uint32_t hsv = ((uint32_t)h << 16) | ((uint32_t)s << 8) | v;
        bool changed = KoNEO_HSVState.valueNoSendCompare(hsv, DPT_Colour_RGB);
        if (changed) KoNEO_HSVState.objectWritten();
        _channelIndex = oldChannelIndex;
        break;
      }
      
      case NEO_KoS: {
        // DPT 5.001 sends 0-100 (percentage), need to scale to 0-255 for internal use
        uint8_t saturationPercent = ko.value(DPT_Scaling);
        uint8_t saturation = (saturationPercent * 255) / 100;
        logInfoP("Segment %d Saturation: %d%% (scaled to %d/255)", channel, saturationPercent, saturation);
        
        // Apply saturation to HSV on segment
        // For HSV control, we need to maintain current hue and value
        // This is a simplified implementation - in practice you might want to 
        // store HSV values in the segment configuration
        uint8_t h = 0;    // Default hue (red)
        uint8_t s = saturation;
        uint8_t v = targetSegment->getBrightness();
        if (v == 0) v = 255;  // Default brightness if not set
        
        uint8_t r, g, b;
        ColorHelper::hsvToRGB(h, s, v, r, g, b);
        targetSegment->setAll(r, g, b);
        
        // Send HSV status feedback with all current HSV values
        _channelIndex = channel;
        targetSegment->getPixel(0, r, g, b);
        ColorHelper::rgbToHSV(r, g, b, h, s, v);
        uint32_t hsv = ((uint32_t)h << 16) | ((uint32_t)s << 8) | v;
        bool changed = KoNEO_HSVState.valueNoSendCompare(hsv, DPT_Colour_RGB);
        if (changed) KoNEO_HSVState.objectWritten();
        _channelIndex = oldChannelIndex;
        break;
      }
      
      case NEO_KoV: {
        // DPT 5.001 sends 0-100 (percentage), need to scale to 0-255 for internal use
        uint8_t valuePercent = ko.value(DPT_Scaling);
        uint8_t value = (valuePercent * 255) / 100;
        logInfoP("Segment %d Value: %d%% (scaled to %d/255)", channel, valuePercent, value);
        
        // Apply value/brightness to HSV on segment
        // Set brightness and scale current colors accordingly
        targetSegment->setBrightness(value);
        
        // For immediate effect, also scale current pixel colors
        uint8_t r, g, b;
        if (targetSegment->getPixel(0, r, g, b)) {
          // Scale current colors by new brightness
          r = (r * value) / 255;
          g = (g * value) / 255;
          b = (b * value) / 255;
          targetSegment->setAll(r, g, b);
        }
        
        // Send HSV status feedback with all current HSV values
        _channelIndex = channel;
        targetSegment->getPixel(0, r, g, b);
        uint8_t h, s, v;
        ColorHelper::rgbToHSV(r, g, b, h, s, v);
        uint32_t hsv = ((uint32_t)h << 16) | ((uint32_t)s << 8) | v;
        bool changed = KoNEO_HSVState.valueNoSendCompare(hsv, DPT_Colour_RGB);
        if (changed) KoNEO_HSVState.objectWritten();
        _channelIndex = oldChannelIndex;
        break;
      }
      
      case NEO_KoRGB: {
        uint32_t rgb = ko.value(DPT_Colour_RGB);
        uint8_t r = (rgb >> 16) & 0xFF;
        uint8_t g = (rgb >> 8) & 0xFF;
        uint8_t b = rgb & 0xFF;
        logInfoP("Segment %d RGB: R=%d G=%d B=%d", channel, r, g, b);
        
        // Apply RGB to the target segment
        targetSegment->setAll(r, g, b);
        
        // Send status feedback
        _channelIndex = channel;
        bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
        if (changed) KoNEO_RGBState.objectWritten();
        _channelIndex = oldChannelIndex;
        break;
      }
      
      case NEO_KoHSV: {
        uint32_t hsv = ko.value(DPT_Colour_RGB); // Using same DPT for now
        logInfoP("Segment %d HSV: 0x%08X", channel, hsv);
        
        // Convert HSV to RGB and apply to segment
        // Extract HSV components from the 32-bit value
        // Assuming format: 0x00HHSSVV (H=hue, S=saturation, V=value)
        uint8_t h = (hsv >> 16) & 0xFF;  // Hue
        uint8_t s = (hsv >> 8) & 0xFF;   // Saturation  
        uint8_t v = hsv & 0xFF;          // Value/Brightness
        
        uint8_t r, g, b;
        ColorHelper::hsvToRGB(h, s, v, r, g, b);
        targetSegment->setAll(r, g, b);
        
        // Send status feedback
        _channelIndex = channel;
        bool changed = KoNEO_HSVState.valueNoSendCompare(hsv, DPT_Colour_RGB);
        if (changed) KoNEO_HSVState.objectWritten();
        _channelIndex = oldChannelIndex;
        break;
      }
      
      case NEO_KoFx: {
        uint8_t effect = ko.value(DPT_Value_1_Ucount);   // 5.010
        logInfoP("Segment %d Effect: %d", channel, effect);
        
        // Apply effect to segment dynamically
        applyEffectToSegment(targetSegment, effect);
        
        // Send status feedback
        _channelIndex = channel;
        bool changed = KoNEO_FxState.valueNoSendCompare(effect, DPT_SceneNumber);
        if (changed) KoNEO_FxState.objectWritten();
        _channelIndex = oldChannelIndex;
        
        // Enable auto-update if effects are being used
        if (effect > 0 && !_autoUpdateEnabled) {
          neoPixelModule.setAutoUpdate(true);
          neoPixelModule.setUpdateSpeed(UpdateSpeed::NORMAL);
          _autoUpdateEnabled = true;
          _effectsEnabled = true;
          logInfoP("Auto-update enabled for effects");
        }
        break;
      }
      
      case NEO_KoPreset: {
        uint8_t preset = ko.value(DPT_SceneNumber);
        logInfoP("Segment %d Preset: %d", channel, preset);
        
        // Apply preset configuration to segment (predefined color/effect combinations)
        switch (preset) {
          case 1:  // Red
            targetSegment->setAll(255, 0, 0);
            targetSegment->setBrightness(255);
            break;
          case 2:  // Green  
            targetSegment->setAll(0, 255, 0);
            targetSegment->setBrightness(255);
            break;
          case 3:  // Blue
            targetSegment->setAll(0, 0, 255);
            targetSegment->setBrightness(255);
            break;
          case 4:  // White
            targetSegment->setAll(255, 255, 255);
            targetSegment->setBrightness(255);
            break;
          case 5:  // Warm White (using color temperature)
            {
              uint8_t r, g, b;
              ColorHelper::kelvinToRGB(2700, r, g, b);  // Warm white ~2700K
              targetSegment->setAll(r, g, b);
              targetSegment->setBrightness(255);
            }
            break;
          case 6:  // Cool White
            {
              uint8_t r, g, b;
              ColorHelper::kelvinToRGB(6500, r, g, b);  // Cool white ~6500K
              targetSegment->setAll(r, g, b);
              targetSegment->setBrightness(255);
            }
            break;
          case 7:  // Rainbow Effect
            applyEffectToSegment(targetSegment, 1);  // Rainbow effect
            targetSegment->setBrightness(200);
            break;
          case 8:  // Off
            targetSegment->setAll(0, 0, 0);
            targetSegment->setBrightness(0);
            break;
          default:
            logWarningP("Unknown preset %d for segment %d", preset, channel);
            break;
        }
        break;
      }
      
      case NEO_KoSegmentPower: {
        bool power = ko.value(DPT_Switch);
        logDebugP("Segment %d Power: %s", channel, power ? "ON" : "OFF");
        
        if (power) {
          // Turn segment on - restore previous brightness or set to full
          uint8_t brightness = targetSegment->getBrightness();
          if (brightness == 0) {
            targetSegment->setBrightness(255);  // Default to full brightness if was off
          }
        } else {
          // Turn segment off - set brightness to 0
          targetSegment->setBrightness(0);
          targetSegment->setAll(0, 0, 0);  // Also clear pixels immediately
        }
        
        // Send status feedback
        _channelIndex = channel;
        bool changed = KoNEO_SegmentPowerState.valueNoSendCompare(power, DPT_Switch);
        if (changed) KoNEO_SegmentPowerState.objectWritten();
        _channelIndex = oldChannelIndex;
        break;
      }
      
      case NEO_KoSegmentBrightness: {
        // DPT 5.001 sends 0-100 (percentage), need to scale to 0-255 for internal use
        uint8_t brightnessPercent = ko.value(DPT_Scaling);
        uint8_t brightness = (brightnessPercent * 255) / 100;
        logInfoP("Segment %d Brightness: %d%% (scaled to %d/255)", channel, brightnessPercent, brightness);
        
        // Set segment brightness
        targetSegment->setBrightness(brightness);
        
        // Send status feedback (send back percentage)
        _channelIndex = channel;
        bool changed = KoNEO_SegmentBrightnessState.valueNoSendCompare(brightnessPercent, DPT_Scaling);
        if (changed) KoNEO_SegmentBrightnessState.objectWritten();
        _channelIndex = oldChannelIndex;
        break;
      }

      // Warm White and Cool White control
      case NEO_KoWW: {
        uint8_t ww = ko.value(DPT_Value_1_Ucount);   // 5.010
        logInfoP("Segment %d Warm White: %d", channel, ww);
        
        // Store warm white in primaryW and apply to all pixels
        uint8_t r = targetSegment->getConfig().r();
        uint8_t g = targetSegment->getConfig().g();
        uint8_t b = targetSegment->getConfig().b();
        targetSegment->setPrimaryColor(r, g, b, ww);
        targetSegment->setAll(r, g, b, ww);
        break;
      }

      case NEO_KoCW: {
        uint8_t cw = ko.value(DPT_Value_1_Ucount);   // 5.010
        logInfoP("Segment %d Cool White: %d", channel, cw);
        
        // For cool white, store in primaryW (Note: secondaryW was removed in refactoring)
        uint8_t r = targetSegment->getConfig().r();
        uint8_t g = targetSegment->getConfig().g();
        uint8_t b = targetSegment->getConfig().b();
        targetSegment->setPrimaryColor(r, g, b, cw);
        targetSegment->setAll(r, g, b, cw);
        break;
      }

      // RGBW combined control
      case NEO_KoRGBW: {
        uint32_t rgbw = ko.value(DPT_Colour_RGBW);
        uint8_t r = (rgbw >> 24) & 0xFF;   // RGBW
        uint8_t g = (rgbw >> 16) & 0xFF;
        uint8_t b = (rgbw >> 8) & 0xFF;
        uint8_t w = rgbw & 0xFF;
        
        logInfoP("Segment %d RGBW: R=%d G=%d B=%d W=%d", channel, r, g, b, w);
        
        // Store in config and apply
        targetSegment->setPrimaryColor(r, g, b, w);
        targetSegment->setAll(r, g, b, w);
        
        // Send status feedback
        _channelIndex = channel;
        bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
        if (changed) KoNEO_RGBWState.objectWritten();
        _channelIndex = oldChannelIndex;
        break;
      }

      // Relative controls (DPT 3.007 - relative dimming)
      case NEO_KoBriRel: {
        uint8_t rel = ko.value(DPT_Control_Dimming);
        uint8_t direction = (rel & 0x08) >> 3; // 0 = decrease, 1 = increase
        uint8_t stepCode = rel & 0x07;
        
        // Calculate step size based on stepCode (0 = stop, 1-7 = different step sizes)
        if (stepCode > 0) {
          uint8_t currentBrightness = targetSegment->getBrightness();
          int16_t step = (stepCode * 255) / 64; // Scale step to 0-255 range
          int16_t newBrightness = direction ? (currentBrightness + step) : (currentBrightness - step);
          newBrightness = constrain(newBrightness, 0, 255);
          
          logInfoP("Segment %d Brightness Relative: %s %d -> %d", channel, 
                   direction ? "UP" : "DOWN", currentBrightness, newBrightness);
          
          targetSegment->setBrightness((uint8_t)newBrightness);
        }
        break;
      }

      case NEO_KoRRel: {
        uint8_t rel = ko.value(DPT_Control_Dimming);
        uint8_t direction = (rel & 0x08) >> 3;
        uint8_t stepCode = rel & 0x07;
        
        if (stepCode > 0) {
          uint8_t currentRed = targetSegment->getConfig().r();
          int16_t step = (stepCode * 255) / 64;
          int16_t newRed = direction ? (currentRed + step) : (currentRed - step);
          newRed = constrain(newRed, 0, 255);
          
          logInfoP("Segment %d Red Relative: %s %d -> %d", channel, 
                   direction ? "UP" : "DOWN", currentRed, newRed);
          
          uint8_t g = targetSegment->getConfig().g();
          uint8_t b = targetSegment->getConfig().b();
          uint8_t w = targetSegment->getConfig().w();
          targetSegment->setPrimaryColor((uint8_t)newRed, g, b, w);
          targetSegment->setAll((uint8_t)newRed, g, b, w);
        }
        break;
      }

      case NEO_KoGRel: {
        uint8_t rel = ko.value(DPT_Control_Dimming);
        uint8_t direction = (rel & 0x08) >> 3;
        uint8_t stepCode = rel & 0x07;
        
        if (stepCode > 0) {
          uint8_t currentGreen = targetSegment->getConfig().g();
          int16_t step = (stepCode * 255) / 64;
          int16_t newGreen = direction ? (currentGreen + step) : (currentGreen - step);
          newGreen = constrain(newGreen, 0, 255);
          
          logInfoP("Segment %d Green Relative: %s %d -> %d", channel, 
                   direction ? "UP" : "DOWN", currentGreen, newGreen);
          
          uint8_t r = targetSegment->getConfig().r();
          uint8_t b = targetSegment->getConfig().b();
          uint8_t w = targetSegment->getConfig().w();
          targetSegment->setPrimaryColor(r, (uint8_t)newGreen, b, w);
          targetSegment->setAll(r, (uint8_t)newGreen, b, w);
        }
        break;
      }

      case NEO_KoBRel: {
        uint8_t rel = ko.value(DPT_Control_Dimming);
        uint8_t direction = (rel & 0x08) >> 3;
        uint8_t stepCode = rel & 0x07;
        
        if (stepCode > 0) {
          uint8_t currentBlue = targetSegment->getConfig().b();
          int16_t step = (stepCode * 255) / 64;
          int16_t newBlue = direction ? (currentBlue + step) : (currentBlue - step);
          newBlue = constrain(newBlue, 0, 255);
          
          logInfoP("Segment %d Blue Relative: %s %d -> %d", channel, 
                   direction ? "UP" : "DOWN", currentBlue, newBlue);
          
          uint8_t r = targetSegment->getConfig().r();
          uint8_t g = targetSegment->getConfig().g();
          uint8_t w = targetSegment->getConfig().w();
          targetSegment->setPrimaryColor(r, g, (uint8_t)newBlue, w);
          targetSegment->setAll(r, g, (uint8_t)newBlue, w);
        }
        break;
      }

      case NEO_KoWRel: {
        uint8_t rel = ko.value(DPT_Control_Dimming);
        uint8_t direction = (rel & 0x08) >> 3;
        uint8_t stepCode = rel & 0x07;
        
        if (stepCode > 0) {
          uint8_t currentWhite = targetSegment->getConfig().w();
          int16_t step = (stepCode * 255) / 64;
          int16_t newWhite = direction ? (currentWhite + step) : (currentWhite - step);
          newWhite = constrain(newWhite, 0, 255);
          
          logInfoP("Segment %d White Relative: %s %d -> %d", channel, 
                   direction ? "UP" : "DOWN", currentWhite, newWhite);
          
          uint8_t r = targetSegment->getConfig().r();
          uint8_t g = targetSegment->getConfig().g();
          uint8_t b = targetSegment->getConfig().b();
          targetSegment->setPrimaryColor(r, g, b, (uint8_t)newWhite);
          targetSegment->setAll(r, g, b, (uint8_t)newWhite);
        }
        break;
      }

      case NEO_KoWWRel: {
        uint8_t rel = ko.value(DPT_Control_Dimming);
        uint8_t direction = (rel & 0x08) >> 3;
        uint8_t stepCode = rel & 0x07;
        
        if (stepCode > 0) {
          uint8_t currentWW = targetSegment->getConfig().w(); // Using primaryW for warm white
          int16_t step = (stepCode * 255) / 64;
          int16_t newWW = direction ? (currentWW + step) : (currentWW - step);
          newWW = constrain(newWW, 0, 255);
          
          logInfoP("Segment %d Warm White Relative: %s %d -> %d", channel, 
                   direction ? "UP" : "DOWN", currentWW, newWW);
          
          uint8_t r = targetSegment->getConfig().r();
          uint8_t g = targetSegment->getConfig().g();
          uint8_t b = targetSegment->getConfig().b();
          targetSegment->setPrimaryColor(r, g, b, (uint8_t)newWW);
          targetSegment->setAll(r, g, b, (uint8_t)newWW);
        }
        break;
      }

      case NEO_KoCWRel: {
        uint8_t rel = ko.value(DPT_Control_Dimming);
        uint8_t direction = (rel & 0x08) >> 3;
        uint8_t stepCode = rel & 0x07;
        
        if (stepCode > 0) {
          uint8_t currentCW = targetSegment->getConfig().w(); // Using primaryW (secondaryW removed)
          int16_t step = (stepCode * 255) / 64;
          int16_t newCW = direction ? (currentCW + step) : (currentCW - step);
          newCW = constrain(newCW, 0, 255);
          
          logInfoP("Segment %d Cool White Relative: %s %d -> %d", channel, 
                   direction ? "UP" : "DOWN", currentCW, newCW);
          
          // For cool white relative, apply as white channel (Note: secondaryW removed in refactoring)
          uint8_t r = targetSegment->getConfig().r();
          uint8_t g = targetSegment->getConfig().g();
          uint8_t b = targetSegment->getConfig().b();
          targetSegment->setPrimaryColor(r, g, b, (uint8_t)newCW);
          targetSegment->setAll(r, g, b, (uint8_t)newCW);
        }
        break;
      }

      case NEO_KoHRel: {
        uint8_t rel = ko.value(DPT_Control_Dimming);
        uint8_t direction = (rel & 0x08) >> 3;
        uint8_t stepCode = rel & 0x07;
        
        if (stepCode > 0) {
          // Get current RGB from segment config
          uint8_t r = targetSegment->getConfig().r();
          uint8_t g = targetSegment->getConfig().g();
          uint8_t b = targetSegment->getConfig().b();
          uint8_t w = targetSegment->getConfig().w();
          
          // Convert RGB to HSV
          uint8_t h, s, v;
          ColorHelper::rgbToHSV(r, g, b, h, s, v);
          
          // Calculate hue step (0-255 maps to 0-360 degrees)
          int16_t step = (stepCode * 255) / 64; // Scale step to 0-255 range
          int16_t newHue = direction ? (h + step) : (h - step);
          
          // Wrap hue around 0-255 (equivalent to 0-360 degrees)
          if (newHue < 0) newHue += 255;
          else if (newHue > 255) newHue -= 255;
          
          logInfoP("Segment %d Hue Relative: %s H=%d->%d S=%d V=%d", channel, 
                   direction ? "UP" : "DOWN", h, newHue, s, v);
          
          // Convert back to RGB
          uint8_t newR, newG, newB;
          ColorHelper::hsvToRGB((uint8_t)newHue, s, v, newR, newG, newB);
          
          // Store in config and apply
          targetSegment->setPrimaryColor(newR, newG, newB, w);
          targetSegment->setAll(newR, newG, newB, w);
        }
        break;
      }

      case NEO_KoSRel: {
        uint8_t rel = ko.value(DPT_Control_Dimming);
        uint8_t direction = (rel & 0x08) >> 3;
        uint8_t stepCode = rel & 0x07;
        
        if (stepCode > 0) {
          // Get current RGB from segment config
          uint8_t r = targetSegment->getConfig().r();
          uint8_t g = targetSegment->getConfig().g();
          uint8_t b = targetSegment->getConfig().b();
          uint8_t w = targetSegment->getConfig().w();
          
          // Convert RGB to HSV
          uint8_t h, s, v;
          ColorHelper::rgbToHSV(r, g, b, h, s, v);
          
          // Calculate saturation step
          int16_t step = (stepCode * 255) / 64;
          int16_t newSat = direction ? (s + step) : (s - step);
          newSat = constrain(newSat, 0, 255);
          
          logInfoP("Segment %d Saturation Relative: %s H=%d S=%d->%d V=%d", channel, 
                   direction ? "UP" : "DOWN", h, s, newSat, v);
          
          // Convert back to RGB
          uint8_t newR, newG, newB;
          ColorHelper::hsvToRGB(h, (uint8_t)newSat, v, newR, newG, newB);
          
          // Store in config and apply
          targetSegment->setPrimaryColor(newR, newG, newB, w);
          targetSegment->setAll(newR, newG, newB, w);
        }
        break;
      }

      case NEO_KoVRel: {
        uint8_t rel = ko.value(DPT_Control_Dimming);
        uint8_t direction = (rel & 0x08) >> 3;
        uint8_t stepCode = rel & 0x07;
        
        if (stepCode > 0) {
          // Get current RGB from segment config
          uint8_t r = targetSegment->getConfig().r();
          uint8_t g = targetSegment->getConfig().g();
          uint8_t b = targetSegment->getConfig().b();
          uint8_t w = targetSegment->getConfig().w();
          
          // Convert RGB to HSV
          uint8_t h, s, v;
          ColorHelper::rgbToHSV(r, g, b, h, s, v);
          
          // Calculate value (brightness) step
          int16_t step = (stepCode * 255) / 64;
          int16_t newValue = direction ? (v + step) : (v - step);
          newValue = constrain(newValue, 0, 255);
          
          logInfoP("Segment %d Value Relative: %s H=%d S=%d V=%d->%d", channel, 
                   direction ? "UP" : "DOWN", h, s, v, newValue);
          
          // Convert back to RGB
          uint8_t newR, newG, newB;
          ColorHelper::hsvToRGB(h, s, (uint8_t)newValue, newR, newG, newB);
          
          // Store in config and apply
          targetSegment->setPrimaryColor(newR, newG, newB, w);
          targetSegment->setAll(newR, newG, newB, w);
        }
        break;
      }

      case NEO_KoRGBRel: {
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
        targetSegment->setAll((uint8_t)newR, (uint8_t)newG, (uint8_t)newB, currentW);
        break;
      }

      case NEO_KoHSVRel: {
        // HSV relative control - expects 3-byte HSV value with relative changes
        uint32_t hsvRel = ko.value(DPT_Colour_RGB); // Using RGB DPT for 3-byte HSV data
        uint8_t hRel = (hsvRel >> 16) & 0xFF; // Hue relative (scaled to 0-255)
        uint8_t sRel = (hsvRel >> 8) & 0xFF;  // Saturation relative
        uint8_t vRel = hsvRel & 0xFF;         // Value relative
        
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
        else if (newH >= 256) newH -= 256; // Hue wraps around in 0-255 range
        newS = constrain(newS, 0, 255);
        newV = constrain(newV, 0, 255);
        
        logInfoP("Segment %d HSV Relative: H=%d->%d, S=%d->%d, V=%d->%d", channel,
                 currentH, newH, currentS, newS, currentV, newV);
        
        // Convert back to RGB and apply
        uint8_t newR, newG, newB;
        ColorHelper::hsvToRGB((uint8_t)newH, (uint8_t)newS, (uint8_t)newV, newR, newG, newB);
        
        // Update config and apply
        targetSegment->setPrimaryColor(newR, newG, newB, currentW);
        targetSegment->setAll(newR, newG, newB, currentW);
        break;
      }

      case NEO_KoRGBWRel: {
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
        targetSegment->setAll((uint8_t)newR, (uint8_t)newG, (uint8_t)newB, (uint8_t)newW);
        break;
      }
      
      // Status-only KOs (prevent "Unhandled KO Index" warnings)
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
    
    // Trigger update after processing KO
    neoPixelModule.updateAll();
  }
}

// Console: show help entries by delegating to core NeoPixel module
void NeoPixelBusModule::showHelp()
{
  // Print the 'neo' command group header via core module
  neoPixelModule.showHelp();
}

// Console: process commands by delegating 'neo' prefixed commands
bool NeoPixelBusModule::processCommand(const std::string command, bool diagnose)
{
  // Forward to core module's console handler
  return neoPixelModule.processCommand(command, diagnose);
}

void NeoPixelBusModule::configureFromETS()
{
  // Initialize the OFM-NeoPixel module first
  neoPixelModule.init();
  
  // 1) Determine number of strips from ETS (max 6 strips supported in virtual configuration)
  const uint8_t maxStrips = std::max<uint8_t>(1, std::min<uint8_t>(6, ParamNEO_NEONumberOfLEDStrips));
  _totalLeds = 0;
  _physicalStrips.clear();
  
  // Clear virtual strip configuration
  _virtualStripConfiguration.clear();

  // 2) Create all physical strips first to determine total LED count
  for (uint8_t i = 0; i < maxStrips; ++i)
  {
    _channelIndex = i;
    
    const LedProtocol proto = mapProtocol((uint8_t)ParamNEOSTRIP_NEOLEDType);
    const ColorOrder order = mapColorOrder((uint8_t)ParamNEOSTRIP_NEOColourOrder);
    const uint8_t dataGpio = (uint8_t)ParamNEOSTRIP_NEODataGPIO;
    const uint16_t pixels = (uint16_t)ParamNEOSTRIP_NEOLength;

    // Determine if this is an SPI protocol and get appropriate GPIO pins
    PhysicalStrip* phys = nullptr;
    if (isSpiProtocol(proto)) {
      uint8_t mosiGpio, sckGpio;
      
      // Check if manual GPIO configuration is enabled
      bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;
      
      if (gpioManualConfig) {
        // Use ETS configured GPIO pins
        mosiGpio = (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO;
        sckGpio = (uint8_t)ParamNEOSTRIP_NEOClockGPIO;
      } else {
        // Use predefined GPIO pins based on strip index and hardware platform
        // Default SPI configurations from hardware definitions
        switch (i) {
          case 0: // Strip 1 - Default SPI configuration
            #if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
              sckGpio = 2;   // KNXIAO_RP2040_PIN6 (GPIO2)
              mosiGpio = 3;  // KNXIAO_RP2040_PIN8 (GPIO3) 
            #elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
              sckGpio = 2;   // KNXIAO_ESP32S3_PIN1 (GPIO2)
              mosiGpio = 3;  // KNXIAO_ESP32S3_PIN2 (GPIO3)
            #else
              sckGpio = 2;   // Fallback
              mosiGpio = 3;
            #endif
            break;
          case 1: // Strip 2 - Second SPI configuration  
            #if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
              sckGpio = 6;   // KNXIAO_RP2040_PIN4 (GPIO6)
              mosiGpio = 7;  // KNXIAO_RP2040_PIN5 (GPIO7)
            #elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
              sckGpio = 4;   // KNXIAO_ESP32S3_PIN3 (GPIO4)
              mosiGpio = 5;  // KNXIAO_ESP32S3_PIN4 (GPIO5)
            #else
              sckGpio = 6;   // Fallback
              mosiGpio = 7;
            #endif
            break;
          case 2: // Strip 3 - Third SPI configuration
            #if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
              sckGpio = 27;  // KNXIAO_RP2040_PIN1 (GPIO27)
              mosiGpio = 28; // KNXIAO_RP2040_PIN2 (GPIO28)
            #elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
              sckGpio = 6;   // KNXIAO_ESP32S3_PIN5 (GPIO6)
              mosiGpio = 7;  // KNXIAO_ESP32S3_PIN6 (GPIO7)
            #else
              sckGpio = 18;  // Fallback
              mosiGpio = 19;
            #endif
            break;
          default: // Strip 4+ - Use fallback values
            sckGpio = 18;
            mosiGpio = 19;
            break;
        }
      }
      
      phys = neoPixelModule.addSpiStrip(mosiGpio, sckGpio, pixels, proto, order);
      logInfoP("SPI Strip %d: %d LEDs, MOSI=%d, SCK=%d, Protocol=%s, ColorOrder=%s%s", 
              i, pixels, mosiGpio, sckGpio, getProtocolName(proto), getColorOrderName(order),
              gpioManualConfig ? " (Manual)" : " (Default)");
    } else {
      // 1-Wire protocols use Data GPIO
      uint8_t dataGpioPin;
      
      // Check if manual GPIO configuration is enabled
      bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;
      
      if (gpioManualConfig) {
        // Use ETS configured GPIO pin
        dataGpioPin = dataGpio;
      } else {
        // Use predefined GPIO pins based on strip index and hardware platform
        // D1-D8 pins for physical strips 0-7
        switch (i) {
          case 0: // Strip 1 - D1
            #if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
              dataGpioPin = 27;  // D1: KNXIAO_RP2040_PIN1 (GPIO27)
            #elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
              dataGpioPin = 2;   // D1: KNXIAO_ESP32S3_PIN1 (GPIO2)
            #else
              dataGpioPin = 27;  // Fallback
            #endif
            break;
          case 1: // Strip 2 - D2
            #if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
              dataGpioPin = 28;  // D2: KNXIAO_RP2040_PIN2 (GPIO28)
            #elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
              dataGpioPin = 3;   // D2: KNXIAO_ESP32S3_PIN2 (GPIO3)
            #else
              dataGpioPin = 28;  // Fallback
            #endif
            break;
          case 2: // Strip 3 - D3
            #if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
              dataGpioPin = 29;  // D3: KNXIAO_RP2040_PIN3 (GPIO29)
            #elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
              dataGpioPin = 4;   // D3: KNXIAO_ESP32S3_PIN3 (GPIO4)
            #else
              dataGpioPin = 29;  // Fallback
            #endif
            break;
          case 3: // Strip 4 - D4
            #if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
              dataGpioPin = 6;   // D4: KNXIAO_RP2040_PIN4 (GPIO6)
            #elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
              dataGpioPin = 5;   // D4: KNXIAO_ESP32S3_PIN4 (GPIO5)
            #else
              dataGpioPin = 6;   // Fallback
            #endif
            break;
          case 4: // Strip 5 - D5
            #if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
              dataGpioPin = 7;   // D5: KNXIAO_RP2040_PIN5 (GPIO7)
            #elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
              dataGpioPin = 6;   // D5: KNXIAO_ESP32S3_PIN5 (GPIO6)
            #else
              dataGpioPin = 7;   // Fallback
            #endif
            break;
          case 5: // Strip 6 - D6
            #if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
              dataGpioPin = 2;   // D6: KNXIAO_RP2040_PIN6 (GPIO2)
            #elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
              dataGpioPin = 7;   // D6: KNXIAO_ESP32S3_PIN6 (GPIO7)
            #else
              dataGpioPin = 2;   // Fallback
            #endif
            break;
          case 6: // Strip 7 - D7
            #if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
              dataGpioPin = 4;   // D7: KNXIAO_RP2040_PIN7 (GPIO4)
            #elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
              dataGpioPin = 8;   // D7: KNXIAO_ESP32S3_PIN8 (GPIO9)
            #else
              dataGpioPin = 4;   // Fallback
            #endif
            break;
          case 7: // Strip 8 - D8
            #if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
              dataGpioPin = 3;   // D8: KNXIAO_RP2040_PIN8 (GPIO3)
            #elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
              dataGpioPin = 9;   // D8: KNXIAO_ESP32S3_PIN8 (GPIO9)
            #else
              dataGpioPin = 3;   // Fallback
            #endif
            break;
          default: // More than 8 strips - use fallback
            dataGpioPin = dataGpio; // Use ETS parameter as fallback
            break;
        }
      }
      
      phys = neoPixelModule.addStrip(dataGpioPin, pixels, proto, order);
      logInfoP("1-Wire Strip %d: %d LEDs, GPIO=%d, Protocol=%s, ColorOrder=%s%s", 
              i, pixels, dataGpioPin, getProtocolName(proto), getColorOrderName(order),
              gpioManualConfig ? " (Manual)" : " (Default)");
    }

    if (phys) {
      _totalLeds += pixels;
      _physicalStrips.push_back(phys);

      // Configure color correction for this strip
      if ((bool)ParamNEOSTRIP_NEOGammaCorrection || (bool)ParamNEOSTRIP_NEOWhiteBalanceCorrection) {
        configureColorCorrection();
        updateColorCorrection(); // Set correction parameters on VirtualStrip once
        logInfoP("Strip %d: Color correction enabled - Gamma=%s(%.1f), WhiteBalance=%s(R:%d G:%d B:%d)", 
                i, 
                _gammaCorrectionEnabled ? "ON" : "OFF", _gammaValue,
                _whiteBalanceEnabled ? "ON" : "OFF", _whiteBalanceRed, _whiteBalanceGreen, _whiteBalanceBlue);
      }
      
      // Configure strip options (swap mode and skip LEDs)
      configureStripOptions();
      if (_swapMode > 0 || _skipFirstLeds > 0) {
        logInfoP("Strip %d: Strip options - SwapMode=%d, SkipFirstLEDs=%d", 
                i, _swapMode, _skipFirstLeds);
      }
    } else {
      logErrorP("Failed to create strip %d", i);
    }
  }
  
  logInfoP("Created %d physical strips with total %d LEDs", maxStrips, _totalLeds);

  // 3) Configure virtual strip order from ETS parameters
  configureVirtualStripOrder();

  // 4) Configure segments once, now that we know the virtual strip layout
  // Segments are defined on the virtual strip
  if (_totalLeds > 0) {
    configureSegments();
    if (_numberOfSegments > 0) {
      logInfoP("Configured %d segments for virtual strip (%d total LEDs)", _numberOfSegments, _totalLeds);
    }
  }

  // 5) Create virtual strip using configured order and apply segments
  if (!_physicalStrips.empty() && _totalLeds > 0) {
    createVirtualStripWithOrder();
    
    // Configure effects for segments (now that virtual strip and segments exist)
    if (_numberOfSegments > 0) {
      configureEffects();
      logInfoP("Applied effects to %d segments", _numberOfSegments);
    }
    
    // Configure power management using OFM PowerManager
    configurePowerManagement();
    
    // Apply initial color correction to ensure ETS settings take effect immediately
    if (_gammaCorrectionEnabled || _whiteBalanceEnabled || _swapMode > 0) {
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

LedProtocol NeoPixelBusModule::mapProtocol(uint8_t p)
{
  // Map ETS LED type enum to OFM's LedProtocol enum
  // Based on NEOLedType enumeration from NeoPixelBus.share.xml
  switch (p)
  {
    case 0:   return LedProtocol::WS2812B;  // WS2812B
    case 1:   return LedProtocol::WS2805;   // WS2805
    case 2:   return LedProtocol::WS2811;   // WS2811
    case 3:   return LedProtocol::WS2813;   // WS2813
    case 4:   return LedProtocol::SK6812;   // SK6812
    case 5:   return LedProtocol::APA102;   // APA102
    case 6:   return LedProtocol::SK9822;   // SK9822
    case 7:   return LedProtocol::WS2812B;  // WS281x (mapped to WS2812B)
    case 8:   return LedProtocol::SK6812;   // SK6812/WS2814 (RGBW)
    case 9:   return LedProtocol::TM1814;   // TM1814
    case 10:  return LedProtocol::WS2811;   // WS2812_400kHz (mapped to WS2811)
    case 21:  return LedProtocol::WS2801;   // WS2801
    case 22:  return LedProtocol::LPD8806;  // LPD8806
    default:  return LedProtocol::WS2812B;  // Default to most common
  }
}

ColorOrder NeoPixelBusModule::mapColorOrder(uint8_t c)
{
  // Map ETS ColorOrder enum to OFM's ColorOrder enum
  // Based on NEOColourOrder enumeration from NeoPixelBus.share.xml
  switch (c)
  {
    case 0:  return ColorOrder::GRB;   // GRB (WS2812/SK6812 standard)
    case 1:  return ColorOrder::RGB;   // RGB (standard RGB order)
    case 2:  return ColorOrder::BRG;   // BRG (rare configuration)
    case 3:  return ColorOrder::RBG;   // RBG (some LED clones)
    case 4:  return ColorOrder::BGR;   // BGR (APA102/SK9822 standard)
    case 5:  return ColorOrder::GBR;   // GBR (some WS2812B clones)
    case 6:  return ColorOrder::RGBW;  // RGBW (4-channel, RGB+White)
    case 7:  return ColorOrder::GRBW;  // GRBW (4-channel, SK6812 standard)
    default: return ColorOrder::GRB;   // Default to WS2812/SK6812 standard
  }
}

void NeoPixelBusModule::configureVirtualStripOrder()
{
  _virtualStripConfiguration.clear();
  
  // Check if we have duplicate configuration (user may have made mistakes)
  bool hasDuplicates = ParamNEO_VirtualStripHasDuplicates;
  if (hasDuplicates) {
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
    static_cast<uint8_t>(ParamNEO_VirtualStripPos6)
  };
  
  // Read the start positions (calculated by ETS JavaScript)
  uint16_t startPositions[6] = {
    static_cast<uint16_t>(ParamNEO_VirtualStripStart1),
    static_cast<uint16_t>(ParamNEO_VirtualStripStart2),
    static_cast<uint16_t>(ParamNEO_VirtualStripStart3),
    static_cast<uint16_t>(ParamNEO_VirtualStripStart4),
    static_cast<uint16_t>(ParamNEO_VirtualStripStart5),
    static_cast<uint16_t>(ParamNEO_VirtualStripStart6)
  };
  
  // Build the virtual strip configuration based on positions
  for (uint8_t pos = 0; pos < 6; pos++) {
    uint8_t physStripIndex = positions[pos];
    
    // Skip unused positions (value 0) or invalid indices
    if (physStripIndex == 0 || physStripIndex > _physicalStrips.size()) {
      continue;
    }
    
    // Convert from 1-based to 0-based indexing
    physStripIndex -= 1;
    
    // Get the LED count for this physical strip
    if (physStripIndex < _physicalStrips.size() && _physicalStrips[physStripIndex]) {
      uint16_t ledCount = _physicalStrips[physStripIndex]->getLedCount();
      uint16_t virtualStart = startPositions[pos];
      
      // Convert from 1-based (ETS user-friendly) to 0-based (internal indexing)
      // ETS shows "Start LED = 1" but internally we need offset 0
      uint16_t virtualStartZeroBased = (virtualStart > 0) ? (virtualStart - 1) : 0;
      
      _virtualStripConfiguration.emplace_back(physStripIndex, virtualStartZeroBased, ledCount);
      
      logInfoP("Virtual position %d: Physical strip %d (%d LEDs) starts at virtual position %d (ETS: %d)", 
               pos + 1, physStripIndex + 1, ledCount, virtualStartZeroBased, virtualStart);
    } else {
      logErrorP("Invalid physical strip index %d at virtual position %d", physStripIndex + 1, pos + 1);
    }
  }
  
  // Validate the configuration
  if (_virtualStripConfiguration.empty()) {
    logWarningP("No valid virtual strip configuration found - using default order");
    createDefaultVirtualStripOrder();
  } else {
    logInfoP("Virtual strip configuration complete: %d physical strips mapped", 
             static_cast<int>(_virtualStripConfiguration.size()));
  }
}

void NeoPixelBusModule::createVirtualStripWithOrder()
{
  if (_physicalStrips.empty() || _totalLeds == 0 || _virtualStripConfiguration.empty()) {
    logErrorP("Cannot create virtual strip: insufficient configuration");
    return;
  }

  // Create virtual strip (always RGB internally, regardless of hardware ColorOrders)
  _virtualStrip = neoPixelModule.addVirtualStrip(_totalLeds, ColorOrder::RGB);
  if (!_virtualStrip) {
    logErrorP("Failed to create virtual strip");
    return;
  }

  auto mgr = neoPixelModule.getManager();
  if (!mgr) {
    logErrorP("NeoPixelManager not available for virtual strip creation");
    return;
  }

  // Attach physical strips to virtual strip according to the configured order
  for (const auto& config : _virtualStripConfiguration) {
    if (config.physicalStripIndex >= _physicalStrips.size()) {
      logErrorP("Invalid physical strip index %d in virtual configuration", config.physicalStripIndex);
      continue;
    }
    
    PhysicalStrip* phys = _physicalStrips[config.physicalStripIndex];
    if (!phys) {
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
    if (skipLeds > 0 && skipLeds < effectiveLeds) {
      logDebugP("Strip %d: Skipping first %d LEDs, effective range: %d-%d", 
               config.physicalStripIndex, skipLeds, skipLeds, effectiveLeds - 1);
    }
    
    if (mgr->attachPhysicalToVirtual(_virtualStrip, phys, attachOffset)) {
      logInfoP("Attached Physical Strip %d to Virtual Strip at position %d (length %d, skip %d)", 
              config.physicalStripIndex, attachOffset, effectiveLeds, skipLeds);
    } else {
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
  if (_numberOfSegments > 0) {
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
  for (size_t i = 0; i < _physicalStrips.size(); i++) {
    if (_physicalStrips[i]) {
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
  switch (protocol) {
    case LedProtocol::APA102:
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
  switch (order) {
    case ColorOrder::RGB:  return "RGB";
    case ColorOrder::RBG:  return "RBG";
    case ColorOrder::GRB:  return "GRB";
    case ColorOrder::GBR:  return "GBR";
    case ColorOrder::BGR:  return "BGR";
    case ColorOrder::BRG:  return "BRG";
    case ColorOrder::RGBW: return "RGBW";
    case ColorOrder::GRBW: return "GRBW";
    default: return "UNKNOWN";
  }
}

const char* NeoPixelBusModule::getProtocolName(LedProtocol protocol)
{
  switch (protocol) {
    case LedProtocol::WS2805:  return "WS2805";
    case LedProtocol::WS2812:  return "WS2812";
    case LedProtocol::WS2812B: return "WS2812B";
    case LedProtocol::WS2813:  return "WS2813";
    case LedProtocol::WS2815:  return "WS2815";
    case LedProtocol::WS2811:  return "WS2811";
    case LedProtocol::SK6812:  return "SK6812";
    case LedProtocol::SK6805:  return "SK6805";
    case LedProtocol::WS2814:  return "WS2814";
    case LedProtocol::TM1814:  return "TM1814";
    case LedProtocol::GS8208:  return "GS8208";
    case LedProtocol::APA102:  return "APA102";
    case LedProtocol::SK9822:  return "SK9822";
    case LedProtocol::WS2801:  return "WS2801";
    case LedProtocol::LPD8806: return "LPD8806";
    default: return "UNKNOWN";
  }
}

// ============================================================================
// Power Management Implementation 
// ============================================================================

void NeoPixelBusModule::configurePowerManagement()
{
  // Read from the first strip's parameters (power limiting is usually global)
  uint8_t oldChannelIndex = _channelIndex;
  _channelIndex = 0; // Use first strip for global power settings
  
  // Check if power limiting is enabled in ETS
  bool powerLimitEnabled = (bool)ParamNEOSTRIP_NEOpowerLimitEnabled;
  
  // Restore channel index
  _channelIndex = oldChannelIndex;
  
  auto mgr = neoPixelModule.getManager();
  if (!mgr) {
    logErrorP("NeoPixelManager not available for power management configuration");
    return;
  }
  
  auto* powerManager = mgr->getPowerManager();
  if (!powerManager) {
    logErrorP("PowerManager not available");
    return;
  }
  
  // Configure power management in OFM
  powerManager->setEnabled(powerLimitEnabled);
  
  if (powerLimitEnabled) {
    // Read ETS parameters from first strip (power management is typically global)
    _channelIndex = 0;
    uint16_t powerLimitMa = (uint16_t)ParamNEOSTRIP_NEOpowerLimitGlobal;
    uint8_t currentPerLed = (uint8_t)ParamNEOSTRIP_NEOcurrentPerLED;
    _channelIndex = oldChannelIndex;
    
    // Set maximum current limit
    powerManager->setMaxCurrent(powerLimitMa);
    
    // Configure LED current profile based on ETS settings
    LedCurrentProfile profile;
    if (currentPerLed > 0) {
      // User specified custom current per LED - assume equal RGB distribution
      uint16_t currentPerChannel = currentPerLed / 3; // RGB split
      profile = LedCurrentProfile(currentPerChannel, currentPerChannel, currentPerChannel, 0);
    } else {
      // Use default profile based on most common LED type
      profile = LedProfiles::WS2812B; // Conservative default
    }
    
    powerManager->setLedProfile(profile);
    
    logInfoP("Power Management: ENABLED - MaxCurrent=%dmA, LEDProfile=R:%dmA G:%dmA B:%dmA W:%dmA", 
             powerLimitMa, profile.redMA, profile.greenMA, profile.blueMA, profile.whiteMA);
  } else {
    logInfoP("Power Management: DISABLED");
  }
}

// ============================================================================
// Global Brightness Control Implementation
// ============================================================================

void NeoPixelBusModule::applyGlobalBrightness(uint8_t brightness)
{
  // Special case: if brightness is full (255), restore original brightness levels
  if (brightness == 255) {
    restoreOriginalBrightness();
    return;
  }
  
  _globalBrightness = brightness;
  
  logInfoP("Applying global brightness: %d%% to all segments", (brightness * 100) / 255);
  
  // Initialize original brightness storage if needed
  if (_originalBrightness.size() != _segments.size()) {
    _originalBrightness.resize(_segments.size());
    
    // Store current brightness levels as original if not done yet
    for (size_t i = 0; i < _segments.size(); i++) {
      if (_segments[i].segment) {
        _originalBrightness[i] = _segments[i].segment->getBrightness();
        logDebugP("Stored original brightness for segment %zu: %d%%", 
                  i, (_originalBrightness[i] * 100) / 255);
      }
    }
  }
  
  // Apply brightness to all configured segments
  for (size_t i = 0; i < _segments.size(); i++) {
    if (_segments[i].segment) {
      // Use stored original brightness, not current (to avoid cumulative effects)
      uint8_t originalBrightness = _originalBrightness[i];
      uint8_t effectiveBrightness = (originalBrightness * brightness) / 255;
      
      // Apply effective brightness to segment
      _segments[i].segment->setBrightness(effectiveBrightness);
      
      logDebugP("Segment %zu brightness: original=%d%%, global=%d%%, effective=%d%%",
                i,
                (originalBrightness * 100) / 255,
                (brightness * 100) / 255, 
                (effectiveBrightness * 100) / 255);
    }
  }
  
  // Also apply to physical strips that support hardware brightness
  for (auto* strip : _physicalStrips) {
    if (strip && strip->supportsHardwareBrightness()) {
      strip->setHardwareBrightness(brightness);
      logDebugP("Applied hardware brightness %d%% to physical strip", (brightness * 100) / 255);
    }
  }
  
  // Trigger immediate update to apply changes
  neoPixelModule.updateAll();
  
  logInfoP("Global brightness %d%% applied to %d segments and %d physical strips", 
           (brightness * 100) / 255, (int)_segments.size(), (int)_physicalStrips.size());
}

void NeoPixelBusModule::restoreOriginalBrightness()
{
  logInfoP("Restoring original brightness levels for all segments");
  
  // Restore original brightness levels for all segments
  for (size_t i = 0; i < _segments.size() && i < _originalBrightness.size(); i++) {
    if (_segments[i].segment) {
      uint8_t originalBrightness = _originalBrightness[i];
      _segments[i].segment->setBrightness(originalBrightness);
      
      logDebugP("Restored segment %zu brightness to original: %d%%",
                i, (originalBrightness * 100) / 255);
    }
  }
  
  // Reset global brightness to full
  _globalBrightness = 255;
  
  // Also reset hardware brightness on physical strips
  for (auto* strip : _physicalStrips) {
    if (strip && strip->supportsHardwareBrightness()) {
      strip->setHardwareBrightness(255);
      logDebugP("Reset hardware brightness to 100%% on physical strip");
    }
  }
  
  // Trigger update to apply changes
  neoPixelModule.updateAll();
  
  logInfoP("Original brightness levels restored for all %d segments", (int)_segments.size());
}

// ============================================================================
// HCL Color Temperature Control Implementation  
// ============================================================================

void NeoPixelBusModule::applyHclColorTemperature(uint16_t kelvin)
{
  _currentHclTemperature = kelvin;
  
  // Validate Kelvin range (typical LED range: 2000K-10000K)
  if (kelvin < 1000 || kelvin > 15000) {
    logWarningP("HCL temperature %dK out of typical range (2000K-10000K)", kelvin);
    return;
  }
  
  logInfoP("Applying HCL color temperature: %dK to all segments", kelvin);
  
  // Convert Kelvin to RGB using ColorHelper
  uint8_t r, g, b;
  ColorHelper::kelvinToRGB(kelvin, r, g, b);
  
  logDebugP("HCL %dK converts to RGB: R=%d, G=%d, B=%d", kelvin, r, g, b);
  
  // Store original colors if this is the first HCL application
  if (!_hclModeEnabled && _originalColors.size() != _segments.size()) {
    _originalColors.resize(_segments.size());
    
    for (size_t i = 0; i < _segments.size(); i++) {
      if (_segments[i].segment) {
        uint8_t origR, origG, origB;
        if (_segments[i].segment->getPixel(0, origR, origG, origB)) {
          _originalColors[i] = {origR, origG, origB};
          logDebugP("Stored original color for segment %zu: R=%d, G=%d, B=%d", 
                    i, origR, origG, origB);
        } else {
          _originalColors[i] = {255, 255, 255}; // Default to white if can't read
        }
      }
    }
  }
  
  // Apply color temperature to all segments
  uint16_t segmentsUpdated = 0;
  for (auto& segmentConfig : _segments) {
    if (segmentConfig.segment) {
      // Apply the Kelvin-derived RGB to the entire segment
      segmentConfig.segment->setAll(r, g, b);
      segmentsUpdated++;
      
      logDebugP("Applied %dK color temperature to segment", kelvin);
    }
  }
  
  _hclModeEnabled = true;
  
  // Trigger immediate update
  neoPixelModule.updateAll();
  
  logInfoP("HCL color temperature %dK applied to %d segments", kelvin, segmentsUpdated);
}

void NeoPixelBusModule::disableHclMode()
{
  if (!_hclModeEnabled) {
    logDebugP("HCL mode already disabled");
    return;
  }
  
  logInfoP("Disabling HCL mode and restoring original colors");
  
  // Restore original colors for all segments
  for (size_t i = 0; i < _segments.size() && i < _originalColors.size(); i++) {
    if (_segments[i].segment) {
      auto& color = _originalColors[i];
      _segments[i].segment->setAll(color[0], color[1], color[2]);
      
      logDebugP("Restored segment %zu color to original: R=%d, G=%d, B=%d",
                i, color[0], color[1], color[2]);
    }
  }
  
  _hclModeEnabled = false;
  _currentHclTemperature = 6500; // Reset to neutral daylight
  
  // Trigger update to apply changes
  neoPixelModule.updateAll();
  
  logInfoP("HCL mode disabled, original colors restored for all %d segments", (int)_segments.size());
}

// ============================================================================
// Color Correction Implementation
// ============================================================================

// Configure color correction from ETS parameters
void NeoPixelBusModule::configureColorCorrection()
{
  // Gamma correction configuration
  _gammaCorrectionEnabled = (bool)ParamNEOSTRIP_NEOGammaCorrection;
  if (_gammaCorrectionEnabled) {
    uint8_t gammaParam = ParamNEOSTRIP_NEOGammaValue;
    _gammaValue = mapGammaValue(gammaParam);
    logInfoP("Gamma correction configured: value=%.2f", _gammaValue);
  }
  
  // White balance configuration  
  _whiteBalanceEnabled = (bool)ParamNEOSTRIP_NEOWhiteBalanceCorrection;
  if (_whiteBalanceEnabled) {
    _whiteBalanceRed = mapWhiteBalanceValue(ParamNEOSTRIP_NEOWhiteBalanceRed);
    _whiteBalanceGreen = mapWhiteBalanceValue(ParamNEOSTRIP_NEOWhiteBalanceGreen);  
    _whiteBalanceBlue = mapWhiteBalanceValue(ParamNEOSTRIP_NEOWhiteBalanceBlue);
    logInfoP("White balance configured: R:%d G:%d B:%d", _whiteBalanceRed, _whiteBalanceGreen, _whiteBalanceBlue);
  }
}

void NeoPixelBusModule::updateColorCorrection()
{
  if (!_initialized) return;
  // Update color correction parameters on VirtualStrip
  // (These are applied during rendering, NOT in-place!)
  // NOTE: setColorCorrection removed from VirtualStrip - color correction deactivated for now
  /*
  if (_virtualStrip) {
    _virtualStrip->setColorCorrection(
      _gammaCorrectionEnabled, _gammaValue,
      _whiteBalanceEnabled, _whiteBalanceRed, _whiteBalanceGreen, _whiteBalanceBlue,
      _swapMode
    );
  }
  */
}

void NeoPixelBusModule::forceColorCorrectionUpdate()
{
  if (!_initialized) return;
  
  // Force immediate update bypassing rate limiting
  updateColorCorrection();
  _lastColorUpdateMs = millis(); // Update timestamp to reset rate limiting
  
  logDebugP("Forced color correction update");
}

float NeoPixelBusModule::mapGammaValue(uint8_t paramValue)
{
  // Map ETS parameter (0-10) to gamma value range (2.0-3.0)
  // ETS XML configuration:
  // Value 0 -> 2.0, Value 1 -> 2.1, ..., Value 10 -> 3.0
  // Each step represents 0.1 gamma increment
  
  // Clamp parameter value to valid range
  if (paramValue > 10) {
    paramValue = 10;
  }
  
  // Convert: gamma = 2.0 + (paramValue * 0.1)
  float gammaValue = 2.0f + (paramValue * 0.1f);
  
  return gammaValue;
}

uint8_t NeoPixelBusModule::mapWhiteBalanceValue(uint8_t paramValue)
{
  // ETS parameter is percentage (0-100), need to map to RGB scaling range (0-255)
  // ETS XML configuration: PT-NEOPercent with range 0-100
  // Convert: rgbValue = (percentage * 255) / 100
  
  // Clamp parameter value to valid range
  if (paramValue > 100) {
    paramValue = 100;
  }
  
  // Convert percentage to 0-255 range for RGB channel scaling
  uint8_t rgbValue = (paramValue * 255) / 100;
  
  return rgbValue;
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
  
  // Skip first LEDs configuration
  _skipFirstLeds = ParamNEOSTRIP_NEOSkipFirstLEDs;
  if (_skipFirstLeds > 0) {
    configureSkipLeds();
    logInfoP("Skip first LEDs configured: %d LEDs", _skipFirstLeds);
  }
}

// Color channel swapping is now applied during rendering (in syncToPhysical)
// NOT in-place on VirtualStrip buffer!
void NeoPixelBusModule::applySwapMode()
{
  // This function is now a NO-OP placeholder
  // Color swapping happens in VirtualStrip::syncToPhysical()
  // to preserve original color values in the buffer
  
  // The swap modes are applied during rendering:
  // Mode 1: R<->G swap
  // Mode 2: R<->B swap
  // Mode 3: G<->B swap
  // Mode 4: R<->G<->B rotate right (R->B, G->R, B->G)
  // Mode 5: R<->G<->B rotate left (R->G, G->B, B->R)
}

// Configure LED skipping offset
void NeoPixelBusModule::configureSkipLeds()
{
  // Skip LEDs implementation:
  // This affects how pixels are mapped from VirtualStrip to PhysicalStrips
  // When skip is applied, the first N LEDs of each PhysicalStrip are not used
  
  if (_skipFirstLeds > 0 && _virtualStrip) {
    // Clear the skipped LEDs to black to ensure they're off
    for (uint16_t stripIdx = 0; stripIdx < _physicalStrips.size(); stripIdx++) {
      auto* phys = _physicalStrips[stripIdx];
      if (phys && _skipFirstLeds < phys->getLedCount()) {
        // Clear the first N LEDs on each physical strip
        for (uint16_t led = 0; led < _skipFirstLeds; led++) {
          phys->setPixel(led, 0, 0, 0); // Set to black
        }
        logDebugP("Strip %d: Cleared first %d LEDs (skip offset)", stripIdx, _skipFirstLeds);
      }
    }
  }
  
  logDebugP("Skip LEDs configuration applied: %d LEDs", _skipFirstLeds);
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
  
  if (_numberOfSegments == 0) {
    logDebugP("No segments configured");
    return;
  }
  
  // Clear existing segments
  _segments.clear();
  _segments.reserve(_numberOfSegments);
  
  // Configure each segment from ETS parameters
  for (uint8_t i = 0; i < _numberOfSegments; i++) {
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
  if (config.startLed > config.endLed) {
    logErrorP("Segment %d: Invalid range - Start(%d) > End(%d)", 
              segmentIndex, config.startLed, config.endLed);
    // Swap them
    uint16_t temp = config.startLed;
    config.startLed = config.endLed;
    config.endLed = temp;
  }
  
  // Ensure segment fits within total LED count
  if (config.endLed >= _totalLeds) {
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
  if (!_virtualStrip || _segments.empty()) {
    return;
  }
  
  auto mgr = neoPixelModule.getManager();
  if (!mgr) {
    logErrorP("NeoPixelManager not available for segment creation");
    return;
  }
  
  // Create actual segments
  for (size_t i = 0; i < _segments.size(); i++) {
    SegmentConfig& config = _segments[i];
    
    // Calculate effective start/end considering offset
    uint16_t effectiveStart = config.startLed + config.offset;
    uint16_t effectiveEnd = config.endLed + config.offset;
    
    // Ensure bounds are within virtual strip
    if (effectiveEnd >= _virtualStrip->getLedCount()) {
      effectiveEnd = _virtualStrip->getLedCount() - 1;
      logWarningP("Segment %zu: Adjusted end to %d (within virtual strip bounds)", i, effectiveEnd);
    }
    
    if (effectiveStart <= effectiveEnd) {
      config.segment = mgr->addSegment(_virtualStrip, effectiveStart, effectiveEnd);
      
      if (config.segment) {
        logInfoP("Created segment %zu: LEDs %d-%d (length %d)",
                 i, effectiveStart, effectiveEnd, effectiveEnd - effectiveStart + 1);
      } else {
        logErrorP("Failed to create segment %zu", i);
      }
    } else {
      logErrorP("Segment %zu: Invalid effective range %d-%d", i, effectiveStart, effectiveEnd);
    }
  }
}

// Apply segment-specific configuration
void NeoPixelBusModule::applySegmentConfiguration()
{
  for (size_t i = 0; i < _segments.size(); i++) {
    SegmentConfig& config = _segments[i];
    if (!config.segment) continue;
    
    // Apply grouping and spacing by setting specific pixels
    if (config.grouping > 1 || config.spacing > 0) {
      applyGroupingAndSpacing(config);
    }
    
    // Note: Reverse direction and mirror effect would be applied
    // when effects are running, not in static configuration
    
    logDebugP("Applied configuration to segment %zu", i);
  }
}

// Apply grouping and spacing to segment (helper function)
void NeoPixelBusModule::applyGroupingAndSpacing(SegmentConfig& config)
{
  if (!config.segment) return;
  
  // Grouping and spacing implementation:
  // - grouping: how many LEDs are treated as one unit
  // - spacing: how many LEDs to skip between groups
  
  uint16_t segmentLength = config.segment->getLength();
  uint16_t groupSize = config.grouping > 0 ? config.grouping : 1;
  uint16_t spacing = config.spacing;
  
  if (groupSize == 1 && spacing == 0) {
    return; // No grouping/spacing needed
  }
  
  // Clear the segment first
  config.segment->clear();
  
  // Apply grouping pattern by setting pixels in groups
  for (uint16_t pos = 0; pos < segmentLength; pos += (groupSize + spacing)) {
    // Set pixels in current group
    for (uint16_t g = 0; g < groupSize && (pos + g) < segmentLength; g++) {
      // This would normally be set by effects, but we mark the pattern
      // by setting a dim white color to show the grouping pattern
      config.segment->setPixel(pos + g, 32, 32, 32); // Dim white for visualization
    }
    // Skip 'spacing' pixels (they remain black)
  }
  
  logDebugP("Applied grouping=%d, spacing=%d to segment (length %d)",
            groupSize, spacing, segmentLength);
}

// Get segment by index
Segment* NeoPixelBusModule::getSegment(uint8_t index) const
{
  if (index < _segments.size()) {
    return _segments[index].segment;
  }
  return nullptr;
}

// =============================================================================
// Effects Implementation
// =============================================================================

void NeoPixelBusModule::configureEffects()
{
  // Check if any segments have effects configured
  bool hasEffects = false;
  
  for (size_t i = 0; i < _segments.size(); ++i) {
    if (_segments[i].segment) {
      _channelIndex = i;  // Set channel context for parameter access
      
      uint8_t effectType = ParamNEO_NEONEOEffectType;
      if (effectType > 0) { // 0 = No Effect
        hasEffects = true;
        applyEffectToSegment(_segments[i].segment, effectType);
        setupEffectConfiguration(_segments[i].segment);
        
        logInfoP("Segment %zu: Applied effect type %d", i, effectType);
      }
    }
  }
  
  // Read global UpdateSpeed parameter
  uint8_t updateSpeedParam = ParamNEO_NEOUpdateSpeed;
  UpdateSpeed speed = UpdateSpeed::NORMAL;
  const char* speedName = "Normal";
  
  // Safely map parameter value to UpdateSpeed enum
  switch (updateSpeedParam) {
    case 100: 
      speed = UpdateSpeed::SLOW; 
      speedName = "Slow (10 FPS)";
      break;
    case 50:  
      speed = UpdateSpeed::NORMAL; 
      speedName = "Normal (20 FPS)";
      break;
    case 33:  
      speed = UpdateSpeed::FAST; 
      speedName = "Fast (30 FPS)";
      break;
    case 20:  
      speed = UpdateSpeed::MAX; 
      speedName = "Max (50 FPS)";
      break;
    case 12:  
      speed = UpdateSpeed::EXTREME; 
      speedName = "Extreme (80 FPS)";
      break;
    case 4:   
      speed = UpdateSpeed::LUDICROUS; 
      speedName = "Ludicrous (120 FPS)";
      break;
    case 0:   
      speed = UpdateSpeed::FTL; 
      speedName = "FTL (240 FPS)";
      break;
    default:
      logInfoP("Invalid UpdateSpeed parameter %d, using Normal", updateSpeedParam);
      speed = UpdateSpeed::NORMAL;
      speedName = "Normal (20 FPS) [fallback]";
      break;
  }
  
  // ALWAYS enable auto-update (even without effects) so KO color changes are rendered
  neoPixelModule.setAutoUpdate(true);
  neoPixelModule.setUpdateSpeed(speed);
  _autoUpdateEnabled = true;
  
  if (hasEffects) {
    _effectsEnabled = true;
    logInfoP("Effects system enabled - Auto-update: %s", speedName);
  } else {
    logInfoP("No effects configured - Auto-update enabled for KO control: %s", speedName);
  }
}

void NeoPixelBusModule::applyEffectToSegment(Segment* segment, uint8_t effectType)
{
  if (!segment) return;
  
  Effect* effect = getEffectFromType(effectType);
  if (effect) {
    segment->setEffect(effect);
    logInfoP("Applied effect '%s' to segment", effect->getName());
  } else {
    logWarningP("Unknown effect type: %d", effectType);
  }
}

Effect* NeoPixelBusModule::getEffectFromType(uint8_t effectType)
{
  // Map effect type ID to actual effect instances
  // Based on the console effect list:
  switch (effectType) {
    case 0: return nullptr;                      // No Effect
    case 1: return EffectPool::getSolid();       // Solid Color
    case 2: return EffectPool::getWipe();        // Color Wipe
    case 3: return EffectPool::getRainbow();     // Rainbow
    case 4: return EffectPool::getPride();       // Pride2015
    case 5: return EffectPool::getConfetti();    // Confetti
    case 6: return EffectPool::getJuggle();      // Juggle
    case 7: return EffectPool::getBPM();         // BPM
    case 8: return EffectPool::getCylon();       // Cylon
    case 9: return EffectPool::getRGBWTest();    // SK6812/RGBW Test
    case 10: return EffectPool::getGarageDoor(); // GarageDoor
    default: return nullptr;
  }
}

void NeoPixelBusModule::setupEffectConfiguration(Segment* segment)
{
  if (!segment) return;
  
  // Get ETS effect parameters for current channel
  auto& config = segment->getConfig();
  
  // Basic effect parameters from ETS
  config.speed = ParamNEO_NEOEffectSpeed;           // Effect speed (0-255)
  config.intensity = ParamNEO_NEOEffectIntensity;   // Effect intensity (0-255)
  
  // Extended effect parameters
  config.option1 = ParamNEO_NEOEffectOption1;       // Custom option 1
  config.option2 = ParamNEO_NEOEffectOption2;       // Custom option 2
  config.mode = ParamNEO_NEOEffectOption3;          // Effect mode
  
  // Effect features (boolean flags)
  config.reverse = ParamNEO_NEOEffectFeature1 ? 1 : 0;  // Reverse direction
  // Feature2 and Feature3 could be used for other boolean options
  
  // Mirror effect (from segment configuration)
  bool mirrorEffect = ParamNEO_NEOSegmentMirrorEffect;
  if (mirrorEffect) {
    // Note: Mirror effect would need to be implemented in the segment update logic
    logInfoP("Mirror effect enabled for segment");
  }
  
  logInfoP("Effect config: Speed=%d, Intensity=%d, Options=[%d,%d,%d], Reverse=%d, Mirror=%s",
           config.speed, config.intensity, config.option1, config.option2, config.mode,
           config.reverse, mirrorEffect ? "YES" : "NO");
}