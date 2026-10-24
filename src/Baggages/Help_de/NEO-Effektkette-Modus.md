# Effektkette: Modus

Aus = Standalone. Master = sendet Sync-Telegramme mit seinem Zustand. Slave = empfaengt Sync und rendert seinen Ausschnitt.
Pro Sync-Gruppe darf nur ein Master konfiguriert sein. Der Master liegt immer am Bandanfang (Offset 0); „Mein Offset“ gilt nur für einen Slave.

**Master:** Sendet bei jeder Änderung von Ein/Aus, Effekt, Farbe, Geschwindigkeit oder Helligkeit und wiederholt den unveränderten Zustand alle 3 s (Buslast: 1 Telegramm je 3 s und Master). Änderungen werden ab dem Ende der Startverzögerung höchstens alle 300 ms gesendet; ein Effektwechsel über die Objekte „Effekt“ und „Effekt schalten“ wird sofort gesendet. Ein/Aus ist EIN, wenn das Master-Segment eingeschaltet und nicht durch „Alle LEDs“ AUS dunkel ist. Die Helligkeit ist die eingestellte Segment-Helligkeit ohne globale Helligkeit. Während ein Effektmanager auf dem Master läuft, sendet er Effekt, Farbe und Geschwindigkeit von vor dem Effektmanager weiter.

**Slave:** Übernimmt Effekt (nur bei einem Wechsel neu gestartet), Farbe, Geschwindigkeit und Helligkeit (mit seiner eigenen globalen Helligkeit) und schaltet sich mit dem Master ein und aus. Ausgeschaltet werden Effekt und Farbe nur übernommen und beim Einschalten sichtbar. Ein gesperrter Slave und ein Slave mit eigenem Effektmanager (laufend oder pausiert) ignorieren die Telegramme.

**Update:** Alle Geräte einer Effektkette gemeinsam aktualisieren. Ein Master mit älterer Firmware meldet immer AUS; ein Slave mit dieser Firmware schaltet sich danach aus. Ein Slave mit älterer Firmware startet seinen Effekt bei jedem Telegramm neu, mit dieser Firmware als Master also mindestens alle 3 s.
