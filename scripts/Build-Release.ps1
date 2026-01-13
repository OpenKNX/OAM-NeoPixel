# This script is just a template and has to be copied and modified per project
# This script should be called from .vscode/tasks.json with
#
#   scripts/Build-Release.ps1                - for Beta builds
#   scripts/Build-Release.ps1 Release        - for Release builds
#   scripts/Build-Release.ps1 SkipFirmware   - for auto-generated files + knxprod only (no firmware)
#
# {
#     "label": "Build-Release",
#     "type": "shell",
#     "command": "scripts/Build-Release.ps1 Release",
#     "args": [],
#     "problemMatcher": [],
#     "group": "test"
# },
# {
#     "label": "Build-Beta",
#     "type": "shell",
#     "command": "scripts/Build-Release.ps1 ",
#     "args": [],
#     "problemMatcher": [],
#     "group": "test"
# },
# {
#     "label": "Build-SkipFirmware",
#     "type": "shell",
#     "command": "scripts/Build-Release.ps1 SkipFirmware",
#     "args": [],
#     "problemMatcher": [],
#     "group": "test"
# }

# set product names, allows mapping of (devel) name in Project to a more consistent name in release
# $settings = scripts/OpenKNX-Build-Settings.ps1

# Generate dynamic GPIO templates before building
# Using -UniversalBuild for release to support all hardware variants
Write-Host "Generating dynamic GPIO templates (Universal Build)..." -ForegroundColor Cyan
scripts/Build-NeoPixelStrips.ps1 -UniversalBuild
if (!$?) { 
    Write-Host "GPIO template generation failed!" -ForegroundColor Red
    exit 1 
}

# Generate effect parameters before building
Write-Host "Generating effect parameters..." -ForegroundColor Cyan
scripts/Build-EffectParameters.ps1
if (!$?) { 
    Write-Host "Effect parameter generation failed!" -ForegroundColor Red
    exit 1 
}

# Determine if we should skip firmware builds
$skipFirmware = ($args[0] -eq "SkipFirmware")

# Filter parameter for pre/post-build: don't pass "SkipFirmware" to OpenKNXproducer
# Pass "Release" if it was Release, otherwise empty (Beta)
$buildParam = if ($args[0] -eq "Release") { "Release" } else { "" }

# execute generic pre-build steps
../OGM-Common/scripts/setup/reusable/Build-Release-Preprocess.ps1 $buildParam
if (!$?) { exit 1 }

# OpenKNX Hardware builds (skip if SkipFirmware mode)
if (-not $skipFirmware) {
    Write-Host "Building firmware for all hardware variants..." -ForegroundColor Cyan
    
    # OpenKNXiao KNeoPiX
    ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1 OpenKNX-XIAO-KNeoPiX-RP2350_V1 uf2
    if (!$?) { exit 1 }
    ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1 OpenKNX-XIAO-KNeoPiX-RP2040_V1 uf2
    if (!$?) { exit 1 }
    ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C3_V1 OpenKNX-XIAO-KNeoPiX-ESP32C3_V1 bin
    if (!$?) { exit 1 }
    ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C6_V1 OpenKNX-XIAO-KNeoPiX-ESP32C6_V1 bin
    if (!$?) { exit 1 }
    ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V1 OpenKNX-XIAO-KNeoPiX-ESP32S3_V1 bin
    if (!$?) { exit 1 }
    #
    # OpenKNXiao Mini
    ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_RP2040_MINI_V1 OpenKNX-XIAO-RP2040-Mini_V1 uf2
    if (!$?) { exit 1 }
    #../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_RP2350_MINI_V1 OpenKNX-XIAO-RP2350-Mini_V1 uf2
    #if (!$?) { exit 1 }

    # OpenKNX REG2
    # ToDo: Add REG2 builds, after defining hardware IDs and platformio environments
} else {
    Write-Host "Skipping firmware builds (SkipFirmware mode)" -ForegroundColor Yellow
}

# execute generic post-build steps
../OGM-Common/scripts/setup/reusable/Build-Release-Postprocess.ps1 $buildParam
if (!$?) { exit 1 }
