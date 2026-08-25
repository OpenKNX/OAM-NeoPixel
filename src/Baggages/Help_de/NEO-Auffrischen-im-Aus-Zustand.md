# Auffrischen im Aus-Zustand

Sendet den Aus-Befehl in regelmäßigen Abständen erneut, solange der Streifen dunkel ist.

## Wofür das gut ist

Manche LED-Chips — vor allem günstige Nachbauten — starten von sich aus eine **eigene Farbshow**, wenn sie über längere Zeit keine Daten mehr sehen. Der Streifen war aus, und plötzlich läuft ein Regenbogen, den niemand angefordert hat.

Das ist kein Fehler der Steuerung: Der Chip interpretiert die Datenpause als „kein Controller angeschlossen" und fällt in seinen Demo-Modus zurück.

Ist diese Option aktiv, schickt die Firmware auch im ausgeschalteten Zustand periodisch ein Aus-Telegramm. Der Chip sieht durchgehend Daten und bleibt dunkel.

## Wann einschalten?

- Der Streifen fängt nach einiger Zeit im Aus-Zustand selbstständig an zu leuchten
- Nach einem Stromausfall oder Neustart zeigt er ein Muster, das nicht von dir kommt
- Du verwendest No-Name-Streifen unbekannter Herkunft

## Wann ausgelassen?

Bei Markenware ist das Verhalten unüblich. Lass die Option aus, solange dein Streifen im Aus-Zustand zuverlässig dunkel bleibt — sie kostet zwar wenig, aber sie erzeugt regelmäßig Datenverkehr auf der Leitung.

## Hinweise

- Die Einstellung gilt **pro Streifen**. Nur der auffällige Streifen braucht sie.
- Sie ändert **nichts** am Verhalten im eingeschalteten Zustand.
- Bleibt der Streifen trotz aktiver Option nicht dunkel, liegt es nicht am Demo-Modus — dann lohnt der Blick auf Signalqualität und Stromversorgung.
