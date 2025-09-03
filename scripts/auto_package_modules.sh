#!/bin/bash

echo "==============================================="
echo "CHTL Auto Module Packaging Script"
echo "Handle Module Directory (source) -> module directory (packaged)"
echo "Support both chaotic and classified structures"
echo "==============================================="

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SOURCE_MODULE_DIR="$PROJECT_ROOT/src/Module"  # 官方源码目录
OUTPUT_MODULE_DIR="$PROJECT_ROOT/build/module"  # 官方打包目录

echo ""
echo "Step 1: Analyze Module Directory Structure..."
echo "Source Module Directory: $SOURCE_MODULE_DIR"
echo "Output Module Directory: $OUTPUT_MODULE_DIR"

# 创建输出目录
mkdir -p "$OUTPUT_MODULE_DIR"

echo ""
echo "Step 2: Detect Module Directory Structure..."

# 检测是否是分类结构（存在CMOD和CJMOD目录）
if [ -d "$SOURCE_MODULE_DIR/CMOD" ] || [ -d "$SOURCE_MODULE_DIR/Cmod" ] || [ -d "$SOURCE_MODULE_DIR/cmod" ] ||
   [ -d "$SOURCE_MODULE_DIR/CJMOD" ] || [ -d "$SOURCE_MODULE_DIR/CJmod" ] || [ -d "$SOURCE_MODULE_DIR/cjmod" ]; then
    echo "  📁 检测到分类结构（CMOD/CJMOD目录分离）"
    DIRECTORY_STRUCTURE="classified"
else
    echo "  📁 检测到混乱结构（模块直接存放）"
    DIRECTORY_STRUCTURE="chaotic"
fi

echo ""
echo "Step 3: Process Modules Based on Directory Structure..."

TOTAL_PROCESSED=0

if [ "$DIRECTORY_STRUCTURE" = "classified" ]; then
    # 处理分类结构
    echo "处理分类结构模块目录..."
    
    # 处理CMOD目录
    for CMOD_VARIANT in "CMOD" "Cmod" "cmod"; do
        if [ -d "$SOURCE_MODULE_DIR/$CMOD_VARIANT" ]; then
            echo "  📦 处理 $CMOD_VARIANT 目录..."
            
            for MODULE_PATH in "$SOURCE_MODULE_DIR/$CMOD_VARIANT"/*; do
                if [ -d "$MODULE_PATH" ]; then
                    MODULE_NAME=$(basename "$MODULE_PATH")
                    echo "    🔧 打包CMOD模块: $MODULE_NAME"
                    
                    # 打包CMOD模块
                    cd "$MODULE_PATH"
                    zip -r "$OUTPUT_MODULE_DIR/$MODULE_NAME.cmod" src/ info/ -x "*.DS_Store" "*.tmp" "*~"
                    TOTAL_PROCESSED=$((TOTAL_PROCESSED + 1))
                fi
            done
        fi
    done
    
    # 处理CJMOD目录
    for CJMOD_VARIANT in "CJMOD" "CJmod" "cjmod"; do
        if [ -d "$SOURCE_MODULE_DIR/$CJMOD_VARIANT" ]; then
            echo "  📦 处理 $CJMOD_VARIANT 目录..."
            
            for MODULE_PATH in "$SOURCE_MODULE_DIR/$CJMOD_VARIANT"/*; do
                if [ -d "$MODULE_PATH" ]; then
                    MODULE_NAME=$(basename "$MODULE_PATH")
                    echo "    🔧 打包CJMOD模块: $MODULE_NAME"
                    
                    # 打包CJMOD模块
                    cd "$MODULE_PATH"
                    zip -r "$OUTPUT_MODULE_DIR/$MODULE_NAME.cjmod" src/ info/ -x "*.DS_Store" "*.tmp" "*~" "*.o" "*.obj"
                    TOTAL_PROCESSED=$((TOTAL_PROCESSED + 1))
                fi
            done
        fi
    done
    
else
    # 处理混乱结构
    echo "处理混乱结构模块目录..."
    
    for MODULE_PATH in "$SOURCE_MODULE_DIR"/*; do
        if [ -d "$MODULE_PATH" ]; then
            MODULE_NAME=$(basename "$MODULE_PATH")
            echo "  🔍 分析模块: $MODULE_NAME"
            
            # 自动识别模块结构
            HAS_CMOD=false
            HAS_CJMOD=false
            HAS_CHTL=false
            
            # 检查CMOD + CJMOD分隔结构
            if [ -d "$MODULE_PATH/CMOD" ] || [ -d "$MODULE_PATH/Cmod" ] || [ -d "$MODULE_PATH/cmod" ]; then
                HAS_CMOD=true
            fi
            
            if [ -d "$MODULE_PATH/CJMOD" ] || [ -d "$MODULE_PATH/CJmod" ] || [ -d "$MODULE_PATH/cjmod" ]; then
                HAS_CJMOD=true
            fi
            
            # 检查传统CMOD结构
            if [ -d "$MODULE_PATH/src" ] && [ -d "$MODULE_PATH/info" ]; then
                # 检查是否包含C++文件
                CPP_COUNT=$(find "$MODULE_PATH/src" -name "*.cpp" -o -name "*.h" 2>/dev/null | wc -l)
                if [ "$CPP_COUNT" -gt 0 ]; then
                    HAS_CJMOD=true
                else
                    HAS_CHTL=true
                fi
            fi
            
            # 检查零散的chtl文件
            CHTL_COUNT=$(find "$MODULE_PATH" -maxdepth 1 -name "*.chtl" 2>/dev/null | wc -l)
            if [ "$CHTL_COUNT" -gt 0 ]; then
                HAS_CHTL=true
            fi
            
            # 根据检测结果打包
            if [ "$HAS_CMOD" = true ] && [ "$HAS_CJMOD" = true ]; then
                echo "    📦 CMOD + CJMOD混合模块 -> $MODULE_NAME.cmod"
                cd "$MODULE_PATH"
                zip -r "$OUTPUT_MODULE_DIR/$MODULE_NAME.cmod" . -x "*.DS_Store" "*.tmp" "*~" "*.o" "*.obj"
                TOTAL_PROCESSED=$((TOTAL_PROCESSED + 1))
                
            elif [ "$HAS_CJMOD" = true ]; then
                echo "    📦 纯CJMOD模块 -> $MODULE_NAME.cjmod"
                cd "$MODULE_PATH"
                zip -r "$OUTPUT_MODULE_DIR/$MODULE_NAME.cjmod" . -x "*.DS_Store" "*.tmp" "*~" "*.o" "*.obj"
                TOTAL_PROCESSED=$((TOTAL_PROCESSED + 1))
                
            elif [ "$HAS_CHTL" = true ]; then
                echo "    📦 纯CMOD模块 -> $MODULE_NAME.cmod"
                cd "$MODULE_PATH"
                zip -r "$OUTPUT_MODULE_DIR/$MODULE_NAME.cmod" . -x "*.DS_Store" "*.tmp" "*~"
                TOTAL_PROCESSED=$((TOTAL_PROCESSED + 1))
                
            else
                echo "    ⚠️  无法识别模块类型: $MODULE_NAME"
            fi
        fi
    done
    
    # 处理零散的chtl文件
    SCATTERED_CHTL=$(find "$SOURCE_MODULE_DIR" -maxdepth 1 -name "*.chtl" 2>/dev/null)
    if [ -n "$SCATTERED_CHTL" ]; then
        echo "  📄 复制零散的chtl文件..."
        for CHTL_FILE in $SCATTERED_CHTL; do
            cp "$CHTL_FILE" "$OUTPUT_MODULE_DIR/"
            echo "    📋 复制: $(basename "$CHTL_FILE")"
            TOTAL_PROCESSED=$((TOTAL_PROCESSED + 1))
        done
    fi
fi

cd "$PROJECT_ROOT"

echo ""
echo "Step 4: Generate Module Directory Summary..."
MODULE_SUMMARY="$OUTPUT_MODULE_DIR/module_directory_summary.txt"
cat > "$MODULE_SUMMARY" << EOF
CHTL Module Directory Summary
Generated: $(date)
Source Directory: $SOURCE_MODULE_DIR
Output Directory: $OUTPUT_MODULE_DIR
Directory Structure: $DIRECTORY_STRUCTURE

Total Processed Modules: $TOTAL_PROCESSED

Module Structure Types:
1. CMOD Structure - Pure CHTL modules (.cmod)
2. CJMOD Structure - Pure CHTL JS extension modules (.cjmod)  
3. CMOD + CJMOD Structure - Mixed modules (.cmod with both parts)

Module Directory Types:
1. Module (source) - Official source code directory
2. module (packaged) - Official packaged module directory
3. module (user) - User created module directory

Both module directories support:
- Chaotic Structure: Direct module placement
- Classified Structure: CMOD/CJMOD directory separation

Packaged Modules:
EOF

ls -la "$OUTPUT_MODULE_DIR"/*.cmod "$OUTPUT_MODULE_DIR"/*.cjmod "$OUTPUT_MODULE_DIR"/*.chtl 2>/dev/null | while read line; do
    echo "  $line" >> "$MODULE_SUMMARY"
done

echo ""
echo "==============================================="
echo "SUCCESS: Auto Module Packaging Complete"
echo ""
echo "Directory Structure: $DIRECTORY_STRUCTURE"
echo "Modules Processed: $TOTAL_PROCESSED"
echo "Output Directory: $OUTPUT_MODULE_DIR"
echo "Summary: $MODULE_SUMMARY"
echo ""
echo "Module Structure vs Module Directory:"
echo "  📁 Module Structure: Fixed design (CMOD/CJMOD/CMOD+CJMOD)"
echo "  📁 Module Directory: Flexible storage (chaotic/classified)"
echo "==============================================="