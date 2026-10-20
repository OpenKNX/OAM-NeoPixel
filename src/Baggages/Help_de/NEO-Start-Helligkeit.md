# Start-Helligkeit

Helligkeit des Segments nach einem Neustart (0-255).

Sie gilt für jeden Effekt des Segments; die globale Helligkeit wirkt zusätzlich als Faktor. Bei „Verhalten nach Neustart“ = „Letzter Zustand“ wird sie durch die gespeicherte Helligkeit ersetzt, sobald gespeicherte Daten vorliegen. Über das Objekt „Segment Helligkeit“ kann sie zur Laufzeit geändert werden.

**Helligkeit und Ein/Aus (wie ein KNX-Dimmer):** „Segment Helligkeit“ = 0 % schaltet das Segment aus; die eingestellte Helligkeit bleibt erhalten und gilt beim nächsten EIN. Ein Wert größer 0 schaltet ein ausgeschaltetes Segment mit diesem Wert ein. Heller-Dimmen (Objekt „Dimmen“) schaltet ein ausgeschaltetes Segment ab der niedrigsten Stufe ein, Dunkler-Dimmen bleibt dort ohne Wirkung; Dimmen auf 0 schaltet nicht aus. Ist die eingestellte Helligkeit beim Einschalten 0, schaltet das Segment mit voller Helligkeit ein. Während „Alle LEDs“ AUS das Segment dunkel hält, wird das Einschalten nur vorgemerkt und „Dimmen“ verändert die Helligkeit nicht; ein laufender Dimmvorgang endet mit „Alle LEDs“ AUS und mit dem Ausschalten des Segments.

**Wertebereich:** 0-255

- 0 = dunkel
- 128 = 50 % Helligkeit
- 255 = maximale Helligkeit

**Standardwert:** 255
