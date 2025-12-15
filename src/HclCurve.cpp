#include "HclCurve.h"

// Enum definitions from XML parameter types
enum PT_hclType {
    PT_hclType_none = 0,    // "Deaktiviert"  
    PT_hclType_sun = 1,     // "Sonnenstand"
    PT_hclType_time = 2     // "Zeit"
};

enum PT_offset {
    PT_offset_disabled = 0, // "Deaktiviert"
    PT_offset_plus = 1,     // "Später"  
    PT_offset_minus = 2     // "Früher"
};


const std::string HclCurve::logPrefix()
{
    std::string name = "HCL<";
    name += std::to_string(_index+1);
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
    
    if(_isConfigured)
    {
        if(_type == PT_hclType_sun)
            logDebugP("Konfiguriert: Sonnenstand %i/%i", ParamNEO_HCLmin, ParamNEO_HCLmax);
        else if(_type == PT_hclType_time)
            logDebugP("Konfiguriert: Zeittabelle (noch nicht implementiert)");
    } else 
        logDebugP("Nicht Konfiguriert");
}

void HclCurve::loop()
{
    if(!_isConfigured) return;

    // Only sun mode is implemented currently
    if(_type != PT_hclType_sun) {
        logDebugP("Zeit-Modus noch nicht implementiert");
        return;
    }

    // Use Timer from OFM-LogicModule
    Timer& timer = Timer::instance();
    
    // Rate limiting: only update once per minute
    uint8_t currentMinute = timer.getMinute();
    if (currentMinute == _lastMinute) {
        return; // Already processed this minute
    }
    _lastMinute = currentMinute;

    sTime *sunRise = timer.getSunInfo(SUN_SUNRISE);
    sTime *sunSet = timer.getSunInfo(SUN_SUNSET);

    if((sunRise->hour == 0 && sunRise->minute == 0) || (sunSet->hour == 0 && sunSet->minute == 0))
    {
        logDebugP("Ungueltige Sonnenstandsdaten");
        return;
    }
        
    logDebugP("Aktuelle Zeit: %i:%i:%i", timer.getHour(), timer.getMinute(), timer.getSecond());

    uint16_t min = ParamNEO_HCLmin;
    uint16_t max = ParamNEO_HCLmax;
    uint16_t kelvinValue = min; // Default value
    
    if(timer.getHour() < sunRise->hour || (timer.getHour() == sunRise->hour && timer.getMinute() < sunRise->minute))
    {
        logDebugP("Vor Sonnenaufgang %i K (%i:%i)", min, sunRise->hour, sunRise->minute);
        kelvinValue = min;
    } else if(timer.getHour() > sunSet->hour || (timer.getHour() == sunSet->hour && timer.getMinute() > sunSet->minute)) {
        logDebugP("Nach Sonnenuntergang %i K (%i:%i)", max, sunSet->hour, sunSet->minute);
        kelvinValue = max;
    } else {
        logDebugP("Zwischen Sonnenaufgang und Sonnenuntergang");
        
        // Use signed arithmetic to prevent underflow
        int startMin = sunRise->hour*60 + sunRise->minute;
        int stopMin = sunSet->hour*60 + sunSet->minute;

        // Apply offsets
        if(ParamNEO_HCLoffsetRiseType == PT_offset_plus)
            startMin += ParamNEO_HCLoffsetRiseMin;
        else if(ParamNEO_HCLoffsetRiseType == PT_offset_minus)
            startMin -= ParamNEO_HCLoffsetRiseMin;

        if(ParamNEO_HCLoffsetSetType == PT_offset_plus)
            stopMin += ParamNEO_HCLoffsetSetMin;
        else if(ParamNEO_HCLoffsetSetType == PT_offset_minus)
            stopMin -= ParamNEO_HCLoffsetSetMin;

        int currentMin = timer.getHour()*60 + timer.getMinute();

        // Clamp to valid day range [0, 24*60-1]
        auto clampDay = [](int m) -> int {
            if (m < 0) return 0;
            if (m > 24*60-1) return 24*60-1;
            return m;
        };
        
        startMin = clampDay(startMin);
        stopMin = clampDay(stopMin);
        currentMin = clampDay(currentMin);

        int diff = stopMin - startMin;
        if (diff <= 0) {
            // Degenerate configuration (start >= stop), use fallback
            logDebugP("Ungueltige Konfiguration: Start >= Stop, verwende Min-Wert %i K", min);
            KoNEO_HCLState.value(min, Dpt(7, 600));
        } else {
            int rel = currentMin - startMin;
            if (rel < 0) rel = 0;
            if (rel > diff) rel = diff;
            
            uint16_t response = ColorHelper::getKelvinFromSun((uint16_t)rel, (uint16_t)diff, min, max);
            logDebugP("Response: %i K (rel=%i, diff=%i)", response, rel, diff);
            kelvinValue = response;
        }
    }
    
    // Debouncing: Only send KNX update if value actually changed
    if (kelvinValue != _lastKelvinValue) {
        logDebugP("Kelvin-Wert geaendert: %i K -> %i K", _lastKelvinValue, kelvinValue);
        KoNEO_HCLState.value(kelvinValue, Dpt(7, 600));
        _lastKelvinValue = kelvinValue;
    }
}