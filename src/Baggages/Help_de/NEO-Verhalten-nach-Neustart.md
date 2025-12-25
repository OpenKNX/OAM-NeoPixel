# Verhalten nach Neustart

Diese Einstellung legt fest, welchen Zustand die LEDs nach einem Neustart oder Spannungswiederkehr annehmen sollen.

## Verfügbare Optionen

### Global verwenden (nur bei Segmenten)
Das Segment übernimmt die globale Einstellung "Verhalten nach Neustart (Global)".

### Aus
Alle LEDs werden nach einem Neustart ausgeschaltet.

**Anwendungsfall:**
- Energiesparen nach Stromausfall
- Verhindert unerwünschte Beleuchtung nach Wartungsarbeiten

### Letzter Zustand
Die LEDs werden im zuletzt aktiven Zustand wiederhergestellt. Das System speichert automatisch:
- Farbe (RGBW-Werte)
- Helligkeit
- Aktiven Effekt
- Effekt-Parameter

**Anwendungsfall:**
- Nahtlose Fortsetzung nach Stromausfall
- Beibehaltung der Beleuchtungsstimmung

**Hinweis:** Der Zustand wird im Flash-Speicher gesichert.

### Standard-Farbe
Die LEDs starten mit einer vordefinierten Farbe und Helligkeit.

**Zusätzliche Parameter (nur bei dieser Option sichtbar):**
- Standard-Farbe R/G/B/W
- Standard-Helligkeit
- Standard-Effekt

**Anwendungsfall:**
- Definierte Grundbeleuchtung nach Neustart
- Statusanzeige mit fester Farbe
- Synchronisierte Startfarbe über alle Segmente
