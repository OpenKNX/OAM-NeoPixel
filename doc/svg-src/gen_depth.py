# ---- MOCK: cleaner 3D depth ----
def depth3d():
    W,H=380,250
    cols,rows,layers=4,3,3
    sx,sy=24,24
    ox,oy=52,34
    x0,y0=70,150
    R=7
    def lx(c,l): return x0+c*sx+l*ox
    def ly(r,l): return y0+r*sy-l*oy
    s=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}">']
    s.append('<defs><marker id="a" markerWidth="11" markerHeight="11" refX="7" refY="4" orient="auto"><path d="M0,0 L8,4 L0,8 Z" fill="#555"/></marker><marker id="g" markerWidth="11" markerHeight="11" refX="7" refY="4" orient="auto"><path d="M0,0 L8,4 L0,8 Z" fill="#E07A1F"/></marker></defs>')
    s.append(f'<text x="{W/2:.0f}" y="24" font-family="sans-serif" font-size="16" fill="#2D7DD2" text-anchor="middle" font-weight="bold">3D: &#187;Tiefe&#171; = gestapelte Ebenen</text>')
    corners=[(0,0),(cols-1,0),(0,rows-1),(cols-1,rows-1)]
    for l in range(layers-1):
        for (c,r) in corners:
            s.append(f'<line x1="{lx(c,l):.0f}" y1="{ly(r,l):.0f}" x2="{lx(c,l+1):.0f}" y2="{ly(r,l+1):.0f}" stroke="#C8D0DA" stroke-width="1.4"/>')
    for l in reversed(range(layers)):
        op=[1.0,0.5,0.28][l]; fill="#2D7DD2" if l==0 else "#7FA8D0"
        for c in range(cols):
            for r in range(rows):
                s.append(f'<circle cx="{lx(c,l):.0f}" cy="{ly(r,l):.0f}" r="{R}" fill="{fill}" opacity="{op:.2f}"/>')
        s.append(f'<text x="{lx(0,l)-4:.0f}" y="{ly(0,l)-12:.0f}" font-family="sans-serif" font-size="13" fill="#888" text-anchor="middle">E{l+1}</text>')
    # Breite
    by=y0+(rows-1)*sy+26
    s.append(f'<line x1="{x0}" y1="{by:.0f}" x2="{lx(cols-1,0):.0f}" y2="{by:.0f}" stroke="#555" stroke-width="2.2" marker-end="url(#a)"/>')
    s.append(f'<text x="{(x0+lx(cols-1,0))/2:.0f}" y="{by+20:.0f}" font-family="sans-serif" font-size="15" fill="#333" text-anchor="middle">Breite</text>')
    # Hoehe
    axx=x0-26
    s.append(f'<line x1="{axx}" y1="{y0}" x2="{axx}" y2="{y0+(rows-1)*sy:.0f}" stroke="#555" stroke-width="2.2" marker-end="url(#a)"/>')
    cyv=y0+(rows-1)*sy/2
    s.append(f'<text x="{axx-7}" y="{cyv:.0f}" font-family="sans-serif" font-size="15" fill="#333" text-anchor="middle" transform="rotate(-90 {axx-7} {cyv:.0f})">H&#246;he</text>')
    # Tiefe (green)
    zx0,zy0=lx(cols-1,0)+16, ly(0,0)
    zx1,zy1=lx(cols-1,layers-1)+16, ly(0,layers-1)
    s.append(f'<line x1="{zx0:.0f}" y1="{zy0:.0f}" x2="{zx1:.0f}" y2="{zy1:.0f}" stroke="#E07A1F" stroke-width="3" marker-end="url(#g)"/>')
    s.append(f'<text x="{zx1+32:.0f}" y="{zy1+50:.0f}" font-family="sans-serif" font-size="15" fill="#C25E10" text-anchor="middle" font-weight="bold">Tiefe</text>')
    s.append('</svg>'); return s
# ---- MOCK: cleaner tiled panels ----
def tilemock():
    W,H=400,250
    cols,rpp=6,3
    x0,y0=95,46
    sx,sy=24,22
    gap=34; R=6
    def py(p,r): return y0+p*(rpp*sy+gap)+r*sy
    def px(c): return x0+c*sx
    s=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}">']
    s.append('<defs><marker id="a" markerWidth="11" markerHeight="11" refX="7" refY="4" orient="auto"><path d="M0,0 L8,4 L0,8 Z" fill="#555"/></marker><marker id="gg" markerWidth="10" markerHeight="10" refX="6" refY="3" orient="auto"><path d="M0,0 L7,3 L0,6 Z" fill="#2D7DD2"/></marker></defs>')
    s.append(f'<text x="{W/2:.0f}" y="22" font-family="sans-serif" font-size="16" fill="#C25E10" text-anchor="middle" font-weight="bold">Gekachelt: &#187;Tiefe&#171; = H&#246;he je Panel</text>')
    for p in range(2):
        fill="#2D7DD2" if p==0 else "#5A9BD8"
        bx,by=px(0)-9,py(p,0)-9; bw,bh=(cols-1)*sx+18,(rpp-1)*sy+18
        s.append(f'<rect x="{bx:.0f}" y="{by:.0f}" width="{bw:.0f}" height="{bh:.0f}" rx="6" fill="none" stroke="#B8C4D4" stroke-width="1.5" stroke-dasharray="4,3"/>')
        s.append(f'<text x="{bx+bw+10:.0f}" y="{by+18:.0f}" font-family="sans-serif" font-size="13" fill="#666">Panel {p+1}</text>')
        for c in range(cols):
            for r in range(rpp): s.append(f'<circle cx="{px(c):.0f}" cy="{py(p,r):.0f}" r="{R}" fill="{fill}"/>')
    # data cable
    s.append(f'<line x1="{px(cols-1):.0f}" y1="{py(0,rpp-1):.0f}" x2="{px(0):.0f}" y2="{py(1,0):.0f}" stroke="#2D7DD2" stroke-width="2.4" stroke-dasharray="5,3" marker-end="url(#gg)"/>')
    # Kachel-Hoehe brace (panel1)
    lx=x0-30
    s.append(f'<line x1="{lx}" y1="{py(0,0)-8:.0f}" x2="{lx}" y2="{py(0,rpp-1)+8:.0f}" stroke="#E07A1F" stroke-width="2.4"/>')
    s.append(f'<line x1="{lx-5}" y1="{py(0,0)-8:.0f}" x2="{lx+5}" y2="{py(0,0)-8:.0f}" stroke="#E07A1F" stroke-width="2.4"/>')
    s.append(f'<line x1="{lx-5}" y1="{py(0,rpp-1)+8:.0f}" x2="{lx+5}" y2="{py(0,rpp-1)+8:.0f}" stroke="#E07A1F" stroke-width="2.4"/>')
    s.append(f'<text x="{lx-7}" y="{(py(0,0)+py(0,rpp-1))/2:.0f}" font-family="sans-serif" font-size="14" fill="#C25E10" text-anchor="middle" font-weight="bold" transform="rotate(-90 {lx-7} {(py(0,0)+py(0,rpp-1))/2:.0f})">Tiefe</text>')
    # total Hoehe
    lx2=lx-30
    s.append(f'<line x1="{lx2}" y1="{py(0,0)-8:.0f}" x2="{lx2}" y2="{py(1,rpp-1)+8:.0f}" stroke="#555" stroke-width="2.2" marker-end="url(#a)" marker-start="url(#a)"/>')
    s.append(f'<text x="{lx2-7}" y="{(py(0,0)+py(1,rpp-1))/2:.0f}" font-family="sans-serif" font-size="14" fill="#333" text-anchor="middle" transform="rotate(-90 {lx2-7} {(py(0,0)+py(1,rpp-1))/2:.0f})">H&#246;he</text>')
    # Breite
    byb=py(1,rpp-1)+26
    s.append(f'<line x1="{px(0):.0f}" y1="{byb:.0f}" x2="{px(cols-1):.0f}" y2="{byb:.0f}" stroke="#555" stroke-width="2.2" marker-end="url(#a)" marker-start="url(#a)"/>')
    s.append(f'<text x="{(px(0)+px(cols-1))/2:.0f}" y="{byb+19:.0f}" font-family="sans-serif" font-size="14" fill="#333" text-anchor="middle">Breite</text>')
    s.append('</svg>'); return s
open("/tmp/topo/mock_3d.svg","w").write("\n".join(depth3d()))
open("/tmp/topo/mock_tile.svg","w").write("\n".join(tilemock()))
print("mocks written")