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
        dataset = h5f["0"]
        df = pd.DataFrame(dataset, columns=['RayId', 'SnapshotID', 'Xloc', 'Yloc', 'Zloc', 'Weight', 'Xdir', 'Ydir', 'Zdir', 'Energy',
                                             'Stokes0', 'Stokes1', 'Stokes2', 'Stokes3', 'pathLength', 'order',
                                             'lastElement', 'extraParam'])
    df = df[df["Weight"] == W_JUST_HIT_ELEM]
    df = df[["Xloc", "Yloc", "Zloc", "lastElement"]]
    return df


BAR = None

def plot(filename: str):
    df = importOutput(filename)
    elems = df["lastElement"].unique()
    int_elems = list(map(int, elems))

    fig, ax = plt.subplots()

    rax = fig.add_axes([0.02, 0.7, 0.1, 0.15])
    radio = RadioButtons(rax, int_elems)

    def react(e):
        global BAR
        ax.clear()
        e = float(e)
        d = df[df["lastElement"] == e]

        # we don't know whether the element is in the XY or XZ plane,
        # this `relevance` tests which axis is more important.
        relevance = lambda v: max(v) - min(v)
        Y = relevance(d["Yloc"]) > relevance(d["Zloc"])
        h = ax.hist2d(d["Xloc"], d["Yloc"] if Y else d["Zloc"], bins=200)
        if BAR:
            BAR.remove()
            BAR = None
        BAR = plt.colorbar(h[3], ax=ax)
        ax.set_title("Footprint of element " + str(int(e)))
        ax.set_xlabel("x in mm")
        ax.set_ylabel(("y" if Y else "z") + "in mm")
        plt.draw()

    radio.on_clicked(react)

    react(int_elems[0])

    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: plot.py <h5-file>")
        sys.exit()
    plot(sys.argv[1])
