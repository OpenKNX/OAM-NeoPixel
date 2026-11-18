<#
.SYNOPSIS
    OpenKNX NeoPixel - Build Release Script

.DESCRIPTION
    Automated build script for OpenKNX NeoPixel firmware across multiple hardware platforms.
    Use -Help for usage information.

.PARAMETER Mode
    Positional shortcut for a switch: Release | Dev | SkipFirmware | SkipOknx | SkipTool | SkipHostCli | SkipKnxprod | Clean | Full | Mini.

.PARAMETER Release
    Build the RELEASE variant (alias -Rel). Default is Dev.

.PARAMETER Dev
    Build the Dev variant. This is the default; the flag is optional.

.PARAMETER Mini
    Build only the hardware kept on the desk.

.PARAMETER Full
    Build all hardware variants, not just the tested set.

.PARAMETER SkipFirmware
    Run the generators, the pre/post steps and the oknx PC tool build, skip the firmware compilation.
    The ETS product of the variant is kept as NeoPixel-Dev.knxprod / NeoPixel-Release.knxprod in the repository root.

.PARAMETER SkipOknx
    Skip building the PC tool oknx (alias -SkipTool, -SkipHostCli); the package takes the binaries already built, if any.

.PARAMETER SkipKnxprod
    Do not run the effect/hardware generators and OpenKNXproducer: include/knxprod.h and the ETS data stay from
    the last run. Stops when include/knxprod.h is missing or belongs to the other variant.

.PARAMETER Clean
    Remove generated files and exit. The committed effect outputs stay (scripts/Build-EffectParameters.ps1 -Clean
    empties them); the next build regenerates them when an input changed.

.PARAMETER Detailed
    Passed to Build-EffectParameters.ps1 and Build-HardwareProfile.ps1: full log instead of the summary lines.

.PARAMETER Help
    Show this help (alias -h).

.EXAMPLE
    ./Build-Release.ps1 -SkipFirmware -SkipOknx                  # Dev knxprod only -> NeoPixel-Dev.knxprod
.EXAMPLE
    ./Build-Release.ps1 -Rel -SkipFirmware -SkipOknx             # Release knxprod only -> NeoPixel-Release.knxprod
.EXAMPLE
    ./Build-Release.ps1 -SkipFirmware -SkipTool -SkipKnxprod     # repackage only, nothing is rebuilt

.NOTES
    AUTHOR : Erkan Colak

.LINK
    https://github.com/OpenKNX/OAM-NeoPixel
#>

param(
    [Parameter(Position=0)]
    [string]$Mode = "",

    [Parameter(Mandatory=$false)]
    [Alias("Rel")]
    [switch]$Release,

    [Parameter(Mandatory=$false)]
    [switch]$Dev,

    [Parameter(Mandatory=$false)]
    [switch]$SkipFirmware,

    [Parameter(Mandatory=$false)]
    [Alias("SkipTool", "SkipHostCli")]
    [switch]$SkipOknx,

    [Parameter(Mandatory=$false)]
    [switch]$SkipKnxprod,

    [Parameter(Mandatory=$false)]
    [switch]$Clean,

    [Parameter(Mandatory=$false)]
    [switch]$Full,

    [Parameter(Mandatory=$false)]
    [switch]$Mini,

    [Parameter(Mandatory=$false)]
    [switch]$Detailed,

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
    Write-Host "  .\Build-Release.ps1 [-Release|-Rel] [-Dev] [-Mini|-Full] [-SkipFirmware] [-SkipOknx|-SkipTool] [-SkipKnxprod] [-Clean] [-Detailed]"
    Write-Host ""
    Write-Host "OPTIONS:" -ForegroundColor Yellow
    Write-Host "  -Release, -Rel Create release build (default: dev)"
    Write-Host "  -Dev           Create dev build (this is the default anyway)"
    Write-Host "  -Mini          Build ONLY the hardware kept on the desk (fast turnaround)"
    Write-Host "  -Full          Build ALL hardware variants (default: tested only)"
    Write-Host "  -SkipFirmware  Generate configs/knxprod and build oknx, skip firmware compilation"
    Write-Host "                 (the ETS product stays as NeoPixel-Dev.knxprod / NeoPixel-Release.knxprod in the repo root)"
    Write-Host "  -SkipOknx      Skip building the PC tool oknx (alias -SkipTool, -SkipHostCli)"
    Write-Host "  -SkipKnxprod   Do not run the generators and OpenKNXproducer; knxprod.h and the ETS data stay from the last run"
    Write-Host "                 (stops when knxprod.h is missing or belongs to the other variant)"
    Write-Host "  -Clean         Remove generated files and exit (committed effect outputs stay)"
    Write-Host "  -Detailed      Full log of Build-EffectParameters.ps1 and Build-HardwareProfile.ps1 (default: summary lines)"
    Write-Host "  -Help, -h      Show this help"
    Write-Host ""
    Write-Host "TARGET SETS:" -ForegroundColor Yellow
    Write-Host "  -Mini          KNeoPiX V1 + V1.5: RP2350, ESP32S3, ESP32S3 TP+WiFi (6 targets)"
    Write-Host "  (default)      tested hardware (14 targets)"
    Write-Host "  -Full          tested + untested + third-party (44 targets)"
    Write-Host ""
    Write-Host "EXAMPLES:" -ForegroundColor Yellow
    Write-Host "  .\Build-Release.ps1              " -NoNewline -ForegroundColor White
    Write-Host "# Dev build (tested hardware)" -ForegroundColor DarkGray
    Write-Host "  .\Build-Release.ps1 -Release     " -NoNewline -ForegroundColor White
    Write-Host "# Release build" -ForegroundColor DarkGray
    Write-Host "  .\Build-Release.ps1 -Mini        " -NoNewline -ForegroundColor White
    Write-Host "# Only the boards on the desk" -ForegroundColor DarkGray
    Write-Host "  .\Build-Release.ps1 -Full        " -NoNewline -ForegroundColor White
    Write-Host "# Include all chip variants and third-party hardware" -ForegroundColor DarkGray
    Write-Host "  .\Build-Release.ps1 -Release -Full" -NoNewline -ForegroundColor White
    Write-Host " # Full release" -ForegroundColor DarkGray
    Write-Host "  .\Build-Release.ps1 -SkipFirmware -SkipOknx" -NoNewline -ForegroundColor White
    Write-Host " # Dev knxprod only -> NeoPixel-Dev.knxprod" -ForegroundColor DarkGray
    Write-Host "  .\Build-Release.ps1 -Rel -SkipFirmware -SkipOknx" -NoNewline -ForegroundColor White
    Write-Host " # Release knxprod only -> NeoPixel-Release.knxprod" -ForegroundColor DarkGray
    Write-Host "  .\Build-Release.ps1 -SkipFirmware -SkipTool -SkipKnxprod" -NoNewline -ForegroundColor White
    Write-Host " # repackage only" -ForegroundColor DarkGray
    Write-Host ""
}

# Show help if requested
if ($Help) {
    Show-Help
    exit 0
}

# Validate Mode parameter manually
$validModes = @("Release", "Dev", "SkipFirmware", "SkipOknx", "SkipTool", "SkipHostCli", "SkipKnxprod", "Clean", "Full", "Mini", "")
if ($Mode -and $Mode -notin $validModes) {
    Write-Host ""
    Write-Host "ERROR: Invalid mode '$Mode'. Valid values: Release, Dev, SkipFirmware, SkipOknx, SkipTool, SkipHostCli, SkipKnxprod, Clean, Full, Mini" -ForegroundColor Red
    Show-Help
    exit 1
}

# Handle both positional and named parameters
$isClean = ($Mode -eq "Clean") -or $Clean
$isRelease = ($Mode -eq "Release") -or $Release
$isSkipFirmware = ($Mode -eq "SkipFirmware") -or $SkipFirmware
$isSkipOknx = ($Mode -in @("SkipOknx", "SkipTool", "SkipHostCli")) -or $SkipOknx
$isSkipKnxprod = ($Mode -eq "SkipKnxprod") -or $SkipKnxprod
$isFull = ($Mode -eq "Full") -or $Full
$isMini = ($Mode -eq "Mini") -or $Mini

if ($isRelease -and (($Mode -eq "Dev") -or $Dev)) {
    Write-Host ""
    Write-Host "ERROR: -Release and -Dev are mutually exclusive. Pick one." -ForegroundColor Red
    exit 1
}

# Mini and Full select opposite ends of the target list - picking both is a typo,
# and silently letting one win would produce a build the caller did not ask for.
if ($isMini -and $isFull) {
    Write-Host ""
    Write-Host "ERROR: -Mini and -Full are mutually exclusive. Pick one." -ForegroundColor Red
    exit 1
}

# Set product names, allows mapping of (devel) name in project to a more consistent name in release
# $settings = scripts/OpenKNX-Build-Settings.ps1

# Handle Clean mode - cleanup all generated files
if ($isClean) {
    # Generators use repository-relative paths. Resolve them from this script even
    # when the caller launches -Clean from another working directory.
    $previousErrorActionPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Stop'
    Push-Location (Split-Path -Parent $PSScriptRoot)
    try {
        # Effect parameters: committed outputs, nothing to clean. The next build regenerates them when an input
        # changed and removes outputs it no longer writes; scripts/Build-EffectParameters.ps1 -Force rebuilds all.

        # Outputs of the hardware generator (HardwareManifest.xml <Output>/<Generate>): not committed
        # (.gitignore), the next Build-Release writes them again. HardwareManifest.slots is a source.
        Write-Host "Removing hardware generator outputs (HardwareManifest.xml)..." -ForegroundColor Cyan
        [xml]$hwManifest = [System.IO.File]::ReadAllText((Resolve-Path 'HardwareManifest.xml').Path, [System.Text.Encoding]::UTF8)
        $hwOutputs = @()
        foreach ($o in @($hwManifest.HardwareManifest.SelectNodes('Output'))) {
            foreach ($a in @($o.Attributes)) { $hwOutputs += $a.Value }
        }
        foreach ($g in @($hwManifest.HardwareManifest.SelectNodes('Generate'))) { $hwOutputs += $g.GetAttribute('output') }
        # <Conflicts output> (GPIO conflict check), <Fixes output> (settings fixed per capability), <PartRows output> (rows of a part per board),
        # <Filters output> (dropdowns filtered per board), <ValueNotices output> (notices under a selection per board)
        foreach ($n in @($hwManifest.HardwareManifest.SelectNodes('Conflicts|Fixes|PartRows|Filters|ValueNotices'))) { $hwOutputs += $n.GetAttribute('output') }
        # <Page dev> (developer view module) and the page icon the generator copies from its assets
        $hwPage = $hwManifest.HardwareManifest.SelectSingleNode('Page')
        if ($hwPage) {
            if ($hwPage.GetAttribute('dev')) { $hwOutputs += $hwPage.GetAttribute('dev') }
            $hwIcon = $hwPage.GetAttribute('icon')
            if ($hwIcon -and (Test-Path -LiteralPath "lib/OGM-HardwareConfig/generator/assets/icons/$hwIcon.png" -PathType Leaf)) { $hwOutputs += "src/Baggages/Icons/$hwIcon.png" }
        }
        foreach ($out in $hwOutputs) {
            if ($out -and (Test-Path -LiteralPath $out -PathType Leaf)) {
                Remove-Item -LiteralPath $out -Force
                Write-Host "  Removed $out" -ForegroundColor DarkGray
            }
        }
        # info images (QR code) of the hardware page: Baggages/<prefix>Info next to the share
        $hwInfoDir = "src/Baggages/$($hwManifest.HardwareManifest.GetAttribute('prefix'))Info"
        if (Test-Path -LiteralPath $hwInfoDir -PathType Container) {
            Remove-Item -LiteralPath $hwInfoDir -Recurse -Force
            Write-Host "  Removed $hwInfoDir" -ForegroundColor DarkGray
        }

        Write-Host "Cleaning generated headers and producer outputs..." -ForegroundColor Cyan
        # Only known generated outputs: preserve hand-written source and release packages.
        foreach ($pattern in @(
            'include/knxprod.h', 'include/buildtime.h', 'include/versions.h', 'include/webassets.h',
            'src/NeoPixel-*.debug.xml', 'src/*.baggages', 'src/*.knxprod', 'NeoPixel-Dev.knxprod', 'NeoPixel-Release.knxprod',
            # Older producer naming: no longer generated, left behind by earlier builds
            'src/NeoPixel.debug.xml', 'src/NeoPixel.appl.debug.xml',
            # Board table of the hardware generator before the hardware module (now include/HardwareProfileData.h)
            'src/HardwareMappingData.h'
        )) {
            foreach ($item in Get-ChildItem -Path $pattern -ErrorAction SilentlyContinue) {
                Remove-Item -LiteralPath $item.FullName -Recurse -Force
                Write-Host "  Removed $($item.Name)" -ForegroundColor DarkGray
            }
        }

        Write-Host "Normalizing line endings..." -ForegroundColor Cyan
        & ./scripts/Normalize-LineEndings.ps1
        if (-not $?) { throw "Line ending normalization failed." }
        Write-Host "All generated source files cleaned." -ForegroundColor Green
    }
    catch {
        Write-Host "Cleanup failed: $_" -ForegroundColor Red
        exit 1
    }
    finally {
        Pop-Location
        $ErrorActionPreference = $previousErrorActionPreference
    }
    exit 0
}

# ============================================================================
# BUILD TARGET CONFIGURATION - Add new hardware variants here
# ============================================================================
#
# Target format: @{ Env = "environment"; Name = "Output-Filename"; Ext = "feature-set" }
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
#
# Example: @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1"; Name = "OpenKNX-XIAO-KNeoPiX-RP2350_V1"; Ext = "rp2350-tp" }
# ============================================================================

# Mini Build Targets (the hardware physically on the desk)
# Purpose: fast turnaround while developing - one RP2350 and one ESP32S3 board per
# KNeoPiX revision (V1, V1.5), so both families stay covered. The ETS product still offers
# all hardware (every release_* env, HardwareManifest.xml).
$miniTargets = @(
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1"; Name = "OpenKNX-XIAO-KNeoPiX-RP2350_V1"; Ext = "rp2350-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V1"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V1"; Ext = "esp32-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V1_TPWIFI"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V1_TPWIFI"; Ext = "esp32-tpip" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V15"; Name = "OpenKNX-XIAO-KNeoPiX-RP2350_V15"; Ext = "rp2350-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V15"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V15"; Ext = "esp32-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V15_TPWIFI"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V15_TPWIFI"; Ext = "esp32-tpip" }
    )

# Standard Build Targets (tested hardware)
$standardTargets = @(
    # OpenKNXiao KNeoPiX
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1"; Name = "OpenKNX-XIAO-KNeoPiX-RP2350_V1"; Ext = "rp2350-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1"; Name = "OpenKNX-XIAO-KNeoPiX-RP2040_V1"; Ext = "rp2040-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V1"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V1"; Ext = "esp32-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V1_IP"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V1_IP"; Ext = "esp32-ip" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V1_TPWIFI"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V1_TPWIFI"; Ext = "esp32-tpip" }
    # OpenKNXiao KNeoPiX V1.5 (SN74LV8T245 level shifter)
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V15"; Name = "OpenKNX-XIAO-KNeoPiX-RP2350_V15"; Ext = "rp2350-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V15"; Name = "OpenKNX-XIAO-KNeoPiX-RP2040_V15"; Ext = "rp2040-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V15"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V15"; Ext = "esp32-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V15_IP"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V15_IP"; Ext = "esp32-ip" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V15_TPWIFI"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32S3_V15_TPWIFI"; Ext = "esp32-tpip" }
    # OpenKNXiao Mini
    @{ Env = "release_OKNXHW_OPENKNXIAO_RP2040_MINI_V1"; Name = "OpenKNX-XIAO-RP2040-Mini_V1"; Ext = "rp2040-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_RP2350_MINI_V1"; Name = "OpenKNX-XIAO-RP2350-Mini_V1"; Ext = "rp2350-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_ESP32S3_MINI_V1"; Name = "OpenKNX-XIAO-ESP32S3-Mini_V1"; Ext = "esp32-tp" }
    # OpenKNX UP1 Board
    @{ Env = "release_OKNXHW_UP1_GW_UART"; Name = "OpenKNX-UP1-GW-UART"; Ext = "rp2040-tp" }
    )

# Full Build Targets (additional, not yet tested hardware)
$fullTargets = @(
    # OpenKNXiao KNeoPiX - ESP32 Variants
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C3_V1"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32C3_V1"; Ext = "esp32-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C5_V1"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32C5_V1"; Ext = "esp32-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C6_V1"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32C6_V1"; Ext = "esp32-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C3_V15"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32C3_V15"; Ext = "esp32-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C5_V15"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32C5_V15"; Ext = "esp32-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C6_V15"; Name = "OpenKNX-XIAO-KNeoPiX-ESP32C6_V15"; Ext = "esp32-tp" }
    
    # OpenKNXiao Mini - ESP32 Variants
    @{ Env = "release_OKNXHW_OPENKNXIAO_ESP32C3_MINI_V1"; Name = "OpenKNX-XIAO-ESP32C3-Mini_V1"; Ext = "esp32-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_ESP32C5_MINI_V1"; Name = "OpenKNX-XIAO-ESP32C5-Mini_V1"; Ext = "esp32-tp" }
    @{ Env = "release_OKNXHW_OPENKNXIAO_ESP32C6_MINI_V1"; Name = "OpenKNX-XIAO-ESP32C6-Mini_V1"; Ext = "esp32-tp" }

    # Gledopto GL-C-309WL ESP32 WLED Digital Unterputz (Gledopto: https://gledopto.com/h-pd-124.html)
    @{ Env = "release_GLEDOPTO_ESP32_WLED_DIGITAL_UP"; Name = "Gledopto-GL-C-309WL-ESP32"; Ext = "esp32-ip" }
    @{ Env = "release_GLEDOPTO_GL_C_017WL";             Name = "Gledopto-GL-C-017WL-ESP32"; Ext = "esp32-ip" }
    @{ Env = "release_GLEDOPTO_GL_C_620WL";              Name = "Gledopto-GL-C-620WL-ESP32"; Ext = "esp32-ip" }
    
    # QuinLED Variants (QuinLED: https://www.quinled.info/)
    @{ Env = "release_QUINLED_DIG2GO";                    Name = "QuinLED-Dig2Go";                    Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_UNO_V3_WIFI";           Name = "QuinLED-Dig-Uno-V3-WiFi";           Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_UNO_V3_ETHERNET";       Name = "QuinLED-Dig-Uno-V3-Ethernet";       Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_UNO_V3_WIFI_AE_PLUS";   Name = "QuinLED-Dig-Uno-V3-WiFi-AE-Plus";   Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_QUAD_V3_WIFI";          Name = "QuinLED-Dig-Quad-V3-WiFi";          Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_QUAD_V3_ETHERNET";      Name = "QuinLED-Dig-Quad-V3-Ethernet";      Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_QUAD_V3_WIFI_AE_PLUS";  Name = "QuinLED-Dig-Quad-V3-WiFi-AE-Plus";  Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_OCTA_32_8L_WIFI";       Name = "QuinLED-Dig-Octa-32-8L-WiFi";       Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_OCTA_32_8L_ETHERNET";   Name = "QuinLED-Dig-Octa-32-8L-Ethernet";   Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_NEXT2";                 Name = "QuinLED-Dig-Next-2";                Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_UNO_V4_WIFI";           Name = "QuinLED-Dig-Uno-V4-WiFi";           Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_UNO_V4_ETHERNET";       Name = "QuinLED-Dig-Uno-V4-Ethernet";       Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_QUAD_V4_WIFI";          Name = "QuinLED-Dig-Quad-V4-WiFi";          Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_QUAD_V4_ETHERNET";      Name = "QuinLED-Dig-Quad-V4-Ethernet";      Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_OCTA_32_8L_V2_WIFI";    Name = "QuinLED-Dig-Octa-32-8L-V2-WiFi";    Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_OCTA_32_8L_V2_ETHERNET"; Name = "QuinLED-Dig-Octa-32-8L-V2-Ethernet"; Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_OCTA_32_8L_V4_WIFI";    Name = "QuinLED-Dig-Octa-32-8L-V4-WiFi";    Ext = "esp32-ip" }
    @{ Env = "release_QUINLED_DIG_OCTA_32_8L_V4_ETHERNET"; Name = "QuinLED-Dig-Octa-32-8L-V4-Ethernet"; Ext = "esp32-ip" }
    )

# Hardware identity list (ETS hardware module, device query, include/HardwareProfileData.h): every release_* env of
# platformio.ini (HardwareManifest.xml <Boards platformio="release_*"/>), whatever target set is built; a release
# firmware knows only its own hardware (DEVICE_HW_ID, DEVICE_NAME).
if ($isMini) {
    $allTargets = $miniTargets
    $targetSetName = "mini targets (hardware on the desk)"
} elseif ($isFull) {
    $allTargets = $standardTargets + $fullTargets
    $targetSetName = "standard + full targets (all hardware)"
} else {
    $allTargets = $standardTargets
    $targetSetName = "standard targets (tested hardware)"
}
Write-Host "Target set: $targetSetName - $($allTargets.Count) target(s)" -ForegroundColor Cyan

# Determine build parameter for OpenKNXproducer
# Pass "Release" or "Dev" to select the matching XML file
$buildParam = if ($isRelease) { "Release" } else { "Dev" }
$releaseName = "NeoPixel-$buildParam"

# With -SkipKnxprod the firmware is compiled against the existing include/knxprod.h while the package takes
# the ETS data of the requested variant; a header of the other variant would ship mismatching ETS data.
if ($isSkipKnxprod) {
    $headerNumber = $null
    $xmlNumber = $null
    if (Test-Path -LiteralPath "include/knxprod.h" -PathType Leaf) {
        $headerNumber = Select-String -LiteralPath "include/knxprod.h" -Pattern '^#define MAIN_ApplicationNumber\s+(\d+)' | Select-Object -First 1
    }
    if (Test-Path -LiteralPath "src/$releaseName.xml" -PathType Leaf) {
        $xmlNumber = Select-String -LiteralPath "src/$releaseName.xml" -Pattern 'ApplicationNumber="(\d+)"' | Select-Object -First 1
    }
    if ($null -eq $headerNumber -or $null -eq $xmlNumber) {
        Write-Host "ERROR: -SkipKnxprod needs include/knxprod.h and src/$releaseName.xml - run once without -SkipKnxprod" -ForegroundColor Red
        exit 1
    }
    if ($headerNumber.Matches[0].Groups[1].Value -ne $xmlNumber.Matches[0].Groups[1].Value) {
        Write-Host "ERROR: include/knxprod.h was built for the other variant (application $($headerNumber.Matches[0].Groups[1].Value), $releaseName is $($xmlNumber.Matches[0].Groups[1].Value)) - run once without -SkipKnxprod" -ForegroundColor Red
        exit 1
    }
}

# PC tool oknx, built first so a tool failure aborts before the firmware build. The post-build step
# packs the binaries of this pio matrix into Tools/oknx; -SkipFirmware still builds it.
function Find-PlatformIOExecutable {
    $command = Get-Command pio, platformio -CommandType Application -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($command) { return $command.Source }

    $coreDir = if ($env:PLATFORMIO_CORE_DIR) { $env:PLATFORMIO_CORE_DIR } else { Join-Path $HOME ".platformio" }
    $isWin = ($env:OS -eq "Windows_NT") -or ($PSVersionTable.PSVersion.Major -le 5)
    $binDir = if ($isWin) { Join-Path (Join-Path $coreDir "penv") "Scripts" } else { Join-Path (Join-Path $coreDir "penv") "bin" }
    $names = if ($isWin) { @("pio.exe", "platformio.exe") } else { @("pio", "platformio") }
    foreach ($name in $names) {
        $candidate = Join-Path $binDir $name
        if (Test-Path -LiteralPath $candidate -PathType Leaf) { return $candidate }
    }
    throw "PlatformIO not found in PATH or '$binDir'. Install PlatformIO or set PLATFORMIO_CORE_DIR to its core directory."
}

$oknxDir = "lib/OFM-FileTransferModule/oknx"
if (-not $isSkipOknx) {
    if (Test-Path (Join-Path $oknxDir "platformio.ini")) {
        Write-Host "Building the oknx PC tool matrix (pio run -> oknx, all OS/arch)..." -ForegroundColor Cyan
        $pioExecutable = Find-PlatformIOExecutable
        Write-Host "Using PlatformIO: $pioExecutable" -ForegroundColor DarkGray
        & $pioExecutable run -d $oknxDir
        if ($LASTEXITCODE -ne 0) { Write-Host "oknx build failed" -ForegroundColor Red; exit 1 }
    } else {
        Write-Host "oknx project not found ($oknxDir) - the release would ship without the PC tool" -ForegroundColor Red
        Write-Host "  check out OFM-FileTransferModule, or pass -SkipOknx to leave it out on purpose" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "Skipping PC tool build (SkipOknx) - the release packs the oknx binaries already built, if any" -ForegroundColor Yellow
}

# The generators write the XML that OpenKNXproducer turns into knxprod.h; with -SkipKnxprod both stay
# from the last run, so header and ETS data keep matching.
if (-not $isSkipKnxprod) {
    # Effect parameters first: they fill the effect blocks of src/NeoPixel.script.src.js, which the
    # hardware generator then assembles into src/NeoPixel.script.js
    Write-Host "Generating effect parameters..." -ForegroundColor Cyan
    scripts/Build-EffectParameters.ps1 -SkipOpenKNXproducer -Detailed:$Detailed
    if (!$?) {
        Write-Host "Effect parameter generation failed!" -ForegroundColor Red
        exit 1
    }

    # Hardware selection, ports and board tables: OGM-HardwareConfig generator, manifest HardwareManifest.xml
    Write-Host "Generating hardware files (HardwareManifest.xml)..." -ForegroundColor Cyan
    $global:LASTEXITCODE = 0
    & lib/OGM-HardwareConfig/generator/Build-HardwareProfile.ps1 -Manifest HardwareManifest.xml -Detailed:$Detailed
    if ($global:LASTEXITCODE -ne 0) {
        Write-Host "Hardware generation failed!" -ForegroundColor Red
        exit 1
    }

    # The generators splice blocks in with mixed newlines; without this the repository
    # shows the same files as modified after every run (see Normalize-LineEndings.ps1).
    Write-Host "Normalizing line endings..." -ForegroundColor Cyan
    scripts/Normalize-LineEndings.ps1
} else {
    Write-Host "Skipping effect/hardware generators (SkipKnxprod) - their outputs stay from the last run" -ForegroundColor Yellow
}

# Execute generic pre-build steps. The shared preprocess passes its arguments on positionally, so the
# skip travels in the environment; cleared first and in finally so it never leaks into a later build.
Remove-Item Env:OPENKNX_SKIP_KNXPROD -ErrorAction SilentlyContinue
if ($isSkipKnxprod) { $env:OPENKNX_SKIP_KNXPROD = "1" }
try {
    lib/OGM-Common/scripts/setup/reusable/Build-Release-Preprocess.ps1 $buildParam
    $preprocessOk = $?
}
finally {
    Remove-Item Env:OPENKNX_SKIP_KNXPROD -ErrorAction SilentlyContinue
}
if (!$preprocessOk) { exit 1 }

# The post-build step deletes release/<releaseName>.knxprod; keep a copy for a direct ETS import.
if (-not $isSkipKnxprod) {
    $knxprodSource = "release/$releaseName.knxprod"
    if (Test-Path -LiteralPath $knxprodSource -PathType Leaf) {
        Copy-Item -LiteralPath $knxprodSource -Destination "$releaseName.knxprod" -Force
        Write-Host "ETS product for import: $releaseName.knxprod" -ForegroundColor Green
    } else {
        Write-Host "OpenKNXproducer left no $knxprodSource - no ETS product to import" -ForegroundColor Yellow
    }
}

# ============================================================================
# BUILD EXECUTION
# ============================================================================

# OpenKNX Hardware builds (skip if SkipFirmware mode)
if (-not $isSkipFirmware) {
    Write-Host "Building firmware for hardware variants..." -ForegroundColor Cyan
    Write-Host "Building $targetSetName..." -ForegroundColor Cyan

    # $allTargets was selected above
    foreach ($target in $allTargets) {
        lib/OGM-Common/scripts/setup/reusable/Build-Step.ps1 $target.Env $target.Name $target.Ext
        if (!$?) { exit 1 }
    }

    if (-not ($isMini -or $isFull)) {
        Write-Host "Skipping full targets (use -Full to build all, -Mini for the desk hardware)" -ForegroundColor Yellow
    }
} else {
    Write-Host "Skipping firmware builds (SkipFirmware mode)" -ForegroundColor Yellow
}

# Execute generic post-build steps
lib/OGM-Common/scripts/setup/reusable/Build-Release-Postprocess.ps1 $buildParam
if (!$?) { exit 1 }
