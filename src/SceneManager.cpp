#include "SceneManager.h"
#include "NeoPixelModule.h"
#include "OpenKNX.h"
#include "knxprod.h"
#include <algorithm>
#include <cstring>

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

SceneManager::SceneCache SceneManager::readEtsCache(uint8_t channel)
{
    SceneCache cache;
    const uint16_t base = NEO_ParamBlockOffset + channel * NEO_ParamBlockSize;
    cache.count = std::min<uint8_t>(MAX_SCENES, (knx.paramByte(base + SCENE_COUNT_OFFSET) >> 4) & 15);
    for (size_t scene = 0; scene < MAX_SCENES; ++scene)
        for (size_t byte = 0; byte < SCENE_SIZE; ++byte)
            cache.scenes[scene][byte] = knx.paramByte(base + SCENE_DATA_START + scene * SCENE_SIZE + byte);
    return cache;
}

void SceneManager::ensureCache(uint8_t channel)
{
    if (channel >= 16) return;
    while (_cache.size() <= channel) _cache.push_back(readEtsCache(_cache.size()));
}

void SceneManager::restoreEtsScenes(uint8_t channel)
{
    ensureCache(channel);
    if (channel < _cache.size()) _cache[channel] = readEtsCache(channel);
}

uint32_t SceneManager::etsSignature(uint8_t channel)
{
    if (channel >= 16) return 0;
    if (_etsSignatureTaken & (1u << channel)) return _etsSignature[channel];
    const uint16_t base = NEO_ParamBlockOffset + channel * NEO_ParamBlockSize;
    uint32_t signature = 2166136261u;
    signature = (signature ^ ((knx.paramByte(base + SCENE_COUNT_OFFSET) >> 4) & 15)) * 16777619u;
    for (uint16_t byte = 0; byte < MAX_SCENES * SCENE_SIZE; ++byte)
        signature = (signature ^ knx.paramByte(base + SCENE_DATA_START + byte)) * 16777619u;
    _etsSignature[channel] = signature;
    _etsSignatureTaken |= 1u << channel;
    return signature;
}

uint8_t SceneManager::getSceneCount(uint8_t channel)
{
    ensureCache(channel);
    return channel < _cache.size() ? _cache[channel].count : 0;
}
uint8_t SceneManager::readSceneField(uint8_t channel, uint8_t scene, uint8_t field)
{
    ensureCache(channel);
    return channel < _cache.size() && scene < MAX_SCENES && field < SCENE_SIZE ? _cache[channel].scenes[scene][field] : 0;
}
void SceneManager::writeSceneField(uint8_t channel, uint8_t scene, uint8_t field, uint8_t value)
{
    ensureCache(channel);
    if (channel < _cache.size() && scene < MAX_SCENES && field < SCENE_SIZE) _cache[channel].scenes[scene][field] = value;
}
void SceneManager::readSceneColorRGB(uint8_t channel, uint8_t scene, uint8_t field, uint8_t& r, uint8_t& g, uint8_t& b)
{
    r = readSceneField(channel, scene, field);
    g = readSceneField(channel, scene, field + 1);
    b = readSceneField(channel, scene, field + 2);
}
void SceneManager::writeSceneColorRGB(uint8_t channel, uint8_t scene, uint8_t field, uint8_t r, uint8_t g, uint8_t b)
{
    writeSceneField(channel, scene, field, r);
    writeSceneField(channel, scene, field + 1, g);
    writeSceneField(channel, scene, field + 2, b);
}
SceneManager::SceneData SceneManager::sceneData(uint8_t channel, uint8_t scene)
{
    ensureCache(channel);
    return channel < _cache.size() && scene >= 1 && scene <= getSceneCount(channel) ? _cache[channel].scenes[scene - 1] : SceneData{};
}
std::vector<uint8_t> SceneManager::exportScenes(uint8_t channel)
{
    std::vector<uint8_t> result{getSceneCount(channel)};
    for (uint8_t scene = 1; scene <= result[0]; ++scene)
    {
        const auto data = sceneData(channel, scene);
        result.insert(result.end(), data.begin(), data.end());
    }
    return result;
}
bool SceneManager::importScenes(uint8_t channel, const uint8_t* data, size_t length)
{
    if (channel >= 16 || !data || !length || data[0] > MAX_SCENES || length != 1u + data[0] * SCENE_SIZE) return false;
    ensureCache(channel);
    _cache[channel] = SceneCache{};
    _cache[channel].count = data[0];
    for (uint8_t scene = 0; scene < data[0]; ++scene)
        memcpy(_cache[channel].scenes[scene].data(), data + 1 + scene * SCENE_SIZE, SCENE_SIZE);
    return true;
}
bool SceneManager::addScene(uint8_t channel, Segment* segment)
{
    if (channel >= 16 || !segment || getSceneCount(channel) >= MAX_SCENES) return false;
    ++_cache[channel].count;
    if (storeScene(channel, _cache[channel].count, segment)) return true;
    --_cache[channel].count;
    return false;
}
bool SceneManager::deleteScene(uint8_t channel, uint8_t scene)
{
    if (!scene || scene != getSceneCount(channel)) return false;
    _cache[channel].scenes[scene - 1] = SceneData{};
    --_cache[channel].count;
    if (channel < _module->getSegments().size() && _module->getSegments()[channel].savedSceneNumber == scene) _module->getSegments()[channel].savedSceneNumber = 0;
    return true;
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

    // Capture current effect type — or, if an Effektmanager runs visibly, store it as EM action.
    // An off segment (or an EM paused by power) stores its live direct state at brightness 0.
    Effect* effect = segment->getEffect();
    uint8_t effectType = getTypeFromEffect(effect);
    bool emRunning = false;
    uint8_t brightness = segment->getBrightness();
    {
        auto& segments = _module->getSegments();
        if (channelIndex < segments.size())
        {
            const auto& cfg = segments[channelIndex];
            const bool visible = NeoPixelBusModule::isSegmentVisible(cfg);
            brightness = visible ? cfg.savedBrightness : 0; // the intent, not the cue-scaled render
            if (visible && cfg.emController.isRunning() && !cfg.suspendedByPower)
            {
                uint8_t emId = cfg.emController.activeEmId();
                if (emId >= 1 && emId <= EM_COUNT)
                {
                    effectType = EM_ACTION_BASE + emId - 1;
                    emRunning = true;
                }
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

    logInfoP("storeScene %d for segment %d: Effect=%d, Primary=(%d,%d,%d,WW=%d,CW=%d), "
             "Secondary=(%d,%d,%d,WW=%d,CW=%d), Brightness=%d",
             sceneNumber, channelIndex, effectType,
             primaryR, primaryG, primaryB, primaryWW, primaryCW,
             secondaryR, secondaryG, secondaryB, secondaryWW, secondaryCW,
             brightness);

    if (effect && !emRunning)
        for (uint8_t p = 0; p < effect->getParameterCount(); ++p)
            if (effect->getParameterType(p) == ParameterType::PARAM_STRING && strlen(config.effectText) > EFFECT_TEXT_SIZE) return false;
    for (uint8_t field = SCENE_EFFECT_PARAM_OFFSET; field < SCENE_SIZE; ++field)
        writeSceneField(channelIndex, sceneIndex, field, 0);

    // Write scene fields to the OAM cache
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

        bool hasText = false;
        for (uint8_t i = 0; i < paramCount && i < 10; i++)
        {

            // A text parameter lives in the effect-text field; getParameter() returns a pointer
            if (effect->getParameterType(i) == ParameterType::PARAM_STRING)
            {
                hasText = true;
                writeSceneField(channelIndex, sceneIndex, SCENE_EFFECT_PARAM_OFFSET + i, 0);
                continue;
            }
            uint32_t paramValue = effect->getParameter(segment, i);
            logInfoP("  param[%d] '%s' = %lu", i, effect->getParameterName(i), (unsigned long)paramValue);
            writeSceneField(channelIndex, sceneIndex, SCENE_EFFECT_PARAM_OFFSET + i, (uint8_t)paramValue);
        }

        // Effect text (14 bytes, zero-padded, cut at a UTF-8 character boundary); empty without a text parameter
        const size_t length = hasText ? std::min<size_t>(EFFECT_TEXT_SIZE, strlen(config.effectText)) : 0;
        for (size_t i = 0; i < EFFECT_TEXT_SIZE; ++i)
            writeSceneField(channelIndex, sceneIndex, FIELD_EFFECT_TEXT + i, i < length ? config.effectText[i] : 0);
    }

    return true;
}

bool SceneManager::isEmActionScene(uint8_t channelIndex, uint8_t sceneNumber)
{
    if (sceneNumber < 1 || sceneNumber > getSceneCount(channelIndex)) return false;
    uint8_t effectType = readSceneField(channelIndex, sceneNumber - 1, FIELD_EFFECT_TYPE);
    return effectType >= EM_ACTION_BASE && effectType < EM_ACTION_BASE + EM_COUNT;
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

    // Read scene data from the OAM cache
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

    // A scene is an authoritative "set this state NOW" command. A running Effektmanager is
    // taken over like by a direct command (no EM-stop return), otherwise its per-frame tick()
    // would overwrite the static scene on the next loop.
    // (The EM-action scene path above returns earlier and starts its own EM.)
    _module->interruptEmIfRunning(channelIndex);

    // Brightness 0 is a switch-off scene: colour, effect and brightness intent stay.
    if (brightness == 0)
    {
        _module->segmentOff(channelIndex, true);
        return true;
    }

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
    // CW exists only on RGBCCT strips; elsewhere the scene parameter is ignored.
    if (!segment->getVirtualStrip() || !segment->getVirtualStrip()->hasDualWhiteChannel())
    {
        primaryCW = 0;
        secondaryCW = 0;
    }
    segment->setPrimaryColor(primaryR, primaryG, primaryB, primaryWW, primaryCW);

    // Apply secondary color
    EffectConfig& config = segment->getConfig();
    config.secondaryRGBW = ((uint32_t)secondaryR << 24) | ((uint32_t)secondaryG << 16) | ((uint32_t)secondaryB << 8) | secondaryWW;
    config.secondaryCW = secondaryCW;

    // Load scene-specific effect parameters (auto-generated)
#ifdef SCENE_EFFECT_PARAMETER_MAPPING_GENERATED
    Effect* effect = segment->getEffect();
    if (effect && effect->getParameterCount() > 0)
    {
        for (uint8_t p = 0; p < effect->getParameterCount() && p < 10; ++p)
        {
            if (effect->getParameterType(p) == ParameterType::PARAM_STRING)
            {
                char text[EFFECT_TEXT_SIZE + 1] = {};
                for (uint8_t i = 0; i < EFFECT_TEXT_SIZE; ++i)
                    text[i] = readSceneField(channelIndex, sceneIndex, FIELD_EFFECT_TEXT + i);
                effect->setParameter(segment, p, (uint32_t)(uintptr_t)text);
            }
            else
            {
                uint32_t value = readSceneField(channelIndex, sceneIndex, SCENE_EFFECT_PARAM_OFFSET + p);
                uint32_t min = effect->getParameterMin(p), max = effect->getParameterMax(p);
                if (effect->getParameterType(p) == ParameterType::PARAM_BOOL)
                {
                    min = 0;
                    max = 1;
                }
                if (effect->getParameterType(p) == ParameterType::PARAM_ENUM && effect->getEnumValueCount(p))
                {
                    min = 0;
                    max = effect->getEnumValueCount(p) - 1;
                }
                effect->setParameter(segment, p, std::min(max, std::max(min, value)));
            }
        }
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
        // Solid uses the valid effect ID 0.  Preserve it as a selected effect
        // so recalling a Solid scene survives the same restore paths as an
        // animated scene.
        cfg.savedEffectValid = true;
        cfg.savedLastWasEffect = true;
        cfg.savedValid = true;

        // Scene brightness is the intent (x global); then switch on and re-enter the new effect.
        _module->applyBrightnessIntent(channelIndex);
        _module->sceneSwitchOn(channelIndex);
    }

    return true;
}
