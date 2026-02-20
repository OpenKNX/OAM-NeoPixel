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

## Einstellungen in ETS

**Anzahl Relais**
Legt fest, wie viele Relais aktiv sind. Bei 0 sind alle Relais deaktiviert und die zugehörigen Kommunikationsobjekte werden ausgeblendet.

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

Werden keine Relais benötigt, die Anzahl einfach auf **0** stellen. Es entstehen dann keinerlei Einschränkungen für die übrige LED-Steuerung.

**Hinweis:** Für komplexere Automatisierungen (z. B. zeitgesteuerte oder verknüpfte Schaltungen) empfiehlt sich ein separates KNX-Schaltaktor-Modul.

