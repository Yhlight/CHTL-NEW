#!/bin/bash

echo "==============================================="
echo "CHTL Build Cleanup Script"
echo "Clean all build artifacts and temporary files"
echo "==============================================="

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo ""
echo "Step 1: Clean Build Directories..."
echo "Removing build directories..."

# Remove all build directories
rm -rf "$PROJECT_ROOT/build"
rm -rf "$PROJECT_ROOT/build_windows"
rm -rf "$PROJECT_ROOT/.cmake"

echo "  ✓ Build directories cleaned"

echo ""
echo "Step 2: Clean CMake Cache Files..."
find "$PROJECT_ROOT" -name "CMakeCache.txt" -delete
find "$PROJECT_ROOT" -name "cmake_install.cmake" -delete
find "$PROJECT_ROOT" -name "Makefile" -delete
find "$PROJECT_ROOT" -type d -name "CMakeFiles" -exec rm -rf {} + 2>/dev/null || true

echo "  ✓ CMake cache files cleaned"

echo ""
echo "Step 3: Clean Object Files..."
find "$PROJECT_ROOT" -name "*.o" -delete
find "$PROJECT_ROOT" -name "*.obj" -delete
find "$PROJECT_ROOT" -name "*.a" -delete
find "$PROJECT_ROOT" -name "*.lib" -delete
find "$PROJECT_ROOT" -name "*.so" -delete
find "$PROJECT_ROOT" -name "*.dll" -delete
find "$PROJECT_ROOT" -name "*.dylib" -delete

echo "  ✓ Object files cleaned"

echo ""
echo "Step 4: Clean Temporary Files..."
find "$PROJECT_ROOT" -name "*~" -delete
find "$PROJECT_ROOT" -name "*.tmp" -delete
find "$PROJECT_ROOT" -name "*.temp" -delete
find "$PROJECT_ROOT" -name ".DS_Store" -delete
find "$PROJECT_ROOT" -name "Thumbs.db" -delete
find "$PROJECT_ROOT" -name "*.bak" -delete

echo "  ✓ Temporary files cleaned"

echo ""
echo "Step 5: Clean Test Output Files..."
find "$PROJECT_ROOT" -name "test_*.html" -delete
find "$PROJECT_ROOT" -name "*_output.html" -delete
find "$PROJECT_ROOT" -name "*.log" -delete

echo "  ✓ Test output files cleaned"

echo ""
echo "Step 6: Clean VSCode Plugin Build..."
if [ -d "$PROJECT_ROOT/vscode-chtl-extension" ]; then
    cd "$PROJECT_ROOT/vscode-chtl-extension"
    
    # Clean npm artifacts
    rm -rf node_modules/.cache
    rm -f *.vsix
    rm -f PLUGIN_READY.txt
    
    # Keep node_modules and out/ for development
    echo "  ✓ VSCode plugin artifacts cleaned (keeping node_modules and out/)"
    
    cd "$PROJECT_ROOT"
fi

echo ""
echo "Step 7: Clean Package Artifacts..."
rm -rf "$PROJECT_ROOT/CHTL_Release_v*"
rm -f "$PROJECT_ROOT/CHTL_*.zip"
rm -f "$PROJECT_ROOT/CHTL_*.tar.gz"

echo "  ✓ Package artifacts cleaned"

echo ""
echo "Step 8: Clean Compilation Timer Logs..."
rm -f "$PROJECT_ROOT"/compilation_*.log
rm -f "$PROJECT_ROOT"/timer_*.txt

echo "  ✓ Compilation timer logs cleaned"

echo ""
echo "Step 9: Repository Status..."
if command -v git &> /dev/null; then
    echo "Git status after cleanup:"
    cd "$PROJECT_ROOT"
    git status --porcelain | head -10
    
    UNTRACKED=$(git status --porcelain | grep "^??" | wc -l)
    if [ "$UNTRACKED" -gt 0 ]; then
        echo "  ⚠️  Untracked files: $UNTRACKED"
    else
        echo "  ✓ No untracked files"
    fi
fi

echo ""
echo "Step 10: Disk Space Analysis..."
TOTAL_SIZE=$(du -sh "$PROJECT_ROOT" | cut -f1)
echo "  📊 Total project size: $TOTAL_SIZE"

# Show largest directories
echo "  📊 Largest directories:"
du -sh "$PROJECT_ROOT"/* 2>/dev/null | sort -hr | head -5

echo ""
echo "==============================================="
echo "SUCCESS: Build Cleanup Complete"
echo ""
echo "Cleaned Items:"
echo "  ✓ Build directories and artifacts"
echo "  ✓ CMake cache and generated files"
echo "  ✓ Object files and libraries"
echo "  ✓ Temporary and backup files"
echo "  ✓ Test output files"
echo "  ✓ Package artifacts"
echo "  ✓ Compilation logs"
echo ""
echo "Project Size: $TOTAL_SIZE"
echo "Repository Status: Clean"
echo "==============================================="