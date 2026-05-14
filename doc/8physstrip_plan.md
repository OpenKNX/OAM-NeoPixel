# 8 Physical Strip Extension Plan

## Goal

Raise the application limit from 6 to 8 physical LED strips without moving any existing KOs.

Target KO result:

- Existing global KOs stay at 400-415.
- Existing strip KOs for strips 1-6 stay at 500-547.
- New strip KOs for strips 7-8 use the already free range 548-563.
- Segment KOs stay at 600-1239.
- One virtual strip and 16 segments remain unchanged.

## Important Constraints

- KO numbering must stay stable.
- ETS parameter memory offsets may change if needed. That is acceptable, but it must be done intentionally and reviewed for overlaps.
- Existing parameter IDs and KO numbers should be kept stable where possible. New parameters should be appended instead of renumbering the current ones.
- Generated artifacts such as `knxprod.h`, `*.debug.xml`, and `*.knxprod` must be regenerated, not edited by hand.
- The current implementation uses several six-wide assumptions in XML, JavaScript, C++, and the hardware-config generator. All of them must be updated together.

## Current Findings

- The OFM-NeoPixel library already allows 8 physical strips.
- The application XML currently exposes only 6 strip channels.
- The ETS parameter model currently limits `NumberOfLEDStrips` to 6.
- The virtual strip configuration currently contains only positions 1-6, start indices 1-6, and end indices 1-6.
- The ETS JavaScript logic currently clamps `NumberOfLEDStrips` to 6 and calculates only 6 virtual positions.
- Runtime code in `StripConfiguration.cpp` and `NeoPixelModule.cpp` clamps the active strip count to 6 and uses fixed arrays of length 6.
- `Build-HardwareConfig.ps1` currently hardcodes `NumStrips = 6` in multiple helper functions and generation calls.
- Extending the strip-related ETS parameter unions from 6 to 8 entries will require a parameter-memory layout review. This does not affect KO numbering, but it can affect parameter offsets and generated share content.

## Non-Goals

- No change to the number of virtual strips.
- No change to the maximum number of segments.
- No change to segment KO layout.
- No refactor of the scene/effect model.
- No redesign of the virtual-strip feature beyond what is required for 8 physical strips.

## Recommended Implementation Order

1. Prepare a KO-stable baseline and acceptance checks.
2. Extend the strip channel count in the application XML.
3. Extend the ETS data model in `NeoPixel.share.xml`.
4. Extend the virtual-strip JavaScript logic.
5. Extend runtime C++ handling.
6. Extend the hardware-config generator and regenerate the auto-generated sections.
7. Regenerate generated artifacts and run integration checks.
8. Update help and README text.

## Work Package 0: Baseline and Acceptance Criteria

### WP0 Purpose

Create a safe baseline before touching the XML and generator layers.

### WP0 Tasks

- Record the current strip KO layout from the generated debug XML.
- Record the current strip limits from the source XML and C++ runtime.
- Define the acceptance criteria for the 8-strip change.

### WP0 Acceptance Criteria

- KOs 400-415 remain unchanged.
- KOs 500-547 remain unchanged.
- New strips 7 and 8 appear at 548-555 and 556-563.
- Segment KOs remain unchanged.
- ETS shows 1-8 strips.
- Virtual strip configuration works for 1-8 strips.
- GPIO conflict detection also covers strips 7 and 8.
- Clean build succeeds.

### WP0 Tests

- Run `OpenKNXproducer` once before the change and keep the resulting KO layout as the comparison baseline.
- Run `Build Release (-Clean)` once before the change and note that the baseline is green.

## Work Package 1: Extend NEOSTRIP Channel Count

### WP1 Purpose

Expose 8 strip channels at the application XML level while keeping the KO stride unchanged.

### WP1 Files

- `src/NeoPixel-Dev.xml`
- `src/NeoPixel-Release.xml`

### WP1 Tasks

- Change `NEOSTRIP` `NumChannels` from 6 to 8 in both source XML variants.
- Keep `KoOffset="500"` unchanged.
- Do not change the strip template KO numbering pattern.

### WP1 Notes

- With the current 8-KO stride, strips 7 and 8 should automatically land in 548-555 and 556-563.
- This work package is KO-stable by design.

### WP1 Tests

- Run `OpenKNXproducer` after the XML change.
- Verify in the generated debug XML that:
  - Strip 1 still starts at KO 500.
  - Strip 6 still starts at KO 540.
  - Strip 7 starts at KO 548.
  - Strip 8 starts at KO 556.
- Verify that no existing strip KO numbers moved.

### WP1 Done When

- The generated application XML contains 8 strip channels with unchanged KO numbering for strips 1-6.

## Work Package 2: Extend ETS Strip Count and Virtual Strip Data Model

### WP2 Purpose

Make the ETS parameter model aware of 8 physical strips.

### WP2 Files

- `src/NeoPixel.share.xml`

### WP2 Tasks

- Change `PT-NumberOfLEDStrips` from max 6 to max 8.
- Update the display text from `von 6` to `von 8`.
- Add `VirtualStripSelect7` and `VirtualStripSelect8` parameter types.
- Extend the canonical virtual strip position parameters from `VirtualStripPos1..6` to `VirtualStripPos1..8`.
- Add the required UI-limited variants for strip counts 6 and 7, plus the canonical 8-strip variant.
- Extend the read-only `VirtualStripStart` parameters from 6 to 8.
- Extend the read-only `VirtualStripEnd` parameters from 6 to 8.
- Append new parameter IDs instead of renumbering the existing 1-6 parameters.
- Extend the corresponding `ParameterRef` entries.
- Extend the `ParameterCalculation` L/R parameter lists so the JavaScript gets `Pos7`, `Pos8`, `Len7`, `Len8`, `Start7`, `Start8`, `End7`, and `End8`.
- Extend the dynamic ETS UI block so the virtual strip table shows rows 7 and 8.
- Recalculate the affected union sizes and downstream memory offsets.

### WP2 Important Detail

This package affects parameter memory layout, not KO layout.

The following areas must be reviewed together:

- Virtual strip configuration union.
- Virtual strip start union.
- Virtual strip end union.
- Auto-generated GPIO port union.
- Auto-generated clock port union.
- Auto-generated relay unions that currently sit behind them.

### WP2 Risk

If the new union sizes are inserted without updating the later offsets, the share XML can end up with overlapping parameter memory.

### WP2 Tests

- Run `OpenKNXproducer` after the XML changes.
- Verify that the generated XML shows `Anzahl der LED-Streifen ... von 8`.
- Verify that `VirtualStripPos7`, `VirtualStripPos8`, `VirtualStripStart7`, `VirtualStripStart8`, `VirtualStripEnd7`, and `VirtualStripEnd8` exist in the generated XML.
- Verify that there are no producer warnings about overlapping memory or duplicate IDs.
- Manually inspect the virtual strip UI table in the generated XML and confirm that rows 7 and 8 are present.

### WP2 Done When

- ETS can represent 1-8 strips and the virtual strip UI contains 8 rows.

## Work Package 3: Extend ETS JavaScript Logic

### WP3 Purpose

Update the non-generated JavaScript logic that currently assumes 6 strips.

### WP3 Files

- `src/NeoPixel.script.js`

### WP3 Tasks

- Change the strip-count clamp from 6 to 8.
- Extend the `NEO_ResetClockGPIOOnLedTypeChange` reset logic to cover `Strip7ClockPort` and `Strip8ClockPort`.
- Extend the virtual strip order switch logic to support `numStrips = 7` and `numStrips = 8`.
- Extend the `pos`, `len`, `start`, and `end` arrays from 6 to 8 elements.
- Add `Pos7`, `Pos8`, `Len7`, `Len8`, `Start7`, `Start8`, `End7`, and `End8` handling.
- Extend the duplicate detection loops from 6 iterations to 8.
- Extend the output assignment block to write `Start7`, `Start8`, `End7`, and `End8`.
- Keep the existing duplicate detection behavior and invalid-value handling unchanged.

### WP3 Notes

- This package covers the manually maintained virtual-strip logic.
- The hardware conflict-detection JavaScript is generated by `Build-HardwareConfig.ps1` and belongs to Work Package 5.

### WP3 Tests

- Run `OpenKNXproducer` and ensure the JavaScript-based parameter calculations still compile.
- Manually test generated values for configurations with 1, 6, 7, and 8 strips.
- Verify that duplicate detection still sets the error flag correctly when the same physical strip is used twice.
- Verify that switching from an SPI strip type to a 1-wire type resets clock ports for strips 1-8.

### WP3 Done When

- The generated ETS application calculates start/end indices correctly for 1-8 strips.

## Work Package 4: Extend Runtime C++ Handling

### WP4 Purpose

Make the firmware honor the new 8-strip limit at runtime.

### WP4 Files

- `src/StripConfiguration.cpp`
- `src/NeoPixelModule.cpp`

### WP4 Tasks

- Change the runtime strip clamps from 6 to 8.
- Extend the fixed `positions[6]` arrays to `positions[8]`.
- Extend the fixed `startPositions[6]` arrays to `startPositions[8]`.
- Extend the `for` loops over virtual strip positions from 6 to 8.
- Extend logging text that still states `max 6 strips`.
- Review helper methods such as default virtual-strip ordering for implicit six-wide assumptions.

### WP4 Notes

- The underlying library already supports 8 strips, so this package is mainly about removing application-level clamps.
- The generated `knxprod.h` should provide the new `ParamNEO_VirtualStripPos7/8` and `ParamNEO_VirtualStripStart7/8` macros after regeneration.

### WP4 Tests

- Build the firmware after regenerating the XML/header artifacts.
- Verify that a configuration with 7 or 8 strips does not get truncated to 6.
- Smoke-test 1-strip, 6-strip, 7-strip, and 8-strip configurations.
- Verify that duplicate virtual-strip mappings still fall back to the default order as before.

### WP4 Done When

- Firmware runtime accepts and uses up to 8 strips without truncation.

## Work Package 5: Extend the Hardware-Config Generator

### WP5 Purpose

Update the generator that currently produces six-strip GPIO, clock, and conflict-detection sections.

### WP5 Files

- `scripts/Build-HardwareConfig.ps1`
- Generated sections in `src/NeoPixel.share.xml`
- Generated sections in `src/NeoPixel.Strip.templ.xml`
- Generated sections in `src/NeoPixel.Hardware.templ.xml`
- Generated `src/HardwareMappingData.h`

### WP5 Tasks

- Change all helper signatures and generation calls that currently use `NumStrips = 6` to 8.
- Extend generated GPIO data-port parameters from strips 1-6 to 1-8.
- Extend generated GPIO clock-port parameters from strips 1-6 to 1-8.
- Extend generated conflict flags and conflict parameter refs to 8 strips.
- Extend generated JavaScript conflict-detection loops to 8 strips.
- Extend generated hardware-change reset logic so it resets data and clock ports for strips 1-8.
- Extend generated alias refs such as `Strip7ClockPort` and `Strip8ClockPort`.
- Recalculate fixed offsets for generated unions so the larger data/clock blocks do not overlap the relay blocks.
- Regenerate `HardwareMappingData.h`.

### WP5 Important Detail

This package is the main reason the change is not just a one-line `NumChannels` bump.

The generator currently assumes:

- 6 data-port parameters.
- 6 clock-port parameters.
- 6 data conflict outputs.
- 6 clock conflict outputs.
- 6-strip JavaScript loops.

All of those assumptions must be updated together.

### WP5 Tests

- Run `pwsh scripts/Build-HardwareConfig.ps1`.
- Verify that generated share/template content contains strip 7 and strip 8 GPIO/clock parameter refs.
- Verify that generated unions do not overlap in the resulting XML.
- Verify that the generated conflict detection still covers all strip pairs.
- Review the diff and confirm that only expected generated sections changed.

### WP5 Done When

- The generator can regenerate a consistent 8-strip hardware configuration without manual patching.

## Work Package 6: Regenerate Generated Artifacts

### WP6 Purpose

Refresh the generated outputs after the source XML, script, C++, and generator changes.

### WP6 Files

- Generated `knxprod.h`
- Generated `src/NeoPixel-Dev.debug.xml`
- Generated `src/NeoPixel.debug.xml`
- Generated `src/NeoPixel.appl.debug.xml`
- Generated `src/NeoPixel.knxprod`
- Any other producer outputs touched by the build flow

### WP6 Tasks

- Run the producer/generation step after Work Packages 1-5.
- Confirm that the generated macros and parameter names now include strip positions 7 and 8.
- Confirm that the channel count now reflects 8 strips.

### WP6 Tests

- Run the VS Code task `OpenKNXproducer`.
- Verify that the generated header contains the new 7/8-related macros.
- Verify that the generated debug XML contains strip KOs 548-563.
- Verify that no existing KO numbers moved.

### WP6 Done When

- All generated artifacts are in sync with the source changes.

## Work Package 7: Documentation and Help Updates

### WP7 Purpose

Bring the user-facing documentation in line with the new application limit.

### WP7 Files

- `README.md`
- `src/Baggages/Help_de/NEO-Anzahl-der-LED-Streifen.md`
- Any other help or README text that still states 6 strips

### WP7 Tasks

- Update text from 6 strips to 8 strips where it describes the application limit.
- If useful, add a short note that the KO range for strips 7 and 8 uses the previously unused range 548-563.
- Verify that segment documentation remains unchanged.

### WP7 Tests

- Search the repo for stale `von 6`, `1-6`, or `6 strips` wording in the NeoPixel docs/help.
- Verify that help text matches the actual application limit and not just the library limit.

### WP7 Done When

- User-facing documentation matches the implementation and ETS UI.

## Validation Matrix

### Build and Generation Checks

- `pwsh scripts/Build-HardwareConfig.ps1`
- VS Code task: `OpenKNXproducer`
- VS Code task: `Build Release (-Clean)`

Expected result:

- All three steps finish successfully.
- No XML overlap or generation errors occur.
- Generated XML and header artifacts are updated.

### KO Regression Checks

- Verify global KOs remain 400-415.
- Verify strip 1-6 KOs remain 500-547.
- Verify strip 7-8 KOs are 548-563.
- Verify segment KOs remain 600-1239.

Expected result:

- Only the new strip KO range is added.
- No existing KO moves.

### ETS UI Checks

- Configure 1 strip and verify the virtual strip UI still behaves correctly.
- Configure 6 strips and verify existing behavior is unchanged.
- Configure 7 strips and verify row 7 appears.
- Configure 8 strips and verify row 8 appears.
- Intentionally configure duplicates and verify the duplicate warning still appears.

Expected result:

- The UI scales from 1 to 8 strips without broken rows or missing fields.

### Runtime Checks

- Create a configuration with 8 strips and mixed protocols.
- Verify that runtime logging reports 8 active strips instead of truncating to 6.
- Verify that virtual strip ordering still maps the correct physical strips.
- Verify that GPIO conflict detection works for strips 7 and 8.
- Verify that SPI-to-1-wire transitions clear clock ports for strips 7 and 8 as well.

Expected result:

- Firmware behavior remains stable and no six-strip fallback remains.

## Suggested Delivery Strategy

Use two implementation passes instead of one large patch set.

### Pass 1

- Work Packages 1-4.
- Goal: source XML, script, and runtime are structurally ready for 8 strips.

### Pass 2

- Work Packages 5-7.
- Goal: generator, generated artifacts, and documentation are aligned and fully validated.

This reduces the risk of mixing source-level logic changes with generated-file churn in the same review.

## Open Decision Before Coding

The current virtual strip UI uses a family of `VirtualStripSelectN` parameter types plus many UI-limited variants.

Two implementation approaches are possible:

- Keep the current pattern and extend it from 6 to 8. This is the smaller behavioral change and the safer migration path.
- Simplify the model to always use canonical `Pos1..8` values and rely on validation instead of many UI-limited variants. This is cleaner long-term, but it is a larger scope change and should not be mixed into the KO-stable 8-strip extension unless there is a strong reason.

Recommendation: keep the current model for the first 8-strip implementation and avoid a UI-model refactor in the same change.
