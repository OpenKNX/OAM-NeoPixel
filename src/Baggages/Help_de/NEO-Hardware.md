# Hardware

Hier wählen Sie die Hardware, auf der die NeoPixel-Firmware läuft. Danach zeigt die ETS nur die Einstellungen, die diese Hardware hat (Ports, Relais, Netzwerk).

---

## Hardware wählen

**Hardware**
Liste der Geräte, die diese Applikation unterstützt. Gespeichert wird die Kennung der Hardware (HW-ID), nicht der Listenplatz: Kommen in einer neuen Applikationsversion Geräte dazu, bleibt die Auswahl erhalten.

**Nicht gewählt**
Hardwareabhängige Einstellungen sind ausgeblendet. Nach einem Update von einer Applikation ohne diese Seite ist die Auswahl einmalig zurückgesetzt.

---

## Abgleich mit dem Gerät

Nur sichtbar, solange keine Hardware gewählt ist. Voraussetzung: Das Gerät ist programmiert (physikalische Adresse und Applikation).

**Hardware vom Gerät abfragen**
Die ETS liest die Hardware aus dem Gerät und setzt die Auswahl. Meldungen:

- **„… erkannt“**: Die Auswahl ist gesetzt.
- **„Abweichend gebaut: …“**: Die Firmware auf dem Gerät hat andere Merkmale als der Listeneintrag (z. B. WLAN auf einer Hardware, deren Firmware sonst keins hat). Es gilt das Gerät. Die Firmware-Varianten einer Hardware (KNeoPix S3: TP, IP, TP mit WLAN) gelten nicht als Abweichung.
- **„… ist dieser Applikation unbekannt“**: Die Applikation ist älter als die Firmware. Applikation aktualisieren.

Um erneut abzufragen, die Auswahl auf **Nicht gewählt** stellen.

---

## Gerätekarte

Bei gewählter Hardware zeigt die Seite das OpenKNX-Logo bzw. bei Geräten anderer Hersteller das Kennzeichen „Drittanbieter Hardware“, den Gerätenamen und eine Beschreibung; hat die Hardware eine Webseite, steht rechts daneben ein QR-Code mit Link.

**Produkt eines Drittanbieters**
Bei diesem Gerät handelt es sich um ein kommerzielles Produkt eines Drittanbieters. Zwischen OpenKNX und dem Hersteller besteht keine geschäftliche oder vertragliche Verbindung.

OpenKNX stellt ausschließlich eine kompatible Firmware zur Verfügung und übernimmt für die Hardware keine Gewährleistung oder Haftung. Gewährleistung, Support und Produktsicherheit liegen beim Hersteller. Die Verwendung erfolgt auf eigene Verantwortung.

---

## Wichtig

- Die Firmware nutzt die Ports der Streifen und die Relais nur, wenn hier die Hardware gewählt ist, für die sie gebaut ist. Sonst bleiben alle Ausgänge aus.
- Nach einem Wechsel der Hardware die Port-Auswahl der Streifen und Relais prüfen: Sie bezieht sich auf die Anschlüsse der neu gewählten Hardware.
