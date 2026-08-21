#!/usr/bin/env bash
# Regeneriert alle Topologie-/Effektkette-Bilder + Segment-Knoten-Icons aus den
# SVG-Quellen in diesem Ordner nach src/Baggages/.
# Voraussetzung: python3 + rsvg-convert (librsvg).  Aufruf:  bash doc/svg-src/regen.sh
set -e
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
B="$ROOT/src/Baggages"
mkdir -p /tmp/topo

# 1) Generatoren ausfuehren (schreiben /tmp/topo/*.svg)
for g in gen_path.py gen_chain.py gen_depth.py; do python3 "$HERE/$g"; done

# 2) SVG -> PNG (Quellname  Zielname  Breite)
while read src name w; do
  [ -z "$src" ] && continue
  rsvg-convert -w "$w" "/tmp/topo/$src.svg" -o "$B/$name.png"
done <<'LIST'
0_1D        topo_1d        340
1_serpRows  topo_serprows  340
2_linRows   topo_linrows   340
3_serpCols  topo_serpcols  340
4_linCols   topo_lincols   340
6_tiledLin  topo_tilelin   340
7_tiledSerp topo_tileserp  340
mock_3d     topo_depth3d   460
mock_tile   topo_depthtile 460
chainm      topo_chainm    520
chains      topo_chains    520
LIST

# 2b) Chain-Bilder haben mehr Rand -> auf Inhalt + schmalen einheitlichen Rand trimmen,
#     damit das Motiv in ETS nicht kleiner wirkt als die Topologie-Bilder.
for f in topo_chainm topo_chains; do
  convert "$B/$f.png" -trim +repage -bordercolor none -border 14 "$B/$f.png"
done

# 3) Segment-Knoten-Icons (48x48) -> Icons/
rsvg-convert -w 48 -h 48 "$HERE/neo_segment.svg"  -o "$B/Icons/neo_segment.png"
rsvg-convert -w 48 -h 48 "$HERE/neo_segmente.svg" -o "$B/Icons/neo_segmente.png"

echo "Alle Bilder aus doc/svg-src/ regeneriert -> src/Baggages/"
