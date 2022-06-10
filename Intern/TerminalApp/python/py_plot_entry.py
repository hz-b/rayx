# Start script with a different python interpreter

import subprocess
from sys import platform

def startPlot(plotTitle: str, plot_type: int):
    """
    Start the plotting process. 
    Decodes params coming from C++ CallObject
    """
    title = plotTitle.decode('utf-8')
    if platform == "win32" : # Winows
        python_venv_bin = ".\\rayxvenv\\bin\\python3"
        module_path = "python\\py_plot.py"
    else: #Linux, MacOS and other
        python_venv_bin = "./rayxvenv/bin/python3"
        module_path = "python/py_plot.py"

    output = subprocess.check_call(
        [python_venv_bin, module_path, title, str(plot_type)])
    return 1
