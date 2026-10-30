# Relais-Steuerung

Je nach OpenKNX-Hardware verfügt das NeoPixel-Gerät über bis zu 4 zusätzliche Schaltausgänge (Relais). Damit lassen sich externe Geräte – wie das LED-Netzteil oder eine Lüftung – direkt über KNX ein- und ausschalten.

---

## ⛔ WARNUNG – Lebensgefahr durch elektrischen Strom

**Diese Funktion ist ausschließlich für Elektrofachkräfte!**

Relais können zum Schalten von **230 V Wechselspannung oder anderen gefährlichen Spannungen** verwendet werden. Unsachgemäße Installation kann zu **elektrischem Schlag, schweren Verletzungen oder Tod** führen.

- Alle Arbeiten an spannungsführenden Teilen nur im **spannungsfreien Zustand** durchführen.
- Vor der Installation die Anlage **allpolig abschalten** und gegen Wiedereinschalten sichern.
- Die Installation muss den geltenden **VDE-Normen und lokalen Vorschriften** entsprechen.
- Bei Unsicherheit: **Elektrofachkraft hinzuziehen!**

---

## Wofür sind die Relais gedacht?

Typische Einsatzfälle:

- **Netzteil-Steuerung** – LED-Stromversorgung über KNX ab- und zuschalten (z. B. per Logik, wenn keine LEDs leuchten)
- **Lüftersteuerung** – Kühlung bei Bedarf aktivieren
- **Begleitschaltungen** – weitere Geräte synchron zur LED-Steuerung schalten

---

## Wichtig: Versorgt das geschaltete Netzteil auch dieses Gerät?

Bevor Sie ein Netzteil über ein Relais schalten, prüfen Sie, **woher dieses OpenKNX-Gerät selbst seinen Strom bezieht** – das hängt von der Hardware-Variante ab:

- **Gerät wird aus demselben Netzteil gespeist** (bei vielen OpenKNX-Geräten der Fall): Schaltet das Relais dieses Netzteil ab, wird auch der Controller stromlos. Das Gerät geht offline und vom KNX-Bus – und kann sich danach **nicht selbst wieder einschalten** (das Relais ließe sich dann nur extern/manuell zurücksetzen).
- **Gerät hat eine eigene KNX-Hilfsspannung oder eine getrennte Versorgung**: Dann bleibt der Controller bei abgeschaltetem LED-Netzteil aktiv und am Bus – nur die LEDs gehen aus.

**Empfehlung:** Das Netzteil, das auch den Controller speist, nicht über ein Relais komplett abschalten – oder eine getrennte Versorgung bzw. KNX-Hilfsspannung für das OpenKNX-Gerät vorsehen.

Für ein Relais, das dieses Gerät versorgt, **„Vor Upload/Neustart“ auf „unverändert“** lassen (Standard). Mit „Aus“ schaltet das Gerät zu Beginn eines ETS-Downloads und vor einem Neustart seine eigene Versorgung ab. Nach jedem Neustart ist das Relais mindestens bis zum Ende der Startverzögerung aus, mit Mindest-Auszeit kann ein EIN später kommen; diesen Zeitraum muss die Beschaltung überbrücken.

---

## Einstellungen in ETS

**Externe Relais**
Legt fest, wie viele Relais aktiv sind. Auf **Deaktiviert** werden alle Relais und die zugehörigen Kommunikationsobjekte ausgeblendet.

**Einschaltverzögerung / Ausschaltverzögerung**
Das Relais schaltet erst nach der eingestellten Zeit (Sekunden). Bei 0 wird sofort geschaltet.

**Mindest-Auszeit**
Verhindert, dass das Relais zu schnell wieder eingeschaltet wird – schützt z. B. Netzteile vor schnellen Ein-/Aus-Zyklen. Ein EIN-Telegramm innerhalb der Mindest-Auszeit wird nicht verworfen, sondern ausgeführt, sobald die Mindest-Auszeit abgelaufen ist (frühestens nach der Einschaltverzögerung). Ein AUS-Telegramm in dieser Wartezeit hebt das vorgemerkte EIN wieder auf; es gilt immer das zuletzt empfangene Telegramm.

**Vor Upload/Neustart**
**unverändert** (Standard) lässt das Relais bis zum Neustart im aktuellen Zustand; **Aus** schaltet es zu Beginn eines ETS-Downloads und vor einem Neustart über ETS, Konsole oder Netzwerkeinstellungen aus, ebenso vor dem Neustart nach einem Firmware-Update über KNX oder Netzwerk (nicht beim USB-Upload-Skript). Beide gelten unabhängig von „LEDs vor Upload/Neustart ausschalten“.

**Nach Neustart**
Zustand nach dem Ende der Startverzögerung: **Aus**, **Ein** oder **Letzter Zustand** (Standard; der gespeicherte Zustand, ohne gültige Daten aus). Hat „Vor Upload/Neustart = Aus“ das Relais ausgeschaltet, wird der Zustand davor gespeichert, nicht dieses Ausschalten; ein per Objekt ausgeschaltetes Relais wird als AUS gespeichert. Nach jedem Neustart ist das Relais mindestens bis zum Ende der Startverzögerung aus; eine Mindest-Auszeit zählt ab dem Neustart, auch für ein EIN-Telegramm, und kann ein EIN später kommen lassen.

---

## Kommunikationsobjekte

Die Relais schalten über ihr Kommunikationsobjekt. Das Gerät schaltet sie nicht selbstständig zusammen mit den LEDs; selbst schaltet es sie nur vor einem Neustart oder ETS-Download („Vor Upload/Neustart“) und nach dem Neustart („Nach Neustart“).

Pro aktiviertem Relais gibt es zwei KOs:

- **Relais n** – Ein/Aus schalten
- **Relais n Status** – Rückmeldung des tatsächlichen Schaltzustands (wird nach jedem Schaltvorgang gesendet; ein verzögertes EIN meldet erst EIN, wenn das Relais geschaltet hat)

---

## Deaktivierung

Werden keine Relais benötigt, die Auswahl einfach auf **Deaktiviert** stellen. Es entstehen dann keinerlei Einschränkungen für die übrige LED-Steuerung.

**Reservierte Pins:** Ein manuell gewählter GPIO, den das Gerät für KNX (TX/RX), den Save-Pin, seine Taster, seine Status-LEDs, den Flash-Speicher des ESP32 (z. B. klassisch GPIO 6–11, S3 26–32 und bei Octal-Speicher 33–37, C3 12–17, C6 24–26 und 28–30) oder bei Ethernet-Boards (QuinLED-Ethernet-Varianten, Gledopto GL-C-620WL) die Ethernet-Schnittstelle benutzt, wird abgelehnt: das Relais bleibt ohne Ausgang (Log: „REFUSED“).

**Hinweis:** Für komplexere Automatisierungen (z. B. zeitgesteuerte oder verknüpfte Schaltungen) empfiehlt sich ein separates KNX-Schaltaktor-Modul.

