# Farbtemperatur Max (Maximum Kelvin)

Maximale Farbtemperatur der HCL-Kurve in Kelvin (1000-10000K).

## Bedeutung
Diese Einstellung bestimmt die **kühlste (höchste)** Farbtemperatur, die während des HCL-Kurvenverlaufs verwendet wird.

## Wann wird dieser Wert verwendet?

### Zeit-Modus
- In der **Mitte** des Zeitfensters (zwischen Start- und Endzeit)
- Der Übergang erfolgt als **Sinuskurve** für sanfte Anpassung

### Sonnen-Modus
- Zur **Sonnenhöchststand** (Mittag, zwischen Aufgang und Untergang)
- Der Übergang erfolgt als **Sinuskurve** basierend auf der Sonnenposition

## Typische Werte
- **5000K**: Tageslicht für konzentriertes Arbeiten
- **5500K**: Natürliches Tageslicht
- **6000K**: Kühles Tageslicht
- **6500K**: Standardtageslicht (D65) - *Empfohlen für Circadian Lighting*
- **7000K**: Kühles, bläuliches Licht (erhöhter Blauanteil)

## Wichtig
- Der Maximalwert **muss größer** sein als der Minimalwert
- Werte über 10000K werden automatisch auf 10000K begrenzt
- **Wert 0 wird als "nicht konfiguriert" interpretiert** und löst eine Warnung aus (Standard 10000K wird verwendet)

## HCL-Kurvenverlauf
- **Zeit-Modus**: Maximum in der Mitte des Zeitfensters
- **Sonnen-Modus**: Maximum bei Sonnenhöchststand (Mittag)
- Übergänge sind **immer Sinuskurven** (sanft und natürlich)

## Empfehlung für circadianes Licht
**6500K** als Maximum für angenehmes, waches Tageslicht ohne zu hohen Blauanteil