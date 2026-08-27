#!/usr/bin/env pwsh
<#
Open ■
┬────┴  Build-EffectParameters
■ KNX   2024 OpenKNX - Erkan Çolak

FILEPATH: scripts/Build-EffectParameters.ps1

.SYNOPSIS
    Generate ETS XML parameters from Effect header files.

.DESCRIPTION
    This script automates the generation of ETS parameter definitions from C++ Effect header files.
    It parses Effect*.h files to extract parameter metadata using introspection methods like
    getParameterName(), getParameterType(), getParameterDescription(), etc.

    The script generates:
    - ParameterTypes XML definitions for all effect parameters
    - Union Parameter definitions with dynamic IDs and offsets
    - ParameterRef entries for template expansion
    - Dynamic conditional UI (choose/when blocks) for effect-specific parameters
    - C++ mapping header for runtime parameter loading
    - Markdown help files for ETS integration

    Key Features:
    - Fully automated parameter discovery from C++ headers
    - Idempotent generation (same IDs on repeated runs)
    - Automatic validation with OpenKNXproducer
    - Rollback mechanism on validation failures
    - Support for multi-language descriptions (de/en)

.PARAMETER Language
    Language for parameter descriptions (de or en), default: de

.EXAMPLE
    .\scripts\Build-EffectParameters.ps1
    Generates parameters with German descriptions (default)

.EXAMPLE
    .\scripts\Build-EffectParameters.ps1 -Language "en"
    Generates parameters with English descriptions

.NOTES
    This script is part of the OpenKNX project automation framework.
    It integrates with OpenKNXproducer for ETS application generation.

.PARAMETER Clean
    Remove all auto-generated files and empty marker blocks in templates

.EXAMPLE
    .\scripts\Build-EffectParameters.ps1 -Clean
    Removes all generated files and empties marker blocks in templates
#>

param(
  [Parameter(Mandatory = $false)]
  [ValidateSet("de", "en")]
  [string]$Language = "de",

  [Parameter(Mandatory = $false)]
  [switch]$Clean = $false,

  [Parameter(Mandatory = $false)]
  [switch]$Force = $false,

  [Parameter(Mandatory = $false)]
  [switch]$SkipOpenKNXproducer = $false
)

$ErrorActionPreference = "Stop"
$SCRIPT_VERSION = "0.1"

# UTF-8 Encoding Configuration (Critical for Windows PowerShell 5.1)
[Console]::InputEncoding = [System.Text.Encoding]::UTF8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8
$PSDefaultParameterValues['*:Encoding'] = 'utf8'

# ====================================================================
# Configuration - Central place for all magic numbers and paths
# ====================================================================
$script:Config = @{
  # Effect Parameter IDs (FIXED - referenced in XML comments)
  EffectParameterStartId     = 600
  EffectParameterStartOffset = 30
  # Reserved area in segment union that dynamic effect parameters must not overwrite
  EffectReservedStartOffset  = 159
  EffectReservedEndOffset    = 522
  # First offset after reserved area for dynamic effect parameter spillover
  EffectSpilloverStartOffset = 523

  # Application/Module Numbers
  ApplicationNumber          = 40
  ModuleType                 = 40

  # Parameter Type Defaults
  DefaultMaxUint8            = 255
  DefaultMaxPercent          = 100

  # Relative paths from repo root
  EffectPoolPath             = "lib/OFM-NeoPixel/src/effects/EffectPool.cpp"
  EffectsDir                 = "lib/OFM-NeoPixel/src/effects"
  SrcDir                     = "src"
  HelpDir                    = "src/Baggages/Help_de"
  IncludeDir                 = "include"

  # XML Templates
  SegmentTemplate            = "src/NeoPixel.Segment.templ.xml"
  StripTemplate              = "src/NeoPixel.Strip.templ.xml"
  ShareXml                   = "src/NeoPixel.share.xml"
  EMShareXml                 = "src/NeoPixel.EM.share.xml"
  MainXml                    = "src/NeoPixel-Dev.xml"

  # Generated XML files
  ParameterTypesXml          = "src/NeoPixel.Effects.ParameterTypes.generated.xml"
  UnionParametersXml         = "src/NeoPixel.Effects.UnionParameters.generated.xml"
  ParameterRefsXml           = "src/NeoPixel.Effects.ParameterRefs.generated.xml"
  DynamicChooseXml           = "src/NeoPixel.Effects.DynamicChoose.generated.xml"

  # C++ Headers and includes
  KnxProdHeader              = "include/knxprod.h"
  CppHeader                  = "src/EffectParameterMapping.h"
  CppHeaderName              = "EffectParameterMapping.h"
  EffectBaseHeader           = "../lib/OFM-NeoPixel/src/effects/Effect.h"
  SegmentHeader              = "../lib/OFM-NeoPixel/src/Segment.h"

  # Scene Configuration
  ScenePartXml               = "src/NeoPixel.Scene.part.xml"
  SceneDataStart             = 160         # First scene at offset 160 within Union
  SceneSize                  = 36               # Bytes per scene (V3: 12 fixed + 10 param slots + 14 effectText)
  SceneEffectParamOffset     = 12  # Effect params start at byte 12 within each scene
  SceneEffectParamSize       = 10    # 10 bytes for effect-specific params per scene
  SceneCountOffset           = 159       # SceneCount param offset within Union
  SceneBaseParamId           = 8         # First free ID within scene part (after 0-7 for fixed params)
  SceneGenericSlotCount      = 10   # Number of generic slots (8 uint8 + 2 bool)
  SceneEffectTextSlotId      = 18   # PPP-ID of the scene EffectText param (PT-CueText, offset +22)
  SceneEffectTextOffset      = 22   # Byte offset of effectText within each scene
  SceneInstances             = 10          # Number of scene instances (must match instances= in Segment.templ.xml)

  # Cue Configuration (NeoPixel.Cue.part.xml)
  CuePartXml                 = "src/NeoPixel.Cue.part.xml"
  CueGenericSlotBaseId       = 1     # Cue Param0..9 IDs are %CUE_PPP+1..+10
  CueGenericSlotCount        = 10     # Number of generic cue slots (Param0..Param9)

  # File names and extensions
  EffectHeaderPattern        = "*.h"
  MarkdownPattern            = "*.md"
  EffectHeaderSuffix         = "Effect.h"
  EffectHeaderPrefix         = "Effect"

  # Effect naming patterns (for matching both conventions)
  EffectNamePattern1         = "*Effect.h"   # BPMEffect.h, CylonEffect.h
  EffectNamePattern2         = "Effect*.h"   # EffectSolid.h, EffectWipe.h

  # Special files to exclude
  ExcludeHeaders             = @("Effect.h", "EffectPool.h", "ScrollTextFonts.h")

  # File patterns for cleanup
  GeneratedXmlPattern        = "NeoPixel.Effects.*.generated.xml"
  PartXmlPattern             = "NeoPixel.Effects.*.part.xml"

  # Help file naming
  HelpFilePrefix             = "NEO-"
  HelpFileSuffix             = ".md"

  # XML Markers
  Markers                    = @{
    UnionStart                = "<!-- BEGIN AUTO-GENERATED: Effect Parameters Union -->"
    UnionEnd                  = "<!-- END AUTO-GENERATED: Effect Parameters Union -->"
    ParamRefsStart            = "<!-- BEGIN AUTO-GENERATED: Effect ParameterRefs -->"
    ParamRefsEnd              = "<!-- END AUTO-GENERATED: Effect ParameterRefs -->"
    DynamicUIStart            = "<!-- BEGIN AUTO-GENERATED: Effect Dynamic UI -->"
    DynamicUIEnd              = "<!-- END AUTO-GENERATED: Effect Dynamic UI -->"
    ModuleStart               = "<!-- BEGIN AUTO-GENERATED: NEOEFF Module -->"
    ModuleEnd                 = "<!-- END AUTO-GENERATED: NEOEFF Module -->"
    EnumStart                 = "<!-- GENERATED_EFFECT_ENUMERATIONS_START -->"
    EnumEnd                   = "<!-- GENERATED_EFFECT_ENUMERATIONS_END -->"
    EnumNo2DStart             = "<!-- GENERATED_EFFECT_ENUMERATIONS_NO2D_START -->"
    EnumNo2DEnd               = "<!-- GENERATED_EFFECT_ENUMERATIONS_NO2D_END -->"
    SceneEffectEnumStart      = "<!-- GENERATED_SCENE_EFFECT_ENUMERATIONS_START -->"
    SceneEffectEnumEnd        = "<!-- GENERATED_SCENE_EFFECT_ENUMERATIONS_END -->"
    SceneEffectEnumNo2DStart  = "<!-- GENERATED_SCENE_EFFECT_ENUMERATIONS_NO2D_START -->"
    SceneEffectEnumNo2DEnd    = "<!-- GENERATED_SCENE_EFFECT_ENUMERATIONS_NO2D_END -->"
    EffectPTStart             = "<!-- BEGIN AUTO-GENERATED: Effect ParameterTypes -->"
    EffectPTEnd               = "<!-- END AUTO-GENERATED: Effect ParameterTypes -->"
    # Scene markers (in NeoPixel.Scene.part.xml)
    SceneEffectParamsStart    = "<!-- BEGIN AUTO-GENERATED: Scene Effect Parameters -->"
    SceneEffectParamsEnd      = "<!-- END AUTO-GENERATED: Scene Effect Parameters -->"
    SceneEffectRefsStart      = "<!-- BEGIN AUTO-GENERATED: Scene Effect ParameterRefs -->"
    SceneEffectRefsEnd        = "<!-- END AUTO-GENERATED: Scene Effect ParameterRefs -->"
    SceneEffectDynamicStart   = "<!-- BEGIN AUTO-GENERATED: Scene Effect Dynamic UI -->"
    SceneEffectDynamicEnd     = "<!-- END AUTO-GENERATED: Scene Effect Dynamic UI -->"
    SceneEffectPCParamsStart  = "<!-- BEGIN AUTO-GENERATED: Scene Effect PC RParameters -->"
    SceneEffectPCParamsEnd    = "<!-- END AUTO-GENERATED: Scene Effect PC RParameters -->"
    # Cue markers (in NeoPixel.Cue.part.xml)
    CueEffectRefsStart        = "<!-- BEGIN AUTO-GENERATED: Cue Effect ParameterRefs -->"
    CueEffectRefsEnd          = "<!-- END AUTO-GENERATED: Cue Effect ParameterRefs -->"
    CueEffectDynamicStart     = "<!-- BEGIN AUTO-GENERATED: Cue Effect Dynamic UI -->"
    CueEffectDynamicEnd       = "<!-- END AUTO-GENERATED: Cue Effect Dynamic UI -->"
    CueEffectTypedParamsStart = "<!-- BEGIN AUTO-GENERATED: Cue Effect Typed Parameters -->"
    CueEffectTypedParamsEnd   = "<!-- END AUTO-GENERATED: Cue Effect Typed Parameters -->"
    # Cue Effect enum markers (in NeoPixel.EM.share.xml)
    CueEffectEnumStart        = "<!-- GENERATED_CUE_EFFECT_ENUMERATIONS_START -->"
    CueEffectEnumEnd          = "<!-- GENERATED_CUE_EFFECT_ENUMERATIONS_END -->"
    CueEffectEnumNo2DStart    = "<!-- GENERATED_CUE_EFFECT_ENUMERATIONS_NO2D_START -->"
    CueEffectEnumNo2DEnd      = "<!-- GENERATED_CUE_EFFECT_ENUMERATIONS_NO2D_END -->"
  }

  # String cleaning/replacement patterns
  CleanPatterns              = @{
    # Remove "Effect" suffix
    RemoveEffectSuffix  = 'Effect$'
    # Remove all non-alphanumeric characters
    AlphanumericOnly    = '[^a-zA-Z0-9]'
    # Remove spaces and hyphens
    RemoveSpacesHyphens = '[-\s]'
    # Replace spaces with hyphens
    SpaceToHyphen       = '\s+'
  }

  # Regex Patterns for C++ Parsing
  Patterns                   = @{
    # Effect class name - supports both naming conventions: *Effect and Effect*
    ClassName          = 'class\s+((?:\w+Effect)|(?:Effect\w+))\s*:'

    # getName() with EFFECT_NAME_DE_EN macro
    GetNameMacro       = 'getName\s*\([^)]*\)\s*override[\s\S]*?return\s+EFFECT_NAME_DE_EN\s*\(\s*"([^"]+)"\s*,\s*"([^"]+)"\s*\)\s*;'

    # getName() with plain string - supports both inline {return "X";} and multi-line return
    GetNamePlain       = 'getName\s*\([^)]*\)\s*override\s*\{?\s*return\s*"([^"]+)"\s*;'

    # getParameterCount() return value
    ParameterCount     = 'getParameterCount\s*\(\s*\)\s*(?:const\s+)?(?:override\s+)?{[^}]*return\s+(\d+)'

    # getParameterName case statement
    ParameterName      = '(?s)getParameterName.*?case\s+{0}\s*:\s*return\s+"([^"]+)"'

    # EffectPool.cpp getEffectByIndex() function
    EffectPoolFunction = 'Effect\*\s+EffectPool::getEffectByIndex\([^)]+\)\s*\{(.*?)\n}'

    # return getXXX() in EffectPool
    EffectPoolReturn   = 'return\s+get(\w+)\(\)'

    # Filename pattern: EffectXXX → XXXEffect
    FilenameToClass    = '^Effect(.+)$'

    # Parameter type matching
    ParamTypeUint8     = 'PARAM_UINT8|PARAM_HUE'

    # Multi-language separator (de | en)
    MultiLangSeparator = '(.+?)\s*\|\s*(.+)'
  }
}

# Helper: Verbose logging
function Write-ScriptVerbose {
  param([string]$Message, [string]$Color = "Gray")
  if ($VerbosePreference -eq 'Continue') {
    Write-Host "  [VERBOSE] $Message" -ForegroundColor $Color
  }
}

# PS 5.1 compatible clear screen
# Helper: Resolve repository root
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
  Write-ScriptVerbose "Repository root: $repoRoot"
  return $repoRoot
}

# Helper: Resolve path relative to repo root
function Resolve-RepoPath {
  param([string]$RelativePath)
  $root = Get-RepoRoot
  $fullPath = Join-Path $root $RelativePath
  Write-ScriptVerbose "Resolved: $RelativePath → $fullPath"
  return $fullPath
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
# Helper Functions - Must be defined BEFORE Clean Mode
# ====================================================================

function Get-AllEffectHeaders {
  $effectsDir = Resolve-RepoPath $script:Config.EffectsDir

  if (-not (Test-Path -LiteralPath $effectsDir)) {
    throw "Effects directory not found: $effectsDir"
  }

  Write-ScriptVerbose "Scanning for effect headers in: $effectsDir"

  # Get all effect headers but exclude the base Effect.h class and EffectPool.h
  # Support both naming patterns:
  #   - Pattern 1: Effect*.h (EffectSolid.h, EffectWipe.h)
  #   - Pattern 2: *Effect.h (BPMEffect.h, CylonEffect.h)
  $allHeaders = Get-ChildItem -Path $effectsDir -Filter $script:Config.EffectHeaderPattern
  $effectHeaders = $allHeaders | Where-Object {
    ($_.Name -like $script:Config.EffectNamePattern1 -or $_.Name -like $script:Config.EffectNamePattern2) -and
    $_.Name -notin $script:Config.ExcludeHeaders
  }

  Write-ScriptVerbose "Found $($effectHeaders.Count) effect headers"

  return $effectHeaders | Sort-Object Name
}

# ====================================================================
# Clean Mode - Remove generated files
# ====================================================================

if ($Clean) {
  Clear-Host

  Show-OpenKNXLogo -SubTitle "EffectParameters Build Script by Erkan Çolak" -Version $SCRIPT_VERSION

  Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
  Write-Host "  Cleaning Auto-Generated Effect Parameter Files" -ForegroundColor Cyan
  Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
  Write-Host ""

  # Use centralized path resolution
  $srcDir = Resolve-RepoPath $script:Config.SrcDir

  # Collect files to delete
  $filesToDelete = @()

  # 1. Collect generated XML fragments
  $xmlFiles = Get-ChildItem -Path $srcDir -Filter $script:Config.GeneratedXmlPattern -ErrorAction SilentlyContinue
  foreach ($file in $xmlFiles) {
    $filesToDelete += [PSCustomObject]@{ Type = "XML Fragment"; Name = $file.Name; Path = $file.FullName }
  }

  # 2. Collect C++ parameter mapping header
  $cppFile = Join-Path $srcDir $script:Config.CppHeaderName
  if (Test-Path $cppFile) {
    $filesToDelete += [PSCustomObject]@{ Type = "C++ Header"; Name = $script:Config.CppHeaderName; Path = $cppFile }
  }

  # 3. Collect old .part.xml files (legacy)
  $partFiles = Get-ChildItem -Path $srcDir -Filter $script:Config.PartXmlPattern -ErrorAction SilentlyContinue
  foreach ($file in $partFiles) {
    $filesToDelete += [PSCustomObject]@{ Type = "Legacy XML"; Name = $file.Name; Path = $file.FullName }
  }

  # 4. Collect auto-generated help files using SAME logic as Generate-HelpFiles
  $helpDir = Resolve-RepoPath $script:Config.HelpDir
  $effectsDir = Resolve-RepoPath $script:Config.EffectsDir

  Write-Host "  Scanning effects for help files..." -ForegroundColor DarkGray
  Write-Host ""

  if ((Test-Path $helpDir) -and (Test-Path $effectsDir)) {
    # Use centralized function to get all effect headers
    $effectHeaders = Get-AllEffectHeaders

    # DEBUG: Uncomment to see detailed scanning process
    # Write-Host "  DEBUG: Found $($effectHeaders.Count) effect headers in $effectsDir" -ForegroundColor Yellow
    # Write-Host "  DEBUG: All headers: $($allHeaders.Count)" -ForegroundColor Yellow
    # Write-Host ""

    foreach ($header in $effectHeaders) {
      $headerContent = Get-Content $header.FullName -Raw -Encoding UTF8

      # Extract effect display name - support both EFFECT_NAME_DE_EN macro and plain string
      $effectDisplayName = ""

      # Try macro first (returns German name)
      if ($headerContent -match $script:Config.Patterns.GetNameMacro) {
        $effectDisplayName = $matches[1]  # Use German name (matches XML display)
      }
      # Fallback: plain string
      elseif ($headerContent -match $script:Config.Patterns.GetNamePlain) {
        $effectDisplayName = $matches[1]
      }
      # Last resort: derive from class name
      elseif ($headerContent -match $script:Config.Patterns.ClassName) {
        $effectDisplayName = $matches[1] -replace $script:Config.CleanPatterns.RemoveEffectSuffix, ''
      }

      # Get parameter count
      $paramCount = 0
      if ($headerContent -match $script:Config.Patterns.ParameterCount) {
        $paramCount = [int]$matches[1]
      }

      # DEBUG: Uncomment to see effect-by-effect processing
      # Write-Host "  Effect: " -NoNewline -ForegroundColor Cyan
      # Write-Host "$($header.Name)" -NoNewline -ForegroundColor White
      # Write-Host " → DisplayName: " -NoNewline -ForegroundColor DarkGray
      # Write-Host "$effectDisplayName" -NoNewline -ForegroundColor Green
      # Write-Host " → Params: " -NoNewline -ForegroundColor DarkGray
      # Write-Host "$paramCount" -ForegroundColor Green

      # For each parameter, generate help filename using SAME logic as Generate-HelpFiles
      if ($paramCount -gt 0 -and $effectDisplayName) {
        # IMPORTANT: Must match Generate-HelpFiles logic: spaces → hyphens
        $effectNameClean = $effectDisplayName -replace $script:Config.CleanPatterns.SpaceToHyphen, '-'

        for ($i = 0; $i -lt $paramCount; $i++) {
          # Extract parameter name using dotall regex
          $pattern = $script:Config.Patterns.ParameterName -f $i
          if ($headerContent -match $pattern) {
            $paramName = $matches[1]
            $paramNameClean = $paramName -replace $script:Config.CleanPatterns.SpaceToHyphen, '-'

            # SAME logic as Generate-HelpFiles: NEO-{ParamName}-{EffectName}.md
            $helpFileName = "$($script:Config.HelpFilePrefix)$paramNameClean-$effectNameClean$($script:Config.HelpFileSuffix)"
            $helpFilePath = Join-Path $helpDir $helpFileName

            # DEBUG: Uncomment to see individual help file checks
            # Write-Host "    → " -NoNewline -ForegroundColor DarkGray
            # Write-Host "$helpFileName" -NoNewline -ForegroundColor White

            # Add to delete list if file exists
            if (Test-Path $helpFilePath) {
              # Write-Host " ✓ FOUND" -ForegroundColor Green
              if (-not ($filesToDelete | Where-Object { $_.Path -eq $helpFilePath })) {
                $filesToDelete += [PSCustomObject]@{ Type = "Help File"; Name = $helpFileName; Path = $helpFilePath }
              }
            } # else {
            # Write-Host " ✗ NOT FOUND" -ForegroundColor Red
            # }
          } # else {
          # Write-Host "    → Param ${i}: " -NoNewline -ForegroundColor DarkGray
          # Write-Host "REGEX FAILED" -ForegroundColor Red
          # }
        }
      }
      # Write-Host ""
    }
  }
  else {
    Write-Host "  ERROR: Paths not found!" -ForegroundColor Red
    Write-Host "  helpDir: $helpDir → Exists: $(Test-Path $helpDir)" -ForegroundColor Yellow
    Write-Host "  effectsDir: $effectsDir → Exists: $(Test-Path $effectsDir)" -ForegroundColor Yellow
  }

  # 5. Collect auto-generated scene help files (NEO-*-Szene-*.md and NEO-Anzahl-Szenen.md)
  if (Test-Path $helpDir) {
    $sceneHelpFiles = Get-ChildItem -Path $helpDir -Filter "NEO-*-Szene-*.md" -ErrorAction SilentlyContinue
    foreach ($file in $sceneHelpFiles) {
      if (-not ($filesToDelete | Where-Object { $_.Path -eq $file.FullName })) {
        $filesToDelete += [PSCustomObject]@{ Type = "Scene Help File"; Name = $file.Name; Path = $file.FullName }
      }
    }
    $sceneCountHelp = Join-Path $helpDir "NEO-Anzahl-Szenen.md"
    if (Test-Path $sceneCountHelp) {
      $filesToDelete += [PSCustomObject]@{ Type = "Scene Help File"; Name = "NEO-Anzahl-Szenen.md"; Path = $sceneCountHelp }
    }
  }

  Write-Host "  ────────────────────────────────────────────────────────────────────────────────────────────────────────────────────" -ForegroundColor DarkGray
  Write-Host ""

  # Show what will be deleted
  Write-Host "  Files to be deleted:" -ForegroundColor Yellow
  Write-Host "  " -NoNewline
  Write-Host ("─" * 116) -ForegroundColor DarkGray

  $groupedFiles = $filesToDelete | Group-Object -Property Type
  foreach ($group in $groupedFiles) {
    Write-Host "  " -NoNewline
    Write-Host "$($group.Name) " -NoNewline -ForegroundColor Cyan
    Write-Host "($($group.Count))" -ForegroundColor Green
    foreach ($file in $group.Group) {
      Write-Host "    • $($file.Name)" -ForegroundColor DarkGray
    }
  }

  Write-Host "  " -NoNewline
  Write-Host "Template Changes" -ForegroundColor Cyan
  Write-Host "    • Empty Effect Parameters Union block" -ForegroundColor DarkGray
  Write-Host "    • Empty Effect ParameterRefs block" -ForegroundColor DarkGray
  Write-Host "    • Empty Effect Dynamic UI block" -ForegroundColor DarkGray
  Write-Host "    • Empty Scene Effect Parameters/ParameterRefs/Dynamic UI in part.xml" -ForegroundColor DarkGray
  Write-Host "    • Empty Effect Type Enumeration in share.xml" -ForegroundColor DarkGray
  Write-Host "    • Empty Scene Effect Enumeration in share.xml" -ForegroundColor DarkGray
  Write-Host "    • Empty Cue Effect Enumeration in EM.share.xml" -ForegroundColor DarkGray
  Write-Host "    • Empty Scene Effect Defaults in script.js" -ForegroundColor DarkGray

  Write-Host "  " -NoNewline
  Write-Host ("─" * 116) -ForegroundColor DarkGray
  Write-Host "  Total: " -NoNewline -ForegroundColor White
  Write-Host "$($filesToDelete.Count) file(s) + template markers" -ForegroundColor Green
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
  Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
  Write-Host "  Deleting Files" -ForegroundColor Cyan
  Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
  Write-Host ""

  # Delete files
  $deletedCount = 0
  foreach ($file in $filesToDelete) {
    try {
      Remove-Item $file.Path -Force
      Write-Host "  ✓ " -NoNewline -ForegroundColor Green
      Write-Host $file.Name -ForegroundColor DarkGray
      $deletedCount++
    }
    catch {
      Write-Host "  ✗ " -NoNewline -ForegroundColor Red
      Write-Host "$($file.Name) - Error: $_" -ForegroundColor Red
    }
  }

  # Empty marker blocks in template
  Write-Host ""
  Write-Host "  • Emptying marker blocks in templates..." -ForegroundColor Cyan

  # 1. Empty NeoPixel.Segment.templ.xml markers
  $templateFile = Resolve-RepoPath $script:Config.SegmentTemplate
  if (Test-Path $templateFile) {
    $content = Get-Content $templateFile -Raw -Encoding UTF8

    # Empty Union Parameters block - keep important comments
    $pattern1 = "(?s)($([regex]::Escape($script:Config.Markers.UnionStart))\r?\n)(.*?)($([regex]::Escape($script:Config.Markers.UnionEnd)))"
    $replacement1 = '$1                <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->' + [Environment]::NewLine +
    '                <!-- Start ID: 600, Start Offset: 30 (FIXED - do not change!) -->' + [Environment]::NewLine +
    '        $3'
    $content = $content -replace $pattern1, $replacement1

    # Empty ParameterRefs block - keep important comments
    $pattern2 = "(?s)($([regex]::Escape($script:Config.Markers.ParamRefsStart))\r?\n)(.*?)($([regex]::Escape($script:Config.Markers.ParamRefsEnd)))"
    $replacement2 = '$1                <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->' + [Environment]::NewLine +
    '              $3'
    $content = $content -replace $pattern2, $replacement2

    # Empty Dynamic UI block - keep important comments
    $pattern3 = "(?s)($([regex]::Escape($script:Config.Markers.DynamicUIStart))\r?\n)(.*?)($([regex]::Escape($script:Config.Markers.DynamicUIEnd)))"
    $replacement3 = '$1                      <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->' + [Environment]::NewLine +
    '                      <!-- Dynamic choose/when blocks for effect-specific parameters -->' + [Environment]::NewLine +
    '              $3'
    $content = $content -replace $pattern3, $replacement3

    Set-Content -Path $templateFile -Value $content -NoNewline -Encoding UTF8
    Write-Host "    ✓ NeoPixel.Segment.templ.xml - marker blocks emptied" -ForegroundColor Green
  }

  # 2. Empty NEOEFF Module in NeoPixel.xml
  $neoPixelXml = Resolve-RepoPath $script:Config.MainXml
  if (Test-Path $neoPixelXml) {
    $content = Get-Content $neoPixelXml -Raw -Encoding UTF8

    # Empty NEOEFF Module block - keep important comment
    $pattern4 = "(?s)($([regex]::Escape($script:Config.Markers.ModuleStart))\r?\n)(.*?)($([regex]::Escape($script:Config.Markers.ModuleEnd)))"
    $replacement4 = '$1  <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->' + [Environment]::NewLine +
    '  $3'
    $content = $content -replace $pattern4, $replacement4

    Set-Content -Path $neoPixelXml -Value $content -NoNewline -Encoding UTF8
    Write-Host "    ✓ NeoPixel.xml - NEOEFF Module removed" -ForegroundColor Green
  }

  # 3. Empty Effect Type Enumeration in NeoPixel.share.xml
  $shareXmlPath = Resolve-RepoPath $script:Config.ShareXml
  if (Test-Path $shareXmlPath) {
    $content = Get-Content $shareXmlPath -Raw -Encoding UTF8

    # Empty Effect Enumeration block - keep markers
    $pattern5 = "(?s)($([regex]::Escape($script:Config.Markers.EnumStart))\r?\n)(.*?)($([regex]::Escape($script:Config.Markers.EnumEnd)))"
    $replacement5 = '$1                <!-- Content will be generated by Build-EffectParameters.ps1 -->' + [Environment]::NewLine +
    '                $3'
    $content = $content -replace $pattern5, $replacement5

    Set-Content -Path $shareXmlPath -Value $content -NoNewline -Encoding UTF8
    Write-Host "    ✓ NeoPixel.share.xml - Effect Type Enumeration emptied" -ForegroundColor Green

    # Reduced lists for hardware without the 2D effects - same markers, same treatment.
    $content = Get-Content $shareXmlPath -Raw -Encoding UTF8
    foreach ($pair in @(
        @($script:Config.Markers.EnumNo2DStart, $script:Config.Markers.EnumNo2DEnd),
        @($script:Config.Markers.SceneEffectEnumNo2DStart, $script:Config.Markers.SceneEffectEnumNo2DEnd))) {
      $pat = "(?s)($([regex]::Escape($pair[0]))\r?\n)(.*?)($([regex]::Escape($pair[1])))"
      $rep = '$1                <!-- Content will be generated by Build-EffectParameters.ps1 -->' + [Environment]::NewLine + '                $3'
      $content = $content -replace $pat, $rep
    }
    Set-Content -Path $shareXmlPath -Value $content -NoNewline -Encoding UTF8
    Write-Host "    [OK] NeoPixel.share.xml - no-2D enumerations emptied" -ForegroundColor Green

    # Also empty Effect ParameterTypes block
    $content = Get-Content $shareXmlPath -Raw -Encoding UTF8
    $pattern6 = "(?s)($([regex]::Escape($script:Config.Markers.EffectPTStart))\r?\n)(.*?)($([regex]::Escape($script:Config.Markers.EffectPTEnd)))"
    $replacement6 = '$1              $3'
    $content = $content -replace $pattern6, $replacement6
    Set-Content -Path $shareXmlPath -Value $content -NoNewline -Encoding UTF8
    Write-Host "    ✓ NeoPixel.share.xml - Effect ParameterTypes emptied" -ForegroundColor Green

    # Also empty Scene Effect Enumeration block (static EM entries 200..215 stay outside the markers)
    $content = Get-Content $shareXmlPath -Raw -Encoding UTF8
    $pattern7 = "(?s)($([regex]::Escape($script:Config.Markers.SceneEffectEnumStart))\r?\n)(.*?)($([regex]::Escape($script:Config.Markers.SceneEffectEnumEnd)))"
    $replacement7 = '$1                <!-- Content will be generated by Build-EffectParameters.ps1 -->' + [Environment]::NewLine +
    '                $3'
    $content = $content -replace $pattern7, $replacement7
    Set-Content -Path $shareXmlPath -Value $content -NoNewline -Encoding UTF8
    Write-Host "    ✓ NeoPixel.share.xml - Scene Effect Enumeration emptied" -ForegroundColor Green
  }

  # 4. Empty EffectTypeMapping.h
  $effectTypeMappingPath = "src/EffectTypeMapping.h"
  if (Test-Path $effectTypeMappingPath) {
    $placeholderContent = @"
/**
 * @file EffectTypeMapping.h
 * @brief Effect Type ID to EffectPool mapping (auto-generated)
 *
 * This file is AUTO-GENERATED by scripts/Build-EffectParameters.ps1
 * DO NOT EDIT MANUALLY!
 *
 * Run the following command to regenerate:
 *   pwsh scripts/Build-EffectParameters.ps1
 */

#pragma once

#error "EffectTypeMapping.h is empty! Run 'pwsh scripts/Build-EffectParameters.ps1' to regenerate."

// This file will be generated by Build-EffectParameters.ps1
// Run the script to generate effect type mapping from EffectPool.cpp
"@
    Set-Content -Path $effectTypeMappingPath -Value $placeholderContent -NoNewline -Encoding UTF8
    Write-Host "    ✓ EffectTypeMapping.h - Emptied with placeholder" -ForegroundColor Green
  }

  # 5. Empty EffectParameterMapping.h
  $effectParamMappingPath = "src/EffectParameterMapping.h"
  if (Test-Path $effectParamMappingPath) {
    $placeholderContent = @"
/**
 * @file EffectParameterMapping.h
 * @brief Effect parameter loading logic (auto-generated)
 *
 * This file is AUTO-GENERATED by scripts/Build-EffectParameters.ps1
 * DO NOT EDIT MANUALLY!
 *
 * Run the following command to regenerate:
 *   pwsh scripts/Build-EffectParameters.ps1
 */

#pragma once

#error "EffectParameterMapping.h is empty! Run 'pwsh scripts/Build-EffectParameters.ps1' to regenerate."

// This file will be generated by Build-EffectParameters.ps1
// Run the script to generate effect parameter loading logic
"@
    Set-Content -Path $effectParamMappingPath -Value $placeholderContent -NoNewline -Encoding UTF8
    Write-Host "    ✓ EffectParameterMapping.h - Emptied with placeholder" -ForegroundColor Green
  }

  # 6. Empty Scene part.xml marker blocks
  $scenePartPath = Resolve-RepoPath $script:Config.ScenePartXml
  if (Test-Path $scenePartPath) {
    $content = Get-Content $scenePartPath -Raw -Encoding UTF8
    $markers = $script:Config.Markers

    $pattern = "(?s)($([regex]::Escape($markers.SceneEffectParamsStart))\r?\n)(.*?)($([regex]::Escape($markers.SceneEffectParamsEnd)))"
    $content = $content -replace $pattern, ('$1' + [Environment]::NewLine + '                            $3')

    $pattern = "(?s)($([regex]::Escape($markers.SceneEffectRefsStart))\r?\n)(.*?)($([regex]::Escape($markers.SceneEffectRefsEnd)))"
    $content = $content -replace $pattern, ('$1' + [Environment]::NewLine + '                            $3')

    $pattern = "(?s)($([regex]::Escape($markers.SceneEffectDynamicStart))\r?\n)(.*?)($([regex]::Escape($markers.SceneEffectDynamicEnd)))"
    $content = $content -replace $pattern, ('$1' + [Environment]::NewLine + '                                $3')

    $pattern = "(?s)($([regex]::Escape($markers.SceneEffectPCParamsStart))\r?\n)(.*?)($([regex]::Escape($markers.SceneEffectPCParamsEnd)))"
    $content = $content -replace $pattern, ('$1' + [Environment]::NewLine + '                                    $3')

    Set-Content -Path $scenePartPath -Value $content -NoNewline -Encoding UTF8
    Write-Host "    ✓ NeoPixel.Scene.part.xml - Scene effect marker blocks emptied" -ForegroundColor Green
  }

  # 7. Empty Scene Effect Defaults in NeoPixel.script.js
  $scriptJsPath = Resolve-RepoPath "src/NeoPixel.script.js"
  if (Test-Path $scriptJsPath) {
    $content = Get-Content $scriptJsPath -Raw -Encoding UTF8

    $startMarker = "// BEGIN AUTO-GENERATED: Scene Effect Defaults"
    $endMarker = "// END AUTO-GENERATED: Scene Effect Defaults"

    # Match the exact marker line so clean regenerations do not preserve stray blank lines.
    $pattern = "(?s)($([regex]::Escape($startMarker))\r?\n)(.*?)($([regex]::Escape($endMarker)))"
    $replacement = ('$1' + [Environment]::NewLine + 'var NEO_SceneEffectDefaults = {};' + [Environment]::NewLine + '$3')
    $content = $content -replace $pattern, $replacement

    $funcStartMarker = "// BEGIN AUTO-GENERATED: Scene Effect Defaults Function"
    $funcEndMarker = "// END AUTO-GENERATED: Scene Effect Defaults Function"

    $pattern = "(?s)($([regex]::Escape($funcStartMarker))\r?\n)(.*?)($([regex]::Escape($funcEndMarker)))"
    $replacement = '$1' + 'function NEO_SetSceneEffectDefaults(input, output, context) { }' + [Environment]::NewLine + '$3'
    $content = $content -replace $pattern, $replacement

    Set-Content -Path $scriptJsPath -Value $content -NoNewline -Encoding UTF8
    Write-Host "    ✓ NeoPixel.script.js - Scene Effect Defaults emptied" -ForegroundColor Green
  }

  # 8. Empty Cue part.xml marker blocks
  $cuePartPath = Resolve-RepoPath $script:Config.CuePartXml
  if (Test-Path $cuePartPath) {
    $content = Get-Content $cuePartPath -Raw -Encoding UTF8
    $markers = $script:Config.Markers

    $pattern = "(?s)($([regex]::Escape($markers.CueEffectRefsStart))\r?\n)(.*?)($([regex]::Escape($markers.CueEffectRefsEnd)))"
    $content = $content -replace $pattern, ('$1' + [Environment]::NewLine + '              $3')

    $pattern = "(?s)($([regex]::Escape($markers.CueEffectDynamicStart))\r?\n)(.*?)($([regex]::Escape($markers.CueEffectDynamicEnd)))"
    $content = $content -replace $pattern, ('$1' + [Environment]::NewLine + '                    $3')

    $pattern = "(?s)($([regex]::Escape($markers.CueEffectTypedParamsStart))\r?\n)(.*?)($([regex]::Escape($markers.CueEffectTypedParamsEnd)))"
    $content = $content -replace $pattern, ('$1' + [Environment]::NewLine + '              $3')

    Set-Content -Path $cuePartPath -Value $content -NoNewline -Encoding UTF8
    Write-Host "    ✓ NeoPixel.Cue.part.xml - Cue effect marker blocks emptied" -ForegroundColor Green
  }

  # 9. Empty Cue Effect Enumeration in NeoPixel.EM.share.xml
  $emSharePath = Resolve-RepoPath $script:Config.EMShareXml
  if (Test-Path $emSharePath) {
    $content = Get-Content $emSharePath -Raw -Encoding UTF8
    $markers = $script:Config.Markers

    $pattern = "(?s)($([regex]::Escape($markers.CueEffectEnumStart))\r?\n)(.*?)($([regex]::Escape($markers.CueEffectEnumEnd)))"
    $replacement = '$1                <!-- Content will be generated by Build-EffectParameters.ps1 -->' + [Environment]::NewLine +
    '                $3'
    $content = $content -replace $pattern, $replacement

    Set-Content -Path $emSharePath -Value $content -NoNewline -Encoding UTF8
    Write-Host "    ✓ NeoPixel.EM.share.xml - Cue Effect Enumeration emptied" -ForegroundColor Green

    # Reduced cue list for hardware without the 2D effects.
    $content = Get-Content $emSharePath -Raw -Encoding UTF8
    $patNo2D = "(?s)($([regex]::Escape($script:Config.Markers.CueEffectEnumNo2DStart))\r?\n)(.*?)($([regex]::Escape($script:Config.Markers.CueEffectEnumNo2DEnd)))"
    $repNo2D = '$1                <!-- Content will be generated by Build-EffectParameters.ps1 -->' + [Environment]::NewLine + '                $3'
    $content = $content -replace $patNo2D, $repNo2D
    Set-Content -Path $emSharePath -Value $content -NoNewline -Encoding UTF8
    Write-Host "    [OK] NeoPixel.EM.share.xml - no-2D cue enumeration emptied" -ForegroundColor Green

  # 10. Empty the generated blocks outside the ETS XML: the effect table in the help file
  #     and the flag list in platformio.custom.ini. Both are marker regions like the ones
  #     above, so a clean has to leave them empty too.
  foreach ($target in @(
      @{ Path = "src/Baggages/Help_de/NEO-Effekt-Typ.md"; Start = "<!-- BEGIN AUTO-GENERATED: Effect Table - DO NOT EDIT MANUALLY -->"; End = "<!-- END AUTO-GENERATED: Effect Table -->"; Note = "<!-- Content will be generated by Build-EffectParameters.ps1 -->" },
      @{ Path = "platformio.custom.ini"; Start = "; BEGIN AUTO-GENERATED: Effect Flags - DO NOT EDIT MANUALLY"; End = "; END AUTO-GENERATED: Effect Flags"; Note = "; Content will be generated by Build-EffectParameters.ps1" })) {
    $full = Resolve-RepoPath $target.Path
    if (-not (Test-Path $full)) { continue }
    $content = Get-Content $full -Raw -Encoding UTF8
    $pat = "(?s)($([regex]::Escape($target.Start))\r?\n)(.*?)($([regex]::Escape($target.End)))"
    $updated = $content -replace $pat, ('$1' + $target.Note + [Environment]::NewLine + '$3')
    if ($updated -ne $content) {
      Set-Content -Path $full -Value $updated -NoNewline -Encoding UTF8
      Write-Host ("    [OK] {0} - generated block emptied" -f (Split-Path $target.Path -Leaf)) -ForegroundColor Green
    }
  }
  }

  # Summary
  Write-Host ""
  Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
  Write-Host "  Cleanup Complete" -ForegroundColor Cyan
  Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
  Write-Host ""
  Write-Host "  Deleted files:      " -NoNewline -ForegroundColor White
  Write-Host "$deletedCount" -ForegroundColor Green
  Write-Host "  Template markers:   " -NoNewline -ForegroundColor White
  Write-Host "emptied" -ForegroundColor Green
  Write-Host ""
  Write-Host "    Run without -Clean to regenerate everything" -ForegroundColor Cyan

  Write-Host ""
  Show-OpenKNXLogo -SubTitle "EffectParameters Build Script by Erkan Çolak" -Version $SCRIPT_VERSION
  exit 0
}

# ====================================================================
# Helper Functions
# ====================================================================

function Extract-SwitchCase {
  param(
    [string]$Content,
    [string]$FunctionName,
    [int]$CaseIndex
  )

  # Regex: Find "case $CaseIndex: return "VALUE";" in getFunction() switch statement
  $pattern = "get$FunctionName\s*\([^)]*\)[^{]*\{[^}]*?case\s+$CaseIndex\s*:\s*return\s+[`"']([^`"']+)[`"']"
  if ($Content -match $pattern) {
    return $Matches[1]
  }
  return $null
}

function Extract-SwitchCaseMultiLang {
  param(
    [string]$Content,
    [string]$FunctionName,
    [int]$CaseIndex,
    [string]$Lang
  )

  # Pattern 1: PARAM_DESC_DE_EN("de text", "en text") macro
  $pattern1 = "(?s)get$FunctionName\s*\([^)]*\)[^{]*\{.*?case\s+$CaseIndex\s*:\s*return\s+PARAM_DESC_DE_EN\s*\(\s*[`"']([^`"']+)[`"']\s*,\s*[`"']([^`"']+)[`"']\s*\)"

  if ($Content -match $pattern1) {
    $deText = $Matches[1]
    $enText = $Matches[2]
    return $(if ($Lang -eq "en") { $enText } else { $deText })
  }

  # Pattern 2: Simple string with pipe separator "de text | en text"
  $pattern2 = "(?s)get$FunctionName\s*\([^)]*\)[^{]*\{.*?case\s+$CaseIndex\s*:\s*return\s+[`"']([^`"']+)[`"']"

  if ($Content -match $pattern2) {
    $fullText = $Matches[1]

    # Check if it contains a pipe separator for multi-language
    if ($fullText -match $script:Config.Patterns.MultiLangSeparator) {
      $deText = $Matches[1].Trim()
      $enText = $Matches[2].Trim()
      return $(if ($Lang -eq "en") { $enText } else { $deText })
    }

    # Single language string - return as is
    return $fullText
  }

  # Fallback: Simple string without multi-lang
  return Extract-SwitchCase -Content $Content -FunctionName $FunctionName -CaseIndex $CaseIndex
}

function Extract-IntValue {
  param(
    [string]$Content,
    [string]$FunctionName,
    [int]$CaseIndex
  )

  # Updated regex - handle multi-line switch statements
  $pattern = "get$FunctionName\s*\([^)]*\)[^{]*\{(?s).*?case\s+$CaseIndex\s*:\s*return\s+(\d+)"
  if ($Content -match $pattern) {
    return [int]$Matches[1]
  }
  return 0
}

function Extract-ParameterType {
  param(
    [string]$Content,
    [int]$CaseIndex
  )

  # Find ParameterType::PARAM_XXX in getParameterType() case - handle multi-line
  $pattern = "getParameterType\s*\([^)]*\)[^{]*\{(?s).*?case\s+$CaseIndex\s*:\s*return\s+ParameterType::(\w+)"
  if ($Content -match $pattern) {
    return $Matches[1]
  }
  return "PARAM_UINT8"  # Default
}

function Extract-EnumOptions {
  param(
    [string]$Content,
    [int]$ParamIndex
  )

  # Find getEnumValueName() function and extract enum options for the specific parameter
  # Example patterns:
  #   if (paramIndex != 1) return nullptr;
  #   switch (enumValue) { case X: return "Name"; ... }
    
  # First check if getEnumValueName exists at all
  if ($Content -notmatch 'getEnumValueName') {
    return @()
  }
    
  # Extract the entire function body via brace counting (regex with lazy
  # matching stops at the first inner closing brace and would truncate
  # functions that contain multiple per-parameter blocks).
  $sigIndex = $Content.IndexOf('getEnumValueName')
  $openIndex = $Content.IndexOf('{', $sigIndex)
  if ($openIndex -lt 0) {
    return @()
  }
  $depth = 0
  $closeIndex = -1
  for ($ci = $openIndex; $ci -lt $Content.Length; $ci++) {
    $chc = $Content[$ci]
    if ($chc -eq '{') { $depth++ }
    elseif ($chc -eq '}') {
      $depth--
      if ($depth -eq 0) { $closeIndex = $ci; break }
    }
  }
  if ($closeIndex -lt 0) {
    return @()
  }

  $functionBody = $Content.Substring($openIndex + 1, $closeIndex - $openIndex - 1)
    
  # Check if this specific parameter index is handled
  # Look for patterns like: "if (paramIndex != 1)" or "if (paramIndex == 1)"
  $isHandled = $false
  if ($functionBody -match "paramIndex\s*!=\s*$ParamIndex.*?return\s+nullptr") {
    # if (paramIndex != X) return nullptr; -> X IS handled
    $isHandled = $true
  }
  elseif ($functionBody -match "paramIndex\s*==\s*$ParamIndex") {
    # if (paramIndex == X) { ... } -> X IS handled  
    $isHandled = $true
  }
    
  if (-not $isHandled) {
    return @()
  }

  # Multi-enum support: when the function handles several parameters via
  # "if (paramIndex == X) { switch (enumValue) {...} }" blocks, scope the
  # case extraction to the block of the requested parameter only.
  # The block runs from "paramIndex == X" to the next "paramIndex ==" (or end).
  $paramBlock = $functionBody
  $eqMatches = [regex]::Matches($functionBody, 'paramIndex\s*==\s*(\d+)')
  if ($eqMatches.Count -gt 0) {
    for ($em = 0; $em -lt $eqMatches.Count; $em++) {
      if ([int]$eqMatches[$em].Groups[1].Value -eq $ParamIndex) {
        $blockStart = $eqMatches[$em].Index
        $blockEnd = if ($em + 1 -lt $eqMatches.Count) { $eqMatches[$em + 1].Index } else { $functionBody.Length }
        $paramBlock = $functionBody.Substring($blockStart, $blockEnd - $blockStart)
        break
      }
    }
  }

  # Now extract all case statements from the switch block
  # Pattern: case XXXXX: return "Name";
  $enumOptions = @()
  $caseMatches = [regex]::Matches($paramBlock, 'case\s+([A-Z_0-9]+)\s*:\s*return\s+"([^"]+)"')
    
  foreach ($match in $caseMatches) {
    $enumConstant = $match.Groups[1].Value
    $enumName = $match.Groups[2].Value
        
    # Try to find the enum definition value in the file
    # Look for patterns like: ENUM_NAME = 0, or enum { ENUM_NAME = 0, ... }
    $enumValue = -1
    if ($enumConstant -match '^\d+$') {
      # Numeric case label (e.g. "case 0:") -> the label IS the value.
      # Must take precedence over the symbolic-constant lookup below, which
      # would otherwise match a comment like "0=5x7" and parse the wrong value.
      $enumValue = [int]$enumConstant
    }
    elseif ($Content -match "enum[^{]*\{[^}]*$enumConstant\s*=\s*(\d+)") {
      $enumValue = [int]$Matches[1]
    }
    elseif ($Content -match "$enumConstant\s*=\s*(\d+)") {
      $enumValue = [int]$Matches[1]
    }
    else {
      # Fallback: use sequential numbering
      $enumValue = $enumOptions.Count
    }
        
    $enumOptions += @{
      Value = $enumValue
      Name  = $enumName
    }
  }
    
  # Sort by value
  if ($enumOptions.Count -gt 0) {
    $enumOptions = $enumOptions | Sort-Object Value
  }
    
  return $enumOptions
}

# ====================================================================
# Effect ID Management - Dynamic Discovery from Headers
# ====================================================================
# IDs are generated dynamically by scanning Effect headers and sorting
# alphabetically by display name. No manual mapping or XML parsing needed!

# Caches the parsed effect list so we don't re-scan for every call
$script:EffectIdCache = $null
$script:EffectIdCacheMTime = $null

function Get-EffectDisplayNameFromHeader {
  param([Parameter(Mandatory)][string]$HeaderPath)

  if (-not (Test-Path -LiteralPath $HeaderPath)) {
    throw "Effect header not found: $HeaderPath"
  }

  $content = Get-Content -LiteralPath $HeaderPath -Raw -Encoding UTF8

  # Try to match EFFECT_NAME_DE_EN macro first (new multi-language pattern)
  $rxMacro = [regex]$script:Config.Patterns.GetNameMacro
  $mMacro = $rxMacro.Match($content)

  if ($mMacro.Success) {
    # Return hashtable with both languages
    return @{
      DE = $mMacro.Groups[1].Value
      EN = $mMacro.Groups[2].Value
    }
  }

  # Fallback: Try old pattern (plain string literal) - assume it's English
  $rxPlain = [regex]$script:Config.Patterns.GetNamePlain
  $mPlain = $rxPlain.Match($content)

  if ($mPlain.Success) {
    # Return only EN, DE is same as EN
    $name = $mPlain.Groups[1].Value
    return @{
      DE = $name
      EN = $name
    }
  }

  throw "Could not find getName() return in: $HeaderPath"
}

function Get-EffectDropdownText {
  param(
    [Parameter(Mandatory)][string]$ClassName,
    [Parameter(Mandatory)][string]$NameDE,
    [string]$NameEN,
    [Parameter(Mandatory)][int]$EffectId
  )

  $baseName = if ($NameDE) { $NameDE } elseif ($NameEN) { $NameEN } else { $ClassName }

  return $baseName
}

function Test-Is2DEffect {
  param(
    [Parameter(Mandatory)][string]$ClassName,
    [Parameter(Mandatory)][string]$NameDE,
    [string]$NameEN
  )

  $baseName = if ($NameDE) { $NameDE } elseif ($NameEN) { $NameEN } else { $ClassName }
  # A class name ending in 2D is the reliable marker: the display name may not carry it at all
  # (TRON, Starfield Warp, Plasma Nebula, UFO Swarm), and the token check below would miss those.
  return ($baseName -match '(?<![A-Za-z0-9])2D(?![A-Za-z0-9])' -or
    $ClassName -match '2D(Effect)?$' -or
    $ClassName -match '(?<![A-Za-z0-9])2D(?![A-Za-z0-9])')
}

# ====================================================================
# Effect Pool Registration Order Parser
# ====================================================================
# Parse EffectPool.cpp to extract the exact registration order.
# This ensures XML IDs match runtime indices exactly.
# ====================================================================

function Parse-EffectPoolRegistrationOrder {
  <#
    .SYNOPSIS
        Parse EffectPool.cpp getEffectByIndex() to extract effect registration order.

    .DESCRIPTION
        This function parses the getEffectByIndex() function in EffectPool.cpp
        to extract the exact order in which effects are registered.

        The registration order directly defines the effect IDs:
          - Index 0 in getEffectByIndex() → XML Value="0"
          - Index 1 in getEffectByIndex() → XML Value="1"

        Returns an ordered list of effect class names (e.g., "SolidEffect", "WipeEffect").

    .EXAMPLE
        $order = Parse-EffectPoolRegistrationOrder
        # Returns: @("SolidEffect", "WipeEffect", "RainbowEffect", ...)
    #>

  $effectPoolPath = Resolve-RepoPath $script:Config.EffectPoolPath

  if (-not (Test-Path $effectPoolPath)) {
    throw "EffectPool.cpp not found at: $effectPoolPath"
  }

  Write-ScriptVerbose "Parsing EffectPool.cpp registration order from: $effectPoolPath"

  $content = Get-Content -Path $effectPoolPath -Raw -Encoding UTF8

  # Extract getEffectByIndex() function body
  $match = [regex]::Match($content, $script:Config.Patterns.EffectPoolFunction, [System.Text.RegularExpressions.RegexOptions]::Singleline)

  if (-not $match.Success) {
    throw "Could not find getEffectByIndex() function in EffectPool.cpp"
  }

  $functionBody = $match.Groups[1].Value

  # Parse return statements: "if (index == currentIndex++) return getXXX();"
  # We need to extract them in order and map getXXX → XXXEffect
  $returnPattern = $script:Config.Patterns.EffectPoolReturn
  $regexMatches = [regex]::Matches($functionBody, $returnPattern)

  $effectOrder = @()

  foreach ($m in $regexMatches) {
    $effectBaseName = $m.Groups[1].Value  # e.g., "Solid", "Wipe", "Rainbow"

    # Handle special naming: EffectSolid/EffectWipe vs. BPMEffect pattern
    $effectClassName = if ($effectBaseName -in @("Solid", "Wipe")) {
      "Effect${effectBaseName}"  # → EffectSolid, EffectWipe
    }
    else {
      "${effectBaseName}Effect"  # → BPMEffect, CylonEffect
    }

    $effectOrder += $effectClassName
    Write-ScriptVerbose "  Registration [$($effectOrder.Count - 1)]: $effectClassName" "DarkGray"
  }

  if ($effectOrder.Count -eq 0) {
    Write-Warning "No effects found in getEffectByIndex(). Pattern might need adjustment."
  }
  else {
    Write-ScriptVerbose "Parsed $($effectOrder.Count) effects from EffectPool.cpp registration order" "Green"
  }

  return $effectOrder
}

function Build-EffectIdMap {
  $effectsDir = Resolve-RepoPath $script:Config.EffectsDir

  Write-ScriptVerbose "Building effect ID map from: $effectsDir"

  # Check cache validity based on newest file modification time
  $latestMTime = (Get-ChildItem -Path $effectsDir -Filter $script:Config.EffectHeaderPattern |
    Sort-Object LastWriteTimeUtc -Descending |
    Select-Object -First 1).LastWriteTimeUtc

  # Cache hit?
  if ($script:EffectIdCache -and $script:EffectIdCacheMTime -eq $latestMTime) {
    Write-ScriptVerbose "Using cached effect ID map" "DarkGray"
    return $script:EffectIdCache
  }

  Write-ScriptVerbose "Cache miss - rebuilding effect ID map" "DarkGray"

  $headers = Get-AllEffectHeaders
  $map = @{}

  Write-ScriptVerbose "First pass: Extracting effect names from headers"

  # First pass: collect all effects with their display names
  foreach ($header in $headers) {
    $basename = $header.BaseName

    # Filename is already the class name (no conversion needed):
    # - EffectSolid.h → EffectSolid
    # - EffectWipe.h → EffectWipe
    # - BPMEffect.h → BPMEffect
    $className = $basename

    try {
      $names = Get-EffectDisplayNameFromHeader -HeaderPath $header.FullName

      $map[$className] = @{
        NameDE   = $names.DE
        NameEN   = $names.EN
        FilePath = $header.FullName
      }
      Write-ScriptVerbose "  Parsed $className → DE:'$($names.DE)' EN:'$($names.EN)'" "DarkGray"
    }
    catch {
      Write-Warning "Could not parse effect $className from $($header.Name): $_"
    }
  }

  Write-ScriptVerbose "Found $($map.Count) effect headers with names"
  Write-ScriptVerbose "Second pass: Assigning IDs based on EffectPool.cpp registration order"

  # Second pass: Assign IDs based on EffectPool.cpp registration order
  # This ensures XML IDs match runtime indices exactly!
  $registrationOrder = Parse-EffectPoolRegistrationOrder

  if ($registrationOrder.Count -eq 0) {
    # NEVER fall back to alphabetical order here: it would re-number every effect ID
    # and silently break external KO effect assignments. Abort instead.
    Write-Host ""
    Write-Host "FATAL: Could not parse EffectPool.cpp getEffectByIndex() registration order." -ForegroundColor Red
    Write-Host "Refusing alphabetical fallback (would scramble all effect IDs)." -ForegroundColor Red
    Write-Host "Fix the getEffectByIndex() format/parser, then re-run." -ForegroundColor Red
    exit 1
  }

  $effectIds = @{}
  $id = 0

  foreach ($effectClassName in $registrationOrder) {
    if ($map.ContainsKey($effectClassName)) {
      $effectIds[$effectClassName] = $id
      $effectName = $map[$effectClassName].NameEN
      Write-ScriptVerbose "  ID $id → $effectClassName ('$effectName')" "Green"
      $id++
    }
    else {
      Write-Warning "Effect $effectClassName found in EffectPool.cpp but no header file found!"
      Write-ScriptVerbose "  Available effect classes: $($map.Keys -join ', ')" "Yellow"
    }
  }

  Write-ScriptVerbose "ID mapping complete: $($effectIds.Count) effects assigned IDs 0-$($id - 1)" "Green"

  # ---------------------------------------------------------------------------
  # Frozen Effect-ID guard (src/EffectIds.lock)
  # Effect IDs are referenced externally (KNX KO effect selection) and MUST stay
  # stable. Compare the freshly computed IDs against the committed lock file and
  # ABORT the build if any EXISTING effect's ID changed. New effects are appended
  # to the lock automatically - the developer never edits the lock by hand.
  # Append-only rule lives in EffectPool.cpp getEffectByIndex().
  # ---------------------------------------------------------------------------
  if ((-not $script:EffectIdsLockChecked) -and $effectIds.Count -gt 0) {
    $script:EffectIdsLockChecked = $true
    $lockPath = Join-Path (Split-Path -Parent $PSScriptRoot) 'src/EffectIds.lock'

    # load existing lock: className -> id
    $locked = @{}
    if (Test-Path $lockPath) {
      foreach ($line in [System.IO.File]::ReadAllLines($lockPath)) {
        $t = $line.Trim()
        if ($t -eq '' -or $t.StartsWith('#')) { continue }
        $parts = $t -split "`t"
        if ($parts.Count -ge 2) { $locked[$parts[1]] = [int]$parts[0] }
      }
    }

    # dangerous case: an existing effect's ID moved
    $violations = @()
    $removed = @()
    foreach ($cn in $locked.Keys) {
      if ($effectIds.ContainsKey($cn)) {
        if ($effectIds[$cn] -ne $locked[$cn]) {
          $violations += "  $cn : locked ID $($locked[$cn]) -> now $($effectIds[$cn])"
        }
      }
      else {
        $removed += "$cn (was ID $($locked[$cn]))"
      }
    }

    if ($violations.Count -gt 0) {
      Write-Host ""
      Write-Host "FATAL: Effect ID(s) changed vs src/EffectIds.lock!" -ForegroundColor Red
      Write-Host "External KO effect assignments would break. Build aborted." -ForegroundColor Red
      $violations | ForEach-Object { Write-Host $_ -ForegroundColor Yellow }
      Write-Host "Effects must be APPEND-ONLY in EffectPool.cpp getEffectByIndex()." -ForegroundColor Yellow
      Write-Host "To retire an effect, keep its slot (tombstone) so following IDs do not shift." -ForegroundColor Yellow
      Write-Host "If an ID change is truly intended, delete src/EffectIds.lock and re-run to re-baseline." -ForegroundColor Yellow
      exit 1
    }
    if ($removed.Count -gt 0) {
      Write-Warning "Effect(s) removed from registration (ID retired): $($removed -join ', ')"
    }

    # rewrite lock from current state (sorted by id) - auto-appends new effects
    $sortedIds = $effectIds.GetEnumerator() | Sort-Object Value
    $lines = @()
    $lines += "# AUTO-MANAGED by Build-EffectParameters.ps1 - DO NOT EDIT BY HAND."
    $lines += "# Frozen effect ID assignment (referenced by KNX KOs). New effects are"
    $lines += "# appended automatically; changing an existing ID makes the build FAIL."
    $lines += "# Columns (TAB-separated): ID  EffectClassName  DisplayNameEN"
    foreach ($e in $sortedIds) {
      $nm = ""
      if ($map.ContainsKey($e.Key)) { $nm = $map[$e.Key].NameEN }
      $lines += ("{0}`t{1}`t{2}" -f $e.Value, $e.Key, $nm)
    }
    $newContent = ($lines -join "`n") + "`n"
    $oldContent = ""
    if (Test-Path $lockPath) { $oldContent = [System.IO.File]::ReadAllText($lockPath) }
    if ($newContent -ne $oldContent) {
      [System.IO.File]::WriteAllText($lockPath, $newContent, (New-Object System.Text.UTF8Encoding($false)))
      if ($locked.Count -eq 0) {
        Write-Host "  Created src/EffectIds.lock ($($effectIds.Count) effects, IDs 0-$($id - 1))" -ForegroundColor Green
      }
      else {
        Write-Host "  Updated src/EffectIds.lock (effect(s) appended)" -ForegroundColor Green
      }
    }
  }

  # Update cache
  $script:EffectIdCache = $effectIds
  $script:EffectIdCacheMTime = $latestMTime

  return $effectIds
}

function Get-EffectId {
  param([Parameter(Mandatory)][string]$EffectName)

  $map = Build-EffectIdMap

  if ($map.ContainsKey($EffectName)) {
    return $map[$EffectName]
  }

  Write-Warning "Unknown effect: $EffectName (not found in effects directory)"
  return -1
}

# ====================================================================
# Effect Type Enumeration XML Generation
# ====================================================================

function Generate-EffectTypeEnumeration {
  <#
    .SYNOPSIS
        Generates XML Enumeration entries for all effects
    .DESCRIPTION
        Scans all Effect headers, extracts display names (DE/EN), sorts by EffectPool.cpp order,
        and generates <Enumeration> XML entries with IDs matching runtime indices
    #>

  Write-Host "  Generating Effect Type Enumeration XML..." -ForegroundColor Cyan
  Write-ScriptVerbose "Starting effect type enumeration generation"

  $headers = Get-AllEffectHeaders
  $effectList = @()

  Write-ScriptVerbose "Collecting effect display names from $($headers.Count) headers"

  # Collect all effects with their display names
  foreach ($header in $headers) {
    $className = $header.BaseName
    try {
      $names = Get-EffectDisplayNameFromHeader -HeaderPath $header.FullName
      $effectList += [PSCustomObject]@{
        ClassName = $className
        NameDE    = $names.DE
        NameEN    = $names.EN
      }
      Write-Host "    ✓ " -NoNewline -ForegroundColor Green
      Write-Host "$className".PadRight(30) -NoNewline -ForegroundColor Gray
      if ($names.DE -ne $names.EN) {
        Write-Host "→ DE: '$($names.DE)' | EN: '$($names.EN)'" -ForegroundColor White
      }
      else {
        Write-Host "→ '$($names.EN)' (no translation)" -ForegroundColor White
      }
      Write-ScriptVerbose "  ${className}: DE='$($names.DE)' EN='$($names.EN)'" "DarkGray"
    }
    catch {
      Write-Warning "    ✗ Could not parse $className : $_"
    }
  }

  Write-ScriptVerbose "Collected $($effectList.Count) effects with display names"

  # ⚠️ CRITICAL: Sort by EffectPool.cpp registration order, NOT alphabetically!
  # XML IDs must match runtime indices from getEffectByIndex()
  Write-ScriptVerbose "Sorting effects by EffectPool.cpp registration order"
  $registrationOrder = Parse-EffectPoolRegistrationOrder

  if ($registrationOrder.Count -eq 0) {
    Write-Warning "Failed to parse EffectPool.cpp! Falling back to alphabetical sort (IDs will be WRONG!)"
    $sorted = $effectList | Sort-Object NameEN
  }
  else {
    # Create lookup for effects by class name
    $effectLookup = @{}
    foreach ($effect in $effectList) {
      $effectLookup[$effect.ClassName] = $effect
    }

    # Build sorted list based on registration order
    $sorted = @()
    foreach ($className in $registrationOrder) {
      if ($effectLookup.ContainsKey($className)) {
        $sorted += $effectLookup[$className]
        Write-ScriptVerbose "  Ordered: $className" "DarkGray"
      }
    }

    Write-Host "  ✓ Effect order synced with EffectPool.cpp registration" -ForegroundColor Green
    Write-ScriptVerbose "Successfully ordered $($sorted.Count) effects" "Green"
  }

  # Build runtime ID map first (registration order defines IDs),
  # then sort for display: 1D first, 2D last.
  $effectsWithRuntimeId = @()
  $runtimeId = 0
  foreach ($effect in $sorted) {
    $effectsWithRuntimeId += [PSCustomObject]@{
      ClassName = $effect.ClassName
      NameDE    = $effect.NameDE
      NameEN    = $effect.NameEN
      RuntimeId = $runtimeId
      Is2D      = (Test-Is2DEffect -ClassName $effect.ClassName -NameDE $effect.NameDE -NameEN $effect.NameEN)
    }
    $runtimeId++
  }

  $displaySorted = $effectsWithRuntimeId | Sort-Object @{ Expression = { if ($_.Is2D) { 1 } else { 0 } } }, @{ Expression = { $_.RuntimeId } }

  # Generate XML enumeration entries - use German as default, add op:headerName for C++ enum export
  Write-ScriptVerbose "Generating XML enumeration entries"
  $xmlLines = @()

  foreach ($effect in $displaySorted) {
    $text = Get-EffectDropdownText -ClassName $effect.ClassName -NameDE $effect.NameDE -NameEN $effect.NameEN -EffectId $effect.RuntimeId
    # Derive clean C++ enum name from ClassName (remove "Effect" prefix/suffix)
    $enumName = $effect.ClassName -replace 'Effect$', '' -replace '^Effect', ''
    if ([string]::IsNullOrEmpty($enumName)) { $enumName = $effect.ClassName }

    # Add comment with English name if different
    if ($effect.NameDE -ne $effect.NameEN) {
      $xmlLines += "                <Enumeration Text=`"$text`" Value=`"$($effect.RuntimeId)`" Id=`"%ENID%`" op:headerName=`"$enumName`"/> <!-- EN: $($effect.NameEN) -->"
    }
    else {
      $xmlLines += "                <Enumeration Text=`"$text`" Value=`"$($effect.RuntimeId)`" Id=`"%ENID%`" op:headerName=`"$enumName`"/>"
    }
    Write-ScriptVerbose "  XML Entry ID $($effect.RuntimeId): '$text' -> enum '$enumName'" "DarkGray"
  }

  Write-Host ""
  Write-Host "  Generated $($xmlLines.Count) effect enumerations (IDs 0-$($runtimeId-1))" -ForegroundColor Green
  Write-Host "  Note: Using DE names as default in XML. EN names in comments." -ForegroundColor Yellow
  Write-ScriptVerbose "Effect type enumeration generation complete" "Green"
  Write-Host ""

  return ($xmlLines -join "`n")
}

function Generate-EffectMappingCpp {
  <#
    .SYNOPSIS
        Generates C++ getEffectFromType() function for EffectConfiguration.cpp
    .DESCRIPTION
        Generates a C++ header with the effect type mapping function that maps
        ETS effect type IDs to EffectPool::getXXX() calls, synchronized with
        the same EffectPool.cpp registration order used for XML enumeration.
    #>

  Write-ScriptVerbose "Generating C++ effect mapping function"

  # Get effect list
  $effectsDir = Resolve-RepoPath $script:Config.EffectsDir
  $effectFiles = Get-ChildItem -Path $effectsDir -Filter "*.h" | Where-Object {
    $_.Name -notin $script:Config.ExcludeHeaders -and
    $_.Name -match '(Effect.+\.h$|.+Effect\.h$)'
  }

  $effectList = @()
  foreach ($file in $effectFiles) {
    $content = Get-Content $file.FullName -Raw -Encoding UTF8

    # Extract class name
    if ($content -match $script:Config.Patterns.ClassName) {
      $className = $Matches[1]
      $names = Get-EffectDisplayNameFromHeader -HeaderPath $file.FullName

      $effectList += @{
        ClassName = $className
        NameDE    = $names.DE
        NameEN    = $names.EN
        FilePath  = $file.FullName
      }
    }
  }

  # Sort by EffectPool.cpp registration order
  $registrationOrder = Parse-EffectPoolRegistrationOrder

  if ($registrationOrder.Count -eq 0) {
    Write-Warning "Failed to parse EffectPool.cpp! Falling back to alphabetical sort"
    $sorted = $effectList | Sort-Object NameEN
  }
  else {
    $effectLookup = @{}
    foreach ($effect in $effectList) {
      $effectLookup[$effect.ClassName] = $effect
    }

    $sorted = @()
    foreach ($className in $registrationOrder) {
      if ($effectLookup.ContainsKey($className)) {
        $sorted += $effectLookup[$className]
      }
    }
  }

  # Generate C++ switch cases
  $currentYear = 2026   # fixed -> deterministic output (no per-build churn); bump when needed
  $cppLines = @()

  # Header comment (matching HardwareMappingData.h style)
  $cppLines += "/**"
  $cppLines += " * @file EffectTypeMapping.h"
  $cppLines += " * @brief Effect Type ID to EffectPool Instance Mapping (Auto-Generated)"
  $cppLines += " *"
  $cppLines += " * This file contains the mapping between ETS Effect Type IDs and EffectPool singleton instances."
  $cppLines += " * The mapping is automatically generated from Effect header files at build time and synchronized"
  $cppLines += " * with the registration order in EffectPool.cpp and the enumeration in NeoPixel.share.xml."
  $cppLines += " *"
  $cppLines += " * @warning AUTO-GENERATED FILE - DO NOT EDIT MANUALLY"
  $cppLines += " * @note Auto-generated - do not edit by hand"
  $cppLines += " * @note Source: Build-EffectParameters.ps1"
  $cppLines += " *"
  $cppLines += " * @copyright Copyright (c) $currentYear OpenKNX (Licensed under GNU GPL v3.0)"
  $cppLines += " */"
  $cppLines += ""
  $cppLines += "#pragma once"
  $cppLines += ""
  $cppLines += "#include ""../lib/OFM-NeoPixel/src/effects/EffectPool.h"""
  $cppLines += ""
  $cppLines += "/**"
  $cppLines += " * @brief Maps ETS effect type ID to actual Effect instance"
  $cppLines += " * "
  $cppLines += " * This function is synchronized with the Effect Type enumeration in"
  $cppLines += " * NeoPixel.share.xml and the registration order in EffectPool.cpp"
  $cppLines += " * "
  $cppLines += " * @param effectType Effect type ID from ETS (0-$($sorted.Count - 1))"
  $cppLines += " * @return Pointer to Effect instance, or Solid effect for unknown types"
  $cppLines += " */"
  $cppLines += "inline Effect* getEffectFromType(uint8_t effectType)"
  $cppLines += "{"
  $cppLines += "    switch (static_cast<PT_NEOEffectType>(effectType))"
  $cppLines += "    {"

  # Generate switch cases using enum names
  $id = 0
  foreach ($effect in $sorted) {
    # Convert ClassName to getXXX() function name
    # BPMEffect -> getBPM()
    # EffectSolid -> getSolid()
    $methodName = $effect.ClassName -replace 'Effect', ''

    # Derive enum name (same logic as enumeration generation)
    $enumName = $effect.ClassName -replace 'Effect$', '' -replace '^Effect', ''
    if ([string]::IsNullOrEmpty($enumName)) { $enumName = $effect.ClassName }

    $comment = "// $($effect.NameEN)"
    if ($effect.NameDE -ne $effect.NameEN) {
      $comment += " (DE: $($effect.NameDE))"
    }

    # Guard the case with the same flag EffectPool.cpp uses, so a disabled effect does not
    # leave an undefined reference behind. Solid is always present and needs no guard.
    if ($methodName -ne "Solid") {
      $cppLines += "#ifndef NEOPIXEL_DISABLE_$($methodName.ToUpper())"
    }
    $cppLines += "        case PT_NEOEffectType::${enumName}: return EffectPool::get${methodName}(); $comment"
    if ($methodName -ne "Solid") {
      $cppLines += "#endif"
    }
    $id++
  }

  # Default case
  $cppLines += "        default: return EffectPool::getSolid(); // Fallback to Solid"
  $cppLines += "    }"
  $cppLines += "}"
  $cppLines += ""
  $cppLines += "/**"
  $cppLines += " * @brief Maps Effect instance pointer back to ETS effect type ID (reverse mapping)"
  $cppLines += " * "
  $cppLines += " * This function is synchronized with getEffectFromType() and provides the reverse mapping."
  $cppLines += " * Used for power-off snapshots to save the currently active effect type ID."
  $cppLines += " * "
  $cppLines += " * @param effect Pointer to Effect instance"
  $cppLines += " * @return Effect type ID (0-$($sorted.Count - 1)), or 0 (Solid) if effect is nullptr or unknown"
  $cppLines += " */"
  $cppLines += "inline uint8_t getTypeFromEffect(Effect* effect)"
  $cppLines += "{"
  $cppLines += "    if (!effect) return 0; // nullptr -> Solid"
  $cppLines += ""

  # Generate reverse mapping checks using enum cast
  $id = 0
  foreach ($effect in $sorted) {
    $methodName = $effect.ClassName -replace 'Effect', ''
    $enumName = $effect.ClassName -replace 'Effect$', '' -replace '^Effect', ''
    if ([string]::IsNullOrEmpty($enumName)) { $enumName = $effect.ClassName }
        
    $comment = "// $($effect.NameEN)"
    if ($effect.NameDE -ne $effect.NameEN) {
      $comment += " (DE: $($effect.NameDE))"
    }

    if ($methodName -ne "Solid") {
      $cppLines += "#ifndef NEOPIXEL_DISABLE_$($methodName.ToUpper())"
    }
    $cppLines += "    if (effect == EffectPool::get${methodName}()) return static_cast<uint8_t>(PT_NEOEffectType::${enumName}); $comment"
    if ($methodName -ne "Solid") {
      $cppLines += "#endif"
    }
    $id++
  }

  $cppLines += ""
  $cppLines += "    return 0; // Unknown effect -> fallback to Solid"
  $cppLines += "}"

  Write-Host ""
  Write-Host "  Generated C++ forward mapping: getEffectFromType() for $($sorted.Count) effects (IDs 0-$($id-1))" -ForegroundColor Green
  Write-Host "  Generated C++ reverse mapping: getTypeFromEffect() for $($sorted.Count) effects" -ForegroundColor Green
  Write-ScriptVerbose "C++ effect mapping generation complete (bidirectional)" "Green"
  Write-Host ""

  return ($cppLines -join "`n")
}

function Update-EffectTypeEnumerationInShareXml {
  <#
    .SYNOPSIS
        Updates the GENERATED_EFFECT_ENUMERATIONS section in NeoPixel.share.xml
    #>

  param([Parameter(Mandatory)][string]$ShareXmlPath)

  Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
  Write-Host "  Updating Effect Type Enumeration in NeoPixel.share.xml" -ForegroundColor Cyan
  Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
  Write-Host ""
  Write-ScriptVerbose "Updating effect type enumeration in: $ShareXmlPath"

  if (-not (Test-Path $ShareXmlPath)) {
    throw "NeoPixel.share.xml not found at: $ShareXmlPath"
  }

  Write-ScriptVerbose "Generating new enumeration XML content"
  # Generate new enumeration XML
  $enumXml = Generate-EffectTypeEnumeration

  # Read current XML
  Write-ScriptVerbose "Reading current XML content"
  $xmlContent = Get-Content $ShareXmlPath -Raw -Encoding UTF8

  # Use markers from config
  $startMarker = $script:Config.Markers.EnumStart
  $endMarker = $script:Config.Markers.EnumEnd

  Write-ScriptVerbose "Looking for markers: $startMarker / $endMarker"

  # Check if markers exist
  if ($xmlContent -notmatch [regex]::Escape($startMarker)) {
    Write-Warning "Marker $startMarker not found in NeoPixel.share.xml"
    Write-Warning "Please add markers manually around the Effect Type enumeration section."
    return
  }

  # Replace content between markers using regex
  $pattern = "(?s)($([regex]::Escape($startMarker))).*?($([regex]::Escape($endMarker)))"
  $replacement = "`$1`n$enumXml`n                `$2"

  Write-ScriptVerbose "Replacing content between markers"
  $newXmlContent = $xmlContent -replace $pattern, $replacement

  # Write back to file
  Write-ScriptVerbose "Writing updated XML back to file"
  Set-Content -Path $ShareXmlPath -Value $newXmlContent -NoNewline -Encoding UTF8

  # Second variant without the 2D effects, for hardware built with NEOPIXEL_DISABLE_2D.
  # The list is already sorted with the 2D effects last, so dropping them keeps the order.
  $no2dLines = @($enumXml -split "`n" | Where-Object { $_ -notmatch 'op:headerName="[A-Za-z]*2D"' })
  $no2dXml = ($no2dLines -join "`n") -replace ' op:headerName="[^"]*"', ''

  $startNo2D = $script:Config.Markers.EnumNo2DStart
  $endNo2D = $script:Config.Markers.EnumNo2DEnd
  if ($newXmlContent -match [regex]::Escape($startNo2D)) {
    $patternNo2D = "(?s)($([regex]::Escape($startNo2D))).*?($([regex]::Escape($endNo2D)))"
    $newXmlContent = $newXmlContent -replace $patternNo2D, "`$1`n$no2dXml`n                `$2"
    Set-Content -Path $ShareXmlPath -Value $newXmlContent -NoNewline -Encoding UTF8
    Write-Host "  * NEOEffectTypeNo2D: $($no2dLines.Count) of $(($enumXml -split "`n").Count) effects" -ForegroundColor Green
  }
  else {
    Write-WarningMsg "Marker $startNo2D not found - the no-2D effect list was not generated"
  }

  Write-Host "  ✓ NeoPixel.share.xml updated successfully!" -ForegroundColor Green
  Write-ScriptVerbose "Effect type enumeration update complete" "Green"
  Write-Host ""
}

function Update-EffectParameterTypesInShareXml {
  <#
    .SYNOPSIS
        Injects generated Effect ParameterTypes into NeoPixel.share.xml between markers
    #>

  param(
    [Parameter(Mandatory)][string]$ShareXmlPath,
    [Parameter(Mandatory)][string]$Content
  )

  Write-Host "  • Injecting Effect ParameterTypes into NeoPixel.share.xml..." -ForegroundColor Cyan

  if (-not (Test-Path $ShareXmlPath)) {
    throw "NeoPixel.share.xml not found at: $ShareXmlPath"
  }

  $xmlContent = Get-Content $ShareXmlPath -Raw -Encoding UTF8

  $startMarker = $script:Config.Markers.EffectPTStart
  $endMarker = $script:Config.Markers.EffectPTEnd

  if ($xmlContent -notmatch [regex]::Escape($startMarker)) {
    Write-Warning "Marker $startMarker not found in NeoPixel.share.xml"
    return
  }

  $pattern = "(?s)($([regex]::Escape($startMarker))).*?($([regex]::Escape($endMarker)))"
  $replacement = "`$1`n$Content`n              `$2"
  $newXmlContent = $xmlContent -replace $pattern, $replacement

  Set-Content -Path $ShareXmlPath -Value $newXmlContent -NoNewline -Encoding UTF8
  Write-Host "  ✓ Effect ParameterTypes injected into NeoPixel.share.xml" -ForegroundColor Green
}


function Parse-EffectHeader {
  param(
    [string]$HeaderPath,
    [string]$Lang
  )

  $fileName = Split-Path -Leaf $HeaderPath
  Write-Host "  " -NoNewline
  Write-Host "Parsing $fileName".PadRight(50) -NoNewline -ForegroundColor Gray
  Write-ScriptVerbose "Parsing effect header: $HeaderPath"

  $content = Get-Content -Path $HeaderPath -Raw -Encoding UTF8

  # Extract effect class name
  if ($content -match $script:Config.Patterns.ClassName) {
    $effectName = $Matches[1]
    Write-ScriptVerbose "  Class name: $effectName" "DarkGray"
  }
  else {
    Write-Warning "Could not extract class name from $HeaderPath"
    return $null
  }

  # Extract effect display name (from getName() return)
  Write-ScriptVerbose "  Extracting display names" "DarkGray"
  $names = Get-EffectDisplayNameFromHeader -HeaderPath $HeaderPath

  # Extract parameter count
  if ($content -match $script:Config.Patterns.ParameterCount) {
    $paramCount = [int]$Matches[1]
    Write-ScriptVerbose "  Parameter count: $paramCount" "DarkGray"
  }
  else {
    $paramCount = 0
    Write-ScriptVerbose "  No parameters found" "DarkGray"
  }

  Write-Host "OK " -NoNewline -ForegroundColor Green
  Write-Host "Effect: " -NoNewline -ForegroundColor DarkGray
  Write-Host "$($names.DE)".PadRight(20) -NoNewline -ForegroundColor White
  Write-Host "ID: " -NoNewline -ForegroundColor DarkGray
  Write-Host "$(Get-EffectId $effectName)".PadRight(4) -NoNewline -ForegroundColor Cyan
  Write-Host "Params: " -NoNewline -ForegroundColor DarkGray
  Write-Host "$paramCount" -ForegroundColor Green

  # Parse parameters
  $parameters = @()
  for ($i = 0; $i -lt $paramCount; $i++) {
    Write-ScriptVerbose "  Parsing parameter $i" "DarkGray"
    $paramName = Extract-SwitchCase -Content $content -FunctionName "ParameterName" -CaseIndex $i
    $paramDesc = Extract-SwitchCaseMultiLang -Content $content -FunctionName "ParameterDescription" -CaseIndex $i -Lang $Lang
    $paramType = Extract-ParameterType -Content $content -CaseIndex $i
    $paramDefault = Extract-IntValue -Content $content -FunctionName "ParameterDefault" -CaseIndex $i
    $paramMin = Extract-IntValue -Content $content -FunctionName "ParameterMin" -CaseIndex $i
    $paramMax = Extract-IntValue -Content $content -FunctionName "ParameterMax" -CaseIndex $i

    # Extract ENUM options if parameter is PARAM_ENUM
    $enumOptions = @()
    if ($paramType -eq 'PARAM_ENUM') {
      $enumOptions = Extract-EnumOptions -Content $content -ParamIndex $i
      if ($enumOptions.Count -gt 0) {
        Write-ScriptVerbose "    Found $($enumOptions.Count) enum options" "DarkGray"
      }
    }

    # Set sensible defaults for Min/Max if not defined (both are 0)
    # For UINT8/HUE types, use full range 0-255 unless explicitly limited
    if ($paramMin -eq 0 -and $paramMax -eq 0) {
      if ($paramType -match $script:Config.Patterns.ParamTypeUint8) {
        $paramMax = $script:Config.DefaultMaxUint8
      }
      elseif ($paramType -eq 'PARAM_PERCENT') {
        $paramMax = $script:Config.DefaultMaxPercent
      }
      elseif ($paramType -eq 'PARAM_ENUM' -and $enumOptions.Count -gt 0) {
        # For ENUM, use the count of options
        $paramMax = $enumOptions.Count - 1
      }
    }

    if ($paramName) {
      $enumInfo = if ($enumOptions.Count -gt 0) { " [" + ($enumOptions | ForEach-Object { $_.Name }) -join ", " + "]" } else { "" }
      Write-Host "       [$i] $paramName ($paramType) = $paramDefault [$paramMin..$paramMax]$enumInfo" -ForegroundColor DarkGray
      Write-ScriptVerbose "    [$i] ${paramName}: type=$paramType default=$paramDefault range=[$paramMin..$paramMax]" "DarkGray"
      $parameters += @{
        Index       = $i
        Name        = $paramName
        Description = $paramDesc
        Type        = $paramType
        Default     = $paramDefault
        Min         = $paramMin
        Max         = $paramMax
        EnumOptions = $enumOptions
      }
    }
  }

  $effectId = Get-EffectId -EffectName $effectName

  # Clean effect name for file naming (remove "Effect" suffix, spaces, hyphens)
  $cleanName = $effectName -replace $script:Config.CleanPatterns.RemoveEffectSuffix, ''
  $cleanName = $cleanName -replace $script:Config.CleanPatterns.AlphanumericOnly, ''

  Write-ScriptVerbose "Parsed effect: $cleanName (ID: $effectId, $($parameters.Count) params)" "Green"

  return @{
    Name       = $cleanName
    ClassName  = $effectName
    NameDE     = $names.DE
    NameEN     = $names.EN
    EffectID   = $effectId
    Parameters = $parameters
  }
}

function Generate-ParameterTypes {
  param([array]$Effects)

  $xml = @()

  foreach ($effect in $Effects) {
    if ($effect.Parameters.Count -eq 0) { continue }

    $xml += ''
    $xml += "              <!-- $($effect.NameDE) Effect Parameters -->"

    foreach ($param in $effect.Parameters) {
      # ParameterType ID format: %AID%_PT-{EffectName}{ParamName}
      # Replace hyphens/spaces with nothing for clean names
      $effectNameClean = $effect.NameDE -replace $script:Config.CleanPatterns.RemoveSpacesHyphens, ''
      $paramNameClean = $param.Name -replace $script:Config.CleanPatterns.RemoveSpacesHyphens, ''
      $ptId = "%AID%_PT-$($effectNameClean)$($paramNameClean)"
      $paramNameWithEffect = "$($param.Name) ($($effect.NameDE))"

      $xml += "              <ParameterType Id=`"$ptId`" Name=`"$paramNameWithEffect`">"

      # Map ParameterType enum to XML type
      $paramTypeEnum = if ($param.Type) { $param.Type } else { "PARAM_UINT8" }

      switch -Regex ($paramTypeEnum) {
        'PARAM_UINT8' {
          $xml += "                <TypeNumber SizeInBit=`"8`" Type=`"unsignedInt`" minInclusive=`"$($param.Min)`" maxInclusive=`"$($param.Max)`"/>"
        }
        'PARAM_HUE' {
          $xml += '                <TypeNumber SizeInBit="8" Type="unsignedInt" minInclusive="0" maxInclusive="255"/>'
        }
        'PARAM_PERCENT' {
          $xml += '                <TypeNumber SizeInBit="8" Type="unsignedInt" minInclusive="0" maxInclusive="100"/>'
        }
        'PARAM_BOOL' {
          $xml += '                <TypeRestriction Base="Value" SizeInBit="1">'
          $xml += '                  <Enumeration Text="Aus" Value="0" Id="%ENID%"/>'
          $xml += '                  <Enumeration Text="Ein" Value="1" Id="%ENID%"/>'
          $xml += '                </TypeRestriction>'
        }
        'PARAM_STRING' {
          # Real ETS text field: 14 chars (112 bit), same size as PT-CueText / Scene EffectText
          $xml += '                <TypeText SizeInBit="112" />'
        }
        'PARAM_ENUM' {
          # Generate enumeration from extracted enum options
          if ($param.EnumOptions -and $param.EnumOptions.Count -gt 0) {
            $xml += '                <TypeRestriction Base="Value" SizeInBit="8">'
            foreach ($enumOption in $param.EnumOptions) {
              $xml += "                  <Enumeration Text=`"$($enumOption.Name)`" Value=`"$($enumOption.Value)`" Id=`"%ENID%`"/>"
            }
            $xml += '                </TypeRestriction>'
          }
          else {
            # Fallback: no enum options found, use generic UINT8
            $xml += "                <TypeNumber SizeInBit=`"8`" Type=`"unsignedInt`" minInclusive=`"$($param.Min)`" maxInclusive=`"$($param.Max)`"/>"
          }
        }
        default {
          # Fallback: UINT8
          $xml += "                <TypeNumber SizeInBit=`"8`" Type=`"unsignedInt`" minInclusive=`"$($param.Min)`" maxInclusive=`"$($param.Max)`"/>"
        }
      }

      $xml += '              </ParameterType>'
    }
  }

  return $xml -join "`n"
}

function Update-NeoEffModule {
  param(
    [string]$NeoPixelXmlPath,
    [bool]$Enable
  )

  $content = Get-Content $NeoPixelXmlPath -Raw -Encoding UTF8

  if ($Enable) {
    Write-Host "  • Adding NEOEFF module to NeoPixel.xml..." -ForegroundColor Cyan

    $neoeffModule = @"

  <!-- Effect Parameter Types - MUST be loaded BEFORE NEO module! -->
  <op:define prefix="NEOEFF" ModuleType="$($script:Config.ModuleType)"
    share="NeoPixel.Effects.ParameterTypes.generated.xml"
    configTransferName="NeoPixel Effect Parameters">
  </op:define>
"@

    # Insert NEOEFF module between markers
    $content = $content -replace `
      "(?s)($([regex]::Escape($script:Config.Markers.ModuleStart)).*?<!-- DO NOT REMOVE THIS MARKER[^>]*-->).*?($([regex]::Escape($script:Config.Markers.ModuleEnd)))", `
      "`$1$neoeffModule`n  `$2"
  }
  else {
    Write-Host "  • Removing NEOEFF module from NeoPixel.xml..." -ForegroundColor Yellow

    # Clear content between markers
    $content = $content -replace `
      "(?s)($([regex]::Escape($script:Config.Markers.ModuleStart)).*?<!-- DO NOT REMOVE THIS MARKER[^>]*-->).*?($([regex]::Escape($script:Config.Markers.ModuleEnd)))", `
      "`$1`n  `$2"
  }

  Set-Content -Path $NeoPixelXmlPath -Value $content -NoNewline -Encoding UTF8
}

function Test-OpenKNXproducer {
  param(
    [string]$WorkingDir = ".",
    [string]$HeaderFile = "include/knxprod.h",
    [string]$SourceDir = "src/NeoPixel"
  )

  Write-Host ""
  Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
  Write-Host "  Verifying with OpenKNXproducer" -ForegroundColor Cyan
  Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
  # Priority:
  #   1) $env:OPENKNXPRODUCER_PATH (explicit)
  #   2) $HOME/bin/OpenKNXproducer.exe (matches VS Code task "~/bin/OpenKNXproducer.exe")
  #   3) <repo-root>/OpenKNXproducer.exe
  #   4) PATH: OpenKNXproducer(.exe)
  $candidates = @()

  if ($env:OPENKNXPRODUCER_PATH) { $candidates += $env:OPENKNXPRODUCER_PATH }

  $homeExe = Join-Path $HOME "bin/OpenKNXproducer.exe"
  $candidates += $homeExe

  # repo-root is usually $WorkingDir; also try current script working directory
  $candidates += (Join-Path $WorkingDir "OpenKNXproducer.exe")
  $candidates += (Join-Path (Get-Location).Path "OpenKNXproducer.exe")

  $openKnxExe = $null
  foreach ($c in $candidates) {
    if ($c -and (Test-Path -LiteralPath $c)) { $openKnxExe = (Resolve-Path -LiteralPath $c).Path; break }
  }
  if (-not $openKnxExe) {
    $cmd = Get-Command OpenKNXproducer, OpenKNXproducer.exe -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($cmd) { $openKnxExe = $cmd.Source }
  }

  if (-not $openKnxExe) {
    throw "OpenKNXproducer not found. Set OPENKNXPRODUCER_PATH or place it at $homeExe or add it to PATH."
  }
  Write-Host ""
  Write-Host "  ▸ Executable: $openKnxExe" -ForegroundColor DarkGray
  Write-Host "  ▸ Command: $openKnxExe create --Debug -h $HeaderFile $SourceDir" -ForegroundColor DarkGray
  Write-Host ""
  Write-Host "  ▶ Running OpenKNXproducer (output below)..." -ForegroundColor Gray
  Write-Host "    Note: Producer may hang on critical errors - check output carefully" -ForegroundColor Yellow
  Write-Host "  " -NoNewline
  Write-Host ("═" * 114) -ForegroundColor DarkGray

  # Run OpenKNXproducer and capture output to detect known vs unknown errors
  try {
    Push-Location $WorkingDir

    Write-Information "Starting OpenKNXproducer in directory: $WorkingDir with Argutments: create --Debug -h $HeaderFile $SourceDir" -InformationAction Continue

    # Capture output to temp file while also showing it
    $tempOut = [System.IO.Path]::GetTempFileName()
    $tempErr = [System.IO.Path]::GetTempFileName()
    $process = Start-Process -FilePath $openKnxExe `
      -ArgumentList "create", "--Debug", "-h", $HeaderFile, $SourceDir `
      -NoNewWindow -Wait -PassThru `
      -RedirectStandardOutput $tempOut `
      -RedirectStandardError $tempErr

    $exitCode = $process.ExitCode
    $stdoutContent = Get-Content $tempOut -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
    $stderrContent = Get-Content $tempErr -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
    Remove-Item $tempOut -Force -ErrorAction SilentlyContinue
    Remove-Item $tempErr -Force -ErrorAction SilentlyContinue

    # Combine stdout and stderr for analysis
    $outputParts = @()
    if ($stdoutContent) { $outputParts += $stdoutContent }
    if ($stderrContent) { $outputParts += $stderrContent }
    $capturedOutput = $outputParts -join "`n"

    # Show captured output
    if ($capturedOutput) {
      Write-Host $capturedOutput
    }

    Pop-Location

    Write-Host "  " -NoNewline
    Write-Host ("═" * 114) -ForegroundColor DarkGray
    Write-Host ""

    if ($exitCode -eq 0) {
      Write-Host "  " -NoNewline
      Write-Host "OK" -NoNewline -ForegroundColor Green
      Write-Host " OpenKNXproducer succeeded (Exit Code: 0)" -ForegroundColor White
      return @{
        Success  = $true
        ExitCode = $exitCode
        Output   = $capturedOutput
      }
    }
    else {
      # Non-zero exit code - check if warnings-only or real errors
      $outputLines = if ($capturedOutput) { $capturedOutput -split "`n" } else { @() }
      $duplicateErrors = $outputLines | Where-Object { $_ -match "^\s*-->" -and $_ -match "duplicate Id" }
      $headerWritten = $outputLines | Where-Object { $_ -match "Writing header file|Unchanged header file" }
      $ptNotDeclared = $outputLines | Where-Object { $_ -match "ParameterType.*was not declared" }
      $helpContextMissing = $outputLines | Where-Object { $_ -match "^\s*-->.*HelpContext.*not found in HelpContext baggage" }

      if ($duplicateErrors.Count -eq 0 -and $ptNotDeclared.Count -eq 0 -and $headerWritten) {
        # Only non-fatal warnings (e.g. missing HelpContext files) - treat as success
        $warnings = @()
        if ($helpContextMissing.Count -gt 0) { $warnings += "$($helpContextMissing.Count) missing HelpContext file(s)" }
        $warningText = if ($warnings.Count -gt 0) { " - warnings: $($warnings -join ', ')" } else { "" }
        Write-Host "  " -NoNewline
        Write-Host "OK" -NoNewline -ForegroundColor Green
        Write-Host " OpenKNXproducer completed (Exit Code: $exitCode$warningText)" -ForegroundColor Yellow
        return @{
          Success  = $true
          ExitCode = $exitCode
          Output   = $capturedOutput
        }
      }
      else {
        Write-Host "  ERROR OpenKNXproducer failed with exit code $exitCode" -ForegroundColor Red
        if ($ptNotDeclared.Count -gt 0) {
          Write-Host "  Found $($ptNotDeclared.Count) undeclared ParameterType error(s)" -ForegroundColor Red
        }
        if ($duplicateErrors.Count -gt 0) {
          Write-Host "  Found $($duplicateErrors.Count) duplicate ID error(s)" -ForegroundColor Red
        }
        return @{
          Success  = $false
          ExitCode = $exitCode
          Output   = $capturedOutput
        }
      }
    }
  }
  catch {
    Pop-Location
    Write-Host "  ERROR Failed to run OpenKNXproducer: $_" -ForegroundColor Red
    return @{
      Success  = $false
      ExitCode = -1
      Output   = $_.Exception.Message
    }
  }
}

function Clear-MarkerRegions {
  param(
    [string]$TemplatePath,
    [string]$NeoPixelXmlPath
  )

  Write-Host "  • Clearing marker regions (keeping markers intact)..." -ForegroundColor Yellow

  $content = Get-Content $TemplatePath -Raw -Encoding UTF8

  # Clear Union Parameters region
  $content = $content -replace `
    "(?s)($([regex]::Escape($script:Config.Markers.UnionStart)).*?<!-- Start ID: \d+, Start Offset: \d+ \(FIXED[^)]*\) -->.*?<!-- Generated:[^>]*-->).*?($([regex]::Escape($script:Config.Markers.UnionEnd)))", `
    "`$1`n`$2"

  # Clear ParameterRefs region
  $content = $content -replace `
    "(?s)($([regex]::Escape($script:Config.Markers.ParamRefsStart)).*?<!-- Generated:[^>]*-->).*?($([regex]::Escape($script:Config.Markers.ParamRefsEnd)))", `
    "`$1`n`$2"

  # Clear Dynamic UI region
  $content = $content -replace `
    "(?s)($([regex]::Escape($script:Config.Markers.DynamicUIStart)).*?<!-- Effect-specific[^>]*-->).*?($([regex]::Escape($script:Config.Markers.DynamicUIEnd)))", `
    "`$1`n`$2"

  Set-Content -Path $TemplatePath -Value $content -Encoding UTF8 -NoNewline
  Write-Host "    OK Marker regions cleared (markers preserved)" -ForegroundColor Green

  # Clear NEOEFF module
  Update-NeoEffModule -NeoPixelXmlPath $NeoPixelXmlPath -Enable $false
  Write-Host "    OK NEOEFF module removed" -ForegroundColor Green

  # Clear Effect ParameterTypes in share XML
  $shareXmlPath = Resolve-RepoPath $script:Config.ShareXml
  if (Test-Path $shareXmlPath) {
    $shareContent = Get-Content $shareXmlPath -Raw -Encoding UTF8
    $shareContent = $shareContent -replace `
      "(?s)($([regex]::Escape($script:Config.Markers.EffectPTStart))).*?($([regex]::Escape($script:Config.Markers.EffectPTEnd)))", `
      "`$1`n              `$2"
    Set-Content -Path $shareXmlPath -Value $shareContent -Encoding UTF8 -NoNewline
    Write-Host "    OK Effect ParameterTypes cleared from share XML" -ForegroundColor Green
  }
}

function Generate-UnionParameters {
  param(
    [array]$Effects,
    [int]$StartId = $script:Config.EffectParameterStartId,
    [int]$StartOffset = $script:Config.EffectParameterStartOffset
  )

  Write-Host "    ▸ Generating Union Parameters (Start ID=$StartId, Offset=$StartOffset)..." -ForegroundColor DarkGray
  Write-ScriptVerbose "Generating union parameters for $($Effects.Count) effects"

  # Generate Parameter definitions for Union block (Static section)
  # ALL parameters must be defined statically - UI visibility controlled in Dynamic section

  $xml = @()
  $xml += ''

  if ($Effects.Count -eq 0 -or ($Effects | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum -eq 0) {
    $xml += '                <!-- No effect-specific parameters found -->'
    Write-ScriptVerbose "No effect parameters to generate"
    return @{
      Xml        = ($xml -join "`n")
      NextId     = $StartId
      NextOffset = $StartOffset
    }
  }

  $currentId = $StartId
  $currentOffset = $StartOffset

  Write-ScriptVerbose "Starting ID: $currentId, Starting offset: $currentOffset"

  foreach ($effect in $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID) {
    $xml += "                <!-- $($effect.NameDE) Effect (ID $($effect.EffectID)) Parameters -->"
    Write-ScriptVerbose "  Processing effect: $($effect.NameDE) (ID $($effect.EffectID), $($effect.Parameters.Count) params)" "DarkGray"

    foreach ($param in $effect.Parameters) {
      # Parameter Type ID format: %AID%_PT-{EffectName}{ParamName}
      $effectNameClean = $effect.NameDE -replace $script:Config.CleanPatterns.AlphanumericOnly, ''
      $paramNameClean = $param.Name -replace $script:Config.CleanPatterns.AlphanumericOnly, ''
      $ptId = "%AID%_PT-$($effectNameClean)$($paramNameClean)"

      # Format ID with leading zeros (073, 074, etc.)
      $paramIdNum = "{0:D3}" -f $currentId
      $paramId = "%AID%_UP-%TT%%CC%$paramIdNum"

      # Name attribute: Unique per channel and effect
      $paramName = "NEO%C%$($effectNameClean)$($paramNameClean)"

      # Determine BitOffset based on parameter type
      $bitOffset = if ($param.Type -eq "PARAM_BOOL") { 7 } else { 0 }

      # PARAM_STRING occupies 14 bytes (TypeText 112 bit), everything else 1 byte
      $paramByteSize = if ($param.Type -eq 'PARAM_STRING') { 14 } else { 1 }

      # Preserve static scene/hcl layout by spilling dynamic params after reserved area.
      # A param must not start in, nor extend into, the reserved area.
      if (($currentOffset + $paramByteSize - 1) -ge $script:Config.EffectReservedStartOffset -and
        $currentOffset -le $script:Config.EffectReservedEndOffset) {
        $currentOffset = $script:Config.EffectSpilloverStartOffset
      }

      # STRING params use empty string default (Value attribute is text)
      $paramValue = if ($param.Type -eq 'PARAM_STRING') { '' } else { $param.Default }

      # Generate Parameter element
      $xml += "                <Parameter Id=`"$paramId`" Offset=`"$currentOffset`" BitOffset=`"$bitOffset`" Name=`"$paramName`" ParameterType=`"$ptId`" Text=`"$($param.Name) ($($effect.NameDE))`" Value=`"$paramValue`"/>"
      Write-ScriptVerbose "    Param ID ${currentId} @ offset ${currentOffset}: $($param.Name)" "DarkGray"
      Write-Host "      + Param ${paramIdNum}: $($param.Name) @ Offset $currentOffset ($paramByteSize byte)" -ForegroundColor DarkGray

      $currentId++
      $currentOffset += $paramByteSize
    }
    $xml += ''
  }

  Write-Host "    OK Generated $($currentId - $StartId) parameters" -ForegroundColor Green

  return @{
    Xml        = ($xml -join "`n")
    NextId     = $currentId
    NextOffset = $currentOffset
  }
}

function Generate-ParameterRefs {
  param(
    [array]$Effects,
    [int]$StartId
  )

  Write-Host "    ▸ Generating ParameterRefs (Start ID=$StartId)..." -ForegroundColor DarkGray

  # Generate ParameterRef definitions (Static section)
  # ALL parameters must have refs - UI visibility controlled in Dynamic section

  $xml = @()
  $xml += ''

  if ($Effects.Count -eq 0 -or ($Effects | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum -eq 0) {
    $xml += '              <!-- No effect-specific parameter refs found -->'
    return ($xml -join "`n")
  }

  $currentId = $StartId

  foreach ($effect in $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID) {
    $xml += "              <!-- $($effect.NameDE) Effect ParameterRefs -->"

    foreach ($param in $effect.Parameters) {
      $paramIdNum = "{0:D3}" -f $currentId
      # Default-Override am Ref noetig: die Effekt-Params liegen in einer Union, das
      # Union-Byte kann nur EINEN Default tragen -> ohne Value= zeigt ETS 0/min.
      # (gleiches Muster wie Szene/Cue, siehe Generate-SceneEffectParameterRefs)
      $defaultValue = if ($param.Type -eq 'PARAM_STRING') { '' } else { $param.Default }
      $xml += "              <ParameterRef Id=`"%AID%_UP-%TT%%CC%${paramIdNum}_R-%TT%%CC%${paramIdNum}01`" RefId=`"%AID%_UP-%TT%%CC%$paramIdNum`" Value=`"$defaultValue`" />"

      $currentId++
    }
  }

  Write-Host "    OK Generated $($currentId - $StartId) parameter refs" -ForegroundColor Green

  return ($xml -join "`n")
}

function Generate-DynamicChoose {
  param(
    [array]$Effects,
    [int]$StartId
  )

  Write-Host "    ▸ Generating Dynamic UI (Start ID=$StartId)..." -ForegroundColor DarkGray

  # Generate choose/when blocks for Dynamic section (UI visibility)
  # Controls which parameters are shown based on selected effect type

  $xml = @()
  $xml += ''

  if ($Effects.Count -eq 0 -or ($Effects | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum -eq 0) {
    $xml += '                    <!-- No effect-specific parameters for dynamic UI -->'
    return ($xml -join "`n")
  }

  # Headline NUR innerhalb der <when> (= nur Effekte mit Params), sonst bleibt sie bei Solid & Co. leer stehen
  $xml += '                    <choose ParamRefId="%AID%_UP-%TT%%CC%057_R-%TT%%CC%05701">'

  $currentId = $StartId

  foreach ($effect in $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID) {
    $xml += "                      <when test=`"$($effect.EffectID)`">"
    $xml += "                        <!-- $($effect.NameDE) Effect Parameters -->"
    $xml += '                        <ParameterSeparator Id="%AID%_PS-nnn" Text="Effekt-spezifische Parameter" UIHint="Headline"/>'

    foreach ($param in $effect.Parameters) {
      $paramIdNum = "{0:D3}" -f $currentId
      $xml += "                        <ParameterRefRef RefId=`"%AID%_UP-%TT%%CC%${paramIdNum}_R-%TT%%CC%${paramIdNum}01`" IndentLevel=`"2`" HelpContext=`"%DOC%`"/>"
      $currentId++
    }

    $xml += '                      </when>'
  }

  $xml += '                    </choose>'
  $xml += ''

  $effectsWithParamsCount = ($Effects | Where-Object { $_.Parameters.Count -gt 0 }).Count
  Write-Host "    OK Generated dynamic UI for $effectsWithParamsCount effect(s)" -ForegroundColor Green

  return ($xml -join "`n")
}

function Generate-HelpFiles {
  param([array]$Effects)

  $helpDir = Resolve-RepoPath $script:Config.HelpDir

  # Create help directory if it doesn't exist
  if (-not (Test-Path $helpDir)) {
    New-Item -ItemType Directory -Path $helpDir -Force | Out-Null
  }

  $createdFiles = @()

  foreach ($effect in $Effects) {
    if ($effect.Parameters.Count -eq 0) { continue }

    foreach ($param in $effect.Parameters) {
      # HelpContext format: NEO-{ParamName}-{EffectName}.md
      # Must match Text attribute: "ParamName (EffectName)"
      # OpenKNXproducer derives: NEO-ParamName-EffectName
      # IMPORTANT: OpenKNXproducer replaces spaces with hyphens, so we must do the same
      $effectNameCleaned = $effect.NameDE -replace $script:Config.CleanPatterns.SpaceToHyphen, '-'  # Space → Hyphen
      $paramNameCleaned = $param.Name -replace $script:Config.CleanPatterns.SpaceToHyphen, '-'            # Space → Hyphen
      $helpFileName = "$($script:Config.HelpFilePrefix)$paramNameCleaned-$effectNameCleaned$($script:Config.HelpFileSuffix)"
      $helpFilePath = Join-Path $helpDir $helpFileName

      # Skip if help file already exists
      if (Test-Path $helpFilePath) {
        continue
      }

      # Generate help content
      $helpContent = @()
      $helpContent += "# $($param.Name)"
      $helpContent += ""
      $helpContent += "**Effekt:** $($effect.NameDE)"
      $helpContent += ""
      if ($param.Description) {
        $helpContent += $param.Description
      }
      else {
        $helpContent += "Parameter zur Steuerung von '$($param.Name)' für den $($effect.NameDE)-Effekt."
      }
      $helpContent += ""
      $helpContent += "**Wertebereich:** $($param.Min) - $($param.Max)"
      # ETS6 renders a single newline as a soft break (= space), so two adjacent label
      # lines collapse onto one line. A blank line forces a real line break.
      $helpContent += ""
      $helpContent += "**Standardwert:** $($param.Default)"

      Set-Content -Path $helpFilePath -Value ($helpContent -join "`n") -Encoding UTF8

      # Track created file with source info
      $source = if ($param.Description) { "aus Header" } else { "Platzhalter" }
      $createdFiles += [PSCustomObject]@{
        FileName = $helpFileName
        Source   = $source
      }
    }
  }

  # Show warning if files were created
  if ($createdFiles.Count -gt 0) {
    Write-Host ""
    Write-Host "INFO: " -NoNewline -ForegroundColor Yellow
    Write-Host "$($createdFiles.Count) help file(s) newly created" -ForegroundColor White
    Write-Host "  " -NoNewline
    Write-Host ("─" * 116) -ForegroundColor DarkGray

    # Group by source
    $fromHeader = $createdFiles | Where-Object { $_.Source -eq "aus Header" }
    $fromPlaceholder = $createdFiles | Where-Object { $_.Source -eq "Platzhalter" }

    if ($fromHeader.Count -gt 0) {
      Write-Host "    With description from header ($($fromHeader.Count)):" -ForegroundColor Green
      foreach ($file in $fromHeader) {
        Write-Host "      • $($file.FileName)" -ForegroundColor DarkGray
      }
    }

    if ($fromPlaceholder.Count -gt 0) {
      Write-Host "    With placeholder text ($($fromPlaceholder.Count)) - please fill in:" -ForegroundColor Yellow
      foreach ($file in $fromPlaceholder) {
        Write-Host "      • $($file.FileName)" -ForegroundColor DarkGray
      }
    }

    Write-Host "  " -NoNewline
    Write-Host ("─" * 116) -ForegroundColor DarkGray
    Write-Host "  Help texts are displayed in ETS for the parameters." -ForegroundColor Gray
    Write-Host "  Edit the .md files directly - they will not be overwritten on next run." -ForegroundColor Gray
    Write-Host ""
  }

  # Return info for summary: count and list of effects without description
  $effectsWithoutDesc = @()
  foreach ($effect in $Effects) {
    # Check if any parameter has placeholder
    $hasPlaceholder = $false
    foreach ($param in $effect.Parameters) {
      if (-not $param.Description) {
        $hasPlaceholder = $true
        break
      }
    }
    if ($hasPlaceholder) {
      $effectsWithoutDesc += $effect.NameDE
    }
  }

  return @{
    Count                     = $createdFiles.Count
    EffectsWithoutDescription = $effectsWithoutDesc
  }
}

function Generate-EffectIdReferenceHelp {
  param([array]$Effects)

  $helpDir = Resolve-RepoPath $script:Config.HelpDir
  if (-not (Test-Path $helpDir)) {
    New-Item -ItemType Directory -Path $helpDir -Force | Out-Null
  }

  $fileName = "NEO-Effekt-ID-Referenz.md"
  $filePath = Join-Path $helpDir $fileName

  $content = @()
  $content += "# Effekt-ID-Referenz"
  $content += ""
  $content += "Diese Datei wird automatisch durch Build-EffectParameters.ps1 erzeugt."
  $content += "Die IDs in dieser Liste sind die verbindliche Zuordnung in ETS/KO (Runtime-IDs)."
  $content += ""
  $content += "Format pro Zeile: ID - DE - EN"
  $content += ""

  foreach ($effect in ($Effects | Sort-Object EffectID)) {
    $nameDE = if ($effect.NameDE) { $effect.NameDE } else { $effect.Name }
    $nameEN = if ($effect.NameEN) { $effect.NameEN } else { $nameDE }
    $content += "- $($effect.EffectID) - $nameDE - $nameEN"
  }

  Set-Content -Path $filePath -Value ($content -join "`n") -Encoding UTF8
  return $fileName
}

function Get-EffectDescriptionDE {
  <#
    .SYNOPSIS
        Read the German half of EFFECT_DESC_DE_EN() from an effect header.
  #>
  param([string]$ClassName)

  $effectsDir = Resolve-RepoPath "lib/OFM-NeoPixel/src/effects"
  if (-not (Test-Path $effectsDir)) { return "" }

  # The file name does not follow the class name everywhere (SolidEffect lives in
  # EffectSolid.h), so locate the header by the class it declares.
  if (-not $script:EffectHeaderByClass) {
    $script:EffectHeaderByClass = @{}
    foreach ($file in (Get-ChildItem -Path $effectsDir -Filter "*.h" -File)) {
      $text = Get-Content $file.FullName -Raw -Encoding UTF8
      foreach ($hit in [regex]::Matches($text, 'class\s+([A-Za-z0-9_]+)\s*:')) {
        if (-not $script:EffectHeaderByClass.ContainsKey($hit.Groups[1].Value)) {
          $script:EffectHeaderByClass[$hit.Groups[1].Value] = $file.FullName
        }
      }
    }
  }

  if ($script:EffectHeaderByClass.ContainsKey($ClassName)) {
    $content = Get-Content $script:EffectHeaderByClass[$ClassName] -Raw -Encoding UTF8
    # C++ splits long texts into adjacent string literals - join them before use.
    if ($content -match '(?s)EFFECT_DESC_DE_EN\s*\(\s*((?:"(?:[^"\\]|\\.)*"\s*)+)') {
      $joined = ""
      foreach ($piece in [regex]::Matches($Matches[1], '"((?:[^"\\]|\\.)*)"')) {
        $joined += $piece.Groups[1].Value
      }
      return ($joined -replace '\\"', '"').Trim().TrimEnd('.')
    }
    # Eleven effects still return a plain English string instead of EFFECT_DESC_DE_EN.
    # Use it so no row stays empty; the caller reports them.
    if ($content -match '(?s)getDescription\s*\([^)]*\)[^{]*\{\s*return\s+((?:"(?:[^"\\]|\\.)*"\s*)+)') {
      $joined = ""
      foreach ($piece in [regex]::Matches($Matches[1], '"((?:[^"\\]|\\.)*)"')) {
        $joined += $piece.Groups[1].Value
      }
      return ($joined -replace '\\"', '"').Trim().TrimEnd('.')
    }
  }
  return ""
}

function Test-HasGermanDescription {
  param([string]$ClassName)
  if (-not $script:EffectHeaderByClass) { [void](Get-EffectDescriptionDE -ClassName $ClassName) }
  if (-not $script:EffectHeaderByClass.ContainsKey($ClassName)) { return $false }
  $content = Get-Content $script:EffectHeaderByClass[$ClassName] -Raw -Encoding UTF8
  return ($content -match 'EFFECT_DESC_DE_EN')
}

function Get-EffectPoolGroups {
  <#
    .SYNOPSIS
        Map each effect class to 'standard' or 'extended' by whether EffectPool.cpp
        includes it inside the NEOPIXEL_MINIMAL_EFFECTS guard.
  #>
  $poolPath = Resolve-RepoPath "lib/OFM-NeoPixel/src/effects/EffectPool.cpp"
  $groups = @{}
  if (-not (Test-Path $poolPath)) { return $groups }

  $depth = 0
  $inMinimal = $false
  $minimalDepth = 0
  foreach ($line in (Get-Content $poolPath -Encoding UTF8)) {
    if ($line -match '^\s*#\s*if') {
      $depth++
      if ($line -match 'NEOPIXEL_MINIMAL_EFFECTS' -and -not $inMinimal) {
        $inMinimal = $true
        $minimalDepth = $depth
      }
    }
    elseif ($line -match '^\s*#\s*endif') {
      if ($inMinimal -and $depth -eq $minimalDepth) { $inMinimal = $false }
      $depth--
    }
    elseif ($line -match '#\s*include\s+"([A-Za-z0-9_]+)\.h"') {
      $groups[$Matches[1]] = if ($inMinimal) { 'extended' } else { 'standard' }
    }
  }
  return $groups
}

function Generate-EffectTypeHelpTable {
  <#
    .SYNOPSIS
        Fill the effect table in NEO-Effekt-Typ.md from the effect headers.
    .DESCRIPTION
        The table used to be maintained by hand and had drifted badly: wrong IDs from an
        older numbering, effects that no longer exist, and five that were missing.
  #>
  param([array]$Effects)

  $helpDir = Resolve-RepoPath $script:Config.HelpDir
  $filePath = Join-Path $helpDir "NEO-Effekt-Typ.md"
  if (-not (Test-Path $filePath)) { return $null }

  $groups = Get-EffectPoolGroups
  $rows = @{ standard = @(); extended = @(); twoD = @() }
  $missingDesc = @()

  foreach ($effect in ($Effects | Sort-Object { [int]$_.EffectID })) {
    $nameDE = if ($effect.NameDE) { $effect.NameDE } else { $effect.Name }
    $desc = Get-EffectDescriptionDE -ClassName $effect.ClassName
    if (-not (Test-HasGermanDescription -ClassName $effect.ClassName)) { $missingDesc += $nameDE }
    $row = "{0,2} | {1,-21} | {2}" -f [int]$effect.EffectID, $nameDE, $desc

    if (Test-Is2DEffect -ClassName $effect.ClassName -NameDE $effect.NameDE -NameEN $effect.NameEN) {
      $rows.twoD += $row
    }
    elseif ($groups[$effect.ClassName] -eq 'extended') { $rows.extended += $row }
    else { $rows.standard += $row }
  }

  $header = "ID | Name                  | Beschreibung"
  $rule = "---+-----------------------+--------------------------------------------------------"

  $out = @()
  $out += "## Verfügbare Effekte"
  $out += ""
  $out += "### Standard-Effekte (immer verfügbar)"
  $out += ""
  $out += '```'
  $out += $header
  $out += $rule
  $out += $rows.standard
  $out += '```'
  $out += ""
  $out += "### Erweiterte Effekte (deaktivierbar mit ``NEOPIXEL_MINIMAL_EFFECTS``)"
  $out += ""
  $out += '```'
  $out += $header
  $out += $rule
  $out += $rows.extended
  $out += '```'
  $out += ""
  $out += "### 2D-Effekte (erfordern Segment mit Matrix-Geometrie)"
  $out += ""
  $out += '```'
  $out += $header
  $out += $rule
  $out += $rows.twoD
  $out += '```'
  $out += ""
  $maxId = ($Effects | ForEach-Object { [int]$_.EffectID } | Measure-Object -Maximum).Maximum
  $out += "**Gesamt: $($Effects.Count) Effekte (ID 0-$maxId)**"

  $body = ($out -join "`n")
  $content = Get-Content $filePath -Raw -Encoding UTF8
  $pattern = '(?s)(<!-- BEGIN AUTO-GENERATED: Effect Table - DO NOT EDIT MANUALLY -->).*?(<!-- END AUTO-GENERATED: Effect Table -->)'
  if ($content -notmatch $pattern) { return $null }
  $updated = $content -replace $pattern, "`$1`n$body`n`$2"
  Set-Content -Path $filePath -Value $updated -Encoding UTF8 -NoNewline

  return @{
    Standard    = $rows.standard.Count
    Extended    = $rows.extended.Count
    TwoD        = $rows.twoD.Count
    MissingDesc = $missingDesc
  }
}

function Generate-EffectFlagList {
  <#
    .SYNOPSIS
        Fill the effect-flag list in platformio.custom.ini.
    .DESCRIPTION
        The list used to be maintained by hand and had drifted: it named four flags that
        no longer exist (CONFETTI, GARAGEDOOR, METEOR, TWINKLE) and left out nineteen that
        do. The flag name is derived exactly as the EffectTypeMapping.h guards derive it.
  #>
  param([array]$Effects)

  $iniPath = Resolve-RepoPath "platformio.custom.ini"
  if (-not (Test-Path $iniPath)) { return $null }

  $groups = Get-EffectPoolGroups
  $standard = @(); $extended = @(); $twoD = @()

  foreach ($effect in ($Effects | Sort-Object { [int]$_.EffectID })) {
    $methodName = $effect.ClassName -replace 'Effect', ''
    if ($methodName -eq "Solid") { continue }
    $nameDE = if ($effect.NameDE) { $effect.NameDE } else { $effect.Name }
    $entry = ";   -D {0,-34} ; {1}" -f "NEOPIXEL_DISABLE_$($methodName.ToUpper())", $nameDE

    if (Test-Is2DEffect -ClassName $effect.ClassName -NameDE $effect.NameDE -NameEN $effect.NameEN) { $twoD += $entry }
    elseif ($groups[$effect.ClassName] -eq 'extended') { $extended += $entry }
    else { $standard += $entry }
  }

  $out = @()
  $out += "; All $($Effects.Count) effects are built in unless a flag below leaves one out."
  $out += "; Solid is the fallback for an unknown selection and cannot be disabled."
  $out += ";"
  $out += "; --- Leave out every 2D effect at once ($($twoD.Count) effects) ---"
  $out += "; Only worth it where the device drives plain strips; ETS then hides them too."
  $out += ";  -D NEOPIXEL_DISABLE_2D"
  $out += ";"
  $out += "; --- Leave out the extended effects at once ($($extended.Count) effects) ---"
  $out += ";  -D NEOPIXEL_MINIMAL_EFFECTS"
  $out += ";"
  $out += "; --- Or name single effects ---"
  $out += "; Always available ($($standard.Count)):"
  $out += $standard
  $out += "; Extended ($($extended.Count)), all covered by NEOPIXEL_MINIMAL_EFFECTS:"
  $out += $extended
  $out += "; 2D ($($twoD.Count)), all covered by NEOPIXEL_DISABLE_2D:"
  $out += $twoD

  $body = ($out -join "`n")
  $content = Get-Content $iniPath -Raw -Encoding UTF8
  $pattern = '(?s)(; BEGIN AUTO-GENERATED: Effect Flags - DO NOT EDIT MANUALLY).*?(; END AUTO-GENERATED: Effect Flags)'
  if ($content -notmatch $pattern) { return $null }
  $updated = $content -replace $pattern, "`$1`n$body`n`$2"
  Set-Content -Path $iniPath -Value $updated -Encoding UTF8 -NoNewline

  return @{ Standard = $standard.Count; Extended = $extended.Count; TwoD = $twoD.Count }
}

function Generate-SceneHelpFiles {
  param([array]$Effects)

  $helpDir = Resolve-RepoPath $script:Config.HelpDir
  if (-not (Test-Path $helpDir)) {
    New-Item -ItemType Directory -Path $helpDir -Force | Out-Null
  }

  $sceneCount = $script:Config.SceneInstances
  $createdFiles = @()

  # Fixed scene parameters (from NeoPixel.Scene.part.xml Text attributes)
  # OpenKNXproducer derives HelpContext from Text: "Effekt Typ (Szene 1)" → NEO-Effekt-Typ-Szene-1
  $fixedParams = @(
    @{ Name = "Effekt-Typ"; Desc = "Wählt den Lichteffekt für diese Szene aus." }
    @{ Name = "Primaerfarbe-RGB"; Desc = "Primärfarbe (RGB) für diese Szene." }
    @{ Name = "Primaerfarbe-Warmweiss"; Desc = "Warmweiß-Anteil der Primärfarbe." }
    @{ Name = "Primaerfarbe-Kaltweiss"; Desc = "Kaltweiß-Anteil der Primärfarbe." }
    @{ Name = "Sekundaerfarbe-RGB"; Desc = "Sekundärfarbe (RGB) für diese Szene." }
    @{ Name = "Sekundaerfarbe-Warmweiss"; Desc = "Warmweiß-Anteil der Sekundärfarbe." }
    @{ Name = "Sekundaerfarbe-Kaltweiss"; Desc = "Kaltweiß-Anteil der Sekundärfarbe." }
    @{ Name = "Helligkeit"; Desc = "Helligkeit der Szene (0-255)." }
  )

  # SceneCount (not per-scene, just one file)
  $sceneCountFile = Join-Path $helpDir "NEO-Anzahl-Szenen.md"
  if (-not (Test-Path $sceneCountFile)) {
    $content = @(
      "# Anzahl Szenen"
      ""
      "Legt fest, wie viele vorkonfigurierte Szenen für dieses Segment verfügbar sind."
      ""
      "**Wertebereich:** 0 (deaktiviert) - $sceneCount"
    )
    Set-Content -Path $sceneCountFile -Value ($content -join "`n") -Encoding UTF8
    $createdFiles += "NEO-Anzahl-Szenen.md"
  }

  # Fixed params: ONE shared help file per param (scene-independent).
  # HelpContext in NeoPixel.Scene.part.xml is "NEO-{Name}-Szene" (no scene number),
  # so a single file serves all scenes. Previously this generated NEO-{Name}-Szene-{N}
  # for every scene (~80 files) that were byte-identical except the "**Szene:** N"
  # header line - pure baggage bloat. The help text is generic across scenes anyway.
  foreach ($fp in $fixedParams) {
    $fileName = "NEO-$($fp.Name)-Szene.md"
    $filePath = Join-Path $helpDir $fileName
    if (-not (Test-Path $filePath)) {
      $content = @(
        "# $($fp.Name -replace '-', ' ')"
        ""
        $fp.Desc
      )
      Set-Content -Path $filePath -Value ($content -join "`n") -Encoding UTF8
      $createdFiles += $fileName
    }
  }

  # Effect-specific scene params intentionally get NO per-scene help files.
  # The scene dynamic UI (Generate-SceneEffectDynamicChoose, section 5b) references
  # per-effect help NEO-{ParamName}-{EffectName} (scene-independent), and the cue UI
  # (5b.1) reuses the very same files. Generating NEO-{ParamName}-Szene-{N} for every
  # param × every scene produced ~920 files that were byte-identical except the scene
  # number in the header and that NOTHING references — pure baggage bloat. Removed.
  # (The 8 fixed scene params above DO need per-scene files: their HelpContext is
  #  derived from Text "… (Szene N)" and is genuinely scene-numbered.)

  if ($createdFiles.Count -gt 0) {
    Write-Host ""
    Write-Host "INFO: " -NoNewline -ForegroundColor Yellow
    Write-Host "$($createdFiles.Count) scene help file(s) newly created" -ForegroundColor White
  }

  return @{ Count = $createdFiles.Count }
}

function Read-StartingIdsFromMarkers {
  param([string]$TemplatePath)

  Write-Host "   Reading starting IDs from template markers..." -ForegroundColor Gray

  if (-not (Test-Path $TemplatePath)) {
    throw "Template file not found: $TemplatePath"
  }

  $templateContent = Get-Content -Path $TemplatePath -Raw -Encoding UTF8

  # Extract starting ID and Offset from Union marker comments
  # Expected format: <!-- Start ID: 073, Start Offset: 30 -->
  $unionMarker = $script:Config.Markers.UnionStart

  if ($templateContent -match "(?s)$([regex]::Escape($unionMarker)).*?Start ID:\s*(\d+).*?Start Offset:\s*(\d+)") {
    $startId = [int]$Matches[1]
    $startOffset = [int]$Matches[2]

    Write-Host "    OK Union: Start ID=$startId, Start Offset=$startOffset" -ForegroundColor DarkGray

    return @{
      StartId     = $startId
      StartOffset = $startOffset
    }
  }
  else {
    throw "Could not extract starting ID/Offset from Union marker! Please ensure marker contains '<!-- Start ID: XXX, Start Offset: YYY -->'"
  }
}

function Backup-Template {
  param([string]$TemplatePath)

  $backupPath = "$TemplatePath.backup"
  Write-Host "  • Creating backup: $(Split-Path -Leaf $backupPath)" -ForegroundColor Gray

  try {
    Copy-Item -Path $TemplatePath -Destination $backupPath -Force
    Write-Host "    OK Backup created successfully" -ForegroundColor DarkGray
    return $backupPath
  }
  catch {
    Write-Warning "Failed to create backup: $_"
    return $null
  }
}

function Restore-Template {
  param(
    [string]$TemplatePath,
    [string]$BackupPath
  )

  if ($BackupPath -and (Test-Path $BackupPath)) {
    Write-Host "   Restoring template from backup..." -ForegroundColor Yellow
    Copy-Item -Path $BackupPath -Destination $TemplatePath -Force
    Write-Host "    OK Template restored" -ForegroundColor Green
  }
}

function Validate-GeneratedXml {
  param(
    [string]$TemplatePath,
    [array]$Effects,
    [int]$StartId,
    [int]$StartOffset
  )

  Write-Host "   Validating generated XML..." -ForegroundColor Cyan

  $templateContent = Get-Content -Path $TemplatePath -Raw -Encoding UTF8
  $errors = @()
  $warnings = @()

  # 1. Check all markers are present
  $markers = @(
    $script:Config.Markers.UnionStart,
    $script:Config.Markers.UnionEnd,
    $script:Config.Markers.ParamRefsStart,
    $script:Config.Markers.ParamRefsEnd,
    $script:Config.Markers.DynamicUIStart,
    $script:Config.Markers.DynamicUIEnd
  )

  foreach ($marker in $markers) {
    if ($templateContent -notmatch [regex]::Escape($marker)) {
      $errors += "Missing marker: $marker"
    }
  }

  # 2. Check for ID collisions IN PARAMETER DEFINITIONS ONLY (not refs)
  # Extract only Parameter definitions (not ParameterRef, not ParameterRefRef)
  # Pattern: <Parameter Id="%AID%_UP-%TT%%CC%(\d+)"
  $paramPattern = '<Parameter\s+Id="%AID%_UP-%TT%%CC%(\d+)"'
  $paramIds = [regex]::Matches($templateContent, $paramPattern) | ForEach-Object { [int]$_.Groups[1].Value }
  $duplicateParamIds = $paramIds | Group-Object | Where-Object { $_.Count -gt 1 }

  if ($duplicateParamIds) {
    foreach ($dup in $duplicateParamIds) {
      $errors += "Duplicate Parameter definition found: $($dup.Name) (appears $($dup.Count) times in <Parameter> tags)"
    }
  }

  # 3. Check for Offset collisions within same Union block
  # Extract offsets and bit offsets from Parameter definitions only
  $paramOffsetPattern = '<Parameter\s+[^>]*Offset="(\d+)"[^>]*BitOffset="(\d+)"'
  $offsetMatches = [regex]::Matches($templateContent, $paramOffsetPattern)
  $offsets = @{}

  foreach ($match in $offsetMatches) {
    $offset = [int]$match.Groups[1].Value
    $bitOffset = [int]$match.Groups[2].Value

    # Only check offsets in effect parameter range (30+)
    if ($offset -ge $StartOffset) {
      $key = "$offset-$bitOffset"

      if ($offsets.ContainsKey($key)) {
        $errors += "Offset collision at Offset=$offset BitOffset=$bitOffset (two parameters at same location)"
      }
      $offsets[$key] = $true
    }
  }

  # 4. Check that all Parameters have corresponding ParameterRefs
  $paramIdPattern = '<Parameter\s+Id="%AID%_UP-%TT%%CC%(\d+)"'
  $refIdPattern = '<ParameterRef\s+Id="%AID%_UP-%TT%%CC%(\d+)_R'

  $paramIds = [regex]::Matches($templateContent, $paramIdPattern) | ForEach-Object { $_.Groups[1].Value }
  $refIds = [regex]::Matches($templateContent, $refIdPattern) | ForEach-Object { $_.Groups[1].Value }

  foreach ($paramId in $paramIds) {
    if ($refIds -notcontains $paramId) {
      $warnings += "Parameter $paramId has no corresponding ParameterRef"
    }
  }

  # 5. Check that generated IDs are EXACTLY in expected range (no gaps, no overflow into buffer zone)
  # Expected: IDs 73-175 (103 parameters total)
  # Buffer Zone: IDs 176-193 MUST remain empty for future effects
  # Manual Parameters: IDs 194+ (HCL, ResetColor, etc.)
  $generatedParamCount = ($Effects | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum
  $expectedMaxId = $StartId + $generatedParamCount - 1
    
  # Get all generated IDs in the effect parameter range
  $generatedIds = $paramIds | ForEach-Object { [int]$_ } | Where-Object { $_ -ge $StartId } | Sort-Object
    
  # Check for IDs BELOW the start
  foreach ($id in $generatedIds) {
    if ($id -lt $StartId) {
      $warnings += "WARNUNG: Parameter ID $id liegt UNTER dem Start-Bereich ($StartId)!"
    }
  }
    
  # Check for IDs in the BUFFER ZONE (should be empty!)
  $bufferZoneStart = $expectedMaxId + 1
  $bufferZoneEnd = 193  # Hard-coded buffer zone limit
  foreach ($id in $generatedIds) {
    if ($id -ge $bufferZoneStart -and $id -le $bufferZoneEnd) {
      $errors += "FEHLER: Parameter ID $id liegt in der BUFFER ZONE ($bufferZoneStart-$bufferZoneEnd)! Diese Range muss für zukünftige Effekte FREI bleiben!"
    }
  }
    
  # Check for gaps in the generated range
  for ($expectedId = $StartId; $expectedId -le $expectedMaxId; $expectedId++) {
    if ($generatedIds -notcontains $expectedId) {
      $warnings += "WARNUNG: Fehlende Parameter ID: $expectedId (erwartet in Range $StartId-$expectedMaxId)"
    }
  }

  # 6. Check XML well-formedness (basic tag balance)
  $openTags = ([regex]::Matches($templateContent, '<(?!/)(?!\?)(?!--)([a-zA-Z][a-zA-Z0-9]*)')).Count
  $closeTags = ([regex]::Matches($templateContent, '</([a-zA-Z][a-zA-Z0-9]*)')).Count
  $selfClosing = ([regex]::Matches($templateContent, '/>')).Count

  $expectedBalance = $openTags - $selfClosing
  if ($expectedBalance -ne $closeTags) {
    $warnings += "XML tag balance check: Open=$openTags, Close=$closeTags, SelfClose=$selfClosing (diff=$($expectedBalance - $closeTags))"
  }

  # 7. Check that all effect IDs referenced in Dynamic UI exist
  $whenTestPattern = '<when\s+test="(\d+)">'
  $effectIdsInUI = [regex]::Matches($templateContent, $whenTestPattern) | ForEach-Object { [int]$_.Groups[1].Value } | Select-Object -Unique
  $definedEffectIds = $Effects | ForEach-Object { $_.EffectID }

  foreach ($uiEffectId in $effectIdsInUI) {
    if ($definedEffectIds -notcontains $uiEffectId) {
      $warnings += "Dynamic UI references effect ID $uiEffectId but no effect with that ID was parsed"
    }
  }

  # Report results
  Write-Host ""
  if ($errors.Count -eq 0 -and $warnings.Count -eq 0) {
    $uniqueParams = ($paramIds | Select-Object -Unique).Count
    $uniqueRefs = ($refIds | Select-Object -Unique).Count

    Write-Host "    OK All validations passed!" -ForegroundColor Green
    Write-Host "       - $generatedParamCount parameters generated (ID $StartId-$expectedMaxId)" -ForegroundColor DarkGray
    Write-Host "       - $uniqueParams unique parameter definitions" -ForegroundColor DarkGray
    Write-Host "       - $uniqueRefs unique parameter refs" -ForegroundColor DarkGray
    Write-Host "       - $($offsets.Count) offset locations used" -ForegroundColor DarkGray
    return $true
  }

  if ($errors.Count -gt 0) {
    Write-Host "    ERROR ERRORS FOUND:" -ForegroundColor Red
    foreach ($err in $errors) {
      Write-Host "       - $err" -ForegroundColor Red
    }
  }

  if ($warnings.Count -gt 0) {
    Write-Host "    ERROR️  WARNINGS:" -ForegroundColor Yellow
    foreach ($warning in $warnings) {
      Write-Host "       - $warning" -ForegroundColor Yellow
    }
  }

  return ($errors.Count -eq 0)
}

function Update-TemplateWithMarkers {
  param(
    [string]$TemplatePath,
    [string]$UnionContent,
    [string]$ParameterRefsContent,
    [string]$DynamicContent,
    [array]$Effects,
    [int]$NextId,
    [int]$NextOffset
  )

  Write-Host "  • Updating template with generated content..." -ForegroundColor Cyan

  if (-not (Test-Path $TemplatePath)) {
    throw "Template file not found: $TemplatePath"
  }

  # Create backup before modifying
  $backupPath = Backup-Template -TemplatePath $TemplatePath

  try {
    $templateContent = Get-Content -Path $TemplatePath -Raw -Encoding UTF8
    $updateCount = 0

    # Update Union Parameters block
    $beginMarker = $script:Config.Markers.UnionStart
    $endMarker = $script:Config.Markers.UnionEnd

    if ($templateContent -match "(?s)$([regex]::Escape($beginMarker)).*?$([regex]::Escape($endMarker))") {
      $replacement = @"
$beginMarker
                <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->
                <!-- Start ID: $StartId, Start Offset: $StartOffset (FIXED - do not change!) -->
                <!-- Generated: ID $StartId-$($NextId - 1), Offset $StartOffset-$($NextOffset - 1) -->
$UnionContent                $endMarker
"@
      $templateContent = $templateContent -replace "(?s)$([regex]::Escape($beginMarker)).*?$([regex]::Escape($endMarker))", $replacement
      Write-Host "    OK Updated Union Parameters block" -ForegroundColor Green
      $updateCount++
    }
    else {
      throw "Union Parameters markers not found in template! Please add markers to template."
    }

    # Update ParameterRefs block
    $beginMarker = $script:Config.Markers.ParamRefsStart
    $endMarker = $script:Config.Markers.ParamRefsEnd

    if ($templateContent -match "(?s)$([regex]::Escape($beginMarker)).*?$([regex]::Escape($endMarker))") {
      $replacement = @"
$beginMarker
              <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->
              <!-- Generated effect-specific parameter refs (ID $StartId-$($NextId - 1)) -->
$ParameterRefsContent
              $endMarker
"@
      $templateContent = $templateContent -replace "(?s)$([regex]::Escape($beginMarker)).*?$([regex]::Escape($endMarker))", $replacement
      Write-Host "    OK Updated ParameterRefs block" -ForegroundColor Green
      $updateCount++
    }
    else {
      throw "ParameterRefs markers not found in template! Please add markers to template."
    }

    # Update Dynamic UI block
    $beginMarker = $script:Config.Markers.DynamicUIStart
    $endMarker = $script:Config.Markers.DynamicUIEnd

    if ($templateContent -match "(?s)$([regex]::Escape($beginMarker)).*?$([regex]::Escape($endMarker))") {
      $replacement = @"
$beginMarker
                    <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->
                    <!-- Effect-specific conditional UI will be generated here -->
$DynamicContent                    $endMarker
"@
      $templateContent = $templateContent -replace "(?s)$([regex]::Escape($beginMarker)).*?$([regex]::Escape($endMarker))", $replacement
      Write-Host "    OK Updated Dynamic UI block" -ForegroundColor Green
      $updateCount++
    }
    else {
      throw "Dynamic UI markers not found in template! Please add markers to template."
    }

    if ($updateCount -eq 3) {
      # Save updated content
      Set-Content -Path $TemplatePath -Value $templateContent -Encoding UTF8 -NoNewline
      Write-Host "    OK Template saved successfully" -ForegroundColor Green

      # Validate the result
      $validationResult = Validate-GeneratedXml -TemplatePath $TemplatePath -Effects $Effects -StartId ($NextId - ($Effects | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum) -StartOffset ($NextOffset - ($Effects | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum)

      if ($validationResult) {
        # Remove backup after successful validation
        if ($backupPath -and (Test-Path $backupPath)) {
          Remove-Item -Path $backupPath -Force
          Write-Host "    OK Backup removed (validation passed)" -ForegroundColor DarkGray
        }
        return $true
      }
      else {
        Write-Warning "Validation failed! Restoring backup..."
        Restore-Template -TemplatePath $TemplatePath -BackupPath $backupPath
        return $false
      }
    }

    throw "Not all blocks were updated (expected 3, got $updateCount)"

  }
  catch {
    Write-Error "Error updating template: $_"
    Restore-Template -TemplatePath $TemplatePath -BackupPath $backupPath
    return $false
  }
}

function Generate-CppMapping {
  param([array]$Effects)

  $currentYear = 2026   # fixed -> deterministic output (no per-build churn); bump when needed

  $cpp = @()
  $cpp += '/**'
  $cpp += ' * @file EffectParameterMapping.h'
  $cpp += ' * @brief Effect Parameter Loading from ETS Configuration (Auto-Generated)'
  $cpp += ' *'
  $cpp += ' * This file contains the mapping logic to load effect-specific parameters from ETS'
  $cpp += ' * configuration (EEPROM) and apply them to Effect instances. Parameters are automatically'
  $cpp += ' * generated from Effect header files and synchronized with NeoPixel.Effects.*.xml definitions.'
  $cpp += ' *'
  $cpp += ' * @warning AUTO-GENERATED FILE - DO NOT EDIT MANUALLY'
  $cpp += ' * @note Auto-generated - do not edit by hand'
  $cpp += ' * @note Source: Build-EffectParameters.ps1'
  $cpp += ' *'
  $cpp += " * @copyright Copyright (c) $currentYear OpenKNX (Licensed under GNU GPL v3.0)"
  $cpp += ' */'
  $cpp += ''
  $cpp += '#ifndef EFFECT_PARAMETER_MAPPING_H'
  $cpp += '#define EFFECT_PARAMETER_MAPPING_H'
  $cpp += ''
  $cpp += '#pragma once'
  $cpp += ''
  $cpp += '#include "knxprod.h"'
  $cpp += '#include "' + $script:Config.EffectBaseHeader + '"'
  $cpp += '#include "' + $script:Config.SegmentHeader + '"'
  $cpp += ''
  $cpp += '/**'
  $cpp += ' * @brief Load effect-specific parameters from EEPROM and apply to effect instance'
  $cpp += ' * '
  $cpp += ' * This function reads effect-specific parameters that were automatically generated'
  $cpp += ' * from Effect*.h headers via Build-EffectParameters.ps1. Parameters are stored in'
  $cpp += ' * EEPROM at offset 14+ within each segment memory block.'
  $cpp += ' * '
  $cpp += ' * @param effect Effect instance to configure'
  $cpp += ' * @param segment Segment instance (for setParameter context)'
  $cpp += ' * @param effectID Effect type ID (0-21)'
  $cpp += ' * @param channelIndex Channel/Segment index (0-15) for parameter access'
  $cpp += ' */'
  $cpp += 'inline void loadEffectParameters(Effect* effect, Segment* segment, uint8_t effectID, uint8_t channelIndex)'
  $cpp += '{'
  $cpp += '    if (!effect || !segment) return;'
  $cpp += '    '
  $cpp += '    uint8_t paramCount = effect->getParameterCount();'
  $cpp += '    if (paramCount == 0) return;  // No parameters to load'
  $cpp += '    '
  $cpp += '    // Set channel index for ParamNEO_* macro expansion'
  $cpp += '    uint8_t _channelIndex = channelIndex;'
  $cpp += '    '
  $cpp += '    // Map effectID to parameter loading code'
  $cpp += '    // Macro names match OpenKNXproducer format: ParamNEO_NEO{EffectName}{ParamName}'
  $cpp += '    // (automatically derived from Parameter Name attribute in XML)'
  $cpp += '    '
  $cpp += '    switch (static_cast<PT_NEOEffectType>(effectID))'
  $cpp += '    {'

  foreach ($effect in $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID) {
    # Derive enum name from ClassName
    $enumName = $effect.ClassName -replace 'Effect$', '' -replace '^Effect', ''
    if ([string]::IsNullOrEmpty($enumName)) { $enumName = $effect.ClassName }
    $cpp += "        case PT_NEOEffectType::${enumName}:  // $($effect.NameDE) Effect"

    # Effect name for macro (remove spaces/hyphens)
    $effectNameClean = $effect.NameDE -replace $script:Config.CleanPatterns.RemoveSpacesHyphens, ''

    for ($i = 0; $i -lt $effect.Parameters.Count; $i++) {
      $param = $effect.Parameters[$i]
      $paramNameClean = $param.Name -replace $script:Config.CleanPatterns.RemoveSpacesHyphens, ''

      # Macro name must match OpenKNXproducer format: ParamNEO_NEO{EffectName}{ParamName}
      # E.g., ParamNEO_NEORainbowSpeed (from Name="NEO%C%RainbowSpeed")
      $macroName = "ParamNEO_NEO$effectNameClean$paramNameClean"

      if ($param.Type -eq 'PARAM_STRING') {
        # TypeText param: OpenKNXproducer emits ParamNEO_* as knx.paramData() (const uint8_t*).
        # Pass the flash pointer; effect copies it into its per-segment buffer.
        $cpp += "            if (paramCount >= $($i + 1)) effect->setParameter(segment, $i, (uint32_t)(uintptr_t)$macroName);"
      }
      else {
        $cpp += "            if (paramCount >= $($i + 1)) effect->setParameter(segment, $i, $macroName);"
      }
    }

    $cpp += '            break;'
    $cpp += '            '
  }

  $cpp += '        default:'
  $cpp += '            // No effect-specific parameters to load'
  $cpp += '            break;'
  $cpp += '    }'
  $cpp += '}'
  $cpp += ''
  $cpp += '#endif // EFFECT_PARAMETER_MAPPING_H'
  $cpp += ''

  return $cpp -join "`n"
}

# ====================================================================
# Scene Effect Parameter Generation
# ====================================================================

function Generate-SceneEffectParameters {
  param([array]$Effects)

  Write-Host "    ▸ Generating Scene Effect Parameters (Generic Slot Approach)..." -ForegroundColor DarkGray

  # Generic Slot Approach: Instead of 107 per-effect Parameters, define only 10 generic slots.
  # All 10 slots are sequential uint8 at offsets +12..+21 (no bool overlay).
  # Aliased ParameterRefs in the ParameterRefs section provide per-effect labels.

  $xml = @()
  $xml += ''
  $xml += '                            <!-- Generic Slot Parameters (10 bytes: offsets +12..+21, all uint8) -->'
  for ($i = 0; $i -lt $script:Config.SceneGenericSlotCount; $i++) {
    $id = $script:Config.SceneBaseParamId + $i  # 8 + i
    $offset = $script:Config.SceneEffectParamOffset + $i  # 12 + i
    $padId = if ($id -lt 10) { "$id" } else { "$id" }
    $xml += "                            <Parameter Id=`"%AID%_UP-%TT%%CC%%PPP+$id%`" Offset=`"%MO+$offset%`" BitOffset=`"0`" Name=`"NEO%C%Scene%SN%Slot$i`" ParameterType=`"%AID%_PT-SceneSlotUint8`" Text=`"Parameter $($i+1) (Szene %SZ%)`" Value=`"0`"/>"
  }

  $nextId = $script:Config.SceneBaseParamId + $script:Config.SceneGenericSlotCount  # 8 + 10 = 18
  Write-Host "    OK Generated $($script:Config.SceneGenericSlotCount) generic scene slot parameters (IDs $($script:Config.SceneBaseParamId)..$($nextId - 1))" -ForegroundColor Green

  return @{
    Xml    = ($xml -join "`n")
    NextId = $nextId
  }
}

function Generate-SceneEffectParameterRefs {
  param(
    [array]$Effects,
    [int]$StartId
  )

  Write-Host "    ▸ Generating Scene Effect ParameterRefs (Generic Slot Aliases)..." -ForegroundColor DarkGray

  # Generic Slot Approach: Generate default refs for the 10 slots,
  # plus aliased refs with per-effect labels (Text attribute overrides).
  # Slot assignment: param[i] → Slot[i] sequentially, regardless of type.

  $xml = @()
  $xml += ''
  $xml += '                            <!-- Generic Slot default refs -->'

  $baseId = $script:Config.SceneBaseParamId  # 8
  for ($i = 0; $i -lt $script:Config.SceneGenericSlotCount; $i++) {
    $id = $baseId + $i
    $xml += "                            <ParameterRef Id=`"%AID%_UP-%TT%%CC%%PPP+$id%_R-%TT%%CC%%PPP+${id}%01`" RefId=`"%AID%_UP-%TT%%CC%%PPP+$id%`" />"
  }

  # Generate aliased ParameterRefs per effect (Text overrides for per-effect labels)
  $effectsWithParams = $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID
  if ($effectsWithParams.Count -gt 0) {
    $xml += ''
    $xml += '                            <!-- Aliased ParameterRefs: per-effect labels pointing to generic slots -->'

    foreach ($effect in $effectsWithParams) {
      $effectClean = $effect.NameDE -replace $script:Config.CleanPatterns.AlphanumericOnly, ''
      $xml += "                            <!-- $($effect.NameDE) (ID $($effect.EffectID)) -->"

      for ($i = 0; $i -lt $effect.Parameters.Count; $i++) {
        $param = $effect.Parameters[$i]
        # Map string parameters to the dedicated scene effect text field (PPP+18).
        if ($param.Type -eq 'PARAM_STRING') {
          $slotId = $script:Config.SceneEffectTextSlotId
        }
        else {
          $slotId = $baseId + $i  # param[i] → Slot[i]
        }
        # Alias suffix: 2-digit ref suffix, unique per slot per effect
        # Format: tcccnnn_R-tcccnnnrr where rr = effectID+2 (avoids 01=default ref)
        $aliasSuffix = "{0:D2}" -f ($effect.EffectID + 2)
        $aliasId = "%AID%_UP-%TT%%CC%%PPP+${slotId}%_R-%TT%%CC%%PPP+${slotId}%${aliasSuffix}"
        $text = "$($param.Name)"
        $defaultValue = if ($param.Type -eq 'PARAM_STRING') { '' } else { $param.Default }
        $xml += "                            <ParameterRef Id=`"$aliasId`" RefId=`"%AID%_UP-%TT%%CC%%PPP+$slotId%`" Text=`"$text`" Value=`"$defaultValue`" />"
      }
    }
  }

  $totalAliased = ($effectsWithParams | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum
  Write-Host "    OK Generated $($script:Config.SceneGenericSlotCount) default refs + $totalAliased aliased refs for $($effectsWithParams.Count) effect(s)" -ForegroundColor Green

  return ($xml -join "`n")
}

function Generate-SceneEffectDynamicChoose {
  param(
    [array]$Effects,
    [int]$StartId
  )

  Write-Host "    ▸ Generating Scene Effect Dynamic UI (choose/when)..." -ForegroundColor DarkGray

  # Generic Slot Approach: Generate choose/when block that shows aliased ParameterRefs per effect.
  # Each effect's when-block references the aliased refs with per-effect Text labels.

  $xml = @()
  $xml += ''

  $effectsWithParams = $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID
  if ($effectsWithParams.Count -eq 0) {
    $xml += '                                <!-- No scene effect parameters for dynamic UI -->'
    return ($xml -join "`n")
  }

  $baseId = $script:Config.SceneBaseParamId  # 8

  # choose on the scene's EffectType (%PPP+0% = scene effect type param)
  $xml += '                                <choose ParamRefId="%AID%_UP-%TT%%CC%%PPP+0%_R-%TT%%CC%%PPP+0%01">'

  foreach ($effect in $effectsWithParams) {
    $xml += "                                  <when test=`"$($effect.EffectID)`">"
    $xml += "                                    <!-- $($effect.NameDE) Effect -->"
    $effectNameClean = $effect.NameDE -replace $script:Config.CleanPatterns.SpaceToHyphen, '-'

    for ($i = 0; $i -lt $effect.Parameters.Count; $i++) {
      $param = $effect.Parameters[$i]
      if ($param.Type -eq 'PARAM_STRING') {
        $slotId = $script:Config.SceneEffectTextSlotId
      }
      else {
        $slotId = $baseId + $i
      }
      $aliasSuffix = "{0:D2}" -f ($effect.EffectID + 2)
      $aliasId = "%AID%_UP-%TT%%CC%%PPP+${slotId}%_R-%TT%%CC%%PPP+${slotId}%${aliasSuffix}"
      # Per-effect HelpContext matching baggage file NEO-{ParamName}-{EffectName}.md
      $paramNameClean = $param.Name -replace $script:Config.CleanPatterns.SpaceToHyphen, '-'
      $helpContext = "NEO-$paramNameClean-$effectNameClean"
      $xml += "                                    <ParameterRefRef RefId=`"$aliasId`" IndentLevel=`"3`" HelpContext=`"$helpContext`"/>"
    }

    $xml += '                                  </when>'
  }

  $xml += '                                </choose>'
  $xml += ''

  Write-Host "    OK Generated scene dynamic UI for $($effectsWithParams.Count) effect(s)" -ForegroundColor Green

  return ($xml -join "`n")
}

function Generate-SceneEffectPCRParameters {
  param([array]$Effects)

  Write-Host "    ▸ Generating Scene Effect PC RParameters (aliased refs for UI refresh)..." -ForegroundColor DarkGray

  $xml = @()
  $baseId = $script:Config.SceneBaseParamId  # 8
  $slotCount = $script:Config.SceneGenericSlotCount  # 10

  # Aliased ParameterRefRefs for all effects — forces ETS to refresh their UI
  # when the ParameterCalculation fires on effect type change.
  $effectsWithParams = $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID
  $totalAliased = 0
  foreach ($effect in $effectsWithParams) {
    $xml += "                                    <!-- $($effect.NameDE) (ID $($effect.EffectID)) -->"
    for ($i = 0; $i -lt $effect.Parameters.Count; $i++) {
      $param = $effect.Parameters[$i]
      if ($param.Type -eq 'PARAM_STRING') {
        $slotId = $script:Config.SceneEffectTextSlotId
      }
      else {
        $slotId = $baseId + $i
      }
      $aliasSuffix = "{0:D2}" -f ($effect.EffectID + 2)
      $refId = "%AID%_UP-%TT%%CC%%PPP+${slotId}%_R-%TT%%CC%%PPP+${slotId}%${aliasSuffix}"
      $aliasName = "E$($effect.EffectID)S$i"
      $xml += "                                    <ParameterRefRef RefId=`"$refId`" AliasName=`"$aliasName`" />"
      $totalAliased++
    }
  }

  # Generic slot refs (these determine the EEPROM value)
  $xml += "                                    <!-- Generic Slot refs -->"
  for ($i = 0; $i -lt $slotCount; $i++) {
    $id = $baseId + $i
    $xml += "                                    <ParameterRefRef RefId=`"%AID%_UP-%TT%%CC%%PPP+$id%_R-%TT%%CC%%PPP+${id}%01`" AliasName=`"Slot$i`" />"
  }

  Write-Host "    OK Generated $totalAliased aliased + $slotCount generic PC RParameters" -ForegroundColor Green
  return ($xml -join "`n")
}

function Get-CueTypedParamPlan {
  # Builds an ordered list of typed-param descriptors with a stable running
  # index K (drives %CUE_TPP+K%). Shared by all three Cue generators so the
  # Parameter, ParameterRef and ParameterRefRef stay perfectly in sync.
  #
  # Typed cue params use an 8-digit ID scheme that bypasses the 3-digit nnn
  # limit while still satisfying the producer ParameterRef regex
  # ([1-3]?\d\d{6})_R-\1\d\d$ : literal prefix "20" + %CC%(2) + %CUE_TPP+K%(4)
  # => 8 digits, first digit "2" (in [1-3]). Each typed param OVERLAYS the
  # generic cue slot it represents (same flash Offset), so runtime byte layout
  # and Save/Restore stay unchanged - it is purely an ETS UI view.
  param([array]$Effects)

  $plan = @()
  $k = 0
  $baseId = $script:Config.CueGenericSlotBaseId  # 1
  $effectsWithParams = $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID

  foreach ($effect in $effectsWithParams) {
    $effectNameClean = $effect.NameDE -replace $script:Config.CleanPatterns.AlphanumericOnly, ''

    for ($i = 0; $i -lt $effect.Parameters.Count; $i++) {
      $param = $effect.Parameters[$i]

      # Map parameter to the generic cue slot it overlays (same flash offset).
      if ($param.Type -eq 'PARAM_STRING') {
        # String params share the dedicated EffectText field (PPP+17 @ CUE_OFF+34).
        $offsetMacro = '%CUE_OFF+34%'
      }
      elseif ($i -lt $script:Config.CueGenericSlotCount) {
        # Numeric/enum/bool params 0..9 map to slot bytes CUE_OFF+1..+10.
        $offsetMacro = "%CUE_OFF+$($i + 1)%"
      }
      else {
        # Beyond the available cue slots -> intentionally truncated (see AGENTS.md).
        continue
      }

      $paramNameClean = $param.Name -replace $script:Config.CleanPatterns.AlphanumericOnly, ''
      $ptId = "%AID%_PT-$effectNameClean$paramNameClean"
      # HelpContext reuses the per-effect baggage files generated for scenes:
      # NEO-{ParamName}-{EffectName} with spaces -> hyphens (must match the scene
      # 5b formula exactly so the same NEO-*.md files are shared, no new files).
      $helpParamHyphen  = $param.Name   -replace $script:Config.CleanPatterns.SpaceToHyphen, '-'
      $helpEffectHyphen = $effect.NameDE -replace $script:Config.CleanPatterns.SpaceToHyphen, '-'
      $helpContext = "NEO-$helpParamHyphen-$helpEffectHyphen"
      # All typed cue params OVERLAY the same generic slot bytes (CUE_OFF+1..+10),
      # unlike segments where every param has its own offset. Because the producer
      # initialises a byte with the default of the LAST parameter declared at that
      # offset, per-effect defaults would corrupt the fresh flash image (a 2D-effect
      # default would leak into every cue slot). Therefore every typed param defaults
      # to 0 and uses full-byte BitOffset=0 (matching the runtime read of the generic
      # slot). Fresh image == all cue param bytes 0, identical to the pre-feature layout.
      $bitOffset = 0
      $value = if ($param.Type -eq 'PARAM_STRING') { '' } else { '0' }
      # Effekt-Default getrennt vom 0-Slot-Wert halten: der Slot/typisierte Parameter
      # bleibt 0 (Fresh-Image == alle Cue-Bytes 0, Rueckwaerts-Kompat), aber der Ref
      # bekommt diesen Default als Value=-Override -> greift, sobald der Effekt gewaehlt
      # ist (gleiches Muster wie Szene/Segment).
      $default = if ($param.Type -eq 'PARAM_STRING') { '' } else { $param.Default }
      $text = "$($param.Name)"
      if ($param.Type -eq 'PARAM_STRING' -and $param.Name -eq 'Text') {
        $text = 'Laufschrift-Text'
      }

      $plan += [pscustomobject]@{
        K            = $k
        EffectID     = $effect.EffectID
        EffectNameDE = $effect.NameDE
        OffsetMacro  = $offsetMacro
        PtId         = $ptId
        BitOffset    = $bitOffset
        NameClean    = "$effectNameClean$paramNameClean"
        Text         = $text
        Value        = $value
        Default      = $default
        HelpContext  = $helpContext
      }
      $k++
    }
  }

  return , $plan
}

function Generate-CueEffectTypedParams {
  # Emits one typed <Parameter> per (effect,param) overlaying the matching cue
  # slot byte. Uses the 8-digit "20" + CC + CUE_TPP scheme and op:nowarn (the
  # overlay with the generic slot at the same offset is intentional).
  param([array]$Effects)

  Write-Host "    ▸ Generating Cue Effect Typed Parameters (typed ETS fields)..." -ForegroundColor DarkGray

  $plan = Get-CueTypedParamPlan -Effects $Effects
  if ($plan.Count -eq 0) {
    return '              <!-- No cue effect typed parameters -->'
  }

  $xml = @()
  $lastEffect = $null
  foreach ($d in $plan) {
    if ($d.EffectID -ne $lastEffect) {
      $xml += "              <!-- $($d.EffectNameDE) (ID $($d.EffectID)) -->"
      $lastEffect = $d.EffectID
    }
    $id = "%AID%_UP-20%CC%%CUE_TPP+$($d.K)%"
    $name = "NEOEM%C%Cue%CUE_NR%$($d.NameClean)"
    $xml += "              <Parameter Id=`"$id`" Offset=`"$($d.OffsetMacro)`" BitOffset=`"$($d.BitOffset)`" Name=`"$name`" ParameterType=`"$($d.PtId)`" Text=`"$($d.Text)`" Value=`"$($d.Value)`" op:nowarn=`"true`"/>"
  }

  Write-Host "    OK Generated $($plan.Count) typed cue parameters" -ForegroundColor Green
  return ($xml -join "`n")
}

function Generate-CueEffectParameterRefs {
  param(
    [array]$Effects
  )

  Write-Host "    ▸ Generating Cue Effect ParameterRefs (typed params)..." -ForegroundColor DarkGray

  $xml = @()
  $xml += ''
  $xml += '              <!-- Generic Cue Slot default refs -->'

  $baseId = $script:Config.CueGenericSlotBaseId  # 1
  for ($i = 0; $i -lt $script:Config.CueGenericSlotCount; $i++) {
    $id = $baseId + $i
    $xml += "              <ParameterRef Id=`"%AID%_UP-%TT%%CC%%CUE_PPP+$id%_R-%TT%%CC%%CUE_PPP+${id}%01`" RefId=`"%AID%_UP-%TT%%CC%%CUE_PPP+$id%`" />"
  }

  $plan = Get-CueTypedParamPlan -Effects $Effects
  if ($plan.Count -gt 0) {
    $xml += ''
    $xml += '              <!-- Typed Cue ParameterRefs: per-effect typed fields overlaying the cue slots -->'

    $lastEffect = $null
    foreach ($d in $plan) {
      if ($d.EffectID -ne $lastEffect) {
        $xml += "              <!-- $($d.EffectNameDE) (ID $($d.EffectID)) -->"
        $lastEffect = $d.EffectID
      }
      $base = "%AID%_UP-20%CC%%CUE_TPP+$($d.K)%"
      $refId = "${base}_R-20%CC%%CUE_TPP+$($d.K)%01"
      $xml += "              <ParameterRef Id=`"$refId`" RefId=`"$base`" Value=`"$($d.Default)`" />"
    }
  }

  Write-Host "    OK Generated $($script:Config.CueGenericSlotCount) default refs + $($plan.Count) typed refs for $(($plan | Select-Object -ExpandProperty EffectID -Unique).Count) effect(s)" -ForegroundColor Green
  return ($xml -join "`n")
}

function Generate-CueEffectDynamicChoose {
  param(
    [array]$Effects
  )

  Write-Host "    ▸ Generating Cue Effect Dynamic UI (choose/when, typed)..." -ForegroundColor DarkGray

  $xml = @()
  $xml += ''

  $plan = Get-CueTypedParamPlan -Effects $Effects
  if ($plan.Count -eq 0) {
    $xml += '                    <!-- No cue effect parameters for dynamic UI -->'
    return ($xml -join "`n")
  }

  $xml += '                    <choose ParamRefId="%AID%_UP-%TT%%CC%%CUE_PPP+0%_R-%TT%%CC%%CUE_PPP+0%01">'

  # Plan is ordered by EffectID (numeric) then param order: break <when> blocks
  # on EffectID change to preserve correct effect grouping.
  $lastEffect = $null
  $effectCount = 0
  foreach ($d in $plan) {
    if ($d.EffectID -ne $lastEffect) {
      if ($null -ne $lastEffect) { $xml += '                      </when>' }
      $xml += "                      <when test=`"$($d.EffectID)`">"
      $xml += "                        <!-- $($d.EffectNameDE) Effect -->"
      $xml += '                        <ParameterSeparator Id="%AID%_PS-nnn" Text="Effekt-spezifische Parameter" UIHint="Headline"/>'
      $lastEffect = $d.EffectID
      $effectCount++
    }
    $refId = "%AID%_UP-20%CC%%CUE_TPP+$($d.K)%_R-20%CC%%CUE_TPP+$($d.K)%01"
    $xml += "                        <ParameterRefRef RefId=`"$refId`" IndentLevel=`"2`" HelpContext=`"$($d.HelpContext)`"/>"
  }
  if ($null -ne $lastEffect) { $xml += '                      </when>' }

  $xml += '                    </choose>'
  $xml += ''

  Write-Host "    OK Generated cue dynamic UI for $effectCount effect(s)" -ForegroundColor Green
  return ($xml -join "`n")
}

function Update-CuePartXml {
  param(
    [string]$CuePartPath,
    [string]$CueRefsContent,
    [string]$CueDynamicContent,
    [string]$CueTypedParamsContent
  )

  Write-Host "    ▸ Updating Cue part.xml..." -ForegroundColor DarkGray

  if (-not (Test-Path $CuePartPath)) {
    Write-Error "Cue part.xml not found: $CuePartPath"
    return $false
  }

  $content = Get-Content -Path $CuePartPath -Raw -Encoding UTF8
  $markers = $script:Config.Markers
  $updateCount = 0

  $pattern = "(?s)($([regex]::Escape($markers.CueEffectTypedParamsStart)))(.*?)($([regex]::Escape($markers.CueEffectTypedParamsEnd)))"
  if ($content -match $pattern) {
    $content = $content -replace $pattern, "`$1`n$CueTypedParamsContent`n              `$3"
    $updateCount++
  }

  $pattern = "(?s)($([regex]::Escape($markers.CueEffectRefsStart)))(.*?)($([regex]::Escape($markers.CueEffectRefsEnd)))"
  if ($content -match $pattern) {
    $content = $content -replace $pattern, "`$1`n$CueRefsContent`n              `$3"
    $updateCount++
  }

  $pattern = "(?s)($([regex]::Escape($markers.CueEffectDynamicStart)))(.*?)($([regex]::Escape($markers.CueEffectDynamicEnd)))"
  if ($content -match $pattern) {
    $content = $content -replace $pattern, "`$1`n$CueDynamicContent`n                    `$3"
    $updateCount++
  }

  if ($updateCount -eq 3) {
    Set-Content -Path $CuePartPath -Value $content -Encoding UTF8 -NoNewline
    Write-Host "    OK Cue part.xml updated ($updateCount blocks)" -ForegroundColor Green
    return $true
  }
  else {
    Write-Warning "Only $updateCount of 3 cue blocks were updated in $CuePartPath"
    return $false
  }
}

function Generate-SceneEffectDefaultsFunction {
  param([array]$Effects)

  Write-Host "    ▸ Generating Scene Effect Defaults JS function..." -ForegroundColor DarkGray

  $effectsWithParams = $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID
  $slotCount = $script:Config.SceneGenericSlotCount  # 10

  $lines = @()
  $lines += "// Set scene slot defaults when effect type changes"
  $lines += "function NEO_SetSceneEffectDefaults(input, output, context) {"
  $lines += "    var effectType = toInt(input.EffectType, 0);"
  $lines += "    var defaults = NEO_SceneEffectDefaults[String(effectType)];"
  $lines += "    var d0, d1, d2, d3, d4, d5, d6, d7, d8, d9;"
  $lines += "    if (defaults) {"
  for ($i = 0; $i -lt $slotCount; $i++) {
    $lines += "        d$i = defaults[$i];"
  }
  $lines += "    } else {"
  for ($i = 0; $i -lt $slotCount; $i++) {
    $lines += "        d$i = 0;"
  }
  $lines += "    }"
  $lines += ""
  $lines += "    // Write to generic slots (determines EEPROM value)"
  for ($i = 0; $i -lt $slotCount; $i++) {
    $lines += "    output.Slot$i = d$i;"
  }
  $lines += ""
  $lines += "    // Write to all aliased refs (forces ETS to refresh UI display)"
  $lines += "    // All aliases for a slot share the same underlying Parameter."
  $lines += "    // Writing to them triggers ETS to update the displayed value."
  foreach ($effect in $effectsWithParams) {
    $comment = "// $($effect.NameDE)"
    $assignments = @()
    for ($i = 0; $i -lt $effect.Parameters.Count; $i++) {
      $assignments += "output.E$($effect.EffectID)S$i = d$i"
    }
    $assignLine = ($assignments -join "; ") + ";"
    $lines += "    $assignLine  $comment"
  }
  $lines += "}"

  Write-Host "    OK Generated JS function with $($effectsWithParams.Count) effect alias groups" -ForegroundColor Green
  return ($lines -join "`n")
}

function Update-ScenePartXml {
  param(
    [string]$ScenePartPath,
    [string]$EffectParamsContent,
    [string]$EffectRefsContent,
    [string]$EffectDynamicContent,
    [string]$PCRParamsContent
  )

  Write-Host "    ▸ Updating Scene part.xml..." -ForegroundColor DarkGray

  if (-not (Test-Path $ScenePartPath)) {
    Write-Error "Scene part.xml not found: $ScenePartPath"
    return $false
  }

  $content = Get-Content -Path $ScenePartPath -Raw -Encoding UTF8

  $markers = $script:Config.Markers
  $updateCount = 0

  # Update Scene Effect Parameters
  $pattern = "(?s)($([regex]::Escape($markers.SceneEffectParamsStart)))(.*?)($([regex]::Escape($markers.SceneEffectParamsEnd)))"
  if ($content -match $pattern) {
    $content = $content -replace $pattern, "`$1`n$EffectParamsContent`n                            `$3"
    $updateCount++
  }

  # Update Scene Effect ParameterRefs
  $pattern = "(?s)($([regex]::Escape($markers.SceneEffectRefsStart)))(.*?)($([regex]::Escape($markers.SceneEffectRefsEnd)))"
  if ($content -match $pattern) {
    $content = $content -replace $pattern, "`$1`n$EffectRefsContent`n                            `$3"
    $updateCount++
  }

  # Update Scene Effect Dynamic UI
  $pattern = "(?s)($([regex]::Escape($markers.SceneEffectDynamicStart)))(.*?)($([regex]::Escape($markers.SceneEffectDynamicEnd)))"
  if ($content -match $pattern) {
    $content = $content -replace $pattern, "`$1`n$EffectDynamicContent`n                                `$3"
    $updateCount++
  }

  # Update Scene Effect PC RParameters
  $pattern = "(?s)($([regex]::Escape($markers.SceneEffectPCParamsStart)))(.*?)($([regex]::Escape($markers.SceneEffectPCParamsEnd)))"
  if ($content -match $pattern) {
    $content = $content -replace $pattern, "`$1`n$PCRParamsContent`n                                    `$3"
    $updateCount++
  }

  if ($updateCount -eq 4) {
    Set-Content -Path $ScenePartPath -Value $content -Encoding UTF8 -NoNewline
    Write-Host "    OK Scene part.xml updated ($updateCount blocks)" -ForegroundColor Green
    return $true
  }
  else {
    Write-Warning "Only $updateCount of 4 scene blocks were updated in $ScenePartPath"
    return $false
  }
}

function Generate-SceneEffectDefaultsJS {
  param([array]$Effects)

  Write-Host "    ▸ Generating Scene Effect Defaults JS lookup table..." -ForegroundColor DarkGray

  $effectsWithParams = $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID
  $slotCount = $script:Config.SceneGenericSlotCount  # 10

  $lines = @()
  $lines += "var NEO_SceneEffectDefaults = {"

  $lastIdx = $effectsWithParams.Count - 1
  for ($idx = 0; $idx -lt $effectsWithParams.Count; $idx++) {
    $effect = $effectsWithParams[$idx]
    $defaults = @()
    for ($i = 0; $i -lt $slotCount; $i++) {
      if ($i -lt $effect.Parameters.Count) {
        $defaults += "$($effect.Parameters[$i].Default)"
      }
      else {
        $defaults += "0"
      }
    }
    $defaultsStr = $defaults -join ", "
    $comma = if ($idx -lt $lastIdx) { "," } else { "" }
    $lines += "    `"$($effect.EffectID)`": [$defaultsStr]$comma  // $($effect.NameDE)"
  }

  $lines += "};"

  Write-Host "    OK Generated JS defaults for $($effectsWithParams.Count) effect(s)" -ForegroundColor Green

  return ($lines -join "`n")
}

function Update-ScriptJS {
  param(
    [string]$ScriptPath,
    [string]$DefaultsContent
  )

  if (-not (Test-Path $ScriptPath)) {
    Write-Warning "Script JS not found: $ScriptPath"
    return $false
  }

  $content = Get-Content -Path $ScriptPath -Raw -Encoding UTF8

  $startMarker = "// BEGIN AUTO-GENERATED: Scene Effect Defaults"
  $endMarker = "// END AUTO-GENERATED: Scene Effect Defaults"

  # Match the exact marker line so repeated runs normalize the block instead of preserving blank lines.
  $pattern = "(?s)($([regex]::Escape($startMarker))\r?\n)(.*?)($([regex]::Escape($endMarker)))"
  if ($content -match $pattern) {
    $replacement = "`$1`n$DefaultsContent`n`$3"
    $content = $content -replace $pattern, $replacement
    Set-Content -Path $ScriptPath -Value $content -Encoding UTF8 -NoNewline
    return $true
  }

  Write-Warning "Scene Effect Defaults markers not found in $ScriptPath"
  return $false
}

function Update-ScriptJSFunction {
  param(
    [string]$ScriptPath,
    [string]$FunctionContent
  )

  if (-not (Test-Path $ScriptPath)) {
    Write-Warning "Script JS not found: $ScriptPath"
    return $false
  }

  $content = Get-Content -Path $ScriptPath -Raw -Encoding UTF8

  $startMarker = "// BEGIN AUTO-GENERATED: Scene Effect Defaults Function"
  $endMarker = "// END AUTO-GENERATED: Scene Effect Defaults Function"

  $pattern = "(?s)($([regex]::Escape($startMarker)))(.*?)($([regex]::Escape($endMarker)))"
  if ($content -match $pattern) {
    $replacement = "`$1`n$FunctionContent`n`$3"
    $content = $content -replace $pattern, $replacement
    Set-Content -Path $ScriptPath -Value $content -Encoding UTF8 -NoNewline
    return $true
  }

  Write-Warning "Scene Effect Defaults Function markers not found in $ScriptPath"
  return $false
}

function Generate-SceneCppMapping {
  param([array]$Effects)

  # Generate loadSceneEffectParameters() C++ function
  # This reads effect params from computed scene offsets (generic, no per-scene switch)

  $currentYear = 2026   # fixed -> deterministic output (no per-build churn); bump when needed

  $cpp = @()
  $cpp += ''
  $cpp += '/**'
  $cpp += ' * @brief Load effect-specific parameters from a scene slot in EEPROM'
  $cpp += ' *'
  $cpp += ' * Reads effect-specific parameters from a scene slot. Parameters are stored'
  $cpp += " * at SCENE_DATA_START + sceneIndex * SCENE_SIZE + $($script:Config.SceneEffectParamOffset) within each segment's Union."
  $cpp += ' *'
  $cpp += ' * @param effect Effect instance to configure'
  $cpp += ' * @param segment Segment instance (for setParameter context)'
  $cpp += ' * @param effectID Effect type ID (0-28)'
  $cpp += ' * @param channelIndex Channel/Segment index (0-15)'
  $cpp += ' * @param sceneIndex Scene index (0-14)'
  $cpp += ' * @note Auto-generated - do not edit by hand'
  $cpp += ' */'
  $cpp += 'inline void loadSceneEffectParameters(Effect* effect, Segment* segment, uint8_t effectID, uint8_t channelIndex, uint8_t sceneIndex)'
  $cpp += '{'
  $cpp += '    if (!effect || !segment) return;'
  $cpp += '    '
  $cpp += '    uint8_t paramCount = effect->getParameterCount();'
  $cpp += '    if (paramCount == 0) return;'
  $cpp += '    '
  $cpp += '    // Compute base address for this scene''s effect parameters'
  $cpp += "    // Scene data layout: $($script:Config.SceneSize) bytes per scene, effect params at offset +$($script:Config.SceneEffectParamOffset)"
  $cpp += "    static constexpr uint16_t SCENE_DATA_START = $($script:Config.SceneDataStart);"
  $cpp += "    static constexpr uint8_t SCENE_SIZE = $($script:Config.SceneSize);"
  $cpp += "    static constexpr uint8_t SCENE_EFFECT_PARAM_OFFSET = $($script:Config.SceneEffectParamOffset);"
  $cpp += "    static constexpr uint8_t SCENE_EFFECT_TEXT_OFFSET = $($script:Config.SceneEffectTextOffset);"
  $cpp += '    static constexpr uint8_t SCENE_EFFECT_TEXT_SIZE = 14;'
  $cpp += '    '
  $cpp += '    uint16_t sceneEffectParamBase = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize'
  $cpp += '                                  + SCENE_DATA_START + sceneIndex * SCENE_SIZE + SCENE_EFFECT_PARAM_OFFSET;'
  $cpp += '    '
  $cpp += '    // STRING parameters are stored in the dedicated effectText field (offset +22, 14 bytes)'
  $cpp += '    uint16_t sceneEffectTextBase = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize'
  $cpp += '                                 + SCENE_DATA_START + sceneIndex * SCENE_SIZE + SCENE_EFFECT_TEXT_OFFSET;'
  $cpp += '    char sceneEffectText[SCENE_EFFECT_TEXT_SIZE + 1];'
  $cpp += '    {'
  $cpp += '        const uint8_t* textData = knx.paramData(sceneEffectTextBase);'
  $cpp += '        if (textData) { memcpy(sceneEffectText, textData, SCENE_EFFECT_TEXT_SIZE); }'
  $cpp += '        else { sceneEffectText[0] = 0; }'
  $cpp += '        sceneEffectText[SCENE_EFFECT_TEXT_SIZE] = 0;'
  $cpp += '    }'
  $cpp += '    (void)sceneEffectText;'
  $cpp += '    '
  $cpp += '    switch (static_cast<PT_NEOEffectType>(effectID))'
  $cpp += '    {'

  foreach ($effect in $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID) {
    # Derive enum name from ClassName
    $enumName = $effect.ClassName -replace 'Effect$', '' -replace '^Effect', ''
    if ([string]::IsNullOrEmpty($enumName)) { $enumName = $effect.ClassName }
    $cpp += "        case PT_NEOEffectType::${enumName}:  // $($effect.NameDE) Effect"

    for ($i = 0; $i -lt $effect.Parameters.Count; $i++) {
      if ($effect.Parameters[$i].Type -eq 'PARAM_STRING') {
        $cpp += "            if (paramCount >= $($i + 1)) effect->setParameter(segment, $i, (uint32_t)(uintptr_t)sceneEffectText);"
      }
      else {
        $cpp += "            if (paramCount >= $($i + 1)) effect->setParameter(segment, $i, knx.paramByte(sceneEffectParamBase + $i));"
      }
    }

    $cpp += '            break;'
    $cpp += '            '
  }

  $cpp += '        default:'
  $cpp += '            break;'
  $cpp += '    }'
  $cpp += '}'

  return $cpp -join "`n"
}

# ====================================================================
# Main Script
# ====================================================================

#Clear-Host
Show-OpenKNXLogo -SubTitle "Build Effect Parameters" -Version $SCRIPT_VERSION

# Determine script and repository root directories
$scriptDir = if ($PSScriptRoot) {
  $PSScriptRoot
}
elseif ($PSCommandPath) {
  Split-Path -Parent $PSCommandPath
}
else {
  $PWD.Path
}

Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  Parsing Effect Headers (Language: $Language)" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan

$effectsDir = Resolve-RepoPath $script:Config.EffectsDir
$effects = @()

if (-not (Test-Path $effectsDir)) {
  Write-Host "ERROR: Effects directory not found: $effectsDir" -ForegroundColor Red
  exit 1
}

# Find all *Effect.h files (using centralized function)
$effectFiles = Get-AllEffectHeaders

Write-Host ""
Write-Host "  Found " -NoNewline -ForegroundColor Gray
Write-Host "$($effectFiles.Count)" -NoNewline -ForegroundColor Green
Write-Host " effect header file(s) in $effectsDir" -ForegroundColor Gray
Write-Host "  " -NoNewline
Write-Host ("─" * 116) -ForegroundColor DarkGray
Write-Host ""

# Parse all effects
foreach ($file in $effectFiles) {
  $effectData = Parse-EffectHeader -HeaderPath $file.FullName -Lang $Language
  if ($effectData) {
    $effects += $effectData
  }
}

if ($effects.Count -eq 0) {
  Write-Host ""
  Write-Host "No effects with parameters found!" -ForegroundColor Yellow
  exit 0
}

Write-Host "  " -NoNewline
Write-Host ("─" * 116) -ForegroundColor DarkGray
$totalParams = ($effects | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum
Write-Host "  Parsed " -NoNewline -ForegroundColor Green
Write-Host "$($effects.Count)" -NoNewline -ForegroundColor White
Write-Host " effect(s) with " -NoNewline -ForegroundColor Green
Write-Host "$totalParams" -NoNewline -ForegroundColor White
Write-Host " total parameter(s)" -ForegroundColor Green
Write-Host ""

# ====================================================================
# Generate XML Fragments
# ====================================================================

Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  Generating Files" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host ""

# Step 0: Update Effect Type Enumeration in NeoPixel.share.xml
$shareXmlPath = Resolve-RepoPath $script:Config.ShareXml
Update-EffectTypeEnumerationInShareXml -ShareXmlPath $shareXmlPath

# Step 0b: Generate C++ Effect Type Mapping Header
$cppMapping = Generate-EffectMappingCpp
$cppMappingHeaderPath = "src/EffectTypeMapping.h"
Set-Content -Path $cppMappingHeaderPath -Value $cppMapping -Encoding UTF8
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "C++ effect type mapping header" -ForegroundColor White
Write-Host "       $cppMappingHeaderPath" -ForegroundColor DarkGray

# Read starting IDs from template markers
Write-Host ""
Write-Host "  Reading template configuration..." -ForegroundColor Cyan
$templatePath = Resolve-RepoPath $script:Config.SegmentTemplate

try {
  $markerConfig = Read-StartingIdsFromMarkers -TemplatePath $templatePath
  $startId = $markerConfig.StartId
  $startOffset = $markerConfig.StartOffset

  Write-Host "  [OK] Start ID: " -NoNewline -ForegroundColor Green
  Write-Host "$startId" -ForegroundColor White
  Write-Host "  [OK] Start Offset: " -NoNewline -ForegroundColor Green
  Write-Host "$startOffset" -ForegroundColor White
}
catch {
  Write-Host "  [ERROR] Failed to read starting IDs from template: $_" -ForegroundColor Red
  Write-Host ""
  Write-Host "Please ensure template has correct markers with ID/Offset information!" -ForegroundColor Yellow
  exit 1
}

Write-Host ""
Write-Host "Generating XML content..." -ForegroundColor Cyan
Write-Host ""

# 1. ParameterTypes - inject into NeoPixel.share.xml
$paramTypesInner = Generate-ParameterTypes -Effects $effects
Update-EffectParameterTypesInShareXml -ShareXmlPath $shareXmlPath -Content $paramTypesInner
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "Effect ParameterTypes injected into NeoPixel.share.xml" -ForegroundColor White

# 1b. Remove NEOEFF module from NeoPixel.xml (no longer needed)
$neoPixelXmlPath = Resolve-RepoPath $script:Config.MainXml
Update-NeoEffModule -NeoPixelXmlPath $neoPixelXmlPath -Enable $false
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "NEOEFF module removed" -ForegroundColor White
Write-Host "       $neoPixelXmlPath" -ForegroundColor DarkGray

# 2. Union Parameters (returns next ID/Offset)
$unionResult = Generate-UnionParameters -Effects $effects -StartId $startId -StartOffset $startOffset
$unionParamsXml = $unionResult.Xml
$nextId = $unionResult.NextId
$nextOffset = $unionResult.NextOffset

$unionParamsPath = "src/NeoPixel.Effects.UnionParameters.generated.xml"
Set-Content -Path $unionParamsPath -Value $unionParamsXml -Encoding UTF8
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "Union Parameters XML" -ForegroundColor White
Write-Host "       $unionParamsPath" -ForegroundColor DarkGray
Write-Host "       Next ID: $nextId, Offset: $nextOffset" -ForegroundColor DarkGray

# 3. ParameterRefs (uses same Start ID)
$paramRefsXml = Generate-ParameterRefs -Effects $effects -StartId $startId
$paramRefsPath = "src/NeoPixel.Effects.ParameterRefs.generated.xml"
Set-Content -Path $paramRefsPath -Value $paramRefsXml -Encoding UTF8
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "ParameterRefs XML" -ForegroundColor White
Write-Host "       $paramRefsPath" -ForegroundColor DarkGray

# 4. Dynamic choose/when blocks (uses same Start ID)
$dynamicChooseXml = Generate-DynamicChoose -Effects $effects -StartId $startId
$dynamicChoosePath = "src/NeoPixel.Effects.DynamicChoose.generated.xml"
Set-Content -Path $dynamicChoosePath -Value $dynamicChooseXml -Encoding UTF8
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "Dynamic choose/when blocks" -ForegroundColor White
Write-Host "       $dynamicChoosePath" -ForegroundColor DarkGray

# 5. C++ Parameter Mapping Header
$cppMapping = Generate-CppMapping -Effects $effects
$cppMappingPath = "src/EffectParameterMapping.h"
Set-Content -Path $cppMappingPath -Value $cppMapping -Encoding UTF8
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "C++ mapping header" -ForegroundColor White
Write-Host "       $cppMappingPath" -ForegroundColor DarkGray

# 5b. Scene Effect Parameters (for NeoPixel.Scene.part.xml)
Write-Host ""
Write-Host "  Scene Effect Parameter Generation..." -ForegroundColor Cyan

$sceneEffectResult = Generate-SceneEffectParameters -Effects $effects
$sceneEffectParamsXml = $sceneEffectResult.Xml
$sceneNextId = $sceneEffectResult.NextId

$sceneEffectRefsXml = Generate-SceneEffectParameterRefs -Effects $effects -StartId $script:Config.SceneBaseParamId
$sceneEffectDynamicXml = Generate-SceneEffectDynamicChoose -Effects $effects -StartId $script:Config.SceneBaseParamId
$sceneEffectPCRParams = Generate-SceneEffectPCRParameters -Effects $effects

# 5b.1 Cue Effect Parameters (for NeoPixel.Cue.part.xml)
$cueEffectTypedParamsXml = Generate-CueEffectTypedParams -Effects $effects
$cueEffectRefsXml = Generate-CueEffectParameterRefs -Effects $effects
$cueEffectDynamicXml = Generate-CueEffectDynamicChoose -Effects $effects

$cuePartPath = Resolve-RepoPath $script:Config.CuePartXml
if (Test-Path $cuePartPath) {
  $cueUpdateSuccess = Update-CuePartXml -CuePartPath $cuePartPath `
    -CueRefsContent $cueEffectRefsXml `
    -CueDynamicContent $cueEffectDynamicXml `
    -CueTypedParamsContent $cueEffectTypedParamsXml
  Write-Host "  [OK] " -NoNewline -ForegroundColor Green
  Write-Host "Cue part.xml updated" -ForegroundColor White
  Write-Host "       $cuePartPath" -ForegroundColor DarkGray
}
else {
  Write-Host "  [SKIP] " -NoNewline -ForegroundColor Yellow
  Write-Host "Cue part.xml not found: $cuePartPath" -ForegroundColor White
}

# Update NeoPixel.Scene.part.xml with generated content
$scenePartPath = Resolve-RepoPath $script:Config.ScenePartXml
if (Test-Path $scenePartPath) {
  $sceneUpdateSuccess = Update-ScenePartXml -ScenePartPath $scenePartPath `
    -EffectParamsContent $sceneEffectParamsXml `
    -EffectRefsContent $sceneEffectRefsXml `
    -EffectDynamicContent $sceneEffectDynamicXml `
    -PCRParamsContent $sceneEffectPCRParams
  Write-Host "  [OK] " -NoNewline -ForegroundColor Green
  Write-Host "Scene part.xml updated" -ForegroundColor White
  Write-Host "       $scenePartPath" -ForegroundColor DarkGray
}
else {
  Write-Host "  [SKIP] " -NoNewline -ForegroundColor Yellow
  Write-Host "Scene part.xml not found: $scenePartPath" -ForegroundColor White
}

# 5c. Append loadSceneEffectParameters() to C++ mapping header
$sceneCppMapping = Generate-SceneCppMapping -Effects $effects
$existingCpp = Get-Content -Path $cppMappingPath -Raw -Encoding UTF8
# Insert before the final #endif
$existingCpp = $existingCpp -replace '#endif // EFFECT_PARAMETER_MAPPING_H', "$sceneCppMapping`n`n#endif // EFFECT_PARAMETER_MAPPING_H"
Set-Content -Path $cppMappingPath -Value $existingCpp -Encoding UTF8 -NoNewline
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "loadSceneEffectParameters() added to C++ mapping header" -ForegroundColor White

# 5d. Scene Effect Defaults JS (for NeoPixel.script.js)
$sceneDefaultsJS = Generate-SceneEffectDefaultsJS -Effects $effects
$scriptJsPath = Resolve-RepoPath "src/NeoPixel.script.js"
if (Test-Path $scriptJsPath) {
  $jsUpdateSuccess = Update-ScriptJS -ScriptPath $scriptJsPath -DefaultsContent $sceneDefaultsJS
  if ($jsUpdateSuccess) {
    Write-Host "  [OK] " -NoNewline -ForegroundColor Green
    Write-Host "Scene effect defaults injected into script.js" -ForegroundColor White
    Write-Host "       $scriptJsPath" -ForegroundColor DarkGray
  }
  else {
    Write-Host "  [WARN] " -NoNewline -ForegroundColor Yellow
    Write-Host "Failed to update Scene Effect Defaults in script.js" -ForegroundColor White
  }

  # 5e. Scene Effect Defaults Function JS (for NeoPixel.script.js)
  $sceneDefaultsFunc = Generate-SceneEffectDefaultsFunction -Effects $effects
  $jsFuncSuccess = Update-ScriptJSFunction -ScriptPath $scriptJsPath -FunctionContent $sceneDefaultsFunc
  if ($jsFuncSuccess) {
    Write-Host "  [OK] " -NoNewline -ForegroundColor Green
    Write-Host "Scene effect defaults function injected into script.js" -ForegroundColor White
  }
  else {
    Write-Host "  [WARN] " -NoNewline -ForegroundColor Yellow
    Write-Host "Failed to update Scene Effect Defaults Function in script.js" -ForegroundColor White
  }
}
else {
  Write-Host "  [SKIP] " -NoNewline -ForegroundColor Yellow
  Write-Host "NeoPixel.script.js not found: $scriptJsPath" -ForegroundColor White
}

# 6. Help Files (Markdown)
Write-Host "  ▸ Generating help files..." -ForegroundColor Gray
$helpFilesInfo = Generate-HelpFiles -Effects $effects
$effectIdRefFile = Generate-EffectIdReferenceHelp -Effects $effects
$effectsWithoutDesc = $helpFilesInfo.EffectsWithoutDescription
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "Help files generated ($effectIdRefFile updated)" -ForegroundColor White

$effectFlagInfo = Generate-EffectFlagList -Effects $effects
if ($effectFlagInfo) {
  Write-Host "  [OK] " -NoNewline -ForegroundColor Green
  Write-Host "platformio.custom.ini flag list: $($effectFlagInfo.Standard) + $($effectFlagInfo.Extended) + $($effectFlagInfo.TwoD) = $($effectFlagInfo.Standard + $effectFlagInfo.Extended + $effectFlagInfo.TwoD) flags" -ForegroundColor White
}
else {
  Write-Host "  [WARN] platformio.custom.ini: marker not found, the flag list was not generated" -ForegroundColor Yellow
}

$effectTableInfo = Generate-EffectTypeHelpTable -Effects $effects
if ($effectTableInfo) {
  Write-Host "  [OK] " -NoNewline -ForegroundColor Green
  Write-Host "NEO-Effekt-Typ.md table: $($effectTableInfo.Standard) standard, $($effectTableInfo.Extended) extended, $($effectTableInfo.TwoD) 2D" -ForegroundColor White
  if ($effectTableInfo.MissingDesc.Count -gt 0) {
    Write-Host "  [WARN] Only an English description (no EFFECT_DESC_DE_EN) for: $($effectTableInfo.MissingDesc -join ', ')" -ForegroundColor Yellow
  }
}
else {
  Write-Host "  [WARN] NEO-Effekt-Typ.md: marker not found, the effect table was not generated" -ForegroundColor Yellow
}

# 6b. Scene Help Files (Markdown)
Write-Host "  ▸ Generating scene help files..." -ForegroundColor Gray
$sceneHelpInfo = Generate-SceneHelpFiles -Effects $effects
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "Scene help files generated ($($sceneHelpInfo.Count) new)" -ForegroundColor White

# ====================================================================
# Update Template
# ====================================================================

Write-Host ""
Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  Updating Template" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan

try {
  $success = Update-TemplateWithMarkers -TemplatePath $templatePath `
    -UnionContent $unionParamsXml `
    -ParameterRefsContent $paramRefsXml `
    -DynamicContent $dynamicChooseXml `
    -Effects $effects `
    -NextId $nextId `
    -NextOffset $nextOffset

  if ($success) {
    Write-Host ""
    Write-Host "[OK] Template updated and validated" -ForegroundColor Green
    Write-Host "     Next available ID: " -NoNewline -ForegroundColor DarkGray
    Write-Host "$nextId" -ForegroundColor White
    Write-Host "     Next available Offset: " -NoNewline -ForegroundColor DarkGray
    Write-Host "$nextOffset" -ForegroundColor White

    # ── Update PT-CueEffect enum in NeoPixel.EM.share.xml ──────────────
    # PT-CueEffect is now an Enum (same entries as PT-NEOEffectType).
    # Uses explicit IDs (%AID%_CE-NN) to avoid %ENID% issues in sub-module context.
    $emSharePath = "src/NeoPixel.EM.share.xml"
    if (Test-Path $emSharePath) {
      $emContent = Get-Content $emSharePath -Raw -Encoding UTF8
      $enumLines = @()
      $cueEffects = $effects | Sort-Object @{ Expression = { if (Test-Is2DEffect -ClassName $_.ClassName -NameDE $_.NameDE -NameEN $_.NameEN) { 1 } else { 0 } } }, @{ Expression = { [int]$_.EffectID } }
      foreach ($effect in $cueEffects) {
        $effectValue = [int]$effect.EffectID
        $idxStr = $effectValue.ToString("00")
        $nameDE = if ($effect.NameDE) { $effect.NameDE } elseif ($effect.Name) { $effect.Name } else { "Effect $effectValue" }
        $nameEN = if ($effect.NameEN) { $effect.NameEN } else { $nameDE }
        $displayName = Get-EffectDropdownText -ClassName $effect.ClassName -NameDE $nameDE -NameEN $nameEN -EffectId $effectValue
        $enumLines += "                <Enumeration Text=`"$displayName`" Value=`"$effectValue`" Id=`"%AID%_PT-CueEffect_EN-$idxStr`"/>"
      }
      $enumBlock = $enumLines -join "`n"
      $emContent = $emContent -replace '(?s)(<!-- GENERATED_CUE_EFFECT_ENUMERATIONS_START -->).*?(<!-- GENERATED_CUE_EFFECT_ENUMERATIONS_END -->)', "`$1`n$enumBlock`n                `$2"
      # Second variant without the 2D effects, for hardware built with NEOPIXEL_DISABLE_2D.
      $cueNo2DLines = @()
      foreach ($effect in $cueEffects) {
        if (Test-Is2DEffect -ClassName $effect.ClassName -NameDE $effect.NameDE -NameEN $effect.NameEN) { continue }
        $effectValue = [int]$effect.EffectID
        $idxStr = $effectValue.ToString("00")
        $nameDE = if ($effect.NameDE) { $effect.NameDE } elseif ($effect.Name) { $effect.Name } else { "Effect $effectValue" }
        $nameEN = if ($effect.NameEN) { $effect.NameEN } else { $nameDE }
        $displayName = Get-EffectDropdownText -ClassName $effect.ClassName -NameDE $nameDE -NameEN $nameEN -EffectId $effectValue
        $cueNo2DLines += "                <Enumeration Text=`"$displayName`" Value=`"$effectValue`" Id=`"%AID%_PT-CueEffectNo2D_EN-$idxStr`"/>"
      }
      $cueNo2DBlock = $cueNo2DLines -join "`n"
      $emContent = $emContent -replace '(?s)(<!-- GENERATED_CUE_EFFECT_ENUMERATIONS_NO2D_START -->).*?(<!-- GENERATED_CUE_EFFECT_ENUMERATIONS_NO2D_END -->)', "`$1`n$cueNo2DBlock`n                `$2"
      Set-Content $emSharePath -Value $emContent -Encoding UTF8 -NoNewline
      Write-Host "  [OK] NeoPixel.EM.share.xml: PT-CueEffect enum updated ($($effects.Count) effects, No2D $($cueNo2DLines.Count))" -ForegroundColor Green
    }

    # ── Update PT-NEOSceneEffectType enum in NeoPixel.share.xml ────────
    # Same effect entries as PT-CueEffect (display-sorted, real EffectIDs);
    # static "Effektmanager N starten" entries (200..215) live outside the markers.
    $sharePath = "src/NeoPixel.share.xml"
    if (Test-Path $sharePath) {
      $shareContent = Get-Content $sharePath -Raw -Encoding UTF8
      $sceneEnumLines = @()
      $sceneEffects = $effects | Sort-Object @{ Expression = { if (Test-Is2DEffect -ClassName $_.ClassName -NameDE $_.NameDE -NameEN $_.NameEN) { 1 } else { 0 } } }, @{ Expression = { [int]$_.EffectID } }
      foreach ($effect in $sceneEffects) {
        $effectValue = [int]$effect.EffectID
        $idxStr = $effectValue.ToString("00")
        $nameDE = if ($effect.NameDE) { $effect.NameDE } elseif ($effect.Name) { $effect.Name } else { "Effect $effectValue" }
        $nameEN = if ($effect.NameEN) { $effect.NameEN } else { $nameDE }
        $displayName = Get-EffectDropdownText -ClassName $effect.ClassName -NameDE $nameDE -NameEN $nameEN -EffectId $effectValue
        $sceneEnumLines += "                <Enumeration Text=`"$displayName`" Value=`"$effectValue`" Id=`"%AID%_PT-NEOSceneEffectType_EN-$idxStr`"/>"
      }
      $sceneEnumBlock = $sceneEnumLines -join "`n"
      $shareContent = $shareContent -replace '(?s)(<!-- GENERATED_SCENE_EFFECT_ENUMERATIONS_START -->).*?(<!-- GENERATED_SCENE_EFFECT_ENUMERATIONS_END -->)', "`$1`n$sceneEnumBlock`n                `$2"
      # Second variant without the 2D effects; the 16 EM actions live outside the markers.
      $sceneNo2DLines = @()
      foreach ($effect in $sceneEffects) {
        if (Test-Is2DEffect -ClassName $effect.ClassName -NameDE $effect.NameDE -NameEN $effect.NameEN) { continue }
        $effectValue = [int]$effect.EffectID
        $idxStr = $effectValue.ToString("00")
        $nameDE = if ($effect.NameDE) { $effect.NameDE } elseif ($effect.Name) { $effect.Name } else { "Effect $effectValue" }
        $nameEN = if ($effect.NameEN) { $effect.NameEN } else { $nameDE }
        $displayName = Get-EffectDropdownText -ClassName $effect.ClassName -NameDE $nameDE -NameEN $nameEN -EffectId $effectValue
        $sceneNo2DLines += "                <Enumeration Text=`"$displayName`" Value=`"$effectValue`" Id=`"%AID%_PT-NEOSceneEffectTypeNo2D_EN-$idxStr`"/>"
      }
      $sceneNo2DBlock = $sceneNo2DLines -join "`n"
      $shareContent = $shareContent -replace '(?s)(<!-- GENERATED_SCENE_EFFECT_ENUMERATIONS_NO2D_START -->).*?(<!-- GENERATED_SCENE_EFFECT_ENUMERATIONS_NO2D_END -->)', "`$1`n$sceneNo2DBlock`n                `$2"
      Set-Content $sharePath -Value $shareContent -Encoding UTF8 -NoNewline
      Write-Host "  [OK] NeoPixel.share.xml: PT-NEOSceneEffectType enum updated ($($effects.Count) effects + 16 EM actions, No2D $($sceneNo2DLines.Count))" -ForegroundColor Green
    }

    # ============================================================
    # FINAL VALIDATION: Test with OpenKNXproducer (optional)
    # ============================================================
    if (-not $SkipOpenKNXproducer) {
      # Validate against Dev XML (primary) — same template used by Release
      $testResult = Test-OpenKNXproducer -WorkingDir "." `
        -HeaderFile "include/knxprod.h" `
        -SourceDir "src/NeoPixel-Dev"
    }
    else {
      Write-Host ""
      Write-Host "  Skipping OpenKNXproducer validation (as requested)" -ForegroundColor Yellow
      $testResult = @{ Success = $true }
    }

    # ── Overlapping-parameter guard ────────────────────────────────────
    # The effect selectors deliberately put two parameters on one byte: the full list and
    # the reduced one shown on hardware without the 2D effects. The producer reports every
    # such pair as WARN 010, which would bury a genuine collision in the noise. Accept only
    # the known pairs (X together with XNo2D) and name anything else.
    if ($testResult.Output) {
      $expectedBuckets = 0
      $unexpectedBuckets = @()
      $bucketMembers = @()
      $inBucket = $false
      foreach ($line in ($testResult.Output -split "`n")) {
        if ($line -match 'found overlapping parameter bucket') {
          if ($inBucket) {
            if ($bucketMembers.Count -eq 2 -and (($bucketMembers[0] -eq "$($bucketMembers[1])No2D") -or ($bucketMembers[1] -eq "$($bucketMembers[0])No2D"))) { $expectedBuckets++ }
            else { $unexpectedBuckets += ($bucketMembers -join ' + ') }
          }
          $inBucket = $true
          $bucketMembers = @()
        }
        elseif ($inBucket -and $line -match 'Parameter\s+([A-Za-z0-9_]+),') { $bucketMembers += $Matches[1] }
        elseif ($inBucket -and $line -notmatch 'WARN 010') {
          if ($bucketMembers.Count -eq 2 -and (($bucketMembers[0] -eq "$($bucketMembers[1])No2D") -or ($bucketMembers[1] -eq "$($bucketMembers[0])No2D"))) { $expectedBuckets++ }
          else { $unexpectedBuckets += ($bucketMembers -join ' + ') }
          $inBucket = $false
          $bucketMembers = @()
        }
      }
      if ($inBucket) {
        if ($bucketMembers.Count -eq 2 -and (($bucketMembers[0] -eq "$($bucketMembers[1])No2D") -or ($bucketMembers[1] -eq "$($bucketMembers[0])No2D"))) { $expectedBuckets++ }
        else { $unexpectedBuckets += ($bucketMembers -join ' + ') }
      }

      if ($unexpectedBuckets.Count -gt 0) {
        Write-Host "  [WARN] Overlapping parameters that are NOT an effect-selector pair - two parameters share a byte:" -ForegroundColor Yellow
        foreach ($entry in ($unexpectedBuckets | Select-Object -Unique)) { Write-Host "    $entry" -ForegroundColor Yellow }
      }
      elseif ($expectedBuckets -gt 0) {
        Write-Host "  [OK] Overlapping parameters: $expectedBuckets effect-selector pair(s), none unexpected" -ForegroundColor Green
      }
    }

    if ($testResult.Success) {
      Write-Host ""
      Write-Host ""
      Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
      Write-Host "  SUCCESS - all validations passed through the OpenKNXproducer build" -ForegroundColor Cyan
      Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
      Write-Host ""
      Write-Host "  [OK] Internal and OpenKNXproducer validations passed!" -ForegroundColor Green
      Write-Host "  [OK] knxprod.h and NeoPixel.Segment.xml are valid!" -ForegroundColor Green

      # Remove old error log from previous failed builds (cleanup)
      $errorLogPath = "src/Build-EffectParameters.err"
      if (Test-Path $errorLogPath) {
        Remove-Item $errorLogPath -Force
        Write-ScriptVerbose "Removed old error log: $errorLogPath" "DarkGray"
      }

      Write-Host ""
      Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
      Write-Host "  Generation Complete" -ForegroundColor Cyan
      Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
      Write-Host ""
      Write-Host "  Effect Summary" -ForegroundColor Cyan
      Write-Host "  ────────────────────────────────────────────────────────────────────────────────────────────────────────────────────" -ForegroundColor DarkGray
      Write-Host ("  {0,-25} {1,4} {2,7}  {3}" -f "Effect Name", "ID", "Params", "Parameter Names") -ForegroundColor White
      Write-Host ("  {0,-25} {1,4} {2,7}  {3}" -f ("─" * 25), ("─" * 4), ("─" * 7), ("─" * 70)) -ForegroundColor DarkGray

      foreach ($effect in $effects) {
        $paramNames = ($effect.Parameters | ForEach-Object { $_.Name }) -join ", "
        if ($paramNames.Length -gt 70) {
          $paramNames = $paramNames.Substring(0, 67) + "..."
        }

        Write-Host ("  {0,-25}" -f $effect.Name) -NoNewline -ForegroundColor Cyan
        Write-Host (" {0,4}" -f $effect.EffectID) -NoNewline -ForegroundColor White
        Write-Host (" {0,7}" -f $effect.Parameters.Count) -NoNewline -ForegroundColor Green
        Write-Host ("  {0}" -f $paramNames) -ForegroundColor DarkGray
      }

      Write-Host "  " -NoNewline
      Write-Host ("─" * 116) -ForegroundColor DarkGray
      Write-Host "  Parsed " -NoNewline -ForegroundColor Green
      Write-Host "$($effects.Count)" -NoNewline -ForegroundColor White
      Write-Host " effect(s) with " -NoNewline -ForegroundColor Green
      Write-Host "$totalParams" -NoNewline -ForegroundColor White
      Write-Host " total parameter(s)" -ForegroundColor Green
      Write-Host ""
      Write-Host "  Generated Files:" -ForegroundColor Cyan
      $boxLine = "  ┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐"
      Write-Host $boxLine -ForegroundColor DarkGray

      # Calculate box width dynamically (Total - 4 for "  │ " - 2 for " │")
      $boxTotalWidth = $boxLine.Length
      $boxContentWidth = $boxTotalWidth - 6
      $labelWidth = 18  # Fixed width for all labels
      $label1 = "ParameterTypes:".PadRight($labelWidth)
      $label2 = "Union Parameters:".PadRight($labelWidth)
      $label3 = "ParameterRefs:".PadRight($labelWidth)
      $label4 = "Dynamic Choose:".PadRight($labelWidth)
      $label5 = "C++ Param Mapping:".PadRight($labelWidth)
      $label6 = "C++ Type Mapping:".PadRight($labelWidth)

      Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
      Write-Host ($label1 + "injected into $($script:Config.ShareXml)").PadRight($boxContentWidth) -NoNewline -ForegroundColor White
      Write-Host " │" -ForegroundColor DarkGray

      Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
      Write-Host ($label2 + $unionParamsPath).PadRight($boxContentWidth) -NoNewline -ForegroundColor White
      Write-Host " │" -ForegroundColor DarkGray

      Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
      Write-Host ($label3 + $paramRefsPath).PadRight($boxContentWidth) -NoNewline -ForegroundColor White
      Write-Host " │" -ForegroundColor DarkGray

      Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
      Write-Host ($label4 + $dynamicChoosePath).PadRight($boxContentWidth) -NoNewline -ForegroundColor White
      Write-Host " │" -ForegroundColor DarkGray

      Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
      Write-Host ($label5 + $cppMappingPath).PadRight($boxContentWidth) -NoNewline -ForegroundColor White
      Write-Host " │" -ForegroundColor DarkGray

      Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
      Write-Host ($label6 + $cppMappingHeaderPath).PadRight($boxContentWidth) -NoNewline -ForegroundColor White
      Write-Host " │" -ForegroundColor DarkGray

      Write-Host "  └──────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘" -ForegroundColor DarkGray
      Write-Host ""

      # Summary Statistics
      $totalHelpFiles = ($effects | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum

      # Count placeholders
      $placeholderCount = 0
      foreach ($effect in $effects) {
        foreach ($param in $effect.Parameters) {
          if (-not $param.Description) {
            $placeholderCount++
          }
        }
      }

      Write-Host "  Ready for ETS Import:" -ForegroundColor Cyan
      Write-Host "  ────────────────────────────────────────────────────────────────────────────────────────────────────────────────────" -ForegroundColor DarkGray
      Write-Host ""
      Write-Host "  Total effects with parameters:    " -NoNewline -ForegroundColor Gray
      Write-Host "$($effects.Count)" -ForegroundColor White

      Write-Host "  Help files generated:             " -NoNewline -ForegroundColor Gray
      Write-Host "$totalHelpFiles" -NoNewline -ForegroundColor White
      if ($placeholderCount -gt 0) {
        Write-Host " ($placeholderCount with placeholders)" -ForegroundColor Gray
      }
      else {
        Write-Host ""
      }

      Write-Host ""

      # Show warning for effects missing getParameterDescription()
      if ($effectsWithoutDesc.Count -gt 0) {
        Write-Host "  WARNING " -NoNewline -ForegroundColor Yellow
        Write-Host "[!] " -NoNewline -ForegroundColor Yellow
        Write-Host "- $placeholderCount Help files with placeholders generated." -ForegroundColor Yellow
        Write-Host "  Effect(s) missing getParameterDescription():" -ForegroundColor Yellow

        # Show first 3 effects
        $showCount = [Math]::Min(3, $effectsWithoutDesc.Count)
        for ($i = 0; $i -lt $showCount; $i++) {
          Write-Host "    • " -NoNewline -ForegroundColor DarkGray
          Write-Host "$($effectsWithoutDesc[$i])" -ForegroundColor White
        }

        if ($effectsWithoutDesc.Count -gt 3) {
          Write-Host "    ... and " -NoNewline -ForegroundColor DarkGray
          Write-Host "$($effectsWithoutDesc.Count - 3)" -NoNewline -ForegroundColor White
          Write-Host " more" -ForegroundColor DarkGray
        }
        Write-Host "  Please implement getParameterDescription() in these effects to provide proper descriptions." -ForegroundColor Yellow
        Write-Host ""
      }

      Write-Host "  ────────────────────────────────────────────────────────────────────────────────────────────────────────────────────" -ForegroundColor DarkGray
      Write-Host ""
      Show-OpenKNXLogo -SubTitle "EffectParameters Build Script by Erkan Çolak" -Version $SCRIPT_VERSION
    }
    else {
      # OpenKNXproducer failed -> ROLLBACK!
      Write-Host ""
      Write-Host "========================================" -ForegroundColor Red
      Write-Host "  ERROR - OpenKNXproducer validation FAILED" -ForegroundColor Red
      Write-Host "========================================" -ForegroundColor Red
      Write-Host ""
      Write-Host "Rolling back changes..." -ForegroundColor Yellow
      Write-Host ""

      # 1. Restore template from backup
      if (Test-Path "$templatePath.backup") {
        Restore-Template -TemplatePath $templatePath
      }
      else {
        # Backup doesn't exist -> Clear marker regions instead
        Clear-MarkerRegions -TemplatePath $templatePath -NeoPixelXmlPath $neoPixelXmlPath
      }

      # 2. Save error log
      $errorLogPath = "src/Build-EffectParameters.err"
      $errorLog = @"
============================================================
OpenKNXproducer Validation Error
Generated: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Exit Code: $($testResult.ExitCode)
============================================================

OpenKNXproducer failed with exit code $($testResult.ExitCode).
Check the console output above for detailed error messages.

Common issues:
  - Missing ParameterType definitions
  - ID collisions or duplicates
  - Invalid XML syntax
  - Missing HelpContext files
  - Incorrect ParameterRefRef attribute order

============================================================
Generated files (review manually):
  - ParameterTypes injected into $($script:Config.ShareXml)
  - $unionParamsPath
  - $paramRefsPath
  - $dynamicChoosePath
  - $cppMappingPath

Template has been restored to previous state.
Fix the errors shown in the console output and run the script again.
============================================================
"@
      Set-Content -Path $errorLogPath -Value $errorLog -Encoding UTF8

      Write-Host "  [OK] Template restored" -ForegroundColor Green
      Write-Host "  [SAVED] Error log: " -NoNewline -ForegroundColor Yellow
      Write-Host "$errorLogPath" -ForegroundColor Gray
      Write-Host ""
      Write-Host "Please review the error log and fix the issues." -ForegroundColor Yellow
      Write-Host "Generated files are preserved for debugging." -ForegroundColor DarkGray
      Write-Host ""

      exit 1
    }
  }
  else {
    Write-Host ""
    Write-Host "Template update failed validation" -ForegroundColor Red
    Write-Host "Check the errors above and review generated files manually." -ForegroundColor Yellow
    Write-Host ""
    exit 1
  }
}
catch {
  Write-Error "Failed to update template: $_"
  exit 1
}
