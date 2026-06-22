### Strom pro LED (mA)

Der geschätzte Stromverbrauch einer einzelnen LED bei maximaler Helligkeit (alle Farben auf 255). Basis für die automatische Stromberechnung.

**Standard-Werte nach LED-Typ:**

- **WS2812B RGB**: 60mA (3×20mA für R,G,B)
- **SK6812 RGB**: 60mA (ähnlich WS2812B)
- **SK6812 RGBW**: 80mA (4×20mA für R,G,B,W)
- **WS2815 RGB**: 45mA (niedriger durch 12V Betrieb)
- **APA102 RGB**: 60mA (3×20mA pro Kanal)

**Spezialwert für WS2815:**

- Setzen Sie den Wert auf **255** für WS2815 LEDs
- Dies aktiviert das spezielle WS2815 Strommodel (12mA Grundverbrauch)
- Genauere Stromberechnung für 12V LEDs

**Anpassung:**

- **Konservativ**: +20% für Sicherheit (72mA statt 60mA)
- **Aggressiv**: -10% wenn Netzteil überdimensioniert
- **Messen**: Multimeter für exakte Werte verwenden

**Auswirkung:**
Höhere Werte = Konservativere Strombegrenzung = Dunklere LEDs bei hoher Last

