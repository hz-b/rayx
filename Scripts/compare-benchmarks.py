import pandas as pd
from jinja2 import Environment, FileSystemLoader


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


def calculate_difference(data):
    data["diff"] = abs((data["mean_2"] - data["mean_1"]) / data["mean_1"]) * 100
    data["highlight"] = (data["diff"] > 100) & (data["mean_1"] < 0.1) | (
        data["diff"] > 10
    ) & (data["mean_1"] >= 0.1)
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
    template = env.get_template("template.html")
    with open(output_file, "w") as f:
        f.write(
            template.render(
                data=data,
                hardware_info=hardware_info,
                filenames=filenames,
            )
        )


def main():
    filenames = [
        "Scripts/benchmark-outputs/statistics_20230705_124414.csv",
        "Scripts/benchmark-outputs/statistics_20230705_135954.csv",
    ]
    hardware_info_1, benchmark_data_1 = read_csv(filenames[0])
    hardware_info_2, benchmark_data_2 = read_csv(filenames[1])
    comparison = compare_benchmarks(benchmark_data_1, benchmark_data_2)
    comparison = (
        comparison.groupby(["test_name"], as_index=False)
        .apply(calculate_difference)
        .reset_index(drop=True)
    )
    hardware_info = compare_hardware_info(hardware_info_1, hardware_info_2)
    output_html(
        comparison,
        hardware_info,
        filenames,
        "Scripts/benchmark-outputs/comparison.html",
    )


if __name__ == "__main__":
    main()
