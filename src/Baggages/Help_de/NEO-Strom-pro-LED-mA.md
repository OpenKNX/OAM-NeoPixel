### Strom pro LED (mA)

Global: der Stromverbrauch einer einzelnen LED, wenn alle ihre Kanäle auf 255 stehen. Die Firmware teilt den Wert ganzzahlig auf die Kanäle des LED-Typs auf (z. B. WS2812B: 3, SK6812: 4, FW1906: 6) und berechnet daraus in jedem Bild den Strom. Der Typ „SK6812“ zählt dabei immer 4 Kanäle, auch mit einer Farbordnung ohne Weiß.

**Wertebereich:** 0–70 mA

- **0**: Es gilt die eingebaute Tabelle je LED-Typ: RGB 3 × 20 mA (60 mA), RGBW 4 × 20 mA (80 mA), 5 Kanäle 5 × 20 mA (100 mA), Streifen mit Taktleitung (APA102/SK9822) eigener Tabellenwert.
- **1–70**: Dieser Wert gilt für jede LED, unabhängig vom Typ. Durch die ganzzahlige Teilung zählt z. B. 70 mA bei 4 Kanälen als 4 × 17 = 68 mA; ein globaler Wert kleiner als die Kanalanzahl ergibt 0 mA je Kanal, also keine Begrenzung (der Wert am Streifen ist nur eine Grenze, siehe unten).

**Empfehlung:**

- **Standard-LEDs (WS2812B RGB, SK6812 RGBW)**: 0 — die Tabelle rechnet je Typ genau 20 mA pro Kanal (60 bzw. 80 mA); bei „Pro LED (automatisch)“ ist der Wert zugleich die Grenze, dort einen Wert eintragen (0 = keine Grenze)
- **Ein fester Wert** (z. B. 60 mA) gilt für jeden Typ gleich; bei RGBW zählt 60 mA nur als 4 × 15 mA
- **Abweichende LEDs**: Wert aus dem Datenblatt oder gemessen (Multimeter, ein Streifen voll weiß, Strom ÷ LED-Anzahl)

**Wirkung des globalen Werts:**

- Er ist die Grundlage der Strommessung in allen Modi, auch bei „Fester Wert (mA)“ und bei global „Deaktiviert“ für Streifen mit eigener Grenze; angezeigt wird er in der ETS nur bei „Pro LED (automatisch)“.
- Bei „Pro LED (automatisch)“ ist er für Streifen mit „Global verwenden“ zusätzlich die Grenze: LED-Anzahl × Wert (bei 0 keine Grenze).

**Wirkung am Streifen („Pro LED“):**

- Der Wert legt nur die Grenze dieses Streifens fest: LED-Anzahl × Wert (bei 0 keine Grenze). Gemessen wird weiterhin mit dem globalen Wert, auch wenn er in der ETS ausgeblendet ist.

Ein höherer Wert rechnet mehr Strom bzw. setzt eine höhere Grenze; ein höherer Messwert lässt die Begrenzung früher greifen.
