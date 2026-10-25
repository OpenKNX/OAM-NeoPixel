### Nach Neustart

Legt fest, wie das Relais nach einem Neustart, einem ETS-Download oder einer Spannungswiederkehr startet. Das Relais bleibt mindestens bis zum Ende der Startverzögerung aus; danach schaltet es in den hier gewählten Zustand (mit Mindest-Auszeit kann ein EIN später kommen, siehe unten). Das Status-Objekt wird dann mit dem Zustand des Relais belegt, ohne zu senden; gesendet wird es nur, wenn „Status nach Neustart senden“ auf „Ja“ steht, und danach bei jeder Änderung.

**Aus:** Das Relais startet ausgeschaltet.

**Ein:** Das Relais startet eingeschaltet.

**Letzter Zustand (Standard):** Das Relais startet im zuletzt gespeicherten Zustand; ohne gültige gespeicherte Daten bleibt es aus (z. B. nach dem ersten Download oder wenn sich die Zahl der Segmente geändert hat).

Ist eine Mindest-Auszeit eingestellt, schaltet ein EIN erst, wenn sie seit dem Neustart abgelaufen ist (die Zeit vor dem Neustart zählt nicht); bis dahin meldet das Status-Objekt AUS.

Gespeichert wird vor einem Neustart, zu Beginn eines ETS-Downloads und vor einem Firmware-Update, auf Geräten mit Save-Pin auch bei Spannungsausfall, und periodisch oder per Objekt, wenn das in den allgemeinen Einstellungen aktiviert ist. Innerhalb von 3 Minuten nach dem letzten Speichern wird nicht erneut gespeichert, außer vor einem Firmware-Update über das USB-Upload-Skript. Gespeichert wird der zuletzt geschaltete Zustand, bei einer laufenden Verzögerung deren Ziel; hat „Vor Upload/Neustart = Aus“ das Relais ausgeschaltet, der Zustand davor. Ein Neustart noch während der Startverzögerung behält den zuvor gespeicherten Zustand.

Ein Relais-Objekt, das während der Startverzögerung eintrifft, wird erst danach ausgeführt und übersteuert diese Einstellung.
