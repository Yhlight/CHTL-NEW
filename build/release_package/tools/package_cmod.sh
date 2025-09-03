#!/bin/bash

echo "==============================================="
echo "CMOD Packaging Script"
echo "Package CMOD modules according to CHTL syntax documentation"
echo "==============================================="

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MODULE_DIR="$PROJECT_ROOT/src/Module"
OUTPUT_DIR="$PROJECT_ROOT/build/packages/cmod"

echo ""
echo "Step 1: Environment Check..."
if [ ! -d "$MODULE_DIR" ]; then
    echo "ERROR: Module directory not found: $MODULE_DIR"
    exit 1
fi

echo "Module Directory: $MODULE_DIR"
echo "Output Directory: $OUTPUT_DIR"

echo ""
echo "Step 2: Create Output Directory..."
mkdir -p "$OUTPUT_DIR"

echo ""
echo "Step 3: Scan for CMOD Modules..."
CMOD_COUNT=0

# Scan for CMOD format modules
for MODULE_PATH in "$MODULE_DIR"/*; do
    if [ -d "$MODULE_PATH" ]; then
        MODULE_NAME=$(basename "$MODULE_PATH")
        
        # Check CMOD standard format:
        # For CMOD + CJMOD separated structure:
        # ModuleName/
        #   CMOD/
        #     src/
        #       ModuleName.chtl (main module file)
        #       Other.chtl (additional files)
        #     info/
        #       ModuleName.chtl (module info file)
        #   CJMOD/
        #     src/
        #       *.cpp *.h (C++ files)
        #     info/
        #       ModuleName.chtl (CJMOD info file)
        
        # Check for separated structure first
        CMOD_DIR="$MODULE_PATH/CMOD"
        CJMOD_DIR="$MODULE_PATH/CJMOD"
        
        if [ -d "$CMOD_DIR" ] && [ -d "$CJMOD_DIR" ]; then
            # CMOD + CJMOD separated structure
            SRC_DIR="$CMOD_DIR/src"
            INFO_DIR="$CMOD_DIR/info"
            MAIN_MODULE="$SRC_DIR/$MODULE_NAME.chtl"
            INFO_MODULE="$INFO_DIR/$MODULE_NAME.chtl"
        else
            # Traditional CMOD structure
            SRC_DIR="$MODULE_PATH/src"
            INFO_DIR="$MODULE_PATH/info"
            MAIN_MODULE="$SRC_DIR/$MODULE_NAME.chtl"
            INFO_MODULE="$INFO_DIR/$MODULE_NAME.chtl"
        fi
        
        echo ""
        echo "Checking module: $MODULE_NAME"
        
        # Validate CMOD structure
        VALID_CMOD=true
        
        if [ ! -d "$SRC_DIR" ]; then
            echo "  ✗ Missing src/ directory"
            VALID_CMOD=false
        else
            echo "  ✓ src/ directory found"
        fi
        
        if [ ! -d "$INFO_DIR" ]; then
            echo "  ✗ Missing info/ directory"
            VALID_CMOD=false
        else
            echo "  ✓ info/ directory found"
        fi
        
        if [ ! -f "$INFO_MODULE" ]; then
            echo "  ✗ Missing info module file: $MODULE_NAME.chtl"
            VALID_CMOD=false
        else
            echo "  ✓ Info module file found"
            
            # Validate info file content
            if grep -q "\[Info\]" "$INFO_MODULE"; then
                echo "  ✓ [Info] section found"
            else
                echo "  ✗ [Info] section missing"
                VALID_CMOD=false
            fi
        fi
        
        # Check if main module exists or if submodules exist
        if [ -f "$MAIN_MODULE" ]; then
            echo "  ✓ Main module file found: $MODULE_NAME.chtl"
        else
            # Check for submodules
            SUBMODULE_COUNT=$(find "$SRC_DIR" -name "*.chtl" | wc -l)
            if [ "$SUBMODULE_COUNT" -gt 0 ]; then
                echo "  ✓ Submodules found: $SUBMODULE_COUNT files"
            else
                echo "  ✗ No module files found in src/"
                VALID_CMOD=false
            fi
        fi
        
        if [ "$VALID_CMOD" = true ]; then
            echo "  ✅ Valid CMOD module: $MODULE_NAME"
            
            # Package the module
            PACKAGE_FILE="$OUTPUT_DIR/$MODULE_NAME.cmod"
            echo "  📦 Packaging to: $PACKAGE_FILE"
            
            # Create CMOD package (ZIP format with .cmod extension)
            cd "$MODULE_PATH"
            if [ -d "CMOD" ] && [ -d "CJMOD" ]; then
                # Package CMOD + CJMOD separated structure
                zip -r "$PACKAGE_FILE" CMOD/ CJMOD/ -x "*.DS_Store" "*.tmp" "*~" "*.o" "*.obj"
                echo "  📋 Packaged as CMOD + CJMOD separated structure"
            else
                # Package traditional CMOD structure
                zip -r "$PACKAGE_FILE" src/ info/ -x "*.DS_Store" "*.tmp" "*~"
                echo "  📋 Packaged as traditional CMOD structure"
            fi
            
            if [ $? -eq 0 ]; then
                echo "  ✅ Package created successfully"
                echo "  📊 Package size: $(ls -lh "$PACKAGE_FILE" | awk '{print $5}')"
                CMOD_COUNT=$((CMOD_COUNT + 1))
            else
                echo "  ✗ Package creation failed"
            fi
        else
            echo "  ✗ Invalid CMOD module: $MODULE_NAME (skipping)"
        fi
    fi
done

cd "$PROJECT_ROOT"

echo ""
echo "Step 4: Generate CMOD Package Summary..."
SUMMARY_FILE="$OUTPUT_DIR/cmod_packages.txt"
cat > "$SUMMARY_FILE" << EOF
CMOD Package Summary
Generated: $(date)

Total CMOD packages created: $CMOD_COUNT

Package Format: ZIP with .cmod extension
Structure Standard:
  ModuleName/
    src/
      ModuleName.chtl (main module, optional if submodules exist)
      Other.chtl (additional module files)
    info/
      ModuleName.chtl (module information file, required)

Info File Requirements:
  - Must contain [Info] section
  - Required fields: name, version, description, author, license
  - Optional fields: dependencies, category, minCHTLVersion, maxCHTLVersion
  - [Export] section is auto-generated by system

Package List:
EOF

for PACKAGE in "$OUTPUT_DIR"/*.cmod; do
    if [ -f "$PACKAGE" ]; then
        PACKAGE_NAME=$(basename "$PACKAGE")
        PACKAGE_SIZE=$(ls -lh "$PACKAGE" | awk '{print $5}')
        echo "  - $PACKAGE_NAME ($PACKAGE_SIZE)" >> "$SUMMARY_FILE"
    fi
done

echo ""
echo "==============================================="
echo "SUCCESS: CMOD Packaging Complete"
echo ""
echo "Packages Created: $CMOD_COUNT"
echo "Output Directory: $OUTPUT_DIR"
echo "Summary File: $SUMMARY_FILE"
echo ""
echo "CMOD Format Compliance:"
echo "  ✓ Standard folder structure enforced"
echo "  ✓ Required info files validated"
echo "  ✓ [Info] section verified"
echo "  ✓ Package format: ZIP with .cmod extension"
echo "==============================================="