### Überspringe erste LEDs

Anzahl der LEDs am Anfang des Streifens, die nicht angesteuert werden sollen. Nützlich wenn der LED-Streifen physisch länger ist als der gewünschte Beleuchtungsbereich.

**Anwendungsfälle:**

- **Reserv-LEDs**: Erste LEDs als Reserve für defekte LEDs
- **Montage-Bereich**: LEDs im Montagebereich ausblenden
- **Segmentierung**: Teil eines langen Streifens für anderen Zweck verwenden
- **Kabelführung**: LEDs im Kabelkanal nicht aktivieren

**Beispiel:**

- LED-Streifen hat 50 LEDs
- "Überspringe erste LEDs" = 10
- "Länge" = 30
- Resultat: LEDs 11-40 werden angesteuert, LEDs 1-10 und 41-50 bleiben aus

**Stromberechnung:**
Übersprungene LEDs werden nicht in die Stromberechnung einbezogen, was Energie spart.

