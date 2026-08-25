# Signal-Polarität

Legt fest, ob die Datenleitung **normal** oder **invertiert** angesteuert wird.

## Automatisch (Chip) — der Normalfall

Die Firmware nimmt die Vorgabe des gewählten LED-Chips. Die meisten Typen arbeiten normal; **TM1814, TM1829 und TM1914** erwarten laut Datenblatt eine invertierte Leitung und bekommen sie automatisch.

**Lass diese Einstellung stehen**, solange du keinen invertierenden Baustein zwischen Controller und Streifen hast.

## Wann umstellen?

Nur, wenn deine **Verkabelung** das Signal dreht:

- ein invertierender Pegelwandler oder Buffer
- ein Transistor als Pegelanpassung, der das Signal umkehrt
- eine Fertiglösung, bei der das Datenblatt eine invertierte Ansteuerung nennt

In diesen Fällen hebt **Invertiert** die Umkehrung wieder auf, und der Streifen sieht das richtige Signal.

## Woran du es erkennst

Ein Streifen mit falscher Polarität bleibt in aller Regel **komplett dunkel** oder zeigt wirres Dauerlicht — er reagiert nicht sinnvoll auf Farbbefehle. Ein Streifen, der grundsätzlich funktioniert und nur gelegentlich flackert, hat **kein** Polaritätsproblem; dort lohnt der Blick auf Stromversorgung und Signalqualität.

## Hinweise

- Bei **SPI-LEDs** (APA102, SK9822, WS2801, LPD8806, LPD6803, P9813) erscheint dieses Feld nicht — sie haben eine eigene Taktleitung und kennen diese Umschaltung nicht.
- Die Einstellung gilt **pro Streifen**. Unterschiedliche Streifen dürfen unterschiedliche Werte haben.
- **Normal** erzwingt die nicht-invertierte Ansteuerung auch bei einem Chip, der von sich aus invertiert wäre. Das ist nur sinnvoll, wenn deine Verkabelung die Umkehrung bereits erledigt.
