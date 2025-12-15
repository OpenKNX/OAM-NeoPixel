# Effekt-Intensität

Steuert die Intensität oder Stärke des ausgewählten Animationseffekts.

**Wert:** 0-255 (0 = minimal, 255 = maximal)

**Funktionsweise:**
- Beeinflusst verschiedene Aspekte je nach Effekt-Typ
- Höhere Werte verstärken die Effekt-Eigenschaften
- Niedrigere Werte reduzieren die Sichtbarkeit des Effekts
- Unabhängig von der globalen Helligkeit

## Effekt-spezifische Anwendung (implementierte Effekte)

**Static (0):**
- Steuert die Gesamthelligkeit der statischen Farbe
- 0 = ausgeschaltet, 255 = maximale Helligkeit

**Wipe (1):**
- Beeinflusst die Helligkeit des Wipe-Effekts
- Wird mit der globalen Helligkeit kombiniert

**Rainbow (2):**
- Steuert die Helligkeit des Regenbogen-Effekts
- Angewendet über FastLEDMath::scale8()

**Pride2015 (3):**
- Beeinflusst die Gesamthelligkeit der Farbwellen
- Komplexer FastLED-Effekt mit dynamischer Helligkeitsmodulation

**Confetti (4):**
- Steuert die Helligkeit der zufälligen Pixel
- Höhere Werte = hellere Konfetti-Pixel

**Juggle (5):**
- Beeinflusst die Helligkeit der Sinuswellen-Punkte
- Angewendet auf die berechneten Farbwerte

**BPM (6):**
- Steuert die Gesamthelligkeit des Beat-Effekts
- Kombiniert mit der Beat-basierten Helligkeitsmodulation

**Cylon (7):**
- Beeinflusst die Helligkeit des springenden Punkts
- Höhere Werte = hellerer Schweif

**SK6812Test (8) / GarageDoor (9):**
- Spezielle Testeffekte mit eigener Intensitätslogik

## Nicht implementierte Effekte
Die folgenden Effekte sind in der ETS-Auswahl verfügbar, aber noch nicht implementiert:
- **Twinkle, Fire, Meteor:** Verwenden derzeit Standard-Solid-Effekt

## Empfohlene Werte

- **Subtile Effekte:** 64-128
- **Standard-Intensität:** 128-192
- **Maximale Wirkung:** 192-255

**Hinweis:** Bei statischen Effekten (Static, Rainbow) hat die Intensität möglicherweise keine sichtbare Auswirkung.