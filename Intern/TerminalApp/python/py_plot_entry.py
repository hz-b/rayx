# Start script with a different python interpreter

import subprocess


def startPlot(plotTitle: str):
    title = plotTitle.decode('utf-8')
    python_venv_bin = "./rayxvenv/bin/python3"
    output = subprocess.check_call(
        [python_venv_bin, "python/py_plot.py", title])
    return 1
