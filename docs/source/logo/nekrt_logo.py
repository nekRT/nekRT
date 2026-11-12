"""Logo: a Lagrange basis polynomial (centre node of N Gauss-Lobatto-Legendre
points) with the area under it shaded, two balls flying off, and a streak
orbiting the y-axis.

Usage:
    python make_logo.py                      # cividis, no axes; writes logo_white*.png/.svg
                                             # and logo_dark*.png/.svg (each square + circle)
    python make_logo.py --cmap magma --axes  # other colormap, axes on
    python make_logo.py --no-orbits          # hide the orbit streak
    python make_logo.py --no-circle          # skip the circle-cropped versions

Requires Python >= 3.10, numpy, matplotlib.
"""
from dataclasses import dataclass, replace

import numpy as np
import matplotlib

import matplotlib.pyplot as plt
from matplotlib.collections import PolyCollection
from matplotlib.patches import Circle
from matplotlib.path import Path


# ----------------------------------------------------------------------------
# Configuration: everything you are likely to want to tweak lives here.
# ----------------------------------------------------------------------------
@dataclass
class Config:
    cmap: str = "magma"          # viridis, magma, inferno, plasma, ...
    show_axes: bool = False
    transparent: bool = False      # transparent background in saved file
    # any matplotlib color; None -> low end of the colormap (dark)
    bg_color: str | None = "white"

    # Polynomial: values at the n_points GLL nodes (left to right).
    # A one-hot vector gives a Lagrange basis polynomial (here: the centre node).
    n_points: int = 9
    node_values: tuple = (0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0)

    # Colormap positions (0..1) used for each element.
    fill_range: tuple = (0.15, 0.75)   # vertical gradient under the curve
    curve_pos: float = 0.0     # 0.0 = darkest colour of the map
    node_pos: float = 0.0
    ball_pos: float = 0.7             # same colour for both balls
    streak_pos: float = 0.92           # same colour for both streaks

    # Balls. Each ball is launched from (ball_x, ball_y) at ball_angle_deg
    # (degrees from +x) and flies ball_dist before stopping.
    #   ball_y = None  -> launch from the curve at ball_x
    #   ball_y = 0.0   -> launch from the x axis (e.g. emerging from below it)
    # The streak runs from the launch point to the ball unless streak_len is set.
    ball_x: tuple = (-0.27, 0.32)
    ball_y: tuple = (0.0, None)
    ball_angle_deg: tuple = (235.0, 55.0)
    ball_dist: tuple = (0.65, 0.78)
    ball_radius: tuple = (0.085, 0.07)
    streak_len: tuple = (None, None)
    streak_width: float = 0.6          # streak width at the ball, x ball radius

    # points, for a 6 in figure (scales with fig_size)
    curve_lw: float = 5.0

    # Orbit streaks circling the y-axis (like electrons orbiting a nucleus).
    # Each orbit: (a, b, tilt_deg, y_center, head_deg, span_deg)
    #   a, b      horizontal / vertical semi-axes of the (perspective-flattened) orbit
    #   tilt_deg  rotation of the orbit ellipse in the image plane
    #   head_deg  orbital angle of the streak's leading end (-90 = front-centre)
    #   span_deg  angular length of the streak
    show_orbits: bool = True
    orbits: tuple = (
        (1.00, 0.20, -25.0, 0.28, 25, 300.0),
    )
    orbit_pos: float = 0.55        # colormap position of the streaks
    orbit_width: float = 0.045     # streak width at the head, in data units
    orbit_head_radius: float = 0.0  # >0 draws a ball at the head of each streak

    margin: float = 0.10           # blank border around the content (fraction)
    # Circle crop: fits the smallest circle around the content and leaves the
    # corners transparent. (main() writes both a square and a circle version.)
    circle: bool = False
    fig_size: tuple = (6, 6)
    dpi: int = 300


# ----------------------------------------------------------------------------
# Numerics
# ----------------------------------------------------------------------------
def gll_nodes(n):
    """n Gauss-Lobatto-Legendre nodes on [-1, 1]: endpoints plus the roots of P'_{n-1}."""
    if n < 2:
        raise ValueError("need at least 2 GLL points")
    interior = np.polynomial.legendre.Legendre.basis(n - 1).deriv().roots()
    return np.concatenate(([-1.0], np.sort(interior.real), [1.0]))


def lagrange_eval(xn, yn, x):
    """Evaluate the Lagrange interpolant through (xn, yn) at points x."""
    x = np.asarray(x, dtype=float)
    out = np.zeros_like(x)
    for j in range(len(xn)):
        lj = np.ones_like(x)
        for m in range(len(xn)):
            if m != j:
                lj *= (x - xn[m]) / (xn[j] - xn[m])
        out += yn[j] * lj
    return out


# ----------------------------------------------------------------------------
# Drawing helpers
# ----------------------------------------------------------------------------
def draw_ribbon(ax, pts, half_w, alpha, rgb, z_of_point, layers=48):
    """Draw a tapered ribbon along the polyline `pts` whose opacity grows from tail
    to head, with a round cap at the head.

    Line segments with round caps and alpha overlap and show as beads. Instead the
    ribbon is built from nested single polygons: layer k covers the stretch from
    point s_k to the head, and the per-layer alphas are chosen so that the
    cumulative opacity follows `alpha` along the length. There are no seams.

    z_of_point gives a zorder per point so a ribbon can pass behind one object and
    in front of another; the ribbon is cut where it changes.
    """
    n = len(pts)
    d = np.gradient(pts, axis=0)
    d /= np.linalg.norm(d, axis=1, keepdims=True)
    nrm = np.column_stack([-d[:, 1], d[:, 0]])
    left = pts + nrm * half_w[:, None]
    right = pts - nrm * half_w[:, None]
    phi = np.linspace(0, np.pi, 14)       # left -> forward -> right
    cap = pts[-1] + half_w[-1] * (np.cos(phi)[:, None] * nrm[-1]
                                  + np.sin(phi)[:, None] * d[-1])

    starts = np.unique(np.linspace(0, n - 1, layers + 1).astype(int))[:-1]
    ends = list(starts[1:]) + [n - 1]
    polys, cols = {}, {}
    prev = 0.0
    for s, e in zip(starts, ends):
        cum = float(np.clip(alpha[(s + e) // 2], 0.0, 0.98))
        if cum <= prev:
            continue
        a_k = 1.0 - (1.0 - cum) / (1.0 - prev)
        prev = cum
        # contiguous runs of equal z between s and the head
        zs = z_of_point[s:]
        cuts = np.flatnonzero(np.diff(zs)) + 1
        for run in np.split(np.arange(s, n), cuts):
            r0, r1 = run[0], min(run[-1] + 1, n - 1)       # overlap one vertex
            top = left[r0:r1 + 1]
            bottom = right[r0:r1 + 1][::-1]
            verts = np.vstack([top, cap, bottom]) if r1 == n - \
                1 else np.vstack([top, bottom])
            z = float(z_of_point[run[0]])
            polys.setdefault(z, []).append(verts)
            cols.setdefault(z, []).append((*rgb, a_k))
    for z, vs in polys.items():
        ax.add_collection(PolyCollection(vs, facecolors=cols[z], edgecolors="none",
                                         zorder=z))


def draw_streak(ax, cmap, p0, p1, color_pos, width, z, ext):
    """Motion streak from p0 (tail) to p1 (ball): fades in, widens toward p1,
    plus two thin converging speed lines."""
    rgb = cmap(color_pos)[:3]
    n = 60
    t = np.linspace(0, 1, n)
    pts = p0[None, :] + t[:, None] * (p1 - p0)[None, :]
    draw_ribbon(ax, pts, 0.5 * width * (0.04 + 0.96 * t ** 0.8),
                0.9 * t ** 1.2, rgb, np.full(n, z))
    ext.append((pts, 0.5 * width))

    normal = np.array([-(p1 - p0)[1], (p1 - p0)[0]])
    normal /= np.linalg.norm(normal)
    for off, frac in ((+1, 0.65), (-1, 0.5)):
        sh = off * normal * width * 1.1
        a = p1 - (p1 - p0) * frac + sh
        b = p1 + sh * 0.4
        ts = np.linspace(0, 1, 40)
        q = a[None, :] + ts[:, None] * (b - a)[None, :]
        draw_ribbon(ax, q, np.full(40, 0.11 * width), 0.8 * ts ** 1.2, rgb,
                    np.full(40, z))
        ext.append((q, 0.11 * width))


def draw_orbit(ax, cmap, orbit, color_pos, width, head_radius, ball_pos, ext):
    """A tapered, fading streak traveling around the y-axis on a flattened ellipse.
    The near half (below the centre line) is drawn in front of the plot, the far
    half behind it, which gives the orbit its depth."""
    a, b, tilt, yc, head, span = orbit
    n = 240
    t = np.linspace(0, 1, n)
    # tail -> head, increasing angle
    th = np.deg2rad(head - span * (1 - t))
    px, py = a * np.cos(th), b * np.sin(th)
    c, s_ = np.cos(np.deg2rad(tilt)), np.sin(np.deg2rad(tilt))
    pts = np.column_stack([c * px - s_ * py, yc + s_ * px + c * py])
    # lower half = nearer the viewer
    z = np.where(np.sin(th) < 0, 7.0, 0.5)
    draw_ribbon(ax, pts, 0.5 * width * (0.04 + 0.96 * t ** 0.8),
                0.95 * t ** 1.1, cmap(color_pos)[:3], z)
    ext.append((pts, 0.5 * width))
    if head_radius > 0:
        draw_ball(ax, cmap, pts[-1], head_radius, ball_pos,
                  z=7.2 if np.sin(th[-1]) < 0 else 0.6)


def draw_ball(ax, cmap, center, r, color_pos, z):
    c = np.array(cmap(color_pos))
    ax.add_patch(Circle(center, r, fc=c, ec="none", zorder=z))
    # soft highlight for a bit of volume
    hl = np.clip(c[:3] + 0.35, 0, 1)
    ax.add_patch(Circle(center + np.array([-0.30, 0.30]) * r, 0.38 * r,
                        fc=(*hl, 0.55), ec="none", zorder=z + 0.1))


# ----------------------------------------------------------------------------
# Main figure
# ----------------------------------------------------------------------------
def make_logo(cfg: Config = Config()):
    cmap = matplotlib.colormaps[cfg.cmap]
    bg = cfg.bg_color or cmap(0.0)
    k = cfg.fig_size[0] / 6.0       # point-sized things scale with the figure

    xn = gll_nodes(cfg.n_points)
    yn = np.array(cfg.node_values, dtype=float)
    if len(yn) != len(xn):
        raise ValueError("node_values must have n_points entries")
    n_balls = len(cfg.ball_x)
    for name in ("ball_y", "ball_angle_deg", "ball_dist", "ball_radius", "streak_len"):
        if len(getattr(cfg, name)) != n_balls:
            raise ValueError(
                f"{name} must have {n_balls} entries (one per ball)")

    x = np.linspace(-1, 1, 800)
    y = lagrange_eval(xn, yn, x)
    baseline = 0.0
    ext = [(np.column_stack([x, y]), 0.03), (np.column_stack([xn, yn]), 0.05)]

    fig, ax = plt.subplots(figsize=cfg.fig_size)
    # circle mode draws its own background disc
    flat = cfg.transparent or cfg.circle
    fig.patch.set_facecolor("none" if flat else bg)
    ax.set_facecolor("none" if flat else bg)

    # --- shaded area between curve and the x axis (positive AND negative lobes):
    # vertical colormap gradient clipped to the union of the two regions.
    def region(yy):
        return Path(np.column_stack([np.r_[x, x[::-1]], np.r_[yy, np.full_like(x, baseline)]]))
    clip = Path.make_compound_path(region(np.maximum(y, baseline)),
                                   region(np.minimum(y, baseline)))
    ytop = max(y.max(), baseline)
    ybot = min(y.min(), baseline)
    grad = np.linspace(cfg.fill_range[1], cfg.fill_range[0], 256)[:, None]
    im = ax.imshow(grad, extent=(-1, 1, ybot, ytop), origin="upper",
                   cmap=cmap, vmin=0, vmax=1, aspect="auto", zorder=1,
                   interpolation="bilinear")
    im.set_clip_path(clip, ax.transData)

    # --- orbit streaks around the y-axis
    if cfg.show_orbits:
        for orb in cfg.orbits:
            draw_orbit(ax, cmap, orb, cfg.orbit_pos, cfg.orbit_width,
                       cfg.orbit_head_radius, cfg.ball_pos, ext)

    # --- curve and nodes
    ax.plot(x, y, color=cmap(cfg.curve_pos), lw=cfg.curve_lw * k,
            solid_capstyle="round", zorder=3)
    ax.scatter(xn, yn, s=70 * k ** 2, color=cmap(cfg.node_pos), edgecolor=bg,
               linewidths=2 * k, zorder=4)

    # --- balls
    for i in range(n_balls):
        x0 = cfg.ball_x[i]
        y0 = cfg.ball_y[i]
        if y0 is None:
            y0 = lagrange_eval(xn, yn, np.array([x0]))[0]
        r = cfg.ball_radius[i]
        ang = np.deg2rad(cfg.ball_angle_deg[i])
        d = np.array([np.cos(ang), np.sin(ang)])
        launch = np.array([x0, y0])
        center = launch + d * cfg.ball_dist[i]
        length = cfg.streak_len[i] if cfg.streak_len[i] is not None else cfg.ball_dist[i]
        tail = center - d * length
        draw_streak(ax, cmap, tail, center - d * r * 0.3, cfg.streak_pos,
                    cfg.streak_width * r, z=5, ext=ext)
        draw_ball(ax, cmap, center, r, cfg.ball_pos, z=6)
        ext.append((center[None, :], r))

    # --- framing: square window centred on the content, with a margin
    pts = np.vstack([p for p, _ in ext])
    pads = np.concatenate([np.full(len(p), pad) for p, pad in ext])
    if cfg.circle:
        # smallest circle enclosing the content (Badoiu-Clarkson iteration)
        centre = 0.5 * (pts.min(axis=0) + pts.max(axis=0))
        for i in range(1, 501):
            far = pts[(np.linalg.norm(pts - centre, axis=1) + pads).argmax()]
            centre = centre + (far - centre) / (i + 1)
        half = (np.linalg.norm(pts - centre, axis=1) +
                pads).max() * (1 + cfg.margin)
        if not cfg.transparent:
            ax.add_patch(Circle(centre, half, fc=bg, ec="none", zorder=-1))
    else:
        lo = (pts - pads[:, None]).min(axis=0)
        hi = (pts + pads[:, None]).max(axis=0)
        centre = 0.5 * (lo + hi)
        half = 0.5 * (hi - lo).max() * (1 + cfg.margin)
    ax.set_aspect("equal")
    ax.set_xlim(centre[0] - half, centre[0] + half)
    ax.set_ylim(centre[1] - half, centre[1] + half)
    if cfg.show_axes:
        txt = cmap(cfg.curve_pos)
        ax.tick_params(colors=txt)
        for sp in ax.spines.values():
            sp.set_color(txt)
        fig.subplots_adjust(0.08, 0.06, 0.98, 0.98)
    else:
        ax.set_axis_off()
        fig.subplots_adjust(0, 0, 1, 1)
    return fig


def main():
    import argparse
    p = argparse.ArgumentParser()
    p.add_argument("--cmap", default=Config.cmap)
    p.add_argument("--axes", action="store_true", help="show axes")
    p.add_argument("--transparent", action="store_true")
    p.add_argument("--no-orbits", action="store_true",
                   help="hide the orbit streaks")
    p.add_argument("--no-circle", action="store_true",
                   help="skip the circle-cropped version")
    p.add_argument("--out", default="logo",
                   help="output stem; writes <stem>_white and <stem>_dark, each as "
                        "square and _circle, as .png and .svg")
    a = p.parse_args()

    base = Config(cmap=a.cmap, show_axes=a.axes, transparent=a.transparent,
                  show_orbits= a.no_orbits)
    # Two looks: white background with the dark curve, and the colormap's own dark
    # end as background with the curve and nodes flipped to the light end.
    looks = {
        "white": base,
        "dark": replace(base, bg_color=None,
                        curve_pos=1.0 - base.curve_pos, node_pos=1.0 - base.node_pos),
    }
    for look, cfg in looks.items():
        versions = [(cfg, f"{a.out}_{look}")]
        if not a.axes and not a.no_circle:   # a circle crop makes no sense with axes on
            versions.append((replace(cfg, circle=True),
                            f"{a.out}_{look}_circle"))
        for c, stem in versions:
            fig = make_logo(c)
            for ext in ("png", "svg"):
                fig.savefig(f"{stem}.{ext}", dpi=c.dpi,
                            transparent=c.transparent or c.circle,
                            facecolor=fig.get_facecolor())
            plt.close(fig)
            print(
                f"saved {stem}.png / .svg  (cmap={c.cmap}, look={look}, circle={c.circle})")


if __name__ == "__main__":
    main()
