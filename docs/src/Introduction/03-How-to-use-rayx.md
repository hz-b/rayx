# RAYX Commands
After a successful build, type `-h` or `--help` for a summary of all known commands.

> Hint: `-c` or `--command` are accepted. But `-command` can result in errors.

```
Terminal application for rayx
Usage: ./rayx [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -c,--ocsv                   Output stored as .csv file.
  -b,--batch INT              Batch size for Vulkan tracing
  -B,--benchmark              Benchmark application and output to stdout
  -x,--cpu                    Tracing on CPU
  -p,--plot                   Plot output footprints and histograms.
  -l,--list                   List available devices
  -d,--device INT             Device ID
  -i,--input TEXT             Input RML File or Directory.
  -v,--version
  -f                          Fix the seed to RAYX::FIXED_SEED (Uses default)
  -s,--seed INT               Provided user seed
  -S,--sequential             trace sequentially
  -V,--verbose                Dump more information
  -F,--format TEXT            Format output CSV / H5 data
```



# Command descriptions
| Command name  | description                                                                                                                                              |
| ------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`      | Prints the help message.                                                                                                                                 |
| `--ocsv`      | To store the result as a .csv (defaults to .h5). This is not recommended with a massive ray amount.                                                      |
| `--batch`     | Tells the tracer how large a batch of rays should be. This is useful for optimizing compute times.                                                       |
| `--benchmark` | Uses RAYX-CORE's profiling to benchmark the application. The results are printed at the end of the trace.                                                |
| `--cpu`       | Run Tracing on CPU, instead of using the GPU.                                                                                                            |
| `--plot`      | After a successful trace, the output data from the last Image Plane element gets plotted. The application will only exit once the plot window is closed. |
| `--list`      | Gives a list of devices in your computer that are supported but rayx.                                                                                    |
| `--device`    | Select a specific device for the computations. This is useful when your PC has multiple GPUs.                                                            |
| `--input`     | Path to the RML file to be used as imported beamline.                                                                                                    |
| `--version`   | Prints the application's meta info.                                                                                                                      |
| `--dummy`     | Run a dummy beamline with some optical elements, useful for testing.                                                                                     |
| `--format`    | Specify the output format of the ray data. Example: \|Ray-ID\|Event-ID\|X-position\|Y-position\|Z-position\|                                             |