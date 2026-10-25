/**
 * @file NeoPixelWeb.h
 * @brief NeoPixel status page /neo (ESP32 builds with OPENKNX_WEBSERVER and a network: KNX_IP_WIFI or KNX_IP_LAN).
 *
 * HTTP handlers run in the httpd task. They only copy buffers that the loop task builds
 * (state JSON, EM JSON, LED image) and queue control commands that the loop task executes.
 */
#pragma once

#if defined(OPENKNX_WEBSERVER) && defined(ARDUINO_ARCH_ESP32) && (defined(KNX_IP_WIFI) || defined(KNX_IP_LAN))
    #define NEOPIXEL_WEB

    #include <atomic>
    #include <cstddef>
    #include <cstdint>
    #include <string>

namespace OpenKNX
{
    namespace Network
    {
        class WebRequest;
        class WebResponse;
    } // namespace Network
} // namespace OpenKNX

class NeoPixelWeb
{
  public:
    /**
     * @brief Registers menu item and routes once; call before the webserver starts.
     * @param configured false: the page only reports "not configured", nothing touches the module
     * @param startMs millis() at the module setup, approximate start of the startup delay
     */
    void setup(bool configured, uint32_t startMs);
    /** @brief Loop task: executes queued commands, builds snapshots while the page polls. */
    void loop();

  private:
    struct Command
    {
        uint32_t seq;
        uint8_t segment;  ///< 0-based
        uint8_t function; ///< Func
        int32_t value;
        uint32_t color; ///< 0xRRGGBBWW for rgb/rgbw
    };

    enum Func : uint8_t
    {
        kFnPower,
        kFnBri,
        kFnDim,
        kFnRgb,
        kFnRgbw,
        kFnWw,
        kFnCw,
        kFnFx,
        kFnScene,
        kFnEmStart,
        kFnEmStop,
        kFnCue,
        kFnLock,
        kFnCount
    };

    enum Result : uint8_t
    {
        kResOk = 0,
        kResLocked = 1,
        kResDeferred = 2,
        kResInvalid = 3,
        kResEmRefused = 4,
        kResSceneMissing = 5,
        kResNoEm = 6,
        kResDropped = 7 ///< not queued during the startup delay (DPT 3.007 dimming, KO without W flag)
    };

    // httpd task
    void handlePage(OpenKNX::Network::WebRequest& req, OpenKNX::Network::WebResponse& res);
    void handleState(OpenKNX::Network::WebRequest& req, OpenKNX::Network::WebResponse& res);
    void handleEm(OpenKNX::Network::WebRequest& req, OpenKNX::Network::WebResponse& res);
    void handlePixels(OpenKNX::Network::WebRequest& req, OpenKNX::Network::WebResponse& res);
    void handleControl(OpenKNX::Network::WebRequest& req, OpenKNX::Network::WebResponse& res);
    void sendBuffer(OpenKNX::Network::WebResponse& res, const std::string& buf, const char* mime);
    void sendUnconfigured(OpenKNX::Network::WebResponse& res);
    /** @brief Parses one command; the body must carry "tok" equal to token (no cross-site form post). */
    static bool parseCommand(const char* body, size_t len, Command& cmd, const char* token);

    // loop task
    void executeCommands();
    uint8_t executeCommand(const Command& cmd);
    void buildStateStep();
    void buildEmStep();
    void copyPixels();
    void mapStrips();
    void appendHead(std::string& j);
    void appendStrips(std::string& j);
    void appendSegment(std::string& j, size_t index);
    void appendTail(std::string& j);
    void appendEm(std::string& j, uint8_t emId);
    uint32_t emChecksum() const;
    void releaseIdle(uint32_t now);

    bool _routesAdded = false;
    bool _configured = false; ///< set once in setup(), before the webserver starts
    uint32_t _startMs = 0;
    char _token[17] = {}; ///< control token, sent in /neo/state only (same-origin read)
    void* _lock = nullptr;  ///< SemaphoreHandle_t: guards _stateFront, _emFront, _pix*
    void* _queue = nullptr; ///< QueueHandle_t of Command

    std::atomic<uint32_t> _stateReqMs{0};
    std::atomic<uint32_t> _emReqMs{0};
    std::atomic<uint32_t> _pixReqMs{0};
    uint32_t _cmdSeq = 0; ///< httpd task only

    // state snapshot: built in steps into _stateBack, swapped into _stateFront under _lock
    std::string _stateFront;
    std::string _stateBack;
    uint8_t _stateStep = 0; ///< 0 = idle
    uint32_t _stateBuiltMs = 0;
    uint32_t _stateSeq = 0;
    bool _stateDirty = false; ///< a command ran: build the next snapshot without waiting

    // EM definitions: rebuilt only when their content changes
    std::string _emFront;
    std::string _emBack;
    uint8_t _emStep = 0; ///< 0 = idle, else next EM id
    uint32_t _emSum = 0;
    uint32_t _emSumBuilding = 0;
    uint32_t _emCheckMs = 0;
    bool _emValid = false;

    // LED image: copy of the virtual buffer with an 8-byte header
    uint8_t* _pix = nullptr;
    size_t _pixCap = 0;
    size_t _pixLen = 0;
    uint16_t _pixFrame = 0;
    uint32_t _pixCopyMs = 0;

    // results of the last executed commands (seq 0 = empty slot), reported in the snapshot
    static constexpr uint8_t kCmdRing = 8;
    uint32_t _cmdRingSeq[kCmdRing] = {};
    uint8_t _cmdRingRes[kCmdRing] = {};
    uint8_t _cmdRingPos = 0;

    // strip -> ETS strip channel and timing selection (refreshed per snapshot)
    static constexpr uint8_t kMaxStrips = 16;
    uint8_t _stripChannel[kMaxStrips] = {};
    uint8_t _stripTiming[kMaxStrips] = {};

    // sampled at most every 10 s (the die sensor read is slow)
    float _temp = 0.0f;
    uint32_t _tempMs = 0;
};

extern NeoPixelWeb neoPixelWeb;

#endif
