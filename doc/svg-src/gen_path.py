# Path diagrams WITH Breite/Hoehe axis labels, higher res
COLS,ROWS=6,4
def make_path_svg(order, oneD=False, gap=0, title_cols=COLS):
    W,H=320,238
    x0,y0=64,58
    rgrid=W-30
    sx=(rgrid-x0-gap)/(title_cols-1)
    bgrid=H-46
    sy=(bgrid-y0)/(ROWS-1)
    def xy(c,r):
        if oneD:
            sxx=(rgrid-x0)/(8-1); return x0+c*sxx, H/2-10
        x=x0+c*sx+(gap if c>=3 else 0)
        return x, y0+r*sy
    s=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}">']
    s.append('<defs><marker id="ar" markerWidth="10" markerHeight="10" refX="7" refY="3.5" orient="auto"><path d="M0,0 L7,3.5 L0,7 Z" fill="#555"/></marker></defs>')
    s.append('<circle cx="86" cy="24" r="7" fill="#F4F6F8" stroke="#333" stroke-width="2.5"/>')
    s.append('<text x="99" y="29" font-family="sans-serif" font-size="13" fill="#333">Start-LED</text>')
    s.append('<circle cx="196" cy="24" r="7" fill="#333"/>')
    s.append('<text x="209" y="29" font-family="sans-serif" font-size="13" fill="#333">Ende-LED</text>')
    R=8
    for i in range(len(order)-1):
        c0,r0=order[i]; c1,r1=order[i+1]
        x0p,y0p=xy(c0,r0); x1p,y1p=xy(c1,r1)
        adj=abs(c0-c1)+abs(r0-r1)==1
        if adj: s.append(f'<line x1="{x0p:.0f}" y1="{y0p:.0f}" x2="{x1p:.0f}" y2="{y1p:.0f}" stroke="#2D7DD2" stroke-width="4" stroke-linecap="round"/>')
        else:   s.append(f'<line x1="{x0p:.0f}" y1="{y0p:.0f}" x2="{x1p:.0f}" y2="{y1p:.0f}" stroke="#C0C0C0" stroke-width="2.2" stroke-dasharray="4,4"/>')
    for (c,r) in order:
        x,y=xy(c,r); s.append(f'<circle cx="{x:.0f}" cy="{y:.0f}" r="{R}" fill="#D8DEE9"/>')
    sx0,sy0=xy(*order[0]); ex0,ey0=xy(*order[-1])
    s.append(f'<circle cx="{sx0:.0f}" cy="{sy0:.0f}" r="{R}" fill="#F4F6F8" stroke="#333" stroke-width="2.5"/>')
    s.append(f'<circle cx="{ex0:.0f}" cy="{ey0:.0f}" r="{R}" fill="#333"/>')
    if oneD:
        ay=H/2+14
        s.append(f'<line x1="{x0}" y1="{ay}" x2="{rgrid}" y2="{ay}" stroke="#555" stroke-width="2" marker-end="url(#ar)"/>')
        s.append(f'<text x="{(x0+rgrid)/2:.0f}" y="{ay+18}" font-family="sans-serif" font-size="15" fill="#333" text-anchor="middle">L&#228;nge (Anzahl LEDs)</text>')
    else:
        # Breite (bottom)
        ay=y0+(ROWS-1)*sy+24
        gx1=xy(COLS-1,0)[0]
        s.append(f'<line x1="{x0}" y1="{ay:.0f}" x2="{gx1:.0f}" y2="{ay:.0f}" stroke="#555" stroke-width="2" marker-end="url(#ar)"/>')
        s.append(f'<text x="{(x0+gx1)/2:.0f}" y="{ay+18:.0f}" font-family="sans-serif" font-size="15" fill="#333" text-anchor="middle">Breite (Spalten)</text>')
        # Hoehe (left)
        axx=x0-28
        s.append(f'<line x1="{axx}" y1="{y0}" x2="{axx}" y2="{y0+(ROWS-1)*sy:.0f}" stroke="#555" stroke-width="2" marker-end="url(#ar)"/>')
        cy=y0+(ROWS-1)*sy/2
        s.append(f'<text x="{axx-6}" y="{cy:.0f}" font-family="sans-serif" font-size="15" fill="#333" text-anchor="middle" transform="rotate(-90 {axx-6} {cy:.0f})">H&#246;he (Zeilen)</text>')
    s.append('</svg>'); return "\n".join(s)

def serp_rows(): 
    o=[]; 
    for r in range(ROWS):
        for c in (range(COLS) if r%2==0 else range(COLS-1,-1,-1)): o.append((c,r))
    return o
def lin_rows(): return [(c,r) for r in range(ROWS) for c in range(COLS)]
def serp_cols():
    o=[]
    for c in range(COLS):
        for r in (range(ROWS) if c%2==0 else range(ROWS-1,-1,-1)): o.append((c,r))
    return o
def lin_cols(): return [(c,r) for c in range(COLS) for r in range(ROWS)]
def tiled(serp):
    o=[]
    for t in [[0,1,2],[3,4,5]]:
        for i,c in enumerate(t):
            for r in (range(ROWS) if (not serp or i%2==0) else range(ROWS-1,-1,-1)): o.append((c,r))
    return o
def oneD(): return [(c,0) for c in range(8)]

open("/tmp/topo/0_1D.svg","w").write(make_path_svg(oneD(), oneD=True))
open("/tmp/topo/1_serpRows.svg","w").write(make_path_svg(serp_rows()))
open("/tmp/topo/2_linRows.svg","w").write(make_path_svg(lin_rows()))
open("/tmp/topo/3_serpCols.svg","w").write(make_path_svg(serp_cols()))
open("/tmp/topo/4_linCols.svg","w").write(make_path_svg(lin_cols()))
open("/tmp/topo/6_tiledLin.svg","w").write(make_path_svg(tiled(False), gap=22))
open("/tmp/topo/7_tiledSerp.svg","w").write(make_path_svg(tiled(True), gap=22))
print("path svgs regenerated with axis labels")