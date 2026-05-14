# Wortuhr (Word Clock) – Implementation Plan

## Time Source

`openknx.time.getLocalTime()` is already used in `src/NeoPixelModule.cpp` (HCL code).  
The `WordClockEffect::update()` will **not** call this directly (effects live in `lib/OFM-NeoPixel`
and must not depend on the outer module layer).  
**Solution:** `NeoPixelModule` pushes `hour` + `minute` into the effect the same way it pushes
time into `HclManager` — via a small static setter on the effect or a shared `WordClockTime`
singleton inside `lib/OFM-NeoPixel`.

---

## Files to Create

| File | Purpose |
|---|---|
| `lib/OFM-NeoPixel/src/effects/WordClockEffect.h` | Effect class (header-only like RainbowEffect) |
| `lib/OFM-NeoPixel/src/wordclock/WordClockLayout.h` | `WordDef` / layout table structs |
| `lib/OFM-NeoPixel/src/wordclock/WordClockLayout_DE_10x11.h` | German 10×11 layout (first layout) |
| `lib/OFM-NeoPixel/src/wordclock/WordClockTime.h` | Tiny singleton: `setTime(h,m)` / `getHour()` / `getMinute()` |

---

## Files to Modify

### 1. `lib/OFM-NeoPixel/src/effects/EffectPool.h`
- Add `#include` disable-guard: `// #define NEOPIXEL_DISABLE_WORDCLOCK`
- Add forward declaration: `static WordClockEffect* getWordClock();`

### 2. `lib/OFM-NeoPixel/src/effects/EffectPool.cpp`
- Add conditional include of `WordClockEffect.h`
- Add static singleton pointer `static WordClockEffect* _wordClock`
- Add getter `EffectPool::getWordClock()`
- Add to `getEffectCount()` counter
- Add to `getEffectByIndex()` — **append at end** to avoid shifting all existing ETS XML IDs

### 3. `src/NeoPixelModule.cpp`
- In the existing time-update block (where `setCurrentTime()` is called for HCL),
  also call `WordClockTime::setTime(hour, minute)`
- Add include for `wordclock/WordClockTime.h`

### 4. `lib/OFM-NeoPixel/src/NeoPixelConsole.cpp`
- Add console command `neo wordclock time <hh> <mm>` for manual time override during testing
- Add to help text block

### 5. `lib/OFM-NeoPixel/src/NeoPixel.h`
- Declare `processWordClockCommand()` if console command is added as its own function

---

## Script-Generated XML (automatic — no manual edit needed)

Running `scripts/Build-EffectParameters.ps1` after the above C++ changes will automatically:

| Generated file | What changes |
|---|---|
| `src/NeoPixel.share.xml` | New `<Enumeration>` entry for "WordClock" in `PT-NEOEffectType` |
| `src/NeoPixel.share.xml` | New `<ParameterType>` block for WordClock parameters |
| `src/NeoPixel.Effects.UnionParameters.generated.xml` | Union parameter entries for Layout, BackgroundBrightness, FadeTime |
| `src/NeoPixel.Effects.ParameterRefs.generated.xml` | `<ParameterRefRef>` entries |
| `src/NeoPixel.Effects.DynamicChoose.generated.xml` | `<when>` block showing WordClock params when effect = WordClock |
| `src/NeoPixel.Segment.templ.xml` | Updated effect parameter injection between markers |
| `src/NeoPixel.Scene.part.xml` | New scene entry for WordClock effect |

The script reads `getName()`, `getParameterCount()`, `getParameterName()`, `getParameterType()`,
`getParameterDefault()`, `getParameterMin()`, `getParameterMax()` from `WordClockEffect.h` —
these must follow the same conventions as e.g. `CometEffect.h`.  
Supports `EFFECT_NAME_DE_EN("Wortuhr", "Word Clock")` and `PARAM_DESC_DE_EN(...)` macros.

---

## Effect Parameters (in `WordClockEffect.h`)

| Index | Name | Type | Default | Min | Max | Description |
|---|---|---|---|---|---|---|
| 0 | Layout | UINT8 | 0 | 0 | 0 | 0 = DE 10×11 (more layouts later) |
| 1 | BackgroundBrightness | Percent | 0 | 0 | 50 | Dimmed inactive LEDs (0 = off) |
| 2 | FadeTime | UINT8 | 5 | 0 | 20 | Crossfade time in ticks on minute change |

---

## Implementation Order

1. Create `WordClockTime.h` (singleton, no dependencies)
2. Create `WordClockLayout.h` + `WordClockLayout_DE_10x11.h` (pure data, no dependencies)
3. Create `WordClockEffect.h` (depends on 1 + 2 + `Effect.h` + `Segment.h`)
4. Modify `EffectPool.h` + `EffectPool.cpp` (register effect)
5. Modify `src/NeoPixelModule.cpp` (wire time source)
6. Modify `lib/OFM-NeoPixel/src/NeoPixelConsole.cpp` (console test command)
7. Run `Build-EffectParameters.ps1` → all XML updates are generated automatically
8. Run `Build-HardwareConfig.ps1` if hardware-specific flags are affected (unlikely)
9. Test build with `platformio run --environment release_OKNXHW_UP1_GW_UART`

---

## Open Questions (to decide before implementation)

- **Minute rounding:** Classic 5-minute steps (ES IST FÜNF NACH ...) or exact?
- **Corner minute LEDs:** Support 4 extra LEDs for minutes 1–4 (common in DE layouts)?
- **Additional layouts:** EN 10×11 planned? If yes, add as second layout from the start.
- **Fade style:** Crossfade active word set, or hard-cut on minute change?
