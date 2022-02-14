#!./rayxvenv/bin/python3

import string
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from pandas import HDFStore


def importOutput():
    """
    Import output h5 format and clean data
    """
    # TODO
    # filename = "output.h5"
    # h5 = HDFStore(filename)
    # print(h5.get())
    df = pd.DataFrame()
    return df


def plotOutput(filename: string):
    """
    Plots output file to a matplotlib figure
    """

    df = importOutput()
    f = plt.figure(figsize=(5, 10))
    plt.scatter(df['Xloc'], df['Yloc'], s=0.2)
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('Image Plane Footprint')
    plt.legend(['Ray'], markerscale=4)
    plt.show()

    return 0
