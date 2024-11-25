#!/bin/bash
set -e

echo "Checking if buildroot needs initial setup..."

# Clean buildroot if needed
if [ -f buildroot/.config ]; then
    cd buildroot
    rm .config
    make clean
    make distclean
    cd ..
fi

# Build buildroot
if [ ! -f buildroot/output/host/bin/aarch64-linux-g++ ]; then
    echo "Setting up buildroot..."
    # Copy our config
    cp dendrophone-board/configs/cm4_defconfig buildroot/configs/
    cd buildroot
    make cm4_defconfig
    make sdk
    cd ..
fi

echo "Building Dendrophone application..."

# Export buildroot directory for CMake
export BUILDROOT_DIR=$(pwd)/buildroot

# Build application
cd src
mkdir -p build
cd build
cmake ..
make
cd ../..

echo "Copying to buildroot overlay..."

# Copy to buildroot overlay
cp src/build/dendrophone_app dendrophone-board/overlay/usr/bin/

echo "Building system image..."
cd buildroot
make

echo "Build complete!"
