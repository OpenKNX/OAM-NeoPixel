# Cue Übergangszeit

Dauer des weichen Übergangs (Fade) vom Ende dieser Cue zur nächsten, in Millisekunden: die erste Hälfte blendet diese Cue aus, die zweite die nächste ein, auch beim Wechsel in einen verketteten Effektmanager.

- **0** — Harter Wechsel ohne Überblendung.
- **> 0** — Diese Cue wird über die angegebene Zeit in die nächste überblendet. Bei der letzten Cue ohne Loop und ohne Verkettung blendet nur diese Cue aus. Ein Wechsel über die Objekte „Cue setzen“ und „Cue +/-“ erfolgt ohne Überblendung.
