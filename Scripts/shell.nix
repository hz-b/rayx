/*
 *
 */

{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [

    # dev tools
    python3
    clang # for clang-format

    # build tools
    pkg-config
    gcc
    gdb
    cmake
    ninja

    # rayx-core dependencies
    boost
    hdf5

    # rayx-ui dependencies
    glslang
    vulkan-headers
    vulkan-loader
    vulkan-validation-layers
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
    export CMAKE_PREFIX_PATH=${pkgs.boost}:${pkgs.hdf5}:${pkgs.cmake}:${pkgs.pkg-config};
    export VK_LAYER_PATH="${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
  '';
}
