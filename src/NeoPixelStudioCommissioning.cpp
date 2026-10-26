#include "NeoPixelModule.h"
#include "NeoPixelStudio.h"

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    #include "EffectTypeMapping.h"
    #include "OpenKNX/Format/JSON/Writer.h"
    #include <algorithm>
    #include <cmath>

using OpenKNX::Format::JSON::Reader;
using OpenKNX::Format::JSON::Writer;
namespace
{
    bool bounded(const Reader& value, unsigned low, unsigned high)
    {
        const double n = value.toFloat();
        return value.isNumber() && std::isfinite(n) && n >= low && n <= high && std::floor(n) == n;
    }
} // namespace

std::string NeoPixelStudio::commissioning(size_t index, bool downloaded, bool runtime) const
{
    if (!downloaded && !runtime && !_commissioning[index].empty()) return _commissioning[index];
    uint8_t _channelIndex = index;
    Writer json;
    json.beginObject().field("start", uint32_t(!downloaded && _module._liveLedLayout ? _module._liveSegmentStarts[index] : ParamNEO_NEOSegmentStart)).field("end", uint32_t(!downloaded && _module._liveLedLayout ? _module._liveSegmentEnds[index] : ParamNEO_NEOSegmentEnd));
    json.key("strips").beginArray();
    const unsigned count = std::min<unsigned>(8, ParamNEO_NEONumberOfLEDStrips);
    for (unsigned strip = 0; strip < count; ++strip)
    {
        _channelIndex = strip;
        json.beginObject().field("id", uint32_t(strip + 1)).field("length", uint32_t(downloaded ? ParamNEOSTRIP_NEOLength : _module.configuredStripLength(strip)));
        json.field("skip", uint32_t(downloaded ? ParamNEOSTRIP_NEOSkipFirstLEDs : _module.configuredStripSkip(strip))).endObject();
    }
    return json.endArray().endObject().str();
}

void NeoPixelStudio::writeCommissioning(Writer& json, size_t index, bool downloaded, bool runtime) const
{
    const std::string text = commissioning(index, downloaded, runtime);
    Reader value(text.c_str());
    json.beginObject().field("start", value.get("start").toUInt()).field("end", value.get("end").toUInt()).key("strips").beginArray();
    for (size_t i = 0; value.get("strips").get(i).valid(); ++i)
    {
        const Reader strip = value.get("strips").get(i);
        json.beginObject().field("id", strip.get("id").toUInt()).field("length", strip.get("length").toUInt()).field("skip", strip.get("skip").toUInt()).endObject();
    }
    json.endArray().endObject();
}

std::string NeoPixelStudio::stopTest()
{
    const std::string previous = _testRestore;
    const int index = _testSegment;
    _testRestore.clear();
    _testSegment = -1;
    if (previous.empty() || index < 0) return {};
    _refresh.store(true);
    // Internal cleanup restores the pre-test state even if a KNX lock arrived
    // meanwhile. New web commands still respect that lock and global power.
    auto& cfg = _module._segments[index];
    const bool locked = cfg.locked;
    cfg.locked = false;
    Reader saved(previous.c_str());
    const Reader commands = saved.get("restore");
    std::string error;
    for (size_t i = 0; commands.get(i).valid(); ++i)
    {
        error = applyDoc(commands.get(i));
        if (!error.empty()) break;
    }
    cfg.locked = locked;
    return error;
}

void NeoPixelStudio::tickTest(uint32_t now)
{
    if (_testSegment >= 0 && (int32_t(now - _testUntil) >= 0 || !_module._globalPowerOn || _module._segments[_testSegment].locked))
    {
        _error = stopTest();
        _refresh.store(true);
    }
}

std::string NeoPixelStudio::stageCommissioning(size_t index, const Reader& value)
{
    const Reader strips = value.get("strips");
    const unsigned count = std::min<unsigned>(8, ParamNEO_NEONumberOfLEDStrips);
    if (!value.isObject() || !strips.isArray() || strips.get(count).valid()) return "Invalid strip configuration";
    unsigned total = 0;
    Writer json;
    json.beginObject().field("start", value.get("start").toUInt()).field("end", value.get("end").toUInt()).key("strips").beginArray();
    for (unsigned i = 0; i < count; ++i)
    {
        const Reader strip = strips.get(i);
        if (!strip.isObject() || !bounded(strip.get("id"), i + 1, i + 1) || !bounded(strip.get("length"), 0, 16383) ||
            !bounded(strip.get("skip"), 0, (strip.get("length").toUInt() ? std::min<unsigned>(255, strip.get("length").toUInt() - 1) : 0))) return "Invalid LED count or skipped LEDs";
        total += strip.get("length").toUInt();
        json.beginObject().field("id", strip.get("id").toUInt()).field("length", strip.get("length").toUInt()).field("skip", strip.get("skip").toUInt()).endObject();
    }
    if (total > 65535 || !bounded(value.get("start"), 1, std::min<unsigned>(total, 16383)) || !bounded(value.get("end"), value.get("start").toUInt(), std::min<unsigned>(total, 16383))) return "Segment range must fit the configured strips (LED numbers start at 1)";
    _commissioning[index] = json.endArray().endObject().str();
    return {};
}

std::string NeoPixelStudio::commissioningControl(size_t index, const Reader& doc, bool validateOnly)
{
    if (validateOnly) return "Commissioning commands cannot be nested";
    const std::string action = doc.get("action").toString();
    if (action == "commissioning") return applyLiveCommissioning(index, doc.get("value"));
    if (action == "testStop") return stopTest();
    if (!_module._globalPowerOn) return "Switch global power on before starting an LED test";
    if (_module._segments[index].emController.isRunning()) return "Choose a direct effect before testing; an effect manager is running";
    const Reader value = doc.get("value");
    if (!bounded(value.get("level"), 1, 64)) return "Test brightness must be between 1 and 64";
    const std::string pattern = value.get("pattern").toString();
    const uint8_t channels = _module._virtualStrip ? _module._virtualStrip->getBytesPerLed() : 3;
    const unsigned effect = pattern == "chase" ? 1 : pattern == "cycle" ? 7
                                                                        : 0;
    if (pattern != "red" && pattern != "green" && pattern != "blue" && pattern != "white" && pattern != "warm" && pattern != "cool" && pattern != "chase" && pattern != "cycle") return "Unknown test pattern";
    if ((pattern == "warm" || pattern == "cool") && channels != 5) return "Separate white channels require RGBCCT";
    if (getTypeFromEffect(getEffectFromType(effect)) != effect) return "Test effect is not compiled into this firmware";
    if (!_testRestore.empty())
    {
        const std::string error = stopTest();
        if (!error.empty()) return error;
    }
    std::string error;
    const std::string previous = exportSegment(index, false, error);
    if (!error.empty()) return error;
    Writer batch;
    batch.beginObject().field("segment", uint32_t(index + 1)).field("action", "batch").key("value").beginArray();
    auto item = [&](const char* action, uint32_t n) {
        batch.beginObject().field("segment", uint32_t(index + 1)).field("action", action).field("value", n).endObject();
    };
    item("effect", effect);
    const char* rgb = pattern == "red" ? "#FF0000" : pattern == "green"                                                              ? "#00FF00"
                                                 : pattern == "blue"                                                                 ? "#0000FF"
                                                 : (pattern == "white" && channels == 3) || pattern == "chase" || pattern == "cycle" ? "#FFFFFF"
                                                                                                                                     : "#000000";
    const unsigned warm = pattern == "warm" ? 255 : pattern == "white" && channels >= 4 ? channels == 5 ? 127 : 255
                                                                                        : 0;
    const unsigned cool = pattern == "cool" ? 255 : pattern == "white" && channels == 5 ? 128
                                                                                        : 0;
    batch.beginObject().field("segment", uint32_t(index + 1)).field("action", "primaryChannels").key("value").beginObject().field("rgb", rgb).field("ww", uint32_t(warm)).field("cw", uint32_t(cool)).endObject().endObject();
    batch.beginObject().field("segment", uint32_t(index + 1)).field("action", "secondaryChannels").key("value").beginObject().field("rgb", "#000000").field("ww", uint32_t(0)).field("cw", uint32_t(0)).endObject().endObject();
    item("brightness", value.get("level").toUInt());
    if (effect)
    {
        for (unsigned p = 0; p < (effect == 1 ? 3u : 2u); ++p)
        {
            const unsigned n = p == 0 ? effect == 1 ? 25 : 2 : p == 1 ? effect == 7 && channels == 5 ? 1 : 0
                                                                      : 1;
            batch.beginObject().field("segment", uint32_t(index + 1)).field("action", "parameter").field("effect", uint32_t(effect)).field("parameter", uint32_t(p)).field("value", uint32_t(n)).endObject();
        }
    }
    item("power", 1);
    batch.endArray().endObject();
    error = apply(batch.str());
    if (!error.empty()) return error;
    _testRestore = previous;
    _testSegment = index;
    _testUntil = millis() + 60000;
    return {};
}
#endif
