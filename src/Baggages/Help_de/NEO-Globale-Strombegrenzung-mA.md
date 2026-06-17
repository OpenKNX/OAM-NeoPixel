### Globale Strombegrenzung (mA)

Der maximale Strom in Milliampere (mA), der von allen LED-Kanälen zusammen verbraucht werden darf. Wird nur verwendet wenn "Strombegrenzung Typ" auf "Global" gesetzt ist.

**Berechnung des Grenzwertes:**

1. **Netzteil-Kapazität prüfen**: z.B. 5V/10A = 10.000mA
2. **Sicherheitsmargin**: 20% Reserve einplanen = 8.000mA
3. **Controller-Verbrauch**: ca. 200-500mA abziehen = 7.500mA
4. **Grenzwert setzen**: 7.500mA eingeben

**Typische Werte:**

- **Kleine Installation** (30 LEDs): 2.000mA
- **Mittlere Installation** (100 LEDs): 6.000mA
- **Große Installation** (300 LEDs): 15.000mA

**Wichtige Hinweise:**

- Nie den vollen Netzteil-Strom verwenden
- Bei mehreren Kanälen: Summe aller Kanäle beachten
- LED-Typ beeinflusst Stromverbrauch erheblich
- WS2815 (12V) verbraucht pro LED weniger Strom

**Fehlerbehebung:**
LEDs werden dunkler als erwartet? Grenzwert könnte zu niedrig sein.

