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

    logInfoP("Applying global brightness: %d%% to all segments", (brightness * 100) / 255);

    // Initialize original brightness storage if needed
    if (_module->_originalBrightness.size() != _module->_segments.size())
    {
        _module->_originalBrightness.resize(_module->_segments.size());

        // Store current brightness levels as original if not done yet
        for (size_t i = 0; i < _module->_segments.size(); i++)
        {
            if (_module->_segments[i].segment)
            {
                _module->_originalBrightness[i] = _module->_segments[i].segment->getBrightness();
                logDebugP("Stored original brightness for segment %zu: %d%%",
                          i, (_module->_originalBrightness[i] * 100) / 255);
            }
        }
    }

    // Apply brightness to all configured segments
    for (size_t i = 0; i < _module->_segments.size(); i++)
    {
        if (_module->_segments[i].segment)
        {
            // Use stored original brightness, not current (to avoid cumulative effects)
            uint8_t originalBrightness = _module->_originalBrightness[i];
            uint8_t effectiveBrightness = (originalBrightness * brightness) / 255;

            // Apply effective brightness to segment
            _module->_segments[i].segment->setBrightness(effectiveBrightness);

            logDebugP("Segment %zu brightness: original=%d%%, global=%d%%, effective=%d%%",
                      i,
                      (originalBrightness * 100) / 255,
                      (brightness * 100) / 255,
                      (effectiveBrightness * 100) / 255);
        }
    }

    logInfoP("Global brightness %d%% applied to %d segments",
             (brightness * 100) / 255, (int)_module->_segments.size());
}

void ColorManagement::restoreOriginalBrightness()
{
    logInfoP("Restoring original brightness levels for all segments");

    // Restore original brightness levels for all segments
    for (size_t i = 0; i < _module->_segments.size() && i < _module->_originalBrightness.size(); i++)
    {
        if (_module->_segments[i].segment)
        {
            uint8_t originalBrightness = _module->_originalBrightness[i];
            _module->_segments[i].segment->setBrightness(originalBrightness);

            logDebugP("Restored segment %zu brightness to original: %d%%",
                      i, (originalBrightness * 100) / 255);
        }
    }

    // Reset global brightness to full
    _module->_globalBrightness = 255;

    logInfoP("Original brightness levels restored for all %d segments", (int)_module->_segments.size());
}

// ============================================================================
// HCL Color Temperature Control Implementation
// ============================================================================

void ColorManagement::applyHclColorTemperature(uint16_t kelvin)
{
    _module->_currentHclTemperature = kelvin;

    // Validate Kelvin range (typical LED range: 2000K-10000K)
    if (kelvin < 2000 || kelvin > 10000)
    {
        logWarningP("HCL temperature %dK out of typical range (2000K-10000K)", kelvin);
        return;
    }

    logInfoP("Applying HCL color temperature: %dK to all segments", kelvin);

    // Convert Kelvin to RGB using ColorHelper
    uint8_t r, g, b;
    ColorHelper::kelvinToRGB(kelvin, r, g, b);

    logDebugP("HCL %dK converts to RGB: R=%d, G=%d, B=%d", kelvin, r, g, b);

    // Store original colors if this is the first HCL application
    if (!_module->_hclModeEnabled && _module->_originalColors.size() != _module->_segments.size())
    {
        _module->_originalColors.resize(_module->_segments.size());

        for (size_t i = 0; i < _module->_segments.size(); i++)
        {
            if (_module->_segments[i].segment)
            {
                uint8_t origR, origG, origB;
                if (_module->_segments[i].segment->getPixel(0, origR, origG, origB))
                {
                    _module->_originalColors[i] = {origR, origG, origB};
                    logDebugP("Stored original color for segment %zu: R=%d, G=%d, B=%d",
                              i, origR, origG, origB);
                }
                else
                {
                    _module->_originalColors[i] = {255, 255, 255}; // Default to white if can't read
                }
            }
        }
    }

    // Apply color temperature to all segments
    uint16_t segmentsUpdated = 0;
    for (auto& segmentConfig : _module->_segments)
    {
        if (segmentConfig.segment)
        {
            // Apply the Kelvin-derived RGB to the entire segment
            segmentConfig.segment->setPrimaryColor(r, g, b, 0);
            segmentsUpdated++;

            logDebugP("Applied %dK color temperature to segment", kelvin);
        }
    }

    _module->_hclModeEnabled = true;

    logInfoP("HCL color temperature %dK applied to %d segments", kelvin, segmentsUpdated);
}

void ColorManagement::disableHclMode()
{
    if (!_module->_hclModeEnabled)
    {
        logDebugP("HCL mode already disabled");
        return;
    }

    logInfoP("Disabling HCL mode and restoring original colors");

    // Restore original colors for all segments
    for (size_t i = 0; i < _module->_segments.size() && i < _module->_originalColors.size(); i++)
    {
        if (_module->_segments[i].segment)
        {
            auto& color = _module->_originalColors[i];
            _module->_segments[i].segment->setPrimaryColor(color[0], color[1], color[2], 0);

            logDebugP("Restored segment %zu color to original: R=%d, G=%d, B=%d",
                      i, color[0], color[1], color[2]);
        }
    }

    _module->_hclModeEnabled = false;
    _module->_currentHclTemperature = 6500; // Reset to neutral daylight

    logInfoP("HCL mode disabled, original colors restored for all %d segments", (int)_module->_segments.size());
}

// ============================================================================
// Color Correction Implementation
// ============================================================================

void ColorManagement::configureColorCorrection()
{
    // Local variable for ETS parameter macros
    uint8_t _channelIndex = _module->getChannelIndex();

    // Gamma correction configuration
    _module->_gammaCorrectionEnabled = (bool)ParamNEOSTRIP_NEOGammaCorrection;
    if (_module->_gammaCorrectionEnabled)
    {
        uint8_t gammaParam = ParamNEOSTRIP_NEOGammaValue;
        _module->_gammaValue = mapGammaValue(gammaParam);
        logInfoP("Gamma correction configured: value=%.2f", _module->_gammaValue);
    }

    // White balance configuration
    _module->_whiteBalanceEnabled = (bool)ParamNEOSTRIP_NEOWhiteBalanceCorrection;
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
    // Map ETS parameter (0-10) to gamma value range (2.0-3.0)
    // ETS XML configuration:
    // Value 0 -> 2.0, Value 1 -> 2.1, ..., Value 10 -> 3.0
    // Each step represents 0.1 gamma increment

    // Clamp parameter value to valid range
    if (paramValue > 10)
    {
        paramValue = 10;
    }

    // Convert: gamma = 2.0 + (paramValue * 0.1)
    float gammaValue = 2.0f + (paramValue * 0.1f);

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
