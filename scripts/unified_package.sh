#!/bin/bash

echo "==============================================="
echo "CHTL Unified Packaging Script"
echo "Create Complete Release Package"
echo "==============================================="

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
RELEASE_DIR="$BUILD_DIR/release_package"
VERSION="1.0.0"

echo ""
echo "Step 1: Environment Check..."
echo "Project Root: $PROJECT_ROOT"
echo "Build Directory: $BUILD_DIR"
echo "Release Directory: $RELEASE_DIR"

echo ""
echo "Step 2: Create Release Directory Structure..."
rm -rf "$RELEASE_DIR"
mkdir -p "$RELEASE_DIR"/{bin,plugins,modules,documentation,examples,tools}

echo ""
echo "Step 3: Package Compiler Binaries..."
# Copy compiler binaries
if [ -f "$BUILD_DIR/release/bin/chtl" ]; then
    cp "$BUILD_DIR/release/bin/chtl" "$RELEASE_DIR/bin/chtl_linux"
    echo "  ✓ Linux compiler copied"
fi

if [ -f "$BUILD_DIR/debug/bin/chtl" ]; then
    cp "$BUILD_DIR/debug/bin/chtl" "$RELEASE_DIR/bin/chtl_linux_debug"
    echo "  ✓ Linux debug compiler copied"
fi

# Look for Windows binaries
if [ -f "$PROJECT_ROOT/build_windows/bin/chtl_windows.exe" ]; then
    cp "$PROJECT_ROOT/build_windows/bin/chtl_windows.exe" "$RELEASE_DIR/bin/chtl.exe"
    echo "  ✓ Windows compiler copied"
fi

echo ""
echo "Step 4: Package VSCode Plugin..."
if [ -f "$BUILD_DIR/vscode-plugin"/*.vsix ]; then
    cp "$BUILD_DIR/vscode-plugin"/*.vsix "$RELEASE_DIR/plugins/"
    echo "  ✓ VSCode plugin copied"
elif [ -d "$PROJECT_ROOT/vscode-chtl-extension/out" ]; then
    # Package plugin manually
    cd "$PROJECT_ROOT/vscode-chtl-extension"
    PLUGIN_NAME="chtl-language-support-$VERSION.vsix"
    
    # Create basic VSIX structure
    mkdir -p temp_vsix/extension
    cp -r out/ temp_vsix/extension/
    cp -r syntaxes/ temp_vsix/extension/
    cp package.json temp_vsix/extension/
    cp README.md temp_vsix/extension/
    cp language-configuration*.json temp_vsix/extension/
    
    cd temp_vsix
    zip -r "../$PLUGIN_NAME" .
    cd ..
    rm -rf temp_vsix
    
    mv "$PLUGIN_NAME" "$RELEASE_DIR/plugins/"
    echo "  ✓ VSCode plugin packaged manually"
    
    cd "$PROJECT_ROOT"
fi

echo ""
echo "Step 5: Package Modules..."
if [ -d "$BUILD_DIR/packages/cmod" ]; then
    cp "$BUILD_DIR/packages/cmod"/*.cmod "$RELEASE_DIR/modules/" 2>/dev/null || true
    CMOD_COUNT=$(ls "$RELEASE_DIR/modules"/*.cmod 2>/dev/null | wc -l)
    echo "  ✓ CMOD packages copied: $CMOD_COUNT"
fi

if [ -d "$BUILD_DIR/packages/cjmod" ]; then
    cp "$BUILD_DIR/packages/cjmod"/*.cjmod "$RELEASE_DIR/modules/" 2>/dev/null || true
    cp "$BUILD_DIR/packages/cjmod"/*.cmod "$RELEASE_DIR/modules/" 2>/dev/null || true
    CJMOD_COUNT=$(ls "$RELEASE_DIR/modules"/*.cjmod "$RELEASE_DIR/modules"/*.cmod 2>/dev/null | wc -l)
    echo "  ✓ CJMOD packages copied: $CJMOD_COUNT"
fi

echo ""
echo "Step 6: Package Documentation..."
cp "$PROJECT_ROOT/CHTL语法文档.md" "$RELEASE_DIR/documentation/" 2>/dev/null || true
cp "$PROJECT_ROOT/CJMOD.md" "$RELEASE_DIR/documentation/" 2>/dev/null || true
cp "$PROJECT_ROOT/目标规划.ini" "$RELEASE_DIR/documentation/" 2>/dev/null || true
cp "$PROJECT_ROOT/完善选择器自动化与引用规则.ini" "$RELEASE_DIR/documentation/" 2>/dev/null || true

# Copy generated documentation (will be created later)
cp "$PROJECT_ROOT/docs"/*.md "$RELEASE_DIR/documentation/" 2>/dev/null || true

DOC_COUNT=$(ls "$RELEASE_DIR/documentation" 2>/dev/null | wc -l)
echo "  ✓ Documentation files copied: $DOC_COUNT"

echo ""
echo "Step 7: Package Examples..."
cp "$PROJECT_ROOT"/*.chtl "$RELEASE_DIR/examples/" 2>/dev/null || true
cp "$PROJECT_ROOT"/*.cjjs "$RELEASE_DIR/examples/" 2>/dev/null || true

# Copy example files from examples directory if exists
if [ -d "$PROJECT_ROOT/examples" ]; then
    cp "$PROJECT_ROOT/examples"/* "$RELEASE_DIR/examples/" 2>/dev/null || true
fi

EXAMPLE_COUNT=$(ls "$RELEASE_DIR/examples" 2>/dev/null | wc -l)
echo "  ✓ Example files copied: $EXAMPLE_COUNT"

echo ""
echo "Step 8: Package Tools..."
cp -r "$PROJECT_ROOT/scripts"/* "$RELEASE_DIR/tools/"
echo "  ✓ Build tools copied"

echo ""
echo "Step 9: Create Installation Scripts..."

# Windows installer
cat > "$RELEASE_DIR/install.bat" << 'EOF'
@echo off
echo Installing CHTL Compiler v1.0.0...
echo.
echo Please add the bin directory to your PATH:
echo %~dp0bin
echo.
echo To install VSCode plugin:
echo code --install-extension plugins\chtl-language-support-1.0.0.vsix
echo.
pause
EOF

# Linux/macOS installer
cat > "$RELEASE_DIR/install.sh" << 'EOF'
#!/bin/bash
echo "Installing CHTL Compiler v1.0.0..."
echo ""
echo "Adding to PATH (add to ~/.bashrc):"
echo "export PATH=\"$(pwd)/bin:\$PATH\""
echo ""
echo "To install VSCode plugin:"
echo "code --install-extension plugins/chtl-language-support-1.0.0.vsix"
echo ""
EOF

chmod +x "$RELEASE_DIR/install.sh"

echo ""
echo "Step 10: Create README..."
cat > "$RELEASE_DIR/README.md" << EOF
# CHTL Compiler Release Package v$VERSION

## Overview
Complete CHTL (Chtholly HyperText Language) compiler package with CJMOD powerful extension system.

## Contents
- \`bin/\` - Compiler binaries (Linux/Windows)
- \`plugins/\` - VSCode plugin (.vsix)
- \`modules/\` - CMOD/CJMOD packages
- \`documentation/\` - Complete documentation
- \`examples/\` - Usage examples
- \`tools/\` - Build scripts and tools

## Installation
### Windows
Run \`install.bat\`

### Linux/macOS
Run \`./install.sh\`

## Features
- ✅ CJMOD powerful extension system
- ✅ 100 syntax features implementation
- ✅ Dual-language separation architecture
- ✅ VSCode professional IDE support
- ✅ UTF-8 Chinese character support
- ✅ Cross-platform compatibility

## Usage
\`\`\`bash
# Compile CHTL file
chtl input.chtl output.html

# Show help
chtl --help

# Show version
chtl --version
\`\`\`

## Documentation
See \`documentation/\` directory for complete guides and references.
EOF

echo ""
echo "Step 11: Create Final Package..."
cd "$BUILD_DIR"
PACKAGE_NAME="CHTL_Complete_Release_v${VERSION}_$(date +%Y%m%d)"

if command -v zip &> /dev/null; then
    zip -r "${PACKAGE_NAME}.zip" release_package/
    echo "  ✓ ZIP package created: ${PACKAGE_NAME}.zip"
fi

if command -v tar &> /dev/null; then
    tar -czf "${PACKAGE_NAME}.tar.gz" release_package/
    echo "  ✓ TAR.GZ package created: ${PACKAGE_NAME}.tar.gz"
fi

echo ""
echo "Step 12: Package Verification..."
TOTAL_SIZE=$(du -sh release_package/ | cut -f1)
FILE_COUNT=$(find release_package/ -type f | wc -l)

echo "  📊 Total package size: $TOTAL_SIZE"
echo "  📊 Total files: $FILE_COUNT"

echo ""
echo "==============================================="
echo "SUCCESS: CHTL Unified Packaging Complete"
echo ""
echo "Release Directory: $RELEASE_DIR"
echo "Package Size: $TOTAL_SIZE"
echo "Files: $FILE_COUNT"
echo ""
echo "Generated Packages:"
if [ -f "${PACKAGE_NAME}.zip" ]; then
    echo "  - ${PACKAGE_NAME}.zip"
fi
if [ -f "${PACKAGE_NAME}.tar.gz" ]; then
    echo "  - ${PACKAGE_NAME}.tar.gz"
fi
echo ""
echo "Ready for distribution!"
echo "==============================================="