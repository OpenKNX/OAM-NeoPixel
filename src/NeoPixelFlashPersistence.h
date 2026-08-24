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
 * Storage: 16 bytes per segment × 16 segments = 256 bytes max (+ relay state)
 */
class NeoPixelFlashPersistence
{
  public:
    // OGM-Common logging support
    const std::string logPrefix() const { return "NeoPixelFlashPersistence"; }

    /**
     * @brief Flash state structure for segment persistence (16 bytes)
     *
     * Stores KO-changed values to restore runtime state after power cycle:
     *   - ETS provides BASE configuration (defaults)
     *   - KOs provide RUNTIME overrides (R/G/B, Brightness, Effect, etc.)
     *   - Flash stores KO-CHANGED values for restoration
     *
     * validFlags bitfield indicates which values were changed via KO:
     *   0x01 = power      (KoNEO_SegmentPower)
     *   0x02 = rgb        (KoNEO_R/G/B)
     *   0x04 = brightness (KoNEO_SegmentBrightness)
     *   0x08 = effectType (KoNEO_Fx)
     *   0x10 = effectFlags (effect state tracking)
     *   0x20 = CCT Kelvin (KoNEO_CCT, DPT 7.600)
     *   0x40-0x80 = reserved for HSV and effect parameters
     */
    struct SegmentFlashState
    {
        uint8_t version;    // = FLASH_FORMAT_VERSION; validated on read (discard on mismatch)
        uint8_t validFlags; // Bitfield: which values are valid/changed via KO

        // KO-changed values (restored if corresponding validFlags bit set):
        uint8_t power;       // 0 = off, 1 = on (KoNEO_SegmentPower)
        uint8_t r, g, b;     // RGB color (KoNEO_R/G/B)
        uint8_t ww, cw;      // Warm/Cool white
        uint8_t brightness;  // Master brightness (KoNEO_SegmentBrightness)
        uint8_t effectType;  // Effect ID (KoNEO_Fx) - 0 = no effect
        uint8_t effectFlags; // bit0=effectValid, bit1=lastWasEffect

        uint8_t sceneNumber; // Last active scene number (0 = no scene)
        uint8_t lastEmId;    // Last active Effektmanager ID (0 = none)
        uint16_t cctKelvin;  // Exact last DPT 7.600 CCT command (0 = none)
        uint8_t reserved;    // Must remain zero; preserves a 16-byte record size

        // Total: 16 bytes per segment × 16 segments = 256 bytes max
    } __attribute__((packed));

    static_assert(sizeof(SegmentFlashState) == 16, "Segment flash layout must stay 16 bytes");

    /**
     * @brief Flash state structure for external relay persistence
     * Stores on/off state bitmask and count
     */
    struct RelayFlashState
    {
        uint8_t count;      // Number of relays configured (0..kMax)
        uint8_t statesMask; // Bitmask: bit0=Relay1, bit1=Relay2, ...
        uint8_t signature;  // 0xA5 when valid
    } __attribute__((packed));

    // Format version of the persisted SegmentFlashState layout. Written into every
    // record's `version` byte and validated on read. BUMP THIS whenever the meaning
    // of any byte changes (even if the total size stays the same) — a firmware
    // update will then DISCARD the now-incompatible saved state and fall back to
    // ETS/defaults, instead of misinterpreting old bytes (which caused "weird
    // behaviour" after firmware updates that kept the old size).
    static constexpr uint8_t FLASH_FORMAT_VERSION = 0x02;

    /**
     * @brief Constructor
     * @param module Pointer to NeoPixelBusModule parent
     */
    explicit NeoPixelFlashPersistence(NeoPixelBusModule* module);

    /**
     * @brief Calculate flash size needed for storing segment states
     * @return Number of bytes required (16 bytes × configured segments)
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
