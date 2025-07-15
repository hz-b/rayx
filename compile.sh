#!/usr/bin/env bash

[[ ! -d .git ]] && echo "You need to be in the root directory of the repo" && exit

echo "Note that it's discouraged to switch between using compile.sh and VSCode calling CMake"
echo "Combining both might lead to an illegal state."
echo "In this case you need to remove the build folder once."
echo
echo "Hint: If you're encountering problems compiling with cuda it might be necessary to select a specific compiler."
echo "      e.g. CXX=/usr/bin/g++-14 ./compile.sh --cuda"
echo


# defaults
mode=Debug
enable_cuda=0
enable_openmp=1
list_targets=0
clean=0
selected_target="all"

# parse flags
for var in "$@"; do
    case "$var" in
        --release)      mode=Release                                  ;;
        --cuda)         enable_cuda=1                                 ;;
        --no-openmp)    enable_openmp=0                               ;;
        --clean)        clean=1                                       ;;
        --list-targets) list_targets=1                               ;;
        --target=*)     selected_target="${var#--target=}"         ;;
        --help)
            cat <<EOF
Usage:
  ./compile.sh [OPTIONS]...

Options:
  --release           Build in Release mode (default: Debug).
  --cuda              Build with CUDA support (default: off).
  --no-openmp         Disable OpenMP (default: on).
  --clean             Delete the build directory (and its cache) first.
  --list-targets      List all available build targets and exit.
  --target=NAME       Build only target NAME (default: all).
  --help              Show this help message and exit.
EOF
            exit 0
            ;;
        *)
            echo "Error: Unknown option '$var'"
            exit 1
            ;;
    esac
done

build="./build/"
conf_args=""

# clean build dir if requested
if [ "$clean" -eq 1 ]; then
    echo "Cleaning build directory..."
    rm -rf "$build"
fi

# list targets and exit
if [ "$list_targets" -eq 1 ]; then
    if [ -d "$build" ]; then
        echo "Available targets in $build:"
        cmake --build "$build" --target help
    else
        echo "No build directory found. Run a configure first."
    fi
    exit
fi

# assemble CMake args
if [ "$enable_cuda" -eq 1 ]; then
    conf_args+=" -DRAYX_ENABLE_CUDA=ON -DRAYX_REQUIRE_CUDA=ON"
else
    conf_args+=" -DRAYX_ENABLE_CUDA=OFF"
fi

if [ "$enable_openmp" -eq 0 ]; then
    conf_args+=" -DRAYX_ENABLE_OPENMP=OFF"
else
    conf_args+=" -DRAYX_ENABLE_OPENMP=ON"
fi

# update git submodules
echo "Updating git submodules ..."
git submodule update --recursive --init

# prepare build directory
[ ! -d "$build" ] && mkdir -p "$build"

# re-run CMake only if config changed
if [ ! -f "$build/conf" ] || [[ "$(cat $build/conf)" != "$conf_args" ]]; then
    echo > "$build/conf"

    if [ "$enable_cuda" != "1" ]; then
        printf '\033[0;33m'
        echo "Building with CUDA backend disabled. Use flag '--cuda' to build with CUDA."
        printf "\033[0m"
    fi

    echo "Setting up build directory for mode $mode and CMake args:$conf_args ..."

    cmake \
      --no-warn-unused-cli \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
      -DCMAKE_BUILD_TYPE="$mode" \
      $conf_args \
      -B "$build" -G Ninja

    echo "$conf_args" > "$build/conf"
fi

# build the chosen target
echo "Compiling target '$selected_target' ..."
cmake --build "$build" --config "$mode" --target "$selected_target" -j "$(nproc)"
