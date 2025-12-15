# Effekt-Feature-2

Zweites Feature-Flag für erweiterte Effekt-Eigenschaften und Kombinationen.

**Wert:** Ein/Aus (Boolean)

**Funktionsweise:**
- Aktiviert sekundäre oder experimentelle Effekt-Features  
- Oft in Kombination mit Feature-1 verwendet
- Ermöglicht komplexere Effekt-Variationen

## Feature-spezifische Verwendung (implementierte Effekte)

**Fire (10):**
- **EIN:** Blaue Flammen-Simulation (Cyan-Weiß-Farbverlauf)
- **AUS:** Normale rote Flammen (Rot-Orange-Gelb-Weiß)

**Sinelon (13):**
- **EIN:** Linearer Bounce-Modus (gleichmäßige Geschwindigkeit)
- **AUS:** Sinuswellen-Bewegung (natürliche Beschleunigung)

**Twinkle (14) & Sparkle (15):**
- **Twinkle EIN:** Variable Helligkeit für jeden Funken
- **Sparkle EIN:** Burst-Modus (gruppierte Funken-Explosionen)
- **AUS:** Konstante Helligkeit/kontinuierliche Funken

**Breathing (16), Strobe (17), Pulse (18):**
- **Breathing EIN:** Regenbogen-Atmung (Farbe ändert sich)
- **Strobe EIN:** Regenbogen-Stroboskop (schnell wechselnde Farben)
- **Pulse EIN:** Regenbogen-Pulse (Farbe zykliert)
- **AUS:** Verwendet konfigurierte Farbe

**Comet (19) & Meteor (20):**
- **Comet EIN:** Regenbogen-Schweif mit Farbverlauf
- **Meteor EIN:** Mehrere Meteore gleichzeitig
- **AUS:** Einzelfarben-Komet/einzelner Meteor

## Effekte ohne Feature-2-Unterstützung

Die folgenden implementierten Effekte nutzen Feature-2 derzeit nicht:
- **Static (0):** Keine erweiterten Features
- **Juggle (5):** Erweiterte Features in Entwicklung
- **SK6812Test (8) / GarageDoor (9):** Spezialeffekte

## Kombination mit Feature-1

Beide Features können gleichzeitig aktiviert werden:
- **Feature-1 + Feature-2 AUS:** Basis-Erweiterung
- **Feature-1 AUS + Feature-2 EIN:** Alternative Erweiterung  
- **Feature-1 + Feature-2 EIN:** Maximale Komplexität (experimentell)

## Entwicklungsstand

**Hinweis:** Die meisten Feature-2-Implementierungen sind experimentell und können in zukünftigen Versionen verfeinert werden.

**Standard:** AUS (für stabiles Verhalten)