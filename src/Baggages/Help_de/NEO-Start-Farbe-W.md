# Standard-Farbe W (Weiß-Kanal)

Weißwert der Standard-Farbe für den Neustart (0-255).

Dieser Parameter ist nur sichtbar wenn:
- **Global:** "Verhalten nach Neustart (Global)" = "Standard-Farbe"
- **Segment:** "Verhalten nach Neustart" = "Standard-Farbe (eigene)"

**Wertebereich:** 0-255
- 0 = kein Weiß (nur RGB)
- 255 = maximales Weiß

**Standardwert:** 0 (kein Weiß-Kanal)

**Hinweis:** 
- Nur relevant für RGBW-LEDs (z.B. SK6812 RGBW)
- Bei RGB-LEDs (WS2812B) hat dieser Wert keine Wirkung
- Zusammen mit R, G und B ergibt sich die RGBW-Farbe beim Neustart
