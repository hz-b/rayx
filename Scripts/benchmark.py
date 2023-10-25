######################################################################
######################################################################
# HOW TO USE:

# Install all necessary python modules:
# python -m pip install openpyxl progress pandas numpy psutil GPUtil jinja2

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
from progress.bar import Bar
import re
import numpy as np
import pandas as pd
from datetime import datetime
import psutil
import GPUtil as GPU
import platform


numberOfRuns = 5
rml_files = {
    "BoringImagePlane.rml",
    "Ellipsoid.rml",
    "PlaneGratingDevAzMisVLS.rml",
    "PlaneGratingDeviationAzMis.rml",
    "PlaneMirrorMis.rml",
    "ReflectionZonePlateAzim200.rml",
    "ReflectionZonePlateDefault200Toroid.rml",
    "toroid.rml",
}

def parse_benchmark_results(result_string):
    # Making \r optional to support both Windows and Linux
    pattern = r"BENCH: ([\w\-\:\.]+): \r?\n([\de\-\.]+)s"
    
    matches = re.findall(pattern, result_string)

    result_dict = {}
    for name, time in matches:
        if name in result_dict:
            result_dict[name] += float(time)
        else:
            result_dict[name] = float(time)

    #print(result_dict)
    return result_dict


# Only execute inside IDE with rayx as root
def checkForTerminal():
    cwd = os.getcwd()
    TerminalApp_Path = "build/bin/release/rayx"
    if platform.system() == "Windows":
        TerminalApp_Path += ".exe"
    path_to_input = "Scripts/benchmark-inputs/"
    test = os.path.join(cwd, TerminalApp_Path)
    return os.path.exists(test), test, path_to_input


def calculate_statistics(results):
    statistics = []

    for file_results in results:
        # Create a list of all unique keys
        all_keys = set(key for result in file_results for key in result.keys())

        file_statistics = {}

        # Calculate the mean and standard deviation for each key
        for key in all_keys:
            values = [result[key] for result in file_results if key in result]
            mean = np.mean(values)
            std_dev = np.std(values)

            file_statistics[key] = {"mean": mean, "std_dev": std_dev}

        statistics.append(file_statistics)

    return statistics


def get_cpu_info():
    try:
        if platform.system() == "Windows":
            return (
                subprocess.check_output("wmic cpu get name")
                .decode()
                .split("\n")[1]
                .strip()
            )
        elif platform.system() == "Linux":
            output = subprocess.check_output("lscpu").decode().split("\n")
            for line in output:
                if "Model name:" in line:
                    return line.split(":")[1].strip()
            
        else:
            return "Unknown system, couldn't get CPU info"
    except Exception as e:
        return f"Couldn't get CPU info: {e}"


def get_hardware_info():
    gpus = GPU.getGPUs()
    gpu_info = {
        "GPU model": gpus[0].name if gpus else "No GPU found",
        "Total GPU memory": f"{gpus[0].memoryTotal}MB" if gpus else "N/A",
    }

    info = {
        "CPU model": get_cpu_info(),
        "CPU cores": psutil.cpu_count(),
        "Total RAM": f"{psutil.virtual_memory().total / (1024 ** 3):.2f}GB",
    }
    info.update(gpu_info)
    return info


def save_statistics(statistics, file_names):
    # Get current date and time
    now = datetime.now().strftime("%Y%m%d_%H%M%S")

    # Get hardware info
    hardware_info = get_hardware_info()

    # Create dataframe for hardware info
    hardware_df = pd.DataFrame.from_dict(
        hardware_info, orient="index", columns=["Value"]
    )

    all_statistics = [hardware_df]
    for i, (file_name, file_statistics) in enumerate(zip(file_names, statistics)):
        df = pd.DataFrame(file_statistics).T
        df.index = pd.MultiIndex.from_product([[file_name], df.index])
        df.index.rename(["File", "Value"], inplace=True)
        df.reset_index(inplace=True)
        df.set_index(["File"], inplace=True)
        all_statistics.append(df)
    result_df = pd.concat(all_statistics)

    # Save as CSV
    csv_filename = f"Scripts/benchmark-outputs/statistics_{now}.csv"
    result_df.to_csv(csv_filename)


def main():
    exists, path, path_to_input_dir = checkForTerminal()
    if not (exists):
        print("Check for build!")
        return

    results = []
    with Bar("Benchmarking", max=len(rml_files) * numberOfRuns) as bar:
        for file in rml_files:
            resultBatch = []
            for i in range(numberOfRuns):
                # print(f"Running {file} [{i}]")
                with tempfile.TemporaryFile() as tempf:
                    proc = subprocess.Popen(
                        [
                            path,
                            "-i",
                            path_to_input_dir + str(file),
                            "--benchmark",
                        ],
                        stdout=tempf,
                    )
                    proc.wait()
                    tempf.seek(0)
                    resultString = tempf.read().decode("utf-8")
                    resultBatch.append(parse_benchmark_results(resultString))

                bar.next()
            results.append(resultBatch)
    statistics = calculate_statistics(results)
    save_statistics(statistics, list(rml_files))

    return


# main
if __name__ == "__main__":
    main()
