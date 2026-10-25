### Ausgangslogik

Legt fest, wie der GPIO-Pegel auf den Schaltzustand abgebildet wird.

**normal (HIGH = EIN):** Bei EIN liegt der Ausgang auf HIGH. Standard für die meisten Relaismodule mit High-Pegel-Ansteuerung.

**invertiert (LOW = EIN):** Bei EIN liegt der Ausgang auf LOW. Nötig für Low-aktive Relaismodule (sehr häufig bei günstigen Modulen).

Bei der Initialisierung der konfigurierten Firmware wird das Relais in beiden Fällen ausgeschaltet (ein unkonfiguriertes Gerät setzt alle Hardware-Ports auf LOW, ein invertiertes Relais zieht dann an); nach der Startverzögerung schaltet es nach seiner Einstellung „Nach Neustart“ (Aus, Ein oder der gespeicherte Zustand, sofern gültige gespeicherte Daten vorliegen). Ein Relais-Objekt, das während der Startverzögerung eintrifft, schaltet erst danach.
