# Verhalten nach Neustart (Segment)

Diese Einstellung legt fest, welchen Zustand dieses LED-Segment nach einem Neustart oder Spannungswiederkehr annehmen soll.

## Optionen

### Global verwenden (Standard)
Das Segment übernimmt die globale Einstellung "Verhalten nach Neustart (Global)".

**Anwendungsfall:**

- Einheitliches Verhalten über alle oder mehrere Segmente
- Zentrale Konfiguration, einfache Verwaltung

**Beispiel:** 
Wenn global "Letzter Zustand" eingestellt ist, wird auch dieses Segment im letzten Zustand wiederhergestellt.

### Aus
Dieses Segment wird nach einem Neustart ausgeschaltet, unabhängig von der globalen Einstellung.

**Anwendungsfall:**

- Dekorative Beleuchtung, die nur manuell aktiviert werden soll
- Energiesparen für selten genutzte Bereiche
- Notbeleuchtung, die nur bei Bedarf eingeschaltet wird

**Beispiel:**

- Global: "Letzter Zustand"
- Segment 1-3: "Global verwenden" (starten im letzten Zustand)
- Segment 4 (Party-Effekt): "Aus" (wird nur manuell aktiviert)

### Letzter Zustand
Dieses Segment wird im zuletzt aktiven Zustand wiederhergestellt, unabhängig von der globalen Einstellung.

Das System speichert vor jedem Neustart automatisch:

- Farbe (RGBW-Werte)
- Helligkeit
- Power-Status (Ein/Aus)

**Wichtig:** Der aktive Effekt und dessen Parameter (Speed, Intensity) werden **nicht** im Flash gespeichert. Nach einem Neustart wird immer der in ETS konfigurierte Effekt verwendet. Dies stellt sicher, dass nach einer ETS-Umprogrammierung keine alten Effekt-Einstellungen die neue Konfiguration überschreiben.

**Anwendungsfall:**

- Wichtige Segmente sollen immer ihren Zustand beibehalten
- Grundbeleuchtung mit zuverlässiger Wiederherstellung

**Hinweis:** Der letzte Zustand wird im Flash-Speicher gesichert und überlebt Neustarts und Stromausfälle.

### Standard-Farbe (eigene)
Dieses Segment startet mit einer eigenen vordefinierten Farbe und Helligkeit, unabhängig von der globalen Einstellung.

Zusätzliche Parameter (nur sichtbar bei dieser Option):

- **Standard-Farbe R/G/B/W:** RGB-Farbwerte (0-255) und optional Weiß-Kanal
- **Standard-Helligkeit:** Helligkeit von 0-255
- **Standard-Effekt:** Optional ein Effekt (z.B. Rainbow, Cylon)

**Anwendungsfall:**

- Statusanzeige mit definierter Startfarbe (z.B. grün = "System bereit")
- Segment mit spezifischer Grundbeleuchtung
- Akzentbeleuchtung mit fester Farbe

**Beispiel:**

- Global: "Letzter Zustand"
- Segment 1-3: "Global verwenden" (starten im letzten Zustand)
- Segment 4 (Status-LED): "Standard-Farbe (eigene)" → grün, 50% Helligkeit, Solid-Effekt

## Priorität

Die Segment-Einstellung hat **immer Vorrang** vor der globalen Einstellung:

1. Ist "Global verwenden" gewählt → globale Einstellung wird angewendet
2. Ist eine andere Option gewählt → Segment-spezifische Einstellung wird angewendet

## Beispiel-Konfiguration

**Wohnraum mit unterschiedlichen Bereichen:**

- **Global:** "Letzter Zustand"

- **Segment 0 (Hauptbeleuchtung):** "Global verwenden"  
  → Stellt letzte Helligkeit/Farbe wieder her

- **Segment 1 (Akzentlicht):** "Global verwenden"  
  → Stellt letzte Farbe wieder her

- **Segment 2 (Effekt-Bereich):** "Aus"  
  → Bleibt dunkel, wird nur manuell für Partys aktiviert

- **Segment 3 (Status-Anzeige):** "Standard-Farbe (eigene)" → Blau, 30%  
  → Zeigt immer System-Bereitschaft mit blauem Licht

**Ergebnis nach Neustart:**

- Segment 0+1: Letzte Farbe/Helligkeit wiederhergestellt
- Segment 2: Aus
- Segment 3: Blaues Statuslicht (30%)

## Tipps

- **Standard-Einstellung belassen:** Wenn alle Segmente gleich reagieren sollen, einfach "Global verwenden" bei allen Segmenten lassen und nur die globale Einstellung ändern.

- **Sicherheitskritische Segmente:** Notbeleuchtung sollte "Standard-Farbe (eigene)" verwenden, um nach Stromausfall zuverlässig zu starten.

- **Energiesparen:** Dekorative Effekte auf "Aus" setzen und nur bei Bedarf aktivieren.

- **Flash-Speicher schonen:** Der "Letzter Zustand"-Modus schreibt bei jedem Neustart in den Flash. Bei sehr häufigen Neustarts kann "Standard-Farbe" die Lebensdauer verlängern.
