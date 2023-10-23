#!/bin/bash

[[ ! -d .git ]] && echo "You need to be in the root directory of the repo" && exit

echo "Note that it's discouraged to switch between using compile.sh and VSCode calling CMake"
echo "Combining both might lead to an illegal state."
echo "In this case you need to remove the build folder once."
echo

mode=Debug

for var in "$@"
do
    if [[ "$var" == "--release" ]]; then
        mode=Release
    fi
    if [[ "$var" == "--help" ]]; then
        echo "Usage:"
        echo " ./compile.sh"
        echo " ./compile.sh --release"
        exit
    fi
done

echo Updating git submodules ...
git submodule update --recursive --init

[ ! -d build ] && mkdir build

if [ ! -f ./build/mode ] || [[ ! "$(cat ./build/mode)" == "$mode" ]]; then
    echo > ./build/mode

    echo Setting up build directory for mode $mode ...
    CXX=clang++ cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=$mode -B ./build -G Ninja

    echo $mode > ./build/mode
fi

# compiling
echo Compiling ...
CXX=clang++ cmake --build ./build --config $mode --target all -j 10 --
