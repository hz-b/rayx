# TerminalApp 
After a successful build, type `-h` or `--help` for a summary of all known commands.

> Hint: `-c` or `--command` are accepted. But `-command` can result in errors.

```
Terminal Application for RAY-X
Usage: ./TerminalApp [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -v,--version                
  -i,--input TEXT             Input RML File Path.
  -x,--cpu                    Tracing on CPU
  -m,--mult                   Multiple plots extension at output.
  -b,--benchmark              Benchmark application:     (RML Parse → Trace → Output Storage)
  -d,--dummy                  Run an in-house built Beamline.
  -c,--ocsv                   Output stored as .csv file.
  -p,--plot                   Plot output footprints and histograms.
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
| `--dummy` | Run a dummy beamline with some optical elements, useful for testing. | 
| `--help` | Prints the help message. | 