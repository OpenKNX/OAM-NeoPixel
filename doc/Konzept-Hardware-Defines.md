# Konzept: Hardware-Abstraktion über Defines & Generator

> **Eine Firmware – viele Boards.** Wie das OAM-NeoPixel aus einer einzigen
> Codebasis dutzende OpenKNX-/Partner-Hardwarevarianten bedient, ohne pro Board
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

 Hauptgruppen:  0x1 = OpenKNX        0x2 = OpenKNX-Ready (Partner)   0x3 = Community
 Beispiel:      0x2122 = OpenKNX-Ready (2) / QuinLED (21) / Dig-Quad-V3 WiFi+AE_Plus (22)
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
   Zahl** — nicht 30 GPIO-Werte. Die ID ist der eindeutige Primärschlüssel; daraus wird
   der Hardware-**Index** abgeleitet (`HardwareMappingData.h`), der wiederum die
   Parameter-/GPIO-Auswahl steuert.
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
 │    + platformio.hardware.ini                                               │
 │    → pro Board: DEVICE_HW_ID, QUINLED_*_GPIO / XIAO_*_GPIO, Kanalzahl …    │
 └─────────────────────────────────┬──────────────────────────────────────────┘
                                   │  liest & wertet aus
                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────┐
 │ 2) GENERATOR — scripts/Build-HardwareConfig.ps1                            │
 │    erzeugt automatisch:                                                    │
 │    • src/HardwareMappingData.h   → C++-Tabelle HW-ID ↔ Index (30 Varianten)│
 │    • NeoPixel.share.xml          → pro HW eigene GPIO-ParameterTypes +     │
 │                                    Relais-/GPIO-UI (switch + #ifdef)       │
 │    • NeoPixel.script.js          → ETS-Logik                               │
 └─────────────────────────────────┬──────────────────────────────────────────┘
                                   │  OpenKNXproducer
                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────┐
 │ 3) PRODUCER — erzeugt .knxprod + include/knxprod.h                         │
 │    knxprod.h liefert Makros NUR für die einkompilierten Varianten:         │
 │    #define ParamNEOSTRIP_NEOGPIODataPortHW0 …  (pro HW-Index)              │
 │    #define ParamNEO_NeoPixelHardwareSelect …   (ETS-Dropdown „Hardware")   │
 └─────────────────────────────────┬──────────────────────────────────────────┘
                                   │  #include
                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────┐
 │ 4) FIRMWARE — src/HardwareMappingLogic.h                                   │
 │    wählt zur Laufzeit/Compilezeit den richtigen GPIO für das Board.        │
 └────────────────────────────────────────────────────────────────────────────┘
```

---

## 4. Auswahl der aktiven Hardware — zwei Modi

`getCurrentHardwareIndex()` (in `HardwareMappingLogic.h`) liefert den Index der
aktiven Variante – je nach Build-Modus:

```cpp
#ifdef DEVICE_HW_ID
    // (A) Single-Hardware-Build (eine .ini-Variante fest kompiliert):
    //     Index direkt aus der Device-ID ableiten.
    return HardwareMapping::mapDeviceHwIdToIndex(DEVICE_HW_ID);
#else
    // (B) Multi-Hardware-ETS-Build: der Anwender wählt in der ETS
    //     „Hardware Auswahl" → liefert den Index als Parameter.
    return (uint8_t)ParamNEO_NeoPixelHardwareSelect;
#endif
```

### Ausblick: Auto-Detection statt Dropdown

Wichtig: Auch im Multi-Hardware-ETS-Build (B) ist auf dem **Gerät** die konkrete
Variante bekannt — denn geflasht wird immer eine **board-spezifische Firmware**
(`.uf2` mit gesetztem `DEVICE_HW_ID`). Die Information ist also im Gerät vorhanden,
nur die ETS „weiß" sie (noch) nicht und fragt sie per Dropdown ab.

**Idee:** Die Firmware **meldet ihre `DEVICE_HW_ID`** (z. B. über eine OpenKNX-
Info-Property / ein Lese-KO / Konsole `neoa`), und die ETS **liest sie nach dem
Setzen der physikalischen Adresse aus** und setzt `ParamNEO_NeoPixelHardwareSelect`
automatisch.

Vorteile:
- **Kein manuelles Dropdown** mehr nötig — weniger Fehlbedienung.
- **Schließt eine Fehlerquelle:** Firmware-Board ≠ im Dropdown gewähltes Board
  (eine der „komische-Sachen"-Ursachen) kann nicht mehr auftreten — die ETS könnte
  bei Abweichung sogar warnen.

Status: **Konzept/Ausblick** — die Bausteine sind da (`DEVICE_HW_ID` im Gerät,
`HardwareMappingData.h` für ID↔Index, ETS kann Geräte-Properties lesen); ein
Auto-Abgleich/Read-Back wäre als Feature umzusetzen.

---

## 5. Der Kniff: `switch` + `#ifdef` (warum keine Tabelle?)

Die GPIO-Auswahl sieht so aus:

```cpp
inline uint8_t getDataPortSelectionForHardwareIndex(uint8_t stripIndex, uint8_t hwIndex = …)
{
    switch (hwIndex)
    {
    #ifdef ParamNEOSTRIP_NEOGPIODataPortHW0
        case 0: return (uint8_t)ParamNEOSTRIP_NEOGPIODataPortHW0;
    #endif
    #ifdef ParamNEOSTRIP_NEOGPIODataPortHW1
        case 1: return (uint8_t)ParamNEOSTRIP_NEOGPIODataPortHW1;
    #endif
        … (HW0 … HW29) …
    }
}
```

**Warum `#ifdef` pro `case` und keine Laufzeit-Tabelle?**
Die `ParamNEOSTRIP_…HWn`-Makros existieren **nur für die tatsächlich
einkompilierten** Hardware-Varianten (vom Producer in `knxprod.h` erzeugt). Eine
Tabelle `{ HW0, HW1, … }` würde Makros referenzieren, die je nach Build gar nicht
definiert sind → **Compile-Fehler**. Das `#ifdef`-Gating ist also kein
Schönheitsfehler, sondern **Voraussetzung** für die bedingte Kompilierung. Der
Wert kommt am Ende immer aus den **Defines** (ETS-Parameter bzw. .ini), nicht aus
hartem Code.

---

## 6. Konkretes Beispiel — KNeoPix RP2350 (WiFi)

1. Header `XIAO.h`: `#define DEVICE_HW_ID 0x131x`, `#define XIAO_*_GPIO …`.
2. `Build-HardwareConfig.ps1` ordnet ihr z. B. **HW-Index 12** zu (→ `HardwareMappingData.h`)
   und erzeugt in `share.xml` die GPIO-Auswahl-Parameter `…HW12`.
3. Producer schreibt `#define ParamNEOSTRIP_NEOGPIODataPortHW12 …` in `knxprod.h`.
4. ETS: Anwender wählt „OpenKNXiao V1 KNeoPix RP2350" → `ParamNEO_NeoPixelHardwareSelect = 12`.
5. Firmware: `getDataPortSelectionForHardwareIndex()` → `case 12:` → korrekter Daten-GPIO.

---

## 7. Wichtige Dateien (Spickzettel)

| Rolle | Datei |
|-------|-------|
| HW-Beschreibung (Defines) | `lib/OGM-HardwareConfig/include/HardwareConfig/**/*.h`, `platformio.hardware.ini` |
| Generator | `scripts/Build-HardwareConfig.ps1` |
| Generierte HW-Index-Tabelle | `src/HardwareMappingData.h` (AUTO-GENERATED) |
| Generierte ETS-Parameter/UI | `src/NeoPixel.share.xml` (in AUTO-GENERATED-Markern) |
| Producer-Makros | `include/knxprod.h` |
| Laufzeit-Auswahl-Logik | `src/HardwareMappingLogic.h` |

---

## 8. Vorteile (für die Präsentation)

- **Eine Firmware/ETS-App** für alle Boards – keine Board-Forks.
- **Neues Board = nur Defines** (Header + .ini, DEVICE_HW_ID) → Generator erledigt den Rest.
- **Kein toter Code:** per `#ifdef` landen nur die einkompilierten Varianten im Binary.
- **ETS-geführt:** Anwender wählt sein Board per Dropdown, nur passende Parameter erscheinen.
- **Konsistenz garantiert:** GPIO-Werte, ETS-Parameter und C++-Mapping stammen aus *einer* Quelle.

---

## 9. „Was muss ich tun, damit meine Hardware reinkommt?"

Die häufigste Entwicklerfrage — und die Kernbotschaft des Konzepts: **so wenig wie möglich.**
Es wird **kein** C++ und **keine** XML von Hand geschrieben — man pflegt nur **Defines**.

### Einmaliger Aufwand (Framework — bereits erledigt)
Das ist die Investition, die *einmal* gemacht wurde und für **alle** Boards trägt:
Generator (`Build-HardwareConfig.ps1`), Marker-Struktur in `share.xml`, das `0xHUVV`-
ID-Schema, die `switch`+`#ifdef`-Auswahllogik und die ID↔Index-Tabelle. → **Nichts davon
muss man pro Board nochmal anfassen.**

### Pro neuer Hardware (der wiederkehrende, minimale Aufwand)
1. **Hardware-Header anlegen/ergänzen** (`lib/OGM-HardwareConfig/include/HardwareConfig/**`):
   - `#define DEVICE_HW_ID 0x….` (eindeutige ID, Schema aus Abschnitt 2)
   - die GPIO-`#define`s des Boards (Daten-/Clock-Pins, Relais, Kanalzahl, Features)
2. **PlatformIO-Build-Target** für das Board in der `platformio.*.ini` ergänzen.
3. **Generator laufen lassen:** `pwsh scripts/Build-HardwareConfig.ps1`
   → erzeugt automatisch: ETS-Dropdown-Eintrag, GPIO-Parameter, `HardwareMappingData.h`
     (ID↔Index), `share.xml`-Blöcke, `script.js`.
4. **Bauen/Prüfen:** `pwsh scripts/Build-Release.ps1` (Producer + Firmware).

**Das war's.** GPIO-Auswahl-UI, ETS-Parameter, C++-Mapping, Konflikt-Erkennung — alles
entsteht generiert aus den Defines. Aufwand pro Board: **Minuten, nur Defines** (kein
Eingriff in Logik/Generator).

> Faustregel: *Neues Board = neuer Header-Eintrag (Defines) + Build-Target + Generatorlauf.*

---

## 10. Technik (für Tekkies): Extraktion via Präprozessor → ETS-Dropdown

Der Clou: Die Hardware-Werte werden **nicht** doppelt gepflegt oder per Regex aus den
Headern „geraten" — sie werden mit **demselben C++-Präprozessor** aufgelöst, den auch
die Firmware nutzt. Dadurch sind die generierten Werte **garantiert identisch** mit dem,
was die Firmware sieht (kein Drift).

### Die Extraktions-Pipeline (`scripts/Build-HardwareConfig.ps1`)

```
 platformio.hardware.ini        scripts/extract_gpio_config_simple.cpp
 (Liste aller Varianten:        (#include der HardwareConfig-Header,
  je env ein DEVICE_*-Macro)     druckt die aufgelösten Werte)
        │                                   │
        └───────────────┬───────────────────┘
                        ▼
      Pro Variante:  g++  -D DEVICE_<VARIANTE>  extract_gpio_config_simple.cpp
      (arm-none-eabi-g++ / xtensa-esp32-elf-g++ / riscv32-esp-elf-g++ — aus der PATH/Toolchain)
                        │
                        │  Präprozessor löst die  #ifdef DEVICE_<X>  /  #ifdef *_GPIO  Ketten auf
                        ▼
      Extractor gibt die KONKRETEN Werte aus:
        DEVICE_HW_ID, DEVICE_NAME, Daten-/Clock-GPIOs, Kanalzahl, Relais-Anzahl, Features …
                        │
                        │  PowerShell parst die Ausgabe → ein HardwareConfig-Objekt je Variante
                        ▼
      Generator schreibt daraus:
        • src/HardwareMappingData.h         (DEVICE_HW_ID ↔ Index)
        • NeoPixel.share.xml  →  ETS-„Hardware Auswahl"-Dropdown  +  per-HW GPIO-Parameter
        • NeoPixel.script.js                (Konflikt-/UI-Logik)
```

### Schritt für Schritt
1. **Varianten-Liste:** Der Generator liest `platformio.hardware.ini`. Jede `[env:…]`
   bzw. jeder `DEVICE_*`-Eintrag = eine Hardware-Variante.
2. **Toolchain finden:** Er sucht einen `*-g++` (arm-none-eabi / xtensa / riscv) in den
   installierten PlatformIO-Toolchains.
3. **Extractor pro Variante laufen lassen:** `extract_gpio_config_simple.cpp` wird je
   Variante mit deren `DEVICE_*`-Define **präprozessiert/kompiliert & ausgeführt**. Weil
   es **dieselben** `HardwareConfig/**`-Header inkludiert, liefert der Präprozessor genau
   die Pins/IDs dieses Boards.
4. **Ausgabe parsen:** stdout des Extractors → Werte je Variante (Marker-Zeilen).
5. **Generieren:** Aus der Variantenliste entstehen `HardwareMappingData.h`, die per-HW
   GPIO-Parameter und das **ETS-Dropdown**.

### Wie die Varianten im ETS-Dropdown landen
Aus der geparsten Liste erzeugt der Generator den Parameter **„Hardware Auswahl"**
(`ParamNEO_NeoPixelHardwareSelect`, UI `UP-4000018`) als **Enumeration** — **ein Eintrag
pro Variante** (`DEVICE_NAME` als Text, der Index als Wert). Genau dieser Index steuert
dann die `switch`+`#ifdef`-Auswahl aus Abschnitt 5.

→ **Konsequenz:** Eine neue Variante in `.ini` + Header → nach `Build-HardwareConfig.ps1`
erscheint sie **automatisch** als neuer Eintrag im ETS-Dropdown, mit ihren GPIO-Parametern.
Kein manuelles Pflegen von Enum-Einträgen oder Parametern.

### Mini-Beispiel (KNeoPix RP2350)

Der Extractor ist **preprocessor-only** — er wird gar nicht kompiliert, nur expandiert.
Seine `.cpp` enthält wörtliche `KEY = VALUE`-Zeilen, in denen Makros stehen; der
Präprozessor ersetzt sie durch die Board-Werte.

**Aufruf** (vereinfacht, eine Variante):
```bash
g++ -E -P -I lib/OGM-HardwareConfig/include \
    -D OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1 \
    scripts/extract_gpio_config_simple.cpp
```

**Ausgabe** (KEY=VALUE, von PowerShell geparst):
```
HW_DEVICE_NAME   = "OpenKNXiao V1 KNeoPix RP2350"
HW_DEVICE_ID_BIT = 0x1311
HW_GPIO_DATA_0   = 0
HW_GPIO_CLOCK_0  = 1
HW_LED_PORT_COUNT = 1
HW_EXT_RELAIS_COUNT = 0
…
```

**Daraus generiert** der Generator u. a.:
```xml
<!-- ETS-Dropdown „Hardware Auswahl" — ein Eintrag je Variante -->
<Enumeration Text="OpenKNXiao V1 KNeoPix RP2350" Value="12" Id="%ENID%" />
```
```cpp
// HardwareMappingData.h — ID ↔ Index
{ 0x1311, 12 },   // OpenKNXiao V1 KNeoPix RP2350
```
Wert `12` = der Hardware-Index, der zur Laufzeit die `switch`+`#ifdef`-Auswahl (Abschnitt 5) steuert.

### Relevante Dateien
| Rolle | Datei |
|-------|-------|
| Varianten-Liste | `platformio.hardware.ini`, `lib/OGM-HardwareConfig/include/HardwareConfig/OpenKNX/platformio.hardware.ini` |
| Extractor (Präprozessor-Werkzeug) | `scripts/extract_gpio_config_simple.cpp` |
| Orchestrierung + Generierung | `scripts/Build-HardwareConfig.ps1` |
| Toolchains | `~/.platformio/packages/toolchain-*/bin/*-g++` |

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
- **Komplexität im Generator:** Das „Magische" konzentriert sich in `Build-HardwareConfig.ps1`
  (groß, viele Sonderfälle). Wer *am Generator* arbeitet, hat eine steile Lernkurve — im
  Gegensatz zum trivialen *pro-Board*-Aufwand.
- **Manuelle ID-/Offset-Vergabe:** Param-IDs und Flash-Offsets werden im Generator von Hand
  vergeben → kollisionsanfällig (real passiert: `00163` doppelt mit Clock-Conflict). Heute
  fängt das nur der Producer ab; ein **Allocator/Registry** wäre robuster.
- **Toolchain-Abhängigkeit:** Die Extraktion braucht einen `*-g++` im PATH. Fehlt die
  Toolchain, schlägt die Generierung fehl (Build-Umgebung muss stimmen).
- **Tooling-Reibung:** PowerShell + macOS-Eigenheiten (z. B. `.ps1`-Exit 126, `pwsh` nötig).
- **Zwei-Stufen-Versionierung muss diszipliniert gepflegt werden:** `ApplicationVersion`
  (ETS-Config) **und** `FLASH_FORMAT_VERSION` (Runtime-State). Vergisst man den Bump →
  „komische Sachen" nach FW-Update (genau der Fall, den wir abgesichert haben).
- **Generierte Artefakte im Repo:** XML/Header sind eingecheckt → man muss nach Quell-
  Änderungen ans **Regenerieren** denken; sonst driften Quelle und Generat. (CI-Check empfohlen.)
- **Indirekte Debugbarkeit:** Bei Fehlern debuggt man durch generierte XML/Makros statt durch
  direkten Code — eine zusätzliche Abstraktionsebene.
- **Fixe Grenzen:** `MAX_HARDWARE_VARIANTS`, feste ID-Bereiche, 3-Bit-Relais-Count o. ä. sind
  Annahmen, die man bei starkem Wachstum sprengen kann.

### Fazit
Das Konzept ist für „**eine Codebasis, viele Boards**" **sehr gut gewählt** — der
Single-Source-Ansatz via Präprozessor ist elegant und vermeidet die klassische
Drift-Falle. Der Preis ist ein **komplexer, sorgfältig zu pflegender Generator** und
**Disziplin bei IDs/Offsets/Versionen**. Größter Hebel zur Verbesserung: **Allocator
statt manueller IDs/Offsets**, ein **CI-Regenerier-Check** und perspektivisch die
**Auto-Detection** (Abschnitt 4) — damit würde der ohnehin geringe Rest-Aufwand und die
Hauptfehlerquellen weiter schrumpfen.

---

## 12. Geplante Optimierungen (Roadmap / Ausblick)

Drei konkrete Verbesserungen — jeweils mit Problem, Lösung und Nutzen begründet.

> **Status (Stand 2026-06-13):** 12.1 Allocator → **Backlog (später)**;
> 12.2 CI-Check → **verworfen (vorerst nicht verfolgt)**;
> 12.3 Auto-Fill → **Backlog (später)** — die gefährliche Hälfte (Mismatch) ist
> bereits abgefangen, siehe Hinweis unten.

### 12.1 Offset-/ID-Allocator statt manueller Vergabe  ★ höchster Hebel — Backlog
- **Problem:** Param-IDs und Flash-Offsets werden im Generator **von Hand** vergeben.
  Das ist kollisionsanfällig — real passiert: die neuen Relais-Namen `00163-00166`
  kollidierten mit den Clock-Conflict-Parametern `00163-00170` (Ausweichen auf
  `00182-00185`). Heute fällt so etwas **erst** beim OpenKNXproducer (RefId-/BaseOffset-
  Integrity) auf, nicht beim Schreiben.
- **Lösung:** Ein zentraler **Allocator/Registry**, der freie IDs/Offsets je Region
  automatisch vergibt und Belegungen bucht (statt Magic-Numbers im Code).
- **Nutzen:** Kollisionen werden **strukturell unmöglich** statt nachträglich erkannt;
  neue Parameter hinzuzufügen wird trivial und reviewsicher.

### 12.2 CI-Regenerier-Check  — verworfen (vorerst nicht verfolgt)
- **Problem:** Generierte Artefakte (XML/Header) sind **im Repo eingecheckt**. Vergisst
  jemand nach einer Quell-/Generator-Änderung das Regenerieren, driften Quelle und
  Generat auseinander → schwer zu findende Fehler.
- **Lösung:** Ein CI-Schritt läuft den Generator und prüft „**Ergebnis == eingecheckter
  Stand?**" (z. B. `git diff --exit-code` nach Generatorlauf).
- **Nutzen:** Drift wird **automatisch** beim Pull-Request erkannt; der eingecheckte Stand
  ist garantiert reproduzierbar.

### 12.3 Auto-Detection statt Dropdown  (siehe Abschnitt 4) — Backlog
- **Problem:** Der Anwender wählt sein Board manuell im ETS-Dropdown. Fehlerquelle:
  Firmware-Board ≠ gewähltes Board → Fehlkonfiguration („komische Sachen").
- **Lösung:** Firmware meldet ihre `DEVICE_HW_ID` (Property/KO/Konsole), ETS liest sie
  nach dem Setzen der phys. Adresse aus und setzt `ParamNEO_NeoPixelHardwareSelect`
  automatisch (bzw. warnt bei Abweichung).
- **Nutzen:** **Kein manuelles Dropdown**, eine ganze Fehlerklasse entfällt. Bausteine
  sind vorhanden (`DEVICE_HW_ID` im Gerät, ID↔Index-Tabelle, ETS-Property-Read).

> **Entscheidung 2026-06-13:** 12.2 (CI) **verworfen**. 12.1 (Allocator) und 12.3 (Auto-Fill)
> bleiben als **Backlog für später** — kein akuter Handlungsbedarf (Mismatch ist gewarnt,
> Kollisionen fängt der Producer ab). 12.1 vor 12.3, wenn es wieder aufgegriffen wird.
