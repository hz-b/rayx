######################################################################
######################################################################
# HOW TO USE:

# Install all necessary python modules:
# python -m pip install openpyxl, progress, pandas, numpy, psutil, GPUtil, jinja2

# Compile RAYX in Release mode
# Close all other programs
# Make sure the Device is plugged in (Laptops not on battery power)
# Make sure Power Mode is on Max Performance
# Run this script
# A csv file will be created in the benchmark-outputs folder
# 2 csv files can be compared using the compare-benchmarks.py script

######################################################################
######################################################################

import sys
import os
import subprocess
import tempfile
import re
import numpy as np
import pandas as pd
from datetime import datetime
import platform


numberOfRuns = 5


def parse_benchmark_results(result_string):
    pattern = r"BENCH: ([\w\-\:\.]+): \r\n([\de\-\.]+)s"
    matches = re.findall(pattern, result_string)

    result_dict = {}
    for name, time in matches:
        if name in result_dict:
            result_dict[name] += float(time)
        else:
            result_dict[name] = float(time)
    return result_dict


# Only execute inside IDE with rayx as root
def checkForTerminal():
    cwd = os.getcwd()
    TerminalApp_Path = "build/bin/debug/rayx-core-tst"
    test = os.path.join(cwd, TerminalApp_Path)
    return os.path.exists(test), test



def main():
    exists, path = checkForTerminal()
    if not (exists):
        print("Check for build!")
        return

    
    proc = subprocess.Popen(
        [path, "-x"],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )

    failed_tests = []
    current_test = ""

    while True:
        output = proc.stdout.readline().decode()
        if output == '' and proc.poll() is not None:
            break
        if output:
            if '[ RUN      ]' in output:
                current_test = output.strip()
            if 'Failure' in output:
                failed_tests.append(current_test)

    if proc.poll() != 0:  # if the subprocess didn't exit correctly
        print("Some tests did not execute successfully")

    if failed_tests:
        print("\nThe following tests failed:")
        for failed_test in failed_tests:
            print(failed_test)

    if proc.poll() != 0:  # if the subprocess didn't exit correctly
        print("\nMake sure that all tests are passing before analyzing coverage")
    proc.wait()

    



    #save_statistics(statistics, list(rml_files))

    return


# main
if __name__ == "__main__":
    main()
