#!/usr/bin/env pwsh
# Open #
# +----+  Test-ParameterLayout
# # KNX   2026 OpenKNX - Erkan Colak
#
# FILEPATH: test/Test-ParameterLayout.ps1

<#
.SYNOPSIS
    OpenKNX NeoPixel - checks the strip and relay parameter memory layout of the generated product.

.DESCRIPTION
    Two ETS parameters that share bits overwrite each other when both are visible; the firmware
    then reads a value nobody set. This test reads the header the OpenKNXproducer generated
    (include/knxprod.h) and the generated product XML and fails when:

      1. two parameters of the strip block (NEOSTRIP_*) share a bit. The hardware variants of one
         parameter (...PortHW0, ...PortHW1, ...) are alternatives and may share.
      2. the hardware variants of one family (strip data port, strip clock port, relay 1-4 port)
         do not all sit on the same offset and bits. The firmware reads them through one index.
      3. a strip port selection type lacks "Manuell" (10) or "please select" (15), the values the
         firmware decodes (classifyPortSelection in src/HardwareMappingLogic.h).

    Run it after scripts/Build-Release.ps1 has generated include/knxprod.h and the product XML.

.PARAMETER KnxprodHeader
    Generated header with the parameter defines (default: include/knxprod.h of this repository).

.PARAMETER ProductXml
    Generated product XML with the parameter types (default: src/NeoPixel-Dev.debug.xml).

.EXAMPLE
    pwsh test/Test-ParameterLayout.ps1

.NOTES
    Author: Erkan Colak
    Exit code: 0 = layout clean, 1 = finding(s), 2 = input missing.
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory = $false)]
    [string]$KnxprodHeader = "",

    [Parameter(Mandatory = $false)]
    [string]$ProductXml = ""
)

function OpenKNX_ShowLogo($AddCustomText = $null) {
    Write-Host ""
    Write-Host "Open " -NoNewline
    Write-Host "$( [char]::ConvertFromUtf32(0x25A0) )" -ForegroundColor Green
    $unicodeString = "$( [char]::ConvertFromUtf32(0x252C) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2500) )$( [char]::ConvertFromUtf32(0x2534) ) "
    if ($AddCustomText) {
        Write-Host "$($unicodeString) $($AddCustomText)" -ForegroundColor Green
    }
    else {
        Write-Host "$($unicodeString)" -ForegroundColor Green
    }
    Write-Host "$( [char]::ConvertFromUtf32(0x25A0) )" -NoNewline -ForegroundColor Green
    Write-Host " KNX"
    Write-Host ""
}

OpenKNX_ShowLogo "Parameter Layout Test"

$repoRoot = Split-Path -Parent $PSScriptRoot
if (-not $KnxprodHeader) { $KnxprodHeader = Join-Path $repoRoot "include/knxprod.h" }
if (-not $ProductXml) { $ProductXml = Join-Path $repoRoot "src/NeoPixel-Dev.debug.xml" }
foreach ($path in @($KnxprodHeader, $ProductXml)) {
    if (-not (Test-Path -Path $path -PathType Leaf)) {
        Write-Host "MISSING: $path - run scripts/Build-Release.ps1 first" -ForegroundColor Red
        exit 2
    }
}

# --- Parse the defines: "#define NAME  <offset>  // <n> Bits, Bit <hi>-<lo> | uint8_t | uint16_t ..."
$parameters = @()
foreach ($line in Get-Content -Path $KnxprodHeader) {
    $m = [regex]::Match($line, '^#define\s+(\w+)\s+(\d+)\s+//\s*(.*)$')
    if (-not $m.Success) { continue }
    $name = $m.Groups[1].Value
    $offset = [int]$m.Groups[2].Value
    $desc = $m.Groups[3].Value.Trim()
    $bits = @()
    $b = [regex]::Match($desc, '^(\d+) Bits?, Bit (\d+)')
    if ($b.Success) {
        # Bit numbers count over the whole big-endian field (paramWord/paramInt): bit 15-8 lie in the
        # first byte, bit 7-0 in the next one
        $count = [int]$b.Groups[1].Value
        $high = [int]$b.Groups[2].Value
        $span = [math]::Floor($high / 8) + 1
        for ($i = $high - $count + 1; $i -le $high; $i++) {
            $byte = $offset + ($span - 1 - [math]::Floor($i / 8))
            $bits += "$byte.$($i % 8)"
        }
    }
    elseif ($desc -match '^u?int8_t') {
        for ($i = 0; $i -lt 8; $i++) { $bits += "$offset.$i" }
    }
    elseif ($desc -match '^u?int16_t') {
        foreach ($o in @($offset, ($offset + 1))) { for ($i = 0; $i -lt 8; $i++) { $bits += "$o.$i" } }
    }
    elseif ($desc -match '^(u?int32_t|float)') {
        foreach ($o in @($offset, ($offset + 1), ($offset + 2), ($offset + 3))) { for ($i = 0; $i -lt 8; $i++) { $bits += "$o.$i" } }
    }
    else { continue }
    $family = ""
    $f = [regex]::Match($name, '^(\w+?PortHW)\d+$')
    if ($f.Success) { $family = $f.Groups[1].Value }
    $parameters += [pscustomobject]@{ Name = $name; Offset = $offset; Desc = $desc; Bits = $bits; Family = $family }
}

$findings = 0

# --- 1. No shared bits inside the strip block (hardware variants of one family excepted)
$strip = @($parameters | Where-Object { $_.Name -like 'NEOSTRIP_NEO*' })
if ($strip.Count -eq 0) {
    Write-Host "MISSING: no NEOSTRIP_* defines in $KnxprodHeader" -ForegroundColor Red
    exit 2
}
$owner = @{}
$overlaps = [ordered]@{}
foreach ($p in $strip) {
    foreach ($bit in $p.Bits) {
        if ($owner.ContainsKey($bit)) {
            $other = $owner[$bit]
            if ($p.Family -and ($p.Family -eq $other.Family)) { continue }
            # One line per parameter pair; hardware variants are reported as their family (...PortHW*)
            $a = $other.Name; if ($other.Family) { $a = "$($other.Family)*" }
            $b = $p.Name; if ($p.Family) { $b = "$($p.Family)*" }
            $key = "$a and $b"
            if (-not $overlaps.Contains($key)) { $overlaps[$key] = @() }
            if ($overlaps[$key] -notcontains $bit) { $overlaps[$key] += $bit }
        }
        else {
            $owner[$bit] = $p
        }
    }
}
foreach ($key in $overlaps.Keys) {
    Write-Host "OVERLAP: $key share byte.bit $($overlaps[$key] -join ', ')" -ForegroundColor Red
    $findings++
}
Write-Host "1. Strip block: $($strip.Count) parameter(s) checked for shared bits"

# --- 2. All hardware variants of a family on the same offset and bits
$families = $parameters | Where-Object { $_.Family } | Group-Object -Property Family
foreach ($group in $families) {
    $first = $group.Group[0]
    foreach ($p in $group.Group) {
        if (($p.Offset -ne $first.Offset) -or ($p.Desc -ne $first.Desc)) {
            Write-Host "VARIANT: $($p.Name) at $($p.Offset) ($($p.Desc)) differs from $($first.Name) at $($first.Offset) ($($first.Desc))" -ForegroundColor Red
            $findings++
        }
    }
}
Write-Host "2. Hardware variants: $(@($families).Count) famil(ies) checked for one common place"

# --- 3. Every strip port selection type offers 10 (Manuell) and 15 (please select)
$xml = Get-Content -Path $ProductXml -Raw
$types = [regex]::Matches($xml, '(?s)<ParameterType [^>]*Name="[^"]*GPIO(?:Clock)?PortHW\d+"[^>]*>.*?</ParameterType>')
foreach ($type in $types) {
    $typeName = [regex]::Match($type.Value, 'Name="([^"]*)"').Groups[1].Value
    foreach ($value in @('10', '15')) {
        if ($type.Value -notmatch "<Enumeration [^>]*Value=`"$value`"") {
            Write-Host "VALUE: $typeName lacks the value $value" -ForegroundColor Red
            $findings++
        }
    }
}
if ($types.Count -eq 0) {
    Write-Host "MISSING: no port selection types in $ProductXml" -ForegroundColor Red
    exit 2
}
Write-Host "3. Port selection types: $($types.Count) checked for the values 10 and 15"

Write-Host ""
if ($findings -gt 0) {
    Write-Host "FAILED: $findings finding(s)" -ForegroundColor Red
    exit 1
}
Write-Host "PASSED: parameter layout clean" -ForegroundColor Green
exit 0
