#!/usr/bin/env pwsh
# Open #
# +----+  Test-FireEffect
# # KNX   2026 OpenKNX - Erkan Colak
#
# FILEPATH: test/Test-FireEffect.ps1

<#
.SYNOPSIS
    OpenKNX NeoPixel - host test of the Fire effect: simulation pace and direction (1D and 2D).

.DESCRIPTION
    Compiles lib/OFM-NeoPixel/src/effects/FireEffect.h with its own Effect.h, FastLEDMath.h and
    ParameterType.h against a Segment double (C++17, ASan/UBSan where the compiler supports it) and checks:

      pace:      one simulation step every 6400 / speed ms, at most 400 ms (the 10 ms floor is not
                 reachable: speed 255 = 25 ms), at most 4 steps per frame, a backlog beyond 4 steps dropped. Speed 128 at 50 ms frames = 1 step per frame,
                 255 = 2, 64 = 0/1 alternating, 1 = one step per 400 ms, 0 = 400 ms, a 2000 ms gap = 4
                 steps and no catch-up afterwards;
      update:    update() and update2D() change the heat field only in frames with a due step;
      direction: parameter 3 (config.feature1) mirrors the rendered order, 1D along the strip and 2D
                 along the columns; config.reverse does not count.

    The sources are copied into a temporary folder, the repository is not changed.

.PARAMETER Root
    Repository root to read the sources from (default: the folder above this script).

.PARAMETER KeepTemp
    Keep the temporary folder (path in the output) for inspection.

.EXAMPLE
    pwsh test/Test-FireEffect.ps1

.NOTES
    Author: Erkan Colak
    Needs clang++ or g++ in PATH. On macOS with only the Command Line Tools set DEVELOPER_DIR to Xcode.
    Exit code: 0 = passed, 1 = check(s) failed or the build failed, 2 = compiler or source missing.
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory = $false)]
    [string]$Root = "",

    [Parameter(Mandatory = $false)]
    [switch]$KeepTemp
)

$ErrorActionPreference = 'Stop'

function OpenKNX_ShowLogo($AddCustomText = $null) {
    Write-Host ""
    Write-Host "Open " -NoNewline
    Write-Host "$( [char]::ConvertFromUtf32(0x25A0) )" -ForegroundColor Green
    $unicodeString = "$( [char]::ConvertFromUtf32(0x252C) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2534) ) "
    if ($AddCustomText) { Write-Host "$($unicodeString) $($AddCustomText)" -ForegroundColor Green }
    else { Write-Host "$($unicodeString)" -ForegroundColor Green }
    Write-Host "$( [char]::ConvertFromUtf32(0x25A0) )" -NoNewline -ForegroundColor Green
    Write-Host " KNX"
    Write-Host ""
}

OpenKNX_ShowLogo "Fire Effect Test"

if (-not $Root) { $Root = Split-Path -Parent $PSScriptRoot }
$effects = Join-Path $Root 'lib/OFM-NeoPixel/src/effects'
$onWindows = [System.Environment]::OSVersion.Platform -eq [System.PlatformID]::Win32NT
$utf8 = New-Object System.Text.UTF8Encoding($false)

$cxx = $null
foreach ($name in @('clang++', 'g++')) {
    $cmd = Get-Command $name -ErrorAction SilentlyContinue
    if ($cmd) { $cxx = $cmd.Path; break }
}
if (-not $cxx) {
    Write-Host "MISSING: clang++ or g++ in PATH" -ForegroundColor Red
    exit 2
}
foreach ($file in @('FireEffect.h', 'Effect.h', 'FastLEDMath.h', 'ParameterType.h')) {
    if (-not (Test-Path -LiteralPath (Join-Path $effects $file) -PathType Leaf)) {
        Write-Host "MISSING: $effects/$file" -ForegroundColor Red
        exit 2
    }
}

$arduinoStub = @'
#pragma once
#include <stdint.h>
inline uint32_t millis() { return 0; }
'@

$segmentStub = @'
#pragma once
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <vector>
struct EffectConfig {
    uint8_t speed = 128, intensity = 255, option1 = 0, option2 = 0;
    bool feature1 = false, feature2 = false;
    uint8_t reverse = 0; // former direction source; the tests set it opposite to feature1
};
struct LedGeometry {
    uint8_t width = 0, height = 0;
    bool is1D() const { return width <= 1 || height <= 1; }
};
class Segment {
  public:
    EffectConfig config;
    LedGeometry geo;
    uint16_t length = 0;
    std::vector<uint32_t> pixels;
    EffectConfig& getConfig() { return config; }
    const EffectConfig& getConfig() const { return config; }
    uint16_t getLength() const { return length; }
    const LedGeometry& getGeometry() const { return geo; }
    void setPixel(uint16_t i, uint8_t r, uint8_t g, uint8_t b)
    {
        if (i >= pixels.size()) { std::fprintf(stderr, "pixel %u out of range\n", unsigned(i)); std::abort(); }
        pixels[i] = (uint32_t(r) << 16) | (uint32_t(g) << 8) | b;
    }
    bool setPixelXY(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b)
    {
        if (x >= geo.width || y >= geo.height) { std::fprintf(stderr, "pixel %u/%u out of range\n", unsigned(x), unsigned(y)); std::abort(); }
        setPixel(uint16_t(y) * geo.width + x, r, g, b);
        return true;
    }
};
'@

$testSource = @'
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "Segment.h"
#include "effects/Effect.h"
#include "effects/FastLEDMath.h"
// 64-bit host: the vtable pointer aligns to 8, the 4-byte check holds on the 32-bit targets only
#undef EFFECT_PER_SEGMENT_INSTANCE
#define EFFECT_PER_SEGMENT_INSTANCE(Type) \
    size_t instanceSize() const override { return sizeof(Type); } \
    Effect* constructAt(void* mem) const override { return new (mem) Type(); } \
    void reconstruct() override { this->~Type(); new (this) Type(); }
#define private public // reach takeSteps() and the heat fields
#include "effects/FireEffect.h"
#undef private

static int failures = 0;
#define CHECK(e) do { if (!(e)) { std::fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #e); ++failures; } } while (0)

// Steps per frame for one speed and frame time, from a fresh effect state
static std::vector<int> steps(uint8_t speed, uint32_t frameMs, int frames)
{
    FireEffect* fire = new FireEffect();
    std::vector<int> out;
    for (int i = 0; i < frames; ++i) out.push_back(fire->takeSteps(speed, frameMs));
    delete fire;
    return out;
}

static bool all(const std::vector<int>& v, int n)
{
    for (int x : v) if (x != n) return false;
    return true;
}

// Cells 0/1 cold, the rest 255: one step pulls cell 2 below 255 (drift from 0/1; a spark adds at most 254)
static void seed1D(FireEffect& fire, uint16_t length)
{
    for (uint16_t i = 0; i < length; ++i) fire._heat[i] = i < 2 ? 0 : 255;
}

static void seed2D(FireEffect& fire, uint8_t w, uint8_t h)
{
    for (uint8_t x = 0; x < w; ++x)
        for (uint8_t y = 0; y < h; ++y) fire._heat2D[x][y] = y < 2 ? 0 : 255;
}

int main()
{
    // Pace: one step every 6400 / speed ms, at most 4 per frame
    CHECK(all(steps(128, 50, 20), 1));
    CHECK(all(steps(255, 50, 20), 2));
    {
        std::vector<int> v = steps(64, 50, 20);
        for (size_t i = 0; i < v.size(); ++i) CHECK(v[i] == int(i % 2));
    }
    {
        // speed 1: 6400 ms clamped to 400 ms
        std::vector<int> v = steps(1, 50, 16);
        for (size_t i = 0; i < v.size(); ++i) CHECK(v[i] == ((i % 8) == 7 ? 1 : 0));
        FireEffect* fire = new FireEffect();
        CHECK(fire->takeSteps(1, 399) == 0);
        CHECK(fire->takeSteps(1, 1) == 1);
        delete fire;
    }
    {
        // speed 0 (ETS minimum is 1) takes the slowest pace instead of dividing by zero
        FireEffect* fire = new FireEffect();
        CHECK(fire->takeSteps(0, 399) == 0);
        CHECK(fire->takeSteps(0, 1) == 1);
        delete fire;
    }
    {
        // A 2000 ms gap: 4 steps, the rest of the backlog is dropped
        FireEffect* fire = new FireEffect();
        CHECK(fire->takeSteps(128, 2000) == 4);
        CHECK(fire->takeSteps(128, 50) == 1);
        CHECK(fire->takeSteps(128, 50) == 1);
        CHECK(fire->takeSteps(64, 2000) == 4);
        CHECK(fire->takeSteps(64, 50) == 0);
        CHECK(fire->takeSteps(64, 50) == 1);
        delete fire;
    }
    {
        // update(): speed 64 at 50 ms frames moves the heat every second frame, speed 128 every frame
        FireEffect* fire = new FireEffect();
        Segment seg;
        seg.length = 32;
        seg.pixels.assign(32, 0);
        seg.config.speed = 64;
        fire->update(&seg, 0); // sizes the heat field
        for (int frame = 0; frame < 8; ++frame)
        {
            seed1D(*fire, 32);
            fire->update(&seg, 50);
            CHECK((fire->_heat[2] != 255) == (frame % 2 == 1));
        }
        seg.config.speed = 128;
        for (int frame = 0; frame < 4; ++frame)
        {
            seed1D(*fire, 32);
            fire->update(&seg, 50);
            CHECK(fire->_heat[2] != 255);
        }
        delete fire;
    }
    {
        // update2D(): same pace per column
        FireEffect* fire = new FireEffect();
        Segment seg;
        seg.geo.width = 4;
        seg.geo.height = 8;
        seg.pixels.assign(32, 0);
        seg.config.speed = 64;
        fire->update2D(&seg, 0); // sizes the heat field
        for (int frame = 0; frame < 8; ++frame)
        {
            seed2D(*fire, 4, 8);
            fire->update2D(&seg, 50);
            for (uint8_t x = 0; x < 4; ++x) CHECK((fire->_heat2D[x][2] != 255) == (frame % 2 == 1));
        }
        delete fire;
    }
    {
        // 1D direction from parameter 3 (feature1)
        const uint16_t len = 16;
        std::vector<uint32_t> forward, reverse;
        for (int dir = 0; dir < 2; ++dir)
        {
            FireEffect* fire = new FireEffect();
            Segment seg;
            seg.length = len;
            seg.pixels.assign(len, 0xdeadbeef);
            fire->setParameter(&seg, 3, dir);
            CHECK(fire->getParameter(&seg, 3) == uint32_t(dir));
            seg.config.reverse = dir ? 0 : 1;
            seg.config.speed = 1;
            fire->update(&seg, 0); // sizes the heat field, no step
            for (uint16_t i = 0; i < len; ++i) fire->_heat[i] = uint8_t(i * 16);
            fire->update(&seg, 0); // renders without a step
            (dir ? reverse : forward) = seg.pixels;
            delete fire;
        }
        CHECK(forward[0] == 0 && forward[len - 1] != 0);
        CHECK(reverse[len - 1] == 0 && reverse[0] != 0);
        for (uint16_t i = 0; i < len; ++i) CHECK(forward[i] == reverse[len - 1 - i]);
    }
    {
        // 2D: without reverse row y 0 shows the top heat row, with parameter 3 the bottom one
        const uint8_t w = 3, h = 8;
        std::vector<uint32_t> forward, reverse;
        for (int dir = 0; dir < 2; ++dir)
        {
            FireEffect* fire = new FireEffect();
            Segment seg;
            seg.geo.width = w;
            seg.geo.height = h;
            seg.pixels.assign(w * h, 0xdeadbeef);
            fire->setParameter(&seg, 3, dir);
            seg.config.reverse = dir ? 0 : 1;
            seg.config.speed = 1;
            fire->update2D(&seg, 0);
            for (uint8_t x = 0; x < w; ++x)
                for (uint8_t y = 0; y < h; ++y) fire->_heat2D[x][y] = uint8_t(y * 16);
            fire->update2D(&seg, 0);
            (dir ? reverse : forward) = seg.pixels;
            delete fire;
        }
        for (uint8_t x = 0; x < w; ++x)
        {
            CHECK(forward[x] != 0 && forward[(h - 1) * w + x] == 0);
            CHECK(reverse[x] == 0 && reverse[(h - 1) * w + x] != 0);
            for (uint8_t y = 0; y < h; ++y) CHECK(forward[y * w + x] == reverse[(h - 1 - y) * w + x]);
        }
    }
    if (failures) { std::fprintf(stderr, "%d check(s) failed\n", failures); return 1; }
    std::printf("PASS: Fire effect pace (1D/2D) and direction from parameter 3 (1D/2D)\n");
    return 0;
}
'@

$tempRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("neopixel-fire-test-" + [Guid]::NewGuid().ToString('N').Substring(0, 8))
$exitCode = 1
try {
    New-Item -ItemType Directory -Path (Join-Path $tempRoot 'effects') -Force | Out-Null
    foreach ($file in @('FireEffect.h', 'Effect.h', 'FastLEDMath.h', 'ParameterType.h')) {
        [System.IO.File]::Copy((Join-Path $effects $file), (Join-Path $tempRoot "effects/$file"), $true)
    }
    [System.IO.File]::WriteAllText((Join-Path $tempRoot 'Arduino.h'), $arduinoStub, $utf8)
    [System.IO.File]::WriteAllText((Join-Path $tempRoot 'Segment.h'), $segmentStub, $utf8)
    [System.IO.File]::WriteAllText((Join-Path $tempRoot 'test.cpp'), $testSource, $utf8)

    $exe = Join-Path $tempRoot 'fire-test'
    if ($onWindows) { $exe += '.exe' }
    $compileArgs = @('-std=c++17', '-I', $tempRoot, (Join-Path $tempRoot 'test.cpp'), '-o', $exe)
    # ASan/UBSan where the host toolchain has them
    if (-not $onWindows) { $compileArgs = @('-fsanitize=address,undefined', '-fno-omit-frame-pointer') + $compileArgs }

    $prev = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    & $cxx @compileArgs
    $buildCode = $LASTEXITCODE
    if ($buildCode -eq 0) {
        & $exe
        $runCode = $LASTEXITCODE
    }
    $ErrorActionPreference = $prev

    if ($buildCode -ne 0) { Write-Host "FAIL: build of the Fire effect test (exit $buildCode)" -ForegroundColor Red }
    elseif ($runCode -ne 0) { Write-Host "FAIL: Fire effect checks (exit $runCode)" -ForegroundColor Red }
    else { $exitCode = 0 }
}
finally {
    if ($KeepTemp) { Write-Host "Temporary folder kept: $tempRoot" }
    else { Remove-Item -LiteralPath $tempRoot -Recurse -Force -ErrorAction SilentlyContinue }
}
exit $exitCode
