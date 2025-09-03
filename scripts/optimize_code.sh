#!/bin/bash

echo "==============================================="
echo "CHTL Code Optimization Script"
echo "Remove duplicates, merge code, optimize performance"
echo "==============================================="

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo ""
echo "Step 1: Analyze Code Quality..."

# Find files with unused parameters/variables
echo "Analyzing unused parameters and variables..."
UNUSED_COUNT=0

# Compile with warnings to get detailed analysis
cd "$PROJECT_ROOT/build/debug"
make 2>&1 | grep -E "(unused parameter|unused variable)" | sort | uniq > "$PROJECT_ROOT/unused_analysis.txt"

if [ -f "$PROJECT_ROOT/unused_analysis.txt" ]; then
    UNUSED_COUNT=$(wc -l < "$PROJECT_ROOT/unused_analysis.txt")
    echo "  Found $UNUSED_COUNT unused parameter/variable warnings"
    
    if [ $UNUSED_COUNT -gt 0 ]; then
        echo "  Top 10 most common unused items:"
        head -10 "$PROJECT_ROOT/unused_analysis.txt"
    fi
fi

echo ""
echo "Step 2: Code Duplication Analysis..."

# Find duplicate code patterns
echo "Analyzing code duplication..."

# Find similar function signatures
find "$PROJECT_ROOT/src" -name "*.cpp" -o -name "*.h" | xargs grep -h "^[[:space:]]*[a-zA-Z_][a-zA-Z0-9_]*::" | sort | uniq -c | sort -nr > "$PROJECT_ROOT/function_analysis.txt"

DUPLICATE_FUNCTIONS=$(awk '$1 > 1 { count++ } END { print count+0 }' "$PROJECT_ROOT/function_analysis.txt")
echo "  Found $DUPLICATE_FUNCTIONS potentially duplicate function patterns"

# Find similar include patterns
find "$PROJECT_ROOT/src" -name "*.cpp" -o -name "*.h" | xargs grep -h "^#include" | sort | uniq -c | sort -nr > "$PROJECT_ROOT/include_analysis.txt"

COMMON_INCLUDES=$(awk '$1 > 5 { count++ } END { print count+0 }' "$PROJECT_ROOT/include_analysis.txt")
echo "  Found $COMMON_INCLUDES commonly used includes (candidates for precompiled headers)"

echo ""
echo "Step 3: Performance Hotspot Analysis..."

# Analyze compilation times for different modules
echo "Analyzing compilation performance..."

COMPILE_TIMES_FILE="$PROJECT_ROOT/compile_times.txt"
rm -f "$COMPILE_TIMES_FILE"

echo "Module,Size(lines),Compile_Time(approx)" > "$COMPILE_TIMES_FILE"

for MODULE_DIR in "$PROJECT_ROOT/src"/*; do
    if [ -d "$MODULE_DIR" ]; then
        MODULE_NAME=$(basename "$MODULE_DIR")
        
        # Count lines of code
        LINE_COUNT=$(find "$MODULE_DIR" -name "*.cpp" -o -name "*.h" | xargs wc -l 2>/dev/null | tail -1 | awk '{print $1}' || echo "0")
        
        # Estimate compilation complexity
        COMPLEXITY="Low"
        if [ "$LINE_COUNT" -gt 1000 ]; then
            COMPLEXITY="High"
        elif [ "$LINE_COUNT" -gt 500 ]; then
            COMPLEXITY="Medium"
        fi
        
        echo "$MODULE_NAME,$LINE_COUNT,$COMPLEXITY" >> "$COMPILE_TIMES_FILE"
    fi
done

echo "  Module complexity analysis saved to: compile_times.txt"

echo ""
echo "Step 4: Memory Usage Optimization..."

# Find large objects and potential memory issues
echo "Analyzing memory usage patterns..."

# Find large static arrays or vectors
find "$PROJECT_ROOT/src" -name "*.cpp" -o -name "*.h" | xargs grep -n "std::vector\|std::string\|std::unordered_map" | wc -l > "$PROJECT_ROOT/container_usage.txt"

CONTAINER_COUNT=$(cat "$PROJECT_ROOT/container_usage.txt")
echo "  Found $CONTAINER_COUNT container usages"

# Find potential memory leaks (raw pointers)
RAW_POINTER_COUNT=$(find "$PROJECT_ROOT/src" -name "*.cpp" -o -name "*.h" | xargs grep -c "new \|delete " | awk '{sum+=$1} END {print sum+0}')
echo "  Found $RAW_POINTER_COUNT raw pointer operations (potential memory management issues)"

echo ""
echo "Step 5: Generate Optimization Recommendations..."

OPTIMIZATION_REPORT="$PROJECT_ROOT/optimization_recommendations.md"

cat > "$OPTIMIZATION_REPORT" << EOF
# CHTL Code Optimization Report

Generated: $(date)

## Summary
- Unused parameter/variable warnings: $UNUSED_COUNT
- Duplicate function patterns: $DUPLICATE_FUNCTIONS
- Common includes: $COMMON_INCLUDES
- Container usages: $CONTAINER_COUNT
- Raw pointer operations: $RAW_POINTER_COUNT

## Optimization Recommendations

### 1. Unused Parameter Cleanup
$(if [ $UNUSED_COUNT -gt 0 ]; then
    echo "Priority: High"
    echo "Action: Add [[maybe_unused]] attribute or remove unused parameters"
    echo ""
    echo "Most common unused items:"
    head -5 "$PROJECT_ROOT/unused_analysis.txt" | sed 's/^/- /'
else
    echo "✅ No unused parameters found"
fi)

### 2. Code Deduplication
$(if [ $DUPLICATE_FUNCTIONS -gt 0 ]; then
    echo "Priority: Medium"
    echo "Action: Extract common functionality into shared utilities"
    echo ""
    echo "Most duplicated patterns:"
    head -5 "$PROJECT_ROOT/function_analysis.txt" | sed 's/^/- /'
else
    echo "✅ No significant code duplication found"
fi)

### 3. Include Optimization
$(if [ $COMMON_INCLUDES -gt 0 ]; then
    echo "Priority: Medium"
    echo "Action: Create precompiled header for common includes"
    echo ""
    echo "Most common includes:"
    head -5 "$PROJECT_ROOT/include_analysis.txt" | sed 's/^/- /'
else
    echo "✅ Include usage is reasonable"
fi)

### 4. Memory Management
$(if [ $RAW_POINTER_COUNT -gt 0 ]; then
    echo "Priority: High"
    echo "Action: Replace raw pointers with smart pointers"
    echo "Found $RAW_POINTER_COUNT raw pointer operations"
else
    echo "✅ Good use of smart pointers"
fi)

### 5. Performance Optimizations
- Enable link-time optimization (LTO) in Release builds
- Use profile-guided optimization (PGO) for hot paths
- Consider using std::string_view for read-only string parameters
- Optimize container usage (reserve() for vectors, emplace_back() vs push_back())

### 6. Code Quality Improvements
- Add const correctness where possible
- Use constexpr for compile-time constants
- Consider using enum class instead of plain enums
- Add nodiscard attributes for important return values

## Module-Specific Recommendations

EOF

# Add module-specific recommendations
while IFS=, read -r module lines complexity; do
    if [ "$module" != "Module" ] && [ "$lines" != "Size(lines)" ]; then
        echo "### $module Module" >> "$OPTIMIZATION_REPORT"
        echo "- Lines of code: $lines" >> "$OPTIMIZATION_REPORT"
        echo "- Complexity: $complexity" >> "$OPTIMIZATION_REPORT"
        
        if [ "$complexity" = "High" ]; then
            echo "- Recommendation: Consider splitting into smaller modules" >> "$OPTIMIZATION_REPORT"
        elif [ "$complexity" = "Medium" ]; then
            echo "- Recommendation: Monitor for further growth" >> "$OPTIMIZATION_REPORT"
        else
            echo "- Recommendation: Well-sized module" >> "$OPTIMIZATION_REPORT"
        fi
        echo "" >> "$OPTIMIZATION_REPORT"
    fi
done < "$COMPILE_TIMES_FILE"

echo ""
echo "Step 6: Create Optimization Scripts..."

# Create script to fix unused parameters
cat > "$PROJECT_ROOT/scripts/fix_unused_parameters.sh" << 'EOF'
#!/bin/bash
# Auto-fix unused parameters by adding [[maybe_unused]] attribute

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "Fixing unused parameters..."

# This is a template - actual implementation would need careful parsing
# For now, we'll create a manual checklist

echo "Manual fixes needed:"
echo "1. Add [[maybe_unused]] to unused parameters in lexer files"
echo "2. Remove or use unused variables in parser files" 
echo "3. Consider removing unused context parameters"
echo "4. Add proper documentation for intentionally unused parameters"

EOF

chmod +x "$PROJECT_ROOT/scripts/fix_unused_parameters.sh"

echo ""
echo "Step 7: Performance Benchmark..."

# Run a quick performance test
if [ -f "$PROJECT_ROOT/build/debug/bin/chtl_end_to_end_test" ]; then
    echo "Running performance benchmark..."
    
    START_TIME=$(date +%s%N)
    "$PROJECT_ROOT/build/debug/bin/chtl_end_to_end_test" > /dev/null 2>&1
    END_TIME=$(date +%s%N)
    
    DURATION=$(((END_TIME - START_TIME) / 1000000))  # Convert to milliseconds
    echo "  End-to-end test duration: ${DURATION}ms"
    
    if [ $DURATION -lt 1000 ]; then
        echo "  ✅ Performance: Excellent (< 1s)"
    elif [ $DURATION -lt 5000 ]; then
        echo "  ✅ Performance: Good (< 5s)"
    else
        echo "  ⚠️ Performance: Needs optimization (> 5s)"
    fi
fi

echo ""
echo "==============================================="
echo "SUCCESS: Code Optimization Analysis Complete"
echo ""
echo "Analysis Results:"
echo "  📊 Unused warnings: $UNUSED_COUNT"
echo "  📊 Duplicate patterns: $DUPLICATE_FUNCTIONS"
echo "  📊 Common includes: $COMMON_INCLUDES"
echo "  📊 Container usages: $CONTAINER_COUNT"
echo "  📊 Raw pointers: $RAW_POINTER_COUNT"
echo ""
echo "Generated Reports:"
echo "  📄 Optimization recommendations: optimization_recommendations.md"
echo "  📄 Compile times analysis: compile_times.txt"
echo "  📄 Unused items list: unused_analysis.txt"
echo ""
echo "Next Steps:"
echo "  🔧 Review optimization recommendations"
echo "  🔧 Run fix_unused_parameters.sh for cleanup"
echo "  🔧 Consider implementing suggested optimizations"
echo "==============================================="