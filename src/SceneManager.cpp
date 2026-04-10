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
    // SceneCount is 4 bits at BitOffset 0
    return knx.paramByte(addr) & 0x0F;
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
        logInfoP("Loaded %d scene effect parameter(s) for effect '%s'",
                 effect->getParameterCount(), effect->getName());
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
