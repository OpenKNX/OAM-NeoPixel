# Aktualisierungsrate

Wie oft die Animation pro Sekunde neu berechnet und an die LEDs gesendet wird (Frames pro Sekunde, FPS). Gilt **global für den gesamten Streifen** — nicht pro Segment.

Höher = flüssiger, aber mehr Rechenlast.

**Slow (10 FPS):** Ruhige, langsame Animationen.

**Normal (20 FPS):** Standard — guter Kompromiss aus Leistung und Flüssigkeit (Voreinstellung). Ideal für ruhige/ambiente Effekte (Solid, Breathing, HCL).

**Fast (30 FPS):** Flüssige, dynamische Effekte — **empfohlen für 2D-Matrizen und Effekte mit viel Bewegung** (Matrix, Snake, Comet, Starfield …).

**Max (50 FPS):** Sehr responsiv.

**Extreme (>60 FPS):** „So schnell wie möglich" — sinnvoll nur bei kurzen Einzelstreifen. Bei vielen/langen LEDs wird die Bildrate ohnehin durch LED-Datenrate und Rechenzeit gedeckelt.

**Hinweis:** Die FPS-Angaben sind ein **Ziel**, kein garantierter Wert — die tatsächlich erreichte Bildrate ist durch die LED-Datenrate (lange Streifen) und die Rechenleistung des Controllers begrenzt. Bei größeren Konfigurationen sind real meist ~50–60 FPS möglich, unabhängig von einer höheren Einstellung. Voreinstellung ist „Normal (20 FPS)" — für animierte 2D-Effekte (Matrix, Snake …) lohnt sich „Fast (30 FPS)".
