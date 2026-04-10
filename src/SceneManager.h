#pragma once

#include "OpenKNX.h"
#include "Segment.h"
#include "effects/Effect.h"
#include "knxprod.h"
#include <stdint.h>

// Forward declarations
class NeoPixelBusModule;

/**
 * @brief Scene/Preset Manager for ETS-configurable lighting scenes
 *
 * Manages up to 10 ETS-configured scenes per segment. Each scene stores:
 * - Effect type, primary/secondary colors (RGBWWCW), brightness
 * - Effect-specific parameters (10 bytes, auto-generated)
 *
 * Uses computed byte offsets for generic scene access (no per-scene switch/case).
 * Scene data is stored in the segment Union at offsets 140-359.
 */
class SceneManager
{
  public:
    inline const std::string logPrefix() { return "SceneMgr"; }

    // Memory layout constants (must match NeoPixel.Segment.templ.xml and NeoPixel.Scene.part.xml)
    static constexpr uint16_t SCENE_DATA_START = 140;        // First scene at offset 140 within Union
    static constexpr uint8_t SCENE_SIZE = 22;                // Bytes per scene
    static constexpr uint8_t SCENE_COUNT_OFFSET = 137;       // SceneCount param offset within Union
    static constexpr uint8_t SCENE_EFFECT_PARAM_OFFSET = 12; // Effect params at byte 12 within scene

    // Scene field offsets within the 22-byte scene slot
    static constexpr uint8_t FIELD_EFFECT_TYPE = 0;
    static constexpr uint8_t FIELD_PRIMARY_R = 1;
    static constexpr uint8_t FIELD_PRIMARY_G = 2;
    static constexpr uint8_t FIELD_PRIMARY_B = 3;
    static constexpr uint8_t FIELD_PRIMARY_WW = 4;
    static constexpr uint8_t FIELD_PRIMARY_CW = 5;
    static constexpr uint8_t FIELD_SECONDARY_R = 6;
    static constexpr uint8_t FIELD_SECONDARY_G = 7;
    static constexpr uint8_t FIELD_SECONDARY_B = 8;
    static constexpr uint8_t FIELD_SECONDARY_WW = 9;
    static constexpr uint8_t FIELD_SECONDARY_CW = 10;
    static constexpr uint8_t FIELD_BRIGHTNESS = 11;

    explicit SceneManager(NeoPixelBusModule* module);
    ~SceneManager() = default;

    /**
     * @brief Recall a scene by number for a specific segment
     *
     * @param channelIndex Segment index (0-15)
     * @param sceneNumber Scene number (1-based, as sent via Preset KO)
     * @param segment Pointer to target segment
     * @return true if scene was applied, false if invalid scene number
     */
    bool recallScene(uint8_t channelIndex, uint8_t sceneNumber, Segment* segment);

    /**
     * @brief Get the configured scene count for a segment
     *
     * @param channelIndex Segment index (0-15)
     * @return Number of configured scenes (0-10)
     */
    uint8_t getSceneCount(uint8_t channelIndex);

  private:
    NeoPixelBusModule* _module;

    /**
     * @brief Read a single byte field from a scene slot
     *
     * Computes the absolute EEPROM address generically:
     *   NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize
     *   + SCENE_DATA_START + sceneIndex * SCENE_SIZE + fieldOffset
     *
     * @param channelIndex Segment index (0-15)
     * @param sceneIndex Scene index (0-based)
     * @param fieldOffset Field offset within the 22-byte scene slot
     * @return Field value (uint8_t)
     */
    uint8_t readSceneField(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset);

    /**
     * @brief Read a 3-byte RGB color from a scene slot (for TypeColor RGB)
     *
     * @param channelIndex Segment index
     * @param sceneIndex Scene index (0-based)
     * @param fieldOffset Start offset of the 3-byte RGB field
     * @param r Output red
     * @param g Output green
     * @param b Output blue
     */
    void readSceneColorRGB(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset,
                           uint8_t& r, uint8_t& g, uint8_t& b);
};
