# Sonnenaufgang-Offset

Konfiguration der zeitlichen Verschiebung für den Sonnenaufgang bei der HCL-Kurve.

Diese Einstellung ermöglicht es, den berechneten astronomischen Sonnenaufgang um einen definierten Zeitraum zu verschieben, um die automatische Farbtemperaturkurve an persönliche Bedürfnisse anzupassen.

## Komponenten:
- **Offset-Typ**: Bestimmt die Richtung der Verschiebung (Plus = später, Minus = früher)
- **Offset-Minuten**: Anzahl der Minuten für die Verschiebung (0-1440)

## Anwendungsbeispiele:
- **Später starten**: Sonnenaufgang + 60 Min für längeres warmes Licht am Morgen
- **Früher starten**: Sonnenaufgang - 30 Min für sanfteres Erwachen

**Abhängigkeiten**: Funktioniert nur bei HCL-Kurventyp "Sonne". Bei "Zeit" oder "Keine" hat diese Einstellung keine Auswirkung.