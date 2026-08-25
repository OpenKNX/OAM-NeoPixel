# Effekt Typ

Wählt den Lichteffekt für diese Szene aus.

## Weniger Effekte auf manchen Geräten

Auf Geräten mit knappem Programmspeicher liefert die Firmware die **2D-Effekte nicht mit** - sie hätten dort keinen Platz. Die ETS-Auswahl bietet deshalb von vornherein nur die Effekte an, die das Gerät wirklich darstellen kann.

Betroffen sind derzeit die **ESP32-C6-Varianten** (KNeoPix und Mini). Alle anderen Geräte zeigen die vollständige Liste.

Welche Hardware das betrifft, entsteht beim Erzeugen der Applikation automatisch aus den Firmware-Einstellungen - es muss nichts von Hand gepflegt werden.

Ein Wechsel der Hardware-Auswahl **verliert nichts**: beide Listen benutzen dieselben Werte und dasselbe Speicherbyte. Stand dort ein 2D-Effekt und du wechselst auf ein Gerät ohne 2D, verschwindet er nur aus der Auswahl - das Gerät spielt dann ersatzweise **Solid**.
