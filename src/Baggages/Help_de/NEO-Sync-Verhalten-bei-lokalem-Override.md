# Sync-Verhalten bei lokalem Override

Sync übernimmt immer = Telegramm vom Master überschreibt lokale Einstellungen.
Lokal hat Vorrang = lokal gesetzter Zustand bleibt bis zum nächsten Neustart.

**Sync übernimmt immer (Master übersteuert):** Jedes Telegramm des Masters wird angewendet, auch Ein/Aus. Eine lokale Änderung am Slave (Farbe, Effekt, Helligkeit, Ein/Aus) wird spätestens mit der Wiederholung nach 3 s überschrieben.

**Lokal hat Vorrang:** Solange kein lokaler Befehl kam, übernimmt der Slave Farbe, Effekt und Helligkeit mit jedem Telegramm, Ein/Aus nur bei einer Änderung am Master und beim ersten Telegramm nach einem Neustart oder einem Sync-Timeout. Ein lokales Steuer-Objekt des Slaves (alle Segment- und Effektmanager-Objekte außer Status-Objekten und „Sperre“) beendet die Übernahme bis zum nächsten Neustart.
