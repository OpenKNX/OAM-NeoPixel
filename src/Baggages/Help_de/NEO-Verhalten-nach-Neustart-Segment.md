# Verhalten nach Neustart (Segment)

Diese Einstellung legt fest, welchen Zustand dieses LED-Segment nach einem Neustart oder Spannungswiederkehr annehmen soll.

## Optionen

### Global verwenden
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

### Letzter Zustand (Standard)
Dieses Segment wird im zuletzt aktiven Zustand wiederhergestellt, unabhängig von der globalen Einstellung.

Gespeichert werden:

- Ein/Aus, Farbe (inkl. Weißkanäle) und Helligkeit
- der gewählte Effekt, die zuletzt abgerufene Szene und ein laufender Effektmanager

Die Parameter des in der ETS eingestellten Effekts kommen aus der ETS; ein per Objekt gewählter anderer Effekt startet mit seinen Standardwerten. Szenen und Effektmanager-Cues bringen ihre eigenen Parameter mit. Bleibt die Zahl der Segmente gleich, gilt das auch nach einem ETS-Download: Ein dort neu gewählter Effekt ist erst mit „An (ETS-Werte)“ oder einem Effekt-Befehl zu sehen, der gespeicherte Effekt startet dann mit seinen Standardwerten; ist der gespeicherte Effekt der ETS-Effekt, wirken geänderte Parameter sofort. Ändert sich die Zahl der Segmente, werden die gespeicherten Daten verworfen.

**Anwendungsfall:**

- Wichtige Segmente sollen immer ihren Zustand beibehalten
- Grundbeleuchtung mit zuverlässiger Wiederherstellung

**Hinweis:** Gespeichert wird vor einem Neustart, vor einem ETS-Download und vor einem Firmware-Update über KNX, Netzwerk oder das USB-Upload-Skript (RP2040/RP2350), auf Geräten mit Save-Pin (z. B. REG2) auch bei Spannungsausfall, und periodisch oder per Objekt, wenn das in den allgemeinen Einstellungen aktiviert ist. Innerhalb von 3 Minuten nach dem letzten Speichern seit dem Gerätestart wird nicht erneut gespeichert, außer vor einem Firmware-Update über das USB-Upload-Skript. Ohne Save-Pin wird nach einem Stromausfall der zuletzt gespeicherte Zustand wiederhergestellt.

### An (ETS-Werte)
Dieses Segment startet mit seiner ETS-Konfiguration, unabhängig von der globalen Einstellung: eingestellter Effekt, Start-Farbe RGB, Start-Farbe W und Start-Helligkeit.

**Anwendungsfall:**

- Statusanzeige mit definierter Startfarbe (z.B. grün = "System bereit")
- Segment mit spezifischer Grundbeleuchtung
- Akzentbeleuchtung mit fester Farbe

**Beispiel:**

- Global: "Letzter Zustand"
- Segment 1-3: "Global verwenden" (starten im letzten Zustand)
- Segment 4 (Status-LED): "An (ETS-Werte)" → grün, 50 % Start-Helligkeit, Solid-Effekt

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

- **Segment 3 (Status-Anzeige):** "An (ETS-Werte)" → Blau, 30 %  
  → Zeigt immer System-Bereitschaft mit blauem Licht

**Ergebnis nach Neustart:**

- Segment 0+1: Letzte Farbe/Helligkeit wiederhergestellt
- Segment 2: Aus
- Segment 3: Blaues Statuslicht (30%)

## Tipps

- **Einheitlich steuern:** Wenn alle Segmente gleich reagieren sollen, bei allen Segmenten "Global verwenden" wählen und nur die globale Einstellung ändern (voreingestellt ist je Segment "Letzter Zustand").

- **Sicherheitskritische Segmente:** Notbeleuchtung sollte "An (ETS-Werte)" verwenden, um nach Stromausfall zuverlässig zu starten.

- **Energiesparen:** Dekorative Effekte auf "Aus" setzen und nur bei Bedarf aktivieren.

- **Flash-Speicher:** Der Zustand wird unabhängig von dieser Einstellung gespeichert (vor einem Neustart, höchstens alle 3 Minuten); sie bestimmt nur, ob er beim Start angewendet wird.
