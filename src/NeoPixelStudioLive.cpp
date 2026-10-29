#include "NeoPixelModule.h"
#include "NeoPixelStudio.h"

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    #include <algorithm>
    #include <array>
    #if defined(ARDUINO_ARCH_ESP32)
        #include <esp_heap_caps.h>
    #endif

using OpenKNX::Format::JSON::Reader;

std::string NeoPixelStudio::applyLiveCommissioning(size_t index, const Reader& value)
{
    const std::string previousDraft = _commissioning[index];
    std::string error = stageCommissioning(index, value);
    if (!error.empty()) return error;
    const std::string planText = _commissioning[index];
    _commissioning[index] = previousDraft;
    Reader plan(planText.c_str());
    auto* manager = _module._neoPixel.getManager();
    if (!manager || manager->isAnyBusy() || manager->isFrameInFlight()) return "LED output is busy; retry the configuration";
    unsigned total = 0, expectedStrips = 0;
    for (size_t i = 0; plan.get("strips").get(i).valid(); ++i)
    {
        const unsigned length = plan.get("strips").get(i).get("length").toUInt();
        total += length;
        expectedStrips += length != 0;
    }
    for (size_t i = 0; i < _module._segments.size(); ++i)
    {
        if (_module._segments[i].locked) return "A segment is locked by KNX; shared LED configuration cannot change";
        if (i != index && _module._segments[i].endLed >= total) return "Another segment exceeds the new LED count; adjust its range first";
    }
    #if defined(ARDUINO_ARCH_ESP32)
    // Driver, physical and virtual buffers coexist during initialization. Keep
    // headroom for KNX/web tasks rather than risking an allocation panic.
    if (total * 32u + 32768u > heap_caps_get_free_size(MALLOC_CAP_INTERNAL) ||
        total * 8u + 8192u > heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL))
        return "Insufficient memory for this LED count";
    #endif
    if (!(error = stopTest()).empty()) return error;

    struct State
    {
        EffectConfig config;
        Effect* effect;
        bool paused;
    };
    std::vector<State> states;
    for (const auto& cfg : _module._segments)
    {
        if (!cfg.segment) return "Segment unavailable";
        states.push_back({cfg.segment->getConfig(), cfg.segment->getEffect(), cfg.segment->isPaused()});
    }
    const auto previousSegments = _module._segments;
    const bool previousLive = _module._liveLedLayout;
    std::array<uint16_t, 8> lengths{}, skips{};
    std::array<uint16_t, 16> starts{}, ends{};
    std::copy_n(_module._liveStripLengths, 8, lengths.begin());
    std::copy_n(_module._liveStripSkips, 8, skips.begin());
    std::copy_n(_module._liveSegmentStarts, 16, starts.begin());
    std::copy_n(_module._liveSegmentEnds, 16, ends.begin());
    const uint8_t previousChannel = _module._channelIndex;

    auto destroy = [&]() {
        manager->allOff(); // Also blanks LEDs removed from the end of a strip.
        if (_module._virtualStrip) manager->removeVirtualStrip(_module._virtualStrip);
        _module._virtualStrip = nullptr;
        for (auto& cfg : _module._segments)
            cfg.segment = nullptr;
        for (auto* strip : _module._physicalStrips)
            manager->removeStrip(strip);
        _module._physicalStrips.clear();
        _module._physicalStripEtsIds.clear();
    };
    auto rebuild = [&](bool sequential, unsigned expected) {
        _module.configureFromETS(true);
        if (_module._physicalStrips.size() != expected || !manager->init()) return false;
        _module.configureVirtualStripOrder();
        if (sequential)
        {
            uint16_t offset = 0;
            for (auto& mapping : _module._virtualStripConfiguration)
            {
                mapping.virtualStartPosition = offset;
                offset += mapping.ledCount;
            }
        }
        _module.createVirtualStripWithOrder();
        if (!_module._virtualStrip) return false;
        for (size_t i = 0; i < states.size(); ++i)
        {
            auto* segment = _module._segments[i].segment;
            if (!segment) return false;
            segment->getConfig() = states[i].config;
            segment->setEffect(states[i].effect);
            if (states[i].paused) segment->pause();
            else
                segment->resume();
            _module.hideIfNotVisible(i);
        }
        _module.configurePowerManagement();
        _module.updateHclTransformContext();
        return true;
    };
    // Retain the pins actually selected at boot, including automatic allocation.
    // Disabling another strip must never move an existing output to another pin.
    for (size_t i = 0; i < _module._physicalStrips.size(); ++i)
    {
        const auto slot = _module._physicalStripEtsIds[i];
        _module._livePinKnown[slot] = true;
        _module._liveDataPins[slot] = _module._physicalStrips[i]->getDataPin();
        _module._liveClockPins[slot] = _module._physicalStrips[i]->getClockPin();
    }
    unsigned previousStripCount = _module._physicalStrips.size();
    destroy();
    _module._liveLedLayout = true;
    for (size_t i = 0; i < 8; ++i)
    {
        _module._liveStripLengths[i] = plan.get("strips").get(i).get("length").toUInt();
        _module._liveStripSkips[i] = plan.get("strips").get(i).get("skip").toUInt();
    }
    for (size_t i = 0; i < _module._segments.size(); ++i)
    {
        auto& cfg = _module._segments[i];
        if (i == index)
        {
            cfg.startLed = plan.get("start").toUInt() - 1;
            cfg.endLed = plan.get("end").toUInt() - 1;
        }
        _module._liveSegmentStarts[i] = cfg.startLed + 1;
        _module._liveSegmentEnds[i] = cfg.endLed + 1;
    }
    if (!rebuild(true, expectedStrips))
    {
        destroy();
        _module._liveLedLayout = previousLive;
        std::copy(lengths.begin(), lengths.end(), _module._liveStripLengths);
        std::copy(skips.begin(), skips.end(), _module._liveStripSkips);
        std::copy(starts.begin(), starts.end(), _module._liveSegmentStarts);
        std::copy(ends.begin(), ends.end(), _module._liveSegmentEnds);
        for (size_t i = 0; i < previousSegments.size(); ++i)
        {
            _module._segments[i] = previousSegments[i];
            _module._segments[i].segment = nullptr;
        }
        const bool restored = rebuild(previousLive, previousStripCount);
        _module._channelIndex = previousChannel;
        return restored ? "LED initialization failed; previous configuration restored" : "LED initialization failed; restart the device to restore ETS configuration";
    }
    _module._channelIndex = previousChannel;
    // Strip counts are shared by every segment. Discard cached plans so all
    // subsequent readbacks and saves describe the geometry actually running.
    for (auto& draft : _commissioning)
        draft.clear();
    return {};
}
#endif
