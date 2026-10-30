#!/usr/bin/env pwsh
# Open #
# +----+  Invoke-NeoPixelEtsTest
# # KNX   2026 OpenKNX - Erkan Colak
#
# FILEPATH: test/ets/Invoke-NeoPixelEtsTest.ps1

<#
.SYNOPSIS
    Runs a data-driven test of an OpenKNX NeoPixel device: imports an ETS sample configuration,
    programs the device and checks bus, console, web page and LED image against expectations.

.DESCRIPTION
    The test is described by a steps file (JSON, next to each sample). The script
      1. imports the sample configuration into the ETS project through Knx-DeviceConfig.ps1
         (KNX-MCP plugin) and programs the device (optional, -Import / -Program)
      2. runs every step: actions (group write, wait, console command) followed by checks
      3. prints PASS/FAIL per check and exits with 0 (all passed), 1 (a check failed)
         or 2 (setup error: import, download, connection)

    Every expectation in a steps file names its source (ETS parameter or help page), so that a
    failing check points either to the device or to the expectation.

    Optional "configStageA" in the steps file: imported before "config" (the sample without LOG/FCB/LMG
    parameters, which the full "config" then adds in a second pass).

    Actions ("do"):
      { "write":  "1/3/0", "value": "1", "dpt": "1.001" }     GroupValue_Write
      { "wait":   1500 }                                       milliseconds
      { "console": "neo effect set 0 2" }                      console command, output ignored

    Checks ("expect"):
      { "type": "ga", "ga": "1/3/1", "dpt": "1.011", "value": "1" [, "tol": 2] }
            GroupValue_Read on the status address, value compared (numbers with tolerance)
      { "type": "console", "cmd": "neo info", "pattern": "Total LEDs:\\s+414" [, "absent": true] }
            regex on the console output of the command; with "group": n the captured group is compared
            ("value", "op"; "$name" = value stored earlier with "store": "name"), or with "changeMs" the
            command runs again after that time and the group must change ("change": true) or stay
            (e.g. "neo phys dump 2" -> lit=, sum=, hash= of the frame sent on the wire)
      { "type": "web", "path": "segs[0].fx", "value": 26 [, "op": "eq|ne|ge|le|gt|lt|match"] }
            value at a path of GET <WebUrl>/neo/state (or "url": "/neo/em")
      { "type": "effect", "range": [0, 63], "frames": 8, "intervalMs": 250, "property": "animated" [, args] }
            LED image from GET <WebUrl>/neo/pixels, property test on the captured frames
      Optional on every check: "timeoutMs" (poll until it passes), "note".

    Effect properties: constant, uniform, animated, dark, lit, brightnessVaries, hueSpread,
    sparse, movingPeak, strobe.

.PARAMETER Steps
    Path of the steps file (JSON).

.PARAMETER Device
    Individual address of the device under test. Default: "device" of the steps file.

.PARAMETER Import
    Import the configuration file named in the steps file ("config") before the steps.

.PARAMETER Program
    Program the device after the import (full download, success checked through bus_get_operations).

.PARAMETER Only
    Run only the steps with these ids.

.PARAMETER Bus
    Backend for group reads and writes: "ets" (KNX-MCP plugin, default) or "oknx" (oknx CLI).

.PARAMETER OknxIp
    KNXnet/IP interface for -Bus oknx and -Console oknx-remote. Default 10.0.0.60.

.PARAMETER Console
    Console backend: "oknx-remote" (oknx console over KNX to the device PA), "oknx-web"
    (oknx console over the web console of the device at -ConsoleIp), "serial" (USB, -SerialPort)
    or "none" (console checks are skipped and reported as SKIP).

.PARAMETER ConsoleIp
    Device IP for -Console oknx-web.

.PARAMETER SerialPort
    Serial port for -Console serial, e.g. /dev/cu.usbmodem844301 or COM5.

.PARAMETER WebUrl
    Base URL of the device web page for web and effect checks, e.g. http://10.0.0.125.

.PARAMETER ClientName
    Name of this client towards the KNX-MCP plugin (shown in the ETS undo list).

.PARAMETER Url
    KNX-MCP endpoint. Default $env:KNXMCP_URL or http://11.11.0.203:51900/mcp.

.PARAMETER KeyFile
    File holding the KNX-MCP access key. Default ~/.claude/skills/claude-ets-tester/access-key.

.PARAMETER DeviceConfigScript
    Path of Knx-DeviceConfig.ps1. Default ~/.claude/skills/claude-ets-tester/Knx-DeviceConfig.ps1.

.PARAMETER Report
    Optional path of a JSON report with every check result.

.EXAMPLE
    ./Invoke-NeoPixelEtsTest.ps1 -Steps ../../doc/ets-samples/1-einzeln/steps-1.1.3.json -Import -Program

.EXAMPLE
    ./Invoke-NeoPixelEtsTest.ps1 -Steps steps-1.1.5.json -Console oknx-web -ConsoleIp 10.0.0.125 -WebUrl http://10.0.0.125

.NOTES
    Author: Erkan Colak. Windows PowerShell 5.1 and PowerShell 7+.
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$Steps,
    [string]$Device,
    [switch]$Import,
    [switch]$Program,
    [string[]]$Only,
    [ValidateSet('ets', 'oknx')][string]$Bus = 'ets',
    [string]$OknxIp = '10.0.0.60',
    [ValidateSet('oknx-remote', 'oknx-web', 'serial', 'none')][string]$Console = 'oknx-remote',
    [string]$ConsoleIp,
    [string]$SerialPort,
    [string]$WebUrl,
    [string]$ClientName = 'neopixel-ets',
    [string]$Url,
    [string]$KeyFile,
    [string]$DeviceConfigScript,
    [string]$Report
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2.0

if (-not $Url) { if ($env:KNXMCP_URL) { $Url = $env:KNXMCP_URL } else { $Url = 'http://11.11.0.203:51900/mcp' } }
if (-not $KeyFile) { $KeyFile = Join-Path $HOME '.claude/skills/claude-ets-tester/access-key' }
if (-not $DeviceConfigScript) { $DeviceConfigScript = Join-Path $HOME '.claude/skills/claude-ets-tester/Knx-DeviceConfig.ps1' }

$Utf8NoBom = New-Object System.Text.UTF8Encoding $false
$Invariant = [Globalization.CultureInfo]::InvariantCulture
$script:SessionId = $null
$script:RpcId = 1
$script:Results = New-Object System.Collections.ArrayList
$script:ConsoleCache = @{}
$script:Store = @{}

# --- KNX-MCP transport ---------------------------------------------------------

function Get-McpKey {
    if ($env:KNXMCP_KEY) { return $env:KNXMCP_KEY }
    if (Test-Path -LiteralPath $KeyFile) { return ([IO.File]::ReadAllText($KeyFile)).Trim() }
    throw "No KNX-MCP access key: set KNXMCP_KEY or create $KeyFile"
}

function Invoke-McpPost([hashtable]$Message, [int]$TimeoutSec, [switch]$NoSession) {
    $headers = @{ 'X-KnxMcp-Key' = (Get-McpKey); 'Accept' = 'application/json, text/event-stream' }
    if (-not $NoSession -and $script:SessionId) { $headers['Mcp-Session-Id'] = $script:SessionId }
    $body = $Utf8NoBom.GetBytes((ConvertTo-Json -InputObject $Message -Depth 20 -Compress))
    $resp = Invoke-WebRequest -Uri $Url -Method Post -Headers $headers -Body $body -ContentType 'application/json; charset=utf-8' `
        -TimeoutSec $TimeoutSec -UseBasicParsing
    $sid = $resp.Headers['Mcp-Session-Id']
    if ($sid) { if ($sid -is [array]) { $sid = $sid[0] }; $script:SessionId = [string]$sid }
    $text = $Utf8NoBom.GetString($resp.RawContentStream.ToArray())
    if (-not $text.Trim()) { return $null }
    if ($text -match '(?m)^data:') {
        $lines = @($text -split "`r?`n" | Where-Object { $_ -like 'data:*' })
        $text = $lines[$lines.Count - 1].Substring(5).Trim()
    }
    return ConvertFrom-Json -InputObject $text
}

function Initialize-Mcp {
    $script:SessionId = $null
    $init = @{ jsonrpc = '2.0'; id = 0; method = 'initialize'; params = @{
            protocolVersion = '2025-03-26'; capabilities = @{}; clientInfo = @{ name = $ClientName; version = '1.0' } } }
    $null = Invoke-McpPost -Message $init -TimeoutSec 30 -NoSession
    if (-not $script:SessionId) { throw 'KNX-MCP initialize returned no session id' }
    $null = Invoke-McpPost -Message @{ jsonrpc = '2.0'; method = 'notifications/initialized' } -TimeoutSec 30
}

function Invoke-EtsTool([string]$Name, [hashtable]$Arguments = @{}, [int]$TimeoutSec = 120) {
    if (-not $script:SessionId) { Initialize-Mcp }
    $script:RpcId++
    $msg = @{ jsonrpc = '2.0'; id = $script:RpcId; method = 'tools/call'; params = @{ name = $Name; arguments = $Arguments } }
    try { $r = Invoke-McpPost -Message $msg -TimeoutSec $TimeoutSec }
    catch { Initialize-Mcp; $r = Invoke-McpPost -Message $msg -TimeoutSec $TimeoutSec }
    if ($r.PSObject.Properties['error'] -and $r.error) { throw ("{0}: {1}" -f $Name, $r.error.message) }
    $payload = $null
    if ($r.result.PSObject.Properties['structuredContent'] -and $r.result.structuredContent) { $payload = $r.result.structuredContent }
    else {
        $text = (@($r.result.content | Where-Object { $_.type -eq 'text' }) | ForEach-Object { $_.text }) -join ''
        $payload = ConvertFrom-Json -InputObject $text
    }
    if ($payload.PSObject.Properties['error'] -and $payload.error) { throw ("{0}: {1} - {2}" -f $Name, $payload.error.code, $payload.error.message) }
    return $payload.data
}

# --- bus backends --------------------------------------------------------------

function Find-Oknx {
    $c = Get-Command oknx -ErrorAction SilentlyContinue
    if ($c) { return $c.Source }
    throw 'oknx not found in PATH'
}

function Invoke-OknxText([string[]]$ArgList, [int]$TimeoutSec = 30) {
    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName = Find-Oknx
    $psi.Arguments = ($ArgList | ForEach-Object { if ($_ -match '\s') { '"' + $_ + '"' } else { $_ } }) -join ' '
    $psi.RedirectStandardOutput = $true; $psi.RedirectStandardError = $true; $psi.UseShellExecute = $false
    $p = [System.Diagnostics.Process]::Start($psi)
    $out = $p.StandardOutput.ReadToEndAsync(); $err = $p.StandardError.ReadToEndAsync()
    if (-not $p.WaitForExit($TimeoutSec * 1000)) { try { $p.Kill() } catch { }; throw "oknx timed out: $($psi.Arguments)" }
    return ($out.Result + $err.Result)
}

function Write-Group([string]$Ga, [string]$Value, [string]$Dpt) {
    if ($Bus -eq 'ets') {
        $a = @{ address = $Ga; value = $Value }
        if ($Dpt) { $a['dpt'] = $Dpt }
        $null = Invoke-EtsTool 'ga_write_value' $a
    } else {
        $sel = $Ga; $v = $Value
        if ($Value -match '^raw:([0-9A-Fa-f]+)$') { $v = [string][Convert]::ToInt32($Matches[1], 16) }   # 6-bit payload (DPT 1/2/3)
        elseif ($Dpt -match '^1[78]\.') { $v = '0x{0:X2}' -f ([int]$Value - 1) }   # scene n is sent as n-1 (oknx does not encode 17/18)
        elseif ($Dpt) { $sel = "$Ga`:$Dpt" }
        $null = Invoke-OknxText @('-q', '-i', $OknxIp, 'ga', 'write', $sel, $v)
    }
}

<#
.SYNOPSIS
    Reads a group address; returns the decoded value as text or $null without an answer.
#>
function Read-Group([string]$Ga, [string]$Dpt) {
    if ($Bus -eq 'ets') {
        $d = Invoke-EtsTool 'ga_read_value' @{ address = $Ga; request = $true; timeout_ms = 3000 }
        foreach ($v in @($d.values)) {
            if ($v.source -eq 'no_response') { return $null }
            foreach ($k in 'value', 'value_text', 'decoded') {
                if ($v.PSObject.Properties[$k] -and $null -ne $v.$k) { return [string]$v.$k }
            }
        }
        return $null
    }
    $text = Invoke-OknxText @('-q', '-i', $OknxIp, 'ga', 'read', $Ga)
    foreach ($line in ($text -split "`r?`n")) {
        # answer row: gaval <tab> <source PA>  <GA>  <hex octets> [(6 bit)]
        if ($line -match ('^gaval\s+\S+\s+' + [regex]::Escape($Ga) + '\s+(.+)$')) { return (ConvertFrom-RawGroupValue $Matches[1].Trim() $Dpt) }
    }
    return $null
}

<#
.SYNOPSIS
    Decodes the raw value oknx prints (hex octets, or 0xNN for 6-bit values) by DPT main number.
#>
function ConvertFrom-RawGroupValue([string]$Raw, [string]$Dpt) {
    $hex = $Raw -replace '\(.*\)', '' -replace '0x', '' -replace '\s', ''
    $bytes = @()
    for ($i = 0; $i + 1 -lt $hex.Length; $i += 2) { $bytes += [Convert]::ToInt32($hex.Substring($i, 2), 16) }
    if ($bytes.Count -eq 0) { return $Raw }
    $main = ($Dpt -split '\.')[0]
    switch ($main) {
        '1' { return [string]($bytes[0] -band 1) }
        '3' { return [string]($bytes[0] -band 15) }
        '5' { if ($Dpt -eq '5.001') { return [string][Math]::Round($bytes[0] * 100.0 / 255) } return [string]$bytes[0] }
        '7' { return [string]($bytes[0] * 256 + $bytes[1]) }
        '16' { return ((-join ($bytes | Where-Object { $_ -ne 0 } | ForEach-Object { [char]$_ }))).Trim() }
        '17' { return [string](($bytes[0] -band 63) + 1) }
        '18' { return [string](($bytes[0] -band 63) + 1) }
        '232' { return ('#{0:X2}{1:X2}{2:X2}' -f $bytes[0], $bytes[1], $bytes[2]) }
        default { return $hex }
    }
}

# --- console backends ------------------------------------------------------------

function Get-CleanText([string]$Text) {
    $t = [regex]::Replace($Text, "\x1b\[[0-9;?]*[A-Za-z]", '')
    $t = [regex]::Replace($t, '\$ [^\r\n]*?(?=\d+d \d\d:)', '')
    return [regex]::Replace($t, '(?m)^\d+d \d\d:\d\d:\d\d: ', '')   # uptime stamp of each console line
}

function Invoke-ConsoleCommand([string]$Command, [int]$IdleMs = 2500, [int]$MaxMs = 60000) {
    if ($Console -eq 'none') { return $null }
    if ($Console -eq 'serial') {
        $port = New-Object System.IO.Ports.SerialPort $SerialPort, 115200
        $port.ReadTimeout = 200; $port.NewLine = "`r`n"
        $port.Open()
        try {
            Start-Sleep -Milliseconds 300; $null = $port.ReadExisting()
            $port.Write($Command + "`r`n")
            $sb = New-Object System.Text.StringBuilder
            $sw = [Diagnostics.Stopwatch]::StartNew(); $last = 0
            while ($sw.ElapsedMilliseconds -lt $MaxMs) {
                $chunk = $port.ReadExisting()
                if ($chunk) { $null = $sb.Append($chunk); $last = $sw.ElapsedMilliseconds }
                elseif ($sw.ElapsedMilliseconds - $last -gt $IdleMs) { break }
                Start-Sleep -Milliseconds 100
            }
            return (Get-CleanText $sb.ToString())
        } finally { $port.Close() }
    }
    # oknx console: remote (KNX, device PA) or web console of the device itself
    $argList = @()
    if ($Console -eq 'oknx-remote') { $argList = @('-i', $OknxIp, $Device, 'con') } else { $argList = @('-i', $ConsoleIp, 'con') }
    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName = Find-Oknx; $psi.Arguments = $argList -join ' '
    $psi.RedirectStandardInput = $true; $psi.RedirectStandardOutput = $true; $psi.RedirectStandardError = $true
    $psi.UseShellExecute = $false
    $p = [System.Diagnostics.Process]::Start($psi)
    $sb = New-Object System.Text.StringBuilder
    $sw = [Diagnostics.Stopwatch]::StartNew()
    $task = $p.StandardOutput.ReadLineAsync()
    $sent = $false; $last = 0; $seenEcho = $false
    try {
        while ($sw.ElapsedMilliseconds -lt $MaxMs) {
            if ($task.Wait(200)) {
                $line = $task.Result
                if ($null -eq $line) { break }
                $last = $sw.ElapsedMilliseconds
                if ($sent) { $null = $sb.AppendLine($line); if ($line -match [regex]::Escape($Command)) { $seenEcho = $true } }
                elseif ($line -match 'verbunden|connected|-- console') {
                    Start-Sleep -Milliseconds 500
                    $p.StandardInput.WriteLine($Command); $p.StandardInput.Flush(); $sent = $true; $last = $sw.ElapsedMilliseconds
                }
                $task = $p.StandardOutput.ReadLineAsync()
            } elseif ($sent -and ($sw.ElapsedMilliseconds - $last) -gt $IdleMs -and ($seenEcho -or ($sw.ElapsedMilliseconds - $last) -gt 3 * $IdleMs)) { break }
            elseif (-not $sent -and $sw.ElapsedMilliseconds -gt 15000) {
                $p.StandardInput.WriteLine($Command); $p.StandardInput.Flush(); $sent = $true; $last = $sw.ElapsedMilliseconds
            }
        }
        try { $p.StandardInput.WriteLine('exit'); $p.StandardInput.Close() } catch { }
        if (-not $p.WaitForExit(5000)) { try { $p.Kill() } catch { } }
    } finally { if (-not $p.HasExited) { try { $p.Kill() } catch { } } }
    return (Get-CleanText $sb.ToString())
}

# --- web and LED image ---------------------------------------------------------------

function Get-WebJson([string]$Path) {
    for ($i = 0; $i -lt 5; $i++) {
        $r = Invoke-WebRequest -Uri ($WebUrl.TrimEnd('/') + $Path) -UseBasicParsing -TimeoutSec 10
        $j = ConvertFrom-Json -InputObject $r.Content
        if (-not ($j.PSObject.Properties['pending'] -and -not $j.PSObject.Properties['v'])) { return $j }
        Start-Sleep -Milliseconds 400   # the page answers {"pending":1} until its snapshot is ready
    }
    throw "$Path stays pending"
}

function Resolve-JsonPath($Object, [string]$Path) {
    $cur = $Object
    foreach ($part in ($Path -split '\.')) {
        if ($part -match '^([^\[]*)((\[\d+\])*)$') {
            $name = $Matches[1]; $idx = $Matches[2]
            if ($name) {
                if ($null -eq $cur -or -not $cur.PSObject.Properties[$name]) { return $null }
                $cur = $cur.$name
            }
            foreach ($m in [regex]::Matches($idx, '\[(\d+)\]')) {
                $i = [int]$m.Groups[1].Value
                $arr = @($cur)
                if ($i -ge $arr.Count) { return $null }
                $cur = $arr[$i]
            }
        } else { return $null }
    }
    return $cur
}

<#
.SYNOPSIS
    Captures LED images from /neo/pixels; returns objects { Frame; Bpl; Leds; Data (byte[]) }.
#>
function Get-PixelFrames([int]$Count, [int]$IntervalMs) {
    $frames = New-Object System.Collections.ArrayList
    $tmp = [IO.Path]::GetTempFileName()
    try {
        $tries = 0
        while ($frames.Count -lt $Count -and $tries -lt $Count * 4) {
            $tries++
            Invoke-WebRequest -Uri ($WebUrl.TrimEnd('/') + '/neo/pixels') -UseBasicParsing -TimeoutSec 10 -OutFile $tmp
            $b = [IO.File]::ReadAllBytes($tmp)
            if ($b.Length -gt 8 -and $b[0] -eq 78 -and $b[1] -eq 80) {
                $null = $frames.Add([pscustomobject]@{ Frame = [int]$b[6] + 256 * [int]$b[7]; Bpl = [int]$b[3]; Leds = [int]$b[4] + 256 * [int]$b[5]; Data = $b })
            }
            Start-Sleep -Milliseconds $IntervalMs
        }
    } finally { Remove-Item -LiteralPath $tmp -ErrorAction SilentlyContinue }
    return , $frames
}

function Get-PixelLevel($Fr, [int]$Led) {
    $o = 8 + $Led * $Fr.Bpl; $s = 0
    for ($c = 0; $c -lt $Fr.Bpl; $c++) { $s += [int]$Fr.Data[$o + $c] }
    return $s
}

function Get-PixelKey($Fr, [int]$Led) {
    $o = 8 + $Led * $Fr.Bpl; $k = ''
    for ($c = 0; $c -lt $Fr.Bpl; $c++) { $k += ('{0:X2}' -f $Fr.Data[$o + $c]) }
    return $k
}

function Get-PixelHue($Fr, [int]$Led) {
    $o = 8 + $Led * $Fr.Bpl
    $r = [double]$Fr.Data[$o]; $g = [double]$Fr.Data[$o + 1]; $b = [double]$Fr.Data[$o + 2]
    $mx = [Math]::Max($r, [Math]::Max($g, $b)); $mn = [Math]::Min($r, [Math]::Min($g, $b))
    if ($mx -le 0 -or ($mx - $mn) -lt 8) { return -1 }
    if ($mx -eq $r) { $h = 60 * ((($g - $b) / ($mx - $mn)) % 6) }
    elseif ($mx -eq $g) { $h = 60 * ((($b - $r) / ($mx - $mn)) + 2) }
    else { $h = 60 * ((($r - $g) / ($mx - $mn)) + 4) }
    if ($h -lt 0) { $h += 360 }
    return [int]$h
}

<#
.SYNOPSIS
    Evaluates one effect property on captured frames; returns @{ Ok; Detail }.
#>
function Test-EffectProperty($Frames, [int]$A, [int]$B, [string]$Property, $Check) {
    $n = $Frames.Count
    if ($n -lt 2) { return @{ Ok = $false; Detail = "only $n frame(s) captured" } }
    $leds = $B - $A + 1
    $frameIds = (@($Frames | ForEach-Object { $_.Frame }) -join ',')
    $distinctFrames = @($Frames | ForEach-Object { $f = $_; (($A..$B) | ForEach-Object { Get-PixelKey $f $_ }) -join '' } | Select-Object -Unique).Count
    $sums = @($Frames | ForEach-Object { $f = $_; $s = 0; foreach ($i in $A..$B) { $s += Get-PixelLevel $f $i }; $s })
    $litCounts = @($Frames | ForEach-Object { $f = $_; @(($A..$B) | Where-Object { (Get-PixelLevel $f $_) -gt 24 }).Count })
    $uniq = @($Frames | ForEach-Object { $f = $_; @(($A..$B) | ForEach-Object { Get-PixelKey $f $_ } | Select-Object -Unique).Count })
    $minSum = ($sums | Measure-Object -Minimum).Minimum; $maxSum = ($sums | Measure-Object -Maximum).Maximum
    $info = "frames=$n ids=$frameIds distinct=$distinctFrames sum=$minSum..$maxSum lit=$(($litCounts) -join '/') colours=$(($uniq) -join '/')"
    switch ($Property) {
        'constant' { return @{ Ok = ($distinctFrames -eq 1 -and $maxSum -gt 0); Detail = $info } }
        'uniform' { return @{ Ok = (@($uniq | Where-Object { $_ -ne 1 }).Count -eq 0 -and $maxSum -gt 0); Detail = $info } }
        'animated' { return @{ Ok = ($distinctFrames -ge [Math]::Min(3, $n) -and $maxSum -gt 0); Detail = $info } }
        'dark' { return @{ Ok = ($maxSum -eq 0); Detail = $info } }
        'lit' { return @{ Ok = ($minSum -gt 0); Detail = $info } }
        'brightnessVaries' {
            $ratio = 0; if ($maxSum -gt 0) { $ratio = [double]$minSum / $maxSum }
            return @{ Ok = ($maxSum -gt 0 -and $ratio -lt 0.7 -and @($uniq | Where-Object { $_ -gt 2 }).Count -le 1); Detail = "$info min/max=$([Math]::Round($ratio,2))" }
        }
        'hueSpread' {
            $f = $Frames[$n - 1]
            $hues = @(($A..$B) | ForEach-Object { Get-PixelHue $f $_ } | Where-Object { $_ -ge 0 } | ForEach-Object { [int]($_ / 30) } | Select-Object -Unique)
            $min = 6; if ($Check.PSObject.Properties['minHueBins']) { $min = [int]$Check.minHueBins }
            $needAnim = $true; if ($Check.PSObject.Properties['animated']) { $needAnim = [bool]$Check.animated }
            return @{ Ok = ($hues.Count -ge $min -and (-not $needAnim -or $distinctFrames -ge 2)); Detail = "$info hueBins=$($hues.Count)" }
        }
        'sparse' {
            $maxFrac = 0.5; if ($Check.PSObject.Properties['maxLitFraction']) { $maxFrac = [double]$Check.maxLitFraction }
            $avg = ($litCounts | Measure-Object -Average).Average
            $needAnim = $true; if ($Check.PSObject.Properties['animated']) { $needAnim = [bool]$Check.animated }
            return @{ Ok = ($avg -gt 0 -and $avg -le $maxFrac * $leds -and (-not $needAnim -or $distinctFrames -ge 2)); Detail = "$info avgLit=$([Math]::Round($avg,1))/$leds" }
        }
        'movingPeak' {
            $pos = @($Frames | ForEach-Object { $f = $_; $best = -1; $bi = -1; foreach ($i in $A..$B) { $l = Get-PixelLevel $f $i; if ($l -gt $best) { $best = $l; $bi = $i } }; $bi })
            $moves = @($pos | Select-Object -Unique).Count
            return @{ Ok = ($moves -ge [Math]::Min(3, $n)); Detail = "$info peak=$(($pos) -join ',')" }
        }
        'strobe' {
            $darkFrames = @($sums | Where-Object { $_ -lt $maxSum * 0.2 }).Count
            return @{ Ok = ($maxSum -gt 0 -and $darkFrames -ge 1 -and $darkFrames -lt $n); Detail = "$info darkFrames=$darkFrames" }
        }
        default { return @{ Ok = $false; Detail = "unknown property $Property" } }
    }
}

# --- checks ---------------------------------------------------------------------

function Get-Field($Object, [string]$Name, $Default = $null) {
    if ($null -ne $Object -and $Object.PSObject.Properties[$Name] -and $null -ne $Object.$Name) { return $Object.$Name }
    return $Default
}

function Test-Value($Have, $Want, [string]$Op, $Tol) {
    if ($null -eq $Have) { return $false }
    if (-not $Op) { $Op = 'eq' }
    if ($Op -eq 'match') { return ([string]$Have -match [string]$Want) }
    $hn = 0.0; $wn = 0.0
    $hs = ([string]$Have) -replace '[^0-9.,+-].*$', '' -replace ',', '.'
    $isNum = [double]::TryParse($hs, [Globalization.NumberStyles]::Float, $Invariant, [ref]$hn) -and
             [double]::TryParse(([string]$Want), [Globalization.NumberStyles]::Float, $Invariant, [ref]$wn)
    if ($isNum) {
        $t = 0.0; if ($null -ne $Tol) { $t = [double]$Tol }
        switch ($Op) {
            'eq' { return [Math]::Abs($hn - $wn) -le $t } 'ne' { return [Math]::Abs($hn - $wn) -gt $t }
            'ge' { return $hn -ge $wn - $t } 'le' { return $hn -le $wn + $t }
            'gt' { return $hn -gt $wn } 'lt' { return $hn -lt $wn }
        }
    }
    $eq = ([string]$Have).Trim() -ieq ([string]$Want).Trim()
    if ($Op -eq 'ne') { return -not $eq }
    return $eq
}

function Invoke-Check($Check) {
    $type = [string](Get-Field $Check 'type' '')
    switch ($type) {
        'ga' {
            $have = Read-Group ([string]$Check.ga) ([string](Get-Field $Check 'dpt' ''))
            $ok = Test-Value $have $Check.value ([string](Get-Field $Check 'op' 'eq')) (Get-Field $Check 'tol')
            return @{ Ok = $ok; Detail = ("{0} = {1} (want {2})" -f $Check.ga, $(if ($null -eq $have) { 'no answer' } else { $have }), $Check.value) }
        }
        'console' {
            if ($Console -eq 'none') { return @{ Ok = $null; Detail = 'console backend none' } }
            $cmd = [string]$Check.cmd
            if ($script:ConsoleCache.ContainsKey($cmd)) { $out = $script:ConsoleCache[$cmd] }
            else { $out = Invoke-ConsoleCommand $cmd; $script:ConsoleCache[$cmd] = $out }   # one call per command and step
            $found = [regex]::Match($out, [string]$Check.pattern, 'Multiline')
            $absent = [bool](Get-Field $Check 'absent' $false)
            $ok = $found.Success; if ($absent) { $ok = -not $found.Success }
            $detail = 'pattern not found'
            if ($found.Success) { $detail = 'found: ' + $found.Value.Trim() }
            $grp = Get-Field $Check 'group'
            if ($found.Success -and $null -ne $grp) {
                # captured value: compare (value/op, "$name" refers to a stored value) and/or store it
                $have = $found.Groups[[int]$grp].Value
                if ($Check.PSObject.Properties['changeMs']) {
                    # same command again after changeMs: the captured value must change (or stay, change=false)
                    Start-Sleep -Milliseconds ([int]$Check.changeMs)
                    $again = [regex]::Match((Invoke-ConsoleCommand $cmd), [string]$Check.pattern, 'Multiline')
                    $second = ''; if ($again.Success) { $second = $again.Groups[[int]$grp].Value }
                    $wantChange = [bool](Get-Field $Check 'change' $true)
                    $ok = $again.Success -and (($have -ne $second) -eq $wantChange)
                    $detail = ("{0} -> {1} (want {2})" -f $have, $second, $(if ($wantChange) { 'change' } else { 'no change' }))
                }
                if ($Check.PSObject.Properties['value']) {
                    $want = [string]$Check.value
                    if ($want -like '$*') { $want = [string]$script:Store[$want.Substring(1)] }
                    $ok = $ok -and (Test-Value $have $want ([string](Get-Field $Check 'op' 'eq')) (Get-Field $Check 'tol'))
                    $detail = ("{0} = {1} (want {2} {3})" -f $Check.pattern, $have, (Get-Field $Check 'op' 'eq'), $want)
                }
                if ($Check.PSObject.Properties['store']) { $script:Store[[string]$Check.store] = $have }
            }
            if (-not $ok -and $env:NEO_TEST_CONSOLE_LOG) { Add-Content -LiteralPath $env:NEO_TEST_CONSOLE_LOG -Value ("### " + $cmd + "`n" + $out) }   # debug: output of failed console checks
            return @{ Ok = $ok; Detail = ("{0} /{1}/ {2}" -f $Check.cmd, $Check.pattern, $detail) }
        }
        'web' {
            if (-not $WebUrl) { return @{ Ok = $null; Detail = 'no -WebUrl' } }
            $j = Get-WebJson ([string](Get-Field $Check 'url' '/neo/state'))
            $have = Resolve-JsonPath $j ([string]$Check.path)
            $ok = Test-Value $have $Check.value ([string](Get-Field $Check 'op' 'eq')) (Get-Field $Check 'tol')
            return @{ Ok = $ok; Detail = ("{0} = {1} (want {2} {3})" -f $Check.path, $(if ($null -eq $have) { 'missing' } else { $have }), (Get-Field $Check 'op' 'eq'), $Check.value) }
        }
        'effect' {
            if (-not $WebUrl) { return @{ Ok = $null; Detail = 'no -WebUrl' } }
            $frames = Get-PixelFrames ([int](Get-Field $Check 'frames' 8)) ([int](Get-Field $Check 'intervalMs' 250))
            $range = @($Check.range)
            return (Test-EffectProperty $frames ([int]$range[0]) ([int]$range[1]) ([string]$Check.property) $Check)
        }
        default { return @{ Ok = $false; Detail = "unknown check type '$type'" } }
    }
}

# --- main -----------------------------------------------------------------------------

if ($Only) { $Only = @($Only | ForEach-Object { $_ -split ',' } | Where-Object { $_ }) }
$stepsPath = (Resolve-Path -LiteralPath $Steps).Path
$spec = ConvertFrom-Json -InputObject ([IO.File]::ReadAllText($stepsPath, $Utf8NoBom))
if (-not $Device) { $Device = [string]$spec.device }
$defaults = Get-Field $spec 'defaults'
$defTimeout = [int](Get-Field $defaults 'timeoutMs' 6000)
$defWait = [int](Get-Field $defaults 'waitMs' 800)
if (-not $WebUrl -and (Get-Field $spec 'webUrl')) { $WebUrl = [string]$spec.webUrl }

Write-Host ("NeoPixel ETS test '{0}' on {1} (bus {2}, console {3})" -f (Get-Field $spec 'name' ''), $Device, $Bus, $Console)

if ($Import -or $Program) {
    $cfg = Join-Path (Split-Path -Parent $stepsPath) ([string]$spec.config)
    $args2 = @('-NoProfile', '-File', $DeviceConfigScript, '-Import', $cfg, '-Pa', $Device, '-ClientName', $ClientName, '-MaxPasses', '15')
    $exe = 'pwsh'; if ($PSVersionTable.PSEdition -ne 'Core') { $exe = 'powershell' }
    $stageA = [string](Get-Field $spec 'configStageA' '')
    if ($stageA) {
        # stage A: the sample without LOG/FCB/LMG parameters; writes stay fast while no module channel is active
        Write-Host "Importing stage A $stageA ..."
        & $exe @('-NoProfile', '-File', $DeviceConfigScript, '-Import', (Join-Path (Split-Path -Parent $stepsPath) $stageA), '-Pa', $Device,
            '-ClientName', $ClientName, '-MaxPasses', '15', '-ContinueOnError')
    }
    Write-Host "Importing $cfg ..."
    # pass A: a write can fail when an earlier write of the same pass hid that parameter; keep going
    & $exe @($args2 + '-ContinueOnError')
    # pass B: must match the file without errors; programs the device on -Program
    if ($Program) { $args2 += '-Program' }
    & $exe @args2
    if ($LASTEXITCODE -ne 0) { Write-Host "Import/download failed (exit $LASTEXITCODE)" -ForegroundColor Red; exit 2 }
    $boot = [int](Get-Field $defaults 'bootWaitMs' 15000)
    if ($Program) { Start-Sleep -Milliseconds $boot }
}

$pass = 0; $fail = 0; $skip = 0
foreach ($st in @($spec.steps)) {
    if ($Only -and ($Only -notcontains [string]$st.id)) { continue }
    Write-Host ("[{0}] {1}" -f $st.id, $st.title)
    $script:ConsoleCache = @{}
    foreach ($a in @(Get-Field $st 'do' @())) {
        if ($a.PSObject.Properties['write']) { Write-Group ([string]$a.write) ([string]$a.value) ([string](Get-Field $a 'dpt' '')); Start-Sleep -Milliseconds $defWait }
        elseif ($a.PSObject.Properties['wait']) { Start-Sleep -Milliseconds ([int]$a.wait) }
        elseif ($a.PSObject.Properties['console']) { if ($Console -ne 'none') { $null = Invoke-ConsoleCommand ([string]$a.console) } }
    }
    foreach ($c in @(Get-Field $st 'expect' @())) {
        $timeout = [int](Get-Field $c 'timeoutMs' $defTimeout)
        $sw = [Diagnostics.Stopwatch]::StartNew()
        do {
            try { $r = Invoke-Check $c } catch { $r = @{ Ok = $false; Detail = 'error: ' + $_.Exception.Message } }
            if ($r.Ok -ne $false) { break }
            if ([string](Get-Field $c 'type' '') -in @('console', 'effect')) { break }   # expensive: no polling
            Start-Sleep -Milliseconds 700
        } while ($sw.ElapsedMilliseconds -lt $timeout)
        $state = 'PASS'; $color = 'Green'
        if ($null -eq $r.Ok) { $state = 'SKIP'; $color = 'Yellow'; $skip++ }
        elseif ($r.Ok) { $pass++ } else { $state = 'FAIL'; $color = 'Red'; $fail++ }
        Write-Host ("  {0} {1,-7} {2}" -f $state, (Get-Field $c 'type' ''), $r.Detail) -ForegroundColor $color
        $null = $script:Results.Add([ordered]@{ step = $st.id; title = $st.title; source = (Get-Field $st 'source' ''); check = $c; state = $state; detail = $r.Detail })
    }
}
Write-Host ("Result: {0} passed, {1} failed, {2} skipped" -f $pass, $fail, $skip) -ForegroundColor $(if ($fail -gt 0) { 'Red' } else { 'Green' })
if ($Report) { [IO.File]::WriteAllText($Report, (ConvertTo-Json -InputObject $script:Results -Depth 8), $Utf8NoBom) }
if ($fail -gt 0) { exit 1 }
exit 0
