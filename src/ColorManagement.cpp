#include "ColorManagement.h"
#include "NeoPixelModule.h"
#include "OpenKNX.h"
#include "colorhelper.h"

ColorManagement::ColorManagement(NeoPixelBusModule* module)
    : _module(module)
{
}

// ============================================================================
// Global Brightness Control Implementation
// ============================================================================

void ColorManagement::applyGlobalBrightness(uint8_t brightness)
{
    // Special case: if brightness is full (255), restore original brightness levels
    if (brightness == 255)
    {
        restoreOriginalBrightness();
        return;
    }

    _module->_globalBrightness = brightness;

    logInfoP("Applying global brightness: %d%% to all segments", (brightness * 100 + 127) / 255);

    // Apply brightness to all configured segments
    // Use savedBrightness from SegmentConfig (always kept up-to-date when segment brightness changes)
    for (size_t i = 0; i < _module->_segments.size(); i++)
    {
        NeoPixelBusModule::SegmentConfig& cfg = _module->_segments[i];
        if (cfg.segment)
        {
            // Use savedBrightness as the "desired" brightness before global scaling
            // savedBrightness is updated whenever segment brightness is set via KO
            uint8_t originalBrightness = cfg.savedBrightness;

            // If savedBrightness is 0 but segment is on, use current brightness as fallback
            if (originalBrightness == 0 && cfg.segment->getBrightness() > 0)
            {
                originalBrightness = cfg.segment->getBrightness();
            }

            uint8_t effectiveBrightness = (originalBrightness * brightness) / 255;

            // Apply effective brightness to segment
            cfg.segment->setBrightness(effectiveBrightness);

            logDebugP("Segment %zu brightness: saved=%d%%, global=%d%%, effective=%d%%",
                      i,
                      (originalBrightness * 100 + 127) / 255,
                      (brightness * 100 + 127) / 255,
                      (effectiveBrightness * 100 + 127) / 255);
        }
    }

    logInfoP("Global brightness %d%% applied to %d segments",
             (brightness * 100 + 127) / 255, (int)_module->_segments.size());
}

void ColorManagement::restoreOriginalBrightness()
{
    logInfoP("Restoring original brightness levels for all segments");

    // Restore original brightness levels for all segments using savedBrightness
    for (size_t i = 0; i < _module->_segments.size(); i++)
    {
        NeoPixelBusModule::SegmentConfig& cfg = _module->_segments[i];
        if (cfg.segment)
        {
            // Restore to savedBrightness (the desired brightness before global scaling)
            uint8_t originalBrightness = cfg.savedBrightness;

            // If savedBrightness is 0, default to full brightness
            if (originalBrightness == 0)
            {
                originalBrightness = 255;
            }

            cfg.segment->setBrightness(originalBrightness);

            logDebugP("Restored segment %zu brightness to saved: %d%%",
                      i, (originalBrightness * 100 + 127) / 255);
        }
    }

    // Reset global brightness to full
    _module->_globalBrightness = 255;

    logInfoP("Original brightness levels restored for all %d segments", (int)_module->_segments.size());
}

// ============================================================================
// HCL (Human Centric Lighting) post-processing implementation
//
// Goal: Circadian Kelvin correction without destroying effects/scenes.
// We post-process the rendered pixels and apply a Kelvin whitepoint mainly to
// white/low-saturation pixels (configurable). For RGBW/RGBCCT we can also
// extract neutral luminance into W to preserve saturation.
//
// Controlled via ETS params (generated in knxprod.h):
//   - ParamNEO_HCLenableKelvin (bool)
//   - ParamNEO_HclApplyMode (0=Nur Weiß, 1=Alle Farben)
//   - ParamNEO_HCLSatThreshold (0..255)
//   - ParamNEO_HCLPreserveCurve (0=Linear,1=Smooth,2=Gamma)
//   - ParamNEO_HCLStrength (0..100 %) : overall mix strength
//   - ParamNEO_HCLTransitionTime (0..255 s) : Kelvin slew time
//   - ParamNEO_HCLBrightnessCompensation (0..100 %) : perceived brightness equalization
//   - ParamNEO_HCLWhiteMix (0..100 %) : how much neutral goes to W (RGBW only)
// ============================================================================

// Structure to pass HCL parameters to the callback
struct HclCallbackData
{
    uint16_t kelvin;
    uint8_t applyMode;
    uint8_t satThreshold;
    uint8_t preserveCurve;
    uint8_t strength;
    uint8_t brightnessComp;
    uint8_t whiteMix;
    uint16_t minKelvin;        // Strip's minimum Kelvin (warm white, e.g., 2700K)
    uint16_t maxKelvin;        // Strip's maximum Kelvin (cool white, e.g., 6500K)
    NeoPixelBusModule* module; // Module pointer for segment access
};

// Static storage for HCL callback data (one global HCL state)
static HclCallbackData s_hclData = {0, 0, 128, 0, 100, 50, 50, 2700, 6500, nullptr};

// ============================================================================
// HCL Color Temperature Control Implementation
// ============================================================================

void ColorManagement::applyHclColorTemperature(uint16_t kelvin)
{
    // 0K is used as "disable" command.
    if (kelvin == 0)
    {
        disableHclMode();
        return;
    }

    _hclTargetKelvin = kelvin;

    // Sanity range
    if (kelvin < 1000 || kelvin > 10000)
    {
        logWarningP("HCL target %dK out of range (1000K-10000K)", kelvin);
    }

    if (!_hclEnabled)
    {
        _hclEnabled = true;
        _hclAppliedKelvin = _hclTargetKelvin;
        _lastHclApplyMs = 0;
        _hclSlewAccumulatorMs = 0;
        logInfoP("HCL post-processing enabled: %d K target", (int)_hclTargetKelvin);
    }
    else if (_hclTargetKelvin != kelvin)
    {
        logInfoP("HCL target Kelvin changed: %d K → %d K", (int)_module->_hclTargetKelvin, (int)kelvin);
    }

    // Keep module state in sync for status getters / diagnostics
    _module->_hclTargetKelvin = _hclTargetKelvin;
    _module->_hclAppliedKelvin = _hclAppliedKelvin;
    _module->_hclModeEnabled = true;

    logInfoP("HCL target set to %dK (post-processing enabled)", (int)_hclTargetKelvin);
}

void ColorManagement::disableHclMode()
{
    if (!_hclEnabled && !_module->_hclModeEnabled)
    {
        logDebugP("HCL already disabled");
        return;
    }

    _hclEnabled = false;
    _hclTargetKelvin = 0;
    _hclAppliedKelvin = 0;
    _lastHclApplyMs = 0;
    _hclSlewAccumulatorMs = 0;

    _module->_hclModeEnabled = false;
    _module->_hclTargetKelvin = 6500;
    _module->_hclAppliedKelvin = 6500;

    // Clear the HCL callback data (kelvin=0 disables the callback)
    s_hclData.kelvin = 0;

    logInfoP("HCL disabled");
}

// ============================================================================
// Color Correction Implementation
// ============================================================================

void ColorManagement::configureColorCorrection()
{
    // Local variable for ETS parameter macros
    uint8_t _channelIndex = _module->getChannelIndex();
    // ═════════════════════════════════════════════════════════════════════════════
    // Color Calibration Master Switch (UP-046)
    // If disabled: Ignore both white balance AND gamma correction
    // ═════════════════════════════════════════════════════════════════════════════
    bool colorCalibrationMasterEnabled = (bool)ParamNEOSTRIP_NEOColorCalibrationMaster;

    // Gamma correction configuration (for colors)
    // Only applied if Master-Checkbox is ON AND Gamma-Checkbox is ON
    _module->_gammaCorrectionEnabled = colorCalibrationMasterEnabled && (bool)ParamNEOSTRIP_NEOGammaCorrection;
    if (_module->_gammaCorrectionEnabled)
    {
        uint8_t gammaParam = static_cast<uint8_t>(ParamNEOSTRIP_NEOGammaValue);
        _module->_gammaValue = mapGammaValue(gammaParam);
        logInfoP("Gamma correction configured: value=%.2f", _module->_gammaValue);
    }

    // White balance configuration
    // Only applied if Master-Checkbox is ON (Weißabgleich has no separate checkbox)
    _module->_whiteBalanceEnabled = colorCalibrationMasterEnabled;
    if (_module->_whiteBalanceEnabled)
    {
        _module->_whiteBalanceRed = mapWhiteBalanceValue(ParamNEOSTRIP_NEOWhiteBalanceRed);
        _module->_whiteBalanceGreen = mapWhiteBalanceValue(ParamNEOSTRIP_NEOWhiteBalanceGreen);
        _module->_whiteBalanceBlue = mapWhiteBalanceValue(ParamNEOSTRIP_NEOWhiteBalanceBlue);
        logInfoP("White balance configured: R:%d G:%d B:%d",
                 _module->_whiteBalanceRed, _module->_whiteBalanceGreen, _module->_whiteBalanceBlue);
    }
}

void ColorManagement::updateColorCorrection()
{
    if (!_module->_initialized) return;
    // Update color correction parameters on VirtualStrip
    // (These are applied during rendering, NOT in-place!)
    // NOTE: setColorCorrection removed from VirtualStrip - color correction deactivated for now
    /*     if (_module->_virtualStrip) {
          _module->_virtualStrip->setColorCorrection(
            _module->_gammaCorrectionEnabled, _module->_gammaValue,
            _module->_whiteBalanceEnabled, _module->_whiteBalanceRed, _module->_whiteBalanceGreen, _module->_whiteBalanceBlue,
            _module->_swapMode
          );
        } */
}

void ColorManagement::forceColorCorrectionUpdate()
{
    if (!_module->_initialized) return;

    // Force immediate update bypassing rate limiting
    updateColorCorrection();
    _module->_lastColorUpdateMs = millis(); // Update timestamp to reset rate limiting

    logDebugP("Forced color correction update");
}

float ColorManagement::mapGammaValue(uint8_t paramValue)
{
    // Map ETS parameter (0-15) to gamma value range (1.2-2.7)
    // ETS XML configuration (4 bits):
    // Value 0 -> 1.2, Value 1 -> 1.3, ..., Value 15 -> 2.7
    // Each step represents 0.1 gamma increment

    // Clamp parameter value to valid range (4 bits = 0-15)
    if (paramValue > 15)
    {
        paramValue = 15;
    }

    // Convert: gamma = 1.2 + (paramValue * 0.1)
    float gammaValue = 1.2f + (paramValue * 0.1f);

    return gammaValue;
}

uint8_t ColorManagement::mapWhiteBalanceValue(uint8_t paramValue)
{
    // ETS parameter is percentage (0-100), need to map to RGB scaling range (0-255)
    // ETS XML configuration: PT-NEOPercent with range 0-100
    // Convert: rgbValue = (percentage * 255) / 100

    // Clamp parameter value to valid range
    if (paramValue > 100)
    {
        paramValue = 100;
    }

    // Convert percentage to 0-255 range for RGB channel scaling
    uint8_t rgbValue = (paramValue * 255) / 100;

    return rgbValue;
}
