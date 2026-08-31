# Effektmanager

Der Effektmanager ermöglicht das Abspielen von vorprogrammierten Lichtsequenzen auf einem virtuellen LED-Segment. Jeder Effektmanager besteht aus bis zu 10 Cues (Einzelszenen), die nacheinander abgespielt werden.

## Konzept

Ein **Effektmanager (EM)** ist eine benannte Sequenz aus Cues. Jeder Cue definiert:

- Welcher **Effekt** gespielt wird (z. B. Fade, Fire, Rainbow)
- **Farbe und Helligkeit** des Effekts
- **Dauer** in Sekunden (0 = unendlich)
- **Überblendzeit** in Millisekunden zum nächsten Cue
- Einen optionalen **Beschreibungstext** (14 Zeichen, z. B. für Anzeigen)

## Einstellungen

### Anzahl Effektmanager
Legt fest, wie viele Effektmanager auf diesem Gerät aktiv sind (0 = keiner, 1–16).  
Nur die aktivierten Effektmanager werden in der Firmware berücksichtigt.

### Beschreibung (Name)
Ein frei wählbarer Name für diesen Effektmanager (bis zu 40 Zeichen).  
Erscheint im KO-Baum und in den KO-Namen. Reine ETS-Doku (nicht auf dem Gerät gespeichert).

### Anzahl aktiver Cues
Wie viele Cues dieser Effektmanager enthält (0 = deaktiviert, 1–10).  
Cues werden der Reihe nach von 1 bis zum letzten aktiven Cue abgespielt.

### Loop

- **Nein** – Der Effektmanager stoppt nach dem letzten Cue (oder wechselt zu „Wenn fertig").
- **Ja** – Die Sequenz wiederholt sich kontinuierlich ab Cue 1.

### Wenn fertig → Effektmanager
Gibt an, welcher Effektmanager nach Abschluss dieser Sequenz automatisch gestartet werden soll.  

- **0** = Stop (kein Folge-EM)
- **1–16** = Nummer des Folge-Effektmanagers

## Steuerung über KNX (pro Segment)

Die Laufzeit-Steuerung gehört zum **Segment** — es läuft immer nur **ein** Effektmanager pro Segment, der Startwert wählt welcher:

- **EM Start** (1 Byte): Startet einen EM. Wert 1–16 = EM-Nummer, 0 = Stop.
- **EM Stop** (1 Bit): Stoppt den laufenden EM. Das Segment kehrt danach zu seinem **direkten Zustand** zurück (zuletzt manuell gesetzte Farbe/Effekt; aus, wenn nichts gesetzt war) — es bleibt **nicht** die letzte Cue stehen.
- **EM Pause/Resume** (1 Bit): 1 = Pause (die aktuelle Cue wird **eingefroren** und bleibt sichtbar, läuft aber nicht weiter), 0 = Resume (an gleicher Stelle fortsetzen).
- **EM Status** (1 Byte): Aktive EM-Nummer (0 = keiner).
- **Aktive Cue** (1 Byte): Aktuell gespielte Cue-Nummer (0 = keine).
- **EM Zustand** (1 Byte): 0 = gestoppt, 1 = läuft, 2 = pausiert.

**Direkte Steuerung übernimmt:** Ein direktes Farb-, Effekt- (Fx) oder Helligkeits-KO auf dem Segment — oder ein Szenen-Abruf — **stoppt einen laufenden EM** und zeigt den manuellen Wert. So holst du das Segment jederzeit unter direkte Kontrolle.

**Festes Licht / fester Cue:** Statisches Licht über Szenen-Abruf (stoppt den EM, zeigt die Szene) oder die normalen Segment-KOs. Eine Cue mit **Dauer 0** hält unendlich (steht still, bis gestoppt/pausiert/weitergeschaltet wird).

## Cues konfigurieren

Jeder Cue wird im gleichnamigen Unterblock konfiguriert:

- **Effekt** – Welcher Lichteffekt gespielt wird (Effekt-ID aus der Effektliste)
- **Parameter 1–5** – Effektspezifische Einstellungen (z. B. Geschwindigkeit, Farbe)
- **R / G / B / W** – Basisfarbe des Effekts (0–255 je Kanal)
- **Helligkeit** – Maximale Helligkeit während dieser Cue (0–255)
- **Dauer (s)** – Wie lange diese Cue gespielt wird (0 = dauerhaft, 65535 = bis Effekt fertig)
- **Überblendzeit (ms)** – Weiche Überblendung zum nächsten Cue
- **Text** – Kurzbeschreibung oder Anzeigetext (max. 14 Zeichen)

## Zusammenspiel mit Segmenten

Ein Effektmanager wird einem **virtuellen Segment** zugewiesen. Das Segment legt fest, welche LEDs bespielt werden. Pro Segment kann getrennt festgelegt werden, welcher Effektmanager nach einem Geräteneustart und welcher bei einer Segment-Power-Flanke AUS → EIN frisch ab Cue 1 startet.

## Hinweis zur Effektkette

Wenn auf einem Segment die **Effektkette** aktiv ist (mehrere Geräte spielen synchron), wird die Effektkette beim Start eines Effektmanagers automatisch pausiert. Nach dem Stop des Effektmanagers wird die Effektkette wieder aufgenommen.