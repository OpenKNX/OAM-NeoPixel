# Status nach Neustart senden

Legt fest, ob das Gerät nach einem Neustart (Wiederkehr der Versorgungsspannung, ETS-Download, Firmware-Update) seine Status-Objekte einmal auf den Bus sendet.

**Verfügbare Optionen:**

- **Nein (Standard)**: Nach Ablauf der Startverzögerung werden die Status-Objekte mit dem aktuellen Zustand belegt, aber nicht gesendet. Ab dann sind sie lesbar (Lesen-Flag) und senden, wenn sich ihr Wert ändert.
- **Ja**: Zusätzlich sendet das Gerät nach Ablauf der Startverzögerung jedes belegte Status-Objekt einmal mit seinem aktuellen Wert. Ein HCL-Status mit 0 K (kein HCL aktiv) wird dabei nicht gesendet, ist aber lesbar.

Status-Telegramme werden gebremst gesendet: höchstens 8 direkt hintereinander, danach eines alle 100 ms. Rückmeldungen auf Telegramme und Zustandsänderungen wechseln sich dabei mit dem Senden nach dem Neustart ab. So überlastet auch ein Telegramm an viele Segmente (z. B. eine Zentral-Szene) den Bus nicht. Ändert sich ein Wert schneller, als er gesendet werden kann (z. B. viele Effektmanager mit kurzen Cues), wird der jeweils aktuelle Wert gesendet; Zwischenwerte entfallen.

**Betroffene Status-Objekte:**

- Allgemein: Alle LEDs Status, Helligkeit Status, Relais 1-4 Status (nur Relais mit gültigem GPIO), HCL Global Zustand
- Je Segment: Segment Ein/Aus Status, Segment Helligkeit Status, Effekt Status, RGB Status, RGBW Status, HSV Status, HCL Zustand, Effekt-Text Status, Effektmanager Status, Aktive Cue, Effektmanager Zustand

**Nicht belegt nach dem Neustart:**

- **Farbtemperatur Status**: Der Kelvin-Wert wird nicht gespeichert. Das Objekt wird erst mit dem nächsten Farbtemperatur-Telegramm belegt und meldet danach den zuletzt empfangenen Wert.
- **Szenensteuerung Status** wird von der Firmware nicht geschrieben.

**Bedeutung der Status-Werte:**

Jedes Status-Objekt meldet den Zustand des Geräts, unabhängig davon, wodurch er entstanden ist (Telegramm, Szene, Effektmanager, globales Objekt, Startverhalten).

- **Segment Ein/Aus Status** ist EIN, wenn das Segment eingeschaltet ist, nicht durch „Alle LEDs“ AUS dunkel ist (ein gesperrtes Segment wird von „Alle LEDs“ nicht geschaltet), ein Effekt aktiv ist und das Segment leuchtet. Ein laufendes Segment leuchtet, solange seine Helligkeit (Segment × global) größer 0 ist; die Farbe wird dabei nicht ausgewertet (Schwarz bei Helligkeit größer 0 gilt als EIN). Ein angehaltenes eingeschaltetes Segment (pausierter Effektmanager) wird nicht neu gezeichnet: Es gilt als EIN, solange mindestens eine LED in seinem Bereich nicht schwarz ist. Bei überlappenden Segmenten zählen dabei auch die LEDs des anderen Segments. Helligkeitsänderungen während der Pause wirken erst nach dem Fortsetzen.
- **RGB/RGBW/HSV Status** melden die aktuelle Primärfarbe. Sie bleibt beim Ausschalten erhalten; eine danach empfangene Farbe wird übernommen und gemeldet.
- **Segment Helligkeit Status** meldet den aktuellen Dimmwert: die eingestellte Helligkeit des Segments selbst, 0 %, solange das Segment aus oder durch „Alle LEDs“ AUS dunkel ist. Die eingestellte Helligkeit bleibt dabei erhalten und gilt beim nächsten EIN wieder. Die globale Helligkeit wirkt als zusätzlicher Faktor und hat ein eigenes Status-Objekt. Ein Effektkette-Slave meldet die vom Master übernommene Helligkeit.
- Ein Telegramm, das den Zustand nicht ändert, erzeugt keine Status-Rückmeldung. Ausnahme: Ein Effekt-Text oder Effekt-Text Anhängen, das der aktive Effekt übernimmt, wird immer mit dem aktuellen Text bestätigt.
- Konsolenbefehle (`neo ...`) sind ein Diagnosewerkzeug: Ihre Wirkung erscheint nicht in jedem Fall im Status (z. B. Segment-Helligkeit über die Konsole, `neo clear`). `neo seg ...` pause/resume/stop/effect wirken direkt auf das Segment und umgehen das Ein/Aus-Modell.
- Ein Telegramm, das während der Startverzögerung empfangen wird, wird nach der Startverzögerung mit dem vollständigen Status des betroffenen Segments (bzw. der allgemeinen Objekte) beantwortet.

**Anwendungsfälle:**

- **Nein**: Visualisierungen und Taster lesen den Zustand bei Bedarf selbst (empfohlen bei vielen Segmenten).
- **Ja**: Teilnehmer ohne Lese-Anforderung (z. B. Tasterbeleuchtung, Logiken) erhalten den Zustand nach einem Neustart automatisch.
