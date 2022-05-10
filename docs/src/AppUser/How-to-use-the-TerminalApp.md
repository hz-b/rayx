# TerminalApp 
After a successful build, type `-h` or `--help` for a summary of all known commands.

> Hint: `-c` or `--command` are accepted. But `-command` can result in errors.

```
RAY-X Terminal usage: TerminalApp [OPTION].. [FILE]

Options:
-p --plot	 Plot output footprints and histograms.
-c --ocsv	 Output stored as .csv file.
-i --input	 Input RML File Path.
-d --dummy	 Run an in-house Beamline.
-h --help	 Output this message.
-v --version
```



# Command descriptions
| Command name | description |
| ------ | ------ |
| `--plot` | After a successful trace, the output data from the last Image Plane element gets plotted. The application will only exit once the plot window is closed. |
| `--ocsv` | To store the result as a .csv (defaults to .h5). This is not recommended with a massive ray amount. |
| `--input` | Path to the RML file to be used as imported beamline. |
| `--version` | Prints the application's meta info. | 
| `--dummy` | Run a dummy beamline with some optical elements, useful for testing. | 
| `--help` | Prints the help message. | 