# Kelvin Slew Rate

Definiert die Geschwindigkeit, mit der sich die Farbtemperatur ändert, damit keine abrupten Sprünge auftreten.

## Werte
- **0…255 K/min** (Kelvin pro Minute)
- Standard: **100 K/min**
- Empfohlen: **50-150 K/min** für kaum wahrnehmbare Übergänge

## Funktionsweise
- **0 K/min**: Sofortige Kelvin-Änderung (kann sichtbare Sprünge verursachen)
- **>0 K/min**: Der Kelvin-Wert "gleitet" sanft zum Zielwert mit der angegebenen Rate

## Beispiele
| Slew Rate | Bedeutung | Anwendung |
|-----------|-----------|-----------|
| **255 K/min** | 4500K Änderung in ~18 min | Schnelle manuelle Anpassung |
| **100 K/min** | 4500K Änderung in 45 min | Standard für HCL-Kurven |
| **50 K/min** | 4500K Änderung in 90 min | Sanfte Tageszeit-Übergänge |
| **10 K/min** | 4500K Änderung in 7.5 h | Unmerkliche circadiane Anpassung |

## Warum wichtig?
- Verhindert sichtbare Sprünge bei minutenweisen Kurven-Updates
- Sorgt für weiches, professionelles Lichtverhalten
- Besonders wichtig bei Zeit- oder Sonnenstand-Kurven
- Kompensiert Zeitsprünge (z.B. beim NTP-Sync)
