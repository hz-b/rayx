#!/bin/sh

[[ ! -d .git ]] && echo "You need to be in the root directory of the repo" && exit

mode=Debug

for var in "$@"
do
    if [[ "$var" == "--release" ]]; then
        mode=Release
    fi
    if [[ "$var" == "--help" ]]; then
        echo "Usage:"
        echo " ./compile"
        echo " ./compile --release"
        exit
    fi
done

echo Updating git submodules ...
git submodule update --recursive

# setup build folder
if [[ ! -d "./build" ]]; then
    echo Setting up build directory ... 
    cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=$mode -B ./build -G Ninja
fi

# compiling
echo Compiling ...
cmake --build ./build --config $mode --target all -j 10 --
