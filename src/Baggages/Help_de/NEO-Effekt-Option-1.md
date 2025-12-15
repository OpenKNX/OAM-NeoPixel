# Effekt-Option-1

Erste effekt-spezifische Einstellung zur Feinabstimmung des Animationsverhaltens.

**Wert:** 0-255 (je nach Effekt unterschiedliche Bedeutung)

## Effekt-spezifische Verwendung (implementierte Effekte)

**Fire (10):**
- Kühlungsrate der Flammen (20-100, Standard: 55)
- 0 = langanhaltende Flammen, 255 = schnell abkühlende Glut

**Theater Chase (11) & Theater Chase Rainbow (12):**
- Abstand zwischen den leuchtenden Punkten (1-10, Standard: 3)
- Bestimmt die Gruppengröße der Chase-Animation

**Sinelon (13):**
- Fade-Geschwindigkeit der Schweif-Spur (200-250, Standard: 235)
- Höhere Werte = langsamerer Fade-Effekt

**Twinkle (14) & Sparkle (15):**
- Fade-Geschwindigkeit der Funken (200-240/180-220, Standard: 220/200)
- 0 = schnelles Verschwinden, 255 = langsames Ausblenden

**Breathing (16), Strobe (17), Pulse (18):**
- **Breathing:** Minimale Helligkeit (0-255, Standard: 0)
- **Strobe:** Ein/Aus-Verhältnis (10-200, Standard: 100 = 50/50)
- **Pulse:** Puls-Breite (10-200, Standard: 100)

**Comet (19) & Meteor (20):**
- **Comet:** Schweif-Fade-Rate (200-250, Standard: 230)
- **Meteor:** Meteor-Größenbereich (2-15, Standard: 3-8)

## Effekte ohne Option-1-Unterstützung

Die folgenden implementierten Effekte nutzen Option-1 derzeit nicht:
- **Static (0):** Keine Optionen verfügbar
- **SK6812Test (8) / GarageDoor (9):** Spezialeffekte mit eigener Logik

## Standard-Werte

Die meisten Effekte funktionieren gut mit Option-1 = 128 (mittlerer Wert). 

**Entwicklungsstand:** Viele Option-1-Implementierungen sind noch experimentell und können verfeinert werden.

**Tipp:** Experimentieren Sie mit verschiedenen Werten um herauszufinden welche am besten zu Ihrem gewünschten Effekt passen.