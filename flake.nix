# this flake is for building, running and developing rayx
#
# overview of commands:
# `nix build`     # build rayx from source and put result in ./result (symlink to nix store)
# `nix run`       # run rayx (run `nix build` if necessary)
# `nix develop`   # development environment (provides build tools and dependencies)
# `nix shell`     # production environment (provides rayx binary)
#
# usage (local):
# nix (build|run|shell|develop) [path/to/rayx[#target]]
# e.g.:
# `nix run`                   # run rayx from current directory
# `nix run .#rayx-cuda`       # run rayx with cuda support from current directory
# `nix run path/to/rayx`      # run rayx from path/to/rayx
#
# usage (remote)
# `nix (build|run|shell|develop) git+https://github.com/hz-b/rayx[?ref=(branchname|commithash)][#target]`
# e.g.:
# `nix run git+https://github.com/hz-b/rayx`                        # run the latest main development branch
# `nix run git+https://github.com/hz-b/rayx?ref=v1.2.0`             # run specific tagged release
# `nix run git+https://github.com/hz-b/rayx?ref=v1.2.0#rayx-cuda`   # run specific tagged release with cuda support
#
# you can also build docker images for rayx:
# `nix build .#rayx-docker`               # build rayx docker image
# `nix build .#rayx-cuda-docker`          # build rayx-cuda docker image
# `docker load < result`                  # load the built image into docker
# `docker run -it rayx-docker [ARG...]`   # run rayx docker image with args going to rayx

{
  description = "RAYX TODO"; # TODO: write description

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";
    self.submodules = true;
  };

  outputs = { self, nixpkgs }:
    let
      version = "1.1.0";
      supportedSystems = [ "x86_64-linux" ]; # TODO: add support for "x86_64-darwin" "aarch64-linux" "aarch64-darwin"
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; });
      unfreeNixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; config.allowUnfree = true; });
    in {
      # build rayx
      # usage:
      # nix build
      # nix build .#rayx-cuda
      packages = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
          unfreePkgs = unfreeNixpkgsFor.${system};

          src = self; # ./.

          commonNativeBuildInputs = with pkgs; [
            git
            cmake
            gcc
            ninja
          ];

          commonBuildInputs = with pkgs; [
            hdf5
            llvmPackages.openmp
          ];

          commonCmakeConfigureFlags = ''
            -DCMAKE_BUILD_TYPE=Release \
            -DRAYX_REQUIRE_CUDA=ON \
            -DRAYX_ENABLE_OPENMP=ON \
            -DRAYX_REQUIRE_OPENMP=ON \
            -DRAYX_ENABLE_H5=ON \
            -DRAYX_REQUIRE_H5=ON \
            -DRAYX_WERROR=OFF
          '';

          commonDockerImageContents = [
            pkgs.bash
            pkgs.coreutils # cat mv, rm, cp, ls, etc.
            pkgs.gnugrep # grep
          ];

          mkDockerImage = { name, tag, rayxPackage }:
            pkgs.dockerTools.buildLayeredImage {
              inherit name tag;
              contents = [ rayxPackage ] ++ commonDockerImageContents;
              config = {
                Entrypoint = [ "${rayxPackage}/bin/rayx" ];
              };
            };
        in
        {
          rayx = pkgs.stdenv.mkDerivation {
            pname = "rayx";
            inherit version;
            inherit src;

            nativeBuildInputs = commonNativeBuildInputs;
            buildInputs = commonBuildInputs;

            configurePhase = ''
              cmake -S . -B build -G "Ninja" \
                -DRAYX_ENABLE_CUDA=OFF \
                -DRAYX_BUILD_RAYX_CLI=ON \
                -DRAYX_BUILD_RAYX_TESTS=OFF \
                -DRAYX_BUILD_RAYX_UI=OFF \
                -DCMAKE_SKIP_BUILD_RPATH=ON \
                ${commonCmakeConfigureFlags}
            '';

            buildPhase = ''
              cmake --build build --target rayx --verbose
            '';

            installPhase = ''
              runHook preInstall

              mkdir -p $out/bin $out/lib

              cp -r build/bin/release/Data $out/bin/
              cp build/bin/release/rayx $out/bin/
              cp -r build/lib/release/* $out/lib/

              runHook postInstall
            '';

            meta.description = "TODO"; # TODO: write description
          };

          rayx-cuda = pkgs.stdenv.mkDerivation {
            pname = "rayx-cuda";
            inherit version;
            inherit src;

            nativeBuildInputs = with unfreePkgs; [
            ] ++ commonNativeBuildInputs;

            buildInputs = with unfreePkgs; [
              cudaPackages.cuda_cudart
              cudaPackages.cuda_nvcc
              cudaPackages.cuda_cccl
            ] ++ commonBuildInputs;

            configurePhase = ''
              cmake -S . -B build -G "Ninja" \
                -DRAYX_ENABLE_CUDA=OFF \
                -DRAYX_BUILD_RAYX_CLI=ON \
                -DRAYX_BUILD_RAYX_TESTS=OFF \
                -DRAYX_BUILD_RAYX_UI=OFF \
                -DCMAKE_SKIP_BUILD_RPATH=ON \
                ${commonCmakeConfigureFlags}
            '';

            buildPhase = ''
              cmake --build build --target rayx --verbose
            '';

            installPhase = ''
              runHook preInstall

              mkdir -p $out/bin $out/lib

              cp -r build/bin/release/Data $out/bin/
              cp build/bin/release/rayx $out/bin/
              cp -r build/lib/release/* $out/lib/

              runHook postInstall
            '';

            meta.description = "TODO"; # TODO: write description
          };

          rayx-ui = pkgs.stdenv.mkDerivation {
            pname = "rayx-ui";
            inherit version;
            inherit src;

            nativeBuildInputs = with unfreePkgs; [
            ] ++ commonNativeBuildInputs;

            # TODO: check which of these are actually needed, probably move some to nativeBuildInputs
            buildInputs = with unfreePkgs; [
              dbus
              mesa
              libdrm
              vulkan-headers
              vulkan-loader
              vulkan-validation-layers
              shaderc
              shaderc.bin
              shaderc.dev
              shaderc.lib
              glslang
              xorg.libX11
              xorg.libXext
              xorg.libXcursor
              xorg.libXinerama
              xorg.libXrandr
              xorg.libXi
              xorg.libxcb
              wayland
              wayland-protocols
            ] ++ commonBuildInputs;

            configurePhase = ''
              cmake -S . -B build -G "Ninja" \
                -DRAYX_ENABLE_CUDA=OFF \
                -DRAYX_BUILD_RAYX_CLI=OFF \
                -DRAYX_BUILD_RAYX_TESTS=OFF \
                -DRAYX_BUILD_RAYX_UI=ON \
                -DCMAKE_SKIP_BUILD_RPATH=ON \
                ${commonCmakeConfigureFlags}
            '';

            buildPhase = ''
              cmake --build build --target rayx-ui --verbose
            '';

            installPhase = ''
              runHook preInstall

              mkdir -p $out/bin $out/lib

              cp -r build/bin/release/Data $out/bin/
              cp build/bin/release/rayx-ui $out/bin/
              cp -r build/lib/release/* $out/lib/

              runHook postInstall
            '';

            meta.description = "TODO"; # TODO: write description
          };

          # TODO: do this as a check
          rayx-tests = pkgs.stdenv.mkDerivation {
            pname = "rayx";
            inherit version;
            inherit src;

            nativeBuildInputs = commonNativeBuildInputs;
            buildInputs = commonBuildInputs;

            configurePhase = ''
              cmake -S . -B build -G "Ninja" \
                -DRAYX_ENABLE_CUDA=OFF \
                -DRAYX_BUILD_RAYX_CLI=OFF \
                -DRAYX_BUILD_RAYX_TESTS=ON \
                -DRAYX_BUILD_RAYX_UI=OFF \
                ${commonCmakeConfigureFlags}
            '';

            buildPhase = ''
              cmake --build build --target rayx-core-tst --verbose
            '';

            installPhase = ''
              runHook preInstall

              mkdir -p $out

              # run tests during install phase, because they require access to the source files
              build/bin/release/rayx-core-tst -x > $out/rayx-core-tst.log

              runHook postInstall
            '';

            meta.description = "TODO"; # TODO: write description
          };

          rayx-docker = mkDockerImage {
            name = "rayx-docker";
            tag = "latest";
            rayxPackage = self.packages.${system}.rayx;
          };

          rayx-docker-versioned = mkDockerImage {
            name = "rayx-docker";
            tag = version;
            rayxPackage = self.packages.${system}.rayx;
          };

          rayx-cuda-docker = mkDockerImage {
            name = "rayx-cuda-docker";
            tag = "latest";
            rayxPackage = self.packages.${system}.rayx-cuda;
          };

          rayx-cuda-docker-versioned = mkDockerImage {
            name = "rayx-cuda-docker";
            tag = version;
            rayxPackage = self.packages.${system}.rayx-cuda;
          };

          default = self.packages.${system}.rayx;
        }
      );

      # usage:
      # nix run
      # nix run .#rayx-cuda
      apps = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
        in
        {
          # running rayx works out of the box, but for rayx-cuda we need to specialize the command,
          # because nix run tries to run the file rayx-cuda, but we want to run the file rayx instead
          rayx-cuda = {
            type = "app";
            program = "${self.packages.${system}.rayx-cuda}/bin/rayx";
          };

          # same as above, but for rayx-ui
          rayx-ui = {
            type = "app";
            program = "${self.packages.${system}.rayx-ui}/bin/rayx-ui";
          };
        }
      );

      # virtual environments
      # usage:
      # nix develop
      # nix develop .#rayx-cuda
      # TODO: configuring rayx with cuda in dev shell does not work
      # TODO: put this into shell.nix and reference it here, to get more compatibility with nix-direnv
      devShells = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
          unfreePkgs = unfreeNixpkgsFor.${system};

          mkDevShell = { rayxPackage, extraBuildInputs }:
            pkgs.mkShell rec {
              buildInputs = with pkgs; [
                rayxPackage
                cmake
                gdb
                gcc
                ninja
                git
                llvmPackages.clang-tools # clangd for format.sh
                # docker # docker is disabled because it does not work on ubuntu
                valgrind
                python313
                python313Packages.pip
                python313Packages.numpy
                python313Packages.h5py
                python313Packages.matplotlib
              ] ++ rayxPackage.buildInputs ++ extraBuildInputs;
              shellHook = ''
                echo "Development shell for rayx"
                echo "Provided packages:"
                echo "${toString buildInputs}"
              '';
            };
        in
        {
          rayx = mkDevShell {
            rayxPackage = self.packages.${system}.rayx;
            extraBuildInputs = [];
          };
          rayx-cuda = mkDevShell {
            rayxPackage = self.packages.${system}.rayx-cuda;
            extraBuildInputs = with unfreePkgs; [
              cudaPackages.cuda_gdb
            ];
          };
          rayx-ui = mkDevShell {
            rayxPackage = self.packages.${system}.rayx-ui;
            extraBuildInputs = with unfreePkgs; [];
          };
          rayx-tests = mkDevShell {
            rayxPackage = self.packages.${system}.rayx-tests;
            extraBuildInputs = [];
          };
          default = self.devShells.${system}.rayx;
        }
      );
    };
}
