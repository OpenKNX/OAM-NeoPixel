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
    ./Build-EffectParameters.ps1
    Generates parameters with German descriptions (default)
    
.EXAMPLE
    ./Build-EffectParameters.ps1 -Language "en"
    Generates parameters with English descriptions
    
.NOTES
    This script is part of the OpenKNX project automation framework.
    It integrates with OpenKNXproducer for ETS application generation.
    
.PARAMETER Clean
    Remove all auto-generated files and optionally empty marker blocks in template
    
.PARAMETER EmptyMarkers
    When used with -Clean, also empties the content between AUTO-GENERATED markers in template
    
.EXAMPLE
    ./Build-EffectParameters.ps1 -Clean
    Removes all generated files
    
.EXAMPLE
    ./Build-EffectParameters.ps1 -Clean -EmptyMarkers
    Removes all generated files AND empties marker blocks in template
#>

param(
    [Parameter(Mandatory=$false)]
    [ValidateSet("de", "en")]
    [string]$Language = "de",
    
    [Parameter(Mandatory=$false)]
    [switch]$Clean = $false,
    
    [Parameter(Mandatory=$false)]
    [switch]$EmptyMarkers = $false
)

$ErrorActionPreference = "Stop"

# ====================================================================
# OpenKNX Logo
# ====================================================================

function Show-OpenKNXLogo {
    param([string]$SubTitle, [string]$Version)
    
    Write-Host ""
    Write-Host ("─" * 116) -ForegroundColor DarkGray
    Write-Host "Open " -NoNewline
    Write-Host "$([char]::ConvertFromUtf32(0x25A0))" -ForegroundColor Green
    $unicodeString = "$([char]::ConvertFromUtf32(0x252C))$([char]::ConvertFromUtf32(0x2500))$([char]::ConvertFromUtf32(0x2500))$([char]::ConvertFromUtf32(0x2500))$([char]::ConvertFromUtf32(0x2500))$([char]::ConvertFromUtf32(0x2534)) "
    
    if ($SubTitle -and $Version) {
        Write-Host "$($unicodeString) $($SubTitle) v$Version" -ForegroundColor Green
    } elseif ($SubTitle) {
        Write-Host "$($unicodeString) $($SubTitle)" -ForegroundColor Green
    } else {
        Write-Host "$($unicodeString)" -ForegroundColor Green
    }
    
    Write-Host "$([char]::ConvertFromUtf32(0x25A0)) KNX" -ForegroundColor Green
    Write-Host ("─" * 116) -ForegroundColor DarkGray
    Write-Host ""
}

# ====================================================================
# Clean Mode - Remove generated files
# ====================================================================

if ($Clean) {
    Clear-Host
    
    Show-OpenKNXLogo -SubTitle "EffectParameters Build Script by Erkan Çolak" -Version "0.1"
    
    Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
    Write-Host "  Cleaning Auto-Generated Effect Parameter Files" -ForegroundColor Cyan
    Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
    Write-Host ""
    
    $scriptDir = Split-Path -Parent $PSCommandPath
    $rootDir = Split-Path -Parent $scriptDir
    $srcDir = Join-Path $rootDir "src"
    
    # Collect files to delete
    $filesToDelete = @()
    
    # 1. Collect generated XML fragments
    $xmlFiles = Get-ChildItem -Path $srcDir -Filter "NeoPixel.Effects.*.generated.xml" -ErrorAction SilentlyContinue
    foreach ($file in $xmlFiles) {
        $filesToDelete += [PSCustomObject]@{ Type = "XML Fragment"; Name = $file.Name; Path = $file.FullName }
    }
    
    # 2. Collect C++ parameter mapping header
    $cppFile = Join-Path $srcDir "EffectParameterMapping.h"
    if (Test-Path $cppFile) {
        $filesToDelete += [PSCustomObject]@{ Type = "C++ Header"; Name = "EffectParameterMapping.h"; Path = $cppFile }
    }
    
    # 3. Collect old .part.xml files (legacy)
    $partFiles = Get-ChildItem -Path $srcDir -Filter "NeoPixel.Effects.*.part.xml" -ErrorAction SilentlyContinue
    foreach ($file in $partFiles) {
        $filesToDelete += [PSCustomObject]@{ Type = "Legacy XML"; Name = $file.Name; Path = $file.FullName }
    }
    
    # 4. Collect auto-generated help files using SAME logic as Generate-HelpFiles
    $helpDir = Join-Path $srcDir "Baggages/Help_de"
    $effectsDir = Join-Path $rootDir "lib/OFM-NeoPixel/src/effects"
    
    Write-Host "  Scanning effects for help files..." -ForegroundColor DarkGray
    Write-Host ""
    
    if ((Test-Path $helpDir) -and (Test-Path $effectsDir)) {
        # Parse effects using same logic as main script
        # Get all *Effect.h files but exclude the base Effect.h class
        $allHeaders = Get-ChildItem -Path $effectsDir -Filter "*.h"
        $effectHeaders = $allHeaders | Where-Object { $_.Name -like "*Effect.h" -and $_.Name -ne "Effect.h" }
        
        # DEBUG: Uncomment to see detailed scanning process
        # Write-Host "  DEBUG: Found $($effectHeaders.Count) effect headers in $effectsDir" -ForegroundColor Yellow
        # Write-Host "  DEBUG: All headers: $($allHeaders.Count)" -ForegroundColor Yellow
        # Write-Host ""
        
        foreach ($header in $effectHeaders) {
            $headerContent = Get-Content $header.FullName -Raw
            
            # Extract effect display name
            $effectDisplayName = ""
            if ($headerContent -match 'getName\s*\(\s*\)\s*(?:const\s+)?(?:override\s+)?{[^}]*return\s+"([^"]+)"') {
                $effectDisplayName = $matches[1]
            } elseif ($headerContent -match 'class\s+(\w+)Effect') {
                $effectDisplayName = $matches[1] -replace 'Effect$', ''
            }
            
            # Get parameter count
            $paramCount = 0
            if ($headerContent -match 'getParameterCount\s*\(\s*\)\s*(?:const\s+)?(?:override\s+)?{[^}]*return\s+(\d+)') {
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
                $effectNameClean = $effectDisplayName -replace '[-\s]', ''
                
                for ($i = 0; $i -lt $paramCount; $i++) {
                    # Extract parameter name using dotall regex
                    if ($headerContent -match "(?s)getParameterName.*?case\s+$i\s*:\s*return\s+`"([^`"]+)`"") {
                        $paramName = $matches[1]
                        $paramNameClean = $paramName -replace '[-\s]', ''
                        
                        # SAME logic as Generate-HelpFiles: NEO-{ParamName}-{EffectName}.md
                        $helpFileName = "NEO-$paramNameClean-$effectNameClean.md"
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
    
    if ($EmptyMarkers) {
        Write-Host "  " -NoNewline
        Write-Host "Template Changes" -ForegroundColor Cyan
        Write-Host "    • Empty Effect Parameters Union block" -ForegroundColor DarkGray
        Write-Host "    • Empty Effect ParameterRefs block" -ForegroundColor DarkGray
        Write-Host "    • Empty Effect Dynamic UI block" -ForegroundColor DarkGray
    }
    
    Write-Host "  " -NoNewline
    Write-Host ("─" * 116) -ForegroundColor DarkGray
    Write-Host "  Total: " -NoNewline -ForegroundColor White
    Write-Host "$($filesToDelete.Count) file(s)" -ForegroundColor Green
    if ($EmptyMarkers) {
        Write-Host " + template markers" -ForegroundColor Green
    }
    Write-Host ""
    
    # Ask for confirmation
    Write-Host "  Continue with cleanup? (y/n): " -NoNewline -ForegroundColor Yellow
    $confirmation = Read-Host
    
    if ($confirmation -ne 'y' -and $confirmation -ne 'Y') {
        Write-Host ""
        Write-Host "  Cleanup cancelled." -ForegroundColor Yellow
        Write-Host ""
        exit 0
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
    if ($EmptyMarkers) {
        Write-Host ""
        Write-Host "  • Emptying marker blocks in templates..." -ForegroundColor Cyan
        
        # 1. Empty NeoPixel.Segment.templ.xml markers
        $templateFile = Join-Path $srcDir "NeoPixel.Segment.templ.xml"
        if (Test-Path $templateFile) {
            $content = Get-Content $templateFile -Raw
            
            # Empty Union Parameters block - keep important comments
            $pattern1 = '(?s)(<!-- BEGIN AUTO-GENERATED: Effect Parameters Union -->\r?\n)(.*?)(<!-- END AUTO-GENERATED: Effect Parameters Union -->)'
            $replacement1 = '$1                <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->' + [Environment]::NewLine + 
                           '                <!-- Start ID: 73, Start Offset: 30 (FIXED - do not change!) -->' + [Environment]::NewLine + 
                           '        $3'
            $content = $content -replace $pattern1, $replacement1
            
            # Empty ParameterRefs block - keep important comments
            $pattern2 = '(?s)(<!-- BEGIN AUTO-GENERATED: Effect ParameterRefs -->\r?\n)(.*?)(<!-- END AUTO-GENERATED: Effect ParameterRefs -->)'
            $replacement2 = '$1                <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->' + [Environment]::NewLine + 
                           '              $3'
            $content = $content -replace $pattern2, $replacement2
            
            # Empty Dynamic UI block - keep important comments
            $pattern3 = '(?s)(<!-- BEGIN AUTO-GENERATED: Effect Dynamic UI -->\r?\n)(.*?)(<!-- END AUTO-GENERATED: Effect Dynamic UI -->)'
            $replacement3 = '$1                      <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->' + [Environment]::NewLine + 
                           '                      <!-- Dynamic choose/when blocks for effect-specific parameters -->' + [Environment]::NewLine + 
                           '              $3'
            $content = $content -replace $pattern3, $replacement3
            
            Set-Content -Path $templateFile -Value $content -NoNewline
            Write-Host "    ✓ NeoPixel.Segment.templ.xml - marker blocks emptied" -ForegroundColor Green
        }
        
        # 2. Empty NEOEFF Module in NeoPixel.xml
        $neoPixelXml = Join-Path $srcDir "NeoPixel.xml"
        if (Test-Path $neoPixelXml) {
            $content = Get-Content $neoPixelXml -Raw
            
            # Empty NEOEFF Module block - keep important comment
            $pattern4 = '(?s)(<!-- BEGIN AUTO-GENERATED: NEOEFF Module -->\r?\n)(.*?)(<!-- END AUTO-GENERATED: NEOEFF Module -->)'
            $replacement4 = '$1  <!-- DO NOT REMOVE THIS MARKER - Used by Build-EffectParameters.ps1 -->' + [Environment]::NewLine + 
                           '  $3'
            $content = $content -replace $pattern4, $replacement4
            
            Set-Content -Path $neoPixelXml -Value $content -NoNewline
            Write-Host "    ✓ NeoPixel.xml - NEOEFF Module removed" -ForegroundColor Green
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
    Write-Host "$(if ($EmptyMarkers) { 'emptied' } else { 'preserved' })" -ForegroundColor $(if ($EmptyMarkers) { 'Green' } else { 'White' })
    Write-Host ""
    
    if (-not $EmptyMarkers) {
        Write-Host "  💡 Use -EmptyMarkers to also empty template blocks for complete clean test" -ForegroundColor Cyan
    } else {
        Write-Host "  💡 Run without -Clean to regenerate everything" -ForegroundColor Cyan
    }
    
    Write-Host ""
    Show-OpenKNXLogo -SubTitle "EffectParameters Build Script by Erkan Çolak" -Version "0.1"
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
    
    # Find PARAM_DESC_DE_EN("de text", "en text") for case $CaseIndex
    $pattern = "get$FunctionName\s*\([^)]*\)[^{]*\{.*?case\s+$CaseIndex\s*:\s*return\s+PARAM_DESC_DE_EN\s*\(\s*[`"']([^`"']+)[`"']\s*,\s*[`"']([^`"']+)[`"']\s*\)"
    
    if ($Content -match $pattern) {
        $deText = $Matches[1]
        $enText = $Matches[2]
        return if ($Lang -eq "en") { $enText } else { $deText }
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

function Get-EffectId {
    param([string]$EffectName)
    
    # Map Effect names to ETS Enum Values (from NeoPixel.share.xml)
    $effectIds = @{
        "EffectSolid" = 0
        "EffectWipe" = 1
        "RainbowEffect" = 2
        "PrideEffect" = 3
        "ConfettiEffect" = 4
        "JuggleEffect" = 5
        "BPMEffect" = 6
        "CylonEffect" = 7
        "RGBWTestEffect" = 8
        "GarageDoorEffect" = 9
        "FireEffect" = 10
        "TheaterChaseEffect" = 11
        "TheaterChaseRainbowEffect" = 12
        "SinelonEffect" = 13
        "TwinkleEffect" = 14
        "SparkleEffect" = 15
        "BreathingEffect" = 16
        "StrobeEffect" = 17
        "PulseEffect" = 18
        "CometEffect" = 19
        "MeteorEffect" = 20
        "RainbowCycleEffect" = 21
    }
    
    if ($effectIds.ContainsKey($EffectName)) {
        return $effectIds[$EffectName]
    }
    
    Write-Warning "Unknown effect: $EffectName (add to mapping!)"
    return -1
}

function Parse-EffectHeader {
    param(
        [string]$HeaderPath,
        [string]$Lang
    )
    
    $fileName = Split-Path -Leaf $HeaderPath
    Write-Host "  " -NoNewline
    Write-Host "Parsing $fileName".PadRight(50) -NoNewline -ForegroundColor Gray
    
    $content = Get-Content -Path $HeaderPath -Raw
    
    # Extract effect class name
    if ($content -match 'class\s+(\w+Effect)\s*:') {
        $effectName = $Matches[1]
    } else {
        Write-Warning "Could not extract class name from $HeaderPath"
        return $null
    }
    
    # Extract effect display name (from getName() return)
    if ($content -match 'getName\(\)[^{]*\{[^}]*return\s+"([^"]+)"') {
        $displayName = $Matches[1]
    } else {
        $displayName = $effectName -replace 'Effect$', ''
    }
    
    # Extract parameter count
    if ($content -match 'getParameterCount\s*\(\)[^{]*\{[^}]*return\s+(\d+)') {
        $paramCount = [int]$Matches[1]
    } else {
        $paramCount = 0
    }
    
    Write-Host "OK " -NoNewline -ForegroundColor Green
    Write-Host "Effect: " -NoNewline -ForegroundColor DarkGray
    Write-Host "$displayName".PadRight(20) -NoNewline -ForegroundColor White
    Write-Host "ID: " -NoNewline -ForegroundColor DarkGray
    Write-Host "$(Get-EffectId $effectName)".PadRight(4) -NoNewline -ForegroundColor Cyan
    Write-Host "Params: " -NoNewline -ForegroundColor DarkGray
    Write-Host "$paramCount" -ForegroundColor Green
    
    # Parse parameters
    $parameters = @()
    for ($i = 0; $i -lt $paramCount; $i++) {
        $paramName = Extract-SwitchCase -Content $content -FunctionName "ParameterName" -CaseIndex $i
        $paramDesc = Extract-SwitchCaseMultiLang -Content $content -FunctionName "ParameterDescription" -CaseIndex $i -Lang $Lang
        $paramType = Extract-ParameterType -Content $content -CaseIndex $i
        $paramDefault = Extract-IntValue -Content $content -FunctionName "ParameterDefault" -CaseIndex $i
        $paramMin = Extract-IntValue -Content $content -FunctionName "ParameterMin" -CaseIndex $i
        $paramMax = Extract-IntValue -Content $content -FunctionName "ParameterMax" -CaseIndex $i
        
        # Set sensible defaults for Min/Max if not defined (both are 0)
        # For UINT8/HUE types, use full range 0-255 unless explicitly limited
        if ($paramMin -eq 0 -and $paramMax -eq 0) {
            if ($paramType -match 'PARAM_UINT8|PARAM_HUE') {
                $paramMax = 255
            } elseif ($paramType -eq 'PARAM_PERCENT') {
                $paramMax = 100
            }
        }
        
        if ($paramName) {
            Write-Host "       [$i] $paramName ($paramType) = $paramDefault [$paramMin..$paramMax]" -ForegroundColor DarkGray
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
    $cleanName = $effectName -replace 'Effect$', ''
    $cleanName = $cleanName -replace '[^a-zA-Z0-9]', ''
    
    return @{
        Name = $cleanName
        ClassName = $effectName
        DisplayName = $displayName
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
    $xml += '        <ApplicationProgram Id="%AID%" ApplicationNumber="40" ApplicationVersion="1" ProgramType="ApplicationProgram" MaskVersion="MV-07B0" DynamicTableManagement="false">'
    $xml += '          <Static>'
    $xml += '            <ParameterTypes>'
    
    foreach ($effect in $Effects) {
        if ($effect.Parameters.Count -eq 0) { continue }
        
        $xml += ''
        $xml += "              <!-- $($effect.DisplayName) Effect Parameters -->"
        
        foreach ($param in $effect.Parameters) {
            # ParameterType ID format: %AID%_PT-{EffectName}{ParamName}
            # Replace hyphens/spaces with nothing for clean names
            $effectNameClean = $effect.DisplayName -replace '[-\s]', ''
            $paramNameClean = $param.Name -replace '[-\s]', ''
            $ptId = "%AID%_PT-$($effectNameClean)$($paramNameClean)"
            $paramNameWithEffect = "$($param.Name) ($($effect.DisplayName))"
            
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
  <op:define prefix="NEOEFF" ModuleType="40"
    share="NeoPixel.Effects.ParameterTypes.generated.xml">
  </op:define>
"@
        
        # Insert NEOEFF module between markers
        $content = $content -replace `
            '(?s)(<!-- BEGIN AUTO-GENERATED: NEOEFF Module -->.*?<!-- DO NOT REMOVE THIS MARKER[^>]*-->).*?(<!-- END AUTO-GENERATED: NEOEFF Module -->)',`
            "`$1$neoeffModule`n  `$2"
    } else {
        Write-Host "  • Removing NEOEFF module from NeoPixel.xml..." -ForegroundColor Yellow
        
        # Clear content between markers
        $content = $content -replace `
            '(?s)(<!-- BEGIN AUTO-GENERATED: NEOEFF Module -->.*?<!-- DO NOT REMOVE THIS MARKER[^>]*-->).*?(<!-- END AUTO-GENERATED: NEOEFF Module -->)',`
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
    
    # Detect OS and OpenKNXproducer path
    $openKnxExe = if ($IsWindows) {
        "OpenKNXproducer.exe"
    } else {
        # macOS/Linux: Check ~/bin/OpenKNXproducer.exe first (mono wrapper)
        if (Test-Path "$HOME/bin/OpenKNXproducer.exe") {
            "$HOME/bin/OpenKNXproducer.exe"
        } else {
            "OpenKNXproducer"
        }
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
        '(?s)(<!-- BEGIN AUTO-GENERATED: Effect Parameters Union -->.*?<!-- Start ID: \d+, Start Offset: \d+ \(FIXED[^)]*\) -->.*?<!-- Generated:[^>]*-->).*?(<!-- END AUTO-GENERATED: Effect Parameters Union -->)',`
        "`$1`n`$2"
    
    # Clear ParameterRefs region
    $content = $content -replace `
        '(?s)(<!-- BEGIN AUTO-GENERATED: Effect ParameterRefs -->.*?<!-- Generated:[^>]*-->).*?(<!-- END AUTO-GENERATED: Effect ParameterRefs -->)',`
        "`$1`n`$2"
    
    # Clear Dynamic UI region
    $content = $content -replace `
        '(?s)(<!-- BEGIN AUTO-GENERATED: Effect Dynamic UI -->.*?<!-- Effect-specific[^>]*-->).*?(<!-- END AUTO-GENERATED: Effect Dynamic UI -->)',`
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
        [int]$StartId,
        [int]$StartOffset
    )
    
    Write-Host "    ▸ Generating Union Parameters (Start ID=$StartId, Offset=$StartOffset)..." -ForegroundColor DarkGray
    
    # Generate Parameter definitions for Union block (Static section)
    # ALL parameters must be defined statically - UI visibility controlled in Dynamic section
    
    $xml = @()
    $xml += ''
    
    if ($Effects.Count -eq 0 -or ($Effects | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum -eq 0) {
        $xml += '                <!-- No effect-specific parameters found -->'
        return @{
            Xml = ($xml -join "`n")
            NextId = $StartId
            NextOffset = $StartOffset
        }
    }
    
    $currentId = $StartId
    $currentOffset = $StartOffset
    
    foreach ($effect in $Effects | Where-Object { $_.Parameters.Count -gt 0 } | Sort-Object EffectID) {
        $xml += "                <!-- $($effect.DisplayName) Effect (ID $($effect.EffectID)) Parameters -->"
        
        foreach ($param in $effect.Parameters) {
            # Parameter Type ID format: %AID%_PT-{EffectName}{ParamName}
            $effectNameClean = $effect.Name -replace '[^a-zA-Z0-9]', ''
            $paramNameClean = $param.Name -replace '[^a-zA-Z0-9]', ''
            $ptId = "%AID%_PT-$($effectNameClean)$($paramNameClean)"
            
            # Format ID with leading zeros (073, 074, etc.)
            $paramIdNum = "{0:D3}" -f $currentId
            $paramId = "%AID%_UP-%TT%%CC%$paramIdNum"
            
            # Name attribute: Unique per channel and effect
            $paramName = "NEO%C%$($effectNameClean)$($paramNameClean)"
            
            # Determine BitOffset based on parameter type
            $bitOffset = if ($param.Type -eq "PARAM_BOOL") { 7 } else { 0 }
            
            # Generate Parameter element
            $xml += "                <Parameter Id=`"$paramId`" Offset=`"$currentOffset`" BitOffset=`"$bitOffset`" Name=`"$paramName`" ParameterType=`"$ptId`" Text=`"$($param.Name) ($($effect.DisplayName))`" Value=`"$($param.Default)`"/>"
            
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
        $xml += "              <!-- $($effect.DisplayName) Effect ParameterRefs -->"
        
        foreach ($param in $effect.Parameters) {
            $paramIdNum = "{0:D3}" -f $currentId
            $refNum = "{0:D5}" -f ([int]$paramIdNum + 0)  # RefId suffix, e.g., 07301
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
        $xml += "                        <!-- $($effect.DisplayName) Effect Parameters -->"
        
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
    
    $helpDir = "src/Baggages/Help_de"
    
    # Create help directory if it doesn't exist
    if (-not (Test-Path $helpDir)) {
        New-Item -ItemType Directory -Path $helpDir -Force | Out-Null
    }
    
    foreach ($effect in $Effects) {
        if ($effect.Parameters.Count -eq 0) { continue }
        
        foreach ($param in $effect.Parameters) {
            # HelpContext format: NEO-{ParamName}-{EffectName}.md
            # Must match Text attribute: "ParamName (EffectName)"
            # OpenKNXproducer derives: NEO-ParamName-EffectName
            $effectNameClean = $effect.DisplayName -replace '[-\s]', ''
            $paramNameClean = $param.Name -replace '[-\s]', ''
            $helpFileName = "NEO-$paramNameClean-$effectNameClean.md"
            $helpFilePath = Join-Path $helpDir $helpFileName
            
            # Skip if help file already exists
            if (Test-Path $helpFilePath) {
                continue
            }
            
            # Generate help content
            $helpContent = @()
            $helpContent += "# $($param.Name)"
            $helpContent += ""
            $helpContent += "**Effekt:** $($effect.DisplayName)"
            $helpContent += ""
            if ($param.Description) {
                $helpContent += $param.Description
            } else {
                $helpContent += "Parameter zur Steuerung von '$($param.Name)' für den $($effect.DisplayName)-Effekt."
            }
            $helpContent += ""
            $helpContent += "**Wertebereich:** $($param.Min) - $($param.Max)"
            $helpContent += "**Standardwert:** $($param.Default)"
            
            Set-Content -Path $helpFilePath -Value ($helpContent -join "`n") -Encoding UTF8
            Write-Host "  OK Created help file: $helpFileName" -ForegroundColor DarkGray
        }
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
    $unionMarker = "<!-- BEGIN AUTO-GENERATED: Effect Parameters Union -->"
    
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
        "<!-- BEGIN AUTO-GENERATED: Effect Parameters Union -->",
        "<!-- END AUTO-GENERATED: Effect Parameters Union -->",
        "<!-- BEGIN AUTO-GENERATED: Effect ParameterRefs -->",
        "<!-- END AUTO-GENERATED: Effect ParameterRefs -->",
        "<!-- BEGIN AUTO-GENERATED: Effect Dynamic UI -->",
        "<!-- END AUTO-GENERATED: Effect Dynamic UI -->"
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
        foreach ($error in $errors) {
            Write-Host "       - $error" -ForegroundColor Red
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
        $beginMarker = "<!-- BEGIN AUTO-GENERATED: Effect Parameters Union -->"
        $endMarker = "<!-- END AUTO-GENERATED: Effect Parameters Union -->"
        
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
        $beginMarker = "<!-- BEGIN AUTO-GENERATED: Effect ParameterRefs -->"
        $endMarker = "<!-- END AUTO-GENERATED: Effect ParameterRefs -->"
        
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
        $beginMarker = "<!-- BEGIN AUTO-GENERATED: Effect Dynamic UI -->"
        $endMarker = "<!-- END AUTO-GENERATED: Effect Dynamic UI -->"
        
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
    
    $cpp = @()
    $cpp += '// AUTO-GENERATED by scripts/Build-EffectParameters.ps1'
    $cpp += '// DO NOT EDIT MANUALLY - Changes will be overwritten!'
    $cpp += ''
    $cpp += '#ifndef EFFECT_PARAMETER_MAPPING_H'
    $cpp += '#define EFFECT_PARAMETER_MAPPING_H'
    $cpp += ''
    $cpp += '#pragma once'
    $cpp += ''
    $cpp += '#include "knxprod.h"'
    $cpp += '#include "../lib/OFM-NeoPixel/src/effects/Effect.h"'
    $cpp += '#include "../lib/OFM-NeoPixel/src/Segment.h"'
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
        $cpp += "        case $($effect.EffectID):  // $($effect.DisplayName) Effect"
        
        # Effect name for macro (remove spaces/hyphens)
        $effectNameClean = $effect.DisplayName -replace '[-\s]', ''
        
        for ($i = 0; $i -lt $effect.Parameters.Count; $i++) {
            $param = $effect.Parameters[$i]
            $paramNameClean = $param.Name -replace '[-\s]', ''
            
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

Clear-Host

Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  Parsing Effect Headers (Language: $Language)" -ForegroundColor Cyan
Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan

$effectsDir = "lib/OFM-NeoPixel/src/effects"
$effects = @()

if (-not (Test-Path $effectsDir)) {
    Write-Host "ERROR: Effects directory not found: $effectsDir" -ForegroundColor Red
    exit 1
}

# Find all *Effect.h files (except base Effect.h and EffectSolid.h/EffectWipe.h which are special)
$effectFiles = Get-ChildItem -Path $effectsDir -Filter "*Effect.h" | Where-Object { 
    $_.Name -ne "Effect.h" -and 
    $_.Name -ne "EffectSolid.h" -and 
    $_.Name -ne "EffectWipe.h"
}

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

# Read starting IDs from template markers
Write-Host ""
Write-Host "  Reading template configuration..." -ForegroundColor Cyan
$templatePath = "src/NeoPixel.Segment.templ.xml"

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
$neoPixelXmlPath = "src/NeoPixel.xml"
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
Generate-HelpFiles -Effects $effects
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
        # FINAL VALIDATION: Test with OpenKNXproducer
        # ============================================================
        $testResult = Test-OpenKNXproducer -WorkingDir "." `
                                            -HeaderFile "include/knxprod.h" `
                                            -SourceDir "src/NeoPixel"
        
        if ($testResult.Success) {
            Write-Host ""
            Write-Host ""
            Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
            Write-Host "  SUCCESS - all validations passed through the OpenKNXproducer build" -ForegroundColor Cyan
            Write-Host "════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════" -ForegroundColor Cyan
            Write-Host ""
            Write-Host "  [OK] Internal and OpenKNXproducer validations passed!" -ForegroundColor Green
            Write-Host "  [OK] knxprod.h and NeoPixel.Segment.xml are valid!" -ForegroundColor Green
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
            Write-Host "  ┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐" -ForegroundColor DarkGray
            
            # Calculate padding for file paths (Box is 114 chars wide, "  │ " = 4, " │" = 2, so content = 108)
            $boxContentWidth = 106
            $label1 = "ParameterTypes:   "
            $label2 = "Union Parameters: "
            $label3 = "ParameterRefs:    "
            $label4 = "Dynamic Choose:   "
            $label5 = "C++ Mapping:      "
            
            Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
            Write-Host ($label1 + $paramTypesPath).PadRight($boxContentWidth) -NoNewline -ForegroundColor White
            Write-Host " │" -ForegroundColor DarkGray
            
            Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
            Write-Host ($label2 + $unionPath).PadRight($boxContentWidth) -NoNewline -ForegroundColor White
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
            
            Write-Host "  └──────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘" -ForegroundColor DarkGray
            Write-Host ""
            Write-Host "  Ready for ETS Import:" -ForegroundColor Cyan
            Write-Host "  ┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐" -ForegroundColor DarkGray
            
            $totalHelpFiles = ($effects | ForEach-Object { $_.Parameters.Count } | Measure-Object -Sum).Sum
            
            Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
            Write-Host ("Effects with parameters: " + $effects.Count).PadRight($boxContentWidth) -NoNewline -ForegroundColor White
            Write-Host " │" -ForegroundColor DarkGray
            
            Write-Host "  │ " -NoNewline -ForegroundColor DarkGray
            Write-Host ("Help files generated:    " + $totalHelpFiles).PadRight($boxContentWidth) -NoNewline -ForegroundColor White
            Write-Host " │" -ForegroundColor DarkGray
            
            Write-Host "  └──────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘" -ForegroundColor DarkGray
            Write-Host ""
            Show-OpenKNXLogo -SubTitle "EffectParameters Build Script by Erkan Çolak" -Version "0.1"
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
