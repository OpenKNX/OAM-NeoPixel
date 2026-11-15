# UI-Redesign-Konzept: Tabellen- & Kanal-Oberflächen (OpenKNX NeoPixel)

> Status: **Konzept / kein Code**. Diese Datei beschreibt nur das gewünschte
> Redesign und dessen Implikationen. Es werden hier **keine** XML/CPP/Skript-Dateien geändert.
>
> Repo-Referenzen mit `Datei:Zeile` beziehen sich auf den Stand bei Erstellung (vor Dev 2.4) und sind
> verschoben. Abgleich mit Dev 2.4: die Segment-Spalte „EM bei Start" heißt heute „Effektmanager nach
> Geräteneustart" (`NEO%C%StartupEMAfterBoot`, `UP-…253`), dazu gibt es „Effektmanager beim Einschalten"
> (`UP-…255`); die Zeichen ⛔ und ⏸ sind die Zustands-Marker, die die Applikation in ETS anzeigt
> (`BASE_MarkInactiveChannel`). Offener Punkt dazu: `doc/TODO.md`, „Kanal-Tabellen".
> Alle Tabellen nutzen den ETS-Mechanismus `<ParameterBlock Layout="Grid">` mit
> `<Rows>`/`<Columns>` und `Cell="row,col"` (siehe `NeoPixel.EM.templ.xml:120-151`).

---

## 1. Inventar aller Tabellen- / Kanal-UIs

Jede „Übersichtstabelle" ist ein `ParameterBlock Layout="Grid"` mit Kopfzeile
(`ParameterSeparator UIHint="Headline" Cell="0,c"`) und je Kanal einer Datenzeile.
Die Datenzeilen entstehen entweder per `op:usePart` / `op:include`-Template
(EM, Cue) oder als voll ausgeschriebene 16-fach-Blöcke (Segmente, Streifen).

### 1.1 Übersichtstabellen

| # | Tabelle | Definition (Datei:Zeile) | Spalten heute | Param / Typ je Datenspalte |
|---|---------|--------------------------|---------------|----------------------------|
| T1 | **EM-Kanalauswahl** | Grid: `NeoPixel.share.xml:1332-1363`; Zeilen-Template: `NeoPixel.EM.templ.xml:164-172` (`<EmTableRow>`) | `Zustand \| EM \| Beschreibung` | Zustand = `NEOEM%C%Enabled` (`PT-EMEnabled`, 8-Bit, 0/1/2), gerendert als Dropdown `Cell="%C%,1"`; „EM %C%" = statischer `ParameterSeparator` `Cell="%C%,2"`; Beschreibung = `NEOEM%C%Name` (`PT-EMName`, ETS-only) `Cell="%C%,3"` |
| T2 | **Cue-Übersicht** (pro EM) | Grid: `NeoPixel.EM.templ.xml:120-151`; Zeilen-Template: `NeoPixel.Cue.part.xml:93-103` (`<CueTableRow>`) | `Cue \| Name \| Effekt \| Dauer (s) \| Übergang (ms)` | „Cue N" = statischer Separator `Cell="N,1"`; Name/Effekt/Dauer/Übergang = Cue-Parameter (`CUE_PPP+16/+0/+14/+15`) |
| T3 | **Segmente-Übersicht** | Grid + 16 ausgeschriebene Zeilen: `NeoPixel.share.xml:2468-2681` | `Beschreibung \| Start LED \| Ende LED \| Effekt \| EM bei Start` | Beschreibung = `Description%C%` (`PT-Text40Byte`); Start/Ende = `NEO%C%SegmentStart/End` (`UP-…050/051`, `PT-MinMaxValue14Bit`); Effekt = `NEO%C%NEOEffectType` (`UP-…057`, `PT-NEOEffectType`, read-only-Variante `…05702` wenn EM zugewiesen); EM bei Start = `NEO%C%StartupEM` (`UP-…253`, `PT-StartupEM`) |
| T4 | **Virtuelle-Streifen-Reihenfolge** | Grid + ausgeschriebene Zeilen: `NeoPixel.share.xml:1424-2449` | `Position \| Physikalischer Streifen \| Name \| Start LED \| End LED` | Position = statischer Separator; Physik.-Streifen = Auswahl-Dropdown je Position (`UP-000xx`); Name = `ParameterSeparator` mit `TextParameterRefId` auf Strip-`Description`; Start/End = `UP-00051/00057` (read-only Status) |

### 1.2 Kanal-Detailseiten (Baum-Knoten)

| # | Kanal-Seite | Definition (Datei:Zeile) | Kopf / Reihenfolge heute |
|---|-------------|--------------------------|--------------------------|
| K1 | **EM %C%** | `NeoPixel.EM.templ.xml:93-161` | Kopf `EM %C%: {{0:…}}` (`Icon="effectmanager"`, Titel via `TextParameterRefId` auf JS-Display-Name `UP-…010`). Reihenfolge: **Zustand-Dropdown** (immer sichtbar, `:102`) → dann (nur wenn ≠ Deaktiviert): Beschreibung, Anzahl Cues, Loop, Next, Text-KOs, Cue-Tabelle, Cue-Subknoten |
| K2 | **Segment %C%** | `NeoPixel.Segment.templ.xml` (Dynamic-Block); im share.xml eingebunden `:2686` | Kopf `Segment …`; enthält Start/Ende, Effekt, Matrix, Effektkette, Startverhalten, `StartupEM` (`:451`), Szenen, HCL |
| K3 | **LED Streifen %C%** | `NeoPixel.Strip.templ.xml:202-343` | Kopf `LED Streifen %C%`; Grundeinstellungen, Port-Zuweisung, Farbkalibrierung, Strombegrenzung |

### 1.3 Zustand-Modell heute (Ist)

`PT-EMEnabled` (`NeoPixel.EM.share.xml:14-20`) ist ein 8-Bit-Enum mit **drei** Werten,
die in **einem** Byte (EM-Offset 19) liegen:

| Wert | Text | FW-Bedeutung (`EffektManager.h:83-85`) |
|------|------|----------------------------------------|
| 0 | Deaktiviert | `isConfigured()==false`; ETS entfernt Params + KOs (GAs verloren) |
| 1 | Aktiv | `isEnabled()` (bei cueCount>0) → EM läuft/ist startfähig |
| 2 | Suspendiert | `isPaused()`; Konfig + KOs bleiben, FW rendert nicht |

Der Zustands-**Marker** (⛔ Deaktiviert / ⏸ Suspendiert) wird über die
JS-Funktion `BASE_MarkInactiveChannel` (`OGM-Common/src/Common.script.js:27-40`)
in den **nicht gespeicherten** Display-Namen `NEOEM%C%DisplayName` (`UP-…010`)
geschrieben (`NeoPixel.EM.templ.xml:73-89`), auf den Baum-Titel und alle EM-KO-Namen
via `TextParameterRefId` zeigen.

---

## 2. Design-Prinzipien (abgeleitete, durchgängige Regeln)

Aus dem konkreten EM-Wunsch abgeleitet und auf **alle** Tabellen/Kanal-Seiten anzuwenden:

- **P1 — Kanal-Nummer-Spalte heißt „Kanal".** Jede Übersichtstabelle beginnt mit
  einer Spalte, die die Kanalnummer trägt, und diese Spalte heißt einheitlich
  **„Kanal"** (nicht „EM", „Nr", „Pos", „Cue"). Ausnahme nur, wo die Nummer
  fachlich klar anders benannt ist (z. B. „Position" der Streifen-Reihenfolge –
  siehe Offene Fragen).
- **P2 — Spalten-Reihenfolge fix: Kanal → Zustand → Beschreibung → Fachspalten.**
  Erst die Identität (Nummer), dann der Ein/Aus-Zustand, dann der Klartext-Name,
  dann fachliche Spalten (Start/Ende, Effekt, …).
- **P3 — Binär-Zustände als Radio statt Dropdown.** Ein/Aus-artige Zustände
  werden als 2-Wert-Auswahl mit Optionsfeldern dargestellt
  (`( ) Deaktiviert  ( ) Aktiviert` bzw. `( ) Nein  ( ) Ja`), nicht als Klapp-Menü.
  ETS-technisch = `TypeRestriction UIHint="Buttons"` (siehe §5d).
- **P4 — Sonder-/Detailzustände raus aus der Übersicht, rein auf die Kanal-Seite.**
  Die Übersicht zeigt nur den primären Binärzustand. Mehrwertige oder seltene
  Zustände (Suspendiert) werden auf der jeweiligen Kanal-Detailseite konfiguriert.
- **P5 — Sonderzustände als Icon im Baum/Kopf.** ⏸ = Suspendiert, ⛔ = Deaktiviert
  erscheinen als Marker am Baum-Titel / Tabellenzeile (weiterhin über den
  nicht-gespeicherten Display-Namen + `BASE_MarkInactiveChannel`).
- **P6 — Kanal-Seite: Beschreibung zuerst, dann Zustandssteuerung.** Der Klartext-Name
  steht ganz oben; das obere (redundante) Zustands-Dropdown entfällt zugunsten der
  Binär-Steuerung aus der Übersicht plus einem seiten-lokalen Suspendiert-Radio.

---

## 3. Mockups „vorher → nachher" (ETS-ASCII)

Radio-Darstellung im Mockup: `(o)` = gewählt, `( )` = nicht gewählt.

### T1 — EM-Kanalauswahl

**Vorher** (`Zustand | EM | Beschreibung`, Zustand = Dropdown):

```
┌──────────────────────────┬───────┬───────────────────────────────┐
│ Zustand                  │ EM    │ Beschreibung                  │
├──────────────────────────┼───────┼───────────────────────────────┤
│ [ Aktiv          ▼ ]     │ EM 1  │ Wohnzimmer Loop               │
│ [ Suspendiert    ▼ ]     │ EM 2  │ Party                         │
│ [ Deaktiviert    ▼ ]     │ EM 3  │ (leer)                        │
└──────────────────────────┴───────┴───────────────────────────────┘
```

**Nachher** (`Kanal | Zustand | Beschreibung`, Zustand = Radio 2-Wert; „Suspendiert" fliegt raus):

```
┌────────┬──────────────────────────────────┬───────────────────────────────┐
│ Kanal  │ Zustand                          │ Beschreibung                  │
├────────┼──────────────────────────────────┼───────────────────────────────┤
│ EM 1   │ (o) Aktiviert    ( ) Deaktiviert │ Wohnzimmer Loop               │
│ ⏸ EM 2 │ (o) Aktiviert    ( ) Deaktiviert │ Party                         │
│ ⛔ EM 3 │ ( ) Aktiviert    (o) Deaktiviert │ (leer)                        │
└────────┴──────────────────────────────────┴───────────────────────────────┘
```

Hinweis: Suspendiert-Zeile (EM 2) bleibt in der Übersicht „Aktiviert" (der Radio
zeigt nur Aktiviert/Deaktiviert), aber das ⏸-Icon in der Kanalspalte signalisiert
den Pausenzustand. Deaktiviert (EM 3) trägt ⛔.

### T3 — Segmente-Übersicht

**Vorher** (`Beschreibung | Start LED | Ende LED | Effekt | EM bei Start`):

```
┌───────────────────┬────────┬────────┬──────────────┬────────────────────┐
│ Beschreibung      │ Start  │ Ende   │ Effekt       │ EM bei Start       │
├───────────────────┼────────┼────────┼──────────────┼────────────────────┤
│ Regal             │ 1      │ 30     │ [Regenb. ▼]  │ [Keiner        ▼]  │
└───────────────────┴────────┴────────┴──────────────┴────────────────────┘
```

**Nachher** (Kanal-Nummer zuerst; Effekt/EM sind Fachspalten, bleiben hinten):

```
┌────────┬───────────────────┬────────┬────────┬──────────────┬──────────────────┐
│ Kanal  │ Beschreibung      │ Start  │ Ende   │ Effekt       │ EM bei Start     │
├────────┼───────────────────┼────────┼────────┼──────────────┼──────────────────┤
│ Seg 1  │ Regal             │ 1      │ 30     │ [Regenb. ▼]  │ [Keiner       ▼] │
└────────┴───────────────────┴────────┴────────┴──────────────┴──────────────────┘
```

Segmente haben keinen Binär-Zustand (das Segment „existiert" über
`NEONumberOfSegments`), daher entfällt hier die Zustand-Spalte – die Regel P2
greift auf die vorhandenen Spalten (Kanal zuerst, dann Beschreibung, dann Fach).

### T4 — Virtuelle-Streifen-Reihenfolge

**Vorher** (`Position | Physikalischer Streifen | Name | Start LED | End LED`).
**Nachher**: „Position" ist bereits die Kanal-/Reihenfolge-Nummer. Empfehlung:
Header von „Position" auf **„Kanal"** vereinheitlichen ODER als bewusste Ausnahme
(fachlich = Reihenfolge, nicht Kanal) belassen — siehe §6. Sonst keine Änderung.

### T2 — Cue-Übersicht (pro EM)

**Vorher** (`Cue | Name | Effekt | Dauer (s) | Übergang (ms)`).
**Nachher**: „Cue" ist die Kanal-artige Nummer. Für Konsistenz mit P1 kann die
Kopfzeile auf **„Kanal"** gesetzt werden; da „Cue" hier aber der etablierte
Fachbegriff ist, ist „Cue" als benannte Ausnahme vertretbar. Spaltenreihenfolge
ist bereits P2-konform (Nummer → Name → Fach). Keine strukturelle Änderung nötig.

### K1 — EM-Kanal-Seite (EM 1)

**Vorher**:

```
▾ EM 1: Wohnzimmer Loop
    Zustand:            [ Aktiv / Deaktiviert / Suspendiert ▼ ]   ← oben, immer sichtbar
    Beschreibung:       [ Wohnzimmer Loop            ]
    Anzahl aktiver Cues [ 3 ] von 10
    Loop                [ … ]
    …
```

**Nachher** (oberes Zustands-Dropdown entfällt; Reihenfolge: 1. Beschreibung, 2. Suspendiert-Radio):

```
▾ ⏸ EM 1: Wohnzimmer Loop
    Beschreibung:       [ Wohnzimmer Loop            ]
    Suspendiert:        ( ) Nein   (o) Ja        ⏸ (Icon im Kopf bei „Ja")
    Anzahl aktiver Cues [ 3 ] von 10
    Loop                [ … ]
    …
```

Das „Deaktiviert/Aktiviert" wird nicht mehr auf der Kanal-Seite gewählt, sondern
in der Übersicht (T1). Auf der Kanal-Seite steht nur noch der Suspendiert-Radio.
(Bei Deaktiviert=true wird die Seite ohnehin von der Übersicht aus gesteuert; der
`choose !=0`-Wrapper `NeoPixel.EM.templ.xml:104` blendet die Detailparameter aus.)

---

## 4. Zustands-Modell: Aufteilung 3-Wert → 2×1-Wert

Der bisherige 3-Wert-`Enabled` (0/1/2) wird in **zwei binäre** Parameter zerlegt:

- **`Enabled`** (0/1) — wertstabil zum bestehenden Bit: 0=Deaktiviert, 1=Aktiviert.
  Wird zum reinen **Aktiv/Deaktiviert-Radio** in der Übersicht (T1).
- **`Suspendiert`** (0/1) — **neu**: 0=Nein (läuft), 1=Ja (pausiert ⏸).
  Wird zum **Suspendiert-Radio** auf der Kanal-Seite (K1).

### Interaktions-Tabelle

| `Enabled` | `Suspendiert` | Bedeutung | FW-Wirkung | Marker |
|-----------|---------------|-----------|------------|--------|
| 0 (Deaktiviert) | – (egal, ausgeblendet) | EM aus | ETS entfernt Params+KOs; FW: `isConfigured()==false` | ⛔ |
| 1 (Aktiviert) | 0 (Nein) | EM läuft | `isEnabled()` (bei cueCount>0), startfähig | – |
| 1 (Aktiviert) | 1 (Ja) | EM pausiert | Konfig+KOs bleiben; FW: `isPaused()`, rendert nicht | ⏸ |

Regel: `Suspendiert` ist nur relevant/sichtbar wenn `Enabled=1`. Bei `Enabled=0`
wird der Suspendiert-Radio ausgeblendet (bestehender `choose !=0`-Wrapper), sein
gespeicherter Wert bleibt aber erhalten (kein Reset beim Aus/Ein).

### Kompatibilität zum Bestand (wichtig)

Das heutige eine Byte kodiert 3 Werte {0,1,2}. Zwei getrennte Bits können den
Altwert **nicht automatisch** rekonstruieren, ohne eine Migrations-Regel:

- Alt `0` → `Enabled=0`, `Suspendiert=0`
- Alt `1` → `Enabled=1`, `Suspendiert=0`
- Alt `2` → `Enabled=1`, `Suspendiert=1`

Zwei Umsetzungsvarianten (Abwägung in §6):

- **Variante A (bit-stabil, empfohlen):** `Enabled` bleibt Byte 19 Bit 0 (Wert 0/1
  wie bisher). `Suspendiert` = **neues Bit** im selben Byte 19 (z. B. Bit 1, heute frei).
  Vorteil: Kein Byte-Offset-Shift, keine Layout-Migration der Cue-Blöcke. Der Altwert
  `2` (Bit1=1, Bit0=0) müsste per JS-Init oder Producer-Regel auf `Enabled=1`
  hochgezogen werden, sonst würde ein bisher-suspendierter EM als „Deaktiviert" gelesen.
- **Variante B (sauber getrennt):** neuer eigenständiger Parameter irgendwo im
  reservierten Header (Byte 0–15 sind laut `EffektManager.h:68` frei). Kostet keine
  Layout-Migration der Cues, verändert aber die FW-Struktur (reserved-Feld belegen).

---

## 5. Implikationen je Tabelle

### 5a. Layout-Change / Migration

- **T1 / K1 (EM):** Neuer Parameter `Suspendiert` nötig.
  - Variante A: Byte 19 Bit 1 (heute frei, Byte 19 nur Bit 0 belegt,
    `NeoPixel.EM.templ.xml:44`). **Kein** Offset-Shift, **keine** Cue-Layout-Migration
    → reprogrammfrei möglich (Fingerprint der Cue-Blöcke unverändert).
  - Variante B: eines der reservierten Header-Bytes 0–15. Ebenfalls kein Shift.
  - In **beiden** Fällen: `PT-EMEnabled` (3-Wert) wird zu `PT-EMEnabledBool` (0/1,
    `UIHint="Buttons"`); neuer `PT-EMSuspended` (0/1, `UIHint="Buttons"`,
    Texte Nein/Ja). Definition in `NeoPixel.EM.share.xml:14-20`.
- **T3 (Segmente):** Nur Kopf-Umbenennung + eine zusätzliche „Kanal"-Spalte
  (statischer `ParameterSeparator` je Zeile, `Cell="row,1"`), restliche Zellen um
  eine Spalte nach rechts (`Cell` col+1). **Kein** neuer gespeicherter Parameter,
  **keine** Byte-/Offset-Migration — reine XML-Layout-Änderung in
  `NeoPixel.share.xml:2468-2681` (16 Zeilen + Header + `<Columns>`).
- **T2 (Cue) / T4 (Streifen-Reihenfolge):** Nur optionale Kopf-Umbenennung, keine
  Struktur-/Speicheränderung.

### 5b. Firmware-Auswirkung (Lesestellen)

Betroffen ist ausschließlich das **Einlesen** des EM-Zustands:

- `NeoPixelModule.cpp:4001` — `data.header.enabled = (uint8_t)ParamNEOEM_NEOEMEnabled;`
  Muss den kombinierten Zustand rekonstruieren, z. B.:
  `enabled = ParamNEOEM_NEOEMEnabled ? (ParamNEOEM_NEOEMSuspended ? 2 : 1) : 0;`
  → hält das **bestehende** on-device-Format (0/1/2) und die Struktur
  `EffektManagerHeader.enabled` (`EffektManager.h:73`) unverändert.
- Konsumenten bleiben unangetastet, wenn die 0/1/2-Semantik erhalten bleibt:
  - `EffektManager.h:83-85` (`isEnabled` / `isPaused` / `isConfigured`)
  - `NeoPixelModule.cpp:6035` (`header.isPaused()`), `:6040` (`!header.enabled`),
    `:6042/6056` (Log-Texte)
  - `EffektManager.cpp:33,273,360` (`isEnabled()`-Gates)
- **Guardmakro** `#if defined(ParamNEOEM_NEOEMEnabled)` (`NeoPixelModule.cpp:3697`)
  bleibt gültig, muss ggf. um `ParamNEOEM_NEOEMSuspended` ergänzt werden.
- **Empfehlung:** on-device-Byte-Format (0/1/2) NICHT ändern, nur die ETS→FW-Abbildung.
  Damit ist die FW-Änderung minimal (eine Zeile) und keine EM-Datenstruktur-Migration nötig.

### 5c. JS-Marker-Logik (⛔ / ⏸)

Heute liest `BASE_MarkInactiveChannel` **einen** Steuerwert
(`NeoPixel.EM.templ.xml:80-83`, Context `{"InactiveValue":0,"PausedValue":2}`) und
setzt ⛔ bei 0, ⏸ bei 2. Nach der Aufteilung gibt es **zwei** Steuerparameter:

- Die JS-Funktion (`OGM-Common/src/Common.script.js:27-40`) erwartet **einen**
  `InactiveControl`-Alias. Optionen:
  1. **JS-Funktion erweitern** (2. LParameter `SuspendControl`): ⛔ wenn
     `Enabled==0`, sonst ⏸ wenn `Suspendiert==1`, sonst kein Marker. Da diese
     Funktion in `OGM-Common` liegt (shared über alle OAMs), muss die Erweiterung
     **abwärtskompatibel** sein (2. Param optional).
  2. **Hilfsparameter kombinieren** (JS-frei): einen nicht gespeicherten
     `MarkState`-Wert (0/1/2) aus `Enabled`+`Suspendiert` per zusätzlicher
     `ParameterCalculation` berechnen und diesen wie heute an `BASE_MarkInactiveChannel`
     füttern — dann bleibt die shared JS-Funktion unverändert. **Empfohlen**, weil
     es OGM-Common nicht anfasst.
- Der Marker-Pfad selbst (Display-Name `UP-…010` + `TextParameterRefId` auf Baum/KOs,
  `NeoPixel.EM.templ.xml:58,97`) bleibt strukturell gleich.

### 5d. Ist ein Radio-in-Zelle in ETS darstellbar?

**Recherche-Ergebnis (XSD `knx_project_20/knx-editor.xsd`):**

- `<Cell row,col>` nimmt beliebige UI-Elemente auf, u. a. `ParameterRefRef` — ein
  parameterbezogenes Steuerelement kann also in einer Grid-Zelle stehen (heute schon:
  T1 rendert dort das Zustand-Dropdown, `NeoPixel.EM.templ.xml:167`).
- Das Aussehen des Steuerelements bestimmt der `UIHint` des zugrunde liegenden
  `TypeRestriction`. Die zulässigen Werte für `TypeRestriction/UIHint` sind laut XSD
  (`knx-editor.xsd:3919-3928`): **`Text` | `DropDown` | `Buttons` | `Segmented`**.
- Einen expliziten Wert **`Radio` / `RadioButton` gibt es NICHT.** Das ETS-Äquivalent
  eines Radio-/Segment-Umschalters ist **`UIHint="Buttons"`** (bzw. `"Segmented"`) —
  eine horizontale 2-/n-Optionsleiste. Für 2 Werte ist das faktisch die gewünschte
  `( ) A  ( ) B`-Darstellung.

**Fazit:** Radio-artige Darstellung in der Zelle ist möglich via
`TypeRestriction UIHint="Buttons"` (2 Enumerationen). Ob ETS `Buttons` innerhalb
einer schmalen Grid-Zelle **platzsparend** rendert, ist visuell zu verifizieren
(Spaltenbreite ggf. erhöhen; ETS erlaubt Gesamtbreite bis 200 %, vgl.
`NeoPixel.share.xml:2488`). **Alternative**, falls `Buttons` in der Zelle zu breit
umbricht oder unschön ist:

- **Alt-1:** `Buttons` nur auf der Kanal-Seite (K1) verwenden, in der Übersicht (T1)
  beim schlanken `DropDown` mit 2 Werten (Aktiviert/Deaktiviert) bleiben. Erfüllt P3
  „nicht 3-Wert-Dropdown" weitgehend (Binär statt 3-Wert), ohne Breitenrisiko.
- **Alt-2:** In der Übersicht `UIHint="CheckBox"` (1-Bit) als „Aktiviert"-Häkchen —
  kompakteste Zellendarstellung, semantisch = Ein/Aus. Verliert die explizite
  „Deaktiviert"-Beschriftung, ist aber die platzsparendste ETS-native Lösung.

---

## 6. Offene Fragen / Risiken + empfohlene Reihenfolge

### Offene Fragen

- **Q1 — Übersichts-Radio-Optik:** `Buttons`-Rendering in einer 30 %-Zelle in ETS
  unerprobt. Muss visuell geprüft werden (§5d). Fallback CheckBox/DropDown steht bereit.
- **Q2 — Migration Altwert `2`:** Bestehende Anlagen mit einem suspendierten EM
  (Byte19=2). Ohne Migrationsregel liest ein neues `Enabled`-Bit (Bit 0) dort `0`
  → EM würde als „Deaktiviert" erscheinen und seine KOs verlieren. Muss über
  Producer-`Rename`/Default-Handling oder bewusste Ansage („suspendierte EMs nach
  Update neu prüfen") abgefangen werden. **Risiko: Datenverlust von GAs.**
- **Q3 — „Kanal" vs. Fachbegriffe:** Bei T2 (Cue) und T4 (Position) kollidiert P1
  mit etablierten Fachbegriffen. Empfehlung: EM+Segmente streng auf „Kanal"/„Seg N",
  Cue+Position als benannte Ausnahmen belassen (User-Entscheidung).
- **Q4 — OGM-Common-Berührung:** JS-Markererweiterung (§5c Option 1) würde shared
  Code ändern (muss für alle OAMs kompatibel bleiben, MEMORY-Regel). Deshalb ist die
  JS-freie Kombinations-Variante (§5c Option 2) vorzuziehen.
- **Q5 — Segment-Kanal-Spalte:** T3 fügt 5 → 6 Spalten hinzu; Gesamtbreite (heute
  ~140 %) muss neu austariert werden, damit „Effekt"/„EM bei Start" nicht umbrechen.

### Empfohlene Umsetzungs-Reihenfolge (schrittweise, migrations-sicher)

1. **Schritt 0 — nur Umbenennungen (null Risiko):** Header „EM" → „Kanal" in T1
   (`share.xml:1359`), optional Cue/Position. Keine Param-/Byte-Änderung.
2. **Schritt 1 — T3 Spaltenumbau:** „Kanal"-Spalte in Segmente-Übersicht einfügen,
   Cells umnummerieren, Breiten justieren. Reine Layout-XML, keine Migration.
3. **Schritt 2 — Zustand als Binär (ohne neues Byte):** `PT-EMEnabled` (3-Wert) →
   2-Wert-Bool (`UIHint="Buttons"`, Werte 0/1). Suspendiert zunächst NICHT
   aufteilen — nur die Übersicht auf Aktiviert/Deaktiviert reduzieren, „Suspendiert"
   temporär weiter über Kanal-Seite als 3. Dropdown-Wert. Testen, ob `Buttons` in
   der Zelle brauchbar rendert (Q1).
4. **Schritt 3 — `Suspendiert` neu (Variante A, Byte 19 Bit 1):** Neuer Bool-Param;
   Kanal-Seite K1 umbauen (oberes Dropdown raus, Reihenfolge Beschreibung →
   Suspendiert-Radio). FW-Abbildung `enabled = En ? (Susp?2:1):0`
   (`NeoPixelModule.cpp:4001`). Migrationsregel für Altwert 2 (Q2) mitliefern.
5. **Schritt 4 — Marker JS-frei kombinieren (§5c Option 2):** Kombi-`MarkState` aus
   En+Susp, `BASE_MarkInactiveChannel` unverändert lassen (kein OGM-Common-Eingriff).
6. **Schritt 5 — Verifikation:** Producer-Build + ETS-Import; prüfen: Marker ⛔/⏸,
   KO-Erhalt bei Suspendiert, GA-Verlust nur bei echtem Deaktivieren, Fingerprint der
   Cue-Blöcke unverändert (kein Zwangs-Reprogramm).
7. **Dokumentation nachziehen** (MEMORY: „Doku proaktiv mit-aktualisieren"):
   Baggage-Hilfetexte `NEO-EM-Aktiv`, `NEO-EM-Beschreibung`, Benutzerhandbuch.
