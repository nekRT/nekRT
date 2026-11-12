"""Logo: Lagrange interpolant through 5 Gauss-Lobatto-Legendre points, with the
area underneath shaded and two balls flying off the curve.

Usage:
    python make_logo.py                      # cividis, no axes, writes logo.png/.svg
    python make_logo.py --cmap magma --axes  # other colormap, axes on
"""
from dataclasses import dataclass, field

import numpy as np
import matplotlib

import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib.patches import Circle, Polygon


# ----------------------------------------------------------------------------
# Configuration: everything you are likely to want to tweak lives here.
# ----------------------------------------------------------------------------
@dataclass
class Config:
    cmap: str = "cividis"          # viridis, magma, inferno, plasma, ...
    show_axes: bool = False
    transparent: bool = False      # transparent background in saved file
    # None -> use the low end of the colormap (dark)
    bg_color: str | None = None

    # Interpolation data: values at the 5 GLL nodes (left to right).
    n_points: int = 9
    # Lagrange basis poly for the centre node
    node_values: tuple = (0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0)

    # Colormap positions (0..1) used for each element.
    fill_range: tuple = (0.15, 0.75)   # vertical gradient under the curve
    curve_pos: float = 0.95
    node_pos: float = 0.95
    ball_pos: tuple = (1.00, 0.78)     # one entry per ball
    streak_pos: tuple = (0.90, 0.68)

    # Balls: x location of launch point on the curve, launch angle (deg from +x),
    # distance flown from the curve, and radius.
    ball_x: tuple = (-0.32, 0.32)
    ball_angle_deg: tuple = (125.0, 55.0)
    ball_dist: tuple = (0.70, 0.78)
    ball_radius: tuple = (0.085, 0.07)
    streak_len: tuple = (0.70, 0.78)

    curve_lw: float = 5.0
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


def lagrange_deriv(xn, yn, x, h=1e-6):
    return (lagrange_eval(xn, yn, x + h) - lagrange_eval(xn, yn, x - h)) / (2 * h)


# ----------------------------------------------------------------------------
# Drawing helpers
# ----------------------------------------------------------------------------
def draw_streak(ax, cmap, p0, p1, color_pos, lw_max, z):
    """Motion streak from p0 (tail) to p1 (ball): fades in alpha, widens toward p1."""
    n = 60
    t = np.linspace(0, 1, n)
    pts = p0[None, :] + t[:, None] * (p1 - p0)[None, :]
    segs = np.stack([pts[:-1], pts[1:]], axis=1)
    base = np.array(cmap(color_pos))
    colors = np.tile(base, (n - 1, 1))
    tm = 0.5 * (t[:-1] + t[1:])
    colors[:, 3] = 0.9 * tm ** 1.2
    widths = lw_max * (0.04 + 0.96 * tm ** 0.8)
    lc = LineCollection(segs, colors=colors, linewidths=widths,
                        capstyle="round", zorder=z)
    ax.add_collection(lc)
    # a couple of thinner parallel "speed lines" for extra motion feel
    normal = np.array([-(p1 - p0)[1], (p1 - p0)[0]])
    normal /= np.linalg.norm(normal)
    for off, frac in ((+1, 0.65), (-1, 0.5)):
        sh = off * normal * lw_max * 0.006
        a = p1 - (p1 - p0) * frac + sh
        b = p1 + sh * 0.4
        ts = np.linspace(0, 1, 40)
        q = a[None, :] + ts[:, None] * (b - a)[None, :]
        sg = np.stack([q[:-1], q[1:]], axis=1)
        cl = np.tile(base, (39, 1))
        cl[:, 3] = 0.8 * (0.5 * (ts[:-1] + ts[1:])) ** 1.2
        ax.add_collection(LineCollection(sg, colors=cl, linewidths=lw_max * 0.22,
                                         capstyle="round", zorder=z))


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

    xn = gll_nodes(cfg.n_points)
    yn = np.array(cfg.node_values, dtype=float)
    if len(yn) != len(xn):
        raise ValueError("node_values must have n_points entries")

    x = np.linspace(-1, 1, 800)
    y = lagrange_eval(xn, yn, x)
    baseline = 0.0

    fig, ax = plt.subplots(figsize=cfg.fig_size)
    fig.patch.set_facecolor("none" if cfg.transparent else bg)
    ax.set_facecolor("none" if cfg.transparent else bg)

    # --- shaded area between curve and the x axis (positive AND negative lobes):
    # vertical colormap gradient clipped to the union of the two regions.
    from matplotlib.path import Path

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

    # --- curve and nodes
    ax.plot(x, y, color=cmap(cfg.curve_pos), lw=cfg.curve_lw,
            solid_capstyle="round", zorder=3)
    ax.scatter(xn, yn, s=70, color=cmap(cfg.node_pos), edgecolor=bg,
               linewidths=2, zorder=4)

    # --- balls leaving the curve
    for i in range(2):
        x0 = cfg.ball_x[i]
        p_curve = np.array([x0, lagrange_eval(xn, yn, np.array([x0]))[0]])
        ang = np.deg2rad(cfg.ball_angle_deg[i])
        d = np.array([np.cos(ang), np.sin(ang)])
        center = p_curve + d * cfg.ball_dist[i]
        tail = center - d * cfg.streak_len[i]
        draw_streak(ax, cmap, tail, center - d * cfg.ball_radius[i] * 0.3,
                    cfg.streak_pos[i], lw_max=cfg.ball_radius[i] * 110, z=5)
        draw_ball(ax, cmap, center, cfg.ball_radius[i], cfg.ball_pos[i], z=6)

    # --- framing
    ax.set_aspect("equal")
    ax.set_xlim(-1.2, 1.2)
    ax.set_ylim(-0.55, 1.85)
    if cfg.show_axes:
        txt = cmap(0.8)
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
    p.add_argument("--out", default="logo",
                   help="output stem (writes .png and .svg)")
    a = p.parse_args()

    cfg = Config(cmap=a.cmap, show_axes=a.axes, transparent=a.transparent)
    fig = make_logo(cfg)
    for ext in ("png", "svg"):
        fig.savefig(f"{a.out}.{ext}", dpi=cfg.dpi,
                    transparent=cfg.transparent,
                    facecolor=fig.get_facecolor())
    print(f"saved {a.out}.png / .svg  (cmap={cfg.cmap}, axes={cfg.show_axes})")


if __name__ == "__main__":
    main()
