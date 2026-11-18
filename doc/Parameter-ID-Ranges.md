# NeoPixel Parameter ID Ranges

This document describes the Parameter and ParameterRef ID allocation owned by the
NeoPixel module. IDs belonging to imported modules such as BASE, LOG, LMG, and FTM
are outside its scope.

## ID structure

OpenKNX parameter IDs use a namespace followed by a local numeric ID. The same
local ID can therefore be used in different namespaces without a collision.

| Namespace | Format | Purpose |
|---|---|---|
| Global NeoPixel | `4000nnn` | Module-wide configuration and internal state |
| Segment | `40CCnnn` | Configuration of segment `CC` |
| Strip | `409Cnnn` | Configuration of physical strip `C` |
| Effect manager | `42CCnnn` | Effect manager `CC` and its cues |
| Segment effect parameters | `21CCEEPP` | Effect-specific parameters of segment `CC` |
| Typed cue parameters | `22CCxxxx` | Effect-specific typed fields for cues |

Effect parameter IDs (`21CC...`, `22CC...`) depend only on the effect number `EE` (the frozen value of
`src/EffectIds.lock`) and the parameter index of the effect. A new effect, a parameter appended to an
effect or a removed effect moves no other ID. A parameter must not be inserted before existing ones
(the index of every later parameter would change). No ID of Dev 2.2 or older uses the prefixes 21 or 22.

The lists below conservatively consider a suffix occupied when it is used by
either a `P` or `UP` ID. A range described as unused is unused in the current
source. Historical gaps may still need to remain unused when upgrade compatibility
with an older product is required.

ParameterRefs normally append `_R-tcccnnnrr` to the Parameter ID. The `rr` value
does not allocate another `nnn`:

- `rr = 01` is the normal reference.
- `rr = 02` is used for several alternate or read-only references.
- Scene slot aliases use `rr = 03..34` for effects 1..32.

The hardware selection is the generated module `NEOHW` (ModuleType 45, `4500001`, `HardwareManifest.xml`); its IDs
come from the OGM-HardwareConfig generator.

## Global NeoPixel namespace: `4000nnn`

Source: [`src/NeoPixel.share.xml`](../src/NeoPixel.share.xml)

| Local IDs | Use |
|---|---|
| `000..002` | Trigger description and global HCL settings |
| `011` | Send the seeded status objects once after a restart |
| `018` | Retired (former hardware selection, now the hardware module `NEOHW`, `4500001`), do not reuse: Release replaces versions down to 0.1, ETS would carry the stored HW-ID into a new parameter with this ID |
| `020..024` | Strip/segment counts, update speed, restart, and global startup behaviour |
| `025..029` | Retired (former global start colour R/G/B/W and brightness), do not reuse |
| `030..059` | Virtual-strip topology, positions, and boundaries |
| `060..061` | Retired (former global start effect selectors), do not reuse |
| `068..074` | Power/current limits, ABL, and effect-manager enable |
| `076..089` | Additional virtual-strip positions, boundaries, and validation flags |
| `091..098` | Retired (former strip data-port conflict flags, memory byte 88), do not reuse |
| `101..108` | Retired (former copies of the strip data ports, memory bytes 80..87), do not reuse |
| `111..118` | Retired (former copies of the strip clock ports, memory bytes 90..97), do not reuse |
| `130..150` | External relay configuration, delays, and minimum off-times (`133`, `134`, `137`, `138` retired: former relay conflict flags, memory byte 103) |
| `155..162` | Relay GPIOs and output polarity |
| `163..170` | Retired (former strip clock-port conflict flags, memory byte 89), do not reuse |
| `171..181` | Virtual-strip extensions for strips 7 and 8 |
| `182..186` | Relay names and reserved relay-name storage |
| `194..196` | Pending LED-type reset state |
| `200..249` | Relay 1 port per board (200 + board number of `HardwareManifest.slots`, numbers 0..49; used today 200..239) |
| `250..299` | Relay 2 port per board (250 + board number; used today 250..289) |
| `300..349` | Relay 3 port per board (300 + board number; used today 300..339) |
| `350..399` | Relay 4 port per board (350 + board number; used today 350..389) |
| `410..420` | Topology and chain UI pictures |
| `900..903`, `911..918`, `921..928` | GPIO conflict flags without memory (generated, `HardwareManifest.xml <Conflicts>`); calculation `PC-900` |

Currently unused global suffixes:

```text
003..010, 012..017, 019, 062..067, 075, 090, 099..100, 109..110, 119..129,
151..154, 187..193, 197..199, 400..409, 421..899, 904..910, 919..920, 929..999
```

`003..010` and `062..067` were used by earlier products (git history; `062` was the
HCL strength in Release 0.1). Release still replaces versions down to 0.1, so ETS
would carry those old values into a new parameter with the same ID: keep them free.
`200..399` is computed in `src/hw/NeoPixel.hw.share.xml` as
`200 + 50 x (relay - 1) + board number` for relays 1..4 (board number = slot of `HardwareManifest.slots`,
0..49); the unused gaps inside it are reserved for further hardware variants. Boards with hard-wired relays
(`HW_RELAY_COUNT`: QuinLED, Gledopto) have no relay port parameters; their IDs stay reserved (slot of the board).

## Segment namespace: `40CCnnn`

Sources:

- [`src/NeoPixel.Segment.templ.xml`](../src/NeoPixel.Segment.templ.xml)
- [`src/NeoPixel.Scene.part.xml`](../src/NeoPixel.Scene.part.xml)
- [`src/NeoPixel.Strip.templ.xml`](../src/NeoPixel.Strip.templ.xml)
- [`scripts/Build-EffectParameters.ps1`](../scripts/Build-EffectParameters.ps1)

| Local IDs | Use |
|---|---|
| `000` | Segment description (`P`, not `UP`) |
| `001..011` | Effect availability flags (`P`, no device memory): `001` segment effect, `002..011` scenes A..J |
| `050..058` | Geometry, direction, mirroring, and effect selectors |
| `066..067` | Startup behaviour and RGB colour |
| `070..072` | Startup W, brightness, and effect-manager stop behaviour |
| `240..255` | HCL, topology, matrix dimensions, synchronization, virtual offsets, scenes, and startup settings |
| `320..519` | Ten scene blocks of 20 IDs each |
| `580..899` | Retired (segment effect parameters up to Dev 2.2), do not reuse |
| `900..949` | Data port of strip `C` per board: `900 + board slot` (slots 0..39 in use: `900..939`) |
| `950..999` | Clock port of strip `C` per board: `950 + board slot` (slots 0..39 in use: `950..989`) |

The per-board port parameters come from the OGM-HardwareConfig generator: template
[`src/hw/NeoPixel.hw.strip.xml`](../src/hw/NeoPixel.hw.strip.xml), output `src/NeoPixel.hw.strip.gen.xml`,
included by the strip template (module NEOSTRIP, ModuleType 40, strips 1..8). They use `%TT%0%C%nnn` with
`%C%` = strip, so for strips 1..8 they lie in the namespaces `4001nnn..4008nnn` of segments 1..8. The board
slot is the number of the board in `HardwareManifest.slots` (fixed per HW-ID, a removed board keeps it); a
slot above 49 would move the data port into the clock range. Segments 9..16 have no IDs there.

### Scene blocks

| Scene | IDs | Scene | IDs |
|---|---|---|---|
| A | `320..339` | F | `420..439` |
| B | `340..359` | G | `440..459` |
| C | `360..379` | H | `460..479` |
| D | `380..399` | I | `480..499` |
| E | `400..419` | J | `500..519` |

Every scene block uses all 20 IDs:

- `+0`: effect selector
- `+1..+7`: fixed scene values
- `+8..+17`: ten generic effect parameter slots
- `+18`: effect text
- `+19`: no-2D effect selector

Calculation: `PC-%TT%%CC%001` (effect availability of the segment and its scenes, writes `001..011`).

### Segment effect parameters: `21CCEEPP`

Written by `scripts/Build-EffectParameters.ps1` into the generated blocks of
`src/NeoPixel.Segment.templ.xml`:

```text
Parameter    %AID%_UP-21%CC%EEPP
ParameterRef %AID%_UP-21%CC%EEPP_R-21%CC%EEPP01
EE = effect number 01..99, PP = parameter index 00..99
```

Example: Wipe (effect 1) uses `21CC0100..0102`, Metaballs2D (effect 32) `21CC3200..3203`. The byte
offsets in the segment union stay sequential as before (they follow the parameter order; the ETS
update maps values by ID, not by offset).

The generator stops on an effect number above 99, a parameter index above 99, two parameters with one
ID, or a `UP-21%CC%` parameter in another source XML.

`580..899` held the effect parameters up to Dev 2.2 (`580 + (EffectID - 1) * 10 + index`). They stay
unused: ETS would carry an old value of Dev 2.2 into a new parameter with the same ID.

Genuinely free segment ranges in the current product:

```text
012..049, 059..065, 068..069, 073..239, 256..319, 520..579,
907..949, 957..999
```

## Strip namespace: `409Cnnn`

Source: [`src/NeoPixel.Strip.templ.xml`](../src/NeoPixel.Strip.templ.xml)

| Local IDs | Use |
|---|---|
| `000` | Strip description |
| `030..039` | LED type/order, GPIO/SPI, length, skip count, and voltage |
| `041..048` | Power/current limits, calibration, and gamma |
| `066..075` | ABL, white balance, SPI/timing, and white/off modes |
| `076..077` | Port carriers `NEO<C>DataPort` / `NEO<C>ClockPort` (`Access="None"`, the byte of the per-board data/clock ports, read by the firmware) |
| `081` | Combined power-limit mode |
| `233` | Manually selected clock GPIO |

Currently unused strip suffixes:

```text
001..029, 040, 049..065, 078..080, 082..232, 234..999
```

## Effect-manager namespace: `42CCnnn`

Sources:

- [`src/NeoPixel.EM.templ.xml`](../src/NeoPixel.EM.templ.xml)
- [`src/NeoPixel.Cue.part.xml`](../src/NeoPixel.Cue.part.xml)

| Local IDs | Use |
|---|---|
| `001..005` | Name, cue count, loop, next manager, and enabled state |
| `009..010` | Text-KO setting and display name |
| `100..279` | Ten generic cue blocks of 18 IDs each |
| `280..289` | Effect availability flags of cue 1..10 (`P`, no device memory) |
| `900..909` | One no-2D effect selector per cue |

| Cue | IDs | Cue | IDs |
|---|---|---|---|
| 1 | `100..117` | 6 | `190..207` |
| 2 | `118..135` | 7 | `208..225` |
| 3 | `136..153` | 8 | `226..243` |
| 4 | `154..171` | 9 | `244..261` |
| 5 | `172..189` | 10 | `262..279` |

Currently unused effect-manager suffixes:

```text
000, 006..008, 011..099, 290..899, 910..999
```

Calculations: `PC-%TT%%CC%001` (state marker of the name), `PC-%TT%%CC%002` (effect availability of
the cues, writes `280..289`).

## Typed cue namespace: `22CCxxxx`

Effect-specific typed cue fields overlay the ten generic slot bytes of their cue. The four-digit suffix
is `%CUE_TPP% + K` (`src/NeoPixel.EM.templ.xml`, `%CUE_TPP%` = cue index x 1000, four digits):

```text
suffix = (cue - 1) * 1000 + EE * 10 + P
EE = effect number 01..99, P = parameter index 0..9
```

| Cue | Suffixes | Cue | Suffixes |
|---|---|---|---|
| 1 | `0010..0999` | 6 | `5010..5999` |
| 2 | `1010..1999` | 7 | `6010..6999` |
| 3 | `2010..2999` | 8 | `7010..7999` |
| 4 | `3010..3999` | 9 | `8010..8999` |
| 5 | `4010..4999` | 10 | `9010..9999` |

Example: Wipe in cue 1 uses `22CC0010..0012`, Metaballs2D in cue 10 `22CC9320..9323`. Numeric
parameters with an index above 9 have no cue slot and are left out; a string parameter with an index
above 9 stops the generator. The generator also stops when `%CUE_TPP%` is not `value="0"
increment="1000" digits="4"`, when the Cue part has more than 10 instances, on two parameters with one ID,
or on a `UP-22%CC%` parameter in another source XML.

Up to Dev 2.2 the typed cue fields used `20CCxxxx` with a running counter over all effects
(`1000 + 500 x (cue - 1) + k`, k 0..142). That namespace is retired and stays unused.
