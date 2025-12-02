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
  -X,--gpu                    Tracing on GPU
  -x,--cpu                    Tracing on CPU
  -p,--plot                   Plot output footprints and histograms.
  -l,--list                   List available devices
  -d,--device INT             Device ID
  -i,--input TEXT             Input RML File or Directory.
  -o,--output TEXT            Output path or filename
  -v,--version                Print application metadata
  -f                          Fix the seed to RAYX::FIXED_SEED (Uses default)
  -s,--seed INT               Provide a user-defined seed
  -S,--sequential             Trace rays sequentially
  -V,--verbose                Print detailed debug and trace info
  -F,--format TEXT            Format output CSV / H5 data
  -m,--maxEvents INT          Maximum number of recorded events per ray
  -R,--record-element INT     Record events only for a specific element (Default: -1 to record for all)
```



# Command descriptions
| Command name       | Description                                                                                                            |
| ------------------ | ---------------------------------------------------------------------------------------------------------------------- |
| `--help`           | Prints the help message.                                                                                               |
| `--ocsv`           | Store the result as a `.csv` (defaults to `.h5`). Not recommended for large ray counts.                                |
| `--batch`          | Specifies how large a batch of rays should be. Useful for compute performance tuning.                                  |
| `--benchmark`      | Benchmarks RAYX core performance. Outputs total runtime stats to stdout.                                               |
| `--gpu`            | Run tracing on the GPU.                                                                                                |
| `--cpu`            | Run tracing on the CPU.                                                                                                |
| `--plot`           | Plots footprints and histograms from the last Image Plane element. Closes only after the user exits the plot window.   |
| `--list`           | Lists all supported compute devices available on the system.                                                           |
| `--device`         | Select a specific device by ID. Use with `--list` to see available IDs.                                                |
| `--input`          | Path to the RML file or directory to be used as the beamline.                                                          |
| `--output`         | Path where the traced ray data should be saved.                                                                        |
| `--version`        | Displays application version and build metadata.                                                                       |
| `--dummy`          | Runs a dummy test beamline with a few optical elements. Useful for quick diagnostics.                                  |
| `--seed`           | Specifies a custom seed for deterministic tracing.                                                                     |
| `--f`              | Fix the seed to a default constant (`RAYX::FIXED_SEED`).                                                               |
| `--sequential`     | Traces rays sequentially rather than in parallel.                                                                      |
| `--verbose`        | Outputs more internal information for debugging and performance tuning.                                                |
| `--format`         | Selects the output format. Supported: `csv`, `h5`.                                                                     |
| `--maxEvents`      | Limits the number of events (e.g., interactions with beamline elements) that are recorded per ray.                     |
| `--record-element` | Restrict event recording to a specific beamline element by index. Default: `-1` to record events for **all** elements. |
