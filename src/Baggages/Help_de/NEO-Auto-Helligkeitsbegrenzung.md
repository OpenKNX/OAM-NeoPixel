### Auto-Helligkeitsbegrenzung

Helligkeit in Prozent, auf die ein Streifen reduziert wird, wenn sein berechneter Stromverbrauch die Stromgrenze erreicht. Wirkt nur mit einem Schwellwert unter 100 %.

**Funktionsweise:**

- **Unter dem Schwellwert**: Keine Einschränkung, volle Helligkeit
- **Zwischen Schwellwert und Grenze**: Die Helligkeit sinkt linear von 100 % auf diesen Wert
- **Über der Grenze**: Skalierung auf 95 % der Stromgrenze, höchstens aber auf diesen Wert
- **Schwellwert 100 %**: Keine Übergangszone; dieser Wert wird nicht verwendet, oberhalb der Grenze gilt die Skalierung auf 95 % der Stromgrenze
- Alle Farbkanäle werden mit demselben Faktor skaliert.

**Gültigkeit:** Der globale Wert gilt für alle Streifen mit „Global verwenden“ (bei „Fester Wert (mA)“ für ihren gemeinsamen Faktor), der Wert am Streifen für einen Streifen mit eigener Grenze.

100 % bedeutet: keine Reduzierung zwischen Schwellwert und Grenze, nur die Skalierung oberhalb der Grenze.

Bei kleinen Werten sinkt der tatsächliche Strom, während der angeforderte steigt: Heller dimmen macht den Streifen in der Übergangszone dunkler, bei 0 % ist er an und über der Grenze aus.

**Empfehlung:**
Ein Wert unter 100 % (z. B. 80 %) gibt einen gleitenden Übergang vor der Grenze statt eines Sprungs beim Überschreiten.

