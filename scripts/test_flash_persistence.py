#!/usr/bin/env python3
"""Host regressions for the production flash serializer (no PlatformIO builds).

Compile the unchanged production .cpp/.h against small KNX/hardware doubles.
This exercises persistence and parameter indexing, not the LED driver or EM engine.
"""
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

STUBS = r'''
#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#define logInfoP(...) ((void)0)
#define logWarningP(...) ((void)0)
#define NEO_WARN_FLASH_DISCARDED 1
#define HIGH 1
#define LOW 0
#define EM_NONE 0
inline void digitalWrite(uint8_t, int) {}
inline unsigned long millis() { return 0; }
struct Flash {
    std::vector<uint8_t> bytes;
    void write(uint8_t* p, size_t n) { bytes.insert(bytes.end(), p, p + n); }
};
inline struct OpenKnx { Flash flash; } openknx;
inline uint8_t fixtureGlobalBehavior = 0, behavior[16] = {}, effects[16] = {};
// Generated NEO_ParamCalcIndex resolves _channelIndex in the caller's scope.
#define ParamNEO_NEOGlobalStartupBehavior fixtureGlobalBehavior
#define ParamNEO_NEOSegmentStartupBehavior behavior[_channelIndex]
#define ParamNEO_NEONEOEffectType effects[_channelIndex]
struct VirtualStrip { void show() {} uint8_t getBytesPerLed() { return 3; } };
class Segment {
public:
    uint8_t brightness = 0, master = 0, color[5] = {};
    uint8_t secondary[5] = {};
    struct Secondary { const uint8_t* c; uint8_t r2() const{return c[0];} uint8_t g2() const{return c[1];} uint8_t b2() const{return c[2];} uint8_t ww2() const{return c[3];} uint8_t cw2() const{return c[4];} uint8_t cw() const{return 0;} };
    Secondary getConfig() const { return {secondary}; }
    void setSecondaryColor(uint8_t r,uint8_t g,uint8_t b,uint8_t w,uint8_t c) { secondary[0]=r;secondary[1]=g;secondary[2]=b;secondary[3]=w;secondary[4]=c; }
    bool paused = false;
    bool isPaused() { return paused; }
    void resume() { paused=false; }
    uint32_t getPrimaryColor() const { return (uint32_t(color[0])<<24)|(uint32_t(color[1])<<16)|(uint32_t(color[2])<<8)|color[3]; }
    uint8_t getBrightness() { return brightness; }
    void setBrightness(uint8_t b) { brightness = b; }
    void setMasterBrightness(uint8_t b) { master = b; }
    void setPrimaryColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t c) {
        color[0]=r; color[1]=g; color[2]=b; color[3]=w; color[4]=c;
    }
    void getPixel(int, uint8_t& r, uint8_t& g, uint8_t& b) {
        r=color[0]; g=color[1]; b=color[2];
    }
    void getPixel(int i, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& w) {
        getPixel(i,r,g,b); w=color[3];
    }
    void getPixel(int i, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& w, uint8_t& c) {
        getPixel(i,r,g,b,w); c=color[4];
    }
};
struct Em {
    uint8_t id = 0;
    uint8_t lastEmId() { return id; }
    void setLastEmId(uint8_t n) { id=n; }
    void saveState() {}
    void restoreState(Segment*, void*) { assert(false && "EM outside test scope"); }
    void reapplyMasterBrightness(Segment*) {}
    bool isRunning() const { return false; }
};
class SceneManager {
public:
    static constexpr uint8_t MAX_SCENES = 10, SCENE_SIZE = 36;
    uint32_t signature = 7;
    uint32_t etsSignature(size_t) const { return signature; }
    std::vector<uint8_t> scenes{0};
    std::vector<uint8_t> exportScenes(size_t) { return scenes; }
    bool importScenes(size_t,const uint8_t* data,size_t length) { scenes.assign(data,data+length);return true; }
    bool recallScene(uint8_t, uint8_t, Segment*) { assert(false); return false; }
};
class NeoPixelBusModule {
public:
    struct SegmentConfig {
        Segment* segment = nullptr;
        bool savedValid = false, savedEffectValid = false, savedLastWasEffect = false, globalSuspended=false, emStartedFromOff=false;
        uint8_t savedPower = 1, savedBrightness = 0, savedEffectType = 0;
        uint8_t savedR = 0, savedG = 0, savedB = 0, savedWW = 0, savedCW = 0;
        uint8_t savedSecondaryR=0,savedSecondaryG=0,savedSecondaryB=0,savedSecondaryWW=0,savedSecondaryCW=0;
        uint8_t savedSceneNumber = 0;
        Em emController;
    };
    std::vector<SegmentConfig> segments;
    static constexpr uint8_t kMaxExternalRelays = 1;
    uint8_t _relayCount = 0, _relayPins[1] = {255}, channel = 0;
    bool _startupRestoreDone = true;
    bool _relayStates[1] = {}, _relayInverted[1] = {}, _globalPowerOn = true;
    static constexpr uint8_t kRelayStartupLast = 0, kRelayStartupOff = 1, kRelayStartupOn = 2;
    uint8_t _relayStartup[1] = {}, _relayHeldMask = 0, _relayMaskBeforeShutdown = 0;
    struct RelayTimerState { bool targetState = false, pendingChange = false; unsigned long triggerTime = 0, lastOffTime = 0; } _relayTimers[1];
    bool relayIntendedState(uint8_t i) const { return _relayStates[i]; }
    uint16_t relayMinOffSeconds(uint8_t) const { return 0; }
    void* _emData = nullptr;
    SceneManager* _sceneManager = nullptr;
    VirtualStrip strip;
    auto& getSegments() { return segments; }
    uint16_t getNumberOfSegments() { return segments.size(); }
    VirtualStrip* getVirtualStrip() { return &strip; }
    void setChannelIndex(uint8_t n) { channel=n; }
    uint8_t getChannelIndex() const { return channel; }
    uint8_t getGlobalBrightness() const { return 255; }
    uint8_t effectiveStartupBehavior(size_t i) const { uint8_t v=behavior[i]?behavior[i]-1:fixtureGlobalBehavior;return v>2?2:v; }
    static bool isSegmentVisible(const SegmentConfig& c) { return c.savedPower && !c.globalSuspended; }
    void captureSyncFreeze(size_t) {}
    void segmentOff(size_t i,bool) { segments[i].savedPower=0;segments[i].segment->setBrightness(0);segments[i].segment->paused=true; }

    void setWarningBlink(int) {}
    void applyEffectToSegment(Segment* seg, uint8_t) { assert(seg == segments[channel].segment); }
};
'''

TESTS = r'''
#include "NeoPixelFlashPersistence.h"
#include "NeoPixelModule.h"
#include <iostream>
using State = NeoPixelFlashPersistence::SegmentFlashState;

static State snapshot(bool globalOn, uint8_t segmentOn, uint8_t brightness, bool paused = false) {
    Segment seg;
    seg.brightness = brightness;
    seg.setPrimaryColor(128,0,255,0,0);
    seg.paused = paused;
    NeoPixelBusModule module;
    module._globalPowerOn = globalOn;
    module.segments.resize(1);
    auto& cfg = module.segments[0];
    cfg.segment = &seg;
    cfg.savedPower = segmentOn;
    cfg.globalSuspended = !globalOn;
    cfg.savedValid = true;
    cfg.savedR = 128; cfg.savedB = 255; cfg.savedBrightness=brightness;
    NeoPixelFlashPersistence persistence(&module);
    openknx.flash.bytes.clear();
    persistence.writeToFlash();
    assert(openknx.flash.bytes.size() == sizeof(State) + 3 + 6 + 370);
    State state;
    memcpy(&state, openknx.flash.bytes.data(), sizeof(state));
    assert(state.brightness == brightness); // retain level for subsequent power-on
    assert(state.r == 128 && state.g == 0 && state.b == 255);
    return state;
}

int main(int argc, char** argv) {
    assert(argc == 2);
    std::string test = argv[1];
    if (test == "segment-off") {
        assert(snapshot(true, 0, 180, true).power == 0);
    } else if (test == "global-off") {
        assert(snapshot(false, 1, 180).power == 0);
    } else if (test == "on-and-dark") {
        assert(snapshot(true, 1, 180).power == 1);
        assert(snapshot(true, 1, 0).power == 1); // dimmed dark is still switched on
        assert(snapshot(true, 1, 180, true).power == 1); // manual pause keeps light on
        assert(snapshot(true, 0, 180).power == 0); // own power remains off
    } else if (test == "startup-matrix") {
        // Every global setting and pair of segment overrides, with saved ON/OFF.
        for (fixtureGlobalBehavior=0; fixtureGlobalBehavior<3; ++fixtureGlobalBehavior)
        for (behavior[0]=0; behavior[0]<4; ++behavior[0])
        for (behavior[1]=0; behavior[1]<4; ++behavior[1])
        for (uint8_t savedPower=0; savedPower<2; ++savedPower) {
            Segment seg[2];
            NeoPixelBusModule module;
            module.segments.resize(2);
            State saved[2] = {};
            uint8_t initial[2];
            for (int i=0; i<2; ++i) {
                int mode = behavior[i] ? behavior[i]-1 : fixtureGlobalBehavior;
                initial[i] = mode == 0 ? 0 : 90;
                seg[i].brightness = initial[i];
                module.segments[i].segment = &seg[i];
                saved[i].version = NeoPixelFlashPersistence::FLASH_FORMAT_VERSION;
                saved[i].validFlags = 0x07;
                saved[i].power = savedPower;
                saved[i].brightness = 180;
                saved[i].r = 128; saved[i].b = 255;
            }
            NeoPixelFlashPersistence persistence(&module);
            persistence.readFromFlash(reinterpret_cast<uint8_t*>(saved), sizeof(saved));
            persistence.restoreStatesAfterStartup();
            for (int i=0; i<2; ++i) {
                int mode = behavior[i] ? behavior[i]-1 : fixtureGlobalBehavior;
                uint8_t expected = mode == 1 ? (savedPower ? 180 : 0) : initial[i];
                if (seg[i].brightness != expected)
                    std::cerr << "global=" << int(fixtureGlobalBehavior) << " overrides=" << int(behavior[0])
                              << "," << int(behavior[1]) << " savedPower=" << int(savedPower)
                              << " segment=" << i << " expected=" << int(expected)
                              << " actual=" << int(seg[i].brightness) << "\n";
                assert(seg[i].brightness == expected);
                if (mode == 1) assert(seg[i].color[0] == 128 && seg[i].color[2] == 255);
            }
        }
    } else if (test == "solid-off-roundtrip") {
        State saved = snapshot(true, 0, 180, true);
        Segment seg;
        seg.brightness = 90;
        NeoPixelBusModule module;
        module.segments.resize(1);
        module.segments[0].segment = &seg;
        behavior[0] = 2; // last state overrides global Aus
        NeoPixelFlashPersistence persistence(&module);
        persistence.readFromFlash(reinterpret_cast<uint8_t*>(&saved), sizeof(saved));
        persistence.restoreStatesAfterStartup();
        assert(seg.brightness == 0 && seg.master == 180);
        assert(seg.color[0] == 128 && seg.color[2] == 255);
    } else if (test == "scene-extension") {
        NeoPixelBusModule module;
        Segment seg;
        module.segments.resize(1); module.segments[0].segment=&seg;
        auto& cfg=module.segments[0];cfg.savedValid=true;cfg.savedBrightness=120;
        SceneManager scenes;scenes.scenes.resize(37);scenes.scenes[0]=1;scenes.scenes[1]=1;scenes.scenes[12]=120;
        module._sceneManager=&scenes;
        seg.setSecondaryColor(10,20,30,40,50);
        NeoPixelFlashPersistence persistence(&module);
        persistence.writeToFlash();
        const auto saved=openknx.flash.bytes;
        scenes.scenes={0};
        behavior[0]=2;
        seg.setSecondaryColor(0,0,0,0,0);
        persistence.readFromFlash(saved.data(),saved.size());
        assert(persistence.hasSceneSnapshot() && scenes.scenes.size()==37 && scenes.scenes[0]==1);
        assert(cfg.savedSecondaryR==10 && cfg.savedSecondaryCW==50);
        assert(seg.secondary[0]==10 && seg.secondary[4]==50);
        // ETS download without scene changes: the learned scenes stay
        openknx.flash.bytes.clear();persistence.writeToFlash();
        const auto unchanged=openknx.flash.bytes;
        assert(unchanged[sizeof(State)+2]==0xA5); // relay signature
        scenes.scenes={0};
        persistence.readFromFlash(unchanged.data(),unchanged.size());
        assert(persistence.hasSceneSnapshot() && scenes.scenes.size()==37 && scenes.scenes[12]==120);
        // ETS scenes changed: the ETS scenes apply, segment state and secondary colour stay
        scenes.signature=8;scenes.scenes={0};
        seg.setSecondaryColor(0,0,0,0,0);cfg.savedBrightness=0;
        persistence.readFromFlash(unchanged.data(),unchanged.size());
        assert(persistence.hasSceneSnapshot() && scenes.scenes.size()==1);
        assert(cfg.savedBrightness==120 && seg.secondary[0]==10 && seg.secondary[4]==50);
        // v1 extension (no signature): imported unless written during an ETS download (header byte 5)
        std::vector<uint8_t> v1(unchanged.begin(),unchanged.begin()+sizeof(State)+3);
        const uint8_t header[6]={'N','S','C',1,1,0};
        v1.insert(v1.end(),header,header+6);
        v1.insert(v1.end(),unchanged.begin()+sizeof(State)+3+6,unchanged.begin()+sizeof(State)+3+6+5);
        v1.insert(v1.end(),unchanged.begin()+sizeof(State)+3+6+9,unchanged.end());
        assert(v1.size()==sizeof(State)+3+6+366);
        scenes.scenes={0};
        persistence.readFromFlash(v1.data(),v1.size());
        assert(persistence.hasSceneSnapshot() && scenes.scenes.size()==37 && scenes.scenes[12]==120);
        // v1 written during an ETS download: empty extension, the secondary colour must not become 0
        v1[sizeof(State)+3+5]=1;for(int b=0;b<5;++b) v1[sizeof(State)+3+6+b]=0;scenes.scenes={0};
        persistence.readFromFlash(v1.data(),v1.size());
        assert(!persistence.hasSceneSnapshot() && scenes.scenes.size()==1 && seg.secondary[0]==10 && cfg.savedSecondaryR==10);
        // Corrupt header: nothing restored, warning
        auto bad=unchanged;bad[sizeof(State)+3+3]=9;cfg.savedBrightness=0;scenes.scenes={0};seg.setSecondaryColor(0,0,0,0,0);
        persistence.readFromFlash(bad.data(),bad.size());
        assert(cfg.savedBrightness==0 && scenes.scenes.size()==1 && seg.secondary[0]==0);
    } else if (test == "scene-extension-two-segments") {
        // The second segment's extension starts one v2 record (370 bytes) after the first
        NeoPixelBusModule module;
        Segment seg[2];
        module.segments.resize(2);
        for (int i=0;i<2;++i){module.segments[i].segment=&seg[i];module.segments[i].savedValid=true;}
        SceneManager scenes;scenes.scenes.resize(37);scenes.scenes[0]=1;scenes.scenes[12]=77;
        module._sceneManager=&scenes;
        seg[0].setSecondaryColor(1,2,3,4,5);seg[1].setSecondaryColor(6,7,8,9,10);
        NeoPixelFlashPersistence persistence(&module);
        openknx.flash.bytes.clear();persistence.writeToFlash();
        const auto saved=openknx.flash.bytes;
        assert(saved.size()==2*sizeof(State)+3+6+2*370);
        const size_t second=2*sizeof(State)+3+6+370;
        assert(saved[second]==6 && saved[second+4]==10 && saved[second+9]==1 && saved[second+9+12]==77);
        behavior[0]=2;behavior[1]=2;
        seg[0].setSecondaryColor(0,0,0,0,0);seg[1].setSecondaryColor(0,0,0,0,0);
        persistence.readFromFlash(saved.data(),saved.size());
        assert(seg[0].secondary[0]==1 && seg[1].secondary[0]==6 && seg[1].secondary[4]==10);
    } else { assert(false && "unknown test"); }
    std::cout << test << " passed\n";
}
'''


def verify(source=None):
    compiler = shutil.which("clang++") or shutil.which("g++")
    if not compiler:
        raise SystemExit("A host C++ compiler is required; no environment will be installed.")
    with tempfile.TemporaryDirectory(prefix="neopixel-flash-test-") as directory:
        path = Path(directory)
        (path / "OpenKNX.h").write_text(STUBS)
        for name in ("NeoPixelModule.h", "SceneManager.h", "Segment.h"):
            (path / name).write_text('#include "OpenKNX.h"\n')
        shutil.copyfile(ROOT / "src/NeoPixelFlashPersistence.h", path / "NeoPixelFlashPersistence.h")
        (path / "production.cpp").write_text(source if source is not None else
                                            (ROOT / "src/NeoPixelFlashPersistence.cpp").read_text())
        (path / "test.cpp").write_text(TESTS)
        for mode in ([], ["-DOPENKNX_DEBUG"]):
            subprocess.run([compiler, "-std=c++17", "-fsanitize=address,undefined",
                            "-fno-omit-frame-pointer", *mode,
                            str(path / "production.cpp"), str(path / "test.cpp"),
                            "-o", str(path / "test")], check=True)
            for case in ("segment-off", "global-off", "on-and-dark", "startup-matrix", "solid-off-roundtrip", "scene-extension",
                         "scene-extension-two-segments"):
                subprocess.run([str(path / "test"), case], check=True)


if __name__ == "__main__":
    verify()
