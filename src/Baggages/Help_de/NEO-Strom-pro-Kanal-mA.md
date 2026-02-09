### Stromlimit für Strip (mA)

Maximaler Stromverbrauch für diesen physischen LED-Streifen in Milliampere. Wird verwendet wenn "Strombegrenzung" auf "Fester Wert (mA)" gesetzt ist.

**Berechnung:**
1. **Netzteilleistung** ermitteln (z.B. 5A = 5000mA)
2. **Sicherheitsmargin** abziehen: 5000mA × 0.8 = 4000mA
3. **Diesen Wert** als Limit eintragen

**Anwendungsfälle:**
- **Eigenes Netzteil**: Dieser Strip hat dediziertes Netzteil
- **Bekannte Kapazität**: Genaue Netzteilleistung ist bekannt
- **Maximale Kontrolle**: Exakte Grenzwerte erforderlich

**Typische Werte:**
- **Kleines Netzteil** (2A): 1600mA
- **Mittleres Netzteil** (5A): 4000mA  
- **Großes Netzteil** (10A): 8000mA

**Vorteil:**
Direkte, vorhersagbare Begrenzung ohne automatische Berechnungen.
