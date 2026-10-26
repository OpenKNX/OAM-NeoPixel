#pragma once

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    #include "OpenKNX/Format/JSON/Reader.h"
    #include "OpenKNX/Format/JSON/Writer.h"
    #include <atomic>
    #include <array>
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
    friend class NeoPixelBusModule;
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

    std::array<std::string, 16> _commissioning;
    std::string _testRestore;
    int _testSegment = -1;
    uint32_t _testUntil = 0;
    std::string commissioning(size_t index, bool downloaded = false, bool runtime = false) const;
    void writeCommissioning(OpenKNX::Format::JSON::Writer& json, size_t index, bool downloaded = false, bool runtime = false) const;
    std::string commissioningControl(size_t index, const OpenKNX::Format::JSON::Reader& doc, bool validateOnly);
    std::string applyLiveCommissioning(size_t index, const OpenKNX::Format::JSON::Reader& value);
    std::string stageCommissioning(size_t index, const OpenKNX::Format::JSON::Reader& value);
    std::string stopTest();
    void tickTest(uint32_t now);

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
