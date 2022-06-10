import sys
import subprocess
import os

VENV_PATH_POSIX = "./rayxvenv/bin/activate"
VENV_PATH_WINDOWS = "rayxvenv\\bin\\activate.bat"


def setup():
    """
    Setup for RAY-X venv
    * Checks "activate" bash script
    * Installs required packages
    New created venv will be in Application path 
    """

    if sys.platform == "linux":
        path = VENV_PATH_POSIX
    elif sys.platform == "darwin":  # MacOS
        path = VENV_PATH_POSIX
    elif sys.platform == "win32":  # Windows
        path = VENV_PATH_WINDOWS

    if(not(os.path.exists(path))):  # no venv found!
        try:
            subprocess.check_call(
                [sys.executable, '-m', 'venv', 'rayxvenv'])  # Make new venv : rayxvenv
            install_packages()
        except Exception as e:
            return False
        return True
    else:
        return True


def install_packages():
    """
    Installs required packages for plotting: pandas, matplotlib etc..
    * matplotlib : Interactive plotting module
    * pandas : BigData Analysis module
    * h5py : HDF5 binary data format module
    * pyqt : Necessary for plot GUI Backend access. (if ran from terminal)
    """
    if sys.platform == "win32" : # Winows
        python_venv_bin = ".\\rayxvenv\\bin\\python3"
    else: #Linux, MacOS and other
        python_venv_bin = "./rayxvenv/bin/python3"
    packages = ['matplotlib', 'pandas', 'h5py','pyqt5']
    
    for package in packages:
        subprocess.check_call(
            [python_venv_bin, '-m', 'pip', 'install', package])
