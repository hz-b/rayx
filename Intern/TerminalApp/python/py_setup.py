import sys
import subprocess
import os

VENV_PATH_POSIX = "/rayxvenv/bin/activate"
VENV_PATH_WINDOWS = "\\rayxvenv\\bin\\activate.bat"


def setup():
    """
    Setup for RAY-X venv
    * Checks "activate" bash script
    * Installs required packages
    New created venv will be in Application path 
    """

    def get_venv_path() -> str:
        real_path = os.path.realpath(__file__)
        _dir = real_path.split(os.sep)
        del _dir[-2:]  # remove last elements related to build/bin
        return str(os.sep + os.path.join(*_dir))

    dir_path = get_venv_path()

    if sys.platform == "linux":
        path = dir_path + VENV_PATH_POSIX
    elif sys.platform == "darwin":  # MacOS
        path = dir_path + VENV_PATH_POSIX
    elif sys.platform == "win32":  # Windows
        path = dir_path + VENV_PATH_WINDOWS
    if(not(os.path.exists(path))):  # no venv found!
        try:
            subprocess.check_call(
                [sys.executable, '-m', 'venv', str(dir_path+os.sep+'rayxvenv')])  # Make new venv : rayxvenv
            install_packages(str(dir_path+os.sep+'rayxvenv'))
        except Exception as e:
            return False
        return True
    else:
        return True


def install_packages(venv_path: str):
    """
    Installs required packages for plotting: pandas, matplotlib etc..
    * matplotlib : Interactive plotting module
    * pandas : BigData Analysis module
    * h5py : HDF5 binary data format module
    * pyqt : Necessary for plot GUI Backend access. (if ran from terminal)
    """
    python_venv_bin = os.path.join(venv_path, "bin", "python3")
    packages = ['matplotlib', 'pandas', 'h5py', 'pyqt5']
    for package in packages:
        subprocess.check_call(
            [python_venv_bin, '-m', 'pip', 'install', package])
