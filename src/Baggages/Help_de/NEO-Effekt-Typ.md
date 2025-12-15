# Effekt-Typ

Wählt den Animationseffekt für die LED-Streifen aus.

**Aktuell verfügbare Effekte in OFM-NeoPixel:**

## **Implementierte Effekte (ID 0-20)**

- **Solid (0):** Statische Farbe ohne Animation
- **Wipe (1):** Progressive Farbfüllung über den Streifen
- **Rainbow (2):** Regenbogen-Farbverlauf basierend auf FastLED
- **Pride2015 (3):** Pride-Effekt mit Farbwellen (FastLED-Port von Mark Kriegsman)
- **Confetti (4):** Zufällige farbige Pixel mit Ausblendung (FastLED-Port)
- **Juggle (5):** Bewegende Sinuswellen-Punkte (FastLED-Port)
- **BPM (6):** Beat-synchrone Animation mit Farbpalette (FastLED-Port)
- **Cylon (7):** Springender Punkt mit Fade-Spur (Knight Rider-Stil)
- **SK6812Test (8):** Testeffekt für SK6812 RGBW-LEDs
- **GarageDoor (9):** Garagentor-Animationseffekt
- **Fire (10):** Realistische Feuer-Simulation mit flackernden Flammen (FastLED Fire2012-Port)
- **Theater Chase (11):** Klassischer Theater-Marquee-Effekt mit konfigurierbaren Punkten
- **Theater Chase Rainbow (12):** Theater-Chase mit zyklischen Regenbogenfarben
- **Sinelon (13):** Bewegter Punkt mit Sinuswellen-Bewegung und Schweif (FastLED-Port)
- **Twinkle (14):** Zufällige Pixel funkeln und verblassen wie Sterne
- **Sparkle (15):** Party-Funkel-Effekt mit zufälligen hellen Blitzen
- **Breathing (16):** Sanfter Atmen-Effekt mit konfigurierbarer Kurve
- **Strobe (17):** Stroboskop-Effekt mit einstellbarem Timing
- **Pulse (18):** Puls-Effekt mit konfigurierbarer Breite und Gamma-Korrektur
- **Comet (19):** Bewegender Komet mit verblassendem Schweif
- **Meteor (20):** Meteor-Schauer mit zufälligen Größen und Farben

## **Parameter-Integration**
Alle Effekte (10-20) sind vollständig mit OAM-Parametern integriert:
- **Speed & Intensity:** Universelle Geschwindigkeits- und Helligkeitssteuerung
- **Option1-3:** Effekt-spezifische Parameter (Fade-Raten, Größen, Häufigkeiten)
- **Feature1-2:** Boolean-Schalter (Bounce-Modi, Farbmodi, Richtungsumkehr)

**Status:** Alle 21 Effekte sind vollständig implementiert und einsatzbereit! Die ETS-Konfiguration entspricht der tatsächlichen Implementierung.

**Hinweis zur Nummerierung:** Die Effekt-IDs in der ETS-Konfiguration (XML) unterscheiden sich von den internen Console-IDs aufgrund unterschiedlicher Implementierungsreihenfolgen.