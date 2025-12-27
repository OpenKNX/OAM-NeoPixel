# HCL Anwendungsmodus

Bestimmt, **auf welche Pixel** die HCL-Farbtemperaturkorrektur angewendet wird.

## Modi

### Nur Weiß (0) - *Empfohlen*
- HCL wirkt **hauptsächlich** auf weiße und entsättigte Pixel
- Farbige/saturierte Pixel werden **geschont**
- Harte Abgrenzung an der Sättigungs-Schwelle (siehe Parameter "Sättigungs-Schwelle")
- Pixel mit explizitem Weiß-Kanal (W) werden immer beeinflusst

**Anwendung**: 
- Wohnraum mit bunten Effekten und weißem Grundlicht
- Circadian Lighting ohne Verfälschung der Effektfarben
- Wenn Effekte ihre Farben behalten sollen

### Alle Farben (1)
- HCL kann **alle Pixel** beeinflussen, auch stark saturierte
- Stärke variiert basierend auf Sättigung:
  - Unter Schwelle: Volle HCL-Korrektur (100%)
  - Über Schwelle: Abnehmende Korrektur bis zur maximalen Sättigung (0%)
- Bunte Szenen können sichtbar in Richtung Kelvin-Weißpunkt verschoben werden

**Anwendung**:
- Reine Weißlicht-Strips (keine Effekte)
- Wenn Farbverschiebung gewünscht ist
- Spezialeffekte mit Farbtemperaturmodulation

## Technische Details
Die Entscheidung basiert auf der **HSV-Sättigung** jedes Pixels:
- **Nur Weiß**: IF(sättigung >= schwelle AND W_Kanal == 0) → SKIP
- **Alle Farben**: Gewichtung = f(sättigung, schwelle, kurve)

## Empfehlung
Für die meisten Anwendungen: **Nur Weiß**
