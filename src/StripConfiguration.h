#pragma once

#include "NeoPixel.h" // For LedProtocol, ColorOrder
#include <cstdint>

/**
 * @brief ETS enum mapping helpers used by NeoPixelBusModule.
 */
class StripConfiguration
{
  public:
    /**
     * @brief Map ETS protocol parameter to LedProtocol enum
     * @param paramLedType ETS parameter value
     * @return LedProtocol enum value
     */
    static LedProtocol mapProtocol(uint8_t paramLedType);

    /**
     * @brief Map ETS color order parameter to ColorOrder enum
     * @param c ETS parameter value
     * @return ColorOrder enum value
     */
    static ColorOrder mapColorOrder(uint8_t c);

    /**
     * @brief Check if protocol requires SPI
     * @param protocol LedProtocol to check
     * @return true if SPI protocol, false otherwise
     */
    static bool isSpiProtocol(LedProtocol protocol);
};
