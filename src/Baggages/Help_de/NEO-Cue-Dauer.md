# Cue Dauer

Gibt an, wie lange diese Cue abgespielt wird (in Sekunden).

- **0** — Die Cue läuft unbegrenzt bis ein Stop-KO empfangen wird.
- **1–65534** — Die Cue stoppt nach der angegebenen Zeit und wechselt zur nächsten Cue.
- **65535** — Die Cue wartet, bis der Effekt fertig ist (z. B. Wipe mit Loop = Aus). Wird das Segment während dieser Cue aus- und wieder eingeschaltet, startet der Effekt neu und die Cue wartet erneut auf sein Ende; lief beim Ausschalten bereits die Übergangszeit zur nächsten Cue, wird diese nach dem Einschalten fortgesetzt.

**Übergangszeit:** Legt fest, wie lange das Überblenden zur nächsten Cue dauert (in Millisekunden).
