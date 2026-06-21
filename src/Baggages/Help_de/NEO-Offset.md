# Offset

Rotiert/verschiebt die Pixel-Zuordnung innerhalb des Segments um N LEDs mit Umbruch am Ende.

**Wert:** Anzahl der LEDs zum Verschieben (0-basiert)

**Funktionsweise:** 

- Der Offset wird VOR dem Rendern der Effekte angewendet
- Animationen "beginnen" N LEDs später im Segment
- Am Ende des Segments erfolgt ein Umbruch zum Anfang

## Anwendungsfälle

**Ecken ausrichten:** Nützlich um ein Muster um Ecken herum oder über mehrere Segmente hinweg auszurichten.

**Effekt-Synchronisation:** Verschiedene Segmente können mit unterschiedlichen Offsets synchronisiert werden.

## Beispiel

Bei einem Segment mit 10 LEDs (0-9) und Offset=3:

- Logische Position 0 → Physische LED 3
- Logische Position 7 → Physische LED 0 (Umbruch)
- Logische Position 9 → Physische LED 2

**Hinweis:** Der Offset ändert nicht die Effekt-Geschwindigkeit, sondern nur die Startposition der Animation.