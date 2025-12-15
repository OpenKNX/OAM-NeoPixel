# Effekt-Feature-1

Erstes Feature-Flag zur Aktivierung spezieller Effekt-Eigenschaften.

**Wert:** Ein/Aus (Boolean)

**Funktionsweise:**
- Schaltet spezielle Funktionen des aktuellen Effekts ein oder aus
- Bietet zusätzliche Kontrollmöglichkeiten über das Effekt-Verhalten
- Arbeitet unabhängig von den Options-Parametern

## Feature-spezifische Verwendung (implementierte Effekte)

**Fire (10):**
- **EIN:** Flammen bewegen sich nach unten (umgekehrte Richtung)
- **AUS:** Flammen bewegen sich nach oben (normale Richtung)

**Theater Chase (11) & Theater Chase Rainbow (12):**
- **EIN:** Trail-Modus (vorherige Pixel verblassen langsam)
- **AUS:** Klarer Modus (alle Pixel werden komplett gelöscht)

**Sinelon (13):**
- **EIN:** Regenbogen-Modus (ignoriert gesetzte Farbe)
- **AUS:** Verwendet die konfigurierte Farbe

**Twinkle (14) & Sparkle (15):**
- **Twinkle EIN:** Regenbogen-Funken statt gesetzter Farbe
- **Sparkle EIN:** Nur weiße Funken (ignoriert andere Farben)
- **AUS:** Verwendet konfigurierte Farbe oder Standard-Verhalten

**Breathing (16), Strobe (17), Pulse (18):**
- **Breathing EIN:** Pause am Höhepunkt des Atmens
- **Strobe EIN:** Zufällige Timing-Intervalle
- **Pulse EIN:** Scharfe Pulse statt weiche Übergänge
- **AUS:** Normales/glattes Verhalten

**Comet (19) & Meteor (20):**
- **Comet EIN:** Bounce-Modus (prallt an den Enden ab)
- **Meteor EIN:** Zufällige Farben für jeden Meteor
- **AUS:** Durchlauf-Modus/konfigurierte Farbe

## Effekte ohne Feature-1-Unterstützung

Die folgenden implementierten Effekte nutzen Feature-1 derzeit nicht:
- **Static (0):** Keine Features verfügbar
- **Juggle (5):** Features noch nicht implementiert
- **BPM (6):** Features noch nicht implementiert
- **SK6812Test (8) / GarageDoor (9):** Spezialeffekte

## Nicht implementierte Effekte
Feature-Beschreibungen für folgende Effekte sind geplant, aber noch nicht verfügbar:
- **Theater Chase, Fire, Meteor, Twinkle, etc.**

## Standard-Verhalten

- **Standard:** AUS (deaktiviert)
- Die meisten Effekte funktionieren ohne Feature-Flags
- Features fügen meist zusätzliche Komplexität hinzu

**Empfehlung:** Experimentieren Sie mit Feature-1 um zusätzliche Effekt-Variationen zu entdecken.