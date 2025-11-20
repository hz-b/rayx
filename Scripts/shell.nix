/*
 *
 */

{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [

    # dev tools
    clang # for clang-format
    python313
    python313Packages.pip
    python313Packages.numpy
    python313Packages.matplotlib
    python313Packages.h5py

    # build tools
    pkg-config
    gcc
    gdb
    cmake
    ninja
    llvmPackages.openmp
    # cudaPackages.cuda_cudart
    # cudaPackages.cuda_nvcc
    # cudaPackages.cuda_cccl

    # rayx-core dependencies
    hdf5

    # rayx-ui dependencies
    glslang
    vulkan-headers
    vulkan-loader
    vulkan-validation-layers
    vulkan-extension-layer
    vulkan-tools
    vulkan-tools-lunarg
    vulkan-utility-libraries

    # SDL dependencies
    xorg.libX11
    xorg.libXext
    xorg.libXrandr
    xorg.libXcursor
    xorg.libXi
    xorg.libXinerama
    xorg.libXScrnSaver
    libxkbcommon
    wayland
    libdrm
    mesa
    alsa-lib
    dbus
    libudev-zero
  ];

  shellHook = ''
    export CMAKE_PREFIX_PATH=${pkgs.hdf5}:${pkgs.cmake}:${pkgs.pkg-config}:${pkgs.vulkan-headers}:${pkgs.vulkan-loader}:${pkgs.vulkan-validation-layers}:${pkgs.vulkan-extension-layer}:${pkgs.vulkan-tools}:${pkgs.vulkan-tools-lunarg}:${pkgs.vulkan-utility-libraries}:${llvmPackages.openmp};
    export VK_LAYER_PATH="${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
  '';
}
