# Topologie

Legt fest wie der physische LED-Streifen in einer 2D- oder 3D-Matrix angeordnet ist.
Standard: 1D — Streifen. Bei 2D Schlangen (Zeilen) ist jede zweite Zeile umgekehrt verdrahtet, was bei den meisten WS2812B-Panels der Fall ist.

## Topologie-Übersicht

- 1D — Streifen: Kein Matrix-Mapping, klassische lineare Kette.
- 2D — Schlangen, Zeilen: Zeilenweise, jede zweite Zeile invertiert.
- 2D — Linear, Zeilen: Zeilenweise, alle Zeilen gleiche Richtung.
- 2D — Schlangen, Spalten: Spaltenweise, jede zweite Spalte invertiert.
- 2D — Linear, Spalten: Spaltenweise, alle Spalten gleiche Richtung.
- 2D — Gekachelt, Spalten linear (Panelkette): Für mehrere Panels in Serie (A->B->C), intern spaltenweise linear je Panel.
- 2D — Gekachelt, Spalten Schlangen (Panelkette): Wie oben, aber intern spaltenweise Schlangenverdrahtung je Panel.

## Wann "Gekachelt" verwenden?

Verwende "Gekachelt", wenn mehrere Matrix-Panels hintereinander verkettet sind und die Verkettung panelweise erfolgt (erst ganzes Panel A, dann ganzes Panel B).

Wichtig: In den gekachelten Topologien bedeutet "Matrix: Tiefe (Ebenen)" die Panel-Höhe pro Tile-Block (z. B. 8 bei 32x8-Panels), nicht 3D-Z-Ebenen.
