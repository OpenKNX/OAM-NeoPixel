#!/usr/bin/env python3
"""Host regressions for the production Studio save/readback code, with filesystem doubles."""
from pathlib import Path
import runpy
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
STUBS = runpy.run_path(str(ROOT / 'scripts/test_neopixel_studio.py'))['STUBS']
STUBS = STUBS[:STUBS.index('struct NeoPixelStudio {')]
STUBS = STUBS.replace('char effectText[241] = {};', 'char effectText[241] = "Hello";')
STUBS = STUBS.replace('    void setParameter(Segment*, uint8_t, uint32_t);',
                      '    uint32_t getParameter(const Segment*, uint8_t p) const { return p==0 ? 12 : 1; }\n    void setParameter(Segment*, uint8_t, uint32_t);')
STUBS = STUBS.replace('    bool isRunning() const', '    uint8_t lastEmId() const { return 0; }\n    bool isRunning() const')
STUBS = STUBS.replace('    int interrupts=0', '    struct Pixel { struct Manager { void flushFrame() {} } manager; Manager* getManager() { return &manager; } } _neoPixel;\n    uint8_t effectiveStartupBehavior(size_t) const { return 1; }\n    struct Persistence { bool present=false; bool hasSceneSnapshot() const { return present; } } persistence; Persistence* _flashPersistence=&persistence;\n    int interrupts=0')
STUBS += r'''
#define EM_NONE 0
inline struct OpenKnx { struct Flash { int calls=0; void save() {++calls;} } flash; } openknx;
#define NEO_ParamBlockSize 704
#define NEO_ParamBlockOffset 0
struct FakeKnx { uint8_t bytes[704] = {}; uint8_t paramByte(size_t p) const { return bytes[p]; } } knx;
const char* effectParameterEtsName(uint8_t, uint8_t p) { static const char* names[]={"Speed","Bool","Mode","Text"}; return names[p]; }
'''
FS = r'''
#pragma once
#include <map>
#include <algorithm>
inline std::map<std::string,std::string> files;
inline bool failRename=false,failWrite=false;
struct File {
    std::string* value=nullptr;
    size_t position=0;
    explicit operator bool() const { return value!=nullptr; }
    size_t size() const { return value ? value->size() : 0; }
    size_t read(uint8_t* data,size_t size) { size=std::min(size,value->size()-position); memcpy(data,value->data()+position,size); position+=size; return size; }
    size_t write(uint8_t value) { return write(&value,1); }
    size_t write(const uint8_t* data,size_t size) { if(failWrite)return 0; value->append((const char*)data,size); return size; }
    void flush(){} void close(){}
};
struct MockFs {
    bool begin() const { return true; }
    File open(const char* path,const char* mode) {
        if(mode[0]=='w'){files[path].clear();return File{&files[path]};}
        auto it=files.find(path);return it==files.end()?File{}:File{&it->second};
    }
    bool exists(const char* path) const { return files.count(path)>0; }
    bool remove(const char* path) { return files.erase(path)>0; }
    bool rename(const char* from,const char* to) { if(failRename)return false; files[to]=files.at(from);files.erase(from);return true; }
} LittleFS;
'''
TESTS = r'''
#include "NeoPixelModule.h"
#define private public
#include "NeoPixelStudio.h"
#undef private
#include "LittleFS.h"
#include "OpenKNX/Format/JSON/Reader.h"
int main() {
    NeoPixelBusModule module;
    Segment segment;
    module._segments.resize(1); module._segments[0].segment=&segment;
    NeoPixelStudio studio(module);
    std::string error;
    assert(studio.readSaved(0,error).empty() && !error.empty());
    assert(studio.saveSegment(0).empty());
    error.clear();
    const std::string first=studio.readSaved(0,error);
    assert(!first.empty() && error.empty());
    module._segments[0].savedBrightness=201;
    failRename=true;
    assert(!studio.saveSegment(0).empty());
    failRename=false;
    assert(studio.readSaved(0,error)==first);
    failWrite=true;
    assert(!studio.saveSegment(0).empty());
    failWrite=false;
    assert(studio.readSaved(0,error)==first);
    module._segments[0].emController.running=true;
    assert(!studio.saveSegment(0).empty());
    module._segments[0].emController.running=false;
    strcpy(segment.config.effectText,"More than fourteen bytes");
    assert(!studio.saveSegment(0).empty());
    strcpy(segment.config.effectText,"Hello");
    NeoPixelStudio afterRestart(module);
    assert(afterRestart.readSaved(0,error)==first);
    uint8_t request[]={32,1,1,0,0}, response[48], length=0;
    std::string received;
    do {
        size_t offset=received.size();request[3]=offset>>8;request[4]=offset&255;
        assert(afterRestart.readback(5,request,response,length));
        assert(response[0]==0 && response[1]==1 && length>=7 && length<=48);
        received.append((const char*)response+6,length-6);
    } while(received.size()<size_t(response[4]*256+response[5]));
    assert(received==first);
    request[2]=0;request[3]=request[4]=0;
    afterRestart.readback(5,request,response,length);
    assert(response[0]==0 && afterRestart._etsExport!=first);
    request[1]=0;
    afterRestart.readback(5,request,response,length);
    assert(response[0]==1 && length==1);
    module._segments[0].savedSceneNumber=0;
    module._segments[0].savedBrightness=9;
    afterRestart.restoreSaved(true);
    afterRestart.restoreSaved(false);
    assert(module._segments[0].savedBrightness==100);
    assert(openknx.flash.calls>=1);
    module.persistence.present=true;
    module._segments[0].savedBrightness=9;
    module._segments[0].savedPower=0;
    module._segments[0].savedEffectType=1;
    afterRestart.restoreSaved(false);
    assert(module._segments[0].savedBrightness==9 && module._segments[0].savedPower==0);
    assert(segment.params[0]==12);
    afterRestart.invalidateSaved();
    error.clear(); assert(afterRestart.readSaved(0,error).empty());
    assert(files.count("/neopixel-studio-invalidated")==1);
    assert(studio.saveSegment(0).empty());
    knx.bytes[0]=1;error.clear();
    assert(studio.readSaved(0,error).empty() && error.find("ETS download")!=std::string::npos);
}
'''


def main():
    with tempfile.TemporaryDirectory(prefix='neopixel-studio-storage-') as directory:
        temp = Path(directory)
        (temp / 'NeoPixelModule.h').write_text('#pragma once\n' + STUBS)
        for header in ('NetworkModule.h', 'EffectTypeMapping.h', 'EffectParameterMapping.h', 'SceneManager.h', 'NeoPixelFlashPersistence.h'):
            (temp / header).write_text('#include "NeoPixelModule.h"\n')
        (temp / 'LittleFS.h').write_text(FS)
        shutil.copyfile(ROOT / 'src/NeoPixelStudio.h', temp / 'NeoPixelStudio.h')
        shutil.copyfile(ROOT / 'src/NeoPixelStudioPersistence.cpp', temp / 'production.cpp')
        control = (ROOT / 'src/NeoPixelStudio.cpp').read_text()
        helpers = control[control.index('    bool integer('):control.index('    std::string utf8(')]
        methods = control[control.index('std::string NeoPixelStudio::apply('):control.index('std::string NeoPixelStudio::snapshot(')]
        (temp / 'SceneManager.h').write_text('#pragma once\n#include "NeoPixelModule.h"\nstruct SceneManager { static constexpr uint8_t MAX_SCENES=10; };\n')
        (temp / 'test.cpp').write_text(TESTS)
        (temp / 'combined.cpp').write_text(TESTS + '\n#include "SceneManager.h"\n' + helpers + methods + '\n#include "production.cpp"\n')
        subprocess.run([shutil.which('clang++') or 'c++', '-std=c++17', '-fsanitize=address,undefined',
                        '-DOPENKNX_WEBSERVER', '-DKNX_IP_WIFI', '-DOPENKNX_LITTLE_FS=1',
                        '-I' + str(temp), '-I' + str(ROOT / 'lib/OFM-Network/src'), '-I' + str(ROOT / 'lib/OGM-Common/src'),
                        str(temp / 'combined.cpp'), str(ROOT / 'lib/OFM-Network/src/OpenKNX/Format/JSON/Reader.cpp'),
                        str(ROOT / 'lib/OFM-Network/src/OpenKNX/Format/JSON/Writer.cpp'), '-o', str(temp / 'test')], check=True)
        subprocess.run([str(temp / 'test')], check=True)
    print('Studio durable save, write failure, ETS signature and paged readback regressions passed')


if __name__ == '__main__':
    main()
