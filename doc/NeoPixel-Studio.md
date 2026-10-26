# NeoPixel Studio

Open `/neopixel` on network firmware builds with `OPENKNX_WEBSERVER`. The page uses
the shared OpenKNX navigation, styles and logo. It controls configured segments;
it cannot change strip definitions, segment geometry, GPIOs, current limits or
power-supply configuration.

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

Scene edits and KNX scene learning use an OAM-owned cache, never the downloaded
KNX parameter table. Bounds are checked before cache imports. Regular module flash
saves include the cache and secondary colours in a versioned extension; old 14-byte
segment records remain readable.

## Save, restart and ETS synchronization

**Save to device** applies the selected draft, saves a versioned Studio snapshot to
LittleFS (`/neopixel-studio-N.json`) using temporary-file replacement, and requests
normal OpenKNX flash persistence. Saving waits for in-flight LED output to finish.

On restart, saved scenes are restored before scene/EM startup handling. Direct Studio
settings, including effect parameters and secondary colours, are restored only for
segments whose ETS startup policy is **Last state**. ETS defaults remain authoritative
for other startup policies. Newer automatic runtime saves take precedence over older
Studio visual snapshots. A configured startup EM keeps its normal higher priority.

An ETS parameter download invalidates normal runtime persistence and all Studio saves,
including an identical download. A filesystem invalidation marker prevents stale
snapshots from being reused. A changed segment parameter hash also rejects old imports.

On the segment's main ETS page, below Beschreibung:

1. **Gespeicherte Studio-Werte in ETS übernehmen** imports the last explicit Save:
   primary/secondary colours, brightness, effect, parameters and all scene data/count.
   Scene slot aliases and reduced effect selectors are refreshed as well.
2. Review the imported project values and download the ETS application to the device.
3. **ETS mit aktuellen Gerätewerten vergleichen** compares ETS with live visual settings
   and scenes without modifying ETS, listing each differing parameter with its ETS value and current device value.
   Differences are a successful comparison result, not a parameter script error.
   The full result is also written to the ETS log. ETS signed ARGB colour values
   are normalised to RGB hex codes before comparison.

Import uses the last explicit Save; comparison uses live state. Import reads back
and verifies the values accepted by ETS (including brightness, RGB/white colours,
effect parameters and scenes). Rejected values cause an error and an attempt to
restore the previous project values. After import, a separate live check reports
whether the runtime has changed since Save; it does not silently replace the saved
snapshot with newer live values. If the report shows saved brightness 106 versus
live brightness 255, save the desired current state again before importing it.
With a fresh matching save and no intervening runtime changes, import followed by
comparison must report identical values. Geometry, hardware,
current limits and effect-manager sequences are outside the import/comparison scope.
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
restoreEts (restore downloaded settings). A batch contains up to 32
commands for one segment, with at most one final scene action. All values are validated
before mutation. Nested batches, saves, loads and ETS restores are rejected. The first version retains
only the latest result and serializes commands across browsers.

HTTP callbacks queue commands or copy immutable snapshots. Effects and filesystem
operations run in the main loop; there are no spin waits. ETS function property 158/10,
command 0x20, transfers versioned UTF-8 JSON in bounded pages with a transfer token.
The generated ETS/C++ mappings share effect IDs and parameter ordering.

## Local preview and validation

```sh
python3 scripts/preview_neopixel_studio.py
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
node --expose-internals scripts/test_ets_js_syntax.cjs
node scripts/test_neopixel_studio_ets.cjs
```

The syntax check uses Acorn bundled with Node.js (or an installed `acorn` package).
For an actual Windows JScript engine check, run this from the project directory:

```bat
cscript //nologo scripts\test_neopixel_studio_ets_wsh.js
```

The Windows smoke test loads the production script as UTF-8 and checks import,
comparison, paging, Unicode and a real COM method. It uses a simulated device;
an end-to-end test against the ETS host and a physical device remains separate.
An optional first argument selects a generated `NeoPixel.script.js` file to test.

Host checks:

```sh
python3 scripts/test_neopixel_studio.py
python3 scripts/test_neopixel_scenes.py
python3 scripts/test_neopixel_studio_persistence.py
python3 scripts/test_flash_persistence.py
node scripts/test_neopixel_studio_ets.cjs
```

C++ host checks run production handlers against hardware doubles under ASan/UBSan.
The WiFi/IP ESP32S3 firmware is built for validation. Physical LED/KNX/ETS round-trip
checks still require a device and ETS. On this Mac OpenKNXproducer emits merged/debug
XML and a header; final schema validation and `.knxprod` packaging need the missing
KNX schema/ETS tooling. TP/WiFi currently has a separate existing multicast link issue
in the checked-out network/KNX dependencies.
