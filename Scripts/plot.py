#!/usr/bin/python3 -B

import h5py
import sys
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.widgets import RadioButtons

import sys

W_JUST_HIT_ELEM = 1

def importOutput(filename: str):
    """
    Import output h5 format and clean data
    """
    # file will be closed when we exit from WITH scope
    with h5py.File(filename, 'r') as h5f:
        keys = []
        for x in h5f.keys():
            if x == "rays": continue
            keys.append(int(x))
        keys = sorted(keys)

        names = []
        for x in keys:
            x = "".join([chr(y) for y in h5f[str(x)]])
            names.append(x)

        dataset = h5f["rays"]
        df = pd.DataFrame(dataset, columns=['Ray-ID', 'Event-ID', 'X-position', 'Y-position', 'Z-position', 'Event-type', 'X-direction', 'Y-direction', 'Z-direction', 'Energy',
                                             'Stokes0', 'Stokes1', 'Stokes2', 'Stokes3', 'pathLength', 'order',
                                             'lastElement', 'lightSourceIndex'])
    df = df[df["Event-type"] == W_JUST_HIT_ELEM]
    df = df[["X-position", "Y-position", "Z-position", "lastElement"]]
    return df, names

BAR = None

def plot(filename: str):
    df, names = importOutput(filename)

    fig, ax = plt.subplots()

    rax = fig.add_axes([0.02, 0.7, 0.15, 0.15])
    radio = RadioButtons(rax, names)

    def react(name):
        global BAR

        e = float(names.index(name))
        d = df[df["lastElement"] == e]

        # we don't know whether the element is in the XY or XZ plane,
        # this `relevance` tests which axis is more important.
        relevance = lambda v: v.max() - v.min()
        Y = relevance(d["Y-position"]) > relevance(d["Z-position"])
        h = ax.hist2d(d["X-position"], d["Y-position"] if Y else d["Z-position"], bins=200)
        if BAR:
            # this overwrites the old colorbar axes, instead of taking new space away from `ax`
            BAR = plt.colorbar(h[3], cax=BAR.ax)
        else:
            BAR = plt.colorbar(h[3], ax=ax)
        ax.set_title("Footprint of element " + str(int(e)))
        ax.set_xlabel("x/mm")
        ax.set_ylabel(("y" if Y else "z") + "/mm")
        plt.draw()

    radio.on_clicked(react)

    react(names[0])

    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: plot.py <h5-file>")
        sys.exit()
    plot(sys.argv[1])
