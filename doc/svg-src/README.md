# SVG-Quellen der ETS-Diagramme (Topologie, Effektkette, Segment-Icons)

Hier liegen die **bearbeitbaren Quellen** der gerenderten PNGs in `src/Baggages/`.
Die PNGs selbst sind generierte Artefakte — Änderungen immer hier machen und neu rendern,
nicht die PNGs direkt editieren.

## Regenerieren

```bash
bash doc/svg-src/regen.sh        # braucht python3 + rsvg-convert (librsvg)
```

Schreibt alle PNGs nach `src/Baggages/` (Topologie/Chain in den Root, Segment-Icons nach `Icons/`).
Danach `pwsh scripts/Build-Release.ps1 -SkipFirmware` zum Neubündeln des `.knxprod`.

## Generator → Zieldateien

| Quelle | erzeugt | Ziel-PNG (Breite) |
|---|---|---|
| `gen_path.py`  | `0_1D,1_serpRows,2_linRows,3_serpCols,4_linCols,6_tiledLin,7_tiledSerp` | `topo_1d, topo_serprows, topo_linrows, topo_serpcols, topo_lincols, topo_tilelin, topo_tileserp` (340) |
| `gen_depth.py` | `mock_3d, mock_tile` | `topo_depth3d, topo_depthtile` (460) |
| `gen_chain.py` | `chainm, chains` | `topo_chainm, topo_chains` (520) |
| `neo_segment.svg`  | — | `Icons/neo_segment.png` (48) — einzelnes Segment |
| `neo_segmente.svg` | — | `Icons/neo_segmente.png` (48) — Segment-Container |

## Farb-/Stil-Konventionen (barrierefrei, kein Rot/Grün)

- **Start-LED** = heller Punkt (`#F4F6F8`) mit dunklem Ring (`#333`); **Ende-LED** = `#333` gefüllt.
  Beide nutzen `#333` (= Achsentext-Ton). Pfad-Bilder haben oben eine Legende.
- **Blau `#2D7DD2`** = LED-Pfad, Sync-Pfeil (Chain), Datenkabel (Kachel), 3D-LEDs.
- **Orange `#C25E10`/`#E07A1F`** = „Tiefe" (3D-Pfeil/Text + Kachel-Maßklammer `|---|`) und Slave-Offset.
- **Grau `#555`/`#333`** = Höhe/Breite-Achsen + Beschriftung.
- Keine grünen/roten Marker mehr (Rot-Grün-Sehschwäche). Hintergrund von Cornelius' Review.

## Backups der alten farbigen Bilder

Die ursprünglichen (grün/rot) Versionen liegen als `src/Baggages/topo_*_color.png`.
Sie sind **nicht** im `.knxprod` gebündelt (nur referenzierte Baggages werden gebündelt),
dienen nur als Rollback. Bei Bedarf zurückkopieren.
