### Vor Upload/Neustart

Legt fest, ob das Gerät dieses Relais ausschaltet, bevor es neu startet (Neustart über ETS oder Konsole, Firmware-Update über KNX oder Netzwerk, Neustart nach geänderten Netzwerkeinstellungen) und zu Beginn eines ETS-Downloads. Nicht ausgeschaltet wird es beim Firmware-Update über das USB-Upload-Skript und bei einem Neustart durch Watchdog oder Absturz. Die Einstellung gilt unabhängig von „LEDs vor Upload/Neustart ausschalten“.

**unverändert (Standard):** Das Relais behält seinen Schaltzustand bis zum Neustart.

**Aus:** Das Relais wird ausgeschaltet; eine laufende Ein- oder Ausschaltverzögerung entfällt. Während eines ETS-Downloads bleibt es aus, solange kein Relais-Objekt es schaltet.

**Versorgt das Relais dieses Gerät selbst** (z. B. über das LED-Netzteil), hier „unverändert“ lassen. Mit „Aus“ schaltet das Gerät zu Beginn des ETS-Downloads seine eigene Versorgung ab, und der Download bricht ab.

Beim Neustart selbst setzt die Firmware den Ausgang in beiden Fällen bei ihrer Initialisierung kurz nach dem Reset (davor bestimmt die Hardware den Pegel) auf AUS, mindestens bis zum Ende der Startverzögerung; danach gilt „Nach Neustart“ (mit Mindest-Auszeit kann ein EIN später kommen). Ein Relais, das dieses Gerät versorgt, braucht deshalb eine Beschaltung, die diesen Zeitraum überbrückt.

Für „Nach Neustart = Letzter Zustand“ wird der zuletzt geschaltete Zustand gespeichert, bei einer laufenden Verzögerung der Zustand, auf den sie schalten würde; hat „Aus“ das Relais ausgeschaltet, der Zustand davor. Bei einem ETS-Download wird zu Beginn gespeichert, der Neustart am Ende speichert nicht mehr. Innerhalb von 3 Minuten nach dem letzten Speichern wird nicht erneut gespeichert; dann gilt der zuvor gespeicherte Zustand.

Eine Änderung wirkt ab dem Neustart nach dem ETS-Download, der sie schreibt; dieser Download läuft noch mit der vorherigen Einstellung. Nach einem Firmware-Update auf diese Version ist das Gerät bis zum ersten ETS-Download mit der neuen Applikation unkonfiguriert; dieser Download schaltet die Relais nicht. Ein unkonfiguriertes Gerät (auch neu oder nach einem Master-Reset) setzt alle Hardware-Ports auf LOW: Ein Relais mit Ausgangslogik „invertiert (LOW = EIN)“ ist dann bis zum ersten Download eingeschaltet.
