### Auto-Helligkeitsbegrenzung

Aktiviert die automatische Reduzierung der Helligkeit wenn der berechnete Stromverbrauch die eingestellten Grenzen überschreitet.

**Funktionsweise:**
- **Kontinuierliche Überwachung**: Ständige Berechnung des aktuellen Stromverbrauchs
- **Sanfte Anpassung**: Allmähliche Helligkeitsreduzierung ohne Sprünge
- **Intelligente Skalierung**: Bevorzugung bestimmter Farben bei Bedarf
- **Sofortige Reaktion**: Schnelle Anpassung bei Farbwechseln

**WLED-Kompatibilität:**
Diese Implementierung folgt dem WLED-Standard für professionelle LED-Installationen mit bewährten Algorithmen.

**Verhalten:**
- **Niedrige Last**: Keine Einschränkung, volle Helligkeit
- **Mittlere Last**: Leichte Reduzierung, kaum sichtbar
- **Hohe Last**: Deutliche Reduzierung, aber gleichmäßig
- **Überlast**: Maximum verfügbare Helligkeit bei Stromgrenze

**Empfehlung:**
Immer aktiviert lassen für sicheren Betrieb. Deaktivierung nur in speziellen Fällen mit überdimensionierter Stromversorgung.

