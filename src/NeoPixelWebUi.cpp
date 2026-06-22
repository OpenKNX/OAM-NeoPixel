#include "NeoPixelModule.h"

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))

    #include "EffectTypeMapping.h"
    #include "HardwareMappingLogic.h"
    #include "NetworkModule.h"
    #include "OpenKNX/Format/JSON/Reader.h"
    #include "OpenKNX/Format/JSON/Writer.h"
    #include "SceneManager.h"
    #include "WebUi/WebUiStaticAssets.h"
    #include "colorhelper.h"
    #include "versions.h"
    #include <algorithm>
    #include <cctype>
    #include <cmath>

    #if defined(ARDUINO_ARCH_ESP32)
        #include <Arduino.h>
    #endif

using OpenKNX::Format::JSON::Reader;
using OpenKNX::Format::JSON::Writer;
using OpenKNX::Network::WEB_GET;
using OpenKNX::Network::WEB_POST;
using OpenKNX::Network::WebRequest;
using OpenKNX::Network::WebResponse;
using OpenKNX::Network::Webserver;

namespace
{
    constexpr const char* kWebUiMenuPath = "/neopixel";
    constexpr const char* kWebUiApiBasePath = "/neopixel/api";
    constexpr uint16_t kWebUiMinTemperatureKelvin = 2000;
    constexpr uint16_t kWebUiMaxTemperatureKelvin = 6500;
    constexpr uint16_t kWebUiDefaultTemperatureKelvin = 3500;
    constexpr uint8_t kWebUiMaxEffectType = NEO_EFFECT_TYPE_MAX_ID;
    constexpr uint8_t kWebUiMaxSegments = 16;
    constexpr uint8_t kWebUiMaxScenesPerSegment = 10;
    constexpr uint32_t kWebUiPersistSaveStartDelayMs = 150;

    #if defined(ARDUINO_ARCH_ESP32)
    struct WebUiHeapSnapshot
    {
        uint32_t free = 0;
        uint32_t minimum = 0;
    };

    WebUiHeapSnapshot webUiHeapSnapshot()
    {
        return {ESP.getFreeHeap(), ESP.getMinFreeHeap()};
    }
    #endif

    struct WebUiRgb
    {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
    };

    struct WebUiColorDetails
    {
        uint16_t hueDegrees = 0;
        uint8_t saturationPercent = 0;
        uint8_t valuePercent = 0;
        uint8_t white = 0;
        uint16_t temperature = kWebUiDefaultTemperatureKelvin;
    };

    struct WebUiAppliedColor
    {
        WebUiColorDetails details;
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        uint8_t warmWhite = 0;
        uint8_t coolWhite = 0;
    };

    class WebUiPersistSaveGuard
    {
      public:
        WebUiPersistSaveGuard(NeoPixel& neoPixel, NeoPixelManager* manager, VirtualStrip* virtualStrip, bool enabled)
            : _neoPixel(neoPixel), _manager(manager), _virtualStrip(virtualStrip), _enabled(enabled),
              _restoreAutoUpdate(enabled && neoPixel.getAutoUpdate())
        {
            if (!_enabled)
            {
                return;
            }

            if (_restoreAutoUpdate)
            {
                _neoPixel.setAutoUpdate(false);
            }

            waitForOutputIdle();
        }

        ~WebUiPersistSaveGuard()
        {
            if (_restoreAutoUpdate)
            {
                _neoPixel.setAutoUpdate(true);
            }
        }

        void waitForOutputIdle() const
        {
            if (!_enabled)
            {
                return;
            }

            if (_manager != nullptr)
            {
                _manager->waitForAll(100);
            }
            else if (_virtualStrip != nullptr)
            {
                _virtualStrip->waitForCompletion(100);
            }
        }

        bool shouldRestoreAutoUpdate() const
        {
            return _restoreAutoUpdate;
        }

        void releaseAutoUpdateRestore()
        {
            _restoreAutoUpdate = false;
        }

      private:
        NeoPixel& _neoPixel;
        NeoPixelManager* _manager;
        VirtualStrip* _virtualStrip;
        bool _enabled = false;
        bool _restoreAutoUpdate = false;
    };

    WebUiRgb webUiKelvinRgb(uint16_t kelvin);
    WebUiRgb webUiBlendRgb(const WebUiRgb& source, const WebUiRgb& target, float factor);

    std::string webUiBuildPersistQueuedJson()
    {
        Writer json;
        json.beginObject();
        json.field("queued", true);
        json.field("message", "Persist save queued");
        json.endObject();
        return json.str();
    }

    const std::string& webUiLayoutFragment()
    {
        static const std::string fragment = []() {
            constexpr const char kBodyStartToken[] = "<body>";
            constexpr const char kBodyEndToken[] = "</body>";

            std::string html(kNeoPixelWebUiIndexHtml);
            size_t bodyStart = html.find(kBodyStartToken);
            size_t bodyEnd = html.rfind(kBodyEndToken);

            std::string result;
            result.reserve(html.size() + 64);
            result += "<link rel=\"stylesheet\" href=\"/neopixel/app.css\">";

            if (bodyStart == std::string::npos || bodyEnd == std::string::npos || bodyEnd <= bodyStart)
            {
                result += html;
                return result;
            }

            bodyStart += sizeof(kBodyStartToken) - 1;
            result.append(html, bodyStart, bodyEnd - bodyStart);
            return result;
        }();

        return fragment;
    }

    template <typename T>
    T clampValue(T value, T minValue, T maxValue)
    {
        return std::min(std::max(value, minValue), maxValue);
    }

    char fromHexChar(char ch)
    {
        if (ch >= '0' && ch <= '9') return static_cast<char>(ch - '0');
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        if (ch >= 'a' && ch <= 'f') return static_cast<char>(10 + ch - 'a');
        return 0;
    }

    std::string webUiUrlDecode(const std::string& value)
    {
        std::string decoded;
        decoded.reserve(value.size());

        for (size_t index = 0; index < value.size(); ++index)
        {
            char ch = value[index];
            if (ch == '+')
            {
                decoded.push_back(' ');
                continue;
            }

            if (ch == '%' && index + 2 < value.size())
            {
                char hi = fromHexChar(value[index + 1]);
                char lo = fromHexChar(value[index + 2]);
                decoded.push_back(static_cast<char>((hi << 4) | lo));
                index += 2;
                continue;
            }

            decoded.push_back(ch);
        }

        return decoded;
    }

    std::string webUiQueryValue(const std::string& uri, const char* key)
    {
        size_t queryPos = uri.find('?');
        if (queryPos == std::string::npos)
        {
            return "";
        }

        const std::string query = uri.substr(queryPos + 1);
        const std::string needle = std::string(key) + "=";
        size_t start = 0;
        while (start < query.size())
        {
            size_t end = query.find('&', start);
            if (end == std::string::npos)
            {
                end = query.size();
            }

            const std::string pair = query.substr(start, end - start);
            if (pair.rfind(needle, 0) == 0)
            {
                return webUiUrlDecode(pair.substr(needle.size()));
            }

            start = end + 1;
        }

        return "";
    }

    std::string webUiSlugify(const char* text)
    {
        if (text == nullptr)
        {
            return "item";
        }

        std::string slug;
        bool lastWasDash = false;
        for (const char* cursor = text; *cursor != '\0'; ++cursor)
        {
            const unsigned char current = static_cast<unsigned char>(*cursor);
            if (std::isalnum(current))
            {
                slug.push_back(static_cast<char>(std::tolower(current)));
                lastWasDash = false;
            }
            else if (!slug.empty() && !lastWasDash)
            {
                slug.push_back('-');
                lastWasDash = true;
            }
        }

        while (!slug.empty() && slug.back() == '-')
        {
            slug.pop_back();
        }

        return slug.empty() ? "item" : slug;
    }

    bool webUiBoolFromString(const std::string& value)
    {
        std::string normalized;
        normalized.reserve(value.size());
        for (char ch : value)
        {
            normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }

        return normalized == "1" || normalized == "true" || normalized == "yes" || normalized == "on";
    }

    uint8_t webUiPercentToByte(uint8_t percent)
    {
        return static_cast<uint8_t>(std::round((clampValue<int>(percent, 0, 100) * 255.0f) / 100.0f));
    }

    uint8_t webUiHueDegreesToByte(uint16_t degrees)
    {
        return static_cast<uint8_t>(std::round((clampValue<int>(degrees, 0, 360) * 255.0f) / 360.0f));
    }

    uint8_t webUiByteToPercent(uint8_t value)
    {
        return static_cast<uint8_t>(std::round((value * 100.0f) / 255.0f));
    }

    uint16_t webUiByteToHueDegrees(uint8_t value)
    {
        return static_cast<uint16_t>(std::round((value * 360.0f) / 255.0f));
    }

    uint16_t webUiTemperatureFromWhiteChannels(uint8_t warmWhite, uint8_t coolWhite, uint16_t fallbackTemperature)
    {
        const uint16_t totalWhite = static_cast<uint16_t>(warmWhite) + static_cast<uint16_t>(coolWhite);
        if (totalWhite == 0)
        {
            return fallbackTemperature;
        }

        const float coolRatio = static_cast<float>(coolWhite) / static_cast<float>(totalWhite);
        const float temperature = kWebUiMinTemperatureKelvin + coolRatio * (kWebUiMaxTemperatureKelvin - kWebUiMinTemperatureKelvin);
        return static_cast<uint16_t>(std::round(temperature));
    }

    WebUiColorDetails webUiColorDetailsFromChannels(uint8_t bytesPerLed, uint16_t fallbackTemperature,
                                                    uint8_t red, uint8_t green, uint8_t blue,
                                                    uint8_t warmWhite, uint8_t coolWhite)
    {
        WebUiColorDetails color;

        uint8_t hue = 0;
        uint8_t saturation = 0;
        uint8_t value = 0;
        ColorHelper::rgbToHSV(red, green, blue, hue, saturation, value);

        color.hueDegrees = webUiByteToHueDegrees(hue);
        color.saturationPercent = webUiByteToPercent(saturation);
        color.valuePercent = webUiByteToPercent(value);

        if (bytesPerLed == 5)
        {
            color.white = static_cast<uint8_t>(std::min<uint16_t>(255, static_cast<uint16_t>(warmWhite) + static_cast<uint16_t>(coolWhite)));
            color.temperature = webUiTemperatureFromWhiteChannels(warmWhite, coolWhite, fallbackTemperature);
        }
        else if (bytesPerLed == 4)
        {
            color.white = warmWhite;
            color.temperature = fallbackTemperature;
        }
        else
        {
            color.white = 0;
            color.temperature = fallbackTemperature;
        }

        return color;
    }

    void webUiWriteColorJson(Writer& json, const char* key, const WebUiColorDetails& color)
    {
        json.key(key).beginObject();
        json.key("hsv").beginObject();
        json.field("h", static_cast<uint32_t>(color.hueDegrees));
        json.field("s", static_cast<uint32_t>(color.saturationPercent));
        json.field("v", static_cast<uint32_t>(color.valuePercent));
        json.endObject();
        json.field("white", static_cast<uint32_t>(color.white));
        json.field("temperature", static_cast<uint32_t>(color.temperature));
        json.endObject();
    }

    WebUiAppliedColor webUiAppliedColorFromPatch(const Reader& colorPatch, const WebUiColorDetails& fallbackColor, uint8_t bytesPerLed)
    {
        WebUiAppliedColor applied;
        const Reader hsvPatch = colorPatch.get("hsv");

        applied.details.hueDegrees = clampValue<uint16_t>(static_cast<uint16_t>(hsvPatch.get("h").toUInt(fallbackColor.hueDegrees)), 0, 360);
        applied.details.saturationPercent = static_cast<uint8_t>(clampValue<int>(static_cast<int>(hsvPatch.get("s").toUInt(fallbackColor.saturationPercent)), 0, 100));
        applied.details.valuePercent = static_cast<uint8_t>(clampValue<int>(static_cast<int>(hsvPatch.get("v").toUInt(fallbackColor.valuePercent)), 0, 100));
        applied.details.white = static_cast<uint8_t>(clampValue<int>(static_cast<int>(colorPatch.get("white").toUInt(fallbackColor.white)), 0, 255));
        applied.details.temperature = clampValue<uint16_t>(static_cast<uint16_t>(colorPatch.get("temperature").toUInt(fallbackColor.temperature)),
                                                           kWebUiMinTemperatureKelvin, kWebUiMaxTemperatureKelvin);

        const uint8_t hue = webUiHueDegreesToByte(applied.details.hueDegrees);
        const uint8_t saturation = webUiPercentToByte(applied.details.saturationPercent);
        const uint8_t value = webUiPercentToByte(applied.details.valuePercent);
        ColorHelper::hsvToRGB(hue, saturation, value, applied.red, applied.green, applied.blue);

        if (bytesPerLed == 5)
        {
            const float coolRatio = clampValue<float>(static_cast<float>(applied.details.temperature - kWebUiMinTemperatureKelvin) /
                                                          static_cast<float>(kWebUiMaxTemperatureKelvin - kWebUiMinTemperatureKelvin),
                                                      0.0f, 1.0f);
            applied.coolWhite = static_cast<uint8_t>(std::round(applied.details.white * coolRatio));
            applied.warmWhite = static_cast<uint8_t>(applied.details.white - applied.coolWhite);
        }
        else if (bytesPerLed == 4)
        {
            applied.warmWhite = applied.details.white;
        }
        else
        {
            const WebUiRgb composed = webUiBlendRgb({applied.red, applied.green, applied.blue},
                                                    webUiKelvinRgb(applied.details.temperature),
                                                    (applied.details.white / 255.0f) * 0.72f);
            applied.red = composed.r;
            applied.green = composed.g;
            applied.blue = composed.b;
        }

        return applied;
    }

    WebUiRgb webUiKelvinRgb(uint16_t kelvin)
    {
        WebUiRgb rgb;
        ColorHelper::kelvinToRGB(kelvin, rgb.r, rgb.g, rgb.b);
        return rgb;
    }

    WebUiRgb webUiBlendRgb(const WebUiRgb& source, const WebUiRgb& target, float factor)
    {
        factor = clampValue<float>(factor, 0.0f, 1.0f);

        WebUiRgb result;
        result.r = static_cast<uint8_t>(std::round(source.r + (target.r - source.r) * factor));
        result.g = static_cast<uint8_t>(std::round(source.g + (target.g - source.g) * factor));
        result.b = static_cast<uint8_t>(std::round(source.b + (target.b - source.b) * factor));
        return result;
    }

    WebUiRgb webUiScaleRgb(const WebUiRgb& input, float factor)
    {
        factor = clampValue<float>(factor, 0.0f, 1.0f);

        WebUiRgb result;
        result.r = static_cast<uint8_t>(clampValue<int>(static_cast<int>(std::round(input.r * factor)), 0, 255));
        result.g = static_cast<uint8_t>(clampValue<int>(static_cast<int>(std::round(input.g * factor)), 0, 255));
        result.b = static_cast<uint8_t>(clampValue<int>(static_cast<int>(std::round(input.b * factor)), 0, 255));
        return result;
    }

    std::string webUiRgbHex(const WebUiRgb& rgb)
    {
        char buffer[8];
        snprintf(buffer, sizeof(buffer), "#%02x%02x%02x", rgb.r, rgb.g, rgb.b);
        return std::string(buffer);
    }

    void webUiPreviewPalette(uint8_t r, uint8_t g, uint8_t b, uint8_t white, uint16_t temperature, uint8_t brightness,
                             std::string& shadowHex, std::string& highlightHex)
    {
        const WebUiRgb baseRgb = {r, g, b};
        const WebUiRgb whiteRgb = webUiKelvinRgb(temperature);

        auto compose = [&](uint8_t whiteAmount, uint8_t brightnessValue) -> WebUiRgb {
            const float whiteFactor = (whiteAmount / 255.0f) * 0.72f;
            const float brightnessFactor = 0.24f + (clampValue<int>(brightnessValue, 1, 255) / 255.0f) * 0.76f;
            return webUiScaleRgb(webUiBlendRgb(baseRgb, whiteRgb, whiteFactor), brightnessFactor);
        };

        const uint8_t shadowWhite = white > 18 ? static_cast<uint8_t>(white - 18) : 0;
        const uint8_t shadowBrightness = brightness > 56 ? static_cast<uint8_t>(brightness - 56) : 24;
        const uint8_t highlightWhite = static_cast<uint8_t>(clampValue<int>(white + 56, 0, 255));
        const uint8_t highlightBrightness = static_cast<uint8_t>(clampValue<int>(brightness + 30, 0, 255));

        shadowHex = webUiRgbHex(compose(shadowWhite, shadowBrightness));
        highlightHex = webUiRgbHex(compose(highlightWhite, highlightBrightness));
    }

    std::string webUiEffectId(uint8_t effectType)
    {
        Effect* effect = getEffectFromType(effectType);
        return webUiSlugify(effect ? effect->getName("en") : "Solid");
    }

    uint8_t webUiResolveEffectType(const std::string& effectId, uint8_t fallbackEffectType)
    {
        if (effectId.empty())
        {
            return fallbackEffectType;
        }

        for (uint8_t effectType = 0; effectType <= kWebUiMaxEffectType; ++effectType)
        {
            if (webUiEffectId(effectType) == effectId)
            {
                return effectType;
            }
        }

        return fallbackEffectType;
    }

    const char* webUiPlatformFamily()
    {
    #ifdef ARDUINO_ARCH_ESP32
        return "esp32";
    #elif defined(ARDUINO_ARCH_RP2040)
        return "rp2040";
    #else
        return "unknown";
    #endif
    }

    const char* webUiNetworkMedium()
    {
    #ifdef KNX_IP_WIFI
        return "wifi";
    #elif defined(KNX_IP_LAN)
        return "ethernet";
    #else
        return "offline";
    #endif
    }

    std::string webUiSegmentLabel(uint8_t segmentIndex)
    {
        return std::string("Segment ") + std::to_string(segmentIndex + 1);
    }

    void webUiSendJson(WebResponse& res, uint16_t statusCode, const std::string& json)
    {
        res.setStatus(statusCode);
        res.setContentType("application/json; charset=utf-8");
        res.setHeader("Cache-Control", "no-store");
        res.send(json.c_str());
    }

    void webUiSendError(WebResponse& res, uint16_t statusCode, const char* message)
    {
        Writer json;
        json.beginObject();
        json.field("error", message);
        json.endObject();
        webUiSendJson(res, statusCode, json.str());
    }
} // namespace

void NeoPixelBusModule::registerWebUiRoutes()
{
    if (_webUiRoutesRegistered)
    {
        return;
    }

    openknxNetwork.webserver.addMenuItem("NeoPixel Studio", kWebUiMenuPath, 20);
    openknxNetwork.webserver.addRoute(WEB_GET, kWebUiMenuPath, [this](WebRequest& req, WebResponse& res) {
        handleWebUiIndexRequest(req, res);
    });
    openknxNetwork.webserver.addRoute(WEB_GET, "/neopixel/", Webserver::Redirect(kWebUiMenuPath));

    openknxNetwork.webserver.addRoute(WEB_GET, "/neopixel/index.html", [](WebRequest&, WebResponse& res) {
        res.setContentType("text/html; charset=utf-8");
        res.setHeader("Cache-Control", "no-store");
        res.sendStatic(kNeoPixelWebUiIndexHtml);
    });
    openknxNetwork.webserver.addRoute(WEB_GET, "/neopixel/app.css", [](WebRequest&, WebResponse& res) {
        res.setContentType("text/css; charset=utf-8");
        res.setHeader("Cache-Control", "no-store");
        res.sendStatic(kNeoPixelWebUiAppCss);
    });
    openknxNetwork.webserver.addRoute(WEB_GET, "/neopixel/app.js", [](WebRequest&, WebResponse& res) {
        res.setContentType("application/javascript; charset=utf-8");
        res.setHeader("Cache-Control", "no-store");
        res.sendStatic(kNeoPixelWebUiAppJs);
    });

    openknxNetwork.webserver.addRoute(WEB_GET, std::string(kWebUiApiBasePath) + "/meta", [this](WebRequest& req, WebResponse& res) {
        handleWebUiMetaRequest(req, res);
    });
    openknxNetwork.webserver.addRoute(WEB_GET, std::string(kWebUiApiBasePath) + "/state", [this](WebRequest& req, WebResponse& res) {
        handleWebUiStateRequest(req, res);
    });
    openknxNetwork.webserver.addRoute(WEB_POST, std::string(kWebUiApiBasePath) + "/state", [this](WebRequest& req, WebResponse& res) {
        handleWebUiStatePatchRequest(req, res);
    });

    _webUiRoutesRegistered = true;
}

void NeoPixelBusModule::handleWebUiIndexRequest(WebRequest&, WebResponse& res)
{
    res.setContentType("text/html; charset=utf-8");
    res.setHeader("Cache-Control", "no-store");
    res.setLayout(true);
    res.setActiveMenu(kWebUiMenuPath);
    res.sendStatic(webUiLayoutFragment().c_str());
}

void NeoPixelBusModule::handleWebUiMetaRequest(WebRequest& req, WebResponse& res)
{
    const std::string language = webUiQueryValue(req.uri, "lang").empty() ? "en" : webUiQueryValue(req.uri, "lang");

    #if defined(ARDUINO_ARCH_ESP32)
    const WebUiHeapSnapshot heapBefore = webUiHeapSnapshot();
    #endif

    {
        std::string json = buildWebUiMetaJson(language.c_str());

    #if defined(ARDUINO_ARCH_ESP32)
        const WebUiHeapSnapshot heapAfterBuild = webUiHeapSnapshot();
        logInfoP("WebUI meta heap: before=%u afterBuild=%u min=%u payload=%u",
                 heapBefore.free,
                 heapAfterBuild.free,
                 heapAfterBuild.minimum,
                 static_cast<unsigned>(json.size()));
    #endif

        webUiSendJson(res, 200, json);
    }

    #if defined(ARDUINO_ARCH_ESP32)
    const WebUiHeapSnapshot heapAfterSend = webUiHeapSnapshot();
    logInfoP("WebUI meta heap released: free=%u min=%u", heapAfterSend.free, heapAfterSend.minimum);
    #endif
}

void NeoPixelBusModule::handleWebUiStateRequest(WebRequest& req, WebResponse& res)
{
    uint8_t requestedSegmentId = 0;
    const std::string segmentId = webUiQueryValue(req.uri, "segmentId");
    const std::string segment = segmentId.empty() ? webUiQueryValue(req.uri, "segment") : segmentId;
    if (!segment.empty())
    {
        requestedSegmentId = static_cast<uint8_t>(std::max(0, atoi(segment.c_str())));
    }

    bool includeScenes = true;
    const std::string includeScenesValue = webUiQueryValue(req.uri, "includeScenes");
    if (!includeScenesValue.empty())
    {
        includeScenes = webUiBoolFromString(includeScenesValue);
    }

    #if defined(ARDUINO_ARCH_ESP32)
    const WebUiHeapSnapshot heapBefore = webUiHeapSnapshot();
    #endif

    {
        std::string json = buildWebUiStateJson(false, requestedSegmentId, includeScenes);

    #if defined(ARDUINO_ARCH_ESP32)
        const WebUiHeapSnapshot heapAfterBuild = webUiHeapSnapshot();
        logInfoP("WebUI state heap: before=%u afterBuild=%u min=%u payload=%u scenes=%d segment=%u",
                 heapBefore.free,
                 heapAfterBuild.free,
                 heapAfterBuild.minimum,
                 static_cast<unsigned>(json.size()),
                 includeScenes ? 1 : 0,
                 static_cast<unsigned>(requestedSegmentId));
    #endif

        webUiSendJson(res, 200, json);
    }

    #if defined(ARDUINO_ARCH_ESP32)
    const WebUiHeapSnapshot heapAfterSend = webUiHeapSnapshot();
    logInfoP("WebUI state heap released: free=%u min=%u scenes=%d segment=%u",
             heapAfterSend.free,
             heapAfterSend.minimum,
             includeScenes ? 1 : 0,
             static_cast<unsigned>(requestedSegmentId));
    #endif
}

std::string NeoPixelBusModule::buildWebUiMetaJson(const char* language) const
{
    Writer json;
    const char* hardwareName = HardwareMapping::getHardwareName(getCurrentHardwareIndex());
    const char* hostName = openknxNetwork.hostName();
    std::string host = (hostName != nullptr && hostName[0] != '\0') ? hostName : "offline";
    if (host == "offline" && openknxNetwork.established())
    {
        host = std::string(openknxNetwork.localIP().toString().c_str());
    }

    json.beginObject();

    json.key("device").beginObject();
    json.field("product", "OAM NeoPixel");
    json.field("firmware", openknx.info.humanFirmwareVersion(true));
    json.field("hardwareName", hardwareName != nullptr ? hardwareName : "Unknown");
    json.field("platformFamily", webUiPlatformFamily());
    json.field("hostname", host);
    json.field("networkMedium", webUiNetworkMedium());
    json.endObject();

    json.key("capabilities").beginObject();
    json.field("maxSegments", static_cast<uint32_t>(kWebUiMaxSegments));
    json.field("maxScenesPerSegment", static_cast<uint32_t>(kWebUiMaxScenesPerSegment));
    json.field("supportsScenes", true);
    json.field("supportsPersist", true);
    json.field("supportsEtsReadback", true);
    json.field("supportsHcl", true);
    json.field("supportsSecondaryColor", true);
    json.field("supportsRgbw", _virtualStrip != nullptr && _virtualStrip->getBytesPerLed() >= 4);
    json.field("supportsRgbcct", true);
    json.endObject();

    json.key("segments").beginArray();
    if (_segments.empty())
    {
        json.beginObject();
        json.field("id", static_cast<uint32_t>(1));
        json.field("label", "Unconfigured");
        json.field("configuredScenes", static_cast<uint32_t>(0));
        json.endObject();
    }
    else
    {
        for (size_t index = 0; index < _segments.size(); ++index)
        {
            json.beginObject();
            json.field("id", static_cast<uint32_t>(index + 1));
            json.field("label", webUiSegmentLabel(static_cast<uint8_t>(index)));
            json.field("configuredScenes",
                       static_cast<uint32_t>(_sceneManager ? _sceneManager->getSceneCount(static_cast<uint8_t>(index)) : 0));
            json.endObject();
        }
    }
    json.endArray();

    json.key("effects").beginArray();
    for (uint8_t effectType = 0; effectType <= kWebUiMaxEffectType; ++effectType)
    {
        Effect* effect = getEffectFromType(effectType);
        if (effect == nullptr)
        {
            continue;
        }

        json.beginObject();
        json.field("id", webUiEffectId(effectType));
        json.field("name", effect->getName(language));
        json.field("family", effectType == static_cast<uint8_t>(PT_NEOEffectType::Solid) ? "Base" : "Built-in");
        json.field("mood", effectType == static_cast<uint8_t>(PT_NEOEffectType::Solid) ? "static" : "animated");
        json.field("description", effect->getDescription(language));

        json.key("parameters").beginArray();
        for (uint8_t parameterIndex = 0; parameterIndex < effect->getParameterCount(); ++parameterIndex)
        {
            json.beginObject();
            json.field("key", webUiSlugify(effect->getParameterName(parameterIndex)));
            json.field("label", effect->getParameterName(parameterIndex));

            const ParameterType parameterType = effect->getParameterType(parameterIndex);
            if (parameterType == ParameterType::PARAM_BOOL)
            {
                json.field("type", "toggle");
            }
            else if (parameterType == ParameterType::PARAM_ENUM && effect->getEnumValueCount(parameterIndex) > 0)
            {
                json.field("type", "select");
                json.key("options").beginArray();
                for (uint8_t enumIndex = 0; enumIndex < effect->getEnumValueCount(parameterIndex); ++enumIndex)
                {
                    const char* optionName = effect->getEnumValueName(parameterIndex, enumIndex);
                    if (optionName != nullptr)
                    {
                        json.value(optionName);
                    }
                }
                json.endArray();
            }
            else
            {
                json.field("type", "range");
                json.field("min", effect->getParameterMin(parameterIndex));
                json.field("max", effect->getParameterMax(parameterIndex));
                json.field("step", static_cast<uint32_t>(1));
            }

            json.endObject();
        }
        json.endArray();
        json.endObject();
    }
    json.endArray();

    json.endObject();
    return json.str();
}

std::string NeoPixelBusModule::buildWebUiStateJson(bool runtimeAndPersisted, uint8_t requestedSegmentId, bool includeScenes) const
{
    Writer json;
    const uint8_t bytesPerLed = _virtualStrip ? _virtualStrip->getBytesPerLed() : 3;
    const uint16_t fallbackTemperature = getCurrentHclTemperature() > 0 ? getCurrentHclTemperature() : kWebUiDefaultTemperatureKelvin;

    json.beginObject();

    json.key("connection").beginObject();
    json.field("online", openknxNetwork.connected() || openknxNetwork.established());
    json.field("runtimeMode", runtimeAndPersisted ? "runtime-and-persisted" : "runtime-only");
    json.field("source", "device");
    json.endObject();

    json.key("segments").beginArray();
    if (_segments.empty())
    {
        json.beginObject();
        json.field("id", static_cast<uint32_t>(1));
        json.field("name", "Unconfigured");

        json.key("runtime").beginObject();
        json.field("power", false);
        json.field("brightness", static_cast<uint32_t>(0));
        const WebUiColorDetails emptyColor = {};
        webUiWriteColorJson(json, "primaryColor", emptyColor);
        webUiWriteColorJson(json, "secondaryColor", emptyColor);
        json.key("hsv").beginObject();
        json.field("h", static_cast<uint32_t>(0));
        json.field("s", static_cast<uint32_t>(0));
        json.field("v", static_cast<uint32_t>(0));
        json.endObject();
        json.field("white", static_cast<uint32_t>(0));
        json.field("temperature", static_cast<uint32_t>(fallbackTemperature));
        json.field("effectId", webUiEffectId(static_cast<uint8_t>(PT_NEOEffectType::Solid)));
        json.key("parameters").beginObject();
        json.endObject();
        json.endObject();

        json.key("scenes").beginArray();
        json.endArray();
        json.endObject();
    }
    else
    {
        for (size_t index = 0; index < _segments.size(); ++index)
        {
            const uint8_t apiSegmentId = static_cast<uint8_t>(index + 1);
            if (requestedSegmentId != 0 && requestedSegmentId != apiSegmentId)
            {
                continue;
            }

            const SegmentConfig& cfg = _segments[index];
            Segment* segmentPtr = cfg.segment;
            if (segmentPtr == nullptr)
            {
                continue;
            }

            const uint8_t red = segmentPtr->getConfig().r();
            const uint8_t green = segmentPtr->getConfig().g();
            const uint8_t blue = segmentPtr->getConfig().b();
            const uint8_t warmWhite = segmentPtr->getConfig().ww();
            const uint8_t coolWhite = segmentPtr->getConfig().cw();
            const uint8_t secondaryRed = segmentPtr->getConfig().r2();
            const uint8_t secondaryGreen = segmentPtr->getConfig().g2();
            const uint8_t secondaryBlue = segmentPtr->getConfig().b2();
            const uint8_t secondaryWarmWhite = segmentPtr->getConfig().ww2();
            const uint8_t secondaryCoolWhite = segmentPtr->getConfig().cw2();
            const uint8_t effectType = segmentPtr->getConfig().effectType;
            const WebUiColorDetails primaryColor = webUiColorDetailsFromChannels(bytesPerLed, fallbackTemperature, red, green, blue, warmWhite, coolWhite);
            const WebUiColorDetails secondaryColor = webUiColorDetailsFromChannels(bytesPerLed, fallbackTemperature,
                                                                                   secondaryRed, secondaryGreen, secondaryBlue,
                                                                                   secondaryWarmWhite, secondaryCoolWhite);

            json.beginObject();
            json.field("id", static_cast<uint32_t>(apiSegmentId));
            json.field("name", webUiSegmentLabel(static_cast<uint8_t>(index)));
            json.field("configuredScenes",
                       static_cast<uint32_t>(_sceneManager ? _sceneManager->getSceneCount(static_cast<uint8_t>(index)) : 0));

            json.key("runtime").beginObject();
            json.field("power", cfg.savedPower != 0);
            json.field("brightness", static_cast<uint32_t>(segmentPtr->getBrightness()));
            webUiWriteColorJson(json, "primaryColor", primaryColor);
            webUiWriteColorJson(json, "secondaryColor", secondaryColor);

            json.key("hsv").beginObject();
            json.field("h", static_cast<uint32_t>(primaryColor.hueDegrees));
            json.field("s", static_cast<uint32_t>(primaryColor.saturationPercent));
            json.field("v", static_cast<uint32_t>(primaryColor.valuePercent));
            json.endObject();

            json.field("white", static_cast<uint32_t>(primaryColor.white));
            json.field("temperature", static_cast<uint32_t>(primaryColor.temperature));
            json.field("effectId", webUiEffectId(effectType));

            json.key("parameters").beginObject();
            Effect* effect = segmentPtr->getEffect();
            if (effect == nullptr)
            {
                effect = getEffectFromType(effectType);
            }

            if (effect != nullptr)
            {
                for (uint8_t parameterIndex = 0; parameterIndex < effect->getParameterCount(); ++parameterIndex)
                {
                    const std::string parameterKey = webUiSlugify(effect->getParameterName(parameterIndex));
                    const ParameterType parameterType = effect->getParameterType(parameterIndex);
                    const uint32_t parameterValue = effect->getParameter(segmentPtr, parameterIndex);

                    if (parameterType == ParameterType::PARAM_BOOL)
                    {
                        json.field(parameterKey.c_str(), parameterValue != 0);
                    }
                    else if (parameterType == ParameterType::PARAM_ENUM && effect->getEnumValueCount(parameterIndex) > 0)
                    {
                        const char* optionName = effect->getEnumValueName(parameterIndex, static_cast<uint8_t>(parameterValue));
                        json.field(parameterKey.c_str(), optionName != nullptr ? optionName : "");
                    }
                    else
                    {
                        json.field(parameterKey.c_str(), parameterValue);
                    }
                }
            }
            json.endObject();

            json.endObject();

            json.key("scenes").beginArray();
            if (includeScenes && _sceneManager != nullptr)
            {
                const uint8_t sceneCount = _sceneManager->getSceneCount(static_cast<uint8_t>(index));
                for (uint8_t sceneNumber = 1; sceneNumber <= sceneCount; ++sceneNumber)
                {
                    SceneManager::SceneSnapshot snapshot;
                    if (!_sceneManager->tryGetSceneSnapshot(static_cast<uint8_t>(index), sceneNumber, snapshot))
                    {
                        continue;
                    }

                    const WebUiColorDetails scenePrimaryColor = webUiColorDetailsFromChannels(bytesPerLed, fallbackTemperature,
                                                                                              snapshot.primaryR, snapshot.primaryG, snapshot.primaryB,
                                                                                              snapshot.primaryWW, snapshot.primaryCW);
                    const WebUiColorDetails sceneSecondaryColor = webUiColorDetailsFromChannels(bytesPerLed, fallbackTemperature,
                                                                                                snapshot.secondaryR, snapshot.secondaryG, snapshot.secondaryB,
                                                                                                snapshot.secondaryWW, snapshot.secondaryCW);

                    std::string previewShadow;
                    std::string previewHighlight;
                    webUiPreviewPalette(snapshot.primaryR, snapshot.primaryG, snapshot.primaryB, scenePrimaryColor.white, scenePrimaryColor.temperature, snapshot.brightness,
                                        previewShadow, previewHighlight);

                    json.beginObject();
                    json.field("id", static_cast<uint32_t>(sceneNumber));
                    json.field("name", std::string("Scene ") + std::to_string(sceneNumber));
                    json.field("effectId", webUiEffectId(snapshot.effectType));
                    webUiWriteColorJson(json, "primaryColor", scenePrimaryColor);
                    webUiWriteColorJson(json, "secondaryColor", sceneSecondaryColor);

                    json.key("hsv").beginObject();
                    json.field("h", static_cast<uint32_t>(scenePrimaryColor.hueDegrees));
                    json.field("s", static_cast<uint32_t>(scenePrimaryColor.saturationPercent));
                    json.field("v", static_cast<uint32_t>(scenePrimaryColor.valuePercent));
                    json.endObject();

                    json.field("white", static_cast<uint32_t>(scenePrimaryColor.white));
                    json.field("temperature", static_cast<uint32_t>(scenePrimaryColor.temperature));
                    json.field("brightness", static_cast<uint32_t>(snapshot.brightness));

                    json.key("preview").beginArray();
                    json.value(previewShadow);
                    json.value(previewHighlight);
                    json.endArray();
                    json.endObject();
                }
            }
            json.endArray();
            json.endObject();
        }
    }
    json.endArray();

    json.endObject();
    return json.str();
}

void NeoPixelBusModule::captureWebUiSegmentState(uint8_t channelIndex, Segment* segment)
{
    if (channelIndex >= _segments.size() || segment == nullptr)
    {
        return;
    }

    SegmentConfig& cfg = _segments[channelIndex];
    cfg.savedR = segment->getConfig().r();
    cfg.savedG = segment->getConfig().g();
    cfg.savedB = segment->getConfig().b();
    cfg.savedWW = segment->getConfig().ww();
    cfg.savedCW = segment->getConfig().cw();
    cfg.savedSecondaryR = segment->getConfig().r2();
    cfg.savedSecondaryG = segment->getConfig().g2();
    cfg.savedSecondaryB = segment->getConfig().b2();
    cfg.savedSecondaryWW = segment->getConfig().ww2();
    cfg.savedSecondaryCW = segment->getConfig().cw2();
    cfg.savedBrightness = segment->getBrightness();
    cfg.savedValid = true;

    Effect* currentEffect = segment->getEffect();
    cfg.savedEffectType = getTypeFromEffect(currentEffect);
    cfg.savedLastWasEffect = (currentEffect != nullptr);
    cfg.savedEffectValid = cfg.savedLastWasEffect;
}

void NeoPixelBusModule::powerWebUiSegmentOff(uint8_t channelIndex, Segment* segment, bool flushToHardware)
{
    if (channelIndex >= _segments.size() || segment == nullptr)
    {
        return;
    }

    captureWebUiSegmentState(channelIndex, segment);
    SegmentConfig& cfg = _segments[channelIndex];
    cfg.savedPower = 0;

    segment->stop();
    if ((_virtualStrip != nullptr) && _virtualStrip->getBytesPerLed() == 5)
    {
        segment->setPrimaryColor(0, 0, 0, 0, 0);
    }
    else
    {
        segment->setPrimaryColor(0, 0, 0, 0);
    }

    NeoPixelManager* manager = _neoPixel.getManager();
    if (manager != nullptr)
    {
        manager->syncAll();
        if (flushToHardware)
        {
            manager->showAll();
        }
    }
    else if (_virtualStrip != nullptr)
    {
        if (flushToHardware)
        {
            _virtualStrip->show();
        }
        else
        {
            _virtualStrip->syncToPhysical();
        }
    }
}

void NeoPixelBusModule::handleWebUiStatePatchRequest(WebRequest& req, WebResponse& res)
{
    if (req.body() == nullptr || req.bodyLength() == 0)
    {
        webUiSendError(res, 400, "Missing request body");
        return;
    }
    if (!_initialized || _segments.empty())
    {
        webUiSendError(res, 503, "NeoPixel module is not configured yet");
        return;
    }

    const Reader patchDoc(reinterpret_cast<const char*>(req.body()), req.bodyLength());
    if (!patchDoc.valid() || !patchDoc.isObject())
    {
        webUiSendError(res, 400, "Invalid JSON body");
        return;
    }

    const bool persist = patchDoc.get("persist").toBool(false);
    if (persist && (_webUiPersistSaveRequested || _webUiPersistSaveInProgress))
    {
        webUiSendError(res, 409, "Persist save already in progress");
        return;
    }

    uint16_t errorStatus = 400;
    std::string errorMessage;
    if (persist)
    {
        const Reader segments = patchDoc.get("segments");
        if (!segments.valid() || !segments.isArray() || !segments.select("/0").valid())
        {
            webUiSendError(res, 400, "Patch must contain at least one segment entry");
            return;
        }

        _webUiPersistPatchBody.assign(reinterpret_cast<const char*>(req.body()), req.bodyLength());
        _webUiPersistKnxWriteRequested = false;
        _webUiPersistModuleFlashRequested = false;
        _webUiPersistRestoreAutoUpdateRequested = false;
        _webUiPersistSaveReadyAt = millis() + kWebUiPersistSaveStartDelayMs;
        _webUiPersistSaveRequested = true;
        webUiSendJson(res, 202, webUiBuildPersistQueuedJson());
        return;
    }

    if (!applyWebUiStatePatch(patchDoc, false, errorStatus, errorMessage))
    {
        webUiSendError(res, errorStatus, errorMessage.c_str());
        return;
    }

    webUiSendJson(res, 200, buildWebUiStateJson(false, 0, true));
}

bool NeoPixelBusModule::applyWebUiStatePatch(const Reader& patchDoc, bool persist, uint16_t& errorStatus, std::string& errorMessage)
{
    const Reader segments = patchDoc.get("segments");
    if (!segments.valid() || !segments.isArray() || !segments.select("/0").valid())
    {
        errorStatus = 400;
        errorMessage = "Patch must contain at least one segment entry";
        return false;
    }

    NeoPixelManager* const manager = _neoPixel.getManager();
    const bool flushToHardware = !persist;
    const Reader sceneAction = patchDoc.get("sceneAction");
    const bool hasSceneAction = sceneAction.valid() && sceneAction.isObject() && !sceneAction.isNull();
    bool deferredKnxMemoryWrite = false;

    for (size_t segmentIndex = 0;; ++segmentIndex)
    {
        const Reader segmentPatch = segments.get(segmentIndex);
        if (!segmentPatch.valid())
        {
            break;
        }
        if (!segmentPatch.isObject())
        {
            errorStatus = 400;
            errorMessage = "Patch segments must be JSON objects";
            return false;
        }

        const uint8_t apiSegmentId = static_cast<uint8_t>(segmentPatch.get("id").toUInt(0));
        if (apiSegmentId < 1 || apiSegmentId > _segments.size())
        {
            errorStatus = 404;
            errorMessage = "Unknown segment id";
            return false;
        }

        const uint8_t channelIndex = static_cast<uint8_t>(apiSegmentId - 1);
        Segment* segment = _segments[channelIndex].segment;
        if (segment == nullptr)
        {
            errorStatus = 503;
            errorMessage = "Requested segment is not available";
            return false;
        }

        const bool actionTargetsSegment = hasSceneAction && sceneAction.get("segmentId").toUInt(0) == apiSegmentId;
        const Reader actionValue = actionTargetsSegment ? sceneAction.get("action") : Reader(nullptr, static_cast<size_t>(0));
        const std::string actionName = actionValue.valid() && actionValue.isString() ? actionValue.toString() : std::string();

        const uint8_t previousChannelIndex = getChannelIndex();
        setChannelIndex(channelIndex);

        if (actionName == "recall")
        {
            const uint8_t sceneId = static_cast<uint8_t>(sceneAction.get("sceneId").toUInt(0));
            if (_sceneManager == nullptr || !_sceneManager->recallScene(channelIndex, sceneId, segment))
            {
                setChannelIndex(previousChannelIndex);
                errorStatus = 400;
                errorMessage = "Unable to recall requested scene";
                return false;
            }

            const bool requestedPower = segmentPatch.get("power").toBool(true);
            if (requestedPower)
            {
                segment->resume();
                if (segment->getEffect() != nullptr)
                {
                    segment->getEffect()->reset();
                }
                captureWebUiSegmentState(channelIndex, segment);
                _segments[channelIndex].savedPower = 1;
            }
            else
            {
                powerWebUiSegmentOff(channelIndex, segment, flushToHardware);
            }
            _segments[channelIndex].savedSceneNumber = sceneId;

            setChannelIndex(previousChannelIndex);
            continue;
        }

        if (actionName == "delete")
        {
            const uint8_t sceneId = static_cast<uint8_t>(sceneAction.get("sceneId").toUInt(0));
            const bool flushSceneMemory = !persist;
            if (_sceneManager == nullptr || !_sceneManager->deleteScene(channelIndex, sceneId, flushSceneMemory))
            {
                setChannelIndex(previousChannelIndex);
                errorStatus = 400;
                errorMessage = "Unable to delete requested scene. Delete the highest-numbered scene first.";
                return false;
            }
            if (!flushSceneMemory)
            {
                deferredKnxMemoryWrite = true;
            }

            setChannelIndex(previousChannelIndex);
            continue;
        }

        const Reader effectPatch = segmentPatch.get("effect");
        const Reader requestedEffectIdValue = effectPatch.get("id");
        const std::string requestedEffectId = requestedEffectIdValue.valid() && requestedEffectIdValue.isString()
                                                  ? requestedEffectIdValue.toString()
                                                  : std::string();
        const uint8_t requestedEffectType = webUiResolveEffectType(requestedEffectId, segment->getConfig().effectType);
        const bool effectChanged = segment->getConfig().effectType != requestedEffectType || segment->getEffect() == nullptr;
        if (effectChanged)
        {
            applyEffectToSegment(segment, requestedEffectType);
        }

        const uint8_t brightness = static_cast<uint8_t>(segmentPatch.get("brightness").toUInt(segment->getBrightness()));

        const uint8_t bytesPerLed = _virtualStrip ? _virtualStrip->getBytesPerLed() : 3;
        const WebUiColorDetails currentPrimaryColor = webUiColorDetailsFromChannels(bytesPerLed, kWebUiDefaultTemperatureKelvin,
                                                                                    segment->getConfig().r(), segment->getConfig().g(), segment->getConfig().b(),
                                                                                    segment->getConfig().ww(), segment->getConfig().cw());
        const WebUiColorDetails currentSecondaryColor = webUiColorDetailsFromChannels(bytesPerLed, kWebUiDefaultTemperatureKelvin,
                                                                                      segment->getConfig().r2(), segment->getConfig().g2(), segment->getConfig().b2(),
                                                                                      segment->getConfig().ww2(), segment->getConfig().cw2());

        const Reader preferredPrimaryColorPatch = segmentPatch.get("primaryColor");
        const Reader legacyColorPatch = segmentPatch.get("color");
        const Reader secondaryColorPatch = segmentPatch.get("secondaryColor");
        const Reader& primaryColorPatch = (preferredPrimaryColorPatch.valid() && preferredPrimaryColorPatch.isObject())
                                              ? preferredPrimaryColorPatch
                                              : legacyColorPatch;

        const WebUiAppliedColor primaryColor = webUiAppliedColorFromPatch(primaryColorPatch, currentPrimaryColor, bytesPerLed);
        const WebUiAppliedColor secondaryColor = webUiAppliedColorFromPatch(secondaryColorPatch, currentSecondaryColor, bytesPerLed);

        SegmentConfig& cfg = _segments[channelIndex];
        cfg.currentH = webUiHueDegreesToByte(primaryColor.details.hueDegrees);
        cfg.currentS = webUiPercentToByte(primaryColor.details.saturationPercent);
        cfg.currentV = webUiPercentToByte(primaryColor.details.valuePercent);

        if (bytesPerLed == 5)
        {
            segment->setPrimaryColor(primaryColor.red, primaryColor.green, primaryColor.blue, primaryColor.warmWhite, primaryColor.coolWhite);
            segment->setSecondaryColor(secondaryColor.red, secondaryColor.green, secondaryColor.blue, secondaryColor.warmWhite, secondaryColor.coolWhite);
        }
        else if (bytesPerLed == 4)
        {
            segment->setPrimaryColor(primaryColor.red, primaryColor.green, primaryColor.blue, primaryColor.warmWhite);
            segment->setSecondaryColor(secondaryColor.red, secondaryColor.green, secondaryColor.blue, secondaryColor.warmWhite);
        }
        else
        {
            segment->setPrimaryColor(primaryColor.red, primaryColor.green, primaryColor.blue, 0);
            segment->setSecondaryColor(secondaryColor.red, secondaryColor.green, secondaryColor.blue, 0);
        }

        cfg.pendingSolidR = primaryColor.red;
        cfg.pendingSolidG = primaryColor.green;
        cfg.pendingSolidB = primaryColor.blue;
        cfg.pendingSolidWW = primaryColor.warmWhite;
        cfg.pendingSolidCW = primaryColor.coolWhite;
        cfg.savedBrightness = brightness;

        uint8_t effectiveBrightness = brightness;
        if (_globalBrightness < 255)
        {
            effectiveBrightness = static_cast<uint8_t>((brightness * _globalBrightness) / 255);
        }
        segment->setBrightness(effectiveBrightness);

        Effect* effect = segment->getEffect();
        if (effect == nullptr)
        {
            effect = getEffectFromType(requestedEffectType);
        }

        const Reader parameterValues = effectPatch.get("parameters");
        if (effect != nullptr && parameterValues.valid() && parameterValues.isObject())
        {
            for (uint8_t parameterIndex = 0; parameterIndex < effect->getParameterCount(); ++parameterIndex)
            {
                const std::string parameterKey = webUiSlugify(effect->getParameterName(parameterIndex));
                const Reader parameterValue = parameterValues.get(parameterKey.c_str());
                if (!parameterValue.valid() || parameterValue.isNull())
                {
                    continue;
                }

                const ParameterType parameterType = effect->getParameterType(parameterIndex);
                uint32_t normalizedValue = effect->getParameterDefault(parameterIndex);

                if (parameterType == ParameterType::PARAM_BOOL)
                {
                    normalizedValue = parameterValue.toBool(false) ? 1u : 0u;
                }
                else if (parameterType == ParameterType::PARAM_ENUM && effect->getEnumValueCount(parameterIndex) > 0)
                {
                    if (parameterValue.isString())
                    {
                        const std::string selected = parameterValue.toString();
                        for (uint8_t enumIndex = 0; enumIndex < effect->getEnumValueCount(parameterIndex); ++enumIndex)
                        {
                            const char* optionName = effect->getEnumValueName(parameterIndex, enumIndex);
                            if (optionName != nullptr && selected == optionName)
                            {
                                normalizedValue = enumIndex;
                                break;
                            }
                        }
                    }
                    else
                    {
                        normalizedValue = parameterValue.toUInt(normalizedValue);
                    }
                }
                else
                {
                    normalizedValue = parameterValue.toUInt(normalizedValue);
                    normalizedValue = clampValue<uint32_t>(normalizedValue, effect->getParameterMin(parameterIndex), effect->getParameterMax(parameterIndex));
                }

                effect->setParameter(segment, parameterIndex, normalizedValue);
            }
        }

        if (effectChanged && segment->getEffect() != nullptr)
        {
            segment->getEffect()->reset();
        }

        if (actionName == "add")
        {
            uint8_t newSceneId = 0;
            const bool flushSceneMemory = !persist;
            if (_sceneManager == nullptr || !_sceneManager->addScene(channelIndex, segment, newSceneId, flushSceneMemory))
            {
                setChannelIndex(previousChannelIndex);
                errorStatus = 400;
                errorMessage = "Unable to add requested scene";
                return false;
            }
            if (!flushSceneMemory)
            {
                deferredKnxMemoryWrite = true;
            }
        }
        else if (actionName == "store")
        {
            const uint8_t sceneId = static_cast<uint8_t>(sceneAction.get("sceneId").toUInt(0));
            const bool flushSceneMemory = !persist;
            if (_sceneManager == nullptr || !_sceneManager->storeScene(channelIndex, sceneId, segment, flushSceneMemory))
            {
                setChannelIndex(previousChannelIndex);
                errorStatus = 400;
                errorMessage = "Unable to store requested scene";
                return false;
            }
            if (!flushSceneMemory)
            {
                deferredKnxMemoryWrite = true;
            }
        }

        cfg.savedSceneNumber = 0;
        const bool requestedPower = segmentPatch.get("power").toBool(true);
        if (requestedPower)
        {
            segment->resume();
            captureWebUiSegmentState(channelIndex, segment);
            cfg.savedPower = 1;
        }
        else
        {
            powerWebUiSegmentOff(channelIndex, segment, flushToHardware);
        }

        if (manager != nullptr)
        {
            manager->syncAll();
            if (flushToHardware)
            {
                manager->showAll();
            }
        }
        else if (_virtualStrip != nullptr)
        {
            if (flushToHardware)
            {
                _virtualStrip->show();
            }
            else
            {
                _virtualStrip->syncToPhysical();
            }
        }

        setChannelIndex(previousChannelIndex);
    }

    if (persist)
    {
        _webUiPersistKnxWriteRequested = deferredKnxMemoryWrite;
        return true;
    }

    return true;
}

void NeoPixelBusModule::processPendingWebUiPersistSave()
{
    if (_webUiPersistPatchBody.empty())
    {
        if (_webUiPersistModuleFlashRequested)
        {
            _webUiPersistModuleFlashRequested = false;
            openknx.flash.save();
            if (_webUiPersistRestoreAutoUpdateRequested)
            {
                _webUiPersistRestoreAutoUpdateRequested = false;
                _neoPixel.setAutoUpdate(true);
            }
            return;
        }

        logWarningP("Queued WebUI persist save skipped: no request body");
        _webUiPersistKnxWriteRequested = false;
        if (_webUiPersistRestoreAutoUpdateRequested)
        {
            _webUiPersistRestoreAutoUpdateRequested = false;
            _neoPixel.setAutoUpdate(true);
        }
        return;
    }

    NeoPixelManager* const manager = _neoPixel.getManager();
    WebUiPersistSaveGuard persistSaveGuard(_neoPixel, manager, _virtualStrip, true);

    const Reader patchDoc(_webUiPersistPatchBody.c_str(), _webUiPersistPatchBody.length());
    uint16_t errorStatus = 400;
    std::string errorMessage;
    _webUiPersistKnxWriteRequested = false;

    if (!patchDoc.valid() || !patchDoc.isObject())
    {
        logErrorP("Queued WebUI persist save failed: invalid JSON body");
        if (_sceneManager != nullptr)
        {
            _sceneManager->discardPendingSceneMemory();
        }
        _webUiPersistPatchBody.clear();
        return;
    }

    if (!applyWebUiStatePatch(patchDoc, true, errorStatus, errorMessage))
    {
        logErrorP("Queued WebUI persist save failed: status=%d message=%s", errorStatus, errorMessage.c_str());
        if (_sceneManager != nullptr)
        {
            _sceneManager->discardPendingSceneMemory();
        }
        _webUiPersistPatchBody.clear();
        _webUiPersistKnxWriteRequested = false;
        return;
    }

    persistSaveGuard.waitForOutputIdle();
    if (_webUiPersistKnxWriteRequested)
    {
        _webUiPersistKnxWriteRequested = false;
        if (_sceneManager == nullptr || !_sceneManager->flushPendingSceneMemory())
        {
            logErrorP("Queued WebUI persist save failed: unable to flush scene parameter memory");
            if (_sceneManager != nullptr)
            {
                _sceneManager->discardPendingSceneMemory();
            }
            _webUiPersistPatchBody.clear();
            return;
        }
        _webUiPersistPatchBody.clear();
        if (persistSaveGuard.shouldRestoreAutoUpdate())
        {
            _webUiPersistRestoreAutoUpdateRequested = true;
            persistSaveGuard.releaseAutoUpdateRestore();
        }
        _webUiPersistModuleFlashRequested = true;
        _webUiPersistSaveRequested = true;
        _webUiPersistSaveReadyAt = millis() + kWebUiPersistSaveStartDelayMs;
        return;
    }
    openknx.flash.save();
    _webUiPersistPatchBody.clear();
}

#endif