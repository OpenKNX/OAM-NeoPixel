# Applikationsbeschreibung OAM-NeoPixel

## Wichtige Hinweise

* Diese KNX-Applikation wird nicht von der KNX Association offiziell bereitgestellt oder unterstützt.
* Parametrierung, Inbetriebnahme und Firmware-Update erfolgen in eigener Verantwortung.
* Firmware und ETS-Applikation müssen zueinander passen. Nach einem Update sollten daher immer die eingesetzte Firmware und die importierte KNXprod gemeinsam geprüft werden.

## Kurzbeschreibung

OAM-NeoPixel ist ein OpenKNX-Modul zur Ansteuerung adressierbarer LED-Streifen über KNX. Die Applikation ermöglicht die Konfiguration von bis zu 8 physischen LED-Streifen, die intern zu einem virtuellen Gesamtstreifen zusammengefasst werden. Auf dieser Basis können bis zu 16 Segmente mit eigenen Farben, Helligkeiten, Effekten und Szenen betrieben werden.

Je nach eingesetzter Hardware stehen zusätzlich Netzwerkfunktionen, OTA-Update, Relaisausgänge und unterschiedliche GPIO- oder SPI-Belegungen zur Verfügung. Die Auswahl der Hardware erfolgt in ETS und muss zur geflashten Firmware passen.

## Funktionsumfang

* Steuerung von bis zu 8 physischen LED-Streifen
* Bildung eines virtuellen LED-Streifens zur freien Anordnung der physischen Streifen
* Bis zu 16 unabhängige Segmente
* 33 Effekte (Nummern 0–32) für statische und dynamische Lichtbilder, davon 14 reine **2D-Matrix-Effekte** (Laufschrift 19 und 20–32, z. B. Uhr, Matrix, Tetris, Plasma Nebula, Starfield Warp; in der Auswahl „2D ·“) und 3, die auf Streifen und Matrix laufen (Cylon, Fire, Noise; „1D/2D ·“); welche davon eine Firmware enthält, hängt von der Hardware ab (siehe „Effekte je Hardware“)
* Konfiguration von Segmenten als 2D-Matrix (Breite × Höhe, Topologie) für matrixfähige Effekte
* **Effektmanager**: Cue-basierte Lichtsequenzen mit Überblendung, Loop und Verkettung mehrerer Effektmanager
* **Effektkette**: ein Effekt nahtlos über mehrere KNX-Geräte (ein Master, mehrere Slaves)
* Szenenspeicher mit bis zu 10 Szenen je Segment
* Farbsteuerung über RGB, HSV, RGBW und RGBCCT
* Farbtemperatursteuerung für geeignete LED-Typen
* Human Centric Lighting (HCL) global und segmentbezogen
* Leistungsbegrenzung und Rückmeldung von Strom, Last und Leistung
* Hardwareabhängige Relaisausgänge
* Wiederherstellung von Zuständen nach Neustart, soweit parametriert

## Geeignete Hardware

Die Applikation wird für mehrere Hardwarefamilien bereitgestellt.

**Von OpenKNX unterstützte Hardware** (hier wird entwickelt und getestet):

* OpenKNXiao KNeoPiX V1/V1.5 und OpenKNXiao Mini auf RP2040- und RP2350-Basis — Referenzplattform, hier läuft es am rundesten
* OpenKNXiao KNeoPiX V1/V1.5 und OpenKNXiao Mini mit ESP32-S3, -C3, -C5 und -C6
* OpenKNX UP1 UART Gateway

**Weitere Hardware ohne Support** (Drittanbieter):

* Gledopto GL-C-309WL, GL-C-017WL und GL-C-620WL auf ESP32-Basis
* QuinLED Dig2Go, Dig-Uno V3/V4, Dig-Quad V3/V4, Dig-Octa 32-8L (V1, V2, V4) und Dig-Next 2

Diese Drittanbieter-Hardware wird zwar mitgebaut, ist aber **weder getestet noch wird sie aktiv durch OpenKNX unterstützt**. Nutzung auf eigenes Risiko: wenn es läuft, gut — wenn nicht, gibt es dafür keinen Support. Für ein zuverlässiges Ergebnis empfehlen wir OpenKNX-Hardware.

Die konkret verfügbaren Funktionen hängen von der ausgewählten Hardware ab. Insbesondere Netzwerkkonfiguration, OTA-Update, Relaisfunktionen und die Anzahl nutzbarer Ports können hardwarebedingt unterschiedlich sein.

## Inbetriebnahme

Nach dem Laden der physikalischen Adresse und der Applikation sollten zuerst die hardwarebezogenen Grundeinstellungen geprüft werden. Dazu gehören insbesondere:

* Auswahl der passenden Hardwarevariante in ETS
* Kontrolle der Anzahl physischer LED-Streifen
* Auswahl des LED-Typs und der Farbkanalreihenfolge
* Parametrierung der verwendeten GPIO- oder SPI-Pins
* Festlegung der Anzahl LEDs je physischem Streifen
* Prüfung der Spannungsversorgung und der Leistungsgrenzen

Die Hardware wird auf der Seite **OpenKNX → Gerät** gewählt:

* **Nicht gewählt:** Unter „Oder automatisch erkennen“ liest der Knopf „Vom Gerät lesen“ den Gerätetyp vom Gerät (vorher die physikalische Adresse programmieren). Alternativ die Hardware direkt in der Liste wählen. Hardwareabhängige Einstellungen sind bis dahin ausgeblendet.
* **Hardware gewählt:** Die Seite zeigt eine Gerätekarte: den Gerätenamen, links das Logo des Herstellers (bei OpenKNX-Geräten das OpenKNX-Logo) und bei Drittanbieter-Hardware darunter den Stempel „Drittanbieter / Hardware“, in der Mitte eine kurze Beschreibung. Hat das Gerät eine Infoseite, steht rechts ein QR-Code mit „Geräteinfo“ darunter; die Adresse steht im QR-Code (OpenKNXiao KNeoPiX: go.openknx.de/kneopix, UP1 UART Gateway: go.openknx.de/up1-gw-uart). Bei Drittanbieter-Hardware folgt ein Hinweis zu Gewährleistung und Support. Der Abfrage-Knopf ist dann ausgeblendet; zum erneuten Abfragen die Auswahl auf „Nicht gewählt“ stellen.

Wenn Firmware und ETS-Hardwareauswahl nicht zusammenpassen, wird die Hardwarekonfiguration aus Sicherheitsgründen nicht wie vorgesehen übernommen. In diesem Fall muss entweder die passende Firmware geflasht oder die ETS-Parametrierung korrigiert werden.

#### Stabile Hardware-Identität über die eindeutige Geräte-ID

Jede unterstützte Hardware besitzt eine **eindeutige Geräte-ID (DEVICE_HW_ID)**. Genau diese ID wird als Hardwareauswahl in der ETS gespeichert und von der Firmware direkt mit ihrer eigenen, einkompilierten Geräte-ID verglichen — nicht etwa ein Listenplatz oder eine laufende Nummer.

Das macht die Zuordnung **build- und update-unabhängig**:

* Es ist unerheblich, mit wie vielen Hardwarevarianten eine Firmware oder das ETS-Produkt erzeugt wurde — dieselbe physische Hardware wird immer korrekt erkannt.
* Das Hinzufügen oder Entfernen *anderer* Hardwarevarianten verschiebt eine bereits im Gerät gespeicherte Auswahl nicht; sie bleibt gültig.
* Bei einem späteren Firmware- oder ETS-Update bleiben Auswahl und Port-Einstellungen erhalten (Ausnahme: der einmalige Umstieg auf diese Seite, siehe oben und CHANGELOG).

Die automatische Hardware-Erkennung in der ETS liest dieselbe Geräte-ID direkt aus dem angeschlossenen Gerät aus und setzt die passende Auswahl, sodass keine manuelle Zuordnung nötig ist.

## Allgemeine Parameter

### In Betrieb senden alle

Das Gerät kann zyklisch einen Betriebsstatus senden. Damit lässt sich auf dem Bus erkennen, dass die Applikation aktiv ist. Das Sendeintervall wird in Sekunden festgelegt.

### Zeit bis das Gerät nach einem Neustart aktiv wird

Nach Busspannungswiederkehr, Reset oder Neustart kann eine Einschaltverzögerung vorgegeben werden. Dadurch wird verhindert, dass viele Geräte unmittelbar nach einem Neustart gleichzeitig Telegramme senden und den Bus unnötig belasten.

### Verhalten beim Neustart

Je nach Parametrierung kann das Gerät nach dem Neustart ausgeschaltet bleiben, definierte ETS-Werte übernehmen oder den zuletzt gespeicherten Zustand wiederherstellen. Welche Variante sinnvoll ist, hängt vom Anwendungsfall und von den Anforderungen an ein reproduzierbares Verhalten nach Spannungswiederkehr ab.

## Physische LED-Streifen

Die physischen LED-Streifen bilden die Hardwareebene der Applikation. Für jeden Streifen werden unter anderem folgende Eigenschaften festgelegt:

* LED-Typ beziehungsweise Protokoll
* Anzahl LEDs
* Farbkanalreihenfolge
* Datenport und gegebenenfalls Taktport
* Zeitverhalten beziehungsweise Timing-Modus
* Spannungs- und Leistungsparameter

Die Applikation unterstützt sowohl klassische RGB-Streifen als auch RGBW- und RGBCCT-Varianten. Bei RGBCCT-Streifen können Warmweiß- und Kaltweiß-Anteile getrennt angesprochen werden. Die Farbobjekte (R, G, B, RGB, H, S, V, HSV) ändern nur die Farbkanäle; der Weißanteil bleibt und wird über die Weiß-Objekte, RGBW oder die Farbtemperatur gesetzt.

## Virtueller LED-Streifen

Die physischen LED-Streifen werden intern zu einem virtuellen Gesamtstreifen zusammengeführt. Dadurch kann die logische Reihenfolge der LEDs von der realen Verkabelung abweichen. Diese Abstraktion vereinfacht die Projektierung deutlich, da Segmente und Effekte auf einer zusammenhängenden logischen LED-Fläche arbeiten.

Wird die Reihenfolge der physischen Streifen geändert, müssen die Start- und Endpositionen der Segmente entsprechend geprüft werden.

## Segmente

Auf dem virtuellen Streifen können bis zu 16 Segmente gebildet werden. Jedes Segment kann separat gesteuert werden. Typische segmentbezogene Einstellungen sind:

* Ein- und Ausschalten
* Helligkeit
* Farbe und Farbtemperatur
* Effekt und Effektparameter
* Gruppierung und Abstand innerhalb des Segments
* Szenenzuordnung
* Startverhalten nach Neustart

Durch die Segmentierung lassen sich auf einem gemeinsamen LED-Aufbau unterschiedliche Lichtzonen mit eigenständigen Funktionen realisieren.

## Effekte

Für jedes Segment kann ein statischer oder dynamischer Effekt gewählt werden. Neben Vollfarbe stehen unterschiedliche Lauf-, Farbwechsel-, Funken-, Feuer-, Regenbogen- und weitere Effektarten zur Verfügung. Die zugehörigen Parameter werden automatisch in die ETS-Applikation eingebunden und sind abhängig vom jeweils gewählten Effekt sichtbar.

Jeder Effekt hat eine feste Nummer (0 = Solid). Neue Effekte bekommen die nächste freie Nummer; die Nummer eines bestehenden Effekts ändert sich nicht, auch wenn eine Firmware ihn weglässt.

### Effekte je Hardware

Die Firmware einer Hardware kann Effekte weglassen, um Speicher zu sparen, z. B. alle 2D-Effekte auf Hardware ohne Platz dafür oder ganze Effektgruppen. Die ETS prüft den Effekt des Segments, jeder Szene und jeder Effektmanager-Cue gegen die Firmware der gewählten Hardware:

* **„Effekt nicht verfügbar“** (Fehler): keine Firmware-Variante dieser Hardware enthält den Effekt; das Gerät zeigt „Solid“.
* **Hinweis:** nur ein Teil der Firmware-Varianten lässt den Effekt weg; ob er läuft, hängt von der geflashten Firmware ab.
* Keine Meldung, solange keine Hardware gewählt ist.

Kommt eine Effektnummer, die die Firmware nicht enthält, über das Effekt-KO, eine Szene oder eine Cue an, zeigt das Segment „Solid“ (Primärfarbe). „Effekt schalten“ überspringt diese Nummern. Welche Schalter einen Effekt weglassen und wie die Gruppen gebildet werden: `doc/Effect-Flags.md`.

## 2D-Matrix-Effekte und Topologie

Ein Segment kann als zweidimensionale Matrix konfiguriert werden — über Breite × Höhe sowie die Verdrahtungs-Topologie (zeilen- oder spaltenweise, linear oder als Schlange/Serpentine). Auf einer so konfigurierten Matrix laufen die 2D-Effekte (z. B. Lauftext, Uhr, Matrix-Regen, Feuer 2D, Plasma, Starfield) in echtem 2D-Raum.

1D-Effekte funktionieren weiterhin auf einer Matrix. Ein 2D-Effekt benötigt jedoch eine Matrix — auf einem reinen 1D-Streifen (ohne Breite × Höhe > 1) kommt er nicht zur Geltung.

## Effektmanager und Cues

Der Effektmanager spielt vordefinierte Lichtsequenzen ab. Jeder Effektmanager besteht aus mehreren **Cues** — Momentaufnahmen aus Effekt, Farben, Helligkeit und effekt-spezifischen Parametern. Cues werden mit einstellbarer Dauer und Überblendzeit nacheinander abgespielt, können in einer Schleife laufen (Loop) und auf einen weiteren Effektmanager verketten.

Ein Effektmanager ist nicht fest an ein Segment gebunden: Jedes Segment hat eigene Steuer-KOs (Effektmanager Start 1–16, Stop, Pause/Resume, Cue setzen, Cue +/-, Status) und kann jeden der 16 Effektmanager abspielen. Solange ein Effektmanager läuft, übernimmt er die Ausgabe des Segments. Wohin das Segment nach dem Stop zurückkehrt, legt „Effektmanager-Stop: Rückkehr“ fest (Letzter Zustand, Segment-Standardwerte oder Aus). Die KOs und Beispiele stehen in `doc/Benutzerhandbuch.md`.

## Effektkette

Mit der Effektkette lassen sich mehrere Geräte als **ein zusammenhängendes LED-Band** steuern: ein Segment ist Master und sendet den Effektzustand, weitere Segmente auf anderen Geräten sind Slaves und stellen jeweils ihren Abschnitt des Gesamtbandes dar. Der Master sitzt am Bandanfang (Offset 0); jeder Slave kennt die Gesamtlänge und seinen eigenen Offset im Band. So läuft ein Effekt nahtlos über Gerätegrenzen hinweg. Ein Watchdog schaltet ein Slave-Segment bei ausbleibender Synchronisation ab.

## Szenen

Pro Segment können bis zu 10 Szenen gespeichert werden. Eine Szene umfasst den aktuellen Zustand eines Segments, insbesondere:

* gewählten Effekt
* Helligkeit
* Farbwerte
* effektbezogene Zusatzparameter

Damit lassen sich wiederkehrende Lichtstimmungen direkt aus ETS oder über Gruppenadressen abrufen. Je nach Anwendungsfall kann eine Szene auch neu eingelernt werden.

## HCL und Farbtemperatur

Die Applikation unterstützt Human Centric Lighting sowohl global als auch segmentbezogen. Dabei kann die Farbtemperatur zeit- oder sonnenstandsabhängig geführt werden. Auf geeigneten LED-Typen wird Weißlicht direkt über die vorhandenen Weißkanäle gebildet. Auf anderen LED-Typen erfolgt eine angenäherte Umsetzung über die Farbkanalsteuerung.

Die HCL-Funktion eignet sich besonders für Anwendungen, bei denen über den Tagesverlauf eine veränderliche Lichtstimmung gewünscht ist.

## Leistungsbegrenzung und Rückmeldungen

Um Netzteile und LED-Streifen zu schützen, kann die Leistungsaufnahme begrenzt werden. Die Applikation unterstützt globale und hardwareabhängig weiter differenzierte Begrenzungsarten. Zusätzlich können folgende Rückmeldungen auf den Bus gesendet werden:

* Gesamtstrom in mA
* Auslastung in Prozent
* Gesamtleistung in W
* Status von Relaisausgängen, soweit vorhanden

Bei aktiver Leistungsbegrenzung kann die Helligkeit dynamisch reduziert werden, um die konfigurierte Obergrenze einzuhalten.

## Diagnose über serielle Konsole

Die Firmware stellt über die serielle Konsole (OpenKNX-Konsole) umfangreiche Diagnosebefehle bereit. Diese ermöglichen eine detaillierte Analyse und Konfiguration der physischen LED-Streifen zur Laufzeit, ohne dass ein Firmware-Update oder eine neue ETS-Parametrierung erforderlich ist.

### Timing-Konfiguration (`neo phys timing`)

Für 1-Wire-Streifen (WS2812B, SK6812 und Klone) kann das Signaltiming direkt angepasst werden:

| Befehl | Beschreibung |
|--------|--------------|
| `neo phys timing <id>` | Aktuellen Timing-Modus anzeigen |
| `neo phys timing <id> info` | Realisierte Wellenform und Timing-Details anzeigen |
| `neo phys timing <id> auto` | Timing aus dem Chip-Profil des LED-Typs |
| `neo phys timing <id> slow5` … `slow20` / `fast5` … `fast25` | Bitrate um 5–20 % gesenkt bzw. 5–25 % erhöht (für problematische Klone) |
| `neo phys timing <id> freq <kHz>` | Bitrate direkt setzen (z. B. 775), wie das ETS-Feld „Timing“ |
| `neo phys timing <id> custom <t0h> <t0l> <t1h> <t1l>` | Vollständig benutzerdefiniertes Timing in Nanosekunden |
| `neo phys timing <id> reset` | Zurücksetzen auf AUTO-Timing |
| `neo phys timing <id> qualify` (Alias `scan`) | Interaktiver Clone-Scan über den ganzen Streifen; weiter mit `neo scan next`, übernehmen mit `neo scan apply`, abbrechen mit `neo scan stop` |
| `neo phys timing <id> profile <N>` | Clone-Profil N dauerhaft anwenden und in Flash speichern |

Der Clone-Scan (`qualify`/`scan`) testet nacheinander 6 vordefinierte Timing-Profile (800 kHz mit 50, 80 und 280 µs Reset, 720 kHz mit 80 und 280 µs, 640 kHz mit 300 µs). Jedes Profil wird 2 Sekunden in einer eigenen Farbe gezeigt; reagieren die LEDs, übernimmt `neo scan apply` das Profil, alternativ später `profile <N>`. `neo phys timings` listet alle Modi und Profile.

### Level-Shifter-Konfiguration (`neo phys config levelshifter`)

Die Firmware unterstützt drei typische Level-Shifter-Chips, die auf gängiger LED-Hardware verbaut sind:

```
neo phys config <id> levelshifter <none|txs0108|hct125|ahct125|lv8t245>
```

#### Was passiert bei der Auswahl?

| Wert | Chip | Typische Hardware | Firmware-Aktion | Was verbessert sich |
|------|------|-------------------|-----------------|---------------------|
| `none` | — | UP1, direkte Verbindung | Keine GPIO-Änderung | — |
| `txs0108` | TI TXS0108E | **KNeoPiX V1** | Siehe Tabelle unten | Zuverlässige Richtungserkennung, saubere Flanken |
| `hct125` | TI 74HCT125 | QuinLED Dig-Uno V3, Dig-Uno V4 | Keine GPIO-Änderung (transparenter Buffer) | Dokumentation; keine aktive Optimierung nötig |
| `ahct125` | TI 74AHCT125 | **QuinLED Dig-Next-2** | Keine GPIO-Änderung | Dokumentation; keine aktive Optimierung nötig |
| `lv8t245` | TI SN74LV8T245 | **KNeoPiX V1.5** | Keine GPIO-Änderung | Dokumentation; keine aktive Optimierung nötig |

#### Detaillierte Maßnahmen für TXS0108E

Der TXS0108E erkennt die Übertragungsrichtung automatisch über eine interne RC-Schaltung. Damit diese korrekt funktioniert und die B-Seite (LED-Seite) saubere Flanken liefert:

| Plattform | Maßnahme | Warum notwendig |
|-----------|----------|-----------------|
| **RP2040 / RP2350** | Interne Pull-up/Pull-down am Datenpin deaktiviert | Ohmsche Last am A-Pin würde den Strom der RC-Schaltung verfälschen und die Richtungserkennung stören |
| **ESP32** | Treiberleistung auf 40 mA erhöht (`GPIO_DRIVE_CAP_3`, Standard: 20 mA) + Pull-Modus auf FLOATING | Höhere Treiberleistung liefert schärfere Flanken auf der A-Seite → schnellere Richtungserkennung → sauberere Ausgangsflanken auf der B-Seite (LED-Leitung) |

#### Warum HCT/AHCT keine Optimierung braucht

74HCT125 und 74AHCT125 sind **unidirektionale** Puffer: der MCU-Ausgang treibt direkt den Logikeingang des Chips, der Ausgang (LED-Seite) wird vollständig vom Chip getrieben. Die Eingangsschwelle (V_IH ≥ 2,0 V) wird von 3,3-V-MCU-Ausgängen zuverlässig erreicht. Die Standard-Treiberleistung (4 mA bei RP2040, 20 mA bei ESP32) ist für einen Logikeingang mehr als ausreichend — eine GPIO-Änderung würde nichts verbessern.

Die Einstellung `hct125`/`ahct125` dient damit primär der **Dokumentation** und der korrekten Anzeige unter `neo phys config <id> info`.

> **Hinweis zur KNeoPiX-Hardware:** Die Firmware setzt den Pegelwandler **automatisch beim Start** (KNeoPiX V1: `txs0108`, V1.5: `lv8t245`) (`HW_LEVELSHIFTER` im Board-Header von OGM-HardwareConfig; ein Build-Flag `NEOPIXEL_HW_LEVELSHIFTER` geht vor). Eine manuelle Konsolenkonfiguration ist nicht mehr erforderlich. Der Befehl steht weiterhin zur Verfügung, um die Einstellung bei Bedarf zu überschreiben.

> **Hinweis zum OE-Pin:** Auf der KNeoPiX-Hardware ist der OE-Pin des TXS0108E fest mit VCCA (3,3 V) verbunden und dauerhaft aktiv. Eine Firmware-gesteuerte Aktivierung/Deaktivierung ist nicht möglich und auch nicht nötig.

Der aktuelle Wert wird unter `neo phys config <id> info` angezeigt.

### Weitere Config-Befehle (`neo phys config`)

| Befehl | Beschreibung |
|--------|--------------|
| `neo phys config <id> info` | Vollständige Konfigurationsübersicht für den Streifen |
| `neo phys timings` | Alle Timing-Modi und Clone-Profile tabellarisch anzeigen |
| `neo phys timing <id> scan` | Automatischen Clone-Scan starten |

## Diagnose und Sicherheit

Die Applikation prüft zur Laufzeit unter anderem folgende Situationen:

* fehlende oder unpassende Hardwareauswahl
* Konflikte bei der GPIO-Belegung
* fehlgeschlagene Initialisierung einzelner LED-Streifen
* ungültige oder nicht passende Konfigurationen nach Änderungen in ETS

Bei erkannter Fehlkonfiguration kann das Modul Schutzfunktionen aktivieren und problematische Konfigurationen verwerfen, um Fehlverhalten oder Schäden an der Hardware zu vermeiden.

## Hinweise zu Firmware- und ETS-Updates

Nach Änderungen an Hardwareprofilen, Effektparametern oder der ETS-Struktur sollte die vorhandene Parametrierung sorgfältig geprüft werden. Insbesondere bei neuen Hardwarevarianten oder erweiterten Hardwaretabellen ist darauf zu achten, dass:

* die passende Firmware für die Zielhardware verwendet wird,
* die Hardware in ETS korrekt ausgewählt ist,
* bei Bedarf die KNXprod neu importiert wird,
* die Applikation nach einem Versionswechsel erneut geladen wird,
* bei einer Effektkette alle beteiligten Geräte gemeinsam aktualisiert werden: ein Master mit älterer Firmware meldet immer AUS, und ein Slave mit neuer Firmware schaltet sich danach aus; ein Slave mit älterer Firmware startet seinen Effekt bei jedem Telegramm des Masters neu.

Einmalige Änderungen beim „Aktualisieren“:

* **Ab Dev 2.3 werden die Effekt-Parameter einmalig zurückgesetzt.** Die effektspezifischen Parameter jedes Segments und jeder Effektmanager-Cue haben neue IDs (`21CC…` bzw. `22CC…`, gebildet aus Effektnummer und Parameterindex) und stehen nach dem „Aktualisieren“ auf ihrem Standardwert; sie müssen neu gesetzt werden. Die Effektauswahl, die Szenen und alle übrigen Parameter behalten ihre Werte. Spätere neue Effekte oder angehängte Parameter verschieben keine ID mehr.
* **Ab Dev 2.4 prüft die ETS die Effekte gegen die gewählte Hardware** („Effekt nicht verfügbar“ bzw. Hinweis am Segment, an Szenen und Cues, siehe „Effekte je Hardware“). Ein Projekt, dessen Effekte die Firmware der Hardware nicht enthält, zeigt nach dem Update diese Meldungen; das Gerät zeigt dort „Solid“.
* Weitere Migrationshinweise je Version stehen in `CHANGELOG.md`.

## Änderungshistorie

Die Änderungen je Version und die Migrationshinweise stehen in [CHANGELOG.md](../CHANGELOG.md).
