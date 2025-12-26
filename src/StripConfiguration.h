#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "NeoPixel.h" // For LedProtocol, ColorOrder

// Forward declaration
class NeoPixelBusModule;

/**
 * @brief Manages physical and virtual LED strip configuration
 * 
 * This class handles:
 * - Reading ETS parameters and building physical/virtual strip layout
 * - Physical strip creation (SPI and 1-Wire protocols)
 * - Virtual strip creation with configurable ordering
 * - GPIO pin auto-allocation
 * - Strip ordering and merging configuration
 */
class StripConfiguration
{
public:
    /**
     * @brief Constructor
     * @param module Pointer to parent NeoPixelBusModule
     */
    explicit StripConfiguration(NeoPixelBusModule* module);

    /**
     * @brief Destructor
     */
    ~StripConfiguration() = default;

    /**
     * @brief Provides prefix for logger macros
     */
    inline const std::string logPrefix() { return "StripCfg"; }

    // ============================================================================
    // Main Configuration
    // ============================================================================

    /**
     * @brief Configure strips from ETS parameters
     * 
     * Main entry point that reads ETS params and builds complete physical + virtual layout:
     * - Creates physical strips with GPIO auto-allocation
     * - Configures virtual strip ordering
     * - Sets up segments
     * - Applies power management and color correction
     */
    void configureFromETS();

    // ============================================================================
    // Virtual Strip Ordering
    // ============================================================================

    /**
     * @brief Configure virtual strip order from ETS parameters
     */
    void configureVirtualStripOrder();

    /**
     * @brief Create virtual strip with user-defined order
     */
    void createVirtualStripWithOrder();

    /**
     * @brief Create default sequential strip order
     */
    void createDefaultVirtualStripOrder();

    // ============================================================================
    // Helper Functions (Static)
    // ============================================================================

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

private:
    NeoPixelBusModule* _module; ///< Pointer to parent NeoPixelBusModule
};
