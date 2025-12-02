# How to Install

Welcome to the installation guide for RAYX! This section is a work in progress as we refine our release distribution. If you encounter any issues, we're here to help. Please open an issue on GitHub [here](https://github.com/hz-b/rayx/issues/new/choose). Your feedback helps us improve our documentation and installers based on real user experiences.

## Releases

You can find the latest releases of RAYX [here](https://github.com/hz-b/rayx/releases/tag/v0.21.2). Each release includes:

- **rayx-core**: The core library binary.
- **rayx**: Command-Line Interface (CLI) application.
- **rayx-ui**: Graphical User Interface (GUI) application.
- **Data Files**: Includes necessary data, shader, and font files.

## Dependencies

There are some dependencies that might not be handled by every distributed package, installer or archive. We give a general overview of missing dependencies you might encounter here.

### rayx-core

- [HDF5 Library](https://www.hdfgroup.org/download-hdf5/) for faster I/O operations.

### rayx-ui

- GPU driver from AMD, Intel, or NVIDIA.

## Installation Instructions

### Windows

We provide the following options for Windows:

- **Portable Version (.zip)**: No installation required. Just unzip and run.
- **Installer (.exe)**: An NSIS installer that guides you through the setup process.

**Note:** You may need to install the [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170).

### Linux

We offer several packages for Linux distributions:

- **Debian Package (.deb)**: For Debian-based systems like Ubuntu.
- **RPM Package (.rpm)**: For Red Hat-based systems like Fedora.
- **Tarball (.tar.gz)**: For other Linux systems. Intended for experienced users comfortable with manual installations.

#### Ubuntu (Debian-based Systems)

To install RAYX on Ubuntu:

1. **Download** the `.deb` package from the [releases page](https://github.com/hz-b/rayx/releases/tag/v0.21.2).

2. **Install** the package using:

   ```bash
   sudo apt install ./rayx-ui_<version>_amd64.deb
   ```

   Replace `<version>` with the actual version number.

3. **Install Dependencies**:

   ```bash
   sudo apt-get install libhdf5-dev
   ```

#### Fedora (Red Hat-based Systems)

To install RAYX on Fedora:

1. **Download** the `.rpm` package from the [releases page](https://github.com/hz-b/rayx/releases/tag/v0.21.2).

2. **Install** the package using:

   ```bash
   sudo dnf install rayx-ui-<version>.rpm
   ```

   Replace `<version>` with the actual version number.

3. **Install Dependencies**:

   ```bash
   sudo dnf install hdf5
   ```

#### Arch Linux

While we don't provide a native package for Arch Linux, you can use the tarball or build from source.

**Install Dependencies**:

```bash
sudo pacman -S hdf5
```

---

### Installing from Tarball (.tar.gz)

The tarball is intended for experienced users who prefer manual installation or are using a Linux distribution not directly supported by our `.deb` or `.rpm` packages.

**Steps**:

1. **Download** the `.tar.gz` file from the [releases page](https://github.com/hz-b/rayx/releases) or use the provided tarball.

2. **Extract** the tarball:

   ```bash
   tar -xzf RAYX-<version>-Linux.tar.gz
   ```

   This will create a directory named `RAYX-<version>-Linux`.

3. **Move** the extracted files to a directory of your choice (e.g., `/opt/rayx`):

   ```bash
   sudo mv RAYX-<version>-Linux /opt/rayx
   ```

4. **Add to PATH** (optional):

   ```bash
   echo 'export PATH=/opt/rayx/bin:$PATH' >> ~/.bashrc
   source ~/.bashrc
   ```

5. **Run RAYX** from the installation directory or after adding it to your PATH.

**Note:** Installing from a tarball does not handle dependencies automatically. You need to ensure all required dependencies are present on your system. Look at the previous sections for guidance.

---

If you have any questions or run into issues during installation, please don't hesitate to [open an issue on GitHub](https://github.com/hz-b/rayx/issues/new/choose). Your input is invaluable in helping us enhance our tools and documentation.