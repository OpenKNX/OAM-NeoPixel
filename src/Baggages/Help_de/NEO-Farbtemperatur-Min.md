# Farbtemperatur Min (Minimum Kelvin)

Minimale Farbtemperatur der HCL-Kurve in Kelvin (1000-10000K).

## Bedeutung
Diese Einstellung bestimmt die **wärmste (niedrigste)** Farbtemperatur, die während des HCL-Kurvenverlaufs verwendet wird.

## Wann wird dieser Wert verwendet?

### Zeit-Modus

- Am **Anfang** und **Ende** des Zeitfensters (Startzeit/Endzeit)
- **Außerhalb** des Zeitfensters (konstant)

### Sonnen-Modus

- Bei **Sonnenaufgang** und **Sonnenuntergang** (inkl. Offsets)
- **Außerhalb** der Sonnenzeiten (vor Aufgang, nach Untergang)

## Auswirkung auf LED-Typen:

### RGBCCT (5-Kanal)

- Bei **Minimum Kelvin**: WW-Kanal = 100%, CW-Kanal = 0%
- Reines, warmes Weißlicht vom Warm-Weiß-Chip
- **Stellen Sie diesen Wert auf die Kelvin-Temperatur Ihres WW-Chips** (typisch 2700K-3000K)

### RGB/RGBW

- RGB-Kanäle werden mit warmer Tönung eingefärbt
- Rotanteil wird erhöht, Blauanteil reduziert

## Typische Werte

- **2200K**: Sehr warmes, kerzenlichtähnliches Licht (Abend/Nacht)
- **2700K**: Warmes Wohnlicht (Standardglühlampe) - *Empfohlen*
- **3000K**: Warmweiß für gemütliche Atmosphäre
- **3500K**: Neutralwarm für Arbeitsbereich mit Wohlfühlfaktor

## Wichtig

- Der Minimalwert **muss kleiner** sein als der Maximalwert
- Werte unter 1000K werden automatisch auf 1000K begrenzt
- **Wert 0 wird als "nicht konfiguriert" interpretiert** und löst eine Warnung aus (Standard 1000K wird verwendet)

## Empfehlung für RGBCCT-Streifen
**Prüfen Sie die Spezifikationen Ihres LED-Streifens** und setzen Sie diesen Wert auf die Kelvin-Temperatur des WW-Chips (z.B. 2700K).