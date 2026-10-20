# Start-Farbe W (Weiß-Kanal)

Weißwert der Start-Farbe des Segments (0-255).

Das Segment startet nach jedem Neustart mit der Start-Farbe RGB und diesem Weißwert. Bei „Verhalten nach Neustart“ = „Letzter Zustand“ wird sie durch die gespeicherte Farbe ersetzt, sobald gespeicherte Daten vorliegen.

**Wertebereich:** 0-255

- 0 = kein Weiß (nur RGB)
- 255 = maximales Weiß

**Standardwert:** 0

**Hinweis:**

- Nur relevant für LEDs mit Weiß-Kanal (z. B. SK6812 RGBW)
- Bei RGB-LEDs (z. B. WS2812B) hat dieser Wert keine Wirkung
