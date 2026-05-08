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

.PARAMETER DebugOutput
    Enable detailed debug output (compiler commands, flags, markers, etc.)
    Use this for troubleshooting build issues

.EXAMPLE
    # NeoPixel LED strips
    ./Build-HardwareConfig.ps1 -FeatureName "NeoPixel" -DefinesPrefix "NEOPIXEL_HW" `
        -TemplateFile "src/NeoPixel.Strip.templ.xml"

.EXAMPLE
    # I2C bus selection with debug output
    ./Build-HardwareConfig.ps1 -FeatureName "I2C" -DefinesPrefix "I2C_HW" `
        -TemplateFile "src/I2C.Config.templ.xml" -MaxPorts 4 -DebugOutput

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
  [string[]]$TemplateFile = @("src/NeoPixel.Hardware.templ.xml", "src/NeoPixel.Strip.templ.xml"),

  [Parameter(Mandatory = $false)]
  [string]$OutputFile = "",

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
  [string]$HardwareConfigSection = "",

  [Parameter(Mandatory = $false)]
  [switch]$ShowDebugParamsInEtsApp,

  [Parameter(Mandatory = $false)]
  [switch]$DebugOutput,

  [Parameter(Mandatory = $false)]
  [string[]]$EnvironmentFilter = @()
)

$ErrorActionPreference = "Stop"
$SCRIPT_VERSION = "0.1"

# ====================================================================
# UTF-8 Encoding Configuration (Critical for Windows PowerShell 5.1)
# ====================================================================
# PowerShell 5.1 (Windows) defaults to non-UTF-8 encoding which breaks Umlauts (ä,ö,ü,ß)
# PowerShell 7+ (macOS/Linux) uses UTF-8 by default
# Set both console output and file I/O to UTF-8 for cross-platform compatibility

# Console output encoding (for external commands)
$OutputEncoding = [System.Text.Encoding]::UTF8

# PowerShell default encoding for Get-Content/Set-Content
# Note: This only works in PowerShell 6+, but doesn't hurt in PS 5.1
if ($PSVersionTable.PSVersion.Major -ge 6) {
  $PSDefaultParameterValues['*:Encoding'] = 'utf8'
}
# For PS 5.1: We explicitly add -Encoding UTF8 to all Get-Content/Set-Content calls

if ($DebugOutput) {
  Write-Host "  [INFO] PowerShell Version: $($PSVersionTable.PSVersion) - UTF-8 Encoding: Enabled" -ForegroundColor DarkGray
}

# Set defaults if not provided
if ([string]::IsNullOrEmpty($ShareXml)) {
  $ShareXml = "src/$FeatureName.share.xml"
}
if ([string]::IsNullOrEmpty($JavaScriptFile)) {
  $JavaScriptFile = "src/$FeatureName.script.js"
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

function Write-WarningMsg {
  param([string]$Message)
  Write-Host "  [$FeatureName] ⚠ " -NoNewline -ForegroundColor Yellow
  Write-Host $Message
}

function Write-ErrorMsg {
  param([string]$Message)
  Write-Host "  [$FeatureName] ✗ " -NoNewline -ForegroundColor Red
  Write-Host $Message
}

# PS 5.1 compatible clear screen
# ====================================================================
# Multiple Templates Mode - Process each template sequentially
# ====================================================================
if ($TemplateFile.Count -gt 1) {
  Write-Host ""
  Write-Step "Processing $($TemplateFile.Count) templates sequentially..."
  Write-Host ""

  $templateIndex = 1
  foreach ($template in $TemplateFile) {
    $templateName = Split-Path -Leaf $template

    if ($Clean) {
      Write-Host "  [$templateIndex/$($TemplateFile.Count)] Cleaning $templateName..." -ForegroundColor Yellow
    } else {
      Write-Host "  [$templateIndex/$($TemplateFile.Count)] Building $templateName..." -ForegroundColor Cyan
    }

    # Build parameters for single template
    $params = @{
      FeatureName = $FeatureName
      DefinesPrefix = $DefinesPrefix
      TemplateFile = @($template)  # Single template as array
      MaxPorts = $MaxPorts
    }

    if ($TestMode) { $params.TestMode = $true }
    if ($Clean) { $params.Clean = $true }
    if ($Force) { $params.Force = $true }
    if ($DryRun) { $params.DryRun = $true }
    if ($VerboseMode) { $params.VerboseMode = $true }
    if ($UniversalBuild) { $params.UniversalBuild = $true }
    if ($HardwareConfigSection) { $params.HardwareConfigSection = $HardwareConfigSection }
    if ($ShowDebugParamsInEtsApp) { $params.ShowDebugParamsInEtsApp = $true }
    if ($EnvironmentFilter.Count -gt 0) { $params.EnvironmentFilter = $EnvironmentFilter }

    # Recursive call with single template
    # Robust cross-platform path resolution
    if ($DebugOutput) {
      Write-Host "  [DEBUG] Multi-template recursive call:" -ForegroundColor Cyan
      Write-Host "    PSCommandPath: $PSCommandPath" -ForegroundColor DarkGray
      Write-Host "    PSScriptRoot: $PSScriptRoot" -ForegroundColor DarkGray
      Write-Host "    MyInvocation.MyCommand.Path: $($MyInvocation.MyCommand.Path)" -ForegroundColor DarkGray
      Write-Host "    PWD: $PWD" -ForegroundColor DarkGray
    }
    
    $scriptPath = if ($PSCommandPath) {
      # PowerShell 7: Use built-in automatic variable
      Write-Host "    Using: PSCommandPath" -ForegroundColor Green
      $PSCommandPath
    } elseif ($PSScriptRoot) {
      # PowerShell 5.1 when run directly: Construct from script directory
      Write-Host "    Using: PSScriptRoot + filename" -ForegroundColor Green
      Join-Path $PSScriptRoot "Build-HardwareConfig.ps1"
    } elseif ($MyInvocation.MyCommand.Path) {
      # Fallback: Use MyInvocation
      Write-Host "    Using: MyInvocation.MyCommand.Path" -ForegroundColor Green
      $MyInvocation.MyCommand.Path
    } else {
      # Last resort: Construct from current directory
      Write-Host "    Using: PWD fallback" -ForegroundColor Yellow
      Join-Path (Join-Path $PWD "scripts") "Build-HardwareConfig.ps1"
    }
    
    Write-Host "    Final scriptPath: $scriptPath" -ForegroundColor $(if (Test-Path $scriptPath) { "Green" } else { "Red" })
    
    if (-not $scriptPath -or -not (Test-Path $scriptPath)) {
      Write-ErrorMsg "Cannot locate Build-HardwareConfig.ps1 for recursive call"
      Write-Host "  Attempted path: $scriptPath" -ForegroundColor Red
      exit 1
    }
    
    & $scriptPath @params

    if ($LASTEXITCODE -ne 0) {
      Write-ErrorMsg "$templateName failed!"
      exit $LASTEXITCODE
    }

    if ($Clean) {
      Write-Success "$templateName cleaned"
    }

    Write-Host ""
    $templateIndex++
  }

  Write-Success "All templates processed successfully"
  exit 0
}

# ====================================================================
# Single Template Mode - Continue with normal processing
# ====================================================================

# Extract single template from array
$TemplateFile = $TemplateFile[0]

if ([string]::IsNullOrEmpty($OutputFile)) {
  $OutputFile = $TemplateFile
}

# ====================================================================
# ====================================================================
# Hardware Parameter ID Configuration
# ====================================================================
# Hardware parameters use dedicated ID ranges in the 700-900 block to:
# - Support up to 100 hardware variants (future-proof)
# - Avoid conflicts with application logic parameters (000-699)
# - Reserve space for future hardware features (900-999)
# - Enable easy integration with other OpenKNX modules
#
# ID Schema: %AID%_UP-%TT%0%C%XXX where XXX is the parameter ID
# - Scene effect params occupy IDs ~300-959 (5 scenes × op:part with %PPP% base=300, increment=120)
# - Data GPIO:  950-974 (25 slots for HW0-24)
# - Clock GPIO: 975-999 (25 slots for HW0-24)
# ====================================================================
$HW_DATA_GPIO_BASE_ID = 950   # Data GPIO base offset (950-974)
$HW_CLOCK_GPIO_BASE_ID = 975  # Clock GPIO base offset (975-999)
$MAX_HARDWARE_VARIANTS = 25   # Maximum supported hardware variants (0-24)

$GPIO_MANUAL_VALUE = 10  # Value for "Manual" GPIO selection (Previous: 10) --> ToDo EC: Use 99, currently conflicts/Problmes
$GPIO_DUMMY_VALUE = 15   # Value for dummy/placeholder option (Previous: 15) --> ToDo EC: Use 98, currently conflicts/Problems
$GPIO_DUMMY_VALUE_EMPTY = 97  # Value for empty dummy/placeholder option NEW! --> ToDo EC: Using 97, We need to check! 


# ====================================================================
# Configuration
# ====================================================================
$script:Config = @{
  # Default paths (relative to repo root)
  ExtractorToolDefault = "scripts/extract_gpio_config.cpp"
  ExtractorExeName     = "extract_gpio_config"

  # Hardware INI file paths (search order)
  HardwareIniPaths     = @(
    "platformio.hardware.ini",
    "lib/OGM-HardwareConfig/include/HardwareConfig/OpenKNX/platformio.hardware.ini"
  )

  # Temporary files
  TempDir              = "scripts"

  # Markers
  Markers              = @{
    # Template markers (NO leading whitespace in definition)
    GPIOStart                 = "<!-- BEGIN AUTO-GENERATED: GPIO Options -->"
    GPIOEnd                   = "<!-- END AUTO-GENERATED: GPIO Options -->"
    ParamDefStart             = "<!-- BEGIN AUTO-GENERATED: GPIO Port Parameters -->"
    ParamDefEnd               = "<!-- END AUTO-GENERATED: GPIO Port Parameters -->"
    # share.xml markers (NO leading whitespace in definition)
    ParamTypeStart            = "<!-- BEGIN AUTO-GENERATED: GPIO Port Selection Types -->"
    ParamTypeEnd              = "<!-- END AUTO-GENERATED: GPIO Port Selection Types -->"
    ClockPortParamsStart      = "<!-- BEGIN AUTO-GENERATED: GPIO Clock Port Parameters -->"
    ClockPortParamsEnd        = "<!-- END AUTO-GENERATED: GPIO Clock Port Parameters -->"
    ClockConflictParamsStart  = "<!-- BEGIN AUTO-GENERATED: GPIO Clock Conflict Parameters -->"
    ClockConflictParamsEnd    = "<!-- END AUTO-GENERATED: GPIO Clock Conflict Parameters -->"
    ConflictParamsStart       = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict Parameters -->"
    ConflictParamsEnd         = "<!-- END AUTO-GENERATED: GPIO Conflict Parameters -->"
    RelayParameterTypeStart   = "<!-- BEGIN AUTO-GENERATED: External Relay ParameterType -->"
    RelayParameterTypeEnd     = "<!-- END AUTO-GENERATED: External Relay ParameterType -->"
    RelayConfigStart          = "<!-- BEGIN AUTO-GENERATED: External Relay Configuration -->"
    RelayConfigEnd            = "<!-- END AUTO-GENERATED: External Relay Configuration -->"
    RelayConflictParamsStart  = "<!-- BEGIN AUTO-GENERATED: External Relay Conflict Parameters -->"
    RelayConflictParamsEnd    = "<!-- END AUTO-GENERATED: External Relay Conflict Parameters -->"
    RelayDelayParamsStart     = "<!-- BEGIN AUTO-GENERATED: External Relay Delay Parameters -->"
    RelayDelayParamsEnd       = "<!-- END AUTO-GENERATED: External Relay Delay Parameters -->"
    RelayMinOffTimeParamsStart = "<!-- BEGIN AUTO-GENERATED: External Relay MinOffTime Parameters -->"
    RelayMinOffTimeParamsEnd   = "<!-- END AUTO-GENERATED: External Relay MinOffTime Parameters -->"
    RelayManualGpioParamsStart = "<!-- BEGIN AUTO-GENERATED: External Relay Manual GPIO Parameters -->"
    RelayManualGpioParamsEnd   = "<!-- END AUTO-GENERATED: External Relay Manual GPIO Parameters -->"
    RelayInvertParamsStart     = "<!-- BEGIN AUTO-GENERATED: External Relay Invert Parameters -->"
    RelayInvertParamsEnd       = "<!-- END AUTO-GENERATED: External Relay Invert Parameters -->"
    RelayParamRefsStart       = "<!-- BEGIN AUTO-GENERATED: External Relay ParameterRefs -->"
    RelayParamRefsEnd         = "<!-- END AUTO-GENERATED: External Relay ParameterRefs -->"
    RelayCopyCalcStart        = "<!-- BEGIN AUTO-GENERATED: External Relay GPIO Copy ParameterCalculations -->"
    RelayCopyCalcEnd          = "<!-- END AUTO-GENERATED: External Relay GPIO Copy ParameterCalculations -->"
    RelayUIStart              = "<!-- BEGIN AUTO-GENERATED: External Relay UI -->"
    RelayUIEnd                = "<!-- END AUTO-GENERATED: External Relay UI -->"
    RelayKOsStart             = "<!-- BEGIN AUTO-GENERATED: External Relay KOs -->"
    RelayKOsEnd               = "<!-- END AUTO-GENERATED: External Relay KOs -->"
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
    ConflictParamStart        = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict Result Parameters -->"
    ConflictParamEnd          = "<!-- END AUTO-GENERATED: GPIO Conflict Result Parameters -->"
    ConflictResultRefsStart   = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict Result ParameterRefs -->"
    ConflictResultRefsEnd     = "<!-- END AUTO-GENERATED: GPIO Conflict Result ParameterRefs -->"
    ConflictCalcStart         = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict Check ParameterCalculations -->"
    ConflictCalcEnd           = "<!-- END AUTO-GENERATED: GPIO Conflict Check ParameterCalculations -->"
    ConflictUIStart           = "<!-- BEGIN AUTO-GENERATED: GPIO Conflict Status Display -->"
    ConflictUIEnd             = "<!-- END AUTO-GENERATED: GPIO Conflict Status Display -->"
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
    JSHardwareIdMapStart      = "// BEGIN AUTO-GENERATED: Hardware ID Mapping for EventHandler"
    JSHardwareIdMapEnd        = "// END AUTO-GENERATED: Hardware ID Mapping for EventHandler"
    JSConflictStart           = "// BEGIN AUTO-GENERATED: Multi-Hardware GPIO Conflict Detection"
    JSConflictEnd             = "// END AUTO-GENERATED: Multi-Hardware GPIO Conflict Detection"
    JSNetworkVisibilityStart  = "// BEGIN AUTO-GENERATED: Network Module Visibility"
    JSNetworkVisibilityEnd    = "// END AUTO-GENERATED: Network Module Visibility"
  }
}

# PowerShell 5.1 compatibility: Create shortcut variable for easier access
$markers = $script:Config.Markers

# DEBUG: Verify Config is loaded correctly
if ($VerboseMode -and $script:Config -and $script:Config.Markers) {
  Write-Host "[CONFIG-TEST] ParamTypeStart = '$($markers['ParamTypeStart'])'" -ForegroundColor Cyan
  Write-Host "[CONFIG-TEST] ParamTypeEnd = '$($markers['ParamTypeEnd'])'" -ForegroundColor Cyan
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

# ====================================================================
# ====================================================================
# Clear content between markers (replace with placeholder comment)
# Enhanced with intelligent message handling for Strip templates
# ====================================================================
function Clear-MarkerContent {
  param(
    [string]$FilePath,
    [string]$StartMarker,
    [string]$EndMarker,
    [string]$PlaceholderComment = "<!-- Cleaned - Ready for regeneration -->",
    [string]$CleaningMessage = $null
  )

  # Show cleaning message if provided
  if ($CleaningMessage) {
    $fileName = Split-Path -Leaf $FilePath
    $suppressedMarkersForStrip = @(
      "GPIO Port Selection Parameter",
      "GPIO Port Selection ParameterRef",
      "GPIO Clock Selection Parameter",
      "GPIO Clock Selection ParameterRef",
      "GPIO Port Selection ParameterRefRef",
      "GPIO Clock Selection ParameterRefRef",
      "Central Manual GPIO Warning",
      "GPIO Copy ParameterCalculation",
      "GPIO Clock Copy ParameterCalculation",
      "GPIO Conflict Parameters",
      "GPIO Clock Conflict Parameters",
      "GPIO Conflict ParameterRefs",
      "GPIO Conflict Result ParameterRefs",
      "GPIO Conflict Check ParameterCalculations",
      "GPIO Conflict Status Display",
      "Conflict Parameters",
      "ParameterRefs",
      "ParameterCalculations",
      "UI Display"
    )

    $shouldSuppress = $fileName -like "*Strip*" -and ($suppressedMarkersForStrip | Where-Object { $CleaningMessage -like "*$_*" })

    if (-not $shouldSuppress) {
      Write-Host "  • Cleaning $fileName ($CleaningMessage)..." -ForegroundColor Cyan
    }
  }

  # Proceed with actual cleaning
  $content = Get-Content -Path $FilePath -Raw -Encoding UTF8
  if (-not $content) { return }

  # Pattern: Match indent + BEGIN marker, content, indent + END marker
  # (?ms) = multiline (^ matches line start) + singleline (. matches newlines)
  # (^[ \t]*) = capture leading whitespace at line start
  # \1 = backreference - END marker MUST have same indent as BEGIN
  $escapedStart = [regex]::Escape($StartMarker)
  $escapedEnd = [regex]::Escape($EndMarker)
  $nl = "`r?`n"
  $pattern = '(?ms)(^[ \t]*)(' + $escapedStart + $nl + ')(.*?)(' + $nl + '\1' + $escapedEnd + ')'

  if ($content -match $pattern) {
    # $1 = indent, $2 = BEGIN marker + newline, $3 = old content, $4 = newline + indent + END marker
    # Keep markers as-is (groups $1, $2 and $4), only replace content ($3)
    $indent = $Matches[1]
    $replacement = '$1$2' + $indent + $PlaceholderComment + '$4'
    $content = $content -replace $pattern, $replacement
    Set-Content -Path $FilePath -Value $content -NoNewline -Encoding UTF8
    Write-Success "Cleared: $(Split-Path -Leaf $FilePath)"
  }
  else {
    # Suppress warnings for specific hardware GPIO markers that moved to Hardware.templ.xml
    $fileName = Split-Path -Leaf $FilePath
    $suppressedMarkersForStrip = @(
      "GPIO Port Selection Parameter",
      "GPIO Port Selection ParameterRef",
      "GPIO Clock Selection Parameter",
      "GPIO Clock Selection ParameterRef",
      "GPIO Port Selection ParameterRefRef",
      "GPIO Clock Selection ParameterRefRef",
      "Central Manual GPIO Warning",
      "GPIO Copy ParameterCalculation",
      "GPIO Clock Copy ParameterCalculation",
      "GPIO Conflict Parameters",
      "GPIO Clock Conflict Parameters",
      "GPIO Conflict ParameterRefs",
      "GPIO Conflict Result ParameterRefs",
      "GPIO Conflict Check ParameterCalculations",
      "GPIO Conflict Status Display",
      "Conflict Parameters",
      "ParameterRefs",
      "ParameterCalculations",
      "UI Display"
    )

    $shouldSuppress = $fileName -like "*Strip*" -and $CleaningMessage -and ($suppressedMarkersForStrip | Where-Object { $CleaningMessage -like "*$_*" })

    if (-not $shouldSuppress) {
      Write-WarningMsg "Markers not found in: $fileName"
    }
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
    Write-WarningMsg "File not found: $FilePath"
    return $false
  }

  $content = Get-Content $FilePath -Raw -Encoding UTF8

  # Pattern: Match indent + BEGIN marker, content, indent + END marker
  # (?ms) = multiline (^ matches line start) + singleline (. matches newlines)
  # \1 = backreference - END marker MUST have same indent as BEGIN
  # Markers are STATIC ANCHORS - they are never moved or rewritten
  $escapedStart = [regex]::Escape($StartMarker)
  $escapedEnd = [regex]::Escape($EndMarker)
  $nl = "`r?`n"
  $pattern = '(?ms)(^[ \t]*)(' + $escapedStart + $nl + ')(.*?)(' + $nl + '\1' + $escapedEnd + ')'

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
    Set-Content -Path $FilePath -Value $content -NoNewline -Encoding UTF8
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

  # PowerShell 5.1 compatible Windows detection ($IsWindows only exists in PS Core 6+)
  $isWin = ($PSVersionTable.PSVersion.Major -le 5) -or ([bool]$IsWindows) -or ($env:OS -match "Windows")
  $exeSuffix = if ($isWin) { ".exe" } else { "" }

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
      $compilers = Get-ChildItem -Path $binDir -Filter "$compilerName$exeSuffix" -File -ErrorAction SilentlyContinue
      if ($compilers) {
        return $compilers[0].FullName
      }
    }
  }

  return $null
}

# ====================================================================
# GPIO Conflict Detection Helper Functions
# ====================================================================
$SHARE_GPIO_DATA_PORT_OFFSET = 80
$SHARE_GPIO_DATA_CONFLICT_OFFSET = 88
$SHARE_GPIO_CLOCK_CONFLICT_OFFSET = 89
$SHARE_GPIO_CLOCK_PORT_OFFSET = 90
$SHARE_RELAY_CONFIG_OFFSET = 98
$SHARE_RELAY_CONFLICT_OFFSET = 103
$SHARE_RELAY_DELAY_OFFSET = 104
$SHARE_RELAY_MIN_OFF_TIME_OFFSET = 120
$SHARE_RELAY_MANUAL_GPIO_OFFSET = 128
$SHARE_RELAY_INVERT_OFFSET = 132
$MAX_PHYSICAL_STRIPS = 8
$MAX_EXTERNAL_RELAYS = 4
$SHARE_DATA_PORT_BASE_ID = 100
$SHARE_DATA_CONFLICT_BASE_ID = 90
$SHARE_CLOCK_PORT_BASE_ID = 110
$SHARE_CLOCK_CONFLICT_BASE_ID = 162

function Generate-GPIOPortParametersInShare {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = $MAX_PHYSICAL_STRIPS
  )

  # Generate GPIO Port selection parameters in share.xml (00101-00108)
  # These store which GPIO port each strip uses (0=unused, 1-7=D0-D6)
  $paramsXml = "              <!-- GPIO Data Port Configuration Union -->`n"
  $paramsXml += "              <Union SizeInBit=`"$($NumStrips * 8)`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"$SHARE_GPIO_DATA_PORT_OFFSET`" BitOffset=`"0`" />`n"
  $paramsXml += "                <!-- All $NumStrips strips GPIO port selections ($($NumStrips * 8) bit total) -->`n"

  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $paramId = ($SHARE_DATA_PORT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $byteOffset = $stripIdx - 1  # Each parameter in its own byte
    $accessMode = if ($ShowDebugParamsInEtsApp) { "Read" } else { "None" }
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%$paramId`" Offset=`"$byteOffset`" BitOffset=`"0`" Name=`"Strip${stripIdx}DataPort`" ParameterType=`"%AID%_PT-GPIOPort8Bit`" Text=`"Strip $stripIdx GPIO Port`" Value=`"255`" Access=`"$accessMode`"/>`n"
  }

  $paramsXml += "              </Union>"

  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['ParamDefStart'] `
    -EndMarker $markers['ParamDefEnd'] `
    -NewContent $paramsXml
}

function Generate-ClockPortParametersInShare {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = $MAX_PHYSICAL_STRIPS
  )

  # Generate Clock GPIO Port selection parameters in share.xml (00111-00118)
  # These store which GPIO port each strip uses for Clock (0=unused, 1-7=D0-D6)
  $paramsXml = "              <!-- GPIO Clock Port Configuration Union -->`n"
  $paramsXml += "              <Union SizeInBit=`"$($NumStrips * 8)`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"$SHARE_GPIO_CLOCK_PORT_OFFSET`" BitOffset=`"0`" />`n"
  $paramsXml += "                <!-- All $NumStrips strips Clock GPIO port selections ($($NumStrips * 8) bit total) -->`n"

  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $paramId = ($SHARE_CLOCK_PORT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $byteOffset = $stripIdx - 1  # Each parameter in its own byte
    $accessMode = if ($ShowDebugParamsInEtsApp) { "Read" } else { "None" }
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%$paramId`" Offset=`"$byteOffset`" BitOffset=`"0`" Name=`"Strip${stripIdx}ClockPort`" ParameterType=`"%AID%_PT-GPIOPort8Bit`" Text=`"Strip $stripIdx Clock GPIO Port`" Value=`"255`" Access=`"$accessMode`"/>`n"
  }

  $paramsXml += "              </Union>"

  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['ClockPortParamsStart'] `
    -EndMarker $markers['ClockPortParamsEnd'] `
    -NewContent $paramsXml
}

function Generate-RelayConfigParametersInShare {
  param(
    [string]$ShareXmlPath,
    [array]$HardwareConfigs
  )

  # Generate External Relay configuration parameters (00130-00136 + HW-specific 00200-00270)
  $paramsXml = "              <!-- External Relay Configuration Union -->`n"
  $paramsXml += "              <Union SizeInBit=`"40`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"$SHARE_RELAY_CONFIG_OFFSET`" BitOffset=`"0`" />`n"
  $paramsXml += "                <!-- Byte 0: Relay 1 GPIO Port (shared) -->`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00131`" Offset=`"0`" BitOffset=`"0`" Name=`"NEOExternalRelay1Port`" ParameterType=`"%AID%_PT-GPIOPort8Bit`" Text=`"Relais 1 Port`" Value=`"$GPIO_DUMMY_VALUE`" Access=`"None`"/>`n"
  $paramsXml += "                <!-- Byte 1: Relay 2 GPIO Port (shared) -->`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00132`" Offset=`"1`" BitOffset=`"0`" Name=`"NEOExternalRelay2Port`" ParameterType=`"%AID%_PT-GPIOPort8Bit`" Text=`"Relais 2 Port`" Value=`"$GPIO_DUMMY_VALUE`" Access=`"None`"/>`n"
  $paramsXml += "                <!-- Byte 2: Relay Count -->`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00130`" Offset=`"2`" BitOffset=`"0`" Name=`"NEOExternalRelayCount`" ParameterType=`"%AID%_PT-ExternalRelayCount`" Text=`"Externe Relais`" Value=`"0`"/>`n"
  $paramsXml += "                <!-- Byte 3: Relay 3 GPIO Port (shared) -->`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00135`" Offset=`"3`" BitOffset=`"0`" Name=`"NEOExternalRelay3Port`" ParameterType=`"%AID%_PT-GPIOPort8Bit`" Text=`"Relais 3 Port`" Value=`"$GPIO_DUMMY_VALUE`" Access=`"None`"/>`n"
  $paramsXml += "                <!-- Byte 4: Relay 4 GPIO Port (shared) -->`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00136`" Offset=`"4`" BitOffset=`"0`" Name=`"NEOExternalRelay4Port`" ParameterType=`"%AID%_PT-GPIOPort8Bit`" Text=`"Relais 4 Port`" Value=`"$GPIO_DUMMY_VALUE`" Access=`"None`"/>`n"
  $paramsXml += "`n"
  $paramsXml += "                <!-- Relay 1 GPIO Selection (hardware-specific) -->`n"

  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $paramId = (200 + $hwIdx).ToString().PadLeft(5, '0')  # 00200, 00201, ...
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%${paramId}`" Offset=`"0`" BitOffset=`"0`" Name=`"NEORelay1GPIOPortHW$hwIdx`" ParameterType=`"%AID%_PT-NeoPixelGPIOPortHW$hwIdx`" Text=`"Relais 1 Port`" Value=`"$GPIO_DUMMY_VALUE`"/>`n"
  }

  $paramsXml += "`n"
  $paramsXml += "                <!-- Relay 2 GPIO Selection (hardware-specific) -->`n"

  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $paramId = (220 + $hwIdx).ToString().PadLeft(5, '0')  # 00220, 00221, ...
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%${paramId}`" Offset=`"1`" BitOffset=`"0`" Name=`"NEORelay2GPIOPortHW$hwIdx`" ParameterType=`"%AID%_PT-NeoPixelGPIOPortHW$hwIdx`" Text=`"Relais 2 Port`" Value=`"$GPIO_DUMMY_VALUE`"/>`n"
  }

  $paramsXml += "`n"
  $paramsXml += "                <!-- Relay 3 GPIO Selection (hardware-specific) -->`n"

  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $paramId = (240 + $hwIdx).ToString().PadLeft(5, '0')  # 00240, 00241, ...
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%${paramId}`" Offset=`"3`" BitOffset=`"0`" Name=`"NEORelay3GPIOPortHW$hwIdx`" ParameterType=`"%AID%_PT-NeoPixelGPIOPortHW$hwIdx`" Text=`"Relais 3 Port`" Value=`"$GPIO_DUMMY_VALUE`"/>`n"
  }

  $paramsXml += "`n"
  $paramsXml += "                <!-- Relay 4 GPIO Selection (hardware-specific) -->`n"

  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $paramId = (260 + $hwIdx).ToString().PadLeft(5, '0')  # 00260, 00261, ...
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%${paramId}`" Offset=`"4`" BitOffset=`"0`" Name=`"NEORelay4GPIOPortHW$hwIdx`" ParameterType=`"%AID%_PT-NeoPixelGPIOPortHW$hwIdx`" Text=`"Relais 4 Port`" Value=`"$GPIO_DUMMY_VALUE`"/>`n"
  }

  $paramsXml += "              </Union>"

  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['RelayConfigStart'] `
    -EndMarker $markers['RelayConfigEnd'] `
    -NewContent $paramsXml
}

function Generate-RelayConflictParametersInShare {
  param(
    [string]$ShareXmlPath
  )

  $accessMode = if ($ShowDebugParamsInEtsApp) { "Read" } else { "None" }

  $paramsXml = "              <!-- External Relay Conflict Flags Union (4 flags in 1 byte, Access=None, JS-only) -->`n"
  $paramsXml += "              <Union SizeInBit=`"8`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"$SHARE_RELAY_CONFLICT_OFFSET`" BitOffset=`"0`" />`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00133`" Offset=`"0`" BitOffset=`"0`" Name=`"Relay1HasConflict`" ParameterType=`"%AID%_PT-ConflictFlag`" Text=`"Relais 1 Conflict`" Value=`"0`" Access=`"$accessMode`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00134`" Offset=`"0`" BitOffset=`"1`" Name=`"Relay2HasConflict`" ParameterType=`"%AID%_PT-ConflictFlag`" Text=`"Relais 2 Conflict`" Value=`"0`" Access=`"$accessMode`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00137`" Offset=`"0`" BitOffset=`"2`" Name=`"Relay3HasConflict`" ParameterType=`"%AID%_PT-ConflictFlag`" Text=`"Relais 3 Conflict`" Value=`"0`" Access=`"$accessMode`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00138`" Offset=`"0`" BitOffset=`"3`" Name=`"Relay4HasConflict`" ParameterType=`"%AID%_PT-ConflictFlag`" Text=`"Relais 4 Conflict`" Value=`"0`" Access=`"$accessMode`"/>`n"
  $paramsXml += "              </Union>"

  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['RelayConflictParamsStart'] `
    -EndMarker $markers['RelayConflictParamsEnd'] `
    -NewContent $paramsXml
}

function Generate-RelayDelayParametersInShare {
  param(
    [string]$ShareXmlPath
  )

  # Generate Relay On/Off Delay parameters (00139-00146, 4 relays × 2 delays = 8 params)
  # Union size: 16 bytes (4 relays × 2 delays × 2 bytes = 16 bytes)
  $paramsXml = "              <!-- External Relay Delay Configuration Union -->`n"
  $paramsXml += "              <Union SizeInBit=`"128`">`n"  # 16 bytes × 8 = 128 bits
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"$SHARE_RELAY_DELAY_OFFSET`" BitOffset=`"0`" />`n"
  
  # Relay 1 Delays (Bytes 0-3: OnDelay=2bytes, OffDelay=2bytes)
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00139`" Offset=`"0`" BitOffset=`"0`" Name=`"NEOExternalRelay1OnDelay`" ParameterType=`"%AID%_PT-RelayDelay`" Text=`"Einschaltverzögerung (s)`" Value=`"0`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00140`" Offset=`"2`" BitOffset=`"0`" Name=`"NEOExternalRelay1OffDelay`" ParameterType=`"%AID%_PT-RelayDelay`" Text=`"Ausschaltverzögerung (s)`" Value=`"0`"/>`n"
  
  # Relay 2 Delays (Bytes 4-7)
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00141`" Offset=`"4`" BitOffset=`"0`" Name=`"NEOExternalRelay2OnDelay`" ParameterType=`"%AID%_PT-RelayDelay`" Text=`"Einschaltverzögerung (s)`" Value=`"0`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00142`" Offset=`"6`" BitOffset=`"0`" Name=`"NEOExternalRelay2OffDelay`" ParameterType=`"%AID%_PT-RelayDelay`" Text=`"Ausschaltverzögerung (s)`" Value=`"0`"/>`n"
  
  # Relay 3 Delays (Bytes 8-11)
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00143`" Offset=`"8`" BitOffset=`"0`" Name=`"NEOExternalRelay3OnDelay`" ParameterType=`"%AID%_PT-RelayDelay`" Text=`"Einschaltverzögerung (s)`" Value=`"0`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00144`" Offset=`"10`" BitOffset=`"0`" Name=`"NEOExternalRelay3OffDelay`" ParameterType=`"%AID%_PT-RelayDelay`" Text=`"Ausschaltverzögerung (s)`" Value=`"0`"/>`n"
  
  # Relay 4 Delays (Bytes 12-15)
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00145`" Offset=`"12`" BitOffset=`"0`" Name=`"NEOExternalRelay4OnDelay`" ParameterType=`"%AID%_PT-RelayDelay`" Text=`"Einschaltverzögerung (s)`" Value=`"0`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00146`" Offset=`"14`" BitOffset=`"0`" Name=`"NEOExternalRelay4OffDelay`" ParameterType=`"%AID%_PT-RelayDelay`" Text=`"Ausschaltverzögerung (s)`" Value=`"0`"/>`n"
  
  $paramsXml += "              </Union>"

  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['RelayDelayParamsStart'] `
    -EndMarker $markers['RelayDelayParamsEnd'] `
    -NewContent $paramsXml
}

function Generate-RelayMinOffTimeParametersInShare {
  param(
    [string]$ShareXmlPath
  )

  # Generate Relay Minimum Off-Time parameters (00147-00150, 8 bytes)
  # Union size: 8 bytes (2 bytes per relay = uint16, 0-65535 seconds)
  $paramsXml = "              <!-- External Relay Minimum Off-Time Union -->`n"
  $paramsXml += "              <Union SizeInBit=`"64`">`n"  # 4 relays × 2 bytes × 8 = 64 bits
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"$SHARE_RELAY_MIN_OFF_TIME_OFFSET`" BitOffset=`"0`" />`n"
  
  # Relay 1-4 MinOffTime (2 bytes each = 0-65535 seconds)
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00147`" Offset=`"0`" BitOffset=`"0`" Name=`"NEOExternalRelay1MinOffTime`" ParameterType=`"%AID%_PT-RelayMinOffTime`" Text=`"Minimale Aus-Zeit (s)`" Value=`"0`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00148`" Offset=`"2`" BitOffset=`"0`" Name=`"NEOExternalRelay2MinOffTime`" ParameterType=`"%AID%_PT-RelayMinOffTime`" Text=`"Minimale Aus-Zeit (s)`" Value=`"0`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00149`" Offset=`"4`" BitOffset=`"0`" Name=`"NEOExternalRelay3MinOffTime`" ParameterType=`"%AID%_PT-RelayMinOffTime`" Text=`"Minimale Aus-Zeit (s)`" Value=`"0`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00150`" Offset=`"6`" BitOffset=`"0`" Name=`"NEOExternalRelay4MinOffTime`" ParameterType=`"%AID%_PT-RelayMinOffTime`" Text=`"Minimale Aus-Zeit (s)`" Value=`"0`"/>`n"
  
  $paramsXml += "              </Union>"

  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['RelayMinOffTimeParamsStart'] `
    -EndMarker $markers['RelayMinOffTimeParamsEnd'] `
    -NewContent $paramsXml
}

function Generate-RelayManualGpioParametersInShare {
  param(
    [string]$ShareXmlPath
  )

  # Generate Manual GPIO Parameters (00155-00158, 4 bytes)
  # GPIO numbers shown when Port dropdown = 10 ("Manuell")
  # Note: This block follows the relay delay/min-off-time section and must stay aligned with the share.xml memory map.
  $paramsXml = ""
  
  # Manual GPIO Numbers Union (00155-00158)
  $paramsXml += "              <!-- External Relay Manual GPIO Numbers Union (triggered when Port=10) -->`n"
  $paramsXml += "              <Union SizeInBit=`"32`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"$SHARE_RELAY_MANUAL_GPIO_OFFSET`" BitOffset=`"0`" />`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00155`" Offset=`"0`" BitOffset=`"0`" Name=`"NEOExternalRelay1GPIO`" ParameterType=`"%AID%_PT-GPIONumber`" Text=`"Relais 1 GPIO`" Value=`"255`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00156`" Offset=`"1`" BitOffset=`"0`" Name=`"NEOExternalRelay2GPIO`" ParameterType=`"%AID%_PT-GPIONumber`" Text=`"Relais 2 GPIO`" Value=`"255`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00157`" Offset=`"2`" BitOffset=`"0`" Name=`"NEOExternalRelay3GPIO`" ParameterType=`"%AID%_PT-GPIONumber`" Text=`"Relais 3 GPIO`" Value=`"255`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00158`" Offset=`"3`" BitOffset=`"0`" Name=`"NEOExternalRelay4GPIO`" ParameterType=`"%AID%_PT-GPIONumber`" Text=`"Relais 4 GPIO`" Value=`"255`"/>`n"
  $paramsXml += "              </Union>"

  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['RelayManualGpioParamsStart'] `
    -EndMarker $markers['RelayManualGpioParamsEnd'] `
    -NewContent $paramsXml
}

function Generate-RelayInvertParametersInShare {
  param(
    [string]$ShareXmlPath
  )

  # Generate Output Logic (Invert) Parameters (00159-00162, 4 bits)
  # One 1-bit flag per relay: 0 = normal (HIGH=ON), 1 = invertiert (LOW=ON)
  $paramsXml = "              <!-- External Relay Output Logic (Invert) Union (4 flags in 1 byte, Access=None, JS-only) -->`n"
  $paramsXml += "              <Union SizeInBit=`"8`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"$SHARE_RELAY_INVERT_OFFSET`" BitOffset=`"0`" />`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00159`" Offset=`"0`" BitOffset=`"0`" Name=`"NEOExternalRelay1OutputLogic`" ParameterType=`"%AID%_PT-RelayOutputLogic`" Text=`"Ausgangslogik`" Value=`"0`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00160`" Offset=`"0`" BitOffset=`"1`" Name=`"NEOExternalRelay2OutputLogic`" ParameterType=`"%AID%_PT-RelayOutputLogic`" Text=`"Ausgangslogik`" Value=`"0`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00161`" Offset=`"0`" BitOffset=`"2`" Name=`"NEOExternalRelay3OutputLogic`" ParameterType=`"%AID%_PT-RelayOutputLogic`" Text=`"Ausgangslogik`" Value=`"0`"/>`n"
  $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%00162`" Offset=`"0`" BitOffset=`"3`" Name=`"NEOExternalRelay4OutputLogic`" ParameterType=`"%AID%_PT-RelayOutputLogic`" Text=`"Ausgangslogik`" Value=`"0`"/>`n"
  $paramsXml += "              </Union>"

  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['RelayInvertParamsStart'] `
    -EndMarker $markers['RelayInvertParamsEnd'] `
    -NewContent $paramsXml
}

function Generate-ConflictParametersInShare {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = $MAX_PHYSICAL_STRIPS
  )

  # Generate conflict flag parameters in share.xml (00091-00098)
  # Template will reference these with 0009%C% token
  $paramsXml = "              <!-- GPIO Data Conflict Flags Union ($NumStrips flags in 1 byte, Access=None, JS-only) -->`n"
  $paramsXml += "              <Union SizeInBit=`"8`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"$SHARE_GPIO_DATA_CONFLICT_OFFSET`" BitOffset=`"0`" />`n"
  $paramsXml += "                <!-- All $NumStrips strips conflict flags ($NumStrips bits in 1 byte) -->`n"

  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $paramId = ($SHARE_DATA_CONFLICT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $bitOffset = $stripIdx - 1
    $accessMode = if ($ShowDebugParamsInEtsApp) { "Read" } else { "None" }
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%$paramId`" Offset=`"0`" BitOffset=`"$bitOffset`" Name=`"Strip${stripIdx}HasDataConflict`" ParameterType=`"%AID%_PT-ConflictFlag`" Text=`"Strip $stripIdx Conflict`" Value=`"0`" Access=`"$accessMode`"/>`n"
  }

  $paramsXml += "              </Union>"

  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['ConflictParamsStart'] `
    -EndMarker $markers['ConflictParamsEnd'] `
    -NewContent $paramsXml
}

function Generate-ClockConflictParametersInShare {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = $MAX_PHYSICAL_STRIPS
  )

  # Generate Clock conflict flag parameters in share.xml (00163-00170)
  $paramsXml = "              <!-- GPIO Clock Conflict Flags Union ($NumStrips flags in 1 byte, Access=None, JS-only) -->`n"
  $paramsXml += "              <Union SizeInBit=`"8`">`n"
  $paramsXml += "                <Memory CodeSegment=`"%AID%_RS-04-00000`" Offset=`"$SHARE_GPIO_CLOCK_CONFLICT_OFFSET`" BitOffset=`"0`" />`n"
  $paramsXml += "                <!-- All $NumStrips strips Clock conflict flags ($NumStrips bits in 1 byte) -->`n"

  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $paramId = ($SHARE_CLOCK_CONFLICT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $bitOffset = $stripIdx - 1
    $accessMode = if ($ShowDebugParamsInEtsApp) { "Read" } else { "None" }
    $paramsXml += "                <Parameter Id=`"%AID%_UP-%TT%$paramId`" Offset=`"0`" BitOffset=`"$bitOffset`" Name=`"Strip${stripIdx}HasClockConflict`" ParameterType=`"%AID%_PT-ConflictFlag`" Text=`"Strip $stripIdx Clock Conflict`" Value=`"0`" Access=`"$accessMode`"/>`n"
  }

  $paramsXml += "              </Union>"

  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['ClockConflictParamsStart'] `
    -EndMarker $markers['ClockConflictParamsEnd'] `
    -NewContent $paramsXml
}

function Generate-ConflictParameterRefsInShare {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = $MAX_PHYSICAL_STRIPS
  )

  # Generate ParameterRefs in share.xml for:
  # 1. GPIO Port Parameters (00101-00108)
  # 2. Conflict Flag Parameters (00091-00098)

  # Build ONLY the content, markers are already in the file and stay static
  $refsXml = "              <!-- GPIO Port Parameters for all $NumStrips strips -->`n"

  # GPIO Port ParameterRefs (00101-00108)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $portParamId = ($SHARE_DATA_PORT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $portRefId = $portParamId + "01"  # 0010101, 0010201, ..., 0010601
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${portParamId}_R-%TT%${portRefId}`" RefId=`"%AID%_UP-%TT%${portParamId}`" />`n"
  }

  $refsXml += "              <!-- Data Conflict Flag Parameters for all $NumStrips strips -->`n"

  # Data Conflict Flag ParameterRefs (00091-00098)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $conflictParamId = ($SHARE_DATA_CONFLICT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $conflictRefId = $conflictParamId + "01"  # 0009101, 0009201, ..., 0009601
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${conflictParamId}_R-%TT%${conflictRefId}`" RefId=`"%AID%_UP-%TT%${conflictParamId}`" />`n"
  }

  $refsXml += "              <!-- Clock GPIO Port Parameters for all $NumStrips strips -->`n"

  # Clock Port ParameterRefs (00111-00118)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockPortParamId = ($SHARE_CLOCK_PORT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $clockPortRefId = $clockPortParamId + "01"  # 0011101, 0011201, ..., 0011601
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${clockPortParamId}_R-%TT%${clockPortRefId}`" RefId=`"%AID%_UP-%TT%${clockPortParamId}`" />`n"
  }

  $refsXml += "              <!-- Clock Conflict Flag Parameters for all $NumStrips strips -->`n"

  # Clock Conflict Flag ParameterRefs (00163-00170)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockConflictParamId = ($SHARE_CLOCK_CONFLICT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $clockConflictRefId = $clockConflictParamId + "01"  # 0011701, 0011801, ..., 0012201
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${clockConflictParamId}_R-%TT%${clockConflictRefId}`" RefId=`"%AID%_UP-%TT%${clockConflictParamId}`" />"
    if ($stripIdx -lt $NumStrips) {
      $refsXml += "`n"
    }
  }

  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['ConflictParamRefsStart'] `
    -EndMarker $markers['ConflictParamRefsEnd'] `
    -NewContent $refsXml
}

function Generate-ConflictParameterRefsInShare {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = $MAX_PHYSICAL_STRIPS
  )

  # Generate ParameterRefs in share.xml for:
  # 1. GPIO Port Parameters (00101-00108)
  # 2. Conflict Flag Parameters (00091-00098)

  # Build ONLY the content, markers are already in the file and stay static
  $refsXml = "              <!-- GPIO Port Parameters for all $NumStrips strips -->`n"

  # GPIO Port ParameterRefs (00101-00108)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $portParamId = ($SHARE_DATA_PORT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $portRefId = $portParamId + "01"  # 0010101, 0010201, ..., 0010601
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${portParamId}_R-%TT%${portRefId}`" RefId=`"%AID%_UP-%TT%${portParamId}`" />`n"
  }

  $refsXml += "              <!-- Data Conflict Flag Parameters for all $NumStrips strips -->`n"

  # Data Conflict Flag ParameterRefs (00091-00098)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $conflictParamId = ($SHARE_DATA_CONFLICT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $conflictRefId = $conflictParamId + "01"  # 0009101, 0009201, ..., 0009601
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${conflictParamId}_R-%TT%${conflictRefId}`" RefId=`"%AID%_UP-%TT%${conflictParamId}`" />`n"
  }

  $refsXml += "              <!-- Clock GPIO Port Parameters for all $NumStrips strips -->`n"

  # Clock Port ParameterRefs (00111-00118)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockPortParamId = ($SHARE_CLOCK_PORT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $clockPortRefId = $clockPortParamId + "01"  # 0011101, 0011201, ..., 0011601
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${clockPortParamId}_R-%TT%${clockPortRefId}`" RefId=`"%AID%_UP-%TT%${clockPortParamId}`" />`n"
  }

  $refsXml += "              <!-- Clock Conflict Flag Parameters for all $NumStrips strips -->`n"

  # Clock Conflict Flag ParameterRefs (00163-00170)
  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockConflictParamId = ($SHARE_CLOCK_CONFLICT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $clockConflictRefId = $clockConflictParamId + "01"  # 0011701, 0011801, ..., 0012201
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${clockConflictParamId}_R-%TT%${clockConflictRefId}`" RefId=`"%AID%_UP-%TT%${clockConflictParamId}`" />"
    if ($stripIdx -lt $NumStrips) {
      $refsXml += "`n"
    }
  }

  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['ConflictParamRefsStart'] `
    -EndMarker $markers['ConflictParamRefsEnd'] `
    -NewContent $refsXml
}

# External Relay ParameterRefs in share.xml
function Generate-RelayParameterRefsInShare {
  param(
    [string]$ShareXmlPath,
    [array]$HardwareConfigs
  )

  $refsXml = "              <!-- External Relay Parameter References -->`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00130_R-%TT%0013001`" RefId=`"%AID%_UP-%TT%00130`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00131_R-%TT%0013101`" RefId=`"%AID%_UP-%TT%00131`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00132_R-%TT%0013201`" RefId=`"%AID%_UP-%TT%00132`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00135_R-%TT%0013501`" RefId=`"%AID%_UP-%TT%00135`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00136_R-%TT%0013601`" RefId=`"%AID%_UP-%TT%00136`" />`n"
  $refsXml += "              <!-- External Relay GPIO Selection Parameter References (HW-specific) -->`n"

  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $relay1ParamId = (200 + $hwIdx).ToString().PadLeft(5, '0')
    $relay1RefId = $relay1ParamId + "01"
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${relay1ParamId}_R-%TT%${relay1RefId}`" RefId=`"%AID%_UP-%TT%${relay1ParamId}`" />`n"
  }

  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $relay2ParamId = (220 + $hwIdx).ToString().PadLeft(5, '0')
    $relay2RefId = $relay2ParamId + "01"
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${relay2ParamId}_R-%TT%${relay2RefId}`" RefId=`"%AID%_UP-%TT%${relay2ParamId}`" />`n"
  }

  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $relay3ParamId = (240 + $hwIdx).ToString().PadLeft(5, '0')
    $relay3RefId = $relay3ParamId + "01"
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${relay3ParamId}_R-%TT%${relay3RefId}`" RefId=`"%AID%_UP-%TT%${relay3ParamId}`" />`n"
  }

  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $relay4ParamId = (260 + $hwIdx).ToString().PadLeft(5, '0')
    $relay4RefId = $relay4ParamId + "01"
    $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%${relay4ParamId}_R-%TT%${relay4RefId}`" RefId=`"%AID%_UP-%TT%${relay4ParamId}`" />`n"
  }

  $refsXml += "              <!-- External Relay Conflict Flags -->`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00133_R-%TT%0013301`" RefId=`"%AID%_UP-%TT%00133`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00134_R-%TT%0013401`" RefId=`"%AID%_UP-%TT%00134`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00137_R-%TT%0013701`" RefId=`"%AID%_UP-%TT%00137`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00138_R-%TT%0013801`" RefId=`"%AID%_UP-%TT%00138`" />`n"
  
  $refsXml += "              <!-- External Relay Delay Parameters -->`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00139_R-%TT%0013901`" RefId=`"%AID%_UP-%TT%00139`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00140_R-%TT%0014001`" RefId=`"%AID%_UP-%TT%00140`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00141_R-%TT%0014101`" RefId=`"%AID%_UP-%TT%00141`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00142_R-%TT%0014201`" RefId=`"%AID%_UP-%TT%00142`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00143_R-%TT%0014301`" RefId=`"%AID%_UP-%TT%00143`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00144_R-%TT%0014401`" RefId=`"%AID%_UP-%TT%00144`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00145_R-%TT%0014501`" RefId=`"%AID%_UP-%TT%00145`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00146_R-%TT%0014601`" RefId=`"%AID%_UP-%TT%00146`" />`n"
  
  $refsXml += "              <!-- External Relay MinOffTime Parameters -->`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00147_R-%TT%0014701`" RefId=`"%AID%_UP-%TT%00147`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00148_R-%TT%0014801`" RefId=`"%AID%_UP-%TT%00148`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00149_R-%TT%0014901`" RefId=`"%AID%_UP-%TT%00149`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00150_R-%TT%0015001`" RefId=`"%AID%_UP-%TT%00150`" />`n"
  
  $refsXml += "              <!-- External Relay Manual GPIO Numbers (shown when Port=10) -->`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00155_R-%TT%0015501`" RefId=`"%AID%_UP-%TT%00155`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00156_R-%TT%0015601`" RefId=`"%AID%_UP-%TT%00156`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00157_R-%TT%0015701`" RefId=`"%AID%_UP-%TT%00157`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00158_R-%TT%0015801`" RefId=`"%AID%_UP-%TT%00158`" />`n"

  $refsXml += "              <!-- External Relay Output Logic (Invert) Parameters -->`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00159_R-%TT%0015901`" RefId=`"%AID%_UP-%TT%00159`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00160_R-%TT%0016001`" RefId=`"%AID%_UP-%TT%00160`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00161_R-%TT%0016101`" RefId=`"%AID%_UP-%TT%00161`" />`n"
  $refsXml += "              <ParameterRef Id=`"%AID%_UP-%TT%00162_R-%TT%0016201`" RefId=`"%AID%_UP-%TT%00162`" />"

  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['RelayParamRefsStart'] `
    -EndMarker $markers['RelayParamRefsEnd'] `
    -NewContent $refsXml
}

# REMOVED: Old duplicate function definitions - moved to lines 1422+
# Functions Generate-ConflictResultParameters, Generate-ConflictResultParameterRefs,
# Generate-ConflictResultCalculations are now defined later in the script

function Generate-ConflictCalculation {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = $MAX_PHYSICAL_STRIPS,
    [array]$HardwareConfigs = @()
  )

  # Generate SINGLE ParameterCalculation in share.xml
  # Reads: Data GPIO ports (00101-00108) + Clock GPIO ports (00111-00118)
  # Writes: Data conflict flags (00091-00098) + Clock conflict flags (00163-00170)

  # LParameters: Read Data+Clock GPIO ports for all configured strips
  $lParams = "<LParameters>`n"
  $lParams += "  <!-- Data GPIO Port selections for all $NumStrips strips -->`n"

  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $portParamId = ($SHARE_DATA_PORT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $refId = $portParamId + "01"
    $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${portParamId}_R-%TT%${refId}`" AliasName=`"Strip${stripIdx}DataPort`" />`n"
  }

  $lParams += "  <!-- Clock GPIO Port selections for all $NumStrips strips -->`n"

  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockPortParamId = ($SHARE_CLOCK_PORT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $clockRefId = $clockPortParamId + "01"
    $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${clockPortParamId}_R-%TT%${clockRefId}`" AliasName=`"Strip${stripIdx}ClockPort`" />`n"
  }

  $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%00020_R-%TT%0002001`" AliasName=`"NumberOfLEDStrips`" />`n"
  $lParams += "  <!-- External Relay Ports -->`n"
  if ($HardwareConfigs.Count -gt 1) {
    $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-4000018_R-400001801`" AliasName=`"HardwareSelection`" />`n"
  }
  $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%00130_R-%TT%0013001`" AliasName=`"RelayCount`" />`n"
  $relaySharedPortParamIds = @('00131', '00132', '00135', '00136')
  for ($relayIdx = 1; $relayIdx -le $MAX_EXTERNAL_RELAYS; $relayIdx++) {
    $sharedPortParamId = $relaySharedPortParamIds[$relayIdx - 1]
    $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${sharedPortParamId}_R-%TT%${sharedPortParamId}01`" AliasName=`"Relay${relayIdx}Port`" />`n"
  }

  for ($relayIdx = 1; $relayIdx -le $MAX_EXTERNAL_RELAYS; $relayIdx++) {
    $relayParamBaseId = 200 + (($relayIdx - 1) * 20)
    for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
      $relayParamId = ($relayParamBaseId + $hwIdx).ToString().PadLeft(5, '0')
      $relayRefId = $relayParamId + "01"
      $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${relayParamId}_R-%TT%${relayRefId}`" AliasName=`"Relay${relayIdx}HW$hwIdx`" />`n"
    }
  }
  $lParams += "</LParameters>"

  # RParameters: Write Data+Clock conflict flags
  $rParams = "<RParameters>`n"
  $rParams += "  <!-- Data conflict outputs for all $NumStrips strips -->`n"

  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $conflictParamId = ($SHARE_DATA_CONFLICT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $refId = $conflictParamId + "01"
    $rParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${conflictParamId}_R-%TT%${refId}`" AliasName=`"Strip${stripIdx}HasDataConflict`" />`n"
  }

  $rParams += "  <!-- Clock conflict outputs for all $NumStrips strips -->`n"

  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockConflictParamId = ($SHARE_CLOCK_CONFLICT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $clockRefId = $clockConflictParamId + "01"
    $rParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${clockConflictParamId}_R-%TT%${clockRefId}`" AliasName=`"Strip${stripIdx}HasClockConflict`" />`n"
  }

  $rParams += "  <!-- Relay conflict outputs -->`n"
  $relayConflictParamIds = @('00133', '00134', '00137', '00138')
  for ($relayIdx = 1; $relayIdx -le $MAX_EXTERNAL_RELAYS; $relayIdx++) {
    $conflictParamId = $relayConflictParamIds[$relayIdx - 1]
    $rParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${conflictParamId}_R-%TT%${conflictParamId}01`" AliasName=`"Relay${relayIdx}HasConflict`" />`n"
  }
  $rParams += "</RParameters>"

  # Single ParameterCalculation
  $calcXml = "<ParameterCalculation Id=`"%AID%_PC-%TT%00060`" Language=`"JavaScript`" Name=`"GPIOConflictTest`" LRTransformationFunc=`"NEO_DetectGPIOConflicts`" RLTransformationFunc=`"NEO_Empty`">`n"
  $calcXml += $lParams + "`n"
  $calcXml += $rParams + "`n"
  $calcXml += "</ParameterCalculation>"

  # Replace content using generic function
  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['ShareConflictCalcStart'] `
    -EndMarker $markers['ShareConflictCalcEnd'] `
    -NewContent $calcXml
}

function Generate-HardwareChangeResetCalculation {
  param(
    [string]$ShareXmlPath,
    [int]$NumStrips = $MAX_PHYSICAL_STRIPS
  )

  # Generate ParameterCalculation that resets all ports to Dummy ($GPIO_DUMMY_VALUE) on hardware change
  # Prevents conflicts from different port value mappings across hardware variants

  # LParameter: Hardware Selection
  $lParams = "<LParameters>`n"
  $lParams += "  <ParameterRefRef RefId=`"%AID%_UP-4000018_R-400001801`" AliasName=`"HardwareSelection`" />`n"
  $lParams += "</LParameters>"

  # RParameters: All Data + Clock ports
  $rParams = "<RParameters>`n"
  $rParams += "  <!-- Reset all Data ports to Dummy (${GPIO_DUMMY_VALUE}) -->`n"

  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $portParamId = ($SHARE_DATA_PORT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
    $refId = $portParamId + "01"
    $rParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${portParamId}_R-%TT%${refId}`" AliasName=`"Strip${stripIdx}DataPort`" />`n"
  }

  $rParams += "  <!-- Reset external relay ports to Dummy (${GPIO_DUMMY_VALUE}) -->`n"
  $relaySharedPortParamIds = @('00131', '00132', '00135', '00136')
  for ($relayIdx = 1; $relayIdx -le $MAX_EXTERNAL_RELAYS; $relayIdx++) {
    $sharedPortParamId = $relaySharedPortParamIds[$relayIdx - 1]
    $rParams += "  <ParameterRefRef RefId=`"%AID%_UP-%TT%${sharedPortParamId}_R-%TT%${sharedPortParamId}01`" AliasName=`"Relay${relayIdx}Port`" />`n"
  }

  $rParams += "  <!-- Reset all Clock ports to Dummy (${GPIO_DUMMY_VALUE}) -->`n"

  for ($stripIdx = 1; $stripIdx -le $NumStrips; $stripIdx++) {
    $clockPortParamId = ($SHARE_CLOCK_PORT_BASE_ID + $stripIdx).ToString().PadLeft(5, '0')
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
    -StartMarker $markers['HWChangeResetCalcStart'] `
    -EndMarker $markers['HWChangeResetCalcEnd'] `
    -NewContent $calcXml
}

function Generate-RelayCopyCalculationsInShare {
  param(
    [string]$ShareXmlPath,
    [array]$HardwareConfigs
  )

  $calcXml = "          <!-- Relay 1 -->`n"
  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $calcId = (140 + $hwIdx).ToString().PadLeft(5, '0')
    $relayParamId = (200 + $hwIdx).ToString().PadLeft(5, '0')
    $calcXml += "          <ParameterCalculation Id=`"%AID%_PC-%TT%${calcId}`" Language=`"JavaScript`" Name=`"Relay1_CopyGPIOPort_HW$hwIdx`" LRTransformationFunc=`"NEO_CopyValue`" RLTransformationFunc=`"NEO_Empty`">`n"
    $calcXml += "            <LParameters>`n"
    $calcXml += "              <ParameterRefRef RefId=`"%AID%_UP-%TT%${relayParamId}_R-%TT%${relayParamId}01`" AliasName=`"TemplateGPIOPort`" />`n"
    $calcXml += "            </LParameters>`n"
    $calcXml += "            <RParameters>`n"
    $calcXml += "              <ParameterRefRef RefId=`"%AID%_UP-%TT%00131_R-%TT%0013101`" AliasName=`"ShareGPIOPort`" />`n"
    $calcXml += "            </RParameters>`n"
    $calcXml += "          </ParameterCalculation>`n"
  }

  $calcXml += "`n          <!-- Relay 2 -->`n"
  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $calcId = (160 + $hwIdx).ToString().PadLeft(5, '0')
    $relayParamId = (220 + $hwIdx).ToString().PadLeft(5, '0')
    $calcXml += "          <ParameterCalculation Id=`"%AID%_PC-%TT%${calcId}`" Language=`"JavaScript`" Name=`"Relay2_CopyGPIOPort_HW$hwIdx`" LRTransformationFunc=`"NEO_CopyValue`" RLTransformationFunc=`"NEO_Empty`">`n"
    $calcXml += "            <LParameters>`n"
    $calcXml += "              <ParameterRefRef RefId=`"%AID%_UP-%TT%${relayParamId}_R-%TT%${relayParamId}01`" AliasName=`"TemplateGPIOPort`" />`n"
    $calcXml += "            </LParameters>`n"
    $calcXml += "            <RParameters>`n"
    $calcXml += "              <ParameterRefRef RefId=`"%AID%_UP-%TT%00132_R-%TT%0013201`" AliasName=`"ShareGPIOPort`" />`n"
    $calcXml += "            </RParameters>`n"
    $calcXml += "          </ParameterCalculation>`n"
  }

  $calcXml += "`n          <!-- Relay 3 -->`n"
  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $calcId = (180 + $hwIdx).ToString().PadLeft(5, '0')
    $relayParamId = (240 + $hwIdx).ToString().PadLeft(5, '0')
    $calcXml += "          <ParameterCalculation Id=`"%AID%_PC-%TT%${calcId}`" Language=`"JavaScript`" Name=`"Relay3_CopyGPIOPort_HW$hwIdx`" LRTransformationFunc=`"NEO_CopyValue`" RLTransformationFunc=`"NEO_Empty`">`n"
    $calcXml += "            <LParameters>`n"
    $calcXml += "              <ParameterRefRef RefId=`"%AID%_UP-%TT%${relayParamId}_R-%TT%${relayParamId}01`" AliasName=`"TemplateGPIOPort`" />`n"
    $calcXml += "            </LParameters>`n"
    $calcXml += "            <RParameters>`n"
    $calcXml += "              <ParameterRefRef RefId=`"%AID%_UP-%TT%00135_R-%TT%0013501`" AliasName=`"ShareGPIOPort`" />`n"
    $calcXml += "            </RParameters>`n"
    $calcXml += "          </ParameterCalculation>`n"
  }

  $calcXml += "`n          <!-- Relay 4 -->`n"
  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $calcId = (200 + $hwIdx).ToString().PadLeft(5, '0')
    $relayParamId = (260 + $hwIdx).ToString().PadLeft(5, '0')
    $calcXml += "          <ParameterCalculation Id=`"%AID%_PC-%TT%${calcId}`" Language=`"JavaScript`" Name=`"Relay4_CopyGPIOPort_HW$hwIdx`" LRTransformationFunc=`"NEO_CopyValue`" RLTransformationFunc=`"NEO_Empty`">`n"
    $calcXml += "            <LParameters>`n"
    $calcXml += "              <ParameterRefRef RefId=`"%AID%_UP-%TT%${relayParamId}_R-%TT%${relayParamId}01`" AliasName=`"TemplateGPIOPort`" />`n"
    $calcXml += "            </LParameters>`n"
    $calcXml += "            <RParameters>`n"
    $calcXml += "              <ParameterRefRef RefId=`"%AID%_UP-%TT%00136_R-%TT%0013601`" AliasName=`"ShareGPIOPort`" />`n"
    $calcXml += "            </RParameters>`n"
    $calcXml += "          </ParameterCalculation>`n"
  }

  $calcXml = $calcXml.TrimEnd("`n")

  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['RelayCopyCalcStart'] `
    -EndMarker $markers['RelayCopyCalcEnd'] `
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
    $dataParamId = ($HW_DATA_GPIO_BASE_ID + $hwIdx).ToString().PadLeft(3, '0')  # 700, 701, 702, ...
    $clockParamId = ($HW_CLOCK_GPIO_BASE_ID + $hwIdx).ToString().PadLeft(3, '0')  # 800, 801, 802, ...

    $hardwareList += @{
      Id = $hwIdx  # Use hardware INDEX for consistency with ParameterType
      Name = $hwConfig.DeviceName
      DataParam = $dataParamId
      ClockParam = $clockParamId
    }
  }

  # Determine parameter IDs based on type
  $manualInputParam = if ($Type -eq "Data") { "033" } else { "034" }
  $mosiParam = "035"  # Only for Data type SPI

  # Start building the XML
  $xml = "<!-- Hardware-specific $Type GPIO Selection - shows correct ports for each hardware -->`n"
  $xml += "<choose ParamRefId=`"%AID%_UP-4000018_R-400001801`">`n"

  # Special case for Hardware=255 (no hardware selected - dummy value)
  $xml += "  <!-- Keine Hardware ausgewählt (Dummy-Wert 255) -->`n"
  $xml += "  <when test=`"255`">`n"
  if ($Type -eq "Data") {
    $xml += "    <!-- Show error message: Hardware must be selected first -->`n"
    $xml += "    <ParameterSeparator Id=`"%AID%_PS-nohw%C%`" Text=`"Keine Hardware ausgewählt! Bitte wählen Sie zuerst eine Hardware aus.`" UIHint=`"Error`"/>`n"
  } else {
    $xml += "    <!-- Clock: Hardware not selected - no message (Data already shows it) -->`n"
    $xml += "    <!-- <ParameterSeparator Id=`"%AID%_PS-nohwclock%C%`" Text=`"`"/> -->`n"
  }
  $xml += "  </when>`n"

  # Generate Choose-block for each hardware
  foreach ($hw in $hardwareList) {
    $paramId = if ($Type -eq "Data") { $hw.DataParam } else { $hw.ClockParam }
    $paramRef = "${paramId}01"

    $xml += "  <!-- $($hw.Name) -->`n"
    $xml += "  <when test=`"$($hw.Id)`">`n"
    $xml += "    <ParameterRefRef RefId=`"%AID%_UP-%TT%0%C%${paramId}_R-%TT%0%C%${paramRef}`" IndentLevel=`"2`"/>`n"
    $xml += "  </when>`n"
  }

  # Default fallback to manual GPIO input
  $defaultParam = if ($Type -eq "Data") { "033" } else { "034" }
  $defaultRef = "${defaultParam}01"
  $xml += "  <when default=`"true`">`n"
  $xml += "    <!-- Fallback to manual input -->`n"
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
    $xml += "    <choose ParamRefId=`"%AID%_UP-%TT%0%C%${paramId}_R-%TT%0%C%${paramRef}`">`n"
    $xml += "      <when test=`"${GPIO_MANUAL_VALUE}`">`n"

    if ($Type -eq "Data") {
      # Data type has special SPI handling
      $xml += "        <choose ParamRefId=`"%AID%_UP-%TT%9%C%030_R-%TT%9%C%03001`">`n"
      $xml += "          <when test=`"5 21 22 23 24 25`">`n"
      $xml += "            <!-- MOSI GPIO nur bei SPI -->`n"
      $xml += "            <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${mosiParam}_R-%TT%9%C%${mosiParam}01`" IndentLevel=`"3`"/>`n"
      $xml += "          </when>`n"
      $xml += "          <when default=`"true`">`n"
      $xml += "            <!-- Data GPIO -->`n"
      $xml += "            <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${manualInputParam}_R-%TT%9%C%${manualInputParam}01`" IndentLevel=`"3`"/>`n"
      $xml += "          </when>`n"
      $xml += "        </choose>`n"
    }
    else {
      # Clock type is simpler
      $xml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${manualInputParam}_R-%TT%9%C%${manualInputParam}01`" IndentLevel=`"3`"/>`n"
    }

    $xml += "      </when>`n"
    $xml += "    </choose>`n"
    $xml += "  </when>`n"
  }

  # Default fallback for manual input
  $xml += "  <when default=`"true`">`n"
  $xml += "    <choose ParamRefId=`"%AID%_UP-%TT%9%C%${defaultParam}_R-%TT%9%C%${defaultRef}`">`n"
  $xml += "      <when test=`"${GPIO_MANUAL_VALUE}`">`n"

  if ($Type -eq "Data") {
    $xml += "        <choose ParamRefId=`"%AID%_UP-%TT%9%C%030_R-%TT%9%C%03001`">`n"
    $xml += "          <when test=`"5 21 22 23 24 25`">`n"
    $xml += "            <!-- MOSI GPIO nur bei SPI -->`n"
    $xml += "            <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${mosiParam}_R-%TT%9%C%${mosiParam}01`" IndentLevel=`"3`"/>`n"
    $xml += "          </when>`n"
    $xml += "          <when default=`"true`">`n"
    $xml += "            <!-- Data GPIO -->`n"
    $xml += "            <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${manualInputParam}_R-%TT%9%C%${manualInputParam}01`" IndentLevel=`"3`"/>`n"
    $xml += "          </when>`n"
    $xml += "        </choose>`n"
  }
  else {
    $xml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%9%C%${manualInputParam}_R-%TT%9%C%${manualInputParam}01`" IndentLevel=`"3`"/>`n"
  }

  $xml += "      </when>`n"
  $xml += "    </choose>`n"
  $xml += "  </when>`n"
  $xml += "</choose>"

  return $xml
}

# ============================================================================
# External Relay ParameterType Generator (hardware-dependent relay count)
# ============================================================================
function Generate-RelayParameterTypeInShare {
  param(
    [string]$ShareXmlPath,
    [array]$HardwareConfigs
  )

  # Find maximum relay count across all hardware variants
  $maxRelayCount = 0
  foreach ($hwConfig in $HardwareConfigs) {
    if ($hwConfig.ExtRelaisCount -gt $maxRelayCount) {
      $maxRelayCount = $hwConfig.ExtRelaisCount
    }
  }

  # Generate ParameterType with options 0 to maxRelayCount
  $ptXml = "<!-- External Relay Count ParameterType (0-$maxRelayCount based on hardware) -->`n"
  $ptXml += "              <ParameterType Id=`"%AID%_PT-ExternalRelayCount`" Name=`"ExternalRelayCount`">`n"
  $ptXml += "                <TypeRestriction Base=`"Value`" SizeInBit=`"3`" UIHint=`"DropDown`">`n"
  $ptXml += "                  <Enumeration Text=`"Deaktiviert`" Value=`"0`" Id=`"%ENID%`" />`n"
  
  for ($i = 1; $i -le $maxRelayCount; $i++) {
    $ptXml += "                  <Enumeration Text=`"$i`" Value=`"$i`" Id=`"%ENID%`" />`n"
  }
  
  $ptXml += "                </TypeRestriction>`n"
  $ptXml += "              </ParameterType>"

  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['RelayParameterTypeStart'] `
    -EndMarker $markers['RelayParameterTypeEnd'] `
    -NewContent $ptXml
}

# External Relay UI generator for share.xml
function Generate-RelayUIInShare {
  param(
    [string]$ShareXmlPath,
    [array]$HardwareConfigs
  )

  $hasHardwareSelection = $HardwareConfigs.Count -gt 1

  $buildRelayChooseBlock = {
    param(
      [int]$RelayIndex,
      [array]$HardwareConfigs,
      [bool]$HasHardwareSelection
    )

    $baseId = switch ($RelayIndex) {
      1 { 200 }
      2 { 220 }
      3 { 240 }
      4 { 260 }
    }

    if (-not $HasHardwareSelection) {
      $paramId = ($baseId).ToString().PadLeft(5, '0')
      return "    <ParameterRefRef RefId=`"%AID%_UP-%TT%${paramId}_R-%TT%${paramId}01`" IndentLevel=`"2`"/>"
    }

    $xml = "    <choose ParamRefId=`"%AID%_UP-4000018_R-400001801`">`n"
    for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
      $paramId = ($baseId + $hwIdx).ToString().PadLeft(5, '0')
      $xml += "      <when test=`"$hwIdx`">`n"
      $xml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%${paramId}_R-%TT%${paramId}01`" IndentLevel=`"2`"/>`n"
      $xml += "      </when>`n"
    }
    $xml += "      <when default=`"true`">`n"
    $xml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Unbekannte Hardware-Auswahl. Bitte prüfen Sie die Geräteeinstellung.`" UIHint=`"Error`"/>`n"
    $xml += "      </when>`n"
    $xml += "    </choose>"
    return $xml
  }

  $relay1Choose = & $buildRelayChooseBlock 1 $HardwareConfigs $hasHardwareSelection
  $relay2Choose = & $buildRelayChooseBlock 2 $HardwareConfigs $hasHardwareSelection
  $relay3Choose = & $buildRelayChooseBlock 3 $HardwareConfigs $hasHardwareSelection
  $relay4Choose = & $buildRelayChooseBlock 4 $HardwareConfigs $hasHardwareSelection

  # Build Manual GPIO checks (show GPIO number when Port=10)
  $buildManualGpioCheck = {
    param(
      [int]$RelayIndex,
      [array]$HardwareConfigs,
      [bool]$HasHardwareSelection
    )

    $baseId = switch ($RelayIndex) {
      1 { 200 }
      2 { 220 }
      3 { 240 }
      4 { 260 }
    }
    
    $gpioParamId = (154 + $RelayIndex).ToString().PadLeft(5, '0')  # 00155, 00156, 00157, 00158

    if (-not $HasHardwareSelection) {
      # Single hardware: Check Port parameter directly
      $paramId = ($baseId).ToString().PadLeft(5, '0')
      $xml = "    <choose ParamRefId=`"%AID%_UP-%TT%${paramId}_R-%TT%${paramId}01`">`n"
      $xml += "      <when test=`"10`">`n"
      $xml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%${gpioParamId}_R-%TT%${gpioParamId}01`" IndentLevel=`"3`" />`n"
      $xml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"⚠️ WARNUNG: Manuelle GPIO-Konfiguration kann zu Hardware-Konflikten führen!`" UIHint=`"Error`" />`n"
      $xml += "      </when>`n"
      $xml += "    </choose>"
      return $xml
    }

    # Multiple hardware: Check hardware selection first, then Port parameter
    $xml = "    <choose ParamRefId=`"%AID%_UP-4000018_R-400001801`">`n"
    for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
      $paramId = ($baseId + $hwIdx).ToString().PadLeft(5, '0')
      $xml += "      <when test=`"$hwIdx`">`n"
      $xml += "        <choose ParamRefId=`"%AID%_UP-%TT%${paramId}_R-%TT%${paramId}01`">`n"
      $xml += "          <when test=`"10`">`n"
      $xml += "            <ParameterRefRef RefId=`"%AID%_UP-%TT%${gpioParamId}_R-%TT%${gpioParamId}01`" IndentLevel=`"3`" />`n"
      $xml += "            <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"⚠️ WARNUNG: Manuelle GPIO-Konfiguration kann zu Hardware-Konflikten führen!`" UIHint=`"Error`" />`n"
      $xml += "          </when>`n"
      $xml += "        </choose>`n"
      $xml += "      </when>`n"
    }
    $xml += "    </choose>"
    return $xml
  }

  $relay1ManualCheck = & $buildManualGpioCheck 1 $HardwareConfigs $hasHardwareSelection
  $relay2ManualCheck = & $buildManualGpioCheck 2 $HardwareConfigs $hasHardwareSelection
  $relay3ManualCheck = & $buildManualGpioCheck 3 $HardwareConfigs $hasHardwareSelection
  $relay4ManualCheck = & $buildManualGpioCheck 4 $HardwareConfigs $hasHardwareSelection

  $uiXml = "<!-- External Relays -->`n"
  if ($hasHardwareSelection) {
    $uiXml += "<choose ParamRefId=`"%AID%_UP-4000018_R-400001801`">`n"
    $uiXml += "  <when test=`"=255`">`n"
    $uiXml += "    <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Keine Hardware ausgewählt`" UIHint=`"Headline`" />`n"
    $uiXml += "    <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Bitte wählen Sie zuerst im Reiter 'Allgemein' eine Hardware aus, bevor Sie die Relais-Steuerung konfigurieren können.`" UIHint=`"Error`" />`n"
    $uiXml += "    <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Die Relais-Konfiguration ist hardwareabhängig und kann erst nach erfolgreicher Hardware-Auswahl aktiviert werden.`" />`n"
    $uiXml += "  </when>`n"
    
    # Check which hardware variants support relays
    $hwWithRelays = @()
    $hwWithoutRelays = @()
    for ($i = 0; $i -lt $HardwareConfigs.Count; $i++) {
      if ($HardwareConfigs[$i].ExtRelaisCount -gt 0) {
        $hwWithRelays += $i
      } else {
        $hwWithoutRelays += $i
      }
    }
    
    # Generate UI for hardware WITHOUT relay support (each hardware gets own when block)
    foreach ($hwIdx in $hwWithoutRelays) {
      $uiXml += "  <when test=`"=$hwIdx`">`n"
      $uiXml += "    <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Hardware unterstützt keine Relais`" UIHint=`"Headline`" />`n"
      $uiXml += "    <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Die ausgewählte Hardware bietet keine Unterstützung für externe Relais. Bitte wählen Sie eine andere Hardware, falls Sie diese Funktion benötigen.`" UIHint=`"Error`" />`n"
      $uiXml += "  </when>`n"
    }
    
    # Generate UI for hardware WITH relay support (combined when block with default)
    if ($hwWithRelays.Count -gt 0) {
      $uiXml += "  <when default=`"true`">`n"  # Catches all hardware with relay support
    }
  }

  $uiXml += "    <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Externe Relais`" UIHint=`"Headline`" />`n"
  $uiXml += "    <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Aktivieren Sie externe Relais und wählen Sie die passenden GPIO-Ports der Hardware. Pro Relais wird ein 1-Bit Schalt-KO eingeblendet.`" />`n"
  $uiXml += "    <ParameterRefRef RefId=`"%AID%_UP-%TT%00130_R-%TT%0013001`" IndentLevel=`"1`" />`n"
  $uiXml += "    <choose ParamRefId=`"%AID%_UP-%TT%00130_R-%TT%0013001`">`n"
  $uiXml += "      <when test=`">=1`">`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Relais 1`" UIHint=`"Headline`" />`n"
  $uiXml += $relay1Choose + "`n"
  $uiXml += $relay1ManualCheck + "`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00133_R-%TT%0013301`" IndentLevel=`"2`" />`n"
  $uiXml += "        <choose ParamRefId=`"%AID%_UP-%TT%00133_R-%TT%0013301`">`n"
  $uiXml += "          <when test=`"1`">`n"
  $uiXml += "            <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"PORT KONFLIKT: Relais 1 verwendet einen bereits belegten Port. Bitte wählen Sie einen freien Port aus!`" UIHint=`"Error`" />`n"
  $uiXml += "          </when>`n"
  $uiXml += "        </choose>`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Schaltverzögerungen (0 = sofort schalten)`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00139_R-%TT%0013901`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00140_R-%TT%0014001`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Netzteilschutz (0 = deaktiviert)`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00147_R-%TT%0014701`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00159_R-%TT%0015901`" IndentLevel=`"2`" />`n"
  $uiXml += "      </when>`n"
  $uiXml += "    </choose>`n"
  $uiXml += "    <choose ParamRefId=`"%AID%_UP-%TT%00130_R-%TT%0013001`">`n"
  $uiXml += "      <when test=`">=2`">`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"`" UIHint=`"HorizontalRuler`" />`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Relais 2`" UIHint=`"Headline`" />`n"
  $uiXml += $relay2Choose + "`n"
  $uiXml += $relay2ManualCheck + "`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00134_R-%TT%0013401`" IndentLevel=`"2`" />`n"
  $uiXml += "        <choose ParamRefId=`"%AID%_UP-%TT%00134_R-%TT%0013401`">`n"
  $uiXml += "          <when test=`"1`">`n"
  $uiXml += "            <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"PORT KONFLIKT: Relais 2 verwendet einen bereits belegten Port. Bitte wählen Sie einen freien Port aus!`" UIHint=`"Error`" />`n"
  $uiXml += "          </when>`n"
  $uiXml += "        </choose>`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Schaltverzögerungen (0 = sofort schalten)`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00141_R-%TT%0014101`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00142_R-%TT%0014201`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Netzteilschutz (0 = deaktiviert)`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00148_R-%TT%0014801`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00160_R-%TT%0016001`" IndentLevel=`"2`" />`n"
  $uiXml += "      </when>`n"
  $uiXml += "    </choose>`n"
  $uiXml += "    <choose ParamRefId=`"%AID%_UP-%TT%00130_R-%TT%0013001`">`n"
  $uiXml += "      <when test=`">=3`">`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"`" UIHint=`"HorizontalRuler`" />`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Relais 3`" UIHint=`"Headline`" />`n"
  $uiXml += $relay3Choose + "`n"
  $uiXml += $relay3ManualCheck + "`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00137_R-%TT%0013701`" IndentLevel=`"2`" />`n"
  $uiXml += "        <choose ParamRefId=`"%AID%_UP-%TT%00137_R-%TT%0013701`">`n"
  $uiXml += "          <when test=`"1`">`n"
  $uiXml += "            <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"PORT KONFLIKT: Relais 3 verwendet einen bereits belegten Port. Bitte wählen Sie einen freien Port aus!`" UIHint=`"Error`" />`n"
  $uiXml += "          </when>`n"
  $uiXml += "        </choose>`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Schaltverzögerungen (0 = sofort schalten)`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00143_R-%TT%0014301`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00144_R-%TT%0014401`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Netzteilschutz (0 = deaktiviert)`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00149_R-%TT%0014901`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00161_R-%TT%0016101`" IndentLevel=`"2`" />`n"
  $uiXml += "      </when>`n"
  $uiXml += "    </choose>`n"
  $uiXml += "    <choose ParamRefId=`"%AID%_UP-%TT%00130_R-%TT%0013001`">`n"
  $uiXml += "      <when test=`">=4`">`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"`" UIHint=`"HorizontalRuler`" />`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Relais 4`" UIHint=`"Headline`" />`n"
  $uiXml += $relay4Choose + "`n"
  $uiXml += $relay4ManualCheck + "`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00138_R-%TT%0013801`" IndentLevel=`"2`" />`n"
  $uiXml += "        <choose ParamRefId=`"%AID%_UP-%TT%00138_R-%TT%0013801`">`n"
  $uiXml += "          <when test=`"1`">`n"
  $uiXml += "            <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"PORT KONFLIKT: Relais 4 verwendet einen bereits belegten Port. Bitte wählen Sie einen freien Port aus!`" UIHint=`"Error`" />`n"
  $uiXml += "          </when>`n"
  $uiXml += "        </choose>`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Schaltverzögerungen (0 = sofort schalten)`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00145_R-%TT%0014501`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00146_R-%TT%0014601`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-nnn`" Text=`"Netzteilschutz (0 = deaktiviert)`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00150_R-%TT%0015001`" IndentLevel=`"2`" />`n"
  $uiXml += "        <ParameterRefRef RefId=`"%AID%_UP-%TT%00162_R-%TT%0016201`" IndentLevel=`"2`" />`n"
  $uiXml += "      </when>`n"
  $uiXml += "    </choose>`n"

  if ($hasHardwareSelection) {
    # Close hardware WITH relay support when-block (if exists)
    if ($hwWithRelays.Count -gt 0) {
      $uiXml += "  </when>`n"
    }
    $uiXml += "</choose>`n"
  }

  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['RelayUIStart'] `
    -EndMarker $markers['RelayUIEnd'] `
    -NewContent $uiXml
}

function Generate-RelayKOsInShare {
  param(
    [string]$ShareXmlPath,
    [array]$HardwareConfigs
  )

  $hasHardwareSelection = $HardwareConfigs.Count -gt 1
  $koXml = "<!-- External Relay KOs -->`n"

  if ($hasHardwareSelection) {
    $koXml += "<choose ParamRefId=`"%AID%_UP-4000018_R-400001801`">`n"
    $koXml += "  <when test=`"!=255`">`n"
  }

  $koXml += "    <choose ParamRefId=`"%AID%_UP-%TT%00130_R-%TT%0013001`">`n"
  $koXml += "      <when test=`">=1`">`n"
  $koXml += "        <ComObjectRefRef RefId=`"%AID%_O-%TT%00008_R-%TT%0000801`" />`n"
  $koXml += "        <ComObjectRefRef RefId=`"%AID%_O-%TT%00012_R-%TT%0001201`" />`n"
  $koXml += "      </when>`n"
  $koXml += "    </choose>`n"
  $koXml += "    <choose ParamRefId=`"%AID%_UP-%TT%00130_R-%TT%0013001`">`n"
  $koXml += "      <when test=`">=2`">`n"
  $koXml += "        <ComObjectRefRef RefId=`"%AID%_O-%TT%00009_R-%TT%0000901`" />`n"
  $koXml += "        <ComObjectRefRef RefId=`"%AID%_O-%TT%00013_R-%TT%0001301`" />`n"
  $koXml += "      </when>`n"
  $koXml += "    </choose>`n"
  $koXml += "    <choose ParamRefId=`"%AID%_UP-%TT%00130_R-%TT%0013001`">`n"
  $koXml += "      <when test=`">=3`">`n"
  $koXml += "        <ComObjectRefRef RefId=`"%AID%_O-%TT%00010_R-%TT%0001001`" />`n"
  $koXml += "        <ComObjectRefRef RefId=`"%AID%_O-%TT%00014_R-%TT%0001401`" />`n"
  $koXml += "      </when>`n"
  $koXml += "    </choose>`n"
  $koXml += "    <choose ParamRefId=`"%AID%_UP-%TT%00130_R-%TT%0013001`">`n"
  $koXml += "      <when test=`">=4`">`n"
  $koXml += "        <ComObjectRefRef RefId=`"%AID%_O-%TT%00011_R-%TT%0001101`" />`n"
  $koXml += "        <ComObjectRefRef RefId=`"%AID%_O-%TT%00015_R-%TT%0001501`" />`n"
  $koXml += "      </when>`n"
  $koXml += "    </choose>`n"

  if ($hasHardwareSelection) {
    $koXml += "  </when>`n"
    $koXml += "</choose>`n"
  }

  return Replace-MarkerContent -FilePath $ShareXmlPath `
    -StartMarker $markers['RelayKOsStart'] `
    -EndMarker $markers['RelayKOsEnd'] `
    -NewContent $koXml
}

# ============================================================================
# JavaScript Generation
# ============================================================================

function Generate-ConflictDetectionJS {
  param(
    [string]$JavaScriptPath,
    [int]$NumStrips = $MAX_PHYSICAL_STRIPS,
    [array]$HardwareConfigs = @()
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

  # External relay setup
  $jsRelaySetup = "  // External relays (only active when RelayCount > 0)`n"
  $jsRelaySetup += "  var relayCount = toInt(input.RelayCount, 0);`n"
  $jsRelaySetup += "  if (relayCount < 0) relayCount = 0;`n"
  $jsRelaySetup += "  if (relayCount > $MAX_EXTERNAL_RELAYS) relayCount = $MAX_EXTERNAL_RELAYS;`n"
  $jsRelaySetup += "`n"
  $jsRelaySetup += "  // Resolve relay ports using shared params; fall back to HW-specific params`n"
  $jsRelaySetup += "  var hwSel = toInt(input.HardwareSelection, 255);`n"
  $jsRelaySetup += "`n"
  $jsRelaySetup += "  var relayPorts = [];`n"
  for ($relayIdx = 1; $relayIdx -le $MAX_EXTERNAL_RELAYS; $relayIdx++) {
    $jsRelaySetup += "  if (relayCount >= $relayIdx) relayPorts.push(NEO_ResolveRelayPort(input, hwSel, $relayIdx));"
    if ($relayIdx -lt $MAX_EXTERNAL_RELAYS) {
      $jsRelaySetup += "`n"
    }
  }

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

  # Initialize relay outputs
  $jsRelayOutputInit = "  // Relay Conflict Flags initial auf 0 setzen`n"
  for ($relayIdx = 1; $relayIdx -le $MAX_EXTERNAL_RELAYS; $relayIdx++) {
    $jsRelayOutputInit += "  output.Relay${relayIdx}HasConflict = 0;"
    if ($relayIdx -lt $MAX_EXTERNAL_RELAYS) {
      $jsRelayOutputInit += "`n"
    }
  }

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

  # Relay vs Strip conflict assignments
  $jsRelayStripDataAssignments = ""
  for ($i = 0; $i -lt $NumStrips; $i++) {
    $stripNum = $i + 1
    $condition = "i == $i"
    $jsRelayStripDataAssignments += "        if ($condition) output.Strip${stripNum}HasDataConflict = 1;`n"
  }

  $jsRelayStripClockAssignments = ""
  for ($i = 0; $i -lt $NumStrips; $i++) {
    $stripNum = $i + 1
    $condition = "i == $i"
    $jsRelayStripClockAssignments += "        if ($condition) {`n"
    $jsRelayStripClockAssignments += "          output.Strip${stripNum}HasClockConflict = 1;`n"
    $jsRelayStripClockAssignments += "          output.Strip${stripNum}HasDataConflict = 1;`n"
    $jsRelayStripClockAssignments += "        }`n"
  }

  $jsRelayConflictAssignments = ""
  $jsRelayConflictAssignmentsOther = ""
  for ($relayIdx = 0; $relayIdx -lt $MAX_EXTERNAL_RELAYS; $relayIdx++) {
    $relayNum = $relayIdx + 1
    $jsRelayConflictAssignments += "        if (r == $relayIdx) output.Relay${relayNum}HasConflict = 1;`n"
    $jsRelayConflictAssignmentsOther += "        if (s == $relayIdx) output.Relay${relayNum}HasConflict = 1;`n"
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
  $fullJS += "// HARDWARE WECHSEL: Reset alle Port-Zuweisungen auf Dummy ($GPIO_DUMMY_VALUE)`n"
  $fullJS += "// Verhindert Konflikte durch unterschiedliche Port-Values bei verschiedenen Hardware-Varianten`n"
  $fullJS += "// ============================================================================================================`n"
  $fullJS += "function NEO_ResetAllPortsOnHardwareChange(input, output, context) {`n"
  for ($i = 1; $i -le $NumStrips; $i++) {
    $fullJS += "  output.Strip${i}DataPort = $GPIO_DUMMY_VALUE;`n"
  }
  for ($i = 1; $i -le $NumStrips; $i++) {
    $fullJS += "  output.Strip${i}ClockPort = $GPIO_DUMMY_VALUE;`n"
  }
  for ($relayIdx = 1; $relayIdx -le $MAX_EXTERNAL_RELAYS; $relayIdx++) {
    $fullJS += "  output.Relay${relayIdx}Port = $GPIO_DUMMY_VALUE;`n"
  }
  $fullJS += "}`n"
  $fullJS += "`n"
  $fullJS += "// ============================================================================================================`n"
  $fullJS += "// HELPER: RESOLVE RELAY PORT`n"
  $fullJS += "// Resolves relay port from shared param (RelayNPort) or hardware-specific fallback (RelayNHWx)`n"
  $fullJS += "// ============================================================================================================`n"
  $fullJS += "function NEO_ResolveRelayPort(input, hwSel, relayIdx) {`n"
  $fullJS += "  var sharedPortParam = 'Relay' + relayIdx + 'Port';`n"
  $fullJS += "  var hwSpecificParam = 'Relay' + relayIdx + 'HW' + hwSel;`n"
  $fullJS += "  var port = toInt(input[sharedPortParam], -1);`n"
  $fullJS += "  if (port === -1 || port === $GPIO_DUMMY_VALUE) {`n"
  $fullJS += "    port = toInt(input[hwSpecificParam], $GPIO_DUMMY_VALUE);`n"
  $fullJS += "  }`n"
  $fullJS += "  return port;`n"
  $fullJS += "}`n"
  $fullJS += "`n"
  $fullJS += "// ============================================================================================================`n"
  $fullJS += "// REAL GPIO CONFLICT DETECTION`n"
  $fullJS += "// Prüft ob ein Strip denselben GPIO Port wie ein anderer Strip verwendet`n"
  $fullJS += "// Wenn Strip X denselben Port wie Strip Y hat → BEIDE bekommen Konflikt-Flag`n"
  $fullJS += "// UNTERSTÜTZT: Data vs Data, Clock vs Clock, Data vs Clock Cross-Konflikte`n"
  $fullJS += "// ============================================================================================================`n"
  $fullJS += "function NEO_DetectGPIOConflicts(input, output, context) {`n"
  $fullJS += "  // Data Port-Werte für alle $NumStrips Strips lesen (0=Nicht verwendet, $GPIO_MANUAL_VALUE=Manuell)`n"
  $fullJS += $jsInit + "`n"
  $fullJS += "  `n"
  $fullJS += "  // Clock Port-Werte für alle $NumStrips Strips lesen (0=Nicht verwendet, $GPIO_MANUAL_VALUE=Manuell)`n"
  $fullJS += $jsClockInit + "`n"
  $fullJS += "  `n"
  $fullJS += $jsRelaySetup + "`n"
  $fullJS += "  `n"
  $fullJS += $jsOutputInit + "`n"
  $fullJS += "  `n"
  $fullJS += $jsClockOutputInit + "`n"
  $fullJS += "  `n"
  $fullJS += $jsRelayOutputInit + "`n"
  $fullJS += "  `n"
  $fullJS += "  // CHECK 1: Data vs Data Konflikte`n"
  $fullJS += "  for (var i = 0; i < $NumStrips; i++) {`n"
  $fullJS += "    var dataI = dataPorts[i];`n"
  $fullJS += "    if (dataI == $GPIO_MANUAL_VALUE || dataI == $GPIO_DUMMY_VALUE) continue;  // Manuell oder Dummy`n"
  $fullJS += "    `n"
  $fullJS += "    for (var j = i + 1; j < $NumStrips; j++) {`n"
  $fullJS += "      var dataJ = dataPorts[j];`n"
  $fullJS += "      `n"
  $fullJS += "      if (dataI == dataJ && dataJ != $GPIO_MANUAL_VALUE && dataJ != $GPIO_DUMMY_VALUE) {`n"
  $fullJS += "        // BEIDE Strips bekommen Data-Konflikt-Flag`n"
  $fullJS += $jsConflictAssignmentsI
  $fullJS += "        `n"
  $fullJS += $jsConflictAssignmentsJ
  $fullJS += "      }`n"
  $fullJS += "    }`n"
  $fullJS += "  }`n"
  $fullJS += "  `n"
  $fullJS += "  // CHECK 2: Clock vs Clock Konflikte`n"
  $fullJS += "  for (var i = 0; i < $NumStrips; i++) {`n"
  $fullJS += "    var clockI = clockPorts[i];`n"
  $fullJS += "    if (clockI == $GPIO_MANUAL_VALUE || clockI == $GPIO_DUMMY_VALUE) continue;  // Manuell oder Dummy`n"
  $fullJS += "    `n"
  $fullJS += "    for (var j = i + 1; j < $NumStrips; j++) {`n"
  $fullJS += "      var clockJ = clockPorts[j];`n"
  $fullJS += "      `n"
  $fullJS += "      if (clockI == clockJ && clockJ != $GPIO_MANUAL_VALUE && clockJ != $GPIO_DUMMY_VALUE) {`n"
  $fullJS += "        // BEIDE Strips bekommen Clock-Konflikt-Flag`n"
  $fullJS += $jsClockConflictAssignmentsI
  $fullJS += "        `n"
  $fullJS += $jsClockConflictAssignmentsJ
  $fullJS += "      }`n"
  $fullJS += "    }`n"
  $fullJS += "  }`n"
  $fullJS += "  `n"
  $fullJS += "  // CHECK 3: Data vs Clock Cross-Konflikte (CRITICAL!)`n"
  $fullJS += "  for (var i = 0; i < $NumStrips; i++) {`n"
  $fullJS += "    var dataI = dataPorts[i];`n"
  $fullJS += "    if (dataI == $GPIO_MANUAL_VALUE || dataI == $GPIO_DUMMY_VALUE) continue;  // Manuell oder Dummy`n"
  $fullJS += "    `n"
  $fullJS += "    for (var j = 0; j < $NumStrips; j++) {`n"
  $fullJS += "      if (i == j) continue;  // Selber Strip`n"
  $fullJS += "      `n"
  $fullJS += "      var clockJ = clockPorts[j];`n"
  $fullJS += "      `n"
  $fullJS += "      if (dataI == clockJ && clockJ != $GPIO_MANUAL_VALUE && clockJ != $GPIO_DUMMY_VALUE) {`n"
  $fullJS += "        // Strip i hat Data-Konflikt, Strip j hat Clock-Konflikt`n"
  $fullJS += $jsCrossDataConflictAssignments
  $fullJS += "        `n"
  $fullJS += $jsCrossClockConflictAssignments
  $fullJS += "      }`n"
  $fullJS += "    }`n"
  $fullJS += "  }`n"
  $fullJS += "  `n"
  $fullJS += "  // CHECK 4: Interne Strip-Konflikte (Data = Clock im selben Strip)`n"
  $fullJS += "  for (var i = 0; i < $NumStrips; i++) {`n"
  $fullJS += "    var dataI = dataPorts[i];`n"
  $fullJS += "    var clockI = clockPorts[i];`n"
  $fullJS += "    `n"
  $fullJS += "    // Skip wenn einer der Ports manuell ist oder Dummy`n"
  $fullJS += "    if (dataI == $GPIO_MANUAL_VALUE || dataI == $GPIO_DUMMY_VALUE || clockI == $GPIO_MANUAL_VALUE || clockI == $GPIO_DUMMY_VALUE) continue;`n"
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
  $fullJS += "`n"
  $fullJS += "  // CHECK 5: Relais vs Strip-Konflikte`n"
  $fullJS += "  for (var r = 0; r < relayPorts.length; r++) {`n"
  $fullJS += "    var relayP = relayPorts[r];`n"
  $fullJS += "    if (relayP == $GPIO_MANUAL_VALUE || relayP == $GPIO_DUMMY_VALUE) continue; // Manuell oder Dummy`n"
  $fullJS += "`n"
  $fullJS += "    for (var i = 0; i < $NumStrips; i++) {`n"
  $fullJS += "      var dataI = dataPorts[i];`n"
  $fullJS += "      if (relayP == dataI && dataI != $GPIO_MANUAL_VALUE && dataI != $GPIO_DUMMY_VALUE) {`n"
  $fullJS += $jsRelayConflictAssignments
  $fullJS += $jsRelayStripDataAssignments
  $fullJS += "      }`n"
  $fullJS += "`n"
  $fullJS += "      var clockI = clockPorts[i];`n"
  $fullJS += "      if (relayP == clockI && clockI != $GPIO_MANUAL_VALUE && clockI != $GPIO_DUMMY_VALUE) {`n"
  $fullJS += $jsRelayConflictAssignments
  $fullJS += $jsRelayStripClockAssignments
  $fullJS += "      }`n"
  $fullJS += "    }`n"
  $fullJS += "  }`n"
  $fullJS += "`n"
  $fullJS += "  // CHECK 6: Relais vs Relais Konflikte`n"
  $fullJS += "  for (var r = 0; r < relayPorts.length; r++) {`n"
  $fullJS += "    var relayR = relayPorts[r];`n"
  $fullJS += "    if (relayR == $GPIO_MANUAL_VALUE || relayR == $GPIO_DUMMY_VALUE) continue;`n"
  $fullJS += "    for (var s = r + 1; s < relayPorts.length; s++) {`n"
  $fullJS += "      var relayS = relayPorts[s];`n"
  $fullJS += "      if (relayR == relayS && relayS != $GPIO_MANUAL_VALUE && relayS != $GPIO_DUMMY_VALUE) {`n"
  $fullJS += $jsRelayConflictAssignments
  $fullJS += "        `n"
  $fullJS += $jsRelayConflictAssignmentsOther
  $fullJS += "      }`n"
  $fullJS += "    }`n"
  $fullJS += "  }`n"
  $fullJS += "}`n"

  # Replace content using generic function
  $success = Replace-MarkerContent -FilePath $JavaScriptPath `
    -StartMarker $markers['JSConflictStart'] `
    -EndMarker $markers['JSConflictEnd'] `
    -NewContent $fullJS

  if (-not $success) {
    # Append at end if markers don't exist
    $jsContent = Get-Content -Path $JavaScriptPath -Raw -Encoding UTF8
    $jsContent += "`n" + $markers['JSConflictStart'] + "`n" + $fullJS + "`n" + $markers['JSConflictEnd'] + "`n"
    Set-Content -Path $JavaScriptPath -Value $jsContent -NoNewline -Encoding UTF8
  }
  return $true
}

function Generate-NetworkVisibilityJS {
  param(
    [string]$JavaScriptPath,
    [array]$HardwareConfigs
  )

  # Generate network-capable hardware INDEX array (not IDs!)
  $networkHardware = @()
  for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
    $hwConfig = $HardwareConfigs[$hwIdx]
    if ($hwConfig.HasNetwork -eq $true) {
      $networkHardware += $hwIdx  # Use hardware INDEX for consistency
    }
  }

  # Build JavaScript function
  $jsFunction = "// Set Network Module visibility based on hardware selection`n"
  $jsFunction += "function NEO_SetNetworkModuleVisibility(input, output, context) {`n"
  $jsFunction += "    var hwSelect = toInt(input.HardwareSelect, 0);`n"
  $jsFunction += "    var wifiHardware = [" + ($networkHardware -join ', ') + "]; // Auto-generated from platformio.hardware.ini`n"
  $jsFunction += "    var hasNetwork = 0;`n"
  $jsFunction += "    `n"
  $jsFunction += "    for (var i = 0; i < wifiHardware.length; i++) {`n"
  $jsFunction += "        if (hwSelect === wifiHardware[i]) {`n"
  $jsFunction += "            hasNetwork = 1;`n"
  $jsFunction += "            break;`n"
  $jsFunction += "        }`n"
  $jsFunction += "    }`n"
  $jsFunction += "    `n"
  $jsFunction += "    output.ShowNetworkModule = hasNetwork;`n"
  $jsFunction += "    info(`"Hardware `" + hwSelect + `" -> Network visible: `" + hasNetwork);`n"
  $jsFunction += "}"

  # Replace content using marker or append at end
  $success = Replace-MarkerContent -FilePath $JavaScriptPath `
    -StartMarker $markers['JSNetworkVisibilityStart'] `
    -EndMarker $markers['JSNetworkVisibilityEnd'] `
    -NewContent $jsFunction

  if (-not $success) {
    # Append at end if markers don't exist
    $jsContent = Get-Content -Path $JavaScriptPath -Raw -Encoding UTF8
    $jsContent += "`n`n" + $markers['JSNetworkVisibilityStart'] + "`n" + $jsFunction + "`n" + $markers['JSNetworkVisibilityEnd'] + "`n"
    Set-Content -Path $JavaScriptPath -Value $jsContent -NoNewline -Encoding UTF8
  }
  return $true
}

# Generate Hardware ID to Index Mapping JavaScript  
function Generate-HardwareIdMappingJS {
  param(
    [string]$JavaScriptPath,
    [array]$HardwareConfigs
  )

  $currentDate = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
  $jsMapping += "// Generated: $currentDate`n"
  $jsMapping += "// Hardware ID to Hardware Index mapping for NEO_detectHardware function`n"
  $jsMapping += "`n"
  $jsMapping += "var hardwareIdMap = {`n"
  $jsMapping += "    // AUTO-GENERATED: Hardware ID → Index mappings will be inserted here by Build-HardwareConfig.ps1`n"
  
  for ($i = 0; $i -lt $HardwareConfigs.Count; $i++) {
    $hwConfig = $HardwareConfigs[$i]
    $hwName = $hwConfig.DeviceName
    $hwId = $hwConfig.DeviceIdBit
    $jsMapping += "    $hwId`: $i"  # Hardware ID → Index
    if ($i -lt ($HardwareConfigs.Count - 1)) {
      $jsMapping += ","
    }
    $jsMapping += "  // $hwName → Index $i`n"
  }
  
  $jsMapping += "};`n"

  $jsMapping += "`n"
  $jsMapping += "// Hardware ID to Hardware Name mapping for UI display`n"
  $jsMapping += "var hardwareNameMap = {`n"
  $jsMapping += "    // AUTO-GENERATED: Hardware ID → HWName mappings will be inserted here by Build-HardwareConfig.ps1`n"
  
  for ($i = 0; $i -lt $HardwareConfigs.Count; $i++) {
    $hwConfig = $HardwareConfigs[$i]
    $hwName = $hwConfig.DeviceName
    $hwId = $hwConfig.DeviceIdBit
    $jsMapping += "    $hwId`: '$hwName'"  # Hardware ID → HWName
    if ($i -lt ($HardwareConfigs.Count - 1)) {
      $jsMapping += ","
    }
    $jsMapping += "  // $hwName → Index $i`n"
  }
  
  $jsMapping += "};`n"

  # Replace content using marker
  $success = Replace-MarkerContent -FilePath $JavaScriptPath `
    -StartMarker $markers['JSHardwareIdMapStart'] `
    -EndMarker $markers['JSHardwareIdMapEnd'] `
    -NewContent $jsMapping

  if (-not $success) {
    Write-WarningMsg "Hardware ID Mapping markers not found in JavaScript file"
    return $false
  }
  return $true
}

# Generate GPIO Port Mapping JavaScript (Port counts and labels)
function Generate-GPIOPortMappingJS {
  param(
    [string]$JavaScriptPath,
    [array]$HardwareConfigs
  )

  try {
    # Generate HW_ID-based port count mapping
    $jsPortCounts = "  // Port counts per hardware ID (HW_ID -> port count)`n"
    $jsPortCounts += "  var portCounts = {`n"

    for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
      $hwConfig = $HardwareConfigs[$hwIdx]
      
      # Skip hardware configs without GPIO ports (e.g., BCU_CONNECTOR variants)
      if (-not $hwConfig -or -not $hwConfig.DeviceIdBit -or -not $hwConfig.DeviceName -or -not $hwConfig.GPIOPorts -or $hwConfig.GPIOPorts.Count -eq 0) {
        if ($DebugOutput) { Write-Host "  [DEBUG] Skipping hardware #$hwIdx (no GPIO ports) in JavaScript generation" -ForegroundColor DarkGray }
        continue
      }
      
      $hwIdHex = "0x" + $hwConfig.DeviceIdBit.ToString('X4')
      $hwName = $hwConfig.DeviceName
      $portCount = $hwConfig.GPIOPorts.Count

      $jsPortCounts += "    ${hwIdHex}: $portCount"
      if ($hwIdx -lt $HardwareConfigs.Count - 1) { $jsPortCounts += "," }
      $jsPortCounts += "  // $hwName`n"
    }

    $jsPortCounts += "  };"

    # Generate GPIO port info mapping (labels only)
    $jsPortInfo = "`n  // GPIO port labels per hardware (for documentation)`n"
    $jsPortInfo += "  var portLabels = {`n"

    for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
      $hwConfig = $HardwareConfigs[$hwIdx]
      
      # Skip hardware configs without GPIO ports (e.g., BCU_CONNECTOR variants)
      if (-not $hwConfig -or -not $hwConfig.DeviceIdBit -or -not $hwConfig.DeviceName -or -not $hwConfig.GPIOPorts -or $hwConfig.GPIOPorts.Count -eq 0) {
        if ($DebugOutput) { Write-Host "  [DEBUG] Skipping hardware #$hwIdx (no GPIO ports) in port labels" -ForegroundColor DarkGray }
        continue
      }
      
      $hwIdHex = "0x" + $hwConfig.DeviceIdBit.ToString('X4')
      $hwName = $hwConfig.DeviceName

      $jsPortInfo += "    ${hwIdHex}: [  // $hwName`n"

      for ($portIdx = 0; $portIdx -lt $hwConfig.GPIOPorts.Count; $portIdx++) {
        $gpioPort = $hwConfig.GPIOPorts[$portIdx]
        
        # Null-check for Windows compatibility
        if (-not $gpioPort -or -not $gpioPort.Label) {
          Write-Host "    [WARNING] Port $portIdx has no label for $hwName - skipping" -ForegroundColor Yellow
          continue
        }
        
        $label = $gpioPort.Label

        $jsPortInfo += "      '$label'"
        if ($portIdx -lt $hwConfig.GPIOPorts.Count - 1) { $jsPortInfo += "," }
        $jsPortInfo += "`n"
      }

      $jsPortInfo += "    ]"
      if ($hwIdx -lt $HardwareConfigs.Count - 1) { $jsPortInfo += "," }
      $jsPortInfo += "`n"
    }

    $jsPortInfo += "  };"

    # Build final JavaScript content
    $currentDate = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $jsPortMaps = "  // Generated: $currentDate`n"
    $jsPortMaps += "  // Port selection: User selects port index (0-based) from dropdown`n"
    $jsPortMaps += "  // C++ firmware resolves index to actual GPIO via NEOPIXEL_HW_PORT_{index+1}_GPIO`n"
    $jsPortMaps += "  // GPIO numbers are compile-time defines, not available in JavaScript`n"
    $jsPortMaps += "`n"
    $jsPortMaps += $jsPortCounts
    $jsPortMaps += $jsPortInfo
    $jsPortMaps += "`n"

    # Replace content using marker
    $success = Replace-MarkerContent -FilePath $JavaScriptPath `
      -StartMarker $markers['JSStart'] `
      -EndMarker $markers['JSEnd'] `
      -NewContent $jsPortMaps

    if (-not $success) {
      Write-WarningMsg "GPIO Port Mapping markers not found in JavaScript file"
      return $false
    }
    
    return $true
  }
  catch {
    Write-ErrorMsg "Failed to generate GPIO Port Mapping JavaScript: $_"
    return $false
  }
}

# Generate C++ Hardware Mapping Header
function Generate-HardwareMappingHeader {
  param(
    [string]$HeaderPath,
    [array]$HardwareConfigs
  )

  try {
    $headerFileName = Split-Path -Leaf $HeaderPath
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
constexpr uint8_t NUM_HARDWARE_VARIANTS = $($HardwareConfigs.Count);

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

    for ($hwIdx = 0; $hwIdx -lt $HardwareConfigs.Count; $hwIdx++) {
      $hwConfig = $HardwareConfigs[$hwIdx]
      
      # Skip hardware configs without required properties
      if (-not $hwConfig -or -not $hwConfig.DeviceIdBit -or -not $hwConfig.DeviceName) {
        if ($DebugOutput) { Write-Host "  [DEBUG] Skipping hardware #$hwIdx (incomplete) in C++ header" -ForegroundColor DarkGray }
        continue
      }
      
      $hwIdHex = "0x" + $hwConfig.DeviceIdBit.ToString('X4')
      $hwName = $hwConfig.DeviceName -replace '"', '\"'

      $cppContent += "`n    { $hwIdHex, $hwIdx, `"$hwName`" }"
      if ($hwIdx -lt $HardwareConfigs.Count - 1) {
        $cppContent += ","
      }
    }

    $cppContent += @"

};

/**
 * @brief Map Device Hardware ID to Hardware Index
 *
 * @param deviceHwId The Device HW ID (16-bit)
 * @return Hardware index (0-$(($HardwareConfigs.Count - 1))) or 0 if unknown
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
 * @param hwIndex Hardware index (0-$(($HardwareConfigs.Count - 1)))
 * @return Hardware name or nullptr if invalid index
 */
inline const char* getHardwareName(uint8_t hwIndex) {
    if (hwIndex < NUM_HARDWARE_VARIANTS) {
        return HARDWARE_ID_MAP[hwIndex].name;
    }
    return nullptr;
}

/**
 * @brief Get Device HW ID by index
 *
 * @param hwIndex Hardware index (0-$(($HardwareConfigs.Count - 1)))
 * @return Device HW ID or 0 if invalid index
 */
inline uint16_t getDeviceHwId(uint8_t hwIndex) {
    if (hwIndex < NUM_HARDWARE_VARIANTS) {
        return HARDWARE_ID_MAP[hwIndex].deviceHwId;
    }
    return 0;
}

} // namespace HardwareMapping
"@

    # Write header file
    Set-Content -Path $HeaderPath -Value $cppContent -NoNewline -Encoding UTF8
    return $true
  }
  catch {
    Write-ErrorMsg "Failed to generate Hardware Mapping Header: $_"
    return $false
  }
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
    $hwName = $hwConfig.DeviceName
    $hwIndex = [array]::IndexOf($HardwareConfigs, $hwConfig)
    $parameterIndex = $HW_CLOCK_GPIO_BASE_ID + $hwIndex  # 800, 801, 802, etc.
    $parameterIndexFormatted = $parameterIndex.ToString("000")  # Convert to 800, 801, etc.
    $calcId = "089" + $hwIndex.ToString()  # 0890, 0891, 0892, etc.

    # Flat ParameterCalculation (NO Choose-wrapper!)
    $calcXml += "<!-- $hwName -->`n"
    $calcXml += "<ParameterCalculation Id=`"%AID%_PC-%TT%9%C%${calcId}`" Language=`"JavaScript`" Name=`"Strip%C%_CopyClockGPIOPort_HW${hwIndex}`" LRTransformationFunc=`"NEO_CopyValue`" RLTransformationFunc=`"NEO_Empty`">`n"
    $calcXml += "  <LParameters>`n"
    $calcXml += "    <ParameterRefRef RefId=`"%AID%_UP-%TT%0%C%${parameterIndexFormatted}_R-%TT%0%C%${parameterIndexFormatted}01`" AliasName=`"TemplateGPIOPort`" />`n"
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
    -StartMarker $markers['GPIOClockCopyCalcStart'] `
    -EndMarker $markers['GPIOClockCopyCalcEnd'] `
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
    $hwName = $hwConfig.DeviceName
    $hwIndex = [array]::IndexOf($HardwareConfigs, $hwConfig)
    $parameterIndex = $HW_DATA_GPIO_BASE_ID + $hwIndex  # 700, 701, 702, etc.
    $parameterIndexFormatted = $parameterIndex.ToString("000")  # Convert to 700, 701, etc.
    $calcId = "080" + $hwIndex.ToString()  # 0800, 0801, 0802, etc.

    # Flat ParameterCalculation (NO Choose-wrapper!)
    $calcXml += "<!-- $hwName -->`n"
    $calcXml += "<ParameterCalculation Id=`"%AID%_PC-%TT%9%C%${calcId}`" Language=`"JavaScript`" Name=`"Strip%C%_CopyGPIOPort_HW${hwIndex}`" LRTransformationFunc=`"NEO_CopyValue`" RLTransformationFunc=`"NEO_Empty`">`n"
    $calcXml += "  <LParameters>`n"
    $calcXml += "    <ParameterRefRef RefId=`"%AID%_UP-%TT%0%C%${parameterIndexFormatted}_R-%TT%0%C%${parameterIndexFormatted}01`" AliasName=`"TemplateGPIOPort`" />`n"
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
    -StartMarker $markers['GPIOCopyCalcStart'] `
    -EndMarker $markers['GPIOCopyCalcEnd'] `
    -NewContent $calcXml
}

# REMOVED: OLD Generate-ConflictUI (wrong - used Strip parameters %TT%9%C%091)
# REMOVED: Generate-ConflictResultParameters - NOT NEEDED!
# REMOVED: Generate-ConflictResultParameterRefs - NOT NEEDED!
# REMOVED: Generate-ConflictResultCalculations - NOT NEEDED!
# Solution: UI references Share.xml parameters directly (%TT%0009%C%)

function Generate-ConflictUI {
  param(
    [string]$TemplatePath
  )

  # Generate UI that shows conflicts by directly referencing Share.xml parameters
  # NO separate Strip parameters needed - just reference Share directly!

  # Hidden ParameterRefRef to Share.xml conflict parameter (makes it available for choose)
  $uiXml = "<ParameterRefRef RefId=`"%AID%_UP-%TT%0009%C%_R-%TT%0009%C%01`" IndentLevel=`"1`" />`n"

  # Add comment about Clock conflicts
  $uiXml += "<!-- Hidden Clock conflict parameter references -->`n"
  $uiXml += "<!-- We can't easily reference Clock conflicts with %C%, so we'll use Data conflicts only for now -->`n"

  # Only show conflict warning if Hardware is selected (NOT 255)
  $uiXml += "<!-- Only show conflict warning if Hardware is selected (NOT 255) -->`n"
  $uiXml += "<choose ParamRefId=`"%AID%_UP-4000018_R-400001801`">`n"
  $uiXml += "  <when test=`"255`">`n"
  $uiXml += "    <!-- No hardware selected → no conflict check -->`n"
  $uiXml += "  </when>`n"
  $uiXml += "  <when default=`"true`">`n"
  $uiXml += "    <!-- Hardware selected → check for conflicts -->`n"
  $uiXml += "    <choose ParamRefId=`"%AID%_UP-%TT%0009%C%_R-%TT%0009%C%01`">`n"
  $uiXml += "      <when test=`"1`">`n"
  $uiXml += "        <ParameterSeparator Id=`"%AID%_PS-gpioconflict%C%`" Text=`"PORT KONFLIKT: Der von Ihnen zugewiesene Port wird bereits von einem anderen physikalischen LED-Streifen verwendet. Bitte wählen Sie einen freien Port aus!`" UIHint=`"Error`" />`n"
  $uiXml += "      </when>`n"
  $uiXml += "    </choose>`n"
  $uiXml += "  </when>`n"
  $uiXml += "</choose>"

  # Replace content using generic function
  return Replace-MarkerContent -FilePath $TemplatePath `
    -StartMarker $markers['ConflictUIStart'] `
    -EndMarker $markers['ConflictUIEnd'] `
    -NewContent $uiXml
}

# ====================================================================
# Main Script
# ====================================================================

## Do not clear host to preserve previous output!
# Clear-Host

Show-OpenKNXLogo -SubTitle "Dynamic GPIO Template Generator by Erkan Çolak" -Version $SCRIPT_VERSION

Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  Generating Hardware-Specific GPIO Configuration" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host ""

Write-Step "Feature: $FeatureName"
Write-Step "Defines Prefix: $DefinesPrefix"
Write-Step "Template: $TemplateFile"
Write-Host ""

# ====================================================================
# Main Script Entry Point
# ====================================================================

# Resolve all paths relative to repo root
$repoRoot = Get-RepoRoot
$scriptDir = Join-Path $repoRoot "scripts"
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

  # Skip confirmation - proceed directly with cleanup
  Write-Host "  ✓ Proceeding with cleanup..." -ForegroundColor Green

  Write-Host ""
  Write-Host "  Cleaning files..." -ForegroundColor Cyan
  Write-Host ""

  # Clean share.xml - separate calls for detailed output
  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (ParameterTypes)" `
    -StartMarker $markers['ParamTypeStart'] `
    -EndMarker $markers['ParamTypeEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (Hardware Selection Parameter)" `
    -StartMarker $markers['HardwareParamStart'] `
    -EndMarker $markers['HardwareParamEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (Hardware Selection ParameterRef)" `
    -StartMarker $markers['HardwareParamRefStart'] `
    -EndMarker $markers['HardwareParamRefEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (Hardware Selection ParameterRefRef UI)" `
    -StartMarker $markers['HardwareParamRefRefStart'] `
    -EndMarker $markers['HardwareParamRefRefEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (GPIO Port Parameters)" `
    -StartMarker $markers['ParamDefStart'] `
    -EndMarker $markers['ParamDefEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (GPIO Clock Port Parameters)" `
    -StartMarker $markers['ClockPortParamsStart'] `
    -EndMarker $markers['ClockPortParamsEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (External Relay ParameterType)" `
    -StartMarker $markers['RelayParameterTypeStart'] `
    -EndMarker $markers['RelayParameterTypeEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (External Relay Parameters)" `
    -StartMarker $markers['RelayConfigStart'] `
    -EndMarker $markers['RelayConfigEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (External Relay Conflict Parameters)" `
    -StartMarker $markers['RelayConflictParamsStart'] `
    -EndMarker $markers['RelayConflictParamsEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (External Relay Delay Parameters)" `
    -StartMarker $markers['RelayDelayParamsStart'] `
    -EndMarker $markers['RelayDelayParamsEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (External Relay MinOffTime Parameters)" `
    -StartMarker $markers['RelayMinOffTimeParamsStart'] `
    -EndMarker $markers['RelayMinOffTimeParamsEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (External Relay Manual GPIO Parameters)" `
    -StartMarker $markers['RelayManualGpioParamsStart'] `
    -EndMarker $markers['RelayManualGpioParamsEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (External Relay Invert Parameters)" `
    -StartMarker $markers['RelayInvertParamsStart'] `
    -EndMarker $markers['RelayInvertParamsEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (External Relay ParameterRefs)" `
    -StartMarker $markers['RelayParamRefsStart'] `
    -EndMarker $markers['RelayParamRefsEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (External Relay GPIO Copy ParameterCalculations)" `
    -StartMarker $markers['RelayCopyCalcStart'] `
    -EndMarker $markers['RelayCopyCalcEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (External Relay UI)" `
    -StartMarker $markers['RelayUIStart'] `
    -EndMarker $markers['RelayUIEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (External Relay KOs)" `
    -StartMarker $markers['RelayKOsStart'] `
    -EndMarker $markers['RelayKOsEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (GPIO Conflict Parameters)" `
    -StartMarker $markers['ConflictParamsStart'] `
    -EndMarker $markers['ConflictParamsEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (GPIO Clock Conflict Parameters)" `
    -StartMarker $markers['ClockConflictParamsStart'] `
    -EndMarker $markers['ClockConflictParamsEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (GPIO Conflict ParameterRefs)" `
    -StartMarker $markers['ConflictParamRefsStart'] `
    -EndMarker $markers['ConflictParamRefsEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (GPIO Conflict ParameterCalculation)" `
    -StartMarker $markers['ShareConflictCalcStart'] `
    -EndMarker $markers['ShareConflictCalcEnd']

  Clear-MarkerContent -FilePath $shareXmlPath `
    -CleaningMessage "share.xml (Hardware Change Reset ParameterCalculation)" `
    -StartMarker $markers['HWChangeResetCalcStart'] `
    -EndMarker $markers['HWChangeResetCalcEnd']

  # Clean template XML - ALL marker blocks
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['GPIOPortParamStart'] `
    -EndMarker $markers['GPIOPortParamEnd'] `
    -CleaningMessage "GPIO Port Selection Parameter"

  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['GPIOPortParamRefStart'] `
    -EndMarker $markers['GPIOPortParamRefEnd'] `
    -CleaningMessage "GPIO Port Selection ParameterRef"

  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['GPIOClockParamRefStart'] `
    -EndMarker $markers['GPIOClockParamRefEnd'] `
    -CleaningMessage "GPIO Clock Selection ParameterRef"

  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['GPIOClockParamStart'] `
    -EndMarker $markers['GPIOClockParamEnd'] `
    -CleaningMessage "GPIO Clock Selection Parameter"

  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['GPIOPortSelectionStart'] `
    -EndMarker $markers['GPIOPortSelectionEnd'] `
    -CleaningMessage "GPIO Port Selection ParameterRefRef"

  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['GPIOClockSelectionStart'] `
    -EndMarker $markers['GPIOClockSelectionEnd'] `
    -CleaningMessage "GPIO Clock Selection ParameterRefRef"

  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['CentralWarningStart'] `
    -EndMarker $markers['CentralWarningEnd'] `
    -CleaningMessage "Central Manual GPIO Warning"

  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['ConflictCalcStart'] `
    -EndMarker $markers['ConflictCalcEnd'] `
    -CleaningMessage "ParameterCalculations"

  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['ConflictUIStart'] `
    -EndMarker $markers['ConflictUIEnd'] `
    -CleaningMessage "UI Display"

  # Clean JavaScript (Port Mapping)
  if (Test-Path $scriptJsPath) {
    Clear-MarkerContent -FilePath $scriptJsPath `
      -CleaningMessage "JavaScript (Port Mapping)" `
      -StartMarker $markers['JSStart'] `
      -EndMarker $markers['JSEnd'] `
      -PlaceholderComment "// Cleaned - Ready for regeneration"
  }

  # Clean JavaScript (Conflict Detection)
  if (Test-Path $scriptJsPath) {
    Clear-MarkerContent -FilePath $scriptJsPath `
      -CleaningMessage "JavaScript (Conflict Detection)" `
      -StartMarker $markers['JSConflictStart'] `
      -EndMarker $markers['JSConflictEnd'] `
      -PlaceholderComment "// Cleaned - Ready for regeneration"
  }

  # Clean JavaScript (Hardware ID Mapping)
  if (Test-Path $scriptJsPath) {
    Clear-MarkerContent -FilePath $scriptJsPath `
      -CleaningMessage "JavaScript (Hardware ID Mapping)" `
      -StartMarker $markers['JSHardwareIdMapStart'] `
      -EndMarker $markers['JSHardwareIdMapEnd'] `
      -PlaceholderComment "// Cleaned - Ready for regeneration"
  }

  # Clean JavaScript (Network Module Visibility)
  if (Test-Path $scriptJsPath) {
    Clear-MarkerContent -FilePath $scriptJsPath `
      -CleaningMessage "JavaScript (Network Module Visibility)" `
      -StartMarker $markers['JSNetworkVisibilityStart'] `
      -EndMarker $markers['JSNetworkVisibilityEnd'] `
      -PlaceholderComment "// Cleaned - Ready for regeneration"
  }

  # Clean template XML (GPIO Copy ParameterCalculation)
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['GPIOCopyCalcStart'] `
    -EndMarker $markers['GPIOCopyCalcEnd'] `
    -CleaningMessage "GPIO Copy ParameterCalculation"

  # Clean template XML (GPIO Clock Copy ParameterCalculation)
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['GPIOClockCopyCalcStart'] `
    -EndMarker $markers['GPIOClockCopyCalcEnd'] `
    -CleaningMessage "GPIO Clock Copy ParameterCalculation"

  # Clean template XML (Conflict Parameters)
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['ConflictParamStart'] `
    -EndMarker $markers['ConflictParamEnd'] `
    -CleaningMessage "Template Conflict Parameters"

  # Clean template XML (Conflict ParameterRefs)
  Clear-MarkerContent -FilePath $templatePath `
    -StartMarker $markers['ConflictResultRefsStart'] `
    -EndMarker $markers['ConflictResultRefsEnd'] `
    -CleaningMessage "Template Conflict ParameterRefs"

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
 *   pwsh scripts/Build-HardwareConfig.ps1
 */

#pragma once

#error "HardwareMappingData.h is empty! Run 'pwsh scripts/Build-HardwareConfig.ps1' to regenerate."

// This file will be generated by Build-HardwareConfig.ps1
// Run the script to generate hardware ID mapping from platformio.hardware.ini
"@
    Set-Content -Path $hardwareMappingPath -Value $placeholderContent -NoNewline -Encoding UTF8
    Write-Host "    ✓ $headerFileName - Emptied with placeholder" -ForegroundColor Green
  }

  Write-Host ""
  Write-Success "All marker blocks cleaned successfully!"
  Write-Host ""
  exit 0
}

if ($VerboseMode) {
  Write-Host "[DEBUG-CHECKPOINT-1] After Clean Mode: Markers.ParamTypeStart = '$($markers['ParamTypeStart'])'" -ForegroundColor Cyan
}

# Step 1: Parse ALL hardware configurations from platformio.hardware.ini
Write-Step "Scanning for hardware configurations..."

# Search for platformio.hardware.ini using configured paths
$hardwareIniPath = $null
foreach ($relativePath in $script:Config.HardwareIniPaths) {
  $testPath = Join-Path $repoRoot $relativePath
  if (Test-Path $testPath) {
    $hardwareIniPath = $testPath
    if ($relativePath -ne $script:Config.HardwareIniPaths[0]) {
      Write-Success "Using platformio.hardware.ini from $relativePath"
    }
    break
  }
}

if (-not $hardwareIniPath) {
  Write-ErrorMsg "platformio.hardware.ini not found in any of the configured locations:"
  foreach ($path in $script:Config.HardwareIniPaths) {
    Write-Host "    • $path" -ForegroundColor DarkGray
  }
  exit 1
}

$iniContent = Get-Content $hardwareIniPath -Raw -Encoding UTF8

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

    # Parse each -D flag - supports both formats:
    # Format 1: -DKEY=VALUE  (simple, no outer quotes)
    # Format 2: "-DKEY=VALUE"  (with outer quotes for special chars like parentheses)
    # Split by newlines and process each line
    $lines = $buildFlagsText -split "`n"
    foreach ($line in $lines) {
      $line = $line.Trim()
      # CRITICAL: Handle both formats correctly without corrupting escaped quotes
      if ($line -match '^"(-D.+)"$') {
        # Format 2: Outer quotes present → remove only outer quotes
        $buildFlags += $matches[1]
      } elseif ($line -match '^(-D.+)$') {
        # Format 1: No outer quotes → take as-is
        $buildFlags += $matches[1]
      }
    }
  }

  $hardwareConfigs += @{
    Name        = $sectionName
    SectionName = $sectionName
    BuildFlags  = $buildFlags
    GPIOPorts   = @()
  }
}

# Filter hardware configs if EnvironmentFilter is specified
if ($EnvironmentFilter.Count -gt 0) {
  $originalCount = $hardwareConfigs.Count
  $hardwareConfigs = $hardwareConfigs | Where-Object {
    # Exact match: Check if this section name is in the filter list
    $EnvironmentFilter -contains $_.Name
  }
  $filteredCount = $hardwareConfigs.Count
  Write-Step "Filtered: $filteredCount/$originalCount configs (exact match)"
}

if ($hardwareConfigs.Count -eq 0) {
  Write-WarningMsg "No hardware configurations found matching pattern: ${DefinesPrefix}_*"
  Write-WarningMsg "Falling back to manual GPIO mode only"
}
elseif ($hardwareConfigs.Count -gt $MAX_HARDWARE_VARIANTS) {
  Write-ErrorMsg "ERROR: Found $($hardwareConfigs.Count) hardware configurations but maximum is $MAX_HARDWARE_VARIANTS!"
  Write-ErrorMsg "Current ID allocation:"
  Write-ErrorMsg "  - Data GPIO:  $HW_DATA_GPIO_BASE_ID-$($HW_DATA_GPIO_BASE_ID + $MAX_HARDWARE_VARIANTS - 1)"
  Write-ErrorMsg "  - Clock GPIO: $HW_CLOCK_GPIO_BASE_ID-$($HW_CLOCK_GPIO_BASE_ID + $MAX_HARDWARE_VARIANTS - 1)"
  Write-ErrorMsg ""
  Write-ErrorMsg "Please either:"
  Write-ErrorMsg "  1. Reduce the number of hardware variants in platformio.hardware.ini"
  Write-ErrorMsg "  2. Increase MAX_HARDWARE_VARIANTS in this script (contact maintainer)"
  exit 1
}
else {
  Write-Success "Found $($hardwareConfigs.Count) hardware configuration(s)"
  Write-Host "  Using ID ranges: Data GPIO $HW_DATA_GPIO_BASE_ID-$($HW_DATA_GPIO_BASE_ID + $hardwareConfigs.Count - 1), Clock GPIO $HW_CLOCK_GPIO_BASE_ID-$($HW_CLOCK_GPIO_BASE_ID + $hardwareConfigs.Count - 1)" -ForegroundColor DarkGray
}

if ($VerboseMode) {
  Write-Host "[DEBUG-CHECKPOINT-2] After HW Scan: Markers.ParamTypeStart = '$($markers['ParamTypeStart'])'" -ForegroundColor Cyan
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
      # Force -CommandType Application to avoid PowerShell aliases (e.g., "cpp" = Copy-ItemProperty in PS 5.1)
      $cmdInfo = Get-Command $cmd -CommandType Application -ErrorAction SilentlyContinue
      if ($cmdInfo) {
        $compiler = $cmdInfo.Source  # Use full path from Get-Command
        Write-Step "Using system compiler: $compiler"
        break
      }
    }
    catch {
      # Compiler binary not found in PATH or not executable - try next candidate
      Write-Verbose "Compiler '$candidate' not available: $_"
    }
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
  $preprocessorArgs = @(
    "-E"  # Preprocessor only
    "-P"
    "-I$includeHardwareConfig"
    "-DFEATURE_PREFIX=$DefinesPrefix"
  )

  # Add hardware-specific build flags with quote normalization for PS 5.1 compatibility
  foreach ($flag in $hwConfig.BuildFlags) {
    # CRITICAL: Normalize escaped quotes for compiler (PS 5.1 keeps them as \")
    # Simple approach: Replace ALL occurrences of \" with "
    # This works for both formats:
    #   -DKEY=\"VALUE\" → -DKEY="VALUE"
    #   "-DKEY=\"VALUE\"" → "-DKEY="VALUE""
    $normalizedFlag = $flag -replace '\\\"', '"'
    if ($DebugOutput) { Write-Host "  [FLAG] '$normalizedFlag'" -ForegroundColor DarkGray }
    $preprocessorArgs += $normalizedFlag
  }

  # DEBUG: Show preprocessor command
  if ($DryRun) {
    $hwDefines = $hwConfig.BuildFlags | Where-Object { $_ -match "^-DOKNXHW" }
    if ($DebugOutput) { Write-Host "    [DEBUG] HW Defines: $($hwDefines -join ' ')" -ForegroundColor Magenta }
  }

  # Add simple extractor source
  $simpleExtractor = Join-Path $scriptDir "extract_gpio_config_simple.cpp"
  $preprocessorArgs += $simpleExtractor
  
  # DEBUG: Always show full compiler command AFTER adding all args
  if ($DebugOutput) {
    Write-Host "    [DEBUG] Compiler: $compiler" -ForegroundColor DarkYellow
    Write-Host "    [DEBUG] Args Count: $($preprocessorArgs.Count)" -ForegroundColor DarkYellow
    Write-Host "    [DEBUG] ALL Args:" -ForegroundColor DarkYellow
    for ($i = 0; $i -lt $preprocessorArgs.Count; $i++) {
      Write-Host "      [$i]: $($preprocessorArgs[$i])" -ForegroundColor DarkGray
    }
    Write-Host "    [DEBUG] Complete Command:" -ForegroundColor Cyan
    Write-Host "      & `"$compiler`" $($preprocessorArgs -join ' ')" -ForegroundColor White
  }
  if ($DebugOutput) { Write-Host "Compiler: $compiler $($preprocessorArgs -join ' ')" -ForegroundColor Gray }

  # Run preprocessor (temporarily disable ErrorActionPreference to avoid exception on compiler errors)
  try {
    $prevErrorAction = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    
    # Run preprocessor - array expansion passes each element as separate argument
    # INI parsing already converted \" to " so args are clean for compiler
    $extractorOutput = & $compiler $preprocessorArgs 2>&1
    
    $ErrorActionPreference = $prevErrorAction

    if ($LASTEXITCODE -ne 0) {
      Write-WarningMsg "  Preprocessing failed for $hwName - skipping"
      Write-Host "  Debug: $($extractorOutput | Select-Object -First 3)" -ForegroundColor DarkGray
      continue
    }

    # Parse output - filter to only KEY=VALUE lines (ignore warnings and line markers)
    $hwData = @{}
    foreach ($line in $extractorOutput) {
      # Remove carriage returns and trim whitespace
      $line = ($line -replace "`r", "").Trim()
      
      # Only match lines that are pure KEY=VALUE (start with capital letter, no spaces before =)
      # Use non-greedy match for key, greedy for value to handle parentheses in values
      if ($line -match '^([A-Z_][A-Z_0-9]*)=(.*)$') {
        $key = $matches[1]
        $value = $matches[2]
        $hwData[$key] = $value
      }
    }

    # DEBUG: Show extracted values
    if ($DryRun) {
      $deviceBitLines = $extractorOutput | Where-Object { $_ -match "DEVICE_ID_BIT" }
      if ($deviceBitLines) {
        if ($DebugOutput) { Write-Host "    [DEBUG] DEVICE_ID_BIT lines: $($deviceBitLines -join ' | ')" -ForegroundColor Yellow }
      }
      else {
        if ($DebugOutput) { Write-Host "    [DEBUG] No DEVICE_ID_BIT found in extractor output" -ForegroundColor Red }
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

    # Extract Network capability flag
    $hwConfig.HasNetwork = if ($hwData.ContainsKey("HW_HAS_NETWORK") -and $hwData["HW_HAS_NETWORK"] -eq "1") {
      $true
    }
    else {
      $false
    }

    # Extract External Relais Support (0=None, 1-4=Count, default=2)
    $hwConfig | Add-Member -NotePropertyName "ExtRelaisCount" -NotePropertyValue 2  # Default
    if ($hwData.ContainsKey("HW_EXT_RELAIS")) {
      $relaisCount = [int]$hwData["HW_EXT_RELAIS"]
      if ($relaisCount -ge 0 -and $relaisCount -le 4) {
        $hwConfig.ExtRelaisCount = $relaisCount
      }
    }

    # DEBUG: Show extracted device info
    $networkInfo = if ($hwConfig.HasNetwork) { " [WiFi]" } else { "" }
    $relaisInfo = " (Relais: $($hwConfig.ExtRelaisCount))"
    Write-Host "    Device: $($hwConfig.DeviceId) - $($hwConfig.DeviceName) [0x$($hwConfig.DeviceIdBit.ToString('X4'))]$networkInfo$relaisInfo" -ForegroundColor Cyan

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
            $hwData[$gpioKey] -replace '"', '' -replace '\\+', '' -replace '[()]', ''
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
      Write-WarningMsg "  No GPIO ports defined for $hwName"
    }
  }
  catch {
    Write-WarningMsg "  Error processing ${hwName}: $_"
  }
}

Write-Host ""
if ($VerboseMode) {
  Write-Host "[DEBUG-CHECKPOINT-3] After HW Processing: Markers.ParamTypeStart = '$($markers['ParamTypeStart'])'" -ForegroundColor Cyan
}
Write-Step "Step 3: Filtering hardware and generating XML structures..."

# Filter out hardware without DEVICE_HW_ID (DeviceIdBit == 0)
$hardwareConfigsFiltered = @()
$hardwareConfigsSkipped = @()

foreach ($hwConfig in $hardwareConfigs) {
  if ($hwConfig.DeviceIdBit -eq 0) {
    $hardwareConfigsSkipped += $hwConfig
    Write-WarningMsg "  Hardware '$($hwConfig.Name)' has no DEVICE_HW_ID - skipping ETS generation"
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

# Check hardware limit
$hardwareOverflow = @()
if ($hardwareConfigs.Count -gt $MAX_HARDWARE_VARIANTS) {
  Write-WarningMsg "Hardware limit exceeded: $($hardwareConfigs.Count) variants found, but only $MAX_HARDWARE_VARIANTS supported"
  Write-WarningMsg "Limiting to first $MAX_HARDWARE_VARIANTS hardware variants"
  $hardwareOverflow = $hardwareConfigs[$MAX_HARDWARE_VARIANTS..($hardwareConfigs.Count - 1)]
  $hardwareConfigs = $hardwareConfigs[0..($MAX_HARDWARE_VARIANTS - 1)]
}

if ($hardwareConfigs.Count -eq 0) {
  Write-WarningMsg "No hardware configurations with DEVICE_HW_ID found"
  Write-WarningMsg "Please add DEVICE_HW_ID defines to hardware headers"
  exit 1
}

Write-Success "Using $($hardwareConfigs.Count) hardware variant(s) with HW_ID for ETS"

# Check if we have any hardware configs with GPIO ports
$hardwareConfigsWithPorts = @($hardwareConfigs | Where-Object { $_.GPIOPorts.Count -gt 0 })

if ($hardwareConfigsWithPorts.Count -eq 0) {
  Write-WarningMsg "No hardware configurations with GPIO ports found"
  Write-WarningMsg "Generating manual-only configuration"
}
else {
  Write-Success "Successfully configured $($hardwareConfigsWithPorts.Count) hardware variant(s)"
}

# Calculate bit size needed for hardware selection
$bitsNeededForHardware = [Math]::Ceiling([Math]::Log($hardwareConfigs.Count) / [Math]::Log(2))
if ($bitsNeededForHardware -lt 1) { $bitsNeededForHardware = 1 }

Write-Step "Hardware Selection requires $bitsNeededForHardware bit(s) for $($hardwareConfigs.Count) variant(s)"

if ($VerboseMode) {
  Write-Host "[DEBUG-CHECKPOINT-4] Before ParameterType Gen: Markers.ParamTypeStart = '$($markers['ParamTypeStart'])'" -ForegroundColor Cyan
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
    <Enumeration Text="--- Bitte wählen Sie eine Hardware aus ---" Value="255" Id="%ENID%" />

"@

  for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
    $hwConfig = $hardwareConfigs[$hwIdx]
    $hwName = $hwConfig.DeviceName
    # Use hardware INDEX as Value (0,1,2,3,4,5) - JavaScript sets indices, not Hardware IDs
    $parameterTypeXml += "    <Enumeration Text=`"${hwName}`" Value=`"${hwIdx}`" Id=`"%ENID%`" />`n"
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
  #$maxValue = [Math]::Max($portCount, 10)  # Manual = 10
  #$bitsNeeded = [Math]::Ceiling([Math]::Log($maxValue + 1) / [Math]::Log(2))
  #if ($bitsNeeded -lt 4) { $bitsNeeded = 4 }
  $bitsNeeded = 8  # Force 8 bits to avoid issues with >16 ports (e.g. OKNXHW_X7 with 20 ports)
  $parameterTypeXml += @"

<!-- GPIO Port Selection for HW${hwIdx}: ${hwName} (${portCount} port(s)) -->
<ParameterType Id="%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}" Name="${FeatureName}GPIOPortHW${hwIdx}">
  <TypeRestriction Base="Value" SizeInBit="$bitsNeeded">
"@

  # Add DUMMY option first (value $GPIO_DUMMY_VALUE) - always shown, never conflicts
  $dummyEnumId = "%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}_EN-${GPIO_DUMMY_VALUE}"
  # Add port enumerations (0 to portCount-1)
  if( $portCount -eq 0) {
    Write-WarningMsg "  Hardware '$hwName' has no GPIO ports defined - generating empty GPIO Port ParameterType"
    $parameterTypeXml += "    <Enumeration Text=`"--- Setzen Sie bitte den Port manuell ---`" Value=`"${GPIO_DUMMY_VALUE}`" Id=`"${dummyEnumId}`" />`n"
    
    $dummyEnumId = "%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}_EN-${GPIO_DUMMY_VALUE_EMPTY}"
    $parameterTypeXml += "    <Enumeration Text=`"-----------------------------------------`" Value=`"${GPIO_DUMMY_VALUE_EMPTY}`" Id=`"${dummyEnumId}`" />`n"
  } else {
    $parameterTypeXml += "    <Enumeration Text=`"--- Wählen Sie bitte ein Port aus ---`" Value=`"${GPIO_DUMMY_VALUE}`" Id=`"${dummyEnumId}`" />`n"
    for ($portIdx = 0; $portIdx -lt $portCount; $portIdx++) {
      $gpioPort = $hwConfig.GPIOPorts[$portIdx]
      $gpioPortLabel = $gpioPort.Label  # Extract Label from Hashtable
      $enumId = "%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}_EN-${portIdx}"
      $parameterTypeXml += "    <Enumeration Text=`"${gpioPortLabel}`" Value=`"${portIdx}`" Id=`"${enumId}`" />`n"
    }
  }

  # Add "Manuell" option with value ${GPIO_MANUAL_VALUE}
  $manualEnumId = "%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}_EN-${GPIO_MANUAL_VALUE}"
  $parameterTypeXml += "    <Enumeration Text=`"Manuell`" Value=`"${GPIO_MANUAL_VALUE}`" Id=`"${manualEnumId}`" />`n"

  $parameterTypeXml += @"
  </TypeRestriction>
</ParameterType>
"@
}

Write-Success "Generated $($hardwareConfigs.Count) hardware-specific GPIO Port ParameterTypes"

# Step 4.7.5: Generate Clock GPIO Port ParameterTypes (identical to Data GPIO, for SPI hardware)
Write-Step "Generating Clock GPIO Port ParameterTypes..."

for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $hwConfig = $hardwareConfigs[$hwIdx]
  $hwName = $hwConfig.Name -replace "^${DefinesPrefix}_", ""
  $portCount = $hwConfig.GPIOPorts.Count

  # Calculate bits needed for this hardware's port count + Manual
  #$maxValue = [Math]::Max($portCount, $GPIO_MANUAL_VALUE)  # Manual = 10
  #$bitsNeeded = [Math]::Ceiling([Math]::Log($maxValue + 1) / [Math]::Log(2))
  #if ($bitsNeeded -lt 4) { $bitsNeeded = 4 }
  $bitsNeeded = 8 # Force 8 bits to avoid issues with >16 ports (e.g. OKNXHW_X7 with 20 ports)

  $parameterTypeXml += @"

<!-- Clock GPIO Port Selection for HW${hwIdx}: ${hwName} (${portCount} port(s)) -->
<ParameterType Id="%AID%_PT-${FeatureName}GPIOClockPortHW${hwIdx}" Name="${FeatureName}GPIOClockPortHW${hwIdx}">
  <TypeRestriction Base="Value" SizeInBit="$bitsNeeded">
"@

  $dummyEnumId = "%AID%_PT-${FeatureName}GPIOClockPortHW${hwIdx}_EN-${GPIO_DUMMY_VALUE}"
  if( $portCount -eq 0) {
    Write-WarningMsg "  Hardware '$hwName' has no GPIO ports defined - generating empty GPIO Clock Port ParameterType"
    $parameterTypeXml += "    <Enumeration Text=`"--- Setzen Sie bitte den Port manuell ---`" Value=`"${GPIO_DUMMY_VALUE}`" Id=`"${dummyEnumId}`" />`n"
  
    $dummyEnumId = "%AID%_PT-${FeatureName}GPIOClockPortHW${hwIdx}_EN-${GPIO_DUMMY_VALUE_EMPTY}"
    $parameterTypeXml += "    <Enumeration Text=`"-----------------------------------------`" Value=`"${GPIO_DUMMY_VALUE_EMPTY}`" Id=`"${dummyEnumId}`" />`n"
  } else {
    # Add DUMMY option first (value $GPIO_DUMMY_VALUE) - same as Data GPIO
    $parameterTypeXml += "    <Enumeration Text=`"--- Wählen Sie bitte ein Port aus ---`" Value=`"${GPIO_DUMMY_VALUE}`" Id=`"${dummyEnumId}`" />`n"
    # Add port enumerations (0 to portCount-1)
    for ($portIdx = 0; $portIdx -lt $portCount; $portIdx++) {
      $gpioPort = $hwConfig.GPIOPorts[$portIdx]
      $gpioPortLabel = $gpioPort.Label  # Extract Label from Hashtable
      $enumId = "%AID%_PT-${FeatureName}GPIOClockPortHW${hwIdx}_EN-${portIdx}"
      $parameterTypeXml += "    <Enumeration Text=`"${gpioPortLabel}`" Value=`"${portIdx}`" Id=`"${enumId}`" />`n"
    }
  }

  # Add "Manuell" option with value 10
  $manualEnumId = "%AID%_PT-${FeatureName}GPIOClockPortHW${hwIdx}_EN-${GPIO_MANUAL_VALUE}"
  $parameterTypeXml += "    <Enumeration Text=`"Manuell`" Value=`"${GPIO_MANUAL_VALUE}`" Id=`"${manualEnumId}`" />`n"

  $parameterTypeXml += @"
  </TypeRestriction>
</ParameterType>
"@
}

Write-Success "Generated $($hardwareConfigs.Count) hardware-specific Clock GPIO Port ParameterTypes"

# Step 4.8: Generate Hardware Selection Parameter (for share.xml)
Write-Step "Generating Hardware Selection Parameter..."
$hardwareParamXml = ""

if ($hardwareConfigs.Count -gt 1) {
  $defaultHwId = 255  # Dummy default: "--- Bitte wählen Sie eine Hardware aus ---"
  $hardwareParamXml = "                <Parameter Id=`"%AID%_UP-4000018`" Offset=`"0`" BitOffset=`"0`" Name=`"NeoPixelHardwareSelect`" ParameterType=`"%AID%_PT-${FeatureName}HardwareSelect`" Text=`"Hardware Auswahl`" Value=`"$defaultHwId`"/>"
  Write-Success "Generated Hardware Selection Parameter (4000018) with default: $defaultHwId (Dummy)"
}

# Step 4.9: Generate separate GPIO Port Parameters (one per hardware, unique IDs)
Write-Step "Generating GPIO Port Parameters (for Template)..."

# Generate Parameters with IDs 700-799 (99 hardware max) in separate namespace %TT%0%C%
$gpioPortParamXml = ""
for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $hwConfig = $hardwareConfigs[$hwIdx]
  $hwName = $hwConfig.DeviceName
  $portCount = $hwConfig.GPIOPorts.Count
  $paramId = $HW_DATA_GPIO_BASE_ID + $hwIdx
  $paramIdFormatted = $paramId.ToString("D3")

  if ($hwIdx -gt 0) {
    $gpioPortParamXml += "`n"
  }
  $gpioPortParamXml += @"
<Parameter Id="%AID%_UP-%TT%0%C%$paramIdFormatted" Offset="5" BitOffset="0" Name="NEO%C%GPIODataPortHW$hwIdx" ParameterType="%AID%_PT-${FeatureName}GPIOPortHW${hwIdx}" Text="Daten Port" Value="${GPIO_DUMMY_VALUE}"/>
"@
}

Write-Success "Generated $($hardwareConfigs.Count) GPIO Port Parameters (IDs: 700-798, namespace %TT%0%C%)"
Write-Host "  Each parameter has unique ID and hardware-specific ParameterType" -ForegroundColor DarkGray

# Step 4.9.1: Generate separate Clock GPIO Parameters (for SPI, IDs 051-068)
Write-Step "Generating Clock GPIO Parameters (for Template, SPI only)..."

$gpioClockParamXml = ""
for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $hwConfig = $hardwareConfigs[$hwIdx]
  $hwName = $hwConfig.DeviceName
  $portCount = $hwConfig.GPIOPorts.Count
  $paramId = $HW_CLOCK_GPIO_BASE_ID + $hwIdx
  $paramIdFormatted = $paramId.ToString("D3")

  if ($hwIdx -gt 0) {
    $gpioClockParamXml += "`n"
  }
  $gpioClockParamXml += @"
<Parameter Id="%AID%_UP-%TT%0%C%$paramIdFormatted" Offset="3" BitOffset="0" Name="NEO%C%GPIOClockPortHW$hwIdx" ParameterType="%AID%_PT-${FeatureName}GPIOClockPortHW${hwIdx}" Text="Clock Port" Value="${GPIO_DUMMY_VALUE}"/>
"@
}

Write-Success "Generated $($hardwareConfigs.Count) Clock GPIO Parameters (IDs: 800-898, namespace %TT%)"
Write-Host "  Each parameter uses same ParameterType as Data GPIO" -ForegroundColor DarkGray

# Step 4.9.5: Generate separate ParameterRefs (one for each Parameter)
Write-Step "Generating GPIO Port ParameterRefs (for Template)..."

$gpioPortParamRefsXml = ""
for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $paramId = $HW_DATA_GPIO_BASE_ID + $hwIdx
  $paramIdFormatted = $paramId.ToString("D3")
  $refId = "${paramIdFormatted}01"

  if ($hwIdx -gt 0) {
    $gpioPortParamRefsXml += "`n"
  }
  $gpioPortParamRefsXml += "<ParameterRef Id=`"%AID%_UP-%TT%0%C%${paramIdFormatted}_R-%TT%0%C%${refId}`" RefId=`"%AID%_UP-%TT%0%C%${paramIdFormatted}`" />"
}

Write-Success "Generated $($hardwareConfigs.Count) GPIO Port ParameterRefs (IDs: 700-798, namespace %TT%0%C%)"
Write-Host "  Each ParameterRef references its corresponding Parameter" -ForegroundColor DarkGray

# Step 4.9.6: Generate separate Clock GPIO ParameterRefs
Write-Step "Generating Clock GPIO ParameterRefs (for Template)..."

$gpioClockParamRefsXml = ""
for ($hwIdx = 0; $hwIdx -lt $hardwareConfigs.Count; $hwIdx++) {
  $paramId = $HW_CLOCK_GPIO_BASE_ID + $hwIdx
  $paramIdFormatted = $paramId.ToString("D3")
  $refId = "${paramIdFormatted}01"

  if ($hwIdx -gt 0) {
    $gpioClockParamRefsXml += "`n"
  }
  $gpioClockParamRefsXml += "<ParameterRef Id=`"%AID%_UP-%TT%0%C%${paramIdFormatted}_R-%TT%0%C%${refId}`" RefId=`"%AID%_UP-%TT%0%C%${paramIdFormatted}`" />"
}

Write-Success "Generated $($hardwareConfigs.Count) Clock GPIO ParameterRefs (IDs: 051-068, namespace %TT%0%C%)"
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
# Step 4.11-4.14: Generate GPIO Conflict Detection (SHARE.XML IMPLEMENTATION)
# ====================================================================================================
# IMPORTANT: Only run once for Hardware.templ.xml (share.xml is global, not per-template)
# Strip.templ.xml will skip this section to avoid duplicate generation
if ($templatePath -like "*Hardware.templ.xml") {
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

  $shareContent = Get-Content -Path $shareXmlPath -Raw -Encoding UTF8

  # Insert STATIC Conflict Parameters
  $startMarker = $markers['ConflictParamsStart']
  $endMarker = $markers['ConflictParamsEnd']

  if ($shareContent -match [regex]::Escape($startMarker)) {
    $escapedStart = [regex]::Escape($startMarker)
    $escapedEnd = [regex]::Escape($endMarker)
    $pattern = "(?ms)(^[ \t]*)($escapedStart\r?\n)(.*?)(\r?\n\1$escapedEnd)"
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
      Set-Content -Path $shareXmlPath -Value $shareContent -NoNewline -Encoding UTF8
      Write-Success "Inserted 18 UNION Conflict Parameters into share.xml"
    }
  }
  else {
    Write-WarningMsg "GPIO Conflict Parameters markers not found in share.xml"
  }

  # ====================================================================================================
  # Step 4.14: Generate GPIO Port Parameters, Clock Parameters, and Calculations
  # ====================================================================================================
  Write-Step "Generating GPIO Conflict Detection System..."

  # Generate GPIO Port Parameters in share.xml (00101-00106)
Write-Host "  • Generating share.xml GPIO Port Parameters (00101-00108)..." -ForegroundColor Cyan
if (Generate-GPIOPortParametersInShare -ShareXmlPath $shareXmlPath -NumStrips $MAX_PHYSICAL_STRIPS) {
  Write-Success "Share.xml GPIO Port Parameters generated ($MAX_PHYSICAL_STRIPS strips)"
}
else {
  Write-WarningMsg "Share.xml GPIO Port Parameter markers not found - skipped"
}

# Generate Clock GPIO Port Parameters in share.xml (00111-00118)
Write-Host "  • Generating share.xml Clock GPIO Port Parameters (00111-00118)..." -ForegroundColor Cyan
if (Generate-ClockPortParametersInShare -ShareXmlPath $shareXmlPath -NumStrips $MAX_PHYSICAL_STRIPS) {
  Write-Success "Share.xml Clock GPIO Port Parameters generated ($MAX_PHYSICAL_STRIPS strips)"
}
else {
  Write-WarningMsg "Share.xml Clock GPIO Port Parameter markers not found - skipped"
}

# Generate External Relay Parameters in share.xml
Write-Host "  • Generating share.xml External Relay Parameters..." -ForegroundColor Cyan
if (Generate-RelayConfigParametersInShare -ShareXmlPath $shareXmlPath -HardwareConfigs $hardwareConfigs) {
  Write-Success "Share.xml External Relay Parameters generated"
}
else {
  Write-WarningMsg "Share.xml External Relay Parameter markers not found - skipped"
}

# Generate External Relay Conflict Parameters in share.xml
Write-Host "  • Generating share.xml External Relay Conflict Parameters..." -ForegroundColor Cyan
if (Generate-RelayConflictParametersInShare -ShareXmlPath $shareXmlPath) {
  Write-Success "Share.xml External Relay Conflict Parameters generated"
}
else {
  Write-WarningMsg "Share.xml External Relay Conflict Parameter markers not found - skipped"
}

# Generate External Relay Delay Parameters in share.xml
Write-Host "  • Generating share.xml External Relay Delay Parameters..." -ForegroundColor Cyan
if (Generate-RelayDelayParametersInShare -ShareXmlPath $shareXmlPath) {
  Write-Success "Share.xml External Relay Delay Parameters generated"
}
else {
  Write-WarningMsg "Share.xml External Relay Delay Parameter markers not found - skipped"
}

# Generate External Relay MinOffTime Parameters in share.xml
Write-Host "  • Generating share.xml External Relay MinOffTime Parameters..." -ForegroundColor Cyan
if (Generate-RelayMinOffTimeParametersInShare -ShareXmlPath $shareXmlPath) {
  Write-Success "Share.xml External Relay MinOffTime Parameters generated"
}
else {
  Write-WarningMsg "Share.xml External Relay MinOffTime Parameter markers not found - skipped"
}

# Generate External Relay Manual GPIO Parameters in share.xml
Write-Host "  • Generating share.xml External Relay Manual GPIO Parameters..." -ForegroundColor Cyan
if (Generate-RelayManualGpioParametersInShare -ShareXmlPath $shareXmlPath) {
  Write-Success "Share.xml External Relay Manual GPIO Parameters generated"
}
else {
  Write-WarningMsg "Share.xml External Relay Manual GPIO Parameter markers not found - skipped"
}

# Generate External Relay Invert Parameters in share.xml
Write-Host "  • Generating share.xml External Relay Invert Parameters..." -ForegroundColor Cyan
if (Generate-RelayInvertParametersInShare -ShareXmlPath $shareXmlPath) {
  Write-Success "Share.xml External Relay Invert Parameters generated"
}
else {
  Write-WarningMsg "Share.xml External Relay Invert Parameter markers not found - skipped"
}

# Generate External Relay ParameterRefs in share.xml
Write-Host "  • Generating share.xml External Relay ParameterRefs..." -ForegroundColor Cyan
if (Generate-RelayParameterRefsInShare -ShareXmlPath $shareXmlPath -HardwareConfigs $hardwareConfigs) {
  Write-Success "Share.xml External Relay ParameterRefs generated"
}
else {
  Write-WarningMsg "Share.xml External Relay ParameterRefs markers not found - skipped"
}

# Generate External Relay GPIO Copy ParameterCalculations in share.xml
Write-Host "  • Generating share.xml External Relay GPIO Copy ParameterCalculations..." -ForegroundColor Cyan
if (Generate-RelayCopyCalculationsInShare -ShareXmlPath $shareXmlPath -HardwareConfigs $hardwareConfigs) {
  Write-Success "Share.xml External Relay GPIO Copy ParameterCalculations generated"
}
else {
  Write-WarningMsg "Share.xml External Relay GPIO Copy ParameterCalculation markers not found - skipped"
}

# Generate External Relay ParameterType in share.xml (hardware-dependent)
Write-Host "  • Generating share.xml External Relay ParameterType..." -ForegroundColor Cyan
if (Generate-RelayParameterTypeInShare -ShareXmlPath $shareXmlPath -HardwareConfigs $hardwareConfigs) {
  Write-Success "Share.xml External Relay ParameterType generated"
}
else {
  Write-WarningMsg "Share.xml External Relay ParameterType markers not found - skipped"
}

# Generate External Relay UI in share.xml
Write-Host "  • Generating share.xml External Relay UI..." -ForegroundColor Cyan
if (Generate-RelayUIInShare -ShareXmlPath $shareXmlPath -HardwareConfigs $hardwareConfigs) {
  Write-Success "Share.xml External Relay UI generated"
}
else {
  Write-WarningMsg "Share.xml External Relay UI markers not found - skipped"
}

# Generate External Relay KOs in share.xml
Write-Host "  • Generating share.xml External Relay KOs..." -ForegroundColor Cyan
if (Generate-RelayKOsInShare -ShareXmlPath $shareXmlPath -HardwareConfigs $hardwareConfigs) {
  Write-Success "Share.xml External Relay KOs generated"
}
else {
  Write-WarningMsg "Share.xml External Relay KOs markers not found - skipped"
}

# Generate Conflict Parameters in share.xml (00091-00098)
Write-Host "  • Generating share.xml Conflict Parameters (00091-00098)..." -ForegroundColor Cyan
if (Generate-ConflictParametersInShare -ShareXmlPath $shareXmlPath -NumStrips $MAX_PHYSICAL_STRIPS) {
  Write-Success "Share.xml Conflict Parameters generated ($MAX_PHYSICAL_STRIPS strips)"
}
else {
  Write-WarningMsg "Share.xml Conflict Parameter markers not found - skipped"
}

# Generate Clock Conflict Parameters in share.xml (00163-00170)
Write-Host "  • Generating share.xml Clock Conflict Parameters (00163-00170)..." -ForegroundColor Cyan
if (Generate-ClockConflictParametersInShare -ShareXmlPath $shareXmlPath -NumStrips $MAX_PHYSICAL_STRIPS) {
  Write-Success "Share.xml Clock Conflict Parameters generated ($MAX_PHYSICAL_STRIPS strips)"
}
else {
  Write-WarningMsg "Share.xml Clock Conflict Parameter markers not found - skipped"
}

# Generate Conflict ParameterRefs in share.xml
Write-Host "  • Generating share.xml Conflict ParameterRefs..." -ForegroundColor Cyan
if (Generate-ConflictParameterRefsInShare -ShareXmlPath $shareXmlPath -NumStrips $MAX_PHYSICAL_STRIPS) {
  Write-Success "Share.xml Conflict ParameterRefs generated"
}
else {
  Write-WarningMsg "Share.xml Conflict ParameterRefs markers not found - skipped"
}

# Generate SINGLE ParameterCalculation in share.xml
Write-Host "  • Generating ParameterCalculation in share.xml..." -ForegroundColor Cyan
if (Generate-ConflictCalculation -ShareXmlPath $shareXmlPath -NumStrips $MAX_PHYSICAL_STRIPS -HardwareConfigs $hardwareConfigs) {
  Write-Success "ParameterCalculation generated in share.xml (reads strip+relay ports, writes strip+relay conflicts)"
}
else {
  Write-WarningMsg "ParameterCalculation markers not found - skipped"
}

# Generate Hardware Change Reset ParameterCalculation
Write-Host "  • Generating Hardware Change Reset ParameterCalculation..." -ForegroundColor Cyan
if (Generate-HardwareChangeResetCalculation -ShareXmlPath $shareXmlPath -NumStrips $MAX_PHYSICAL_STRIPS) {
  Write-Success "Hardware Change Reset ParameterCalculation generated (resets all ports to $GPIO_DUMMY_VALUE)"
}
else {
  Write-WarningMsg "Hardware Change Reset markers not found - skipped"
}

# Generate JavaScript Detection Function
Write-Host "  • Generating JavaScript (NEO_DetectGPIOConflicts)..." -ForegroundColor Cyan
if (Generate-ConflictDetectionJS -JavaScriptPath $scriptJsPath -NumStrips $MAX_PHYSICAL_STRIPS -HardwareConfigs $hardwareConfigs) {
  Write-Success "JavaScript conflict detection generated"
}
else {
  Write-WarningMsg "JavaScript generation failed"
}

# Generate Network Visibility JavaScript Function  
Write-Host "  • Generating JavaScript (NEO_SetNetworkModuleVisibility)..." -ForegroundColor Cyan
if (Generate-NetworkVisibilityJS -JavaScriptPath $scriptJsPath -HardwareConfigs $hardwareConfigs) {
  Write-Success "JavaScript network visibility generated"
}
else {
  Write-WarningMsg "JavaScript network visibility generation failed"
}

# Generate Hardware ID Mapping JavaScript
Write-Host "  • Generating JavaScript (hardwareIdMap)..." -ForegroundColor Cyan
if (Generate-HardwareIdMappingJS -JavaScriptPath $scriptJsPath -HardwareConfigs $hardwareConfigs) {
  Write-Success "JavaScript hardware ID mapping generated"
}
else {
  Write-WarningMsg "JavaScript hardware ID mapping generation failed"
}

# Generate GPIO Copy ParameterCalculation in Template (0%C%001 → 0010%C%)
# ONLY for Hardware.templ.xml (not for Strip.templ.xml)
if ($templatePath -like "*Hardware.templ.xml") {
  Write-Host "  • Generating GPIO Copy ParameterCalculation in Template..." -ForegroundColor Cyan
  if (Generate-GPIOCopyCalculation -TemplatePath $templatePath -HardwareConfigs $hardwareConfigs) {
    Write-Success "Template GPIO Copy Calculation generated (dynamic hardware mapping)"
  }
  else {
    Write-WarningMsg "Template GPIO Copy Calculation markers not found - skipped"
  }
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping GPIO Copy Calculation (not Hardware.templ.xml)" -ForegroundColor DarkGray }
}

# Generate Clock GPIO Copy ParameterCalculation in Template (0%C%051 → 0011%C%)
# ONLY for Hardware.templ.xml (not for Strip.templ.xml)
if ($templatePath -like "*Hardware.templ.xml") {
  Write-Host "  • Generating Clock GPIO Copy ParameterCalculation in Template..." -ForegroundColor Cyan
  if (Generate-GPIOClockCopyCalculation -TemplatePath $templatePath -HardwareConfigs $hardwareConfigs) {
    Write-Success "Template Clock GPIO Copy Calculation generated (dynamic hardware mapping)"
  }
  else {
    Write-WarningMsg "Template Clock GPIO Copy Calculation markers not found - skipped"
  }
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping Clock GPIO Copy Calculation (not Hardware.templ.xml)" -ForegroundColor DarkGray }
}

# Generate UI in template (references share.xml parameters with %C% token)
# ONLY for Strip.templ.xml (not for Hardware.templ.xml)
if ($templatePath -like "*Strip.templ.xml") {
  Write-Host "  • Generating Conflict UI for Template..." -ForegroundColor Cyan
  if (Generate-ConflictUI -TemplatePath $templatePath) {
    Write-Success "Template UI generated (references share.xml 0009%C%)"
  }
  else {
    Write-WarningMsg "Template UI markers not found - skipped"
  }
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping Conflict UI (not Strip.templ.xml)" -ForegroundColor DarkGray }
}

# Template Conflict Parameters - NOT NEEDED!
# Template UI uses 0009%C% which directly references share.xml parameters 00091-00096
# Creating Template parameters would DUPLICATE the IDs and cause conflicts
Write-Host "  • Template Conflict Parameters: Using share.xml parameters directly via %C% token..." -ForegroundColor Cyan
Write-Success "Template references share.xml 00091-00096 via 0009%C% pattern (no duplicates)"

  Write-Success "GPIO Conflict Detection System Complete"
  Write-Host "  • share.xml: Parameters 00091-00096, Calculation reads 00101-00106" -ForegroundColor DarkGray
  Write-Host "  • Template: UI references 0009%C% → Strip1=00091, Strip2=00092, etc." -ForegroundColor DarkGray
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping GPIO Conflict Detection System (already generated for Hardware.templ.xml)" -ForegroundColor DarkGray }
}

# ====================================================================================================
# Step 5: Update share.xml with ParameterCalculation
# ====================================================================================================
# IMPORTANT: Only run once for Hardware.templ.xml (share.xml is global, not per-template)
# Strip.templ.xml will skip this section to avoid duplicate generation
if ($templatePath -like "*Hardware.templ.xml") {
  Write-Step "Generating GPIO Conflict ParameterCalculation for share.xml..."

  $shareContent = Get-Content -Path $shareXmlPath -Raw -Encoding UTF8

if ($DebugOutput) {
  Write-Host "  [DEBUG] Config.Markers.ParamTypeStart = '$($markers['ParamTypeStart'])'" -ForegroundColor Magenta
  Write-Host "  [DEBUG] Config.Markers.ParamTypeEnd = '$($markers['ParamTypeEnd'])'" -ForegroundColor Magenta
}

$startMarker = $markers['ParamTypeStart']
$endMarker = $markers['ParamTypeEnd']

if ($DebugOutput) {
  Write-Host "  [DEBUG] startMarker = '$startMarker'" -ForegroundColor Magenta
  Write-Host "  [DEBUG] endMarker = '$endMarker'" -ForegroundColor Magenta
}

# Pattern WITH leading whitespace capture to preserve marker indentation
$escapedStart = [regex]::Escape($startMarker)
$escapedEnd = [regex]::Escape($endMarker)
$pattern = "(?ms)(^[ \t]*)($escapedStart\r?\n)(.*?)(\r?\n\1$escapedEnd)"

if ($DebugOutput) {
  Write-Host "  [DEBUG] DryRun = $DryRun" -ForegroundColor Magenta
  Write-Host "  [DEBUG] Pattern: $pattern" -ForegroundColor Magenta
  Write-Host "  [DEBUG] Pattern length: $($pattern.Length)" -ForegroundColor Magenta
}

# DEBUG: Check how many times the pattern matches
if ($DebugOutput) {
  Write-Host "  [DEBUG] Start marker: '$startMarker'" -ForegroundColor Yellow
  Write-Host "  [DEBUG] End marker: '$endMarker'" -ForegroundColor Yellow
  Write-Host "  [DEBUG] Pattern: $pattern" -ForegroundColor Yellow
  $regexMatches = [regex]::Matches($shareContent, $pattern)
  Write-Host "  [DEBUG] Pattern matches $($regexMatches.Count) time(s) in share.xml" -ForegroundColor $(if ($regexMatches.Count -ne 1) { 'Red' }else { 'Green' })
  if ($regexMatches.Count -gt 1) {
    Write-Host "  [DEBUG] THIS IS THE BUG - Pattern should match exactly ONCE!" -ForegroundColor Red
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
$shareContent = Get-Content -Path $shareXmlPath -Raw -Encoding UTF8

if ($hardwareParamXml -ne "") {
  $startMarker = $markers['HardwareParamStart']
  $endMarker = $markers['HardwareParamEnd']
  $escapedStart = [regex]::Escape($startMarker)
  $escapedEnd = [regex]::Escape($endMarker)
  $pattern = "(?ms)(^[ \t]*)($escapedStart\r?\n)(.*?)(\r?\n\1$escapedEnd)"

  if ($shareContent -match $pattern) {
    $replacement = "`$1`$2$hardwareParamXml`$4"
    $shareContent = $shareContent -replace $pattern, $replacement
    Set-Content -Path $shareXmlPath -Value $shareContent -NoNewline -Encoding UTF8
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
  $startMarker = $markers['HardwareParamRefStart']
  $endMarker = $markers['HardwareParamRefEnd']

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

$startMarker = $markers['HardwareParamRefRefStart']
$endMarker = $markers['HardwareParamRefRefEnd']

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
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping share.xml updates (already generated for first template)" -ForegroundColor DarkGray }
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
# Single warning shown when EITHER Data OR Clock is set to "Manuell"
# CRITICAL: ParameterSeparator appears only ONCE in code to avoid duplicate warnings
$centralWarningXml = @"
<!-- Zentrale Warnung: Zeige Warnung wenn Data oder Clock Port auf "Manuell" gesetzt ist -->
<!-- Hidden parameter references -->
<ParameterRefRef RefId="%AID%_UP-%TT%0010%C%_R-%TT%0010%C%01" IndentLevel="1" />
<ParameterRefRef RefId="%AID%_UP-%TT%0011%C%_R-%TT%0011%C%01" IndentLevel="1" />
<!-- Data Port = Manuell OR (LED is SPI AND Clock Port = Manuell) -->
<choose ParamRefId="%AID%_UP-%TT%0010%C%_R-%TT%0010%C%01">
  <when test="${GPIO_MANUAL_VALUE}">
    <!-- Data Port is Manuell → show warning (regardless of Clock Port) -->
  </when>
  <when default="true">
    <!-- Data Port NOT Manuell → check if Clock Port is Manuell for SPI LEDs -->
    <choose ParamRefId="%AID%_UP-%TT%9%C%030_R-%TT%9%C%03001">
      <when test="5 21 22 23 24 25">
        <!-- LED type is SPI → check Clock Port -->
        <choose ParamRefId="%AID%_UP-%TT%0011%C%_R-%TT%0011%C%01">
          <when test="${GPIO_MANUAL_VALUE}">
            <!-- Clock Port is Manuell → show warning -->
          </when>
        </choose>
      </when>
    </choose>
  </when>
</choose>
<!-- Single ParameterSeparator shown if ANY of the above conditions matched -->
<choose ParamRefId="%AID%_UP-%TT%0010%C%_R-%TT%0010%C%01">
  <when test="${GPIO_MANUAL_VALUE}">
    <ParameterSeparator Id="%AID%_PS-manualgpio%C%" Text="WARNUNG: Manuelle GPIO-Konfiguration nur für Experten!&#xD;&#xA;Fehlerhafte Einstellungen können das Gerät dauerhaft unbrauchbar machen, die Kommunikation blockieren oder Hardware beschädigen." UIHint="Information" />
  </when>
</choose>
<choose ParamRefId="%AID%_UP-%TT%9%C%030_R-%TT%9%C%03001">
  <when test="5 21 22 23 24 25">
    <choose ParamRefId="%AID%_UP-%TT%0011%C%_R-%TT%0011%C%01">
      <when test="${GPIO_MANUAL_VALUE}">
        <choose ParamRefId="%AID%_UP-%TT%0010%C%_R-%TT%0010%C%01">
          <when test="${GPIO_MANUAL_VALUE}">
            <!-- Data already Manuell → warning already shown above, skip -->
          </when>
          <when default="true">
            <!-- Only Clock is Manuell → show warning -->
            <ParameterSeparator Id="%AID%_PS-manualgpio%C%" Text="WARNUNG: Manuelle GPIO-Konfiguration nur für Experten!&#xD;&#xA;Fehlerhafte Einstellungen können das Gerät dauerhaft unbrauchbar machen, die Kommunikation blockieren oder Hardware beschädigen." UIHint="Information" />
          </when>
        </choose>
      </when>
    </choose>
  </when>
</choose>
"@

Write-Success "Generated Central Manual GPIO Warning with ParameterRefRef structure"
Write-Host "  Hidden ParameterRefRef followed by choose-blocks - same pattern as GPIO conflict" -ForegroundColor DarkGray

# Step 7: Generate JavaScript GPIO port mapping
Write-Step "Generating JavaScript GPIO port mapping..."

$scriptJsPath = Resolve-RepoPath "src/NeoPixel.script.js"
if (Generate-GPIOPortMappingJS -JavaScriptPath $scriptJsPath -HardwareConfigs $hardwareConfigs) {
  Write-Success "JavaScript GPIO port mapping generated"
}
else {
  Write-ErrorMsg "Failed to generate JavaScript GPIO port mapping"
  Write-Host "  This usually means:" -ForegroundColor Yellow
  Write-Host "  - Template file (NeoPixel.script.js) is outdated or missing markers" -ForegroundColor Yellow
  Write-Host "  - Hardware config data is incomplete or malformed" -ForegroundColor Yellow
  exit 1
}

# Step 8.5: Update template file with GPIO Port Selection Parameters (001-032, namespace 0)
# ONLY for Hardware.templ.xml (not for Strip.templ.xml)
if ($templatePath -like "*Hardware.templ.xml") {
  Write-Step "Updating template file with GPIO Port Selection Parameters (8 separate)..."

  $gpioPortParamStartMarker = $markers['GPIOPortParamStart']
  $gpioPortParamEndMarker = $markers['GPIOPortParamEnd']

  if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioPortParamStartMarker -EndMarker $gpioPortParamEndMarker -NewContent $gpioPortParamXml) {
    if ($DryRun) {
      Write-Host "  [DRY-RUN] Would insert GPIO Port Selection Parameters (001-032, namespace 0)" -ForegroundColor Yellow
    }
    else {
      Write-Success "GPIO Port Selection Parameters (001-032, namespace 0) inserted - $($hardwareConfigs.Count) parameters"
    }
  }
  else {
    Write-WarningMsg "GPIO Port Selection Parameter markers not found in template"
  }
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping GPIO Port Parameters (not Hardware.templ.xml)" -ForegroundColor DarkGray }
}

# Step 8.5.5.1: Update template file with Clock GPIO Selection Parameters (200-232, namespace 0)
# ONLY for Hardware.templ.xml (not for Strip.templ.xml)
if ($templatePath -like "*Hardware.templ.xml") {
  Write-Step "Updating template file with Clock GPIO Selection Parameters (8 separate, SPI)..."

  $gpioClockParamStartMarker = $markers['GPIOClockParamStart']
  $gpioClockParamEndMarker = $markers['GPIOClockParamEnd']

  if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioClockParamStartMarker -EndMarker $gpioClockParamEndMarker -NewContent $gpioClockParamXml) {
    if ($DryRun) {
      Write-Host "  [DRY-RUN] Would insert Clock GPIO Selection Parameters (051-089)" -ForegroundColor Yellow
    }
    else {
      Write-Success "Clock GPIO Selection Parameters (051-089) inserted - $($hardwareConfigs.Count) parameters for SPI"
    }
  }
  else {
    Write-WarningMsg "Clock GPIO Selection Parameter markers not found in template"
  }
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping Clock GPIO Parameters (not Hardware.templ.xml)" -ForegroundColor DarkGray }
}

# Step 8.5.6: Update template file with GPIO Port Selection ParameterRefs (8 separate)
# ONLY for Hardware.templ.xml (not for Strip.templ.xml)
if ($templatePath -like "*Hardware.templ.xml") {
  Write-Step "Updating template file with GPIO Port Selection ParameterRefs (8 separate)..."

  $gpioPortParamRefStartMarker = $markers['GPIOPortParamRefStart']
  $gpioPortParamRefEndMarker = $markers['GPIOPortParamRefEnd']

  if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioPortParamRefStartMarker -EndMarker $gpioPortParamRefEndMarker -NewContent $gpioPortParamRefsXml) {
    if ($DryRun) {
      Write-Host "  [DRY-RUN] Would insert 8 GPIO Port Selection ParameterRefs (07301-08001)" -ForegroundColor Yellow
    }
    else {
      Write-Success "GPIO Port Selection ParameterRefs (07301-08001) inserted - 8 separate refs"
    }
  }
  else {
    Write-WarningMsg "GPIO Port Selection ParameterRef markers not found in template"
  }
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping GPIO Port ParameterRefs (not Hardware.templ.xml)" -ForegroundColor DarkGray }
}

# Step 8.5.6.1: Update template file with GPIO Clock Selection ParameterRefs
# ONLY for Hardware.templ.xml (not for Strip.templ.xml)
if ($templatePath -like "*Hardware.templ.xml") {
  Write-Step "Updating template file with GPIO Clock Selection ParameterRefs..."

  $gpioClockParamRefStartMarker = $markers['GPIOClockParamRefStart']
  $gpioClockParamRefEndMarker = $markers['GPIOClockParamRefEnd']

  if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioClockParamRefStartMarker -EndMarker $gpioClockParamRefEndMarker -NewContent $gpioClockParamRefsXml) {
    if ($DryRun) {
      Write-Host "  [DRY-RUN] Would insert 8 GPIO Clock Selection ParameterRefs (08101-08801)" -ForegroundColor Yellow
    }
    else {
      Write-Success "GPIO Clock Selection ParameterRefs (08101-08801) inserted - 8 separate refs"
    }
  }
  else {
    Write-WarningMsg "GPIO Clock Selection ParameterRef markers not found in template"
  }
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping GPIO Clock ParameterRefs (not Hardware.templ.xml)" -ForegroundColor DarkGray }
}

# Step 8.5.7: Update template file with GPIO Port Selection ParameterRefRef (choose-block)
# ONLY for Hardware.templ.xml (not for Strip.templ.xml)
if ($templatePath -like "*Hardware.templ.xml") {
  Write-Step "Updating template file with GPIO Port Selection ParameterRefRef (choose-block)..."

  $gpioPortSelectionStartMarker = $markers['GPIOPortSelectionStart']
  $gpioPortSelectionEndMarker = $markers['GPIOPortSelectionEnd']

  if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioPortSelectionStartMarker -EndMarker $gpioPortSelectionEndMarker -NewContent $gpioSelectionXml) {
    if ($DryRun) {
      Write-Host "  [DRY-RUN] Would insert GPIO Port Selection ParameterRefRef choose-block (8 hardware variants)" -ForegroundColor Yellow
    }
    else {
      Write-Success "GPIO Port Selection ParameterRefRef choose-block inserted - hardware-specific UI display"
    }
  }
  else {
    Write-WarningMsg "GPIO Port Selection ParameterRefRef markers not found in template"
  }
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping GPIO Port ParameterRefRef (not Hardware.templ.xml)" -ForegroundColor DarkGray }
}

# Step 8.5.8: Update template file with Clock GPIO Selection ParameterRefRef (choose-block, SPI only)
# ONLY for Hardware.templ.xml (not for Strip.templ.xml)
if ($templatePath -like "*Hardware.templ.xml") {
  Write-Step "Updating template file with Clock GPIO Selection ParameterRefRef (SPI only)..."

  $gpioClockSelectionStartMarker = $markers['GPIOClockSelectionStart']
  $gpioClockSelectionEndMarker = $markers['GPIOClockSelectionEnd']

  if (Replace-MarkerContent -FilePath $templatePath -StartMarker $gpioClockSelectionStartMarker -EndMarker $gpioClockSelectionEndMarker -NewContent $gpioClockSelectionXml) {
    if ($DryRun) {
      Write-Host "  [DRY-RUN] Would insert Clock GPIO Selection ParameterRefRef choose-block (SPI LEDs)" -ForegroundColor Yellow
    }
    else {
      Write-Success "Clock GPIO Selection ParameterRefRef choose-block inserted - SPI LEDs only"
    }
  }
  else {
    Write-WarningMsg "Clock GPIO Selection ParameterRefRef markers not found in template"
  }
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping Clock GPIO ParameterRefRef (not Hardware.templ.xml)" -ForegroundColor DarkGray }
}

# Step 8.5.9: Update template file with Central Manual GPIO Warning
# ONLY for Hardware.templ.xml (not for Strip.templ.xml)
if ($templatePath -like "*Hardware.templ.xml") {
  Write-Step "Updating template file with Central Manual GPIO Warning..."

  $centralWarningStartMarker = $markers['CentralWarningStart']
  $centralWarningEndMarker = $markers['CentralWarningEnd']

  if (Replace-MarkerContent -FilePath $templatePath -StartMarker $centralWarningStartMarker -EndMarker $centralWarningEndMarker -NewContent $centralWarningXml) {
    if ($DryRun) {
      Write-Host "  [DRY-RUN] Would insert Central Manual GPIO Warning (expert user notice)" -ForegroundColor Yellow
    }
    else {
      Write-Success "Central Manual GPIO Warning inserted - shows when user selects 'Manuell'"
    }
  }
  else {
    Write-WarningMsg "Central Manual GPIO Warning markers not found in template"
  }
}
else {
  if ($DebugOutput) { Write-Host "  ℹ Skipping Central Manual GPIO Warning (not Hardware.templ.xml)" -ForegroundColor DarkGray }
}

# Step 8.8: Update template file with UI code for GPIO Conflict Status Display
# ONLY for Hardware.templ.xml (referenced by Strip.templ.xml via op:include)
if ($templatePath -like "*Hardware.templ.xml") {
  Write-Step "Updating Hardware template with GPIO Conflict UI (references Share.xml)..."

  # Generate conflict UI (references Share.xml parameters directly - NO separate Strip parameters!)
  if (Generate-ConflictUI -TemplatePath $templatePath) {
    Write-Success "GPIO Conflict Status UI generated (references Share %TT%0009%C% directly)"
  }
  else {
    Write-WarningMsg "GPIO Conflict Status UI markers not found in Hardware template"
  }
}
else {
  Write-Host "  ℹ Skipping conflict UI (not Hardware.templ.xml)" -ForegroundColor DarkGray
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

if ($DebugOutput) {
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

if ($hardwareOverflow.Count -gt 0) {
  Write-Host ""
  Write-Host "    Hardware Limit Exceeded" -ForegroundColor Yellow
  Write-Host "    Maximum supported: $MAX_HARDWARE_VARIANTS hardware variants" -ForegroundColor DarkYellow
  Write-Host "    Ignored Hardware (beyond limit):" -ForegroundColor Yellow
  foreach ($hw in $hardwareOverflow) {
    Write-Host "      ⊗ " -NoNewline -ForegroundColor DarkGray
    Write-Host "$($hw.DeviceName) (HW_ID: 0x$($hw.DeviceIdBit.ToString('X4')))" -ForegroundColor DarkYellow
  }
  Write-Host ""
  Write-Host "    💡 To support more hardware, adjust ID ranges in Build-HardwareConfig.ps1" -ForegroundColor Cyan
}

Write-Host ""

# ====================================================================
# Generate C++ Hardware Mapping Header
# ====================================================================
Write-Step "Generating C++ Hardware Mapping Header..."

$hardwareMappingPath = Resolve-RepoPath $HardwareMappingHeader
if (Generate-HardwareMappingHeader -HeaderPath $hardwareMappingPath -HardwareConfigs $hardwareConfigs) {
  Write-Success "Generated C++ Hardware Mapping: $(Split-Path -Leaf $hardwareMappingPath)"
  Write-Host "  • $($hardwareConfigs.Count) hardware variant(s) mapped" -ForegroundColor DarkGray
}
else {
  Write-ErrorMsg "Failed to generate C++ Hardware Mapping Header"
  Write-Host "  Expected path: $hardwareMappingPath" -ForegroundColor Yellow
  exit 1
}

Write-Host ""
Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  Multi-Hardware GPIO Template Generation Complete" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host ""

exit 0
