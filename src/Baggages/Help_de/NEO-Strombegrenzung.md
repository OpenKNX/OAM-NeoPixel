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
Bei aktiver Strombegrenzung wird der berechnete Stromverbrauch über die Objekte „Stromverbrauch Gesamt“, „Auslastung Gesamt“ und „Leistung Gesamt“ (sowie je Streifen) gemeldet. Die Werte werden alle 5 s geprüft und erst nach der Startverzögerung gesendet. Gesendet wird eine Änderung ab 50 mA bzw. 2 % der Stromgrenze (der größere Wert), ab 2 Prozentpunkten Auslastung oder ab 1 W Leistung sowie der Rückgang auf 0. Die Telegramme werden nacheinander über die Sendebegrenzung der Status-Objekte gesendet (nach einem kurzen Schub höchstens eines je 100 ms, gemeinsam mit den Status-Objekten); kann eine Prüfung nicht alle senden, beginnt die nächste Prüfung mit dem ersten noch nicht gesendeten Objekt. Lässt „Alle LEDs“ AUS alle Segmente dunkel, werden 0 mA, 0 % und 0 W gemeldet.

Solange die Strombegrenzung die Helligkeit reduziert, pulsiert die Status-LED orange; die Anzeige endet bei der nächsten Prüfung (alle 5 s) ohne Reduzierung. Sie hat Vorrang vor den anderen Warnungen (lila: gespeicherte Daten verworfen, gelb: unbekannter Effekt); eine solche Warnung wird danach wieder angezeigt.

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

**Wo:** Beim jeweiligen physischen LED-Streifen (Baum: *Physikalische LED Streifen → Streifen X → Strombegrenzung*). Die Werte hier unter „Strombegrenzung (Global)" sind nur die Standardwerte für alle Streifen mit „Global verwenden".

- **Optionen:** **Deaktiviert** (Keine Begrenzung)
- **Global verwenden** (Nutzt globale Einstellungen)
- **Fester Wert (mA)** (Eigenes Stromlimit)
- **Pro LED (automatisch)** (Eigene mA/LED-Berechnung)

**Verwendung:** Jeder Strip hat eigenes Netzteil.

---

## Funktionsweise:
Der Stromverbrauch wird bei aktiver Strombegrenzung in jedem Bild aus den Pixelwerten berechnet. Steht die Strombegrenzung auf „Deaktiviert“, wird er nur noch einmal je Sekunde für die Konsole berechnet; „Stromverbrauch Gesamt“ und „Auslastung Gesamt“ melden dann 0, die übrigen Objekte senden nicht.

In allen Modi gilt dieselbe Kurve: Bis zum Schwellwert keine Reduzierung; zwischen Schwellwert und Grenze sinkt die Helligkeit linear von 100 % auf den Wert der „Auto-Helligkeitsbegrenzung“; oberhalb der Grenze wird auf 95 % der Grenze skaliert, aber nie heller als an der Grenze. Bei Schwellwert 100 % gibt es keine Übergangszone: volle Helligkeit bis zur Grenze, darüber Skalierung auf 95 % der Stromgrenze. Mit der Voreinstellung 100 % für die Auto-Helligkeitsbegrenzung wird zwischen Schwellwert und Grenze nicht reduziert.

- **Global „Fester Wert (mA)“:** Alle Streifen mit „Global verwenden“ teilen sich die globale Stromgrenze und erhalten gemeinsam einen Faktor, berechnet mit dem globalen Schwellwert und der globalen Auto-Helligkeitsbegrenzung. Eine Reduzierung wirkt sofort; das Wiederansteigen folgt der „Anstiegszeit ABL“.
- **Global „Pro LED (automatisch)“:** Jeder Streifen mit „Global verwenden“ erhält die Grenze LED-Anzahl × mA/LED und wird einzeln begrenzt, mit dem globalen Schwellwert und der globalen Auto-Helligkeitsbegrenzung.
- **Streifen mit eigener Grenze** („Fester Wert“ oder „Pro LED“): einzeln begrenzt mit dem Schwellwert und der Auto-Helligkeitsbegrenzung des Streifens; die Begrenzung wirkt im selben Bild, ohne zeitlichen Übergang.

## Auswahlhilfe:

- **1 Netzteil für alles:** Global aktivieren, Strips auf "Global verwenden"
- **1 Netzteil pro Strip:** Global deaktiviert, pro Strip eigene Einstellungen
- **Gemischt:** Global für Haupt-Strips, einzelne Strips mit eigenen Einstellungen

**Empfehlung:** Strombegrenzung immer aktiviert lassen, außer in kontrollierten Testumgebungen. Die Strombegrenzung ist ein wichtiges Sicherheitsfeature.
