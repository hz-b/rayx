# How to Build

For building and running the project, we recommend using [Visual Studio Code](https://code.visualstudio.com/) (VSCode) as your IDE, along with the C/C++ and CMake Tools extensions. These extensions significantly simplify the building process. However, you are free to use any IDE of your choice. If you are on a UNIX-like system, the `compile.sh` script can be used for compilation.

## Cloning the Repository

Clone the git repository by running one of the following commands:

- Using SSH:

`git clone --recurse-submodules git@github.com:hz-b/rayx.git`

- Using HTTPS:

`git clone --recurse-submodules https://github.com/hz-b/rayx.git`

## On Windows

### Prerequisites
- Install [CMake](https://cmake.org/download/).
- Install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) for Windows (at least version 1.3 or newer).
- Install the [HDF5](https://www.hdfgroup.org/downloads/hdf5/) library.
- Install Python3, python3-dev, and python3-matplotlib.
- We recommend using the MSVC compiler for Windows, available through [Visual Studio](https://visualstudio.microsoft.com/downloads/) (2019 or newer).

### Known Issues
- Installing the VulkanSDK at the root directory of your drive may cause issues. If you encounter problems with the VulkanSDK, consider installing it in a different directory.

### Building with VSCode
- Open the project in VSCode. You will be prompted to select a build kit (e.g., gcc).
- Allow the CMake Extension to configure the project.
- You can then build the project using the build button in the bottom panel.

To use a custom generator for CMake, such as Ninja for faster builds, you can set it in the `.vscode/settings.json` file. The following is an example configuration for the generator in VSCode:
![](../../res/vscode_ninja_config.png)

### Building with Visual Studio
- Open a terminal in the project folder.
- Run the following command, replacing it with your version of Visual Studio:

`cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release`


## On Ubuntu

### Prerequisites
- Ensure cmake, gcc, gdb, and make are installed and up to date.
- Install the Vulkan SDK from [here](https://vulkan.lunarg.com/sdk/home). Select a version under Linux -> Ubuntu Packages.
- Install Python3, python3-dev, and python3-matplotlib.
- The project leverages [libhdf5](https://github.com/BlueBrain/HighFive) for data management and incorporates various other libraries for graphical user interfaces, linear algebra computations, and handling different aspects of the X11 window system. Install the necessary libraries with the following command:

`apt update && apt -y install libblas-dev liblapack-dev libhdf5-dev libgtk-3-dev pkg-config libxi-dev libxcursor-dev libxinerama-dev libxrandr-dev`
- Ensure the libraries are installed at `/usr/include/hdf5/serial` and `/usr/lib/x86_64-linux-gnu/hdf5/serial`.

## On Arch Linux

Arch Linux users can obtain all necessary packages through pacman, yay, or other package managers. Specific instructions will be provided later.

## On Fedora

To install the required packages on Fedora, run the following command:

`sudo dnf install cmake gcc gdb vulkan vulkan-tools vulkan-validation-layers hdf5-devel ninja-build gcc-c++ vulkan-loader-devel glslc blas-devel lapack-devel gtk3-devel pkg-config libXi-devel libXcursor-devel libXinerama-devel libXrandr-devel`
