# How to Build

For building and running the project, we recommend using [Visual Studio Code](https://code.visualstudio.com/) (VSCode) as your IDE, along with the C/C++ and CMake Tools extensions. These extensions significantly simplify the building process. However, you are free to use any IDE of your choice. If you are on a UNIX-like system, the `compile.sh` script can be used for compilation (see [using compile.sh](#using-compile.sh)).

## CMake Options:

|Option|Type|Default|Description|
|--|--|--|--|
|WERROR|BOOL|OFF|Treat warnings as errors. If warnings occur, compilation is aborted.|
|RAYX_ENABLE_CUDA|BOOL|ON|Enable search for Cuda on your system, if it is found, the project will be built with cuda.|
|RAYX_REQUIRE_CUDA|BOOL|OFF|Abort compilation if Cuda can not be found on your system.|

## Cloning the Repository

Clone the git repository by running one of the following commands:

- Using SSH:

`git clone --recurse-submodules git@github.com:hz-b/rayx.git`

- Using HTTPS:

`git clone --recurse-submodules https://github.com/hz-b/rayx.git`

## On Windows

### Prerequisites

- We recommend using the MSVC compiler for Windows, available through [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/)
- Install [CMake](https://cmake.org/download/)
- Install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) for Windows (at least version 1.3 or newer)
- Install the [HDF5](https://www.hdfgroup.org/downloads/hdf5/) library
- Install Python3, python3-dev, and python3-matplotlib
- Install the [Boost](https://www.boost.org/users/download/) library
    - Add Boost to your PATH
- Optional (required for Tracing on the GPU):
    - Install [Cuda](https://developer.nvidia.com/Cuda-downloads?target_os=Windows&target_arch=x86_64) (Tested with [Cuda 12.5.1](https://developer.nvidia.com/cuda-12-5-1-download-archive?target_os=Windows&target_arch=x86_64))
	See: [Determining-compiler-and-Cuda-version](#determining-compiler-and-cuda-version)
	- Add Cuda to your PATH.

### Building with VSCode
- Open the project in VSCode. You will be prompted to select a build kit (e.g., Visual Studio 2022).
- Allow the CMake Extension to configure the project.
- You can then build the project using the build button in the bottom panel.

To configure CMake options, press `ctrl` + `shift` + `p` and run: `Preferences: Open Workspace Settings (JSON)`. Here is an example of a possible configuration:
```json
{
    "cmake.generator": "Ninja", // Generate Ninja build files
    "cmake.configureSettings": {
        "RAYX_ENABLE_CUDA": "OFF", // Disable building with Cuda
    },
}
```
see: [Cmake Options](#cmake-options)

### Known Issues
- Installing the VulkanSDK at the root directory of your drive may cause issues. If you encounter problems with the VulkanSDK, consider installing it in a different directory.

## On Linux

### Prerequisites

- CMake
- Boost
- Vulkan SDK
- HDF5
- OpenMP
	(`openmp` for GCC, or `openmpi` for Clang)
- Optional:
	- Cuda [Determining-compiler-and-Cuda-version](#determining-compiler-and-cuda-version)
	- Python3
		- matplotlib

#### Ubuntu

- Install the Vulkan SDK from [here](https://vulkan.lunarg.com/sdk/home). Select a version under Linux -> Ubuntu Packages.

- The project leverages [libhdf5](https://github.com/BlueBrain/HighFive) for data management and incorporates various other libraries for graphical user interfaces, linear algebra computations, and handling different aspects of the X11 window system. Install the necessary libraries with the following command:

	`apt update && apt -y install libblas-dev liblapack-dev libhdf5-dev libgtk-3-dev pkg-config libxi-dev libxcursor-dev libxinerama-dev libxrandr-dev`

### Determining compiler and Cuda version

- MSVC:
	Depending on the Cuda version, different versions of MSVC may be supported.
    - Please take a look at the supported versions of MSVC for your Cuda installation:
        - <https://docs.nvidia.com/cuda/cuda-installation-guide-microsoft-windows/index.html>
        - Visual Studio 2019 is no longer supported, beginning at Cuda version 12.5.1
    - This projects was tested with:
        - Cuda version 12.5.1 and Visual Studio 2022
- GCC:
	Depending on the Cuda version, different versions of GCC may be supported.
	We recommend using the associated compiler shipped with your package manager.
    - Ubuntu:
    install `nvidia-cuda-dev` and `nvidia-cuda-toolkit-gcc`
    shipped compiler: `/usr/bin/cuda-g++`
    - Arch / Manjaro:
    install `cuda`
    shipped compiler: `/opt/cuda/bin/g++`
	- This projects was tested with:
		- Cuda version 12.3.2 and GCC version 13.3
		- Cuda version 12.5.1 and GCC version 13.3
	- For more information on compatible versions:
       - <https://stackoverflow.com/questions/6622454/Cuda-incompatible-with-my-gcc-version>
       - <https://gist.github.com/ax3l/9489132>
- Clang: Using clang as Cuda compiler is currently not supported. Only builds with cuda disabled are supported. See [Cmake Options](#cmake-options).

### Building with compile.sh

usage: `CXX=<path/to/compiler> ./compile.sh [OPTIONS]...`
#### Options:
- `--release` build in release mode (default: build in debug mode)
- `--cuda` enable compilation with Cuda (default: build without Cuda)\
see [Determining-compiler-and-Cuda-version](#determining-compiler-and-cuda-version) \
example usage: `CXX=cuda-g++ ./compile.sh --cuda`

### Building with CMake

`mkdir build && cd build`
`cmake [OPTIONS...] -G Ninja ..`
See [Cmake Options](#cmake-options).

## Troubleshooting:

In case you encounter issues, feel free to seek help on [github issues](https://github.com/hz-b/rayx/issues).
Please provide **system information**, the **build command** and the **error log**.
