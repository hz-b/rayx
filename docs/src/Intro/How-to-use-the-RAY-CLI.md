# RAY-CLI 
After a successful build, type `-h` or `--help` for a summary of all known commands.

> Hint: `-c` or `--command` are accepted. But `-command` can result in errors.

```
Terminal application for rayx
Usage: ./RAY-CLI [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -c,--ocsv                   Output stored as .csv file.
  -b,--batch INT              Batch size for Vulkan tracing
  -B,--benchmark              Benchmark application: (RML -> Trace -> Output)
  -x,--cpu                    Tracing on CPU
  -p,--plot                   Plot output footprints and histograms.
  -i,--input TEXT             Input RML File or Directory.
  -f                          Fix the seed to RAYX::FIXED_SEED (Uses default)
  -v,--version
  -s,--seed INT               Provided user seed
  -V,--verbose                Dump more information
```



# Command descriptions
| Command name | description |
| ------ | ------ |
| `--plot` | After a successful trace, the output data from the last Image Plane element gets plotted. The application will only exit once the plot window is closed. |
| `--mult`| Instead of having the final Image Plane, get rays Footprints for each Object in the beamline. (Only used with --plot)|
 `--ocsv` | To store the result as a .csv (defaults to .h5). This is not recommended with a massive ray amount. |
| `--input` | Path to the RML file to be used as imported beamline. |
| `--cpu` | Run Tracing on CPU, instead of using the GPU. |
| `--version` | Prints the application's meta info. | 
| `--help` | Prints the help message. | 