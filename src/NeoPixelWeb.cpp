/**
 * @file NeoPixelWeb.cpp
 * @brief NeoPixel status page /neo: snapshot builder, LED image copy and control queue.
 *
 * Endpoints: GET /neo (page), GET /neo/state (JSON, ~1 s), GET /neo/em (EM definitions),
 * GET /neo/pixels (binary LED image), POST /neo/control (one command, executed in the loop task).
 */
#include "NeoPixelWeb.h"

#ifdef NEOPIXEL_WEB

    #include "NeoPixelModule.h" // first: knxprod.h defines the PT_ types the generated headers use
    #include "EffectTypeMapping.h"
    #include "HardwareMappingData.h"
    #include "NeoPixelTimingSelection.h"
    #include "OpenKNX/Network/Module.h"
    #include "SceneManager.h"
    #include "buildtime.h"
    #include "test/PerformanceTracker.h"
    #include "webassets.h"
    #include <Arduino.h>
    #include <cmath>
    #include <cstdarg>
    #include <cstdio>
    #include <cstring>
    #include <freertos/FreeRTOS.h>
    #include <freertos/queue.h>
    #include <freertos/semphr.h>
    #if __has_include(<esp_random.h>)
        #include <esp_random.h>
    #else
        #include <esp_system.h>
    #endif
    #if __has_include("rmt/rmt_neopixel_serial.h")
        #include "rmt/rmt_neopixel_serial.h"
        #define NEOPIXEL_WEB_RMT
    #endif

using OpenKNX::Network::WEB_GET;
using OpenKNX::Network::WEB_POST;
using OpenKNX::Network::WebRequest;
using OpenKNX::Network::WebResponse;
using OpenKNX::Network::Webserver;

extern PerformanceTracker g_perfTracker;

NeoPixelWeb neoPixelWeb;

static_assert(NEOPIXEL_MAX_PHYSICAL_STRIPS <= 16, "NeoPixelWeb strip table too small");

namespace
{
    constexpr uint32_t kStateIntervalMs = 1000;  ///< snapshot age the page polls at
    constexpr uint32_t kStateActiveMs = 10000;   ///< build only while the page polled recently
    constexpr uint32_t kPixIntervalMs = 100;     ///< LED image copy rate while polled
    constexpr uint32_t kPixActiveMs = 3000;
    constexpr uint32_t kReleaseIdleMs = 30000;   ///< free the buffers after this idle time
    constexpr uint32_t kEmCheckMs = 2000;
    constexpr size_t kPixHeader = 8;
    constexpr size_t kPixHeapReserve = 48 * 1024; ///< heap that must stay free after the copy buffer
    constexpr uint8_t kCmdQueueLen = 8;
    constexpr uint8_t kCmdPerLoop = 2;
    constexpr size_t kMaxBody = 160;
    constexpr uint8_t kStepSwap = 0xFF; ///< buffer complete, waiting for the lock to swap it in

    void addf(std::string& s, const char* fmt, ...)
    {
        char buf[256];
        va_list ap, ap2;
        va_start(ap, fmt);
        va_copy(ap2, ap);
        const int n = vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        if (n > 0 && (size_t)n < sizeof(buf))
            s.append(buf, (size_t)n);
        else if (n > 0)
        {
            // longer than the stack buffer: format straight into the string, never truncated
            const size_t at = s.size();
            s.resize(at + (size_t)n + 1);
            vsnprintf(&s[at], (size_t)n + 1, fmt, ap2);
            s.resize(at + (size_t)n);
        }
        va_end(ap2);
    }

    /// JSON string from a byte field (cue names): stops at NUL or maxLen, escapes quote, backslash, control bytes
    void addJsonStr(std::string& s, const char* text, size_t maxLen)
    {
        s += '"';
        for (size_t i = 0; i < maxLen && text[i]; i++)
        {
            const unsigned char c = (unsigned char)text[i];
            if (c == '"' || c == '\\')
            {
                s += '\\';
                s += (char)c;
            }
            else if (c < 0x20)
                addf(s, "\\u%04x", c);
            else
                s += (char)c;
        }
        s += '"';
    }

    bool timeElapsed(uint32_t now, uint32_t since, uint32_t ms) { return (uint32_t)(now - since) >= ms; }

    /// request stamp (0 = never) within the last ms
    bool recent(const std::atomic<uint32_t>& stamp, uint32_t now, uint32_t ms)
    {
        const uint32_t t = stamp.load();
        return t != 0 && !timeElapsed(now, t, ms);
    }

    inline SemaphoreHandle_t lockOf(void* p) { return (SemaphoreHandle_t)p; }
} // namespace

// ============================================================================
// Setup / loop (loop task)
// ============================================================================

void NeoPixelWeb::setup(bool configured, uint32_t startMs)
{
    if (_routesAdded) return;
    _configured = configured;
    _startMs = startMs;
    _token[0] = '\0'; // created with the first snapshot, once RF runs and esp_random() is a hardware RNG
    _lock = (void*)xSemaphoreCreateMutex();
    _queue = (void*)xQueueCreate(kCmdQueueLen, sizeof(Command));
    if (!_lock || !_queue) return; // no page without them; retried at the next setup()
    _routesAdded = true;

    Webserver& ws = openknxNetwork.webserver;
    ws.addMenuItem("NeoPixel", "/neo", -100);
    // Referenced only by /neo: addStylesheet()/addJavaScript() would put them on every page.
    ws.addRoute(WEB_GET, "/assets/neopixel.css", Webserver::Asset(WebAssets::neopixel_css_mime, WebAssets::neopixel_css_gz, sizeof(WebAssets::neopixel_css_gz)));
    ws.addRoute(WEB_GET, "/assets/neopixel.js", Webserver::Asset(WebAssets::neopixel_js_mime, WebAssets::neopixel_js_gz, sizeof(WebAssets::neopixel_js_gz)));
    ws.addRoute(WEB_GET, "/neo", [this](WebRequest& q, WebResponse& r) { handlePage(q, r); });
    ws.addRoute(WEB_GET, "/neo/state", [this](WebRequest& q, WebResponse& r) { handleState(q, r); });
    ws.addRoute(WEB_GET, "/neo/em", [this](WebRequest& q, WebResponse& r) { handleEm(q, r); });
    ws.addRoute(WEB_GET, "/neo/pixels", [this](WebRequest& q, WebResponse& r) { handlePixels(q, r); });
    ws.addRoute(WEB_POST, "/neo/control", [this](WebRequest& q, WebResponse& r) { handleControl(q, r); });
}

void NeoPixelWeb::loop()
{
    if (!_routesAdded || !_configured) return;
    const uint32_t now = millis();

    executeCommands();

    if (_stateStep != 0)
        buildStateStep();
    else if (recent(_stateReqMs, now, kStateActiveMs) && (_stateFront.empty() || _stateDirty || timeElapsed(now, _stateBuiltMs, kStateIntervalMs)))
    {
        _stateDirty = false; // a command during this build sets it again: one more snapshot right after
        _stateStep = 1;
        buildStateStep();
    }

    if (_emStep != 0)
        buildEmStep();
    else if ((recent(_emReqMs, now, kStateActiveMs) || recent(_stateReqMs, now, kStateActiveMs)) && (!_emValid || timeElapsed(now, _emCheckMs, kEmCheckMs)))
    {
        _emCheckMs = now;
        const uint32_t sum = emChecksum();
        if (!_emValid || sum != _emSum)
        {
            _emSumBuilding = sum;
            _emStep = 1;
            buildEmStep();
        }
    }

    if (recent(_pixReqMs, now, kPixActiveMs) && timeElapsed(now, _pixCopyMs, kPixIntervalMs))
    {
        _pixCopyMs = now;
        copyPixels();
    }

    releaseIdle(now);
}

void NeoPixelWeb::releaseIdle(uint32_t now)
{
    // Frees what an idle page left behind; try-lock so the loop never waits on a running handler.
    const bool stateIdle = !recent(_stateReqMs, now, kReleaseIdleMs) && _stateStep == 0 && (!_stateFront.empty() || _stateBack.capacity() > 32);
    const bool emIdle = !recent(_emReqMs, now, kReleaseIdleMs) && !recent(_stateReqMs, now, kReleaseIdleMs) && _emStep == 0 && (_emValid || _emBack.capacity() > 32);
    const bool pixIdle = !recent(_pixReqMs, now, kReleaseIdleMs) && _pix != nullptr;
    if (!stateIdle && !emIdle && !pixIdle) return;
    if (xSemaphoreTake(lockOf(_lock), 0) != pdTRUE) return;
    if (stateIdle)
    {
        std::string().swap(_stateFront);
        std::string().swap(_stateBack);
    }
    if (emIdle)
    {
        std::string().swap(_emFront);
        std::string().swap(_emBack);
        _emValid = false;
    }
    uint8_t* pix = nullptr;
    if (pixIdle)
    {
        pix = _pix;
        _pix = nullptr;
        _pixCap = _pixLen = 0;
    }
    xSemaphoreGive(lockOf(_lock));
    free(pix);
}

// ============================================================================
// Control commands
// ============================================================================

void NeoPixelWeb::executeCommands()
{
    Command cmd;
    for (uint8_t i = 0; i < kCmdPerLoop; i++)
    {
        if (xQueueReceive((QueueHandle_t)_queue, &cmd, 0) != pdTRUE) return;
        _cmdRingRes[_cmdRingPos] = executeCommand(cmd);
        _cmdRingSeq[_cmdRingPos] = cmd.seq;
        _cmdRingPos = (uint8_t)((_cmdRingPos + 1) % kCmdRing);
        _stateDirty = true; // result in the next snapshot, not a second later
    }
}

uint8_t NeoPixelWeb::executeCommand(const Command& cmd)
{
    NeoPixelBusModule& m = openknxNeoPixelModule;
    if (cmd.segment >= m._segments.size() || cmd.segment >= NEO_ChannelCount || !m._segments[cmd.segment].segment) return kResInvalid;
    NeoPixelBusModule::SegmentConfig& cfg = m._segments[cmd.segment];

    uint8_t index = 0;
    KNXValue value((uint8_t)0);
    Dpt dpt = DPT_Switch;
    const int32_t v = cmd.value;
    switch (cmd.function)
    {
        case kFnPower:
            index = NEO_KoSegmentPower;
            value = KNXValue(v != 0);
            break;
        case kFnBri:
            if (v < 0 || v > 100) return kResInvalid;
            index = NEO_KoSegmentBrightness;
            value = KNXValue((uint8_t)v);
            dpt = DPT_Scaling;
            break;
        case kFnDim: // DPT 3.007: up/down with step code 1 (full range), 0 = stop
            index = NEO_KoBriRel;
            value = KNXValue((uint8_t)(v > 0 ? 0x09 : v < 0 ? 0x01 : 0x00));
            dpt = Dpt(5, 10);
            break;
        case kFnRgb:
            index = NEO_KoRGB;
            value = KNXValue((uint32_t)(cmd.color >> 8));
            dpt = DPT_Colour_RGB;
            break;
        case kFnRgbw:
            index = NEO_KoRGBW;
            value = KNXValue(cmd.color);
            dpt = DPT_Colour_RGBW;
            break;
        case kFnWw:
        case kFnCw:
            if (v < 0 || v > 255) return kResInvalid;
            index = cmd.function == kFnWw ? NEO_KoWW : NEO_KoCW;
            value = KNXValue((uint8_t)v);
            dpt = DPT_Value_1_Ucount;
            break;
        case kFnFx:
            if (v < 0 || v > 254) return kResInvalid;
            index = NEO_KoFx;
            value = KNXValue((uint8_t)v);
            dpt = DPT_Value_1_Ucount;
            break;
        case kFnScene: // DPT 18.001 recall: bits 0-5 = scene - 1
            if (v < 1 || v > 64) return kResInvalid;
            if (!m._sceneManager || v > m._sceneManager->getSceneCount(cmd.segment)) return kResSceneMissing;
            index = NEO_KoScene;
            value = KNXValue((uint8_t)(v - 1));
            dpt = Dpt(5, 10);
            break;
    #ifdef NEO_KoEmStart
        case kFnEmStart:
        {
            if (v < 1 || v > EM_COUNT || v > (int32_t)ParamNEO_NEOEMEnabled || !m._emData) return kResEmRefused;
            if (!m._emData[v - 1].header.isEnabled()) return kResEmRefused;
            index = NEO_KoEmStart;
            value = KNXValue((uint8_t)v);
            dpt = DPT_Value_1_Ucount;
            break;
        }
        case kFnEmStop:
            if (!cfg.emController.isRunning()) return kResNoEm;
            index = NEO_KoEmStop;
            value = KNXValue(true);
            break;
        case kFnCue: // DPT 3.007 cue step, step code 1
            if (!cfg.emController.isRunning()) return kResNoEm;
            index = NEO_KoCueRel;
            value = KNXValue((uint8_t)(v > 0 ? 0x09 : 0x01));
            dpt = Dpt(5, 10);
            break;
    #endif
        case kFnLock:
            index = NEO_KoLock;
            value = KNXValue(v != 0);
            break;
        default:
            return kResInvalid;
    }

    // processInputKo() ignores a locked segment's controlling KOs
    if (cfg.locked && cmd.function != kFnLock) return kResLocked;

    // Like a received telegram, without one on the bus: value into the input KO, then the
    // module dispatch. ko.value() would also set WriteRequest and process it a second time in knx.loop().
    const uint16_t koNr = (uint16_t)(NEO_KoBlockOffset + cmd.segment * NEO_KoBlockSize + index);
    GroupObject& ko = knx.getGroupObject(koNr);
    // During the startup delay and while its queue drains processInputKo() queues the KO; deferKo() refuses
    // DPT 3.007 dimming of the delay and KOs without W flag. The result is read from the queue itself.
    const bool gate = !m._startupRestoreDone || m._deferredKoHead < m._deferredKoCount;
    const uint8_t before = m._deferredKoCount;
    ko.valueNoSend(value, dpt);
    openknx.common.processInputKo(ko);
    if (!gate) return kResOk;
    // queued = newest entry is this KO and the queue changed (a full queue drops its oldest entry instead)
    const bool queued = m._deferredKoCount > m._deferredKoHead && m._deferredKo[m._deferredKoCount - 1].number == koNr &&
                        (m._deferredKoCount != before || before >= NeoPixelBusModule::kDeferredKoMax);
    if (queued) return kResDeferred;
    return m._startupRestoreDone ? kResOk : kResDropped; // draining: a refused KO was processed directly
}

// ============================================================================
// State snapshot (loop task, one part per pass)
// ============================================================================

void NeoPixelWeb::buildStateStep()
{
    NeoPixelBusModule& m = openknxNeoPixelModule;
    const size_t segCount = m._segments.size();
    // steps: 1 = head + strips, 2 .. 1 + segCount = one segment each, then tail, then swap
    if (_stateStep == kStepSwap)
    {
        // Swap only when the handler does not hold the lock; retried next pass.
        if (xSemaphoreTake(lockOf(_lock), 0) != pdTRUE) return;
        _stateFront.swap(_stateBack);
        xSemaphoreGive(lockOf(_lock));
        _stateStep = 0;
        _stateBuiltMs = millis();
        return;
    }
    if (_stateStep == 1)
    {
        if (!_token[0]) snprintf(_token, sizeof(_token), "%08lx%08lx", (unsigned long)esp_random(), (unsigned long)esp_random());
        mapStrips();
        _stateBack.clear();
        _stateBack.reserve(_stateFront.capacity() > 2048 ? _stateFront.capacity() : 2048);
        appendHead(_stateBack);
        appendStrips(_stateBack);
        _stateBack += ",\"segs\":[";
        _stateStep = 2;
        return;
    }
    const size_t seg = (size_t)_stateStep - 2;
    if (seg < segCount && seg < NEO_ChannelCount)
    {
        if (seg) _stateBack += ',';
        appendSegment(_stateBack, seg);
        _stateStep++;
        return;
    }
    _stateBack += ']';
    appendTail(_stateBack);
    _stateBack += '}';
    _stateStep = kStepSwap;
    buildStateStep();
}

void NeoPixelWeb::mapStrips()
{
    // Each created strip carries the ETS channel it was built from; a channel without strip
    // (0 LEDs, refused pin, HW mismatch) has no entry. Unknown entries stay unmapped (0xFF).
    NeoPixelBusModule& m = openknxNeoPixelModule;
    memset(_stripChannel, 0xFF, sizeof(_stripChannel));
    memset(_stripTiming, 0, sizeof(_stripTiming));
    const size_t count = std::min(m._physicalStrips.size(), m._physicalStripChannel.size());
    for (size_t k = 0; k < count && k < kMaxStrips; k++)
    {
        const uint8_t _channelIndex = m._physicalStripChannel[k]; // read by the ParamNEOSTRIP_* macros; the module's index stays untouched
        if (!m._physicalStrips[k] || _channelIndex >= NEOPIXEL_MAX_PHYSICAL_STRIPS) continue;
        _stripChannel[k] = _channelIndex;
        _stripTiming[k] = NeoPixelTimingSelection::sanitize((uint8_t)ParamNEOSTRIP_NEOTiming & 0x0F);
    }
}

void NeoPixelWeb::appendHead(std::string& j)
{
    NeoPixelBusModule& m = openknxNeoPixelModule;
    NeoPixelManager* mgr = m._neoPixel.getManager();
    const uint32_t now = millis();
    const uint8_t bpl = m._virtualStrip ? m._virtualStrip->getBytesPerLed() : 3;
    const uint32_t interval = m._neoPixel.getUpdateInterval();

    addf(j, "{\"v\":1,\"seq\":%lu,\"up\":%lu,\"leds\":%u,\"bpl\":%u", (unsigned long)++_stateSeq, (unsigned long)uptime(), (unsigned)m._totalLeds, (unsigned)bpl);
    addf(j, ",\"bar\":{\"pw\":%u,\"bri\":%u,\"fps\":%u,\"fpsT\":%lu,\"hcl\":%u,\"err\":%u,\"warn\":%u,\"warn2\":%u}",
         m._globalPowerOn ? 1 : 0, (unsigned)((m._globalBrightness * 100 + 127) / 255),
         m._renderPaused ? 0u : (unsigned)(m._neoPixel.getActualFps() + 0.5f), (unsigned long)(interval ? 1000 / interval : 0),
         (unsigned)m._hclGlobalStatusKelvin, (unsigned)m._activeErrorCode, (unsigned)m._activeWarnCode, (unsigned)m._warnCodeBeforeAbl);

    // Power: ETS 0 = Deaktiviert, 1 = Fester Wert, 2 = Pro LED
    uint32_t gMa = 0, gLim = 0;
    uint8_t gLoad = 0;
    PowerManager* pm = mgr ? mgr->getPowerManager() : nullptr;
    if (mgr) mgr->getGlobalPowerStats(gMa, gLim, gLoad);
    addf(j, ",\"pwr\":{\"mode\":%u,\"lim\":%lu,\"thr\":%u,\"abl\":%u,\"perLed\":%u,\"ma\":%lu,\"load\":%u}",
         (unsigned)ParamNEO_NEOpowerLimitCombined, (unsigned long)gLim, pm ? (unsigned)pm->getThresholdPercent() : 0u,
         pm ? (unsigned)pm->getMaxBrightnessPercent() : 0u, (unsigned)ParamNEO_NEOcurrentPerLED, (unsigned long)gMa, (unsigned)gLoad);

    // Hardware and runtime
    if (_tempMs == 0 || timeElapsed(now, _tempMs, 10000))
    {
        _tempMs = now | 1;
        _temp = openknx.hardware.cpuTemperature();
    }
    #ifdef DEVICE_HW_ID
    const uint16_t hwId = DEVICE_HW_ID;
    #else
    const uint16_t hwId = 0;
    #endif
    const char* board = HardwareMapping::getHardwareName(HardwareMapping::mapDeviceHwIdToIndex(hwId));
    uint16_t etsId = 0;
    const char* etsName = "";
    #ifdef ParamNEO_NeoPixelHardwareSelect
    if (m._hwConfigMismatch)
    {
        etsId = (uint16_t)ParamNEO_NeoPixelHardwareSelect;
        etsName = etsId == 255 ? "" : HardwareMapping::getHardwareName(HardwareMapping::mapDeviceHwIdToIndex(etsId));
    }
    #endif
    uint8_t build = 0; // 0 = KNX TP, 1 = TP + WLAN, 2 = IP over WLAN, 3 = IP over LAN
    #if defined(KNX_IP_LAN)
    build = 3;
    #elif defined(KNX_IP_WIFI)
        #if defined(MASK_VERSION) && MASK_VERSION == 0x07B0
    build = 1;
        #else
    build = 2;
        #endif
    #endif
    addf(j, ",\"hw\":{\"chip\":\"%s\",\"mhz\":%lu,\"cores\":%u,\"hwid\":%u,\"board\":", ESP.getChipModel(), (unsigned long)getCpuFrequencyMhz(), (unsigned)ESP.getChipCores(), (unsigned)hwId);
    addJsonStr(j, board ? board : "", 48);
    addf(j, ",\"ets\":%u,\"etsName\":", (unsigned)etsId);
    addJsonStr(j, etsName ? etsName : "", 48);
    addf(j, ",\"ls\":%u,\"build\":%u", (unsigned)kHwDefaultLevelShifter, (unsigned)build);
    const uint32_t ftAvg = g_perfTracker.getAverageTime();
    addf(j, ",\"ft\":%lu,\"ftMin\":%lu,\"ftMax\":%lu,\"temp\":%d", (unsigned long)ftAvg,
         (unsigned long)(g_perfTracker.minUpdateTime == UINT32_MAX ? 0 : g_perfTracker.minUpdateTime), (unsigned long)g_perfTracker.maxUpdateTime,
         std::isnan(_temp) ? 0 : (int)(_temp * 10.0f));
    addf(j, ",\"heap\":%lu,\"heapMin\":%lu,\"heapTot\":%lu", (unsigned long)freeMemory(), (unsigned long)openknx.common.freeMemoryMin(), (unsigned long)ESP.getHeapSize());
    #ifdef NEOPIXEL_WEB_RMT
    addf(j, ",\"rmtFree\":%lu,\"rmtTot\":%lu", (unsigned long)RMT_NeoPixel_Serial::getAvailableRmtChannels(), (unsigned long)RMT_NeoPixel_Serial::getTotalRmtChannels());
    #endif
    uint8_t valid = 0;
    for (const auto& s : m._segments)
        if (s.savedValid) valid++;
    // startLeft is approximate: Common starts the delay a little before _startMs
    const uint32_t delayMs = ParamBASE_StartupDelayTimeMS, since = now - _startMs;
    addf(j, ",\"start\":%u,\"startLeft\":%lu,\"deferred\":%u,\"savedValid\":%u}",
         m._startupRestoreDone ? 1 : 0, (unsigned long)(openknx.afterStartupDelay() || since >= delayMs ? 0 : delayMs - since),
         (unsigned)(m._deferredKoCount > m._deferredKoHead ? m._deferredKoCount - m._deferredKoHead : 0), (unsigned)valid);
    addf(j, ",\"emCount\":%u,\"emSum\":%lu,\"tok\":\"%s\"", (unsigned)ParamNEO_NEOEMEnabled, (unsigned long)_emSum, _token);
}

void NeoPixelWeb::appendStrips(std::string& j)
{
    NeoPixelBusModule& m = openknxNeoPixelModule;
    NeoPixelManager* mgr = m._neoPixel.getManager();
    j += ",\"strips\":[";
    for (size_t i = 0; i < m._physicalStrips.size() && i < kMaxStrips; i++)
    {
        PhysicalStrip* p = m._physicalStrips[i];
        if (i) j += ',';
        if (!p)
        {
            j += "null";
            continue;
        }
        uint16_t vs = 0xFFFF;
        for (const auto& v : m._virtualStripConfiguration)
            if (v.physicalStripIndex == i) vs = v.virtualStartPosition;
        uint32_t ma = 0, lim = 0;
        uint8_t load = 0;
        const bool stats = mgr && mgr->getStripPowerStats(p, ma, lim, load);
        const PhysicalStripConfig* c = p->getConfig();
        const char* proto = NeoPixelBusModule::getProtocolName(p->getProtocol());
        const char* order = NeoPixelBusModule::getColorOrderName(p->getColorOrder());
        j += "{\"p\":";
        addJsonStr(j, proto ? proto : "", 40);
        j += ",\"order\":";
        addJsonStr(j, order ? order : "", 16);
        addf(j, ",\"n\":%u,\"pin\":%lu,\"clk\":%ld,\"spi\":%u,\"ch\":%d,\"vs\":%d,\"tim\":%u", (unsigned)p->getLedCount(),
             (unsigned long)p->getDataPin(), p->isSpiStrip() ? (long)p->getClockPin() : -1L, p->isSpiStrip() ? 1 : 0,
             _stripChannel[i] == 0xFF ? -1 : (int)_stripChannel[i], vs == 0xFFFF ? -1 : (int)vs, (unsigned)NeoPixelTimingSelection::bitrateKhz(_stripTiming[i]));
        addf(j, ",\"tov\":%u,\"pm\":%u,\"lim\":%u,\"perLed\":%u,\"thr\":%u,\"abl\":%u,\"ma\":%ld,\"sl\":%lu,\"load\":%u}",
             NeoPixelTimingSelection::isProtocolDefault(_stripTiming[i]) ? 0 : 1, c ? (unsigned)c->getPowerLimitMode() : 1u,
             c ? (unsigned)c->getMaxCurrentMa() : 0u, c ? (unsigned)c->getCurrentPerLedMa() : 0u, c ? (unsigned)c->getPowerLimitThreshold() : 0u,
             c ? (unsigned)c->getAutoBrightnessLimit() : 0u, stats ? (long)ma : -1L, (unsigned long)lim, (unsigned)load);
    }
    j += ']';
}

void NeoPixelWeb::appendSegment(std::string& j, size_t i)
{
    NeoPixelBusModule& m = openknxNeoPixelModule;
    const NeoPixelBusModule::SegmentConfig& cfg = m._segments[i];
    const Segment* seg = cfg.segment;
    const uint32_t now = millis();
    const uint32_t c1 = seg ? seg->getPrimaryColor() : 0;
    const uint32_t c2 = seg ? seg->getSecondaryColor() : 0;
    const uint8_t cw = seg ? seg->getConfig().cw() : 0;
    const uint8_t fx = seg ? getTypeFromEffect(const_cast<Effect*>(seg->getEffect())) : 0;
    addf(j, "{\"a\":%u,\"b\":%u,\"geo\":[%u,%u,%u,%u],\"pw\":%u,\"vis\":%u,\"gs\":%u,\"lock\":%u,\"bri\":%u",
         (unsigned)cfg.startLed, (unsigned)cfg.endLed, (unsigned)cfg.matrixWidth, (unsigned)cfg.matrixHeight, (unsigned)cfg.matrixDepth,
         (unsigned)cfg.topology, cfg.savedPower ? 1 : 0, NeoPixelBusModule::isSegmentVisible(cfg) ? 1 : 0, cfg.globalSuspended ? 1 : 0,
         cfg.locked ? 1 : 0, (unsigned)(cfg.savedBrightness ? std::max<int>(1, (cfg.savedBrightness * 100 + 127) / 255) : 0));
    addf(j, ",\"c\":\"#%02x%02x%02x\",\"w\":%u,\"cw\":%u,\"c2\":%ld,\"fx\":%u", (unsigned)((c1 >> 24) & 0xFF), (unsigned)((c1 >> 16) & 0xFF),
         (unsigned)((c1 >> 8) & 0xFF), (unsigned)(c1 & 0xFF), (unsigned)cw, (long)((c2 >> 8) & 0xFFFFFF), (unsigned)fx);
    const uint8_t rs = cfg.emController.runState();
    addf(j, ",\"em\":%u,\"cue\":%u,\"rs\":%u,\"susp\":%u,\"el\":%lu,\"ret\":%u", (unsigned)cfg.emController.activeEmId(), (unsigned)cfg.emController.activeCueNum(),
         (unsigned)rs, cfg.suspendedByPower ? 1 : 0, (unsigned long)cfg.emController.cueElapsedMs(), (unsigned)cfg.emStopReturnMode);
    const uint8_t scenes = m._sceneManager ? m._sceneManager->getSceneCount((uint8_t)i) : 0;
    uint8_t hm = 0, hl = 0;
    if (i < m._hclTransformContext.segmentConfigs.size())
    {
        const NeoHclSegmentConfig& h = m._hclTransformContext.segmentConfigs[i];
        hm = (uint8_t)h.hclMode;
        hl = h.hclMode == NeoHclMode::Global ? m._hclTransformContext.globalHclConfig.resolvedMaster : h.hclMode == NeoHclMode::Custom ? h.customHclConfig.resolvedMaster : 0;
    }
    addf(j, ",\"sc\":%u,\"scene\":%u,\"hcl\":%u,\"hm\":%u,\"hl\":%u", (unsigned)scenes, (unsigned)cfg.savedSceneNumber,
         i < NeoPixelBusModule::kStatusSegmentMax ? (unsigned)m._hclSegmentStatusKelvin[i] : 0u, (unsigned)hm, (unsigned)hl);

    // Effektkette: rx/tx = seconds since the last received/sent telegram, -1 = none since boot
    const bool timeout = cfg.syncMode == 2 && cfg.syncTimeoutSteps > 0 && cfg.lastSyncMs == 0 && m._startupRestoreDone;
    addf(j, ",\"ch\":{\"m\":%u,\"pol\":%u,\"to\":%u,\"total\":%u,\"off\":%u,\"rx\":%ld,\"tx\":%ld,\"tmo\":%u,\"ov\":%u",
         (unsigned)cfg.syncMode, (unsigned)cfg.syncOverridePolicy, (unsigned)cfg.syncTimeoutSteps, (unsigned)cfg.virtualTotalLength,
         (unsigned)cfg.virtualOffset, cfg.lastSyncRxMs ? (long)((now - cfg.lastSyncRxMs) / 1000) : -1L,
         cfg.lastSyncSentMs ? (long)((now - cfg.lastSyncSentMs) / 1000) : -1L, timeout ? 1 : 0, cfg.localOverride ? 1 : 0);
    // master: payload of the last telegram (while an EM runs: the frozen pre-EM state it sends)
    const uint8_t* pl = cfg.syncFreezeValid && cfg.emController.isRunning() ? cfg.syncFreezePayload : cfg.syncPayloadValid ? cfg.lastSyncPayload : nullptr;
    if (cfg.syncMode == 1 && pl)
        addf(j, ",\"pl\":[%u,%u,%u,%u,%u,%u,%u,%u,%u]", pl[0], pl[1], pl[2], pl[3], pl[4], pl[5], pl[6], pl[7], pl[8]);
    j += "}}";
}

void NeoPixelWeb::appendTail(std::string& j)
{
    NeoPixelBusModule& m = openknxNeoPixelModule;
    const uint32_t now = millis();
    j += ",\"relays\":[";
    for (uint8_t i = 0; i < m._relayCount && i < NeoPixelBusModule::kRelayStorageSize; i++)
    {
        uint16_t offDelay = 0, minOff = 0;
    #if defined(ParamNEO_NEOExternalRelay1OffDelay)
        if (i == 0) offDelay = (uint16_t)ParamNEO_NEOExternalRelay1OffDelay;
    #endif
    #if defined(ParamNEO_NEOExternalRelay2OffDelay)
        if (i == 1) offDelay = (uint16_t)ParamNEO_NEOExternalRelay2OffDelay;
    #endif
    #if defined(ParamNEO_NEOExternalRelay3OffDelay)
        if (i == 2) offDelay = (uint16_t)ParamNEO_NEOExternalRelay3OffDelay;
    #endif
    #if defined(ParamNEO_NEOExternalRelay4OffDelay)
        if (i == 3) offDelay = (uint16_t)ParamNEO_NEOExternalRelay4OffDelay;
    #endif
    #if defined(ParamNEO_NEOExternalRelay1MinOffTime)
        if (i == 0) minOff = (uint16_t)ParamNEO_NEOExternalRelay1MinOffTime;
    #endif
    #if defined(ParamNEO_NEOExternalRelay2MinOffTime)
        if (i == 1) minOff = (uint16_t)ParamNEO_NEOExternalRelay2MinOffTime;
    #endif
    #if defined(ParamNEO_NEOExternalRelay3MinOffTime)
        if (i == 2) minOff = (uint16_t)ParamNEO_NEOExternalRelay3MinOffTime;
    #endif
    #if defined(ParamNEO_NEOExternalRelay4MinOffTime)
        if (i == 3) minOff = (uint16_t)ParamNEO_NEOExternalRelay4MinOffTime;
    #endif
        // Rest of the minimum off-time, as scheduleRelayChange() computes it
        uint32_t lockS = 0;
        const unsigned long lastOff = m._relayTimers[i].lastOffTime;
        if (!m._relayStates[i] && minOff > 0 && lastOff != 0)
        {
            const long elapsed = (long)(now - lastOff);
            if (elapsed >= 0 && (unsigned long)elapsed < minOff * 1000UL) lockS = (minOff * 1000UL - (unsigned long)elapsed + 999) / 1000;
        }
        long pendS = -1;
        if (m._relayTimers[i].pendingChange)
        {
            const long left = (long)(m._relayTimers[i].triggerTime - now);
            pendS = left > 0 ? (left + 999) / 1000 : 0;
        }
        addf(j, "%s{\"st\":%u,\"inv\":%u,\"pin\":%u,\"off\":%u,\"min\":%u,\"lock\":%lu,\"pend\":%ld,\"to\":%u}", i ? "," : "",
             m._relayStates[i] ? 1 : 0, m._relayInverted[i] ? 1 : 0, (unsigned)m._relayPins[i], (unsigned)offDelay, (unsigned)minOff,
             (unsigned long)lockS, pendS, m._relayTimers[i].targetState ? 1 : 0);
    }
    j += ']';
    // [seq, result] of the last executed commands, oldest first
    j += ",\"cmds\":[";
    bool first = true;
    for (uint8_t k = 0; k < kCmdRing; k++)
    {
        const uint8_t at = (uint8_t)((_cmdRingPos + k) % kCmdRing);
        if (!_cmdRingSeq[at]) continue;
        addf(j, "%s[%lu,%u]", first ? "" : ",", (unsigned long)_cmdRingSeq[at], (unsigned)_cmdRingRes[at]);
        first = false;
    }
    j += ']';
}

// ============================================================================
// EM definitions (loop task, one EM per pass)
// ============================================================================

uint32_t NeoPixelWeb::emChecksum() const
{
    const NeoPixelBusModule& m = openknxNeoPixelModule;
    const uint8_t count = std::min<uint8_t>(EM_COUNT, (uint8_t)ParamNEO_NEOEMEnabled);
    uint32_t h = 2166136261u ^ count;
    if (!m._emData) return h;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(m._emData);
    const size_t n = (size_t)count * sizeof(EffektManagerData);
    for (size_t k = 0; k < n; k++) h = (h ^ p[k]) * 16777619u;
    return h | 1;
}

void NeoPixelWeb::buildEmStep()
{
    NeoPixelBusModule& m = openknxNeoPixelModule;
    const uint8_t count = m._emData ? std::min<uint8_t>(EM_COUNT, (uint8_t)ParamNEO_NEOEMEnabled) : 0;
    if (_emStep == kStepSwap)
    {
        if (xSemaphoreTake(lockOf(_lock), 0) != pdTRUE) return; // retried next pass
        _emFront.swap(_emBack);
        _emValid = true;
        _emSum = _emSumBuilding;
        xSemaphoreGive(lockOf(_lock));
        _emStep = 0;
        return;
    }
    if (_emStep == 1)
    {
        _emBack.clear();
        addf(_emBack, "{\"sum\":%lu,\"ems\":[", (unsigned long)_emSumBuilding);
    }
    if (_emStep <= count)
    {
        if (_emStep > 1) _emBack += ',';
        appendEm(_emBack, _emStep);
        _emStep++;
        return;
    }
    _emBack += "]}";
    _emStep = kStepSwap;
    buildEmStep();
}

void NeoPixelWeb::appendEm(std::string& j, uint8_t emId)
{
    const EffektManagerData& e = openknxNeoPixelModule._emData[emId - 1];
    const uint8_t cues = std::min<uint8_t>(EM_CUE_COUNT, e.header.cueCount);
    addf(j, "{\"id\":%u,\"st\":%u,\"loop\":%u,\"next\":%u,\"cues\":[", (unsigned)emId, (unsigned)e.header.enabled, e.header.loop ? 1 : 0, (unsigned)e.header.nextEmId);
    for (uint8_t k = 0; k < cues; k++)
    {
        const EffektCue& q = e.cues[k];
        j += k ? ",{\"n\":" : "{\"n\":";
        addJsonStr(j, q.cueName, sizeof(q.cueName));
        addf(j, ",\"fx\":%u,\"c\":\"#%02x%02x%02x\",\"w\":%u,\"bri\":%u,\"d\":%u,\"f\":%u}", (unsigned)q.effectId, q.r, q.g, q.b, q.w,
             (unsigned)((q.brightness * 100 + 127) / 255), (unsigned)q.durationSec, (unsigned)q.fadeMs);
    }
    j += "]}";
}

// ============================================================================
// LED image (loop task)
// ============================================================================

void NeoPixelWeb::copyPixels()
{
    VirtualStrip* vs = openknxNeoPixelModule._virtualStrip;
    if (!vs || !vs->getBuffer()) return;
    const size_t len = vs->getBufferSize();
    const size_t need = kPixHeader + len;
    if (!_pix || _pixCap < need)
    {
        // Allocation outside the lock; a too small heap gets no image instead of a crash later.
        if ((size_t)freeMemory() < need + kPixHeapReserve) return;
        uint8_t* fresh = (uint8_t*)PSRAM_MALLOC(need);
        if (!fresh) return;
        if (xSemaphoreTake(lockOf(_lock), 0) != pdTRUE)
        {
            free(fresh);
            return;
        }
        uint8_t* old = _pix;
        _pix = fresh;
        _pixCap = need;
        _pixLen = 0;
        xSemaphoreGive(lockOf(_lock));
        free(old);
    }
    if (xSemaphoreTake(lockOf(_lock), 0) != pdTRUE) return;
    const uint16_t leds = vs->getLedCount();
    _pixFrame++;
    _pix[0] = 'N';
    _pix[1] = 'P';
    _pix[2] = 1;
    _pix[3] = vs->getBytesPerLed();
    _pix[4] = (uint8_t)(leds & 0xFF);
    _pix[5] = (uint8_t)(leds >> 8);
    _pix[6] = (uint8_t)(_pixFrame & 0xFF);
    _pix[7] = (uint8_t)(_pixFrame >> 8);
    memcpy(_pix + kPixHeader, vs->getBuffer(), len);
    _pixLen = need;
    xSemaphoreGive(lockOf(_lock));
}

// ============================================================================
// HTTP handlers (httpd task: copy and queue only)
// ============================================================================

void NeoPixelWeb::handlePage(WebRequest&, WebResponse& res)
{
    char v[16];
    snprintf(v, sizeof(v), "?v=%lu", (unsigned long)BUILD_TIMESTAMP);
    std::string html;
    html.reserve(256);
    html.append("<link rel='stylesheet' href='/assets/neopixel.css").append(v).append("'>");
    html.append("<div class='container neo' id='neo'></div><script src='/assets/neopixel.js").append(v).append("'></script>");
    res.setLayout(true);
    res.setActiveMenu("/neo");
    res.setContentType("text/html");
    res.send(html.c_str());
}

void NeoPixelWeb::sendBuffer(WebResponse& res, const std::string& buf, const char* mime)
{
    res.setHeader("Cache-Control", "no-store");
    res.setContentType(mime);
    if (xSemaphoreTake(lockOf(_lock), pdMS_TO_TICKS(200)) != pdTRUE)
    {
        res.setStatus(503);
        res.send("{\"busy\":1}");
        return;
    }
    if (buf.empty())
    {
        xSemaphoreGive(lockOf(_lock));
        res.send("{\"pending\":1}"); // first snapshot not built yet: the page retries
        return;
    }
    res.send((const uint8_t*)buf.data(), (int)buf.size());
    xSemaphoreGive(lockOf(_lock));
}

void NeoPixelWeb::sendUnconfigured(WebResponse& res)
{
    // Static data only: without an ETS download nothing of the module is set up.
    #ifdef DEVICE_HW_ID
    const uint16_t hwId = DEVICE_HW_ID;
    #else
    const uint16_t hwId = 0;
    #endif
    const char* board = HardwareMapping::getHardwareName(HardwareMapping::mapDeviceHwIdToIndex(hwId));
    std::string j;
    addf(j, "{\"v\":1,\"unconf\":1,\"up\":%lu,\"hwid\":%u,\"board\":", (unsigned long)uptime(), (unsigned)hwId);
    addJsonStr(j, board ? board : "", 48);
    j += '}';
    res.setHeader("Cache-Control", "no-store");
    res.setContentType("application/json");
    res.send((const uint8_t*)j.data(), (int)j.size());
}

void NeoPixelWeb::handleState(WebRequest&, WebResponse& res)
{
    if (!_configured) return sendUnconfigured(res);
    _stateReqMs.store(millis() | 1);
    sendBuffer(res, _stateFront, "application/json");
}

void NeoPixelWeb::handleEm(WebRequest&, WebResponse& res)
{
    if (!_configured)
    {
        res.setHeader("Cache-Control", "no-store");
        res.setContentType("application/json");
        res.send("{\"sum\":0,\"ems\":[]}");
        return;
    }
    _emReqMs.store(millis() | 1);
    sendBuffer(res, _emFront, "application/json");
}

void NeoPixelWeb::handlePixels(WebRequest&, WebResponse& res)
{
    res.setHeader("Cache-Control", "no-store");
    res.setContentType("application/octet-stream");
    if (!_configured)
    {
        res.send(""); // no LED image without a configuration
        return;
    }
    _pixReqMs.store(millis() | 1);
    if (xSemaphoreTake(lockOf(_lock), pdMS_TO_TICKS(200)) != pdTRUE)
    {
        res.setStatus(503);
        res.send("");
        return;
    }
    if (!_pix || _pixLen == 0)
    {
        xSemaphoreGive(lockOf(_lock));
        res.send(""); // empty: no image yet (first request or too little heap)
        return;
    }
    res.send(_pix, (int)_pixLen);
    xSemaphoreGive(lockOf(_lock));
}

bool NeoPixelWeb::parseCommand(const char* body, size_t len, Command& cmd, const char* token)
{
    // Flat JSON {"segment":n,"function":"name","value":n|"#rrggbb"|"#rrggbb/w","tok":"hex"}; segment is 1-based.
    char buf[kMaxBody + 1];
    if (!body || len == 0 || len > kMaxBody) return false;
    memcpy(buf, body, len);
    buf[len] = '\0';
    auto field = [&buf](const char* key) -> const char* {
        char pat[16];
        snprintf(pat, sizeof(pat), "\"%s\"", key);
        const char* p = strstr(buf, pat);
        if (!p) return nullptr;
        p = strchr(p + strlen(pat), ':');
        if (!p) return nullptr;
        p++;
        while (*p == ' ') p++;
        return p;
    };
    const char* s = field("segment");
    const char* f = field("function");
    const char* v = field("value");
    const char* t = field("tok");
    if (!s || !f || !v || *f != '"') return false;
    // token from /neo/state: a cross-site form post cannot read it
    const size_t tl = token ? strlen(token) : 0;
    if (tl == 0 || !t || *t != '"' || strncmp(t + 1, token, tl) != 0 || t[1 + tl] != '"') return false;
    char* end = nullptr;
    const long seg = strtol(s, &end, 10);
    if (end == s || seg < 1 || seg > 255) return false;
    static const char* const names[kFnCount] = {"power", "bri", "dim", "rgb", "rgbw", "ww", "cw", "fx", "scene", "emStart", "emStop", "cue", "lock"};
    f++;
    int fn = -1;
    for (int k = 0; k < kFnCount; k++)
    {
        const size_t n = strlen(names[k]);
        if (strncmp(f, names[k], n) == 0 && f[n] == '"') fn = k;
    }
    if (fn < 0) return false;
    cmd.segment = (uint8_t)(seg - 1);
    cmd.function = (uint8_t)fn;
    cmd.value = 0;
    cmd.color = 0;
    if (fn == kFnRgb || fn == kFnRgbw)
    {
        // "#rrggbb" or "#rrggbb/w"
        if (v[0] != '"' || v[1] != '#') return false;
        char hex[7];
        for (int k = 0; k < 6; k++)
        {
            if (!isxdigit((unsigned char)v[2 + k])) return false;
            hex[k] = v[2 + k];
        }
        hex[6] = '\0';
        const uint32_t rgb = (uint32_t)strtoul(hex, nullptr, 16);
        long w = 0;
        if (v[8] == '/')
        {
            w = strtol(v + 9, &end, 10);
            if (end == v + 9 || w < 0 || w > 255) return false;
        }
        else if (v[8] != '"')
            return false;
        cmd.color = (rgb << 8) | (uint32_t)w;
        return true;
    }
    const char* num = *v == '"' ? v + 1 : v;
    const long val = strtol(num, &end, 10);
    if (end == num) return false;
    cmd.value = (int32_t)val;
    return true;
}

void NeoPixelWeb::handleControl(WebRequest& req, WebResponse& res)
{
    res.setHeader("Cache-Control", "no-store");
    res.setContentType("application/json");
    if (!_configured)
    {
        res.setStatus(409);
        res.send("{\"error\":\"not configured\"}");
        return;
    }
    Command cmd;
    if (!parseCommand((const char*)req.body(), req.bodyLength(), cmd, _token))
    {
        res.setStatus(400);
        res.send("{\"error\":\"bad command\"}");
        return;
    }
    cmd.seq = ++_cmdSeq;
    if (xQueueSend((QueueHandle_t)_queue, &cmd, 0) != pdTRUE)
    {
        res.setStatus(503);
        res.send("{\"error\":\"busy\"}");
        return;
    }
    _stateReqMs.store(millis() | 1); // keep the snapshot running for the result
    char out[32];
    snprintf(out, sizeof(out), "{\"seq\":%lu}", (unsigned long)cmd.seq);
    res.send(out);
}

#endif // NEOPIXEL_WEB
