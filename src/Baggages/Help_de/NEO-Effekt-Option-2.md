# Effekt-Option-2

Zweite effekt-spezifische Einstellung für erweiterte Anpassungen des Animationsverhaltens.

**Wert:** 0-255 (je nach Effekt unterschiedliche Bedeutung)

## Effekt-spezifische Verwendung (implementierte Effekte)

**Fire (10):**
- Funken-Rate (50-200, Standard: 120)
- Wahrscheinlichkeit für neue Funken am Boden der Flamme

**Theater Chase (11) & Theater Chase Rainbow (12):**
- Punkt-Größe (1-5, Standard: 1)
- Anzahl aufeinanderfolgender LEDs pro leuchtendem Punkt

**Sinelon (13):**
- Punkt-Größe (1-5, Standard: 1)
- Anzahl LEDs für den bewegten Punkt

**Twinkle (14) & Sparkle (15):**
- **Twinkle:** Dichte/Wahrscheinlichkeit (10-200, Standard: 100)
- **Sparkle:** Anzahl gleichzeitiger Funken (1-8, basierend auf Speed)

**Breathing (16), Strobe (17), Pulse (18):**
- **Breathing:** Atmungs-Kurve (0=linear, 255=exponentiell)
- **Strobe:** Minimale Helligkeit wenn "aus" (0-100, Standard: 0)
- **Pulse:** Gamma-Korrektur-Stärke (0-255, Standard: 128)

**Comet (19) & Meteor (20):**
- **Comet:** Schweif-Länge (5-30, Standard: 10)
- **Meteor:** Meteor-Häufigkeit (100-5000ms, basierend auf Speed)

## Effekte ohne Option-2-Unterstützung

Die folgenden implementierten Effekte nutzen Option-2 derzeit nicht:
- **Static (0):** Keine Optionen verfügbar
- **SK6812Test (8) / GarageDoor (9):** Spezialeffekte

## Interaktion mit Option-1

Option-2 arbeitet oft in Kombination mit Option-1:
- Option-1 steuert meist die Haupteigenschaft des Effekts
- Option-2 verfeinert das Verhalten oder fügt Sekundäreffekte hinzu
- Beide zusammen ermöglichen eine umfassende Anpassung

## Entwicklungsstand

**Hinweis:** Die Option-2-Implementierungen sind teilweise experimentell und können in zukünftigen Versionen verfeinert werden.

**Standard:** Beginnen Sie mit Option-2 = 128 für ausgewogenes Verhalten und experimentieren Sie mit anderen Werten.