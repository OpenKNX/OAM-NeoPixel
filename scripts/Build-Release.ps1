# This script is just a template and has to be copied and modified per project
# This script should be called from .vscode/tasks.json with
#
#   scripts/Build-Release.ps1            - for Beta builds
#   scripts/Build-Release.ps1 Release    - for Release builds
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
# }

# set product names, allows mapping of (devel) name in Project to a more consistent name in release
# $settings = scripts/OpenKNX-Build-Settings.ps1

# Generate effect parameters before building
Write-Host "Generating effect parameters..." -ForegroundColor Cyan
scripts/Build-EffectParameters.ps1
if (!$?) { 
    Write-Host "Effect parameter generation failed!" -ForegroundColor Red
    exit 1 
}

# execute generic pre-build steps
../OGM-Common/scripts/setup/reusable/Build-Release-Preprocess.ps1 $args[0]
if (!$?) { exit 1 }

# build firmware based on generated headerfile for ESP32
#../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_ESP32 firmware-ESP32 bin
#if (!$?) { exit 1 }

../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1 OpenKNX-XIAO-KNeoPiX-RP2350_V1 uf2
if (!$?) { exit 1 }

#../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1 OpenKNX-XIAO-KNeoPiX-RP2040_V1 uf2
#if (!$?) { exit 1 }
#
#../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_RP2040_MINI_V1 OpenKNX-XIAO-RP2040-Mini_V1 uf2
#if (!$?) { exit 1 }
#
#../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_RP2350_MINI_V1 OpenKNX-XIAO-RP2350-Mini_V1 uf2
#if (!$?) { exit 1 }
#
#../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OKNXHW_OPENKNXIAO_ESP32S3_MINI_V1 OpenKNX-XIAO-ESP32S3-Mini_V1 bin
#if (!$?) { exit 1 } 

# execute generic post-build steps
../OGM-Common/scripts/setup/reusable/Build-Release-Postprocess.ps1 $args[0]
if (!$?) { exit 1 }
