#!/usr/bin/python3 -B

import h5py
import sys
import pandas as pd
import matplotlib.pyplot as plt

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

def plot(filename: str):
    df = importOutput(filename)
    elems = df["lastElement"].unique()
    fig, axs = plt.subplots(len(elems))
    fig.suptitle("Ray Density")
    for e, ax in zip(elems, axs):
        d = df[df["lastElement"] == e]
        ax.hist2d(d["Xloc"], d["Yloc"], bins=200)
        ax.set_title("Footprint of element " + str(int(e)))
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: plot.py <h5-file>")
        sys.exit()
    plot(sys.argv[1])
