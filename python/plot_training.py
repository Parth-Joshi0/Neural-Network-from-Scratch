"""Plot lap completion rate and average reward from a train.py log.

Usage:
    python train.py | tee train_log.txt
    python plot_training.py train_log.txt [output.png]

Lap rate is exact (train.py prints every successful episode). Average reward
uses the avg10 column of the lines printed every 10th episode.
"""
import re
import sys
from pathlib import Path

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter

BUCKET = 500
LINE = re.compile(r'Episode\s+(\d+) \| reward:\s+([-\d.]+) \| avg10:\s+([-\d.]+) '
                  r'\| steps:\s+(\d+) .*success: (True|False)')

BLUE, INK, MUTED, GRID, AXIS, BG = '#2a78d6', '#0b0b0b', '#52514e', '#e4e3df', '#b5b3ad', '#fcfcfb'


def parse_log(path):
    episodes = {}
    with open(path) as f:
        for line in f:
            m = LINE.search(line)
            if m:
                episodes[int(m[1])] = (float(m[3]), m[5] == 'True')
    if not episodes:
        sys.exit(f'No training lines found in {path}')
    return episodes


def bucket(episodes):
    last = max(episodes)
    xs, rates, rewards = [], [], []
    for end in range(BUCKET, last + 1, BUCKET):
        laps = sum(1 for e, (_, ok) in episodes.items() if end - BUCKET < e <= end and ok)
        sampled = [episodes[e][0] for e in range(end - BUCKET + 10, end + 1, 10) if e in episodes]
        if not sampled:
            continue
        xs.append(end)
        rates.append(100 * laps / BUCKET)
        rewards.append(sum(sampled) / len(sampled))
    return xs, rates, rewards


def plot(episodes, out_path):
    xs, rates, rewards = bucket(episodes)
    laps = sorted(e for e, (_, ok) in episodes.items() if ok)
    total = max(episodes)

    plt.rcParams.update({'font.size': 11, 'text.color': INK, 'axes.labelcolor': MUTED,
                         'xtick.color': MUTED, 'ytick.color': MUTED})
    fig, axes = plt.subplots(2, 1, figsize=(9, 6.2), sharex=True, facecolor=BG)
    panels = [
        (axes[0], rates, 'Lap completion rate (%)', lambda v, _: f'{v:.0f}%',
         max(10, (max(rates) // 10 + 1) * 10)),
        (axes[1], rewards, 'Average episode reward', lambda v, _: f'{v:,.0f}', None),
    ]
    for ax, ys, title, fmt, top in panels:
        ax.set_facecolor(BG)
        ax.fill_between(xs, ys, color=BLUE, alpha=0.12, linewidth=0)
        ax.plot(xs, ys, color=BLUE, linewidth=2, solid_joinstyle='round')
        ax.plot(xs[-1], ys[-1], 'o', color=BLUE, markersize=6, markeredgecolor=BG, markeredgewidth=2)
        if laps:
            ax.axvline(laps[0], color=MUTED, linestyle=(0, (3, 3)), linewidth=1)
        ax.set_title(title, loc='left', fontsize=12, color=INK, pad=8)
        ax.yaxis.set_major_formatter(FuncFormatter(fmt))
        ax.set_ylim(0, top)
        ax.grid(axis='y', color=GRID, linewidth=1)
        ax.set_axisbelow(True)
        for side in ('top', 'right', 'left'):
            ax.spines[side].set_visible(False)
        ax.spines['bottom'].set_color(AXIS)
        ax.tick_params(length=0)

    if laps:
        left_side = laps[0] > total * 0.3
        axes[0].text(laps[0] + (-1 if left_side else 1) * total * 0.01, axes[0].get_ylim()[1] * 0.93,
                     f'First lap\nepisode {laps[0]:,}', ha='right' if left_side else 'left',
                     va='top', fontsize=10, color=MUTED)

    axes[1].set_xlim(0, total * 1.02)
    axes[1].xaxis.set_major_formatter(FuncFormatter(lambda v, _: '0' if v == 0 else f'{v / 1000:g}k'))
    axes[1].set_xlabel(f'Training episode (each point = {BUCKET} episodes)')
    fig.suptitle(f'REINFORCE training: {total:,} episodes, {len(laps):,} laps',
                 x=0.065, ha='left', fontsize=14, fontweight='bold')
    fig.tight_layout(rect=(0, 0, 1, 0.96))
    fig.savefig(out_path, dpi=150, facecolor=BG)
    print(f'Saved {out_path}  ({total:,} episodes, {len(laps):,} laps'
          + (f', first at episode {laps[0]:,})' if laps else ')'))


if __name__ == '__main__':
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    default_out = Path(__file__).resolve().parent.parent / 'assets' / 'training_results.png'
    out = Path(sys.argv[2]) if len(sys.argv) > 2 else default_out
    out.parent.mkdir(parents=True, exist_ok=True)
    plot(parse_log(sys.argv[1]), out)
