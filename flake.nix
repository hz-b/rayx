{
  description = "RAYX simulation tool with CUDA, OpenMP, valgrind, and Ninja build";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";
  };

  outputs = { self, nixpkgs }:
    let
      version = "1.1.0";
      supportedSystems = [ "x86_64-linux" ]; # TODO: add support for "x86_64-darwin" "aarch64-linux" "aarch64-darwin"
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; config.allowUnfree = true; });
    in {
      packages = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
          pkgsUnfree = nixpkgsFor.${system};

          src = pkgs.fetchgit {
            url = "https://github.com/hz-b/rayx";
            rev = "5694a0117e7d8e58a1b19f35202891270dc5ee91"; # git commit hash for desired version
            sha256 = "sha256-clfHvQK9JI/TWDmpXiTtBie4q++F9+F37uzTvLqvMlc="; # fill after build failure
            fetchSubmodules = true;
          };

          commonNativeBuildInputs = with pkgs; [
            cmake
            gcc
            llvmPackages.openmp
            ninja
          ];

          commonBuildInputs = with pkgs; [
            hdf5
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
        in
        {
          rayx = pkgs.stdenv.mkDerivation {
            pname = "rayx";
            inherit version;
            inherit src;

            nativeBuildInputs = with pkgs; [
            ] ++ commonNativeBuildInputs;

            buildInputs = with pkgs; [
            ] ++ commonBuildInputs;

            configurePhase = ''
              mkdir build
              cd build
              cmake -G Ninja .. \
                -DRAYX_ENABLE_CUDA=OFF \
                ${commonCmakeConfigureFlags}
            '';

            buildPhase = ''
              ninja -v rayx
            '';

            installPhase = ''
              mkdir -p $out/bin
              cp -r bin/release/Data $out/bin
              cp -r bin/release/rayx $out/bin/

              mkdir -p $out/lib
              cp -r lib/release/* $out/lib
            '';

            meta.description = "TODO"; # TODO: write description
          };

          rayx-cuda = pkgs.stdenv.mkDerivation {
            pname = "rayx-cuda";
            inherit version;
            inherit src;

            nativeBuildInputs = with pkgs; [
              cudaPackages.cuda_nvcc
              cudaPackages.cuda_cccl
            ] ++ commonNativeBuildInputs;

            buildInputs = with pkgs; [
              cudaPackages.cuda_cudart
            ] ++ commonBuildInputs;

            configurePhase = ''
              mkdir build
              cd build
              cmake -G Ninja .. \
                -DRAYX_ENABLE_CUDA=ON \
                ${commonCmakeConfigureFlags}
            '';

            buildPhase = ''
              ninja -v rayx
            '';

            installPhase = ''
              mkdir -p $out/bin
              cp -r bin/release/Data $out/bin
              cp -r bin/release/rayx $out/bin
              mv $out/bin/rayx $out/bin/rayx-cuda

              mkdir -p $out/lib
              cp -r lib/release/* $out/lib
            '';

            meta.description = "TODO"; # TODO: write description
          };

          default = self.packages.${system}.rayx;
        }
      );
    };
}
