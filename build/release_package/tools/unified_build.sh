#!/bin/bash

echo "==============================================="
echo "CHTL Unified Build Script"
echo "Build Compiler + Package Modules + Build VSCode Plugin"
echo "==============================================="

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_TYPE="${1:-release}"

echo ""
echo "Project Root: $PROJECT_ROOT"
echo "Build Type: $BUILD_TYPE"

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    PLATFORM="windows"
else
    echo "ERROR: Unsupported platform: $OSTYPE"
    exit 1
fi

echo "Platform: $PLATFORM"

echo ""
echo "Step 1: Build CHTL Compiler..."
if [ "$PLATFORM" = "windows" ]; then
    cmd //c "$PROJECT_ROOT/scripts/windows/build_${BUILD_TYPE}.bat"
else
    bash "$PROJECT_ROOT/scripts/$PLATFORM/build_${BUILD_TYPE}.sh"
fi

if [ $? -ne 0 ]; then
    echo "ERROR: Compiler build failed"
    exit 1
fi

echo ""
echo "Step 2: Package CMOD Modules..."
bash "$PROJECT_ROOT/scripts/package_cmod.sh"

if [ $? -ne 0 ]; then
    echo "ERROR: CMOD packaging failed"
    exit 1
fi

echo ""
echo "Step 3: Package CJMOD Modules..."
bash "$PROJECT_ROOT/scripts/package_cjmod.sh"

if [ $? -ne 0 ]; then
    echo "ERROR: CJMOD packaging failed"
    exit 1
fi

echo ""
echo "Step 4: Build VSCode Plugin..."
bash "$PROJECT_ROOT/scripts/build_vscode_plugin.sh"

if [ $? -ne 0 ]; then
    echo "ERROR: VSCode plugin build failed"
    exit 1
fi

echo ""
echo "Step 5: Create Unified Package..."
bash "$PROJECT_ROOT/scripts/unified_package.sh"

if [ $? -ne 0 ]; then
    echo "ERROR: Unified packaging failed"
    exit 1
fi

echo ""
echo "==============================================="
echo "SUCCESS: CHTL Unified Build Complete"
echo ""
echo "Build Type: $BUILD_TYPE"
echo "Platform: $PLATFORM"
echo ""
echo "Generated:"
echo "  - CHTL Compiler ($BUILD_TYPE)"
echo "  - Packaged CMOD modules"
echo "  - Packaged CJMOD modules"
echo "  - VSCode Plugin"
echo "  - Unified release package"
echo "==============================================="