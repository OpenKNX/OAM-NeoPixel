def chain(role="master", hi=0):
    W,H=500,330
    ndev=3; per=4; total=ndev*per
    x0=55; xr=W-45; sx=(xr-x0)/(total-1); yled=210; R=12
    def lx(i): return x0+i*sx
    s=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}">']
    s.append('<defs><marker id="a" markerWidth="11" markerHeight="11" refX="7" refY="4" orient="auto"><path d="M0,0 L8,4 L0,8 Z" fill="#555"/></marker>'
             '<marker id="gr" markerWidth="10" markerHeight="10" refX="6" refY="3.5" orient="auto"><path d="M0,0 L7,3.5 L0,7 Z" fill="#2D7DD2"/></marker>'
             '<marker id="or" markerWidth="11" markerHeight="11" refX="7" refY="4" orient="auto"><path d="M0,0 L8,4 L0,8 Z" fill="#C25E10"/></marker></defs>')
    tt="Master" if role=="master" else "Slave"
    s.append(f'<text x="{W/2:.0f}" y="26" font-family="sans-serif" font-size="18" fill="#2D7DD2" text-anchor="middle" font-weight="bold">Effektkette &#8212; dieses Ger&#228;t = {tt}</text>')
    sub="Mehrere Ger&#228;te bilden ein zusammenh&#228;ngendes LED-Band" if role=="master" else "Zeigt seinen Abschnitt ab &#187;Mein Offset&#171;"
    s.append(f'<text x="{W/2:.0f}" y="44" font-family="sans-serif" font-size="14" fill="#666" text-anchor="middle">{sub}</text>')
    roles=["Master","Slave","Slave"]; by=82; bh=50; box=[]
    for d in range(ndev):
        cx0=lx(d*per); cx1=lx(d*per+per-1); mid=(cx0+cx1)/2
        box.append((cx0-14,by,(cx1-cx0)+28,bh,mid))
    ysync=73
    a0=box[0][4]; a1=(box[2][4] if role=="master" else box[hi][4])
    s.append(f'<line x1="{a0:.0f}" y1="{ysync}" x2="{a1-3:.0f}" y2="{ysync}" stroke="#2D7DD2" stroke-width="2.6" marker-end="url(#gr)"/>')
    s.append(f'<rect x="{(a0+a1)/2-24:.0f}" y="{ysync-20:.0f}" width="48" height="17" rx="4" fill="#FFFFFF"/>')
    s.append(f'<text x="{(a0+a1)/2:.0f}" y="{ysync-6:.0f}" font-family="sans-serif" font-size="14" fill="#2D7DD2" text-anchor="middle" font-weight="bold">Sync</text>')
    syncdevs=(1,2) if role=="master" else (hi,)
    for d in syncdevs: s.append(f'<line x1="{box[d][4]:.0f}" y1="{ysync}" x2="{box[d][4]:.0f}" y2="{by-3:.0f}" stroke="#2D7DD2" stroke-width="1.6" stroke-dasharray="3,3"/>')
    for d in range(ndev):
        bx,by_,bw,bh_,mid=box[d]; is_hi=(d==hi)
        fill="#E8F2FC" if is_hi else "#FFFFFF"; bcol="#2D7DD2" if is_hi else "#9AA7B5"; sw=3.4 if is_hi else 1.8
        s.append(f'<rect x="{bx:.0f}" y="{by_}" width="{bw:.0f}" height="{bh_}" rx="7" fill="{fill}" stroke="{bcol}" stroke-width="{sw}"/>')
        s.append(f'<text x="{mid:.0f}" y="{by_+21:.0f}" font-family="sans-serif" font-size="15" fill="#333" text-anchor="middle" font-weight="bold">Ger&#228;t {d+1}</text>')
        rc="#2D7DD2" if roles[d]=="Master" else "#8A98A8"
        s.append(f'<text x="{mid:.0f}" y="{by_+40:.0f}" font-family="sans-serif" font-size="13.5" fill="{rc}" text-anchor="middle">{roles[d]}</text>')
        if is_hi: s.append(f'<text x="{mid:.0f}" y="{by_+bh_+17:.0f}" font-family="sans-serif" font-size="13" fill="#2D7DD2" text-anchor="middle" font-weight="bold">&#9650; dieses Ger&#228;t</text>')
        s.append(f'<line x1="{mid:.0f}" y1="{by_+bh_+(21 if is_hi else 2):.0f}" x2="{mid:.0f}" y2="{yled-18:.0f}" stroke="#C8D0DA" stroke-width="1.5"/>')
    # LED band: path
    for i in range(total-1):
        own = (role=="master") or (i//per==hi and (i+1)//per==hi)
        col="#2D7DD2" if own else "#DCE2E9"; w=3.5 if own else 2.5
        s.append(f'<line x1="{lx(i):.0f}" y1="{yled}" x2="{lx(i+1):.0f}" y2="{yled}" stroke="{col}" stroke-width="{w}" stroke-linecap="round"/>')
    for i in range(total):
        op=1.0 if (role=="master" or i//per==hi) else 0.28
        s.append(f'<circle cx="{lx(i):.0f}" cy="{yled}" r="{R}" fill="#D8DEE9" opacity="{op:.2f}"/>')
    if role=="master":
        s.append(f'<circle cx="{lx(0):.0f}" cy="{yled}" r="{R}" fill="#F4F6F8" stroke="#333" stroke-width="2.5"/>')
        s.append(f'<circle cx="{lx(total-1):.0f}" cy="{yled}" r="{R}" fill="#333"/>')
        s.append(f'<text x="{lx(0):.0f}" y="{yled-22:.0f}" font-family="sans-serif" font-size="12.5" fill="#333" text-anchor="middle" font-weight="bold">Start</text>')
        s.append(f'<text x="{lx(total-1):.0f}" y="{yled-22:.0f}" font-family="sans-serif" font-size="12.5" fill="#333" text-anchor="middle" font-weight="bold">Ende</text>')
        gy=yled+34
        s.append(f'<line x1="{lx(0):.0f}" y1="{gy}" x2="{lx(total-1):.0f}" y2="{gy}" stroke="#555" stroke-width="2.2" marker-end="url(#a)" marker-start="url(#a)"/>')
        s.append(f'<text x="{W/2:.0f}" y="{gy+20:.0f}" font-family="sans-serif" font-size="14" fill="#333" text-anchor="middle">Gesamtl&#228;nge (alle LEDs zusammen)</text>')
    else:
        ox=lx(hi*per); gy=yled+34
        s.append(f'<line x1="{lx(0):.0f}" y1="{gy}" x2="{ox:.0f}" y2="{gy}" stroke="#C25E10" stroke-width="2.4" marker-end="url(#or)"/>')
        s.append(f'<text x="{(lx(0)+ox)/2:.0f}" y="{gy+19:.0f}" font-family="sans-serif" font-size="13.5" fill="#C25E10" text-anchor="middle" font-weight="bold">Mein Offset</text>')
        s.append(f'<text x="{(lx(hi*per)+lx(hi*per+per-1))/2:.0f}" y="{gy+19:.0f}" font-family="sans-serif" font-size="13.5" fill="#333" text-anchor="middle">mein Abschnitt</text>')
    s.append('</svg>'); return "\n".join(s)
open("/tmp/topo/chainm.svg","w").write(chain("master",0))
open("/tmp/topo/chains.svg","w").write(chain("slave",1))
print("ok")