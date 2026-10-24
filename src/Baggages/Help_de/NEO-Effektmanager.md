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

- **EM Start** (1 Byte): Startet einen EM. Wert 1–16 = EM-Nummer, 0 = Stop. Ein Start auf einem ausgeschalteten Segment schaltet es ein.
- **EM Stop** (1 Bit): Der Wert 1 stoppt den laufenden EM; der Wert 0 wird ignoriert. Das Segment nimmt danach den Zustand gemäß „Effektmanager-Stop: Rückkehr“ an — es bleibt **nicht** die letzte Cue stehen.
- **EM Pause/Resume** (1 Bit): 1 = Pause (die aktuelle Cue wird **eingefroren** und bleibt sichtbar, läuft aber nicht weiter), 0 = Resume (an gleicher Stelle fortsetzen). Auf einem ausgeschalteten Segment wirkt Resume erst mit dem nächsten EIN; eine Pause auf einem ausgeschalteten Segment wird vom nächsten EIN aufgehoben (es setzt den EM fort).
- **EM Status** (1 Byte): Aktive EM-Nummer (0 = keiner).
- **Aktive Cue** (1 Byte): Aktuell gespielte Cue-Nummer (0 = keine).
- **EM Zustand** (1 Byte): 0 = gestoppt, 1 = läuft, 2 = pausiert.

**Ein/Aus:** „Segment Ein/Aus“ = AUS (ebenso „Segment Helligkeit“ = 0 %) **pausiert** einen laufenden EM und schaltet das Segment dunkel; Cue- und Fade-Position bleiben erhalten. EIN setzt ihn an derselben Stelle fort, auch einen manuell pausierten EM. Ein EIN auf ein bereits eingeschaltetes Segment lässt einen manuell pausierten EM pausiert; steht die Segment-Helligkeit dabei auf 0, setzt EIN ihn mit voller Helligkeit fort. Beim Ausschalten wird das Segment gelöscht; beim Einschalten (auch mit „Alle LEDs“ EIN) startet der Effekt der aktuellen Cue neu, damit auch ein Effekt, der sein Bild nur einmal aufbaut (z. B. Wipe ohne Wiederholung), wieder sichtbar wird. Cue-Dauer und Überblendung laufen an derselben Stelle weiter. Ist „Effektmanager beim Einschalten“ gewählt, startet bei einem EIN auf ein ausgeschaltetes Segment stattdessen dieser frisch ab Cue 1.

**Start aus dem ausgeschalteten Zustand:** Startet ein EM auf einem ausgeschalteten Segment (Objekt, EM-Szene, Effektmanager nach Geräteneustart), schaltet das Ende oder der Stop dieses EM das Segment wieder aus — bei jeder Rückkehr-Einstellung. Ein EIN während des Laufs hebt das auf. Diese Herkunft wird mit dem Zustand gespeichert: Auch nach einem Neustart mit wiederhergestelltem EM schaltet dessen Ende oder Stop das Segment wieder aus.

**Alle LEDs:** „Alle LEDs“ AUS pausiert die EM aller nicht gesperrten Segmente, EIN setzt sie fort (Segmente mit eigenem AUS bleiben aus). Ein gesperrtes Segment wird von „Alle LEDs“ nicht geschaltet und behält seinen Zustand; nach dem Entsperren übernimmt es den aktuellen Zustand von „Alle LEDs“.

**Direkte Steuerung übernimmt:** Ein direktes Farb- oder Effekt-KO (Fx) auf dem Segment — oder ein Szenen-Abruf — **beendet einen laufenden EM** ohne Rückkehr (Effekt und Farbe der aktuellen Cue bleiben) und zeigt den manuellen Wert; ein Einzelkanal-Objekt wie „Rot“ ändert nur seinen Kanal. So holst du das Segment jederzeit unter direkte Kontrolle. Auf einem ausgeschalteten Segment werden Farbe und Effekt nur übernommen und mit dem nächsten EIN sichtbar. Die Segment-Helligkeit (absolut oder relativ) dimmt einen laufenden EM, ohne ihn zu stoppen.

**Festes Licht / fester Cue:** Statisches Licht über Szenen-Abruf (beendet den EM, zeigt die Szene und schaltet das Segment ein; eine Szene mit Helligkeit 0 schaltet es aus) oder die normalen Segment-KOs. Eine Cue mit **Dauer 0** hält unendlich (steht still, bis gestoppt/pausiert/weitergeschaltet wird).

## Cues konfigurieren

Jeder Cue wird im gleichnamigen Unterblock konfiguriert:

- **Effekt** – Welcher Lichteffekt gespielt wird (Effekt-ID aus der Effektliste)
- **Effekt-Parameter** – bis zu 10 effektspezifische Einstellungen (z. B. Geschwindigkeit, Intensität); welche erscheinen, hängt vom gewählten Effekt ab
- **R / G / B / W** – Basisfarbe des Effekts (0–255 je Kanal)
- **Helligkeit** – Maximale Helligkeit während dieser Cue (0–255)
- **Dauer (s)** – Wie lange diese Cue gespielt wird (0 = dauerhaft, 65535 = bis Effekt fertig)
- **Überblendzeit (ms)** – Weiche Überblendung zum nächsten Cue
- **Text** – Kurzbeschreibung oder Anzeigetext (max. 14 Zeichen)

## Zusammenspiel mit Segmenten

Ein Effektmanager wird einem **virtuellen Segment** zugewiesen. Das Segment legt fest, welche LEDs bespielt werden. Pro Segment kann getrennt festgelegt werden, welcher Effektmanager nach einem Geräteneustart und welcher bei einer Segment-Power-Flanke AUS → EIN frisch ab Cue 1 startet.

## Hinweis zur Effektkette

Wenn auf einem Segment die **Effektkette** aktiv ist (mehrere Geräte spielen synchron), wird die Effektkette beim Start eines Effektmanagers automatisch pausiert. Nach dem Stop des Effektmanagers wird die Effektkette wieder aufgenommen. Ein Master sendet während seines EM den direkten Zustand von vor dem EM weiter (Effekt, Farbe, Geschwindigkeit); Ein/Aus und Helligkeit bleiben aktuell. Ein Slave mit eigenem EM (laufend oder pausiert) ignoriert die Telegramme des Masters.