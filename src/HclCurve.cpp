#include "HclCurve.h"
#include <math.h>

// Enum definitions from XML parameter types
enum PT_hclType
{
    PT_hclType_none = 0, // "Deaktiviert"
    PT_hclType_sun = 1,  // "Sonnenstand"
    PT_hclType_time = 2  // "Zeit"
};

const std::string HclCurve::logPrefix()
{
    std::string name = "HCL<";
    name += std::to_string(_index + 1);
    name += ">";
    return name;
}

const uint8_t HclCurve::channelIndex()
{
    return _index;
}

void HclCurve::setup(uint8_t index)
{
    _index = index;
    _type = ParamNEO_HCLtype;
    _isConfigured = _type != PT_hclType_none;
    _lastKelvinValue = 0;
    _lastMinute = 255; // Invalid value to force first update

    if (_isConfigured)
    {
        uint16_t kMin = (uint16_t)ParamNEO_HCLminKelvin;
        uint16_t kMax = (uint16_t)ParamNEO_HCLmaxKelvin;

        if (_type == PT_hclType_sun)
        {
            logInfoP("HCL Sun-based curve configured: Kelvin %u..%u K", (unsigned)kMin, (unsigned)kMax);
            if (kMin == 0 || kMax == 0)
                logWarningP("HCL: Min/Max Kelvin not configured in ETS (values are 0)");
        }
        else if (_type == PT_hclType_time)
        {
            logInfoP("HCL Time-based curve configured: Kelvin %u..%u K, Window %02u:%02u-%02u:%02u",
                     (unsigned)kMin, (unsigned)kMax,
                     (unsigned)ParamNEO_HCLStartHour, (unsigned)ParamNEO_HCLStartMinute,
                     (unsigned)ParamNEO_HCLEndHour, (unsigned)ParamNEO_HCLEndMinute);
            if (kMin == 0 || kMax == 0)
                logWarningP("HCL: Min/Max Kelvin not configured in ETS (values are 0) - will use 1000K..10000K defaults in loop");
        }
    }
    else
        logInfoP("HCL curve: Not configured (disabled)");
}

void HclCurve::loop()
{
    if (!_isConfigured) return;

    // HCL curve computes a Kelvin target and writes it to the KO.
    // Pixel-level "true HCL" application happens in NeoPixelModule (post-processing).

    Timer &timer = Timer::instance();

    // Rate limiting: only update once per minute
    const uint8_t currentMinute = timer.getMinute();
    if (currentMinute == _lastMinute)
        return;
    _lastMinute = currentMinute;

    // Warn if system time appears to be unset (00:00:00)
    if (timer.getHour() == 0 && timer.getMinute() == 0 && timer.getSecond() == 0)
    {
        logWarningP("HCL: System time not set (00:00:00) - device may not have synchronized time via NTP or KNX");
    }

    // If Kelvin is disabled, publish 0K (so module can disable HCL cleanly)
    if (!ParamNEO_HCLenableKelvin)
    {
        if (_lastKelvinValue != 0)
        {
            logDebugP("Kelvin deaktiviert -> 0K");
            KoNEO_HCLState.value((uint16_t)0, Dpt(7, 600));
            _lastKelvinValue = 0;
        }
        return;
    }

    // Bounds
    uint16_t kMin = (uint16_t)ParamNEO_HCLminKelvin;
    uint16_t kMax = (uint16_t)ParamNEO_HCLmaxKelvin;
    if (kMin > kMax)
    {
        uint16_t tmp = kMin;
        kMin = kMax;
        kMax = tmp;
    }

    // Apply safe defaults if not configured
    bool usedDefaults = false;
    if (kMin < 1000)
    {
        kMin = 1000;
        usedDefaults = true;
    }
    if (kMax > 10000)
    {
        kMax = 10000;
        usedDefaults = true;
    }
    if (kMax == 0 || kMin == 0)
    {
        kMin = 1000;
        kMax = 10000;
        usedDefaults = true;
    }

    if (usedDefaults && _lastMinute == 255)
    {
        logWarningP("HCL: Using default Kelvin range 1000K..10000K (ETS parameters not configured)");
    }

    // Helper: clamp minutes to [0..1439]
    auto clampDay = [](int m) -> int {
        if (m < 0) return 0;
        if (m > 24 * 60 - 1) return 24 * 60 - 1;
        return m;
    };

    const int nowMin = (int)timer.getHour() * 60 + (int)timer.getMinute();

    // Compute target Kelvin
    uint16_t kelvinValue = kMin;

    if (_type == PT_hclType_sun)
    {
        // Sun-based curve: warm at edges (sunrise/sunset), cool at midday (peak).
        sTime *sunRise = timer.getSunInfo(SUN_SUNRISE);
        sTime *sunSet = timer.getSunInfo(SUN_SUNSET);

        if ((sunRise->hour == 0 && sunRise->minute == 0) || (sunSet->hour == 0 && sunSet->minute == 0))
        {
            // Sun times not available yet; fall back to min
            kelvinValue = kMin;
        }
        else
        {
            int startMin = (int)sunRise->hour * 60 + (int)sunRise->minute;
            int stopMin = (int)sunSet->hour * 60 + (int)sunSet->minute;

            // Signed offsets (minutes)
            startMin += (int)ParamNEO_HCLoffsetSunrise;
            stopMin += (int)ParamNEO_HCLoffsetSunset;

            startMin = clampDay(startMin);
            stopMin = clampDay(stopMin);

            // Guard: invalid window
            if (stopMin <= startMin)
            {
                kelvinValue = kMin;
            }
            else if (nowMin <= startMin || nowMin >= stopMin)
            {
                // Outside window: warm
                kelvinValue = kMin;
            }
            else
            {
                const float p = (float)(nowMin - startMin) / (float)(stopMin - startMin); // 0..1
                // Half-sine: 0 at edges, 1 at middle
                const float s = sinf(p * 3.14159265f);
                const float k = (float)kMin + ((float)(kMax - kMin) * s);
                kelvinValue = (uint16_t)(k + 0.5f);
            }
        }
    }
    else if (_type == PT_hclType_time)
    {
        // Time-based curve: warm at edges, cool at middle within user window.
        const int beginMin = clampDay((int)ParamNEO_HCLStartHour * 60 + (int)ParamNEO_HCLStartMinute);
        const int endMin = clampDay((int)ParamNEO_HCLEndHour * 60 + (int)ParamNEO_HCLEndMinute);

        // Support windows spanning midnight (e.g., 22:00 -> 06:00)
        bool inWindow = false;
        float p = 0.0f;

        if (beginMin == endMin)
        {
            // Degenerate: treat as always in window
            inWindow = true;
            p = 0.5f;
        }
        else if (beginMin < endMin)
        {
            inWindow = (nowMin >= beginMin && nowMin <= endMin);
            if (inWindow)
                p = (float)(nowMin - beginMin) / (float)(endMin - beginMin);
        }
        else
        {
            // Wrap over midnight
            inWindow = (nowMin >= beginMin || nowMin <= endMin);
            if (inWindow)
            {
                const int span = (24 * 60 - beginMin) + endMin;
                int rel = (nowMin >= beginMin) ? (nowMin - beginMin) : ((24 * 60 - beginMin) + nowMin);
                p = (span > 0) ? ((float)rel / (float)span) : 0.5f;
            }
        }

        if (!inWindow)
        {
            kelvinValue = kMin;
        }
        else
        {
            const float s = sinf(p * 3.14159265f);
            const float k = (float)kMin + ((float)(kMax - kMin) * s);
            kelvinValue = (uint16_t)(k + 0.5f);
        }
    }
    else
    {
        // Disabled / unknown type
        kelvinValue = 0;
    }

    // Debounce KNX update
    if (kelvinValue != _lastKelvinValue)
    {
        logInfoP("HCL Kelvin updated: %u K → %u K (min=%u, max=%u)",
                 (unsigned)_lastKelvinValue, (unsigned)kelvinValue,
                 (unsigned)kMin, (unsigned)kMax);
        KoNEO_HCLState.value(kelvinValue, Dpt(7, 600));
        _lastKelvinValue = kelvinValue;
    }
}