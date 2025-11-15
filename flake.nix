{
  description = "RAYX simulation tool with CUDA, OpenMP, valgrind, and Ninja build";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; config.allowUnfree = true; };

      src = pkgs.fetchgit {
        url = "https://github.com/hz-b/rayx";
        rev = "5694a0117e7d8e58a1b19f35202891270dc5ee91"; # git commit hash for desired version
        sha256 = "sha256-clfHvQK9JI/TWDmpXiTtBie4q++F9+F37uzTvLqvMlc="; # fill after build failure
        fetchSubmodules = true;
      };
    in {
      packages.${system} = rec {
        rayx = pkgs.stdenv.mkDerivation {
          pname = "rayx";
          version = "1.1.0";
          inherit src;

          nativeBuildInputs = with pkgs; [
            cmake
            gcc
            llvmPackages.openmp
            ninja
            patchelf
          ];

          buildInputs = with pkgs; [
            hdf5
          ];

          configurePhase = ''
            mkdir build
            cd build
            cmake -G Ninja .. \
              -DCMAKE_BUILD_TYPE=Release \
              -DCMAKE_SKIP_BUILD_RPATH=ON \
              -DRAYX_ENABLE_CUDA=OFF \
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

          buildPhase = ''
            ninja -v rayx
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp -r bin/release/Data $out/bin
            cp -r bin/release/rayx $out/bin

            mkdir -p $out/lib
            cp -r lib/release/* $out/lib
          '';

          meta.description = "TODO"; # TODO: write description
        };

        rayx-cuda = pkgs.stdenv.mkDerivation {
          pname = "rayx-cuda";
          version = "1.1.0";
          inherit src;

          nativeBuildInputs = with pkgs; [
            cmake
            gcc14 # pin gcc version for cuda version 12 compatibility
            llvmPackages.openmp
            ninja
            patchelf
          ];

          buildInputs = with pkgs; [
            hdf5
            cudaPackages.cuda_cudart
            cudaPackages.cuda_nvcc
          ];

          configurePhase = ''
            mkdir build
            cd build
            echo "g++ --version"
            g++ --version
            echo "nvcc --version"
            nvcc --version
            cmake -G Ninja .. \
              -DCMAKE_BUILD_TYPE=Release \
              -DCMAKE_SKIP_BUILD_RPATH=ON \
              -DRAYX_ENABLE_CUDA=ON \
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

          buildPhase = ''
            ninja -v rayx
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp -r bin/release/Data $out/bin
            cp -r bin/release/rayx $out/bin

            mkdir -p $out/lib
            cp -r lib/release/* $out/lib
          '';

          meta.description = "TODO"; # TODO: write description
        };

        rayx-run-tests = pkgs.stdenv.mkDerivation {
          pname = "rayx-run-tests";
          version = "1.1.0";
          inherit src;

          nativeBuildInputs = with pkgs; [
            cmake
            gcc
            llvmPackages.openmp
            ninja
            patchelf
            valgrind
          ];

          buildInputs = with pkgs; [
            hdf5
          ];

          configurePhase = ''
            mkdir build
            cd build
            cmake -G Ninja .. \
              -DCMAKE_BUILD_TYPE=Release \
              -DRAYX_ENABLE_CUDA=OFF \
              -DRAYX_REQUIRE_CUDA=ON \
              -DRAYX_ENABLE_OPENMP=ON \
              -DRAYX_REQUIRE_OPENMP=ON \
              -DRAYX_ENABLE_H5=ON \
              -DRAYX_REQUIRE_H5=ON \
              -DRAYX_WERROR=ON \
              -DRAYX_BUILD_TESTS=ON \
              -DRAYX_BUILD_RAYX_CLI=ON \
              -DRAYX_BUILD_RAYX_UI=OFF
          '';

          buildPhase = ''
            ninja -v rayx rayx-core-tst
          '';

          # TODO: provide a better way to access test input file
          # TODO: testing should not be in the build phase, but rayx-core-tst depends on source directory being preset
          installPhase = ''
            mkdir -p $out

            RML=../Intern/rayx-core/tests/input/METRIX_U41_G1_H1_318eV_PS_MLearn_v114.rml
            # run rayx with valgrind to catch memory issues
            valgrind --error-exitcode=42 ./bin/release/rayx -x -V -i $RML |& tee $out/log_rayx.txt
            # run tests
            ./bin/release/rayx-core-tst -x |& tee $out/log_rayx-core-tst.txt
          '';

          meta.description = "TODO"; # TODO: write description
        };

        default = rayx;
      };

      devShells.${system} = rec {
        rayx = pkgs.mkShell {
          buildInputs = with pkgs; [
            self.packages.${system}.rayx
            hdf5
          ];

          shellHook = ''
            echo "To get info about rayx, run: rayx --help"
          '';
        };

        rayx-cuda = pkgs.mkShell {
          buildInputs = with pkgs; [
            self.packages.${system}.rayx
            hdf5
            cudaPackages.cuda_cudart
          ];

          shellHook = ''
            echo "To get info about rayx, run: rayx --help"
          '';
        };

        rayx-run-tests = pkgs.mkShell {
          buildInputs = with pkgs; [
            self.packages.${system}.rayx-run-tests
          ];

          shellHook = ''
            echo "Inspect the test results in log_rayx.txt and log_rayx-core-tst.txt"
          '';
        };

        default = rayx;
      };
    };
}
