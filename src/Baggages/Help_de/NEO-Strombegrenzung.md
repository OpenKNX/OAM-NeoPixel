### Strombegrenzung

Automatische Strombegrenzung (Automatic Brightness Limiting - ABL) zum Schutz der Stromversorgung und zur Vermeidung von Überlastung.

**Warum Strombegrenzung?** 
- **Schutz der Stromversorgung**: Verhindert Überlastung des Netzteils 
- **Stabilität**: Vermeidet Spannungseinbrüche bei hoher Last
- **Sicherheit**: Reduziert Brandrisiko durch Überhitzung 
- **Compliance**: Einhaltung von Stromgrenzen bei Installationen


## Netzteil-Dimensionierung

Die benötigte Netzteilleistung berechnet sich wie folgt:

**Leistung (Watt) = (Maximaler Strom in mA × 5V) ÷ 1000**

**Beispiel:** Eingestellter Maximalstrom: 8000 mA → Berechnung: (8000 × 5) ÷ 1000 = 40 Watt → **Empfehlung: 40W + 20% Reserve = mindestens 48W Netzteil**

**Hinweis:** 
Der berechnete Stromverbrauch wird über das KO "Stromverbrauch Gesamt" in Echtzeit übermittelt und kann zur Überwachung genutzt werden.
---

## Verfügbare Modi:

### Deaktiviert
Strombegrenzung ist ausgeschaltet. Keine automatische Helligkeitsreduzierung.

**Verwendung nur wenn:** Professionelle Installation mit überdimensioniertem Netzteil ODER Spezielle Anwendungen die maximale Helligkeit erfordern ODER Kontrollierte Testumgebungen

⚠️ **Warnung:** Ohne Strombegrenzung besteht Überlastungsgefahr!

### Global verwenden
Nutzt die global konfigurierten Strombegrenzungs-Einstellungen.

**Eigenschaften:** Strip übernimmt alle Einstellungen von "Strombegrenzung (Global)" · Zentrale Verwaltung für mehrere Strips · Änderungen an globalen Einstellungen wirken sich auf alle Strips aus

**Empfohlen für:** Mehrere Strips an einem gemeinsamen Netzteil · Einheitliche Konfiguration über alle Strips · Vereinfachte Verwaltung

**Hinweis:** Dies ist die Standardeinstellung für physische LED-Streifen.

### Benutzerdefiniert (Fester Wert)
Ein fester Stromgrenzwert in Milliampere speziell für diesen Strip.

**Eigenschaften:** Direkter Grenzwert in mA · Klare, vorhersagbare Begrenzung · Überschreibt globale Einstellungen

**Empfohlen für:** Strips mit eigenem Netzteil · Wenn genaues Stromlimit für einzelnen Strip erforderlich ist · Unterschiedlich dimensionierte Netzteile

**Beispiel:** Strip an 3A-Netzteil → Grenzwert 2700mA (mit Sicherheitspuffer)

### Benutzerdefiniert (Pro LED)
Begrenzung basierend auf maximalem Strom pro einzelner LED.

**Eigenschaften:** Grenzwert in mA pro LED · Automatische Berechnung: LED-Anzahl × mA/LED = Gesamtlimit für diesen Strip · Passt sich automatisch an LED-Anzahl an

**Empfohlen für:** Strips mit unterschiedlichen LED-Typen und Stromverbrauch · Flexible Konfigurationen pro Strip · Wenn exakte mA/LED-Werte bekannt sind

**Beispiel:** 50 LEDs × 60mA/LED = 3000mA Limit für diesen Strip

---

## Ebenen:

### Global
Gilt für **alle physischen LED-Streifen zusammen**.

**Verwendung:** Ein Netzteil versorgt alle Streifen.

### Pro Physischem Strip
Jeder Strip kann eigene Einstellungen haben oder globale nutzen.

- **Optionen:** **Deaktiviert** (Keine Begrenzung)
- **Global verwenden** (Nutzt globale Einstellungen)
- **Fester Wert (mA)** (Eigenes Stromlimit)
- **Pro LED (automatisch)** (Eigene mA/LED-Berechnung)

**Verwendung:** Jeder Strip hat eigenes Netzteil.

---

## Funktionsweise:
Kontinuierliche Überwachung des berechneten Stromverbrauchs · Automatische Reduzierung der Helligkeit bei Überschreitung · Sanfte Übergänge ohne sichtbares Flackern

## Auswahlhilfe:
- **1 Netzteil für alles:** Global aktivieren, Strips auf "Global verwenden"
- **1 Netzteil pro Strip:** Global deaktiviert, pro Strip eigene Einstellungen
- **Gemischt:** Global für Haupt-Strips, einzelne Strips mit eigenen Einstellungen

**Empfehlung:** Strombegrenzung immer aktiviert lassen, außer in kontrollierten Testumgebungen. Die Strombegrenzung ist ein wichtiges Sicherheitsfeature.
