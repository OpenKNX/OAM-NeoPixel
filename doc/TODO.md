# ToDo — OAM-NeoPixel

**Die eine Liste.** Alles Offene steht hier; wo ein Punkt eine lange Herleitung hat, verweist er auf das
Dokument, das sie trägt. Erledigtes wird gelöscht, nicht abgehakt — was hier steht, steht noch aus.

Stand: **2026-11-17** (Dev 2.12 / Release 1.0) · **29 offene Punkte**. Nummeriert nach Priorität, Prio 1
zuerst; kommt ein Punkt dazu oder fällt einer weg, wird neu nummeriert.

Regeln: ETS-/Generator-Änderungen gehen in `scripts/Build-*.ps1` bzw. `HardwareManifest.xml` und `src/hw/` (Hardware-Generator in
OGM-HardwareConfig), nie in die generierten Dateien; Speicherlayout nur
per Anhängen ändern; jede Inhaltsänderung an der App braucht einen Versions-Bump.

## Übersicht, sortiert

Prio 1 = vor dem Public Release, 2 = vor dem Public Release wenn klein, 3 = nach dem Public Release. Aufwand: S = Stunden, M = 1-2 Tage, L = mehr.

| Nr | Thema | Prio | Risiko | Aufwand | Stand |
|---|---|---|---|---|---|
| 1 | Dev 2.5 in der echten ETS testen (Aktualisieren, Verfügbarkeitsmeldung, Hardwarewechsel, Release 0.x → 1.0) | 1 | hoch | M |  |
| 2 | knxOTA auf den 4-MB-ESP32-Zielen | 1 | hoch | M | Release-Entscheidung (USB-Flash) |
| 3 | ESP32: zu wenige RMT-Sendekanäle für die Ports — C3/C5/C6 2, S3 4, ESP32 8 (nicht gemessen) | 1 | hoch | M-L | Treiber in OFM-NeoPixel |
| 4 | Strommessung: Offset-Überlauf ab ~13 107 LEDs, dazu Restpunkte | 1 | mittel | S (Überlauf) / M | OFM-Lib, besprechen |
| 5 | GPIO-Konfliktprüfung (836 Eingänge) in der echten ETS testen | 2 | mittel | S |  |
| 6 | 2D-Effekt auf 1D-Segment bleibt dunkel → Fallback Solid | 2 | mittel | S | entschieden |
| 7 | GPIO-Regressionswächter im Generator | 2 | mittel | M |  |
| 8 | TP + WiFi: Gruppenmonitor, IP-LED, Setup-AP, Hardware-Test | 2 | mittel | M | Monitor bis zur Lib-Absicherung aus |
| 9 | Welche Effektgruppen das Produkt ausliefert | 2 | niedrig | S | entscheiden |
| 10 | `dependencies.txt` und Lib-Branches vor dem Release | 2 | mittel | S |  |
| 11 | Konfigurator ausbauen | 2 | niedrig | L | Katalog zuerst |
| 12 | /neo: Steuerung ohne Schutz | 3 | mittel | S | wartet auf Anmeldung in OFM-Network |
| 13 | Timing-Dropdown auf das Chip-Profil reduzieren | 3 | mittel | S |  |
| 14 | Effektkette: Konzeptlücken | 3 | mittel | L |  |
| 15 | Hardware-Gating der ETS-Knoten | 3 | niedrig | S | GA-Frage offen |
| 16 | Dimmzeit als ETS-Parameter | 3 | niedrig | S | Versions-Bump |
| 17 | Lücken aus den Beispielen und dem Anwendungsfall-Katalog | 3 | niedrig | M |  |
| 18 | Kanal-Tabellen nach dem Muster „Kanalauswahl" | 3 | niedrig | L | Konzept zuerst |
| 19 | Effektmanager/Cue: Hue-Konvention, Phasen-Drift der Kette | 3 | niedrig | S |  |
| 20 | Überlappende Segmente dokumentieren | 3 | niedrig | S |  |
| 21 | Effektzustand je Segment: Restpunkte | 3 | niedrig | S |  |
| 22 | OFM-relevante Teile nach OFM-NeoPixel umziehen | 3 | schlafend | L |  |
| 23 | Dual-Core: Rendering auf Core1 | 3 | schlafend | L |  |
| 24 | Hardware-Seite: ETS5-Screenshot | 3 | niedrig | S |  |
| 25 | LPD6803 / P9813 vervollständigen (ESP32, Farbreihenfolge, Latch) | 3 | niedrig | M |  |
| 26 | Tests und Skripte im Repo, die nicht PowerShell sind | 3 | niedrig | M |  |
| 27 | Konsole: Effekt-Name eines nicht gebauten Effekts meldet „not found" | 3 | niedrig | S | OFM-NeoPixel |
| 28 | Effekt-Generator: ID-Prüfung nur für literale IDs, Cue-Parameter über 10 Slots still weggelassen | 3 | niedrig | S |  |
| 29 | Device-Registry: Board-Header als einzige Quelle oder JSON von Hand | 3 | niedrig | M | entscheiden, mit Dom abstimmen |

---

## Prio 1 — vor dem Public Release

### 1 — Dev 2.5 in der echten ETS testen

Bisher nur im OpenKNX-Viewer und mit dem Producer geprüft. In ETS 6 nachstellen:

- **Aktualisieren von Dev 2.2 auf 2.5:** die Effekt-Parameter (IDs `21CC…`/`22CC…` seit Dev 2.3, `faccd02`) stehen
  danach einmalig auf dem Standardwert; Effektauswahl, Szenen und alle übrigen Parameter behalten ihre Werte. Prüfen,
  dass genau das passiert und kein alter Wert in einem anderen Parameter landet.
- **Verfügbarkeitsmeldung:** „Effekt nicht verfügbar" (Fehler) bzw. „Hinweis" am Segment, an jeder Szene und an jeder
  Cue: eine Hardware wählen, deren Firmware den Effekt weglässt (z. B. ein 2D-Effekt auf einer Hardware mit
  `NEOPIXEL_DISABLE_2D`), dann wieder eine mit dem Effekt. Rechenzeit der Berechnung beobachten.
- **Hardwarewechsel:** Meldungen, Port-Werte und Konfliktprüfung nach dem Wechsel der Hardware; werden die
  Port-Werte beim „Aktualisieren" übernommen (Frage aus dem Port-Konzept)?
- **Release 0.x → 1.0:** Release nimmt Updates ab 0.1 an (`ReplacesVersionsDownTo="0.1"`, `src/NeoPixel-Release.xml`).
  Seit `faccd02` haben alle Effekt-Parameter neue IDs, die Werte gehen beim „Aktualisieren" also unabhängig vom
  alten Schema auf den Standardwert. Nachstellen, dass das bei einem alten Release-Gerät (0.8 gibt es in drei
  ID-Schemata) so ist und die übrigen Werte stimmen.

### 2 — knxOTA auf den 4-MB-ESP32-Zielen

Die 4-MB-Partitionstabelle `esp32_4mb.csv` (OGM-Common `data/partitions/`) hat nur `app0` (0x280000). Ohne zweite
App-Partition liefert `esp_ota_get_next_update_partition()` nichts, `FileTransferModule::otaSlotAvailable()` gibt
`false`, und `CheckFeatures` bietet weder Update noch gzip noch Delta an. Betroffen sind 18 der 44 Release-Envs:
Gledopto ×3, QuinLED Uno/Quad V3 WiFi, Ethernet, AE+ (6), Dig2Go, Octa V1 WiFi/Ethernet, OpenKNXiao C3/C6 Mini,
KNeoPiX C3/C6 V1/V1.5.

Weg: zwei App-Slots plus ein kleines Dateisystem, Update nur über Delta (auf ESP32 schreibt der Delta-Pfad direkt in
den OTA-Slot, `FileTransferModule.cpp` `Update.begin()`/`Update.write()`; der Patch braucht nur Zehner-KB im
Dateisystem). Budget: 4 194 304 B − 36 864 B (Bootloader, Tabelle) − 212 992 B (otadata, phy_init, knx, openknx,
nvs) = 3 944 448 B für beide Slots und das Dateisystem; Slots auf 0x10000 ausgerichtet → 1 900 544 B je Slot bei
128 KB Dateisystem, 1 835 008 B bei 256 KB.

Abbildgrößen vom 2026-11-15 gegen 1 900 544 B:

| Ziel | Flash belegt | passt |
|---|---|---|
| WiFi (309WL, 017WL, QuinLED V3 WiFi, Dig2Go, Octa V1 WiFi) | ~1 968 700 – 1 969 085 B | nein, ~68 KB zu groß |
| Ethernet und 620WL | ~1 529 700 – 1 530 105 B | ja |
| C3 | ~1 258 000 B | ja |
| C6 | ~1 221 000 B | ja |

Schritte: neue Tabelle mit `app0` + `app1` (Ort klären: OGM-Common ist Shared-Lib, oder eine eigene Tabelle im
OAM); die WiFi-Ziele um ~68 KB verkleinern, erst messen (Größe je Bibliothek/Objektdatei aus `.map`), keine Funktion
ohne Zustimmung streichen; danach `${ftc_delta.build_flags}` an den umgestellten Envs ergänzen
(`platformio.custom.ini`, Block vor `[custom_releases]`). Abnahme: alle Release-Envs bauen, jedes umgestellte Ziel
mit Reserve in Prozent unter der Slotgröße, `otaSlotAvailable()` = `true`, `CheckFeatures` meldet Bit `0x80`
(Delta), ein Delta-Update auf echter Hardware.

Für die Release-Entscheidung: Umpartitionieren löscht das Dateisystem, bestehende Geräte brauchen einmal USB-Flash;
danach sprengt jedes Wachstum über die Slotgröße den Bau; Voll-Abbild-Updates bleiben auf 4 MB unmöglich.

### 3 — ESP32: zu wenige RMT-Sendekanäle für die Ports

OFM-NeoPixel belegt je seriellem Streifen einen RMT-Sendekanal (`rmt/rmt_neopixel_serial.cpp`, `rmt_new_tx_channel`)
und begrenzt die Streifenzahl nirgends auf die Kanäle des Chips. Ein Streifen, der keinen Kanal mehr bekommt,
**bleibt dunkel**, nur ein Log-Fehler zeigt es. Kanäle laut ESP-IDF `soc_caps.h` (`SOC_RMT_TX_CANDIDATES_PER_GROUP`)
gegen die Ports im Board-Header (`HW_PORT_COUNT`):

| Chip | RMT-TX | betroffene Hardware (Ports) |
|---|---|---|
| ESP32-C3 / -C5 / -C6 | 2 | KNeoPiX V1/V1.5 C3/C5/C6 (8), OpenKNXiao C3/C5/C6 Mini (8) |
| ESP32-S3 | 4 | KNeoPiX V1/V1.5 S3 (8), OpenKNXiao S3 Mini (8) |
| ESP32 | 8 | QuinLED Dig-Octa 32-8L WiFi/Ethernet (9) |

Gledopto und die übrigen QuinLED-Boards (2–8 Ports) passen.

Auf Hardware nicht geprüft. Optionen: Parallel-Ausgabe (I2S, auf dem S3 LCD_CAM, Konzept in
`doc/Konzept-ESP32S3-LCDCAM.md`), oder die ETS-Auswahl je Chip auf die Kanalzahl begrenzen und einen Hinweis zeigen.
Erst messen, dann entscheiden.

Der RMT-Treiber liegt in `OFM-NeoPixel` (Shared-Lib).

### 4 — Strommessung / Strombegrenzung: Restpunkte aus den Reviews

**Zuerst dran — Offset-Überlauf:** `PowerManager::calculateTotalCurrent` rechnet `uint16_t offset = i * bytesPerPixel`
(OFM `PowerManager.h` ~584, ebenso ~606 und ~740). Ab ~13 107 LEDs bei 5 Byte/Pixel läuft der Offset über; die ETS
erlaubt 16 383 LEDs. Shared-Lib, also vorher besprechen.

Danach:
- **SPI-Streifen (APA102/SK9822/LPD8806/…)** zählen im Budget mit, werden aber nicht gedimmt (`applyScaleToPhysicalBuffer`
  überspringt sie, `NeoPixelManager.cpp` ~790 und ~1188). Dazu misst `PowerManager.h` (~575-595) den Puffer ab Byte 0
  mit dem Pixel-Abstand, der SPI-Treiber stellt aber Start-Frames davor (`pio_neopixel_spi.cpp` ~204) → die Messung ist
  verschoben. Lösung: 5-Bit-Global-Brightness, oder SPI aus dem Budget nehmen; Messung ab dem ersten Pixel.
- **TM1814:** die Skalierung beginnt bei Offset 0 (`NeoPixelManager.cpp` ~1195-1219) und trifft damit das C1/C2-Präfix
  (Stromeinstellung des Chips); die letzten 2 LEDs werden nicht gemessen.
- **16-Bit-Protokolle (SM16825, UCS8904):** Messung liest 8-Bit-Kanäle, die Skalierung endet nach 5 Byte je Pixel →
  falsch gemessen und nur teilweise gedimmt.
- **Anstiegszeit ABL am Streifen** wird gespeichert (`setAblSlewRate`), aber nirgends angewendet; die ETS zeigt den Parameter.
- **Ein ungedimmtes Bild beim Ausschalten:** `showVirtualStripIfIdle()` → `VirtualStrip::show()` ohne `applyPowerLimit()`.
- **„Strom pro LED" wirkt versteckt:** bei global „Fester Wert"/„Deaktiviert" wird damit gemessen (Standard 60 mA),
  die ETS zeigt ihn nur bei „Pro LED".
- **Fallback-Grenze der Streifen-Objekte:** `getMaxCurrentMa()` für jeden Modus, der Cache-Pfad modusabhängig.
- **Status-Burst:** eine Runde kann alle 8 Burst-Credits verbrauchen → ein folgender Gruppenbefehl kommt bis ~0,8 s
  später; nach „Alle LEDs" EIN kann einmal ein veralteter Wert raus.
- **Konsole:** „Global Load" rechnet bei „Deaktiviert" gegen den PowerManager-Standard 5000 mA; die Info-Ausgabe in
  `NeoPixelModule.cpp` zeigt `NEOcurrentPerChannel` als „Max Current" statt `NEOpowerLimitGlobal`.
- **FW1906 (6 Kanäle):** `profileForProtocol` teilt durch 6, `calculateTotalCurrent` liest 5 Kanäle → 5/6 gezählt.
- **Farbordnung vs. Protokoll:** die ETS setzt für TM1914/FW1906 GRBW, die Firmware misst 3 bzw. 6 Kanäle; „SK6812"
  zählt immer 4. `VirtualStrip` entscheidet den W-Pfad nach der Farbordnung — passt bei diesen Typen nicht zusammen.
- **Standardwert „Strom pro LED" = 60** misst RGBW mit 4 × 15 mA statt 4 × 20 mA; Standard 0 prüfen.

---

## Prio 2 — vor dem Public Release, wenn klein

### 5 — GPIO-Konfliktprüfung in der echten ETS testen

Die Prüfung ist eine erzeugte ParameterCalculation (`PC-%TT%00900`, `NEOHW_conflictCheck`, `HardwareManifest.xml <Conflicts>`)
mit 836 Eingängen (797 Port-Parameter je Board und Signal, 39 weitere) und 20 Flags ohne Gerätespeicher. Getestet nur im
OpenKNX-Viewer und mit Node.js (ein Lauf rund 18 µs), nicht unter Windows-JScript. In ETS 6 nachstellen: Gerät einfügen,
„Aktualisieren" auf 2.5 (ETS rechnet je übernommenem Wert neu), Konflikt erzeugen und auflösen (Streifen-
und Relais-Seite), Download, Zeit je Änderung und für das Aktualisieren messen.

### 6 — 2D-Effekt auf einem 1D-Segment bleibt dunkel

Auf einem 1D-Segment ruft der Dispatch in `Segment.cpp` `update()` auf, und das ist bei den 2D-Effekten leer
(z.B. `Aurora2DEffect.h`) → das Segment bleibt dunkel, ohne Hinweis. **Entschieden:** Fallback auf „Solid"
(Primärfarbe) in der Firmware, nicht in der ETS — beim Cue ist die Ziel-Topologie zur Config-Zeit unbekannt.
Ort: der Dispatch in `Segment.cpp` (`update3D`/`update2D`/`update`) oder pro 2D-Effekt. Der umgekehrte Fall
(1D-Effekt auf 2D-Matrix) funktioniert.

### 7 — GPIO-Regressionswächter im Generator

Der gespeicherte Port-Wert ist die **Hardware-Portnummer − 1** (`HW_PORT_<n>` des Board-Headers,
OGM-HardwareConfig `generator/templates/PortListTypes.xml`); das Label steckt nur in `Text=`. Label umbenennen ist
unkritisch, **die `HW_PORT_<n>` im Header umnummerieren oder mittendrin einfügen verschiebt die Bedeutung** bei gleicher
HW-ID → bestehende Installationen zeigen still auf einen anderen Pin. Regel bis dahin: neue Ports ans Ende.

- **Wächter A (umsetzen):** vor dem Überschreiben die bestehende `src/NeoPixel.hw.types.gen.xml` einlesen, pro
  DEVICE_HW_ID Wert→Label rekonstruieren und mit der neuen Liste vergleichen; Abbruch, wenn sich die Bedeutung eines
  Werts ändert.
- **Wächter B (optional):** mehrdeutige Indizes (LED- und Relais-Ausgang) ausgeben; betrifft nur den Hardware-Wechsel.

Reine Generator-Logik, kein Versions-Bump.

### 8 — TP + WiFi (`..._ESP32S3_V1_TPWIFI`, `..._ESP32S3_V15_TPWIFI`)

- **Hardware-Test:** die TPWIFI-Envs sind nur gebaut, auf keinem Gerät gelaufen.
- **Web-Gruppenmonitor:** der Busmonitor-Schalter (OFM-Network `Webserver/GroupMonitor.cpp`) setzt den TP-UART ohne
  Zugriffsschutz in den Monitor-Modus → in der Lib absichern. Bis dahin ist `OPENKNX_WEBMONITOR` im TPWIFI-Env per
  `build_unflags` aus.
- **KNX-IP-Status-LED (Funktion 11):** auf TP + WiFi immer orange/rot (`knxIpEnabled()` = false) → ausblenden oder umdeuten.
- **Setup-AP:** schließt standardmäßig nie von selbst; das Öffnen per Prog-Taste auf einem TP-Gerät bewerten
  (OFM-Network `WifiSetup.cpp`).
- **Weitere Boards:** TP + WiFi gibt es nur für KNeoPiX V1/V1.5 (ESP32-S3); C3/C5/C6 und XIAO-ESP32S3-Mini erst mit Testhardware.

### 9 — Welche Effektgruppen das Produkt ausliefert

`src/NeoPixel.EffectGroups.xml` ist leer; die Envs lassen heute nur über `NEOPIXEL_DISABLE_2D` (No2D) und einzelne
`NEOPIXEL_DISABLE_<X>` Effekte weg. Entscheiden, welche Gruppen es gibt (z. B. für die knappen 4-MB-Ziele aus Punkt 2)
und welches Env welche Gruppe abschaltet. Die ETS meldet die Folgen über die Verfügbarkeitsprüfung
(`doc/Effect-Flags.md`).

### 10 — `dependencies.txt` und Lib-Branches vor dem Release

Die committete `dependencies.txt` nennt ältere Stände als die Libs im Arbeitsbaum (z. B. TPUart `5edaf0e` gegen
`1c8ffc0`, knx `c19d925` gegen `a09d18d`, OGM-HardwareConfig `5704eae` gegen `eae7f09`); der Build schreibt die Datei
neu. Die Lib-Branches liegen nur lokal oder vor ihrem Upstream (Stand 2026-11-15: knx `ec/v1dev-ec` +12, TPUart
`ec/main` +6, OFM-Network `ec/v1dev-ec` +190/−3, OFM-FileTransferModule `ec/v1dev` +222, OFM-NeoPixel `v1dev` +3,
OFM-LightManager `v1dev` +1, OGM-Common `ec/feat-flash-size` und OGM-HardwareConfig `ec/feat-hw-profile` ohne Upstream).
Vor dem Release: Lib-Branches pushen, `dependencies.txt` aus dem Release-Build committen.

### 11 — Konfigurator ausbauen

Der Konfigurator (`doc/examples/index.html`) erzeugt die Konfigurationstransfer-Strings der Beispiele. Ausbau in dieser
Reihenfolge:

1. **Katalog aus dem Build:** Parameternamen, Enums, Standardwerte und die Effektliste inkl. 2D und Gruppen als JSON
   aus dem Build statt von Hand (heute z. B. der 2D-Bereich 20..32 fest in `index.html:1201`; `web/assets/effects.json`
   gibt es schon). Voraussetzung für alles Weitere.
2. **Hardwareauswahl:** nur die Ports der gewählten Hardware und nur die Effekte, die ihre Firmware hat
   (Port-Tabellen, `NEOHW_table_EffectsOff`).
3. **Eine Vorlage je Anwendungsfall A–H** aus `doc/Anwendungsfaelle-KNX.md`; das Dokument verlinkt darauf.
4. **Prüfungen vor dem Export:** LED-Anzahl gegen Strom und Speicher, GPIO-Konflikte, Effekte, die auf der Hardware
   fehlen. Die ETS-Prüfungen (Konfliktprüfung, Effekt-Hinweise) simulieren die Beispiele heute nicht.
5. **Export der Gruppenadressen** als ETS-Importdatei, druckbares Tastenlayout.
6. **ETS-Export zurücklesen**, um ihn zu bearbeiten.
7. **Automatische Prüfung:** die erzeugten Strings im Viewer (uct) importieren, damit kein Beispiel veraltet.

`doc/examples/README-Beispielkonfigurationen.md` und die fünf Beispiele (garage, kuechenzeile, matrix-panel, terrasse,
wintergarten) sind gegen Dev 2.4/2.5 nicht neu geprüft. `**/*.html` steht in `.gitignore`: eine neue HTML-Datei braucht
`git add -f`.

---

## Prio 3 — nach dem Public Release

### 12 — /neo: Steuerung ohne Schutz

Die Steuerung auf `/neo` schreibt intern ins KO (kein Bus-Telegramm) und ist offen, weil OFM-Network keine Anmeldung
hat. Sobald es eine gibt, die Steuerung daran koppeln. Später: Editor für Effektmanager/Cues, bis dahin nur Anzeige.

### 13 — Timing-Dropdown auf das Chip-Profil reduzieren

Der 16-stufige Bitraten-Regler stammt aus der Zeit ohne Chip-Profile; WLED und ESPHome lösen es über den Chiptyp.
Ein Override skaliert das Profil und schiebt T1H aus dem Chipfenster (SK6812 bei ETS 750: 853 ns gegen 750 ns
Maximum). **Vorschlag:** in der ETS nur „Chip-Profil", die übrigen Stufen über `neo phys timing <id> <kHz>`.
Der Text für Wert 0 lautet noch „800 kHz (Standard)" (`src/NeoPixel.share.xml`), die
Firmware nutzt bei 0 aber das Chip-Profil — Text anpassen. **Nicht ändern: die Latch-Zeit** — sie bindet erst ab ~484 FPS.

### 14 — Effektkette: Konzeptlücken (Terrasse)

- **EM ohne gemeinsame Fläche:** der EM-Start pausiert das virtuelle Band (OFM `EffektManager.cpp`) → jedes Gerät
  rendert sein eigenes Panel (Terrasse 3 × 60×8 statt 180×8). Der Master sendet im EM seinen Vor-EM-Zustand
  (`composeSyncPayload`); die Cues der Kette laufen nicht synchron.
- **Lokale 2D-Effekte je Slave als Kopie:** Uhr, Lauftext, Snake, GameOfLife, Fire 2D, Aurora, DNA, Lissajous,
  Metaballs, Tetris zeichnen lokal (Zielbild: OFM `doc/EM-Cue-Effektkette.md`).
- **Slave-Neustart allein:** bekommt den EM-Start des Masters erst beim nächsten Schaltpunkt.
- **Mehrere Ketten-Segmente auf einem Gerät:** keine gemeinsame Simulation; nach getrenntem Neustart ist eine Naht möglich.

### 15 — Hardware-Gating: Knoten ausblenden, bis eine Hardware gewählt ist

Solange die Hardware-Auswahl des Moduls „Gerät" (`NEOHW_HardwareSelect`, `%NEOHW_SelectRef%`) = 0 („Nicht gewählt") ist,
zeigen Relais, Physikalisch inkl. Strombegrenzung, Virtuell und Effektmanager heute je einen Hinweis „Keine Hardware ausgewählt".
Besser: die Knoten in `<choose %NEOHW_SelectRef%><when test="!=0">` wrappen, Hinweis auf die Seite OpenKNX → „Gerät", die
Einzelmeldungen entfernen. Kein Speicherlayout-Change. **Offen:** was passiert mit verknüpften GAs an KOs
ausgeblendeter Knoten?

### 16 — Dimmzeit als ETS-Parameter

Beim relativen Dimmen (DPT 3.007) bestimmt der Dimmer die Dimmzeit. Heute fest: `DIM_RAMP_FULL_MS = 3000`
(`src/SegmentController.cpp`, mit TODO-Kommentar). Parameter „Dimmzeit 0→100 %" pro Segment (1–10 s, Default 3 s)
→ Layout-Änderung, Versions-Bump, Voll-Download. Optional: zyklische Sender (wiederholte kleine Steps) diskret
ehren statt fester Rate.

### 17 — Lücken aus den Beispielen und dem Anwendungsfall-Katalog

Aus dem Durchspielen der Beispiele:

- **„Letzter Zustand" ohne Save-Pin:** kein NeoPixel-Board hat `SAVE_INTERRUPT_PIN` → nach einem Stromausfall kommt
  der zuletzt *gespeicherte* Zustand. Vorschlag: entprellter, begrenzter Save nach Änderungen.
- **Relais beim Neustart:** die 3-min-Sperre von `flash.save()` (OGM-Common) behält kurz nach einem Speichern den
  älteren Zustand; das Relais bleibt bis zum Ende der Startverzögerung AUS; ein Master-Reset speichert über den
  Unload-Hook und ist aus der Applikation nicht von einem ETS-Download zu unterscheiden. Nicht auf Hardware geprüft:
  ob die statische Farbe erneut gesendet wird, sobald das Relais das Netzteil einschaltet.
- **Relais nicht an den LED-Zustand gekoppelt** (nur über das eigene Objekt oder eine Logik).
- **Sperre:** kein Status-Objekt, das Verhalten beim Entsperren ist undefiniert.
- **Dimmen ohne Minimum:** relatives Dimmen geht bis 0.
- **HSV-Objekt:** ETS DPT 232.600 (RGB), die Firmware dekodiert H/S/V; die H/S/V-Einzelobjekte rechnen nach einem
  RGB-Befehl mit veralteten Werten.
- **„Effekt schalten"-Objekt** ist in der ETS ausgeblendet, sendet aber sofort einen Sync.
- **ConfigTransfer TypeColor:** ob die ETS Farben als Zahl (ARGB int32) annimmt, mit einem echten ETS-Export prüfen.
- **Logik auf demselben Gerät (absolute KO-Nummer):** nur im Code verfolgt, nicht auf Hardware getestet.

Aus `doc/Anwendungsfaelle-KNX.md` (offene Lücken; Entscheidung je Punkt offen):

- **Show nur per Wert:** ein Schalt-Taster (1 Bit) kann keinen EM starten; Vorschlag: KO „letzter EM ein/aus".
- **Kein Effekt-Layering je Segment:** Vorschlag: bewusst so lassen und über mehrere Segmente lösen (siehe Punkt 20).
- **Kein eigenes Weiß-Bedienobjekt** (SK6812): Weiß ein/aus/dimmen nur über W/WRel und Helligkeit.
- **Effekt-Parameter nicht live per KO** (Geschwindigkeit, Farbe des laufenden Effekts).
- **Kein Wert → Balken/VU** (KNX-Wert als Füllstand).
- **Kein Pixel-/Matrix-Inhalt über KNX.**
- **Kein voller Zustand-Restore für Benachrichtigungen:** nach einem einmaligen EM geht das Segment auf den direkten
  Zustand zurück; ein vorher laufender EM läuft nicht weiter (`stopEffektManager` → `EM_STOP_LAST`).
- **Keine Zwangsführung (DPT 2)** je Segment.

### 18 — Kanal-Tabellen nach dem Muster „Kanalauswahl" (Logikmodul)

Tabellen wie Logiken → Kanalauswahl: **alle Kanäle sichtbar**, je Zeile Aktivierung (Deaktiviert / Aktiv …) plus
Beschreibung, statt „Anzahl …"-Zähler. Erst ein eigenes Konzept (welche Tabellen: Segmente, LED-Streifen,
Effektmanager, Relais, Szenen; Auswirkung auf Parameter-/KO-Layout, Migration der „Anzahl"-Parameter,
Versionssprung), dann umsetzen. Bezug: EM Aktiv/Inaktiv-Dreizustand, Rename-Muster für inaktive Knoten; Entwurf für die
Effektmanager-Tabellen in `doc/EM-Tabellen-UI-Konzept.md`.

### 19 — Effektmanager / Cue: Kleinkram

- **Hue-Konvention:** ein Hue-Byte kann nicht „Segmentfarbe" und Hue 0 (= Rot) zugleich kodieren. Clock2D: 0 =
  Segmentfarbe, >0 = fixer Hue (Rot nur über 255). Eine Konvention für alle Hue-Effekte festlegen.
- **Effektkette, Phasen-Drift:** Slaves starten den Effekt beim Sync neu und laufen dann frei (sichtbar bei
  Blink/Strobe/Theater Chase). Der Heartbeat (3 s) wiederholt nur den Zustand; ein Tick-/Phasen-Zähler im
  Sync-Telegramm (5 Byte im 14-Byte-KO frei) fehlt. Erst auf Hardware bewerten.

### 20 — Überlappende Segmente: Verhalten dokumentieren

„Last writer wins" (das höher nummerierte Segment gewinnt, kein Blend) ist so gewollt, aber nirgends beschrieben.
Optional ein kurzer ETS-Hinweis bei den Segmenten.

### 21 — Effektzustand je Segment: Restpunkte

- Die Fire-Instanz trägt immer den 4-KB-2D-Puffer, auch auf 1D.
- Heap-Churn bei EM-Cues Fire ↔ Rainbow (4,6 KB alloc/free je Wechsel).
- `freeMemory()` ist der gesamte freie Heap, nicht der größte Block → Fragmentierung kann trotz Reserve scheitern.

### 22 — OFM-relevante Teile nach OFM-NeoPixel umziehen

Alle OFM-/Engine-relevanten Teile für den Producer nach `OFM-NeoPixel` verlagern (OAM bleibt OAM, OFM bleibt OFM).
Die Engine-Doku liegt schon in `OFM-NeoPixel/doc/EM-Cue-Effektkette.md`. Stand und offene Entscheidungen:

- `NeoPixel::processInputKo` der OFM ist leer (`lib/OFM-NeoPixel/src/NeoPixel.cpp` ~291); die KOs verarbeitet das OAM.
- Zwei `OpenKNX::Module`-Klassen: OFM `NeoPixel` (`NeoPixel.h` ~48) und OAM `NeoPixelBusModule` → auf eine einigen.
- Pin-Auflösung als Schnittstelle (die Ports kommen heute aus dem erzeugten HardwareProfile, `hwPortGpio`).
- NEOSTRIP-Kanal aufteilen oder in die OFM legen.
- Wohin die Generatoren gehören (`Build-EffectParameters.ps1` liest die Effekte der OFM).
- `op:verify` gegen `lib/OFM-NeoPixel/library.json`.
- Help_de aufteilen (OFM-Effekthilfe / OAM-Produkthilfe).
- Die Offset-Anforderungen der OFM-Teile dokumentieren.

### 23 — Dual-Core: Rendering auf Core1

Alles läuft auf Core0 (KNX und alle Modul-`loop()` inkl. Rendering), Core1 ist ungenutzt. Gemessen: 862 LEDs +
Tetris-2D-KI + Cylon ≈ 28 ms/Frame, **56 % CPU bei 20 Hz** auf einem Kern. Knackpunkt sind die geteilten Daten
(Segment-Configs, `_hclTransformContext`, KO-getriebene Änderungen, `applyCue`): heute harmlos, weil alles
sequenziell läuft; auf zwei Kernen braucht es Double-Buffering oder Locks, dazu Flash-Save (Core-Lockout) und
DMA-Channel-Ownership getrennt. Erst angehen, wenn größere Matrizen oder höhere FPS gefragt sind.

### 24 — Hardware-Seite: ETS5-Screenshot

Die Seite OpenKNX → Gerät (Gerätekarte aus ETS-Elementen, Generator in OGM-HardwareConfig) ist umgesetzt. Offen:
ETS5-Screenshot der fertigen knxprod ansehen (Raster 71/29 mit 150-px-QR-Bild, bisher nur 75/25 mit 130 px gemessen).

### 25 — LPD6803 / P9813 vervollständigen (ESP32, Farbreihenfolge, Latch)

Beide laufen seit Dev 2.2 als SPI-Typen (MOSI + Clock); auf RP2040/RP2350 treibt sie `pio_neopixel_spi.cpp`. Offen, alles
ohne Hardware-Test bisher:

- ESP32: `hal/HW_NeoPixel_SPI.cpp` (`rgbToBuffer`, Start-/End-Frames) kennt beide nicht, der Streifen bleibt dunkel
  (OFM-NeoPixel).
- P9813 Farbreihenfolge: das Skript setzt `CO_BGR` (`NeoPixel.script.src.js` `map[24]`), der PIO-Treiber schreibt nach
  `orderChannels` die Bytes in umgekehrter Folge; laut Frame-Definition (`IHardwareDriver.h`: Flag + B + G + R) passt nur
  `RGB`. Auch die Chip-Vorgabe `getColorOrder(P9813)` (GRB) stimmt dann nicht. Entscheiden: Skript-Vorgabe oder Packing in der Lib.
- LPD6803 End-Frame: `pio_neopixel_spi.cpp` sendet ein 32-Bit-Null-Wort, FastLED `nLeds/32 + 1` Worte; ab ca. 32-64 LEDs
  latchen die hinteren LEDs evtl. nicht. Datenblatt / Oszi.
- Konsole der Lib (`NeoPixelConsole.cpp`): eigene SPI-Liste ohne APA102-Clone, LPD6803, P9813; Namen fehlen.

### 26 — Tests und Skripte im Repo, die nicht PowerShell sind

Repo-Skripte sollen PowerShell sein (5.1-kompatibel). Heute nicht: `test/*.py` (z. B. `test/test_build_release_clean.py`),
die `.cjs`/`.js`-Tests, `doc/svg-src/gen_*.py` und `regen.sh`. Je Datei entscheiden: nach PowerShell portieren oder als
interne Werkzeuge kennzeichnen.

### 27 — Konsole: Effekt-Name eines nicht gebauten Effekts meldet „not found"

`neo effect set <Segment> <Nummer>` nimmt für eine nicht gebaute Nummer „Solid" und meldet das; die Suche über den Namen
überspringt nicht gebaute Effekte (`getEffectByIndex` = `nullptr`) und meldet „not found"
(OFM `NeoPixelConsole.cpp` ~4056-4079). Gleiches Verhalten wie die Nummer oder eine eigene Meldung „nicht gebaut".

### 28 — Effekt-Generator: ID-Prüfung nur für literale IDs, Cue-Parameter über 10 Slots still weggelassen

- `Assert-ChannelIdNumbers` (`scripts/Build-EffectParameters.ps1`) prüft nur literale dreistellige IDs im Segment- und
  EM-Template; IDs aus Makros (`%CUE_PPP+k%`, `%PPP+k%`, …) und die Part-/Share-Dateien prüft es nicht.
- `Get-CueTypedParamPlan` lässt Zahl-/Enum-/Bool-Parameter ab Index 10 (mehr als die 10 Cue-Slots) ohne Meldung weg
  (`continue`); ein Effekt mit mehr als 10 solchen Parametern hat sie in der Cue nicht. Meldung oder Abbruch ergänzen.

### 29 — Device-Registry: Board-Header als einzige Quelle oder JSON von Hand

Die Device-Registry (OGM-HardwareConfig, Branch `origin/v1-registry`, `devices/*.json` mit `schema.json`) beschreibt die
OpenKNX-Geräte (REG1, UP1, PiPico BCU Connector) ein zweites Mal neben den Board-Headern. Entscheiden, ob die Board-Header
die einzige Quelle werden und das JSON daraus erzeugt wird (Tendenz des Eigentümers) oder ob das JSON von Hand gepflegt
bleibt. Falls erzeugt: ein eigenes Define für die lange Katalogbeschreibung (höchstens 400 Zeichen / 4 Zeilen) neben dem
kurzen Kartentext `HW_INFO_TEXT` (höchstens 255 Zeichen). Mit Dom abstimmen.
