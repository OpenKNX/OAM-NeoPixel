# HCL Startzeit

Startzeit für das **Zeitfenster im Zeit-basierten HCL-Modus**.

## Gültig wenn
- **HCL-Kurvetyp = Zeit**

## Funktionsweise
- **Im Zeitfenster** (zwischen Start- und Endzeit):
  - Am Anfang (Startzeit): **Minimum Kelvin** (z.B. 2700K = warm)
  - In der Mitte: **Maximum Kelvin** (z.B. 6500K = kühl/Tageslicht)
  - Am Ende (Endzeit): Wieder **Minimum Kelvin** (warm)
  - Übergang als **Sinuskurve** (sanft und natürlich)

- **Außerhalb des Zeitfensters**:
  - Feste Farbtemperatur bei **Minimum Kelvin** (Warmton)

## Beispiel
Startzeit: **06:00**, Endzeit: **22:00**, Min: **2700K**, Max: **6500K**
- 06:00 Uhr: 2700K (warm, Morgenstart)
- 14:00 Uhr: 6500K (kühl, Tageshöhepunkt)
- 22:00 Uhr: 2700K (warm, Abendende)
- 00:00-06:00 & 22:00-24:00: 2700K (außerhalb = konstant warm)

## Hinweis
Mitternachts-Überschreitungen werden unterstützt (z.B. 22:00 - 06:00).
