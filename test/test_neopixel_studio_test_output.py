#!/usr/bin/env python3
"""Verify production commissioning white tests across mixed physical outputs."""
from pathlib import Path
import shutil
import subprocess
import tempfile
ROOT=Path(__file__).resolve().parents[1]
STUBS=r'''
#include <cstdint>
#include <string>
#include <vector>
#include <cassert>
struct ProtocolHelper { static bool hasWhiteChannel(unsigned channels) { return channels>=4; } };
struct PhysicalStrip { unsigned channels; unsigned getColorOrder() const { return channels; } };
struct NeoPixelBusModule {
    struct Segment { unsigned startLed=5,endLed=34; };
    struct Mapping { unsigned physicalStripIndex,virtualStartPosition,ledCount; };
    std::vector<Segment> _segments{{5,34}};
    std::vector<Mapping> _virtualStripConfiguration{{0,0,10},{1,10,10},{2,20,20}};
    std::vector<PhysicalStrip*> _physicalStrips;
};
struct NeoPixelStudio {
    NeoPixelBusModule& _module; int _testSegment=-1; std::string _testPattern;
    bool transformTestPixel(uint16_t,uint8_t&,uint8_t&,uint8_t&,uint8_t*,uint8_t*);
};
'''
TESTS=r'''
int main() {
    NeoPixelBusModule module; PhysicalStrip rgb{3},rgbw{4},cct{5};
    module._physicalStrips={&rgb,&rgbw,&cct}; NeoPixelStudio studio{module};
    uint8_t r=0,g=0,b=0,ww=15,cw=17;
    assert(!studio.transformTestPixel(5,r,g,b,&ww,&cw));
    studio._testSegment=0;
    for (const auto* pattern: {"white","warm","cool"}) {
        studio._testPattern=pattern;
        r=g=b=0; ww=pattern==std::string("cool")?0:32; cw=pattern==std::string("cool")?32:0;
        assert(studio.transformTestPixel(5,r,g,b,&ww,&cw));
        assert(r>0 && g>0 && b>0 && r<=32 && g<=32 && b<=32);
        if(studio._testPattern=="warm") assert(r>g && g>b);
        if(studio._testPattern=="cool") assert(b>g && g>r);
        // Dedicated W / WW / CW stay isolated: no synthesized RGB on either strip.
        for(unsigned pixel:{10,20}) {
            r=g=b=0; const uint8_t oldWW=ww,oldCW=cw;
            assert(studio.transformTestPixel(pixel,r,g,b,&ww,&cw));
            assert(r==0 && g==0 && b==0 && ww==oldWW && cw==oldCW);
        }
    }
    // Pure RGB buffers support the same white tests without W pointers.
    studio._testPattern="white";r=g=b=20;
    assert(studio.transformTestPixel(5,r,g,b,nullptr,nullptr));assert(r==20&&g==20&&b==20);
    studio._testPattern="warm";r=g=b=20;
    assert(studio.transformTestPixel(5,r,g,b,nullptr,nullptr));assert(r==20&&r>g&&g>b);
    r=12;g=34;b=56;ww=7;cw=8;
    assert(!studio.transformTestPixel(4,r,g,b,&ww,&cw));
    assert(!studio.transformTestPixel(35,r,g,b,&ww,&cw));
    assert(r==12&&g==34&&b==56&&ww==7&&cw==8);
    studio._testPattern="red";
    assert(studio.transformTestPixel(5,r,g,b,&ww,&cw));
    assert(r==12&&g==34&&b==56);
}
'''
with tempfile.TemporaryDirectory(prefix='neopixel-test-output-') as directory:
    temp=Path(directory)
    (temp/'NeoPixelModule.h').write_text('#pragma once\n'+STUBS)
    (temp/'NeoPixelStudio.h').write_text('#pragma once\n')
    shutil.copyfile(ROOT/'src/NeoPixelStudioTestOutput.cpp',temp/'output.cpp')
    (temp/'test.cpp').write_text('#include "output.cpp"\n'+TESTS)
    subprocess.run([shutil.which('clang++') or 'c++','-std=c++17','-fsanitize=address,undefined','-DOPENKNX_WEBSERVER','-DKNX_IP_WIFI','-I'+str(temp),str(temp/'test.cpp'),'-o',str(temp/'test')],check=True)
    subprocess.run([str(temp/'test')],check=True)
print('Mixed RGB/RGBW/RGBCCT white tests: native channel isolation, RGB fallback, brightness and segment scope passed')
