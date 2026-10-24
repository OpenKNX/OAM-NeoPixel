### Strombegrenzung Schwellwert (%)

Prozentsatz der maximalen Stromgrenze, ab dem die automatische Helligkeitsreduzierung aktiviert wird. Ermöglicht sanfte Übergänge statt harter Grenzen.

**Gültigkeit:** Der globale Wert gilt für alle Streifen mit „Global verwenden“ (bei „Fester Wert (mA)“ für ihren gemeinsamen Faktor), der Wert am Streifen für einen Streifen mit eigener Grenze.

**Funktionsweise:**

- **Unter Schwellwert**: Keine Begrenzung, volle Helligkeit möglich
- **Über Schwellwert**: Graduelle Helligkeitsreduzierung beginnt
- **An der Grenze**: Die Helligkeit ist auf den Wert der „Auto-Helligkeitsbegrenzung“ gesunken
- **Über der Grenze**: Skalierung auf 95 % der Stromgrenze, höchstens aber auf den Wert der „Auto-Helligkeitsbegrenzung“

**Typische Werte:**

- **80%**: Frühe, sanfte Begrenzung (empfohlen)
- **90%**: Späte Begrenzung, höhere Helligkeit
- **95%**: Sehr späte Begrenzung, fast harte Grenze
- **100%**: Harte Grenze ohne Übergang: volle Helligkeit bis zur Grenze, darüber Skalierung auf 95 % der Stromgrenze; die „Auto-Helligkeitsbegrenzung“ wirkt dann nicht

**Beispiel bei 5000mA Limit:**

- **80% Schwellwert**: Begrenzung startet ab 4000mA
- Bei 4000-5000mA wird die Helligkeit linear von 100 % auf den Wert der „Auto-Helligkeitsbegrenzung“ reduziert
- Über 5000mA wird auf 4750mA (95 %) skaliert, höchstens aber auf den Wert der Auto-Helligkeitsbegrenzung

Mit der Voreinstellung 100 % für die Auto-Helligkeitsbegrenzung wird zwischen Schwellwert und Grenze nicht reduziert.

**Empfehlung:**
80-90% für beste Balance zwischen Helligkeit und sanften Übergängen. Niedrigere Werte für kritische Installationen.

