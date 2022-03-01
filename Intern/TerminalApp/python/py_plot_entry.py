# Start script with a different python interpreter

import subprocess

def startPlot():
    python_venv_bin = "./rayxvenv/bin/python3"
    output = subprocess.check_call([python_venv_bin,"python/py_plot.py"])
    return 1 