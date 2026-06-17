#include "SceneManager.h"
#include "NeoPixelModule.h"
#include "OpenKNX.h"
#include "knxprod.h"

// Include auto-generated effect parameter mappings (for loadSceneEffectParameters)
#if defined(__has_include)
    #if __has_include("EffectParameterMapping.h")
        #define SCENE_EFFECT_PARAMETER_MAPPING_GENERATED
        #include "EffectParameterMapping.h"
    #endif
#endif

// Include auto-generated effect type mapping (for getEffectFromType)
#include "EffectTypeMapping.h"

SceneManager::SceneManager(NeoPixelBusModule* module)
    : _module(module)
{
}

uint8_t SceneManager::getSceneCount(uint8_t channelIndex)
{
    uint16_t addr = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_COUNT_OFFSET;
    return (knx.paramByte(addr) & 0xF0) >> 4;
}

uint8_t SceneManager::readSceneField(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset)
{
    uint16_t addr = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_DATA_START + sceneIndex * SCENE_SIZE + fieldOffset;
    return knx.paramByte(addr);
}

void SceneManager::readSceneColorRGB(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset,
                                     uint8_t& r, uint8_t& g, uint8_t& b)
{
    // TypeColor RGB is stored as 3 consecutive bytes (R, G, B) - OpenKNXproducer format
    // Note: PT-ColorRGB in ETS stores as 0x00RRGGBB (24-bit), but in Union it's 3 bytes
    uint16_t baseAddr = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_DATA_START + sceneIndex * SCENE_SIZE + fieldOffset;

    // Read the 3-byte color value (stored as big-endian RGB in EEPROM)
    r = knx.paramByte(baseAddr);
    g = knx.paramByte(baseAddr + 1);
    b = knx.paramByte(baseAddr + 2);
}

void SceneManager::writeSceneField(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset, uint8_t value)
{
    uint16_t addr = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_DATA_START + sceneIndex * SCENE_SIZE + fieldOffset;
    uint8_t* data = knx.paramData(addr);
    if (data) *data = value;
}

void SceneManager::writeSceneColorRGB(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset,
                                      uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t baseAddr = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_DATA_START + sceneIndex * SCENE_SIZE + fieldOffset;
    uint8_t* data = knx.paramData(baseAddr);
    if (data)
    {
        data[0] = r;
        data[1] = g;
        data[2] = b;
    }
}

bool SceneManager::storeScene(uint8_t channelIndex, uint8_t sceneNumber, Segment* segment)
{
    if (!segment)
    {
        logErrorP("storeScene: segment is null for channel %d", channelIndex);
        return false;
    }

    uint8_t sceneCount = getSceneCount(channelIndex);
    if (sceneNumber < 1 || sceneNumber > sceneCount)
    {
        logInfoP("storeScene: Scene %d out of range for segment %d (configured: %d scenes)",
                 sceneNumber, channelIndex, sceneCount);
        return false;
    }

    uint8_t sceneIndex = sceneNumber - 1; // Convert to 0-based

    // Capture current effect type — or, if an Effektmanager is running, store it as EM action
    Effect* effect = segment->getEffect();
    uint8_t effectType = getTypeFromEffect(effect);
    bool emRunning = false;
    {
        auto& segments = _module->getSegments();
        if (channelIndex < segments.size() && segments[channelIndex].emController.isRunning())
        {
            uint8_t emId = segments[channelIndex].emController.activeEmId();
            if (emId >= 1 && emId <= EM_COUNT)
            {
                effectType = EM_ACTION_BASE + emId - 1;
                emRunning = true;
            }
        }
    }

    // Capture current colors from segment config
    const EffectConfig& config = segment->getConfig();
    uint8_t primaryR = config.r();
    uint8_t primaryG = config.g();
    uint8_t primaryB = config.b();
    uint8_t primaryWW = config.ww();
    uint8_t primaryCW = config.cw();
    uint8_t secondaryR = config.r2();
    uint8_t secondaryG = config.g2();
    uint8_t secondaryB = config.b2();
    uint8_t secondaryWW = config.ww2();
    uint8_t secondaryCW = config.cw2();
    uint8_t brightness = segment->getBrightness();

    logInfoP("storeScene %d for segment %d: Effect=%d, Primary=(%d,%d,%d,WW=%d,CW=%d), "
             "Secondary=(%d,%d,%d,WW=%d,CW=%d), Brightness=%d",
             sceneNumber, channelIndex, effectType,
             primaryR, primaryG, primaryB, primaryWW, primaryCW,
             secondaryR, secondaryG, secondaryB, secondaryWW, secondaryCW,
             brightness);

    // Write scene fields to parameter memory
    writeSceneField(channelIndex, sceneIndex, FIELD_EFFECT_TYPE, effectType);
    writeSceneColorRGB(channelIndex, sceneIndex, FIELD_PRIMARY_R, primaryR, primaryG, primaryB);
    writeSceneField(channelIndex, sceneIndex, FIELD_PRIMARY_WW, primaryWW);
    writeSceneField(channelIndex, sceneIndex, FIELD_PRIMARY_CW, primaryCW);
    writeSceneColorRGB(channelIndex, sceneIndex, FIELD_SECONDARY_R, secondaryR, secondaryG, secondaryB);
    writeSceneField(channelIndex, sceneIndex, FIELD_SECONDARY_WW, secondaryWW);
    writeSceneField(channelIndex, sceneIndex, FIELD_SECONDARY_CW, secondaryCW);
    writeSceneField(channelIndex, sceneIndex, FIELD_BRIGHTNESS, brightness);

    // Write effect-specific parameters (up to 10 bytes at offset 12) — not for EM actions
    if (effect && !emRunning)
    {
        uint8_t paramCount = effect->getParameterCount();
        logInfoP("Storing effect '%s' with %d parameter(s):", effect->getName(), paramCount);
        uint16_t effectParamBase = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_DATA_START + sceneIndex * SCENE_SIZE + SCENE_EFFECT_PARAM_OFFSET;
        for (uint8_t i = 0; i < paramCount && i < 10; i++)
        {
            uint32_t paramValue = effect->getParameter(segment, i);
            logInfoP("  param[%d] '%s' = %lu", i, effect->getParameterName(i), (unsigned long)paramValue);
            uint8_t* data = knx.paramData(effectParamBase + i);
            if (data) *data = (uint8_t)(paramValue & 0xFF);
        }
    }

    // Request flash save to persist the changed parameter table
    knx.writeMemory();

    return true;
}

bool SceneManager::recallScene(uint8_t channelIndex, uint8_t sceneNumber, Segment* segment)
{
    if (!segment)
    {
        logErrorP("recallScene: segment is null for channel %d", channelIndex);
        return false;
    }

    uint8_t sceneCount = getSceneCount(channelIndex);
    if (sceneNumber < 1 || sceneNumber > sceneCount)
    {
        logInfoP("Scene %d out of range for segment %d (configured: %d scenes)",
                 sceneNumber, channelIndex, sceneCount);
        return false;
    }

    uint8_t sceneIndex = sceneNumber - 1; // Convert to 0-based

    // Read scene data from EEPROM using computed byte offsets
    uint8_t effectType = readSceneField(channelIndex, sceneIndex, FIELD_EFFECT_TYPE);

    // EM action: scene starts an Effektmanager instead of applying a static effect
    if (effectType >= EM_ACTION_BASE && effectType < EM_ACTION_BASE + EM_COUNT)
    {
        uint8_t emId = effectType - EM_ACTION_BASE + 1;
        logInfoP("Scene %d for segment %d: starting Effektmanager %d", sceneNumber, channelIndex, emId);
        _module->startEffektManager(channelIndex, emId);
        return true;
    }

    // Read primary color (RGB + WW + CW)
    uint8_t primaryR, primaryG, primaryB;
    readSceneColorRGB(channelIndex, sceneIndex, FIELD_PRIMARY_R, primaryR, primaryG, primaryB);
    uint8_t primaryWW = readSceneField(channelIndex, sceneIndex, FIELD_PRIMARY_WW);
    uint8_t primaryCW = readSceneField(channelIndex, sceneIndex, FIELD_PRIMARY_CW);

    // Read secondary color (RGB + WW + CW)
    uint8_t secondaryR, secondaryG, secondaryB;
    readSceneColorRGB(channelIndex, sceneIndex, FIELD_SECONDARY_R, secondaryR, secondaryG, secondaryB);
    uint8_t secondaryWW = readSceneField(channelIndex, sceneIndex, FIELD_SECONDARY_WW);
    uint8_t secondaryCW = readSceneField(channelIndex, sceneIndex, FIELD_SECONDARY_CW);

    // Read brightness
    uint8_t brightness = readSceneField(channelIndex, sceneIndex, FIELD_BRIGHTNESS);

    logInfoP("Scene %d for segment %d: Effect=%d, Primary=(%d,%d,%d,WW=%d,CW=%d), "
             "Secondary=(%d,%d,%d,WW=%d,CW=%d), Brightness=%d",
             sceneNumber, channelIndex, effectType,
             primaryR, primaryG, primaryB, primaryWW, primaryCW,
             secondaryR, secondaryG, secondaryB, secondaryWW, secondaryCW,
             brightness);

    // A scene is an authoritative "set this state NOW" command. Stop any running
    // Effektmanager on this segment first, otherwise its per-frame tick() would
    // overwrite the static scene we are about to apply on the next loop.
    // (The EM-action scene path above returns earlier and starts its own EM.)
    _module->stopEffektManager(channelIndex);

    // Reset config options/features to defaults before applying the new effect.
    // Without this, leftover values from the previous effect bleed through
    // (e.g. Cylon's EyeSize in option2 becomes Rainbow's saturation).
    {
        EffectConfig& cfg = segment->getConfig();
        cfg.option1 = 0;
        cfg.option2 = 0;
        cfg.option3 = 0;
        cfg.feature1 = false;
        cfg.feature2 = false;
        cfg.feature3 = false;
        cfg.reverse = 0;
        cfg.fade = 0;
        cfg.count = 0;
        cfg.mode = 0;
    }

    // Apply effect type using existing infrastructure
    _module->applyEffectToSegment(segment, effectType);

    // Apply primary color
    segment->setPrimaryColor(primaryR, primaryG, primaryB, primaryWW);
    // Note: CW channel - if supported by segment, set via extended API
    // TODO: Add CW support to Segment::setPrimaryColor if needed

    // Apply secondary color
    EffectConfig& config = segment->getConfig();
    config.secondaryRGBW = ((uint32_t)secondaryR << 24) | ((uint32_t)secondaryG << 16) | ((uint32_t)secondaryB << 8) | secondaryWW;

    // Apply brightness
    segment->setBrightness(brightness);

    // Load scene-specific effect parameters (auto-generated)
#ifdef SCENE_EFFECT_PARAMETER_MAPPING_GENERATED
    Effect* effect = segment->getEffect();
    if (effect && effect->getParameterCount() > 0)
    {
        loadSceneEffectParameters(effect, segment, effectType, channelIndex, sceneIndex);
        uint8_t paramCount = effect->getParameterCount();
        logInfoP("Effect '%s' configured with %d parameter(s):", effect->getName(), paramCount);
        for (uint8_t i = 0; i < paramCount; i++)
        {
            logInfoP("  param[%d] '%s' = %lu", i, effect->getParameterName(i), (unsigned long)effect->getParameter(segment, i));
        }
    }
#endif

    // Update saved state in SegmentConfig for flash persistence
    auto& segments = _module->getSegments();
    if (channelIndex < segments.size())
    {
        auto& cfg = segments[channelIndex];
        cfg.savedR = primaryR;
        cfg.savedG = primaryG;
        cfg.savedB = primaryB;
        cfg.savedWW = primaryWW;
        cfg.savedCW = primaryCW;
        cfg.savedBrightness = brightness;
        cfg.savedEffectType = effectType;
        cfg.savedEffectValid = (effectType > 0);
        cfg.savedValid = true;
    }

    return true;
}
