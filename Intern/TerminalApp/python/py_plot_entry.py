# Start script with a different python interpreter

import subprocess
import os

def startPlot(plotTitle: str, plot_type: int):
    """
    Start the plotting process. 
    Decodes params coming from C++ CallObject
    """
    title = plotTitle.decode('utf-8')
    def get_venv_path() -> str:
        real_path = os.path.realpath(__file__)
        _dir = real_path.split(os.sep)
        del _dir[-2:] # remove last elements related to build/bin
        _dir = os.path.join(*_dir)
        return str(os.sep + _dir)

    path = get_venv_path()
    python_venv_bin = os.path.join(path,"rayxvenv","bin","python3")
    plot_path = os.path.join(path,"python","py_plot.py")
    output = subprocess.check_call(
        [python_venv_bin, plot_path, title, str(plot_type)])
    return 1
