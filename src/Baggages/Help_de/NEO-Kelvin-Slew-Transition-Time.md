# Kelvin Slew / Übergangszeit

Glättet Änderungen der Farbtemperatur über die Zeit, damit keine abrupten Sprünge auftreten.

## Werte
- **0…255 Sekunden**
- Standard: **10 s** (sanfter Übergang)
- Empfohlen: **30-60 s** für kaum wahrnehmbare Übergänge

## Funktionsweise
- **0 s**: Sofortige Kelvin-Änderung (kann sichtbare Sprünge verursachen)
- **>0 s**: Der Kelvin-Wert "gleitet" sanft zum Zielwert
- Die Übergangsgeschwindigkeit wird automatisch berechnet basierend auf:
  - Differenz zwischen aktuellem und Ziel-Kelvin
  - Konfigurierter Übergangszeit
  - Update-Rate (50 Hz / alle 20ms)

## Beispiele
- **10 s**: Schnelle Anpassung (z.B. bei manuellen KNX-Änderungen)
- **60 s**: Sehr sanfter Übergang (ideal für automatische Tageszeit-Kurven)
- **120 s**: Extrem langsamer Übergang (für unmerkliche circadiane Anpassungen)

## Warum wichtig?
- Verhindert sichtbare Sprünge bei minutenweisen Kurven-Updates
- Sorgt für weiches, professionelles Lichtverhalten
- Besonders wichtig bei Zeit- oder Sonnenstand-Kurven
- Kompensiert Zeitsprünge (z.B. beim NTP-Sync)
