# LEDs vor Upload/Neustart ausschalten

Diese Einstellung bestimmt, ob alle LEDs vor einem ETS-Upload oder Geräteneustart automatisch ausgeschaltet werden.

**Empfehlung:** Diese Option sollte normalerweise aktiviert bleiben (Standard: Ja).

**Verfügbare Optionen:**

- **Ja (Standard)**: Alle LEDs werden vor dem Upload/Neustart ausgeschaltet
- **Nein**: LEDs bleiben in ihrem aktuellen Zustand

**Anwendungsfälle:**

- **Ja**: Verhindert unerwünschte Lichteffekte während des Neustarts und schützt die Hardware
- **Nein**: Nützlich bei speziellen Szenarien, bei denen der LED-Zustand erhalten bleiben soll 

**Hinweis:** Bei aktivierter Option werden alle laufenden Effekte gestoppt, alle LEDs und die externen Relais ausgeschaltet, bevor die Firmware neu startet (Neustart, ETS-Download, Firmware-Update über KNX oder Netzwerk). Die Relais starten danach im zuletzt gespeicherten Zustand (ohne gültige gespeicherte Daten aus): Wird nach dem Ausschalten gespeichert (Neustart, ETS-Download, Firmware-Update über Netzwerk; nicht innerhalb von 3 Minuten nach einem früheren Speichern), starten sie aus; beim Firmware-Update über KNX wird vorher gespeichert, sie starten im vorherigen Zustand. Ein Neustart oder ETS-Download noch während der Startverzögerung speichert die Relais im zuvor gespeicherten Zustand. Wann gespeichert wird, beschreibt die Hilfe zu „Verhalten nach Neustart“. Die Segmente starten nach ihrem „Verhalten nach Neustart“.
