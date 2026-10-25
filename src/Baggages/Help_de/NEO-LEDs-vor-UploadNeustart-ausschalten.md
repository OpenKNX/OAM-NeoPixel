# LEDs vor Upload/Neustart ausschalten

Diese Einstellung bestimmt, ob alle LEDs vor einem ETS-Upload oder Geräteneustart automatisch ausgeschaltet werden.

**Empfehlung:** Diese Option sollte normalerweise aktiviert bleiben (Standard: Ja).

**Verfügbare Optionen:**

- **Ja (Standard)**: Alle LEDs werden vor dem Upload/Neustart ausgeschaltet
- **Nein**: LEDs bleiben in ihrem aktuellen Zustand

**Anwendungsfälle:**

- **Ja**: Verhindert unerwünschte Lichteffekte während des Neustarts und schützt die Hardware
- **Nein**: Nützlich bei speziellen Szenarien, bei denen der LED-Zustand erhalten bleiben soll 

**Hinweis:** Bei aktivierter Option werden alle laufenden Effekte gestoppt und alle LEDs ausgeschaltet, bevor die Firmware neu startet (Neustart, ETS-Download, Firmware-Update über KNX oder Netzwerk). Die Segmente starten danach nach ihrem „Verhalten nach Neustart“.

Die externen Relais hängen nicht von dieser Einstellung ab: Ob ein Relais vor Upload/Neustart ausgeschaltet wird und in welchem Zustand es nach dem Neustart startet, legen die Spalten „Vor Upload/Neustart“ und „Nach Neustart“ auf der Seite „Relais“ fest. In früheren Versionen schaltete diese Option auch die Relais aus. Die Voreinstellung von „Vor Upload/Neustart“ ist jetzt „unverändert“, auch für bestehende Projekte nach der Aktualisierung der Applikation: Die Relais behalten dann vor einem ETS-Download oder Neustart ihren Zustand, auch wenn diese Option auf „Ja“ steht. Wer das alte Verhalten von „Ja“ möchte, stellt „Vor Upload/Neustart“ auf „Aus“.

Beim Update auf diese Version kann ein Relais mit „Nach Neustart = Letzter Zustand“ einmal AUS starten: Gilt der zuletzt gespeicherte Zustand noch aus der vorherigen Firmware und hatte diese die Relais vor dem Speichern ausgeschaltet (Option „Ja“: bei einem Neustart, einem ETS-Download oder einem Firmware-Update über Netzwerk), ist AUS gespeichert.
