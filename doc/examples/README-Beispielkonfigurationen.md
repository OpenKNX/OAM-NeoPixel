# Beispielkonfigurationen (Konfigurationstransfer)

Fünf importierbare Beispiele für das ETS-Produkt OpenKNX NeoPixel, von einfach bis komplex. Jedes Beispiel liegt in zwei Formen vor:

- `<beispiel>.txt` – die Transfer-Strings in Import-Reihenfolge (Zeilen mit `#` sind Kommentare)
- `<beispiel>.json` – dieselben Strings plus Einstellungen, Gruppenadress-Vorschläge und Begleitmodule, maschinenlesbar

**Konfigurator:** [index.html](index.html) erzeugt die Strings aus denselben Vorlagen mit eigenen Werten (LED-Anzahl, LED-Typ, Segmente, Matrix, Effektkette, Start-Farbe, Gruppenadress-Basis, Tasterbelegung 6- oder 8-fach). Die Datei läuft ohne Internetverbindung direkt aus dem Ordner; mit den Vorlagenwerten erzeugt sie genau die Strings dieser Seite.

## JSON-Schema (schemaVersion 1)

```text
{
  schemaVersion: 1, id, number, title, description,
  product: { name, header, testedAgainst: [{variant, appId, appVersion}], moduleVersions: {MODUL: "0x.."|"-"} },
  importOrder: [string],
  summary: [{ setting, value }],                          // Kurzübersicht (Tabelle unten)
  devices: [{                                             // ein Eintrag je KNX-Gerät
    id, name, hardware: { deviceHwId, text },
    transferStrings: [{ order, module, channel, purpose, kind: "neopixel"|"companion", string }],
    keySettings: [{ module, channel, paramName, value, valueText }],      // alle Werte der NeoPixel-Strings
    companionModules: [{ module, channel, purpose, keySettings: [...], transferString }],  // LOG / FCB / LMG
    groupAddresses: [{ ga, name, dpt, sender, reason,
                       links: [{ device, ko, koName, koFunctionText, dpt, size, flags, direction: "sendet"|"empfängt" }] }]
  }],
  checkAfterImport: [string],
  keyLayouts: { note, device, "6": [Zeile], "8": [Zeile] }   // Tasterbelegung, Zeile = { key, function,
               sends: [{ press: "kurz"|"lang", ga, dpt, value }], statusLed: { ga, dpt } | null }
}
```

`paramName` ist der Name aus dem Transfer-String (`~` = Kanalnummer, `:n` = n-te ParameterRef). `value` ist der Rohwert, wie ihn der Import schreibt (Farben als ARGB-Ganzzahl, z. B. -19349 = #FFB46B), `valueText` der ETS-Text bzw. die Farbe als #RRGGBB. `flags` sind die KO-Flags (K=C, L=R, S=W, Ü=T, A=U, I) aus der Applikation.

## Was der Konfigurationstransfer macht

Ein Transfer-String enthält die Parameter genau eines Moduls und Kanals, zum Beispiel `OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/1§...§;OpenKNX` für LED-Streifen 1. Kanal 0 sind die Basiseinstellungen des Moduls. Die Strings enthalten nur Werte, die vom Standard abweichen. **Fast alle anderen Parameter dieses Kanals werden beim Import auf den Standardwert gesetzt.** Ausnahmen: Parameter ohne festen Standardwert (z. B. die Kanalnummer eines internen Logik-Eingangs) behalten ihren Wert; Parameter mit mehreren Referenzen und unterschiedlichen Standardwerten (Szenen-Parameter, einige Logik-Schwellwerte) erhalten den Standardwert der zuletzt geschriebenen Referenz.

## Import in der ETS

1. Applikation OpenKNX NeoPixel (Release oder Dev) in das Projekt einfügen und die Parameter öffnen.
2. Seite **Konfigurationstransfer → Import** öffnen.
3. Ersten String der Liste in **Transfer-String** einfügen.
4. **Import-Ziel: „automatisch aus Export übernehmen“** wählen.
5. **Importieren** klicken und das **Import-Resultat** prüfen: `[OK]`. In der Dev-Applikation steht bei LED-Streifen und Effektmanager `[OK; Version "-" -> undefined]`; das ist ein Hinweis, kein Fehler.
6. Schritte 3–5 für jeden weiteren String in der angegebenen Reihenfolge wiederholen.
7. Die Punkte unter „Nach dem Import prüfen“ erledigen (vor allem Ports), dann die Applikation herunterladen.

**Reihenfolge:** Basiseinstellungen (Hardware, Anzahl Streifen und Segmente, Effektmanager) → LED-Streifen → Segmente → Effektmanager → Begleitmodule (Lichtmanager, Funktionsblöcke, Logik). Der Import schreibt Parameter auch dann, wenn sie gerade ausgeblendet sind; die Reihenfolge sorgt dafür, dass die Seiten nach jedem Schritt sichtbar sind und das Ergebnis direkt geprüft werden kann. Ports erst nach dem Import auswählen: ein Import der Basiseinstellungen oder eines LED-Streifens setzt die Port-Auswahl dieses Kanals zurück. Ein Wechsel des LED-Typs setzt außerdem die Takt-Ports aller acht Streifen zurück (nur für SPI-LEDs relevant).

## LED-Typen: RGB und RGBW

Die Beispiele nutzen beide Familien: Küchenzeile, Garage und Matrix-Panel WS2812B (RGB, Farbordnung GRB), Wintergarten und Terrasse SK6812 RGBW (Farbordnung GRBW). Im Konfigurator lässt sich jede Vorlage auf den anderen Typ umstellen; die Farbordnung folgt dem Typ. Sie steht im String, wenn sie vom Standard GRB abweicht (SK6812 RGBW: GRBW); fehlt sie, setzt der Import GRB, was für WS2812B richtig ist. Die ETS-Typen „SK6812“ (4), „TM1914“ (14) und „FW1906“ (15) werden nicht angeboten: bei ihnen passt die Farbordnung der ETS nicht zur Kanalzahl, mit der die Firmware rechnet.

Ein RGB-Streifen hat keine weiße LED: die Firmware verwirft den W-Anteil für solche Streifen. Weiß entsteht dort aus der RGB-Farbe (z. B. #FFB46B für Warmweiß), ein W-Wert wird nicht geschrieben. Bei RGBW kommt Weiß über W (z. B. RGB #000000 + W 255); der Farbton des Weiß hängt von der weißen LED des Streifens ab.

## Strombegrenzung

- **Ein Netzteil für alles:** global „Fester Wert (mA)“, die Streifen bleiben auf „Global verwenden“ (Küchenzeile, Wintergarten, Terrasse je Gerät, Matrix-Panel).
- **Ein Netzteil je Streifen:** global „Deaktiviert“, an jedem Streifen Strombegrenzung „Benutzerdefiniert (Fester Wert)“ mit dem Feld „Strom pro Kanal (mA)“ als Grenze des Streifens (Garage). Streifen mit eigener Grenze werden auch bei global „Deaktiviert“ begrenzt und melden ihren Strom.
- Alle Beispiele setzen die Auto-Helligkeitsbegrenzung auf 80 %: zwischen 80 % der Grenze (Schwellwert, Standard) und der Grenze sinkt die Helligkeit gleitend auf 80 %; oberhalb der Grenze wird auf 95 % der Grenze skaliert, höchstens aber auf die Auto-Helligkeitsbegrenzung. Mit 80 % erhält der Streifen bis etwa 1,19 × Grenze 80 % des angeforderten Stroms, darüber 95 % der Grenze.
- „Strom pro LED“ (global) bestimmt, wie der Strom gerechnet wird. Der Standard 60 mA passt zu WS2812B (3 × 20 mA). Die RGBW-Beispiele setzen 0: die Firmware nimmt dann die Tabelle des LED-Typs, für SK6812 RGBW 4 × 20 mA = 80 mA. Ein Wert über 0 wird ganzzahlig auf die Kanäle verteilt (70 / 4 = 17 mA, also 68 mA je RGBW-LED). Der Parameter ist in der ETS nur bei „Pro LED (automatisch)“ sichtbar; der Import setzt ihn trotzdem, und er wirkt bei „Fester Wert“ und bei global „Deaktiviert“ auf die Messung. In der Betriebsart „Pro LED (automatisch)“ würde 0 keine Grenze bedeuten; die Beispiele nutzen diese Betriebsart nicht.
- Die Grenzwerte sind Beispiele (Netzteil-Annahme jeweils in der Tabelle) und müssen an das tatsächliche Netzteil angepasst werden.
- „Stromverbrauch Gesamt“ und „Stromverbrauch“ je Streifen sind DPT 7.012 (mA).

## Grenzen

- **Gruppenadressen und KO-Flags werden nicht übertragen.** Die Vorschläge in den JSON-Dateien und unten müssen von Hand verknüpft werden. KOs, die durch den Import ausgeblendet werden, verlieren ihre Verknüpfungen.
- **Ports werden nicht übertragen.** Die Port-Auswahl ist je Hardware ein eigener Parameter, dessen Name von der Position der Hardware in der Liste abhängt; diese Position hängt davon ab, welche Hardware ein Produkt-Build enthält (die Hardwareliste wird beim Build zusammengestellt). Hardware-Auswahl (Geräte-ID) wird übertragen, die Ports werden nach dem Import gewählt.
- **Allgemeine Einstellungen** (Standort, Zeitzone, Zeitquelle) sind nicht enthalten.
- **Modulversionen:** Die Strings sind gegen Dev (0xAD01/0x12; NeoPixel-Dev-v12.knxprod, OAM 0dcdd26, OFM-NeoPixel 40a20b0) und Release (0xAD00/0x0C; NeoPixel-Release-v12.knxprod, OAM 0dcdd26, OFM-NeoPixel 40a20b0) geprüft; Modulversionen NEO 0x5, LOG 0x44, FCB 0xA, LMG 0x4. Kennt eine spätere Version einen Parameter nicht mehr, meldet der Import „Unbekannter Parameter“; diese Werte fehlen dann.
- Die Beispiele verwenden die Hardware OpenKNXiao V1.5 KNeoPix RP2350 (Geräte-ID 4882). Für andere Hardware die Auswahl ändern und die Ports neu wählen.

## 1. Einfach – Küchenzeile

Ein Streifen mit 120 LEDs unter den Hängeschränken (WS2812B), ein Segment über den ganzen Streifen. Statische Start-Farbe mit 80 % Helligkeit, nach einem Neustart wird der letzte Zustand wiederhergestellt. Zwei Szenen (Kochen / Abend) für den Taster. Ein Netzteil, globale Strombegrenzung.

| Einstellung | Wert |
|---|---|
| Hardware | OpenKNXiao V1.5 KNeoPix RP2350 (4882) |
| LED-Streifen 1 | WS2812B, Farbordnung GRB, 120 LEDs |
| Segment 1 | LED 1–120, Effekt Solid |
| Start-Farbe / Helligkeit | #FFB46B, 204 von 255 (80 %) |
| Verhalten nach Neustart | Letzter Zustand (Standardwert des Segments) |
| Szenen | 1 Kochen #FFD7A8 / 255, 2 Abend #FF8C40 / 90 |
| Strombegrenzung | Global „Fester Wert“ 3000 mA, Übergang ab 80 % der Grenze auf 80 % Helligkeit, Strom pro LED 60 mA (ein Netzteil, z. B. 5 V / 4 A) |

Transfer-Strings (Reihenfolge einhalten):

1. Basiseinstellungen: Hardware, Strombegrenzung

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/0§NeoPixelHardwareSelect=4882§NEOpowerLimitGlobal=3000§NEOpowerLimitCombined=1§NEOautoBrightnessLimit=80§;OpenKNX
    ```

2. LED-Streifen 1: Küchenzeile

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/1§Description~=K%C3%BCchenzeile§NEO~Length=120§;OpenKNX
    ```

3. Segment 1: Küchenzeile

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/1§Description~=K%C3%BCchenzeile§NEO~SegmentEnd=120§NEO~SegmentStartupColor=-19349§NEO~SegmentStartupBrightness=204§NEO~SceneCount=2§NEO~SceneAPrimaryColor=-10328§NEO~SceneBPrimaryColor=-29632§NEO~SceneBBrightness=90§;OpenKNX
    ```

Gruppenadressen (Vorschlag):

| GA | Name | DPT | KOs | Zweck |
|---|---|---|---|---|
| 1/0/0 | Küchenzeile Ein/Aus | DPST-1-1 |  KO 600 | Schalten des Segments. |
| 1/1/0 | Küchenzeile Dimmen | DPST-3-7 |  KO 625 | Relatives Dimmen der Segment-Helligkeit. |
| 1/2/0 | Küchenzeile Helligkeit | DPST-5-1 |  KO 602 | Absoluter Helligkeitswert. |
| 1/3/0 | Küchenzeile RGB | DPST-232-600 |  KO 619 | Farbe setzen (RGB, 3 Byte). |
| 1/4/0 | Küchenzeile Szene | DPST-18-1 |  KO 617 | Wert 0 = Szene 1 (Kochen), Wert 1 = Szene 2 (Abend); Bit 7 gesetzt = aktuellen Zustand als Szene speichern. |
| 1/5/0 | Küchenzeile Sperre | DPST-1-1 |  KO 641 | 1 sperrt alle steuernden KOs des Segments (z. B. Reinigung). |
| 1/6/0 | Küchenzeile Ein/Aus Status | DPST-1-11 |  KO 601 (sendet) | Rückmeldung für Taster-LED und Visualisierung. |
| 1/6/1 | Küchenzeile Helligkeit Status | DPST-5-1 |  KO 603 (sendet) | Rückmeldung der Helligkeit. |
| 1/6/2 | Küchenzeile Stromverbrauch | DPST-7-12 |  KO 404 (sendet) | Berechneter Gesamtstrom in mA (gemeldet, solange die globale Strombegrenzung aktiv ist). |

### Tasterbelegung (z. B. MDT Taster 6- oder 8-fach)

Für NeoPixel Küche. Tastenpaare sind als Wippe gedacht (links/rechts).

- **Um:** das Status-Objekt der Taste mit der Status-GA verbinden, damit die Taste den aktuellen Zustand kennt; der NeoPixel sendet den Ein/Aus-Status bei jeder Änderung, auch durch Effektmanager, Szene oder „Alle LEDs“ (Lesen-Flag gesetzt).
- **Dimmen:** das Dimm-Objekt des NeoPixel ist DPT 3.007 Start/Stopp (3 s für den ganzen Bereich); heller dimmen schaltet ein ausgeschaltetes Segment ein, auf 0 gedimmt meldet der Status Aus.
- **Cue +/−:** ein Start-Telegramm springt eine Cue weiter bzw. zurück, solange ein Effektmanager läuft oder pausiert ist; an der ersten/letzten Cue bleibt er stehen.
- **Szene:** Szenennummer n wird als Telegrammwert n − 1 gesendet; nicht angelegte Szenen werden ignoriert, eine Szene schaltet das Segment ein.
- **Sperre:** sperrt alle steuernden Objekte des Segments (auch die gerätinterne Logik); der NeoPixel hat kein Sperr-Statusobjekt, die LED kann den Sperrzustand nicht anzeigen.
- **Status-LED:** Bei Wippen für mehrere Segmente zeigt die LED den Status des ersten Segments. Ob eine Status-LED ein 1-Byte-Objekt (5.010) auswerten kann, hängt vom Taster ab.
- **MDT-Bezeichnungen:** Funktionen sind allgemein beschrieben (Schalten/Um, Dimmen relativ 3.007 mit Stopp-Telegramm beim Loslassen, Szene 18.001, Wert senden 5.001/5.010, Status-LED über Objekt); die Bezeichnungen in der MDT-Applikation können abweichen.

**6-fach**

| Taste | Funktion | Sendet an | Status-LED |
|---|---|---|---|
| 1 | Küchenzeile – Wippe links: Ein (kurz) / heller (lang) | 1/0/0 · DPST-1-1 · 1 (Ein) (kurz)<br>1/1/0 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 1/6/0 · DPST-1-11 |
| 2 | Küchenzeile – Wippe rechts: Aus (kurz) / dunkler (lang) | 1/0/0 · DPST-1-1 · 0 (Aus) (kurz)<br>1/1/0 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 1/6/0 · DPST-1-11 |
| 3 | Szene 1 „Kochen“ (kurz) | 1/4/0 · DPST-18-1 · Szene 1 (Telegrammwert 0) (kurz) | – |
| 4 | Szene 2 „Abend“ (kurz) | 1/4/0 · DPST-18-1 · Szene 2 (Telegrammwert 1) (kurz) | – |
| 5 | Küchenzeile – Um (kurz) | 1/0/0 · DPST-1-1 · Um (abwechselnd 1/0) (kurz) | 1/6/0 · DPST-1-11 |
| 6 | Sperre Um (kurz) – keine Status-LED vom NeoPixel | 1/5/0 · DPST-1-1 · Um (1 = gesperrt) (kurz) | – |

**8-fach**

| Taste | Funktion | Sendet an | Status-LED |
|---|---|---|---|
| 1 | Küchenzeile – Wippe links: Ein (kurz) / heller (lang) | 1/0/0 · DPST-1-1 · 1 (Ein) (kurz)<br>1/1/0 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 1/6/0 · DPST-1-11 |
| 2 | Küchenzeile – Wippe rechts: Aus (kurz) / dunkler (lang) | 1/0/0 · DPST-1-1 · 0 (Aus) (kurz)<br>1/1/0 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 1/6/0 · DPST-1-11 |
| 3 | Szene 1 „Kochen“ (kurz) | 1/4/0 · DPST-18-1 · Szene 1 (Telegrammwert 0) (kurz) | – |
| 4 | Szene 2 „Abend“ (kurz) | 1/4/0 · DPST-18-1 · Szene 2 (Telegrammwert 1) (kurz) | – |
| 5 | Küchenzeile – Um (kurz) | 1/0/0 · DPST-1-1 · Um (abwechselnd 1/0) (kurz) | 1/6/0 · DPST-1-11 |
| 6 | Sperre Um (kurz) – keine Status-LED vom NeoPixel | 1/5/0 · DPST-1-1 · Um (1 = gesperrt) (kurz) | – |
| 7 | Helligkeit 100 % (kurz) | 1/2/0 · DPST-5-1 · 100 % (kurz) | – |
| 8 | Helligkeit 30 % (kurz) | 1/2/0 · DPST-5-1 · 30 % (kurz) | – |

**Nach dem Import prüfen:**

- Hardware-Auswahl passend zur Firmware.
- Daten-Port von LED-Streifen 1 auswählen.
- LED-Typ und Anzahl, falls abweichend.
- Strombegrenzung an das Netzteil anpassen (Beispiel 3000 mA für 5 V / 4 A mit Reserve).
- Start-Farbe und Cue-Farben im Segment bzw. Effektmanager.

## 2. Wintergarten

Zwei Streifen (SK6812/WS2814 (RGBW)) mit 150 und 150 LEDs. Segmente: Dachkante links (Streifen 1), Dachkante rechts (erste 90 LEDs von Streifen 2) und Sitzecke (letzte 60 LEDs von Streifen 2). Die Dachkanten folgen der HCL-Farbtemperatur von Lichtmanager 1, die Sitzecke nicht. Relais 1 schaltet das gemeinsame LED-Netzteil, solange mindestens ein Segment an ist; die globale Strombegrenzung schützt dieses Netzteil.

| Einstellung | Wert |
|---|---|
| Hardware | OpenKNXiao V1.5 KNeoPix RP2350 (4882) |
| LED-Streifen 1 und 2 | SK6812/WS2814 (RGBW), Farbordnung GRBW, 150 / 150 LEDs |
| Segment 1 Dachkante links | LED 1–150, Start-Farbe #000000 + W 255, HCL global |
| Segment 2 Dachkante rechts | LED 151–240, wie Segment 1 |
| Segment 3 Sitzecke | LED 241–300, #FF9A40 + W 60, Helligkeit 180, HCL deaktiviert |
| HCL | Globaler Lichtmanager = Lichtmanager 1; LMG-Kanal 1 aktiviert, Profil 1 an allen Tagen |
| Strombegrenzung | Global „Fester Wert“ 5000 mA, Übergang ab 80 % der Grenze auf 80 % Helligkeit, Strom pro LED 0 = Tabelle des LED-Typs (RGB 3 × 20 mA, RGBW 4 × 20 mA), Streifen „Global verwenden“ (ein Netzteil) |
| Relais 1 | LED-Netzteil, Ausschaltverzögerung 30 s, Mindest-Auszeit 10 s, gesteuert von Logik 1+2 |
| Status nach Neustart senden | Ja (damit die Netzteil-Logik nach einem Neustart den Segmentzustand erhält) |

**HCL:** Die HCL-Quelle ist ein Segment-Parameter. Standard ist „Globalen Lichtmanager verwenden“; im Beispiel ist der globale Lichtmanager auf Lichtmanager 1 gesetzt und die Sitzecke auf „Deaktiviert“. Lichtmanager 1 muss in der Kanalauswahl aktiviert sein, das erledigt der LMG-String. Profil 1 gilt im Standard nur Mo–Fr; der String schaltet Sa, So, Urlaub und Feiertag dazu.

**Netzteil-Relais:** Das Relais folgt nur seinem eigenen KO. Logik 1 und 2 bilden „mindestens ein Segment an“ und schreiben das Ergebnis direkt in KO 408 (Relais 1). Sie lesen die Status-KOs über die KO-Nummer („Absolutes KO“), weil ein Telegramm, das das Gerät selbst sendet, nicht bei seinen eigenen KOs ankommt.

**Strombegrenzung:** Beide Streifen hängen an dem einen geschalteten Netzteil, deshalb eine globale Grenze für beide (Streifen „Global verwenden“).

Transfer-Strings (Reihenfolge einhalten):

1. Basiseinstellungen: Hardware, 2 Streifen, 3 Segmente, HCL, Strombegrenzung, Relais

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/0§HCLMaster=1§NeoPixelHardwareSelect=4882§NEONumberOfLEDStrips=2§NEONumberOfSegments=3§NEOpowerLimitGlobal=5000§NEOcurrentPerLED=0§NEOpowerLimitCombined=1§NEOautoBrightnessLimit=80§NEOStatusSendOnStartup=1§NEOExternalRelayCount=1§NEOExternalRelay1Name=LED-Netzteil§NEOExternalRelay1OffDelay=30§NEOExternalRelay1MinOffTime=10§;OpenKNX
    ```

2. LED-Streifen 1: Dachkante links

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/1§Description~=Dachkante%20links§NEO~LEDType=8§NEO~ColourOrder=7§NEO~Length=150§;OpenKNX
    ```

3. LED-Streifen 2: Dachkante rechts + Sitzecke

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/2§Description~=Dachkante%20rechts%20%2B%20Sitzecke§NEO~LEDType=8§NEO~ColourOrder=7§NEO~Length=150§;OpenKNX
    ```

4. Segment 1: Dachkante links

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/1§Description~=Dachkante%20links§NEO~SegmentEnd=150§NEO~SegmentStartupColor=-16777216§NEO~SegmentStartupW=255§;OpenKNX
    ```

5. Segment 2: Dachkante rechts

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/2§Description~=Dachkante%20rechts§NEO~SegmentStart=151§NEO~SegmentEnd=240§NEO~SegmentStartupColor=-16777216§NEO~SegmentStartupW=255§;OpenKNX
    ```

6. Segment 3: Sitzecke

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/3§Description~=Sitzecke§NEO~SegmentStart=241§NEO~SegmentEnd=300§NEO~SegmentStartupColor=-26048§NEO~SegmentStartupW=60§NEO~SegmentStartupBrightness=180§NEO~HCLMode=0§;OpenKNX
    ```

7. Lichtmanager 1: HCL-Kurve für die Dachkanten, an allen Tagen

    ```text
    OpenKNX,cv1,0xAD00:0xC/LMG:0x4/1§CH~Name=Wintergarten%20HCL§CH~Active=1§CH~ProfileCount=1§CH~DayNightSource=2§CH~P1_Name=Alle%20Tage§CH~P1_DaySa=1§CH~P1_DaySo=1§CH~P1_DayUrl=1§CH~P1_DayFei=1§;OpenKNX
    ```

8. Logik 1: ODER der Status-KOs Segment 1 und 2 (gerätintern über KO-Nummer)

    ```text
    OpenKNX,cv1,0xAD00:0xC/LOG:0x44/1§f~Name=Netzteil%3A%20Dachkanten%20an§f~LogicType=2§f~Logic=2§f~E1=1§f~E1OtherKO:2=601§f~E1UseOtherKO=1§f~E2=1§f~E2OtherKO:2=654§f~E2UseOtherKO=1§;OpenKNX
    ```

9. Logik 2: ODER aus Segment 3 und Logik 1, schreibt in KO 408 (Relais 1)

    ```text
    OpenKNX,cv1,0xAD00:0xC/LOG:0x44/2§f~Name=Netzteil%20Relais%201§f~LogicType=2§f~Logic=2§f~E1=1§f~E1OtherKO:2=707§f~E1UseOtherKO=1§f~I1=1§f~I1Function=1§f~OOnKOSend=1§f~OOnKOSendNumber=408§f~OOffKOSend=1§f~OOffKOSendNumber=408§;OpenKNX
    ```

Gruppenadressen (Vorschlag):

| GA | Name | DPT | KOs | Zweck |
|---|---|---|---|---|
| 2/0/0 | Dachkante Ein/Aus | DPST-1-1 |  KO 600,  KO 653 | Beide Dachkanten gemeinsam schalten. |
| 2/0/1 | Sitzecke Ein/Aus | DPST-1-1 |  KO 706 | Sitzecke getrennt schalten. |
| 2/1/0 | Dachkante Dimmen | DPST-3-7 |  KO 625,  KO 678 | Relatives Dimmen beider Dachkanten. |
| 2/1/1 | Sitzecke Dimmen | DPST-3-7 |  KO 731 | Relatives Dimmen der Sitzecke. |
| 2/3/0 | Sitzecke RGBW | DPST-251-600 |  KO 729 | Farbe inkl. Weißkanal (6 Byte). |
| 2/3/1 | Sitzecke HSV | DPST-232-600 |  KO 727 | Farbe als Farbton/Sättigung/Helligkeit. |
| 2/6/0 | Dachkante links Status | DPST-1-11 |  KO 601 (sendet) | Taster-LED; die Netzteil-Logik liest das KO direkt über die KO-Nummer, nicht über die GA. |
| 2/6/1 | Dachkante rechts Status | DPST-1-11 |  KO 654 (sendet) | Wie 2/6/0. |
| 2/6/2 | Sitzecke Status | DPST-1-11 |  KO 707 (sendet) | Wie 2/6/0. |
| 2/6/3 | HCL Farbtemperatur Status | DPST-7-600 |  KO 407 (sendet) | Aktuelle Farbtemperatur des globalen Lichtmanagers (Kelvin). |
| 2/6/4 | Wintergarten Stromverbrauch | DPST-7-12 |  KO 404 (sendet) | Berechneter Gesamtstrom in mA (gemeldet, solange die globale Strombegrenzung aktiv ist). |
| 2/7/0 | Netzteil Status | DPST-1-11 |  KO 412 (sendet) | Rückmeldung von Relais 1. Das Relais-KO 408 braucht keine GA, Logik 2 schreibt direkt hinein. |
| 0/0/1 | Uhrzeit | DPST-10-1 |  KO 2 | Lichtmanager und Logik brauchen eine gültige Uhrzeit (alternativ Datum/Uhrzeit DPT 19.001, je nach Einstellung unter Allgemein). |
| 0/0/2 | Datum | DPST-11-1 |  KO 3 | Wie 0/0/1. |

### Tasterbelegung (z. B. MDT Taster 6- oder 8-fach)

Für NeoPixel Wintergarten. Tastenpaare sind als Wippe gedacht (links/rechts).

- **Um:** das Status-Objekt der Taste mit der Status-GA verbinden, damit die Taste den aktuellen Zustand kennt; der NeoPixel sendet den Ein/Aus-Status bei jeder Änderung, auch durch Effektmanager, Szene oder „Alle LEDs“ (Lesen-Flag gesetzt).
- **Dimmen:** das Dimm-Objekt des NeoPixel ist DPT 3.007 Start/Stopp (3 s für den ganzen Bereich); heller dimmen schaltet ein ausgeschaltetes Segment ein, auf 0 gedimmt meldet der Status Aus.
- **Cue +/−:** ein Start-Telegramm springt eine Cue weiter bzw. zurück, solange ein Effektmanager läuft oder pausiert ist; an der ersten/letzten Cue bleibt er stehen.
- **Szene:** Szenennummer n wird als Telegrammwert n − 1 gesendet; nicht angelegte Szenen werden ignoriert, eine Szene schaltet das Segment ein.
- **Sperre:** sperrt alle steuernden Objekte des Segments (auch die gerätinterne Logik); der NeoPixel hat kein Sperr-Statusobjekt, die LED kann den Sperrzustand nicht anzeigen.
- **Status-LED:** Bei Wippen für mehrere Segmente zeigt die LED den Status des ersten Segments. Ob eine Status-LED ein 1-Byte-Objekt (5.010) auswerten kann, hängt vom Taster ab.
- **MDT-Bezeichnungen:** Funktionen sind allgemein beschrieben (Schalten/Um, Dimmen relativ 3.007 mit Stopp-Telegramm beim Loslassen, Szene 18.001, Wert senden 5.001/5.010, Status-LED über Objekt); die Bezeichnungen in der MDT-Applikation können abweichen.

**6-fach**

| Taste | Funktion | Sendet an | Status-LED |
|---|---|---|---|
| 1 | Dachkante – Wippe links: Ein (kurz) / heller (lang) | 2/0/0 · DPST-1-1 · 1 (Ein) (kurz)<br>2/1/0 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 2/6/0 · DPST-1-11 |
| 2 | Dachkante – Wippe rechts: Aus (kurz) / dunkler (lang) | 2/0/0 · DPST-1-1 · 0 (Aus) (kurz)<br>2/1/0 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 2/6/0 · DPST-1-11 |
| 3 | Sitzecke – Wippe links: Ein (kurz) / heller (lang) | 2/0/1 · DPST-1-1 · 1 (Ein) (kurz)<br>2/1/1 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 2/6/2 · DPST-1-11 |
| 4 | Sitzecke – Wippe rechts: Aus (kurz) / dunkler (lang) | 2/0/1 · DPST-1-1 · 0 (Aus) (kurz)<br>2/1/1 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 2/6/2 · DPST-1-11 |
| 5 | Dachkante – Um (kurz) | 2/0/0 · DPST-1-1 · Um (abwechselnd 1/0) (kurz) | 2/6/0 · DPST-1-11 |
| 6 | Sitzecke – Um (kurz) | 2/0/1 · DPST-1-1 · Um (abwechselnd 1/0) (kurz) | 2/6/2 · DPST-1-11 |

**8-fach**

| Taste | Funktion | Sendet an | Status-LED |
|---|---|---|---|
| 1 | Dachkante – Wippe links: Ein (kurz) / heller (lang) | 2/0/0 · DPST-1-1 · 1 (Ein) (kurz)<br>2/1/0 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 2/6/0 · DPST-1-11 |
| 2 | Dachkante – Wippe rechts: Aus (kurz) / dunkler (lang) | 2/0/0 · DPST-1-1 · 0 (Aus) (kurz)<br>2/1/0 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 2/6/0 · DPST-1-11 |
| 3 | Sitzecke – Wippe links: Ein (kurz) / heller (lang) | 2/0/1 · DPST-1-1 · 1 (Ein) (kurz)<br>2/1/1 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 2/6/2 · DPST-1-11 |
| 4 | Sitzecke – Wippe rechts: Aus (kurz) / dunkler (lang) | 2/0/1 · DPST-1-1 · 0 (Aus) (kurz)<br>2/1/1 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 2/6/2 · DPST-1-11 |
| 5 | Dachkante – Um (kurz) | 2/0/0 · DPST-1-1 · Um (abwechselnd 1/0) (kurz) | 2/6/0 · DPST-1-11 |
| 6 | Sitzecke – Um (kurz) | 2/0/1 · DPST-1-1 · Um (abwechselnd 1/0) (kurz) | 2/6/2 · DPST-1-11 |
| 7 | frei (z. B. für andere Gewerke) | – | – |
| 8 | frei (z. B. für andere Gewerke) | – | – |

**Nach dem Import prüfen:**

- Hardware-Auswahl passend zur Firmware.
- Daten-Ports von LED-Streifen 1 und 2 auswählen.
- Port von Relais 1 auswählen.
- Allgemein: Standort (Breiten-/Längengrad) und Zeitquelle für den Lichtmanager.
- Strombegrenzung an das gemeinsame Netzteil anpassen; „Strom pro LED“ (global) steht bei RGBW auf 0: die Firmware rechnet dann mit der Tabelle des LED-Typs (4 × 20 mA); ein Wert über 0 wird ganzzahlig auf die Kanäle verteilt (70 / 4 = 17 mA). Der Parameter ist in der ETS nur bei „Pro LED (automatisch)“ sichtbar, wirkt aber auch hier auf die Messung.
- Versorgt das geschaltete Netzteil auch den Controller, darf das Relais es nicht abschalten (siehe Hilfe „Relais“).
- Am Gerät prüfen, ob eine statische Farbe nach dem Einschalten des Netzteils erscheint (die Netzteil-Logik schaltet erst, wenn das Segment schon an ist).
- Am Gerät prüfen: Nach einem Neustart schaltet das Relais nur, wenn die Logik die Status-Meldungen beim Start erhält („Status nach Neustart senden“ = Ja ist gesetzt); prüfen, ob die Startverzögerung der Logik die erste Meldung verschluckt.
- Start-Farbe und Cue-Farben im Segment bzw. Effektmanager.

## 3. Garage

Drei Streifen (WS2812B): Einfahrt (120 LEDs), Wand links (180) und Wand rechts (180), je ein Segment und je ein eigenes Netzteil. Effektmanager 1 „Einfahrt“: Begrüßung (Komet, 10 s), Ausleuchten (Weiß, hält) und Nachlauf (Atmen, 30 s), ohne Loop. Torkontakt oder einer der Bewegungsmelder startet den Effektmanager auf allen drei Segmenten; 5 Minuten nach der letzten Meldung springt er auf den Nachlauf, danach gehen die Segmente aus.

| Einstellung | Wert |
|---|---|
| Hardware | OpenKNXiao V1.5 KNeoPix RP2350 (4882) |
| LED-Streifen | 1 Einfahrt 120, 2 Wand links 180, 3 Wand rechts 180 (WS2812B, Farbordnung GRB) |
| Segmente | 1 Einfahrt 1–120, 2 Links 121–300, 3 Rechts 301–480; nach EM-Stop: Aus |
| Effektmanager 1 „Einfahrt“ | Cue 1 Komet 10 s / 1000 ms, Cue 2 Solid weiß hält (Dauer 0), Cue 3 Atmen 30 s / 3000 ms, Loop aus |
| Funktion 1 | ODER aus 3 Eingängen, sendet bei jedem Eingangstelegramm |
| Logik 1–3 | Treppenlicht 5 min ab letzter Meldung; EIN → Wert 1 in „EM Start“, AUS → Wert 3 in „Cue setzen“ |
| Strombegrenzung | Global „Deaktiviert“; je Streifen „Benutzerdefiniert (Fester Wert)“ mit „Strom pro Kanal (mA)“ 4000 / 6400 / 6400 mA und Übergang ab 80 % auf 80 % Helligkeit (ein Netzteil je Streifen); gemessen mit Strom pro LED 60 mA |

**Strombegrenzung:** Die Streifen liegen weit auseinander und haben je ein eigenes Netzteil. Deshalb bleibt die globale Strombegrenzung auf „Deaktiviert“ und jeder Streifen hat eine eigene feste Grenze passend zu seinem Netzteil (Streifen → Strombegrenzung „Benutzerdefiniert (Fester Wert)“, Feld „Strom pro Kanal (mA)“). Hängen alle Streifen an einem Netzteil, stattdessen global „Fester Wert“ verwenden.

**Effektmanager und Segmente:** Ein Effektmanager ist nicht fest an ein Segment gebunden. Er läuft auf dem Segment, dessen KO „EM Start“ die Nummer des Effektmanagers erhält (Wert 1 = EM 1, Wert 0 = Stop), oder über die Segment-Parameter „Effektmanager nach Geräteneustart“ / „Effektmanager beim Einschalten“. Hier wird EM 1 auf allen drei Segmenten gestartet.

**Ablauf:** Funktion 1 (ODER, 3 Eingänge) meldet Anwesenheit. Logik 1–3 (je Segment eine) arbeiten als Treppenlicht mit 5 Minuten: EIN schreibt 1 in „EM Start“ des Segments, das Zeitende schreibt 3 in „Cue setzen“ und springt damit auf „Nachlauf“. Neue Meldungen verlängern die Zeit, ohne den Effektmanager neu zu starten (Wiederholungsfilter). Nach dem Nachlauf endet der Effektmanager, die Segmente gehen aus („Effektmanager-Stop: Rückkehr“ = Aus). Cue 2 hält (Dauer 0), deshalb gibt es keinen Übergang zu Cue 3: der Sprung über „Cue setzen“ ist ein harter Wechsel. Wird der Effektmanager von Hand gestoppt, während die Treppenlicht-Zeit läuft, startet ihn eine neue Meldung erst nach Ablauf dieser Zeit wieder. Ein Funktionsblock statt Logik am Eingang, weil die Logik nur zwei externe Eingänge hat.

Transfer-Strings (Reihenfolge einhalten):

1. Basiseinstellungen: Hardware, 3 Streifen, 3 Segmente, 1 Effektmanager

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/0§NeoPixelHardwareSelect=4882§NEONumberOfLEDStrips=3§NEONumberOfSegments=3§NEOEMEnabled=1§;OpenKNX
    ```

2. LED-Streifen 1: Einfahrt

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/1§Description~=Einfahrt§NEO~Length=120§NEO~powerLimitCombined=2§NEO~currentPerChannel=4000§NEO~autoBrightnessLimit=80§;OpenKNX
    ```

3. LED-Streifen 2: Wand links

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/2§Description~=Wand%20links§NEO~Length=180§NEO~powerLimitCombined=2§NEO~currentPerChannel=6400§NEO~autoBrightnessLimit=80§;OpenKNX
    ```

4. LED-Streifen 3: Wand rechts

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/3§Description~=Wand%20rechts§NEO~Length=180§NEO~powerLimitCombined=2§NEO~currentPerChannel=6400§NEO~autoBrightnessLimit=80§;OpenKNX
    ```

5. Segment 1: Einfahrt

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/1§Description~=Einfahrt§NEO~SegmentEnd=120§NEO~SegmentEmStopReturnMode=2§;OpenKNX
    ```

6. Segment 2: Links

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/2§Description~=Links§NEO~SegmentStart=121§NEO~SegmentEnd=300§NEO~SegmentEmStopReturnMode=2§;OpenKNX
    ```

7. Segment 3: Rechts

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/3§Description~=Rechts§NEO~SegmentStart=301§NEO~SegmentEnd=480§NEO~SegmentEmStopReturnMode=2§;OpenKNX
    ```

8. Effektmanager 1: Einfahrt

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOEM:-/1§NEOEM~CueCount=3§NEOEM~Cue1Effect:1=13§NEOEM~Cue1Effect:2=13§NEOEM~Cue1Duration:1=10§NEOEM~Cue1Duration:2=10§NEOEM~Cue1Fade:1=1000§NEOEM~Cue1Fade:2=1000§NEOEM~Cue1Text:1=Begr%C3%BC%C3%9Fung§NEOEM~Cue1Text:2=Begr%C3%BC%C3%9Fung§NEOEM~Cue1CometSpeed=170§NEOEM~Cue1CometTailLength=20§NEOEM~Cue2Text:1=Ausleuchten§NEOEM~Cue2Text:2=Ausleuchten§NEOEM~Cue3Effect:1=11§NEOEM~Cue3Effect:2=11§NEOEM~Cue3Brightness=160§NEOEM~Cue3Duration:1=30§NEOEM~Cue3Duration:2=30§NEOEM~Cue3Fade:1=3000§NEOEM~Cue3Fade:2=3000§NEOEM~Cue3Text:1=Nachlauf§NEOEM~Cue3Text:2=Nachlauf§NEOEM~Cue3BreathingSpeed=60§NEOEM~Name=Einfahrt§;OpenKNX
    ```

9. Funktionsblöcke: 1 Kanal sichtbar

    ```text
    OpenKNX,cv1,0xAD00:0xC/FCB:0xA/0§VisibleChannels=1§;OpenKNX
    ```

10. Funktion 1: ODER aus Torkontakt und zwei Bewegungsmeldern (3 Bus-Eingänge)

    ```text
    OpenKNX,cv1,0xAD00:0xC/FCB:0xA/1§Channel~Name=Garage%20Anwesenheit§CH~ChannelType=2§CH~NameKo0=Torkontakt§CH~NameKo1=BM%20links§CH~NameKo2=BM%20rechts§CH~LogicKo2D=1§CH~LogicBehavOut=0§;OpenKNX
    ```

11. Logik 1: Anwesenheit → EM 1 auf Segment 1 (Einfahrt) starten, nach 5 min Cue 3 „Nachlauf“

    ```text
    OpenKNX,cv1,0xAD00:0xC/LOG:0x44/1§f~Name=Garage%20EM%20Einfahrt§f~LogicType=2§f~Logic=2§f~E1=1§f~E1OtherKO:2=259§f~E1UseOtherKO=1§f~OStairtimeBase=1§f~OStairtimeTime=5§f~OStair=1§f~OStairOff=0§f~OOutputFilter=3§f~ODpt=2§f~OOnKOSend=1§f~OOnKOSendNumber=642§f~OOffDpt5=3§f~OOffKOSend=1§f~OOffKOSendNumber=646§;OpenKNX
    ```

12. Logik 2: Anwesenheit → EM 1 auf Segment 2 (Links) starten, nach 5 min Cue 3 „Nachlauf“

    ```text
    OpenKNX,cv1,0xAD00:0xC/LOG:0x44/2§f~Name=Garage%20EM%20Links§f~LogicType=2§f~Logic=2§f~E1=1§f~E1OtherKO:2=259§f~E1UseOtherKO=1§f~OStairtimeBase=1§f~OStairtimeTime=5§f~OStair=1§f~OStairOff=0§f~OOutputFilter=3§f~ODpt=2§f~OOnKOSend=1§f~OOnKOSendNumber=695§f~OOffDpt5=3§f~OOffKOSend=1§f~OOffKOSendNumber=699§;OpenKNX
    ```

13. Logik 3: Anwesenheit → EM 1 auf Segment 3 (Rechts) starten, nach 5 min Cue 3 „Nachlauf“

    ```text
    OpenKNX,cv1,0xAD00:0xC/LOG:0x44/3§f~Name=Garage%20EM%20Rechts§f~LogicType=2§f~Logic=2§f~E1=1§f~E1OtherKO:2=259§f~E1UseOtherKO=1§f~OStairtimeBase=1§f~OStairtimeTime=5§f~OStair=1§f~OStairOff=0§f~OOutputFilter=3§f~ODpt=2§f~OOnKOSend=1§f~OOnKOSendNumber=748§f~OOffDpt5=3§f~OOffKOSend=1§f~OOffKOSendNumber=752§;OpenKNX
    ```

Gruppenadressen (Vorschlag):

| GA | Name | DPT | KOs | Zweck |
|---|---|---|---|---|
| 3/7/0 | Garage Torkontakt | DPST-1-1 |  KO 250 | 1 = Tor offen; Eingang 1 der Funktion 1. |
| 3/7/1 | Garage Bewegung links | DPST-1-1 |  KO 251 | Eingang 2 der Funktion 1. |
| 3/7/2 | Garage Bewegung rechts | DPST-1-1 |  KO 252 | Eingang 3 der Funktion 1. |
| 3/4/0 | Garage EM Start/Stop | DPST-5-10 |  KO 642,  KO 695,  KO 748 | Wert 1 startet Effektmanager 1 auf allen drei Segmenten, Wert 0 stoppt ihn. Die Logikkanäle schreiben gerätintern direkt in diese KOs. |
| 3/4/1 | Garage EM Stop | DPST-1-1 |  KO 644,  KO 697,  KO 750 | Effektmanager sofort beenden. |
| 3/4/2 | Garage Cue +/- | DPST-3-7 |  KO 640,  KO 693,  KO 746 | Nächste/vorherige Cue des laufenden Effektmanagers (Heller = nächste). |
| 3/0/0 | Garage Licht Ein/Aus | DPST-1-1 |  KO 600,  KO 653,  KO 706 | Manuelles Schalten ohne Effektmanager. |
| 3/1/0 | Garage Dimmen | DPST-3-7 |  KO 625,  KO 678,  KO 731 | Relatives Dimmen. |
| 3/5/0 | Garage Sperre | DPST-1-1 |  KO 641,  KO 694,  KO 747 | 1 sperrt die steuernden KOs (Automatik aus). |
| 3/6/0 | Garage EM Zustand | DPST-5-10 |  KO 652 (sendet) | 0 = gestoppt, 1 = läuft, 2 = pausiert (Segment 1). |
| 3/6/1 | Garage aktive Cue | DPST-5-10 |  KO 645 (sendet) | Nummer der laufenden Cue (Segment 1). |
| 3/6/5 | Garage Licht Status | DPST-1-11 |  KO 601 (sendet) | Ein/Aus-Status von Segment 1 für die Status-LED des Tasters. |
| 3/6/2 | Garage Stromverbrauch Einfahrt | DPST-7-12 |  KO 505 (sendet) | Berechneter Strom von LED-Streifen 1 in mA (Streifen mit eigener Grenze melden auch bei global „Deaktiviert“). |
| 3/6/3 | Garage Stromverbrauch Wand links | DPST-7-12 |  KO 513 (sendet) | Berechneter Strom von LED-Streifen 2 in mA (Streifen mit eigener Grenze melden auch bei global „Deaktiviert“). |
| 3/6/4 | Garage Stromverbrauch Wand rechts | DPST-7-12 |  KO 521 (sendet) | Berechneter Strom von LED-Streifen 3 in mA (Streifen mit eigener Grenze melden auch bei global „Deaktiviert“). |

### Tasterbelegung (z. B. MDT Taster 6- oder 8-fach)

Für NeoPixel Garage. Tastenpaare sind als Wippe gedacht (links/rechts).

- **Um:** das Status-Objekt der Taste mit der Status-GA verbinden, damit die Taste den aktuellen Zustand kennt; der NeoPixel sendet den Ein/Aus-Status bei jeder Änderung, auch durch Effektmanager, Szene oder „Alle LEDs“ (Lesen-Flag gesetzt).
- **Dimmen:** das Dimm-Objekt des NeoPixel ist DPT 3.007 Start/Stopp (3 s für den ganzen Bereich); heller dimmen schaltet ein ausgeschaltetes Segment ein, auf 0 gedimmt meldet der Status Aus.
- **Cue +/−:** ein Start-Telegramm springt eine Cue weiter bzw. zurück, solange ein Effektmanager läuft oder pausiert ist; an der ersten/letzten Cue bleibt er stehen.
- **Szene:** Szenennummer n wird als Telegrammwert n − 1 gesendet; nicht angelegte Szenen werden ignoriert, eine Szene schaltet das Segment ein.
- **Sperre:** sperrt alle steuernden Objekte des Segments (auch die gerätinterne Logik); der NeoPixel hat kein Sperr-Statusobjekt, die LED kann den Sperrzustand nicht anzeigen.
- **Status-LED:** Bei Wippen für mehrere Segmente zeigt die LED den Status des ersten Segments. Ob eine Status-LED ein 1-Byte-Objekt (5.010) auswerten kann, hängt vom Taster ab.
- **MDT-Bezeichnungen:** Funktionen sind allgemein beschrieben (Schalten/Um, Dimmen relativ 3.007 mit Stopp-Telegramm beim Loslassen, Szene 18.001, Wert senden 5.001/5.010, Status-LED über Objekt); die Bezeichnungen in der MDT-Applikation können abweichen.

**6-fach**

| Taste | Funktion | Sendet an | Status-LED |
|---|---|---|---|
| 1 | Garage Licht – Wippe links: Ein (kurz) / heller (lang) | 3/0/0 · DPST-1-1 · 1 (Ein) (kurz)<br>3/1/0 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 3/6/5 · DPST-1-11 |
| 2 | Garage Licht – Wippe rechts: Aus (kurz) / dunkler (lang) | 3/0/0 · DPST-1-1 · 0 (Aus) (kurz)<br>3/1/0 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 3/6/5 · DPST-1-11 |
| 3 | Effektmanager „Einfahrt“ starten (kurz) | 3/4/0 · DPST-5-10 · 1 (EM 1) (kurz) | 3/6/0 · DPST-5-10 |
| 4 | Effektmanager beenden (kurz) | 3/4/1 · DPST-1-1 · 1 (Stop) (kurz) | – |
| 5 | Vorherige Cue – Wippe links, Dimmfunktion (lang) | 3/4/2 · DPST-3-7 · dunkler = vorherige Cue; Stopp beim Loslassen wird ignoriert (lang) | – |
| 6 | Nächste Cue – Wippe rechts, Dimmfunktion (lang) | 3/4/2 · DPST-3-7 · heller = nächste Cue; Stopp beim Loslassen wird ignoriert (lang) | – |

**8-fach**

| Taste | Funktion | Sendet an | Status-LED |
|---|---|---|---|
| 1 | Garage Licht – Wippe links: Ein (kurz) / heller (lang) | 3/0/0 · DPST-1-1 · 1 (Ein) (kurz)<br>3/1/0 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 3/6/5 · DPST-1-11 |
| 2 | Garage Licht – Wippe rechts: Aus (kurz) / dunkler (lang) | 3/0/0 · DPST-1-1 · 0 (Aus) (kurz)<br>3/1/0 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 3/6/5 · DPST-1-11 |
| 3 | Effektmanager „Einfahrt“ starten (kurz) | 3/4/0 · DPST-5-10 · 1 (EM 1) (kurz) | 3/6/0 · DPST-5-10 |
| 4 | Effektmanager beenden (kurz) | 3/4/1 · DPST-1-1 · 1 (Stop) (kurz) | – |
| 5 | Vorherige Cue – Wippe links, Dimmfunktion (lang) | 3/4/2 · DPST-3-7 · dunkler = vorherige Cue; Stopp beim Loslassen wird ignoriert (lang) | – |
| 6 | Nächste Cue – Wippe rechts, Dimmfunktion (lang) | 3/4/2 · DPST-3-7 · heller = nächste Cue; Stopp beim Loslassen wird ignoriert (lang) | – |
| 7 | Sperre Um (kurz) – Automatik aus, keine Status-LED vom NeoPixel | 3/5/0 · DPST-1-1 · Um (1 = gesperrt) (kurz) | – |
| 8 | Garage Licht – Um (kurz) | 3/0/0 · DPST-1-1 · Um (abwechselnd 1/0) (kurz) | 3/6/5 · DPST-1-11 |

**Nach dem Import prüfen:**

- Hardware-Auswahl passend zur Firmware.
- Daten-Ports der LED-Streifen 1–3 auswählen.
- Strombegrenzung je Streifen an das jeweilige Netzteil anpassen: Streifen → Strombegrenzung „Benutzerdefiniert (Fester Wert)“, Feld „Strom pro Kanal (mA)“ (Beispiel: 5 V / 5 A → 4000 mA, 5 V / 8 A → 6400 mA). Hängen alle Streifen an einem Netzteil, stattdessen global „Fester Wert“ und an den Streifen „Global verwenden“.
- Start-Farbe und Cue-Farben im Segment bzw. Effektmanager.

## 4. Terrasse (Effektkette über drei Geräte)

Drei Abschnitte, jeder mit eigenem NeoPixel-Gerät und eigenem Netzteil. Pro Gerät 8 Holzbalken mit je 60 LEDs (SK6812/WS2814 (RGBW)), jeder Balken an einem eigenen Ausgang. Der virtuelle Streifen (480 LEDs) ist ein Segment als 2D-Matrix 60×8. Die Effektkette verbindet die Matrizen für direkt gewählte Effekte zu einer Fläche von 180×8: Gerät 1 ist Master (Offset 0), die übrigen Geräte sind Slaves, Gesamtlänge 1440. Effektmanager „Abendstimmung“ (Sonnenuntergang, Welle, Glut, Loop) ist auf allen Geräten gleich angelegt und läuft auf jedem Gerät für sich auf 60×8; Logik 1 auf Gerät 1 startet ihn bei Sonnenuntergang und stoppt ihn um 23:00.

| Einstellung | Wert |
|---|---|
| Hardware | OpenKNXiao V1.5 KNeoPix RP2350 (4882), je Gerät |
| LED-Streifen 1–8 | SK6812/WS2814 (RGBW), Farbordnung GRBW, je 60 LEDs, Reihenfolge Balken 1–8 |
| Segment 1 | LED 1–480, Topologie 2D — Linear, Zeilen, Breite 60, Höhe 8, Start-Farbe #FF9040 + W 120 |
| Effektkette | Gerät 1 Master / Gerät 2 Slave Offset 480 / Gerät 3 Slave Offset 960, Gesamtlänge 1440 (gemeinsame Fläche 180×8 nur für direkt gewählte Effekte) |
| Slave-Timeout / Override | 30 s / Sync übernimmt immer (Standardwerte) |
| Effektmanager 1 „Abendstimmung“ | Cue 1 Gradient, Cue 2 Aurora 2D, Cue 3 Fire, je 600 s, Übergang 4000 ms, Loop ein; läuft je Gerät auf 60×8 |
| Strombegrenzung | Global „Fester Wert“ 16000 mA, Übergang ab 80 % der Grenze auf 80 % Helligkeit, Strom pro LED 0 = Tabelle des LED-Typs (RGB 3 × 20 mA, RGBW 4 × 20 mA) je Gerät (ein Netzteil je Abschnitt, z. B. 5 V / 20 A) |
| Logik 1 (nur Gerät 1) | Zeitschaltuhr: Sonnenuntergang EIN (Wert 1), 23:00 AUS (Wert 0) |
| Effektmanager-Stop: Rückkehr | Aus (um 23:00 wird es dunkel) |

**Topologie:** Jeder Balken hat einen eigenen Ausgang; alle Balken beginnen am selben Ende. Im virtuellen Streifen folgen die Balken aufeinander (Balken 1 = LED 1–60, Balken 2 = LED 61–120 …). Das entspricht „2D — Linear, Zeilen“ mit Breite 60 (LEDs pro Balken) und Höhe 8 (Balken). Ist ein Balken umgekehrt montiert, passt die Zuordnung nicht mehr; dann alle Balken gleich ausrichten.

**Effektkette:** Mit Gesamtlänge 1440 und Höhe 8 rechnet die Firmware für direkt gewählte Effekte, die der Master überträgt, eine gemeinsame Matrix von 180×8; Offset 480 und 960 entsprechen den Spalten 60 und 120. Die drei Abschnitte müssen also in Balkenrichtung nebeneinander liegen. Slave-Timeout 30 s und „Sync übernimmt immer“ sind Standardwerte und deshalb nicht im String: fällt der Master aus, gehen die Slaves nach 30 s aus. Ein/Aus, Dimmen, Helligkeit und Farbe gehen per Gruppenadresse an alle Geräte: während der Abendstimmung ignorieren die Slaves die Kette (eigener Effektmanager) und brauchen das Telegramm direkt; ohne Effektmanager überträgt die Kette denselben Wert vom Master.

**Effektmanager:** Ein laufender Effektmanager auf dem Master wird nicht über die Kette übertragen (der Master sendet dann weiter den Zustand von vor dem EM). Deshalb hat jedes Gerät denselben EM 1, und die GA 4/4/0 startet ihn auf allen drei Geräten. Solange ein Effektmanager läuft, ist die Effektkette am Segment ausgesetzt: jedes Gerät zeigt den Effekt auf seiner eigenen Matrix 60×8, nicht auf der gemeinsamen Fläche 180×8. Die Abschnitte starten mit demselben Telegramm, laufen danach mit eigener Zeitbasis.

**Strombegrenzung:** Jeder Abschnitt hat ein eigenes Netzteil für seine acht Balken; jedes Gerät begrenzt deshalb global auf dieses Netzteil. Hat jeder Balken ein eigenes Netzteil, global „Deaktiviert“ und je Streifen „Fester Wert“ setzen (wie in der Garage).

**Logik:** Die Zeitschaltuhr sitzt auf Gerät 1. Sie sendet über ihr Ausgangs-KO auf 4/4/0 (für Gerät 2 und 3) und schreibt zusätzlich gerätintern in KO 642 von Gerät 1.

### NeoPixel Terrasse Abschnitt 1 (Master)

Transfer-Strings (Reihenfolge einhalten):

1. Basiseinstellungen: Hardware, 8 Streifen, 1 Segment, 1 Effektmanager, Strombegrenzung

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/0§NeoPixelHardwareSelect=4882§NEONumberOfLEDStrips=8§NEOpowerLimitGlobal=16000§NEOcurrentPerLED=0§NEOpowerLimitCombined=1§NEOautoBrightnessLimit=80§NEOEMEnabled=1§;OpenKNX
    ```

2. LED-Streifen 1: Balken 1

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/1§Description~=Balken%201§NEO~LEDType=8§NEO~ColourOrder=7§NEO~Length=60§;OpenKNX
    ```

3. LED-Streifen 2: Balken 2

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/2§Description~=Balken%202§NEO~LEDType=8§NEO~ColourOrder=7§NEO~Length=60§;OpenKNX
    ```

4. LED-Streifen 3: Balken 3

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/3§Description~=Balken%203§NEO~LEDType=8§NEO~ColourOrder=7§NEO~Length=60§;OpenKNX
    ```

5. LED-Streifen 4: Balken 4

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/4§Description~=Balken%204§NEO~LEDType=8§NEO~ColourOrder=7§NEO~Length=60§;OpenKNX
    ```

6. LED-Streifen 5: Balken 5

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/5§Description~=Balken%205§NEO~LEDType=8§NEO~ColourOrder=7§NEO~Length=60§;OpenKNX
    ```

7. LED-Streifen 6: Balken 6

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/6§Description~=Balken%206§NEO~LEDType=8§NEO~ColourOrder=7§NEO~Length=60§;OpenKNX
    ```

8. LED-Streifen 7: Balken 7

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/7§Description~=Balken%207§NEO~LEDType=8§NEO~ColourOrder=7§NEO~Length=60§;OpenKNX
    ```

9. LED-Streifen 8: Balken 8

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/8§Description~=Balken%208§NEO~LEDType=8§NEO~ColourOrder=7§NEO~Length=60§;OpenKNX
    ```

10. Segment 1: 2D-Matrix 60×8, Effektkette Master

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/1§Description~=Terrasse%201§NEO~SegmentEnd=480§NEO~SegmentTopology=2§NEO~MatrixWidth=60§NEO~SyncMode=1§NEO~VirtualTotalLength=1440§NEO~SegmentStartupColor=-28608§NEO~SegmentStartupW=120§NEO~SegmentEmStopReturnMode=2§;OpenKNX
    ```

11. Effektmanager 1: Abendstimmung

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOEM:-/1§NEOEM~CueCount=3§NEOEM~Loop=1§NEOEM~Cue1Effect:1=17§NEOEM~Cue1Effect:2=17§NEOEM~Cue1Brightness=220§NEOEM~Cue1Duration:1=600§NEOEM~Cue1Duration:2=600§NEOEM~Cue1Fade:1=4000§NEOEM~Cue1Fade:2=4000§NEOEM~Cue1Text:1=Sonnenunterg.§NEOEM~Cue1Text:2=Sonnenunterg.§NEOEM~Cue1GradientSpeed=16§NEOEM~Cue1GradientEndHue=32§NEOEM~Cue2Effect:1=30§NEOEM~Cue2Effect:2=30§NEOEM~Cue2Duration:1=600§NEOEM~Cue2Duration:2=600§NEOEM~Cue2Fade:1=4000§NEOEM~Cue2Fade:2=4000§NEOEM~Cue2Text:1=Welle§NEOEM~Cue2Text:2=Welle§NEOEM~Cue2Aurora2DSpeed=40§NEOEM~Cue2Aurora2DHue=20§NEOEM~Cue2Aurora2DScale=24§NEOEM~Cue3Effect:1=8§NEOEM~Cue3Effect:2=8§NEOEM~Cue3Brightness=180§NEOEM~Cue3Duration:1=600§NEOEM~Cue3Duration:2=600§NEOEM~Cue3Fade:1=4000§NEOEM~Cue3Fade:2=4000§NEOEM~Cue3Text:1=Glut§NEOEM~Cue3Text:2=Glut§NEOEM~Cue3FireSpeed=60§NEOEM~Cue3FireCooling=110§NEOEM~Cue3FireSparking=60§NEOEM~Name=Abendstimmung§;OpenKNX
    ```

12. Logik 1: Zeitschaltuhr, EIN bei Sonnenuntergang, AUS um 23:00; sendet 1/0 auf die EM-GA und schreibt gerätintern in KO 642

    ```text
    OpenKNX,cv1,0xAD00:0xC/LOG:0x44/1§f~Name=Terrasse%20Abendstimmung§f~LogicType=5§f~Logic=5§f~Td1DuskDawn=8§f~Td2DuskDawn=1§f~TRestoreState=1§f~Td1Value=1§f~Td2HourAbs=23§f~Td2MinuteAbs=0§f~ODpt=2§f~OOnKOSend=1§f~OOnKOSendNumber=642§f~OOffKOSend=1§f~OOffKOSendNumber=642§;OpenKNX
    ```

Gruppenadressen (Vorschlag):

| GA | Name | DPT | KOs | Zweck |
|---|---|---|---|---|
| 4/0/0 | Terrasse Ein/Aus | DPST-1-1 | G1: KO 600, G2: KO 600, G3: KO 600 | Auf allen Geräten verknüpft: ohne laufenden Effektmanager überträgt zwar auch die Kette Ein/Aus, während der Abendstimmung ignorieren die Slaves die Kette aber (eigener Effektmanager) und brauchen das Telegramm direkt. Mit „Sync übernimmt immer“ setzt das direkte Telegramm am Slave nur die lokale Übersteuerung, die bei dieser Richtlinie nicht wirkt; der nächste Kettenwert des Masters ist derselbe. Mit „Lokal hat Vorrang“ würde das direkte Telegramm die Slaves bis zum Neustart von der Kette lösen. |
| 4/1/0 | Terrasse Dimmen | DPST-3-7 | G1: KO 625, G2: KO 625, G3: KO 625 | Auf allen Geräten verknüpft (Grund wie 4/0/0): jedes Gerät dimmt selbst; ohne Effektmanager gleicht die Kette die Helligkeit an die des Masters an. |
| 4/2/0 | Terrasse Helligkeit | DPST-5-1 | G1: KO 602, G2: KO 602, G3: KO 602 | Auf allen Geräten verknüpft (Grund wie 4/0/0). |
| 4/3/0 | Terrasse RGBW | DPST-251-600 | G1: KO 623, G2: KO 623, G3: KO 623 | Auf allen Geräten verknüpft: ein Farbbefehl beendet den Effektmanager des Geräts, das ihn empfängt; nur am Master würde die Abendstimmung auf den Slaves weiterlaufen. Ohne Effektmanager überträgt auch die Kette die Farbe. |
| 4/4/0 | Terrasse Abendstimmung Start/Stop | DPST-5-10 | G1: KO 102 (sendet), G2: KO 642, G3: KO 642, G1: KO 642 | Wert 1 startet EM 1, Wert 0 stoppt ihn. Jedes Gerät hat seinen eigenen EM 1; ein laufender EM auf dem Master wird nicht über die Kette übertragen, und während ein EM läuft, rendert jedes Gerät auf seinen eigenen 60×8. Gerät 1 erhält den Wert zusätzlich gerätintern von Logik 1. Das Ausgangs-KO der Logik ist DPT 5.005, die EM-Start-KOs DPT 5.010; beide 1 Byte ohne Skalierung, der Wert 1 kommt unverändert an. |
| 4/4/1 | Terrasse Cue +/- | DPST-3-7 | G1: KO 640, G2: KO 640, G3: KO 640 | Nächste/vorherige Cue auf allen drei Geräten. |
| 4/4/2 | Terrasse EM Stop | DPST-1-1 | G1: KO 644, G2: KO 644, G3: KO 644 | EM auf allen drei Geräten beenden. |
| 4/5/0 | Terrasse Sperre | DPST-1-1 | G1: KO 641, G2: KO 641, G3: KO 641 | Sperrt die steuernden KOs; ein gesperrter Slave ignoriert die Kette. |
| 4/6/0 | Terrasse Ein/Aus Status | DPST-1-11 | G1: KO 601 (sendet) | Rückmeldung des Masters. |
| 4/6/1 | Terrasse EM Zustand | DPST-5-10 | G1: KO 652 (sendet) | 0 = gestoppt, 1 = läuft, 2 = pausiert. |
| 4/6/2 | Terrasse Stromverbrauch Gerät 1 | DPST-7-12 | G1: KO 404 (sendet) | Berechneter Gesamtstrom dieses Abschnitts in mA. |
| 4/6/3 | Terrasse Stromverbrauch Gerät 2 | DPST-7-12 | G2: KO 404 (sendet) | Berechneter Gesamtstrom dieses Abschnitts in mA. |
| 4/6/4 | Terrasse Stromverbrauch Gerät 3 | DPST-7-12 | G3: KO 404 (sendet) | Berechneter Gesamtstrom dieses Abschnitts in mA. |
| 4/7/0 | Terrasse Effektkette Sync | DPST-16-1 | G1: KO 650 (sendet), G2: KO 650, G3: KO 650 | Verbindet Master und Slaves; nur diese drei KOs auf diese GA legen. Der Master sendet bei jeder Änderung und alle 3 s. |
| 0/0/1 | Uhrzeit | DPST-10-1 | G1: KO 2 | Zeitschaltuhr der Logik braucht eine gültige Uhrzeit. |
| 0/0/2 | Datum | DPST-11-1 | G1: KO 3 | Sonnenuntergang wird aus Datum und Standort berechnet. |

### NeoPixel Terrasse Abschnitt 2 (Slave)

Transfer-Strings (Reihenfolge einhalten):

1. Basiseinstellungen: Hardware, 8 Streifen, 1 Segment, 1 Effektmanager, Strombegrenzung – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 1

2. LED-Streifen 1: Balken 1 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 2

3. LED-Streifen 2: Balken 2 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 3

4. LED-Streifen 3: Balken 3 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 4

5. LED-Streifen 4: Balken 4 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 5

6. LED-Streifen 5: Balken 5 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 6

7. LED-Streifen 6: Balken 6 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 7

8. LED-Streifen 7: Balken 7 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 8

9. LED-Streifen 8: Balken 8 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 9

10. Segment 1: 2D-Matrix 60×8, Effektkette Slave

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/1§Description~=Terrasse%202§NEO~SegmentEnd=480§NEO~SegmentTopology=2§NEO~MatrixWidth=60§NEO~SyncMode=2§NEO~VirtualTotalLength=1440§NEO~VirtualOffset=480§NEO~SegmentStartupColor=-28608§NEO~SegmentStartupW=120§NEO~SegmentEmStopReturnMode=2§;OpenKNX
    ```

11. Effektmanager 1: Abendstimmung – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 11

### NeoPixel Terrasse Abschnitt 3 (Slave)

Transfer-Strings (Reihenfolge einhalten):

1. Basiseinstellungen: Hardware, 8 Streifen, 1 Segment, 1 Effektmanager, Strombegrenzung – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 1

2. LED-Streifen 1: Balken 1 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 2

3. LED-Streifen 2: Balken 2 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 3

4. LED-Streifen 3: Balken 3 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 4

5. LED-Streifen 4: Balken 4 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 5

6. LED-Streifen 5: Balken 5 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 6

7. LED-Streifen 6: Balken 6 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 7

8. LED-Streifen 7: Balken 7 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 8

9. LED-Streifen 8: Balken 8 – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 9

10. Segment 1: 2D-Matrix 60×8, Effektkette Slave

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/1§Description~=Terrasse%203§NEO~SegmentEnd=480§NEO~SegmentTopology=2§NEO~MatrixWidth=60§NEO~SyncMode=2§NEO~VirtualTotalLength=1440§NEO~VirtualOffset=960§NEO~SegmentStartupColor=-28608§NEO~SegmentStartupW=120§NEO~SegmentEmStopReturnMode=2§;OpenKNX
    ```

11. Effektmanager 1: Abendstimmung – identisch mit NeoPixel Terrasse Abschnitt 1 (Master), Schritt 11

### Tasterbelegung (z. B. MDT Taster 6- oder 8-fach)

Für NeoPixel Terrasse Abschnitt 1 (Master). Tastenpaare sind als Wippe gedacht (links/rechts).

- **Um:** das Status-Objekt der Taste mit der Status-GA verbinden, damit die Taste den aktuellen Zustand kennt; der NeoPixel sendet den Ein/Aus-Status bei jeder Änderung, auch durch Effektmanager, Szene oder „Alle LEDs“ (Lesen-Flag gesetzt).
- **Dimmen:** das Dimm-Objekt des NeoPixel ist DPT 3.007 Start/Stopp (3 s für den ganzen Bereich); heller dimmen schaltet ein ausgeschaltetes Segment ein, auf 0 gedimmt meldet der Status Aus.
- **Cue +/−:** ein Start-Telegramm springt eine Cue weiter bzw. zurück, solange ein Effektmanager läuft oder pausiert ist; an der ersten/letzten Cue bleibt er stehen.
- **Szene:** Szenennummer n wird als Telegrammwert n − 1 gesendet; nicht angelegte Szenen werden ignoriert, eine Szene schaltet das Segment ein.
- **Sperre:** sperrt alle steuernden Objekte des Segments (auch die gerätinterne Logik); der NeoPixel hat kein Sperr-Statusobjekt, die LED kann den Sperrzustand nicht anzeigen.
- **Status-LED:** Bei Wippen für mehrere Segmente zeigt die LED den Status des ersten Segments. Ob eine Status-LED ein 1-Byte-Objekt (5.010) auswerten kann, hängt vom Taster ab.
- **MDT-Bezeichnungen:** Funktionen sind allgemein beschrieben (Schalten/Um, Dimmen relativ 3.007 mit Stopp-Telegramm beim Loslassen, Szene 18.001, Wert senden 5.001/5.010, Status-LED über Objekt); die Bezeichnungen in der MDT-Applikation können abweichen.

**6-fach**

| Taste | Funktion | Sendet an | Status-LED |
|---|---|---|---|
| 1 | Terrasse – Wippe links: Ein (kurz) / heller (lang) | 4/0/0 · DPST-1-1 · 1 (Ein) (kurz)<br>4/1/0 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 4/6/0 · DPST-1-11 |
| 2 | Terrasse – Wippe rechts: Aus (kurz) / dunkler (lang) | 4/0/0 · DPST-1-1 · 0 (Aus) (kurz)<br>4/1/0 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 4/6/0 · DPST-1-11 |
| 3 | Abendstimmung starten (kurz) | 4/4/0 · DPST-5-10 · 1 (EM 1) (kurz) | 4/6/1 · DPST-5-10 |
| 4 | Abendstimmung beenden (kurz) | 4/4/0 · DPST-5-10 · 0 (Stop) (kurz) | – |
| 5 | Vorherige Cue – Wippe links, Dimmfunktion (lang) | 4/4/1 · DPST-3-7 · dunkler = vorherige Cue; Stopp beim Loslassen wird ignoriert (lang) | – |
| 6 | Nächste Cue – Wippe rechts, Dimmfunktion (lang) | 4/4/1 · DPST-3-7 · heller = nächste Cue; Stopp beim Loslassen wird ignoriert (lang) | – |

**8-fach**

| Taste | Funktion | Sendet an | Status-LED |
|---|---|---|---|
| 1 | Terrasse – Wippe links: Ein (kurz) / heller (lang) | 4/0/0 · DPST-1-1 · 1 (Ein) (kurz)<br>4/1/0 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 4/6/0 · DPST-1-11 |
| 2 | Terrasse – Wippe rechts: Aus (kurz) / dunkler (lang) | 4/0/0 · DPST-1-1 · 0 (Aus) (kurz)<br>4/1/0 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 4/6/0 · DPST-1-11 |
| 3 | Abendstimmung starten (kurz) | 4/4/0 · DPST-5-10 · 1 (EM 1) (kurz) | 4/6/1 · DPST-5-10 |
| 4 | Abendstimmung beenden (kurz) | 4/4/0 · DPST-5-10 · 0 (Stop) (kurz) | – |
| 5 | Vorherige Cue – Wippe links, Dimmfunktion (lang) | 4/4/1 · DPST-3-7 · dunkler = vorherige Cue; Stopp beim Loslassen wird ignoriert (lang) | – |
| 6 | Nächste Cue – Wippe rechts, Dimmfunktion (lang) | 4/4/1 · DPST-3-7 · heller = nächste Cue; Stopp beim Loslassen wird ignoriert (lang) | – |
| 7 | Sperre Um (kurz) – keine Status-LED vom NeoPixel | 4/5/0 · DPST-1-1 · Um (1 = gesperrt) (kurz) | – |
| 8 | Helligkeit 100 % (kurz) | 4/2/0 · DPST-5-1 · 100 % (kurz) | – |

**Nach dem Import prüfen:**

- Je Gerät: Hardware-Auswahl passend zur Firmware.
- Je Gerät: Daten-Ports der LED-Streifen 1–8 auswählen (Balken 1 = Zeile 1).
- Gerät 1: Standort und Zeitquelle unter Allgemein (Sonnenuntergang).
- Strombegrenzung je Gerät an das Netzteil des Abschnitts anpassen; „Strom pro LED“ (global) steht bei RGBW auf 0: die Firmware rechnet dann mit der Tabelle des LED-Typs (4 × 20 mA); ein Wert über 0 wird ganzzahlig auf die Kanäle verteilt (70 / 4 = 17 mA). Der Parameter ist in der ETS nur bei „Pro LED (automatisch)“ sichtbar, wirkt aber auch hier auf die Messung. Hat jeder Balken ein eigenes Netzteil, global „Deaktiviert“ und je Streifen „Fester Wert“ setzen.
- Alle drei Geräte mit derselben Firmware-Version betreiben (Effektkette).
- Startet ein Slave allein neu, verpasst er den EM-Start; er läuft erst ab dem nächsten Schaltpunkt (oder einem manuellen Start über 4/4/0) wieder mit.
- Start-Farbe und Cue-Farben im Segment bzw. Effektmanager.

## 5. Matrix-Panel 16×16

Ein Panel mit 16×16 LEDs (WS2812B), verdrahtet als 2D — Schlangen, Zeilen (→←→←). Ein Segment als 2D-Matrix mit dem Effekt Plasma Nebula. Globale Strombegrenzung 3000 mA, Start-Helligkeit 50 %.

| Einstellung | Wert |
|---|---|
| Hardware | OpenKNXiao V1.5 KNeoPix RP2350 (4882) |
| LED-Streifen 1 | WS2812B, Farbordnung GRB, 256 LEDs |
| Segment 1 | LED 1–256, Topologie 2D — Schlangen, Zeilen (→←→←), 16×16 |
| Effekt | Plasma Nebula (26), Speed 48 |
| Start-Helligkeit | 128 von 255 (50 %) |
| Strombegrenzung | Global „Fester Wert“ 3000 mA, Übergang ab 80 % der Grenze auf 80 % Helligkeit, Strom pro LED 60 mA (ein Netzteil, z. B. 5 V / 4 A) |

**Topologie:** „2D — Schlangen, Zeilen“ (→←→←) für Panels, bei denen jede zweite Zeile zurückläuft. Breite × Höhe = 16 × 16 = 256 = Segmentlänge. Plasma Nebula ist ein 2D-Effekt und wirkt nur mit gesetzter Matrix.

Transfer-Strings (Reihenfolge einhalten):

1. Basiseinstellungen: Hardware, Strombegrenzung

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/0§NeoPixelHardwareSelect=4882§NEOpowerLimitGlobal=3000§NEOpowerLimitCombined=1§NEOautoBrightnessLimit=80§;OpenKNX
    ```

2. LED-Streifen 1: Panel 16x16

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEOSTRIP:-/1§Description~=Panel%2016x16§NEO~Length=256§;OpenKNX
    ```

3. Segment 1: Matrix 16×16, Plasma Nebula

    ```text
    OpenKNX,cv1,0xAD00:0xC/NEO:0x5/1§Description~=Matrix§NEO~SegmentEnd=256§NEO~SegmentTopology=1§NEO~NEOEffectType:1=26§NEO~NEOEffectType:2=26§NEO~MatrixWidth=16§NEO~MatrixHeight=16§NEO~SegmentStartupBrightness=128§NEO~PlasmaNebulaSpeed=48§;OpenKNX
    ```

Gruppenadressen (Vorschlag):

| GA | Name | DPT | KOs | Zweck |
|---|---|---|---|---|
| 5/0/0 | Matrix Ein/Aus | DPST-1-1 |  KO 600 | Schalten. |
| 5/1/0 | Matrix Dimmen | DPST-3-7 |  KO 625 | Relatives Dimmen. |
| 5/4/0 | Matrix Effekt | DPST-5-10 |  KO 615 | Effektnummer setzen, z. B. 26 Plasma Nebula, 22 Matrix 2D, 30 Aurora 2D. |
| 5/3/0 | Matrix HSV | DPST-232-600 |  KO 621 | Grundfarbe für Effekte, die eine Primärfarbe nutzen. |
| 5/6/0 | Matrix Ein/Aus Status | DPST-1-11 |  KO 601 (sendet) | Rückmeldung. |
| 5/6/1 | Matrix Effekt Status | DPST-5-10 |  KO 616 (sendet) | Aktive Effektnummer. |
| 5/6/2 | Matrix Stromverbrauch | DPST-7-12 |  KO 404 (sendet) | Berechneter Gesamtstrom in mA (gemeldet, solange die globale Strombegrenzung aktiv ist). |

### Tasterbelegung (z. B. MDT Taster 6- oder 8-fach)

Für NeoPixel Matrix. Tastenpaare sind als Wippe gedacht (links/rechts).

- **Um:** das Status-Objekt der Taste mit der Status-GA verbinden, damit die Taste den aktuellen Zustand kennt; der NeoPixel sendet den Ein/Aus-Status bei jeder Änderung, auch durch Effektmanager, Szene oder „Alle LEDs“ (Lesen-Flag gesetzt).
- **Dimmen:** das Dimm-Objekt des NeoPixel ist DPT 3.007 Start/Stopp (3 s für den ganzen Bereich); heller dimmen schaltet ein ausgeschaltetes Segment ein, auf 0 gedimmt meldet der Status Aus.
- **Cue +/−:** ein Start-Telegramm springt eine Cue weiter bzw. zurück, solange ein Effektmanager läuft oder pausiert ist; an der ersten/letzten Cue bleibt er stehen.
- **Szene:** Szenennummer n wird als Telegrammwert n − 1 gesendet; nicht angelegte Szenen werden ignoriert, eine Szene schaltet das Segment ein.
- **Sperre:** sperrt alle steuernden Objekte des Segments (auch die gerätinterne Logik); der NeoPixel hat kein Sperr-Statusobjekt, die LED kann den Sperrzustand nicht anzeigen.
- **Status-LED:** Bei Wippen für mehrere Segmente zeigt die LED den Status des ersten Segments. Ob eine Status-LED ein 1-Byte-Objekt (5.010) auswerten kann, hängt vom Taster ab.
- **MDT-Bezeichnungen:** Funktionen sind allgemein beschrieben (Schalten/Um, Dimmen relativ 3.007 mit Stopp-Telegramm beim Loslassen, Szene 18.001, Wert senden 5.001/5.010, Status-LED über Objekt); die Bezeichnungen in der MDT-Applikation können abweichen.

**6-fach**

| Taste | Funktion | Sendet an | Status-LED |
|---|---|---|---|
| 1 | Matrix – Wippe links: Ein (kurz) / heller (lang) | 5/0/0 · DPST-1-1 · 1 (Ein) (kurz)<br>5/1/0 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 5/6/0 · DPST-1-11 |
| 2 | Matrix – Wippe rechts: Aus (kurz) / dunkler (lang) | 5/0/0 · DPST-1-1 · 0 (Aus) (kurz)<br>5/1/0 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 5/6/0 · DPST-1-11 |
| 3 | Effekt Plasma Nebula (kurz) | 5/4/0 · DPST-5-10 · 26 (kurz) | 5/6/1 · DPST-5-10 |
| 4 | Effekt Matrix 2D (kurz) | 5/4/0 · DPST-5-10 · 22 (kurz) | 5/6/1 · DPST-5-10 |
| 5 | Effekt Aurora 2D (kurz) | 5/4/0 · DPST-5-10 · 30 (kurz) | 5/6/1 · DPST-5-10 |
| 6 | Effekt Metaballs 2D (kurz) | 5/4/0 · DPST-5-10 · 32 (kurz) | 5/6/1 · DPST-5-10 |

**8-fach**

| Taste | Funktion | Sendet an | Status-LED |
|---|---|---|---|
| 1 | Matrix – Wippe links: Ein (kurz) / heller (lang) | 5/0/0 · DPST-1-1 · 1 (Ein) (kurz)<br>5/1/0 · DPST-3-7 · heller, Stopp beim Loslassen (lang) | 5/6/0 · DPST-1-11 |
| 2 | Matrix – Wippe rechts: Aus (kurz) / dunkler (lang) | 5/0/0 · DPST-1-1 · 0 (Aus) (kurz)<br>5/1/0 · DPST-3-7 · dunkler, Stopp beim Loslassen (lang) | 5/6/0 · DPST-1-11 |
| 3 | Effekt Plasma Nebula (kurz) | 5/4/0 · DPST-5-10 · 26 (kurz) | 5/6/1 · DPST-5-10 |
| 4 | Effekt Matrix 2D (kurz) | 5/4/0 · DPST-5-10 · 22 (kurz) | 5/6/1 · DPST-5-10 |
| 5 | Effekt Aurora 2D (kurz) | 5/4/0 · DPST-5-10 · 30 (kurz) | 5/6/1 · DPST-5-10 |
| 6 | Effekt Metaballs 2D (kurz) | 5/4/0 · DPST-5-10 · 32 (kurz) | 5/6/1 · DPST-5-10 |
| 7 | Effekt Lissajous 2D (kurz) | 5/4/0 · DPST-5-10 · 31 (kurz) | 5/6/1 · DPST-5-10 |
| 8 | Effekt Starfield Warp (kurz) | 5/4/0 · DPST-5-10 · 25 (kurz) | 5/6/1 · DPST-5-10 |

**Nach dem Import prüfen:**

- Hardware-Auswahl passend zur Firmware.
- Daten-Port von LED-Streifen 1 auswählen.
- Läuft die erste Zeile des Panels von rechts nach links, „Richtung umkehren“ am Segment setzen oder Topologie prüfen.
- Strombegrenzung an das Netzteil anpassen (Beispiel 3000 mA für 5 V / 4 A mit Reserve).
- Start-Farbe und Cue-Farben im Segment bzw. Effektmanager.
