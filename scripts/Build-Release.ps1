# This script is just a template and has to be copied and modified per project
# This script should be called from .vscode/tasks.json with
#
#   scripts/Build-Release.ps1                - for Beta builds
#   scripts/Build-Release.ps1 Release        - for Release builds
#   scripts/Build-Release.ps1 -Release       - for Release builds (parameter)
#   scripts/Build-Release.ps1 SkipFirmware   - for auto-generated files + knxprod only (no firmware)
#   scripts/Build-Release.ps1 -SkipFirmware  - for auto-generated files + knxprod only (parameter)
#   scripts/Build-Release.ps1 Clean          - clean all generated files
#   scripts/Build-Release.ps1 -Clean         - clean all generated files (parameter)
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

param(
    [Parameter(Position=0)]
    [ValidateSet("Release", "SkipFirmware", "Clean", "")]
    [string]$Mode = "",

    [Parameter(Mandatory=$false)]
    [switch]$Release,

    [Parameter(Mandatory=$false)]
    [switch]$SkipFirmware,

    [Parameter(Mandatory=$false)]
    [switch]$Clean
)

# Handle both positional and named parameters
$isClean = ($Mode -eq "Clean") -or $Clean
$isRelease = ($Mode -eq "Release") -or $Release
$isSkipFirmware = ($Mode -eq "SkipFirmware") -or $SkipFirmware

# set product names, allows mapping of (devel) name in Project to a more consistent name in release
# $settings = scripts/OpenKNX-Build-Settings.ps1

# Handle Clean mode - cleanup all generated files
if ($isClean) {
    Write-Host "Cleaning all generated files..." -ForegroundColor Yellow
    Write-Host ""

    Write-Host "Cleaning effect parameters..." -ForegroundColor Cyan
    & scripts/Build-EffectParameters.ps1 -Clean -Force

    Write-Host ""
    Write-Host "Cleaning hardware config..." -ForegroundColor Cyan
    & scripts/Build-HardwareConfig.ps1 -Clean -Force

    Write-Host ""
    Write-Host "✓ All generated files cleaned!" -ForegroundColor Green
    Write-Host ""

    # Ask if user wants to build after cleaning
    Write-Host "Cleaning Done. Build now full Release with Firmware? (y/n): " -NoNewline -ForegroundColor Yellow
    $buildAfterClean = Read-Host

    if ($buildAfterClean -ne 'y' -and $buildAfterClean -ne 'Y') {
        Write-Host ""
        Write-Host "Cleanup completed. Exiting without build." -ForegroundColor Cyan
        exit 0
    }

    Write-Host ""
    Write-Host "Continuing with build..." -ForegroundColor Cyan
    Write-Host ""
    # Don't exit - continue with normal build flow
}

# Generate dynamic GPIO templates before building
# Using -UniversalBuild for release to support all hardware variants
Write-Host "Generating dynamic GPIO templates (Universal Build)..." -ForegroundColor Cyan
scripts/Build-HardwareConfig.ps1 -UniversalBuild
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

# Determine build parameter for OpenKNXproducer
# Pass "Release" if it was Release, otherwise empty (Beta)
$buildParam = if ($isRelease) { "Release" } else { "" }

# execute generic pre-build steps
../OGM-Common/scripts/setup/reusable/Build-Release-Preprocess.ps1 $buildParam
if (!$?) { exit 1 }

# OpenKNX Hardware builds (skip if SkipFirmware mode)
if (-not $isSkipFirmware) {
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
    ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_REG2_PIPICO_V1 OpenKNX-REG2-PiPico_V1 uf2
    if (!$?) { exit 1 }
    ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_REG2_PIPICO_W_V1 OpenKNX-REG2-PiPicoW_V1 uf2
    if (!$?) { exit 1 }
    ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_REG2_PIPICO2_V1 OpenKNX-REG2-PiPico2_V1 uf2
    if (!$?) { exit 1 }
    ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_REG2_PIPICO2_W_V1 OpenKNX-REG2-PiPico2W_V1 uf2
    if (!$?) { exit 1 }
    ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_REG2_ESP32S3_PICO_V1 OpenKNX-REG2-ESP32S3-Pico_V1 bin
    if (!$?) { exit 1 }
} else {
    Write-Host "Skipping firmware builds (SkipFirmware mode)" -ForegroundColor Yellow
}

# execute generic post-build steps
../OGM-Common/scripts/setup/reusable/Build-Release-Postprocess.ps1 $buildParam
if (!$?) { exit 1 }
