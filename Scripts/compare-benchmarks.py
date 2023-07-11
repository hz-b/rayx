import pandas as pd
from jinja2 import Environment, FileSystemLoader
import os
import sys
import numpy as np


def read_csv(file):
    data = pd.read_csv(
        file, header=0, names=["test_name", "operation", "mean", "std_dev"]
    )
    hardware_info = data.loc[
        data["std_dev"].isna() & ~data["test_name"].isin(["mean", "std_dev"])
    ]
    benchmark_data = data.loc[~data["std_dev"].isna()]
    return hardware_info, benchmark_data


def compare_benchmarks(benchmark1, benchmark2):
    merged = pd.merge(
        benchmark1,
        benchmark2,
        on=["test_name", "operation"],
        how="outer",
        suffixes=("_1", "_2"),
    )
    return merged


def adaptive_round(x):
    if x >= 1:
        return round(x, 2)
    elif x >= 0.01:
        return round(x, 4)
    else:
        return round(x, 6)


def calculate_difference(data):
    data["mean_1_diff"] = ((data["mean_2"] - data["mean_1"]) / data["mean_1"]) * 100
    data["mean_2_diff"] = ((data["mean_1"] - data["mean_2"]) / data["mean_2"]) * 100

    data["highlight_1"] = False
    data["highlight_2"] = False

    # Update highlight for row based on value magnitude and percentage difference
    for index, row in data.iterrows():
        if (
            abs(row["mean_1"]) > 0.1
            and abs(row["mean_1_diff"]) > 10
            or abs(row["mean_1"]) <= 0.1
            and abs(row["mean_1_diff"]) > 50
        ):
            data.at[index, "highlight_2"] = True
        if (
            abs(row["mean_2"]) > 0.1
            and abs(row["mean_2_diff"]) > 10
            or abs(row["mean_2"]) <= 0.1
            and abs(row["mean_2_diff"]) > 50
        ):
            data.at[index, "highlight_1"] = True

    return data


def compare_hardware_info(hardware_info_1, hardware_info_2):
    hardware_info_1 = hardware_info_1.rename(
        columns={"test_name": "parameter", "operation": "value_1"}
    )
    hardware_info_2 = hardware_info_2.rename(
        columns={"test_name": "parameter", "operation": "value_2"}
    )
    hardware_info = pd.merge(
        hardware_info_1,
        hardware_info_2,
        on="parameter",
        how="outer",
    )
    return hardware_info


def output_html(data, hardware_info, filenames, output_file):
    env = Environment(loader=FileSystemLoader("Scripts"))
    env.filters["abs"] = abs
    template = env.get_template("template.html")
    with open(output_file, "w") as f:
        f.write(
            template.render(
                data=data,
                hardware_info=hardware_info,
                filenames=filenames,
            )
        )
        print("Output written to", output_file)


def main(filenames):
    hardware_info_1, benchmark_data_1 = read_csv(filenames[0])
    hardware_info_2, benchmark_data_2 = read_csv(filenames[1])
    comparison = compare_benchmarks(benchmark_data_1, benchmark_data_2)
    comparison = (
        comparison.groupby(["test_name"], as_index=False)
        .apply(calculate_difference)
        .reset_index(drop=True)
    )
    comparison["mean_1"] = comparison["mean_1"].apply(adaptive_round)
    comparison["mean_2"] = comparison["mean_2"].apply(adaptive_round)
    comparison["mean_1_diff"] = comparison["mean_1_diff"].apply(round)
    comparison["mean_2_diff"] = comparison["mean_2_diff"].apply(round)

    hardware_info = compare_hardware_info(hardware_info_1, hardware_info_2)
    output_html(
        comparison,
        hardware_info,
        [os.path.basename(filename) for filename in filenames],
        "Scripts/benchmark-outputs/comparison.html",
    )


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: compare-benchmarks.py <csv_file_1> <csv_file_2>")
        sys.exit(1)
    main(sys.argv[1:])
