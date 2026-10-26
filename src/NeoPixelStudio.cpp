#include "NeoPixelStudio.h"
#include "NeoPixelModule.h"
#include "SceneManager.h"

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    #include "EffectTypeMapping.h"
    #include "NetworkModule.h"
    #include "OpenKNX/Charset.hpp"
    #include "OpenKNX/Format/JSON/Reader.h"
    #include "OpenKNX/Format/JSON/Writer.h"
    #include "WebUi/NeoPixelStudioAssets.h"
    #include <cctype>
    #include <cmath>
    #include <cstdlib>
    #include <cstring>

using OpenKNX::Format::JSON::Reader;
using OpenKNX::Format::JSON::Writer;
using namespace OpenKNX::Network;

namespace
{
    void sendJson(WebResponse& res, uint16_t status, const char* body)
    {
        res.setStatus(status);
        res.setContentType("application/json; charset=utf-8");
        res.setHeader("Cache-Control", "no-store");
        res.send(body);
    }

    bool integer(const Reader& value, uint32_t min, uint32_t max)
    {
        const float number = value.toFloat(-1);
        return value.isNumber() && std::isfinite(number) && double(number) >= double(min) && double(number) <= double(max) && std::floor(number) == number;
    }

    std::string utf8(const char* value)
    {
        std::string result;
        if (value) OpenKNX::Charset::encodeUtf8(result, value, strlen(value));
        return result;
    }

    void color(Writer& json, const char* key, uint8_t r, uint8_t g, uint8_t b, uint8_t ww, uint8_t cw)
    {
        char hex[8];
        snprintf(hex, sizeof(hex), "#%02x%02x%02x", r, g, b);
        json.key(key).beginObject().field("rgb", hex).field("ww", uint32_t(ww)).field("cw", uint32_t(cw)).endObject();
    }
} // namespace

void NeoPixelStudio::setup(bool configured)
{
    if (_registered) return;
    _state = configured ? "{\"ready\":false,\"configured\":true,\"segments\":[]}"
                        : "{\"ready\":false,\"configured\":false,\"segments\":[]}";
    Writer effects;
    effects.beginArray();
    for (unsigned id = 0; id <= 254; ++id)
    {
        Effect* effect = getEffectFromType(id);
        if (!effect || getTypeFromEffect(effect) != id) continue;
        effects.beginObject().field("id", uint32_t(id)).field("name", effect->getName("en")).key("parameters").beginArray();
        for (uint8_t p = 0; p < effect->getParameterCount(); ++p)
        {
            const auto type = effect->getParameterType(p);
            effects.beginObject().field("id", uint32_t(p)).field("name", effect->getParameterName(p));
            effects.field("type", type == ParameterType::PARAM_STRING ? "text" : type == ParameterType::PARAM_BOOL ? "bool"
                                                                             : type == ParameterType::PARAM_ENUM   ? "enum"
                                                                                                                   : "number");
            effects.field("min", effect->getParameterMin(p)).field("max", effect->getParameterMax(p));
            if (type == ParameterType::PARAM_STRING) effects.field("value", effect->getParameterDefaultText(p) ? effect->getParameterDefaultText(p) : "");
            else
                effects.field("value", effect->getParameterDefault(p));
            effects.key("options").beginArray();
            if (type == ParameterType::PARAM_ENUM)
                for (uint8_t n = 0; n < effect->getEnumValueCount(p); ++n)
                    effects.value(effect->getEnumValueName(p, n));
            effects.endArray().endObject();
        }
        effects.endArray().endObject();
    }
    effects.endArray();
    _effects = effects.str();

    auto& web = openknxNetwork.webserver;
    web.addMenuItem("NeoPixel Studio", "/neopixel", 20);
    web.addRoute(WEB_GET, "/neopixel", [](WebRequest&, WebResponse& res) {
        res.setLayout(true);
        res.setActiveMenu("/neopixel");
        res.sendStatic(kNeoPixelStudioHtml);
    });
    web.addRoute(WEB_GET, "/neopixel/", Webserver::Redirect("/neopixel"));
    web.addRoute(WEB_GET, "/neopixel/studio.js", [](WebRequest&, WebResponse& res) {
        res.setContentType("application/javascript; charset=utf-8");
        res.setHeader("Cache-Control", "no-cache");
        res.sendStatic(kNeoPixelStudioJs);
    });
    web.addRoute(WEB_GET, "/neopixel/api/effects", [this](WebRequest&, WebResponse& res) {
        sendJson(res, 200, _effects.c_str());
    });
    web.addRoute(WEB_GET, "/neopixel/api/state", [this](WebRequest& req, WebResponse& res) {
        if (!_acceptCommands.load())
        {
            sendJson(res, 200, "{\"ready\":false,\"programming\":true,\"segments\":[]}");
            return;
        }
        const std::string selected = req.getQueryParam("segment");
        if (!selected.empty())
        {
            char* end = nullptr;
            const unsigned long id = strtoul(selected.c_str(), &end, 10);
            if (!end || *end || id < 1 || id > 16)
            {
                sendJson(res, 400, "{\"error\":\"Invalid segment\"}");
                return;
            }
            _requestedSegment.store(id);
        }
        _refresh.store(true);
        if (_guard.exchange(1, std::memory_order_acquire))
        {
            sendJson(res, 503, "{\"error\":\"Studio busy; retry shortly\"}");
            return;
        }
        const std::string state = _state;
        _guard.store(0, std::memory_order_release);
        sendJson(res, 200, state.c_str());
    });
    web.addRoute(WEB_POST, "/neopixel/api/control", [this](WebRequest& req, WebResponse& res) {
        if (!_acceptCommands.load())
        {
            sendJson(res, 503, "{\"error\":\"ETS download in progress\"}");
            return;
        }
        if (!req.body() || req.bodyLength() == 0 || req.bodyLength() > 4096)
        {
            sendJson(res, 413, "{\"error\":\"Invalid command size\"}");
            return;
        }
        if (_guard.exchange(1, std::memory_order_acquire))
        {
            sendJson(res, 503, "{\"error\":\"Studio busy; retry shortly\"}");
            return;
        }
        if (!_pending.empty() || _processing)
        {
            _guard.store(0, std::memory_order_release);
            sendJson(res, 409, "{\"error\":\"Another command is pending\"}");
            return;
        }
        _pending.assign(reinterpret_cast<const char*>(req.body()), req.bodyLength());
        _pendingId = ++_nextId;
        const uint32_t id = _pendingId;
        _guard.store(0, std::memory_order_release);
        Writer reply;
        reply.beginObject().field("id", id).endObject();
        sendJson(res, 202, reply.c_str());
    });
    _registered = true;
}

void NeoPixelStudio::loop()
{
    if (!_acceptCommands.load()) return;
    std::string command;
    uint32_t id = 0;
    if (_guard.exchange(1, std::memory_order_acquire)) return;
    if (!_pending.empty())
    {
        const Reader pending(_pending.c_str());
        if (pending.get("action").equals("save"))
        {
            for (auto* strip : _module.getPhysicalStrips())
            {
                if (strip && strip->isBusy())
                {
                    _guard.store(0, std::memory_order_release);
                    return;
                }
            }
        }
        command.swap(_pending);
        id = _pendingId;
        _processing = true;
    }
    _guard.store(0, std::memory_order_release);

    if (!command.empty())
    {
        _error = apply(command);
        _completedId = id;
        _refresh.store(true);
    }
    if (!_refresh.load() || (command.empty() && uint32_t(millis() - _lastRefresh) < 250)) return;
    // Build outside the guard: callbacks only ever see a complete immutable snapshot.
    std::string state = snapshot();
    if (_guard.exchange(1, std::memory_order_acquire)) return;
    _state.swap(state);
    _processing = false;
    _refresh.store(false);
    _lastRefresh = millis();
    _guard.store(0, std::memory_order_release);
}

std::string NeoPixelStudio::apply(const std::string& command)
{
    Reader doc(command.c_str(), command.size());
    return applyDoc(doc);
}

std::string NeoPixelStudio::applyDoc(const Reader& doc, bool validateOnly, uint8_t expectedEffect)
{
    if (!doc.isObject() || !integer(doc.get("segment"), 1, _module._segments.size())) return "Unknown segment";
    const size_t index = static_cast<uint32_t>(doc.get("segment").toFloat()) - 1;
    auto& cfg = _module._segments[index];
    Segment* segment = cfg.segment;
    if (!segment) return "Segment unavailable";
    const std::string action = doc.get("action").toString();
    if (action == "load") return {};
    if (action == "save") return validateOnly ? "Save cannot be nested" : saveSegment(index);
    if (cfg.locked) return "Segment is locked by KNX";
    const Reader value = doc.get("value");
    if (action == "batch")
    {
        if (validateOnly || !value.isArray() || !value.get(size_t(0)).isObject()) return "Invalid batch";
        uint8_t stagedEffect = getTypeFromEffect(segment->getEffect());
        size_t count = 0;
        for (; value.get(count).valid(); ++count)
        {
            if (count >= 32) return "Batch too large";
            const Reader item = value.get(count);
            if (item.get("segment").toUInt() != index + 1 || item.get("action").equals("save") || item.get("action").equals("load") || item.get("action").equals("restoreEts") || item.get("action").equals("batch")) return "Invalid batch action";
            const std::string error = applyDoc(item, true, stagedEffect);
            if (!error.empty()) return error;
            if (item.get("action").equals("effect")) stagedEffect = static_cast<uint32_t>(item.get("value").toFloat());
        }
        size_t sceneActions = 0;
        for (size_t i = 0; i < count; ++i)
            if (value.get(i).get("action").toString().find("scene") == 0) ++sceneActions;
        if (sceneActions > 1 || (sceneActions == 1 && value.get(count - 1).get("action").toString().find("scene") != 0)) return "A batch supports one final scene action";
        bool capturesScene = false;
        for (size_t i = 0; i < count; ++i)
            if (value.get(i).get("action").equals("sceneAdd") || value.get(i).get("action").equals("sceneStore")) capturesScene = true;
        if (capturesScene)
        {
            std::string captureText = segment->getConfig().effectText;
            for (size_t i = 0; i < count; ++i)
            {
                const Reader item = value.get(i);
                if (item.get("action").equals("effect")) captureText.clear();
                if (item.get("action").equals("parameter") && item.get("value").isString())
                {
                    const std::string input = item.get("value").toString();
                    captureText.clear();
                    if (!OpenKNX::Charset::decodeUtf8(captureText, input.c_str(), input.size())) return "Invalid scene text";
                }
            }
            if (captureText.size() > 14) return "Scenes support at most 14 bytes of effect text";
        }
        for (size_t i = 0; i < count; ++i)
        {
            const std::string error = applyDoc(value.get(i));
            if (!error.empty()) return error;
        }
        return {};
    }
    if (action == "power")
    {
        if (!integer(value, 0, 1)) return "Invalid power value";
        if (validateOnly) return {};
        if (static_cast<uint32_t>(value.toFloat()))
        {
            const uint8_t intent = cfg.savedBrightness;
            _module.segmentOn(index);
            // Studio sends a complete visual state: an explicit level 0 remains dark.
            cfg.savedBrightness = intent;
            const uint8_t effective = uint16_t(intent) * _module._globalBrightness / 255;
            segment->setMasterBrightness(effective);
            if (cfg.emController.isRunning()) cfg.emController.reapplyMasterBrightness(segment);
            else
                segment->setBrightness(effective);
        }
        else
            _module.segmentOff(index, true);
        if (!_module._localTestMode) _module.statusAfterCommand(index + 1);
        return {};
    }
    if (action == "sceneRecall" || action == "sceneStore" || action == "sceneAdd" || action == "sceneDelete")
    {
        if (!_module._sceneManager) return "Scenes unavailable";
        const uint8_t count = _module._sceneManager->getSceneCount(index);
        if (action == "sceneAdd")
        {
            if (count >= SceneManager::MAX_SCENES) return "Maximum scene count reached";
        }
        else if (!integer(value, 1, count) || (action == "sceneDelete" && static_cast<uint32_t>(value.toFloat()) != count))
            return "Invalid scene; only the highest scene can be deleted";
        if (validateOnly) return {};
        const uint8_t previous = _module._channelIndex;
        _module._channelIndex = index;
        bool success = action == "sceneAdd" ? _module._sceneManager->addScene(index, segment) : action == "sceneDelete" ? _module._sceneManager->deleteScene(index, static_cast<uint32_t>(value.toFloat()))
                                                                                            : action == "sceneStore"    ? _module._sceneManager->storeScene(index, static_cast<uint32_t>(value.toFloat()), segment)
                                                                                                                        : _module._sceneManager->recallScene(index, static_cast<uint32_t>(value.toFloat()), segment);
        if (success && action == "sceneRecall") cfg.savedSceneNumber = static_cast<uint32_t>(value.toFloat());
        _module._channelIndex = previous;
        if (success && !_module._localTestMode) _module.statusAfterCommand(index + 1);
        return success ? std::string() : "Scene action failed";
    }
    Effect* effect = validateOnly && expectedEffect != 255 ? getEffectFromType(expectedEffect) : segment->getEffect();
    uint32_t number = 0;
    uint32_t parameter = 0;
    std::string text;
    uint8_t r = 0, g = 0, b = 0;
    const bool channelColour = action == "primaryChannels" || action == "secondaryChannels";
    uint8_t white = 0, cool = 0;
    const bool primary = action == "primary";
    const bool secondary = action == "secondary";
    if (action == "restoreEts")
    {
        if (validateOnly) return "Restore ETS settings cannot be nested";
    }
    else if (action == "effect")
    {
        if (!integer(value, 0, 254)) return "Invalid effect";
        number = static_cast<uint32_t>(value.toFloat());
        if (getTypeFromEffect(getEffectFromType(number)) != number) return "Effect unavailable";
    }
    else if (action == "brightness" || action == "primaryWhite" || action == "secondaryWhite" || action == "primaryCool" || action == "secondaryCool")
    {
        if (!integer(value, 0, 255)) return "Value must be between 0 and 255";
        number = static_cast<uint32_t>(value.toFloat());
        const uint8_t channels = _module._virtualStrip ? _module._virtualStrip->getBytesPerLed() : 3;
        if (action != "brightness" && channels < (action == "primaryCool" || action == "secondaryCool" ? 5 : 4)) return "White channel unavailable";
    }
    else if (primary || secondary || channelColour)
    {
        const Reader rgb = channelColour ? value.get("rgb") : value;
        text = rgb.toString();
        if (!rgb.isString() || text.size() != 7 || text[0] != '#') return "Expected #RRGGBB";
        for (size_t i = 1; i < text.size(); ++i)
            if (!isxdigit(static_cast<unsigned char>(text[i]))) return "Invalid RGB colour";
        number = strtoul(text.c_str() + 1, nullptr, 16);
        r = number >> 16;
        g = number >> 8;
        b = number;
        if (channelColour)
        {
            if (!value.isObject() || !integer(value.get("ww"), 0, 255) || !integer(value.get("cw"), 0, 255)) return "Expected RGB and white channels";
            white = static_cast<uint32_t>(value.get("ww").toFloat());
            cool = static_cast<uint32_t>(value.get("cw").toFloat());
            const uint8_t channels = _module._virtualStrip ? _module._virtualStrip->getBytesPerLed() : 3;
            if ((channels < 4 && white != 0) || (channels < 5 && cool != 0)) return "White channel unavailable";
        }
    }
    else if (action == "parameter")
    {
        if (!effect || !integer(doc.get("effect"), 0, 254) || static_cast<uint32_t>(doc.get("effect").toFloat()) != getTypeFromEffect(effect)) return "Effect changed; refresh the page";
        if (!effect->getParameterCount() || !integer(doc.get("parameter"), 0, effect->getParameterCount() - 1)) return "Unknown parameter";
        parameter = static_cast<uint32_t>(doc.get("parameter").toFloat());
        const ParameterType type = effect->getParameterType(parameter);
        if (type == ParameterType::PARAM_STRING)
        {
            if (!value.isString()) return "Expected text";
            const std::string input = value.toString();
            if (input.find('\0') != std::string::npos || !OpenKNX::Charset::decodeUtf8(text, input.c_str(), input.size()) || text.size() > 240) return "Text must fit 240 Latin-15 characters";
        }
        else
        {
            uint32_t min = effect->getParameterMin(parameter), max = effect->getParameterMax(parameter);
            if (type == ParameterType::PARAM_BOOL)
            {
                min = 0;
                max = 1;
            }
            if (type == ParameterType::PARAM_ENUM && effect->getEnumValueCount(parameter))
            {
                min = 0;
                max = effect->getEnumValueCount(parameter) - 1;
            }
            if (!integer(value, min, max)) return "Parameter outside allowed range";
            number = static_cast<uint32_t>(value.toFloat());
        }
    }
    else
        return "Unknown control action";

    if (validateOnly) return {};

    // All validation precedes takeover: an invalid request must not stop an EM.
    const uint8_t previousChannel = _module._channelIndex;
    _module._channelIndex = index;
    if (action != "brightness") _module.interruptEmIfRunning(index);
    cfg.activeDimming = NeoPixelBusModule::SegmentConfig::NONE;
    if (cfg.syncMode == 2) cfg.localOverride = true;
    if (action == "restoreEts")
    {
        _module.applySegmentDefaultState(index);
        _module.reEnterEffect(index);
        if (_module._sceneManager) _module._sceneManager->restoreEtsScenes(index);
        cfg.savedLastWasEffect = true;
        cfg.hsvTracked = false;
    }
    else if (action == "effect")
    {
        _module.applyEffectToSegment(segment, number);
        cfg.savedEffectType = number;
        cfg.savedEffectValid = true;
        cfg.savedLastWasEffect = true;
    }
    else if (action == "brightness")
    {
        cfg.savedBrightness = number;
        const uint8_t effective = uint16_t(number) * _module._globalBrightness / 255;
        segment->setMasterBrightness(effective);
        if (cfg.emController.isRunning()) cfg.emController.reapplyMasterBrightness(segment);
        else
            segment->setBrightness(effective);
    }
    else if (action == "parameter")
    {
        effect->setParameter(segment, parameter, effect->getParameterType(parameter) == ParameterType::PARAM_STRING ? uint32_t(uintptr_t(text.c_str())) : number);
    }
    else
    {
        const auto& c = segment->getConfig();
        if (action == "primaryChannels") segment->setPrimaryColor(r, g, b, white, cool);
        else if (action == "secondaryChannels")
            segment->setSecondaryColor(r, g, b, white, cool);
        else if (primary)
            segment->setPrimaryColor(r, g, b, c.ww(), c.cw());
        else if (secondary)
            segment->setSecondaryColor(r, g, b, c.ww2(), c.cw2());
        else if (action == "primaryWhite")
            segment->setPrimaryColor(c.r(), c.g(), c.b(), number, c.cw());
        else if (action == "primaryCool")
            segment->setPrimaryColor(c.r(), c.g(), c.b(), c.ww(), number);
        else if (action == "secondaryWhite")
            segment->setSecondaryColor(c.r2(), c.g2(), c.b2(), number, c.cw2());
        else if (action == "secondaryCool")
            segment->setSecondaryColor(c.r2(), c.g2(), c.b2(), c.ww2(), number);
        cfg.hsvTracked = false;
    }
    if (action != "brightness") cfg.savedSceneNumber = 0;
    _module.captureDirectColor(index);
    _module.hideIfNotVisible(index);
    if (!_module._localTestMode)
    {
        _module.statusAfterCommand(1 + index);
        if (cfg.syncMode == 1) _module.sendSyncTelegram(index);
    }
    _module._channelIndex = previousChannel;
    return {};
}

std::string NeoPixelStudio::snapshot()
{
    const uint32_t selected = _requestedSegment.load();
    Writer json;
    json.beginObject().field("ready", true).field("configured", true).field("globalPower", _module._globalPowerOn);
    json.field("completedId", _completedId).field("error", _error);
    json.field("channels", uint32_t(_module._virtualStrip ? _module._virtualStrip->getBytesPerLed() : 3));
    json.key("segments").beginArray();
    for (size_t i = 0; i < _module._segments.size(); ++i)
    {
        const auto& cfg = _module._segments[i];
        Segment* segment = cfg.segment;
        if (!segment) continue;
        const auto& c = segment->getConfig();
        Effect* effect = segment->getEffect();
        json.beginObject().field("id", uint32_t(i + 1)).field("locked", cfg.locked);
        const bool details = selected == i + 1;
        json.field("detailsLoaded", details);
        json.field("power", _module.isSegmentVisible(cfg)).field("em", cfg.emController.isRunning());
        json.field("brightness", uint32_t(cfg.savedBrightness)).field("effect", uint32_t(getTypeFromEffect(effect)));
        color(json, "primary", c.r(), c.g(), c.b(), c.ww(), c.cw());
        color(json, "secondary", c.r2(), c.g2(), c.b2(), c.ww2(), c.cw2());
        json.field("ownPower", cfg.savedPower != 0);
        json.field("activeScene", uint32_t(cfg.savedSceneNumber));
        json.key("scenes").beginArray();
        if (details && _module._sceneManager)
        {
            const uint8_t count = _module._sceneManager->getSceneCount(i);
            for (uint8_t n = 1; n <= count; ++n)
            {
                const auto data = _module._sceneManager->sceneData(i, n);
                json.beginObject().field("id", uint32_t(n)).field("effect", uint32_t(data[0])).field("brightness", uint32_t(data[11]));
                color(json, "primary", data[1], data[2], data[3], data[4], data[5]);
                color(json, "secondary", data[6], data[7], data[8], data[9], data[10]);
                json.key("slots").beginArray();
                for (uint8_t p = 0; p < 10; ++p)
                    json.value(uint32_t(data[12 + p]));
                json.endArray();
                char sceneText[15] = {};
                memcpy(sceneText, data.data() + 22, 14);
                json.field("text", utf8(sceneText)).endObject();
            }
        }
        json.endArray();
        json.key("parameters").beginArray();
        if (effect && details)
        {
            for (uint8_t p = 0; p < effect->getParameterCount(); ++p)
            {
                const auto type = effect->getParameterType(p);
                const char* kind = type == ParameterType::PARAM_BOOL ? "bool" : type == ParameterType::PARAM_ENUM ? "enum"
                                                                            : type == ParameterType::PARAM_STRING ? "text"
                                                                                                                  : "number";
                json.beginObject().field("id", uint32_t(p)).field("name", effect->getParameterName(p)).field("type", kind);
                json.field("min", effect->getParameterMin(p)).field("max", effect->getParameterMax(p));
                if (type == ParameterType::PARAM_STRING) json.field("value", utf8(c.effectText));
                else
                    json.field("value", effect->getParameter(segment, p));
                json.key("options").beginArray();
                if (type == ParameterType::PARAM_ENUM)
                    for (uint8_t n = 0; n < effect->getEnumValueCount(p); ++n)
                        json.value(effect->getEnumValueName(p, n));
                json.endArray().endObject();
            }
        }
        json.endArray().endObject();
    }
    json.endArray().endObject();
    return json.str();
}
#endif
