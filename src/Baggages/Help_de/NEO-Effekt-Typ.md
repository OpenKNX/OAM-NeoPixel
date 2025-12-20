# Effekt-Typ

Wählt den Animationseffekt für die LED-Streifen aus.

**Aktuell verfügbare Effekte in OFM-NeoPixel:**

## **Implementierte Effekte (ID 0-21)**

- **No Effect (0):** Keine Animation
- **Solid (1):** Statische Farbe ohne Animation
- **Wipe (2):** Progressive Farbfüllung über den Streifen
- **Rainbow (3):** Regenbogen-Farbverlauf basierend auf FastLED
- **Pride2015 (4):** Pride-Effekt mit Farbwellen (FastLED-Port von Mark Kriegsman)
- **Confetti (5):** Zufällige farbige Pixel mit Ausblendung (FastLED-Port)
- **Juggle (6):** Bewegende Sinuswellen-Punkte (FastLED-Port)
- **BPM (7):** Beat-synchrone Animation mit Farbpalette (FastLED-Port)
- **Cylon (8):** Springender Punkt mit Fade-Spur (Knight Rider-Stil)
- **RGBWTest (9):** Testeffekt für SK6812 RGBW-LEDs
- **GarageDoor (10):** Garagentor-Animationseffekt
- **Fire (11):** Realistische Feuer-Simulation mit flackernden Flammen (FastLED Fire2012-Port)
- **Theater Chase (12):** Klassischer Theater-Marquee-Effekt mit konfigurierbaren Punkten
- **Theater Chase Rainbow (13):** Theater-Chase mit zyklischen Regenbogenfarben
- **Sinelon (14):** Bewegter Punkt mit Sinuswellen-Bewegung und Schweif (FastLED-Port)
- **Twinkle (15):** Zufällige Pixel funkeln und verblassen wie Sterne
- **Sparkle (16):** Party-Funkel-Effekt mit zufälligen hellen Blitzen
- **Breathing (17):** Sanfter Atmen-Effekt mit konfigurierbarer Kurve
- **Strobe (18):** Stroboskop-Effekt mit einstellbarem Timing
- **Pulse (19):** Puls-Effekt mit konfigurierbarer Breite und Gamma-Korrektur
- **Comet (20):** Bewegender Komet mit verblassendem Schweif
- **Meteor (21):** Meteor-Schauer mit zufälligen Größen und Farben

## **Standard-Effekte (0-8)**
Diese Effekte sind immer verfügbar.

## **Erweiterte Effekte (9-21)**
Diese Effekte sind nur verfügbar, wenn das Projekt ohne `NEOPIXEL_MINIMAL_EFFECTS` Flag kompiliert wird.

## **Parameter-Integration**
Alle Effekte sind vollständig mit OAM-Parametern integriert:
- **Speed & Intensity:** Universelle Geschwindigkeits- und Helligkeitssteuerung
- **Option1-3:** Effekt-spezifische Parameter (Fade-Raten, Größen, Häufigkeiten)
- **Feature1-2:** Boolean-Schalter (Bounce-Modi, Farbmodi, Richtungsumkehr)

**Status:** Alle 22 Effekte (0-21) sind vollständig implementiert und einsatzbereit!