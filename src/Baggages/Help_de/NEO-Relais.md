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

- **Netzteil-Steuerung** – LED-Stromversorgung automatisch abschalten, wenn keine LEDs leuchten
- **Lüftersteuerung** – Kühlung bei Bedarf aktivieren
- **Begleitschaltungen** – weitere Geräte synchron zur LED-Steuerung schalten

---

## Wichtig: Versorgt das geschaltete Netzteil auch dieses Gerät?

Bevor Sie ein Netzteil über ein Relais schalten, prüfen Sie, **woher dieses OpenKNX-Gerät selbst seinen Strom bezieht** – das hängt von der Hardware-Variante ab:

- **Gerät wird aus demselben Netzteil gespeist** (bei vielen OpenKNX-Geräten der Fall): Schaltet das Relais dieses Netzteil ab, wird auch der Controller stromlos. Das Gerät geht offline und vom KNX-Bus – und kann sich danach **nicht selbst wieder einschalten** (das Relais ließe sich dann nur extern/manuell zurücksetzen).
- **Gerät hat eine eigene KNX-Hilfsspannung oder eine getrennte Versorgung**: Dann bleibt der Controller bei abgeschaltetem LED-Netzteil aktiv und am Bus – nur die LEDs gehen aus.

**Empfehlung:** Das Netzteil, das auch den Controller speist, nicht über ein Relais komplett abschalten – oder eine getrennte Versorgung bzw. KNX-Hilfsspannung für das OpenKNX-Gerät vorsehen.

---

## Einstellungen in ETS

**Externe Relais**
Legt fest, wie viele Relais aktiv sind. Auf **Deaktiviert** werden alle Relais und die zugehörigen Kommunikationsobjekte ausgeblendet.

**Einschaltverzögerung / Ausschaltverzögerung**
Das Relais schaltet erst nach der eingestellten Zeit (Sekunden). Bei 0 wird sofort geschaltet.

**Mindest-Auszeit**
Verhindert, dass das Relais zu schnell wieder eingeschaltet wird – schützt z. B. Netzteile vor schnellen Ein-/Aus-Zyklen.

---

## Kommunikationsobjekte

Pro aktiviertem Relais gibt es zwei KOs:

- **Relais n** – Ein/Aus schalten
- **Relais n Status** – Rückmeldung des aktuellen Zustands (wird automatisch nach jedem Schaltvorgang gesendet)

---

## Deaktivierung

Werden keine Relais benötigt, die Auswahl einfach auf **Deaktiviert** stellen. Es entstehen dann keinerlei Einschränkungen für die übrige LED-Steuerung.

**Hinweis:** Für komplexere Automatisierungen (z. B. zeitgesteuerte oder verknüpfte Schaltungen) empfiehlt sich ein separates KNX-Schaltaktor-Modul.

