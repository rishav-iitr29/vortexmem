#!/bin/bash

GREEN='\033[0;32m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m' # No Color

echo -e "${BOLD}  VortexMem: Automated Core Test Execution Suite  ${NC}"

# 1. Ensure the compiled binary executable exists
BINARY="./build/vortexmem"
if [ ! -f "$BINARY" ]; then
    echo -e "Error: Compiled executable not found at $BINARY"
    echo -e "Please navigate to your build folder and run 'make' first."
    exit 1
fi

# 2. Re-create the output target results folder cleanly
echo -e "\n${BLUE}Initializing results directory...${NC}"
rm -rf results
mkdir -p results

# 3. Define an associative-style map of test inputs and output filenames
# Array format: "input_file_name:output_result_name:Test description"
DECLARE_TESTS=(
    "seq_alloc_test.txt:seq_alloc_result.txt:Sequential Allocation Validation"
    "allocation_failure_test.txt:allocation_failure_result.txt:OOM Success Rate Drop"
    "allocator_comparison_test.txt:allocator_comparison_result.txt:First-Fit vs Buddy Layout Split"
    "stress_allocation_test.txt:stress_allocation_result.txt:Heavy Memory Fragmentation Load"
    "cache_hit_test.txt:cache_hit_result.txt:Cache Temporal Locality Tracking"
    "multilevel_cache_test.txt:multilevel_cache_result.txt:Multi-level Spatial Cache Rescue"
    "lru_test.txt:lru_result.txt:Least Recently Used Eviction Verification"
    "page_fault_test.txt:page_fault_result.txt:OS Page Mapping and Cluster Faults"
    "translation_test.txt:translation_result.txt:Dirty Bit Paging & Clock Write-Backs"
)

# 4. Loop over the array and stream operations through the C++ CLI engine
for entry in "${DECLARE_TESTS[@]}"; do
    IFS=":" read -r input output desc <<< "$entry"
    
    echo -n -e "Running Test: $(printf '%-45s' "$desc")... "
    
    if [ -f "tests/$input" ]; then
        # Feed the workload file directly into the input stream of our binary
        $BINARY < "tests/$input" > "results/$output" 2>&1
        echo -e "${GREEN} Completed${NC}"
    else
        echo -e "Error: tests/$input missing!"
    fi
done

echo -e "${GREEN}${BOLD}All tests executed successfully!${NC}"
echo -e "Logs and telemetry files saved inside the ${BOLD}results/${NC} directory."