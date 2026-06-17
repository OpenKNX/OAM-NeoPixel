# Timing-Modus

Bestimmt den Timing-Modus für die Datenübertragung auf den LED-Streifen. Dies optimiert die Synchronisation mit verschiedenen Arten von WS2812B und ähnlichen LED-Streifen.

## Verfügbare Modi:

- **0 AUTO**: 800 kHz, automatische Erkennung (Standard)
- **1 AUTO_LEGACY**: 960 kHz*, für WS2812C/D und Onboard-LEDs
- **2 SLOW_20PCT**: 640 kHz, -20 % für Signalprobleme
- **3 SLOW_15PCT**: 680 kHz, -15 %
- **4 SLOW_10PCT**: 720 kHz, -10 %
- **5 SLOW_5PCT**: 760 kHz, -5 %
- **6 FAST_5PCT**: 840 kHz, +5 % Leistungssteigerung
- **7 FAST_10PCT**: 880 kHz, +10 %
- **8 FAST_15PCT**: 920 kHz, +15 %
- **9 FAST_20PCT**: 960 kHz, +20 %
- **10 FAST_25PCT**: 1000 kHz, +25 % Maximum

*LEGACY-Modus verwendet einen festen Klock-Divider; die tatsächliche Bitrate hängt von der CPU-Frequenz ab.

## Empfehlungen:

- **AUTO (Standard)**: Für die meisten Anwendungen ausreichend und wird automatisch optimiert
- **AUTO_LEGACY**: Wenn ältere WS2812C/D LEDs oder Onboard-LEDs verwendet werden
- **SLOW_xx**: Bei Signalintegritätsproblemen, langen Kabeln oder Störeinflüssen
- **FAST_xx**: Bei stabilen Verbindungen zur Verbesserung der Datenrate

## Fehlerbehandlung:

Falls LEDs flackern oder nicht ansprechen:

1. Mit **AUTO** starten (Standard)
2. Bei Problemen zu **AUTO_LEGACY** wechseln
3. Falls weiterhin Probleme: Zu **SLOW_5PCT** bis **SLOW_20PCT** reduzieren
4. Kabelqualität und -länge überprüfen

## Technische Details:

- WS2812B benötigt typischerweise 800 kHz Takt
- Die Timing-Modi passen die Bitrate um 5-25% an
- Längere Kabel erfordern möglicherweise niedrigere Bitraten
- Mehrere Streifen können unterschiedliche Timing-Modi benötigen
