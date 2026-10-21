### Ausgangslogik

Legt fest, wie der GPIO-Pegel auf den Schaltzustand abgebildet wird.

**normal (HIGH = EIN):** Bei EIN liegt der Ausgang auf HIGH. Standard für die meisten Relaismodule mit High-Pegel-Ansteuerung.

**invertiert (LOW = EIN):** Bei EIN liegt der Ausgang auf LOW. Nötig für Low-aktive Relaismodule (sehr häufig bei günstigen Modulen).

Bei der Initialisierung der Firmware wird das Relais in beiden Fällen ausgeschaltet; nach der Startverzögerung wird der gespeicherte Zustand wiederhergestellt, sofern gültige gespeicherte Daten vorliegen. Ein Relais-Objekt, das während der Startverzögerung eintrifft, schaltet erst danach.
