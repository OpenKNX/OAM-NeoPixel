# NeoPixel — Benutzerhandbuch (KNX)

Praktische Anleitung: **„Was muss ich womit verknüpfen, um X zu erreichen?"**
Ergänzend zum Anwendungsfall-Katalog (`doc/Anwendungsfaelle-KNX.md`).

---

## 1 · Grundkonzept

- Ein Gerät steuert mehrere **Segmente** (LED-Abschnitte). Jedes Segment hat eigene KOs.
- Jedes Segment hat **zwei Steuer-Ebenen**:
  - **Direkt** = manuell/statisch: Ein/Aus, Helligkeit, Farbe, Einzel-Effekt, Szene.
  - **Effektmanager (EM)** = abgespielte **Cue-Sequenz** (Effekt+Farbe+Dauer+Fade+Text).
- Es läuft **entweder** ein EM **oder** die Direkt-Ebene. Merksätze:
  - Ein direktes **Farb-/Effekt-/Szenen-KO** unterbricht einen laufenden EM.
  - **EM-Stop** bringt das Segment dorthin zurück, wo der Segment-Parameter **„Effektmanager-Stop: Rückkehr"** es festlegt: Letzter Zustand (Standard; aus, wenn nichts gesetzt war), Segment-Standardwerte oder Aus.

> Vier **Wege**, um KOs zu treiben: **(a) Taster/GA direkt**, **(b) LogicModul** (Bedingungen,
> Verzögerung, Rückkehr), **(c) Funktionsblöcke** (Blinker, CountDown/Timer, Aggregator),
> **(d) Lichtmanager** (HCL/adaptiv, Tag/Nacht).

---

## 2 · Erste Schritte (ETS)

1. **Hardware/Streifen einrichten:** LED-Typ (WS2812B / SK6812 RGBW / WWCW …), LED-Anzahl, Farbreihenfolge je physischem Streifen.
2. **Segment(e) anlegen:** Start-LED + Länge je Segment; optional 2D-Matrix (Breite×Höhe).
3. **KOs aktivieren:** Pro Segment nur die KOs einschalten, die du brauchst (RGB *oder* HSV *oder* Einzelkanäle …).
4. **Gruppenadressen (GA) verknüpfen:** Taster/Sensoren/Visu mit den passenden Segment-KOs verbinden (siehe Rezepte).

---

## 3 · KO-Referenz (echte ETS-Namen)

### Geräte-globale KOs (wirken auf ALLE Segmente)
| ETS-Name | DPT | Richtung | Zweck |
|---|---|---|---|
| Alle LEDs | 1.1 | Eingang | alle Segmente ein/aus (Zentral) |
| Helligkeit | 5.1 | Eingang | alle Segmente dimmen (Zentral) — **stoppt keine Show** |

### Segment-KOs
| ETS-Name | DPT | Richtung | Zweck |
|---|---|---|---|
| Segment Ein/Aus | 1.1 | Eingang | Schalten |
| Segment Helligkeit | 5.1 | Eingang | Helligkeit absolut (0–100 %) |
| Dimmen | 3.7 | Eingang | relativ heller/dunkler |
| RGB | 232.600 | Eingang | Farbe (3 Byte) |
| RGBW | 251.600 | Eingang | Farbe + Weiß |
| HSV | 232.600 | Eingang | Farbton/Sättigung/Helligkeit |
| Rot / Grün / Blau / Weiß | 5.10 | Eingang | Einzelkanal 0–255 |
| Warmweiß / Kaltweiß | 5.10 | Eingang | Tunable-White-Kanäle |
| Farbtemperatur | 7.600 | Eingang | Kelvin (WWCW) |
| …dimmen (Rot/Grün/Blau/Weiß/Warmweiß/Kaltweiß/Farbton/Sättigung/Helligkeit (Value)) | 3.7 | Eingang | Einzelkanal relativ |
| RGB dimmen / HSV dimmen | 232.600 | Eingang | Farbe relativ |
| RGBW dimmen | 251.600 | Eingang | Farbe + Weiß relativ |
| Effekt | 5.10 | Eingang | Effekt wählen (Effektnummer 0–32); eine Nummer, die die Firmware nicht enthält, läuft als „Solid" |
| Effekt schalten | 3.7 | Eingang | nächster/vorheriger Effekt (DPT 3.7: hoch=nächster, runter=vorheriger), überspringt nicht gebaute Effekte |
| Cue +/- | 3.7 | Eingang | nächste/vorherige Cue des laufenden EM (ohne Auto-Timer-Sprung) |
| Sperre | 1.1 | Eingang | 1 = alle steuernden KOs dieses Segments sperren, 0 = frei |
| Szenensteuerung | 18.1 | Eingang | Szene abrufen / speichern (Lern-Bit) |
| …Status (Ein/Aus, Helligkeit, RGB, HSV, RGBW, Effekt, Farbtemp., Szene) | je DPT | Ausgang | Rückmeldung für Visu |
| HCL Zustand | 7.600 | Ausgang | Farbtemperatur des Lichtmanagers für dieses Segment |
| Effektkette Sync | 16.1 | intern | Multi-Device-Synchronisation |

### Effektmanager-KOs (pro Segment)
| ETS-Name | DPT | Richtung | Zweck |
|---|---|---|---|
| Effektmanager Start | 5.10 | Eingang | Show starten (Wert 1–16; **0 = Stop**) |
| Effektmanager Stop | 1.1 | Eingang | Show stoppen → zurück auf Direkt-Ebene |
| Effektmanager Pause/Resume | 1.1 | Eingang | 1 = Pause (Bild eingefroren), 0 = weiter |
| Cue setzen | 5.10 | Eingang | in der Show zu Cue 1–10 springen |
| Effekt-Text | 16.1 | Eingang | Anzeigetext setzen (Laufschrift) |
| Effekt-Text Anhängen | 16.1 | Eingang | Text ergänzen |
| Effektmanager Status | 5.10 | Ausgang | welcher EM läuft (0 = keiner) |
| Aktive Cue | 5.10 | Ausgang | aktuelle Cue-Nummer |
| Effektmanager Zustand | 5.10 | Ausgang | 0 = gestoppt, 1 = läuft, 2 = pausiert |
| Effekt-Text Status | 16.1 | Ausgang | aktueller Text |

---

## 4 · Rezepte (Schritt für Schritt)

Jedes Rezept: **Ziel → KO(s) → Verknüpfung → Ergebnis.**

### R1 · Licht ein/aus + dimmen (1 Taster)
- KOs: **Segment Ein/Aus** (1.1), **Dimmen** (3.7).
- Taster „Dimmen" (1-Tasten/2-Tasten): kurz → *Segment Ein/Aus*, lang → *Dimmen*.
- Ergebnis: Streifen schaltet und dimmt wie ein KNX-Dimmaktor.

### R2 · Helligkeit auf festen Wert
- KO: **Segment Helligkeit** (5.1).
- Taster „Wert senden" (z. B. 50 %) oder Visu-Schieberegler → *Segment Helligkeit*.

### R3 · Farbe setzen
- KO: **RGB** (232.600) oder **HSV**; für RGBW-Streifen **RGBW** (251.600).
- Visu-Farbrad → *RGB*. Für feste Farbe per Taster: **Szene** (R10) oder Logik mit festem RGB-Wert.

### R4 · Weiß / Tunable White (WWCW)
- Nur Weiß (SK6812): **Weiß** (5.10) auf 255, RGB auf 0 → reines Weiß; **Segment Helligkeit** dimmt.
- Warm/Kalt (WWCW): **Farbtemperatur** (7.600, Kelvin) – ein Wert-Taster pro Stufe, oder Visu/Lichtmanager (R15).

### R5 · Einen bestimmten Effekt anwerfen
- KO: **Effekt** (5.10).
- Taster „Wert senden": Wert = Effektnummer (siehe Effektliste) → *Effekt*. Mehrere Taster = mehrere Effekte.
- Nicht jede Firmware enthält jeden Effekt (z. B. ohne 2D-Effekte). Die ETS meldet am Segment, an der Szene und an der Cue „Effekt nicht verfügbar", wenn die Firmware der gewählten Hardware den Effekt nicht hat; das Gerät zeigt dann „Solid".

### R6 · Effekte durchschalten (nächster/vorheriger)
- **Effekt schalten** (3.7): hoch = nächster, runter = vorheriger Effekt — ein „Dimmen"-Taster auf dieses KO steppt durch die Effektliste; Effekte, die die Firmware nicht enthält, werden übersprungen.

### R7 · Effekt-Show (EM) starten / stoppen
- KO: **Effektmanager Start** (5.10).
- Taster „Wert senden" N (= EM-Nummer) → Show startet. **Wert 0** (oder **Effektmanager Stop**) → Show endet, Segment kehrt zum direkten Licht zurück.
- Toggle-Taster (zwei Werte N/0) für „ein Taster startet/stoppt".

### R8 · Show pausieren / fortsetzen
- KO: **Effektmanager Pause/Resume** (1.1). 1 = einfrieren (Bild bleibt), 0 = weiter.

### R9 · In der Show zu einem Cue springen
- KO: **Cue setzen** (5.10). Wert 1–10 = gewünschte Cue.

### R10 · Szene abrufen und speichern
- KO: **Szenensteuerung** (18.1).
- Abrufen: Szenen-Taster (DPT 18) → Look erscheint (stoppt einen laufenden EM).
- Speichern: Szenen-Taster mit **Lern-Bit** → aktueller Zustand wird auf den Szenen-Platz gelernt.
- **Szene startet eine Show:** In ETS kann der Szenen-Effekttyp auf **„Effektmanager N starten"** gesetzt werden — dann ruft diese Szene den EM auf (Farben/Helligkeit entfallen, Steuerung über dessen KOs). So geht „Szene = Show" ganz ohne Logik.

### R11 · Text / Laufschrift anzeigen (Matrix)
- KOs: **Effekt-Text** (16.1), optional **Effekt-Text Anhängen**; Effekt = „Laufschrift" (19).
- Visu/Logik sendet den Text-String → *Effekt-Text*. Er bleibt erhalten, bis ein neuer kommt.

### R12 · Trigger → Animation → Rückkehr (z. B. Klingel) — via LogicModul
**Ziel:** Klingel löst eine Animation aus, danach zurück zum vorherigen Licht.
1. In ETS einen **Effektmanager** als „Notification" anlegen (z. B. EM 16 = „3× rot blinken", **Loop = Nein**).
2. Im **LogicModul**: Eingang = Klingel-GA → Ausgang sendet **Wert 16** auf **Effektmanager Start**.
3. Da der EM mit *Loop = Nein* nach der letzten Cue endet, kehrt das Segment **automatisch** zurück (Ziel laut „Effektmanager-Stop: Rückkehr").
- Variante „feste Farbe": Logik sendet **RGB** (rot) → unterbricht EM; eine zweite Logik-Aktion (verzögert) sendet die alte Farbe/Szene zurück.
- Die Rückkehr geht auf die **Direkt-Ebene**, nicht zu einer vorher laufenden Show (offener Punkt in `doc/TODO.md`).

### R13 · Bewegung/Präsenz → Licht (Treppenlicht) — via Funktionsblock
- Einfach: Melder-GA → **Segment Ein/Aus**.
- Mit Nachlaufzeit (Treppenhaus): **Funktionsblock „CountDown/Timer"** — Eingang = Melder, Ausgang (nach Ablauf = 0) → **Segment Ein/Aus**.

### R14 · Notification-Blinken — via Funktionsblock
- **Funktionsblock „Blinker"** (Anzahl, Ein-/Aus-Zeit) — Ausgang → **Segment Ein/Aus** (Segment blinkt) oder kombiniert mit fester Farbe über **RGB**.

### R15 · Tagesabhängiges Weißlicht (HCL) — via Lichtmanager
- Im **Lichtmanager** einen Kanal konfigurieren (HCL-Kurve, Tag/Nacht, optional Umgebungslicht-Lux).
- Segment-Parameter **„Segment-Lichtmanager"** auf diesen LM-Kanal setzen → Farbtemperatur/Helligkeit folgen automatisch dem Tagesverlauf.
- Status: **Farbtemperatur Status**, **HCL Zustand**.

### R16 · Zentralfunktion (Zentral-Aus / Zentral-Dimmen)
- Einfach: das geräte-globale **Alle LEDs** (1.1) bzw. **Helligkeit** (5.1) verknüpfen → wirkt auf alle Segmente (dimmt Shows mit, stoppt sie nicht).
- Alternativ: eine gemeinsame GA auf die **Segment Ein/Aus** / **Segment Helligkeit** mehrerer Segmente.

### R17 · Mehrere Geräte synchron (Effektkette)
- Effektkette im Segment aktivieren; die **Effektkette Sync**-KOs der Geräte über eine GA verbinden → ein Effekt läuft über mehrere Geräte/Streifen.

### R18 · Netzteil/Lüfter schalten (Relais)
- Relais-KO (1.1) mit der Last verknüpfen; in ETS Ein-/Aus-Verzögerung und Mindest-Aus-Zeit setzen.

### R19 · Verhalten nach Neustart
- Segment-Parameter **„Verhalten nach Neustart"**: Global verwenden / Aus / Letzter Zustand / An (ETS-Werte).
- Segment-Parameter **„Effektmanager nach Geräteneustart"** → nach dem Booten startet automatisch der gewählte EM; **„Effektmanager beim Einschalten"** startet ihn beim Einschalten des Segments.

---

## 5 · Status-KOs für die Visualisierung
Für Rückmeldungen die **…Status**-Objekte (Ausgang) verknüpfen: *Segment Ein/Aus Status*,
*Segment Helligkeit Status*, *RGB/HSV/Farbtemperatur Status*, *Effekt Status*, *Szenensteuerung
Status*, sowie **Effektmanager Status** (welcher EM), **Aktive Cue**, **Effektmanager Zustand**
(0/1/2) und **Effekt-Text Status**.

---

## 6 · Tipps & Fallstricke
- **Nur ein Farbmodell pro Segment verknüpfen** (RGB *oder* HSV *oder* Einzelkanäle), sonst überschreiben sich Telegramme.
- **EM vs. Direkt:** Während eine Show läuft, holt ein Farb-/Effekt-/Szenen-KO das Segment sofort unter direkte Kontrolle. **EM-Stop** = zurück zum manuellen Licht.
- **Helligkeit + laufende Show:** Helligkeits-Telegramme (**Segment Helligkeit** / **Dimmen** *und* das globale **Helligkeit**) **dimmen** eine laufende Show, ohne sie zu stoppen. Nur **Farbe/Effekt/Szene** holen das Segment unter direkte Kontrolle.
- **Zentral schalten/dimmen:** entweder das geräte-globale **Alle LEDs** / **Helligkeit**, oder eine gemeinsame GA auf den Segment-KOs mehrerer Segmente.
- **„Effekt schalten" (FxRel)** und **„Cue +/-"** steppen relativ (DPT 3.7) — ein Taster für nächster/vorheriger Effekt bzw. Cue. **„Sperre"** (DPT 1) blockt alle steuernden KOs des Segments.
- **Nach ETS-Update:** siehe `doc/Applikationsbeschreibung.md`, „Hinweise zu Firmware- und ETS-Updates", und die Migrationshinweise in `CHANGELOG.md`.
