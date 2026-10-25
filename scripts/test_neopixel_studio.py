#!/usr/bin/env python3
"""Run Studio's production command handler against hardware doubles under ASan/UBSan."""
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
STUBS = r'''
#include <cassert>
#include <cmath>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "OpenKNX/Format/JSON/Reader.h"
#define OPENKNX_CHARSET
#include "OpenKNX/Charset.hpp"
using OpenKNX::Format::JSON::Reader;
enum class ParameterType { PARAM_UINT8, PARAM_BOOL, PARAM_ENUM, PARAM_STRING };
struct Config {
    char effectText[241] = {};
    uint8_t colours[10] = {10,20,30,40,50,60,70,80,90,100};
    uint8_t r() const { return colours[0]; } uint8_t g() const { return colours[1]; }
    uint8_t b() const { return colours[2]; } uint8_t ww() const { return colours[3]; }
    uint8_t cw() const { return colours[4]; } uint8_t r2() const { return colours[5]; }
    uint8_t g2() const { return colours[6]; } uint8_t b2() const { return colours[7]; }
    uint8_t ww2() const { return colours[8]; } uint8_t cw2() const { return colours[9]; }
};
class Segment;
struct Effect {
    uint8_t id;
    uint8_t getParameterCount() const { return 4; }
    ParameterType getParameterType(uint8_t p) const { return static_cast<ParameterType>(p); }
    uint32_t getParameterMin(uint8_t p) const { return p == 0 ? 1 : 0; }
    uint32_t getParameterMax(uint8_t) const { return 255; }
    uint8_t getEnumValueCount(uint8_t) const { return 3; }
    void setParameter(Segment*, uint8_t, uint32_t);
};
Effect effects[2] = {{0}, {1}};
Effect* getEffectFromType(uint8_t id) { return &effects[id == 1 ? 1 : 0]; }
uint8_t getTypeFromEffect(Effect* e) { return e ? e->id : 0; }
struct Segment {
    Config config;
    Effect* effect = &effects[1];
    uint8_t brightness = 100, master = 100;
    uint32_t params[3] = {};
    std::string text;
    Effect* getEffect() { return effect; }
    Config& getConfig() { return config; }
    void setBrightness(uint8_t b) { brightness = b; }
    void setMasterBrightness(uint8_t b) { master = b; }
    void setPrimaryColor(uint8_t r,uint8_t g,uint8_t b,uint8_t w,uint8_t c) {
        config.colours[0]=r;config.colours[1]=g;config.colours[2]=b;config.colours[3]=w;config.colours[4]=c;
    }
    void setSecondaryColor(uint8_t r,uint8_t g,uint8_t b,uint8_t w,uint8_t c) {
        config.colours[5]=r;config.colours[6]=g;config.colours[7]=b;config.colours[8]=w;config.colours[9]=c;
    }
};
void Effect::setParameter(Segment* s, uint8_t p, uint32_t value) {
    // On embedded targets the API carries the text pointer in a uint32_t. Host tests avoid
    // dereferencing that truncated pointer; text validation is still production code.
    if (p < 3) s->params[p] = value;
}
struct Em {
    bool running = false;
    int reapplied = 0;
    bool isRunning() const { return running; }
    void reapplyMasterBrightness(Segment*) { ++reapplied; }
};
struct VirtualStrip { uint8_t channels = 5; uint8_t getBytesPerLed() const { return channels; } };
struct NeoPixelBusModule {
    struct SegmentConfig {
        enum { NONE = 0 };
        Segment* segment = nullptr;
        bool savedValid=false,suspendedByPower=false,locked=false,localOverride=false,hsvTracked=true,savedEffectValid=false,savedLastWasEffect=false;
        uint8_t syncMode=0, activeDimming=1, savedEffectType=0, savedBrightness=100, savedSceneNumber=2, savedPower=1;
        uint8_t savedR=0,savedG=0,savedB=0,savedWW=0,savedCW=0;
        Em emController;
    };
    std::vector<SegmentConfig> _segments;
    uint8_t _channelIndex=7,_globalBrightness=128;
    bool _localTestMode=false;
    VirtualStrip strip;
    VirtualStrip* _virtualStrip=&strip;
    int interrupts=0, captures=0, hides=0, statuses=0, syncs=0;
    void segmentOn(size_t i) { _segments[i].savedPower=1; if(!_segments[i].savedBrightness)_segments[i].savedBrightness=255; } void segmentOff(size_t i,bool) { _segments[i].savedPower=0; }
    struct SceneDouble {
        static constexpr uint8_t MAX_SCENES=10;
        std::vector<uint8_t> bytes{0};
        uint8_t getSceneCount(size_t) { return 1; }
        std::vector<uint8_t> exportScenes(size_t) { return bytes; }
        bool importScenes(size_t,const uint8_t* data,size_t length) { bytes.assign(data,data+length); return true; }
        void resetCache() { bytes={0}; }
        bool addScene(size_t,Segment*) { return true; } bool deleteScene(size_t,uint8_t) { return true; }
        bool storeScene(size_t,uint8_t,Segment*) { return true; } bool recallScene(size_t,uint8_t,Segment*) { return true; }
    } scenes;
    SceneDouble* _sceneManager=&scenes;
    void interruptEmIfRunning(size_t i) { ++interrupts; _segments[i].emController.running=false; }
    void applyEffectToSegment(Segment* s,uint8_t id) { s->effect=getEffectFromType(id); }
    void captureDirectColor(size_t) { ++captures; }
    void hideIfNotVisible(size_t) { ++hides; }
    void statusAfterCommand(uint8_t) { ++statuses; }
    void sendSyncTelegram(size_t) { ++syncs; }
};
struct NeoPixelStudio {
    NeoPixelBusModule& _module;
    std::string saveSegment(size_t) { return {}; }
    std::string apply(const std::string&);
    std::string applyDoc(const Reader&,bool=false,uint8_t=255);
};
'''
TESTS = r'''
int main() {
    NeoPixelBusModule module;
    Segment segment;
    module._segments.resize(1);
    module._segments[0].segment=&segment;
    auto& cfg=module._segments[0];
    NeoPixelStudio studio{module};
    auto bad = [&](const char* json) {
        int before=module.interrupts;
        assert(!studio.apply(json).empty());
        assert(module.interrupts==before);
        assert(module._channelIndex==7);
    };
    bad("{}");
    bad(R"({"segment":0,"action":"effect","value":1})");
    bad(R"({"segment":257,"action":"effect","value":1})");
    bad(R"({"segment":1.5,"action":"effect","value":1})");
    bad(R"({"segment":100e-2,"action":"brightness","value":256})");
    bad(R"({"segment":1,"action":"stripConfig","value":1})");
    bad(R"({"segment":1,"action":"effect","value":254})");
    bad(R"({"segment":1,"action":"primary","value":"#12345x"})");
    bad(R"({"segment":1,"action":"primary","value":"#1234567"})");
    bad(R"({"segment":1,"action":"parameter","effect":0,"parameter":0,"value":4})");
    bad(R"({"segment":1,"action":"parameter","effect":1,"parameter":4,"value":4})");
    bad(R"({"segment":1,"action":"parameter","effect":1,"parameter":0,"value":0})");
    bad(R"({"segment":1,"action":"parameter","effect":1,"parameter":1,"value":2})");
    bad(R"({"segment":1,"action":"parameter","effect":1,"parameter":2,"value":3})");
    bad(R"({"segment":1,"action":"parameter","effect":1,"parameter":3,"value":"😀"})");
    cfg.locked=true;
    bad(R"({"segment":1,"action":"primary","value":"#123456"})");
    cfg.locked=false;
    module.strip.channels=3;
    bad(R"({"segment":1,"action":"primaryWhite","value":80})");
    module.strip.channels=4;
    bad(R"({"segment":1,"action":"secondaryCool","value":80})");
    module.strip.channels=5;
    assert(studio.apply(R"({"segment":100e-2,"action":"primary","value":"#123456"})").empty());
    assert(segment.config.r()==0x12 && segment.config.g()==0x34 && segment.config.b()==0x56);
    assert(segment.config.ww()==40 && segment.config.cw()==50);
    assert(studio.apply(R"({"segment":1,"action":"secondaryCool","value":90})").empty());
    assert(segment.config.cw2()==90 && segment.config.ww2()==90);
    bad(R"({"segment":1,"action":"primaryChannels","value":{"rgb":"#123456","ww":256,"cw":0}})");
    bad(R"({"segment":1,"action":"primaryChannels","value":{"rgb":"#123456","ww":0}})");
    assert(segment.config.r()==0x12 && segment.config.ww()==40);
    module.strip.channels=3;
    bad(R"({"segment":1,"action":"primaryChannels","value":{"rgb":"#000000","ww":1,"cw":0}})");
    module.strip.channels=4;
    bad(R"({"segment":1,"action":"secondaryChannels","value":{"rgb":"#000000","ww":0,"cw":1}})");
    module.strip.channels=5;
    assert(studio.apply(R"({"segment":1,"action":"primaryChannels","value":{"rgb":"#000000","ww":180,"cw":75}})").empty());
    assert(segment.config.r()==0 && segment.config.g()==0 && segment.config.b()==0 && segment.config.ww()==180 && segment.config.cw()==75);
    assert(studio.apply(R"({"segment":1,"action":"primaryChannels","value":{"rgb":"#ff6600","ww":0,"cw":0}})").empty());
    assert(segment.config.r()==255 && segment.config.g()==102 && segment.config.ww()==0 && segment.config.cw()==0);
    assert(studio.apply(R"({"segment":1,"action":"secondaryChannels","value":{"rgb":"#000000","ww":0,"cw":220}})").empty());
    assert(segment.config.r2()==0 && segment.config.g2()==0 && segment.config.b2()==0 && segment.config.ww2()==0 && segment.config.cw2()==220);
    cfg.emController.running=true;
    cfg.syncMode=1;
    int interrupts=module.interrupts;
    assert(studio.apply(R"({"segment":1,"action":"brightness","value":2e2})").empty());
    assert(cfg.savedBrightness==200 && segment.master==100);
    assert(cfg.emController.running && cfg.emController.reapplied==1 && module.interrupts==interrupts);
    assert(module.syncs==1);
    assert(studio.apply(R"({"segment":1,"action":"parameter","effect":1,"parameter":0,"value":200})").empty());
    assert(segment.params[0]==200 && !cfg.emController.running && cfg.savedSceneNumber==0);
    assert(studio.apply(R"({"segment":1,"action":"parameter","effect":1,"parameter":3,"value":"Grüße"})").empty());
    cfg.syncMode=2;
    assert(studio.apply(R"({"segment":1,"action":"effect","value":0})").empty());
    assert(segment.effect->id==0 && cfg.savedEffectValid && cfg.savedLastWasEffect && cfg.localOverride);
    assert(module._channelIndex==7);
    const uint8_t oldR=segment.config.r();
    const std::string invalidBatch=R"({"segment":1,"action":"batch","value":[{"segment":1,"action":"primaryChannels","value":{"rgb":"#000000","ww":0,"cw":0}},{"segment":1,"action":"brightness","value":256}]})";
    bad(invalidBatch.c_str());
    assert(segment.config.r()==oldR);
    assert(studio.apply(R"({"segment":1,"action":"batch","value":[{"segment":1,"action":"effect","value":1},{"segment":1,"action":"parameter","effect":1,"parameter":0,"value":33}]})").empty());
    assert(segment.params[0]==33);
    cfg.savedBrightness=0;
    assert(studio.apply(R"({"segment":1,"action":"power","value":1})").empty());
    assert(cfg.savedBrightness==0 && segment.master==0 && cfg.savedPower==1);
}
'''


def main():
    source = (ROOT / 'src/NeoPixelStudio.cpp').read_text()
    handler = source[source.index('std::string NeoPixelStudio::apply('):source.index('std::string NeoPixelStudio::snapshot(')]
    numeric = source[source.index('    bool integer('):source.index('    std::string utf8(')]
    with tempfile.TemporaryDirectory(prefix='neopixel-studio-test-') as directory:
        path = Path(directory)
        (path / 'test.cpp').write_text(STUBS + '\nstruct SceneManager { static constexpr uint8_t MAX_SCENES=10; };\n' + numeric + handler + TESTS)
        subprocess.run([shutil.which('clang++') or 'c++', '-std=c++17', '-fsanitize=address,undefined',
                        '-fno-omit-frame-pointer', '-I' + str(ROOT / 'lib/OFM-Network/src'),
                        '-I' + str(ROOT / 'lib/OGM-Common/src'), str(path / 'test.cpp'),
                        str(ROOT / 'lib/OFM-Network/src/OpenKNX/Format/JSON/Reader.cpp'),
                        '-o', str(path / 'test')], check=True)
        subprocess.run([str(path / 'test')], check=True)
    print('Studio command validation and runtime control regressions passed')


if __name__ == '__main__':
    main()
