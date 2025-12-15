# Ende-LED

Definiert den ersten LED-Index nach dem Segment (exklusiv).

**Wert:** 0-basierter LED-Index (exklusiv)

**Beispiel:** Ende=313 bedeutet, dass das Segment bei LED-Index 312 endet (LED 313 gehört nicht mehr zum Segment).

**Berechnung:** Die Anzahl der LEDs im Segment = Ende − Start

## Beispiele

- **Start=0, Ende=313** → LEDs 0..312 (313 LEDs insgesamt)
- **Start=10, Ende=50** → LEDs 10..49 (40 LEDs insgesamt)

**Wichtig:** Der Ende-Wert ist exklusiv, d.h. die LED mit diesem Index gehört NICHT mehr zum Segment.

**Tipp:** Um ein Segment zu erstellen, das wirklich bei LED 313 (inklusive) endet, setzen Sie Ende = 314.