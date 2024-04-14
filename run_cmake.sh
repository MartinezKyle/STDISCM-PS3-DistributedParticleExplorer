#!/bin/bash

VCPKG_DIR="./vcpkg"

if [ ! -d build ]; then
    mkdir build
fi

# Run cmake with the specified command
cmake --no-warn-unused-cli -DCMAKE_TOOLCHAIN_FILE:STRING=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -S$PWD -B$PWD/build -G "Visual Studio 17 2022" -Thost=x64 -Ax64

# Build the project
cmake --build $PWD/build --config Debug --target ClientServer -j 14
