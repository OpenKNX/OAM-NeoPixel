### Strombegrenzung

Automatische Strombegrenzung (Automatic Brightness Limiting - ABL) zum Schutz der Stromversorgung und zur Vermeidung von Überlastung.

**Warum Strombegrenzung?**
- **Schutz der Stromversorgung**: Verhindert Überlastung des Netzteils
- **Stabilität**: Vermeidet Spannungseinbrüche bei hoher Last
- **Sicherheit**: Reduziert Brandrisiko durch Überhitzung
- **Compliance**: Einhaltung von Stromgrenzen bei Installationen

---

## Verfügbare Modi:

### Deaktiviert
Strombegrenzung ist ausgeschaltet. Keine automatische Helligkeitsreduzierung.

**Verwendung nur wenn:**
- Professionelle Installation mit überdimensioniertem Netzteil
- Spezielle Anwendungen die maximale Helligkeit erfordern
- Kontrollierte Testumgebungen

⚠️ **Warnung:** Ohne Strombegrenzung besteht Überlastungsgefahr!

### Global
Ein gemeinsamer Stromgrenzwert für alle LED-Streifen zusammen.

**Eigenschaften:**
- Einfachste Konfiguration
- Ein Grenzwert in mA für das gesamte System
- Ideal für gemeinsame Stromversorgung aller LEDs

**Empfohlen für:**
- Standard-Installationen mit einem Netzteil
- WLED-kompatible Konfiguration
- Einfache Anwendungen

### Pro Kanal
Individuelle Stromgrenzwerte für jeden einzelnen Farbkanal (Rot, Grün, Blau).

**Eigenschaften:**
- Separate Limits pro Kanal in mA
- Bessere Kontrolle bei Multi-Kanal Installationen
- Für separate Stromversorgungen pro Kanal

**Empfohlen für:**
- Multiple Netzteile (eines pro Kanal)
- Komplexe Installationen
- Spezielle Hardware-Anforderungen

### Pro LED
Begrenzung basierend auf maximalem Strom pro einzelner LED.

**Eigenschaften:**
- Grenzwert in mA pro LED
- Automatische Skalierung bei unterschiedlichen Streifenlängen
- Unabhängig von der Anzahl der LEDs

**Empfohlen für:**
- Verschiedene LED-Streifen mit unterschiedlichen Längen
- Verschiedene LED-Typen mit unterschiedlichem Stromverbrauch
- WLED-kompatible Konfiguration
- Flexible Installationen

---

## Funktionsweise:
- Kontinuierliche Überwachung des berechneten Stromverbrauchs
- Automatische Reduzierung der Helligkeit bei Überschreitung
- Sanfte Übergänge ohne sichtbares Flackern
- WLED-kompatible Implementierung

## Auswahlhilfe:
- **Einfache Installation**: Global verwenden
- **Multiple Netzteile**: Pro Kanal verwenden  
- **Verschiedene LED-Typen**: Pro LED verwenden
- **WLED-Kompatibilität**: Global oder Pro LED empfohlen

**Empfehlung:** Strombegrenzung immer aktiviert lassen, außer in kontrollierten Testumgebungen. Die Strombegrenzung ist ein wichtiges Sicherheitsfeature.
