# Effektmanager beim Einschalten

Startet bei jeder Flanke **Segment AUS → EIN** den ausgewählten Effektmanager frisch ab Cue 1. Die Flanke entsteht durch „Segment Ein/Aus“ = EIN, durch einen Wert größer 0 auf „Segment Helligkeit“ oder durch Heller-Dimmen auf einem ausgeschalteten Segment. Ein EIN auf ein bereits eingeschaltetes Segment startet ihn nicht neu.

Ein zuvor durch Segment AUS pausierter Lauf wird dabei verworfen. Ist „Alle LEDs“ AUS, wird der Start bis „Alle LEDs“ EIN aufgeschoben, damit Cue-Zeit und Effektposition nicht unsichtbar weiterlaufen. Lässt sich der Effektmanager nicht starten (deaktiviert, suspendiert, keine Cues), schaltet das Segment normal ein.

Ist **Keiner** gewählt, setzt EIN auf ein ausgeschaltetes Segment einen pausierten Effektmanager an derselben Cue- und Fade-Position fort, auch einen manuell pausierten; sonst startet der Effekt eines ausgeschalteten Segments mit der eingestellten Helligkeit neu; ein bereits eingeschaltetes Segment läuft unverändert weiter, ein manuell pausierter Effektmanager bleibt pausiert. Steht die Segment-Helligkeit auf 0, schaltet EIN mit voller Helligkeit ein und setzt einen pausierten Effektmanager fort.
