<#
.SYNOPSIS
    OpenKNX NeoPixel - Build Release Script

.DESCRIPTION
    Automated build script for OpenKNX NeoPixel firmware across multiple hardware platforms.
    Use -Help for usage information.

.LINK
    https://github.com/OpenKNX/OAM-NeoPixel
#>

param(
    [Parameter(Position=0)]
    [string]$Mode = "",

    [Parameter(Mandatory=$false)]
    [switch]$Release,

    [Parameter(Mandatory=$false)]
    [switch]$SkipFirmware,

    [Parameter(Mandatory=$false)]
    [switch]$Clean,

    [Parameter(Mandatory=$false)]
    [switch]$Full,

    [Parameter(Mandatory=$false)]
    [Alias("h")]
    [switch]$Help
)

# OpenKNX Logo function
function OpenKNX_ShowLogo($AddCustomText = $null) {
    Write-Host ""
    Write-Host "Open " -NoNewline
    Write-Host "$( [char]::ConvertFromUtf32(0x25A0) )" -ForegroundColor Green
    $unicodeString = "$( [char]::ConvertFromUtf32(0x252C) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2534) ) "

    if ($AddCustomText) { 
        Write-Host "$($unicodeString) $($AddCustomText)"  -ForegroundColor Green
    }
    else {
        Write-Host "$($unicodeString)"  -ForegroundColor Green
    }

    Write-Host "$( [char]::ConvertFromUtf32(0x25A0) )" -NoNewline -ForegroundColor Green
    Write-Host " KNX"
    Write-Host ""
}

# Function to display help
function Show-Help {
    OpenKNX_ShowLogo "Build Release Script"
    Write-Host "USAGE:" -ForegroundColor Yellow
    Write-Host "  .\Build-Release.ps1 [-Release] [-Full] [-SkipFirmware] [-Clean]"
    Write-Host ""
    Write-Host "OPTIONS:" -ForegroundColor Yellow
    Write-Host "  -Release       Create release build (default: dev)"
    Write-Host "  -Full          Build ALL hardware variants (default: tested only)"
    Write-Host "  -SkipFirmware  Generate configs only, skip firmware compilation"
    Write-Host "  -Clean         Remove generated files and exit"
    Write-Host "  -Help, -h      Show this help"
    Write-Host ""
    Write-Host "EXAMPLES:" -ForegroundColor Yellow
    Write-Host "  .\Build-Release.ps1              " -NoNewline -ForegroundColor White
    Write-Host "# Dev build (tested hardware)" -ForegroundColor DarkGray
    Write-Host "  .\Build-Release.ps1 -Release     " -NoNewline -ForegroundColor White
    Write-Host "# Release build" -ForegroundColor DarkGray
    Write-Host "  .\Build-Release.ps1 -Full        " -NoNewline -ForegroundColor White
    Write-Host "# Include untested REG2 boards" -ForegroundColor DarkGray
    Write-Host "  .\Build-Release.ps1 -Release -Full" -NoNewline -ForegroundColor White
    Write-Host " # Full release" -ForegroundColor DarkGray
    Write-Host ""
}

# Show help if requested
if ($Help) {
    Show-Help
    exit 0
}

# Validate Mode parameter manually
$validModes = @("Release", "SkipFirmware", "Clean", "Full", "")
if ($Mode -and $Mode -notin $validModes) {
    Write-Host ""
    Write-Host "ERROR: Invalid mode '$Mode'. Valid values: Release, SkipFirmware, Clean, Full" -ForegroundColor Red
    Show-Help
    exit 1
}

# Handle both positional and named parameters
$isClean = ($Mode -eq "Clean") -or $Clean
$isRelease = ($Mode -eq "Release") -or $Release
$isSkipFirmware = ($Mode -eq "SkipFirmware") -or $SkipFirmware
$isFull = ($Mode -eq "Full") -or $Full

# Set product names, allows mapping of (devel) name in project to a more consistent name in release
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
    Write-Host "Cleaning generated header files..." -ForegroundColor Cyan
    $knxprodHeader = "include/knxprod.h"
    if (Test-Path $knxprodHeader) {
        Remove-Item $knxprodHeader -Force
        Write-Host "  ✓ Removed $knxprodHeader" -ForegroundColor Green
    } else {
        Write-Host "  • $knxprodHeader not found (already clean)" -ForegroundColor DarkGray
    }

    Write-Host ""
    Write-Host "✓ All generated files cleaned!" -ForegroundColor Green
    Write-Host ""

    exit 0
    ## Ask if user wants to build after cleaning
    #Write-Host "Cleaning done. Build full release with firmware now? (y/n): " -NoNewline -ForegroundColor Yellow
    #$buildAfterClean = Read-Host
    #
    #if ($buildAfterClean -ne 'y' -and $buildAfterClean -ne 'Y') {
    #    Write-Host ""
    #    Write-Host "Cleanup completed. Exiting without build." -ForegroundColor Cyan
    #    exit 0
    #}

    Write-Host ""
    Write-Host "Continuing with build..." -ForegroundColor Cyan
    Write-Host ""
        # Don't exit - continue with normal build flow
}

# ============================================================================
# BUILD TARGET CONFIGURATION - Add new hardware variants here
# ============================================================================
#
# Target format: @{ Env = "environment"; Name = "Output-Filename"; Ext = "feature-set"; HwSection = "section-name" }
#   Env       - PlatformIO environment name from platformio.ini (e.g., release_OKNXHW_...)
#   Name      - Output firmware filename without extension (e.g., OpenKNX-XIAO-KNeoPiX-RP2350_V1)
#   Ext       - Build-Step featureSet (historical field name; not a literal file extension)
#               featureSet replaces the old binaryFormat setting in a compatible way.
#               It is interpreted as an enum with some deprecated values for compatibility:
#                       bin (deprecated)   - old SAMD processor (deprecated)
#                       uf2 (deprecated)   - RP2040 without OTA
#                       esp32 (deprecated) - ESP32 with OTA
#                       esp32-ip (new)     - ESP32 with OTA
#                       esp32-tp (new)     - ESP32 with KNX
#                       esp32-tpip (new)   - ESP32 with KNX and OTA
#                       esp32-iptp (new)   - ESP32 with KNX and OTA
#                       rp2040-ip (new)    - RP2040 with OTA
#                       rp2040-tp (new)    - RP2040 with KNX
#                       rp2040-tpip (new)  - RP2040 with KNX and OTA
#                       rp2040-iptp (new)  - RP2040 with KNX and OTA
#                       rp2350-ip (new)    - RP2350 with OTA
#                       rp2350-tp (new)    - RP2350 with KNX
#                       rp2350-tpip (new)  - RP2350 with KNX and OTA
#                       rp2350-iptp (new)  - RP2350 with KNX and OTA
#               Inherent logic:
#                 a device with OTA does not need a KNX upload script
#                 ESP is always IP and OTA is always possible
#                 RP2040/RP2350 needs to distinguish between IP and TP variants
#   HwSection - EXACT section name from platformio.hardware.ini (e.g., neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2350_V1)
#               This section name is used by Build-HardwareConfig.ps1 to extract build_flags for the C preprocessor
#
# Example: @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1"; Name = "OpenKNX-XIAO-KNeoPiX-RP2350_V1"; Ext = "rp2350-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2350_V1" }
# ============================================================================

# Standard Build Targets (tested hardware)
$standardTargets = @(
    # OpenKNXiao KNeoPiX
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1"; Name = "OpenKNX-XIAO-KNeoPiX-RP2350_V1"; Ext = "rp2350-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2350_V1" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1"; Name = "OpenKNX-XIAO-KNeoPiX-RP2040_V1"; Ext = "rp2040-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V1"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V1"; Ext = "esp32-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_ESP32S3_V1" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V1_IP"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V1_IP"; Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_ESP32S3_V1" }
    # OpenKNXiao Mini
    @{ Env = "release_OKNXHW_OPENKNXIAO_RP2040_MINI_V1"; Name = "OpenKNX-XIAO-RP2040-Mini_V1"; Ext = "rp2040-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_RP2040_MINI_V1" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_RP2350_MINI_V1"; Name = "OpenKNX-XIAO-RP2350-Mini_V1"; Ext = "rp2350-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_RP2350_MINI_V1" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_ESP32S3_MINI_V1"; Name = "OpenKNX-XIAO-ESP32S3-Mini_V1"; Ext = "esp32-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_ESP32S3_MINI_V1" }
    # OpenKNX REG2 - PiPico Variants
    @{ Env = "release_OKNXHW_REG2_PIPICO_V1"; Name = "OpenKNX-REG2-PiPico_V1"; Ext = "rp2040-tp"; HwSection = "neopixel_oknxhw_REG2_PIPICO_V1" }
    @{ Env = "release_OKNXHW_REG2_PIPICO_W_V1"; Name = "OpenKNX-REG2-PiPicoW_V1"; Ext = "rp2040-tp"; HwSection = "neopixel_oknxhw_REG2_PIPICO_W_V1" }
    @{ Env = "release_OKNXHW_REG2_PIPICO2_V1"; Name = "OpenKNX-REG2-PiPico2_V1"; Ext = "rp2350-tp"; HwSection = "neopixel_oknxhw_REG2_PIPICO2_V1" }
    @{ Env = "release_OKNXHW_REG2_PIPICO2_W_V1"; Name = "OpenKNX-REG2-PiPico2W_V1"; Ext = "rp2350-tp"; HwSection = "neopixel_oknxhw_REG2_PIPICO2_W_V1" }
    # OpenKNX PiPico BCU Connector
    @{ Env = "release_DEVICE_PIPICO_BCU_CONNECTOR"; Name = "OpenKNX-PiPico-BCU-Connector"; Ext = "rp2040-tp"; HwSection = "neopixel_oknxhw_DEVICE_PIPICO_BCU_CONNECTOR" }
    @{ Env = "release_DEVICE_PIPICO2_BCU_CONNECTOR"; Name = "OpenKNX-PiPico2-BCU-Connector"; Ext = "rp2350-tp"; HwSection = "neopixel_oknxhw_DEVICE_PIPICO2_BCU_CONNECTOR" }
    # OpenKNX UP1 Board
    @{ Env = "release_OKNXHW_UP1_GW_UART"; Name = "OpenKNX-UP1-GW-UART"; Ext = "rp2040-tp"; HwSection = "neopixel_oknxhw_UP1_GW_UART" }
    # Gledopto GL-C-309WL ESP32 WLED Digital Unterputz
    @{ Env = "release_GLEDOPTO_ESP32_WLED_DIGITAL_UP"; Name = "Gledopto-GL-C-309WL-ESP32"; Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_GLEDOPTO_ESP32_WLED_DIGITAL_UP" }
    # QuinLED Variants
    @{ Env = "release_QUINLED_DIG2GO";                    Name = "QuinLED-Dig2Go";                    Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_QUINLED_DIG2GO" }
    @{ Env = "release_QUINLED_DIG_UNO_V3_WIFI";           Name = "QuinLED-Dig-Uno-V3-WiFi";           Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_QUINLED_DIG_UNO_V3_WIFI" }
    @{ Env = "release_QUINLED_DIG_UNO_V3_ETHERNET";       Name = "QuinLED-Dig-Uno-V3-Ethernet";       Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_QUINLED_DIG_UNO_V3_ETHERNET" }
    @{ Env = "release_QUINLED_DIG_UNO_V3_WIFI_AE_PLUS";   Name = "QuinLED-Dig-Uno-V3-WiFi-AE-Plus";   Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_QUINLED_DIG_UNO_V3_WIFI_AE_PLUS" }
    @{ Env = "release_QUINLED_DIG_QUAD_V3_WIFI";          Name = "QuinLED-Dig-Quad-V3-WiFi";          Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_QUINLED_DIG_QUAD_V3_WIFI" }
    @{ Env = "release_QUINLED_DIG_QUAD_V3_ETHERNET";      Name = "QuinLED-Dig-Quad-V3-Ethernet";      Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_QUINLED_DIG_QUAD_V3_ETHERNET" }
    @{ Env = "release_QUINLED_DIG_QUAD_V3_WIFI_AE_PLUS";  Name = "QuinLED-Dig-Quad-V3-WiFi-AE-Plus";  Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_QUINLED_DIG_QUAD_V3_WIFI_AE_PLUS" }
    @{ Env = "release_QUINLED_DIG_OCTA_32_8L_WIFI";       Name = "QuinLED-Dig-Octa-32-8L-WiFi";       Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_QUINLED_DIG_OCTA_32_8L_WIFI" }
    @{ Env = "release_QUINLED_DIG_OCTA_32_8L_ETHERNET";   Name = "QuinLED-Dig-Octa-32-8L-Ethernet";   Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_QUINLED_DIG_OCTA_32_8L_ETHERNET" }
    @{ Env = "release_QUINLED_DIG_NEXT2";                 Name = "QuinLED-Dig-Next-2";                Ext = "esp32-ip"; HwSection = "neopixel_oknxhw_QUINLED_DIG_NEXT2" }
    )

# Full Build Targets (additional, not yet tested hardware)
$fullTargets = @(
    # OpenKNXiao KNeoPiX - ESP32 Variants
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C3_V1"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32C3_V1"; Ext = "esp32-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_ESP32C3_V1" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C5_V1"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32C5_V1"; Ext = "esp32-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_ESP32C5_V1" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C6_V1"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32C6_V1"; Ext = "esp32-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_ESP32C6_V1" }
    
    # OpenKNXiao Mini - ESP32 Variants
    @{ Env = "release_OKNXHW_OPENKNXIAO_ESP32C3_MINI_V1"; Name = "OpenKNX-XIAO-ESP32C3-Mini_V1"; Ext = "esp32-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_ESP32C3_MINI_V1" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_ESP32C5_MINI_V1"; Name = "OpenKNX-XIAO-ESP32C5-Mini_V1"; Ext = "esp32-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_ESP32C5_MINI_V1" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_ESP32C6_MINI_V1"; Name = "OpenKNX-XIAO-ESP32C6-Mini_V1"; Ext = "esp32-tp"; HwSection = "neopixel_oknxhw_OPENKNXIAO_ESP32C6_MINI_V1" }
   
    @{ Env = "release_OKNXHW_REG2_ESP32S3_PICO_V1"; Name = "OpenKNX-REG2-ESP32S3-Pico_V1"; Ext = "esp32-tp"; HwSection = "neopixel_oknxhw_REG2_ESP32S3_V1" })

# Generate dynamic GPIO templates before building
# Collect exact hardware section names from all targets that will be built
$allTargets = $standardTargets
if ($isFull) {
    $allTargets = $standardTargets + $fullTargets
}

# Extract unique hardware section names from targets
$hardwareFilter = $allTargets | ForEach-Object { $_.HwSection } | Select-Object -Unique

Write-Host "Generating GPIO templates for selected hardware..." -ForegroundColor Cyan
Write-Host "  Section filter: $($hardwareFilter -join ', ')" -ForegroundColor DarkGray
scripts/Build-HardwareConfig.ps1 -UniversalBuild -EnvironmentFilter $hardwareFilter
if (!$?) {
    Write-Host "GPIO template generation failed!" -ForegroundColor Red
    exit 1
}

# Generate effect parameters before building
Write-Host "Generating effect parameters..." -ForegroundColor Cyan
scripts/Build-EffectParameters.ps1 -SkipOpenKNXproducer
if (!$?) {
    Write-Host "Effect parameter generation failed!" -ForegroundColor Red
    exit 1
}

# Determine build parameter for OpenKNXproducer
# Pass "Release" or "Dev" to select the matching XML file
$buildParam = if ($isRelease) { "Release" } else { "Dev" }

# Execute generic pre-build steps
../OGM-Common/scripts/setup/reusable/Build-Release-Preprocess.ps1 $buildParam
if (!$?) { exit 1 }

# ============================================================================
# BUILD EXECUTION
# ============================================================================

# OpenKNX Hardware builds (skip if SkipFirmware mode)
if (-not $isSkipFirmware) {
    Write-Host "Building firmware for hardware variants..." -ForegroundColor Cyan
    
    # Build standard targets
    Write-Host "Building standard targets (tested hardware)..." -ForegroundColor Cyan
    foreach ($target in $standardTargets) {
        ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 $target.Env $target.Name $target.Ext
        if (!$?) { exit 1 }
    }
    
    # Build full targets if requested
    if ($isFull) {
        Write-Host "Building full targets (untested REG2 hardware)..." -ForegroundColor Cyan
        foreach ($target in $fullTargets) {
            ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 $target.Env $target.Name $target.Ext
            if (!$?) { exit 1 }
        }
    } else {
        Write-Host "Skipping full targets (use -Full to build all)" -ForegroundColor Yellow
    }
} else {
    Write-Host "Skipping firmware builds (SkipFirmware mode)" -ForegroundColor Yellow
}

# Execute generic post-build steps
../OGM-Common/scripts/setup/reusable/Build-Release-Postprocess.ps1 $buildParam
if (!$?) { exit 1 }
