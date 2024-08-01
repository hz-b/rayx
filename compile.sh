#!/bin/bash

[[ ! -d .git ]] && echo "You need to be in the root directory of the repo" && exit

echo "Note that it's discouraged to switch between using compile.sh and VSCode calling CMake"
echo "Combining both might lead to an illegal state."
echo "In this case you need to remove the build folder once."
echo

mode=Debug
cuda="-DRAYX_ENABLE_CUDA=OFF"

for var in "$@"
do
    if [[ "$var" == "--release" ]]; then
        mode=Release
    elif [[ "$var" == "--cuda" ]]; then
        cuda="-DRAYX_ENABLE_CUDA=ON -DRAYX_REQUIRES_CUDA=ON"
    elif [[ "$var" == "--help" ]]; then
        echo "Usage:"
        echo " ./compile.sh [OPTIONS]..."
        echo "Options:"
        echo "--release 'Build in Release mode. Otherwise build in Debug mode'"
        echo "--cuda 'Build with Cuda for tracing on GPU. Otherwise build without Cuda'"
        exit
    else
        echo "Error: Unknown option '$var'"
        exit 1
    fi
done

build="./build/$mode"
conf="$cuda"

echo Updating git submodules ...
git submodule update --recursive --init

[ ! -d $build ] && mkdir -p $build

if [ ! -f $build/conf ] || [[ ! "$(cat $build/conf)" == "$conf" ]]; then
    echo > $build/conf

    echo Setting up build directory for mode $mode and conf $conf ...
    cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=$mode $cuda -B $build -G Ninja

    echo "$conf" > $build/conf
fi

# compiling
echo Compiling ...
cmake --build $build --config $mode --target all -j $(nproc) --
