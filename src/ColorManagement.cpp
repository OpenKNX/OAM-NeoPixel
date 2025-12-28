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

// ============================================================================
// HCL Callback Data (forward declaration for disableHclMode)
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
};

// Static storage for HCL callback data (one global HCL state)
static HclCallbackData s_hclData = {0, 0, 128, 0, 100, 50, 50};

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

    _module->_hclModeEnabled = false;
    _module->_hclTargetKelvin = 6500;
    _module->_hclAppliedKelvin = 6500;

    // Clear the HCL callback data (kelvin=0 disables the callback)
    s_hclData.kelvin = 0;

    logInfoP("HCL disabled");
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
//   - ParamNEO_HclApplyMode (0=Nur Wei�, 1=Alle Farben)
//   - ParamNEO_HCLSatThreshold (0..255)
//   - ParamNEO_HCLPreserveCurve (0=Linear,1=Smooth,2=Gamma)
//   - ParamNEO_HCLStrength (0..100 %) : overall mix strength
//   - ParamNEO_HCLTransitionTime (0..255 s) : Kelvin slew time
//   - ParamNEO_HCLBrightnessCompensation (0..100 %) : perceived brightness equalization
//   - ParamNEO_HCLWhiteMix (0..100 %) : how much neutral goes to W (RGBW only)
// ============================================================================

namespace
{
    inline uint8_t u8_max3(uint8_t a, uint8_t b, uint8_t c)
    {
        return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
    }

    inline uint8_t u8_min3(uint8_t a, uint8_t b, uint8_t c)
    {
        return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
    }

    inline uint8_t lerp_u8(uint8_t a, uint8_t b, uint8_t frac /*0..255*/)
    {
        const uint16_t inv = 255u - frac;
        return (uint8_t)((a * inv + b * (uint16_t)frac + 127u) / 255u);
    }

    inline uint8_t clamp_u8(int v)
    {
        if (v < 0) return 0;
        if (v > 255) return 255;
        return (uint8_t)v;
    }

    // Integer smoothstep (t in 0..255)
    inline uint8_t smoothstep_u8(uint8_t t)
    {
        // t^2 * (3 - 2t) in 0..1 (scaled to 0..255)
        const uint32_t tt = (uint32_t)t * (uint32_t)t;   // 0..65025
        const uint32_t a = 3u * 255u - 2u * (uint32_t)t; // 0..765
        const uint32_t v = (tt * a + (255u * 255u / 2u)) / (255u * 255u);
        return (uint8_t)((v > 255u) ? 255u : v);
    }

    // HCL blend weight from saturation.
    //
    // applyMode=0 (Nur Weiss):
    //   - Only pixels below threshold are affected (hard cutoff) unless they already have W.
    //   - Weight falls from 255 (sat=0) to 0 (sat=threshold).
    //
    // applyMode=1 (Alle Farben):
    //   - All pixels can be affected.
    //   - Threshold defines the point where we start fading out; above threshold we fade to 0 at sat=255.
    //   - This matches typical "commercial" HCL behavior: whites get strong correction, saturated colors less.
    inline uint8_t hclWeightFromSat(uint8_t sat, uint8_t threshold, uint8_t applyMode, uint8_t curve /*0..2*/)
    {
        uint32_t w = 0;

        if (applyMode == 0)
        {
            // Nur Weiss: weight 255..0 within [0..threshold]
            if (threshold == 0)
            {
                w = (sat == 0) ? 255u : 0u;
            }
            else
            {
                if (sat >= threshold) return 0;
                w = 255u - ((uint32_t)sat * 255u) / threshold;
            }
        }
        else
        {
            // Alle Farben: full weight below threshold, then fade to 0 at sat=255
            if (sat <= threshold)
            {
                w = 255u;
            }
            else
            {
                const uint32_t denom = (threshold >= 255) ? 1u : (255u - (uint32_t)threshold);
                w = ((255u - (uint32_t)sat) * 255u) / denom;
            }
        }

        if (w > 255u) w = 255u;
        const uint8_t ww = (uint8_t)w;

        switch (curve)
        {
            default:
            case 0: // Linear
                return ww;
            case 1: // Smooth
                return smoothstep_u8(ww);
            case 2: // Gamma (approx gamma=2)
                return (uint8_t)(((uint16_t)ww * (uint16_t)ww + 127u) / 255u);
        }
    }

    inline void apply_hcl_pixel(uint16_t kelvin,
                                uint8_t applyMode,
                                uint8_t satThreshold,
                                uint8_t preserveCurve,
                                uint8_t strengthPct,
                                uint8_t brightnessCompPct,
                                uint8_t whiteMixPct,
                                uint8_t& r, uint8_t& g, uint8_t& b,
                                uint8_t* wOpt)
    {
        uint8_t kr, kg, kb;
        ColorHelper::kelvinToRGB(kelvin, kr, kg, kb);

        const uint8_t vmax = u8_max3(r, g, b);
        const uint8_t vmin = u8_min3(r, g, b);
        const uint8_t sat = (vmax == 0) ? 0 : (uint8_t)(((uint16_t)(vmax - vmin) * 255u) / vmax);
        const uint8_t wIn = (wOpt) ? *wOpt : 0;

        // ApplyMode=Nur Weiss: hard cutoff above threshold unless explicit W
        if (applyMode == 0 && sat >= satThreshold && wIn == 0) return;

        const uint8_t preserve = hclWeightFromSat(sat, satThreshold, applyMode, preserveCurve);
        uint8_t strength = (strengthPct > 100) ? 100 : strengthPct;

        uint16_t wFrac16 = (uint16_t)preserve * (uint16_t)strength; // 0..25500
        uint8_t frac = (uint8_t)((wFrac16 + 50u) / 100u);           // 0..255

        if (frac == 0) return;

        // Use current pixel value/brightness basis (include W)
        uint8_t v = vmax;
        if (wIn > v) v = wIn;

        int tr = ((int)kr * (int)v) / 255;
        int tg = ((int)kg * (int)v) / 255;
        int tb = ((int)kb * (int)v) / 255;
        int tw = 0;

        // Perceived brightness compensation (optional)
        if (brightnessCompPct > 0)
        {
            uint8_t rr, rg, rb;
            ColorHelper::kelvinToRGB(6500, rr, rg, rb);

            const uint32_t yRef = 54u * rr + 183u * rg + 19u * rb;
            const uint32_t yKel = 54u * kr + 183u * kg + 19u * kb;
            if (yKel > 0)
            {
                uint32_t scaleQ8 = (yRef << 8) / yKel; // 256=1.0
                if (scaleQ8 < 128u) scaleQ8 = 128u;
                if (scaleQ8 > 512u) scaleQ8 = 512u;

                uint8_t comp = (brightnessCompPct > 100) ? 100 : brightnessCompPct;
                int32_t delta = (int32_t)scaleQ8 - 256;
                scaleQ8 = (uint32_t)(256 + (delta * comp) / 100);

                tr = (int)((tr * (int)scaleQ8 + 128) >> 8);
                tg = (int)((tg * (int)scaleQ8 + 128) >> 8);
                tb = (int)((tb * (int)scaleQ8 + 128) >> 8);
            }
        }

        // RGBW: extract neutral part into W (configurable)
        if (wOpt)
        {
            int n = u8_min3((uint8_t)clamp_u8(tr), (uint8_t)clamp_u8(tg), (uint8_t)clamp_u8(tb));
            uint8_t mix = (whiteMixPct > 100) ? 100 : whiteMixPct;
            int extracted = (n * (int)mix + 50) / 100;
            tr -= extracted;
            tg -= extracted;
            tb -= extracted;
            tw = extracted;
        }

        r = lerp_u8(r, clamp_u8(tr), frac);
        g = lerp_u8(g, clamp_u8(tg), frac);
        b = lerp_u8(b, clamp_u8(tb), frac);
        if (wOpt) *wOpt = lerp_u8(wIn, clamp_u8(tw), frac);
    }
} // namespace

// ============================================================================
// HCL Pixel Transform Callback (called from VirtualStrip::syncToPhysical)
// ============================================================================

/**
 * @brief Static callback for HCL pixel transformation
 *
 * This is called from VirtualStrip::syncToPhysical() for each pixel,
 * AFTER effects have rendered but BEFORE sending to hardware.
 * The effect buffer is NOT modified, so effects that read back pixels work correctly.
 */
static void hclPixelTransformCallback(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t* w, void* userData)
{
    (void)userData; // Use static data instead

    if (s_hclData.kelvin == 0) return; // HCL disabled

    apply_hcl_pixel(s_hclData.kelvin,
                    s_hclData.applyMode,
                    s_hclData.satThreshold,
                    s_hclData.preserveCurve,
                    s_hclData.strength,
                    s_hclData.brightnessComp,
                    s_hclData.whiteMix,
                    r, g, b, w);
}

void ColorManagement::applyHclPostProcess()
{
    if (!_module || !_module->_initialized) return;
    if (!_hclEnabled) return;
    if (_hclTargetKelvin == 0) return;
    // Note: HCL enable/disable controlled by HCLtype parameter (checked in HclCurve::loop)

    const unsigned long now = millis();

    // Rate limit
    constexpr unsigned long kMinIntervalMs = 20; // 50 Hz
    if (_lastHclApplyMs != 0 && (now - _lastHclApplyMs) < kMinIntervalMs)
        return;
    const unsigned long dtMs = (_lastHclApplyMs == 0) ? kMinIntervalMs : (now - _lastHclApplyMs);
    _lastHclApplyMs = now;

    // Kelvin slew / transition
    uint16_t target = _hclTargetKelvin;
    uint16_t applied = _hclAppliedKelvin;

    uint16_t transitionSec = (uint16_t)ParamNEO_HCLTransitionTime;
    if (transitionSec == 0)
    {
        applied = target;
    }
    else
    {
        int32_t diff = (int32_t)target - (int32_t)applied;
        if (diff != 0)
        {
            uint32_t denom = (uint32_t)transitionSec * 1000u;
            uint32_t adiff = (uint32_t)((diff < 0) ? -diff : diff);
            uint32_t step = (adiff * (uint32_t)dtMs) / denom;
            if (step == 0) step = 1;
            if (step > adiff) step = adiff;
            applied = (uint16_t)((int32_t)applied + ((diff < 0) ? -(int32_t)step : (int32_t)step));
        }
    }
    _hclAppliedKelvin = applied;

    // Log Kelvin transitions
    if (applied != _module->_hclAppliedKelvin)
    {
        logDebugP("HCL Kelvin slewed: %u K → %u K (target=%u K, transition=%us)",
                  (unsigned)_module->_hclAppliedKelvin, (unsigned)applied,
                  (unsigned)target, (unsigned)transitionSec);
    }

    // Keep module state in sync for status getters / diagnostics
    _module->_hclTargetKelvin = target;
    _module->_hclAppliedKelvin = applied;
    _module->_hclModeEnabled = true;

    // Update HCL callback parameters (used during syncToPhysical)
    s_hclData.kelvin = applied;
    s_hclData.applyMode = (uint8_t)ParamNEO_HclApplyMode;
    s_hclData.satThreshold = (uint8_t)ParamNEO_HCLSatThreshold;
    s_hclData.preserveCurve = (uint8_t)ParamNEO_HCLPreserveCurve;
    s_hclData.strength = (uint8_t)ParamNEO_HCLStrength;
    s_hclData.brightnessComp = (uint8_t)ParamNEO_HCLBrightnessCompensation;
    s_hclData.whiteMix = (uint8_t)ParamNEO_HCLWhiteMix;

    // Register the HCL callback on VirtualStrip (if not already done)
    // The callback is called during syncToPhysical() for each pixel
    if (_module->_virtualStrip)
    {
        _module->_virtualStrip->setPixelTransformCallback(hclPixelTransformCallback, nullptr);
    }

    // NOTE: Pixel transformation now happens in VirtualStrip::syncToPhysical()
    // This preserves the effect buffer, so effects like Cylon that read back pixels work correctly
}

// ============================================================================
// Color Correction Implementation
// ============================================================================

void ColorManagement::configureColorCorrection()
{
    // Local variable for ETS parameter macros
    uint8_t _channelIndex = _module->getChannelIndex();

    // Gamma correction configuration (for colors)
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
