# ETS-Gerätetest NeoPixel

`Invoke-NeoPixelEtsTest.ps1` prüft ein NeoPixel-Gerät gegen eine ETS-Konfiguration: Sample importieren
(KNX-MCP-Plugin über `Knx-DeviceConfig.ps1`), programmieren, dann Schritte aus einer JSON-Datei abarbeiten
und je Prüfung PASS/FAIL ausgeben. Exit 0 = alles bestanden, 1 = mindestens eine Prüfung fehlgeschlagen,
2 = Import oder Download fehlgeschlagen.

Hilfe: `Get-Help ./Invoke-NeoPixelEtsTest.ps1 -Detailed`. Windows PowerShell 5.1 und PowerShell 7.

## Schritte-Datei

```json
{ "name": "...", "device": "1.1.3", "config": "1-einzeln-1.1.3.json", "webUrl": "http://10.0.0.125",
  "defaults": { "timeoutMs": 6000, "waitMs": 600, "bootWaitMs": 20000 },
  "steps": [ { "id": "K01", "title": "S1 aus", "source": "KO Segment Ein/Aus -> Status",
               "do":     [ { "write": "1/3/0", "value": "0", "dpt": "1.001" }, { "wait": 1000 },
                           { "console": "neo effect set 0 2" } ],
               "expect": [ { "type": "ga", "ga": "1/3/1", "dpt": "1.011", "value": "0" },
                           { "type": "console", "cmd": "neo info", "pattern": "Total LEDs:\\s+414" },
                           { "type": "web", "path": "segs[0].pw", "value": 0 },
                           { "type": "effect", "range": [0, 63], "frames": 8, "intervalMs": 250, "property": "animated" } ] } ] }
```

- `source` nennt, woher die Erwartung kommt (ETS-Parameter, Hilfeseite, KO-Text) - nicht, was das Gerät gerade tut.
- `ga`: GroupValue_Read auf die Status-GA; Zahlen mit `tol`, Vergleich mit `op` (eq, ne, ge, le, gt, lt, match).
- `console`: Regex auf die Ausgabe des Konsolenbefehls (`absent: true` = darf nicht vorkommen).
- `web`: Wert unter einem Pfad von `/neo/state` (oder `url`), nur Geräte mit Webseite (ESP32).
- `effect`: LED-Bild aus `/neo/pixels` (Header `NP`, Bytes je LED, Anzahl, Bildzähler), Eigenschaft
  `constant`, `uniform`, `animated`, `dark`, `lit`, `brightnessVaries`, `hueSpread`, `sparse`, `movingPeak`, `strobe`.

## Wege zum Gerät

| Zweck | Schalter | Weg |
|---|---|---|
| Gruppen lesen/schreiben | `-Bus ets` (Standard) | KNX-MCP-Plugin `ga_write_value` / `ga_read_value` über die ETS-Busverbindung |
| | `-Bus oknx -OknxIp 10.0.0.60` | `oknx ga write` / `oknx ga read` |
| Konsole | `-Console oknx-remote` (Standard) | `oknx -i <OknxIp> <PA> con` - Konsole über KNX, kein USB nötig |
| | `-Console oknx-web -ConsoleIp 10.0.0.125` | Webkonsole des Geräts (ESP32) |
| | `-Console serial -SerialPort /dev/cu.usbmodem844301` | USB, 115200 Baud |
| Webseite / LED-Bild | `-WebUrl http://10.0.0.125` | `/neo/state`, `/neo/pixels` |

Der Client erscheint in der ETS-Undo-Liste unter `-ClientName` (Standard `neopixel-ets`).

## Beispiele

```powershell
# Sample importieren, programmieren, prüfen
./Invoke-NeoPixelEtsTest.ps1 -Steps <samples>/1-einzeln/steps-1.1.3.json -Import -Program

# ohne ETS-Schreibzugriff: alle 33 Effekte auf 1.1.5 (Konsole + LED-Bild)
./Invoke-NeoPixelEtsTest.ps1 -Steps effects/effects-1.1.5.json -Console oknx-web -ConsoleIp 10.0.0.125 -Report effects.json
```

Die Samples (Import-Dateien, Schritte, README, GA-Liste) liegen unter
`OpenKNX-Bench-Backups/ets-config/samples/`.
