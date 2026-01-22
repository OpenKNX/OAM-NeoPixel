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
    [Parameter(Mandatory=$false)]
    [ValidateSet("de", "en")]
    [string]$Language = "de",

    [Parameter(Mandatory=$false)]
    [switch]$Clean = $false,

    [Parameter(Mandatory=$false)]
    [switch]$Force = $false,

    [Parameter(Mandatory=$false)]
    [switch]$SkipOpenKNXproducer = $false
)

$ErrorActionPreference = "Stop"
$SCRIPT_VERSION = "0.1"

# ====================================================================
# Configuration - Central place for all magic numbers and paths
# ====================================================================
$script:Config = @{
    # Effect Parameter IDs (FIXED - referenced in XML comments)
    EffectParameterStartId = 73
    EffectParameterStartOffset = 30

    # Application/Module Numbers
    ApplicationNumber = 40
    ModuleType = 40

    # Parameter Type Defaults
    DefaultMaxUint8 = 255
    DefaultMaxPercent = 100

    # Relative paths from repo root
    EffectPoolPath = "lib/OFM-NeoPixel/src/effects/EffectPool.cpp"
    EffectsDir = "lib/OFM-NeoPixel/src/effects"
    SrcDir = "src"
    HelpDir = "src/Baggages/Help_de"
    IncludeDir = "include"

    # XML Templates
    SegmentTemplate = "src/NeoPixel.Segment.templ.xml"
    StripTemplate = "src/NeoPixel.Strip.templ.xml"
    ShareXml = "src/NeoPixel.share.xml"
    MainXml = "src/NeoPixel.xml"

    # Generated XML files
    ParameterTypesXml = "src/NeoPixel.Effects.ParameterTypes.generated.xml"
    UnionParametersXml = "src/NeoPixel.Effects.UnionParameters.generated.xml"
    ParameterRefsXml = "src/NeoPixel.Effects.ParameterRefs.generated.xml"
    DynamicChooseXml = "src/NeoPixel.Effects.DynamicChoose.generated.xml"

    # C++ Headers and includes
    KnxProdHeader = "include/knxprod.h"
    CppHeader = "src/EffectParameterMapping.h"
    CppHeaderName = "EffectParameterMapping.h"
    EffectBaseHeader = "../lib/OFM-NeoPixel/src/effects/Effect.h"
    SegmentHeader = "../lib/OFM-NeoPixel/src/Segment.h"

    # File names and extensions
    EffectHeaderPattern = "*.h"
    MarkdownPattern = "*.md"
    EffectHeaderSuffix = "Effect.h"
    EffectHeaderPrefix = "Effect"

    # Effect naming patterns (for matching both conventions)
    EffectNamePattern1 = "*Effect.h"   # BPMEffect.h, CylonEffect.h
    EffectNamePattern2 = "Effect*.h"   # EffectSolid.h, EffectWipe.h

    # Special files to exclude
    ExcludeHeaders = @("Effect.h", "EffectPool.h")

    # File patterns for cleanup
    GeneratedXmlPattern = "NeoPixel.Effects.*.generated.xml"
    PartXmlPattern = "NeoPixel.Effects.*.part.xml"

    # Help file naming
    HelpFilePrefix = "NEO-"
    HelpFileSuffix = ".md"

    # XML Markers
    Markers = @{
        UnionStart = "<!-- BEGIN AUTO-GENERATED: Effect Parameters Union -->"
        UnionEnd = "<!-- END AUTO-GENERATED: Effect Parameters Union -->"
        ParamRefsStart = "<!-- BEGIN AUTO-GENERATED: Effect ParameterRefs -->"
        ParamRefsEnd = "<!-- END AUTO-GENERATED: Effect ParameterRefs -->"
        DynamicUIStart = "<!-- BEGIN AUTO-GENERATED: Effect Dynamic UI -->"
        DynamicUIEnd = "<!-- END AUTO-GENERATED: Effect Dynamic UI -->"
        ModuleStart = "<!-- BEGIN AUTO-GENERATED: NEOEFF Module -->"
        ModuleEnd = "<!-- END AUTO-GENERATED: NEOEFF Module -->"
        EnumStart = "<!-- GENERATED_EFFECT_ENUMERATIONS_START -->"
        EnumEnd = "<!-- GENERATED_EFFECT_ENUMERATIONS_END -->"
    }

    # String cleaning/replacement patterns
    CleanPatterns = @{
        # Remove "Effect" suffix
        RemoveEffectSuffix = 'Effect$'
        # Remove all non-alphanumeric characters
        AlphanumericOnly = '[^a-zA-Z0-9]'
        # Remove spaces and hyphens
        RemoveSpacesHyphens = '[-\s]'
        # Replace spaces with hyphens
        SpaceToHyphen = '\s+'
    }

    # Regex Patterns for C++ Parsing
    Patterns = @{
        # Effect class name - supports both naming conventions: *Effect and Effect*
        ClassName = 'class\s+((?:\w+Effect)|(?:Effect\w+))\s*:'

        # getName() with EFFECT_NAME_DE_EN macro
        GetNameMacro = 'getName\s*\([^)]*\)\s*override[\s\S]*?return\s+EFFECT_NAME_DE_EN\s*\(\s*"([^"]+)"\s*,\s*"([^"]+)"\s*\)\s*;'

        # getName() with plain string - supports both inline {return "X";} and multi-line return
        GetNamePlain = 'getName\s*\([^)]*\)\s*override\s*\{?\s*return\s*"([^"]+)"\s*;'

        # getParameterCount() return value
        ParameterCount = 'getParameterCount\s*\(\s*\)\s*(?:const\s+)?(?:override\s+)?{[^}]*return\s+(\d+)'

        # getParameterName case statement
        ParameterName = '(?s)getParameterName.*?case\s+{0}\s*:\s*return\s+"([^"]+)"'

        # EffectPool.cpp getEffectByIndex() function
        EffectPoolFunction = 'Effect\*\s+EffectPool::getEffectByIndex\([^)]+\)\s*\{(.*?)\n}'

        # return getXXX() in EffectPool
        EffectPoolReturn = 'return\s+get(\w+)\(\)'

        # Filename pattern: EffectXXX → XXXEffect
        FilenameToClass = '^Effect(.+)$'

        # Parameter type matching
        ParamTypeUint8 = 'PARAM_UINT8|PARAM_HUE'

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
    } elseif ($PSCommandPath) {
        Split-Path -Parent $PSCommandPath
    } else {
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
    } elseif ($SubTitle) {
        $line2 = "$($unicodeString)$($SubTitle)"
    } else {
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
            $headerContent = Get-Content $header.FullName -Raw

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
    } else {
        Write-Host "  ERROR: Paths not found!" -ForegroundColor Red
        Write-Host "  helpDir: $helpDir → Exists: $(Test-Path $helpDir)" -ForegroundColor Yellow
        Write-Host "  effectsDir: $effectsDir → Exists: $(Test-Path $effectsDir)" -ForegroundColor Yellow
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
    Write-Host "    • Empty Effect Type Enumeration in share.xml" -ForegroundColor DarkGray

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
        } catch {
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
            $content = Get-Content $templateFile -Raw

            # Empty Union Parameters block - keep important comments
            $pattern1 = "(?s)($([regex]::Escape($script:Config.Markers.UnionStart))\r?\n)(.*?)($([regex]::Escape($script:Config.Markers.UnionEnd)))"
            $replacement1 = '$1                <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->' + [Environment]::NewLine +
                           '                <!-- Start ID: 73, Start Offset: 30 (FIXED - do not change!) -->' + [Environment]::NewLine +
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

            Set-Content -Path $templateFile -Value $content -NoNewline
            Write-Host "    ✓ NeoPixel.Segment.templ.xml - marker blocks emptied" -ForegroundColor Green
        }

        # 2. Empty NEOEFF Module in NeoPixel.xml
        $neoPixelXml = Resolve-RepoPath $script:Config.MainXml
        if (Test-Path $neoPixelXml) {
            $content = Get-Content $neoPixelXml -Raw

            # Empty NEOEFF Module block - keep important comment
            $pattern4 = "(?s)($([regex]::Escape($script:Config.Markers.ModuleStart))\r?\n)(.*?)($([regex]::Escape($script:Config.Markers.ModuleEnd)))"
            $replacement4 = '$1  <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->' + [Environment]::NewLine +
                           '  $3'
            $content = $content -replace $pattern4, $replacement4

            Set-Content -Path $neoPixelXml -Value $content -NoNewline
            Write-Host "    ✓ NeoPixel.xml - NEOEFF Module removed" -ForegroundColor Green
        }

        # 3. Empty Effect Type Enumeration in NeoPixel.share.xml
        $shareXmlPath = Resolve-RepoPath $script:Config.ShareXml
        if (Test-Path $shareXmlPath) {
            $content = Get-Content $shareXmlPath -Raw

            # Empty Effect Enumeration block - keep markers
            $pattern5 = "(?s)($([regex]::Escape($script:Config.Markers.EnumStart))\r?\n)(.*?)($([regex]::Escape($script:Config.Markers.EnumEnd)))"
            $replacement5 = '$1                <!-- Content will be generated by Build-EffectParameters.ps1 -->' + [Environment]::NewLine +
                           '                $3'
            $content = $content -replace $pattern5, $replacement5

            Set-Content -Path $shareXmlPath -Value $content -NoNewline
            Write-Host "    ✓ NeoPixel.share.xml - Effect Type Enumeration emptied" -ForegroundColor Green
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
            Set-Content -Path $effectTypeMappingPath -Value $placeholderContent -NoNewline
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
            Set-Content -Path $effectParamMappingPath -Value $placeholderContent -NoNewline
            Write-Host "    ✓ EffectParameterMapping.h - Emptied with placeholder" -ForegroundColor Green
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

    $content = Get-Content -LiteralPath $HeaderPath -Raw

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

    $content = Get-Content -Path $effectPoolPath -Raw

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
        } else {
            "${effectBaseName}Effect"  # → BPMEffect, CylonEffect
        }

        $effectOrder += $effectClassName
        Write-ScriptVerbose "  Registration [$($effectOrder.Count - 1)]: $effectClassName" "DarkGray"
    }

    if ($effectOrder.Count -eq 0) {
        Write-Warning "No effects found in getEffectByIndex(). Pattern might need adjustment."
    } else {
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
                NameDE = $names.DE
                NameEN = $names.EN
                FilePath = $header.FullName
            }
            Write-ScriptVerbose "  Parsed $className → DE:'$($names.DE)' EN:'$($names.EN)'" "DarkGray"
        } catch {
            Write-Warning "Could not parse effect $className from $($header.Name): $_"
        }
    }

    Write-ScriptVerbose "Found $($map.Count) effect headers with names"
    Write-ScriptVerbose "Second pass: Assigning IDs based on EffectPool.cpp registration order"

    # Second pass: Assign IDs based on EffectPool.cpp registration order
    # This ensures XML IDs match runtime indices exactly!
    $registrationOrder = Parse-EffectPoolRegistrationOrder

    if ($registrationOrder.Count -eq 0) {
        Write-Error "Failed to parse EffectPool.cpp registration order! Falling back to alphabetical sort."
        $registrationOrder = $map.Keys | Sort-Object
    }

    $effectIds = @{}
    $id = 0

    foreach ($effectClassName in $registrationOrder) {
        if ($map.ContainsKey($effectClassName)) {
            $effectIds[$effectClassName] = $id
            $effectName = $map[$effectClassName].NameEN
            Write-ScriptVerbose "  ID $id → $effectClassName ('$effectName')" "Green"
            $id++
        } else {
            Write-Warning "Effect $effectClassName found in EffectPool.cpp but no header file found!"
            Write-ScriptVerbose "  Available effect classes: $($map.Keys -join ', ')" "Yellow"
        }
    }

    Write-ScriptVerbose "ID mapping complete: $($effectIds.Count) effects assigned IDs 0-$($id - 1)" "Green"

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
                NameDE = $names.DE
                NameEN = $names.EN
            }
            Write-Host "    ✓ " -NoNewline -ForegroundColor Green
            Write-Host "$className".PadRight(30) -NoNewline -ForegroundColor Gray
            if ($names.DE -ne $names.EN) {
                Write-Host "→ DE: '$($names.DE)' | EN: '$($names.EN)'" -ForegroundColor White
            } else {
                Write-Host "→ '$($names.EN)' (no translation)" -ForegroundColor White
            }
            Write-ScriptVerbose "  ${className}: DE='$($names.DE)' EN='$($names.EN)'" "DarkGray"
        } catch {
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
    } else {
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

    # Generate XML enumeration entries - use German as default
    Write-ScriptVerbose "Generating XML enumeration entries"
    $xmlLines = @()
    $id = 0

    foreach ($effect in $sorted) {
        $text = $effect.NameDE
        # Add comment with English name if different
        if ($effect.NameDE -ne $effect.NameEN) {
            $xmlLines += "                <Enumeration Text=`"$text`" Value=`"$id`" Id=`"%ENID%`"/> <!-- EN: $($effect.NameEN) -->"
        } else {
            $xmlLines += "                <Enumeration Text=`"$text`" Value=`"$id`" Id=`"%ENID%`"/>"
        }
        Write-ScriptVerbose "  XML Entry ID ${id}: '$text'" "DarkGray"
        $id++
    }

    Write-Host ""
    Write-Host "  Generated $($xmlLines.Count) effect enumerations (IDs 0-$($id-1))" -ForegroundColor Green
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
        $content = Get-Content $file.FullName -Raw

        # Extract class name
        if ($content -match $script:Config.Patterns.ClassName) {
            $className = $Matches[1]
            $names = Get-EffectDisplayNameFromHeader -HeaderPath $file.FullName

            $effectList += @{
                ClassName = $className
                NameDE = $names.DE
                NameEN = $names.EN
                FilePath = $file.FullName
            }
        }
    }

    # Sort by EffectPool.cpp registration order
    $registrationOrder = Parse-EffectPoolRegistrationOrder

    if ($registrationOrder.Count -eq 0) {
        Write-Warning "Failed to parse EffectPool.cpp! Falling back to alphabetical sort"
        $sorted = $effectList | Sort-Object NameEN
    } else {
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
    $currentYear = (Get-Date).Year
    $currentDateTime = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
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
    $cppLines += " * @note Generated: $currentDateTime"
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
    $cppLines += "    switch (effectType)"
    $cppLines += "    {"

    # Generate switch cases
    $id = 0
    foreach ($effect in $sorted) {
        # Convert ClassName to getXXX() function name
        # BPMEffect -> getBPM()
        # EffectSolid -> getSolid()
        $methodName = $effect.ClassName -replace 'Effect', ''

        $comment = "// $($effect.NameEN)"
        if ($effect.NameDE -ne $effect.NameEN) {
            $comment += " (DE: $($effect.NameDE))"
        }

        $cppLines += "        case ${id}: return EffectPool::get${methodName}(); $comment"
        $id++
    }

    # Default case
    $cppLines += "        default: return EffectPool::getSolid(); // Fallback to Solid"
    $cppLines += "    }"
    $cppLines += "}"

    Write-Host ""
    Write-Host "  Generated C++ mapping for $($sorted.Count) effects (IDs 0-$($id-1))" -ForegroundColor Green
    Write-ScriptVerbose "C++ effect mapping generation complete" "Green"
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
    $xmlContent = Get-Content $ShareXmlPath -Raw

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

    Write-Host "  ✓ NeoPixel.share.xml updated successfully!" -ForegroundColor Green
    Write-ScriptVerbose "Effect type enumeration update complete" "Green"
    Write-Host ""
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

    $content = Get-Content -Path $HeaderPath -Raw

    # Extract effect class name
    if ($content -match $script:Config.Patterns.ClassName) {
        $effectName = $Matches[1]
        Write-ScriptVerbose "  Class name: $effectName" "DarkGray"
    } else {
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
    } else {
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

        # Set sensible defaults for Min/Max if not defined (both are 0)
        # For UINT8/HUE types, use full range 0-255 unless explicitly limited
        if ($paramMin -eq 0 -and $paramMax -eq 0) {
            if ($paramType -match $script:Config.Patterns.ParamTypeUint8) {
                $paramMax = $script:Config.DefaultMaxUint8
            } elseif ($paramType -eq 'PARAM_PERCENT') {
                $paramMax = $script:Config.DefaultMaxPercent
            }
        }

        if ($paramName) {
            Write-Host "       [$i] $paramName ($paramType) = $paramDefault [$paramMin..$paramMax]" -ForegroundColor DarkGray
            Write-ScriptVerbose "    [$i] ${paramName}: type=$paramType default=$paramDefault range=[$paramMin..$paramMax]" "DarkGray"
            $parameters += @{
                Index = $i
                Name = $paramName
                Description = $paramDesc
                Type = $paramType
                Default = $paramDefault
                Min = $paramMin
                Max = $paramMax
            }
        }
    }

    $effectId = Get-EffectId -EffectName $effectName

    # Clean effect name for file naming (remove "Effect" suffix, spaces, hyphens)
    $cleanName = $effectName -replace $script:Config.CleanPatterns.RemoveEffectSuffix, ''
    $cleanName = $cleanName -replace $script:Config.CleanPatterns.AlphanumericOnly, ''

    Write-ScriptVerbose "Parsed effect: $cleanName (ID: $effectId, $($parameters.Count) params)" "Green"

    return @{
        Name = $cleanName
        ClassName = $effectName
        NameDE = $names.DE
        NameEN = $names.EN
        EffectID = $effectId
        Parameters = $parameters
    }
}

function Generate-ParameterTypes {
    param([array]$Effects)

    $xml = @()
    # Format as complete share file for op:define module
    $xml += '<?xml version="1.0" encoding="utf-8"?>'
    $xml += '<!-- AUTO-GENERATED by scripts/Build-EffectParameters.ps1 - DO NOT EDIT MANUALLY! -->'
    $xml += '<KNX xmlns:op="http://github.com/OpenKNX/OpenKNXproducer" xmlns="http://knx.org/xml/project/20">'
    $xml += '  <ManufacturerData>'
    $xml += '    <Manufacturer RefId="M-00FA">'
    $xml += '      <ApplicationPrograms>'
    $xml += "        <ApplicationProgram Id=`"%AID%`" ApplicationNumber=`"$($script:Config.ApplicationNumber)`" ApplicationVersion=`"1`" ProgramType=`"ApplicationProgram`" MaskVersion=`"MV-07B0`" DynamicTableManagement=`"false`">"
    $xml += '          <Static>'
    $xml += '            <ParameterTypes>'

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

            $xml += "<ParameterType Id=`"$ptId`" Name=`"$paramNameWithEffect`">"

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
                default {
                    # Fallback: UINT8
                    $xml += "                <TypeNumber SizeInBit=`"8`" Type=`"unsignedInt`" minInclusive=`"$($param.Min)`" maxInclusive=`"$($param.Max)`"/>"
                }
            }

            $xml += '              </ParameterType>'
        }
    }

    $xml += '            </ParameterTypes>'
    $xml += '          </Static>'
    $xml += '        </ApplicationProgram>'
    $xml += '      </ApplicationPrograms>'
    $xml += '    </Manufacturer>'
    $xml += '  </ManufacturerData>'
    $xml += '</KNX>'

    return $xml -join "`n"
}

function Update-NeoEffModule {
    param(
        [string]$NeoPixelXmlPath,
        [bool]$Enable
    )

    $content = Get-Content $NeoPixelXmlPath -Raw

    if ($Enable) {
        Write-Host "  • Adding NEOEFF module to NeoPixel.xml..." -ForegroundColor Cyan

        $neoeffModule = @"

  <!-- Effect Parameter Types - MUST be loaded BEFORE NEO module! -->
  <op:define prefix="NEOEFF" ModuleType="$($script:Config.ModuleType)"
    share="NeoPixel.Effects.ParameterTypes.generated.xml">
  </op:define>
"@

        # Insert NEOEFF module between markers
        $content = $content -replace `
            "(?s)($([regex]::Escape($script:Config.Markers.ModuleStart)).*?<!-- DO NOT REMOVE THIS MARKER[^>]*-->).*?($([regex]::Escape($script:Config.Markers.ModuleEnd)))",`
            "`$1$neoeffModule`n  `$2"
    } else {
        Write-Host "  • Removing NEOEFF module from NeoPixel.xml..." -ForegroundColor Yellow

        # Clear content between markers
        $content = $content -replace `
            "(?s)($([regex]::Escape($script:Config.Markers.ModuleStart)).*?<!-- DO NOT REMOVE THIS MARKER[^>]*-->).*?($([regex]::Escape($script:Config.Markers.ModuleEnd)))",`
            "`$1`n  `$2"
    }

    Set-Content -Path $NeoPixelXmlPath -Value $content -NoNewline
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

    # Run OpenKNXproducer WITHOUT redirecting output (show in real-time)
    # BUT capture exit code
    try {
        Push-Location $WorkingDir

        Write-Information "Starting OpenKNXproducer in directory: $WorkingDir with Argutments: create --Debug -h $HeaderFile $SourceDir" -InformationAction Continue
        # Execute without redirect - output goes directly to console
        $process = Start-Process -FilePath $openKnxExe `
            -ArgumentList "create", "--Debug", "-h", $HeaderFile, $SourceDir `
            -NoNewWindow -Wait -PassThru

        $exitCode = $process.ExitCode

        Pop-Location

        Write-Host "  " -NoNewline
        Write-Host ("═" * 114) -ForegroundColor DarkGray
        Write-Host ""

        if ($exitCode -eq 0) {
            Write-Host "  " -NoNewline
            Write-Host "OK" -NoNewline -ForegroundColor Green
            Write-Host " OpenKNXproducer succeeded (Exit Code: 0)" -ForegroundColor White
            return @{
                Success = $true
                ExitCode = $exitCode
                Output = "(output shown above)"
            }
        } else {
            Write-Host "  ERROR OpenKNXproducer failed with exit code $exitCode" -ForegroundColor Red
            return @{
                Success = $false
                ExitCode = $exitCode
                Output = "(output shown above)"
            }
        }
    } catch {
        Pop-Location
        Write-Host "  ERROR Failed to run OpenKNXproducer: $_" -ForegroundColor Red
        return @{
            Success = $false
            ExitCode = -1
            Output = $_.Exception.Message
        }
    }
}

function Clear-MarkerRegions {
    param(
        [string]$TemplatePath,
        [string]$NeoPixelXmlPath
    )

    Write-Host "  • Clearing marker regions (keeping markers intact)..." -ForegroundColor Yellow

    $content = Get-Content $TemplatePath -Raw

    # Clear Union Parameters region
    $content = $content -replace `
        "(?s)($([regex]::Escape($script:Config.Markers.UnionStart)).*?<!-- Start ID: \d+, Start Offset: \d+ \(FIXED[^)]*\) -->.*?<!-- Generated:[^>]*-->).*?($([regex]::Escape($script:Config.Markers.UnionEnd)))",`
        "`$1`n`$2"

    # Clear ParameterRefs region
    $content = $content -replace `
        "(?s)($([regex]::Escape($script:Config.Markers.ParamRefsStart)).*?<!-- Generated:[^>]*-->).*?($([regex]::Escape($script:Config.Markers.ParamRefsEnd)))",`
        "`$1`n`$2"

    # Clear Dynamic UI region
    $content = $content -replace `
        "(?s)($([regex]::Escape($script:Config.Markers.DynamicUIStart)).*?<!-- Effect-specific[^>]*-->).*?($([regex]::Escape($script:Config.Markers.DynamicUIEnd)))",`
        "`$1`n`$2"

    Set-Content -Path $TemplatePath -Value $content -Encoding UTF8 -NoNewline
    Write-Host "    OK Marker regions cleared (markers preserved)" -ForegroundColor Green

    # Clear NEOEFF module
    Update-NeoEffModule -NeoPixelXmlPath $NeoPixelXmlPath -Enable $false
    Write-Host "    OK NEOEFF module removed" -ForegroundColor Green
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
            Xml = ($xml -join "`n")
            NextId = $StartId
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

            # Generate Parameter element
            $xml += "                <Parameter Id=`"$paramId`" Offset=`"$currentOffset`" BitOffset=`"$bitOffset`" Name=`"$paramName`" ParameterType=`"$ptId`" Text=`"$($param.Name) ($($effect.NameDE))`" Value=`"$($param.Default)`"/>"
            Write-ScriptVerbose "    Param ID ${currentId} @ offset ${currentOffset}: $($param.Name)" "DarkGray"
            Write-Host "      + Param ${paramIdNum}: $($param.Name) @ Offset $currentOffset" -ForegroundColor DarkGray

            $currentId++
            $currentOffset++
        }
        $xml += ''
    }

    Write-Host "    OK Generated $($currentId - $StartId) parameters" -ForegroundColor Green

    return @{
        Xml = ($xml -join "`n")
        NextId = $currentId
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
            $xml += "              <ParameterRef Id=`"%AID%_UP-%TT%%CC%${paramIdNum}_R-%TT%%CC%${paramIdNum}01`" RefId=`"%AID%_UP-%TT%%CC%$paramIdNum`" />"

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

    $xml += '                    <ParameterSeparator Id="%AID%_PS-nnn" Text="Effekt-spezifische Parameter" UIHint="Headline" />'
    $xml += '                    <choose ParamRefId="%AID%_UP-%TT%%CC%057_R-%TT%%CC%05701">'

    $currentId = $StartId

    foreach ($effect in $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID) {
        $xml += "                      <when test=`"$($effect.EffectID)`">"
        $xml += "                        <!-- $($effect.NameDE) Effect Parameters -->"

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
            } else {
                $helpContent += "Parameter zur Steuerung von '$($param.Name)' für den $($effect.NameDE)-Effekt."
            }
            $helpContent += ""
            $helpContent += "**Wertebereich:** $($param.Min) - $($param.Max)"
            $helpContent += "**Standardwert:** $($param.Default)"

            Set-Content -Path $helpFilePath -Value ($helpContent -join "`n") -Encoding UTF8

            # Track created file with source info
            $source = if ($param.Description) { "aus Header" } else { "Platzhalter" }
            $createdFiles += [PSCustomObject]@{
                FileName = $helpFileName
                Source = $source
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
        Count = $createdFiles.Count
        EffectsWithoutDescription = $effectsWithoutDesc
    }
}

function Read-StartingIdsFromMarkers {
    param([string]$TemplatePath)

    Write-Host "   Reading starting IDs from template markers..." -ForegroundColor Gray

    if (-not (Test-Path $TemplatePath)) {
        throw "Template file not found: $TemplatePath"
    }

    $templateContent = Get-Content -Path $TemplatePath -Raw

    # Extract starting ID and Offset from Union marker comments
    # Expected format: <!-- Start ID: 073, Start Offset: 30 -->
    $unionMarker = $script:Config.Markers.UnionStart

    if ($templateContent -match "(?s)$([regex]::Escape($unionMarker)).*?Start ID:\s*(\d+).*?Start Offset:\s*(\d+)") {
        $startId = [int]$Matches[1]
        $startOffset = [int]$Matches[2]

        Write-Host "    OK Union: Start ID=$startId, Start Offset=$startOffset" -ForegroundColor DarkGray

        return @{
            StartId = $startId
            StartOffset = $startOffset
        }
    } else {
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
    } catch {
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

    $templateContent = Get-Content -Path $TemplatePath -Raw
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

    # 5. Check that generated IDs are in expected range
    $generatedParamCount = ($Effects | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum
    $expectedMaxId = $StartId + $generatedParamCount - 1

    foreach ($paramId in $paramIds) {
        $id = [int]$paramId
        if ($id -ge $StartId -and $id -gt $expectedMaxId) {
            $warnings += "Parameter ID $id is beyond expected range (StartId=$StartId, Expected Max=$expectedMaxId)"
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
        $templateContent = Get-Content -Path $TemplatePath -Raw
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
        } else {
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
        } else {
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
        } else {
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
            } else {
                Write-Warning "Validation failed! Restoring backup..."
                Restore-Template -TemplatePath $TemplatePath -BackupPath $backupPath
                return $false
            }
        }

        throw "Not all blocks were updated (expected 3, got $updateCount)"

    } catch {
        Write-Error "Error updating template: $_"
        Restore-Template -TemplatePath $TemplatePath -BackupPath $backupPath
        return $false
    }
}

function Generate-CppMapping {
    param([array]$Effects)

    $currentYear = (Get-Date).Year
    $currentDateTime = Get-Date -Format "yyyy-MM-dd HH:mm:ss"

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
    $cpp += " * @note Generated: $currentDateTime"
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
    $cpp += '    switch (effectID)'
    $cpp += '    {'

    foreach ($effect in $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID) {
        $cpp += "        case $($effect.EffectID):  // $($effect.NameDE) Effect"

        # Effect name for macro (remove spaces/hyphens)
        $effectNameClean = $effect.NameDE -replace $script:Config.CleanPatterns.RemoveSpacesHyphens, ''

        for ($i = 0; $i -lt $effect.Parameters.Count; $i++) {
            $param = $effect.Parameters[$i]
            $paramNameClean = $param.Name -replace $script:Config.CleanPatterns.RemoveSpacesHyphens, ''

            # Macro name must match OpenKNXproducer format: ParamNEO_NEO{EffectName}{ParamName}
            # E.g., ParamNEO_NEORainbowSpeed (from Name="NEO%C%RainbowSpeed")
            $macroName = "ParamNEO_NEO$effectNameClean$paramNameClean"

            $cpp += "            if (paramCount >= $($i + 1)) effect->setParameter(segment, $i, $macroName);"
        }

        $cpp += '            break;'
        $cpp += '            '
    }

    $cpp += '        // Effects without parameters: 0,1,3,8,9,10,11,12,13,14,15,16,17,18,19,20,21'
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
# Main Script
# ====================================================================

#Clear-Host
Show-OpenKNXLogo -SubTitle "Build Effect Parameters" -Version $SCRIPT_VERSION

# Determine script and repository root directories
$scriptDir = if ($PSScriptRoot) {
    $PSScriptRoot
} elseif ($PSCommandPath) {
    Split-Path -Parent $PSCommandPath
} else {
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
} catch {
    Write-Host "  [ERROR] Failed to read starting IDs from template: $_" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please ensure template has correct markers with ID/Offset information!" -ForegroundColor Yellow
    exit 1
}

Write-Host ""
Write-Host "Generating XML content..." -ForegroundColor Cyan
Write-Host ""

# 1. ParameterTypes XML (doesn't need IDs, just definitions)
$paramTypesXml = Generate-ParameterTypes -Effects $effects
$paramTypesPath = "src/NeoPixel.Effects.ParameterTypes.generated.xml"
Set-Content -Path $paramTypesPath -Value $paramTypesXml -Encoding UTF8
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "ParameterTypes XML" -ForegroundColor White
Write-Host "       $paramTypesPath" -ForegroundColor DarkGray

# 1b. Add NEOEFF module to NeoPixel.xml
$neoPixelXmlPath = Resolve-RepoPath $script:Config.MainXml
Update-NeoEffModule -NeoPixelXmlPath $neoPixelXmlPath -Enable $true
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "NEOEFF module added" -ForegroundColor White
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

# 6. Help Files (Markdown)
Write-Host "  ▸ Generating help files..." -ForegroundColor Gray
$helpFilesInfo = Generate-HelpFiles -Effects $effects
$effectsWithoutDesc = $helpFilesInfo.EffectsWithoutDescription
Write-Host "  [OK] " -NoNewline -ForegroundColor Green
Write-Host "Help files generated" -ForegroundColor White

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

        # ============================================================
        # FINAL VALIDATION: Test with OpenKNXproducer (optional)
        # ============================================================
        if (-not $SkipOpenKNXproducer) {
            $testResult = Test-OpenKNXproducer -WorkingDir "." `
                                                -HeaderFile "include/knxprod.h" `
                                                -SourceDir "src/NeoPixel"
        } else {
            Write-Host ""
            Write-Host "  Skipping OpenKNXproducer validation (as requested)" -ForegroundColor Yellow
            $testResult = @{ Success = $true }
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
            Write-Host ($label1 + $paramTypesPath).PadRight($boxContentWidth) -NoNewline -ForegroundColor White
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
            } else {
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
        } else {
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
            } else {
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
  - $paramTypesPath
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
    } else {
        Write-Host ""
        Write-Host "Template update failed validation" -ForegroundColor Red
        Write-Host "Check the errors above and review generated files manually." -ForegroundColor Yellow
        Write-Host ""
        exit 1
    }
} catch {
    Write-Error "Failed to update template: $_"
    exit 1
}
