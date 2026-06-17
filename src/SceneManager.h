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
 * - Effect text (14 bytes, DPT 16.001)
 *
 * Uses computed byte offsets for generic scene access (no per-scene switch/case).
 * Scene data is stored in the segment Union at offsets 160-519 (10 x 36 bytes).
 */
class SceneManager
{
  public:
    inline const std::string logPrefix() { return "SceneMgr"; }

    // Memory layout constants (must match NeoPixel.Segment.templ.xml and NeoPixel.Scene.part.xml)
    static constexpr uint16_t SCENE_DATA_START = 160;        // First scene at offset 160 within Union
    static constexpr uint8_t SCENE_SIZE = 36;                // Bytes per scene (V3: 12 fixed + 10 slots + 14 text)
    static constexpr uint8_t SCENE_COUNT_OFFSET = 159;       // SceneCount param offset within Union
    static constexpr uint8_t SCENE_EFFECT_PARAM_OFFSET = 12; // Effect params at byte 12 within scene

    // Scene field offsets within the 36-byte scene slot
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
    static constexpr uint8_t FIELD_EFFECT_TEXT = 22; // 14 bytes, DPT 16.001
    static constexpr uint8_t EFFECT_TEXT_SIZE = 14;

    // EffectType values >= EM_ACTION_BASE start an Effektmanager instead of an effect
    static constexpr uint8_t EM_ACTION_BASE = 200; // 200 = EM 1 ... 215 = EM 16

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

    /**
     * @brief Store the current segment state into a scene slot (DPT 18.001 learn)
     *
     * Captures the segment's current effect type, primary/secondary colors (RGBWWCW),
     * brightness, and effect-specific parameters, writing them into the scene's
     * parameter memory. Changes are in RAM only until flash is persisted.
     *
     * @param channelIndex Segment index (0-15)
     * @param sceneNumber Scene number (1-based, as received via DPT 18.001)
     * @param segment Pointer to source segment
     * @return true if scene was stored, false if invalid scene number
     */
    bool storeScene(uint8_t channelIndex, uint8_t sceneNumber, Segment* segment);

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
     * @param fieldOffset Field offset within the 36-byte scene slot
     * @return Field value (uint8_t)
     */
    uint8_t readSceneField(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset);

    /**
     * @brief Write a single byte field to a scene slot
     *
     * @param channelIndex Segment index (0-15)
     * @param sceneIndex Scene index (0-based)
     * @param fieldOffset Field offset within the 36-byte scene slot
     * @param value Value to write
     */
    void writeSceneField(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset, uint8_t value);

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

    /**
     * @brief Write a 3-byte RGB color to a scene slot
     *
     * @param channelIndex Segment index
     * @param sceneIndex Scene index (0-based)
     * @param fieldOffset Start offset of the 3-byte RGB field
     * @param r Red
     * @param g Green
     * @param b Blue
     */
    void writeSceneColorRGB(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset,
                            uint8_t r, uint8_t g, uint8_t b);
};
