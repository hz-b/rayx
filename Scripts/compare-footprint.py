#!/usr/bin/env python3
"""Compare a RAYX footprint against a RAY-UI export, as a self-contained HTML page.

  ./compare-footprint.py rayx.csv --object 15 [--rayui Camera-RawRaysOutgoing.csv] [-o out.html]

RAYX records image-plane hits in element coordinates where x/z are in-plane and y is
the normal; RAY-UI reports the same plane as OX/OY. So RAYX z is plotted against RAY-UI OY.
"""
import argparse, html, math, sys

SEQ = ["#cde2fb", "#b7d3f6", "#9ec5f4", "#86b6ef", "#6da7ec", "#5598e7",
       "#3987e5", "#2a78d6", "#256abf", "#1c5cab", "#184f95", "#104281", "#0d366b"]


def in_plane(a, b, c):
    """Drop the axis with the smallest spread - that is the surface normal - and keep the
    other two in coordinate order. Mirrors and image planes put the normal on different
    axes, and RAYX and RAY-UI disagree about which, so detect it rather than assume."""
    cols = [a, b, c]
    spread = [max(v) - min(v) if v else 0.0 for v in cols]
    normal = spread.index(min(spread))
    kept = [i for i in (0, 1, 2) if i != normal]
    return cols[kept[0]], cols[kept[1]], "xyz"[kept[0]], "xyz"[kept[1]]


RAYX_FIELD = [f"electric_field_{a} ({p})" for a in "xyz" for p in ("real", "imag")]
RAYX_NEEDED = ["position_x", "position_y", "position_z", "object_id"] + RAYX_FIELD


def read_rayx(path, obj):
    """The CSV only holds the attributes in the record mask, so look columns up by name."""
    xa, ya, za, ii = [], [], [], []
    with open(path) as f:
        header = [s.strip() for s in next(f).split(",")]
        missing = [n for n in RAYX_NEEDED if n not in header]
        if missing:
            sys.exit(f"{path}: missing column(s) {', '.join(missing)}; "
                     "re-export with these attributes recorded")
        col = {n: header.index(n) for n in RAYX_NEEDED}
        field = [col[n] for n in RAYX_FIELD]
        for line in f:
            c = [s.strip() for s in line.split(",")]
            if len(c) < len(header) or c[col["object_id"]] != str(obj):
                continue
            xa.append(float(c[col["position_x"]]))
            ya.append(float(c[col["position_y"]]))
            za.append(float(c[col["position_z"]]))
            ii.append(sum(float(c[k]) ** 2 for k in field))
    if not xa:
        return [], [], [], "", ""
    u, v, au, av = in_plane(xa, ya, za)
    return u, v, ii, au, av


def read_rayui(path):
    xs, ys, zs, ii = [], [], [], []
    with open(path) as f:
        for _ in range(2):
            next(f)
        for line in f:
            c = [s.strip() for s in line.split("\t")]
            if len(c) < 12:
                continue
            try:
                x, y, z, s0 = float(c[3]), float(c[4]), float(c[5]), float(c[11])
            except ValueError:
                continue          # units row
            xs.append(x); ys.append(y); zs.append(z); ii.append(s0)
    if not xs:
        return [], [], [], "", ""
    u, v, au, av = in_plane(xs, ys, zs)
    return u, v, ii, au.upper(), av.upper()


def hist(vals, lo, hi, n):
    if hi <= lo:
        hi = lo + 1.0
    out = [0] * n
    for v in vals:
        k = int((v - lo) / (hi - lo) * n)
        if 0 <= k < n:
            out[k] += 1
        elif k == n:
            out[n - 1] += 1
    return out


def stats(v):
    n = len(v)
    if not n:
        return dict(n=0, lo=0, hi=0, mean=0, rms=0)
    m = sum(v) / n
    return dict(n=n, lo=min(v), hi=max(v), mean=m,
                rms=math.sqrt(max(sum((t - m) ** 2 for t in v) / n, 0.0)))


def axis_ticks(lo, hi, want=5):
    if hi <= lo:
        return [lo]
    raw = (hi - lo) / want
    mag = 10 ** math.floor(math.log10(raw))
    step = min((s * mag for s in (1, 2, 2.5, 5, 10)), key=lambda s: abs(s - raw))
    t, out = math.ceil(lo / step) * step, []
    while t <= hi + step * 1e-9:
        out.append(round(t, 10)); t += step
    return out


def fmt(v):
    a = abs(v)
    return f"{v:.0f}" if a >= 100 else f"{v:.2f}" if a >= 1 else f"{v:.3f}"


W, H, PL, PR, PT, PB = 430, 250, 52, 14, 16, 38


def frame(title, lo, hi, ylo, yhi, body, ylab):
    xt = "".join(
        f'<line x1="{PL+(t-lo)/(hi-lo)*(W-PL-PR):.1f}" y1="{H-PB}" x2="{PL+(t-lo)/(hi-lo)*(W-PL-PR):.1f}" y2="{H-PB+4}" class="tk"/>'
        f'<text x="{PL+(t-lo)/(hi-lo)*(W-PL-PR):.1f}" y="{H-PB+16}" class="tl" text-anchor="middle">{fmt(t)}</text>'
        for t in axis_ticks(lo, hi))
    yt = "".join(
        f'<line x1="{PL-4}" y1="{H-PB-(t-ylo)/(yhi-ylo)*(H-PT-PB):.1f}" x2="{W-PR}" y2="{H-PB-(t-ylo)/(yhi-ylo)*(H-PT-PB):.1f}" class="gr"/>'
        f'<text x="{PL-8}" y="{H-PB-(t-ylo)/(yhi-ylo)*(H-PT-PB)+4:.1f}" class="tl" text-anchor="end">{fmt(t)}</text>'
        for t in axis_ticks(ylo, yhi, 4))
    return (f'<figure><figcaption>{html.escape(title)}</figcaption>'
            f'<svg viewBox="0 0 {W} {H}" role="img">{yt}{body}{xt}'
            f'<line x1="{PL}" y1="{H-PB}" x2="{W-PR}" y2="{H-PB}" class="ax"/>'
            f'<text x="4" y="12" class="tl">{html.escape(ylab)}</text></svg></figure>')


def profile(title, a, b, lo, hi, nb, unit):
    ha = hist(a, lo, hi, nb)
    hb = hist(b, lo, hi, nb) if b else None
    peak = max(ha + (hb or [0])) or 1
    bw = (W - PL - PR) / nb

    def steps(hv, cls):
        d = []
        for i, v in enumerate(hv):
            x0, x1 = PL + i * bw, PL + (i + 1) * bw
            y = H - PB - v / peak * (H - PT - PB)
            d.append(f"{'M' if i == 0 else 'L'}{x0:.1f},{y:.1f}L{x1:.1f},{y:.1f}")
        return f'<path d="{"".join(d)}" class="{cls}"/>'

    marks = steps(ha, "s1")
    if hb:
        marks += steps(hb, "s2")
    tips = "".join(
        f'<rect x="{PL+i*bw:.1f}" y="{PT}" width="{bw:.2f}" height="{H-PT-PB}" class="hit">'
        f'<title>{fmt(lo+(i+0.5)*(hi-lo)/nb)} {unit}\nRAYX {ha[i]}'
        + (f'\nRAY-UI {hb[i]}' if hb else '') + '</title></rect>'
        for i in range(nb))
    return frame(title, lo, hi, 0, peak, marks + tips, "rays")


def density(title, xs, ys, xlo, xhi, ylo, yhi, nb=46):
    g = [[0] * nb for _ in range(nb)]
    hot = 0
    for x, y in zip(xs, ys):
        i = min(int((x - xlo) / (xhi - xlo) * nb), nb - 1)
        j = min(int((y - ylo) / (yhi - ylo) * nb), nb - 1)
        if 0 <= i < nb and 0 <= j < nb:
            g[j][i] += 1
            hot = max(hot, g[j][i])
    hot = hot or 1
    cw, ch = (W - PL - PR) / nb, (H - PT - PB) / nb
    cells = []
    for j in range(nb):
        for i in range(nb):
            v = g[j][i]
            if not v:
                continue
            c = SEQ[min(int(math.sqrt(v / hot) * (len(SEQ) - 1)), len(SEQ) - 1)]
            cells.append(f'<rect x="{PL+i*cw:.1f}" y="{H-PB-(j+1)*ch:.1f}" width="{cw+.4:.2f}" '
                         f'height="{ch+.4:.2f}" fill="{c}"><title>{v} rays</title></rect>')
    return frame(title, xlo, xhi, ylo, yhi, "".join(cells), "vertical (mm)")


def main():
    p = argparse.ArgumentParser()
    p.add_argument("rayx"); p.add_argument("--object", type=int, default=15)
    p.add_argument("--rayui"); p.add_argument("-o", default="footprint-comparison.html")
    p.add_argument("--label", default="UE112 camera")
    a = p.parse_args()

    ax, az, ai, aU, aV = read_rayx(a.rayx, a.object)
    if not ax:
        sys.exit(f"no events for object {a.object} in {a.rayx}")
    bx, by, bi, bU, bV = read_rayui(a.rayui) if a.rayui else ([], [], [], "", "")

    # RAY-UI reports absolute flux in S0 while RAYX normalises |E|^2 to 1; compare shapes
    if bi and max(bi) > 0:
        k = (sum(ai) / len(ai)) / (sum(bi) / len(bi))
        bi = [t * k for t in bi]
        intensity_note = f" RAY-UI intensity scaled by {k:.3g} (S0 is absolute flux)."
    else:
        intensity_note = ""
    axes_note = f" In-plane axes: RAYX {aU}/{aV}" + (f" vs RAY-UI O{bU}/O{bV}." if bU else ".")

    xlo, xhi = min(ax + bx), max(ax + bx)
    zlo, zhi = min(az + by), max(az + by)
    px, pz = (xhi - xlo) * .04 or 1, (zhi - zlo) * .04 or 1
    xlo, xhi, zlo, zhi = xlo - px, xhi + px, zlo - pz, zhi + pz
    ilo, ihi = 0.0, max(ai + bi) * 1.02 or 1

    charts = [density(f"RAYX — footprint density ({aU}/{aV})", ax, az, xlo, xhi, zlo, zhi)]
    if bx:
        charts.append(density(f"RAY-UI — footprint density (O{bU}/O{bV})", bx, by, xlo, xhi, zlo, zhi))
    charts += [profile(f"Profile along {aU}", ax, bx, xlo, xhi, 60, "mm"),
               profile(f"Profile along {aV}", az, by, zlo, zhi, 60, "mm"),
               profile("Intensity", ai, bi, ilo, ihi, 48, "")]

    rows = ""
    for nm, va, vb in ((f"in-plane {aU} (mm)", ax, bx), (f"in-plane {aV} (mm)", az, by), ("intensity", ai, bi)):
        sa, sb = stats(va), stats(vb)
        rows += (f"<tr><th>{nm}</th><td>{sa['n']}</td><td>{fmt(sa['mean'])}</td><td>{fmt(sa['rms'])}</td>"
                 f"<td>{fmt(sa['lo'])} … {fmt(sa['hi'])}</td>")
        rows += (f"<td>{sb['n']}</td><td>{fmt(sb['mean'])}</td><td>{fmt(sb['rms'])}</td>"
                 f"<td>{fmt(sb['lo'])} … {fmt(sb['hi'])}</td></tr>" if bx else
                 "<td>—</td><td>—</td><td>—</td><td>—</td></tr>")

    legend = ('<p class="key"><span class="sw s1w"></span>RAYX'
              + ('<span class="sw s2w"></span>RAY-UI' if bx else '') + '</p>')
    note = "" if bx else '<p class="note">No RAY-UI export supplied — pass <code>--rayui</code> to overlay it.</p>'

    with open(a.o, "w") as f:
        f.write(f"""<!doctype html><meta charset="utf-8"><title>Footprint — {html.escape(a.label)}</title>
<style>
:root{{color-scheme:light;--surface:#fcfcfb;--ink:#0b0b0b;--ink2:#52514e;--rule:#dcdbd5;--s1:#2a78d6;--s2:#eb6834}}
@media(prefers-color-scheme:dark){{:root:not([data-theme=light]){{color-scheme:dark;--surface:#1a1a19;--ink:#fff;--ink2:#c3c2b7;--rule:#3a3a37;--s1:#3987e5;--s2:#d95926}}}}
body{{margin:0;padding:24px;background:var(--surface);color:var(--ink);font:14px/1.5 system-ui,sans-serif}}
h1{{font-size:17px;margin:0 0 2px}}.sub{{color:var(--ink2);margin:0 0 18px}}
.grid{{display:flex;flex-wrap:wrap;gap:20px}}figure{{margin:0;flex:1 1 430px;max-width:100%}}
figcaption{{font-weight:600;margin-bottom:4px}}svg{{width:100%;height:auto;overflow:visible}}
.tl{{fill:var(--ink2);font-size:10px}}.gr{{stroke:var(--rule);stroke-width:1}}.ax{{stroke:var(--rule);stroke-width:1}}
.tk{{stroke:var(--rule);stroke-width:1}}
.s1{{fill:none;stroke:var(--s1);stroke-width:2}}.s2{{fill:none;stroke:var(--s2);stroke-width:2}}
.hit{{fill:transparent}}.hit:hover{{fill:var(--rule);opacity:.35}}
.key{{margin:0 0 14px;color:var(--ink2)}}.sw{{display:inline-block;width:11px;height:11px;border-radius:2px;margin:0 6px 0 0;vertical-align:-1px}}
.s1w{{background:var(--s1)}}.s2w{{background:var(--s2);margin-left:18px}}
table{{border-collapse:collapse;margin-top:22px;font-size:13px}}th,td{{text-align:right;padding:5px 10px;border-bottom:1px solid var(--rule)}}
th:first-child,td:first-child{{text-align:left}}thead th{{color:var(--ink2);font-weight:600}}
.note{{color:var(--ink2)}}code{{font-size:12px}}
</style>
<h1>Footprint comparison — {html.escape(a.label)}</h1>
<p class="sub">RAYX object {a.object}. RAYX <code>z</code> is plotted against RAY-UI <code>OY</code>: the normal axis is detected per element, not assumed.{html.escape(axes_note)}{html.escape(intensity_note)}</p>
{legend}{note}
<div class="grid">{''.join(charts)}</div>
<table><thead><tr><th></th><th colspan="4">RAYX</th><th colspan="4">RAY-UI</th></tr>
<tr><th></th><th>n</th><th>mean</th><th>rms</th><th>range</th><th>n</th><th>mean</th><th>rms</th><th>range</th></tr></thead>
<tbody>{rows}</tbody></table>
""")
    print(f"  wrote {a.o}  (RAYX {len(ax)} rays" + (f", RAY-UI {len(bx)})" if bx else ", no RAY-UI data)"))


if __name__ == "__main__":
    main()
