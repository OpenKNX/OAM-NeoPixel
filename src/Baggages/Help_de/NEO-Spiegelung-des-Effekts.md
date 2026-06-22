# Spiegelung des Effekts

Rendert nur die erste Hälfte des Segments und spiegelt sie auf die zweite Hälfte für Symmetrie.

**Funktionsweise:**

- Nur die erste Hälfte wird berechnet und angezeigt
- Die zweite Hälfte erhält eine gespiegelte Kopie der ersten Hälfte
- Bei ungerader Segment-Länge bleibt die mittlere LED unverändert
- Erzeugt symmetrische "von der Mitte ausgehende" Effekte

## Anwendungsfälle

**Symmetrische Effekte:** Ideal für "Vom-Zentrum-nach-außen" Looks ohne speziellen Effekt programmieren zu müssen.

**Architektonische Beleuchtung:** Perfekt für symmetrische Gebäude- oder Raumbeleuchtung.

**Dekorative Anwendungen:** Erzeugt harmonische, ausgewogene Lichtmuster.

## Beispiele

**Segment mit 8 LEDs (gerade Anzahl):**
```
Berechnet: [A][B][C][D]
Angezeigt: [A][B][C][D][D][C][B][A]
```

**Segment mit 9 LEDs (ungerade Anzahl):**
```
Berechnet: [A][B][C][D][M]
Angezeigt: [A][B][C][D][M][D][C][B][A]
```
(M = mittlere LED bleibt unverändert)

**Hinweis:** Die Spiegelung wird nach allen anderen Transformationen angewendet.