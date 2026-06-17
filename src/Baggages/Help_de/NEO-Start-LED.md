# Start-LED

Definiert den ersten LED-Index für das Segment.

**Wert:** 0-basierter LED-Index (inklusive)

**Beispiel:** Start=0 bedeutet, dass das Segment bei der ersten LED (Index 0) beginnt.

**Hinweis:** Der Start-Wert ist inklusive, d.h. die LED mit diesem Index gehört zum Segment dazu.

## Verwendung mit Ende-LED

Zusammen mit der Ende-LED definiert der Start-Wert den physischen LED-Bereich für das Segment:

- **Start = 0, Ende = 313** → LEDs 0..312 (313 LEDs insgesamt)
- Die Anzeige "313 LEDs" zeigt die Anzahl = Ende − Start

**Tipp:** Um ein Segment zu erstellen, das wirklich bei LED 313 (inklusive) endet, setzen Sie Ende = 314, da der Ende-Wert exklusiv ist.