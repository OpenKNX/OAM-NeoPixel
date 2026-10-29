# NeoPixel Studio

Open `/neopixel` on network firmware builds with `OPENKNX_WEBSERVER`. The page uses
the shared OpenKNX navigation, styles and logo. It controls configured segments;
the live editor changes only visual settings. The commissioning tab additionally
applies LED counts, skipped leading LEDs and the selected segment's range live,
and exports them for an ETS download. GPIOs, protocols, matrix topology, current limits and power-supply
configuration remain exclusively in ETS.

An unprogrammed device shows that an ETS application download is required. Startup
and an ETS download in progress have separate status messages; controls stay hidden
until Studio is ready.

## Controls and draft workflow

- Choose any compiled-in effect and edit its numeric, enum, boolean or text parameters.
  Numeric parameters have both a slider and a numeric input.
- Pick primary and secondary colours using saturation/value, hue, hex, native colour
  input or numeric R/G/B fields. Arrow keys control the saturation/value area.
- RGB, White and Mix modes select RGB only, white only or combined RGB/white output.
- White level controls white-channel output. RGBW uses its fixed white channel;
  RGBCCT splits the level between warm and cool white. A 2000–6500 K control maps
  to that warm/cool ratio. On RGB-only output, the temperature is approximated by RGB.
  Actual colour depends on the LEDs. The white preview and swatch reflect both tone and white level.
- Segment power switches output on/off without changing its hardware/power configuration.
  Existing global power, brightness, KNX locks and output limits continue to apply.

**Automatic apply** is enabled by default: completing an edit immediately updates
the live device output, without pressing Apply live. The button switches automatic
apply off so edits remain browser drafts, independently retained per segment while
switching between segments. **Apply live** validates and applies the selected segment's
complete draft in one main-loop operation. Enabling automatic apply also applies the
selected segment's pending draft. Automatic apply changes runtime values; saving to
flash still requires **Save to device**. **Reset draft** discards that segment's pending changes.
The change review shows the values and scene action that will be applied.
**Reload live values** discards the selected draft and refreshes the actual runtime
values, including changes made through KNX or Studio. It does not undo live edits.
**Restore ETS settings** discards the selected draft and immediately applies the
colours (including white and secondary channels), brightness, effect and its
parameters from the downloaded ETS parameter table. It also restores that segment's
ETS scenes, stops its running effect manager and takes direct control. Segment and
global power states, locks and hardware configuration remain unchanged.
Neither action overwrites the saved Studio snapshot. Press **Save to device** explicitly
to replace that snapshot; ETS import continues to read the last explicit save.

Changing a colour, effect or parameter takes manual control from an effect manager.
A standalone brightness change can dim an effect manager; applying a full visual
draft selects a direct effect. Global OFF continues to hide switched-on segments.

## Responsive layout and commissioning

The page uses the same white cards, framed tabs, green active controls and compact
sliders as the NeoPixel status page. At Full-HD width both colour pickers and effect
parameters sit side by side. Phones use a compact navigation bar, a small action
menu, a single colour picker selected by Primary/Secondary tabs and separate
Steuern / Szenen / Inbetriebnahme panels. These overrides are scoped to Studio.

In **Inbetriebnahme**, edit the number of LEDs (or addressable LED groups) for each
existing strip, skipped leading LEDs (0–255), and the selected segment's first/last
LED. LED numbers start at 1; a strip count of 0 disables that existing strip. The
segment range must fit the combined strips. An end beyond the total is clamped
to the maximum immediately on input, including with Auto apply off. The correction
is reported in the page and applied live when Auto apply is on. The form does not add strips or change
pins, LED protocol, matrix dimensions or power settings.

With **Auto apply: On**, editing a number field applies the layout after a short
typing pause; confirming the field applies it immediately. **LED-Konfiguration live anwenden** also applies it
manually when Auto apply is off. The firmware waits for an idle frame, blanks the
old output, rebuilds the buffers and restores colours, effects and power states.
A shared layout change is rejected if a segment is locked or another segment would
extend past the new total. Failed driver initialization attempts restore the old layout.

**Save to device** stores the selected segment's layout with its Studio snapshot.
Import it with the existing ETS button and download the ETS application to make
it the authoritative startup configuration. A restart uses the downloaded ETS LED
layout; a saved plan remains available for manual live application and ETS import.
Live comparison reports the layout actually running. Pins, protocols, relays and
power settings continue to come from ETS. ETS downloads invalidate saved plans.

Tests include red, green, blue, white, warm/cool white, a looping
wipe and automatic channel cycling. They use the selected segment's current ETS
mapping and normal LED rendering/current limits. White tests use the capabilities
of each physical strip: RGB synthesizes a white tone, RGBW uses its single white
channel for all three choices, and RGBCCT isolates WW/CW (or combines them for
white). HCL is temporarily bypassed inside the test segment so it cannot alter the
wiring test colours; other segments retain HCL. Test brightness is limited to
1–64 / 255; global power must be on, the segment unlocked and an effect manager
stopped. A compiled-in Wipe/Test effect is required for the respective pattern.
**Stop** restores the previous visual state and segment power. Tests stop after
60 seconds, on a lock/global-off, and before normal flash persistence. Saving a
Studio snapshot while testing is rejected, so patterns cannot replace that save.

## Scenes

Studio lists up to ten scenes per segment. Recalling a static scene previews its
effect, colours and brightness in the draft.
Recall, Store draft here, Add scene from draft and Delete apply immediately when
automatic apply is enabled. When disabled, they are staged until Apply or Save.
Only the highest-numbered scene can be deleted. One scene action can be staged at a
time; it runs after the visual draft. Recalling a scene then editing it produces a
direct visual draft; store it explicitly if the scene should be replaced.

Scenes include both RGB/white colours, brightness, effect, ten parameter slots and
14 Latin-15 text bytes. Existing ETS scenes that start an effect manager can be recalled.
The page does not edit effect-manager sequences. Long live text remains supported,
but a scene or ETS-importable save requires text to fit its 14-byte ETS field.
A scene learned over the KNX scene object cuts a longer text to 14 bytes instead.

Scene edits and KNX scene learning use an OAM-owned cache, never the downloaded
KNX parameter table. Bounds are checked before cache imports. Regular module flash
saves include the cache and secondary colours in a versioned extension; old 14-byte
segment records remain readable. After a KNX scene learn the module requests a regular
save as soon as the OpenKNX 3-minute write protection allows it, unless another save
ran since the learn, so learned scenes survive a power loss.

## Save, restart and ETS synchronization

**Save to device** applies the selected draft, saves a versioned Studio snapshot to
LittleFS (`/neopixel-studio-N.json`) using temporary-file replacement, and requests
normal OpenKNX flash persistence. Saving waits for in-flight LED output to finish.

On restart, saved scenes are restored before scene/EM startup handling. Direct Studio
settings, including effect parameters and secondary colours, are restored only for
segments whose ETS startup policy is **Last state**. ETS defaults remain authoritative
for other startup policies. Newer automatic runtime saves take precedence over older
Studio visual snapshots. A configured startup EM keeps its normal higher priority.

An ETS parameter download invalidates all Studio saves, including an identical download.
Segment and relay states ("Letzter Zustand") are kept. Learned and Studio-edited scenes
(also unsaved edits) are kept while the ETS scenes of their segment are unchanged;
otherwise the ETS scenes apply.
A filesystem invalidation marker prevents stale
snapshots from being reused. A changed segment parameter hash also rejects old imports.

On the segment's main ETS page, below Beschreibung:

1. **Gespeicherte Studio-Werte in ETS übernehmen** imports the last explicit Save:
   primary/secondary colours, brightness, effect, parameters, all scene data/count
   and the optional commissioning plan.
   Active scene slot/text aliases and reduced effect selectors are refreshed and
   verified as well. Inactive effect aliases retain their unrelated defaults.
2. Review the imported project values and download the ETS application to the device.
3. **ETS mit aktuellen Gerätewerten vergleichen** compares ETS with live visual settings
   and scenes without modifying ETS, listing each differing parameter with its ETS value and current device value.
   Differences are a successful comparison result, not a parameter script error.
   Both buttons act on the selected segment, including the shared strip length/skip
   fields. Other segments are not imported or compared. The result states the
   number of checked values and lists each difference. Each result line is also
   written as a separate numbered ETS log entry so a shortened task-tree summary
   cannot hide the complete list. Visible effect-specific scene aliases are
   compared, even when their generic backing slots still contain different values. ETS signed ARGB colour values
   are normalised to RGB hex codes before comparison.

The readback progress shows cumulative **Block i/N** and **bytes received/total**
for the selected segment and identifies saved versus live data. The underlying
Common transfer restarts its packet sequence numbers for each 42-byte JSON page;
those local counters are hidden. The percentage increases throughout Save import,
ETS verification and the subsequent live check. Old snapshots without scene or
commissioning data explicitly report that those fields were not checked.

The Studio On/Off switch and temporary test patterns are runtime controls. They
do not modify or import ETS startup behaviour. Picker mode, Auto apply, drafts
and the test brightness control are not independent ETS parameters; colour
channels, final values and stored scene data are the synchronised settings.

Import uses the last explicit Save; comparison uses live state. Import reads back
and verifies the values accepted by ETS (including brightness, RGB/white colours,
effect parameters and scenes). Rejected values cause an error and an attempt to
restore the previous project values. After import, a separate live check reports
whether the runtime has changed since Save; it does not silently replace the saved
snapshot with newer live values. If the report shows saved brightness 106 versus
live brightness 255, save the desired current state again before importing it.
With a fresh matching save and no intervening runtime changes, import followed by
comparison must report identical values. Pending LED counts/skip/segment ranges
are additionally synchronised when present. Other hardware settings, current limits
and effect-manager sequences stay outside the import/comparison scope.
ETS remains the source of truth. No Studio command overwrites downloaded KNX tables.

The ETS schema uses reserved segment bytes 696–702 for secondary startup colours and
separate white channels; the segment block stays 704 bytes. Legacy startup W keeps its
neutral-white behavior until separate white channels are enabled. Regenerate and
reimport the ETS product for these fields and the online buttons.

## API and threading

- `GET /neopixel/api/effects`: compiled-in effects with parameter metadata/defaults.
- `GET /neopixel/api/state`: cached live segments, colours, parameters, scenes and last result.
- `POST /neopixel/api/control`: JSON command, maximum 4096 bytes. HTTP 202 acknowledges
  queueing; poll state for `completedId` and `error` to confirm execution.

Commands include effect, brightness, RGB/white channels, parameter, power,
sceneRecall/sceneStore/sceneAdd/sceneDelete, save, load (reload live values) and
restoreEts (restore downloaded settings), commissioning (apply an LED layout live),
testStart and testStop. A batch contains up to 32
commands for one segment, with at most one final scene action. All values are validated
before mutation. Nested batches, saves, loads, ETS restores and commissioning/test commands are rejected. The first version retains
only the latest result and serializes commands across browsers.

HTTP callbacks queue commands or copy immutable snapshots. Effects and filesystem
operations run in the main loop; there are no spin waits. ETS function property 158/10,
command 0x20, transfers versioned UTF-8 JSON in bounded pages with a transfer token.
The generated ETS/C++ mappings share effect IDs and parameter ordering.

## Local preview and validation

```sh
python3 test/preview_neopixel_studio.py
```

Open <http://127.0.0.1:8765/neopixel>. The dependency-free server binds to loopback and
serves production page assets with simulated API responses. Choose scenarios for
RGB/RGBW/RGBCCT, locks, output off, effect manager, no segments, startup,
unprogrammed devices, ETS download and API outage.
Use `--port` or `--state-file` to override the port or simulated persistent-save file.
Reload after editing assets. Multiple tabs share the simulated state. The preview does
not execute LED drivers or KNX timing.

ETS parameter scripts target Windows Active Scripting JScript / ECMAScript 3.
They do not require a global `JSON` object, ES5 array helpers or browser APIs.
Device responses use a bounded data-only JSON parser. Optional ETS/COM methods
are detected without assuming that `typeof` returns `function`.

Check syntax and the simulated legacy runtime with Node.js:

```sh
node --expose-internals test/test_ets_js_syntax.cjs
node test/test_neopixel_studio_ets.cjs
```

The syntax check uses Acorn bundled with Node.js (or an installed `acorn` package).
For an actual Windows JScript engine check, run this from the project directory:

```bat
cscript //nologo test\test_neopixel_studio_ets_wsh.js
```

The Windows smoke test loads the production script as UTF-8 and checks import,
comparison, paging, Unicode and a real COM method. It uses a simulated device;
an end-to-end test against the ETS host and a physical device remains separate.
An optional first argument selects a generated `NeoPixel.script.js` file to test.

Host checks:

```sh
python3 test/test_neopixel_studio.py
python3 test/test_neopixel_scenes.py
python3 test/test_neopixel_studio_persistence.py
python3 test/test_flash_persistence.py
node test/test_neopixel_studio_ets.cjs
```

C++ host checks run production handlers against hardware doubles under ASan/UBSan.
The WiFi/IP ESP32S3 firmware is built for validation. Physical LED/KNX/ETS round-trip
checks still require a device and ETS. On this Mac OpenKNXproducer emits merged/debug
XML and a header; final schema validation and `.knxprod` packaging need the missing
KNX schema/ETS tooling. TP/WiFi currently has a separate existing multicast link issue
in the checked-out network/KNX dependencies.
