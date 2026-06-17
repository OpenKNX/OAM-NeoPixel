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
Ein frei wählbarer Name für diesen Effektmanager (bis zu 16 Zeichen).  
Erscheint im KO-Baum und in Visualisierungen.

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

## Steuerung über KNX

Jeder Effektmanager besitzt vier Kommunikationsobjekte:

- EM Start (1 Byte):
	Startet den EM. Wert 1-16 = EM-Nummer, 0 = Stop.
- EM Status (1 Byte):
	Gibt die aktive EM-Nummer zurueck (0 = kein EM aktiv).
- EM Stop (1 Bit):
	Stoppt den laufenden EM sofort.
- Aktive Cue (1 Byte):
	Gibt die aktuell gespielte Cue-Nummer zurueck (0 = keine Cue aktiv).

## Cues konfigurieren

Jeder Cue wird im gleichnamigen Unterblock konfiguriert:

- **Effekt** – Welcher Lichteffekt gespielt wird (Effekt-ID aus der Effektliste)
- **Parameter 1–5** – Effektspezifische Einstellungen (z. B. Geschwindigkeit, Farbe)
- **R / G / B / W** – Basisfarbe des Effekts (0–255 je Kanal)
- **Helligkeit** – Maximale Helligkeit während dieser Cue (0–255)
- **Dauer (s)** – Wie lange dieser Cue gespielt wird (0 = dauerhaft bis Stop-KO)
- **Überblendzeit (ms)** – Weiche Überblendung zum nächsten Cue
- **Text** – Kurzbeschreibung oder Anzeigetext (max. 14 Zeichen)

## Zusammenspiel mit Segmenten

Ein Effektmanager wird einem **virtuellen Segment** zugewiesen. Das Segment legt fest, welche LEDs bespielt werden. Über das Segment-KO „Effektmanager bei Start" kann beim Gerätestart automatisch ein EM gestartet werden.

## Hinweis zur Effektkette

Wenn auf einem Segment die **Effektkette** aktiv ist (mehrere Geräte spielen synchron), wird die Effektkette beim Start eines Effektmanagers automatisch pausiert. Nach dem Stop des Effektmanagers wird die Effektkette wieder aufgenommen.