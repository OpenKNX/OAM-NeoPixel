# Verhalten nach Neustart (Global)

Diese Einstellung legt fest, welchen Zustand alle LED-Segmente nach einem Neustart oder Spannungswiederkehr standardmäßig annehmen sollen.

## Optionen

### Aus
Alle LEDs werden nach einem Neustart ausgeschaltet. Die Segmente bleiben dunkel, bis sie über den KNX-Bus oder andere Eingänge aktiviert werden.

**Anwendungsfall:** 

- Energiesparen nach Stromausfall
- Verhindert unerwünschte Beleuchtung nach Wartungsarbeiten

### Letzter Zustand
Die LEDs werden im zuletzt aktiven Zustand wiederhergestellt. Gespeichert werden je Segment:

- Ein/Aus, Farbe (inkl. Weißkanäle) und Helligkeit
- der gewählte Effekt, die zuletzt abgerufene Szene und ein laufender Effektmanager

Die Parameter des in der ETS eingestellten Effekts kommen aus der ETS; ein per Objekt gewählter anderer Effekt startet mit seinen Standardwerten. Szenen und Effektmanager-Cues bringen ihre eigenen Parameter mit. Bleibt die Zahl der Segmente gleich, gilt das auch nach einem ETS-Download: Ein dort neu gewählter Effekt ist erst mit „An (ETS-Werte)“ oder einem Effekt-Befehl zu sehen, der gespeicherte Effekt startet dann mit seinen Standardwerten; ist der gespeicherte Effekt der ETS-Effekt, wirken geänderte Parameter sofort. Ändert sich die Zahl der Segmente, werden die gespeicherten Daten verworfen. Telegramme, die während der Startverzögerung eintreffen, werden unabhängig von dieser Einstellung danach in ihrer Reihenfolge ausgeführt, also nach der Wiederherstellung und dem Effektmanager nach Geräteneustart (bis zu 128 Objekt-Aktualisierungen; ein Telegramm an mehrere Segmente zählt je Segment; darüber hinaus entfallen die ältesten; je Relais zählt nur der letzte Wert). Dimm-Telegramme (Start/Stopp) aus dieser Zeit werden verworfen.

Als Ein/Aus wird gespeichert, ob das Segment sichtbar eingeschaltet war: Ein Segment, das bei „Alle LEDs“ AUS gespeichert wurde, startet ausgeschaltet. Ein ausgeschaltet gespeichertes Segment mit Effektmanager startet ausgeschaltet; der Effektmanager startet ab Cue 1 und wird sofort pausiert, das nächste EIN setzt ihn fort (sofern kein Effektmanager nach Geräteneustart oder beim Einschalten konfiguriert ist).

**Anwendungsfall:**

- Nahtlose Fortsetzung nach kurzzeitigen Stromausfällen
- Beibehaltung der Beleuchtungsstimmung nach Updates

**Hinweis:** Gespeichert wird vor einem Neustart, vor einem ETS-Download und vor einem Firmware-Update über KNX, Netzwerk oder das USB-Upload-Skript (RP2040/RP2350), auf Geräten mit Save-Pin (z. B. UP1) auch bei Spannungsausfall, und periodisch oder per Objekt, wenn das in den allgemeinen Einstellungen aktiviert ist. Innerhalb von 3 Minuten nach dem letzten Speichern seit dem Gerätestart wird nicht erneut gespeichert, außer vor einem Firmware-Update über das USB-Upload-Skript. Ohne Save-Pin wird nach einem Stromausfall der zuletzt gespeicherte Zustand wiederhergestellt. Die externen Relais starten unabhängig von dieser Einstellung nach ihrer eigenen Einstellung „Nach Neustart“ auf der Seite „Relais“; was vor Neustart, ETS-Download und Firmware-Update für sie gespeichert wird, beschreibt die Hilfe zu „Vor Upload/Neustart“ und „Nach Neustart“ dort.

### An (ETS-Werte)
Jedes Segment startet mit seiner eigenen ETS-Konfiguration: eingestellter Effekt, Start-Farbe und Start-Helligkeit des Segments.

**Anwendungsfall:**

- Definierte Grundbeleuchtung nach Neustart (z.B. warmweißes Grundlicht)
- Synchronisierte Startfarbe über alle Segmente
- Signalfarbe für "System bereit" (z.B. grün nach erfolgreichem Boot)

## Segment-Override
Jedes Segment kann diese globale Einstellung überschreiben. Im Segment-Parameter "Verhalten nach Neustart" kann gewählt werden:

- **Global verwenden:** Nutzt diese globale Einstellung
- **Aus / Letzter Zustand / An (ETS-Werte):** Überschreibt die globale Einstellung für dieses spezifische Segment

Dies ermöglicht z.B.:

- Global: "Letzter Zustand"
- Segment 1-3: "Global verwenden" (übernehmen letzten Zustand)
- Segment 4: "Aus" (bleibt dunkel nach Neustart, Notbeleuchtung nur bei Bedarf)
- Segment 5: "An (ETS-Werte)" (startet immer mit blauem Statuslicht)

## Beispiel-Szenarien

**Wohnzimmer-Beleuchtung:**

- Global: "Letzter Zustand" 
- → Nach Stromausfall wird die gewünschte Abendstimmung wiederhergestellt

**Shop-Display:**

- Global: "An (ETS-Werte)" (je Segment z. B. warmweiß, 80 % Start-Helligkeit)
- → Nach Öffnung am Morgen startet immer die definierte Grundbeleuchtung

**Sicherheitsbeleuchtung:**

- Global: "Aus"
- Segment 1 (Notausgang): "An (ETS-Werte)" mit grüner Start-Farbe
- → Normale Bereiche dunkel, Sicherheitsbeleuchtung immer aktiv
