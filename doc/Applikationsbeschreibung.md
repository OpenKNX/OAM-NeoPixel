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

### Version 0.2.0

* Ausbau der Hardwareunterstützung, insbesondere für weitere ESP32- und QuinLED-Varianten
* Erweiterung der Applikation auf bis zu 8 physische LED-Streifen
* Überarbeitete Hardwarekonfiguration und verbesserte Hardwareauswahl in ETS
* Ausbau der Dokumentation und der Release-Artefakte

### Version 0.1.0

* Erste öffentlich nutzbare Entwicklungsfassung der NeoPixel-KNX-Applikation



