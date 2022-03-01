#! ./rayxvenv/bin/python3

import string
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import h5py 

def importOutput(filename: string):
    """
    Import output h5 format and clean data
    """
    # file will be closed when we exit from WITH scope
    with h5py.File(filename, 'r') as h5f:
        dataset = h5f['data/0']  # default, Columns are hard-coded, to be changed if necessary
        np_dataset = np.array_split(list(dataset), len(dataset)/16)
        df = pd.DataFrame(np_dataset, columns=['Xloc', 'Yloc', 'Zloc', 'Weight', 'Xdir', 'Ydir', 'Zdir', 'Energy',
                                               'Stokes0', 'Stokes1', 'Stokes2', 'Stokes3', 'pathLength', 'order', 'lastElement', 'extraParam'])  # Change when changing vectors
    return df


def plotOutput(filename: string):
    """
    Plots output file to a matplotlib figure
    """
    #TODO: Enhance
    df = importOutput(filename)
    f = plt.figure(figsize=(10, 10))
    f.canvas.manager.set_window_title('Image Plane Footprint') 
    plt.scatter(df['Xloc'], df['Yloc'], s=0.2)
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('Image Plane Footprint')
    plt.legend(['Ray'], markerscale=4)
    plt.show()
    return 0


if __name__ == '__main__':
    plotOutput('output.h5')
