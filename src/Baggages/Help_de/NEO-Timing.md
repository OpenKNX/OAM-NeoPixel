# Timing (Bitrate)

Stellt die **Bitrate** der Datenübertragung zum LED-Streifen ein — also wie schnell die einzelnen Bits gesendet werden. Die Auswahl zeigt direkt die Frequenz in **kHz**.

> **Wichtig:** Das ist **Signal-Feintuning, keine Chip-Auswahl.** Den LED-Chip wählst du oben bei **„LED Typ"**. Dieses Feld korrigiert nur die *Geschwindigkeit* des Signals.

## Standard: 800 kHz

**WS2812B, SK6812, WS2813, WS2815** und die allermeisten 1-Wire-LEDs arbeiten mit **800 kHz**. Läuft dein Streifen sauber → **auf 800 lassen.**

## Wann herunter (langsamer)?

Niedrigere Werte geben dem Signal mehr Zeit — hilft bei:

- **schwachem/verschliffenem Signal** (lange oder dünne Leitungen, Verluste über Pegelwandler)
- **langen LED-Ketten**
- **billigen Clones**, die bei 800 kHz flackern oder Farben verdrehen

Der Bereich **750–790 kHz** ist bewusst in **5-kHz-Schritten** fein abgestuft — hier findest du für zickige Clones den sauberen Punkt (oft ~770–780 kHz).

## Wann herauf (schneller)?

Höhere Werte (**840–960 kHz**) nur für Chips, die **crispere/schnellere Flanken** bevorzugen — z. B. **WS2812C/D** (Onboard-LEDs), oft ~960 kHz.

## So findest du den richtigen Wert

1. Mit **800 kHz** starten.
2. Flackert es, oder sind Farben falsch (typisch bei Clones)? → in **5-kHz-Schritten herunter** (790, 785, 780, …), bis Weiß ruhig **und** Rot=Rot ist.
3. Nach oben nur, wenn 800 zu langsam wirkt (selten).

Mehrere Streifen dürfen unterschiedliche Werte haben.

## Hinweise

- **640/720 kHz** sind Notnägel für sehr schwache Signale — **nicht mit WS2811 (400 kHz) verwechseln**, das ist ein eigener „LED Typ", kein Timing.
- Dieses Feld gilt **nur für 1-Wire-LEDs** (WS2812/SK6812/…). **SPI-LEDs** (APA102, SK9822, WS2801, LPD8806, LPD6803, P9813) werden über ihre eigene **Clock-Leitung** getaktet — dort erscheint stattdessen der Parameter **„SPI Clock"**, und dieses Timing-Feld wird ausgeblendet.
