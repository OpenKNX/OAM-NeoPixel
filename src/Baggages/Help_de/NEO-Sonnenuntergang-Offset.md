# Sonnenuntergang-Offset

Konfiguration der zeitlichen Verschiebung für den Sonnenuntergang bei der HCL-Kurve.

Diese Einstellung ermöglicht es, den berechneten astronomischen Sonnenuntergang um einen definierten Zeitraum zu verschieben, um die automatische Farbtemperaturkurve an persönliche Bedürfnisse anzupassen.

## Komponenten:
- **Offset-Typ**: Bestimmt die Richtung der Verschiebung (Plus = später, Minus = früher)
- **Offset-Minuten**: Anzahl der Minuten für die Verschiebung (0-1440)

## Anwendungsbeispiele:
- **Früher starten**: Sonnenuntergang - 45 Min für früheren Beginn des warmen Abendlichts
- **Später starten**: Sonnenuntergang + 30 Min für längeres kühles Licht am Abend

**Abhängigkeiten**: Funktioniert nur bei HCL-Kurventyp "Sonne". Bei "Zeit" oder "Keine" hat diese Einstellung keine Auswirkung.