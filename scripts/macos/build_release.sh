#!/bin/bash

echo "==============================================="
echo "CHTL Compiler Release Build Script (macOS)"
echo "Professional Build System - Clang/Xcode"
echo "==============================================="

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build/release"
CMAKE_FILE="$PROJECT_ROOT/CMakeLists_Professional.txt"

echo ""
echo "Step 1: Environment Check..."
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found"
    echo "Please install CMake: brew install cmake"
    exit 1
fi

echo "CMake found: $(cmake --version | head -n1)"

if ! command -v clang++ &> /dev/null; then
    echo "ERROR: Clang++ not found"
    echo "Please install Xcode Command Line Tools: xcode-select --install"
    exit 1
fi

echo ""
echo "Step 2: Compiler Detection..."
export CC=clang
export CXX=clang++
echo "Using Clang: $(clang++ --version | head -n1)"

echo ""
echo "Step 3: macOS SDK Detection..."
SDK_PATH=$(xcrun --show-sdk-path)
echo "macOS SDK: $SDK_PATH"

echo ""
echo "Step 4: Clean Previous Build..."
if [ -d "$BUILD_DIR" ]; then
    echo "Removing previous build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo ""
echo "Step 5: CMake Configuration..."
cmake "$PROJECT_ROOT" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.14

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed"
    exit 1
fi

echo ""
echo "Step 6: Build Project..."
make -j$(sysctl -n hw.ncpu)

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed"
    exit 1
fi

echo ""
echo "Step 7: Verify Build Results..."
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
echo "Step 8: Package Creation..."
make package

echo ""
echo "==============================================="
echo "SUCCESS: CHTL Compiler Release Build Complete"
echo ""
echo "Output Directory: $BUILD_DIR/bin/"
echo "Executable: chtl"
echo "Package: CHTL-Compiler-1.0.0-Darwin.tar.gz"
echo ""
echo "Features:"
echo "  - CJMOD powerful extension system"
echo "  - 100 syntax features implementation"
echo "  - Dual-language separation architecture"
echo "  - UTF-8 Chinese character support"
echo "  - macOS optimized build"
echo "==============================================="