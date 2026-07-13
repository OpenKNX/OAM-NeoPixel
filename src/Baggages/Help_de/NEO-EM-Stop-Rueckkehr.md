# Effektmanager-Stop: Rückkehr

Diese Einstellung legt fest, welchen Zustand dieses Segment annimmt, **nachdem ein Effektmanager gestoppt wurde** – sei es manuell über das KO "EM Stop", durch das Beenden einer Cue-Kette, oder weil ein anderer Effektmanager übernimmt.

## Optionen

### Letzter Zustand (Standard)
Das Segment kehrt zu dem Effekt bzw. der Farbe zurück, die **vor dem Start des Effektmanagers** zuletzt direkt eingestellt war (z. B. eine Solid-Farbe oder ein Einzel-Effekt). War vor dem EM nichts gesetzt, bleibt das Segment im letzten gültigen Direkt-Zustand.

> **Live-Dimmen bleibt erhalten:** Eine während des Effektmanagers über das Helligkeits-KO eingestellte Helligkeit wird in diesem Modus beibehalten – nur Effekt und Farbe kehren zum Vorzustand zurück.

**Anwendungsfall:**

- Der Effektmanager ist eine zeitlich begrenzte "Show", danach soll wieder das normale Grundlicht erscheinen.
- Taster startet eine Cue-Sequenz; nach dem Stopp leuchtet wieder die zuvor gewählte Farbe.

### Segment-Standardwerte
Das Segment wird auf die **in der ETS hinterlegten Segment-Startwerte** zurückgesetzt (Effekt, Farbe, Weiß, Helligkeit), als hätte es gerade neu gestartet. Anders als bei "Letzter Zustand" wird hier auch eine live eingestellte Helligkeit auf den ETS-Startwert zurückgesetzt.

**Anwendungsfall:**

- Definierter Grundzustand unabhängig davon, was vor dem Effektmanager lief.
- Wartungs-/Reset-Szenarien: nach jeder Show ein reproduzierbares Standardbild.

### Aus
Das Segment wird nach dem EM-Stop **ausgeschaltet** (dunkel).

**Anwendungsfall:**

- Dekorative Effekt-Shows, die nur während des Effektmanagers leuchten sollen.
- Energiesparen: nach Ende der Sequenz bleibt das Segment dunkel, bis es erneut aktiviert wird.
