# Changes


## ec/ALPHA-DEV-v2.0.0: 2026-10-31 (not released, see doc/TODO.md Prio 0 and 1)

ETS product **2.0** (`NeoPixel-Dev`), release variant **0.15** (`NeoPixel-Release`). Alpha dev build for testers,
not a public release: the Prio 0 and Prio 1 items of `doc/TODO.md` are still open; it carries everything since the tag `v0.3`
(2026-06-21). The parameter layout, the object numbers and the flash layout changed on the way, so a device needs
this knxprod and a full ETS download after the firmware update.

**Migration — read before updating**

What a device needs depends on the version it comes from.

* Dev applications before 0.14 cannot be updated (`ReplacesVersionsDownTo="0.14"`): ETS only offers "Applikationsprogramm ändern", which resets every parameter and drops every link, so configure these devices again. Release updates from 0.1; whether the effect parameters of Release 0.1–0.8 (and of a Dev 0.14 build of 2026-09-02 before `2e7361c`) land on the right parameters is not checked yet (`doc/TODO.md`)
* Update the device in ETS with "Aktualisieren", not "Applikationsprogramm ändern": only the update takes the parameter values over
* From an application before Dev 1.1 / Release 0.11, LightManager 0.4: after the ETS update every light manager is deactivated; activate it again under "Kanalauswahl", its settings are kept
* Set the port selection of the LED strips and relays again: the port parameters of each hardware are numbered by its position in the hardware list, and that list changed (KNeoPiX V1.5 added, REG2 and the PiPico BCU Connectors removed, the product now lists every hardware)
* Effektkette, firmware from before 2026-10-20: update all devices of a chain together. A master with older firmware always reports OFF and a slave with new firmware then switches off; a slave with older firmware restarts its effect on every telegram of the master
* The current objects changed from DPT 7.600 to 7.012 (mA) with Dev 1.2 / Release 0.12: check the group addresses linked to them
* From an application before 0.5 (Release 0.3/0.4): object numbers moved. A segment has 53 objects instead of 40 (segment n moves by 13 × (n−1)), the LightManager objects moved by +210 (single objects, 1252 → 1462) and +240 (channels, 1260 → 1500), and the Effektmanager control objects (Start/Stop/Status/CueSet/EffektText/SyncChain) are segment objects K42–K52. ETS moves group addresses by object number, so check these links. Timing values 9 and 10 (old 960 duplicate / 1000 kHz) map to 750/765 kHz; check the timing
* RP2040/RP2350 release firmware (KNeoPiX V1/V1.5, OpenKNXiao Mini, UP1) from before 2026-08-25: the KNX region moved to `0x0E0000` and the filesystem grew. After the firmware update the device needs an ETS download; "Letzter Zustand" and the filesystem content are gone. The debug envs keep the old regions, so switching between a debug and a release image moves them as well
* ESP32: what moves depends on the firmware the device runs; the boundary is the build of 2026-09-03 19:11 (commit `420ad99`). OTA never rewrites the partition table, so where something moves, flash over USB with the factory image, then download with ETS
  * every ESP32 before 2026-08-25, and the 8 MB targets (ESP32-S3, -C5) up to `420ad99`: `knx` moves to `0x290000` and grows from 64 to 100 KB; `nvs` stays at `0x2C0000`, the WiFi credentials are kept
  * 4 MB targets (OpenKNXiao C3/C6, KNeoPiX C3/C6, Gledopto, QuinLED) from 2026-08-25 up to `420ad99`: `knx` is already at `0x290000`, but `openknx`, `nvs` and the filesystem move, so the WiFi credentials, the saved state and the filesystem content are gone
  * firmware from `420ad99` on already has this layout and needs no USB flash

**Product**
* Feature: timing rework with chip profiles; the ETS bit rate 0 uses the chip profile, every other value scales it
* Feature: brightness cascade global × segment × effect; lock object, Effektmanager stop mode, cue +/-, Effektmanager pause/resume and state objects
* Feature: per-strip signal polarity, white calculation and refresh in the off state; WRGB and a chip-default option for the colour order
* Feature: file transfer over KNX on every architecture; printf from ROM on ESP32-C3, -S3 and -C5
* Feature: segment and global power model — Effektmanager pauses on OFF, lock priority, switch-on by brightness and scene; status objects computed from the live state and optionally sent once after a restart
* Feature: DPT 3.007 start/stop dimming of the relative objects
* Feature: Effektkette with power, heartbeat (3 s), override policy and bounds; the status page shows the last sync payload of a slave and colours the sync link by its state
* Feature: boot and segment-power autostart of the Effektmanager are separate
* Feature: per-relay behaviour before upload/restart (unchanged / off) and after restart (off / on / last state)
* Feature: the 2D effects are hidden in ETS where the device cannot render them; C6 targets build without them
* Feature: WiFi module in the network channel of the ETS product; WiFi envs with setup access point, gateway ping and QR code
* Feature: KNeoPiX V1.5 (SN74LV8T245 level shifter) and KNeoPiX V1.4/V1.5 ESP32-S3 builds with KNX over TP and WiFi
* Feature: NeoPixel status page `/neo` (hardware, setup with top view and LED image, control, segments, Effektmanager, Effektkette, strips/power/relays) as the default page, and the NeoPixel Studio with live and draft mode, LED test and ETS synchronisation; both share `np-base.css`, the Studio gets state changes over the UI socket
* Feature: example configurations for ConfigTransfer and an offline configurator (`doc/examples`), made for Dev 1.2 / Release 0.12; the import into 2.0 / 0.15 is not checked
* Change: the ETS pages follow the setup workflow: Allgemein → Relais → Physikalisch → Virtuell/Segmente → Effektmanager
* Change: REG2 and the PiPico BCU Connectors are no longer built, they have no level shifter
* Change: the ETS product offers every hardware in every build (`HardwareListMode 'Full'`); the generator stops on a single hardware and on a duplicate `DEVICE_HW_ID`
* Change: OFM-UsbExchange is no longer part of the product; it was compiled out in every env
* Fix: the saved segment state (power, brightness, colour, effect, scene) is applied after a restart and kept across an ETS download, only for segments set to "Letzter Zustand"; relays follow their own after-restart parameter; after a power cut the last saved state comes back, see `doc/TODO.md`
* Fix: one colour and one white value for the direct colour objects, RGBW, RGBCCT, start colour and sync; WS2805 CCT drives its white channels; the H/S/V objects follow the HSV object
* Fix: port selections have their own bytes and are decoded by value; the relay port comes from the parameter of the selected hardware; a GPIO already driven by a relay or another strip, a board button, a status LED, the ESP32 flash bus or the Ethernet PHY is refused
* Fix: relays — an inverted relay is off at boot, an ON within the minimum off-time is deferred instead of dropped
* Fix: scenes — numbers not configured on the segment are ignored, learned scenes keep their effect text and survive an unchanged ETS download
* Fix: power objects are sent within the status budget and declared as DPST-14-56, the strip load of the fallback path is clamped to 100 %, strips with their own limit are measured while global limiting is off
* Fix: Effektmanager — stop only on value 1, the cue effect is entered again when a segment is switched on, the started-from-OFF origin survives a restart
* Fix: an Effektkette master sits at band offset 0
* Fix: HCL is applied only from an active LightManager channel and mapped over the physical length of a segment
* Fix: power limiting uses the ETS current per LED (RGBW read 33 % high before) and the ETS voltage; the ABL warning is raised and cleared with the limiter
* Fix: a strip behind a disabled strip attaches at its ETS position; the ETS keeps the WRGB/chip-default colour order of a TM1814 when the LED type changes instead of falling back to GRB
* Fix: a lock stops a running dim ramp; unknown or disabled effect IDs are rejected; the segment offset rotates only inside Start..End
* Fix: shutdown blanks the LEDs without stalling the download; telegrams of the startup delay are processed after the flash restore

**Build and test**
* `Build-Release.ps1` takes the options of the IP-Interface build: `-Rel`/`-Dev`, `-SkipOknx` (`-SkipTool`, `-SkipHostCli`), `-SkipKnxprod`; `-SkipFirmware -SkipOknx` leaves `NeoPixel-Dev.knxprod` (with `-Rel` `NeoPixel-Release.knxprod`) in the repository root, `-Clean` removes all producer outputs
* The generators sort and write the same output under Windows PowerShell 5.1 and PowerShell 7
* `test/ets/Invoke-NeoPixelEtsTest.ps1`: data-driven device test against an ETS configuration (bus, console, web page, LED image)
