#include "StripConfiguration.h"

LedProtocol StripConfiguration::mapProtocol(uint8_t p)
{
    switch (p)
    {
        case 0: return LedProtocol::WS2812B;
        case 1: return LedProtocol::WS2805_RGBCCT;
        case 2: return LedProtocol::WS2811;
        case 3: return LedProtocol::WS2813;
        case 4: return LedProtocol::SK6812;
        case 5: return LedProtocol::APA102;
        case 6: return LedProtocol::SK9822;
        case 7: return LedProtocol::WS2812B;
        case 8: return LedProtocol::SK6812;
        case 9: return LedProtocol::TM1814;
        case 10: return LedProtocol::WS2811_400KHZ;
        case 11: return LedProtocol::TM1829;
        case 12: return LedProtocol::UCS8903;
        case 13: return LedProtocol::APA106;
        case 14: return LedProtocol::TM1914;
        case 15: return LedProtocol::FW1906;
        case 16: return LedProtocol::UCS8904;
        case 17: return LedProtocol::WS2805_RGBCCT;
        case 18: return LedProtocol::SM16825;
        case 19: return LedProtocol::WS2811;
        case 20: return LedProtocol::WS2812B;
        case 21: return LedProtocol::WS2801;
        case 22: return LedProtocol::LPD8806;
        case 23: return LedProtocol::LPD6803;
        case 24: return LedProtocol::P9813;
        case 25: return LedProtocol::APA102_CLONE;
        case 30: return LedProtocol::SK6812_RGBCCT;
        case 31: return LedProtocol::WS2814_RGBCCT;
        case 99: return LedProtocol::WS2812B;
        default: return LedProtocol::WS2812B;
    }
}

ColorOrder StripConfiguration::mapColorOrder(uint8_t c)
{
    switch (c)
    {
        case 0: return ColorOrder::GRB;
        case 1: return ColorOrder::RGB;
        case 2: return ColorOrder::BRG;
        case 3: return ColorOrder::RBG;
        case 4: return ColorOrder::BGR;
        case 5: return ColorOrder::GBR;
        case 6: return ColorOrder::RGBW;
        case 7: return ColorOrder::GRBW;
        case 8: return ColorOrder::RGBCCT;
        case 9: return ColorOrder::GRBCCT;
        case 10: return ColorOrder::RGBCTW;
        case 11: return ColorOrder::GRBCTW;
        case 12: return ColorOrder::WRGB;
        case 13: return ColorOrder::NONE; // Chip default
        default: return ColorOrder::GRB;
    }
}

bool StripConfiguration::isSpiProtocol(LedProtocol protocol)
{
    switch (protocol)
    {
        case LedProtocol::APA102:
        case LedProtocol::SK9822:
        case LedProtocol::WS2801:
        case LedProtocol::LPD8806:
            return true;
        default:
            return false;
    }
}
