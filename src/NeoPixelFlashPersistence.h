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
 * Storage: 10 bytes per segment × 16 segments = 160 bytes max (+ relay state)
 */
class NeoPixelFlashPersistence
{
  public:
    // OGM-Common logging support
    const std::string logPrefix() const { return "NeoPixelFlashPersistence"; }

    /**
     * @brief Flash state structure for segment persistence (10 bytes)
     * 
     * Phase 1 (NOW): Stores only power, color, brightness
     *   - Effect type/parameters always come from ETS
     * 
     * Phase 2 (FUTURE): Will store KO-changed effect parameters
     *   - reserved1: Bitflags for which params changed via KO
     *   - reserved2-3: Effect parameter values when changed via KO
     */
    struct SegmentFlashState
    {
        uint8_t power;      // 0 = off, 1 = on
        uint8_t r, g, b;    // RGB color (0-255 each)
        uint8_t ww, cw;     // Warm/Cool white (0-255 each)
        uint8_t brightness; // Master brightness (0-255)
        
        // Reserved for Phase 2: KO-based effect parameter persistence
        uint8_t reserved1;  // Future: effectParamsChangedFlags (bitfield)
        uint8_t reserved2;  // Future: effect parameter value 1
        uint8_t reserved3;  // Future: effect parameter value 2
        // Total: 10 bytes per segment
    } __attribute__((packed));

    /**
     * @brief Flash state structure for external relay persistence
     * Stores on/off state bitmask and count
     */
    struct RelayFlashState
    {
        uint8_t count;        // Number of relays configured (0..kMax)
        uint8_t statesMask;   // Bitmask: bit0=Relay1, bit1=Relay2, ...
        uint8_t signature;    // 0xA5 when valid
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
    NeoPixelBusModule* _module; // Parent module reference
    uint8_t _channelIndex = 0;  // Current channel index for ParamNEO_NEO* macros
    RelayFlashState _relayFlashState = {0, 0, 0};
    bool _relayFlashValid = false;

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
