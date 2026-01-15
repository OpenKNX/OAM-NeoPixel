# Verhalten nach Neustart (Global)

Diese Einstellung legt fest, welchen Zustand alle LED-Segmente nach einem Neustart oder Spannungswiederkehr standardmäßig annehmen sollen.

## Optionen

### Aus
Alle LEDs werden nach einem Neustart ausgeschaltet. Die Segmente bleiben dunkel, bis sie über den KNX-Bus oder andere Eingänge aktiviert werden.

**Anwendungsfall:** 
- Energiesparen nach Stromausfall
- Verhindert unerwünschte Beleuchtung nach Wartungsarbeiten

### Letzter Zustand
Die LEDs werden im zuletzt aktiven Zustand wiederhergestellt. Das System speichert vor jedem Neustart automatisch:
- Farbe (RGBW-Werte)
- Helligkeit
- Power-Status (Ein/Aus)

**Wichtig:** Der aktive Effekt und dessen Parameter (Speed, Intensity) werden **nicht** im Flash gespeichert. Nach einem Neustart wird immer der in ETS konfigurierte Effekt verwendet. Dies stellt sicher, dass nach einer ETS-Umprogrammierung keine alten Effekt-Einstellungen die neue Konfiguration überschreiben.

**Anwendungsfall:**
- Nahtlose Fortsetzung nach kurzzeitigen Stromausfällen
- Beibehaltung der Beleuchtungsstimmung nach Updates

**Hinweis:** Der letzte Zustand wird im Flash-Speicher gesichert und überlebt Neustarts und Stromausfälle.

### Standard-Farbe
Die LEDs starten mit einer vordefinierten Farbe und Helligkeit. 

Zusätzliche Parameter (nur sichtbar bei dieser Option):
- **Standard-Farbe R/G/B/W:** RGB-Farbwerte (0-255) und optional Weiß-Kanal
- **Standard-Helligkeit:** Helligkeit von 0-255
- **Standard-Effekt:** Optional ein Effekt (z.B. Rainbow, Cylon)

**Anwendungsfall:**
- Definierte Grundbeleuchtung nach Neustart (z.B. warmweißes Grundlicht)
- Synchronisierte Startfarbe über alle Segmente
- Signalfarbe für "System bereit" (z.B. grün nach erfolgreichem Boot)

## Segment-Override
Jedes Segment kann diese globale Einstellung überschreiben. Im Segment-Parameter "Verhalten nach Neustart" kann gewählt werden:
- **Global verwenden:** Nutzt diese globale Einstellung
- **Aus/Letzter Zustand/Standard-Farbe (eigene):** Überschreibt die globale Einstellung für dieses spezifische Segment

Dies ermöglicht z.B.:
- Global: "Letzter Zustand"
- Segment 1-3: "Global verwenden" (übernehmen letzten Zustand)
- Segment 4: "Aus" (bleibt dunkel nach Neustart, Notbeleuchtung nur bei Bedarf)
- Segment 5: "Standard-Farbe (eigene)" (startet immer mit blauem Statuslicht)

## Beispiel-Szenarien

**Wohnzimmer-Beleuchtung:**
- Global: "Letzter Zustand" 
- → Nach Stromausfall wird die gewünschte Abendstimmung wiederhergestellt

**Shop-Display:**
- Global: "Standard-Farbe" (warmweiß, 80% Helligkeit)
- → Nach Öffnung am Morgen startet immer die definierte Grundbeleuchtung

**Sicherheitsbeleuchtung:**
- Global: "Aus"
- Segment 1 (Notausgang): "Standard-Farbe (eigene)" mit grün
- → Normale Bereiche dunkel, Sicherheitsbeleuchtung immer aktiv
