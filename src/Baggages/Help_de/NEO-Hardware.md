# Gerät

Hier wählen Sie das Gerät, auf dem die NeoPixel-Firmware läuft. Erst dann sind seine Anschlüsse und Einstellungen sichtbar (Ports, Relais, Netzwerk). Ohne Auswahl bleiben alle Anschlüsse des Geräts aus.

---

## Gerät auswählen

**Gerätetyp**
Liste der Geräte, die diese Applikation unterstützt. Gespeichert wird die Kennung des Geräts (HW-ID), nicht der Listenplatz: Kommen in einer neuen Applikationsversion Geräte dazu, bleibt die Auswahl erhalten.

**Nicht gewählt**
Geräteabhängige Einstellungen sind ausgeblendet; im Baum steht „Gerät (nicht gewählt)“.

---

## Oder automatisch erkennen

Nur sichtbar, solange kein Gerät gewählt ist.

**Vom Gerät lesen**
Liest den Gerätetyp aus dem Gerät und setzt die Auswahl. Ist der Knopf grau, fehlt die Verbindung zum Gerät:

1. Physikalische Adresse programmieren (wenn nötig: Programmiertaste drücken).
2. Links eine andere Seite anklicken, dann zurück auf diese Seite.
3. Knopf klicken.

Meldungen:

- **„… (0x…) erkannt.“**: Die Auswahl ist gesetzt.
- **„… erkannt. Abweichend gebaut: …“**: Die Firmware auf dem Gerät hat andere Merkmale als der Listeneintrag (z. B. WLAN auf einem Gerät, dessen Firmware sonst keins hat). Es gilt das Gerät. Die Firmware-Varianten eines Geräts (KNeoPix S3: TP, IP, TP mit WLAN) gelten nicht als Abweichung.
- **„… ist dieser Applikation unbekannt. Applikation aktualisieren.“**: Die Applikation ist älter als die Firmware.
- **„Keine Antwort auf die Geräteabfrage …“**: Die Firmware auf dem Gerät ist älter als die Applikation und kennt die Abfrage noch nicht, oder das Gerät ist nicht erreichbar. Firmware aktualisieren oder den Gerätetyp von Hand wählen.

Um erneut zu lesen, die Auswahl auf **Nicht gewählt** stellen.

---

## Gerätekarte

Bei gewähltem Gerät zeigt die Seite den Gerätenamen, links das Logo des Herstellers (bei OpenKNX-Geräten das OpenKNX-Logo), in der Mitte eine Beschreibung und, wenn das Gerät eine Webseite hat, rechts einen QR-Code mit „Geräteinfo“ darunter. Geräte anderer Hersteller tragen unter dem Logo den Stempel „Drittanbieter / Hardware“, unter der Karte steht ein Hinweis.

**Hardware eines Drittanbieters**
Das Gerät stammt von einem anderen Hersteller. OpenKNX ist ein Open-Source-Projekt ohne geschäftliche oder vertragliche Verbindung zum Hersteller.

OpenKNX liefert nur die Firmware und übernimmt für die Hardware keine Gewährleistung oder Haftung. Gewährleistung, Support und Produktsicherheit liegen beim Hersteller. Die Verwendung erfolgt auf eigene Verantwortung.

---

## Wichtig

- Die Firmware nutzt die Ports der Streifen und die Relais nur, wenn hier das Gerät gewählt ist, für das sie gebaut ist. Sonst bleiben alle Ausgänge aus.
- Nach einem Wechsel des Gerätetyps die Port-Auswahl der Streifen und Relais prüfen: Sie bezieht sich auf die Anschlüsse des neu gewählten Geräts.
