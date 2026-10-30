<#
.SYNOPSIS
    Normalize generated files to LF line endings without a UTF-8 BOM.

.DESCRIPTION
    The generator scripts splice blocks into existing files with PowerShell string
    operations. Get-Content/Set-Content and the `-join "`n"` inserts do not agree on a
    newline, so the result is a file with mixed CRLF/LF endings. Git cannot round-trip
    such a file through its eol filter, so it reports the file as modified on every
    status even when the content is byte-identical after normalization - which made
    Build-Release.ps1 appear to dirty the same handful of files on every run.

    The repository pins `eol=lf` in .gitattributes, so this normalizes the generated
    files to LF after the generators have written them. Windows PowerShell 5.1 writes
    a UTF-8 BOM with Set-Content -Encoding UTF8 (pwsh 7 does not); it is removed too.
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory = $false)]
    [switch]$Quiet
)

$repoRoot = Split-Path -Parent $PSScriptRoot

# Files and globs written or spliced by Build-EffectParameters.ps1 / Build-HardwareConfig.ps1
# (Help_de/*.md also covers the hand-written help files in that folder)
$patterns = @(
    "src/NeoPixel.share.xml"
    "src/NeoPixel.EM.share.xml"
    "src/NeoPixel.Strip.share.xml"
    "src/NeoPixel.conf.xml"
    "src/NeoPixel.Scene.part.xml"
    "src/NeoPixel.Cue.part.xml"
    "src/NeoPixel.Segment.templ.xml"
    "src/NeoPixel.Strip.templ.xml"
    "src/NeoPixel.Hardware.templ.xml"
    "src/NeoPixel.EM.templ.xml"
    "src/NeoPixel-Dev.xml"
    "src/NeoPixel-Release.xml"
    "src/NeoPixel.script.js"
    "src/NeoPixel.Effects.*.generated.xml"
    "src/EffectTypeMapping.h"
    "src/EffectParameterMapping.h"
    "src/HardwareMappingData.h"
    "src/HardwareMappingLogic.h"
    "src/EffectIds.lock"
    "src/Baggages/Help_de/*.md"
    "doc/Effect-Flags.md"
    "include/knxprod.h"
    "include/hardware.h"
)

$changed = 0
$scanned = 0

foreach ($pattern in $patterns) {
    $full = Join-Path $repoRoot $pattern
    foreach ($file in (Get-ChildItem -Path $full -File -ErrorAction SilentlyContinue)) {
        $scanned++
        $bytes = [System.IO.File]::ReadAllBytes($file.FullName)
        if ($bytes.Length -eq 0) { continue }

        # Strip CR only where it precedes LF; a lone CR is not a line ending here.
        $out = [System.Collections.Generic.List[byte]]::new($bytes.Length)
        $hadCrLf = $false
        # Leading UTF-8 BOM, written by Windows PowerShell 5.1
        $hadBom = $bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF
        $start = if ($hadBom) { 3 } else { 0 }
        for ($i = $start; $i -lt $bytes.Length; $i++) {
            if ($bytes[$i] -eq 13 -and ($i + 1) -lt $bytes.Length -and $bytes[$i + 1] -eq 10) {
                $hadCrLf = $true
                continue
            }
            $out.Add($bytes[$i])
        }

        if ($hadCrLf -or $hadBom) {
            [System.IO.File]::WriteAllBytes($file.FullName, $out.ToArray())
            $changed++
            if (-not $Quiet) {
                $rel = $file.FullName.Substring($repoRoot.Length + 1) -replace '\\', '/'
                Write-Host "  - Normalized (LF, no BOM): $rel" -ForegroundColor DarkGray
            }
        }
    }
}

if (-not $Quiet) {
    if ($changed -gt 0) {
        Write-Host "  [OK] Normalized to LF without BOM ($changed of $scanned file(s))" -ForegroundColor Green
    }
    else {
        Write-Host "  [OK] Already LF without BOM ($scanned file(s) checked)" -ForegroundColor Green
    }
}

exit 0
