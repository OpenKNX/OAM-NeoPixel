# HCL-Quelle

Legt fest, aus welcher Quelle dieses Segment seine HCL-Farbtemperatur bezieht.

## Modi

### Deaktiviert
Für dieses Segment wird keine HCL-Farbtemperatur angewendet.

### Globalen Lichtmanager verwenden
Das Segment verwendet den in den allgemeinen NeoPixel-HCL-Einstellungen ausgewählten Lichtmanager.

### Segment-Lichtmanager verwenden
Das Segment verwendet einen eigenen Lichtmanager, der direkt in diesem Segment ausgewählt wird.

## Hinweise

- Die eigentliche HCL-Kurve wird durch OFM-LightManager erzeugt.
- Die NeoPixel-Parameter in diesem Bereich steuern nur, wie stark und auf welche Pixel die übernommene Farbtemperatur angewendet wird.
- Ohne gültige Lichtmanager-Zuordnung bleibt die HCL-Anwendung für das Segment wirkungslos.

## Empfehlung
Nutzen Sie die globale Quelle für ein einheitliches Verhalten über mehrere Segmente. Verwenden Sie einen Segment-Lichtmanager, wenn einzelne Segmente bewusst einem anderen Tageslichtverlauf folgen sollen.