# RAY-X Profiling

Here you can find an overview of the profiling tools used in RAY-X. Further reading can be done in the code (see Instrumentor.h file in "Debug"-folder).

_The code for this profiler, was taken from a tutorial by TheCherno on Youtube._

## Usage

The Profiling used in RAYX is a simple json export of the time each profiled function took. If you want to add profiling to a function, simply add:

`RAYX_PROFILE_FUNCTION;`

at the beginning of the function. 

For profiling specific scopes, you can use `RAYX_PROFILE_SCOPE`, which takes in a name for the scoped timer. 

## Profiled Data

The data will be output to the directory, the executable was started in. You can use the tracing functionality of any chromium based browser to read the data.

For example, use the URL "chrome://tracing" for the Chrome browser.

## Sessions

The profiling can happen in multiple sessions. You can't have to sessions running at the same time though. This is functionality is mainly supposed to give the option to separate data for different parts of the program. 

An example would be seperating measurements into _startup, runtime, shutdown._ 

The Macros used to create and end sessions:

* `RAYX_PROFILE_BEGIN_SESSION`
* `RAYX_PROFILE_END_SESSION` 