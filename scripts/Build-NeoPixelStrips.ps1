#!/usr/bin/env pwsh
<#
Open ■
┬────┴  Build-NeoPixelStrips
■ KNX   2024 OpenKNX - Erkan Çolak

FILEPATH: scripts/Build-NeoPixelStrips.ps1

.SYNOPSIS
    NeoPixel LED Strip template generator - wrapper for Build-DynamicGPIOTemplate.ps1

.DESCRIPTION
    Generates ETS XML template for NeoPixel LED strips with dynamic GPIO selection
    based on hardware configuration. Auto-detects hardware from platformio.hardware.ini
    or allows manual override.

.PARAMETER TestMode
    Use hardcoded test data instead of real hardware configuration

.PARAMETER HardwareConfigSection
    Manually specify hardware section from platformio.hardware.ini
    (e.g., "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1")

.NOTES
    This script delegates to Build-DynamicGPIOTemplate.ps1 with NeoPixel-specific parameters.
    Part of the OpenKNX project automation framework.
#>

param(
    [Parameter(Mandatory=$false)]
    [switch]$TestMode,
    
    [Parameter(Mandatory=$false)]
    [string]$HardwareConfigSection = "",
    
    [Parameter(Mandatory=$false)]
    [switch]$UniversalBuild
)

$ErrorActionPreference = "Stop"

# Get script directory and repo root (platform-independent)
$scriptDir = if ($PSScriptRoot) { 
    $PSScriptRoot 
} elseif ($PSCommandPath) { 
    Split-Path -Parent $PSCommandPath 
} else { 
    $PWD.Path 
}

$repoRoot = Split-Path -Parent $scriptDir

# Auto-detect hardware configuration if not specified
if (-not $HardwareConfigSection -and -not $TestMode -and -not $UniversalBuild) {
    $hardwareIniPath = Join-Path $repoRoot "platformio.hardware.ini"
    
    if (Test-Path $hardwareIniPath) {
        Write-Host "  [NeoPixel] Auto-detecting hardware configuration..." -ForegroundColor Cyan
        
        # Read platformio.hardware.ini and find first neopixel_oknxhw_* section
        $iniContent = Get-Content $hardwareIniPath -Raw
        
        # Find all neopixel hardware sections
        $sectionMatches = [regex]::Matches($iniContent, '\[(neopixel_oknxhw_[^\]]+)\]')
        
        if ($sectionMatches.Count -gt 0) {
            $HardwareConfigSection = $sectionMatches[0].Groups[1].Value
            Write-Host "  [NeoPixel] ✓ Using hardware config: $HardwareConfigSection" -ForegroundColor Green
        } else {
            Write-Host "  [NeoPixel] ⚠ No neopixel_oknxhw_* sections found - using manual mode" -ForegroundColor Yellow
        }
    }
}

# Universal Build: Use the "richest" hardware configuration (most GPIO ports)
if ($UniversalBuild -and -not $HardwareConfigSection) {
    Write-Host "  [NeoPixel] UNIVERSAL BUILD MODE: Using richest hardware config" -ForegroundColor Yellow
    # Default to KNeoPix RP2040 V1 which has 7 GPIO ports (most comprehensive)
    $HardwareConfigSection = "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1"
    Write-Host "  [NeoPixel] ✓ Using: $HardwareConfigSection (7 GPIO ports)" -ForegroundColor Green
}

# Build parameter hashtable
$params = @{
    FeatureName = "NeoPixel"
    DefinesPrefix = "NEOPIXEL_HW"
    TemplateFile = "src/NeoPixel.Strip.templ.xml"
    MaxPorts = 7
}

if ($UniversalBuild) {
    Write-Host "  [NeoPixel] UNIVERSAL BUILD: KNXprod supports all hardware variants" -ForegroundColor Cyan
}

if ($TestMode) {
    $params.TestMode = $true
    Write-Host "  [NeoPixel] TEST MODE activated" -ForegroundColor Yellow
}

if ($HardwareConfigSection) {
    $params.HardwareConfigSection = $HardwareConfigSection
}

# Call generic GPIO template builder
Write-Host "  [NeoPixel] Calling Build-DynamicGPIOTemplate.ps1..." -ForegroundColor Cyan
Write-Host "  [NeoPixel] Parameters: $($params | Out-String)" -ForegroundColor DarkGray

& "$scriptDir/Build-DynamicGPIOTemplate.ps1" @params

exit $LASTEXITCODE
