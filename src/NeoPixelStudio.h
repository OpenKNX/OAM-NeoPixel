#pragma once

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    #include "OpenKNX/Format/JSON/Reader.h"
    #include <atomic>
    #include <cstdint>
    #include <string>

class NeoPixelBusModule;

class NeoPixelStudio
{
  public:
    explicit NeoPixelStudio(NeoPixelBusModule& module) : _module(module) {}
    void setup(bool configured);
    void loop();
    void restoreSaved(bool scenesOnly);
    void invalidateSaved();
    bool readback(uint8_t length, const uint8_t* data, uint8_t* result, uint8_t& resultLength);

  private:
    NeoPixelBusModule& _module;
    // HTTP runs on another task on ESP32. Never wait for it from the rendering loop.
    std::atomic<uint32_t> _guard{0};
    std::atomic<bool> _refresh{true};
    std::atomic<bool> _acceptCommands{true};
    std::atomic<uint32_t> _requestedSegment{1};
    std::string _pending;
    std::string _state = "{\"ready\":false,\"segments\":[]}";
    std::string _effects;
    uint32_t _nextId = 0;
    uint32_t _pendingId = 0;
    uint32_t _completedId = 0;
    uint32_t _lastRefresh = 0;
    std::string _error;
    bool _registered = false;
    bool _processing = false;

    std::string _etsExport;
    uint16_t _etsToken = 0;
    uint8_t _etsSegment = 0, _etsSource = 0;
    uint32_t configSignature(size_t index) const;
    std::string exportSegment(size_t index, bool forSave, std::string& error) const;
    std::string saveSegment(size_t index);
    std::string readSaved(size_t index, std::string& error) const;
    std::string apply(const std::string& command);
    std::string applyDoc(const OpenKNX::Format::JSON::Reader& doc, bool validateOnly = false, uint8_t expectedEffect = 255);
    std::string snapshot();
};
#endif
