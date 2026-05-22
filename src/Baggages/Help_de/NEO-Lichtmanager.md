# Lichtmanager

Wählt den Lichtmanager aus, dessen Farbtemperatur-Sollwert für die globale HCL-Anwendung des NeoPixel-Moduls verwendet wird.

## Bedeutung
Der ausgewählte Lichtmanager liefert die aktuelle Farbtemperatur. NeoPixel berechnet dabei keine eigene HCL-Kurve mehr, sondern übernimmt den Sollwert aus OFM-LightManager und mischt ihn gemäß den eingestellten NeoPixel-HCL-Parametern in die Ausgabe ein.

## Optionen
- **Kein Lichtmanager**: Es ist keine globale HCL-Quelle aktiv.
- **Lichtmanager 1..16**: Verwendet den jeweiligen Lichtmanager als globale Quelle.

## Wirkung im Projekt
- Diese Auswahl gilt für alle Segmente, deren HCL-Quelle auf **global** steht.
- Segment-spezifische Quellen bleiben davon unberührt.
- Ist der gewählte Lichtmanager global oder individuell gesperrt, wird keine HCL-Anwendung aus diesem Manager übernommen.

## Voraussetzungen
- Der OFM-LightManager muss im Gerät eingebunden und aktiviert sein.
- Die Anzahl sichtbarer Lichtmanager muss ausreichend konfiguriert sein.

## Empfehlung
Verwenden Sie hier den Lichtmanager, der die zentrale Tageslicht- oder Raumlogik für mehrere Segmente bereitstellt.