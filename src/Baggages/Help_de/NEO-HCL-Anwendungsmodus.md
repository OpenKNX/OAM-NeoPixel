# HCL Anwendungsmodus

Bestimmt, **auf welche Pixel** die HCL-Farbtemperaturkorrektur angewendet wird.

## Modi

### Nur Weiß (0) - *Empfohlen für Mischbetrieb*

- HCL wirkt **nur** auf weiße und entsättigte Pixel (niedrige Sättigung)
- Farbige/saturierte Pixel werden **komplett geschont**
- Harte Abgrenzung an der Sättigungs-Schwelle
- Pixel mit explizitem Weiß-Kanal (W) werden immer beeinflusst

**Anwendung**: 

- Wohnraum mit bunten Effekten und weißem Grundlicht
- Circadian Lighting ohne Verfälschung der Effektfarben
- Wenn Effekte ihre Farben behalten sollen (z.B. roter Cylon bleibt rot)

**Beispiel**: Cylon-Effekt in Rot bleibt rot, weißes Grundlicht wird warm/kalt

### Alle Farben (1) - *Für reine Weißlicht-Strips*

- HCL beeinflusst **alle Pixel gleichmäßig**, unabhängig von Sättigung
- **VOLLE** HCL-Korrektur (100%) auf alle Farben
- Sättigungs-Schwelle wird **ignoriert**
- Auch gesättigte Farben werden zum Kelvin-Weißpunkt verschoben

**Anwendung**:

- Reine Weißlicht-Strips (keine Effekte)
- Stimmungsbeleuchtung, bei der ALLES warm/kalt werden soll
- Wenn bewusste Farbverschiebung gewünscht ist

**Beispiel**: Roter Cylon wird warm-orange morgens, kühl-pink abends

### Hohe Sättigung (2) - *Für Farbakzente*

- HCL wirkt **nur** auf gesättigte (bunte) Pixel (hohe Sättigung)
- Weiße/entsättigte Pixel werden **geschont**
- **Invertiert** zu "Nur Weiß": Startet bei Schwelle, volle Wirkung bei maximaler Sättigung
- Neutrale Bereiche bleiben konstant

**Anwendung**:

- Farbakzente sollen Tagesstimmung widerspiegeln
- Neutrales Weißlicht bleibt unverändert
- Dekorative Beleuchtung mit Stimmungsanpassung

**Beispiel**: Rote Akzente werden warm/kalt, weißes Arbeitslicht bleibt neutral

## Technische Details
Die Entscheidung basiert auf der **HSV-Sättigung** jedes Pixels:

- **Nur Weiß**: `IF (sättigung >= schwelle AND W_Kanal == 0) → SKIP`
- **Alle Farben**: Gewichtung = 100% (immer)
- **Hohe Sättigung**: `IF (sättigung < schwelle) → SKIP`

## Empfehlung

- **Mischbetrieb** (Effekte + Grundlicht): **Nur Weiß**
- **Reines Weißlicht**: **Alle Farben**
- **Farbakzente + neutrales Licht**: **Hohe Sättigung**
