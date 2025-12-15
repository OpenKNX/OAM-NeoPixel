# Anzahl-der-LED-Streifen

Anzahl der gleichzeitig verwendbaren LED-Streifen (1-8).

Diese Einstellung bestimmt, wie viele LED-Kanäle auf dem Gerät gleichzeitig aktiv sein können. Jeder Kanal kann einen separaten LED-Streifen unabhängig steuern.

**Verfügbare Optionen:**
- **1-4 Kanäle**: Funktioniert auf allen unterstützten Mikrocontrollern
- **5-8 Kanäle**: Nur auf RP2040-basierten Geräten (z.B. SCORPIO Board)

**Hardware-Limitierungen:**
- **RP2040**: Bis zu 8 NRZ-Kanäle (4×PIO0 + 4×PIO1) oder unbegrenzt SPI-Kanäle
- **ESP32**: Bis zu 4 NRZ-Kanäle aufgrund RMT-Limitierung
- **Andere**: Bis zu 4 NRZ-Kanäle

**Wichtig**: Höhere Kanalzahlen benötigen entsprechende Hardware-Unterstützung.