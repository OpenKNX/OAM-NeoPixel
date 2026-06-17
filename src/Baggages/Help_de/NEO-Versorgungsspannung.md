### Betriebsspannung

Die Betriebsspannung des LED-Streifens wird ausschließlich für die Berechnung der Leistungsaufnahme in Watt verwendet. Sie hat **keine Auswirkung** auf die tatsächliche Hardware-Spannung – diese wird durch Ihre Stromversorgung bestimmt.

**Verfügbare Spannungen:**

- **5V**: Standard für die meisten LED-Streifen (WS2812B, WS2815, SK6812, APA102)
- **12V**: Für 12V-LED-Streifen
- **24V**: Für 24V-LED-Streifen

**Standard:** 5V

**Verwendungszweck:**
Diese Einstellung ermöglicht eine korrekte Berechnung der Leistungsaufnahme in Watt:

- **Leistung (W) = Strom (mA) × Spannung (V) ÷ 1000**

Die berechnete Leistung wird über die Kommunikationsobjekte gesendet:

- **Pro Streifen**: KO "Streifen X Leistung (W)"
- **Gesamt**: KO "Gesamtleistung (W)"

**Wichtig:**

- Die Stromaufnahme (mA) ist spannungsunabhängig und wird physikalisch korrekt berechnet
- Nur die Watt-Berechnung verwendet diese Spannungseinstellung
- Stellen Sie sicher, dass die eingestellte Spannung mit Ihrer tatsächlichen Stromversorgung übereinstimmt

**Beispiel:**
Ein LED-Streifen mit 450 mA Stromaufnahme:

- Bei 5V: 450 mA × 5V = 2,25 W
- Bei 12V: 450 mA × 12V = 5,4 W
- Bei 24V: 450 mA × 24V = 10,8 W
