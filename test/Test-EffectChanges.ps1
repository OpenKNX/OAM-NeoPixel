#!/usr/bin/env pwsh
# Open #
# +----+  Test-EffectChanges
# # KNX   2026 OpenKNX - Erkan Colak
#
# FILEPATH: test/Test-EffectChanges.ps1

<#
.SYNOPSIS
    OpenKNX NeoPixel - regression test of scripts/Build-EffectParameters.ps1 against changes of the effects.

.DESCRIPTION
    Copies the repository (without the shared libraries) into a temporary folder, with a private copy of
    lib/OFM-NeoPixel/src/effects, and runs the effect generator once per scenario after editing effect
    headers, EffectPool.cpp/.h, the group file or a generated page. Per scenario it checks:

      allowed change: -Check reports outdated before the run and current after it, a second run writes
                      nothing, exactly the expected files change, and no Parameter or ParameterRef ID of the
                      segment template, the cue part or the scene part changes its meaning (name, type, text,
                      referenced parameter) or disappears unless the scenario removes it;
      forbidden change: the run stops with the expected message, writes no file, and -Check stays outdated.

    Scenarios: parameter description / default / maximum changed, parameter appended, inserted, reordered,
    removed (last / middle), renamed (same type), type changed (kind, enum value reordered / removed / appended, number
    range widened / narrowed), lock files missing, lock files after a stopped run and after -WebCatalogOnly, effect appended (1D and 2D), effect removed (last / middle), effects
    reordered, 2D effect without the NEOPIXEL_DISABLE_2D entry, effect groups (valid, unknown effect,
    duplicate name, Solid), effect number above 99, cue string parameter index above 9, a generated page
    without its block marker.

    The shared repositories are not touched. Run it from any folder.

.PARAMETER Scenario
    Run only these scenarios (names as in the result table), default: all.

.PARAMETER KeepTemp
    Keep the temporary folder (path in the output) for inspection.

.EXAMPLE
    pwsh test/Test-EffectChanges.ps1

.EXAMPLE
    pwsh test/Test-EffectChanges.ps1 -Scenario ParamAppend,ParamInsert -KeepTemp

.NOTES
    Author: Erkan Colak
    Exit code: 0 = every scenario passed, 1 = failure(s).
#>

param(
  [string[]]$Scenario = @(),
  [switch]$KeepTemp
)

$ErrorActionPreference = 'Stop'
# "-Scenario A,B" through pwsh -File arrives as one string
$Scenario = @($Scenario | ForEach-Object { $_ -split ',' } | ForEach-Object { $_.Trim() } | Where-Object { $_ })
$repo = Split-Path -Parent $PSScriptRoot
$psExe = (Get-Process -Id $PID).Path
$tempRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("neopixel-effect-test-" + [Guid]::NewGuid().ToString('N').Substring(0, 8))
$script:Results = New-Object System.Collections.Generic.List[object]
$utf8 = New-Object System.Text.UTF8Encoding($false)

# ------------------------------------------------------------------------------------------------
# Fixture
# ------------------------------------------------------------------------------------------------

# @brief Copies a folder, leaving out generated debug files
function Copy-Folder([string]$From, [string]$To) {
  New-Item -ItemType Directory -Path $To -Force | Out-Null
  foreach ($item in (Get-ChildItem -LiteralPath $From -Force)) {
    if ($item.Name -like '*.debug.xml' -or $item.Name -eq '.DS_Store') { continue }
    $dest = Join-Path $To $item.Name
    if ($item.PSIsContainer) { Copy-Folder $item.FullName $dest }
    else { [System.IO.File]::Copy($item.FullName, $dest, $true) }
  }
}

# @brief Base fixture: scripts, src, web, doc/Effect-Flags.md, top-level files and a copy of the effect sources
function New-BaseFixture([string]$Dir) {
  foreach ($d in @('scripts', 'src', 'web')) { Copy-Folder (Join-Path $repo $d) (Join-Path $Dir $d) }
  New-Item -ItemType Directory -Path (Join-Path $Dir 'doc') -Force | Out-Null
  Copy-Item -LiteralPath (Join-Path $repo 'doc/Effect-Flags.md') -Destination (Join-Path $Dir 'doc/Effect-Flags.md')
  foreach ($f in (Get-ChildItem -LiteralPath $repo -File)) { Copy-Item -LiteralPath $f.FullName -Destination (Join-Path $Dir $f.Name) }
  # a real folder (no symlink): the scenarios edit effect headers and EffectPool
  Copy-Folder (Join-Path $repo 'lib/OFM-NeoPixel/src/effects') (Join-Path $Dir 'lib/OFM-NeoPixel/src/effects')
}

# @brief Runs the generator in a fixture; returns exit code and output
function Invoke-Generator([string]$Dir, [string[]]$GenArgs) {
  Push-Location -LiteralPath $Dir
  try {
    $prev = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    $out = & $psExe -NoProfile -File (Join-Path $Dir 'scripts/Build-EffectParameters.ps1') @GenArgs 2>&1 | Out-String
    $code = $LASTEXITCODE
    $ErrorActionPreference = $prev
  }
  finally { Pop-Location }
  $out = [regex]::Replace($out, "\x1b\[[0-9;]*m", '')
  return @{ Code = $code; Out = $out }
}

# @brief SHA-256 per file under src, web, doc (repo-relative path, forward slashes)
function Get-TreeHashes([string]$Dir) {
  $h = @{}
  $sha = [System.Security.Cryptography.SHA256]::Create()
  foreach ($d in @('src', 'web', 'doc')) {
    $root = Join-Path $Dir $d
    if (-not (Test-Path -LiteralPath $root)) { continue }
    foreach ($f in (Get-ChildItem -LiteralPath $root -Recurse -File)) {
      if ($f.Name -like '*.backup' -or $f.Name -like '*.err') { continue }
      $rel = $f.FullName.Substring($Dir.Length).TrimStart('\', '/') -replace '\\', '/'
      $h[$rel] = [BitConverter]::ToString($sha.ComputeHash([System.IO.File]::ReadAllBytes($f.FullName)))
    }
  }
  $sha.Dispose()
  return $h
}

# @brief Changed, added and removed files between two hash maps
function Compare-Trees($Old, $New) {
  $changed = @()
  foreach ($k in $New.Keys) {
    if (-not $Old.ContainsKey($k)) { $changed += "+$k" }
    elseif ($Old[$k] -ne $New[$k]) { $changed += $k }
  }
  foreach ($k in $Old.Keys) { if (-not $New.ContainsKey($k)) { $changed += "-$k" } }
  return , @($changed | Sort-Object)
}

$script:LockFiles = @('src/EffectIds.lock', 'src/EffectParameters.lock')
$script:IdFiles = @('src/NeoPixel.Segment.templ.xml', 'src/NeoPixel.Cue.part.xml', 'src/NeoPixel.Scene.part.xml')

# @brief Parameter (Name, ParameterType, Text) and ParameterRef (RefId) per ID of the files that carry effect IDs
function Get-IdMap([string]$Dir) {
  $p = @{}; $r = @{}
  foreach ($rel in $script:IdFiles) {
    $text = [System.IO.File]::ReadAllText((Join-Path $Dir $rel))
    foreach ($m in [regex]::Matches($text, '<Parameter\s+Id="([^"]+)"([^>]*)>')) {
      $a = $m.Groups[2].Value
      $name = [regex]::Match($a, '\bName="([^"]*)"').Groups[1].Value
      $type = [regex]::Match($a, '\bParameterType="([^"]*)"').Groups[1].Value
      $txt = [regex]::Match($a, '\bText="([^"]*)"').Groups[1].Value
      $p["$rel|$($m.Groups[1].Value)"] = "Name=$name|Type=$type|Text=$txt"
    }
    foreach ($m in [regex]::Matches($text, '<ParameterRef\s+Id="([^"]+)"\s+RefId="([^"]+)"')) { $r["$rel|$($m.Groups[1].Value)"] = $m.Groups[2].Value }
  }
  return @{ P = $p; R = $r }
}

# ------------------------------------------------------------------------------------------------
# Assertions
# ------------------------------------------------------------------------------------------------

# @brief ID check: every old ID keeps its meaning or is listed as changed/removed; new IDs match AddedPattern
function Test-Ids($Old, $New, [string[]]$Removed, [string[]]$Changed, [string]$AddedPattern) {
  $f = @()
  foreach ($kind in @('P', 'R')) {
    foreach ($k in $Old[$kind].Keys) {
      $id = $k.Split('|')[1]
      if (-not $New[$kind].ContainsKey($k)) {
        if (-not ($Removed | Where-Object { $id -like $_ })) { $f += "$kind $k removed" }
      }
      elseif ($Old[$kind][$k] -ne $New[$kind][$k] -and -not ($Changed | Where-Object { $id -like $_ })) {
        $f += "$kind $k changed: $($Old[$kind][$k]) -> $($New[$kind][$k])"
      }
    }
    foreach ($k in $New[$kind].Keys) {
      if ($Old[$kind].ContainsKey($k)) { continue }
      $id = $k.Split('|')[1]
      if (-not $AddedPattern -or $id -notmatch $AddedPattern) { $f += "$kind $k added" }
    }
  }
  return , $f
}

# @brief Scenario: edit, run, check the allowed result
function Test-Allowed([string]$Name, [scriptblock]$Edit, [string[]]$Expect, [string[]]$Removed = @(), [string[]]$Changed = @(), [string]$Added = '', [scriptblock]$Extra = $null, [switch]$SameFingerprint) {
  if ($Scenario.Count -gt 0 -and $Scenario -notcontains $Name) { return }
  $dir = Join-Path $tempRoot $Name
  $f = @()
  try {
  Copy-Folder $script:Base $dir
  & $Edit $dir
  $before = Get-TreeHashes $dir
  $ids = Get-IdMap $dir
  $c = Invoke-Generator $dir @('-Check')
  if ($c.Code -ne 1) { $f += "-Check before the run: exit $($c.Code), expected 1" }
  $g = Invoke-Generator $dir @('-SkipOpenKNXproducer')
  if ($g.Code -ne 0) { $f += "run: exit $($g.Code): " + (($g.Out -split "`n" | Where-Object { $_ -match 'FATAL|ERROR|Exception|throw|  x ' } | Select-Object -First 3) -join ' / ') }
  else {
    $c = Invoke-Generator $dir @('-Check')
    if ($c.Code -ne 0) { $f += "-Check after the run: exit $($c.Code), expected 0" }
    $g2 = Invoke-Generator $dir @('-SkipOpenKNXproducer')
    if ($g2.Code -ne 0 -or $g2.Out -notmatch 'Effekte: unver') { $f += "second run is not 'unveraendert' (exit $($g2.Code))" }
    $after = Get-TreeHashes $dir
    $diff = Compare-Trees $before $after
    $got = @($diff | Where-Object { $_ -ne 'src/EffectParameters.fingerprint' })
    $exp = @($Expect | Sort-Object)
    $missing = @($exp | Where-Object { $got -notcontains $_ })
    $unexpected = @($got | Where-Object { $exp -notcontains $_ })
    if ($missing.Count -gt 0) { $f += "expected change missing: $($missing -join ', ')" }
    if ($unexpected.Count -gt 0) { $f += "unexpected change: $($unexpected -join ', ')" }
    if (-not $SameFingerprint -and $diff -notcontains 'src/EffectParameters.fingerprint') { $f += "fingerprint not rewritten" }
    if ($SameFingerprint -and $diff -contains 'src/EffectParameters.fingerprint') { $f += "fingerprint rewritten although the inputs did not change" }
    $f += Test-Ids $ids (Get-IdMap $dir) $Removed $Changed $Added
    if ($Extra) { $f += @(& $Extra $dir) }
  }
  }
  catch { $f += "test error: $($_.Exception.Message)" }
  Add-Result $Name 'allowed' $f
}

# @brief Scenario: edit, run, check that the run stops with the message and writes nothing
function Test-Stop([string]$Name, [scriptblock]$Edit, [string]$Message, [switch]$AllowWrites, [scriptblock]$Extra = $null) {
  if ($Scenario.Count -gt 0 -and $Scenario -notcontains $Name) { return }
  $dir = Join-Path $tempRoot $Name
  $f = @()
  try {
  Copy-Folder $script:Base $dir
  & $Edit $dir
  $before = Get-TreeHashes $dir
  $g = Invoke-Generator $dir @('-SkipOpenKNXproducer')
  if ($g.Code -eq 0) { $f += "run did not stop" }
  if ($g.Out -notmatch $Message) { $f += "message /$Message/ missing; got: " + (($g.Out -split "`n" | Where-Object { $_ -match 'FATAL|ERROR|throw|  x |Exception' } | Select-Object -First 2) -join ' / ') }
  $diff = Compare-Trees $before (Get-TreeHashes $dir)
  # -AllowWrites: a stop after the outputs (missing block marker) may leave outputs; the locks never change
  if ($AllowWrites) { $diff = @($diff | Where-Object { $script:LockFiles -contains ($_ -replace '^[+-]', '') }) }
  if ($diff.Count -gt 0) { $f += "stopped run wrote: $($diff -join ', ')" }
  $c = Invoke-Generator $dir @('-Check')
  if ($c.Code -ne 1) { $f += "-Check after the stop: exit $($c.Code), expected 1" }
  if ($Extra) { $f += @(& $Extra $dir) }
  }
  catch { $f += "test error: $($_.Exception.Message)" }
  Add-Result $Name 'stops' $f
}

# @brief Lock line of (effect, index) in src/EffectParameters.lock, $null when missing
function Get-LockLine([string]$Dir, [int]$EffectId, [int]$Index) {
  foreach ($l in [System.IO.File]::ReadAllLines((Join-Path $Dir 'src/EffectParameters.lock'))) {
    if ($l.StartsWith("$EffectId`t") -and ($l -split "`t")[2] -eq "$Index") { return $l }
  }
  return $null
}

# @brief Result of a custom scenario (several runs): failures from the script block
function Test-Custom([string]$Name, [string]$Kind, [scriptblock]$Body) {
  if ($Scenario.Count -gt 0 -and $Scenario -notcontains $Name) { return }
  $dir = Join-Path $tempRoot $Name
  $f = @()
  try {
    Copy-Folder $script:Base $dir
    $f += @(& $Body $dir)
  }
  catch { $f += "test error: $($_.Exception.Message)" }
  Add-Result $Name $Kind $f
}

function Add-Result([string]$Name, [string]$Kind, [array]$Failures) {
  $script:Results.Add([pscustomobject]@{ Scenario = $Name; Expected = $Kind; Result = $(if ($Failures.Count -eq 0) { 'PASS' } else { 'FAIL' }); Detail = ($Failures -join ' | ') })
  $color = if ($Failures.Count -eq 0) { 'Green' } else { 'Red' }
  Write-Host ("{0,-22} {1,-8} {2}" -f $Name, $Kind, $(if ($Failures.Count -eq 0) { 'PASS' } else { 'FAIL' })) -ForegroundColor $color
  foreach ($x in $Failures) { Write-Host "    $x" -ForegroundColor Yellow }
}

# ------------------------------------------------------------------------------------------------
# Edit helpers
# ------------------------------------------------------------------------------------------------

function Edit-File([string]$Path, [string]$Old, [string]$New) {
  $text = [System.IO.File]::ReadAllText($Path)
  $n = ([regex]::Matches($text, [regex]::Escape($Old))).Count
  if ($n -ne 1) { throw "$Path : '$Old' found $n times" }
  [System.IO.File]::WriteAllText($Path, $text.Replace($Old, $New), $utf8)
}

function Fx([string]$Dir, [string]$File) { return (Join-Path $Dir "lib/OFM-NeoPixel/src/effects/$File") }

# @brief Wipe parameters: name lines of the getParameterName switch
function Set-WipeNames([string]$Dir, [string[]]$Names) {
  $path = Fx $Dir 'EffectWipe.h'
  $old = "            case 0: return `"Speed`";`n            case 1: return `"Direction`";`n            case 2: return `"Loop`";`n"
  $new = ''
  for ($i = 0; $i -lt $Names.Count; $i++) { $new += "            case ${i}: return `"$($Names[$i])`";`n" }
  Edit-File $path $old $new
  # every parameter needs its ETS label (getParameterLabel, PARAM_LABEL_DE_EN): the labels of the header for its names, the name otherwise
  $oldLabels = "            case 0: return PARAM_LABEL_DE_EN(`"Geschwindigkeit`", `"Speed`");`n            case 1: return PARAM_LABEL_DE_EN(`"Richtung`", `"Direction`");`n            case 2: return PARAM_LABEL_DE_EN(`"Wiederholen`", `"Repeat`");`n"
  $known = @{ Speed = @('Geschwindigkeit', 'Speed'); Direction = @('Richtung', 'Direction'); Loop = @('Wiederholen', 'Repeat') }
  $newLabels = ''
  for ($i = 0; $i -lt $Names.Count; $i++) {
    $pair = if ($known.ContainsKey($Names[$i])) { $known[$Names[$i]] } else { @($Names[$i], $Names[$i]) }
    $newLabels += "            case ${i}: return PARAM_LABEL_DE_EN(`"$($pair[0])`", `"$($pair[1])`");`n"
  }
  Edit-File $path $oldLabels $newLabels
  Edit-File $path 'uint8_t getParameterCount() const override { return 3; }' "uint8_t getParameterCount() const override { return $($Names.Count); }"
}

# @brief New effect header (copy of BPMEffect.h) and its registration at the end of getEffectByIndex()
function Add-Effect([string]$Dir, [string]$Base, [string]$DisplayName, [int]$ParamCount = 2, [switch]$TwoD, [switch]$NoTwoDEntry) {
  $text = [System.IO.File]::ReadAllText((Fx $Dir 'BPMEffect.h')).Replace('BPMEffect', "${Base}Effect")
  $text = $text.Replace('const char* getName(const char* lang = nullptr) override { return "BPM"; }', "const char* getName(const char* lang = nullptr) override { return `"$DisplayName`"; }")
  $text = $text.Replace('uint8_t getParameterCount() const override { return 2; }', "uint8_t getParameterCount() const override { return $ParamCount; }")
  # a 2D effect is one whose getCapabilities() has no DIM_1D (Build-EffectParameters.ps1 Get-EffectDimension)
  if ($TwoD) {
    $text = $text.Replace("uint8_t getParameterCount() const override { return $ParamCount; }", "uint8_t getParameterCount() const override { return $ParamCount; }`n    uint8_t getCapabilities() const override { return DIM_2D; }")
  }
  [System.IO.File]::WriteAllText((Fx $Dir "${Base}Effect.h"), $text, $utf8)
  $flag = "NEOPIXEL_DISABLE_" + $Base.ToUpperInvariant()
  Edit-File (Fx $Dir 'EffectPool.cpp') "    s_effectNumbers = n;" "#ifndef $flag`n    if (index == n) return get${Base}();`n#endif`n    n++;`n`n    s_effectNumbers = n;"
  if ($TwoD -and -not $NoTwoDEntry) {
    Edit-File (Fx $Dir 'EffectPool.h') "#ifdef NEOPIXEL_DISABLE_2D`n" "#ifdef NEOPIXEL_DISABLE_2D`n    #ifndef $flag`n        #define $flag`n    #endif`n"
  }
}

function Remove-Registration([string]$Dir, [string]$Getter, [string]$Flag) {
  Edit-File (Fx $Dir 'EffectPool.cpp') "#ifndef $Flag`n    if (index == n) return get${Getter}();`n#endif`n    n++;`n" ''
}

function Set-Groups([string]$Dir, [string]$Groups) {
  Edit-File (Join-Path $Dir 'src/NeoPixel.EffectGroups.xml') "<EffectGroups>`n" "<EffectGroups>`n$Groups`n"
}

# ------------------------------------------------------------------------------------------------
# Run
# ------------------------------------------------------------------------------------------------

Write-Host "Test-EffectChanges: fixture $tempRoot"
$script:Base = Join-Path $tempRoot '_base'
New-BaseFixture $script:Base
$b = Invoke-Generator $script:Base @('-Check')
if ($b.Code -ne 0) {
  # the fixture is the repository state: it must be current (else commit/regenerate first)
  Write-Host "Base fixture is not current (-Check exit $($b.Code)) - run scripts/Build-EffectParameters.ps1 in the repository first" -ForegroundColor Red
  if (-not $KeepTemp) { Remove-Item -LiteralPath $tempRoot -Recurse -Force }
  exit 1
}

$help = 'src/Baggages/Help_de'
$xmlAll = @('src/NeoPixel.Segment.templ.xml', 'src/NeoPixel.Cue.part.xml', 'src/NeoPixel.Scene.part.xml', 'src/NeoPixel.share.xml',
  'src/NeoPixel.EM.share.xml', 'src/NeoPixel.script.src.js', 'src/EffectParameterMapping.h', 'src/EffectTypeMapping.h',
  'src/WebUi/NeoPixelEffectCatalog.generated.h', 'web/assets/effects.json', 'src/NeoPixelEffectGroups.h', 'src/EffectIds.lock',
  "$help/NEO-Effekt-Typ.md", "$help/NEO-Effekt-Typ-Szene.md", "$help/NEO-Cue-Effekt.md", "$help/NEO-Effekt-ID-Referenz.md", 'doc/Effect-Flags.md')

# 1 description
Test-Allowed 'ParamDescription' {
  param($d) Edit-File (Fx $d 'EffectWipe.h') 'PARAM_DESC_DE_EN("Geschwindigkeit: Wischgeschwindigkeit"' 'PARAM_DESC_DE_EN("Geschwindigkeit: Wischgeschwindigkeit (Test)"'
} -Expect @("$help/NEO-Speed-Wipe.md")

# hand edit of a generated help file: outdated, one run restores the base file, the fingerprint stays
Test-Allowed 'HelpHandEdit' {
  param($d) [System.IO.File]::AppendAllText((Join-Path $d "$help/NEO-Speed-Wipe.md"), "`nHand edit`n")
} -Expect @("$help/NEO-Speed-Wipe.md") -SameFingerprint -Extra {
  param($d)
  if ([System.IO.File]::ReadAllText((Join-Path $d "$help/NEO-Speed-Wipe.md")) -ne [System.IO.File]::ReadAllText((Join-Path $script:Base "$help/NEO-Speed-Wipe.md"))) { 'help file not restored to the generated text' }
}

# 2 default and maximum
Test-Allowed 'ParamDefaultMax' {
  param($d)
  Edit-File (Fx $d 'EffectWipe.h') 'case 0: return 25; // default speed' 'case 0: return 30; // default speed'
  Edit-File (Fx $d 'TheaterChaseEffect.h') 'case 1: return 10;  // Spacing max' 'case 1: return 12;  // Spacing max'
} -Expect @('src/NeoPixel.Segment.templ.xml', 'src/NeoPixel.Cue.part.xml', 'src/NeoPixel.Scene.part.xml', 'src/NeoPixel.share.xml',
  'src/NeoPixel.script.src.js', 'web/assets/effects.json', 'src/WebUi/NeoPixelEffectCatalog.generated.h',
  "$help/NEO-Speed-Wipe.md", "$help/NEO-Spacing-Theater-Chase.md", 'src/EffectParameters.lock') -Changed @()

# 3 parameter appended to an early effect
Test-Allowed 'ParamAppend' {
  param($d)
  Set-WipeNames $d @('Speed', 'Direction', 'Loop', 'Fade')
} -Expect @('src/NeoPixel.Segment.templ.xml', 'src/NeoPixel.Cue.part.xml', 'src/NeoPixel.Scene.part.xml', 'src/NeoPixel.share.xml',
  'src/NeoPixel.script.src.js', 'src/EffectParameterMapping.h', 'web/assets/effects.json', 'src/WebUi/NeoPixelEffectCatalog.generated.h',
  "+$help/NEO-Fade-Wipe.md", 'src/EffectParameters.lock') -Added '^%AID%_UP-(21%CC%0103|22%CC%%CUE_TPP\+13%)(_R-.*)?$|%PPP\+11%_R-%TT%%CC%%PPP\+11%03$'

# 4 parameter inserted in the middle / reordered
Test-Stop 'ParamInsert' { param($d) Set-WipeNames $d @('Speed', 'Fade', 'Direction', 'Loop') } -Message "moved from index"

# 4b a parameter without its ETS label (getParameterLabel / PARAM_LABEL_DE_EN in the effect header)
Test-Stop 'ParamLabelMissing' {
  param($d) Edit-File (Fx $d 'EffectWipe.h') "            case 2: return PARAM_LABEL_DE_EN(`"Wiederholen`", `"Repeat`");`n" ''
} -Message "parameter 2 \(Loop\) has no label"
Test-Stop 'ParamReorder' { param($d) Set-WipeNames $d @('Speed', 'Loop', 'Direction') } -Message "moved from index"

# 5 parameter removed: the last one (allowed) / one in the middle (stops)
Test-Allowed 'ParamRemoveLast' { param($d) Set-WipeNames $d @('Speed', 'Direction') } `
  -Expect @('src/NeoPixel.Segment.templ.xml', 'src/NeoPixel.Cue.part.xml', 'src/NeoPixel.Scene.part.xml', 'src/NeoPixel.share.xml',
  'src/NeoPixel.script.src.js', 'src/EffectParameterMapping.h', 'web/assets/effects.json', 'src/WebUi/NeoPixelEffectCatalog.generated.h',
  "-$help/NEO-Loop-Wipe.md", 'src/EffectParameters.lock') `
  -Removed @('%AID%_UP-21%CC%0102*', '%AID%_UP-22%CC%%CUE_TPP+12%*', '%AID%_UP-%TT%%CC%%PPP+10%_R-%TT%%CC%%PPP+10%03') `
  -Extra { param($d) if (-not ([System.IO.File]::ReadAllText((Join-Path $d 'src/EffectParameters.lock')) -match "1`tEffectWipe`t2`tLoop`tbool`tremoved")) { "lock has no 'removed' entry for Wipe index 2" } }
Test-Stop 'ParamRemoveMiddle' { param($d) Set-WipeNames $d @('Speed', 'Loop') } -Message "moved from index"
Test-Stop 'ParamReuseRemoved' {
  param($d)
  # a removed index keeps its ETS values: a new parameter there stops the run
  Edit-File (Join-Path $d 'src/EffectParameters.lock') "1`tEffectWipe`t2`tLoop`tbool`n" "1`tEffectWipe`t2`tLoop`tbool`tremoved`n"
  Set-WipeNames $d @('Speed', 'Direction', 'Fade')
} -Message "was 'Loop' \(removed\)"

# 6 parameter renamed: same ID, new name / type / text, help file replaced
Test-Allowed 'ParamRename' { param($d) Set-WipeNames $d @('Speed', 'Richtung', 'Loop') } `
  -Expect @('src/NeoPixel.Segment.templ.xml', 'src/NeoPixel.Cue.part.xml', 'src/NeoPixel.share.xml', 'src/NeoPixel.script.src.js',
  'src/EffectParameterMapping.h', 'web/assets/effects.json', 'src/WebUi/NeoPixelEffectCatalog.generated.h',
  "-$help/NEO-Direction-Wipe.md", "+$help/NEO-Richtung-Wipe.md", 'src/EffectParameters.lock', 'src/NeoPixel.Scene.part.xml') `
  -Changed @('%AID%_UP-21%CC%0101', '%AID%_UP-22%CC%%CUE_TPP+11%', '%AID%_UP-%TT%%CC%%PPP+9%_R-%TT%%CC%%PPP+9%03') `
  -Extra {
    param($d)
    # a rename keeps the type: the lock line of index 1 differs from the base only in the name
    $old = Get-LockLine $script:Base 1 1
    $new = Get-LockLine $d 1 1
    if (-not $old -or -not $new) { 'lock line of Wipe index 1 missing' }
    elseif ($new -cne $old.Replace("`tDirection`t", "`tRichtung`t")) { "lock line of Wipe index 1: '$new', expected the base type ('$old')" }
  }

# 7 effect appended
$effectAdded = '^%AID%_UP-(21%CC%33\d\d|22%CC%%CUE_TPP\+33\d%)(_R-.*)?$|_R-%TT%%CC%%PPP\+\d+%35$'
Test-Allowed 'EffectAppend' { param($d) Add-Effect $d 'Blink' 'Blink' } `
  -Expect @('src/NeoPixel.Segment.templ.xml', 'src/NeoPixel.Cue.part.xml', 'src/NeoPixel.Scene.part.xml', 'src/NeoPixel.share.xml',
  'src/NeoPixel.EM.share.xml', 'src/NeoPixel.script.src.js', 'src/EffectParameterMapping.h', 'src/EffectTypeMapping.h',
  'web/assets/effects.json', 'src/WebUi/NeoPixelEffectCatalog.generated.h', 'src/NeoPixelEffectGroups.h', 'src/EffectIds.lock',
  'src/EffectParameters.lock', "$help/NEO-Effekt-Typ.md", "$help/NEO-Effekt-Typ-Szene.md", "$help/NEO-Cue-Effekt.md", "$help/NEO-Effekt-ID-Referenz.md", 'doc/Effect-Flags.md',
  "+$help/NEO-BPM-Blink.md", "+$help/NEO-Hue-Blink.md") -Added $effectAdded `
  -Extra {
    param($d)
    $h = [System.IO.File]::ReadAllText((Join-Path $d 'src/NeoPixelEffectGroups.h'))
    if ($h -notmatch '#if defined\(NEOPIXEL_DISABLE_BLINK\)\s+#define NEO_FXOFF_33 1') { 'NEO_FXOFF_33 not guarded by NEOPIXEL_DISABLE_BLINK only' }
    if ([System.IO.File]::ReadAllText((Join-Path $d 'src/EffectIds.lock')) -notmatch "33`tBlinkEffect") { 'EffectIds.lock has no number 33' }
    $two = [System.IO.File]::ReadAllText((Join-Path $d 'src/NeoPixel.Effects2D.values'))
    if ($two -match '(?m)^33$') { 'Blink (1D) listed in NeoPixel.Effects2D.values' }
  }

# 10 new 2D effect
Test-Allowed 'Effect2DAppend' { param($d) Add-Effect $d 'Blink2D' 'Blink 2D' -TwoD } `
  -Expect @('src/NeoPixel.Segment.templ.xml', 'src/NeoPixel.Cue.part.xml', 'src/NeoPixel.Scene.part.xml', 'src/NeoPixel.share.xml',
  'src/NeoPixel.EM.share.xml', 'src/NeoPixel.script.src.js', 'src/EffectParameterMapping.h', 'src/EffectTypeMapping.h',
  'web/assets/effects.json', 'src/WebUi/NeoPixelEffectCatalog.generated.h', 'src/NeoPixelEffectGroups.h', 'src/EffectIds.lock',
  'src/EffectParameters.lock', "$help/NEO-Effekt-Typ.md", "$help/NEO-Effekt-Typ-Szene.md", "$help/NEO-Cue-Effekt.md", "$help/NEO-Effekt-ID-Referenz.md", 'doc/Effect-Flags.md', 'src/NeoPixel.Effects2D.values',
  "+$help/NEO-BPM-Blink-2D.md", "+$help/NEO-Hue-Blink-2D.md") -Added $effectAdded `
  -Extra {
    param($d)
    $h = [System.IO.File]::ReadAllText((Join-Path $d 'src/NeoPixelEffectGroups.h'))
    if ($h -notmatch '#if defined\(NEOPIXEL_DISABLE_BLINK2D\) \|\| defined\(NEOPIXEL_DISABLE_2D\)\s+#define NEO_FXOFF_33 1') { 'NEO_FXOFF_33 not guarded by NEOPIXEL_DISABLE_2D' }
    $s = [System.IO.File]::ReadAllText((Join-Path $d 'src/NeoPixel.share.xml'))
    $two = [System.IO.File]::ReadAllText((Join-Path $d 'src/NeoPixel.Effects2D.values'))
    $all = [regex]::Match($s, '(?s)GENERATED_EFFECT_ENUMERATIONS_START(.*?)GENERATED_EFFECT_ENUMERATIONS_END').Groups[1].Value
    if ($two -notmatch '(?m)^33$') { 'Blink 2D missing in NeoPixel.Effects2D.values' }
    if ($all -notmatch 'Value="33"') { 'Blink 2D missing in the full list' }
  }
Test-Stop 'Effect2DNoEntry' { param($d) Add-Effect $d 'Blink2D' 'Blink 2D' -TwoD -NoTwoDEntry } -Message "member of NEOPIXEL_DISABLE_2D"

# 8 effect removed / 9 effects reordered
Test-Stop 'EffectRemoveLast' {
  param($d) Remove-Registration $d 'Metaballs2D' 'NEOPIXEL_DISABLE_METABALLS2D'; Remove-Item -LiteralPath (Fx $d 'Metaballs2DEffect.h')
} -Message "removed from EffectPool.cpp getEffectByIndex"
Test-Stop 'EffectRemoveMiddle' { param($d) Remove-Registration $d 'BPM' 'NEOPIXEL_DISABLE_BPM' } -Message "Effect ID\(s\) changed"
Test-Stop 'EffectReorder' {
  param($d)
  Edit-File (Fx $d 'EffectPool.cpp') "#ifndef NEOPIXEL_DISABLE_JUGGLE`n    if (index == n) return getJuggle();`n#endif`n    n++;`n#ifndef NEOPIXEL_DISABLE_BPM`n    if (index == n) return getBPM();`n#endif`n" "#ifndef NEOPIXEL_DISABLE_BPM`n    if (index == n) return getBPM();`n#endif`n    n++;`n#ifndef NEOPIXEL_DISABLE_JUGGLE`n    if (index == n) return getJuggle();`n#endif`n"
} -Message "Effect ID\(s\) changed"

# 11 effect groups
Test-Allowed 'GroupAdd' { param($d) Set-Groups $d '  <Group name="Small" text="test" effects="FireEffect CandleEffect" />' } `
  -Expect @('src/NeoPixelEffectGroups.h') -Extra {
    param($d)
    $h = [System.IO.File]::ReadAllText((Join-Path $d 'src/NeoPixelEffectGroups.h'))
    if ($h -notmatch '(?s)#ifdef NEOPIXEL_DISABLE_GROUP_SMALL.*#define NEOPIXEL_DISABLE_FIRE.*#define NEOPIXEL_DISABLE_CANDLE.*#endif') { 'group block missing' }
  }
Test-Stop 'GroupUnknownEffect' { param($d) Set-Groups $d '  <Group name="Small" effects="NoSuchEffect" />' } -Message "unknown effect 'NoSuchEffect'"
Test-Stop 'GroupDuplicate' { param($d) Set-Groups $d "  <Group name=`"Small`" effects=`"FireEffect`" />`n  <Group name=`"small`" effects=`"CandleEffect`" />" } -Message "used twice"
Test-Stop 'GroupSolid' { param($d) Set-Groups $d '  <Group name="Small" effects="EffectSolid" />' } -Message "cannot be disabled"

# 12 guards: effect number above 99, cue string parameter index above 9
Test-Stop 'EffectNumber100' {
  param($d)
  for ($n = 33; $n -le 100; $n++) { $pc = 0; if ($n -eq 100) { $pc = 2 }; Add-Effect $d "Gen$n" "Gen $n" -ParamCount $pc }
} -Message "Effect ID 100 cannot own generated parameters"
Test-Stop 'CueStringIndex10' {
  param($d)
  $names = @('Speed', 'Direction', 'Loop', 'P3', 'P4', 'P5', 'P6', 'P7', 'P8', 'P9', 'Text10')
  Set-WipeNames $d $names
  Edit-File (Fx $d 'EffectWipe.h') '            case 2: return ParameterType::PARAM_BOOL;  // Loop' "            case 2: return ParameterType::PARAM_BOOL;  // Loop`n            case 10: return ParameterType::PARAM_STRING;"
} -Message "typed cue parameter index 10"

# 6b parameter type: kind change, enum values, number range
$wipeEnum = "            case WIPE_EDGES_IN: return `"Aussen nach Mitte`";`n"
Test-Stop 'ParamKindChange' {
  param($d) Edit-File (Fx $d 'EffectWipe.h') 'case 2: return ParameterType::PARAM_BOOL;  // Loop' 'case 2: return ParameterType::PARAM_UINT8;  // Loop'
} -Message "parameter 'Loop' \(index 2\): type bool -> number"
Test-Stop 'ParamEnumReorder' {
  param($d)
  Edit-File (Fx $d 'EffectWipe.h') 'case WIPE_LEFT_TO_RIGHT: return "Links nach rechts";' 'case WIPE_LEFT_TO_RIGHT: return "Rechts nach links X";'
  Edit-File (Fx $d 'EffectWipe.h') 'case WIPE_RIGHT_TO_LEFT: return "Rechts nach links";' 'case WIPE_RIGHT_TO_LEFT: return "Links nach rechts";'
  Edit-File (Fx $d 'EffectWipe.h') 'case WIPE_LEFT_TO_RIGHT: return "Rechts nach links X";' 'case WIPE_LEFT_TO_RIGHT: return "Rechts nach links";'
} -Message "enum value 0 'Links nach rechts' is now 'Rechts nach links'"
Test-Stop 'ParamEnumRemove' {
  param($d)
  Edit-File (Fx $d 'EffectWipe.h') $wipeEnum ''
  Edit-File (Fx $d 'EffectWipe.h') 'return (paramIndex == 1) ? 6 : 0;' 'return (paramIndex == 1) ? 5 : 0;'
} -Message "enum value 5 'Aussen nach Mitte' removed"
Test-Allowed 'ParamEnumAppend' {
  param($d)
  Edit-File (Fx $d 'EffectWipe.h') $wipeEnum ($wipeEnum + "            case 6: return `"Zufall`";`n")
  Edit-File (Fx $d 'EffectWipe.h') 'return (paramIndex == 1) ? 6 : 0;' 'return (paramIndex == 1) ? 7 : 0;'
} -Expect @('src/NeoPixel.share.xml', 'web/assets/effects.json', 'src/WebUi/NeoPixelEffectCatalog.generated.h', 'src/EffectParameters.lock', "$help/NEO-Direction-Wipe.md") `
  -Extra { param($d) if ((Get-LockLine $d 1 1) -notmatch ';5=Aussen nach Mitte;6=Zufall$') { "lock line of Wipe index 1 lacks the appended value: $(Get-LockLine $d 1 1)" } }
Test-Allowed 'ParamRangeWiden' {
  param($d)
  Edit-File (Fx $d 'TheaterChaseEffect.h') 'case 1: return 1; // Spacing min' 'case 1: return 0; // Spacing min'
  Edit-File (Fx $d 'TheaterChaseEffect.h') 'case 1: return 10;  // Spacing max' 'case 1: return 12;  // Spacing max'
} -Expect @('src/NeoPixel.share.xml', 'web/assets/effects.json', 'src/WebUi/NeoPixelEffectCatalog.generated.h',
  "$help/NEO-Spacing-Theater-Chase.md", 'src/EffectParameters.lock') `
  -Extra { param($d) if ((Get-LockLine $d 9 1) -cne "9`tTheaterChaseEffect`t1`tSpacing`tnumber:0..12") { "lock line of TheaterChase index 1: $(Get-LockLine $d 9 1)" } }
Test-Stop 'ParamRangeNarrow' {
  param($d) Edit-File (Fx $d 'TheaterChaseEffect.h') 'case 1: return 10;  // Spacing max' 'case 1: return 8;  // Spacing max'
} -Message "range 1\.\.10 narrowed to 1\.\.8"

# lock files: created when missing (first run), written only by a successful full run
Test-Allowed 'LockMissing' {
  param($d) foreach ($l in $script:LockFiles) { Remove-Item -LiteralPath (Join-Path $d $l) }
} -Expect @('+src/EffectIds.lock', '+src/EffectParameters.lock') -SameFingerprint -Extra {
  param($d)
  foreach ($l in $script:LockFiles) {
    if ([System.IO.File]::ReadAllText((Join-Path $d $l)) -cne [System.IO.File]::ReadAllText((Join-Path $script:Base $l))) { "$l not re-created with the base content" }
  }
}
Test-Custom 'LockAfterStop' 'allowed' {
  param($d)
  $page = Join-Path $d 'doc/Effect-Flags.md'
  $marker = '<!-- BEGIN AUTO-GENERATED: Effect Flags - DO NOT EDIT MANUALLY -->'
  # 1. parameter appended, but the run stops at the missing block marker: the lock keeps the base state
  Set-WipeNames $d @('Speed', 'Direction', 'Loop', 'Fade')
  Edit-File $page $marker '<!-- BEGIN GENERATED: Effect Flags -->'
  $g = Invoke-Generator $d @('-SkipOpenKNXproducer')
  if ($g.Code -eq 0 -or $g.Out -notmatch 'not written by this run, kept') { "step 1: run did not stop at the missing marker (exit $($g.Code))" }
  foreach ($l in $script:LockFiles) {
    if ([System.IO.File]::ReadAllText((Join-Path $d $l)) -cne [System.IO.File]::ReadAllText((Join-Path $script:Base $l))) { "step 1: $l changed by the stopped run" }
  }
  # 2. both reverted: a full run back to the base state
  Edit-File $page '<!-- BEGIN GENERATED: Effect Flags -->' $marker
  Copy-Item -LiteralPath (Fx $script:Base 'EffectWipe.h') -Destination (Fx $d 'EffectWipe.h') -Force
  $g = Invoke-Generator $d @('-SkipOpenKNXproducer')
  if ($g.Code -ne 0) { "step 2: revert run exit $($g.Code)" }
  # 3. another parameter at the index the stopped run had: passes, the lock names it
  Set-WipeNames $d @('Speed', 'Direction', 'Loop', 'Glow')
  $g = Invoke-Generator $d @('-SkipOpenKNXproducer')
  if ($g.Code -ne 0) { "step 3: run with the other parameter: exit $($g.Code): " + (($g.Out -split "`n" | Where-Object { $_ -match 'FATAL|ERROR|index' } | Select-Object -First 2) -join ' / ') }
  $lock = [System.IO.File]::ReadAllText((Join-Path $d 'src/EffectParameters.lock'))
  if ($lock -notmatch "1`tEffectWipe`t3`tGlow`tnumber:") { 'step 3: lock has no Glow at Wipe index 3' }
  if ($lock -match "1`tEffectWipe`t3`tFade") { 'step 3: lock still names Fade at Wipe index 3' }
  $c = Invoke-Generator $d @('-Check')
  if ($c.Code -ne 0) { "step 3: -Check exit $($c.Code), expected 0" }
}
Test-Custom 'WebCatalogNoLock' 'allowed' {
  param($d)
  Set-WipeNames $d @('Speed', 'Direction', 'Loop', 'Fade')
  $g = Invoke-Generator $d @('-WebCatalogOnly')
  if ($g.Code -ne 0) { "-WebCatalogOnly: exit $($g.Code)" }
  foreach ($l in $script:LockFiles) {
    if ([System.IO.File]::ReadAllText((Join-Path $d $l)) -cne [System.IO.File]::ReadAllText((Join-Path $script:Base $l))) { "$l written by -WebCatalogOnly" }
  }
}

# generated page without its block marker (doc/Effect-Flags.md): kept, the run fails, -Check stays outdated
Test-Stop 'PageMarkerMissing' {
  param($d) Edit-File (Join-Path $d 'doc/Effect-Flags.md') '<!-- BEGIN AUTO-GENERATED: Effect Flags - DO NOT EDIT MANUALLY -->' '<!-- BEGIN GENERATED: Effect Flags -->'
} -Message "not written by this run, kept" -AllowWrites -Extra {
  # the help pages written whole (no block marker) must exist and must not look like a per-parameter help file, which
  # Remove-OrphanedEffectOutputs (Build-EffectParameters.ps1) deletes once a run no longer writes it
  param($d)
  foreach ($p in @('NEO-Effekt-Typ.md', 'NEO-Effekt-Typ-Szene.md', 'NEO-Cue-Effekt.md')) {
    $f = Join-Path $d "$help/$p"
    if (-not (Test-Path -LiteralPath $f)) { "$p was deleted"; continue }
    $text = [System.IO.File]::ReadAllText($f).Replace("`r`n", "`n").TrimStart([char]0xFEFF)
    if ($text -match '\A# [^\n]+\n\n\*\*Effekt:\*\* [^\n]+\n') { "$p matches the orphan pattern of a per-parameter help file" }
  }
}

# ------------------------------------------------------------------------------------------------
Write-Host ""
$failed = @($script:Results | Where-Object { $_.Result -ne 'PASS' })
if ($script:Results.Count -eq 0) { Write-Host "no scenario matched -Scenario $($Scenario -join ',')" -ForegroundColor Red; $failed = @('none') }
Write-Host ("{0} scenario(s), {1} passed, {2} failed" -f $script:Results.Count, ($script:Results.Count - $failed.Count), $failed.Count) -ForegroundColor $(if ($failed.Count) { 'Red' } else { 'Green' })
if ($KeepTemp) { Write-Host "kept: $tempRoot" } else { Remove-Item -LiteralPath $tempRoot -Recurse -Force }
if ($failed.Count -gt 0) { exit 1 }
exit 0
