#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

// Forward declarations
class NeoPixelBusModule;
class Segment;

/**
 * @brief Manages color correction, global brightness, and HCL color temperature
 *
 * This class handles:
 * - Global brightness control across all segments
 * - HCL (Human Centric Lighting) color temperature control
 * - Gamma correction configuration
 * - White balance configuration
 * - Color correction updates
 */
class ColorManagement
{
  public:
    /**
     * @brief Constructor
     * @param module Pointer to parent NeoPixelBusModule
     */
    explicit ColorManagement(NeoPixelBusModule* module);

    /**
     * @brief Destructor
     */
    ~ColorManagement() = default;

    /**
     * @brief Provides prefix for logger macros
     */
    inline const std::string logPrefix() { return "ColorMgmt"; }

    // ============================================================================
    // Global Brightness Control
    // ============================================================================

    /**
     * @brief Apply global brightness to all segments
     * @param brightness Brightness value (0-255, where 255 = restore original)
     */
    void applyGlobalBrightness(uint8_t brightness);

    /**
     * @brief Restore original brightness levels for all segments
     */
    void restoreOriginalBrightness();

    // ============================================================================
    // HCL Color Temperature Control
    // ============================================================================

    /**
     * @brief Set target HCL color temperature in Kelvin.
     *
     * This enables "true HCL" post-processing: we do NOT overwrite the active
     * effect/scenes. Instead we post-process the rendered pixels and apply a
     * Kelvin whitepoint mainly to white/low-saturation pixels (configurable).
     *
     * @param kelvin Color temperature in Kelvin (0 disables HCL; typical 1000..10000)
     */
    void applyHclColorTemperature(uint16_t kelvin);

    /**
     * @brief Disable HCL mode and restore original colors
     */
    void disableHclMode();

    /**
     * @brief Apply HCL post-processing to rendered pixels ("true HCL").
     *
     * Call this once per frame AFTER effects/scenes rendered their pixels.
     */
    void applyHclPostProcess();

    // ============================================================================
    // Color Correction Configuration
    // ============================================================================

    /**
     * @brief Configure color correction from ETS parameters
     */
    void configureColorCorrection();

    /**
     * @brief Update color correction parameters on VirtualStrip
     */
    void updateColorCorrection();

    /**
     * @brief Force immediate color correction update bypassing rate limiting
     */
    void forceColorCorrectionUpdate();

    // ============================================================================
    // Color Correction Helpers
    // ============================================================================

    /**
     * @brief Map ETS gamma parameter to gamma value
     * @param paramValue ETS parameter value (0-10)
     * @return Gamma value (2.0-3.0)
     */
    static float mapGammaValue(uint8_t paramValue);

    /**
     * @brief Map ETS white balance parameter to RGB value
     * @param paramValue ETS parameter value (0-100)
     * @return RGB value (0-255)
     */
    static uint8_t mapWhiteBalanceValue(uint8_t paramValue);

  private:
    NeoPixelBusModule* _module; ///< Pointer to parent NeoPixelBusModule

    // HCL state
    bool _hclEnabled = false;
    uint16_t _hclTargetKelvin = 0;
    uint16_t _hclAppliedKelvin = 0;
    unsigned long _lastHclApplyMs = 0;
    uint32_t _hclSlewAccumulatorMs = 0; ///< Accumulated time for fractional K steps
};