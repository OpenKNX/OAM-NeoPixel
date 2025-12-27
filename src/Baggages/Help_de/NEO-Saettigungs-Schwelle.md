# Sättigungs Schwelle

Legt fest, **ab welcher Farbsättigung die HCL‑Korrektur deutlich reduziert** wird („Color Preserve“).

## Werte
- Bereich: **0…255**
- Default: **64**

## Bedeutung
- **0:** HCL wirkt praktisch nur auf völlig neutrales Weiß/Grau
- **64 (empfohlen):** HCL wirkt auf Weiß und Pastellfarben, schützt aber kräftige Farben
- **255:** HCL wirkt auch auf stark gesättigte Farben (kann bunte Szenen verfälschen)

## Zusammenspiel
- **Form der Abblendung** bestimmt, *wie* die Abblendung passiert (Linear/Smooth/Gamma)
- **HCL Strength/Mix** begrenzt die maximale Wirkung zusätzlich
