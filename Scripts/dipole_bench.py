######################################################################
######################################################################
# Inspired by benchmark.py

# HOW TO USE:

# Install all necessary python modules:
# python -m pip install progress, pandas, numpy, json

# Compile RAYX in Release mode
#
# Idealy:
# Close all other programs
# Make sure the Device is plugged in (Laptops not on battery power)
# Make sure Power Mode is on Max Performance

# Run this script
# The output will be written in the Terminal without context
######################################################################
######################################################################


import sys
import os
from subprocess import Popen, PIPE
import tempfile
import numpy as np
import pandas as pd
import json


numberOfRuns = 1

rml_files = {
    "dipole_bench_200.rml",
    "dipole_bench_2000.rml",
    "dipole_bench_20000.rml",
    "dipole_bench_200000.rml",
    "dipole_bench_2000000.rml",
    "dipole_bench_20000000.rml",
}

def main():
    TerminalApp_Path = "build/bin/release/rayx"
    os.chdir('../')
    path = os.path.join(os.getcwd(), TerminalApp_Path)

    path_to_input = "Scripts/dipoleRmls/"

    results = []

    for file in rml_files:
        for i in range(numberOfRuns):
            resultBatch = []
            with tempfile.TemporaryFile() as tempf:
                with Popen(
                    [
                        path,
                        "-i",
                        path_to_input + str(file),
                        "-x",
                        "--benchmark",
                        #"-c",
                    ],
                    stdout=PIPE,
                    universal_newlines= True,
                ) as proc:
                    proc.wait()
            with open('results.json', 'r') as json_file:
                result = json.load(json_file)
                result = result['traceEvents']
                for data in result:
                    if data['name'] == 'virtual std::vector<Ray> RAYX::DipoleSource::getRays() const':
                        results.append(int(data['dur']/1000))      
    for item in results:
        print(item)
    return

if __name__ == "__main__":
    main()