# Lichtmanager

Wählt den Lichtmanager aus, dessen Farbtemperatur-Sollwert für die HCL-Anwendung verwendet wird: in den allgemeinen HCL-Einstellungen für alle Segmente mit globaler Quelle, im Segment („Segment-Lichtmanager“) nur für dieses Segment.

## Bedeutung
Der ausgewählte Lichtmanager liefert die aktuelle Farbtemperatur. NeoPixel berechnet dabei keine eigene HCL-Kurve mehr, sondern übernimmt den Sollwert aus OFM-LightManager und mischt ihn gemäß den eingestellten NeoPixel-HCL-Parametern in die Ausgabe ein.

## Optionen

- **Kein Lichtmanager**: Es ist keine HCL-Quelle aktiv.
- **Lichtmanager 1..16**: Verwendet den jeweiligen Lichtmanager als Quelle.

## Wirkung im Projekt

- Die Auswahl in den allgemeinen HCL-Einstellungen gilt für alle Segmente, deren HCL-Quelle auf **global** steht.
- Segment-spezifische Quellen bleiben davon unberührt.
- Ist der gewählte Lichtmanager global oder individuell gesperrt, wird keine HCL-Anwendung aus diesem Manager übernommen.

## Voraussetzungen

- Der OFM-LightManager muss im Gerät eingebunden und aktiviert sein.
- Der gewählte Lichtmanager muss unter Lichtmanager → Kanalauswahl auf „Aktiviert“ stehen und darf nicht „Suspendiert“ sein. Sonst wird keine Farbtemperatur angewendet, das HCL-Status-Objekt meldet 0 K und das Gerät schreibt einmalig eine Warnung ins Log.
- Nach einem ETS-Update von einer älteren Applikation sind alle Lichtmanager deaktiviert und müssen in der Kanalauswahl wieder aktiviert werden; ihre Einstellungen bleiben erhalten.

## Empfehlung
Verwenden Sie hier den Lichtmanager, der die zentrale Tageslicht- oder Raumlogik für mehrere Segmente bereitstellt.