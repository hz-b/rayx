#!/usr/bin/env bash

[[ ! -d .git ]] && echo "You need to be in the root directory of the repo" && exit

echo "Note that it's discouraged to switch between using compile.sh and VSCode calling CMake"
echo "Combining both might lead to an illegal state."
echo "In this case you need to remove the build folder once."
echo

mode=Debug
enable_cuda=0
enable_openmp=1

for var in "$@"
do
    if [[ "$var" == "--release" ]]; then
        mode=Release
    elif [[ "$var" == "--cuda" ]]; then
        enable_cuda=1
    elif [[ "$var" == "--no-openmp" ]]; then
        enable_openmp=0
    elif [[ "$var" == "--help" ]]; then
        echo "Usage:"
        echo " ./compile.sh [OPTIONS]..."
        echo "Options:"
        echo "--release 'Build in Release mode. Otherwise build in Debug mode'"
        echo "--cuda 'Build with Cuda for tracing on GPU. Otherwise build without Cuda'"
        echo "--no-openmp 'Force Build without OpenMP for multithreaded tracing on the CPU.'"
        exit
    else
        echo "Error: Unknown option '$var'"
        exit 1
    fi
done

build="./build/$mode"
conf=""

if [ "$enable_cuda" -eq "1" ]; then
    conf="$conf -D RAYX_ENABLE_CUDA=ON -DRAYX_REQUIRE_CUDA=ON"
else
    conf="$conf -D RAYX_ENABLE_CUDA=OFF"
fi

if [ "$enable_openmp" -eq "0" ]; then
    conf="$conf -D RAYX_ENABLE_OPENMP=OFF"
else
    conf="$conf -D RAYX_ENABLE_OPENMP=ON"
fi

echo Updating git submodules ...
git submodule update --recursive --init

[ ! -d $build ] && mkdir -p $build

if [ ! -f $build/conf ] || [[ ! "$(cat $build/conf)" == "$conf" ]]; then
    echo > $build/conf

    if [ "$enable_cuda" != "1" ]; then
        printf '\033[0;33m'
        echo "Building with Cuda backend disabled. Use flag '--cuda' to build with Cuda."
        printf "\033[0m"
    fi

    echo Setting up build directory for mode $mode and conf $conf ...
    cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=$mode $conf -B $build -G Ninja

    echo "$conf" > $build/conf
fi

# compiling
echo Compiling ...
cmake --build $build --config $mode --target all -j $(nproc) --
