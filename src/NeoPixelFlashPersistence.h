#pragma once

#include "OpenKNX.h"
#include "SceneManager.h"
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
 * Storage: 19 bytes per segment × 16 segments = 304 bytes max (+ relay state)
 */
class NeoPixelFlashPersistence
{
  public:
    // OGM-Common logging support
    const std::string logPrefix() const { return "NeoPixelFlashPersistence"; }

    /**
     * @brief Flash state structure for segment persistence (19 bytes)
     *
     * Stores KO-changed values to restore runtime state after power cycle:
     *   - ETS provides BASE configuration (defaults)
     *   - KOs provide RUNTIME overrides (primary/secondary colour, brightness, effect, etc.)
     *   - Flash stores KO-CHANGED values for restoration
     *
     * validFlags bitfield indicates which values were changed via KO:
     *   0x01 = power      (KoNEO_SegmentPower)
     *   0x02 = colors     (primary/secondary colour state)
     *   0x04 = brightness (KoNEO_SegmentBrightness)
     *   0x08 = effectType (KoNEO_Fx)
     *   0x10 = effectFlags (effect state tracking)
     *   0x20-0x80 = reserved for HSV, CCT, effect parameters
     */
    struct SegmentFlashState
    {
        uint8_t version;    // = FLASH_FORMAT_VERSION; validated on read (discard on mismatch)
        uint8_t validFlags; // Bitfield: which values are valid/changed via KO

        // KO-changed values (restored if corresponding validFlags bit set):
        uint8_t power;   // 0 = off, 1 = on (KoNEO_SegmentPower)
        uint8_t r, g, b; // RGB color (KoNEO_R/G/B)
        uint8_t ww, cw;  // Warm/Cool white (reserved for future KOs)
        uint8_t secondaryR, secondaryG, secondaryB;
        uint8_t secondaryWW, secondaryCW;
        uint8_t brightness;  // Master brightness (KoNEO_SegmentBrightness)
        uint8_t effectType;  // Effect ID (KoNEO_Fx) - 0 = no effect
        uint8_t effectFlags; // bit0=effectValid, bit1=lastWasEffect

        // Reserved for future KO parameters (HSV, CCT, effect params):
        uint8_t reserved[3]; // reserved[0] = last active scene number (0 = no scene)
                             // reserved[1] = last active Effektmanager ID (0 = none)
                             // reserved[2] = reserved
        //
        // BACKWARD COMPATIBILITY WITHIN FORMAT 0x02:
        //   These bytes were repurposed from the zero-initialized reserved[] pool.
        //   Saves written by earlier 0x02 firmware had reserved[]=0, and 0 is a
        //   valid "none" sentinel for BOTH fields (scene 0 = no scene, EM_NONE == 0).
        //   Restoring such a blob therefore yields "no scene / no EM" with no data loss.
        //   IMPORTANT: if the meaning of any reserved[] byte changes in a way where
        //   0 is no longer a safe default, bump FLASH_FORMAT_VERSION and gate the
        //   read on it before reinterpreting old blobs.

        // Total: 19 bytes per segment × 16 segments = 304 bytes max
    } __attribute__((packed));

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

    struct SceneFlashHeader
    {
        uint8_t signature[4];
        uint8_t version;
        uint8_t segmentCount;
        uint8_t maxScenes;
        uint8_t sceneSize;
    } __attribute__((packed));

    struct SceneSegmentFlashState
    {
        uint8_t payload[1 + SceneManager::MAX_SCENES * SceneManager::SCENE_SIZE];
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
     * @return Number of bytes required (19 bytes × configured segments)
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

    /**
     * @brief Clear persisted runtime and scene snapshots for an ETS download.
     *
     * The next flash save writes an empty runtime snapshot and an invalid scene
     * snapshot header so the next boot uses the freshly programmed ETS state.
     */
    void invalidateForEtsDownload();

  private:
    NeoPixelBusModule* _module; // Parent module reference
    uint8_t _channelIndex = 0;  // Current channel index for ParamNEO_NEO* macros
    RelayFlashState _relayFlashState = {0, 0, 0};
    bool _relayFlashValid = false;
    bool _invalidateForEtsDownload = false;

    /**
     * @brief Save current segment state to flash structure
     * @param segmentIndex Segment index (0-based)
     * @param state Output structure to fill
     * @return true if state was saved successfully
     */
    bool saveSegmentState(uint8_t segmentIndex, SegmentFlashState& state);

    void writeSceneStatesToFlash(bool invalidateSnapshot);
    bool readSceneStatesFromFlash(const uint8_t* data, uint16_t size, uint16_t offset, uint16_t segmentCount);

    /**
     * @brief Restore segment state after startup
     * @param segmentIndex Segment index (0-based)
     * @param state Flash structure to restore from
     */
    void restoreSegmentState(uint8_t segmentIndex, const SegmentFlashState& state);
};
