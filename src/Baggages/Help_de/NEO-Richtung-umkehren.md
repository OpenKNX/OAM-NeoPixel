# Richtung umkehren

Kehrt die Pixel-Reihenfolge des Segments um, sodass Effekte in die entgegengesetzte Richtung laufen.

**Funktionsweise:**

- Die LED-Zuordnung wird von Start→Ende umgekehrt
- Effekte laufen rückwärts durch das Segment
- Die physischen LEDs bleiben unverändert, nur die logische Reihenfolge wird umgekehrt

## Anwendungsfälle

**Gegenläufige Effekte:** Verschiedene Segmente können in entgegengesetzte Richtungen animiert werden.

**Symmetrische Anordnungen:** Ideal für spiegelsymmetrische LED-Anordnungen.

**Korrektur der Verkabelung:** Kompensiert ungünstige Verkabelungsrichtungen ohne Neuverdrahtung.

## Beispiel

**Normales Segment (Start=0, Ende=5):** Der Effekt läuft über die LEDs 0 → 1 → 2 → 3 → 4.

**Umgekehrtes Segment:** Der Effekt läuft über die LEDs 4 → 3 → 2 → 1 → 0.

**Hinweis:** Die Richtungsumkehr wird vor allen anderen Transformationen (Offset, Gruppierung) angewendet.