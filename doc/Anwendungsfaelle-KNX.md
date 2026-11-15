# NeoPixel — Anwendungsfälle im KNX-Umfeld

Adressierbare LED-Streifen (WS2812B, SK6812 RGBW, WWCW, APA102 …) über KNX. Dieses Dokument listet die
Anwendungsfälle, zeigt je Fall die Wege (welches KO, welcher Mechanismus) und das Beispiel in `doc/examples/`, das
ihn zeigt. Stand: Dev 2.4.

Schritt-für-Schritt-Anleitungen mit den ETS-Namen der KOs: `doc/Benutzerhandbuch.md`. Parameter und Funktionen der
Applikation: `doc/Applikationsbeschreibung.md`. Offene Lücken: `doc/TODO.md`, Punkt „Lücken aus den Beispielen und dem
Anwendungsfall-Katalog".

---

## 0 — Zwei Steuer-Ebenen pro Segment

- **Direkt-Ebene** (manuell/statisch): Schalten, Helligkeit, Farbe (RGB/HSV/RGBW/W/WW/CW/Farbtemperatur), Effekt, Szene.
- **Effektmanager-Ebene** (Sequenz): ein Effektmanager (EM) spielt eine Cue-Liste ab (Effekt, Farbe, Dauer,
  Überblendung, Text); Start, Stop, Pause/Resume, Cue setzen, Cue +/-.

Es läuft entweder ein EM oder die Direkt-Ebene. Ein direktes Farb-, Effekt- oder Szenen-KO unterbricht einen
laufenden EM (eine Szene vom Typ „Effektmanager N starten" startet stattdessen diesen EM). Helligkeit und Dimmen
dimmen die Show, Aus pausiert sie, Ein nimmt sie wieder auf; Cue +/- und Sperre unterbrechen sie nicht. Wohin das
Segment nach dem Stop eines EM zurückkehrt, legt der Segment-Parameter „Effektmanager-Stop: Rückkehr" fest: Letzter
Zustand (Standard), Segment-Standardwerte oder Aus.

---

## 1 — Steuer-Bausteine (KOs pro Segment, ETS-Namen)

Jedes Segment hat 53 KOs (K0–K52).

| Gruppe | KOs (DPT) | Funktion |
|---|---|---|
| Schalten | Segment Ein/Aus (1.1) · Segment Ein/Aus Status (1.11) | Ein/Aus + Status |
| Helligkeit | Segment Helligkeit (5.1) · Dimmen (3.7) · Segment Helligkeit Status (5.1) | absolut / relativ dimmen |
| Farbe | RGB (232.600) · RGBW (251.600) · HSV (232.600) · Rot/Grün/Blau (5.10) · Farbton (5.3) · Sättigung, Helligkeit (Value) (5.1) · je Kanal „… dimmen" (3.7) · RGB/HSV dimmen (232.600) · RGBW dimmen (251.600) · RGB/HSV/RGBW Status | Farbe absolut/relativ + Status |
| Weiß / Tunable White | Weiß, Warmweiß, Kaltweiß (5.10) · Farbtemperatur (7.600) · Weiß/Warmweiß/Kaltweiß dimmen (3.7) · Farbtemperatur Status | Weiß-Kanal / warm-kalt |
| Effekt | Effekt (5.10 = Effektnummer) · Effekt schalten (3.7) · Effekt Status | Effekt wählen / durchschalten / Status |
| Effektmanager | Effektmanager Start (5.10, 1–16, 0 = Stop) · Effektmanager Stop (1.1) · Effektmanager Pause/Resume (1.1) · Cue setzen (5.10) · Cue +/- (3.7) · Effektmanager Status · Aktive Cue · Effektmanager Zustand (0 gestoppt, 1 läuft, 2 pausiert) | Show steuern + Status |
| Text / Laufschrift | Effekt-Text (16.1) · Effekt-Text Anhängen (16.1) · Effekt-Text Status | Text setzen/anhängen + Status |
| Szene | Szenensteuerung (18.1, Abruf und Lern-Bit) · Szenensteuerung Status | Look abrufen/speichern oder einen EM starten (Szenen-Effekt „Effektmanager N starten", Werte 200–215) |
| Sperre | Sperre (1.1) | 1 sperrt alle steuernden KOs des Segments |
| Multi-Device | Effektkette Sync (16.1, intern) | Effekt über mehrere Geräte synchronisieren |
| HCL | HCL Zustand (7.600) + Parameter Lichtmanager des Segments | Human Centric Lighting über OFM-LightManager |

Geräteweit: Alle LEDs (1.1) und Helligkeit (5.1) mit Status, Stromverbrauch/Auslastung/Leistung Gesamt, HCL Global
Zustand, Relais 1–4 (1.1) mit Status.

**Effekte und Hardware:** Effektnummern 0–32 (0 = Solid; 2D-Effekte: Laufschrift 19 und 20–32, in der Auswahl „2D ·“). Die Firmware einer Hardware kann
Effekte weglassen (z. B. alle 2D-Effekte auf Hardware mit `NEOPIXEL_DISABLE_2D`). Die ETS meldet am Segment, an jeder
Szene und an jeder Cue „Effekt nicht verfügbar", wenn die Firmware der gewählten Hardware den Effekt nicht enthält, und
einen Hinweis, wenn nur ein Teil der Firmware-Varianten ihn weglässt. Kommt über KO, Szene oder Cue eine Nummer an, die
die Firmware nicht hat, läuft „Solid"; „Effekt schalten" überspringt diese Nummern. Details: `doc/Effect-Flags.md`.

---

## 2 — Wege (KNX-Mechanismen, mit denen man die KOs treibt)

| Weg | Wofür | Beispiel |
|---|---|---|
| Direkte GA-Verknüpfung | Sensor-/Taster-KO direkt auf NeoPixel-KO | Bewegungsmelder → Segment Ein/Aus |
| Schalt-Taster (1.1) | Ein/Aus | kurz = umschalten |
| Dimm-Taster (3.7) | relativ heller/dunkler, Effekt oder Cue weiter/zurück | lang = dimmen |
| Wert-Taster (5.10) | bestimmter Effekt / EM / Cue | Wert 5 → EM 5, Wert 3 → Effekt 3 |
| Szenen-Taster (18.1) | Look abrufen/speichern | Szene 1 „Kochen" |
| Logikmodul (OFM-LogicModule, im Produkt) | Bedingungen, Verzögerung, Treppenlicht, Zeitschaltuhr, Trigger → Aktion → Rückkehr | Klingel → Show, dann zurück |
| Funktionsblöcke (OFM-FunctionBlocks, im Produkt) | Blinker, CountDown (Treppenhaus/Timer), Aggregator (ODER/UND) … | Treppenlicht, Benachrichtigung blinken |
| Lichtmanager (OFM-LightManager, im Produkt) | HCL, Tag/Nacht, Umgebungslicht, Sperre der HCL-Steuerung | tagesabhängig warm-/kaltweiß |
| Zeit/Schaltuhr | tages-/zeitabhängig | abends warmweiß |
| Visualisierung | freie Werte | RGB-Farbrad, Texteingabe |

---

## 3 — Anwendungsfälle

### A · Statisches Licht

Beispiele: `kuechenzeile` (Start-Farbe, Dimmen, Szenen), `wintergarten` (RGBW, Weiß über W, HCL).

| # | Use-Case | Weg(e) | Status-KO |
|---|---|---|---|
| A1 | Weiß ein/aus + dimmen (SK6812 W-Kanal) | Segment Ein/Aus + Dimmen; Weiß über Weiß / RGBW / Start-Farbe | Segment Ein/Aus Status, Segment Helligkeit Status |
| A2 | Tunable White (warm/kalt, WWCW) | Farbtemperatur (7.600) oder Warmweiß/Kaltweiß einzeln + „… dimmen" | Farbtemperatur Status |
| A3 | Farblicht (fester/freier Farbton) | RGB/RGBW/HSV; fester Farbton über Szene oder Wert-Taster; frei über Visu | RGB/HSV/RGBW Status |
| A4 | Einzelkanal fein (nur R, nur W …) | Rot/Grün/Blau/Weiß, Farbton/Sättigung/Helligkeit (Value) + „… dimmen" | Status des Farbmodells |

### B · Effekte (einzeln)

Beispiel: `matrix-panel` (2D-Effekt Plasma Nebula auf 16×16).

| # | Use-Case | Weg(e) | Status-KO |
|---|---|---|---|
| B1 | Einen Effekt zeigen (Fire, Rainbow, Plasma …) | Effekt = Effektnummer (Wert-Taster); nicht gebaute Nummer → Solid | Effekt Status |
| B2 | Effekt durchschalten (nächster/vorheriger) | Effekt schalten (3.7), überspringt nicht gebaute Effekte | Effekt Status |
| B3 | Effekt-Parameter ändern (Geschwindigkeit, Farbe …) | nur ETS (Segment, Szene, Cue); kein KO für den laufenden Effekt | — |

### C · Effekt-Shows / Sequenzen (Effektmanager)

Beispiele: `garage` (EM „Einfahrt", Start und Cue setzen aus der Logik), `terrasse` (EM mit Loop auf drei Geräten).

| # | Use-Case | Weg(e) | Status-KO |
|---|---|---|---|
| C1 | Show starten/stoppen | Effektmanager Start (1–16; 0 = Stop) bzw. Effektmanager Stop | Effektmanager Status |
| C2 | Show pausieren/fortsetzen | Effektmanager Pause/Resume (1 = Pause, 0 = weiter) | Effektmanager Zustand |
| C3 | In der Show zu einer Cue springen | Cue setzen (Cue-Nr.) oder Cue +/- (3.7) | Aktive Cue |
| C4 | Show automatisch bei Gerätestart / beim Einschalten | Parameter „Effektmanager nach Geräteneustart" / „Effektmanager beim Einschalten" | Effektmanager Status |
| C5 | Show-Verkettung / Endlos | ETS: Loop, nächster Effektmanager | Effektmanager Status |

### D · Text / Laufschrift (Matrix)

Kein Beispiel in `doc/examples/`.

| # | Use-Case | Weg(e) | Status-KO |
|---|---|---|---|
| D1 | Text anzeigen / scrollen | Effekt-Text (16.1) + Effekt „Laufschrift" (19) | Effekt-Text Status |
| D2 | Text dynamisch anhängen | Effekt-Text Anhängen | Effekt-Text Status |
| D3 | Uhr anzeigen | Effekt „Uhr 2D" (20) auf einer Matrix | — |

### E · Szenen

Beispiel: `kuechenzeile` (Szenen „Kochen" und „Abend", Speichern über das Lern-Bit).

| # | Use-Case | Weg(e) | Status-KO |
|---|---|---|---|
| E1 | Look abrufen (Film/Party/Normal) | Szenensteuerung (18.1); unterbricht einen EM und zeigt den Look, schaltet das Segment ein | Szenensteuerung Status |
| E2 | Aktuellen Look speichern | Szenensteuerung mit Lern-Bit | Szenensteuerung Status |
| E3 | Szene startet eine Show | Szenen-Effekt „Effektmanager N starten" (Werte 200–215) | Szenensteuerung Status |

### F · Trigger → Aktion → Rückkehr

Beispiel: `garage` (Anwesenheit → EM Start, nach Ablauf Cue setzen).

| # | Use-Case | Weg(e) | Rückkehr |
|---|---|---|---|
| F1 | Ereignis → Animation (Klingel: 3× blinken) | Logik → Effektmanager Start eines Benachrichtigungs-EM (Loop aus) | automatisch nach der letzten Cue, Ziel laut „Effektmanager-Stop: Rückkehr" |
| F2 | Ereignis → feste Farbe (Alarm: rot) | Logik → RGB/Effekt (unterbricht EM) | Logik sendet alten Wert/Szene zurück |
| F3 | Ereignis → Text (Postfach voll → Laufschrift) | Logik → Effekt-Text | wie F1/F2 |
| F4 | Status anzeigen (Fenster offen = orange) | Sensor → Logik → RGB/Effekt | Sensor-Reset → Logik zurück |
| F5 | Vollen Zustand sichern und exakt zurück (inkl. laufendem EM) | nicht vorhanden: nach einem einmaligen EM kehrt das Segment auf die Direkt-Ebene zurück, ein vorher laufender EM läuft nicht weiter | Szene oder Logik |

### G · Automatik / Trigger-Quellen

Beispiele: `garage` (Anwesenheit, Treppenlicht 5 min über Logik), `wintergarten` (HCL über den Lichtmanager),
`terrasse` (Zeitschaltuhr Sonnenuntergang/23:00 über Logik).

| # | Use-Case | Weg(e) |
|---|---|---|
| G1 | Bewegung/Präsenz → Licht an, Timeout → aus | Melder → Segment Ein/Aus (+ Logik- oder Funktionsblock-Timer) |
| G2 | Treppenhaus/Zeit-Licht | Logik (Treppenlicht) oder Funktionsblock CountDown → Segment Ein/Aus; kein eingebauter Timer im Segment |
| G3 | Zeit-/tagesabhängig (abends warmweiß) | Uhrzeit → Logik → Farbtemperatur/Helligkeit oder EM Start |
| G4 | HCL / Human Centric Lighting | Lichtmanager → Farbtemperatur/Helligkeit über den Tag (Segment-Parameter Lichtmanager) |
| G5 | Aufwachlicht / Sonnenaufgang | EM-Cue mit langer Dauer und Überblendung |

### H · Betrieb / Integration

Beispiele: `wintergarten` (Netzteil-Relais aus dem Segmentzustand über Logik, Status für Taster-LEDs), `terrasse`
(Effektkette über drei Geräte), `kuechenzeile` (Sperre).

| # | Use-Case | Weg(e) |
|---|---|---|
| H1 | Statusrückmeldung / Visu | alle Status-KOs |
| H2 | Zentralfunktionen (Zentral-Aus/Dimmen) | geräteweit Alle LEDs / Helligkeit, oder eine GA auf Segment Ein/Aus / Segment Helligkeit mehrerer Segmente |
| H3 | Mehrere Geräte synchron (ein Effekt über mehrere Geräte) | Effektkette (Master/Slave, Effektkette Sync) |
| H4 | Netzteil/Lüfter schalten | Relais 1–4 (1.1), mit Verzögerung und Mindest-Auszeit; an den LED-Zustand nur über Logik gekoppelt |
| H5 | Verhalten nach Neustart / Busausfall | „Verhalten nach Neustart" (Global verwenden / Aus / Letzter Zustand / An (ETS-Werte)) und „Effektmanager nach Geräteneustart" |
| H6 | Bedienung sperren (Reinigung, Party) | Sperre (1.1); kein Sperr-Status-KO |
