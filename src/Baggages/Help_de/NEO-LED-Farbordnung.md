### LED-Farbordnung

Die Farbordnung definiert, in welcher Reihenfolge die Farbkanäle an die LEDs gesendet werden. Verschiedene LED-Typen erwarten unterschiedliche Farbordnungen.

**Verfügbare Farbordnungen:**

- **GRB**: Grün-Rot-Blau (Standard für WS2812B, WS2815)
- **RGB**: Rot-Grün-Blau (selten verwendet)
- **BRG**: Blau-Rot-Grün (selten)
- **RBG**: Rot-Blau-Grün (einige LED-Klone)
- **BGR**: Blau-Grün-Rot (**Standard für APA102, SK9822**)
- **GBR**: Grün-Blau-Rot (einige WS2812B-Klone)

**RGBW Varianten** für 4-Kanal LEDs:

- **RGBW**: Rot-Grün-Blau-Weiß
- **GRBW**: Grün-Rot-Blau-Weiß (**Standard für SK6812**)

**RGBCCT Varianten** für 5-Kanal LEDs (mit separatem Warm- und Kaltweiß):

- **RGBCCT**: Rot-Grün-Blau-Warmweiß-Kaltweiß
- **GRBCCT**: Grün-Rot-Blau-Warmweiß-Kaltweiß
- **RGBCTW**: Rot-Grün-Blau-Kaltweiß-Warmweiß (umgekehrte W-Reihenfolge)
- **GRBCTW**: Grün-Rot-Blau-Kaltweiß-Warmweiß

**RGBCCT für HCL (Human Centric Lighting):**
Für optimale Farbtemperatursteuerung empfehlen wir 5-Kanal RGBCCT-Streifen. Das HCL-System kann die WW/CW-Kanäle direkt ansteuern:

- **Warme Farbtemperatur** (z.B. 2700K): WW hoch, CW niedrig
- **Kühle Farbtemperatur** (z.B. 6500K): WW niedrig, CW hoch
- **Neutrale Farbtemperatur**: Mischung aus beiden

**Häufige LED-Typen und ihre Farbordnung:**

- **WS2812B**: GRB
- **WS2815**: GRB
- **SK6812**: GRBW (mit weißem Kanal)
- **APA102**: BGR 
- **SK9822**: BGR
- **RGBCCT-Streifen**: RGBCCT oder GRBCCT (je nach Hersteller)

**Fehlerdiagnose:** Wenn die Farben nicht korrekt angezeigt werden (z.B. Rot erscheint als Blau), ist meist die falsche Farbordnung gewählt. Testen Sie verschiedene Ordnungen bis die Farben stimmen.
