# VortexMem - Test Artifacts

## Overview

The directory ```tests/``` contains the test scripts used to validate the memory management simulator. The automated suite evaluates physical memory allocation, hardware cache cascading, and virtual memory page replacement.

## Test Categories

1. Memory Allocation Reliability Tests – Fragmentation profiles and OOM (Out-of-Memory) handling.
2. Hardware Pipeline Tests – Spatial/Temporal locality and L2 rescue mechanics.
3. Virtual Memory Tests – Page faults, Dirty Bits, and Clock algorithm evictions.
<br> <br>

# 1. Memory Allocation Reliability Tests

### Test 1.1: Sequential Allocation (```seq_alloc_test.txt```)

Description: Validates basic contiguous memory allocation using First-Fit.

Expected Behavior:

- All memory requests are satisfied with 100% success.
- 0% internal fragmentation.



### Test 1.2: Allocation Failure & Reliability (```allocation_failure_test.txt```)

Description: Intentionally over-allocates the physical memory bounds to test system stability.

Expected Behavior:

- System rejects allocations safely without crashing.
- Success Rate dynamically drops as requests fail.
- Memory Utilization caps safely at or near 100%.



### Test 1.3: Allocator Layout Comparison (```allocator_comparison_test.txt```)

Description: Runs the exact same workload through the First-Fit allocator and the Buddy System allocator to demonstrate their mathematical trade-offs.

Expected Output Metrics:

**First-Fit**

- Reports >0% External Fragmentation.
- Reports 0% Internal Fragmentation.

**Buddy System**

- Reports 0% External Fragmentation.
- Reports >0% Internal Fragmentation (due to $2^k$ block padding).


### Test 1.4: Stress & Coalescing (```stress_allocation_test.txt```)

Description: A high-volume test that allocates, fragments, and frees multiple distinct blocks.

Expected Behavior:

When multiple adjacent blocks are freed, the dump command shows them coalescing into unified, contiguous memory blocks.

<br>

# 2. Hardware Pipeline & Cache Tests

### Test 2.1: Temporal Locality (```cache_hit_test.txt```)

Description: Accesses the same memory address repeatedly.

Expected Behavior:

- First Access: Cold Miss across all layers.
- Subsequent Accesses: 100% Hit Rate on the TLB and L1 Cache.


### Test 2.2: Multi-Level Spatial Rescue (```multilevel_cache_test.txt```)

Description: Demonstrates spatial locality utilizing differing cache line widths (L1 = 32B, L2 = 64B).

Expected Behavior:

- Reading ```0x1000``` fetches data into both L1 and L2.
- Reading ```0x1020``` bypasses L1 (out of 32B bounds) but is caught by L2.
- Metric Signature: L1 Misses > 0, L2 Hits > 0.


### Test 2.3: LRU Eviction Verification (```lru_test.txt```)

Description: Floods the caches with unique addresses to exceed structural capacity, forcing Least Recently Used evictions.

Expected Behavior:

Addresses that are read multiple times survive in the cache, while older, unused addresses are flushed out.

<br>

# 3. Virtual Memory Tests

### Test 3.1: OS Page Faults (```page_fault_test.txt```)

Description: Tests the MMU's ability to cluster memory requests into 4 KB pages.

Expected Behavior:

- Reading ```0x0010``` and ```0x00A0``` triggers only 1 Page Fault (they share Virtual Page 0).
- Reading ```0x1000``` and ```0x2000``` triggers new Page Faults.


### Test 3.2: Dirty Bit Write-Backs (```translation_test.txt```)

Description: Uses the write command to modify memory, then floods the RAM to force the OS Clock algorithm to evict those modified pages.

Expected Behavior:

- Modified pages trigger Disk Write-Backs upon eviction.
- Metric Signature: [Paging Telemetry] Disk Write-Backs > 0.

<br>

# Execution & Automation Instructions

## The Bash Runner

All tests can be executed sequentially using the automated bash script located in the project root:

```bash 
chmod +x run_all_tests.sh 
./run_all_tests.sh
```

This script pipes the ```.txt``` workloads into the vortexmem executable and saves the detailed telemetry dumps into the ```results/``` directory.

---

## Python Validation

An included Python script (```validate_results.py```) parses the ```results/``` folder to ensure all output telemetry strictly adheres to operating system mathematical invariants (e.g., $Total Memory = Used + Free$).

```bash 
python3 validate_results.py 
```

---

## Expected Automation Output

```
VortexMem: Validating Telemetry Results

stress_allocation_result.txt passed (Allocation Rate: 100.00%)
cache_hit_result.txt passed (TLB Hits: 2, Page Faults: 1)
allocator_comparison_result.txt passed (Allocation Rate: 100.00%)
allocation_failure_result.txt passed (Allocation Rate: 100.00%)
multilevel_cache_result.txt passed (TLB Hits: 1, Page Faults: 1)
translation_result.txt passed (TLB Hits: 0, Page Faults: 9)
page_fault_result.txt passed (TLB Hits: 0, Page Faults: 4)
lru_result.txt passed (TLB Hits: 2, Page Faults: 6)
seq_alloc_result.txt passed (Allocation Rate: 100.00%)


All 9 tests passed validation!
```