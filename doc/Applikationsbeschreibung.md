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
* Umfangreiche Effektbibliothek für statische und dynamische Lichtbilder
* Szenenspeicher mit bis zu 10 Szenen je Segment
* Farbsteuerung über RGB, HSV, RGBW und RGBCCT
* Farbtemperatursteuerung für geeignete LED-Typen
* Human Centric Lighting (HCL) global und segmentbezogen
* Leistungsbegrenzung und Rückmeldung von Strom, Last und Leistung
* Hardwareabhängige Relaisausgänge
* Wiederherstellung von Zuständen nach Neustart, soweit parametriert

## Geeignete Hardware

Die Applikation wird für mehrere Hardwarefamilien bereitgestellt. Dazu gehören insbesondere:

* OpenKNXiao KNeoPiX und OpenKNXiao Mini auf RP2040- und RP2350-Basis
* OpenKNX REG2 mit PiPico- und PiPico2-Varianten
* OpenKNX UP1 GW-UART
* Gledopto GL-C-309WL auf ESP32-Basis
* QuinLED Dig2Go, Dig-Uno V3, Dig-Quad V3, Dig-Octa-32-8L und Dig-Next-2

Die konkret verfügbaren Funktionen hängen von der ausgewählten Hardware ab. Insbesondere Netzwerkkonfiguration, OTA-Update, Relaisfunktionen und die Anzahl nutzbarer Ports können hardwarebedingt unterschiedlich sein.

## Inbetriebnahme

Nach dem Laden der physikalischen Adresse und der Applikation sollten zuerst die hardwarebezogenen Grundeinstellungen geprüft werden. Dazu gehören insbesondere:

* Auswahl der passenden Hardwarevariante in ETS
* Kontrolle der Anzahl physischer LED-Streifen
* Auswahl des LED-Typs und der Farbkanalreihenfolge
* Parametrierung der verwendeten GPIO- oder SPI-Pins
* Festlegung der Anzahl LEDs je physischem Streifen
* Prüfung der Spannungsversorgung und der Leistungsgrenzen

Wenn Firmware und ETS-Hardwareauswahl nicht zusammenpassen, wird die Hardwarekonfiguration aus Sicherheitsgründen nicht wie vorgesehen übernommen. In diesem Fall muss entweder die passende Firmware geflasht oder die ETS-Parametrierung korrigiert werden.

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

Die Applikation unterstützt sowohl klassische RGB-Streifen als auch RGBW- und RGBCCT-Varianten. Bei RGBCCT-Streifen können Warmweiß- und Kaltweiß-Anteile getrennt angesprochen werden.

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

Bei einem Firmware-Update sollte geprüft werden, ob sich Effektumfang oder Effektparameter zwischen zwei Versionen geändert haben. In diesem Fall kann eine neue Parametrierung oder ein erneuter Download der Applikation erforderlich sein.

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
| `neo phys timing <id> auto` | Automatisches Standardtiming (800 kHz) |
| `neo phys timing <id> slow10pct` | Bitrate um 10 % reduziert (für problematische Klone) |
| `neo phys timing <id> custom <t0h> <t0l> <t1h> <t1l>` | Vollständig benutzerdefiniertes Timing in Nanosekunden |
| `neo phys timing <id> reset` | Zurücksetzen auf AUTO-Timing |
| `neo phys timing <id> scan` | Automatischer Clone-Scan: durchläuft alle Profile, LEDs zeigen Reaktion optisch an |
| `neo phys timing <id> profile <N>` | Clone-Profil N dauerhaft anwenden und in Flash speichern |

Der Clone-Scan (`scan`) testet nacheinander 6 vordefinierte Timing-Profile durch. Jedes Profil leuchtet die ersten LEDs für 3 Sekunden in einer profilspezifischen Farbe auf. Reagieren die LEDs, kann das zugehörige Profil über `profile <N>` dauerhaft gespeichert werden.

### Level-Shifter-Konfiguration (`neo phys config levelshifter`)

Die Firmware unterstützt drei typische Level-Shifter-Chips, die auf gängiger LED-Hardware verbaut sind:

```
neo phys config <id> levelshifter <none|txs0108|hct125|ahct125>
```

#### Was passiert bei der Auswahl?

| Wert | Chip | Typische Hardware | Firmware-Aktion | Was verbessert sich |
|------|------|-------------------|-----------------|---------------------|
| `none` | — | REG2, UP1, direkte Verbindung | Keine GPIO-Änderung | — |
| `txs0108` | TI TXS0108E | **KNeoPiX** (alle Varianten) | Siehe Tabelle unten | Zuverlässige Richtungserkennung, saubere Flanken |
| `hct125` | TI 74HCT125 | QuinLED Dig-Uno V3, Dig-Quad V3, Dig-Octa | Keine GPIO-Änderung (transparenter Buffer) | Dokumentation; keine aktive Optimierung nötig |
| `ahct125` | TI 74AHCT125 | **QuinLED Dig-Next-2** | Keine GPIO-Änderung | Dokumentation; keine aktive Optimierung nötig |

#### Detaillierte Maßnahmen für TXS0108E

Der TXS0108E erkennt die Übertragungsrichtung automatisch über eine interne RC-Schaltung. Damit diese korrekt funktioniert und die B-Seite (LED-Seite) saubere Flanken liefert:

| Plattform | Maßnahme | Warum notwendig |
|-----------|----------|-----------------|
| **RP2040 / RP2350** | Interne Pull-up/Pull-down am Datenpin deaktiviert | Ohmsche Last am A-Pin würde den Strom der RC-Schaltung verfälschen und die Richtungserkennung stören |
| **ESP32** | Treiberleistung auf 40 mA erhöht (`GPIO_DRIVE_CAP_3`, Standard: 20 mA) + Pull-Modus auf FLOATING | Höhere Treiberleistung liefert schärfere Flanken auf der A-Seite → schnellere Richtungserkennung → sauberere Ausgangsflanken auf der B-Seite (LED-Leitung) |

#### Warum HCT/AHCT keine Optimierung braucht

74HCT125 und 74AHCT125 sind **unidirektionale** Puffer: der MCU-Ausgang treibt direkt den Logikeingang des Chips, der Ausgang (LED-Seite) wird vollständig vom Chip getrieben. Die Eingangsschwelle (V_IH ≥ 2,0 V) wird von 3,3-V-MCU-Ausgängen zuverlässig erreicht. Die Standard-Treiberleistung (4 mA bei RP2040, 20 mA bei ESP32) ist für einen Logikeingang mehr als ausreichend — eine GPIO-Änderung würde nichts verbessern.

Die Einstellung `hct125`/`ahct125` dient damit primär der **Dokumentation** und der korrekten Anzeige unter `neo phys config <id> info`.

> **Hinweis zur KNeoPiX-Hardware:** Ab Firmware-Version 0.3.0 wird `txs0108` für alle KNeoPiX-Boards **automatisch beim Start** gesetzt (Build-Flag `NEOPIXEL_HW_LEVELSHIFTER=1` in der Hardware-Konfiguration). Eine manuelle Konsolenkonfiguration ist nicht mehr erforderlich. Der Befehl steht weiterhin zur Verfügung, um die Einstellung bei Bedarf zu überschreiben.

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
* die Applikation nach einem Versionswechsel erneut geladen wird.

## Änderungshistorie

### Version 0.3.0 (in Entwicklung)

* Neues Timing-System für 1-Wire-Streifen: individuelle Timing-Modi (AUTO, SLOW/FAST ±5–25 %), vollständig benutzerdefiniertes Custom-Timing in Nanosekunden
* Clone-Timing-Scan: automatischer, nicht-blockierender Durchlauf von 6 vordefinierten Clone-Profilen mit optischer LED-Rückmeldung (3 s pro Profil)
* Clone-Profile dauerhaft in Flash speicherbar via `neo phys timing <id> profile <N>`
* Level-Shifter-Konfiguration: Unterstützung für TXS0108E, 74HCT125 und 74AHCT125; automatische Aktivierung per Board-Build-Flag (`NEOPIXEL_HW_LEVELSHIFTER`) — kein manuelles Konsolenkommando mehr nötig für bekannte Hardware (KNeoPiX, QuinLED)
* TXS0108E-Optimierungen: RP2040 deaktiviert interne Pull-Widerstände; ESP32 erhöht Treiberleistung auf 40 mA + FLOATING (sauberere Flanken, zuverlässigere Richtungserkennung)
* HCT/AHCT-Unterstützung: Typen dokumentierend eintragbar, keine GPIO-Eingriffe nötig (transparenter Buffer, Standard-Treiberleistung ausreichend)
* Konsolenbefehl `neo phys config <id> info` zeigt Timing und Level-Shifter-Status vollständig an

### Version 0.2.0

* Ausbau der Hardwareunterstützung, insbesondere für weitere ESP32- und QuinLED-Varianten
* Erweiterung der Applikation auf bis zu 8 physische LED-Streifen
* Überarbeitete Hardwarekonfiguration und verbesserte Hardwareauswahl in ETS
* Ausbau der Dokumentation und der Release-Artefakte

### Version 0.1.0

* Erste öffentlich nutzbare Entwicklungsfassung der NeoPixel-KNX-Applikation



