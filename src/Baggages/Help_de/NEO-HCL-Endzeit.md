# HCL Endzeit

Endzeit für das **Zeitfenster im Zeit-basierten HCL-Modus**.

## Gültig wenn
- **HCL Kurve Typ = Zeit**

## Funktionsweise
Definiert das **Ende des aktiven Zeitfensters**, in dem die HCL-Kurve läuft.

- **Innerhalb des Fensters** (Start bis Ende): Sinuskurve von Min → Max → Min Kelvin
- **Außerhalb des Fensters**: Konstanter **Minimum Kelvin-Wert** (Warmton)

## Mitternachts-Überschreitung
Die Implementierung unterstützt **Zeitfenster über Mitternacht**:
- Beispiel: Startzeit **22:00**, Endzeit **06:00**
  - 22:00-23:59: Im Fenster (Kurve läuft)
  - 00:00-06:00: Immer noch im Fenster (Kurve läuft weiter)
  - 06:00-22:00: Außerhalb (konstant Min Kelvin)

## Beispiel Normal
Startzeit: **06:00**, Endzeit: **22:00**
- Aktiv: 06:00-22:00 (16 Stunden Sinuskurve)
- Inaktiv: 22:00-06:00 (8 Stunden konstant Min Kelvin)

## Beispiel Mitternacht
Startzeit: **18:00**, Endzeit: **08:00**
- Aktiv: 18:00-08:00 (14 Stunden über Mitternacht)
- Inaktiv: 08:00-18:00 (10 Stunden konstant Min Kelvin)
