#!/bin/bash

# Exit on error
set -e

# Use Homebrew's Emscripten directly
EMSDK_PATH="/opt/homebrew/Cellar/emscripten/4.0.1"
if [ ! -x "$EMSDK_PATH/bin/emcc" ]; then
    echo "Error: Emscripten not found at $EMSDK_PATH/bin/emcc. Ensure it's installed via Homebrew (brew install emscripten)."
    exit 1
fi

# Setup native build
echo "Configuring native build..."
mkdir -p build/native
cd build/native
cmake ../..
cd ../..

# Setup Emscripten build
echo "Configuring wasm build..."
mkdir -p build/wasm
cd build/wasm
emcmake cmake ../.. -DCMAKE_TOOLCHAIN_FILE="$EMSDK_PATH/libexec/cmake/Modules/Platform/Emscripten.cmake"
cd ../..

# Setup Python build
echo "Configuring Python build..."
mkdir -p build/python
cd build/python
cmake ../.. -DPython3_EXECUTABLE=$(which python3)
cd ../..

# Setup iOS build
echo "Configuring iOS build..."
mkdir -p build/ios
cd build/ios
cmake ../.. -DBUILD_IOS=ON -LA > cmake_debug.txt 2>&1
cd ../..


# Setup root-level Makefile for custom targets
echo "Configuring root-level Makefile..."
cmake .  # Generates Makefile in scalatrix/ with native as default



echo "CMake setup complete. Use 'make native', 'make wasm', 'make python', 'make ios', 'make build_all', or 'make clean_all'."