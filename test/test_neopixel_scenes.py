#!/usr/bin/env python3
"""Exercise the production scene cache without writable KNX parameters."""
from pathlib import Path
import runpy
import shutil
import subprocess
import tempfile
ROOT = Path(__file__).resolve().parents[1]
STUBS = runpy.run_path(str(ROOT / 'test/test_neopixel_studio.py'))['STUBS']
STUBS = STUBS[:STUBS.index('struct NeoPixelStudio {')]
STUBS = STUBS.replace('struct Config {', '''struct Config {

    uint8_t option1=0,option2=0,option3=0,reverse=0,fade=0,count=0,mode=0;
    bool feature1=false,feature2=false,feature3=false;
    uint32_t secondaryRGBW=0;uint8_t secondaryCW=0;
''')
STUBS = STUBS.replace('    void setParameter(Segment*, uint8_t, uint32_t);', '''    const char* getName() const { return "Fixture"; }
    const char* getParameterName(uint8_t) const { return "Fixture"; }
    uint32_t getParameter(const Segment*,uint8_t) const { return 1; }
    void setParameter(Segment*, uint8_t, uint32_t);''')
STUBS = STUBS.replace('    Effect* getEffect()', '''    struct VStrip { bool hasDualWhiteChannel() const { return true; } };
    VStrip* getVirtualStrip() const { return nullptr; }
    uint8_t getBrightness() const { return brightness; }
    Effect* getEffect()''')
STUBS = STUBS.replace('bool locked=false,', 'bool suspendedByPower=false,locked=false,')
STUBS = STUBS.replace('    bool isRunning() const', '    uint8_t activeEmId() const { return running?1:0; }\n    bool isRunning() const')
STUBS = STUBS.replace('    int interrupts=0', '''    auto& getSegments() { return _segments; }
    static bool isSegmentVisible(const SegmentConfig& c) { return c.savedPower!=0; }
    void startEffektManager(size_t,uint8_t) {}
    void applyBrightnessIntent(size_t i) { _segments[i].segment->brightness=_segments[i].savedBrightness; }
    void sceneSwitchOn(size_t i) { _segments[i].savedPower=1; }
    int interrupts=0''')
STUBS += r'''
#define logInfoP(...) ((void)0)
#define logErrorP(...) ((void)0)
#define EM_COUNT 16
#define NEO_ParamBlockOffset 0
#define NEO_ParamBlockSize 704
using EffectConfig=Config;
struct Knx { uint8_t data[704*16]={}; uint8_t paramByte(size_t i) { assert(i<sizeof(data));return data[i]; } } knx;
'''
TEST = r'''
#include "SceneManager.h"
#include "NeoPixelModule.h"
int main() {
    knx.data[159]=0x10;
    knx.data[160]=1;knx.data[171]=120;
    const std::vector<uint8_t> original(knx.data,knx.data+sizeof(knx.data));
    NeoPixelBusModule module;Segment segment;module._segments.resize(1);module._segments[0].segment=&segment;
    SceneManager scenes(&module);
    assert(scenes.getSceneCount(0)==1);
    assert(scenes.storeScene(0,1,&segment));
    assert(scenes.sceneData(0,1)[1]==10);
    assert(scenes.addScene(0,&segment));assert(scenes.getSceneCount(0)==2);
    assert(!scenes.deleteScene(0,1));
    const auto saved=scenes.exportScenes(0);
    assert(saved.size()==73 && saved[0]==2);
    assert(scenes.deleteScene(0,2));
    SceneManager restored(&module);
    assert(restored.importScenes(0,saved.data(),saved.size()));
    assert(restored.getSceneCount(0)==2);
    assert(restored.recallScene(0,2,&segment));
    assert(segment.config.r()==10 && segment.params[0]==1);
    assert(!restored.importScenes(0,saved.data(),saved.size()-1));
    assert(restored.getSceneCount(0)==2);
    assert(std::equal(original.begin(),original.end(),knx.data));
    assert(scenes.importScenes(1,saved.data(),saved.size()));
    scenes.restoreEtsScenes(0);
    assert(scenes.getSceneCount(0)==1 && scenes.sceneData(0,1)[1]==0);
    assert(scenes.exportScenes(1)==saved); // another segment's edits remain untouched
    scenes.restoreEtsScenes(255); // invalid channels cannot read outside ETS parameters
    assert(std::equal(original.begin(),original.end(),knx.data));
    strcpy(segment.config.effectText,"Longer than fourteen bytes");
    assert(!scenes.storeScene(0,1,&segment));
    assert(std::equal(original.begin(),original.end(),knx.data));
}
'''
def main():
    with tempfile.TemporaryDirectory(prefix='neopixel-scene-cache-') as directory:
        temp=Path(directory)
        (temp/'effects').mkdir()
        (temp/'OpenKNX.h').write_text('#pragma once\n'+STUBS)
        for name in ('NeoPixelModule.h','Segment.h','knxprod.h','EffectTypeMapping.h','EffectParameterMapping.h','effects/Effect.h'):
            (temp/name).write_text('#include "OpenKNX.h"\n')
        shutil.copyfile(ROOT/'src/SceneManager.h',temp/'SceneManager.h')
        shutil.copyfile(ROOT/'src/SceneManager.cpp',temp/'production.cpp')
        (temp/'test.cpp').write_text(TEST+'\n#include "production.cpp"\n')
        subprocess.run([shutil.which('clang++') or 'c++','-std=c++17','-fsanitize=address,undefined','-I'+str(temp),'-I'+str(ROOT/'lib/OFM-Network/src'),'-I'+str(ROOT/'lib/OGM-Common/src'),str(temp/'test.cpp'),'-o',str(temp/'test')],check=True)
        subprocess.run([str(temp/'test')],check=True)
    print('Scene cache, learn/add/delete/recall, import bounds and immutable ETS memory regressions passed')
if __name__=='__main__':main()
