# Weiß-Berechnung

Bestimmt, ob RGB-Farben den **dedizierten Weißkanal** deines Streifens mitbenutzen.

Dieses Feld erscheint nur bei LED-Typen, die einen Weißkanal haben — RGBW und die 5-Kanal-Varianten mit Warm- und Kaltweiß.

## Warum es das gibt

Ein RGBW-Streifen kann Weiß auf zwei Wegen erzeugen:

- **aus Rot + Grün + Blau gemischt** — funktioniert, wirkt aber oft leicht farbstichig und braucht drei Kanäle Strom
- **über die weiße LED** — sauberer im Farbton, heller, und deutlich sparsamer

Ohne diese Einstellung bleibt der Weißkanal bei normalen Farbeffekten **dunkel**. Er lässt sich dann nur direkt ansprechen, etwa über das Weiß-Kommunikationsobjekt.

## Aus

Heutiges Verhalten: RGB bleibt RGB, der Weißkanal wird nur angesteuert, wenn du ihn ausdrücklich setzt.

Sinnvoll, wenn du Weiß bewusst getrennt steuerst oder die Farbwiedergabe deiner Effekte nicht verändern willst.

## Genau (RGB reduzieren)

Der gemeinsame Anteil wandert auf den Weißkanal und wird bei Rot, Grün und Blau **abgezogen**. Die wahrgenommene Helligkeit bleibt damit gleich, aber Weißtöne kommen aus der weißen LED statt aus der Mischung.

Sauberere Weißtöne bei **geringerem Strom**, weil eine weiße LED weniger braucht als drei farbige zusammen.

## Hinweise

- Bei **5-Kanal-Streifen** (Warm- und Kaltweiß) wirkt die Einstellung derzeit nicht — dort entscheidet die Farbtemperatur, welcher der beiden Weißkanäle leuchtet.
- Die Einstellung wirkt auf **Effekte und Farbbefehle**. Setzt du den Weißkanal direkt, bleibt dieser Wert unangetastet.
- Sie gilt **pro Streifen** — ein RGBW-Streifen darf sie nutzen, ein RGB-Streifen daneben zeigt das Feld gar nicht.
