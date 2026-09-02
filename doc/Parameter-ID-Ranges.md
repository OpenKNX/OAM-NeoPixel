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
| Typed cue parameters | `20CCxxxx` | Effect-specific typed fields for cues |

The lists below conservatively consider a suffix occupied when it is used by
either a `P` or `UP` ID. A range described as unused is unused in the current
source. Historical gaps may still need to remain unused when upgrade compatibility
with an older product is required.

ParameterRefs normally append `_R-tcccnnnrr` to the Parameter ID. The `rr` value
does not allocate another `nnn`:

- `rr = 01` is the normal reference.
- `rr = 02` is used for several alternate or read-only references.
- Scene slot aliases use `rr = 03..34` for effects 1..32.

## Global NeoPixel namespace: `4000nnn`

Source: [`src/NeoPixel.share.xml`](../src/NeoPixel.share.xml)

| Local IDs | Use |
|---|---|
| `000..002` | Trigger description and global HCL settings |
| `020..029` | Strip/segment counts, update speed, restart, and global startup settings |
| `030..059` | Virtual-strip topology, positions, and boundaries |
| `060..061` | Global startup effect selectors |
| `068..074` | Power/current limits, ABL, and effect-manager enable |
| `076..089` | Additional virtual-strip positions, boundaries, and validation flags |
| `091..098` | Strip data-port conflict flags |
| `101..108` | Resolved data ports for strips 1..8 |
| `111..118` | Resolved clock ports for strips 1..8 |
| `130..150` | External relay configuration, conflicts, delays, and minimum off-times |
| `155..162` | Relay GPIOs and output polarity |
| `163..170` | Strip clock-port conflict flags |
| `171..181` | Virtual-strip extensions for strips 7 and 8 |
| `182..186` | Relay names and reserved relay-name storage |
| `194..196` | Pending LED-type reset state |
| `200..206` | Relay 1 hardware-specific port alternatives |
| `250..256` | Relay 2 hardware-specific port alternatives |
| `300..306` | Relay 3 hardware-specific port alternatives |
| `350..356` | Relay 4 hardware-specific port alternatives |
| `410..420` | Topology and chain UI pictures |

Currently unused global suffixes:

```text
003..019, 062..067, 075, 090, 099..100, 109..110, 119..129,
151..154, 187..193, 197..199, 207..249, 257..299, 307..349,
357..409, 421..999
```

## Segment namespace: `40CCnnn`

Sources:

- [`src/NeoPixel.Segment.templ.xml`](../src/NeoPixel.Segment.templ.xml)
- [`src/NeoPixel.Scene.part.xml`](../src/NeoPixel.Scene.part.xml)
- [`src/NeoPixel.Hardware.templ.xml`](../src/NeoPixel.Hardware.templ.xml)
- [`scripts/Build-EffectParameters.ps1`](../scripts/Build-EffectParameters.ps1)

| Local IDs | Use |
|---|---|
| `000` | Segment description (`P`, not `UP`) |
| `050..058` | Geometry, direction, mirroring, and effect selectors |
| `066..067` | Startup behaviour and RGB colour |
| `070..072` | Startup W, brightness, and effect-manager stop behaviour |
| `240..255` | HCL, topology, matrix dimensions, synchronization, virtual offsets, scenes, and startup settings |
| `320..519` | Ten scene blocks of 20 IDs each |
| `580..899` | Stable effect-parameter blocks of 10 IDs each |
| `900..906` | Hardware-specific data GPIO alternatives |
| `950..956` | Hardware-specific clock GPIO alternatives |

The hardware templates use `%TT%0%C%900..906` and `%TT%0%C%950..956`.
For corresponding channels this is the same namespace as `%TT%%CC%`, so these
ranges must be treated as occupied by the segment allocator.

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

### Stable effect blocks

The generator assigns ten IDs per stable EffectID:

```text
parameter ID = 580 + (EffectID - 1) * 10 + ParameterIndex
```

Unused positions inside these blocks are reserved. They must not be assigned to
another effect, even though no Parameter currently occupies them.

| Block | Effect | Used | Reserved, currently unused |
|---|---|---|---|
| `580..589` | Wipe | `580..582` | `583..589` |
| `590..599` | Rainbow | `590..594` | `595..599` |
| `600..609` | Pride2015 | `600` | `601..609` |
| `610..619` | Juggle | `610..613` | `614..619` |
| `620..629` | BPM | `620..621` | `622..629` |
| `630..639` | Cylon | `630..635` | `636..639` |
| `640..649` | Test | `640..641` | `642..649` |
| `650..659` | Fire | `650..654` | `655..659` |
| `660..669` | Theater Chase | `660..665` | `666..669` |
| `670..679` | Sparkle | `670..676` | `677..679` |
| `680..689` | Breathing | `680..684` | `685..689` |
| `690..699` | Strobe | `690..694` | `695..699` |
| `700..709` | Comet | `700..705` | `706..709` |
| `710..719` | Noise | `710..714` | `715..719` |
| `720..729` | Palette | `720..723` | `724..729` |
| `730..739` | Blitz | `730..734` | `735..739` |
| `740..749` | Gradient | `740..743` | `744..749` |
| `750..759` | Kerze | `750..752` | `753..759` |
| `760..769` | Laufschrift | `760..764` | `765..769` |
| `770..779` | Uhr2D | `770..779` | none |
| `780..789` | Snake2D | `780..783` | `784..789` |
| `790..799` | Matrix2D | `790..793` | `794..799` |
| `800..809` | Tetris2D | `800..805` | `806..809` |
| `810..819` | TRON | `810..813` | `814..819` |
| `820..829` | Starfield Warp | `820..823` | `824..829` |
| `830..839` | Plasma Nebula | `830..833` | `834..839` |
| `840..849` | UFO Swarm | `840..843` | `844..849` |
| `850..859` | Game of Life 2D | `850..853` | `854..859` |
| `860..869` | DNA2D | `860..863` | `864..869` |
| `870..879` | Aurora2D | `870..873` | `874..879` |
| `880..889` | Lissajous2D | `880..883` | `884..889` |
| `890..899` | Metaballs2D | `890..893` | `894..899` |

Genuinely free segment ranges in the current product:

```text
001..049, 059..065, 068..069, 073..239, 256..319, 520..579,
907..949, 957..999
```

Effect 33 would naturally receive `900..909`, which collides with the hardware
IDs `900..906`. New effects therefore cannot simply continue the current formula;
they need an explicit allocation strategy.

## Strip namespace: `409Cnnn`

Source: [`src/NeoPixel.Strip.templ.xml`](../src/NeoPixel.Strip.templ.xml)

| Local IDs | Use |
|---|---|
| `000` | Strip description |
| `030..039` | LED type/order, GPIO/SPI, length, skip count, and voltage |
| `041..048` | Power/current limits, calibration, and gamma |
| `066..075` | ABL, white balance, SPI/timing, and white/off modes |
| `081` | Combined power-limit mode |
| `233` | Manually selected clock GPIO |

Currently unused strip suffixes:

```text
001..029, 040, 049..065, 076..080, 082..232, 234..999
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
000, 006..008, 011..099, 280..899, 910..999
```

## Typed cue namespace: `20CCxxxx`

Effect-specific typed cue fields use a separate, legacy four-digit suffix
namespace. Each cue reserves 500 suffixes and currently uses offsets `0..142`.

| Cue | Used | Reserved, currently unused |
|---|---|---|
| 1 | `1000..1142` | `1143..1499` |
| 2 | `1500..1642` | `1643..1999` |
| 3 | `2000..2142` | `2143..2499` |
| 4 | `2500..2642` | `2643..2999` |
| 5 | `3000..3142` | `3143..3499` |
| 6 | `3500..3642` | `3643..3999` |
| 7 | `4000..4142` | `4143..4499` |
| 8 | `4500..4642` | `4643..4999` |
| 9 | `5000..5142` | `5143..5499` |
| 10 | `5500..5642` | `5643..5999` |

This namespace is independent of the three-digit segment effect allocation and
was not the source of the `tcccnnn_R-tcccnnnrr` errors fixed by moving stable
segment effect parameters into `580..899`.