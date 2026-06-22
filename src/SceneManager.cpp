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

void SceneManager::initializeFromKnxParameters(uint8_t segmentCount)
{
    _sceneCaches.clear();
    _sceneCaches.resize(segmentCount);
    for (uint8_t channelIndex = 0; channelIndex < segmentCount; ++channelIndex)
    {
        ensureSceneCache(channelIndex);
    }
}

bool SceneManager::ensureSceneCache(uint8_t channelIndex)
{
    if (_sceneCaches.size() <= channelIndex)
    {
        _sceneCaches.resize(static_cast<size_t>(channelIndex) + 1);
    }

    SceneCache& cache = _sceneCaches[channelIndex];
    if (cache.initialized)
    {
        return true;
    }

    return refreshSceneCacheFromKnxParameters(channelIndex);
}

bool SceneManager::refreshSceneCacheFromKnxParameters(uint8_t channelIndex)
{
    if (_sceneCaches.size() <= channelIndex)
    {
        _sceneCaches.resize(static_cast<size_t>(channelIndex) + 1);
    }

    SceneCache& cache = _sceneCaches[channelIndex];

    const uint16_t countAddress = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_COUNT_OFFSET;
    const uint8_t configuredSceneCount = static_cast<uint8_t>((knx.paramByte(countAddress) & 0xF0) >> 4);
    cache.sceneCount = configuredSceneCount > MAX_SCENES ? MAX_SCENES : configuredSceneCount;

    const uint16_t sceneBaseAddress = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_DATA_START;
    for (uint16_t index = 0; index < sizeof(cache.data); ++index)
    {
        cache.data[index] = knx.paramByte(sceneBaseAddress + index);
    }

    cache.initialized = true;
    return true;
}

SceneManager::SceneCache* SceneManager::sceneCache(uint8_t channelIndex)
{
    if (!ensureSceneCache(channelIndex) || channelIndex >= _sceneCaches.size())
    {
        return nullptr;
    }
    return &_sceneCaches[channelIndex];
}

const SceneManager::SceneCache* SceneManager::sceneCache(uint8_t channelIndex) const
{
    if (channelIndex >= _sceneCaches.size() || !_sceneCaches[channelIndex].initialized)
    {
        return nullptr;
    }
    return &_sceneCaches[channelIndex];
}

uint16_t SceneManager::sceneFieldIndex(uint8_t sceneIndex, uint8_t fieldOffset)
{
    return static_cast<uint16_t>(sceneIndex) * SCENE_SIZE + fieldOffset;
}

uint8_t SceneManager::getSceneCount(uint8_t channelIndex)
{
    SceneCache* cache = sceneCache(channelIndex);
    return cache != nullptr ? cache->sceneCount : 0;
}

bool SceneManager::setSceneCount(uint8_t channelIndex, uint8_t sceneCount)
{
    if (sceneCount > MAX_SCENES)
    {
        logErrorP("setSceneCount: invalid count %d for channel %d", sceneCount, channelIndex);
        return false;
    }

    SceneCache* cache = sceneCache(channelIndex);
    if (cache == nullptr)
    {
        logErrorP("setSceneCount: no scene cache for channel %d", channelIndex);
        return false;
    }

    cache->sceneCount = sceneCount;
    rememberSceneMemoryWrite(NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_COUNT_OFFSET,
                             static_cast<uint8_t>((sceneCount & 0x0F) << 4));
    return true;
}

bool SceneManager::tryGetSceneSnapshot(uint8_t channelIndex, uint8_t sceneNumber, SceneSnapshot& snapshot)
{
    uint8_t sceneCount = getSceneCount(channelIndex);
    if (sceneNumber < 1 || sceneNumber > sceneCount)
    {
        return false;
    }

    uint8_t sceneIndex = sceneNumber - 1;

    snapshot.effectType = readSceneField(channelIndex, sceneIndex, FIELD_EFFECT_TYPE);
    readSceneColorRGB(channelIndex, sceneIndex, FIELD_PRIMARY_R,
                      snapshot.primaryR, snapshot.primaryG, snapshot.primaryB);
    snapshot.primaryWW = readSceneField(channelIndex, sceneIndex, FIELD_PRIMARY_WW);
    snapshot.primaryCW = readSceneField(channelIndex, sceneIndex, FIELD_PRIMARY_CW);

    readSceneColorRGB(channelIndex, sceneIndex, FIELD_SECONDARY_R,
                      snapshot.secondaryR, snapshot.secondaryG, snapshot.secondaryB);
    snapshot.secondaryWW = readSceneField(channelIndex, sceneIndex, FIELD_SECONDARY_WW);
    snapshot.secondaryCW = readSceneField(channelIndex, sceneIndex, FIELD_SECONDARY_CW);
    snapshot.brightness = readSceneField(channelIndex, sceneIndex, FIELD_BRIGHTNESS);

    return true;
}

bool SceneManager::clearSceneSlot(uint8_t channelIndex, uint8_t sceneIndex)
{
    bool success = true;
    for (uint8_t fieldOffset = 0; fieldOffset < SCENE_SIZE; ++fieldOffset)
    {
        success = writeSceneField(channelIndex, sceneIndex, fieldOffset, 0) && success;
    }
    return success;
}

bool SceneManager::copySceneSlot(uint8_t channelIndex, uint8_t sourceSceneIndex, uint8_t targetSceneIndex)
{
    if (sourceSceneIndex == targetSceneIndex)
    {
        return true;
    }

    uint8_t slotData[SCENE_SIZE];
    for (uint8_t fieldOffset = 0; fieldOffset < SCENE_SIZE; ++fieldOffset)
    {
        slotData[fieldOffset] = readSceneField(channelIndex, sourceSceneIndex, fieldOffset);
    }

    bool success = true;
    for (uint8_t fieldOffset = 0; fieldOffset < SCENE_SIZE; ++fieldOffset)
    {
        success = writeSceneField(channelIndex, targetSceneIndex, fieldOffset, slotData[fieldOffset]) && success;
    }
    return success;
}

bool SceneManager::addScene(uint8_t channelIndex, Segment* segment, uint8_t& newSceneNumber, bool flushMemory)
{
    newSceneNumber = 0;

    if (segment == nullptr)
    {
        logErrorP("addScene: segment is null for channel %d", channelIndex);
        return false;
    }

    const uint8_t sceneCount = getSceneCount(channelIndex);
    if (sceneCount >= MAX_SCENES)
    {
        logInfoP("addScene: segment %d already uses all %d scene slots", channelIndex, MAX_SCENES);
        return false;
    }

    newSceneNumber = static_cast<uint8_t>(sceneCount + 1);
    if (!setSceneCount(channelIndex, newSceneNumber))
    {
        newSceneNumber = 0;
        return false;
    }

    if (!clearSceneSlot(channelIndex, static_cast<uint8_t>(newSceneNumber - 1)) || !storeScene(channelIndex, newSceneNumber, segment, false))
    {
        clearSceneSlot(channelIndex, static_cast<uint8_t>(newSceneNumber - 1));
        setSceneCount(channelIndex, sceneCount);
        if (flushMemory)
        {
            flushPendingSceneMemory();
        }
        else
        {
            discardPendingSceneMemory();
        }
        newSceneNumber = 0;
        return false;
    }

    if (flushMemory && !flushPendingSceneMemory())
    {
        newSceneNumber = 0;
        return false;
    }

    return true;
}

bool SceneManager::deleteScene(uint8_t channelIndex, uint8_t sceneNumber, bool flushMemory)
{
    const uint8_t sceneCount = getSceneCount(channelIndex);
    if (sceneNumber < 1 || sceneNumber > sceneCount)
    {
        logInfoP("deleteScene: Scene %d out of range for segment %d (configured: %d scenes)",
                 sceneNumber, channelIndex, sceneCount);
        return false;
    }

    if (sceneNumber != sceneCount)
    {
        logInfoP("deleteScene: Scene %d cannot be deleted for segment %d because scene %d is the highest configured scene",
                 sceneNumber, channelIndex, sceneCount);
        return false;
    }

    if (!clearSceneSlot(channelIndex, static_cast<uint8_t>(sceneCount - 1)))
    {
        return false;
    }
    if (!setSceneCount(channelIndex, static_cast<uint8_t>(sceneCount - 1)))
    {
        return false;
    }

    if (flushMemory)
    {
        return flushPendingSceneMemory();
    }
    return true;
}

bool SceneManager::exportScenes(uint8_t channelIndex, uint8_t* data, uint8_t& length)
{
    if (data == nullptr)
    {
        length = 0;
        return false;
    }

    const uint8_t sceneCount = getSceneCount(channelIndex);
    const uint16_t payloadLength = static_cast<uint16_t>(1 + sceneCount * SCENE_SIZE);
    if (payloadLength > 255)
    {
        logErrorP("exportScenes: payload too large for channel %d", channelIndex);
        length = 0;
        return false;
    }

    uint8_t offset = 0;
    data[offset++] = sceneCount;
    for (uint8_t sceneIndex = 0; sceneIndex < sceneCount; ++sceneIndex)
    {
        for (uint8_t fieldOffset = 0; fieldOffset < SCENE_SIZE; ++fieldOffset)
        {
            data[offset++] = readSceneField(channelIndex, sceneIndex, fieldOffset);
        }
    }

    length = offset;
    return true;
}

uint8_t SceneManager::readSceneField(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset)
{
    const SceneCache* cache = sceneCache(channelIndex);
    if (cache == nullptr || sceneIndex >= MAX_SCENES || fieldOffset >= SCENE_SIZE)
    {
        return 0;
    }
    return cache->data[sceneFieldIndex(sceneIndex, fieldOffset)];
}

void SceneManager::readSceneColorRGB(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset,
                                     uint8_t& r, uint8_t& g, uint8_t& b)
{
    r = readSceneField(channelIndex, sceneIndex, fieldOffset);
    g = readSceneField(channelIndex, sceneIndex, static_cast<uint8_t>(fieldOffset + 1));
    b = readSceneField(channelIndex, sceneIndex, static_cast<uint8_t>(fieldOffset + 2));
}

bool SceneManager::writeSceneField(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset, uint8_t value)
{
    uint16_t addr = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_DATA_START + sceneIndex * SCENE_SIZE + fieldOffset;
    return writeParameterByte(addr, value);
}

bool SceneManager::writeSceneColorRGB(uint8_t channelIndex, uint8_t sceneIndex, uint8_t fieldOffset,
                                      uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t baseAddr = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_DATA_START + sceneIndex * SCENE_SIZE + fieldOffset;
    const uint8_t values[3] = {r, g, b};
    return writeParameterBytes(baseAddr, values, 3);
}

bool SceneManager::writeParameterByte(uint16_t address, uint8_t value)
{
    if (address < NEO_ParamBlockOffset)
    {
        logErrorP("writeParameterByte: address %d is outside the NeoPixel parameter block", address);
        return false;
    }

    const uint16_t relativeAddress = static_cast<uint16_t>(address - NEO_ParamBlockOffset);
    const uint8_t channelIndex = static_cast<uint8_t>(relativeAddress / NEO_ParamBlockSize);
    const uint16_t fieldOffset = static_cast<uint16_t>(relativeAddress % NEO_ParamBlockSize);
    SceneCache* cache = sceneCache(channelIndex);
    if (cache == nullptr)
    {
        logErrorP("writeParameterByte: no scene cache for channel %d", channelIndex);
        return false;
    }

    if (fieldOffset == SCENE_COUNT_OFFSET)
    {
        const uint8_t sceneCount = static_cast<uint8_t>((value & 0xF0) >> 4);
        if (sceneCount > MAX_SCENES)
        {
            logErrorP("writeParameterByte: invalid scene count %d for channel %d", sceneCount, channelIndex);
            return false;
        }
        cache->sceneCount = sceneCount;
        rememberSceneMemoryWrite(address, value);
        return true;
    }

    if (fieldOffset < SCENE_DATA_START || fieldOffset >= SCENE_DATA_START + sizeof(cache->data))
    {
        logErrorP("writeParameterByte: address %d is outside the scene cache", address);
        return false;
    }

    cache->data[fieldOffset - SCENE_DATA_START] = value;
    rememberSceneMemoryWrite(address, value);
    return true;
}

bool SceneManager::writeParameterBytes(uint16_t address, const uint8_t* values, uint8_t length)
{
    if (values == nullptr)
    {
        return false;
    }

    bool success = true;
    for (uint8_t index = 0; index < length; ++index)
    {
        success = writeParameterByte(static_cast<uint16_t>(address + index), values[index]) && success;
    }
    return success;
}

void SceneManager::rememberSceneMemoryWrite(uint16_t address, uint8_t value)
{
    for (PendingSceneMemoryWrite& pendingWrite : _pendingSceneMemoryWrites)
    {
        if (pendingWrite.address == address)
        {
            pendingWrite.value = value;
            return;
        }
    }

    _pendingSceneMemoryWrites.push_back({address, value});
}

bool SceneManager::flushPendingSceneMemory()
{
    _pendingSceneMemoryWrites.clear();
    return true;
}

void SceneManager::discardPendingSceneMemory()
{
    _pendingSceneMemoryWrites.clear();
}

bool SceneManager::importScenes(uint8_t channelIndex, const uint8_t* data, uint8_t length)
{
    if (data == nullptr || length == 0)
    {
        return false;
    }

    const uint8_t sceneCount = data[0];
    if (sceneCount > MAX_SCENES)
    {
        logErrorP("importScenes: invalid count %d for channel %d", sceneCount, channelIndex);
        return false;
    }

    const uint16_t expectedLength = static_cast<uint16_t>(1 + sceneCount * SCENE_SIZE);
    if (length < expectedLength)
    {
        logErrorP("importScenes: payload too short for channel %d (%d < %d)", channelIndex, length, expectedLength);
        return false;
    }

    const uint16_t countAddress = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize + SCENE_COUNT_OFFSET;
    const uint8_t configuredSceneCount = static_cast<uint8_t>((knx.paramByte(countAddress) & 0xF0) >> 4);
    const uint8_t clampedConfiguredSceneCount = configuredSceneCount > MAX_SCENES ? MAX_SCENES : configuredSceneCount;
    if (sceneCount != clampedConfiguredSceneCount)
    {
        logWarningP("importScenes: flash snapshot count %d restored for channel %d although ETS config has %d scene(s); synchronize ETS with the device if this runtime state should become the project state",
                    sceneCount, channelIndex, clampedConfiguredSceneCount);
    }

    if (_sceneCaches.size() <= channelIndex)
    {
        _sceneCaches.resize(static_cast<size_t>(channelIndex) + 1);
    }

    SceneCache& cache = _sceneCaches[channelIndex];
    cache.initialized = true;
    cache.sceneCount = sceneCount;
    memset(cache.data, 0, sizeof(cache.data));

    const uint8_t* sceneData = data + 1;
    if (sceneCount > 0)
    {
        memcpy(cache.data, sceneData, static_cast<size_t>(sceneCount) * SCENE_SIZE);
    }

    discardPendingSceneMemory();
    return true;
}

bool SceneManager::storeScene(uint8_t channelIndex, uint8_t sceneNumber, Segment* segment, bool flushMemory)
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
    bool writeSuccess = true;
    writeSuccess = writeSceneField(channelIndex, sceneIndex, FIELD_EFFECT_TYPE, effectType) && writeSuccess;
    writeSuccess = writeSceneColorRGB(channelIndex, sceneIndex, FIELD_PRIMARY_R, primaryR, primaryG, primaryB) && writeSuccess;
    writeSuccess = writeSceneField(channelIndex, sceneIndex, FIELD_PRIMARY_WW, primaryWW) && writeSuccess;
    writeSuccess = writeSceneField(channelIndex, sceneIndex, FIELD_PRIMARY_CW, primaryCW) && writeSuccess;
    writeSuccess = writeSceneColorRGB(channelIndex, sceneIndex, FIELD_SECONDARY_R, secondaryR, secondaryG, secondaryB) && writeSuccess;
    writeSuccess = writeSceneField(channelIndex, sceneIndex, FIELD_SECONDARY_WW, secondaryWW) && writeSuccess;
    writeSuccess = writeSceneField(channelIndex, sceneIndex, FIELD_SECONDARY_CW, secondaryCW) && writeSuccess;
    writeSuccess = writeSceneField(channelIndex, sceneIndex, FIELD_BRIGHTNESS, brightness) && writeSuccess;

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
            writeSuccess = writeParameterByte(static_cast<uint16_t>(effectParamBase + i), static_cast<uint8_t>(paramValue & 0xFF)) && writeSuccess;
        }
    }

    if (!writeSuccess)
    {
        return false;
    }

    if (flushMemory)
    {
        return flushPendingSceneMemory();
    }

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

    const VirtualStrip* virtualStrip = segment->getVirtualStrip();
    const uint8_t bytesPerLed = virtualStrip ? virtualStrip->getBytesPerLed() : 3;

    if (bytesPerLed == 5)
    {
        segment->setPrimaryColor(primaryR, primaryG, primaryB, primaryWW, primaryCW);
        segment->setSecondaryColor(secondaryR, secondaryG, secondaryB, secondaryWW, secondaryCW);
    }
    else
    {
        segment->setPrimaryColor(primaryR, primaryG, primaryB, primaryWW);
        segment->setSecondaryColor(secondaryR, secondaryG, secondaryB, secondaryWW);
    }

    // Apply brightness
    segment->setBrightness(brightness);

    // Load scene-specific effect parameters (auto-generated)
#ifdef SCENE_EFFECT_PARAMETER_MAPPING_GENERATED
    Effect* effect = segment->getEffect();
    if (effect && effect->getParameterCount() > 0)
    {
        const uint8_t paramCount = effect->getParameterCount();
        for (uint8_t parameterIndex = 0; parameterIndex < paramCount && parameterIndex < 10; ++parameterIndex)
        {
            effect->setParameter(segment, parameterIndex,
                                 readSceneField(channelIndex, sceneIndex, static_cast<uint8_t>(SCENE_EFFECT_PARAM_OFFSET + parameterIndex)));
        }
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
        cfg.savedSecondaryR = secondaryR;
        cfg.savedSecondaryG = secondaryG;
        cfg.savedSecondaryB = secondaryB;
        cfg.savedSecondaryWW = secondaryWW;
        cfg.savedSecondaryCW = secondaryCW;
        cfg.savedBrightness = brightness;
        cfg.savedEffectType = effectType;
        cfg.savedEffectValid = (effectType > 0);
        cfg.savedValid = true;
    }

    return true;
}
