# this flake is for building, running and developing rayx
#
# usage (from repo root):
# `nix build`     # build rayx (NOTE: write permission to /nix/store is required)
# `nix run`       # run rayx
# `nix develop`   # development environment
# `nix shell`     # production environment
#
# you can also build docker images
# `nix build .#rayx-docker`               # build rayx docker image
# `nix build .#rayx-cuda-docker`          # build rayx-cuda docker image
# `docker load < result`                  # load the built image into docker
# `docker run -it rayx-docker [ARG...]`   # run rayx docker image with args going to rayx
#
# usage (without repo)
# `nix build github:hz-b/rayx`   # build rayx (NOTE: write permission to /nix/store is required)
# `nix run github:hz-b/rayx`     # run rayx
# `nix shell github:hz-b/rayx`   # production environment
#
# NOTE: if you want cuda support, append `path/to/rayx#rayx-cuda` to the commands above

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

          src = if builtins.pathExists ./.
            then
              # if we are building locally, use the local repo
              ./.
            else pkgs.fetchgit {
              # if we are building remotely, we use the latest release
              # this way `nix run github:hz-b/rayx` works as expected, providing the latest release
              # TODO: this needs to be updated in every new release
              url = "https://github.com/hz-b/rayx";
              rev = "9da96500cbcb0f887b00dcbec610b81a9c645833"; # git commit hash for desired version
              sha256 = "sha256-lZ52ZkscNqaE2PxvT7rV+B76jCOE/FKshwI3BgP8hn0="; # fill after build failure
              fetchSubmodules = true;
            };

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
            -DCMAKE_SKIP_BUILD_RPATH=ON \
            -DRAYX_REQUIRE_CUDA=ON \
            -DRAYX_ENABLE_OPENMP=ON \
            -DRAYX_REQUIRE_OPENMP=ON \
            -DRAYX_ENABLE_H5=ON \
            -DRAYX_REQUIRE_H5=ON \
            -DRAYX_WERROR=OFF \
            -DRAYX_BUILD_TESTS=OFF \
            -DRAYX_BUILD_RAYX_CLI=ON \
            -DRAYX_BUILD_RAYX_UI=OFF
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
                ${commonCmakeConfigureFlags}
            '';

            buildPhase = ''
              ninja -v rayx
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
        {
          # running rayx works out of the box, but for rayx-cuda we need to specialize the command,
          # because nix run tries to run the file rayx-cuda, but we want to run the file rayx instead
          rayx-cuda = {
            type = "app";
            program = "${self.packages.${system}.rayx-cuda}/bin/rayx";
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

          mkDevShell = { rayxPackage }:
            pkgs.mkShell rec {
              buildInputs = with pkgs; [
                cmake
                gdb
                gcc
                ninja
                git
                llvmPackages.clang-tools # clangd for format.sh
                docker
                valgrind
                python313
                python313Packages.pip
                python313Packages.numpy
                python313Packages.h5py
                python313Packages.matplotlib
              ] ++ rayxPackage.buildInputs;
              shellHook = ''
                echo "Development shell for rayx"
                echo "Provided packages:"
                echo "${toString buildInputs}"
              '';
            };
        in
        {
          rayx = mkDevShell { rayxPackage = self.packages.${system}.rayx; };
          rayx-cuda = mkDevShell { rayxPackage = self.packages.${system}.rayx-cuda; };
          default = self.devShells.${system}.rayx;
        }
      );
    };
}
