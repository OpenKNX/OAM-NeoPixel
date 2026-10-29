#include "NeoPixelModule.h"
#include "NeoPixelStudio.h"

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    #include <algorithm>

// Wiring tests bypass HCL only inside the tested segment. Native white LEDs
// remain isolated; RGB-only physical outputs synthesize white at test brightness.
bool NeoPixelStudio::transformTestPixel(uint16_t pixel, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t* ww, uint8_t* cw)
{
    if (_testSegment < 0) return false;
    const auto& segment = _module._segments[_testSegment];
    if (pixel < segment.startLed || pixel > segment.endLed) return false;
    if (_testPattern != "white" && _testPattern != "warm" && _testPattern != "cool") return true;
    for (const auto& mapping : _module._virtualStripConfiguration)
    {
        if (pixel < mapping.virtualStartPosition || unsigned(pixel - mapping.virtualStartPosition) >= mapping.ledCount) continue;
        const auto* strip = _module._physicalStrips[mapping.physicalStripIndex];
        if (ProtocolHelper::hasWhiteChannel(strip->getColorOrder())) return true;
        const unsigned level = ww ? std::min<unsigned>(255, unsigned(*ww) + (cw ? *cw : 0)) : std::max({r, g, b});
        r = _testPattern == "cool" ? level * 201 / 255 : level;
        g = _testPattern == "warm" ? level * 138 / 255 : _testPattern == "cool" ? level * 226 / 255
                                                                                : level;
        b = _testPattern == "warm" ? level * 18 / 255 : level;
        return true;
    }
    return true;
}
#endif
