#!/bin/bash

echo "==============================================="
echo "CHTL Compiler Release Build Script (Linux)"
echo "Professional Build System - GCC/Clang"
echo "==============================================="

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build/release"
CMAKE_FILE="$PROJECT_ROOT/CMakeLists_Professional.txt"

echo ""
echo "Step 1: Environment Check..."
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found"
    echo "Please install CMake: sudo apt-get install cmake"
    exit 1
fi

echo "CMake found: $(cmake --version | head -n1)"

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "ERROR: No C++ compiler found"
    echo "Please install: sudo apt-get install build-essential"
    exit 1
fi

echo ""
echo "Step 2: Compiler Detection..."
if command -v clang++ &> /dev/null; then
    export CC=clang
    export CXX=clang++
    echo "Using Clang: $(clang++ --version | head -n1)"
else
    export CC=gcc
    export CXX=g++
    echo "Using GCC: $(g++ --version | head -n1)"
fi

echo ""
echo "Step 3: Clean Previous Build..."
if [ -d "$BUILD_DIR" ]; then
    echo "Removing previous build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo ""
echo "Step 4: CMake Configuration..."
cmake "$PROJECT_ROOT" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed"
    exit 1
fi

echo ""
echo "Step 5: Build Project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed"
    exit 1
fi

echo ""
echo "Step 6: Verify Build Results..."
if [ -f "bin/chtl" ]; then
    echo "SUCCESS: chtl executable created"
    echo "File size: $(ls -lh bin/chtl | awk '{print $5}')"
    
    echo ""
    echo "Testing compiler:"
    ./bin/chtl --version
else
    echo "ERROR: chtl executable not found"
    exit 1
fi

echo ""
echo "Step 7: Package Creation..."
make package

echo ""
echo "==============================================="
echo "SUCCESS: CHTL Compiler Release Build Complete"
echo ""
echo "Output Directory: $BUILD_DIR/bin/"
echo "Executable: chtl"
echo "Package: CHTL-Compiler-1.0.0-Linux.tar.gz"
echo ""
echo "Features:"
echo "  - CJMOD powerful extension system"
echo "  - 100 syntax features implementation"
echo "  - Dual-language separation architecture"
echo "  - UTF-8 Chinese character support"
echo "  - Production quality optimization"
echo "==============================================="