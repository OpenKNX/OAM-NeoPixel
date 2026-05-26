#pragma once

#include "OpenKNX.h"
#include "Segment.h"
#include "effects/Effect.h"
#include "knxprod.h"
#include <stdint.h>
#include <vector>

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
 * ETS scene data is read once from the segment Union and then kept in an
 * OAM-owned runtime cache. Web UI changes must not write into KNX table memory
 * on ESP32 IP targets because that memory can be flash-backed.
 */
class SceneManager
{
  public:
    inline const std::string logPrefix() { return "SceneMgr"; }

    static constexpr uint8_t MAX_SCENES = 10;

    struct SceneSnapshot
    {
        uint8_t effectType = 0;
        uint8_t primaryR = 0;
        uint8_t primaryG = 0;
        uint8_t primaryB = 0;
        uint8_t primaryWW = 0;
        uint8_t primaryCW = 0;
        uint8_t secondaryR = 0;
        uint8_t secondaryG = 0;
        uint8_t secondaryB = 0;
        uint8_t secondaryWW = 0;
        uint8_t secondaryCW = 0;
        uint8_t brightness = 0;
    };

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
     * @brief Initialize the runtime scene cache from ETS parameter memory.
     *
     * Call this after the KNX parameter table has been restored and before
     * module flash snapshots can be saved. Later module flash restore may
     * override this cache with Web UI persisted scenes.
     */
    void initializeFromKnxParameters(uint8_t segmentCount);

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
     * @brief Read a configured scene into a plain snapshot structure
     *
     * @param channelIndex Segment index (0-15)
     * @param sceneNumber Scene number (1-based)
     * @param snapshot Output structure that receives the scene fields
     * @return true if the scene exists and was read, false otherwise
     */
    bool tryGetSceneSnapshot(uint8_t channelIndex, uint8_t sceneNumber, SceneSnapshot& snapshot);

    /**
     * @brief Append a new scene slot initialized from the current segment state
     *
     * @param channelIndex Segment index (0-15)
     * @param segment Pointer to source segment
     * @param newSceneNumber Receives the new 1-based scene number
     * @param flushMemory Clear the pending scene RAM write journal after the operation
     * @return true if the scene was appended, false otherwise
     */
    bool addScene(uint8_t channelIndex, Segment* segment, uint8_t& newSceneNumber, bool flushMemory = true);

    /**
     * @brief Delete the last configured scene slot
     *
     * Scene numbers are positional. Deleting a middle slot would renumber the
     * later scenes, so only the highest configured scene number can be removed.
     *
     * @param channelIndex Segment index (0-15)
     * @param sceneNumber Scene number (1-based); must match the highest configured scene
     * @param flushMemory Clear the pending scene RAM write journal after the operation
     * @return true if the scene was removed, false otherwise
     */
    bool deleteScene(uint8_t channelIndex, uint8_t sceneNumber, bool flushMemory = true);

    /**
     * @brief Export raw scene bytes for ETS sync
     *
     * The payload layout is: [sceneCount, scene1(22 bytes), scene2(22 bytes), ...]
     *
     * @param channelIndex Segment index (0-15)
     * @param data Output buffer
     * @param length Receives the payload length
     * @return true if the payload was written, false otherwise
     */
    bool exportScenes(uint8_t channelIndex, uint8_t* data, uint8_t& length);

    /**
     * @brief Store the current segment state into a scene slot (DPT 18.001 learn)
     *
     * Captures the segment's current effect type, primary/secondary colors (RGBWWCW),
     * brightness, and effect-specific parameters, writing them into the OAM
     * scene cache. Durable scene snapshots are written through the NeoPixel
     * module flash data, not through the KNX parameter flash partition.
     *
     * @param channelIndex Segment index (0-15)
     * @param sceneNumber Scene number (1-based, as received via DPT 18.001)
     * @param segment Pointer to source segment
     * @param flushMemory Clear the pending scene RAM write journal after the operation
     * @return true if scene was stored, false if invalid scene number
     */
    bool storeScene(uint8_t channelIndex, uint8_t sceneNumber, Segment* segment, bool flushMemory = true);

    bool importScenes(uint8_t channelIndex, const uint8_t* data, uint8_t length);
    /**
     * @brief Clear the pending scene RAM write journal.
     *
     * Scene persistence is handled by NeoPixel module flash snapshots so Web UI
     * scene changes do not touch the KNX parameter flash partition.
     */
    bool flushPendingSceneMemory();
    void discardPendingSceneMemory();

  private:
    NeoPixelBusModule* _module;

    struct PendingSceneMemoryWrite
    {
        uint16_t address = 0;
        uint8_t value = 0;
    };

    struct SceneCache
    {
        bool initialized = false;
        uint8_t sceneCount = 0;
        uint8_t data[MAX_SCENES * SCENE_SIZE] = {0};
    };

    std::vector<SceneCache> _sceneCaches;
    std::vector<PendingSceneMemoryWrite> _pendingSceneMemoryWrites;

    bool ensureSceneCache(uint8_t channelIndex);
    bool refreshSceneCacheFromKnxParameters(uint8_t channelIndex);
    SceneCache* sceneCache(uint8_t channelIndex);
    const SceneCache* sceneCache(uint8_t channelIndex) const;
    static uint16_t sceneFieldIndex(uint8_t sceneIndex, uint8_t fieldOffset);
    bool setSceneCount(uint8_t channelIndex, uint8_t sceneCount);
    bool clearSceneSlot(uint8_t channelIndex, uint8_t sceneIndex);
    bool copySceneSlot(uint8_t channelIndex, uint8_t sourceSceneIndex, uint8_t targetSceneIndex);
    bool writeParameterByte(uint16_t address, uint8_t value);
    bool writeParameterBytes(uint16_t address, const uint8_t* values, uint8_t length);
    void rememberSceneMemoryWrite(uint16_t address, uint8_t value);

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
     * @brief Write a single byte field to a scene slot
     *
     * @param channelIndex Segment index (0-15)
     * @param sceneIndex Scene index (0-based)
     * @param fieldOffset Field offset within the 22-byte scene slot
     * @param value Value to write
     */
    bool writeSceneField(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset, uint8_t value);

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
    bool writeSceneColorRGB(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset,
                            uint8_t r, uint8_t g, uint8_t b);
};
