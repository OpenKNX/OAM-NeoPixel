# Gruppierung

Behandelt N aufeinanderfolgende LEDs als einen virtuellen Pixel (alle erhalten die gleiche Farbe).

**Wert:** Anzahl der LEDs pro Gruppe (Standard: 1)

**Funktionsweise:**

- Mehrere physische LEDs werden zu einem logischen Pixel zusammengefasst
- Alle LEDs in einer Gruppe erhalten dieselbe Farbe
- Reduziert die scheinbare Auflösung des Segments

## Anwendungsfälle

**Dicke Pixel:** Ideal für größere "Pixel" oder niedrigere scheinbare Auflösung.

**Gleichmäßige Beleuchtung:** Sorgt für homogenere Lichtverteilung bei dicht gepackten LEDs.

## Beispiele

- **Gruppierung=1:** Jede LED ist ein eigener Pixel (Standard)
- **Gruppierung=3:** Dreiergruppen wirken als ein Pixel
- **Gruppierung=5:** Fünfergruppen erhalten die gleiche Farbe

## Zusammenspiel mit Spacing

Eine wiederholende Einheit besteht aus: **(Gruppierung + Spacing) LEDs**

**Beispiel:** Gruppierung=3, Spacing=2 → "●●●○○●●●○○●●●○○..."