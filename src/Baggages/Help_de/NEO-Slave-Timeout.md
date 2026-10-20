# Slave-Timeout

Zeit nach der ein Slave die LEDs ausschaltet wenn kein Sync-Telegramm empfangen wurde. Aus = kein Timeout.

Die Zeit läuft ab dem Ende der Startverzögerung und beginnt mit jedem empfangenen Sync-Telegramm neu, auch mit einem ignorierten. Da der Master seinen Zustand alle 3 s wiederholt, löst der Timeout nur aus, wenn der Master ausfällt. Beim Timeout wird das Segment ausgeschaltet wie mit „Segment Ein/Aus“ = AUS; das nächste Telegramm eines eingeschalteten Masters schaltet es wieder ein. Ein gesperrter Slave, ein Slave mit eigenem Effektmanager und ein Slave mit lokalem Vorrang werden nicht ausgeschaltet; der Timeout greift, sobald das endet.
