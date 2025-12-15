# Spacing

Fügt dunkle Lücken von M LEDs zwischen den Gruppen ein.

**Wert:** Anzahl der dunklen LEDs zwischen Gruppen (Standard: 0)

**Funktionsweise:**
- Erstellt Abstände zwischen den Gruppen
- Die LEDs im Spacing-Bereich bleiben dunkel (ausgeschaltet)
- Eine wiederholende Einheit besteht aus: **(Gruppierung + Spacing) LEDs**

## Anwendungsfälle

**Getrennte Pixel:** Schafft visuelle Trennung zwischen LED-Gruppen.

**Segmentierte Anzeigen:** Ideal für 7-Segment-Anzeigen oder ähnliche Anwendungen.

**Reduzierte Dichte:** Verringert die scheinbare LED-Dichte für spezielle Effekte.

## Beispiele

- **Spacing=0:** Keine Lücken zwischen Gruppen (Standard)
- **Spacing=1:** Eine dunkle LED zwischen jeder Gruppe
- **Spacing=3:** Drei dunkle LEDs zwischen jeder Gruppe

## Zusammenspiel mit Gruppierung

**Gruppierung=3, Spacing=2:**
```
●●●○○●●●○○●●●○○...
```
- ●●● = 3 LEDs mit gleicher Farbe (Gruppe)
- ○○ = 2 dunkle LEDs (Spacing)
- Wiederholende Einheit = 5 LEDs total