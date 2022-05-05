# Start script with a different python interpreter

import subprocess


def startPlot(plotTitle: str, plot_type: int):
    """
    Start the plotting process. 
    Decodes params coming from C++ CallObject
    """
    title = plotTitle.decode('utf-8')
    python_venv_bin = "./rayxvenv/bin/python3"
    output = subprocess.check_call(
        [python_venv_bin, "python/py_plot.py", title, str(plot_type)])
    return 1
