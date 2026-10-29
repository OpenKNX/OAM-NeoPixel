#include "NeoPixelFlashPersistence.h"
#include "NeoPixelModule.h"
#include "NeoPixelStudio.h"
#include "SceneManager.h"

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    #include "EffectParameterMapping.h"
    #include "EffectTypeMapping.h"
    #include "NetworkModule.h"
    #include "OpenKNX/Charset.hpp"
    #include "OpenKNX/Format/JSON/Reader.h"
    #include "OpenKNX/Format/JSON/Writer.h"
    #include <algorithm>
    #include <cstring>
    #if OPENKNX_LITTLE_FS
        #include <LittleFS.h>
    #endif

using OpenKNX::Format::JSON::Reader;
using OpenKNX::Format::JSON::Writer;

namespace
{
    std::string savedPath(size_t index) { return "/neopixel-studio-" + std::to_string(index + 1) + ".json"; }
    std::string rgb(uint8_t r, uint8_t g, uint8_t b)
    {
        char text[8];
        snprintf(text, sizeof(text), "#%02X%02X%02X", r, g, b);
        return text;
    }
    void number(Writer& json, const char* name, uint32_t value)
    {
        json.beginObject().field("name", name).field("value", value).endObject();
    }
    void text(Writer& json, const char* name, const std::string& value)
    {
        json.beginObject().field("name", name).field("value", value).endObject();
    }
} // namespace

uint32_t NeoPixelStudio::configSignature(size_t index) const
{
    uint32_t signature = 2166136261u;
    for (size_t byte = 0; byte < NEO_ParamBlockSize; ++byte)
        signature = (signature ^ knx.paramByte(NEO_ParamBlockOffset + index * NEO_ParamBlockSize + byte)) * 16777619u;
    return signature;
}

std::string NeoPixelStudio::exportSegment(size_t index, bool forSave, std::string& error) const
{
    const auto& cfg = _module._segments[index];
    Segment* segment = cfg.segment;
    Effect* effect = segment ? segment->getEffect() : nullptr;
    if (!effect)
    {
        error = "Segment hat keinen aktiven Effekt";
        return {};
    }
    if (forSave && cfg.emController.isRunning())
    {
        error = "Vor dem Speichern einen direkten Effekt wählen; ein Effektmanager läuft";
        return {};
    }
    const uint8_t id = getTypeFromEffect(effect);
    const auto& c = segment->getConfig();
    Writer json;
    json.beginObject().field("version", uint32_t(1)).field("segment", uint32_t(index + 1));
    json.field("configSignature", configSignature(index)).field("effect", uint32_t(id));
    json.field("channels", uint32_t(_module._virtualStrip ? _module._virtualStrip->getBytesPerLed() : 3));
    json.key("commissioning"); writeCommissioning(json, index, false, !forSave);
    json.key("parameters").beginArray();
    number(json, "NEOEffectType", id);
    number(json, "SegmentStartupBrightness", cfg.savedBrightness);
    text(json, "SegmentStartupColor", rgb(c.r(), c.g(), c.b()));
    number(json, "SegmentStartupW", c.ww());
    number(json, "SegmentStartupCW", c.cw());
    number(json, "SegmentStartupSeparateWhite", 1);
    text(json, "SegmentStartupSecondaryColor", rgb(c.r2(), c.g2(), c.b2()));
    number(json, "SegmentStartupSecondaryWW", c.ww2());
    number(json, "SegmentStartupSecondaryCW", c.cw2());
    for (uint8_t p = 0; p < effect->getParameterCount(); ++p)
    {
        const char* name = effectParameterEtsName(id, p);
        if (!name)
        {
            error = "Effektzuordnung der Gerätekonfiguration fehlt; Effektparameter neu erzeugen";
            return {};
        }
        if (effect->getParameterType(p) == ParameterType::PARAM_STRING)
        {
            if (forSave && strlen(c.effectText) > 14)
            {
                error = "Die Gerätekonfiguration speichert höchstens 14 Byte Effekttext; Text vor dem Speichern kürzen";
                return {};
            }
            std::string utf8;
            OpenKNX::Charset::encodeUtf8(utf8, c.effectText, strlen(c.effectText));
            text(json, name, utf8);
        }
        else
            number(json, name, effect->getParameter(segment, p));
    }
    json.endArray();
    json.key("scenes").beginArray();
    if (_module._sceneManager)
        for (uint8_t byte : _module._sceneManager->exportScenes(index))
            json.value(uint32_t(byte));
    json.endArray();
    json.key("restore").beginArray();
    auto command = [&](const char* action) { json.beginObject().field("segment", uint32_t(index + 1)).field("action", action).key("value"); };
    command("effect");
    json.value(uint32_t(id)).endObject();
    command("primaryChannels");
    json.beginObject().field("rgb", rgb(c.r(), c.g(), c.b())).field("ww", uint32_t(c.ww())).field("cw", uint32_t(c.cw())).endObject().endObject();
    command("secondaryChannels");
    json.beginObject().field("rgb", rgb(c.r2(), c.g2(), c.b2())).field("ww", uint32_t(c.ww2())).field("cw", uint32_t(c.cw2())).endObject().endObject();
    command("brightness");
    json.value(uint32_t(cfg.savedBrightness)).endObject();
    for (uint8_t p = 0; p < effect->getParameterCount(); ++p)
    {
        command("parameter");
        if (effect->getParameterType(p) == ParameterType::PARAM_STRING)
        {
            std::string value;
            OpenKNX::Charset::encodeUtf8(value, c.effectText, strlen(c.effectText));
            json.value(value);
        }
        else
            json.value(effect->getParameter(segment, p));
        json.field("effect", uint32_t(id)).field("parameter", uint32_t(p)).endObject();
    }
    command("power");
    json.value(uint32_t(cfg.savedPower != 0)).endObject();
    json.endArray().endObject();
    return json.str();
}

std::string NeoPixelStudio::saveSegment(size_t index)
{
    #if OPENKNX_LITTLE_FS
    std::string error;
    const std::string json = exportSegment(index, true, error);
    if (!error.empty()) return error;
    if (!LittleFS.begin()) return "Dateisystem des Geräts nicht verfügbar";
    if (LittleFS.exists("/neopixel-studio-invalidated"))
    {
        for (size_t i = 0; i < 16; ++i)
        {
            const auto old = savedPath(i);
            if (LittleFS.exists(old.c_str()) && !LittleFS.remove(old.c_str())) return "Alte Studio-Stände lassen sich nicht löschen";
        }
        if (!LittleFS.remove("/neopixel-studio-invalidated")) return "Studio-Ungültig-Markierung lässt sich nicht löschen";
    }
    const std::string path = savedPath(index), temporary = path + ".tmp";
    File file = LittleFS.open(temporary.c_str(), "w");
    if (!file) return "Studio-Speicherdatei lässt sich nicht öffnen";
    const bool written = file.write(reinterpret_cast<const uint8_t*>(json.data()), json.size()) == json.size();
    file.flush();
    file.close();
    if (!written || !LittleFS.rename(temporary.c_str(), path.c_str()))
    {
        LittleFS.remove(temporary.c_str());
        return "Studio-Einstellungen nicht gespeichert; der vorherige Stand bleibt";
    }
    openknx.flash.save();
    return {};
    #else
    return "Dateisystem des Geräts ist nicht aktiviert";
    #endif
}

std::string NeoPixelStudio::readSaved(size_t index, std::string& error) const
{
    #if OPENKNX_LITTLE_FS
    if (!LittleFS.begin())
    {
        error = "Device filesystem unavailable";
        return {};
    }
    if (LittleFS.exists("/neopixel-studio-invalidated"))
    {
        error = "Studio saves were invalidated by an ETS download";
        return {};
    }
    File file = LittleFS.open(savedPath(index).c_str(), "r");
    if (!file)
    {
        error = "No Studio settings saved for this segment";
        return {};
    }
    const size_t length = file.size();
    if (!length || length > 8192)
    {
        error = "Invalid Studio save file";
        return {};
    }
    std::string json(length, '\0');
    const bool read = file.read(reinterpret_cast<uint8_t*>(&json[0]), length) == length;
    file.close();
    Reader doc(json.c_str(), json.size());
    if (!read || !doc.isObject() || doc.get("version").toUInt() != 1 || doc.get("segment").toUInt() != index + 1 || !doc.get("parameters").isArray())
    {
        error = "Invalid Studio save file";
        return {};
    }
    if (doc.get("configSignature").toUInt() != configSignature(index))
    {
        error = "ETS download changed this segment; save Studio settings again before importing";
        return {};
    }
    return json;
    #else
    error = "Device filesystem is not enabled";
    return {};
    #endif
}

void NeoPixelStudio::invalidateSaved()
{
    _testRestore.clear(); _testSegment = -1;
    _acceptCommands.store(false);
    // Retire an in-flight LED frame before filesystem writes (also safe on RP2040).
    if (auto* manager = _module._neoPixel.getManager()) manager->flushFrame();
    #if OPENKNX_LITTLE_FS
    if (LittleFS.begin())
    {
        File marker = LittleFS.open("/neopixel-studio-invalidated", "w");
        if (marker)
        {
            marker.write(uint8_t(1));
            marker.close();
        }
        for (size_t i = 0; i < 16; ++i)
            LittleFS.remove(savedPath(i).c_str());
    }
    #endif
    // The scene cache stays: the flash save follows, the ETS scene signature decides at the next start
    _etsExport.clear();
}

void NeoPixelStudio::restoreSaved(bool scenesOnly)
{
    for (size_t index = 0; index < _module._segments.size(); ++index)
    {
        std::string error;
        const std::string json = readSaved(index, error);
        if (!error.empty()) continue;
        const Reader saved(json.c_str());
        if (scenesOnly && saved.get("commissioning").isObject()) stageCommissioning(index, saved.get("commissioning"));
        if (scenesOnly)
        {
            if (_module._flashPersistence && _module._flashPersistence->hasSceneSnapshot()) continue;
            const Reader scenes = saved.get("scenes");
            std::vector<uint8_t> bytes;
            for (size_t i = 0; scenes.get(i).valid() && i <= 360; ++i)
            {
                const Reader value = scenes.get(i);
                if (!value.isNumber() || value.toUInt(256) > 255)
                {
                    bytes.clear();
                    break;
                }
                bytes.push_back(value.toUInt());
            }
            if (_module._sceneManager && !bytes.empty() && !scenes.get(bytes.size()).valid()) _module._sceneManager->importScenes(index, bytes.data(), bytes.size());
            continue;
        }
        auto& cfg = _module._segments[index];
        if (!cfg.segment || _module.effectiveStartupBehavior(index) != 1 || cfg.emController.lastEmId() != EM_NONE || cfg.savedSceneNumber != 0) continue;
        const Reader commands = saved.get("restore");
        if (!commands.isArray() || !commands.get(size_t(0)).isObject()) continue;
        const bool normalSnapshot = _module._flashPersistence && _module._flashPersistence->hasSceneSnapshot();
        if (normalSnapshot && cfg.savedEffectType != saved.get("effect").toUInt()) continue;
        uint8_t effect = getTypeFromEffect(cfg.segment->getEffect());
        bool valid = true;
        size_t count = 0;
        for (; commands.get(count).valid(); ++count)
        {
            const Reader command = commands.get(count);
            const std::string action = command.get("action").toString();
            if (count >= 24 || command.get("segment").toUInt() != index + 1 || (action != "effect" && action != "primaryChannels" && action != "secondaryChannels" && action != "brightness" && action != "power" && action != "parameter") || !applyDoc(command, true, effect).empty())
            {
                valid = false;
                break;
            }
            if (action == "effect") effect = command.get("value").toUInt();
        }
        if (!valid) continue;
        for (size_t i = 0; i < count; ++i)
        {
            // Normal flash owns the latest power/colour/brightness; Studio supplements its parameters.
            if (normalSnapshot && !commands.get(i).get("action").equals("parameter")) continue;
            applyDoc(commands.get(i));
        }
    }
}

bool NeoPixelStudio::readback(uint8_t length, const uint8_t* data, uint8_t* result, uint8_t& resultLength)
{
    auto fail = [&](uint8_t code) { result[0] = code; resultLength = 1; return true; };
    if (length != 5 || data[1] < 1 || data[1] > _module._segments.size() || data[2] > 1) return fail(1);
    const uint16_t offset = (uint16_t(data[3]) << 8) | data[4];
    if (offset == 0)
    {
        std::string error;
        _etsExport = data[2] == 1 ? readSaved(data[1] - 1, error) : exportSegment(data[1] - 1, false, error);
        if (!error.empty() || _etsExport.empty() || _etsExport.size() > 8192) return fail(data[2] == 1 ? 2 : 3);
        ++_etsToken;
        _etsSegment = data[1];
        _etsSource = data[2];
    }
    if (_etsSegment != data[1] || _etsSource != data[2] || offset >= _etsExport.size()) return fail(4);
    const size_t count = std::min<size_t>(42, _etsExport.size() - offset);
    result[0] = 0;
    result[1] = 1;
    result[2] = _etsToken >> 8;
    result[3] = _etsToken & 0xFF;
    result[4] = _etsExport.size() >> 8;
    result[5] = _etsExport.size() & 0xFF;
    memcpy(result + 6, _etsExport.data() + offset, count);
    resultLength = 6 + count;
    return true;
}
#endif
