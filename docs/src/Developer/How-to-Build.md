# How to Build

For building the project and running the project, we recommend to use [Visual Studio Code](https://code.visualstudio.com/) as your IDE with these extensions: C/C++ and CMake Tools.
You can use any IDE you like, but the VSCode extensions make the building process very easy.
If you are on UNIX you can also use the `compile.sh` script to compile.

Clone the git repository with:
`git clone --recurse-submodules git@github.com:hz-b/RAY-X.git -b development`

to get the dev branch and to make sure, you also clone all of it's submodules.

## On Windows
- Install [CMake](https://cmake.org/download/)
- Install [Vulkan](https://vulkan.lunarg.com/) for Windows (1.3 or newer)
- Install [HDF5](https://www.hdfgroup.org/downloads/hdf5/) library 
- Install python3, python3-dev and python3-matplotlib 
- Open the project in VSCode, where you will be asked to select a build kit (gcc, etc.)
- Also allow the CMake Extension to configure the project
- Then you can click build in the bottom panel

## On Linux
- Make sure you have cmake, gcc, gdb and make installed and on the latest version
- Download Vulkan SDK from [https://vulkan.lunarg.com/sdk/home](https://vulkan.lunarg.com/sdk/home)
- run `source setup-env.sh` in terminal in the downloaded folder and run `vkcube` to test it (!now only in this specific terminal available!)
- add: `source "path/to/dir/setup-env.sh" `to `~/.profile` file to have vulkan available in any terminal/IDE... (e.g: `source "$HOME/Downloads/1.3.xxx./setup-env.sh"`)
- test again with vkcube in any terminal/IDE
- see also [https://vulkan.lunarg.com/doc/sdk/1.3.224.1/linux/getting_started.html](https://vulkan.lunarg.com/doc/sdk/1.3.224.1/linux/getting_started.html)
- Install python3, python3-dev and python3-matplotlib 
- in usr/bin/ `ln -s "/path/to/vulkansdk/x86_64/bin/glslc" glslc`
- The project also uses [libhdf5](https://github.com/BlueBrain/HighFive). Make sure that you have the correct libraries _(Ubuntu and debian-based Distros)_ :  `apt update && apt -y install libblas-dev liblapack-dev libhdf5-dev`
- Make sure that the libraries are installed at `/usr/include/hdf5/serial` and `/usr/lib/x86_64-linux-gnu/hdf5/serial`

## Fedora

```
sudo dnf install cmake gcc gdb vulkan vulkan-tools vulkan-validation-layers hdf5-devel ninja-build gcc-c++ vulkan-loader-devel glslc
```
