#!./rayxvenv/bin/python3

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import h5py


def importOutput():
    # TODO
    filename = "output.h5"
    h5 = h5py.File(filename, 'r')
    df = pd.read_hdf(filename)
    return df


def plotOutput():
    df = importOutput()
    f = plt.figure(figsize=(5, 10))
    plt.scatter(df['Xloc'], df['Yloc'], s=0.2)
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('Image Plane Footprint')
    plt.legend(['Ray'], markerscale=4)
    plt.show()

    return 0
