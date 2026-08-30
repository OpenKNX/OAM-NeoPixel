# Timing

Bestimmt, welche **Bitzeiten** zum LED-Streifen gesendet werden.

> **Wichtig:** Das ist **Signal-Feintuning, keine Chip-Auswahl.** Den LED-Chip wählst du oben bei **„LED Typ"**. Dieses Feld verändert nur die Geschwindigkeit des Signals.

## Standard: „800 kHz (Standard)" = Chip-Profil

Auf diesem Wert benutzt die Firmware das **Timing-Profil des gewählten LED-Chips** — die Pulsbreiten aus dem Datenblatt, nicht einfach eine Frequenz. Für SK6812 sind das andere Werte als für WS2811 oder TM1814, auch wenn beide „800 kHz" heißen.

**Läuft dein Streifen sauber → auf diesem Wert lassen.** Das ist die einzige Einstellung, bei der alle vier Flanken innerhalb der Chip-Spezifikation liegen.

## Alle anderen Werte sind ein Experten-Override

Ein anderer Wert **streckt oder staucht das Chip-Profil** auf die gewählte Bitrate. Die Pulsverhältnisse bleiben erhalten, die absoluten Zeiten wandern mit.

Das hat einen Preis: bei niedrigen Werten wird auch die **1-Bit-Hochzeit** länger und verlässt irgendwann das Fenster des Chips. Beim SK6812 erlaubt das Datenblatt 580–1000 ns; die folgenden Werte zeigen, wie schnell der Spielraum kleiner wird:

| Einstellung | tatsächlich | 1-Bit-Hochzeit |
|---|---|---|
| Chip-Profil | 806 kHz | 800 ns |
| 790 kHz | 790 kHz | 816 ns |
| 780 kHz | 780 kHz | 826 ns |
| 750 kHz | 750 kHz | 859 ns |
| 640 kHz | 640 kHz | 1006 ns |

In der Praxis vertragen die meisten Chips das, weil sie 0 und 1 über eine Schwelle unterscheiden — aber es ist außerhalb der Spezifikation, und du solltest wissen, dass du sie verlässt.

## Wann herunter?

Niedrigere Werte geben dem Signal mehr Zeit — das hilft bei:

- **schwachem oder verschliffenem Signal** (lange oder dünne Leitungen, schwache Pegelwandler)
- **langen LED-Ketten**
- **billigen Clones**, die beim Chip-Profil flackern oder Farben verdrehen

## Wann herauf?

Höhere Werte (**840–960 kHz**) nur für Chips, die schnellere Flanken bevorzugen — etwa **WS2812C/D** als Onboard-LED.

## So findest du den richtigen Wert

1. Mit dem **Chip-Profil** starten.
2. Flackert es, oder sind Farben falsch? Schrittweise heruntergehen, bis Weiß ruhig steht **und** Rot wirklich Rot ist.
3. Nach oben nur, wenn das Chip-Profil zu langsam wirkt — das ist selten.

Mehrere Streifen dürfen unterschiedliche Werte haben.

## Was wirklich anliegt, kannst du nachsehen

In der Konsole zeigt

```
neo phys timing <Nr> info
```

die **tatsächlich erzeugte** Wellenform: T0H, T0L, T1H, T1L, Bitperiode, Latch-Zeit, Polarität und den Taktteiler. Nicht den eingestellten Wunsch, sondern das, was auf der Leitung liegt.

Das ist wichtig, weil die Beschriftung nur ein Zielwert ist. Die Hardware kann nicht jede Frequenz exakt treffen, und einige Nachbarwerte landen auf demselben Signal — **770 und 775 kHz** ergeben dasselbe, ebenso **780 und 785 kHz**. Wenn ein Schritt nichts ändert, steht dort die Erklärung.

## Hinweise

- **640 und 720 kHz** sind Notnägel für sehr schwache Signale.
- **WS2811** läuft mit **800 kHz**. Der ETS-Typ **„WS2812_400kHz"** wählt dagegen ausdrücklich das separate 400-kHz-Profil.
- Dieses Feld gilt **nur für 1-Wire-LEDs** (WS2812, SK6812 und verwandte). **SPI-LEDs** (APA102, SK9822, WS2801, LPD8806, LPD6803, P9813) takten über ihre eigene Clock-Leitung — dort erscheint stattdessen **„SPI Clock"**, und dieses Feld wird ausgeblendet.
