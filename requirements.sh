#!/bin/bash

VCPKG_DIR="./vcpkg"

# Clone vcpkg if it doesn't exist
if [ ! -d "$VCPKG_DIR" ]; then
    git clone https://github.com/microsoft/vcpkg.git $VCPKG_DIR
    if [ $? -ne 0 ]; then
        echo "Failed to clone vcpkg repository."
        exit 1
    fi

    $VCPKG_DIR/bootstrap-vcpkg.sh -disableMetrics
    if [ $? -ne 0 ]; then
        echo "Failed to bootstrap vcpkg."
        exit 1
    fi
fi

# Bootstrap vcpkg
cd $VCPKG_DIR

# Install packages
./vcpkg install boost-asio zlib sfml nlohmann-json boost-thread boost-filesystem
if [ $? -ne 0 ]; then
    echo "Failed to install packages with vcpkg."
    exit 1
fi

# Integrate vcpkg with the build system
./vcpkg integrate install
if [ $? -ne 0 ]; then
    echo "Failed to integrate vcpkg."
    exit 1
fi

echo "vcpkg setup complete"
