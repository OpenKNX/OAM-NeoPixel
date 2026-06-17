# Form der Abblendung

Dieser Parameter bestimmt, **wie stark die HCL‑Korrektur bei zunehmender Farbsättigung zurückgenommen wird** („Color Preserve“).
So bleiben farbige Szenen und Effekte möglichst unverfälscht, während **weiß/neutral** weiterhin korrekt nach Farbtemperatur angepasst wird.

## Wirkung

- Niedrige Sättigung (weiß/pastell): HCL wirkt stark
- Hohe Sättigung (bunte Farben): HCL wirkt schwach bis gar nicht (je nach Schwelle/Kurve)

## Werte

- **Linear (0):** gleichmäßiger Übergang
- **Smooth (1):** weicher Übergang (empfohlen)
- **Gamma (2):** stärkere Schonung der Farben (HCL fällt schneller ab)

## Tipp
In Kombination mit **„Sättigungs Schwelle“** einstellbar:

- Höhere Schwelle → HCL greift länger auch bei Pastellfarben
- Niedrigere Schwelle → Farben werden früher geschont
