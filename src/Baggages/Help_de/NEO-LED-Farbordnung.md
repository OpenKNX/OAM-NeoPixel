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

**Häufige LED-Typen und ihre Farbordnung:**
- **WS2812B**: GRB
- **WS2815**: GRB
- **SK6812**: GRBW (mit weißem Kanal)
- **APA102**: BGR 
- **SK9822**: BGR 

**Fehlerdiagnose:** Wenn die Farben nicht korrekt angezeigt werden (z.B. Rot erscheint als Blau), ist meist die falsche Farbordnung gewählt. Testen Sie verschiedene Ordnungen bis die Farben stimmen.
