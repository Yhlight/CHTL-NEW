#!/bin/bash

echo "==============================================="
echo "CHTL Compilation Timer and Monitor"
echo "Monitor compilation time and memory usage"
echo "Protect user system from runaway processes"
echo "==============================================="

# Configuration
MAX_COMPILATION_TIME=300  # 5 minutes max
MAX_MEMORY_MB=2048       # 2GB max memory
CHECK_INTERVAL=2         # Check every 2 seconds
LOG_FILE="compilation_$(date +%Y%m%d_%H%M%S).log"

# Command to execute
COMMAND="$@"

if [ -z "$COMMAND" ]; then
    echo "Usage: $0 <compilation_command>"
    echo "Example: $0 make -j4"
    echo "Example: $0 cmake --build . --config Release"
    exit 1
fi

echo ""
echo "Configuration:"
echo "  Max Compilation Time: ${MAX_COMPILATION_TIME}s"
echo "  Max Memory Usage: ${MAX_MEMORY_MB}MB"
echo "  Check Interval: ${CHECK_INTERVAL}s"
echo "  Log File: $LOG_FILE"
echo ""
echo "Command: $COMMAND"

# Create log file
cat > "$LOG_FILE" << EOF
CHTL Compilation Timer Log
Started: $(date)
Command: $COMMAND
Max Time: ${MAX_COMPILATION_TIME}s
Max Memory: ${MAX_MEMORY_MB}MB

Time(s) | Memory(MB) | CPU(%) | Status
--------|-----------|--------|--------
EOF

echo ""
echo "Starting monitored compilation..."

# Start the compilation process
$COMMAND &
COMPILE_PID=$!

echo "Compilation PID: $COMPILE_PID"
echo "Monitoring started..."

# Monitor the process
START_TIME=$(date +%s)
ELAPSED=0

while kill -0 $COMPILE_PID 2>/dev/null; do
    CURRENT_TIME=$(date +%s)
    ELAPSED=$((CURRENT_TIME - START_TIME))
    
    # Check time limit
    if [ $ELAPSED -gt $MAX_COMPILATION_TIME ]; then
        echo ""
        echo "⚠️  TIMEOUT: Compilation exceeded ${MAX_COMPILATION_TIME}s"
        echo "Terminating compilation process..."
        
        # Kill the process tree
        pkill -P $COMPILE_PID 2>/dev/null
        kill $COMPILE_PID 2>/dev/null
        sleep 2
        kill -9 $COMPILE_PID 2>/dev/null
        
        echo "TIMEOUT: Compilation killed at ${ELAPSED}s" >> "$LOG_FILE"
        echo ""
        echo "ERROR: Compilation timeout - process terminated for system protection"
        exit 1
    fi
    
    # Get memory and CPU usage
    if command -v ps &> /dev/null; then
        PROCESS_INFO=$(ps -p $COMPILE_PID -o pid,ppid,%mem,%cpu,rss 2>/dev/null | tail -n +2)
        if [ -n "$PROCESS_INFO" ]; then
            MEMORY_KB=$(echo "$PROCESS_INFO" | awk '{print $5}')
            MEMORY_MB=$((MEMORY_KB / 1024))
            CPU_PERCENT=$(echo "$PROCESS_INFO" | awk '{print $4}')
            
            # Check memory limit
            if [ $MEMORY_MB -gt $MAX_MEMORY_MB ]; then
                echo ""
                echo "⚠️  MEMORY LIMIT: Process using ${MEMORY_MB}MB (limit: ${MAX_MEMORY_MB}MB)"
                echo "Terminating compilation process..."
                
                kill $COMPILE_PID 2>/dev/null
                sleep 2
                kill -9 $COMPILE_PID 2>/dev/null
                
                echo "MEMORY_LIMIT: Compilation killed at ${ELAPSED}s, ${MEMORY_MB}MB" >> "$LOG_FILE"
                echo ""
                echo "ERROR: Memory limit exceeded - process terminated for system protection"
                exit 1
            fi
            
            # Log status
            printf "%7d | %9d | %6s | Running\n" $ELAPSED $MEMORY_MB "$CPU_PERCENT" >> "$LOG_FILE"
            
            # Progress indicator
            printf "\r⏱️  Time: %ds | Memory: %dMB | CPU: %s%% " $ELAPSED $MEMORY_MB "$CPU_PERCENT"
        fi
    else
        printf "\r⏱️  Time: %ds | Status: Running " $ELAPSED
    fi
    
    sleep $CHECK_INTERVAL
done

# Get exit status
wait $COMPILE_PID
EXIT_STATUS=$?

FINAL_TIME=$(date +%s)
TOTAL_ELAPSED=$((FINAL_TIME - START_TIME))

echo ""
echo ""
echo "Compilation finished!"
echo "Total time: ${TOTAL_ELAPSED}s"

# Final log entry
echo "COMPLETED: Total time ${TOTAL_ELAPSED}s, Exit status: $EXIT_STATUS" >> "$LOG_FILE"
echo "Finished: $(date)" >> "$LOG_FILE"

# Summary
if [ $EXIT_STATUS -eq 0 ]; then
    echo "✅ SUCCESS: Compilation completed successfully"
    echo "  ⏱️  Total time: ${TOTAL_ELAPSED}s"
    echo "  📊 Log saved: $LOG_FILE"
else
    echo "❌ FAILED: Compilation failed with exit status $EXIT_STATUS"
    echo "  ⏱️  Total time: ${TOTAL_ELAPSED}s"
    echo "  📊 Log saved: $LOG_FILE"
fi

echo ""
echo "Performance Summary:"
if [ -f "$LOG_FILE" ]; then
    echo "  📈 Peak memory usage:"
    grep -E "^[0-9]" "$LOG_FILE" | awk '{print $3}' | sort -nr | head -1 | xargs -I {} echo "     {}MB"
    
    echo "  📈 Average CPU usage:"
    grep -E "^[0-9]" "$LOG_FILE" | awk '{sum+=$5; count++} END {if(count>0) printf "     %.1f%%\n", sum/count}'
fi

echo ""
echo "==============================================="

exit $EXIT_STATUS