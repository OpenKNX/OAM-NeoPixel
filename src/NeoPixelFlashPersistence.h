#pragma once

#include "OpenKNX.h"
#include <stdint.h>

// Forward declarations
class NeoPixelBusModule;
class Segment;

/**
 * @brief Flash persistence handler for NeoPixel LED states
 * 
 * Handles saving and restoring of segment states (colors, brightness, effects)
 * to/from flash memory. Works with OGM-Common automatic save system.
 * 
 * Storage: 10 bytes per segment × 16 segments = 160 bytes max
 */
class NeoPixelFlashPersistence
{
public:
    // OGM-Common logging support
    const std::string logPrefix() const { return "NeoPixelFlashPersistence"; }
    
    /**
     * @brief Flash storage structure for one segment (10 bytes)
     */
    struct SegmentFlashState {
        uint8_t power;           // 1 = on, 0 = off (1 byte)
        uint8_t r, g, b, w;      // RGBW color values (4 bytes)
        uint8_t brightness;      // Brightness 0-255 (1 byte)
        uint8_t effectType;      // Effect ID 0-10 (1 byte)
        uint8_t effectSpeed;     // Effect speed 0-255 (1 byte)
        uint8_t effectIntensity; // Effect intensity 0-255 (1 byte)
        // Total: 10 bytes per segment
    } __attribute__((packed));

    /**
     * @brief Constructor
     * @param module Pointer to NeoPixelBusModule parent
     */
    explicit NeoPixelFlashPersistence(NeoPixelBusModule* module);

    /**
     * @brief Calculate flash size needed for storing segment states
     * @return Number of bytes required (10 bytes × configured segments)
     */
    uint16_t calculateFlashSize() const;

    /**
     * @brief Write current LED states to flash
     * Called automatically by OGM-Common (manual save, periodic save)
     */
    void writeToFlash();

    /**
     * @brief Read LED states from flash
     * Called automatically by OGM-Common on startup
     * @param data Flash data buffer (managed by OGM-Common)
     * @param size Size of data buffer
     */
    void readFromFlash(const uint8_t* data, uint16_t size);

    /**
     * @brief Restore LED states after startup delay
     * Called by module after startup delay expires
     */
    void restoreStatesAfterStartup();

private:
    NeoPixelBusModule* _module;  // Parent module reference

    /**
     * @brief Save current segment state to flash structure
     * @param segmentIndex Segment index (0-based)
     * @param state Output structure to fill
     * @return true if state was saved successfully
     */
    bool saveSegmentState(uint8_t segmentIndex, SegmentFlashState& state);

    /**
     * @brief Restore segment state after startup
     * @param segmentIndex Segment index (0-based)
     * @param state Flash structure to restore from
     */
    void restoreSegmentState(uint8_t segmentIndex, const SegmentFlashState& state);
};
