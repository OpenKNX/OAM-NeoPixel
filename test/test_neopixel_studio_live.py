#!/usr/bin/env python3
"""Exercise production live reconfiguration with owned, replaceable LED buffers."""
from pathlib import Path
import shutil
import subprocess
import tempfile
ROOT = Path(__file__).resolve().parents[1]
STUBS = r'''
#include "OpenKNX/Format/JSON/Reader.h"
#include "OpenKNX/Format/JSON/Writer.h"
#include <array>
#include <vector>
#include <string>
#include <cassert>
using OpenKNX::Format::JSON::Reader;
using OpenKNX::Format::JSON::Writer;
#define ParamNEO_NEONumberOfLEDStrips 2
struct Effect {};
struct EffectConfig { unsigned brightness=106, colour=0xff0022; };
struct Segment {
    EffectConfig config; Effect* effect=nullptr; bool paused=false;
    EffectConfig& getConfig() { return config; }
    Effect* getEffect() { return effect; }
    bool isPaused() { return paused; }
    void setEffect(Effect* e) { effect=e; }
    void pause() { paused=true; } void resume() { paused=false; }
};
struct PhysicalStrip { unsigned getDataPin() { return 5; } unsigned getClockPin() { return 255; } };
struct VirtualStrip { std::vector<Segment*> segments; ~VirtualStrip() { for(auto* s:segments) delete s; } };
struct Manager {
    bool busy=false, frame=false; int blanks=0;
    bool isAnyBusy() { return busy; } bool isFrameInFlight() { return frame; }
    bool init() { return true; }
    void allOff() { ++blanks; }
    void removeStrip(PhysicalStrip* p) { delete p; }
    void removeVirtualStrip(VirtualStrip* p) { delete p; }
};
struct NeoPixelBusModule {
    struct Config { Segment* segment=nullptr; unsigned startLed=0,endLed=29; bool locked=false; unsigned savedPower=1; };
    struct Mapping { unsigned virtualStartPosition=0,ledCount=0; };
    Manager manager;
    struct Pixel { Manager* m; Manager* getManager() { return m; } } _neoPixel{&manager};
    bool _livePinKnown[8]={}; uint8_t _liveDataPins[8]={},_liveClockPins[8]={};
    bool _liveLedLayout=false; uint8_t _channelIndex=3, _stripsWithoutGpio=0;
    uint16_t _liveStripLengths[8]={},_liveStripSkips[8]={},_liveSegmentStarts[16]={},_liveSegmentEnds[16]={};
    std::vector<Config> _segments{2};
    std::vector<PhysicalStrip*> _physicalStrips;
    std::vector<unsigned> _physicalStripEtsIds;
    std::vector<Mapping> _virtualStripConfiguration;
    VirtualStrip* _virtualStrip=nullptr;
    int failures=0, powerUpdates=0;
    NeoPixelBusModule() { configureFromETS(true); configureVirtualStripOrder(); createVirtualStripWithOrder(); }
    ~NeoPixelBusModule() { delete _virtualStrip; for(auto* p:_physicalStrips) delete p; }
    void configureFromETS(bool) {
        _virtualStripConfiguration.clear();
        if(failures) { --failures; return; }
        for(unsigned i=0;i<2;++i) if(!_liveLedLayout || _liveStripLengths[i]) { _physicalStrips.push_back(new PhysicalStrip); _physicalStripEtsIds.push_back(i); }
    }
    void configureVirtualStripOrder() { _virtualStripConfiguration={{0,_liveLedLayout?_liveStripLengths[0]:30u},{30,_liveLedLayout?_liveStripLengths[1]:30u}}; }
    void applyVirtualStripLength(uint32_t=0) {}
    void createVirtualStripWithOrder() { _virtualStrip=new VirtualStrip; for(auto& c:_segments) { c.segment=new Segment; _virtualStrip->segments.push_back(c.segment); } }
    void hideIfNotVisible(size_t i) { if(!_segments[i].savedPower) _segments[i].segment->pause(); }
    void configurePowerManagement() { ++powerUpdates; }
    void updateHclTransformContext() {}
};
struct NeoPixelStudio {
    NeoPixelBusModule& _module;
    std::array<std::string,16> _commissioning;
    explicit NeoPixelStudio(NeoPixelBusModule& m):_module(m) {}
    std::string stopTest() { return {}; }
    std::string stageCommissioning(size_t,const Reader&);
    std::string applyLiveCommissioning(size_t,const Reader&);
};
'''
TESTS = r'''
int main() {
    NeoPixelBusModule module; NeoPixelStudio studio(module); Effect effect;
    module._segments[0].segment->setEffect(&effect);
    module._segments[0].segment->getConfig()={71,0x123456};
    module._segments[1].savedPower=0; module._segments[1].segment->pause();
    const char* good=R"({"start":4,"end":100,"strips":[{"id":1,"length":80,"skip":3},{"id":2,"length":30,"skip":0}]})";
    Reader plan(good);
    module.failures=1;
    assert(studio.applyLiveCommissioning(0,plan).find("vorherige Konfiguration wiederhergestellt")!=std::string::npos);
    assert(!module._liveLedLayout && module._segments[0].endLed==29);
    assert(module._segments[0].segment->getConfig().brightness==71);
    module.manager.blanks=0; module.powerUpdates=0;
    module.manager.busy=true;
    assert(!studio.applyLiveCommissioning(0,plan).empty());
    assert(!module._liveLedLayout && module.manager.blanks==0);
    module.manager.busy=false; module._segments[1].locked=true;
    assert(!studio.applyLiveCommissioning(0,plan).empty());
    assert(module.manager.blanks==0);
    module._segments[1].locked=false;
    assert(studio.applyLiveCommissioning(0,plan).empty());
    assert(module._liveLedLayout && module._liveStripLengths[0]==80 && module._liveStripSkips[0]==3);
    assert(module._segments[0].startLed==3 && module._segments[0].endLed==99);
    assert(module._segments[0].segment->getEffect()==&effect);
    assert(module._segments[0].segment->getConfig().brightness==71 && module._segments[0].segment->getConfig().colour==0x123456);
    assert(module._segments[1].segment->isPaused() && module._segments[1].savedPower==0);
    assert(module._channelIndex==3 && module.powerUpdates==1);
    assert(module._virtualStripConfiguration[1].virtualStartPosition==80);
    const char* larger=R"({"start":1,"end":120,"strips":[{"id":1,"length":100,"skip":0},{"id":2,"length":30,"skip":0}]})";
    module.failures=1; Reader failed(larger);
    assert(studio.applyLiveCommissioning(0,failed).find("vorherige Konfiguration wiederhergestellt")!=std::string::npos);
    assert(module._liveStripLengths[0]==80 && module._segments[0].endLed==99);
    assert(module._segments[0].segment->getConfig().brightness==71);
    Reader tooSmall(R"({"start":1,"end":10,"strips":[{"id":1,"length":10,"skip":0},{"id":2,"length":0,"skip":0}]})");
    const int blanks=module.manager.blanks;
    assert(studio.applyLiveCommissioning(0,tooSmall).find("Ein anderes Segment")!=std::string::npos);
    assert(module.manager.blanks==blanks);
    module.failures=2;
    assert(studio.applyLiveCommissioning(0,failed).find("Gerät neu starten")!=std::string::npos);
    assert(module._segments[0].segment==nullptr && module._segments[1].segment==nullptr);
}
'''
def main():
    source=(ROOT/'src/NeoPixelStudioCommissioning.cpp').read_text()
    bounded=source[source.index('namespace\n{'):source.index('std::string NeoPixelStudio::commissioning')]
    stage=source[source.index('std::string NeoPixelStudio::stageCommissioning'):source.index('std::string NeoPixelStudio::commissioningControl')]
    with tempfile.TemporaryDirectory(prefix='neopixel-live-') as directory:
        temp=Path(directory)
        (temp/'NeoPixelModule.h').write_text('#pragma once\n'+STUBS)
        (temp/'NeoPixelStudio.h').write_text('#pragma once\n')
        shutil.copyfile(ROOT/'src/NeoPixelStudioLive.cpp',temp/'live.cpp')
        (temp/'test.cpp').write_text('#include "NeoPixelModule.h"\n#include <algorithm>\n#include <cmath>\n'+bounded+stage+'\n#include "'+str(temp/'live.cpp')+'"\n'+TESTS)
        subprocess.run([shutil.which('clang++') or 'c++','-std=c++17','-fsanitize=address,undefined','-DOPENKNX_WEBSERVER','-DKNX_IP_WIFI','-I'+str(temp),'-I'+str(ROOT/'lib/OFM-Network/src'),'-I'+str(ROOT/'lib/OGM-Common/src'),str(temp/'test.cpp'),str(ROOT/'lib/OFM-Network/src/OpenKNX/Format/JSON/Reader.cpp'),str(ROOT/'lib/OFM-Network/src/OpenKNX/Format/JSON/Writer.cpp'),'-o',str(temp/'test')],check=True)
        subprocess.run([str(temp/'test')],check=True)
    print('Live LED layout: geometry, state preservation, locks, bounds and initialization rollback passed')
if __name__=='__main__': main()
