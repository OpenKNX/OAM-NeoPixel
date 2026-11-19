# Konzept: Hardware-Abstraktion über Defines & Generator

> **Eine Firmware – viele Boards.** Wie das OAM-NeoPixel aus einer einzigen
> Codebasis dutzende Hardwarevarianten von OpenKNX und Drittanbietern bedient, ohne pro Board
> eigenen Code zu pflegen.

---

## 1. Das Problem

Jedes LED-Board hat andere GPIO-Pins (Daten, Clock, Relais), andere Anzahl
Kanäle, andere Features (Ethernet, Audio, …). Trotzdem soll es **genau eine**
Firmware + **genau eine** ETS-Applikation geben. Lösung: Die Hardware wird über
**Defines** beschrieben, ein **Generator** macht daraus Code + ETS-Parameter,
und zur Laufzeit/Compilezeit wird die passende Variante ausgewählt.

---

## 2. Die Device-Hardware-ID (`0xHUVV`)

Jede Variante hat eine eindeutige 16-Bit-ID:

```
 0xHUVV
   │││└─ VV = Variante   (8 Bit, 256 Stück)
   ││└── U  = Subgruppe   (4 Bit)
   │└─── H  = Hauptgruppe (4 Bit)
   └──── Hex

 Hauptgruppen:  0x1 = OpenKNX        0x2 = Drittanbieter             0x3 = Community
 Beispiel:      0x2122 = Drittanbieter (2) / QuinLED (21) / Dig-Quad-V3 WiFi+AE_Plus (22)
                0x13xx = XIAO (KNeoPix/Mini, RP2040/RP2350/ESP32…)
```

Definiert wird sie als `#define DEVICE_HW_ID 0x….` im jeweiligen Hardware-Header.

### „Wozu eine eigene ID, wir haben doch schon alle GPIO-Infos?"

Häufige Frage — und ein wichtiger Designpunkt. Die vielen GPIO-Defines beschreiben
**WIE** ein Board verdrahtet ist (Konfiguration). Die `DEVICE_HW_ID` beschreibt
**WELCHES** Board es ist (Identität). Das sind zwei verschiedene Dinge:

| | GPIO-/Feature-Defines | `DEVICE_HW_ID` |
|---|---|---|
| Antwort auf | *Wie* ist es verdrahtet? | *Welches* Board ist es? |
| Umfang | dutzende Werte | **eine** kompakte 16-Bit-Zahl |
| Rolle | Detail-Konfiguration | **stabiler Schlüssel / Identität** |

Warum man die ID *braucht*, obwohl die GPIO-Infos vorliegen:

1. **Schlüssel statt Steckbrief:** Um zu wissen „welches Board", vergleicht man **eine
   Zahl** — nicht 30 GPIO-Werte. Die ID ist der eindeutige Primärschlüssel: die ETS-Auswahl speichert sie,
   die Firmware vergleicht sie mit ihrer eigenen und nutzt die Port-Bytes nur bei Gleichheit.
2. **Stabil über Konfig-Änderungen:** Pins kann man umsortieren, Features ergänzen — die
   `DEVICE_HW_ID` bleibt. Sie ist die kanonische Referenz, die GPIO-Tabelle ist Beiwerk.
3. **ETS-/Katalog-Identität:** Die ID verknüpft die Variante mit dem ETS-Produkt
   (Hardware/SerialNumber in der `.knxprod`) — Bestellnummer/Produktidentität.
4. **Auto-Detection & Diagnose:** Für „welches Board steckt da?" (Abschnitt 4) meldet die
   Firmware **die ID** (eine Zahl), nicht die ganze Pin-Tabelle. Auch Logs/Telemetrie/
   Support referenzieren kompakt „`0x1311`".
5. **Strukturiert & kollisionsfrei vergebbar:** Durch das `0xHUVV`-Schema ist die ID
   nicht opak, sondern **kategorisierbar** (Hersteller/Gruppe/Variante) und zentral
   vergebbar — zwei Boards können nie dieselbe ID haben.

> Kurz: Die GPIO-Defines sind der **Steckbrief**, die `DEVICE_HW_ID` ist der
> **Personalausweis**. Man identifiziert über den Ausweis, nicht über das Vermessen
> aller Merkmale.

---

## 3. Der Datenfluss (Quelle → ETS & Firmware)

```
 ┌────────────────────────────────────────────────────────────────────────────┐
 │ 1) QUELLE — Hardware-Beschreibung (Defines)                                │
 │    lib/OGM-HardwareConfig/include/HardwareConfig/**.h                      │
 │    → pro Board: DEVICE_HW_ID, DEVICE_NAME, HW_PORT_COUNT/_n_GPIO/_n_LABEL,  │
 │      DEVICE_THIRD_PARTY …                                                  │
 │    + Build-Envs (platformio.custom.ini): Board, ohne 2D                    │
 └─────────────────────────────────┬──────────────────────────────────────────┘
                                   │  Präprozessor je release_*-Env
                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────┐
 │ 2) GENERATOR — lib/OGM-HardwareConfig/generator/Build-HardwareProfile.ps1  │
 │    Manifest HardwareManifest.xml, Vorlagen src/hw/*.xml, script.src.js     │
 │    schreibt ganze Dateien (nie in handgeschriebene):                       │
 │    • src/NeoPixel.hw.module.gen.* → Hardware-Modul NEOHW (Auswahl, Abfrage)│
 │    • src/NeoPixel.hw.*.gen.xml   → per op:include in share/Strip.templ   │
 │    • src/NeoPixel.hw.conf.xml    → Feature-Flags (%NEOHW_..._CAP_No2D …)   │
 │    • src/NeoPixel.script.js      → aus script.src.js + Board-Tabellen      │
 │    • include/HardwareProfileData.h → eigenes Profil, bekannte HW-IDs      │
 │    • HardwareManifest.slots       → Board-Nummer je HW-ID (wird committet)  │
 └─────────────────────────────────┬──────────────────────────────────────────┘
                                   │  OpenKNXproducer
                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────┐
 │ 3) PRODUCER — erzeugt .knxprod + include/knxprod.h                         │
 │    knxprod.h liefert je Signal einen Träger auf dem gemeinsamen Byte:      │
 │    #define ParamNEOSTRIP_NEODataPort …  (alle Boards teilen das Byte)      │
 │    #define ParamNEOHW_HardwareSelect …  (Modul „Gerät", die HW-ID)         │
 └─────────────────────────────────┬──────────────────────────────────────────┘
                                   │  #include
                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────┐
 │ 4) FIRMWARE — src/HardwareMappingLogic.h + HardwarePorts.h (hwPortGpio)    │
 │    Portwert k → HW_PORT_<k+1>_GPIO des eigenen Board-Headers.              │
 └────────────────────────────────────────────────────────────────────────────┘
```

Reihenfolge in `scripts/Build-Release.ps1`: `Build-EffectParameters.ps1` (füllt die Effekt-Blöcke in
`src/NeoPixel.script.src.js`), dann `Normalize-LineEndings.ps1`, dann der gemeinsame Preprocess aus OGM-Common mit
Hardware-Generator und Producer.

---

## 4. Auswahl der aktiven Hardware

Die Hardware wählt der Anwender auf der Seite OpenKNX → „Gerät" (Modul `NEOHW`, vom Generator erzeugt, ModuleType 45); gespeichert
wird die `DEVICE_HW_ID`, 0 = nicht gewählt. Der Button „Vom Gerät lesen" liest sie per Function Property
158/4 vom Gerät (Antwort aus `include/HardwareProfileData.h`: HW-ID und Fähigkeiten; die Relais-Anzahl ist nur ETS)
und meldet Fähigkeiten, die vom erwarteten Profil abweichen.

Jede Release-Firmware ist für genau ein Board gebaut (`DEVICE_HW_ID` im Board-Header; ohne sie bricht der Build ab).
`HardwareMappingLogic.h` vergleicht die ETS-Auswahl mit ihr:

```cpp
_hwConfigMismatch = !hardwareSelectionMatches(selectedHardwareId()); // ParamNEOHW_HardwareSelect == DEVICE_HW_ID
gpio = HardwareProfile::signalGpio(selectedHardwareId(), portWert, manuellerGpio); // GPIO_NONE (255) = Streifen aus
```

Passt die Auswahl nicht (auch „nicht gewählt"), nutzt die Firmware kein Port-Byte, auch nicht „Manuell": Streifen und
Relais bleiben aus, die Status-LED blinkt. Fehlt `ParamNEOHW_HardwareSelect` in `knxprod.h`, bricht der Build ab
(`#error`), statt die Prüfung still wegzulassen. Der eigene Name kommt aus `DEVICE_NAME`, andere Hardware wird nur
mit ihrer ID angezeigt. Fehlt die eigene `DEVICE_HW_ID` im ETS-Modul, bricht der Build ab (`static_assert`).

---

## 5. Ein Byte für alle Boards (Träger-Parameter)

Jedes Board hat je Streifen eigene Port-Parameter (`…GPIODataPortHW<n>`, eigene Auswahlliste), alle auf **demselben
Byte** der Streifen-Union (Daten Offset 5, Clock Offset 3; Relais r im Relais-Byte). ETS schreibt das Byte aus dem
Parameter des gewählten Boards. Die Firmware liest es über einen Träger-Parameter ohne Oberfläche (`Access="None"`):
`ParamNEOSTRIP_NEODataPort`, `ParamNEOSTRIP_NEOClockPort`, `ParamNEO_NEOExternalRelay<r>Port`. Damit braucht die
Firmware keine Board-Nummer und keinen `switch` über alle Boards. Den Portwert bildet `hwPortGpio()`
(OGM-HardwareConfig `include/HardwareConfig/Helpers/HardwarePorts.h`) auf `HW_PORT_<k+1>_GPIO` ab, 255 für einen nicht beschriebenen Port.

---

## 6. Konkretes Beispiel — KNeoPix RP2350

1. Header `Xiao.h`: `#define DEVICE_HW_ID 0x1310`, Ports über den gemeinsamen Block
   `HW_PORT_1_GPIO OKNXHW_OPENKNXIAO_D1` … `HW_PORT_8_GPIO OKNXHW_OPENKNXIAO_D10`.
2. Der Generator liest das Env `release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1`; `HardwareManifest.slots` gibt der
   HW-ID 0x1310 die Board-Nummer 4 (Parameter `…HW4`, nur ETS).
3. ETS: Anwender wählt auf der Seite OpenKNX → „Gerät" „OpenKNXiao V1 KNeoPix RP2350" (oder fragt das Gerät ab) →
   `NEOHW_HardwareSelect = 4880`; Port „D3" im Parameter `…HW4` → Wert 2 (Portnummer − 1) im Byte des Streifens.
4. Firmware: Auswahl 4880 == `DEVICE_HW_ID` → Träger `ParamNEOSTRIP_NEODataPort` = 2 → `hwPortGpio(2)` = `HW_PORT_3_GPIO`.

---

## 7. Wichtige Dateien (Spickzettel)

| Rolle | Datei |
|-------|-------|
| HW-Beschreibung (Defines) | `lib/OGM-HardwareConfig/include/HardwareConfig/**/*.h` |
| Generator + Bausteine | `lib/OGM-HardwareConfig/generator/` (README dort) |
| Manifest, Board-Nummern | `HardwareManifest.xml`, `HardwareManifest.slots` |
| NeoPixel-Vorlagen | `src/hw/*.xml`, `src/NeoPixel.script.src.js` |
| Generierte Dateien | `src/NeoPixel.hw.*.gen.xml`, `src/NeoPixel.hw.module.gen.js`, `src/NeoPixel.hw.conf.xml`, `src/NeoPixel.script.js`, `include/HardwareProfileData.h` |
| Producer-Makros | `include/knxprod.h` |
| Laufzeit-Auswahl-Logik | `src/HardwareMappingLogic.h`, `lib/OGM-HardwareConfig/include/HardwareConfig/Helpers/HardwarePorts.h` |

---

## 8. Vorteile (für die Präsentation)

- **Eine Firmware/ETS-App** für alle Boards – keine Board-Forks.
- **Neues Board = nur Defines** (Header + .ini, DEVICE_HW_ID) → Generator erledigt den Rest.
- **Kein toter Code:** die Firmware kennt nur ihr eigenes Board (Träger-Byte + `hwPortGpio`).
- **ETS-geführt:** Anwender wählt sein Board per Dropdown, nur passende Parameter erscheinen.
- **Konsistenz garantiert:** GPIO-Werte, ETS-Parameter und C++-Mapping stammen aus *einer* Quelle.

---

## 9. „Was muss ich tun, damit meine Hardware reinkommt?"

Die häufigste Entwicklerfrage — und die Kernbotschaft des Konzepts: **so wenig wie möglich.**
Es wird **kein** C++ und **keine** XML von Hand geschrieben — man pflegt nur **Defines**.

### Einmaliger Aufwand (Framework — bereits erledigt)
Generischer Generator in OGM-HardwareConfig, Manifest `HardwareManifest.xml`, Vorlagen in `src/hw/`, das
`0xHUVV`-ID-Schema, das Hardware-Modul, die Träger-Parameter und die Slot-Datei. → **Nichts davon muss man pro Board
nochmal anfassen.**

### Pro neuer Hardware (der wiederkehrende, minimale Aufwand)
1. **Hardware-Header anlegen/ergänzen** (`lib/OGM-HardwareConfig/include/HardwareConfig/**`):
   - `#define DEVICE_HW_ID 0x….` (eindeutige ID, Schema aus Abschnitt 2)
   - die Ports `HW_PORT_COUNT`, `HW_PORT_<n>_GPIO`, `HW_PORT_<n>_LABEL` (lückenlos ab 1)
2. **PlatformIO-Build-Target** `release_<Board>` in `platformio.custom.ini` ergänzen (Board, Board-Define); das
   Manifest nimmt jedes `release_*`-Env. Relais in ETS: `<Set>`-Zeile im Manifest.
3. **Generator laufen lassen** (`lib/OGM-HardwareConfig/generator/Build-HardwareProfile.ps1`, als VS-Code-Task „Build HardwareConfig", oder `Build-Release.ps1`): neue Board-Nummer am
   Ende von `HardwareManifest.slots`, Eintrag im Hardware-Modul, Port-Listen, `HardwareProfileData.h`, `script.js`.
4. **Bauen/Prüfen:** `pwsh scripts/Build-Release.ps1` (Producer + Firmware), neue ApplicationVersion.

> Faustregel: *Neues Board = neuer Header-Eintrag (Defines) + Build-Target + Generatorlauf.*

---

## 10. Technik (für Tekkies): Präprozessor → ETS-Dropdown

Die Hardware-Werte werden **nicht** doppelt gepflegt oder per Regex aus den Headern „geraten" — der Generator
löst sie mit dem C-Präprozessor auf (`clang++`/`g++ -E`, Board-Define + `-D`-Flags des Envs), dieselben Header,
die auch die Firmware sieht.

1. **Boards:** `<Boards platformio="release_*" />` – je Env das Board-Define und die `-D`-Flags
   (`pio project config`).
2. **Je Board:** `DEVICE_HW_ID`, `DEVICE_NAME`, Ports (`HW_PORT_*`), Fähigkeiten (`<Capability expr>`) und Werte
   (`<Value>`) des Manifests per Präprozessor.
3. **Sortierung/Nummern:** aufsteigend nach HW-ID (`order="hwid"`); die Board-Nummer in Parameter-IDs kommt aus
   `HardwareManifest.slots` (`ids="slot"`), nie aus der Listenposition.
4. **Vorlagen:** `<Generate>` erzeugt aus `src/hw/*.xml` und den Bausteinen von OGM-HardwareConfig ganze Dateien;
   eine Datei wird nur geschrieben, wenn sich ihr Inhalt ändert.

Die Auswahl im Hardware-Modul (`NEOHW_HardwareSelect`, `UP-4500001`) speichert die **HW-ID** (0 = nicht gewählt), der
Portwert ist die **Portnummer − 1** (`PortListTypes.xml`). Einzelheiten: `lib/OGM-HardwareConfig/generator/README.md`.

---

## 11. Bewertung des Konzepts (Vor- & Nachteile)

> Ehrliche Einschätzung aus Entwicklersicht — als Diskussionsgrundlage für meine Präsentation.

### Stärken
- **Eine Quelle der Wahrheit:** GPIO-Werte, ETS-Parameter und C++-Mapping stammen aus
  *denselben* Defines, aufgelöst vom *selben* Präprozessor wie die Firmware → **kein Drift**,
  Korrektheit „by construction".
- **Skaliert hervorragend:** Neue Hardware = nur Defines + Build-Target + Generatorlauf
  (Minuten). Der teure Teil (Generator/Framework) ist einmalig.
- **Eine Firmware/ETS-App für alle Boards** — keine Forks, keine divergierenden Branches.
- **Kein toter Code** im Binary (`#ifdef`-Gating), ETS zeigt nur passende Parameter.
- **Maschinell validiert:** OpenKNXproducer prüft Offsets/IDs/Unions → viele Fehler fallen
  sofort auf (z. B. die ID-Kollision, die wir beim Relais-Umbau hatten).

### Schwächen / Risiken
- **Komplexität im Generator:** Das „Magische" konzentriert sich im Generator (OGM-HardwareConfig) und den
  NeoPixel-Vorlagen in `src/hw/`. Wer *am Generator* arbeitet, hat eine steile Lernkurve — im
  Gegensatz zum trivialen *pro-Board*-Aufwand.
- **Manuelle ID-/Offset-Vergabe:** Param-IDs und Flash-Offsets werden im Generator von Hand
  vergeben → kollisionsanfällig (real passiert: `00163` doppelt mit Clock-Conflict). Heute
  fängt das nur der Producer ab; ein **Allocator/Registry** wäre robuster.
- **Toolchain-Abhängigkeit:** Der Generator braucht einen C-Präprozessor (`clang++`/`g++`) und `pio`.
- **Tooling-Reibung:** PowerShell + macOS-Eigenheiten (z. B. `.ps1`-Exit 126, `pwsh` nötig).
- **Zwei-Stufen-Versionierung muss diszipliniert gepflegt werden:** `ApplicationVersion`
  (ETS-Config) **und** `FLASH_FORMAT_VERSION` (Runtime-State). Vergisst man den Bump →
  „komische Sachen" nach FW-Update (genau der Fall, den wir abgesichert haben).
- **Generierte Artefakte:** NeoPixel nimmt XML/Header per `.gitignore` aus, der Build erzeugt sie
  (`Build-Release.ps1`); OGM-HardwareConfig prüft seine Demo in der CI mit `-Check`.
- **Indirekte Debugbarkeit:** Bei Fehlern debuggt man durch generierte XML/Makros statt durch
  direkten Code — eine zusätzliche Abstraktionsebene.
- **Fixe Grenzen:** feste ID-Bereiche, 3-Bit-Relais-Count o. ä. sind
  Annahmen, die man bei starkem Wachstum sprengen kann.

### Fazit
Das Konzept ist für „**eine Codebasis, viele Boards**" **sehr gut gewählt** — der
Single-Source-Ansatz via Präprozessor ist elegant und vermeidet die klassische
Drift-Falle. Der Preis ist ein **komplexer, sorgfältig zu pflegender Generator** und
**Disziplin bei IDs/Offsets/Versionen**. Größter Hebel zur Verbesserung: **Allocator
statt manueller IDs/Offsets** (Abschnitt 12.1); CI-Check und Geräteabfrage sind inzwischen umgesetzt
(12.2, 12.3).

---

## 12. Geplante Optimierungen (Roadmap / Ausblick)

Drei konkrete Verbesserungen — jeweils mit Problem, Lösung und Nutzen begründet.

> **Status (Stand 2026-06-13):** 12.1 Allocator → **Backlog (später)**;
> 12.2 CI-Check → **verworfen (vorerst nicht verfolgt)**;
> 12.3 Auto-Fill → **Backlog (später)** — die gefährliche Hälfte (Mismatch) ist
> bereits abgefangen, siehe Hinweis unten.
> **Stand 2026-11-14:** 12.3 ist umgesetzt, 12.2 ist angelegt (siehe dort); 12.1 bleibt Backlog.

### 12.1 Offset-/ID-Allocator statt manueller Vergabe (höchster Hebel, Backlog)
- **Problem:** Param-IDs und Flash-Offsets werden im Generator **von Hand** vergeben.
  Das ist kollisionsanfällig — real passiert: die neuen Relais-Namen `00163-00166`
  kollidierten mit den Clock-Conflict-Parametern `00163-00170` (Ausweichen auf
  `00182-00185`). Heute fällt so etwas **erst** beim OpenKNXproducer (RefId-/BaseOffset-
  Integrity) auf, nicht beim Schreiben.
- **Lösung:** Ein zentraler **Allocator/Registry**, der freie IDs/Offsets je Region
  automatisch vergibt und Belegungen bucht (statt Magic-Numbers im Code).
- **Nutzen:** Kollisionen werden **strukturell unmöglich** statt nachträglich erkannt;
  neue Parameter hinzuzufügen wird trivial und reviewsicher.

### 12.2 CI-Regenerier-Check  — angelegt (2026-11-14: `Build-HardwareProfile.ps1 -Check` + GitHub Action `.github/workflows/hardware-profile.yml` in OGM-HardwareConfig, Windows PowerShell 5.1, prüft die Demo; ein grüner Lauf ist nicht belegt)
- **Problem (Stand 2026-06):** Generierte Artefakte (XML/Header) waren **im Repo eingecheckt**. Vergisst
  jemand nach einer Quell-/Generator-Änderung das Regenerieren, driften Quelle und
  Generat auseinander → schwer zu findende Fehler.
- **Lösung:** Ein CI-Schritt läuft den Generator und prüft „**Ergebnis == eingecheckter
  Stand?**" (z. B. `git diff --exit-code` nach Generatorlauf).
- **Nutzen:** Drift wird **automatisch** beim Pull-Request erkannt; der eingecheckte Stand
  ist garantiert reproduzierbar.

### 12.3 Auto-Detection statt Dropdown  (siehe Abschnitt 4) — umgesetzt
- Der Button „Vom Gerät lesen" auf der Seite OpenKNX → „Gerät" liest `DEVICE_HW_ID` und die Fähigkeiten per
  Function Property 158/4 und setzt die Auswahl; Abweichungen vom erwarteten Profil meldet er.

> **Entscheidung 2026-06-13 (überholt, siehe Status am Anfang von Abschnitt 12):** 12.2 (CI) **verworfen**. 12.1 (Allocator) und 12.3 (Auto-Fill)
> bleiben als **Backlog für später** — kein akuter Handlungsbedarf (Mismatch ist gewarnt,
> Kollisionen fängt der Producer ab). 12.1 vor 12.3, wenn es wieder aufgegriffen wird.
