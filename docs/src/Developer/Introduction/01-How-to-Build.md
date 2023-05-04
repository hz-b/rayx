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
  - Make sure to check the Vulkan Memory Allocator option (not used yet, but likely in the future)
- Install [HDF5](https://www.hdfgroup.org/downloads/hdf5/) library 
- Install python3, python3-dev and python3-matplotlib 
- We also recommend to use the MSVC compiler for Windows [Visual Studio](https://visualstudio.microsoft.com/de/downloads/) (2019 or newer)

Known Issues:
- Sometimes installing the VulkanSDK in the root directory of your drive causes problems. If you have problems with the VulkanSDK, try installing it in a different directory.

### VSCode
- Open the project in VSCode, where you will be asked to select a build kit (gcc, etc.)
- Also allow the CMake Extension to configure the project
- Then you can click build in the bottom panel

You can also set a custom generator for cmake in the .vscode/settings.json file. Ninja is recommended for faster builds. In the following image you can see an example config for the generator in vscode.
![](../../res/vscode_ninja_config.png)

### Manually
- Open a terminal in the project folder
- Run `cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release` (Replace with your Visual Studio version)

## On Ubuntu
- Make sure you have cmake, gcc, gdb and make installed and on the latest version
- Install Vulkan SDK from [here](https://vulkan.lunarg.com/sdk/home) (Under Linux -> Ubuntu Packages you can select a version)
- Install python3, python3-dev and python3-matplotlib 
- The project also uses [libhdf5](https://github.com/BlueBrain/HighFive). Make sure that you have the correct libraries _(Ubuntu and debian-based Distros)_ :  `apt update && apt -y install libblas-dev liblapack-dev libhdf5-dev`
- Make sure that the libraries are installed at `/usr/include/hdf5/serial` and `/usr/lib/x86_64-linux-gnu/hdf5/serial`

## On Arch Linux
- You can get all packages through pacman/yay/...
- Specific instructions will be added later

## Fedora

```
sudo dnf install cmake gcc gdb vulkan vulkan-tools vulkan-validation-layers hdf5-devel ninja-build gcc-c++ vulkan-loader-devel glslc
```
