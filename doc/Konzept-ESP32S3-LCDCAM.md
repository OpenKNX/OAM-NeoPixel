# Konzept: 8 serielle LED-Strips auf ESP32-S3 über LCD_CAM (i80, parallel, DMA)

Stand 2026-10-25 · Status: **Konzept, keine Implementierung, keine Lib-Änderung** · Entscheidung Erkan Çolak: Option A = LCD_CAM-Parallelausgabe **neben** dem bestehenden RMT-Backend · Version 3, **reviewed (adversariale Runden 1-3 und Schlussprüfung)**, alle Befunde eingearbeitet (s. Anhang).

Abgleich 2026-11-15 (Dev 2.4): Der Teil-Init-Fehler aus D10 (a) ist behoben: `NeoPixel::setup()` bricht nur noch ab,
wenn kein Streifen hochkommt (`lib/OFM-NeoPixel/src/NeoPixel.cpp:79-80`); ein Streifen ohne RMT-Kanal bleibt allein
dunkel. D10 (b), das RMT-Latch-Gate (`waitingForReset` wird nur in `isBusy()` gelöscht, `show()` verweigert solange),
steht so weiter im Code. `platformio.hardware.ini` gibt es nicht mehr; Ports und Pegelwandler kommen aus den
Board-Headern von OGM-HardwareConfig. Die übrigen Zeilenangaben sind vom 2026-10-25. Offener Punkt dazu: `doc/TODO.md`,
„ESP32: zu wenige RMT-Sendekanäle".

## Kurzfassung

**Entscheidungen (Empfehlung je eine Zeile):**
- **D1 API:** LCD_CAM-Register + GDMA (A2) statt `esp_lcd`. Keine Hilfspins, Teilpuffer, freie Ganzzahlteiler, cache-sicherer Ende-ISR; IDF-Version absichern.
- **D2 Raster:** Bus-Solver nimmt das größte S ≤ 10, das ins gemessene Budget passt und alle Datenblattfenster einhält; Untergrenze S = 4 (312,5 ns).
- **D3 Zuordnung:** ≤ 4 serielle Strips bleiben auf RMT wie heute. Darüber gehen inkompatible und längste Strips auf RMT, der Rest auf LCD; entschieden wird in der OAM.
- **D4 Toleranz:** Bitzeit ±2 %; Strips mit ETS-Timing ≠ 0 bevorzugt auf RMT.
- **D5 Speicher reicht nicht:** Strip mit Log ablehnen; Streaming erst bei echtem Bedarf.
- **D6 ETS-Parameter „Ausgabe-Backend“:** später, mit Versionssprung.
- **D7 Puffer-Helfer RMT/LCD/PIO:** zusammenführen, im selben Lib-Change.
- **D8 V1 (TXS0108E):** LCD erst nach Messung M4, bis dahin nur V1.5.
- **D9 Frame-Sync:** Batch mit `bool commit()` um jeden Mehr-Strip-Aufrufer; `isBusy()` = Bus belegt; Lanes auf 0 halten ihre Farbe.
- **D10 Vorhandene Lib-Fehler, vor LCD beheben:** (a) Ab 5 seriellen Strips auf dem S3 bleibt heute das ganze Modul dunkel (`NeoPixel.cpp:79-85`). (b) Verdacht auf ein RMT-Latch-Gate, das nur über `isBusy()` freigegeben wird.

**Aufwand:** ≈ 12-18 Personentage (Prototyp 2-3, Bench 1-2, Lib-Integration 5-7, OAM 1-2, Review/Test/Doku 2-3). Schätzung.

**Die 3 Hauptrisiken:**
1. Interner DMA-Speicher: 8 × 300 RGBW brauchen bei S = 10 96 KB, 8 × 1024 RGB 246 KB; der freie Heap ist unbekannt.
2. Acht gleichzeitige Flanken, besonders mit dem TXS0108E auf V1.
3. Ende-Erkennung und Pegel am Frame-Ende bzw. beim Boot sind unbelegt (GDMA-EOF kommt zu früh, Verhalten im always_out-Modus).

**Zuerst am Bench messen:**
- M5: freier interner DMA-Heap und größter Block.
- M1/M3: Idle- und Boot-Pegel, Frame-Ende.
- M2: realisierte T0H/T1H pro Lane, Plateau-Scan.
- M4: gleichzeitiges Schalten auf V1 und V1.5.
- M6: Codierzeit.

---

Legende: [belegt] = beim Schreiben an Quelle, Header oder Build verifiziert · [Schätzung] = Schätzung oder Ableitung, nicht gemessen · [offen] = nur am Bench klärbar · ok / verletzt = Datenblattfenster eingehalten / verletzt.

Pfadkürzel:
- `OFM` = `/Users/ecolak/Entwicklung/OpenKNX_Clean/OFM-NeoPixel`
- `OAM` = `/Users/ecolak/Entwicklung/OpenKNX_Clean/OAM-NeoPixel`
- `HWC` = `/Users/ecolak/Entwicklung/OpenKNX_Clean/OGM-HardwareConfig`
- `OGM` = `/Users/ecolak/Entwicklung/OpenKNX_Clean/OGM-Common`
- `LIBS` = `~/.platformio/packages/framework-arduinoespressif32-libs/esp32s3` (installierte Header und Libs)
- `IDF` = `https://github.com/espressif/esp-idf/blob/858a988d6e/` (genau der IDF-Commit, gegen den das Projekt linkt, s. 1.1)
- `NPB` = `https://github.com/Makuna/NeoPixelBus/blob/2.8.4/`
- `FL` = `https://github.com/FastLED/FastLED/blob/c426c95374e743c1f3c9d8aef6476fab50684223/`

---

## 0. Ausgangslage

| Fakt | Beleg |
|---|---|
| Der ESP32-S3 hat 4 RMT-TX-Kanäle. | [belegt] `LIBS/include/soc/esp32s3/include/soc/soc_caps.h:276` |
| OFM belegt einen RMT-Kanal pro seriellem Strip. Auf dem S3 gibt es 4 Slots; ab dem 5. Strip schlägt `init()` mit "No free RMT channels available!" fehl. | [belegt] `OFM/src/rmt/rmt_neopixel_serial.cpp:318-342` |
| Die Factory wählt auf dem ESP32 für 1-Wire immer RMT. | [belegt] `OFM/src/hal/DriverFactory.cpp:82-86` |
| KNeoPiX S3 V1 und V1.5 deklarieren je 8 Ports. | [belegt] `OAM/platformio.hardware.ini:247`, `:439` |
| Die Ports D1..D5 und D8..D10 liegen auf **GPIO 2, 3, 4, 5, 6, 7, 8 und 9**. | [belegt] `OAM/platformio.hardware.ini:252-266`, `:444-458`; `HWC/include/HardwareConfig/OpenKNX/Xiao.h:388-398` |
| **Heute bleibt das gesamte NeoPixel-Modul dunkel, sobald mehr als 4 serielle Strips konfiguriert sind (nicht nur Strip 5-8).** Ablauf: `addStrip` nimmt alle 8 an, weil der Ressourcenzähler nur initialisierte Strips zählt und beim Anlegen noch keiner initialisiert ist. Danach scheitert `init()` für Strip 5. `NeoPixelManager::init()` lässt die übrigen Strips weiterlaufen, gibt aber `false` zurück. `NeoPixel::setup()` bricht daraufhin vor `_initialized = true` ab, und `NeoPixel::loop()` kehrt ab dann sofort zurück. | [belegt] Code-Trace: `OFM/src/NeoPixelManager.cpp:118`, `:1829`, `:1863`, `:574-580`; `OFM/src/NeoPixel.cpp:79-85`, `:106`; Reihenfolge in OAM `NeoPixelModule.cpp:4015` vor `:211`. [offen] Am Gerät nicht beobachtet. |
| Das ist ein **eigener Lib-Fehler**: `NeoPixel.cpp:79` behandelt einen Teil-Init wie einen Totalausfall. Er gilt auf jeder Plattform und bei jedem fehlgeschlagenen Strip-Init, unabhängig von LCD. | → D10 |
| Level-Shifter: V1 hat TXS0108E (`=1`), V1.5 hat SN74LV8T245 (`=4`). | [belegt] `OAM/platformio.hardware.ini:245`, `:437`; `OFM/src/LevelShifterType.h:62`, `:65` |

---

## 1. ESP-IDF-API im tatsächlich verwendeten Framework

### 1.1 Welche Version läuft wirklich?

| Ebene | Wert | Beleg |
|---|---|---|
| Plattform | pioarduino `54.03.21-2` | [belegt] `OGM/platformio.esp32.ini:17` |
| Arduino-Core | 3.2.1 | [belegt] `~/.platformio/platforms/espressif32@src-eecc8a7…/platform.json:36`; `~/.platformio/packages/framework-arduinoespressif32/package.json:3` |
| ESP-IDF (vorkompiliert) | `release/v5.4` @ `858a988d6e` | [belegt] `platform.json:42`; `LIBS/../versions.txt:2` |
| sdkconfig-Variante | `qio_opi` (XIAO S3 mit 8 MB Octal-PSRAM) | [belegt] `boards/seeed_xiao_esp32s3.json:6`; `qio_opi`-Pfade in `OAM/.pio/build/release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V15_IP/firmware.map` |
| `esp_lcd` und GDMA in den Libs **vorhanden** | `LIBS/lib/libesp_lcd.a` exportiert `esp_lcd_new_i80_bus` und `esp_lcd_panel_io_tx_color`; `LIBS/qio_opi/libesp_hw_support.a` exportiert `gdma_new_ahb_channel`, `gdma_link_mount_buffers` und `gdma_start` | [belegt] `nm`; `-lesp_lcd` in `LIBS/flags/ld_libs` |

Unter `~/.platformio/packages/framework-espidf` liegt IDF 5.5.2. Das ist **nicht** die Version, die das Projekt nutzt. Alle IDF-Zitate stammen deshalb aus Commit `858a988d6e`. Das lokal installierte `lcd_ll.h` und `dma_types.h` sind byte-identisch mit diesem Stand ([belegt] `diff`).

### 1.2 LCD_CAM-Ressourcen auf dem S3

| Ressource | Wert | Beleg |
|---|---|---|
| i80-Busse | 1 | [belegt] `soc_caps.h:296`, `:301` |
| max. Datenbreite | 16 | [belegt] `soc_caps.h:298`, `:302` |
| GDMA-Kanalpaare | 5 (AHB, PSRAM-fähig) | [belegt] `soc_caps.h:167`; `LIBS/qio_opi/include/sdkconfig.h:114` |
| Async-FIFO des LCD | 16 Einträge | [belegt] `LIBS/include/hal/esp32s3/include/hal/lcd_ll.h:31` |
| Takt | `LCD_CLK = src / (N + b/a)` mit N = 2..256; `PCLK = LCD_CLK / M` mit M = 1..64; Quelle PLL160M, PLL240M oder XTAL | [belegt] `lcd_ll.h:28-30`, `:89-106`, `:118-126`, `:162-176`; `LIBS/include/soc/esp32s3/include/soc/clk_tree_defs.h:184-187` |
| DMA-Deskriptor | max. 4095 B pro Deskriptor, 4-Byte-aligned 4092 B; 3 Worte = 12 B pro Deskriptor | [belegt] `LIBS/include/hal/include/hal/dma_types.h:21-32`, `:58-59` |
| Ausgänge `LCD_DATA_OUT0..15` | über die GPIO-Matrix auf jeden Pin legbar | [belegt] `LIBS/include/soc/esp32s3/include/soc/gpio_sig_map.h:236-266` |

### 1.3 Weg A1: öffentliche `esp_lcd`-i80-API

Aufrufe: `esp_lcd_new_i80_bus()`, `esp_lcd_new_panel_io_i80()` und `esp_lcd_panel_io_tx_color(io, -1, buf, len)` (`LIBS/include/esp_lcd/include/esp_lcd_io_i80.h:25-37`, `:50`, `:66-101`; `esp_lcd_panel_io.h:72`).

Ein kontinuierlicher Bitstrom ist damit möglich. Die Datenphase dauert so lange, wie der DMA Daten liefert (`lcd_always_out_en`, `IDF components/esp_lcd/i80/esp_lcd_panel_io_i80.c:215`). Mit `lcd_cmd = -1` entfällt die Command-Phase (`:690-700`). FastLEDs S3-LCD_CAM-Treiber nutzt diesen Weg (s. 1.6).

Einschränkungen im verwendeten IDF-Stand:

| Punkt | Folge | Beleg |
|---|---|---|
| WR- und DC-GPIO müssen ≥ 0 sein, dazu alle 8 Daten-GPIOs. | **Gefordert sind gültige GPIO-Nummern, keine herausgeführten Pins.** Möglich sind ein interner GPIO, der weder herausgeführt noch von Flash oder PSRAM belegt ist (Schaltplan prüfen [offen]), D11/D12 (GPIO42/41, falls auf KNeoPiX frei [offen]) oder Umrouten: Nach `esp_lcd_new_i80_bus()` kann jeder Pin per `esp_rom_gpio_connect_out_signal()` neu verdrahtet werden. FastLED legt WR und DC auf GPIO0, einen Strapping-Pin. | [belegt] `esp_lcd_panel_io_i80.c:655-676`; `HWC/.../Xiao.h:388-405` (inkl. `:399-400`); FL `src/platforms/esp/32/drivers/i2s/i2s_lcd_cam_peripheral_esp.cpp.hpp:133-134` |
| PCLK ist nur als `src/2/M` einstellbar (Vorteiler fest 2, ganzzahlig, M ≤ 64): 80 MHz/M aus PLL160 oder 120 MHz/M aus PLL240. | 8 MHz ist erreichbar, 6,4 MHz nicht. Das Feintuning der Bitrate (2.3) ist stark eingeschränkt. | [belegt] `esp_lcd_panel_io_i80.c:304-306`, `:578-583`; `IDF components/esp_lcd/priv_include/esp_lcd_common.h:28` |
| Pro Transaktion **ein zusammenhängender Puffer** | Der ganze Frame braucht einen Block, das birgt Fragmentierungsrisiko. | [belegt] `esp_lcd_panel_io_i80.c:793-802` (Color-Pfad), `:519` |
| Die Fertigmeldung kommt über den LCD-`TRANS_DONE`-Interrupt. | Wann er relativ zum letzten FIFO-Eintrag feuert, zeigt der Code nicht → M3 [Schätzung] | [belegt] `:196-198`, `:746-749` |
| Der Interrupt wird ohne IRAM-Flag angelegt. | Bei Flash-Operationen kommt die Fertigmeldung verspätet. | [belegt] `esp_lcd_common.h:25`; `esp_lcd_panel_io_i80.c:195` |
| `tx_color()` blockiert mit `portMAX_DELAY`, wenn der Transaktionspool voll ist. | Davor ist ein eigenes Busy-Gate nötig. | [belegt] `:543-548`, `:560` |
| PSRAM-Puffer sind erlaubt (`access_ext_mem = true`). | Siehe 1.5, nicht empfohlen. | [belegt] `:614-617`, `:521-531` |

### 1.4 Weg A2: LCD_CAM-Register (`hal/lcd_ll.h`) und GDMA (`esp_private/gdma.h`, `gdma_link.h`)

So arbeitet NeoPixelBus (s. 1.6).

Vorteile gegenüber A1:
- Es werden nur die Datensignale geroutet, ohne WR- oder DC-Pin (NPB `src/internal/methods/NeoEsp32LcdXMethod.h:543-546`).
- Der Ganzzahlteiler `src/(N·M)` ist frei wählbar, aus 160 oder 240 MHz (`lcd_ll.h:118-176`).
- Der Frame kann über die Deskriptorkette auf **mehrere Teilpuffer** verteilt werden und verträgt damit Fragmentierung.
- Ungenutzte Lanes müssen nicht geroutet werden.
- Ein GDMA-Kanal mit `flags.isr_cache_safe` bedient seinen Interrupt auch bei abgeschaltetem Cache ([belegt] `LIBS/include/esp_hw_support/dma/include/esp_private/gdma.h:34`; `CONFIG_GDMA_ISR_HANDLER_IN_IRAM=1`, `sdkconfig.h:974`).

Nachteile:
- `esp_private` ist keine stabile API. Im verwendeten IDF ist `gdma_new_channel()` bereits `deprecated`; NPB ruft ihn noch auf, der Ersatz ist `gdma_new_ahb_channel()` ([belegt] `gdma.h:126`, `:481-482`; NPB `NeoEsp32LcdXMethod.h:432-436`).
- Takt-Enable, Reset und Fehlerpfad liegen bei uns.
- Die GDMA-EOF meldet früher als das LCD-Ende (Hinweis NPB `:246`), siehe R5.
- Ein Umstieg auf IDF 5.5 ist absehbar (auskommentierte Plattform `55.03.36`, [belegt] `OGM/platformio.esp32.ini:18`).

**Fazit:** Den kontinuierlichen Strom liefern beide Wege. A1 braucht dafür zwei gültige Hilfs-GPIOs oder Umrouten, einen zusammenhängenden Puffer und groben Takt. A2 braucht nichts davon.

### 1.5 Speicherort: interner SRAM oder PSRAM

| Fakt | Beleg |
|---|---|
| PSRAM ist aktiv. `malloc` > 4096 B landet **zuerst im PSRAM**, und der PSRAM-Heap hat kein DMA-Cap. | [belegt] `LIBS/qio_opi/include/sdkconfig.h:993-994`, `:1003-1004` |
| WiFi und LwIP versuchen PSRAM. | [belegt] `sdkconfig.h:1005` |
| Laut IDF-Doku (RGB-LCD auf derselben LCD_CAM-Einheit) teilen sich EDMA und CPU die PSRAM-Bandbreite. Zitat: „the LCD CANNOT function if the external memory cache is disabled“, als Beispiele nennt die Doku OTA- und NVS-Writes. Bei einem DMA-Unterlauf gibt das LCD Dummy-Bytes aus. | [belegt] https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32s3/api-reference/peripherals/lcd/rgb_lcd.html |
| `CONFIG_SPIRAM_XIP_FROM_PSRAM` ist nicht gesetzt. | [belegt] `sdkconfig.h`: nur `CONFIG_SOC_SPIRAM_XIP_SUPPORTED` (`:238`) |

Ein Unterlauf heißt bei WS2812 gedehnte Pegel, also Bitfehler oder vorzeitiges Latchen. OpenKNX schreibt Flash bei ETS-Download, Speichern und OTA. Daraus folgt:
- Der **codierte DMA-Puffer muss intern liegen**, explizit mit `heap_caps_malloc(..., MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA)`.
- Die Pixelpuffer der Strips dürfen bleiben, wo `malloc` sie hinlegt, weil nur die CPU sie beim Codieren liest.
- [Schätzung] Erwartet: GDMA liest aus internem SRAM auch während Flash-Operationen weiter. Prüfen mit M7.

**Speicherbudget:** Statisch belegt sind `.dram0.data` 23 052 B und `.dram0.bss` 48 600 B, zusammen 71 652 B. Bis `.dram0.heap_start` belegt das DRAM-Fenster einschließlich `.dram0.dummy` (63 488 B, IRAM-Spiegelung) **135 144 B**. Gemessen mit `xtensa-esp32s3-elf-size -A` auf `OAM/.pio/build/release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V15_IP/firmware.elf`.

Die 327 680 B im Board-JSON sind PlatformIOs Abrechnungsgröße, keine Heap-Obergrenze. **Wie viel interner DMA-Heap zur Laufzeit frei ist und wie groß der größte freie Block ist (nach WiFi, KNX und Strips), ist unbekannt** [offen]. Messung M5 ist Voraussetzung für jede Zusage zu Strip-Längen.

### 1.6 Was NeoPixelBus und FastLED tun

| | NeoPixelBus 2.8.4 `NeoEsp32LcdX8/X16` | FastLED master c426c953, S3-LCD_CAM |
|---|---|---|
| API | Register und private GDMA, eigene Deskriptorkette, GDMA-EOF-Callback setzt `lcd_start = 0` (NPB `NeoEsp32LcdXMethod.h:241-249`, `:319-360`, `:432-444`) | `esp_lcd` i80, `bus_width = 16`, WR und DC auf GPIO0 (FL `…/i2s/i2s_lcd_cam_peripheral_esp.cpp.hpp:131-160`) |
| Kodierung | **3 Takte pro Bit** in der Folge 1 / Bit / 0, also T0H = ⅓ und T1H = ⅔ der Bitzeit (NPB `:48-91`, `:54`) | Standard „wave8“ mit 8 Takten pro Bit und 16-Bit-Worten, das ergibt 384 B pro RGB-LED (FL `…/i2s/channel_driver_i2s.cpp.hpp:35-37`, `:378-385`); alternativ 3 Worte pro Bit (`:400-401`). Der ältere i80-Treiber („144 KB per 1000 LEDs“) ist `.disabled` (FL `…/lcd_cam/lcd_driver_i80.h.disabled:14-16`). |
| Takt | PLL240 mit `clkm_div = Bitzeit/3 · 240 MHz`; 1250 ns ergibt 100, also 2,4 MHz; Bruchteiler erlaubt (NPB `:375-415`) | Standard 2,4 MHz, sonst aus dem Chip-Timing berechnet (FL `…/i2s_lcd_cam_peripheral_esp.cpp.hpp:32-34`; `channel_driver_i2s.cpp.hpp:323-325`) |
| Speicher | 1 Puffer im internen DMA-RAM, 24 B pro Byte des längsten Strips, **Reset-Bytes im Puffer** (NPB `:288-311`, `:609-610`) | 2 Puffer, **PSRAM bevorzugt**, LOW-Tail im Puffer (FL `channel_driver_i2s.cpp.hpp:364`, `:383-389`; `i2s_lcd_cam_peripheral_esp.cpp.hpp:223-240`) |
| Ungleiche Längen | Gemeinsamer Puffer nach dem längsten Strip; jede Lane muss in jedem Frame neu geschrieben werden (`AlwaysUpdate()`, NPB `:649-655`) | Alle Lanes werden auf die längste aufgefüllt, nur RGB (FL `channel_driver_i2s.cpp.hpp:307-308`) |
| Blockierend? | `FillBuffers()` wartet mit `while (lcd_start) yield();` (NPB `:491-495`) | Bei Rekonfiguration `while (isBusy()) yield` (FL `:340-343`) |

**Übernehmen:**
- Registerweg ohne Zusatzpins
- Null-Tail am Pufferende

**Nicht übernehmen:**
- die 3-Takt-Kodierung, weil sie unsere Datenblattfenster verletzt (2.3)
- PSRAM-Puffer (1.5)
- blockierende Waits (Projektregel)
- Bruchteiler: er dithert, aus demselben Grund wie beim PIO-Integer-Divider (Skill `neopixel-driver`, Abschnitt 2). Auch IDF erzwingt ganzzahlig (`esp_lcd_panel_io_i80.c:578`).
- das Neuschreiben aller Lanes in jedem Frame (s. 4.4)

---

## 2. Kodierung

### 2.1 Prinzip

Der Bus ist 8 Bit breit, Datenleitung *k* führt Strip *k*. Ein Bus-Byte ist ein Abtastwert aller 8 Lanes und steht einen PCLK-Takt lang an (Dauer `tS`). Ein LED-Bit belegt `S` Takte, also gilt `S · tS = Bitzeit`.

- **Takt und `S` gelten für den ganzen Bus.**
- `n0` (High-Takte eines 0-Bits) und `n1` (High-Takte eines 1-Bits) sind **pro Lane frei**: Lanes mit gleicher Bitzeit dürfen also unterschiedliche T0H und T1H haben.

**Kodierung bei gemeinsamem `S`:**
1. Pro Byte-Position die 8 Lane-Bytes in 8 Bit-Ebenen `P` transponieren.
2. Takt `j` eines Bits ist dann `out[j] = A[j] | (P & B[j])`. Dabei enthält `A[j]` die Lanes mit `j < n0`, `B[j]` die Lanes mit `n0 ≤ j < n1`.
3. Lanes, die in diesem Frame nicht gesendet werden oder deren Daten schon zu Ende sind, fallen aus `A` und `B` heraus und liegen konstant auf 0.

Farbreihenfolge, Channel-Swap, TM1814-Präfix und SM16825-Settings stehen bereits im Lane-Puffer, wie bei RMT (`OFM/src/rmt/rmt_neopixel_serial.cpp:243-265`, `:474-505`). Der Bus fasst sie nicht an.

### 2.2 Bitzeit aus dem SerialTiming-Profil

Quelle bleiben `SerialTiming::profileFor()` und, bei ETS-Timing ≠ 0, `SerialTiming::scaledTo()` ([belegt] `OFM/src/SerialTimingProfile.h:261-338`, `:352-365`; Aufruf in `OAM/src/NeoPixelModule.cpp:4049-4069`).

Neu wäre `SerialTiming::solveLcd()`, analog zu `solvePio()` mit derselben Gewichtung `e0·2 + e1·2 + eb·3` ([belegt] `SerialTimingProfile.h:189-246`, `:228-229`). Die Funktion:
- sucht für **alle LCD-Lanes gemeinsam** Taktquelle, Ganzzahlteiler `N·M` und `S` (in den Grenzen `Smin..Smax`),
- setzt `n0` und `n1` pro Lane,
- **behandelt Datenblattfenster als harte Grenze, nicht als Kostenterm.** Mit reiner Fehlergewichtung wählt sie bei WS2805 und S = 10 `n1 = 8` (T1H 1000 ns = Fenster-Maximum), weil das 1 ns weniger Fehler bringt als `n1 = 7` (875 ns).

### 2.3 Quantisierung (Profile aus `SerialTimingProfile.h`, auf den nächsten Takt gerundet)

Datenblattfenster gibt es nur dort, wo sie im Code-Kommentar stehen: SK6812 `:265-266`, SK6805 `:271-273`, WS2814 `:277-279`, WS2805 `:292-294`. Alle Werte sind berechnet ([Schätzung], Skript `calc/grid.py` im Scratchpad) und vom Reviewer unabhängig nachgerechnet.

| Raster | Profil | S | T0H / T1H | Bit | Fenster |
|---|---|---|---|---|---|
| **125 ns (8 MHz)** | WS2812x/SK6812 400/800 | 10 | 375 / 750 | 1250 | SK6812 ok |
| | SK6805 350/650 | 10 | 375 / 625 | 1250 | ok |
| | WS2814 300/670 | 10 | 250 / 625 | 1250 | ok, aber T0L = 1000 = **Fenster-Maximum** |
| | WS2805 312/938 | 10 | 250 / 1000 → besser 875 | 1250 | mit n1 = 8 liegen T1H und T0L genau **auf dem Maximum**; n1 = 7 (875/375) liegt innen |
| | WS2811 800k 300/900 | 10 | 250 / 875 | 1250 | – |
| | TM1814/TM1914 360/720 | 10 | 375 / 750 | 1250 | – |
| | GS8208 / TM1829 (Bit 1200) | 10 | 250 / 875 bzw. 750 | **1250 (+4,2 %)** | – |
| | APA106 (Bit 1700) | 14 | 375 / 1375 | 1750 | anderes S |
| | WS2811_400KHZ (Bit 2500) | 20 | 500 / 1250 | 2500 | anderes S |
| **312,5 ns (3,2 MHz)** | WS2812x/SK6812 | 4 | 312,5 / 937,5 | 1250 | SK6812 ok (T1H 137 ns über Nominal 800) |
| | SK6805 | 4 | 312,5 / 625 | 1250 | ok |
| | WS2814 | 4 | 312,5 / 625 | 1250 | ok |
| | WS2805 | 4 | 312,5 / 937,5 | 1250 | ok, genau die Wellenform, die der Code-Kommentar als hardware-verifiziert führt (`:293-295`) |
| 416,7 ns (2,4 MHz, NPB) | WS2812x/SK6812 | 3 | 417 / 833 | 1250 | **SK6812 T0H > 400 verletzt** |
| | SK6805 | 3 | 417 / 833 | 1250 | **T1H > 750 verletzt, T1L 417 < 450 verletzt** |
| | WS2814 | 3 | 417 / 833 | 1250 | **T0H > 380 verletzt, T1L < 580 verletzt** |
| | WS2805 | 3 | 417 / 833 | 1250 | **T0H > 380 verletzt** |

**Ergebnis:** S = 3 scheidet aus. **S = 4 und S = 10 erfüllen beide alle im Code dokumentierten Fenster.**
- S = 4 braucht 40 % des Speichers, kann pro Lane aber nur in Schritten von 312,5 ns nachstimmen.
- S = 10 stimmt in 125-ns-Schritten nach.

Zum Vergleich: RMT realisiert die Profile auf 25-ns-Ticks nahezu exakt ([belegt] `rmt_neopixel_serial.cpp:14-16`, `:383-393`).

**ETS-Timing-Override:** Die Tabelle `kBitrateKhz` steht in `OAM/src/NeoPixelTimingSelection.h:14-17`, nicht als `timingFreqTable` in `NeoPixelModule.cpp`, wie CLAUDE.md sagt. Auf einem festen 125-ns-Raster fallen die 16 ETS-Werte auf **5 Raten** zusammen:

| S | realisierte Rate | ETS-Werte (kHz) |
|---|---|---|
| 8 | 1000 kHz | 960 |
| 9 | 888,9 kHz | 880, 920 |
| 10 | 800 kHz | 765-790, 800, **840** |
| 11 | 727 kHz | 720, 750, 760 |
| 12 | 666,7 kHz | 640, 680 |

Mit A2 kann `solveLcd` den Bustakt selbst verschieben. Beispiel: 240 MHz / 31 bei S = 10 ergibt 1291,7 ns ≈ 774,2 kHz (N = 31, M = 1). Das geht aber nur mit **einem** Wert für alle LCD-Lanes, daher Regel R-T in 3.2. Mit A1 bleibt es bei 80/M bzw. 120/M MHz.

### 2.4 Speicher pro Frame

`Bytes = S × 8 × max_k(Pufferbytes_k über die gesendeten Lanes) × (Busbreite / 8) + Tail`

Der Bedarf hängt **nur vom längsten LCD-Strip** ab, nicht von der Zahl der Lanes (≤ 8). Allokiert wird für den längsten registrierten Strip. Alle Zahlen sind berechnet und vom Reviewer bestätigt:

| Konfiguration | Bits/Lane | S = 10 (125 ns) | S = 4 (312,5 ns) | S = 3 (NPB) | Deskriptoren bei S = 10 | Leitungszeit |
|---|---|---|---|---|---|---|
| 8 × 300 RGB | 7 200 | 72 000 B | 28 800 B | 21 600 B | 18 | 9,00 ms |
| **8 × 300 RGBW** | 9 600 | **96 000 B** | 38 400 B | 28 800 B | 24 | 12,00 ms |
| **8 × 1024 RGB** | 24 576 | **245 760 B** | **98 304 B** | 73 728 B | 61 | 30,72 ms |
| 8 × 1024 RGBW | 32 768 | 327 680 B | 131 072 B | 98 304 B | 81 | 40,96 ms |

Ein 16-Bit-Bus verdoppelt alle Werte und bringt bei 8 Ports nichts. Deskriptoren: je 4092 B, 12 B pro Deskriptor.

**Tail:** einige Dutzend Null-Takte (z. B. 64 Takte ≈ 8 µs bei 125 ns), damit die 16-Einträge-FIFO leerläuft und die Leitung auf Idle geht. Ob das genügt, hängt vom Verhalten am Transferende im always_out-Modus ab [offen] (M1/M3). Der eigentliche Latch (bis 300 µs, `SerialTimingProfile.h:333`) läuft als Zeit-Gate ab Transferende (s. 4.4). Im Puffer würde er 2400 Bytes kosten.

**Einordnung ([Schätzung]):**
- 96 KB intern sind plausibel.
- 8 × 1024 RGB passt bei S = 10 (246 KB) sehr wahrscheinlich nicht, bei S = 4 (98 KB) möglicherweise.
- Die Entscheidung fällt mit M5. Daraus folgt D2: S richtet sich nach dem gemessenen Budget.

---

## 3. Gemischte Strips

### 3.1 Regeln auf dem Bus

| Fall | Regel |
|---|---|
| verschiedene Längen | Nach dem letzten Bit liegt die Lane konstant auf 0. Das ist Idle bzw. ein früher Latch und für den Chip harmlos. Eine Lane auf 0 ohne Bits ändert nichts: die LEDs behalten ihre gelatchte Farbe. |
| RGB / RGBW / RGBCCT / 16-Bit / Präfixe | Nur die Pufferlänge ist anders, also gilt dasselbe wie bei verschiedenen Längen. |
| verschiedene T0H/T1H bei gleicher Bitzeit | `n0` und `n1` pro Lane (2.1). Beispiel bei S = 10: WS2812B, SK6812, WS2814 und WS2805 auf einem Bus. |
| invertierte Chips (TM1814, TM1829, TM1914) | Invertierung pro Pin in der GPIO-Matrix (`out_inv`). Idle wird dadurch High, wie bei RMT `invert_out` ([belegt] RMT `rmt_neopixel_serial.cpp:359-361`; NPB `:544`). |
| andere Bitzeit (APA106, WS2811_400KHZ, GS8208/TM1829 außerhalb der Toleranz, ETS-Override ≠ Bus) | **muss auf RMT**. Ist RMT voll, wird der Strip abgelehnt. |
| WS2805 auf TXS0108E (V1) | **nie auf LCD**: Dieser Strip darf sich heute bewusst nicht mit anderen Ausgaben überlappen ([belegt] `OFM/src/PhysicalStrip.cpp:837-850`; Exklusiv-Phase `NeoPixelManager.cpp:1330`, `:1388-1430`). |
| APA102/SPI | unberührt, eigener Pfad (`DriverFactory.cpp:87-90`) |

### 3.2 Zuordnungsregel (Vorschlag)

- **≤ 4 serielle Strips: alles RMT wie heute.** Bestehende Installationen verhalten sich unverändert.
- **> 4 serielle Strips:** Die OAM (Produktcode, nicht Lib) prüft vor dem Anlegen alle Strip-Parameter (Protokoll, ETS-Timing, Länge, belegte Relais-Pins) und vergibt den Treiber explizit über `addStrip(..., DriverType, ...)` ([belegt] `OFM/src/NeoPixelManager.cpp:147`, `:313`). Heute wird dort ohne DriverType aufgerufen (`OAM/src/NeoPixelModule.cpp:4015`). Reihenfolge:
  1. Nicht LCD-taugliche Strips (3.1) bekommen RMT.
  2. Die übrigen RMT-Plätze gehen an die **längsten** Strips, weil der LCD-Speicher nur vom längsten LCD-Strip abhängt.
  3. Alle übrigen kompatiblen Strips gehen auf den LCD-Bus.
  4. Was weder RMT noch LCD bekommt, wird mit klarer Log-Meldung abgelehnt.
- **R-T (Toleranz):** Eine Lane ist LCD-tauglich, wenn die gemeinsame Buslösung ihre Bitzeit auf ≤ 2 % trifft und `n0`/`n1` innerhalb der bekannten Fenster liegen. Lanes mit ETS-Timing ≠ 0 kommen bevorzugt auf RMT, weil sie dort exakt umgesetzt werden.

Alternative „fest 1-4 RMT, 5-8 LCD“: einfacher, verschenkt aber Speicher und lehnt inkompatible Strips auf den Ports 5-8 ab, obwohl auf 1-4 ein RMT-Platz besser gepasst hätte.

---

## 4. Integration in OFM-NeoPixel (alles Lib-Änderung, erst nach GO)

### 4.1 Neue Teile neben `src/rmt/`

- `src/lcd/lcd_parallel_bus.{h,cpp}`: Singleton pro LCD_CAM. Enthält Lane-Registry (≤ 8), Bus-Plan (`solveLcd`), interne DMA-Teilpuffer und Deskriptorkette, Encoder, Start, Ende-Erkennung, Recovery und Diagnosezähler.
- `src/lcd/lcd_neopixel_lane.{h,cpp}`: `IHardwareDriver` pro Strip mit eigenem Pixelpuffer, `colorOrder`, `channelSwap`, Polarität, Level-Shifter-GPIO-Setup und Realwerten für die Konsole.
- `DriverImplementation::LCD_SERIAL` und `DriverType::SERIAL_PARALLEL` in `OFM/src/IHardwareDriver.h:69-89`. Den Namen im Switch `PhysicalStrip.cpp:686-699` ergänzen.
- **Gemeinsamer Puffer-Helfer:** `rgbToBuffer`, TM1814-Präfix und SM16825-Settings gibt es heute im RMT-Treiber (`rmt_neopixel_serial.cpp:18-30`, `:243-265`, `:474-505`) und im PIO-Treiber. Eine dritte Kopie wäre ein weiterer Zwilling (D7).

### 4.2 DriverFactory und Ressourcen

- `DriverFactory.cpp:82-86` bekommt unter `CONFIG_IDF_TARGET_ESP32S3 && SOC_LCDCAM_I80_LCD_SUPPORTED` einen Zweig `SERIAL_PARALLEL → LCD_NeoPixel_Lane`. `AUTO` bleibt RMT.
- `checkResourcesAvailable(protocol)` zählt heute jeden 1-Wire-Strip als RMT und nur initialisierte Strips (`NeoPixelManager.cpp:1826-1835`, `:1855-1882`). Die Prüfung steckt in **5** `addStrip`-Varianten (`:118`, `:161`, `:217`, `:262`, `:339`). Die Varianten ohne DriverType (`:104`, `:299`, `:327`) können RMT und LCD nicht unterscheiden. Alle Zwillinge müssen getrennte Zähler für RMT und LCD bekommen und gegen den angeforderten Typ prüfen.

### 4.3 PhysicalStrip, Konfiguration, Farbreihenfolge

- **Keine neue Konfig-Klasse:** `SerialStripConfig` bleibt (vgl. `rmt_neopixel_serial.cpp:615-710`).
- **Die Farbreihenfolge bleibt an drei Stellen:** `PhysicalStrip::_colorOrder`, `_config` und Lane-`colorOrder`. Es gelten die RMT-Regeln:
  - `applyConfig()` überschreibt nie mit `NONE` (`rmt_neopixel_serial.cpp:634-635`).
  - `PhysicalStrip::init()` ruft nach `driver->init()` (`PhysicalStrip.cpp:285`) `applyConfig(_config)` auf (`:293`).
  - Der Bus liest nur fertige Bytes, damit entsteht keine vierte Stelle.
- **Polarität:** wird vor `init()` gesetzt (`PhysicalStrip.cpp:282-283`), weil das GPIO-Routing beim Init entsteht.
- **Custom-Timing:** landet vor `init()` nur in `_config` (`PhysicalStrip.cpp:291-292`, `setCustomTiming` `:724-735`). Der Bus-Plan wird deshalb nach dem Init aller Lanes berechnet und bei Lane-Änderungen nur im Leerlauf neu aufgesetzt.

### 4.4 Frame-Sync mit dem bestehenden `show()`-Pfad

**Heute ([belegt]):**
- `updateAll()` läuft als `syncAll()` → `applyPowerLimit()` → `showAll()`. `syncAll()` geht nie in einen laufenden Frame (`NeoPixelManager.cpp:1488-1508`).
- `beginFrame()` ruft `show()` nur für **dirty** Strips auf und pollt danach einmal (`:1314-1352`, Dirty-Skip `:1331`, Poll `:1349-1351`).
- `show()` wird auch direkt aufgerufen, ohne dass danach `isBusy()` gepollt wird: `loopOffRefresh` (`:1541-1564`, einziger Pfad bei globalem AUS, `OAM/src/NeoPixelModule.cpp:341-349`), `neo test` (`NeoPixelConsole.cpp:1783-1801`) und `detectRGBW` (`PhysicalStripConfig.cpp:324-341`).

**Vorgeschlagene Semantik (Version 3, nach Review-Runde 2):**

0. **Gewählte Grundregel:** `isBusy()` einer LCD-Lane meldet „**Bus belegt**“ (Transfer oder Latch-Gate irgendeiner LCD-Lane), nicht nur die eigene Beteiligung. Daraus folgt die Invariante **`!isBusy()` ⇒ ein `show()` außerhalb eines Batches wird angenommen**. Davon hängt die LED-Abschaltung der OAM ab (`OAM/src/NeoPixelModule.cpp:526-553`): Sie prüft alle Strips auf `isBusy()`, blankt dann **einen** Strip und ruft `show()` auf; bei `false` loggt sie nur und zählt weiter (`:552-553`). Mit „busy nur bei Beteiligung“ würde eine nicht beteiligte Lane bei belegtem Bus als frei gelten, ihr `show()` würde abgewiesen und der Strip bliebe hell. Die Alternative „im Batch immer `true`, `commit()` wartet nicht-blockierend und die Deadline startet erst beim DMA-Start“ wird verworfen: Sie verschiebt das Busy-Wissen in den Manager und ändert die Bedeutung von `show()` = gestartet.
1. **Batch statt „sofort alles senden“.**
   - Der Manager öffnet einen Batch, ruft für alle betroffenen Strips `show()` auf und schließt ihn mit `bool commit()`. Das ist eine kleine Lib-Änderung, ein statischer Helfer, der ohne LCD-Bus nichts tut.
   - Im Batch prüft `show()` einer LCD-Lane nur, ob der Bus frei ist. Ist er frei, wird die Lane markiert und `true` zurückgegeben, aber nur vorläufig. Ist er belegt, gibt `show()` `false` zurück.
   - `commit()` codiert **nur die markierten Lanes**, startet den Transfer und gibt `true`/`false` zurück.
   - Außerhalb eines Batches startet `show()` sofort, nur mit dieser Lane.
   - **Nicht markierte Lanes liegen konstant auf 0** und behalten ihre Farbe (3.1). Dadurch:
     - wirkt `_suppressUnchangedFrames` weiter;
     - bleibt das ETS-OffRefresh pro Strip (`OAM/src/NeoPixelModule.cpp:4137`, 5 s, `NeoPixelManager.h:158`);
     - bekommen saubere Lanes und Relais-Strips keine Pulse.
     - Konstant 0 heißt „Farbe halten“, und das ist gewollt.
2. **Fehlerpfad von `commit()` (HIGH):** `beginFrame()` sammelt heute jeden Strip mit `show() == true` in `_startedStrips` (`NeoPixelManager.cpp:1333-1343`). `advanceParallel()` markiert jeden, der nicht busy ist, als gesendet (`:1373-1374`). Liefert `commit()` `false`, setzt der Manager deshalb `_frameSuccess = false`, erhöht `_errorCount` und **entfernt die markierten LCD-Lanes aus `_startedStrips`**. Sie bleiben dirty und werden im nächsten Frame wiederholt. Sonst zählte eine nie gesendete Lane als gesendet und bliebe mit `_suppressUnchangedFrames` hängen. Zusätzlich führt jede Lane ein Flag „im letzten Transfer tatsächlich gesendet“, das `markFrameSent()` prüft. Das ist eine zweite Sicherung, falls ein künftiger Aufrufer den Rückgabewert ignoriert.
3. **Position von `commit()` in `beginFrame()`:** nach der Show-Schleife (`NeoPixelManager.cpp:1325-1344`) und **vor** `_phaseDeadlineMs = millis() + deadline` (`:1346`). Dann verbraucht die Codierzeit nicht die Reserve der Deadline.
4. **Alle Aufrufer, die mehrere Strips nacheinander zeigen**, bekommen den Batch ([belegt] `grep -- "->show()"` über `OFM/src` und `OAM/src`):

   | Aufrufer | Stelle | Batch nötig |
   |---|---|---|
   | `NeoPixelManager::beginFrame` | `NeoPixelManager.cpp:1325-1344` | ja |
   | `NeoPixelManager::loopOffRefresh` | `:1555-1565` | ja |
   | `VirtualStrip::show` → `pstrip->show()` je Physical | `VirtualStrip.cpp:630-649`; Aufruf in OAM `showVirtualStripIfIdle` `NeoPixelModule.cpp:399-400`. **Ein** VirtualStrip enthält dort alle Strips. Ohne Batch startet die erste LCD-Lane den Bus, und alle weiteren LCD-Lanes werden abgewiesen und nie wiederholt. | ja; Rückgabe = `allSuccess && commit()` |
   | `test/AnimationTest.cpp:138-139` (zwei Strips direkt hintereinander, nur mit `OPENKNX_NEOPIXEL_AUTO_TEST`) | Test | ja |
   | `test/Benchmark.h:201-221` (Schleife über Strips) | Test | ja |
   | `advanceExclusive` (`:1416`) | nur TXS0108E-WS2805, nie LCD (3.1) | nein |
   | OAM LED-Abschaltung (`NeoPixelModule.cpp:552`) | ein Strip pro Durchlauf, vorher Busy-Scan | nein (Regel 0) |
   | Konsole (`NeoPixelConsole.cpp:1783-1801`, `:5312`-`:5920`), `PhysicalStripConfig.cpp` (`:44`-`:340`), Timing-Scan `NeoPixel.cpp:256` | je ein Strip | nein, s. Punkt 7 |
5. **Folge von Regel 0 für diese OAM:** Per-Lane-Busy hätte hier ohnehin nichts gebracht. Die OAM hat **einen** VirtualStrip über alle Strips. `syncToPhysical` überspringt einen busy Strip und meldet Fehler (`VirtualStrip.cpp:528-531`), und `updateAll` bricht dann vor `showAll` ab (`NeoPixelManager.cpp:1501-1505`). Das gilt heute für jeden busy RMT-Strip genauso. Mit dem LCD-Bus ist ein laufender Konsolen- oder OffRefresh-Transfer einer Lane also, wie bisher bei einem RMT-Strip, ein ausgesetzter Render-Tick. Auch die Einschränkung bei `allOff()` (`flushFrame` kehrt bei Phase Idle sofort zurück, `:1450`, `:1662-1672`) besteht heute schon und wird nicht schlechter.
6. **`show()` während eines laufenden Transfers** gibt `false` zurück, wie RMT-busy (`rmt_neopixel_serial.cpp:510`). Es gibt **keine** Regel „seit dem Start nicht beschrieben → true“. Begründung: Rohzugriffe über `PhysicalStrip::getBuffer()` (`PhysicalStrip.cpp:627-631`) und die ABL-Skalierung in place (`NeoPixelManager.cpp:1193-1219`) sind für den Treiber unsichtbar, und `markFrameSent` (`:1374`) würde sonst nie gesendete Daten verbuchen.
7. **Einzel-Strip-Aufrufer in der Konsole** (`neo test` `NeoPixelConsole.cpp:1783-1801`, `detectRGBW` `PhysicalStripConfig.cpp:319-341`): Läuft gerade ein Render-Frame, wird ihr `show()` abgewiesen. Bei RMT ist das heute genauso. Vorschlag für **alle Protokolle**: vor jedem Schritt ein begrenztes `PhysicalStrip::waitForTransfer()` (Deadline aus dem Treiber, `PhysicalStrip.cpp:558-578`), danach eine Log-Zeile, wenn `show()` trotzdem `false` liefert. Heute loggt `neo test` das Ergebnis nur über `logWs2805TestFrame`.
8. **Latch-Gate nach Uhr, nicht nach Poll:** `show()` und `isBusy()` werten beide selbst aus, ob „fertig und `now − Ende ≥ Reset`“ gilt. Bei RMT wird `waitingForReset` nur in `isBusy()` gelöscht, und `show()` verweigert, solange es gesetzt ist (`rmt_neopixel_serial.cpp:510`, `:561-565`). Ein `show()`-Muster ohne Poll bleibt dort deshalb nach dem ersten Frame hängen. Das ist ein **Verdacht auf einen bestehenden RMT-Fehler**, nur per Code-Trace ermittelt [offen] und getrennt zu prüfen (D10).
9. **`getTransferTimeoutUs()`:** muss den **Bus-Frame** abdecken (längste registrierte LCD-Lane + Latch + Reserve), nicht die eigene Länge, weil `isBusy()` nach Regel 0 den ganzen Bus meldet. Sonst setzt `beginFrame()` die Deadline zu knapp (`NeoPixelManager.cpp:1342-1347`), und `advanceParallel()` bricht den Frame ab (`:1359-1371`).
10. **`setPixel()` und `clear()` während eines Transfers** sind unkritisch, weil der DMA nur den codierten Puffer liest. Anders bei RMT: dort streamt der Encoder direkt aus dem Pixelpuffer (`rmt_neopixel_serial.cpp:584-587`).
11. **Konsole und Timing-Scan während eines laufenden Frames (Review-Runde 3, MED):** `clear()` + `show()` in der Konsole (z. B. `NeoPixelConsole.cpp:5311-5312`) oder im Timing-Scan (`NeoPixel.cpp:256`) während eines Render-Frames bricht den Frame fälschlich ab, weil der Konsolen-Transfer die Lanes in `_startedStrips` (Regel 0: Bus belegt) erneut busy macht und `advanceParallel()` nach Ablauf der Deadline abbricht (`NeoPixelManager.cpp:1363-1370`). **Entscheidung:** Ein blockierendes Warten auf `!isFrameInFlight()` würde nie enden, weil `_framePhase` nur in `showAll()` weiterläuft (`NeoPixelManager.h:174`). Deshalb gilt:
   - **Konsolen-Aufrufer** rufen vor dem Zeigen `_manager->flushFrame(timeout)` auf (`NeoPixelManager.cpp:1448-1482`). `flushFrame()` treibt `showAll()` selbst an und ist zeitlich begrenzt.
   - **Der Timing-Scan** wartet nicht. Ist `isFrameInFlight()` gesetzt, überspringt er den Durchlauf und zeigt im nächsten `NeoPixel::loop()`. Der Scan läuft dort nach dem `showAll()`-Aufruf (`OFM/src/NeoPixel.cpp:113`, `:116-120`). So verschiebt auch `showVirtualStripIfIdle` (`OAM/src/NeoPixelModule.cpp:393-396`).
   - `advanceParallel()` wird nicht umgebaut. Das Flag „tatsächlich gesendet“ sichert nur `markFrameSent()` ab.
12. **„Tatsächlich gesendet“ als Schnittstelle:** neue virtuelle Methode in `IHardwareDriver`, z. B. `bool wasLastShowSent() const`. Die Vorgabe `true` gilt für RMT und PIO, die LCD-Lane überschreibt sie. `PhysicalStrip::markFrameSent()` fragt sie ab.
13. **Batch-Helfer mit Verschachtelungszähler:** `begin()` erhöht den Zähler, `commit()` verringert ihn, und erst bei 0 wird codiert und gestartet. Grund: Ein `VirtualStrip::show()` innerhalb eines Manager-Batches darf nicht vorzeitig senden. `commit()` ohne markierte Lane gibt `true` zurück und tut nichts. Läuft `VirtualStrip::show()` verschachtelt in einem äußeren Batch, ist sein `true` nur vorläufig. Ob die Lanes wirklich gesendet wurden, entscheidet erst das äußere `commit()`, das dazu das Flag „tatsächlich gesendet“ setzt. Scheitert `commit()` dauerhaft, wird das gedrosselt geloggt (z. B. höchstens einmal pro Sekunde, mit Zähler).
14. **Benchmark-Tests:** Die Schleifen in `test/Benchmark.h:96`, `:155`, `:282`, `:312`, `:446` und `:509` brauchen `waitForTransfer()` mit der Deadline aus dem Treiber. Das feste `waitForTransfer(5)` (5 ms, `test/Benchmark.h:359`) ist kürzer als ein Bus-Frame (12 ms bei 8 × 300 RGBW). Die Messung bei `:368-370` erfasst nur die Startzeit von `show()`.

### 4.5 Nicht blockierend, Watchdog

- Im Lane-Pfad wird nie gewartet. CPU-Last entsteht nur durch das Codieren in `commit()`.
  - [Schätzung] 8 × 300 RGBW (96 000 Ausgabebytes): ≈ 1-2 ms bei 240 MHz. Allein die Stores kosten ≈ 0,4 ms bei einem Store pro Takt.
  - [Schätzung] 8 × 1024 RGB bei S = 10: ein Mehrfaches davon.
  - Gemessen wird mit M6.
- **Auf mehrere Loop-Durchläufe verteilen bringt mit einem Puffer keine Überlappung**: Codieren kann die Leitungszeit desselben Puffers nicht nutzen und verschiebt nur den Start. Verteiltes Codieren müsste außerdem `show()` = gestartet neu definieren und die Deadline erst ab DMA-Start laufen lassen. Echte Überlappung kostet einen zweiten Puffer (doppelter Speicher) oder Streaming (4.7). Phase 1: in einem Stück codieren, sofern M6 ≤ ~2 ms ergibt.
- **Recovery** wie bei RMT: Deadline überschritten → GDMA stoppen und zurücksetzen, LCD-FIFO zurücksetzen, Leitungen für die Reset-Zeit auf Low, Lane-Zustand zurücksetzen. Einzige Wartezeit ist ein begrenztes `delayMicroseconds(resetUs ≤ 300)` ([belegt] Vorbild `rmt_neopixel_serial.cpp:545-558`).
  - Bei A2 meldet ein GDMA-Kanal mit `isr_cache_safe` das Ende auch während Flash-Operationen (1.4). Falsche Stalls durch eine verspätete Meldung entfallen dann.
  - Ob das LCD-Startbit sich selbst löscht und als Status taugt, ist unbelegt [offen]: NPB löscht es in Software (`NeoEsp32LcdXMethod.h:247`), esp_lcd liest es nie.
- **Grenzen:** Task-WDT 5 s ([belegt] `sdkconfig.h:1045`), OpenKNX-Watchdog ~16 s (CLAUDE.md). Alle Wartezeiten liegen im ms-Bereich.

### 4.6 Konsole und realisierte Wellenform

Heute gibt `neo phys timing <id> info` auf dem ESP32 nur „ERROR: Timing info only available on RP2040/RP2350“ aus ([belegt] `OFM/src/NeoPixelConsole.cpp:5076-5087`, `:5245-5246`). Dabei führt der RMT-Treiber die Realwerte bereits (`rmt_neopixel_serial.h:67-71`). Vorschlag: ein ESP-Zweig für **beide** Backends.

- **RMT:** Kanal, Tick 25 ns, realisierte T0H/T0L/T1H/T1L, Bit, Latch, Polarität, DMA.
- **LCD:**
  - Lane und GPIO, Quelle, Teiler, PCLK, `tS`, `S`, `n0`/`n1`
  - realisierte T0H/T0L/T1H/T1L, Bit in kHz, Latch, Polarität
  - Frame-Bytes, Deskriptoren, Speicherort
  - Codierzeit (letzte und maximale), Zähler für Stalls und Recovery
- **Zwillinge, die mitgezogen werden müssen:**
  - `neo info` Ressourcen (`NeoPixelConsole.cpp:1316-1320`)
  - RMT-Hardwarezeile (`:1462-1472`)
  - Tuner-Text „All four values are applied independently“ (`:4993-4998`): Auf LCD-Lanes werden T0H/T1H gerastert, T0L/T1L folgen aus der Bus-Bitzeit, eine Bitratenänderung einer einzelnen Lane wird abgelehnt.
  - Timing-Scan `NeoPixel::loopTimingScan` (`OFM/src/NeoPixel.cpp:184` ff.): Für LCD-Lanes wird er gesperrt oder auf `n0`/`n1`-Schritte umgestellt.
  - Konsolen-`addStrip` (`NeoPixelConsole.cpp:2846`)

### 4.7 Phase-2-Option „Streaming“ (nur bei Bedarf)

- Prinzip: Deskriptorring (2-4 × 4 KB), Nachcodieren im GDMA-EOF-ISR. Der Speicher wird damit nahezu unabhängig von der Länge.
- Bedingungen: Der ISR muss cache-sicher sein (`isr_cache_safe`), und die Quellpuffer müssen intern liegen, weil PSRAM bei Flash-Operationen nicht erreichbar ist (1.5).
- Budget pro 4-KB-Block bei 8 MB/s: 512 µs.
- Deutlich riskanter, deshalb nicht in Phase 1.

### 4.8 Konflikte auf KNeoPiX S3

| Ressource | Befund |
|---|---|
| LCD_CAM-Pins | A2 routet nur die 8 Strip-GPIOs 2-9 über die GPIO-Matrix, ohne WR, DC oder CS. GPIO3 ist ein Strapping-Pin; er wird nur beim Reset gelesen, der Ausgang entsteht erst nach dem Init ([belegt] `Xiao.h:388-398`). |
| Kamera / LCD | In OFM, OAM, OGM-Common und OFM-Network nicht verwendet ([belegt] `grep esp_lcd|LCD_CAM|esp_camera`: keine Treffer). |
| TP-UART | GPIO43/44 (`Xiao.h:606-609`), unabhängig von LCD_CAM. Wenige ms Codierzeit liegen weit unter dem TP-UART-Pufferbudget (Projektnotiz „Ring 256 B ≈ 347 ms“) [Schätzung]. |
| WiFi | Keine gemeinsame Hardware. Risiken nur EMI und Versorgung (M8). |
| USB | `ARDUINO_USB_MODE=1` (HW-CDC/JTAG), kein Konflikt. |
| GDMA | 5 TX-Kanäle (`soc_caps.h:167`). LCD braucht einen, RMT-DMA nutzt einen nur für WS2805 (`rmt_neopixel_serial.cpp:145-153`). Der APA102-Pfad nutzt Arduino-`SPIClass` ohne GDMA (`HW_NeoPixel_SPI.cpp:308-340`). |
| Relais (`NEOPIXEL_EXT_RELAIS=4`) | Belegen Ports aus derselben Port-Zuordnung (`OAM/src/NeoPixelModule.cpp:3480-3560`, `mapPortIndexToGpio`). Ein Relais-Port wird nie LCD-Lane. |

---

## 5. Risiken und Messplan

### 5.1 Risiken

| # | Risiko | Gegenmaßnahme |
|---|---|---|
| R1 | Interner DMA-Speicher reicht nicht oder ist fragmentiert. | A2 mit Teilpuffern; lange Strips auf RMT (3.2); S nach Budget (D2); beim Init klar ablehnen. |
| R2 | Rasterabweichung (z. B. WS2805 T0H 250 statt 312, GS8208 +4,2 %); Fenster-Randwerte bei S = 10. | Solver mit harten Fenstern (2.2), Regel R-T, Plateau-Scan M2. |
| R3 | 8 gleichzeitige Flanken: SSO und Ground-Bounce, besonders im TXS0108E (V1). | M4; LCD auf V1 erst nach M4 (D8); optional Lanes um 1 Takt versetzen. |
| R4 | `esp_private`-API ändert sich beim Umstieg auf IDF 5.5. | Versions-Guard und `static_assert` auf die IDF-Version; Header-Diff als Pflichtschritt des Upgrades. |
| R5 | GDMA-EOF kommt vor dem letzten FIFO-Byte (NPB `:246`); Verhalten am Transferende ist unbekannt. | Null-Tail ≥ FIFO-Tiefe, Latch-Gate ab EOF + FIFO-Laufzeit; M3 prüft das letzte Bit der längsten Lane. |
| R6 | Pegel beim Routen, beim Boot und zwischen Frames unbekannt („erste LED blitzt“). | M1; notfalls Pins erst nach einem Null-Transfer routen. |
| R7 | PSRAM und Flash-Operationen (1.5) | Codierter Puffer nur intern. |
| R8 | Codierzeit blockiert `loop()`. | M6; bei > 2 ms zweiter Puffer oder Streaming, kein bloßes Verteilen (4.5). |
| R9 | Recovery betrifft alle Lanes im laufenden Transfer. | Zähler pro Bus; Ursache analysieren. |
| R10 | NPB-Hinweis: „Dummy phase(s) MUST be enabled for DMA to trigger reliably“ (NPB `:426-429`). | Im Prototyp beide Varianten testen. |
| R11 | Strips hinter abgeschaltetem Relais: Pulse auf DIN könnten rückspeisen. | Durch Batch-Semantik (4.4) nur bei echten `show()`; prüfen, wie Relais und Strip-Versorgung zusammenhängen [offen]. |

### 5.2 Bench-Messplan

| # | Messung | Aufbau / Kriterium |
|---|---|---|
| M1 | Idle- und Boot-Pegel, Pegel zwischen Frames, invertierte Lane | Logic-Analyzer, 8 Kanäle, ≥ 100 MS/s. Kriterium: kein Puls außerhalb eines Frames. |
| M2 | T0H, T1H und Bit pro Lane gegen `neo phys timing <id> info` | Oszilloskop an 2 Lanes hinter dem Level-Shifter. Abweichung ≤ ±1 Takt, Bit-Jitter 0. Plateau-Scan `n0`/`n1` ±1 mit WS2812B, SK6812 (Original und Klon), WS2814, WS2805, TM1814. |
| M3 | Frameende | Letztes Bit der längsten Lane vollständig, Latch ≥ Profilwert, Lanes auf 0 behalten ihre Farbe. |
| M4 | Gleichzeitiges Schalten | Masse- und Versorgungsripple, Übersprechen V1 (TXS0108E) gegen V1.5 (SN74LV8T245). |
| M5 | Speicher | `heap_caps_get_free_size` und `heap_caps_get_largest_free_block` für `MALLOC_CAP_INTERNAL|MALLOC_CAP_DMA`, jeweils vor und nach WiFi, KNX und Bus-Init. |
| M6 | CPU | Codierzeit min/mittel/max in µs, Loop-Zeit, FPS bei 8 × 300 RGBW. |
| M7 | Flash-Stress | ETS-Download, Speichern und OTA über KNX während laufender Effekte. Kriterium: keine Glitches, Stall-Zähler 0. |
| M8 | WiFi-Stress | Dauer-Traffic und OTA über IP. Kriterium: keine Bitfehler; KNX-Verluste gegen RMT-only vergleichen. |
| M9 | Dauerlauf | 24 h; Uptime und Reset-Grund prüfen. |
| M10 | Regression | ≤ 4 Strips: Wellenform identisch zu heute. RP2350- und ESP-Build grün, beide flashen. |

### 5.3 Schrittplan und Aufwand ([Schätzung] Schätzung)

| Schritt | Inhalt | Aufwand |
|---|---|---|
| S0 | Entscheidungen D1-D10 | 0,5 PT |
| S1 | Prototyp außerhalb der Lib (Test-Env im OAM): A2, 8 Lanes, feste Muster, S = 10 und S = 4, interne Teilpuffer, Konsolen-Dump | 2-3 PT |
| S2 | Bench M1-M6 am Prototyp | 1-2 PT |
| S3 | OFM-Integration nach GO: `solveLcd`, Bus, Lane, Factory, Ressourcen-Zwillinge, Batch-Hook, Puffer-Helfer, Konsole | 5-7 PT |
| S4 | OAM-Zuordnung (`NeoPixelModule.cpp` und Konsolen-Zwilling), Logs | 1-2 PT |
| S5 | Adversariales Review, M7-M10, Doku (Skill `neopixel-driver`, `doc/`, `Help_de`) | 2-3 PT |
| | **Summe** | **≈ 12-18 PT** |

---

## 6. Offene Entscheidungen (mit Empfehlung)

| # | Frage | Optionen | Empfehlung |
|---|---|---|---|
| D1 | API | A1 `esp_lcd`: öffentlich, braucht 2 gültige Hilfs-GPIOs oder Umrouten, einen zusammenhängenden Puffer, PCLK 80/M bzw. 120/M, Ende-ISR nicht cache-sicher · A2: LL + GDMA | **A2**: fragmentierungsfest, freie Ganzzahlteiler (Bitrate einstellbar), cache-sicherer Ende-ISR, keine Hilfspins; IDF-Version absichern (R4). |
| D2 | Raster | S = 10 (125 ns, feine Lane-Stimmung) · S = 4 (312,5 ns, 40 % Speicher; alle Code-Fenster eingehalten) · Bus-Solver | **Bus-Solver: größtes S ≤ 10, dessen Frame ins gemessene Budget (M5) passt und alle Fenster einhält; Untergrenze S = 4.** |
| D3 | Zuordnung | fest 1-4 RMT / 5-8 LCD · alles LCD · Scan-Regel 3.2 | **Scan-Regel 3.2**: ≤ 4 Strips RMT wie heute, sonst inkompatible und längste Strips auf RMT. |
| D4 | Toleranz der Bitzeit | ±2 % · ±5 % · exakt | **±2 %**; ETS-Timing ≠ 0 bevorzugt auf RMT. |
| D5 | Speicher reicht nicht | ablehnen · PSRAM-Fallback · Streaming | **Ablehnen mit Log** in Phase 1; Streaming nur bei echtem Bedarf. |
| D6 | ETS-Parameter „Ausgabe-Backend“ | jetzt · später | **später**, braucht Versionssprung mit `ReplacesVersionsDownTo`. |
| D7 | Puffer-Helfer RMT/LCD/PIO zusammenführen | ja · nein | **ja**, im selben Lib-Change. |
| D8 | LCD auf V1 (TXS0108E) | erlauben · erst nach M4 · aus | **erst nach M4**, bis dahin nur V1.5. |
| D9 | Frame-Sync | Batch um jeden Mehr-Strip-Aufrufer mit `bool commit()` (4.4 Punkt 4) · alle Lanes immer senden (NPB-Stil) | **Batch**, `isBusy()` = Bus belegt (Regel 0), `commit()`-Fehler entfernt Lanes aus `_startedStrips`; Lanes auf 0 halten ihre Farbe; OffRefresh, Dirty-Filter und Relais-Strips bleiben pro Strip. |
| D10 | Vorhandene Lib-Fehler (vor LCD separat beheben) | a) Teil-Init macht das Modul dunkel (`NeoPixel.cpp:79-85`) · b) Verdacht: RMT-Latch-Gate nur über `isBusy()` | **beide separat** analysieren und fixen; a) ist bei mehr als 4 Strips auf dem S3 der heutige Totalausfall. |

---

## Anhang: Review-Befunde und Einarbeitung

Ein unabhängiger Reviewer hat alle Zahlen nachgerechnet und Zitate sowie API-Aussagen gegen die installierten Header und den IDF-Commit geprüft. Eingearbeitet wurden:
- §0: Das ganze Modul bleibt dunkel, nicht nur Strip 5-8 (D10a).
- Frame-Sync umgebaut: Batch-Hook, Lanes auf 0 halten die Farbe, Busy pro Lane, Latch-Gate nach Uhr, keine Regel „nicht beschrieben → true“.
- Fenster-Randwerte bei S = 10; S = 4 hält ebenfalls alle Fenster ein (D2 neu); die WS2811-Zeile ergänzt.
- ETS-Collapse um 840 kHz ergänzt, Tabelle der 5 Raten.
- A1-Pinargument abgeschwächt (gültige GPIO-Nummer genügt, D11/D12, Umrouten unabhängig von der Reihenfolge).
- TRANS_DONE-Zeitpunkt, 64-Byte-Tail und LCD-Startbit als unbelegt markiert; GDMA `isr_cache_safe` ergänzt.
- Aussage zum verteilten Codieren korrigiert; CPU-Schätzung auf 1-2 ms angehoben.
- Speicherbudget: 135 144 B DRAM-Fenster statt Vermischung mit IRAM.
- Zwillinge ergänzt (5 `addStrip`-Varianten, Konsole `:1462-1472`, Timing-Scan).
- Zeilenkorrekturen: `PhysicalStrip.cpp:282-283`, `:285`/`:293`, `:291-292`; `NeoPixelManager.cpp:1829`, `:1855-1882`; esp_lcd Color-Pfad `:793-802`.

**Review-Runde 2** (gegen Version 2), eingearbeitet in 4.4:
- **HIGH:** `commit()` gibt `bool` zurück. Bei `false` werden die LCD-Lanes aus `_startedStrips` entfernt und `_frameSuccess` auf `false` gesetzt; zusätzlich ein Flag pro Lane „tatsächlich gesendet“ für `markFrameSent`.
- **HIGH:** Der Batch umschließt **jeden** Aufrufer, der mehrere Strips zeigt. Das betrifft auch `VirtualStrip::show()`, das die OAM über `showVirtualStripIfIdle` für alle Strips aufruft. Die Aufrufer sind in einer Tabelle aufgelistet.
- **MED:** `isBusy()` meldet jetzt „Bus belegt“. Damit hält die Annahme der LED-Abschaltung in `NeoPixelModule.cpp:526-553`.
- **MED:** Per-Lane-Busy hätte bei einem einzelnen VirtualStrip nichts gebracht; die Einschränkung bei `allOff()` ist dieselbe wie heute.
- **MED:** `commit()` steht vor dem Setzen der Deadline.
- **LOW:** Konsole: begrenztes `waitForTransfer()` und Log-Zeile für alle Protokolle.

**Review-Runde 3** (gegen Version 3): Die sechs Punkte aus Runde 2 sind als behoben bestätigt. Eingearbeitet in 4.4, Punkte 11-14:
- Konsole: `flushFrame(timeout)` vor dem Zeigen; Timing-Scan: überspringen und im nächsten Durchlauf zeigen. Die erste Fassung dieses Punkts, ein blockierendes Warten auf `!isFrameInFlight()`, hätte sich aufgehängt. Das fand die Schlussprüfung.
- Bei Verschachtelung ist das `true` eines inneren `VirtualStrip::show()` nur vorläufig.
- `IHardwareDriver`-Methode für „tatsächlich gesendet“.
- Verschachtelungszähler im Batch-Helfer.
- `commit()` ohne Lane gibt `true` zurück.
- Gedrosseltes Log bei Dauerfehlern.
- Wartezeiten in den Benchmark-Tests.

