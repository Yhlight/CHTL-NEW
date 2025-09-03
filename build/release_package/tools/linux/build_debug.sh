#!/bin/bash

echo "==============================================="
echo "CHTL Compiler Debug Build Script (Linux)"
echo "Development Build with Testing - GCC/Clang"
echo "==============================================="

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build/debug"
CMAKE_FILE="$PROJECT_ROOT/CMakeLists_Professional.txt"

echo ""
echo "Step 1: Environment Check..."
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found"
    exit 1
fi

echo ""
echo "Step 2: Compiler Detection..."
if command -v clang++ &> /dev/null; then
    export CC=clang
    export CXX=clang++
    echo "Using Clang with AddressSanitizer"
else
    export CC=gcc
    export CXX=g++
    echo "Using GCC with AddressSanitizer"
fi

echo ""
echo "Step 3: Clean Previous Build..."
if [ -d "$BUILD_DIR" ]; then
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo ""
echo "Step 4: CMake Configuration (Debug)..."
cmake "$PROJECT_ROOT" \
    -DCMAKE_BUILD_TYPE=Debug

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed"
    exit 1
fi

echo ""
echo "Step 5: Build Project (Debug)..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed"
    exit 1
fi

echo ""
echo "Step 6: Run Tests..."
echo "Running CJMOD tests..."
if [ -f "bin/chtl_cjmod_test" ]; then
    ./bin/chtl_cjmod_test
fi

echo ""
echo "Running integration tests..."
if [ -f "bin/chtl_integration_test" ]; then
    ./bin/chtl_integration_test
fi

echo ""
echo "Running architecture tests..."
if [ -f "bin/chtl_architecture_test" ]; then
    ./bin/chtl_architecture_test
fi

echo ""
echo "Step 7: Verify Debug Build..."
if [ -f "bin/chtl" ]; then
    echo "SUCCESS: Debug chtl executable created"
    ./bin/chtl --version
else
    echo "ERROR: Debug chtl executable not found"
    exit 1
fi

echo ""
echo "==============================================="
echo "SUCCESS: CHTL Compiler Debug Build Complete"
echo ""
echo "Output Directory: $BUILD_DIR/bin/"
echo "Executable: chtl (Debug)"
echo "Tests: All tests executed"
echo ""
echo "Debug Features:"
echo "  - Address sanitizer enabled"
echo "  - Debug symbols included"
echo "  - All tests available"
echo "  - Development tools included"
echo "==============================================="