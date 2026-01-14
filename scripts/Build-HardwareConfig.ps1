#!/usr/bin/env pwsh
<#
Open ■
┬────┴  Build-HardwareConfig
■ KNX   2024 OpenKNX - Erkan Çolak

FILEPATH: scripts/Build-HardwareConfig.ps1

.SYNOPSIS
    Generic GPIO template generator - builds ETS XML with dynamic GPIO dropdowns based on hardware

.DESCRIPTION
    This script extracts hardware-specific GPIO configurations via C++ preprocessor
    and generates ETS XML templates with dynamic GPIO options.
    
    Reusable for ANY feature that needs hardware-dependent GPIO selection:
    - NeoPixel LED strips
    - I2C bus selection
    - UART port selection
    - SPI device selection
    - etc.

.PARAMETER FeatureName
    Name of the feature (e.g., "NeoPixel", "I2C", "UART")
    Used for: Display messages and default paths

.PARAMETER DefinesPrefix
    Prefix for hardware defines (e.g., "NEOPIXEL_HW", "I2C_HW", "UART_HW")
    Format: {PREFIX}_GPIO_PORTS, {PREFIX}_PORT_COUNT, {PREFIX}_PORT_1_GPIO, etc.

.PARAMETER TemplateFile
    Path to XML template file with marker blocks (e.g., "src/NeoPixel.Strip.templ.xml")
    The file will be modified in-place between marker comments

.PARAMETER ExtractorTool
    Path to C++ extractor tool (default: "scripts/extract_gpio_config.cpp")

.PARAMETER MaxPorts
    Maximum number of GPIO ports to check (default: 7)

.EXAMPLE
    # NeoPixel LED strips
    ./Build-HardwareConfig.ps1 -FeatureName "NeoPixel" -DefinesPrefix "NEOPIXEL_HW" `
        -TemplateFile "src/NeoPixel.Strip.templ.xml"

.EXAMPLE
    # I2C bus selection
    ./Build-HardwareConfig.ps1 -FeatureName "I2C" -DefinesPrefix "I2C_HW" `
        -TemplateFile "src/I2C.Config.templ.xml" -MaxPorts 4

.NOTES
    Author: OpenKNX Project - Erkan Çolak
    Requires: C++ compiler (g++/arm-none-eabi-g++) in PATH
    
    This script is part of the OpenKNX project automation framework.
    It dynamically generates hardware-specific GPIO configurations for ETS applications.
#>

#===================================================================
# Parameters
#===================================================================
param(
  [Parameter(Mandatory = $false)]
  [string]$FeatureName = "NeoPixel",
    
  [Parameter(Mandatory = $false)]
  [string]$DefinesPrefix = "NEOPIXEL_HW",
    
  [Parameter(Mandatory = $false)]
  [string]$TemplateFile = "src/NeoPixel.Strip.templ.xml",
    
  [Parameter(Mandatory = $false)]
  [string]$OutputFile = "",
    
  [Parameter(Mandatory = $false)]
  [string]$ExtractorTool = "scripts/extract_gpio_config.cpp",
    
  [Parameter(Mandatory = $false)]
  [int]$MaxPorts = 7,
    
  [Parameter(Mandatory = $false)]
  [string]$ShareXml = "",
    
  [Parameter(Mandatory = $false)]
  [string]$JavaScriptFile = "",
    
  [Parameter(Mandatory = $false)]
  [string]$HardwareMappingHeader = "src/HardwareMappingData.h",
    
  [Parameter(Mandatory = $false)]
  [switch]$TestMode,
    
  [Parameter(Mandatory = $false)]
  [switch]$Clean,
    
  [Parameter(Mandatory = $false)]
  [switch]$Force,
    
  [Parameter(Mandatory = $false)]
  [switch]$DryRun,
    
  [Parameter(Mandatory = $false)]
  [switch]$VerboseMode,
    
  [Parameter(Mandatory = $false)]
  [switch]$UniversalBuild,
    
  [Parameter(Mandatory = $false)]
  [string]$HardwareConfigSection = ""
)

$ErrorActionPreference = "Stop"
$SCRIPT_VERSION = "0.1"

# Set defaults if not provided
if ([string]::IsNullOrEmpty($ShareXml)) {
  $ShareXml = "src/$FeatureName.share.xml"
}
if ([string]::IsNullOrEmpty($JavaScriptFile)) {
  $JavaScriptFile = "src/$FeatureName.script.js"
}
if ([string]::IsNullOrEmpty($OutputFile)) {
  $OutputFile = $TemplateFile
}

# ====================================================================
# Configuration
# ====================================================================
$script:Config = @{
  # Default paths (relative to repo root)
  ExtractorToolDefault = "scripts/extract_gpio_config.cpp"
  ExtractorExeName     = "extract_gpio_config"
    
  # Temporary files
  TempDir              = "scripts"
    
  # Markers
  Markers              = @{
    # Template markers (NO leading whitespace in definition)
    GPIOStart                 = "<!-- BEGIN AUTO-GENERATED: GPIO Options -->"
    GPIOEnd                   = "<!-- END AUTO-GENERATED: GPIO Options -->"
    ParamDefStart             = "<!-- BEGIN AUTO-GENERATED: GPIO Port Parameters -->"
    ParamDefEnd               = "<!-- END AUTO-GENERATED: GPIO Port Parameters -->"
    ConflictParamStart        = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict Result Parameters -->"
    ConflictParamEnd          = "<!-- END AUTO-GENERATED: GPIO Conflict Result Parameters -->"
    ConflictCalcStart         = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict Check ParameterCalculations -->"
    ConflictCalcEnd           = "<!-- END AUTO-GENERATED: GPIO Conflict Check ParameterCalculations -->"
    ConflictUIStart           = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict Status Display -->"
    ConflictUIEnd             = "<!-- END AUTO-GENERATED: GPIO Conflict Status Display -->"
    # share.xml markers (NO leading whitespace in definition)
    ParamTypeStart            = "<!-- BEGIN AUTO-GENERATED: GPIO Port Selection Types -->"
    ParamTypeEnd              = "<!-- END AUTO-GENERATED: GPIO Port Selection Types -->"
    ClockPortParamsStart      = "<!-- BEGIN AUTO-GENERATED: GPIO Clock Port Parameters -->"
    ClockPortParamsEnd        = "<!-- END AUTO-GENERATED: GPIO Clock Port Parameters -->"
    ClockConflictParamsStart  = "<!-- BEGIN AUTO-GENERATED: GPIO Clock Conflict Parameters -->"
    ClockConflictParamsEnd    = "<!-- END AUTO-GENERATED: GPIO Clock Conflict Parameters -->"
    ConflictParamsStart       = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict Parameters -->"
    ConflictParamsEnd         = "<!-- END AUTO-GENERATED: GPIO Conflict Parameters -->"
    ConflictParamRefsStart    = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict ParameterRefs -->"
    ConflictParamRefsEnd      = "<!-- END AUTO-GENERATED: GPIO Conflict ParameterRefs -->"
    ShareConflictCalcStart    = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict Check ParameterCalculation -->"
    ShareConflictCalcEnd      = "<!-- END AUTO-GENERATED: GPIO Conflict Check ParameterCalculation -->"
    HWChangeResetCalcStart    = "<!-- BEGIN AUTO-GENERATED: GPIO Hardware Change Reset ParameterCalculation -->"
    HWChangeResetCalcEnd      = "<!-- END AUTO-GENERATED: GPIO Hardware Change Reset ParameterCalculation -->"
    HardwareParamStart        = "<!-- BEGIN AUTO-GENERATED: Hardware Selection Parameter -->"
    HardwareParamEnd          = "<!-- END AUTO-GENERATED: Hardware Selection Parameter -->"
    HardwareParamRefStart     = "<!-- BEGIN AUTO-GENERATED: Hardware Selection ParameterRef -->"
    HardwareParamRefEnd       = "<!-- END AUTO-GENERATED: Hardware Selection ParameterRef -->"
    HardwareParamRefRefStart  = "<!-- BEGIN AUTO-GENERATED: Hardware Selection ParameterRefRef -->"
    HardwareParamRefRefEnd    = "<!-- END AUTO-GENERATED: Hardware Selection ParameterRefRef -->"
    # Template markers for GPIO Port Selection Parameter
    GPIOPortParamStart        = "<!-- BEGIN AUTO-GENERATED: GPIO Port Selection Parameter -->"
    GPIOPortParamEnd          = "<!-- END AUTO-GENERATED: GPIO Port Selection Parameter -->"
    GPIOClockParamStart       = "<!-- BEGIN AUTO-GENERATED: GPIO Clock Selection Parameter -->"
    GPIOClockParamEnd         = "<!-- END AUTO-GENERATED: GPIO Clock Selection Parameter -->"
    GPIOPortParamRefStart     = "<!-- BEGIN AUTO-GENERATED: GPIO Port Selection ParameterRef -->"
    GPIOPortParamRefEnd       = "<!-- END AUTO-GENERATED: GPIO Port Selection ParameterRef -->"
    GPIOClockParamRefStart    = "<!-- BEGIN AUTO-GENERATED: GPIO Clock Selection ParameterRef -->"
    GPIOClockParamRefEnd      = "<!-- END AUTO-GENERATED: GPIO Clock Selection ParameterRef -->"
    GPIOPortSelectionStart    = "<!-- BEGIN AUTO-GENERATED: GPIO Port Selection ParameterRefRef -->"
    GPIOPortSelectionEnd      = "<!-- END AUTO-GENERATED: GPIO Port Selection ParameterRefRef -->"
    GPIOClockSelectionStart   = "<!-- BEGIN AUTO-GENERATED: GPIO Clock Selection ParameterRefRef -->"
    GPIOClockSelectionEnd     = "<!-- END AUTO-GENERATED: GPIO Clock Selection ParameterRefRef -->"
    ConflictResultRefsStart   = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict Result ParameterRefs -->"
    ConflictResultRefsEnd     = "<!-- END AUTO-GENERATED: GPIO Conflict Result ParameterRefs -->"
    GPIOCopyCalcStart         = "<!-- BEGIN AUTO-GENERATED: GPIO Copy ParameterCalculation -->"
    GPIOCopyCalcEnd           = "<!-- END AUTO-GENERATED: GPIO Copy ParameterCalculation -->"
    GPIOClockCopyCalcStart    = "<!-- BEGIN AUTO-GENERATED: GPIO Clock Copy ParameterCalculation -->"
    GPIOClockCopyCalcEnd      = "<!-- END AUTO-GENERATED: GPIO Clock Copy ParameterCalculation -->"
    # Central Manual GPIO Warning marker
    CentralWarningStart       = "<!-- BEGIN AUTO-GENERATED: Central Manual GPIO Warning -->"
    CentralWarningEnd         = "<!-- END AUTO-GENERATED: Central Manual GPIO Warning -->"
    # JavaScript markers (NO leading whitespace in definition)
    JSStart                   = "// BEGIN AUTO-GENERATED: Multi-Hardware GPIO Port Mapping"
    JSEnd                     = "// END AUTO-GENERATED: Multi-Hardware GPIO Port Mapping"
    JSConflictStart           = "// BEGIN AUTO-GENERATED: Multi-Hardware GPIO Conflict Detection"
    JSConflictEnd             = "// END AUTO-GENERATED: Multi-Hardware GPIO Conflict Detection"
  }
}

# DEBUG: Verify Config is loaded correctly
if ($VerboseMode) {
  Write-Host "[CONFIG-TEST] ParamTypeStart = '$($script:Config.Markers.ParamTypeStart)'" -ForegroundColor Cyan
  Write-Host "[CONFIG-TEST] ParamTypeEnd = '$($script:Config.Markers.ParamTypeEnd)'" -ForegroundColor Cyan
}

# ====================================================================
# Helper Functions - Path resolution
# ====================================================================
function Get-RepoRoot {
  $scriptDir = if ($PSScriptRoot) { 
    $PSScriptRoot 
  }
  elseif ($PSCommandPath) { 
    Split-Path -Parent $PSCommandPath 
  }
  else { 
    $PWD.Path 
  }
    
  $repoRoot = Split-Path -Parent $scriptDir
  return $repoRoot
}

function Resolve-RepoPath {
  param([string]$RelativePath)
  $root = Get-RepoRoot
  $fullPath = Join-Path $root $RelativePath
  return $fullPath
}

function Clear-MarkerContent {
  param(
    [string]$FilePath,
    [string]$StartMarker,
    [string]$EndMarker,
    [string]$PlaceholderComment = "<!-- Cleaned - Ready for regeneration -->"
  )
    
  if (-not (Test-Path $FilePath)) {
    Write-Warning "File not found: $FilePath"
    return
  }
    
  $content = Get-Content $FilePath -Raw
  
  # Pattern: Match indent + BEGIN marker, content, indent + END marker
  # (?ms) = multiline (^ matches line start) + singleline (. matches newlines)
  # (^[ \t]*) = capture leading whitespace at line start
  # \1 = backreference - END marker MUST have same indent as BEGIN
  $escapedStart = [regex]::Escape($StartMarker)
  $escapedEnd = [regex]::Escape($EndMarker)
  $pattern = "(?ms)(^[ \t]*)($escapedStart`r?`n)(.*?)(`r?`n\1$escapedEnd)"
    
  if ($content -match $pattern) {
    # $1 = indent, $2 = BEGIN marker + newline, $3 = old content, $4 = newline + indent + END marker
    # Keep markers as-is (groups $1, $2 and $4), only replace content ($3)
    $indent = $Matches[1]
    $replacement = "`$1`$2$indent$PlaceholderComment`$4"
    $content = $content -replace $pattern, $replacement
    Set-Content -Path $FilePath -Value $content -NoNewline
    Write-Success "Cleared: $(Split-Path -Leaf $FilePath)"
  }
  else {
    Write-Warning "Markers not found in: $(Split-Path -Leaf $FilePath)"
  }
}

# ====================================================================
# Replace content between markers (generic function for all generators)
# ====================================================================
function Replace-MarkerContent {
  param(
    [string]$FilePath,
    [string]$StartMarker,
    [string]$EndMarker,
    [string]$NewContent
  )
    
  if (-not (Test-Path $FilePath)) {
    Write-Warning "File not found: $FilePath"
    return $false
  }
    
  $content = Get-Content $FilePath -Raw
  
  # Pattern: Match indent + BEGIN marker, content, indent + END marker
  # (?ms) = multiline (^ matches line start) + singleline (. matches newlines)
  # \1 = backreference - END marker MUST have same indent as BEGIN
  # Markers are STATIC ANCHORS - they are never moved or rewritten
  $escapedStart = [regex]::Escape($StartMarker)
  $escapedEnd = [regex]::Escape($EndMarker)
  $pattern = "(?ms)(^[ \t]*)($escapedStart`r?`n)(.*?)(`r?`n\1$escapedEnd)"
    
  if ($content -match $pattern) {
    # $1 = indent, $2 = BEGIN marker + newline, $3 = old content, $4 = newline + indent + END marker
    # Keep markers as-is (groups $2 and $4), only replace content ($3)
    $indent = $Matches[1]
    
    # Re-indent NewContent: Find minimal indent, remove it, add marker indent
    # This preserves RELATIVE indentation within the content
    $lines = $NewContent -split "`n"
    
    # Find minimal indentation of non-empty lines
    $minIndent = $null
    foreach ($line in $lines) {
      if ($line -match '^\s*$') { continue }  # Skip empty lines
      if ($line -match '^(\s*)') {
        $lineIndent = $Matches[1].Length
        if ($null -eq $minIndent -or $lineIndent -lt $minIndent) {
          $minIndent = $lineIndent
        }
      }
    }
    if ($null -eq $minIndent) { $minIndent = 0 }
    
    # Remove minimal indent from all lines, add marker indent
    $reindentedLines = @()
    foreach ($line in $lines) {
      if ($line -match '^\s*$') {
        # Empty line - keep as-is
        $reindentedLines += $line
      }
      else {
        # Remove base indent, keep relative indent
        $lineContent = $line.Substring([Math]::Min($minIndent, $line.Length))
        $reindentedLines += $indent + $lineContent
      }
    }
    $reindentedContent = $reindentedLines -join "`n"
    
    # Replacement: Keep $1 (indent), $2 (BEGIN+newline) and $4 (newline+indent+END), replace $3 (content)
    # This ensures markers stay exactly where they are
    $replacement = "`$1`$2$reindentedContent`$4"
    $content = $content -replace $pattern, $replacement
    Set-Content -Path $FilePath -Value $content -NoNewline
    return $true
  }
  else {
    return $false
  }
}

# ====================================================================
# OpenKNX Logo
# ====================================================================
function Show-OpenKNXLogo {
  param([string]$SubTitle, [string]$Version)
    
  $boxWidth = 112  # Content width inside the box
    
  Write-Host ""
  Write-Host "  ┌" -NoNewline -ForegroundColor DarkGray
  Write-Host ("─" * $boxWidth) -NoNewline -ForegroundColor DarkGray
  Write-Host "─┐" -ForegroundColor DarkGray
    
  # Line 1: "Open ■" - Open in normal color, ■ in green
  $line1Length = 6  # "Open " (5) + "■" (1)
  Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
  Write-Host "Open " -NoNewline
  Write-Host "$([char]::ConvertFromUtf32(0x25A0))" -NoNewline -ForegroundColor Green
  Write-Host (" " * ($boxWidth - $line1Length)) -NoNewline
  Write-Host "│" -ForegroundColor DarkGray
    
  # Line 2: Unicode + SubTitle + Version (all green)
  $unicodeString = "$([char]::ConvertFromUtf32(0x252C))$([char]::ConvertFromUtf32(0x2500))$([char]::ConvertFromUtf32(0x2500))$([char]::ConvertFromUtf32(0x2500))$([char]::ConvertFromUtf32(0x2500))$([char]::ConvertFromUtf32(0x2534)) "
    
  if ($SubTitle -and $Version) {
    $line2 = "$($unicodeString)$($SubTitle) v$Version"
  }
  elseif ($SubTitle) {
    $line2 = "$($unicodeString)$($SubTitle)"
  }
  else {
    $line2 = "$($unicodeString)"
  }
    
  Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
  Write-Host $line2 -NoNewline -ForegroundColor Green
  Write-Host (" " * ($boxWidth - $line2.Length)) -NoNewline
  Write-Host "│" -ForegroundColor DarkGray
    
  # Line 3: "■ KNX" - ■ in green, KNX in normal color
  $line3Length = 5  # "■" (1) + " KNX" (4)
  Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
  Write-Host "$([char]::ConvertFromUtf32(0x25A0))" -NoNewline -ForegroundColor Green
  Write-Host " KNX" -NoNewline
  Write-Host (" " * ($boxWidth - $line3Length)) -NoNewline
  Write-Host "│" -ForegroundColor DarkGray
    
  Write-Host "  └" -NoNewline -ForegroundColor DarkGray
  Write-Host ("─" * $boxWidth) -NoNewline -ForegroundColor DarkGray
  Write-Host "─┘" -ForegroundColor DarkGray
  Write-Host ""
}

# ====================================================================
# Helper Functions - Compiler Detection
# ====================================================================
function Find-PlatformIOCompiler {
  # Search for PlatformIO toolchains in common locations
  $platformioHome = if ($env:PLATFORMIO_CORE_DIR) {
    $env:PLATFORMIO_CORE_DIR
  }
  else {
    Join-Path $HOME ".platformio"
  }
    
  $packagesDir = Join-Path $platformioHome "packages"
    
  if (-not (Test-Path $packagesDir)) {
    return $null
  }
    
  # Search for toolchains with g++ compiler
  $toolchainDirs = Get-ChildItem -Path $packagesDir -Filter "toolchain-*" -Directory -ErrorAction SilentlyContinue
    
  $compilerNames = @(
    "arm-none-eabi-g++",      # RP2040, SAMD
    "xtensa-esp32-elf-g++",   # ESP32
    "riscv32-esp-elf-g++",    # ESP32-C3/C6/H2
    "*-g++"                   # Generic fallback
  )
    
  foreach ($toolchainDir in $toolchainDirs) {
    $binDir = Join-Path $toolchainDir.FullName "bin"
    if (-not (Test-Path $binDir)) { continue }
        
    foreach ($compilerName in $compilerNames) {
      $compilers = Get-ChildItem -Path $binDir -Filter $compilerName -File -ErrorAction SilentlyContinue
      if ($compilers) {
        return $compilers[0].FullName
      }
    }
  }
    
  return $null
}

# ====================================================================
# Helper Functions - Console output
# ====================================================================
function Write-Step {
  param([string]$Message)
  Write-Host "  [$FeatureName] " -NoNewline -ForegroundColor Cyan
  Write-Host $Message
}

function Write-Success {
  param([string]$Message)
  Write-Host "  [$FeatureName] ✓ " -NoNewline -ForegroundColor Green
  Write-Host $Message
}

function Write-Warning {
  param([string]$Message)
  Write-Host "  [$FeatureName] ⚠ " -NoNewline -ForegroundColor Yellow
  Write-Host $Message
}

function Write-ErrorMsg {
  param([string]$Message)
  Write-Host "  [$FeatureName] ✗ " -NoNewline -ForegroundColor Red
  Write-Host $Message
}

# ====================================================================
# GPIO Conflict Detection Helper Functions
# ====================================================================
function Generate-GPIOPortParametersInShare {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = 6
  )
    
  # Generate GPIO Port selection parameters in share.xml (00101-00106)
  # These store which GPIO port each strip uses (0=unused, 1-7=D0-D6)
  $paramsXml = "              <Union SizeInBit=`"48`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"60`" BitOffset=`"0`" />`n"
  $paramsXml += "                <!-- All 6 strips GPIO port selections (6 * 8 bit = 48 bit total) -->`n"
    
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $paramId = "00" + (100 + $stripIdx).ToString()  # 00101, 00102, ..., 00106
    $byteOffset = $stripIdx - 1  # Each parameter in its own byte
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%$paramId`" Offset=`"$byteOffset`" BitOffset=`"0`" Name=`"Strip${stripIdx}DataPort`" ParameterType=`"%AID%_PT-GPIOPort8Bit`" Text=`"Strip $stripIdx GPIO Port`" Value=`"15`" Access=`"Read`" UIHint=`"None`" SizeInBit=`"8`"/>`n"
  }
    
  $paramsXml += "              </Union>"
    
  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $script:Config.Markers.ParamDefStart `
    -EndMarker $script:Config.Markers.ParamDefEnd `
    -NewContent $paramsXml
}

function Generate-ClockPortParametersInShare {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = 6
  )
    
  # Generate Clock GPIO Port selection parameters in share.xml (00111-00116)
  # These store which GPIO port each strip uses for Clock (0=unused, 1-7=D0-D6)
  $paramsXml = "              <Union SizeInBit=`"48`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"67`" BitOffset=`"0`" />`n"
  $paramsXml += "                <!-- All 6 strips Clock GPIO port selections (6 * 8 bit = 48 bit total) -->`n"
    
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $paramId = "00" + (110 + $stripIdx).ToString()  # 00111, 00112, ..., 00116
    $byteOffset = $stripIdx - 1  # Each parameter in its own byte
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%$paramId`" Offset=`"$byteOffset`" BitOffset=`"0`" Name=`"Strip${stripIdx}ClockPort`" ParameterType=`"%AID%_PT-GPIOPort8Bit`" Text=`"Strip $stripIdx Clock GPIO Port`" Value=`"15`" Access=`"Read`" UIHint=`"None`" SizeInBit=`"8`"/>`n"
  }
    
  $paramsXml += "              </Union>"
    
  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $script:Config.Markers.ClockPortParamsStart `
    -EndMarker $script:Config.Markers.ClockPortParamsEnd `
    -NewContent $paramsXml
}

function Generate-ConflictParametersInShare {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = 6
  )
    
  # Generate conflict flag parameters in share.xml (00091-00096)
  # Template will reference these with 0009%C% token
  $paramsXml = "              <Union SizeInBit=`"$NumStrips`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"59`" BitOffset=`"0`" />`n"
  $paramsXml += "                <!-- All 6 strips conflict flags (6 bit total) -->`n"
    
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $paramId = "000" + (90 + $stripIdx).ToString()  # 00091, 00092, ..., 00096
    $bitOffset = $stripIdx - 1
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%$paramId`" Offset=`"0`" BitOffset=`"$bitOffset`" Name=`"Strip${stripIdx}HasDataConflict`" ParameterType=`"%AID%_PT-ConflictFlag`" Text=`"Strip $stripIdx Conflict`" Value=`"0`" Access=`"Read`" UIHint=`"None`" SizeInBit=`"1`"/>`n"
  }
    
  $paramsXml += "              </Union>"
    
  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $script:Config.Markers.ConflictParamsStart `
    -EndMarker $script:Config.Markers.ConflictParamsEnd `
    -NewContent $paramsXml
}

function Generate-ClockConflictParametersInShare {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = 6
  )
    
  # Generate Clock conflict flag parameters in share.xml (00117-00122)
  $paramsXml = "              <Union SizeInBit=`"$NumStrips`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"66`" BitOffset=`"0`" />`n"
  $paramsXml += "                <!-- All 6 strips Clock conflict flags (6 bit total) -->`n"
    
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $paramId = (116 + $stripIdx).ToString().PadLeft(5, '0')  # 00117, 00118, ..., 00122
    $bitOffset = $stripIdx - 1
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%$paramId`" Offset=`"0`" BitOffset=`"$bitOffset`" Name=`"Strip${stripIdx}HasClockConflict`" ParameterType=`"%AID%_PT-ConflictFlag`" Text=`"Strip $stripIdx Clock Conflict`" Value=`"0`" Access=`"Read`" UIHint=`"None`" SizeInBit=`"1`"/>`n"
  }
    
  $paramsXml += "              </Union>"
    
  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $script:Config.Markers.ClockConflictParamsStart `
    -EndMarker $script:Config.Markers.ClockConflictParamsEnd `
    -NewContent $paramsXml
}

function Generate-ConflictParameterRefsInShare {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = 6
  )
    
  # Generate ParameterRefs in share.xml for:
  # 1. GPIO Port Parameters (00101-00106)
  # 2. Conflict Flag Parameters (00091-00096)
    
  # Build ONLY the content, markers are already in the file and stay static
  $refsXml = "              <!-- GPIO Port Parameters for all 6 strips -->`n"
    
  # GPIO Port ParameterRefs (00101-00106)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $portParamId = "00" + (100 + $stripIdx).ToString()  # 00101, 00102, ..., 00106
    $portRefId = $portParamId + "01"  # 0010101, 0010201, ..., 0010601
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${portParamId}_R-%TT%${portRefId}`" RefId=`"%AID%_UP-%TT%${portParamId}`" UIHint=`"None`" />`n"
  }
    
  $refsXml += "              <!-- Data Conflict Flag Parameters for all 6 strips -->`n"
    
  # Data Conflict Flag ParameterRefs (00091-00096)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $conflictParamId = "000" + (90 + $stripIdx).ToString()  # 00091, 00092, ..., 00096
    $conflictRefId = $conflictParamId + "01"  # 0009101, 0009201, ..., 0009601
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${conflictParamId}_R-%TT%${conflictRefId}`" RefId=`"%AID%_UP-%TT%${conflictParamId}`" UIHint=`"None`" />`n"
  }
    
  $refsXml += "              <!-- Clock GPIO Port Parameters for all 6 strips -->`n"
    
  # Clock Port ParameterRefs (00111-00116)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockPortParamId = "00" + (110 + $stripIdx).ToString()  # 00111, 00112, ..., 00116
    $clockPortRefId = $clockPortParamId + "01"  # 0011101, 0011201, ..., 0011601
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${clockPortParamId}_R-%TT%${clockPortRefId}`" RefId=`"%AID%_UP-%TT%${clockPortParamId}`" UIHint=`"None`" />`n"
  }
    
  $refsXml += "              <!-- Clock Conflict Flag Parameters for all 6 strips -->`n"
    
  # Clock Conflict Flag ParameterRefs (00117-00122)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockConflictParamId = (116 + $stripIdx).ToString().PadLeft(5, '0')  # 00117, 00118, ..., 00122
    $clockConflictRefId = $clockConflictParamId + "01"  # 0011701, 0011801, ..., 0012201
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${clockConflictParamId}_R-%TT%${clockConflictRefId}`" RefId=`"%AID%_UP-%TT%${clockConflictParamId}`" UIHint=`"None`" />"
    if ($stripIdx -lt $NumStrips) {
      $refsXml += "`n"
    }
  }
    
  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $script:Config.Markers.ConflictParamRefsStart `
    -EndMarker $script:Config.Markers.ConflictParamRefsEnd `
    -NewContent $refsXml
}

function Generate-ConflictCalculation {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = 6
  )
    
  # Generate SINGLE ParameterCalculation in share.xml
  # Reads: Data GPIO ports (00101-00106) + Clock GPIO ports (00111-00116)
  # Writes: Data conflict flags (00091-00096) + Clock conflict flags (00117-00122)
    
  # LParameters: Read Data+Clock GPIO ports for all 6 strips
  $lParams = "<LParameters>`n"
  $lParams += "  <!-- Data GPIO Port selections for all 6 strips -->`n"
    
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $portParamId = "00" + (100 + $stripIdx).ToString()  # 00101, 00102, ..., 00106
    $refId = "00" + (100 + $stripIdx).ToString() + "01"  # 0010101, 0010201, ..., 0010601
    $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${portParamId}_R-%TT%${refId}`" AliasName=`"Strip${stripIdx}DataPort`" />`n"
  }
    
  $lParams += "  <!-- Clock GPIO Port selections for all 6 strips -->`n"
    
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockPortParamId = "00" + (110 + $stripIdx).ToString()  # 00111, 00112, ..., 00116
    $clockRefId = "00" + (110 + $stripIdx).ToString() + "01"  # 0011101, 0011201, ..., 0011601
    $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${clockPortParamId}_R-%TT%${clockRefId}`" AliasName=`"Strip${stripIdx}ClockPort`" />`n"
  }
    
  $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%00020_R-%TT%0002001`" AliasName=`"NumberOfLEDStrips`" />`n"
  $lParams += "</LParameters>"
    
  # RParameters: Write Data+Clock conflict flags
  $rParams = "<RParameters>`n"
  $rParams += "  <!-- Data conflict outputs for all 6 strips -->`n"
    
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $conflictParamId = "000" + (90 + $stripIdx).ToString()  # 00091, 00092, ..., 00096
    $refId = "000" + (90 + $stripIdx).ToString() + "01"  # 0009101, 0009201, ..., 0009601
    $rParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${conflictParamId}_R-%TT%${refId}`" AliasName=`"Strip${stripIdx}HasDataConflict`" />`n"
  }
    
  $rParams += "  <!-- Clock conflict outputs for all 6 strips -->`n"
    
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockConflictParamId = (116 + $stripIdx).ToString().PadLeft(5, '0')  # 00117, 00118, ..., 00122
    $clockRefId = $clockConflictParamId + "01"  # 0011701, 0011801, ..., 0012201
    $rParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${clockConflictParamId}_R-%TT%${clockRefId}`" AliasName=`"Strip${stripIdx}HasClockConflict`" />`n"
  }
    
  $rParams += "</RParameters>"
    
  # Single ParameterCalculation
  $calcXml = "<ParameterCalculation Id=`"%AID%_PC-%TT%00060`" Language=`"JavaScript`" Name=`"GPIOConflictTest`" LRTransformationFunc=`"NEO_DetectGPIOConflicts`" RLTransformationFunc=`"NEO_Empty`">`n"
  $calcXml += $lParams + "`n"
  $calcXml += $rParams + "`n"
  $calcXml += "</ParameterCalculation>"
    
  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $script:Config.Markers.ShareConflictCalcStart `
    -EndMarker $script:Config.Markers.ShareConflictCalcEnd `
    -NewContent $calcXml
}

function Generate-HardwareChangeResetCalculation {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = 6
  )
    
  # Generate ParameterCalculation that resets all ports to Dummy (15) on hardware change
  # Prevents conflicts from different port value mappings across hardware variants
    
  # LParameter: Hardware Selection
  $lParams = "<LParameters>`n"
  $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-4000018_R-400001801`" AliasName=`"HardwareSelection`" />`n"
  $lParams += "</LParameters>"
    
  # RParameters: All Data + Clock ports
  $rParams = "<RParameters>`n"
  $rParams += "  <!-- Reset all Data ports to Dummy (15) -->`n"
    
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $portParamId = "00" + (100 + $stripIdx).ToString()
    $refId = $portParamId + "01"
    $rParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${portParamId}_R-%TT%${refId}`" AliasName=`"Strip${stripIdx}DataPort`" />`n"
  }
    
  $rParams += "  <!-- Reset all Clock ports to Dummy (15) -->`n"
    
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockPortParamId = "00" + (110 + $stripIdx).ToString()
    $clockRefId = $clockPortParamId + "01"
    $rParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${clockPortParamId}_R-%TT%${clockRefId}`" AliasName=`"Strip${stripIdx}ClockPort`" />`n"
  }
    
  $rParams += "</RParameters>"
    
  # ParameterCalculation
  $calcXml = "<ParameterCalculation Id=`"%AID%_PC-%TT%00061`" Language=`"JavaScript`" Name=`"GPIOResetOnHWChange`" LRTransformationFunc=`"NEO_ResetAllPortsOnHardwareChange`" RLTransformationFunc=`"NEO_Empty`">`n"
  $calcXml += $lParams + "`n"
  $calcXml += $rParams + "`n"
  $calcXml += "</ParameterCalculation>"
    
  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $script:Config.Markers.HWChangeResetCalcStart `
    -EndMarker $script:Config.Markers.HWChangeResetCalcEnd `
    -NewContent $calcXml
}

# Hardware-specific UI Choose-blocks generator
function Generate-HardwareSpecificGPIOSelectionUI {
  param(
    [ValidateSet("Data", "Clock")]
    [string]$Type,
    [array]$HardwareConfigs
  )
    
  # Build hardware list from configs with parameter offsets
  $hardwareList = @()
  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $hwConfig = $HardwareConfigs[$hwIdx]
    $dataParamId = (73 + $hwIdx).ToString().PadLeft(3, '0')  # 073, 074, 075, ...
    $clockParamId = (81 + $hwIdx).ToString().PadLeft(3, '0')  # 081, 082, 083, ...
    
    $hardwareList += @{
      Id = $hwConfig.DeviceIdBit
      Name = $hwConfig.DeviceName
      DataParam = $dataParamId
      ClockParam = $clockParamId
    }
  }
    
  # Determine which parameter set to use
  $paramPrefix = if ($Type -eq "Data") { "Data" } else { "Clock" }
  $manualInputParam = if ($Type -eq "Data") { "033" } else { "034" }
  $mosiParam = "035"  # Only for Data type SPI
    
  # Start building the XML
  $xml = "<!-- Hardware-specific $Type GPIO Selection - shows correct ports for each hardware -->`n"
  $xml += "<choose ParamRefId=`"%AID%_UP-4000018_R-400001801`">`n"
    
  # Generate Choose-block for each hardware
  foreach ($hw in $hardwareList) {
    $paramId = if ($Type -eq "Data") { $hw.DataParam } else { $hw.ClockParam }
    $paramRef = "${paramId}01"
        
    $xml += "  <!-- $($hw.Name) -->`n"
    $xml += "  <when test=`"$($hw.Id)`">`n"
    $xml += "    <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${paramId}_R-%TT%9%C%${paramRef}`" IndentLevel=`"2`"/>`n"
    $xml += "  </when>`n"
  }
    
  # Default fallback
  $defaultParam = if ($Type -eq "Data") { "073" } else { "081" }
  $defaultRef = "${defaultParam}01"
  $xml += "  <when default=`"true`">`n"
  $xml += "    <!-- Fallback to RP2040 -->`n"
  $xml += "    <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${defaultParam}_R-%TT%9%C%${defaultRef}`" IndentLevel=`"2`"/>`n"
  $xml += "  </when>`n"
  $xml += "</choose>`n"
    
  # Manual input field section - hardware-specific
  $xml += "<!-- Manual $Type GPIO input field when set to 'Manuell' (value 10) -->`n"
  $xml += "<choose ParamRefId=`"%AID%_UP-4000018_R-400001801`">`n"
    
  foreach ($hw in $hardwareList) {
    $paramId = if ($Type -eq "Data") { $hw.DataParam } else { $hw.ClockParam }
    $paramRef = "${paramId}01"
        
    $xml += "  <when test=`"$($hw.Id)`">`n"
    $xml += "    <choose ParamRefId=`"%AID%_UP-%TT%9%C%${paramId}_R-%TT%9%C%${paramRef}`">`n"
    $xml += "      <when test=`"10`">`n"
        
    if ($Type -eq "Data") {
      # Data type has special SPI handling
      $xml += "        <choose ParamRefId=`"%AID%_UP-%TT%9%C%030_R-%TT%9%C%03001`">`n"
      $xml += "          <when test=`"5 21 22 23 24 25`">`n"
      $xml += "            <!-- MOSI GPIO nur bei SPI -->`n"
      $xml += "            <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${mosiParam}_R-%TT%9%C%${mosiParam}01`" IndentLevel=`"3`" HelpContext=`"%DOC%`"/>`n"
      $xml += "          </when>`n"
      $xml += "          <when default=`"true`">`n"
      $xml += "            <!-- Data GPIO -->`n"
      $xml += "            <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${manualInputParam}_R-%TT%9%C%${manualInputParam}01`" IndentLevel=`"3`" HelpContext=`"%DOC%`"/>`n"
      $xml += "          </when>`n"
      $xml += "        </choose>`n"
    }
    else {
      # Clock type is simpler
      $xml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${manualInputParam}_R-%TT%9%C%${manualInputParam}01`" IndentLevel=`"3`" HelpContext=`"%DOC%`"/>`n"
    }
        
    $xml += "      </when>`n"
    $xml += "    </choose>`n"
    $xml += "  </when>`n"
  }
    
  # Default fallback for manual input
  $xml += "  <when default=`"true`">`n"
  $xml += "    <choose ParamRefId=`"%AID%_UP-%TT%9%C%${defaultParam}_R-%TT%9%C%${defaultRef}`">`n"
  $xml += "      <when test=`"10`">`n"
    
  if ($Type -eq "Data") {
    $xml += "        <choose ParamRefId=`"%AID%_UP-%TT%9%C%030_R-%TT%9%C%03001`">`n"
    $xml += "          <when test=`"5 21 22 23 24 25`">`n"
    $xml += "            <!-- MOSI GPIO nur bei SPI -->`n"
    $xml += "            <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${mosiParam}_R-%TT%9%C%${mosiParam}01`" IndentLevel=`"3`" HelpContext=`"%DOC%`"/>`n"
    $xml += "          </when>`n"
    $xml += "          <when default=`"true`">`n"
    $xml += "            <!-- Data GPIO -->`n"
    $xml += "            <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${manualInputParam}_R-%TT%9%C%${manualInputParam}01`" IndentLevel=`"3`" HelpContext=`"%DOC%`"/>`n"
    $xml += "          </when>`n"
    $xml += "        </choose>`n"
  }
  else {
    $xml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${manualInputParam}_R-%TT%9%C%${manualInputParam}01`" IndentLevel=`"3`" HelpContext=`"%DOC%`"/>`n"
  }
    
  $xml += "      </when>`n"
  $xml += "    </choose>`n"
  $xml += "  </when>`n"
  $xml += "</choose>"
    
  return $xml
}

# ============================================================================
# JavaScript Generation
# ============================================================================

function Generate-ConflictDetectionJS {
  param(
    [string]$JavaScriptPath,
    [int]$NumStrips = 6
  )
    
  # Initialize port array  
  $jsInit = "  var dataPorts = [`n    input.Strip1DataPort"
  for ($i = 2; $i -le $NumStrips; $i++) {
    $jsInit += ",`n    input.Strip${i}DataPort"
  }
  $jsInit += "`n  ];"
    
  # Initialize clock port array
  $jsClockInit = "  var clockPorts = [`n    input.Strip1ClockPort"
  for ($i = 2; $i -le $NumStrips; $i++) {
    $jsClockInit += ",`n    input.Strip${i}ClockPort"
  }
  $jsClockInit += "`n  ];"
    
  # Initialize data outputs
  $jsOutputInit = "  // Alle Data Conflict Flags initial auf 0 setzen`n"
  for ($i = 1; $i -le $NumStrips; $i++) {
    $jsOutputInit += "  output.Strip${i}HasDataConflict = 0;`n"
  }
  $jsOutputInit = $jsOutputInit.TrimEnd("`n")
    
  # Initialize clock outputs
  $jsClockOutputInit = "  // Alle Clock Conflict Flags initial auf 0 setzen`n"
  for ($i = 1; $i -le $NumStrips; $i++) {
    $jsClockOutputInit += "  output.Strip${i}HasClockConflict = 0;`n"
  }
  $jsClockOutputInit = $jsClockOutputInit.TrimEnd("`n")
    
  # Data conflict detection logic (if-else chain for ECMA3 compatibility)
  $jsConflictAssignmentsI = ""
  for ($i = 0; $i -lt $NumStrips; $i++) {
    $stripNum = $i + 1
    $condition = "i == $i"
    $jsConflictAssignmentsI += "        if ($condition) output.Strip${stripNum}HasDataConflict = 1;`n"
  }
    
  $jsConflictAssignmentsJ = ""
  for ($j = 0; $j -lt $NumStrips; $j++) {
    $stripNum = $j + 1
    $condition = "j == $j"
    $jsConflictAssignmentsJ += "        if ($condition) output.Strip${stripNum}HasDataConflict = 1;`n"
  }
    
  # Clock conflict detection logic (if-else chain for ECMA3 compatibility)
  # WICHTIG: Clock Conflicts setzen AUCH Data Conflicts für UI-Anzeige!
  $jsClockConflictAssignmentsI = ""
  for ($i = 0; $i -lt $NumStrips; $i++) {
    $stripNum = $i + 1
    $condition = "i == $i"
    $jsClockConflictAssignmentsI += "        if ($condition) {`n"
    $jsClockConflictAssignmentsI += "          output.Strip${stripNum}HasClockConflict = 1;`n"
    $jsClockConflictAssignmentsI += "          output.Strip${stripNum}HasDataConflict = 1;  // Für UI-Anzeige`n"
    $jsClockConflictAssignmentsI += "        }`n"
  }
    
  $jsClockConflictAssignmentsJ = ""
  for ($j = 0; $j -lt $NumStrips; $j++) {
    $stripNum = $j + 1
    $condition = "j == $j"
    $jsClockConflictAssignmentsJ += "        if ($condition) {`n"
    $jsClockConflictAssignmentsJ += "          output.Strip${stripNum}HasClockConflict = 1;`n"
    $jsClockConflictAssignmentsJ += "          output.Strip${stripNum}HasDataConflict = 1;  // Für UI-Anzeige`n"
    $jsClockConflictAssignmentsJ += "        }`n"
  }
    
  # Cross-conflict logic: Data vs Clock (erweitert mit UI-Flags)
  $jsCrossDataConflictAssignments = ""
  for ($i = 0; $i -lt $NumStrips; $i++) {
    $stripNum = $i + 1
    $condition = "i == $i"
    $jsCrossDataConflictAssignments += "        if ($condition) output.Strip${stripNum}HasDataConflict = 1;`n"
  }
    
  $jsCrossClockConflictAssignments = ""
  for ($j = 0; $j -lt $NumStrips; $j++) {
    $stripNum = $j + 1
    $condition = "j == $j"
    $jsCrossClockConflictAssignments += "        if ($condition) {`n"
    $jsCrossClockConflictAssignments += "          output.Strip${stripNum}HasClockConflict = 1;`n"
    $jsCrossClockConflictAssignments += "          output.Strip${stripNum}HasDataConflict = 1;  // Für UI-Anzeige`n"
    $jsCrossClockConflictAssignments += "        }`n"
  }

  # Full JavaScript function with helper
  $fullJS = "`n// ============================================================================================================`n"
  $fullJS += "// HELPER: Copy value from input to output`n"
  $fullJS += "// ============================================================================================================`n"
  $fullJS += "function NEO_CopyValue(input, output, context) {`n"
  $fullJS += "  output.ShareGPIOPort = input.TemplateGPIOPort;`n"
  $fullJS += "}`n"
  $fullJS += "`n"
  $fullJS += "// ============================================================================================================`n"
  $fullJS += "// HARDWARE WECHSEL: Reset alle Port-Zuweisungen auf Dummy (15)`n"
  $fullJS += "// Verhindert Konflikte durch unterschiedliche Port-Values bei verschiedenen Hardware-Varianten`n"
  $fullJS += "// ============================================================================================================`n"
  $fullJS += "function NEO_ResetAllPortsOnHardwareChange(input, output, context) {`n"
  for ($i = 1; $i -le $NumStrips; $i++) {
    $fullJS += "  output.Strip${i}DataPort = 15;`n"
  }
  for ($i = 1; $i -le $NumStrips; $i++) {
    $fullJS += "  output.Strip${i}ClockPort = 15;`n"
  }
  $fullJS += "}`n"
  $fullJS += "`n"
  $fullJS += "// ============================================================================================================`n"
  $fullJS += "// REAL GPIO CONFLICT DETECTION`n"
  $fullJS += "// Prüft ob ein Strip denselben GPIO Port wie ein anderer Strip verwendet`n"
  $fullJS += "// Wenn Strip X denselben Port wie Strip Y hat → BEIDE bekommen Konflikt-Flag`n"
  $fullJS += "// UNTERSTÜTZT: Data vs Data, Clock vs Clock, Data vs Clock Cross-Konflikte`n"
  $fullJS += "// ============================================================================================================`n"
  $fullJS += "function NEO_DetectGPIOConflicts(input, output, context) {`n"
  $fullJS += "  // Data Port-Werte für alle $NumStrips Strips lesen (0=Nicht verwendet, 10=Manuell)`n"
  $fullJS += $jsInit + "`n"
  $fullJS += "  `n"
  $fullJS += "  // Clock Port-Werte für alle $NumStrips Strips lesen (0=Nicht verwendet, 10=Manuell)`n"
  $fullJS += $jsClockInit + "`n"
  $fullJS += "  `n"
  $fullJS += $jsOutputInit + "`n"
  $fullJS += "  `n"
  $fullJS += $jsClockOutputInit + "`n"
  $fullJS += "  `n"
  $fullJS += "  // CHECK 1: Data vs Data Konflikte`n"
  $fullJS += "  for (var i = 0; i < 6; i++) {`n"
  $fullJS += "    var dataI = dataPorts[i];`n"
  $fullJS += "    if (dataI == 10 || dataI == 15) continue;  // Manuell oder Dummy`n"
  $fullJS += "    `n"
  $fullJS += "    for (var j = i + 1; j < 6; j++) {`n"
  $fullJS += "      var dataJ = dataPorts[j];`n"
  $fullJS += "      `n"
  $fullJS += "      if (dataI == dataJ && dataJ != 10 && dataJ != 15) {`n"
  $fullJS += "        // BEIDE Strips bekommen Data-Konflikt-Flag`n"
  $fullJS += $jsConflictAssignmentsI
  $fullJS += "        `n"
  $fullJS += $jsConflictAssignmentsJ
  $fullJS += "      }`n"
  $fullJS += "    }`n"
  $fullJS += "  }`n"
  $fullJS += "  `n"
  $fullJS += "  // CHECK 2: Clock vs Clock Konflikte`n"
  $fullJS += "  for (var i = 0; i < 6; i++) {`n"
  $fullJS += "    var clockI = clockPorts[i];`n"
  $fullJS += "    if (clockI == 10 || clockI == 15) continue;  // Manuell oder Dummy`n"
  $fullJS += "    `n"
  $fullJS += "    for (var j = i + 1; j < 6; j++) {`n"
  $fullJS += "      var clockJ = clockPorts[j];`n"
  $fullJS += "      `n"
  $fullJS += "      if (clockI == clockJ && clockJ != 10 && clockJ != 15) {`n"
  $fullJS += "        // BEIDE Strips bekommen Clock-Konflikt-Flag`n"
  $fullJS += $jsClockConflictAssignmentsI
  $fullJS += "        `n"
  $fullJS += $jsClockConflictAssignmentsJ
  $fullJS += "      }`n"
  $fullJS += "    }`n"
  $fullJS += "  }`n"
  $fullJS += "  `n"
  $fullJS += "  // CHECK 3: Data vs Clock Cross-Konflikte (CRITICAL!)`n"
  $fullJS += "  for (var i = 0; i < 6; i++) {`n"
  $fullJS += "    var dataI = dataPorts[i];`n"
  $fullJS += "    if (dataI == 10 || dataI == 15) continue;  // Manuell oder Dummy`n"
  $fullJS += "    `n"
  $fullJS += "    for (var j = 0; j < 6; j++) {`n"
  $fullJS += "      if (i == j) continue;  // Selber Strip`n"
  $fullJS += "      `n"
  $fullJS += "      var clockJ = clockPorts[j];`n"
  $fullJS += "      `n"
  $fullJS += "      if (dataI == clockJ && clockJ != 10 && clockJ != 15) {`n"
  $fullJS += "        // Strip i hat Data-Konflikt, Strip j hat Clock-Konflikt`n"
  $fullJS += $jsCrossDataConflictAssignments
  $fullJS += "        `n"
  $fullJS += $jsCrossClockConflictAssignments
  $fullJS += "      }`n"
  $fullJS += "    }`n"
  $fullJS += "  }`n"
  $fullJS += "  `n"
  $fullJS += "  // CHECK 4: Interne Strip-Konflikte (Data = Clock im selben Strip)`n"
  $fullJS += "  for (var i = 0; i < 6; i++) {`n"
  $fullJS += "    var dataI = dataPorts[i];`n"
  $fullJS += "    var clockI = clockPorts[i];`n"
  $fullJS += "    `n"
  $fullJS += "    // Skip wenn einer der Ports manuell ist oder Dummy`n"
  $fullJS += "    if (dataI == 10 || dataI == 15 || clockI == 10 || clockI == 15) continue;`n"
  $fullJS += "    `n"
  $fullJS += "    // Konflikt wenn Data-Port = Clock-Port im selben Strip`n"
  $fullJS += "    if (dataI == clockI) {`n"
  $fullJS += "      // Strip hat internen Konflikt: Data und Clock auf demselben Port`n"
  for ($i = 0; $i -lt $NumStrips; $i++) {
    $stripNum = $i + 1
    $condition = "i == $i"
    $fullJS += "      if ($condition) {`n"
    $fullJS += "        output.Strip${stripNum}HasDataConflict = 1;`n"
    $fullJS += "        output.Strip${stripNum}HasClockConflict = 1;`n"
    $fullJS += "      }`n"
  }
  $fullJS += "    }`n"
  $fullJS += "  }`n"
  $fullJS += "}`n"
    
  # Replace content using generic function
  $success = Replace-MarkerContent -FilePath $JavaScriptPath `
    -StartMarker $script:Config.Markers.JSConflictStart `
    -EndMarker $script:Config.Markers.JSConflictEnd `
    -NewContent $fullJS
    
  if (-not $success) {
    # Append at end if markers don't exist
    $jsContent = Get-Content -Path $JavaScriptPath -Raw
    $jsContent += "`n" + $script:Config.Markers.JSConflictStart + "`n" + $fullJS + "`n" + $script:Config.Markers.JSConflictEnd + "`n"
    Set-Content -Path $JavaScriptPath -Value $jsContent -NoNewline
  }
  return $true
}

function Generate-GPIOClockCopyCalculation {
  param(
    [string]$TemplatePath,
    [array]$HardwareConfigs
  )
    
  # Generate 8 SEPARATE ParameterCalculations - ALL flat without Choose-wrappers
  # OpenKNXproducer does NOT process Choose-blocks in Static/ParameterCalculations section!
  # All 8 will be in final XML, but only the one matching current hardware will execute
  # (because parameters only exist for selected hardware)
    
  $calcXml = ""
    
  # Generate one ParameterCalculation for each hardware (WITHOUT Choose-wrapper)
  foreach ($hwConfig in $HardwareConfigs) {
    $hwId = $hwConfig.DeviceIdBit
    $hwName = $hwConfig.DeviceName
    $hwIndex = [array]::IndexOf($HardwareConfigs, $hwConfig)
    $parameterIndex = 81 + $hwIndex  # 081, 082, 083, etc.
    $parameterIndexFormatted = $parameterIndex.ToString("000")  # Convert to 081, 082, etc.
    $calcId = "089" + $hwIndex.ToString()  # 0890, 0891, 0892, etc.
        
    # Flat ParameterCalculation (NO Choose-wrapper!)
    $calcXml += "<!-- $hwName -->`n"
    $calcXml += "<ParameterCalculation Id=`"%AID%_PC-%TT%9%C%${calcId}`" Language=`"JavaScript`" Name=`"Strip%C%_CopyClockGPIOPort_HW${hwIndex}`" LRTransformationFunc=`"NEO_CopyValue`" RLTransformationFunc=`"NEO_Empty`">`n"
    $calcXml += "  <LParameters>`n"
    $calcXml += "    <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${parameterIndexFormatted}_R-%TT%9%C%${parameterIndexFormatted}01`" AliasName=`"TemplateGPIOPort`" />`n"
    $calcXml += "  </LParameters>`n"
    $calcXml += "  <RParameters>`n"
    $calcXml += "    <ParameterRefRef RefId=`"%AID%_UP-%TT%0011%C%_R-%TT%0011%C%01`" AliasName=`"ShareGPIOPort`" />`n"
    $calcXml += "  </RParameters>`n"
    $calcXml += "</ParameterCalculation>"
    if($hwIndex -lt ($HardwareConfigs.Count - 1)) {
      $calcXml += "`n"
    }
    
  }
    
  # Replace content using generic function
  return Replace-MarkerContent -FilePath $TemplatePath `
    -StartMarker $script:Config.Markers.GPIOClockCopyCalcStart `
    -EndMarker $script:Config.Markers.GPIOClockCopyCalcEnd `
    -NewContent $calcXml
}

function Generate-GPIOCopyCalculation {
  param(
    [string]$TemplatePath,
    [array]$HardwareConfigs
  )
    
  # Generate 8 SEPARATE ParameterCalculations - ALL flat without Choose-wrappers
  # OpenKNXproducer does NOT process Choose-blocks in Static/ParameterCalculations section!
  # All 8 will be in final XML, but only the one matching current hardware will execute
  # (because parameters only exist for selected hardware)
    
  $calcXml = ""
    
  # Generate one ParameterCalculation for each hardware (WITHOUT Choose-wrapper)
  foreach ($hwConfig in $HardwareConfigs) {
    $hwId = $hwConfig.DeviceIdBit
    $hwName = $hwConfig.DeviceName
    $hwIndex = [array]::IndexOf($HardwareConfigs, $hwConfig)
    $parameterIndex = 73 + $hwIndex  # 073, 074, 075, etc.
    $parameterIndexFormatted = $parameterIndex.ToString("000")  # Convert to 073, 074, etc.
    $calcId = "080" + $hwIndex.ToString()  # 0800, 0801, 0802, etc.
        
    # Flat ParameterCalculation (NO Choose-wrapper!)
    $calcXml += "<!-- $hwName -->`n"
    $calcXml += "<ParameterCalculation Id=`"%AID%_PC-%TT%9%C%${calcId}`" Language=`"JavaScript`" Name=`"Strip%C%_CopyGPIOPort_HW${hwIndex}`" LRTransformationFunc=`"NEO_CopyValue`" RLTransformationFunc=`"NEO_Empty`">`n"
    $calcXml += "  <LParameters>`n"
    $calcXml += "    <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${parameterIndexFormatted}_R-%TT%9%C%${parameterIndexFormatted}01`" AliasName=`"TemplateGPIOPort`" />`n"
    $calcXml += "  </LParameters>`n"
    $calcXml += "  <RParameters>`n"
    $calcXml += "    <ParameterRefRef RefId=`"%AID%_UP-%TT%0010%C%_R-%TT%0010%C%01`" AliasName=`"ShareGPIOPort`" />`n"
    $calcXml += "  </RParameters>`n"
    $calcXml += "</ParameterCalculation>"
    if($hwIndex -lt ($HardwareConfigs.Count - 1)) {
      $calcXml += "`n"
    }
  }
    
  # Replace content using generic function
  return Replace-MarkerContent -FilePath $TemplatePath `
    -StartMarker $script:Config.Markers.GPIOCopyCalcStart `
    -EndMarker $script:Config.Markers.GPIOCopyCalcEnd `
    -NewContent $calcXml
}

function Generate-ConflictUI {
  param(
    [string]$TemplatePath
  )
    
  # Generate UI that shows conflicts for BOTH Data and Clock
  # Uses a clever approach: Show conflict if EITHER Data OR Clock conflict exists
  # since Clock parameters don't follow %C% pattern, we'll use specific mapping
    
  # Hidden parameters for conflict status reading
  $uiXml = "<ParameterRefRef RefId=`"%AID%_UP-%TT%0009%C%_R-%TT%0009%C%01`" IndentLevel=`"1`" />`n"
    
  # Add hidden Clock conflict parameters based on Strip number
  # Strip1=%C%=1 → Clock Conflict = 00117
  # Strip2=%C%=2 → Clock Conflict = 00118, etc.
  $uiXml += "<!-- Hidden Clock conflict parameter references -->`n"
  $uiXml += "<choose ParamRefId=`"%AID%_UP-%TT%00020_R-%TT%0002001`">`n"  # Use Strip Count to determine which
  $uiXml += "  <!-- We can't easily reference Clock conflicts with %C%, so we'll use Data conflicts only for now -->`n"
  $uiXml += "</choose>`n"
    
  # Combined conflict display - triggers on Data conflicts (which includes Cross-conflicts)
  $uiXml += "<choose ParamRefId=`"%AID%_UP-%TT%0009%C%_R-%TT%0009%C%01`">`n"
  $uiXml += "  <when test=`"1`">`n"
  $uiXml += "    <ParameterSeparator Id=`"%AID%_PS-gpioconflict%C%`" Text=`"⛔ GPIO PORT KONFLIKT: Dieser Port wird bereits von einem anderen Streifen verwendet, oder Data- und Clock-Port sind identisch!`" UIHint=`"Error`" />`n"
  $uiXml += "  </when>`n"
  $uiXml += "</choose>"
    
  # Replace content using generic function
  return Replace-MarkerContent -FilePath $TemplatePath `
    -StartMarker $script:Config.Markers.ConflictUIStart `
    -EndMarker $script:Config.Markers.ConflictUIEnd `
    -NewContent $uiXml
}

# REMOVED: Generate-HardwareSpecificParameters - This was the wrong approach
# The issue is that Choose-blocks should be in ParameterCalculations, not ParameterRefRef
# Template-parameters with %TT% and %C% should be used directly

# ====================================================================
# Main Script
# ====================================================================

Clear-Host

Show-OpenKNXLogo -SubTitle "Dynamic GPIO Template Generator by Erkan Çolak" -Version $SCRIPT_VERSION

Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  Generating Hardware-Specific GPIO Configuration" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host ""

Write-Step "Feature: $FeatureName"
Write-Step "Defines Prefix: $DefinesPrefix"
Write-Step "Template: $TemplateFile"
Write-Host ""

# Resolve all paths relative to repo root
$repoRoot = Get-RepoRoot
$scriptDir = Join-Path $repoRoot "scripts"
$extractorToolPath = Resolve-RepoPath $ExtractorTool
$templatePath = Resolve-RepoPath $TemplateFile
$shareXmlPath = Resolve-RepoPath $ShareXml
$scriptJsPath = Resolve-RepoPath $JavaScriptFile

Write-Host "  ────────────────────────────────────────────────────────────────────────────────────────────────────────────────────" -ForegroundColor DarkGray
Write-Host ""

# Handle Clean mode
if ($Clean) {
  Write-Step "Cleaning marker blocks (Clean mode)..."
  Write-Host ""
  
  # List what will be cleaned
  Write-Host "  Files to be cleaned:" -ForegroundColor Cyan
  Write-Host "    • share.xml (ParameterTypes, Hardware Selection, GPIO Ports, etc.)" -ForegroundColor DarkGray
  Write-Host "    • Template file (GPIO Options, Port Parameters, Conflict Detection)" -ForegroundColor DarkGray
  Write-Host "    • JavaScript file (GPIO Mapping, Conflict Detection)" -ForegroundColor DarkGray
  Write-Host "    • HardwareMappingData.h" -ForegroundColor DarkGray
  Write-Host ""
  
  # Ask for confirmation (unless -Force)
  if (-not $Force) {
    Write-Host "  Continue with cleanup? (y/n): " -NoNewline -ForegroundColor Yellow
    $confirmation = Read-Host
    
    if ($confirmation -ne 'y' -and $confirmation -ne 'Y') {
      Write-Host ""
      Write-Host "  Cleanup cancelled." -ForegroundColor Yellow
      Write-Host ""
      exit 0
    }
  }
  
  Write-Host ""
  Write-Host "  Cleaning files..." -ForegroundColor Cyan
  Write-Host ""
    
  # Clean share.xml
  Write-Host "  • Cleaning share.xml (ParameterTypes)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $shareXmlPath `
    -StartMarker $script:Config.Markers.ParamTypeStart `
    -EndMarker $script:Config.Markers.ParamTypeEnd
    
  Write-Host "  • Cleaning share.xml (Hardware Selection Parameter)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $shareXmlPath `
    -StartMarker $script:Config.Markers.HardwareParamStart `
    -EndMarker $script:Config.Markers.HardwareParamEnd
    
  Write-Host "  • Cleaning share.xml (Hardware Selection ParameterRef)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $shareXmlPath `
    -StartMarker $script:Config.Markers.HardwareParamRefStart `
    -EndMarker $script:Config.Markers.HardwareParamRefEnd
    
  Write-Host "  • Cleaning share.xml (Hardware Selection ParameterRefRef UI)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $shareXmlPath `
    -StartMarker $script:Config.Markers.HardwareParamRefRefStart `
    -EndMarker $script:Config.Markers.HardwareParamRefRefEnd
    
  Write-Host "  • Cleaning share.xml (GPIO Port Parameters)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $shareXmlPath `
    -StartMarker $script:Config.Markers.ParamDefStart `
    -EndMarker $script:Config.Markers.ParamDefEnd
    
  Write-Host "  • Cleaning share.xml (GPIO Clock Port Parameters)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $shareXmlPath `
    -StartMarker $script:Config.Markers.ClockPortParamsStart `
    -EndMarker $script:Config.Markers.ClockPortParamsEnd
    
  Write-Host "  • Cleaning share.xml (GPIO Conflict Parameters)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $shareXmlPath `
    -StartMarker $script:Config.Markers.ConflictParamsStart `
    -EndMarker $script:Config.Markers.ConflictParamsEnd

  Write-Host "  • Cleaning share.xml (GPIO Clock Conflict Parameters)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $shareXmlPath `
    -StartMarker $script:Config.Markers.ClockConflictParamsStart `
    -EndMarker $script:Config.Markers.ClockConflictParamsEnd
    
  Write-Host "  • Cleaning share.xml (GPIO Conflict ParameterRefs)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $shareXmlPath `
    -StartMarker $script:Config.Markers.ConflictParamRefsStart `
    -EndMarker $script:Config.Markers.ConflictParamRefsEnd
    
  Write-Host "  • Cleaning share.xml (GPIO Conflict Check ParameterCalculation)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $shareXmlPath `
    -StartMarker $script:Config.Markers.ShareConflictCalcStart `
    -EndMarker $script:Config.Markers.ShareConflictCalcEnd
    
  Write-Host "  • Cleaning share.xml (GPIO Hardware Change Reset ParameterCalculation)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $shareXmlPath `
    -StartMarker $script:Config.Markers.HWChangeResetCalcStart `
    -EndMarker $script:Config.Markers.HWChangeResetCalcEnd
    
  # Clean template XML - ALL marker blocks
  Write-Host "  • Cleaning template XML (GPIO Port Selection Parameter)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.GPIOPortParamStart `
    -EndMarker $script:Config.Markers.GPIOPortParamEnd
    
  Write-Host "  • Cleaning template XML (GPIO Port Selection ParameterRef)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.GPIOPortParamRefStart `
    -EndMarker $script:Config.Markers.GPIOPortParamRefEnd
    
  Write-Host "  • Cleaning template XML (GPIO Clock Selection ParameterRef)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.GPIOClockParamRefStart `
    -EndMarker $script:Config.Markers.GPIOClockParamRefEnd
    
  Write-Host "  • Cleaning template XML (GPIO Clock Selection Parameter)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.GPIOClockParamStart `
    -EndMarker $script:Config.Markers.GPIOClockParamEnd
    
  Write-Host "  • Cleaning template XML (GPIO Port Selection ParameterRefRef)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.GPIOPortSelectionStart `
    -EndMarker $script:Config.Markers.GPIOPortSelectionEnd
    
  Write-Host "  • Cleaning template XML (GPIO Clock Selection ParameterRefRef)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.GPIOClockSelectionStart `
    -EndMarker $script:Config.Markers.GPIOClockSelectionEnd
    
  Write-Host "  • Cleaning template XML (Central Manual GPIO Warning)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.CentralWarningStart `
    -EndMarker $script:Config.Markers.CentralWarningEnd
    
  Write-Host "  • Cleaning template XML (Conflict Parameters)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.ConflictParamStart `
    -EndMarker $script:Config.Markers.ConflictParamEnd
    
  Write-Host "  • Cleaning template XML (ParameterRefs)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.ConflictResultRefsStart `
    -EndMarker $script:Config.Markers.ConflictResultRefsEnd
    
  Write-Host "  • Cleaning template XML (ParameterCalculations)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.ConflictCalcStart `
    -EndMarker $script:Config.Markers.ConflictCalcEnd
    
  Write-Host "  • Cleaning template XML (UI Display)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.ConflictUIStart `
    -EndMarker $script:Config.Markers.ConflictUIEnd
    
  # Clean JavaScript (Port Mapping)
  Write-Host "  • Cleaning JavaScript (Port Mapping)..." -ForegroundColor Cyan
  if (Test-Path $scriptJsPath) {
    Clear-MarkerContent -FilePath $scriptJsPath `
      -StartMarker $script:Config.Markers.JSStart `
      -PlaceholderComment "// Cleaned - Ready for regeneration" `
      -EndMarker $script:Config.Markers.JSEnd
  }
    
  # Clean JavaScript (Conflict Detection)
  Write-Host "  • Cleaning JavaScript (Conflict Detection)..." -ForegroundColor Cyan
  if (Test-Path $scriptJsPath) {
    Clear-MarkerContent -FilePath $scriptJsPath `
      -PlaceholderComment "// Cleaned - Ready for regeneration" `
      -StartMarker $script:Config.Markers.JSConflictStart `
      -EndMarker $script:Config.Markers.JSConflictEnd
  }
    
  # Clean template XML (GPIO Copy ParameterCalculation)
  Write-Host "  • Cleaning template XML (GPIO Copy ParameterCalculation)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.GPIOCopyCalcStart `
    -EndMarker $script:Config.Markers.GPIOCopyCalcEnd
    
  # Clean template XML (GPIO Clock Copy ParameterCalculation)
  Write-Host "  • Cleaning template XML (GPIO Clock Copy ParameterCalculation)..." -ForegroundColor Cyan
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $script:Config.Markers.GPIOClockCopyCalcStart `
    -EndMarker $script:Config.Markers.GPIOClockCopyCalcEnd
    
  # Clean HardwareMappingData.h
  Write-Host "  • Cleaning HardwareMappingData.h..." -ForegroundColor Cyan
  $hardwareMappingPath = Resolve-RepoPath $HardwareMappingHeader
  if (Test-Path $hardwareMappingPath) {
    $headerFileName = Split-Path -Leaf $HardwareMappingHeader
    $placeholderContent = @"
/**
 * @file $headerFileName
 * @brief Hardware ID to Index mapping (auto-generated)
 * 
 * This file is AUTO-GENERATED by scripts/Build-HardwareConfig.ps1
 * DO NOT EDIT MANUALLY!
 * 
 * Run the following command to regenerate:
 *   pwsh scripts/Build-HardwareConfig.ps1 -FeatureName "NeoPixel" -DefinesPrefix "NEOPIXEL_HW"
 * Or use the wrapper:
 *   pwsh scripts/Build-HardwareConfigTemplates.ps1
 */

#pragma once

#error "HardwareMappingData.h is empty! Run 'pwsh scripts/Build-HardwareConfigTemplates.ps1' to regenerate."

// This file will be generated by Build-HardwareConfig.ps1
// Run the script to generate hardware ID mapping from platformio.hardware.ini
"@
    Set-Content -Path $hardwareMappingPath -Value $placeholderContent -NoNewline
    Write-Host "    ✓ $headerFileName - Emptied with placeholder" -ForegroundColor Green
  }
    
  Write-Host ""
  Write-Success "All marker blocks cleaned successfully!"
  Write-Host ""
  exit 0
}

if ($VerboseMode) {
  Write-Host "[DEBUG-CHECKPOINT-1] After Clean Mode: Markers.ParamTypeStart = '$($script:Config.Markers.ParamTypeStart)'" -ForegroundColor Cyan
}

# Step 1: Parse ALL hardware configurations from platformio.hardware.ini
Write-Step "Scanning for hardware configurations..."

$hardwareIniPath = Join-Path $repoRoot "platformio.hardware.ini"
if (-not (Test-Path $hardwareIniPath)) {
  Write-ErrorMsg "platformio.hardware.ini not found!"
  exit 1
}

$iniContent = Get-Content $hardwareIniPath -Raw

# Find ALL sections matching the feature name pattern (case-insensitive)
# Pattern: [neopixel_oknxhw_...] for NeoPixel feature
$featureNameLower = $FeatureName.ToLower()
$sectionPattern = "\[${featureNameLower}_oknxhw_([^\]]+)\]"
$sectionMatches = [regex]::Matches($iniContent, $sectionPattern, [System.Text.RegularExpressions.RegexOptions]::IgnoreCase)

$hardwareConfigs = @()

foreach ($match in $sectionMatches) {
  # Extract full section name (case-preserved)
  $sectionStartPos = $match.Index
  $sectionLineStart = $iniContent.LastIndexOf("`n", $sectionStartPos) + 1
  $sectionBracketEnd = $iniContent.IndexOf("]", $sectionStartPos)
  $sectionName = $iniContent.Substring($sectionLineStart + 1, $sectionBracketEnd - $sectionLineStart - 1)
    
  Write-Step "Found hardware config: $sectionName"
    
  # Extract section content
  $nextSectionPos = $iniContent.IndexOf("`n[", $sectionBracketEnd)
  if ($nextSectionPos -eq -1) { $nextSectionPos = $iniContent.Length }
    
  $sectionContent = $iniContent.Substring($sectionBracketEnd + 1, $nextSectionPos - $sectionBracketEnd - 1)
    
  # Extract build_flags
  $buildFlags = @()
  if ($sectionContent -match "(?s)build_flags\s*=(.*?)(?=\n\S|\z)") {
    $buildFlagsText = $matches[1]
        
    # Parse each -D flag
    $defineMatches = [regex]::Matches($buildFlagsText, "-D([^\s]+)")
    foreach ($defineMatch in $defineMatches) {
      $defineValue = $defineMatch.Groups[1].Value
      # Escape quotes for shell: KEY="VALUE" -> KEY=\"VALUE\"
      $defineValue = $defineValue -replace '"', '\"'
      $buildFlags += "-D$defineValue"
    }
  }
    
  $hardwareConfigs += @{
    Name        = $sectionName
    SectionName = $sectionName
    BuildFlags  = $buildFlags
    GPIOPorts   = @()
  }
}

if ($hardwareConfigs.Count -eq 0) {
  Write-Warning "No hardware configurations found matching pattern: ${DefinesPrefix}_*"
  Write-Warning "Falling back to manual GPIO mode only"
  $gpioPorts = @()
}
else {
  Write-Success "Found $($hardwareConfigs.Count) hardware configuration(s)"
}

if ($hardwareConfigs.Count -eq 0) {
  Write-Warning "No hardware configurations found matching pattern: ${DefinesPrefix}_*"
  Write-Warning "Falling back to manual GPIO mode only"
  $gpioPorts = @()
}
else {
  Write-Success "Found $($hardwareConfigs.Count) hardware configuration(s)"
}

if ($VerboseMode) {
  Write-Host "[DEBUG-CHECKPOINT-2] After HW Scan: Markers.ParamTypeStart = '$($script:Config.Markers.ParamTypeStart)'" -ForegroundColor Cyan
}

# Step 2: For EACH hardware config, extract GPIO configuration using preprocessor only
Write-Step "Step 2: Extracting GPIO configurations from hardware..."

# Detect C++ compiler (we only need the preprocessor)
$compiler = $null
$pioCompiler = Find-PlatformIOCompiler
if ($pioCompiler) {
  $compiler = $pioCompiler
  Write-Step "Found PlatformIO compiler: $(Split-Path -Leaf $compiler)"
}

if (-not $compiler) {
  $systemCompilers = @("g++", "clang++", "cpp")
  foreach ($cmd in $systemCompilers) {
    try {
      $null = Get-Command $cmd -ErrorAction SilentlyContinue
      if ($?) {
        $compiler = $cmd
        Write-Step "Using system compiler: $compiler"
        break
      }
    }
    catch {}
  }
}

if (-not $compiler) {
  Write-ErrorMsg "No C++ compiler/preprocessor found!"
  exit 1
}

# Include paths for hardware config resolution
$includeHardwareConfig = Join-Path $repoRoot "lib/OGM-HardwareConfig/include"

# Process each hardware configuration
for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $hwConfig = $hardwareConfigs[$hwIdx]
  $hwName = $hwConfig.Name
    
  Write-Host ""
  Write-Step "[$($hwIdx + 1)/$($hardwareConfigs.Count)] Processing: $hwName"
    
  # Use preprocessor only (no compilation needed!)
  # This is much simpler and avoids linking/compiler issues
  $preprocessorArgs = @(
    "-E"  # Preprocessor only
    "-P"  # No line markers
    "-I$includeHardwareConfig"
    "-DFEATURE_PREFIX=$DefinesPrefix"
  )
    
  # Add hardware-specific build flags
  $preprocessorArgs += $hwConfig.BuildFlags
    
  # DEBUG: Show preprocessor command
  if ($DryRun) {
    $hwDefines = $hwConfig.BuildFlags | Where-Object { $_ -match "^-DOKNXHW" }
    Write-Host "    [DEBUG] HW Defines: $($hwDefines -join ' ')" -ForegroundColor Magenta
  }
    
  # Add simple extractor source
  $simpleExtractor = Join-Path $scriptDir "extract_gpio_config_simple.cpp"
  $preprocessorArgs += $simpleExtractor
    
  # Run preprocessor
  try {
    $extractorOutput = & $compiler $preprocessorArgs 2>&1
        
    if ($LASTEXITCODE -ne 0) {
      Write-Warning "  Preprocessing failed for $hwName - skipping"
      Write-Host "  Debug: $($extractorOutput | Select-Object -First 3)" -ForegroundColor DarkGray
      continue
    }
        
    # Parse output
    $hwData = @{}
    foreach ($line in $extractorOutput) {
      if ($line -match '^([A-Z_0-9]+)=(.+)$') {
        $hwData[$matches[1]] = $matches[2]
      }
    }
        
    # DEBUG: Show extracted values
    if ($DryRun) {
      $deviceBitLines = $extractorOutput | Where-Object { $_ -match "DEVICE_ID_BIT" }
      if ($deviceBitLines) {
        Write-Host "    [DEBUG] DEVICE_ID_BIT lines: $($deviceBitLines -join ' | ')" -ForegroundColor Yellow
      }
      else {
        Write-Host "    [DEBUG] No DEVICE_ID_BIT found in extractor output" -ForegroundColor Red
      }
    }
        
    # Store device info in hwConfig (concatenate multiple quoted strings)
    $hwConfig.DeviceId = if ($hwData.ContainsKey("HW_DEVICE_ID")) { 
      # Concatenate multiple string literals: "A" "B" -> "AB"
      $deviceId = $hwData["HW_DEVICE_ID"] -replace '"\s*"', ''  # Remove quotes between strings
      $deviceId -replace '"', ''  # Remove all quotes
    }
    else { 
      $hwName 
    }
    $hwConfig.DeviceName = if ($hwData.ContainsKey("HW_DEVICE_NAME")) { 
      $deviceName = $hwData["HW_DEVICE_NAME"] -replace '"\s*"', ''
      $deviceName -replace '"', ''
    }
    else { 
      $hwName 
    }
    $hwConfig.DeviceIdBit = if ($hwData.ContainsKey("HW_DEVICE_ID_BIT")) {
      # Parse hex value (0x1001 -> 4097)
      $hexValue = $hwData["HW_DEVICE_ID_BIT"]
      [int]($hexValue)
    }
    else {
      0
    }
        
    # DEBUG: Show extracted device info
    Write-Host "    Device: $($hwConfig.DeviceId) - $($hwConfig.DeviceName) [0x$($hwConfig.DeviceIdBit.ToString('X4'))]" -ForegroundColor Cyan
        
    # Extract GPIO ports
    $hwPortsAvailable = $hwData["HW_GPIO_PORTS_DEFINED"] -eq "1"
    $portCount = [int]$hwData["HW_GPIO_PORT_COUNT"]
        
    if ($hwPortsAvailable -and $portCount -gt 0) {
      Write-Success "  Found $portCount GPIO port(s)"
            
      for ($i = 1; $i -le $portCount; $i++) {
        $gpioKey = "HW_GPIO_PORT_${i}_GPIO_DATA"
        $labelKey = "HW_GPIO_PORT_${i}_LABEL"
                
        if ($hwData.ContainsKey($gpioKey)) {
          $label = if ($hwData.ContainsKey($labelKey)) { 
            # Remove quotes and multiple backslashes from preprocessor output
            $hwData[$labelKey] -replace '"', '' -replace '\\+', ''
          }
          else { 
            "Port $i" 
          }
                    
          # Extract GPIO number for JavaScript (even though C++ uses NEOPIXEL_HW_PORT_{index}_GPIO)
          $gpioKey = "NEOPIXEL_HW_PORT_${i}_GPIO"
          $gpioNum = if ($hwData.ContainsKey($gpioKey)) {
            $hwData[$gpioKey] -replace '"', '' -replace '\\+', ''
          }
          else {
            -1  # Unknown GPIO
          }
                    
          $hwConfig.GPIOPorts += @{
            Index = $i
            Label = $label
            GPIO  = $gpioNum
          }
                    
          Write-Host "    • Port ${i}: $label" -ForegroundColor DarkGray
        }
      }
    }
    else {
      Write-Warning "  No GPIO ports defined for $hwName"
    }
  }
  catch {
    Write-Warning "  Error processing ${hwName}: $_"
  }
}

Write-Host ""
if ($VerboseMode) {
  Write-Host "[DEBUG-CHECKPOINT-3] After HW Processing: Markers.ParamTypeStart = '$($script:Config.Markers.ParamTypeStart)'" -ForegroundColor Cyan
}
Write-Step "Step 3: Filtering hardware and generating XML structures..."

# Filter out hardware without DEVICE_HW_ID (DeviceIdBit == 0)
$hardwareConfigsFiltered = @()
$hardwareConfigsSkipped = @()

foreach ($hwConfig in $hardwareConfigs) {
  if ($hwConfig.DeviceIdBit -eq 0) {
    $hardwareConfigsSkipped += $hwConfig
    Write-Warning "  Hardware '$($hwConfig.Name)' has no DEVICE_HW_ID - skipping ETS generation"
  }
  else {
    $hardwareConfigsFiltered += $hwConfig
  }
}

# Replace hardwareConfigs with filtered list for ETS generation
$hardwareConfigs = $hardwareConfigsFiltered

if ($hardwareConfigsSkipped.Count -gt 0) {
  Write-Host "  Skipped $($hardwareConfigsSkipped.Count) hardware(s) without HW_ID" -ForegroundColor DarkYellow
}

if ($hardwareConfigs.Count -eq 0) {
  Write-Warning "No hardware configurations with DEVICE_HW_ID found"
  Write-Warning "Please add DEVICE_HW_ID defines to hardware headers"
  exit 1
}

Write-Success "Using $($hardwareConfigs.Count) hardware variant(s) with HW_ID for ETS"

# Check if we have any hardware configs with GPIO ports
$hardwareConfigsWithPorts = @($hardwareConfigs | Where-Object { $_.GPIOPorts.Count -gt 0 })

if ($hardwareConfigsWithPorts.Count -eq 0) {
  Write-Warning "No hardware configurations with GPIO ports found"
  Write-Warning "Generating manual-only configuration"
  $gpioPorts = @()
}
else {
  Write-Success "Successfully configured $($hardwareConfigsWithPorts.Count) hardware variant(s)"
}

# Calculate bit size needed for hardware selection
$bitsNeededForHardware = [Math]::Ceiling([Math]::Log($hardwareConfigs.Count) / [Math]::Log(2))
if ($bitsNeededForHardware -lt 1) { $bitsNeededForHardware = 1 }

Write-Step "Hardware Selection requires $bitsNeededForHardware bit(s) for $($hardwareConfigs.Count) variant(s)"

if ($VerboseMode) {
  Write-Host "[DEBUG-CHECKPOINT-4] Before ParameterType Gen: Markers.ParamTypeStart = '$($script:Config.Markers.ParamTypeStart)'" -ForegroundColor Cyan
}

# Step 4: Generate Hardware Selection + Hardware-Specific GPIO Port ParameterTypes
Write-Step "Generating ParameterTypes (Hardware Selection + Hardware-Specific GPIO Ports)..."

$parameterTypeXml = ""

# Generate Hardware Selection ParameterType
if ($hardwareConfigs.Count -gt 1) {
  $bitsNeededForHardware = 16  # Force 16 bits for DEVICE_ID_BIT values
  $parameterTypeXml += @"
<!-- Hardware Selection -->
<ParameterType Id="%AID%_PT-${FeatureName}HardwareSelect" Name="${FeatureName}HardwareSelect">
  <TypeRestriction Base="Value" SizeInBit="$bitsNeededForHardware">

"@
    
  for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
    $hwConfig = $hardwareConfigs[$hwIdx]
    $hwName = $hwConfig.DeviceName
    $deviceIdBit = $hwConfig.DeviceIdBit
    $parameterTypeXml += "    <Enumeration Text=`"${hwName}`" Value=`"${deviceIdBit}`" Id=`"%ENID%`" />`n"
  }
    
  $parameterTypeXml += @"
  </TypeRestriction>
</ParameterType>
"@
  Write-Success "Generated Hardware Selection ParameterType"
}

# Generate Hardware-Specific GPIO Port ParameterTypes
Write-Step "Generating hardware-specific GPIO Port ParameterTypes..."

for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $hwConfig = $hardwareConfigs[$hwIdx]
  $hwName = $hwConfig.Name -replace "^${DefinesPrefix}_", ""
  $portCount = $hwConfig.GPIOPorts.Count
    
  # Calculate bits needed for this hardware's port count + Manual
  $maxValue = [Math]::Max($portCount, 10)  # Manual = 10
  $bitsNeeded = [Math]::Ceiling([Math]::Log($maxValue + 1) / [Math]::Log(2))
  if ($bitsNeeded -lt 4) { $bitsNeeded = 4 }
    
  $parameterTypeXml += @"

<!-- GPIO Port Selection for HW${hwIdx}: ${hwName} (${portCount} port(s)) -->
<ParameterType Id="%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}" Name="${FeatureName}GPIOPortHW${hwIdx}">
  <TypeRestriction Base="Value" SizeInBit="$bitsNeeded">
"@
    
  # Add DUMMY option first (value 15) - always shown, never conflicts
  $dummyEnumId = "%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}_EN-15"
  $parameterTypeXml += "    <Enumeration Text=`"--- Wählen Sie bitte ein Port aus ---`" Value=`"15`" Id=`"${dummyEnumId}`" />`n"
    
  # Add port enumerations (0 to portCount-1)
  for ($portIdx = 0; $portIdx -lt $portCount; $portIdx++) {
    $gpioPort = $hwConfig.GPIOPorts[$portIdx]
    $gpioPortLabel = $gpioPort.Label  # Extract Label from Hashtable
    $enumId = "%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}_EN-${portIdx}"
    $parameterTypeXml += "    <Enumeration Text=`"${gpioPortLabel}`" Value=`"${portIdx}`" Id=`"${enumId}`" />`n"
  }
    
  # Add "Manuell" option with value 10
  $manualEnumId = "%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}_EN-10"
  $parameterTypeXml += "    <Enumeration Text=`"Manuell`" Value=`"10`" Id=`"${manualEnumId}`" />`n"
    
  $parameterTypeXml += @"
  </TypeRestriction>
</ParameterType>
"@
}

Write-Success "Generated $($hardwareConfigs.Count) hardware-specific GPIO Port ParameterTypes"

# Step 4.8: Generate Hardware Selection Parameter (for share.xml)
Write-Step "Generating Hardware Selection Parameter..."
$hardwareParamXml = ""

if ($hardwareConfigs.Count -gt 1) {
  $defaultHwId = $hardwareConfigs[0].DeviceIdBit
  $hardwareParamXml = "                <Parameter Id=`"%AID%_UP-4000018`" Offset=`"0`" BitOffset=`"0`" Name=`"NEO_NeoPixelHardwareSelect`" ParameterType=`"%AID%_PT-${FeatureName}HardwareSelect`" Text=`"Hardware Auswahl`" Value=`"$defaultHwId`"/>"
  Write-Success "Generated Hardware Selection Parameter (4000018) with default: $defaultHwId"
}

# Step 4.9: Generate 8 separate GPIO Port Parameters (one per hardware, unique IDs)
Write-Step "Generating 8 separate GPIO Port Parameters (for Template)..."

# Generate 8 separate Parameters, each with unique ID (073, 074, 075, ..., 080)
$gpioPortParamXml = ""
for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $hwConfig = $hardwareConfigs[$hwIdx]
  $hwName = $hwConfig.DeviceName
  $portCount = $hwConfig.GPIOPorts.Count
  $paramId = 73 + $hwIdx
    
  if ($hwIdx -gt 0) {
    $gpioPortParamXml += "`n"
  }
  $gpioPortParamXml += @"
<Parameter Id="%AID%_UP-%TT%9%C%0$paramId" Offset="5" BitOffset="4" Name="NEO%C%GPIODataPortHW$hwIdx" ParameterType="%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}" Text="Daten Port" Value="15"/>
"@
}

Write-Success "Generated 8 separate GPIO Port Parameters (IDs: 073-080, one per hardware)"
Write-Host "  Each parameter has unique ID and hardware-specific ParameterType" -ForegroundColor DarkGray

# Step 4.9.1: Generate 8 separate Clock GPIO Parameters (for SPI, IDs 081-088)
Write-Step "Generating 8 separate Clock GPIO Parameters (for Template, SPI only)..."

$gpioClockParamXml = ""
for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $hwConfig = $hardwareConfigs[$hwIdx]
  $hwName = $hwConfig.DeviceName
  $portCount = $hwConfig.GPIOPorts.Count
  $paramId = 81 + $hwIdx
    
  if ($hwIdx -gt 0) {
    $gpioClockParamXml += "`n"
  }
  $gpioClockParamXml += @"
<Parameter Id="%AID%_UP-%TT%9%C%0$paramId" Offset="23" BitOffset="0" Name="NEO%C%GPIOClockPortHW$hwIdx" ParameterType="%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}" Text="Clock Port" Value="15"/>
"@
}

Write-Success "Generated 8 separate Clock GPIO Parameters (IDs: 081-088, for SPI LEDs)"
Write-Host "  Each parameter uses same ParameterType as Data GPIO" -ForegroundColor DarkGray

# Step 4.9.5: Generate 8 separate ParameterRefs (one for each Parameter)
Write-Step "Generating 8 separate GPIO Port ParameterRefs (for Template)..."

$gpioPortParamRefsXml = ""
for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $paramId = 73 + $hwIdx
  $refId = "${paramId}01"
  
  if ($hwIdx -gt 0) {
    $gpioPortParamRefsXml += "`n"
  }
  $gpioPortParamRefsXml += "<ParameterRef Id=`"%AID%_UP-%TT%9%C%0${paramId}_R-%TT%9%C%0${refId}`" RefId=`"%AID%_UP-%TT%9%C%0${paramId}`" />"
}

Write-Success "Generated 8 separate GPIO Port ParameterRefs (IDs: 07301-08001)"
Write-Host "  Each ParameterRef references its corresponding Parameter" -ForegroundColor DarkGray

# Step 4.9.6: Generate 8 separate Clock GPIO ParameterRefs
Write-Step "Generating 8 separate Clock GPIO ParameterRefs (for Template)..."

$gpioClockParamRefsXml = ""
for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $paramId = 81 + $hwIdx
  $refId = "${paramId}01"
  
  if ($hwIdx -gt 0) {
    $gpioClockParamRefsXml += "`n"
  }
  $gpioClockParamRefsXml += "<ParameterRef Id=`"%AID%_UP-%TT%9%C%0${paramId}_R-%TT%9%C%0${refId}`" RefId=`"%AID%_UP-%TT%9%C%0${paramId}`" />"
}

Write-Success "Generated 8 separate Clock GPIO ParameterRefs (IDs: 08101-08801)"
Write-Host "  Each ParameterRef references its corresponding Clock Parameter" -ForegroundColor DarkGray

# Step 4.10: Generate ParameterRefs (for share.xml)
Write-Step "Generating ParameterRefs for share.xml..."
$hardwareParamRefXml = ""

if ($hardwareConfigs.Count -gt 1) {
  $hardwareParamRefXml = "<ParameterRef Id=`"%AID%_UP-4000018_R-400001801`" RefId=`"%AID%_UP-4000018`" />"
  Write-Success "Generated Hardware Selection ParameterRef"
}

# GPIO Port ParameterRefs are in Template (per-strip), not in share.xml
Write-Success "GPIO Port ParameterRefs will be generated in Template (per-strip)"

# ====================================================================================================
# Step 4.11: Generate STATIC Conflict Result Parameters in share.xml (NOT in template!)
# ====================================================================================================
Write-Step "Generating STATIC GPIO Conflict Result Parameters (share.xml)..."

# Generate UNION with 18 boolean flags (6 strips × 3 flags)
# Like Virtual Strip: Union SizeInBit with Memory CodeSegment + UP Parameters
# IMPORTANT: Parameters MUST have %TT% token like VirtualStrip (required by OpenKNX format)
$shareConflictParamsXml = "<Union SizeInBit=`"24`">`n"
$shareConflictParamsXml += "  <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"41`" BitOffset=`"0`" />`n"

$byteOffset = 0
for ($stripIdx = 1; $stripIdx -le 6; $stripIdx++) {
  $baseId = 87 + ($stripIdx * 3)  # 90, 93, 96, 99, 102, 105
    
  $shareConflictParamsXml += "  <!-- Strip $stripIdx Conflict Flags -->`n"
  $shareConflictParamsXml += "  <Parameter Id=`"%AID%_UP-%TT%$(($baseId).ToString('00000'))`" Offset=`"$byteOffset`" BitOffset=`"0`" Name=`"Strip${stripIdx}HasDataConflict`" ParameterType=`"%AID%_PT-ConflictFlag`" Text=`"Strip $stripIdx Data Konflikt`" Value=`"0`" Access=`"Read`"/>`n"
  $shareConflictParamsXml += "  <Parameter Id=`"%AID%_UP-%TT%$(($baseId+1).ToString('00000'))`" Offset=`"$byteOffset`" BitOffset=`"1`" Name=`"Strip${stripIdx}HasClockConflict`" ParameterType=`"%AID%_PT-ConflictFlag`" Text=`"Strip $stripIdx Clock Konflikt`" Value=`"0`" Access=`"Read`"/>`n"
  $shareConflictParamsXml += "  <Parameter Id=`"%AID%_UP-%TT%$(($baseId+2).ToString('00000'))`" Offset=`"$byteOffset`" BitOffset=`"2`" Name=`"Strip${stripIdx}HasCrossConflict`" ParameterType=`"%AID%_PT-ConflictFlag`" Text=`"Strip $stripIdx Cross Konflikt`" Value=`"0`" Access=`"Read`"/>`n"
    
  $byteOffset = $byteOffset + 1
}
$shareConflictParamsXml += "</Union>"

# Generate ParameterRefs for union parameters (also need %TT% token like VirtualStrip!)
$shareConflictParamRefsXml = ""
for ($stripIdx = 1; $stripIdx -le 6; $stripIdx++) {
  $baseId = 87 + ($stripIdx * 3)
    
  $shareConflictParamRefsXml += "<!-- Strip $stripIdx Conflict Flags -->`n"
  $shareConflictParamRefsXml += "<ParameterRef Id=`"%AID%_UP-%TT%$(($baseId).ToString('00000'))_R-%TT%$(($baseId).ToString('00000'))01`" RefId=`"%AID%_UP-%TT%$(($baseId).ToString('00000'))`" />`n"
  $shareConflictParamRefsXml += "<ParameterRef Id=`"%AID%_UP-%TT%$(($baseId+1).ToString('00000'))_R-%TT%$(($baseId+1).ToString('00000'))01`" RefId=`"%AID%_UP-%TT%$(($baseId+1).ToString('00000'))`" />`n"
  $shareConflictParamRefsXml += "<ParameterRef Id=`"%AID%_UP-%TT%$(($baseId+2).ToString('00000'))_R-%TT%$(($baseId+2).ToString('00000'))01`" RefId=`"%AID%_UP-%TT%$(($baseId+2).ToString('00000'))`" />`n"
}

Write-Success "Generated 18 UNION Conflict Parameters in share.xml (IDs 00090-00107)"
Write-Host "  • Strip 1: 00090-00092, Strip 2: 00093-00095, Strip 3: 00096-00098" -ForegroundColor DarkGray
Write-Host "  • Strip 4: 00099-00101, Strip 5: 00102-00104, Strip 6: 00105-00107" -ForegroundColor DarkGray
Write-Host "  • These are UNION (UP) like Virtual Strip - ParameterCalculation can write!" -ForegroundColor Yellow

# ====================================================================================================
# Step 4.12: SKIP - ConflictFlag ParameterType manually in share.xml
# ====================================================================================================
# ConflictFlag ParameterType is manually placed in share.xml BEFORE auto-generated section

# ====================================================================================================
# Step 4.13: Insert UNION Conflict Parameters into share.xml (like Virtual Strip!)
# ====================================================================================================
Write-Step "Inserting UNION Conflict Parameters into share.xml..."

$shareXmlPath = Resolve-RepoPath $ShareXml
if (-not (Test-Path $shareXmlPath)) {
  Write-ErrorMsg "Share XML not found: $shareXmlPath"
  exit 1
}

$shareContent = Get-Content -Path $shareXmlPath -Raw

# Insert STATIC Conflict Parameters
$startMarker = $script:Config.Markers.ConflictParamsStart
$endMarker = $script:Config.Markers.ConflictParamsEnd

if ($shareContent -match [regex]::Escape($startMarker)) {
  $escapedStart = [regex]::Escape($startMarker)
  $escapedEnd = [regex]::Escape($endMarker)
  $pattern = "(?ms)(^[ \t]*)($escapedStart`r?`n)(.*?)(`r?`n\1$escapedEnd)"
  $indent = $Matches[1]
  $indentedXml = ($shareConflictParamsXml -split "`n" | ForEach-Object {
      if ($_ -ne "") { "$indent$_" }
    }) -join "`n"
  $replacement = "`$1`$2$indentedXml`$4"
  $shareContent = $shareContent -replace $pattern, $replacement
    
  if ($DryRun) {
    Write-Host "  [DRY-RUN] Would insert 18 UNION Conflict Parameters into share.xml" -ForegroundColor Yellow
  }
  else {
    Set-Content -Path $shareXmlPath -Value $shareContent -NoNewline
    Write-Success "Inserted 18 UNION Conflict Parameters into share.xml"
  }
}
else {
  Write-Warning "GPIO Conflict Parameters markers not found in share.xml"
}

# ====================================================================================================
# Step 4.14: Generate GPIO Conflict Detection (SHARE.XML IMPLEMENTATION)
# ====================================================================================================
Write-Step "Generating GPIO Conflict Detection System..."

# Generate GPIO Port Parameters in share.xml (00101-00106)
Write-Host "  • Generating share.xml GPIO Port Parameters (00101-00106)..." -ForegroundColor Cyan
if (Generate-GPIOPortParametersInShare -ShareXmlPath $shareXmlPath -NumStrips 6) {
  Write-Success "Share.xml GPIO Port Parameters generated (6 strips)"
}
else {
  Write-Warning "Share.xml GPIO Port Parameter markers not found - skipped"
}

# Generate Clock GPIO Port Parameters in share.xml (00111-00116)
Write-Host "  • Generating share.xml Clock GPIO Port Parameters (00111-00116)..." -ForegroundColor Cyan
if (Generate-ClockPortParametersInShare -ShareXmlPath $shareXmlPath -NumStrips 6) {
  Write-Success "Share.xml Clock GPIO Port Parameters generated (6 strips)"
}
else {
  Write-Warning "Share.xml Clock GPIO Port Parameter markers not found - skipped"
}

# Generate Conflict Parameters in share.xml (00091-00096)
Write-Host "  • Generating share.xml Conflict Parameters (00091-00096)..." -ForegroundColor Cyan
if (Generate-ConflictParametersInShare -ShareXmlPath $shareXmlPath -NumStrips 6) {
  Write-Success "Share.xml Conflict Parameters generated (6 strips)"
}
else {
  Write-Warning "Share.xml Conflict Parameter markers not found - skipped"
}

# Generate Clock Conflict Parameters in share.xml (00117-00122)
Write-Host "  • Generating share.xml Clock Conflict Parameters (00117-00122)..." -ForegroundColor Cyan
if (Generate-ClockConflictParametersInShare -ShareXmlPath $shareXmlPath -NumStrips 6) {
  Write-Success "Share.xml Clock Conflict Parameters generated (6 strips)"
}
else {
  Write-Warning "Share.xml Clock Conflict Parameter markers not found - skipped"
}

# Generate Conflict ParameterRefs in share.xml
Write-Host "  • Generating share.xml Conflict ParameterRefs..." -ForegroundColor Cyan
if (Generate-ConflictParameterRefsInShare -ShareXmlPath $shareXmlPath -NumStrips 6) {
  Write-Success "Share.xml Conflict ParameterRefs generated"
}
else {
  Write-Warning "Share.xml Conflict ParameterRefs markers not found - skipped"
}

# Generate SINGLE ParameterCalculation in share.xml
Write-Host "  • Generating ParameterCalculation in share.xml..." -ForegroundColor Cyan
if (Generate-ConflictCalculation -ShareXmlPath $shareXmlPath -NumStrips 6) {
  Write-Success "ParameterCalculation generated in share.xml (reads 00101-00106, writes 00091-00096)"
}
else {
  Write-Warning "ParameterCalculation markers not found - skipped"
}

# Generate Hardware Change Reset ParameterCalculation
Write-Host "  • Generating Hardware Change Reset ParameterCalculation..." -ForegroundColor Cyan
if (Generate-HardwareChangeResetCalculation -ShareXmlPath $shareXmlPath -NumStrips 6) {
  Write-Success "Hardware Change Reset ParameterCalculation generated (resets all ports to 15)"
}
else {
  Write-Warning "Hardware Change Reset markers not found - skipped"
}

# Generate JavaScript Detection Function
Write-Host "  • Generating JavaScript (NEO_DetectGPIOConflicts)..." -ForegroundColor Cyan
if (Generate-ConflictDetectionJS -JavaScriptPath $scriptJsPath -NumStrips 6) {
  Write-Success "JavaScript conflict detection generated"
}
else {
  Write-Warning "JavaScript generation failed"
}

# Generate GPIO Copy ParameterCalculation in Template (9%C%073 → 0010%C%)
Write-Host "  • Generating GPIO Copy ParameterCalculation in Template..." -ForegroundColor Cyan
if (Generate-GPIOCopyCalculation -TemplatePath $templatePath -HardwareConfigs $hardwareConfigs) {
  Write-Success "Template GPIO Copy Calculation generated (dynamic hardware mapping)"
}
else {
  Write-Warning "Template GPIO Copy Calculation markers not found - skipped"
}

# Generate Clock GPIO Copy ParameterCalculation in Template (9%C%081 → 0011%C%)
Write-Host "  • Generating Clock GPIO Copy ParameterCalculation in Template..." -ForegroundColor Cyan
if (Generate-GPIOClockCopyCalculation -TemplatePath $templatePath -HardwareConfigs $hardwareConfigs) {
  Write-Success "Template Clock GPIO Copy Calculation generated (dynamic hardware mapping)"
}
else {
  Write-Warning "Template Clock GPIO Copy Calculation markers not found - skipped"
}

# Generate UI in template (references share.xml parameters with %C% token)
Write-Host "  • Generating Conflict UI for Template..." -ForegroundColor Cyan
if (Generate-ConflictUI -TemplatePath $templatePath) {
  Write-Success "Template UI generated (references share.xml 0009%C%)"
}
else {
  Write-Warning "Template UI markers not found - skipped"
}

Write-Success "GPIO Conflict Detection System Complete"
Write-Host "  • share.xml: Parameters 00091-00096, Calculation reads 00101-00106" -ForegroundColor DarkGray
Write-Host "  • Template: UI references 0009%C% → Strip1=00091, Strip2=00092, etc." -ForegroundColor DarkGray

# ====================================================================================================
# Step 5: Update share.xml with ParameterCalculation
Write-Step "Generating GPIO Conflict ParameterCalculation for share.xml..."

$shareContent = Get-Content -Path $shareXmlPath -Raw

if ($VerboseMode) {
  Write-Host "  [DEBUG] Config.Markers.ParamTypeStart = '$($script:Config.Markers.ParamTypeStart)'" -ForegroundColor Magenta
  Write-Host "  [DEBUG] Config.Markers.ParamTypeEnd = '$($script:Config.Markers.ParamTypeEnd)'" -ForegroundColor Magenta
}

$startMarker = $script:Config.Markers.ParamTypeStart
$endMarker = $script:Config.Markers.ParamTypeEnd

if ($VerboseMode) {
  Write-Host "  [DEBUG] startMarker = '$startMarker'" -ForegroundColor Magenta
  Write-Host "  [DEBUG] endMarker = '$endMarker'" -ForegroundColor Magenta
}

# Pattern WITH leading whitespace capture to preserve marker indentation
$escapedStart = [regex]::Escape($startMarker)
$escapedEnd = [regex]::Escape($endMarker)
$pattern = "(?ms)(^[ \t]*)($escapedStart`r?`n)(.*?)(`r?`n\1$escapedEnd)"

if ($VerboseMode) {
  Write-Host "  [DEBUG] DryRun = $DryRun" -ForegroundColor Magenta
  Write-Host "  [DEBUG] Pattern: $pattern" -ForegroundColor Magenta
  Write-Host "  [DEBUG] Pattern length: $($pattern.Length)" -ForegroundColor Magenta
}

# DEBUG: Check how many times the pattern matches
if ($DryRun -or $Debug) {
  Write-Host "  [DEBUG] Start marker: '$startMarker'" -ForegroundColor Yellow
  Write-Host "  [DEBUG] End marker: '$endMarker'" -ForegroundColor Yellow  
  Write-Host "  [DEBUG] Pattern: $pattern" -ForegroundColor Yellow
  $matches = [regex]::Matches($shareContent, $pattern)
  Write-Host "  [DEBUG] Pattern matches $($matches.Count) time(s) in share.xml" -ForegroundColor $(if ($matches.Count -ne 1) { 'Red' }else { 'Green' })
  if ($matches.Count -gt 1) {
    Write-Host "  [DEBUG] THIS IS THE BUG - Pattern should match exactly ONCE!" -ForegroundColor Red
    for ($i = 0; $i -lt $matches.Count; $i++) {
      #Write-Host "  [DEBUG]   Match $($i+1) at position $($matches[$i].Index), length $($matches[$i].Length)" -ForegroundColor Yellow
    }
  }
}

if (Replace-MarkerContent -FilePath $shareXmlPath -StartMarker $startMarker -EndMarker $endMarker -NewContent $parameterTypeXml) {
  if ($DryRun) {
    Write-Host "  [DRY-RUN] Would update share.xml with ParameterTypes" -ForegroundColor Yellow
    Write-Host "    Content to insert: $($parameterTypeXml.Length) chars" -ForegroundColor DarkGray
    Write-Host "    First 200 chars of generated content:" -ForegroundColor DarkGray
    Write-Host "    $($parameterTypeXml.Substring(0, [Math]::Min(200, $parameterTypeXml.Length)))" -ForegroundColor DarkGray
  }
  else {
    Write-Success "ParameterTypes updated in share.xml"
  }
}
else {
  Write-ErrorMsg "Markers not found in share.xml: $startMarker / $endMarker"
  exit 1
}

# Step 5.5: Update share.xml with Hardware Selection Parameter
Write-Step "Updating share.xml with Hardware Selection Parameter..."

# Reload share.xml content after previous update
$shareContent = Get-Content -Path $shareXmlPath -Raw

if ($hardwareParamXml -ne "") {
  $startMarker = $script:Config.Markers.HardwareParamStart
  $endMarker = $script:Config.Markers.HardwareParamEnd
  $escapedStart = [regex]::Escape($startMarker)
  $escapedEnd = [regex]::Escape($endMarker)
  $pattern = "(?ms)(^[ \t]*)($escapedStart`r?`n)(.*?)(`r?`n\1$escapedEnd)"
    
  if ($shareContent -match $pattern) {
    $replacement = "`$1`$2$hardwareParamXml`$4"
    $shareContent = $shareContent -replace $pattern, $replacement
    Set-Content -Path $shareXmlPath -Value $shareContent -NoNewline
    Write-Success "Hardware Selection Parameter updated in share.xml"
  }
  else {
    Write-ErrorMsg "Hardware Parameter markers not found in share.xml"
    exit 1
  }
}
else {
  Write-Success "Skipped Hardware Selection Parameter (only 1 hardware)"
}

# Step 5.6: Update share.xml with Hardware Selection ParameterRef
Write-Step "Updating share.xml with Hardware Selection ParameterRef..."

if ($hardwareParamRefXml -ne "") {
  $startMarker = $script:Config.Markers.HardwareParamRefStart
  $endMarker = $script:Config.Markers.HardwareParamRefEnd
  
  if (Replace-MarkerContent -FilePath $shareXmlPath -StartMarker $startMarker -EndMarker $endMarker -NewContent $hardwareParamRefXml) {
    Write-Success "Hardware Selection ParameterRef updated in share.xml"
  }
  else {
    Write-ErrorMsg "Hardware ParameterRef markers not found in share.xml"
    exit 1
  }
}
else {
  Write-Success "Skipped Hardware Selection ParameterRef (only 1 hardware)"
}

# ====================================================================================================
# Step 5.7: Update share.xml with Hardware Selection ParameterRefRef (UI Display)
Write-Step "Updating share.xml with Hardware Selection ParameterRefRef (UI)..."

$startMarker = $script:Config.Markers.HardwareParamRefRefStart
$endMarker = $script:Config.Markers.HardwareParamRefRefEnd

if ($hardwareConfigs.Count -gt 1) {
  # Multiple hardware: Show Hardware Selection ParameterRefRef
  $indent = "                "  # 16 spaces
  $hwParamRefRefXml = "${indent}<ParameterRefRef RefId=`"%AID%_UP-4000018_R-400001801`" IndentLevel=`"1`" />"
}
else {
  # Single hardware: Empty content
  $hwParamRefRefXml = "                "  # Just indentation for clean structure
}

if (Replace-MarkerContent -FilePath $shareXmlPath -StartMarker $startMarker -EndMarker $endMarker -NewContent $hwParamRefRefXml) {
  if ($hardwareConfigs.Count -gt 1) {
    Write-Success "Hardware Selection ParameterRefRef (UI) updated in share.xml"
  }
  else {
    Write-Success "Hardware Selection ParameterRefRef cleaned (only 1 hardware)"
  }
}
else {
  Write-ErrorMsg "Hardware Selection ParameterRefRef markers not found in share.xml"
  exit 1
}

# Step 6: Generate Hardware-Specific GPIO Port Selection ParameterRefRef (UI Choose-blocks)
Write-Step "Generating Hardware-Specific GPIO Port Selection ParameterRefRef (UI Choose-blocks)..."

# Hardware-specific ParameterRefRef with Choose-blocks for UI display
$gpioSelectionXml = Generate-HardwareSpecificGPIOSelectionUI -Type "Data" -HardwareConfigs $hardwareConfigs

Write-Success "Generated hardware-specific GPIO Port Selection ParameterRefRef with Choose-blocks"
Write-Host "  UI shows different GPIO options for each hardware variant" -ForegroundColor Green

# Step 6.1: Generate Hardware-Specific Clock GPIO Selection ParameterRefRef (UI Choose-blocks)
Write-Step "Generating Hardware-Specific Clock GPIO Selection ParameterRefRef (UI Choose-blocks)..."

# Hardware-specific Clock ParameterRefRef with Choose-blocks for UI display
$gpioClockSelectionXml = Generate-HardwareSpecificGPIOSelectionUI -Type "Clock" -HardwareConfigs $hardwareConfigs

Write-Success "Generated hardware-specific Clock GPIO Selection ParameterRefRef with Choose-blocks"
Write-Host "  UI shows different GPIO options for each hardware variant" -ForegroundColor Green

# Step 6.2: Generate Central Manual GPIO Warning (Simplified)
Write-Step "Generating Central Manual GPIO Warning (Simplified)..."

# Warning uses Share parameters (0010%C% for Data, 0011%C% for Clock) - hardware-independent
$centralWarningXml = @"
<!-- Zentrale Warnung: Zeige Warnung wenn Data oder Clock Port auf "Manuell" gesetzt ist -->
<!-- Hidden parameter references -->
<ParameterRefRef RefId="%AID%_UP-%TT%0010%C%_R-%TT%0010%C%01" IndentLevel="1" />
<ParameterRefRef RefId="%AID%_UP-%TT%0011%C%_R-%TT%0011%C%01" IndentLevel="1" />
<!-- Check Data Port (Share Param 0010%C%) = Manuell (10) -->
<choose ParamRefId="%AID%_UP-%TT%0010%C%_R-%TT%0010%C%01">
  <when test="10">
    <ParameterSeparator Id="%AID%_PS-manualdata%C%" Text="⚠ Die manuelle GPIO-Konfiguration ist nur für fortgeschrittene Anwender empfohlen!&#xD;&#xA;Falsche Einstellungen können zu Fehlfunktionen führen." UIHint="Information" />
  </when>
</choose>
<!-- Check Clock Port (Share Param 0011%C%, only for SPI LEDs) = Manuell (10) -->
<choose ParamRefId="%AID%_UP-%TT%9%C%030_R-%TT%9%C%03001">
  <when test="5 21 22 23 24 25">
    <choose ParamRefId="%AID%_UP-%TT%0011%C%_R-%TT%0011%C%01">
      <when test="10">
        <ParameterSeparator Id="%AID%_PS-manualclock%C%" Text="⚠ Die manuelle GPIO-Konfiguration ist nur für fortgeschrittene Anwender empfohlen!&#xD;&#xA;Falsche Einstellungen können zu Fehlfunktionen führen." UIHint="Information" />
      </when>
    </choose>
  </when>
</choose>
"@

Write-Success "Generated Central Manual GPIO Warning with ParameterRefRef structure"
Write-Host "  Hidden ParameterRefRef followed by choose-blocks - same pattern as GPIO conflict" -ForegroundColor DarkGray

# Step 7: Generate JavaScript GPIO port mapping
Write-Step "Generating JavaScript GPIO port mapping..."

# Generate HW_ID-based port count mapping
$jsPortCounts = "  // Port counts per hardware ID (HW_ID -> port count)`n"
$jsPortCounts += "  var portCounts = {`n"

for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $hwConfig = $hardwareConfigs[$hwIdx]
  $hwIdHex = "0x" + $hwConfig.DeviceIdBit.ToString('X4')
  $hwName = $hwConfig.DeviceName
  $portCount = $hwConfig.GPIOPorts.Count
    
  $jsPortCounts += "    ${hwIdHex}: $portCount"
  if ($hwIdx -lt $hardwareConfigs.Count - 1) { $jsPortCounts += "," }
  $jsPortCounts += "  // $hwName`n"
}

$jsPortCounts += "  };"

# Generate GPIO port info mapping (labels only - GPIO numbers resolved at compile time)
$jsPortInfo = "`n  // GPIO port labels per hardware (for documentation)`n"
$jsPortInfo += "  var portLabels = {`n"

for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $hwConfig = $hardwareConfigs[$hwIdx]
  $hwIdHex = "0x" + $hwConfig.DeviceIdBit.ToString('X4')
  $hwName = $hwConfig.DeviceName
    
  $jsPortInfo += "    ${hwIdHex}: [  // $hwName`n"
    
  for ($portIdx = 0; $portIdx -lt $hwConfig.GPIOPorts.Count; $portIdx++) {
    $gpioPort = $hwConfig.GPIOPorts[$portIdx]
    $label = $gpioPort.Label
        
    $jsPortInfo += "      '$label'"
    if ($portIdx -lt $hwConfig.GPIOPorts.Count - 1) { $jsPortInfo += "," }
    $jsPortInfo += "`n"
  }
    
  $jsPortInfo += "    ]"
  if ($hwIdx -lt $hardwareConfigs.Count - 1) { $jsPortInfo += "," }
  $jsPortInfo += "`n"
}

$jsPortInfo += "  };"

$jsPortMaps = @"
  // Generated: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
  // Port selection: User selects port index (0-based) from dropdown
  // C++ firmware resolves index to actual GPIO via NEOPIXEL_HW_PORT_{index+1}_GPIO
  // GPIO numbers are compile-time defines, not available in JavaScript
  
$jsPortCounts
$jsPortInfo
  
"@

# Step 8: Update JavaScript file
Write-Step "Updating JavaScript file..."

$scriptJsPath = Resolve-RepoPath "src/NeoPixel.script.js"
if (Test-Path $scriptJsPath) {
  $scriptContent = Get-Content -Path $scriptJsPath -Raw
  $jsStartMarker = $script:Config.Markers.JSStart
  $jsEndMarker = $script:Config.Markers.JSEnd
    
  if ($scriptContent -match [regex]::Escape($jsStartMarker)) {
    # Use new pattern that preserves marker indentation
    $escapedStart = [regex]::Escape($jsStartMarker)
    $escapedEnd = [regex]::Escape($jsEndMarker)
    $jsPattern = "(?ms)(^[ \t]*)($escapedStart`r?`n)(.*?)(`r?`n\1$escapedEnd)"
    $jsReplacement = "`$1`$2$jsPortMaps`$4"
    $updatedJsContent = $scriptContent -replace $jsPattern, $jsReplacement
    if ($DryRun) {
      Write-Host "  [DRY-RUN] Would update JavaScript file:" -ForegroundColor Yellow
      Write-Host "    Original size: $($scriptContent.Length) chars" -ForegroundColor DarkGray
      Write-Host "    New size: $($updatedJsContent.Length) chars" -ForegroundColor DarkGray
      Write-Host "    Content to insert: $($jsPortMaps.Length) chars" -ForegroundColor DarkGray
    }
    else {
      Set-Content -Path $scriptJsPath -Value $updatedJsContent -NoNewline
      Write-Success "JavaScript port mapping updated"
    }
  }
  else {
    Write-Warning "JavaScript markers not found - skipping"
  }
}
else {
  Write-Warning "NeoPixel.script.js not found - skipping JavaScript update"
}

# Step 8.5: Update template file with GPIO Port Selection Parameters (073-080)
Write-Step "Updating template file with GPIO Port Selection Parameters (8 separate)..."

# Load template content once for all Step 8.5.x operations
$templateContent = Get-Content $templatePath -Raw

$gpioPortParamStartMarker = $script:Config.Markers.GPIOPortParamStart
$gpioPortParamEndMarker = $script:Config.Markers.GPIOPortParamEnd

if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioPortParamStartMarker -EndMarker $gpioPortParamEndMarker -NewContent $gpioPortParamXml) {
  if ($DryRun) {
    Write-Host "  [DRY-RUN] Would insert 8 GPIO Port Selection Parameters (073-080)" -ForegroundColor Yellow
  }
  else {
    Write-Success "GPIO Port Selection Parameters (073-080) inserted - 8 separate parameters"
  }
}
else {
  Write-Warning "GPIO Port Selection Parameter markers not found in template"
}

# Step 8.5.5.1: Update template file with Clock GPIO Selection Parameters (081-088)
Write-Step "Updating template file with Clock GPIO Selection Parameters (8 separate, SPI)..."

$gpioClockParamStartMarker = $script:Config.Markers.GPIOClockParamStart
$gpioClockParamEndMarker = $script:Config.Markers.GPIOClockParamEnd

if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioClockParamStartMarker -EndMarker $gpioClockParamEndMarker -NewContent $gpioClockParamXml) {
  if ($DryRun) {
    Write-Host "  [DRY-RUN] Would insert 8 Clock GPIO Selection Parameters (081-088)" -ForegroundColor Yellow
  }
  else {
    Write-Success "Clock GPIO Selection Parameters (081-088) inserted - 8 separate parameters for SPI"
  }
}
else {
  Write-Warning "Clock GPIO Selection Parameter markers not found in template"
}

# Step 8.5.6: Update template file with GPIO Port Selection ParameterRefs (8 separate)
Write-Step "Updating template file with GPIO Port Selection ParameterRefs (8 separate)..."

$gpioPortParamRefStartMarker = $script:Config.Markers.GPIOPortParamRefStart
$gpioPortParamRefEndMarker = $script:Config.Markers.GPIOPortParamRefEnd

if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioPortParamRefStartMarker -EndMarker $gpioPortParamRefEndMarker -NewContent $gpioPortParamRefsXml) {
  if ($DryRun) {
    Write-Host "  [DRY-RUN] Would insert 8 GPIO Port Selection ParameterRefs (07301-08001)" -ForegroundColor Yellow
  }
  else {
    Write-Success "GPIO Port Selection ParameterRefs (07301-08001) inserted - 8 separate refs"
  }
}
else {
  Write-Warning "GPIO Port Selection ParameterRef markers not found in template"
}

# Step 8.5.6.1: Update template file with GPIO Clock Selection ParameterRefs
Write-Step "Updating template file with GPIO Clock Selection ParameterRefs..."

$gpioClockParamRefStartMarker = $script:Config.Markers.GPIOClockParamRefStart
$gpioClockParamRefEndMarker = $script:Config.Markers.GPIOClockParamRefEnd

if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioClockParamRefStartMarker -EndMarker $gpioClockParamRefEndMarker -NewContent $gpioClockParamRefsXml) {
  if ($DryRun) {
    Write-Host "  [DRY-RUN] Would insert 8 GPIO Clock Selection ParameterRefs (08101-08801)" -ForegroundColor Yellow
  }
  else {
    Write-Success "GPIO Clock Selection ParameterRefs (08101-08801) inserted - 8 separate refs"
  }
}
else {
  Write-Warning "GPIO Clock Selection ParameterRef markers not found in template"
}

# Step 8.5.7: Update template file with GPIO Port Selection ParameterRefRef (choose-block)
Write-Step "Updating template file with GPIO Port Selection ParameterRefRef (choose-block)..."

$gpioPortSelectionStartMarker = $script:Config.Markers.GPIOPortSelectionStart
$gpioPortSelectionEndMarker = $script:Config.Markers.GPIOPortSelectionEnd

if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioPortSelectionStartMarker -EndMarker $gpioPortSelectionEndMarker -NewContent $gpioSelectionXml) {
  if ($DryRun) {
    Write-Host "  [DRY-RUN] Would insert GPIO Port Selection ParameterRefRef choose-block (8 hardware variants)" -ForegroundColor Yellow
  }
  else {
    Write-Success "GPIO Port Selection ParameterRefRef choose-block inserted - hardware-specific UI display"
  }
}
else {
  Write-Warning "GPIO Port Selection ParameterRefRef markers not found in template"
}

# Step 8.5.8: Update template file with Clock GPIO Selection ParameterRefRef (choose-block, SPI only)
Write-Step "Updating template file with Clock GPIO Selection ParameterRefRef (SPI only)..."

$gpioClockSelectionStartMarker = $script:Config.Markers.GPIOClockSelectionStart
$gpioClockSelectionEndMarker = $script:Config.Markers.GPIOClockSelectionEnd

if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioClockSelectionStartMarker -EndMarker $gpioClockSelectionEndMarker -NewContent $gpioClockSelectionXml) {
  if ($DryRun) {
    Write-Host "  [DRY-RUN] Would insert Clock GPIO Selection ParameterRefRef choose-block (SPI LEDs)" -ForegroundColor Yellow
  }
  else {
    Write-Success "Clock GPIO Selection ParameterRefRef choose-block inserted - SPI LEDs only"
  }
}
else {
  Write-Warning "Clock GPIO Selection ParameterRefRef markers not found in template"
}

# Step 8.5.9: Update template file with Central Manual GPIO Warning
Write-Step "Updating template file with Central Manual GPIO Warning..."

$centralWarningStartMarker = $script:Config.Markers.CentralWarningStart
$centralWarningEndMarker = $script:Config.Markers.CentralWarningEnd

if (Replace-MarkerContent -FilePath $templatePath -StartMarker $centralWarningStartMarker -EndMarker $centralWarningEndMarker -NewContent $centralWarningXml) {
  if ($DryRun) {
    Write-Host "  [DRY-RUN] Would insert Central Manual GPIO Warning (expert user notice)" -ForegroundColor Yellow
  }
  else {
    Write-Success "Central Manual GPIO Warning inserted - shows when user selects 'Manuell'"
  }
}
else {
  Write-Warning "Central Manual GPIO Warning markers not found in template"
}

# Step 8.8: Update template file with UI code for GPIO Conflict Status Display (HW-specific)
Write-Step "Updating template file with GPIO Conflict Status UI..."

if (Generate-ConflictUI -TemplatePath $templatePath -NumHardwareVariants 8 -BaseConflictParamId 91) {
  Write-Success "GPIO Conflict Status UI generated (hardware-aware choose-block)"
}
else {
  Write-Warning "GPIO Conflict Status UI markers not found in template"
}

# Step 9: OBSOLETE - GPIO Options section now handled by Steps 8.5.7 and 8.5.8
# Step 10: Summary
Write-Host ""
Write-Host "  ────────────────────────────────────────────────────────────────────────────────────────────────────────────────────" -ForegroundColor DarkGray
Write-Host ""
if ($DryRun) {
  Write-Host "  DRY-RUN Summary (No files were modified)" -ForegroundColor Yellow
}
else {
  Write-Host "  Summary" -ForegroundColor Cyan
}
Write-Host "  ────────────────────────────────────────────────────────────────────────────────────────────────────────────────────" -ForegroundColor DarkGray

$totalPorts = 0
foreach ($hwConfig in $hardwareConfigs) {
  $totalPorts += $hwConfig.GPIOPorts.Count
}

Write-Host "    Hardware Variants:  " -NoNewline -ForegroundColor DarkGray
Write-Host "$($hardwareConfigs.Count)" -ForegroundColor Green

if ($hardwareConfigsSkipped.Count -gt 0) {
  Write-Host "    Skipped (no HW_ID): " -NoNewline -ForegroundColor DarkGray
  Write-Host "$($hardwareConfigsSkipped.Count)" -ForegroundColor Yellow
}

Write-Host "    Total GPIO Ports:   " -NoNewline -ForegroundColor DarkGray
Write-Host "$totalPorts" -ForegroundColor Green

Write-Host "    Share XML:          " -NoNewline -ForegroundColor DarkGray
Write-Host "$(Split-Path -Leaf $shareXmlPath)" -ForegroundColor White

Write-Host "    Template:           " -NoNewline -ForegroundColor DarkGray
Write-Host "$(Split-Path -Leaf $templatePath)" -ForegroundColor White

Write-Host "    JavaScript:         " -NoNewline -ForegroundColor DarkGray
Write-Host "$(Split-Path -Leaf $scriptJsPath)" -ForegroundColor White

Write-Host ""
Write-Host "    Hardware Details:" -ForegroundColor Cyan
foreach ($hwConfig in $hardwareConfigs) {
  $hwName = $hwConfig.Name -replace "^${DefinesPrefix}_", ""
  Write-Host "      • " -NoNewline -ForegroundColor DarkGray
  Write-Host "$hwName" -NoNewline -ForegroundColor Green
  Write-Host " → $($hwConfig.GPIOPorts.Count) port(s)" -ForegroundColor DarkGray
    
  if ($VerboseMode) {
    foreach ($port in $hwConfig.GPIOPorts) {
      Write-Host "          Port $($port.Index): $($port.Label)" -ForegroundColor DarkGray
    }
  }
}

if ($hardwareConfigsSkipped.Count -gt 0) {
  Write-Host ""
  Write-Host "    Skipped Hardware (no DEVICE_HW_ID):" -ForegroundColor Yellow
  foreach ($hwConfig in $hardwareConfigsSkipped) {
    $hwName = $hwConfig.Name -replace "^${DefinesPrefix}_", ""
    Write-Host "      ⊗ " -NoNewline -ForegroundColor DarkGray
    Write-Host "$hwName" -ForegroundColor DarkYellow
  }
}

Write-Host ""

# ====================================================================
# Generate C++ Hardware Mapping Header
# ====================================================================
Write-Step "Generating C++ Hardware Mapping Header..."

$hardwareMappingPath = Resolve-RepoPath $HardwareMappingHeader
$headerFileName = Split-Path -Leaf $HardwareMappingHeader
$timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
$year = Get-Date -Format "yyyy"

$cppContent = @"
/**
 * @file $headerFileName
 * @brief Hardware Device ID to Index Mapping (Auto-Generated)
 *
 * This file contains the mapping between Device Hardware IDs and Hardware Indices.
 * The mapping is automatically generated from platformio.hardware.ini at build time.
 *
 * @warning AUTO-GENERATED FILE - DO NOT EDIT MANUALLY
 * @note Generated: $timestamp
 * @note Source: Build-HardwareConfig.ps1
 *
 * @copyright Copyright (c) $year OpenKNX (Licensed under GNU GPL v3.0)
 */

#pragma once

#include <cstdint>

namespace HardwareMapping {

/**
 * @brief Number of supported hardware variants
 */
constexpr uint8_t NUM_HARDWARE_VARIANTS = $($hardwareConfigs.Count);

/**
 * @brief Device Hardware ID to Index mapping structure
 */
struct HardwareIdMapping {
    uint16_t deviceHwId;
    uint8_t hwIndex;
    const char* name;
};

/**
 * @brief Hardware ID mapping table
 */
constexpr HardwareIdMapping HARDWARE_ID_MAP[NUM_HARDWARE_VARIANTS] = {
"@

for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $hwConfig = $hardwareConfigs[$hwIdx]
  $hwIdHex = "0x" + $hwConfig.DeviceIdBit.ToString('X4')
  $hwName = $hwConfig.DeviceName -replace '"', '\"'
  
  $cppContent += "`n    { $hwIdHex, $hwIdx, `"$hwName`" }"
  if ($hwIdx -lt $hardwareConfigs.Count - 1) {
    $cppContent += ","
  }
}

$cppContent += @"

};

/**
 * @brief Map Device Hardware ID to Hardware Index
 * 
 * @param deviceHwId The Device HW ID (16-bit)
 * @return Hardware index (0-$(($hardwareConfigs.Count - 1))) or 0 if unknown
 */
inline uint8_t mapDeviceHwIdToIndex(uint16_t deviceHwId) {
    for (uint8_t i = 0; i < NUM_HARDWARE_VARIANTS; i++) {
        if (HARDWARE_ID_MAP[i].deviceHwId == deviceHwId) {
            return HARDWARE_ID_MAP[i].hwIndex;
        }
    }
    return 0; // Fallback to first hardware
}

/**
 * @brief Get hardware name by index
 * 
 * @param hwIndex Hardware index (0-$(($hardwareConfigs.Count - 1)))
 * @return Hardware name or nullptr if invalid
 */
inline const char* getHardwareName(uint8_t hwIndex) {
    if (hwIndex < NUM_HARDWARE_VARIANTS) {
        return HARDWARE_ID_MAP[hwIndex].name;
    }
    return nullptr;
}

} // namespace HardwareMapping
"@

Set-Content -Path $hardwareMappingPath -Value $cppContent -Encoding UTF8
Write-Success "Generated C++ Hardware Mapping: $(Split-Path -Leaf $hardwareMappingPath)"
Write-Host "  • $($hardwareConfigs.Count) hardware variant(s) mapped" -ForegroundColor DarkGray

Write-Host ""
Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  Multi-Hardware GPIO Template Generation Complete" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host ""

exit 0