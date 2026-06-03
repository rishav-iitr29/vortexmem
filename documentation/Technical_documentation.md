# VortexMem - Technical Documentation

## Implementation Summary

### CORE FEATURES IMPLEMENTED

- Advanced Physical Memory Management: Supports standard continuous algorithms (First-fit, Best-fit, Worst-fit) alongside a high-performance Buddy System Allocator.
- Unified Hardware Pipeline: Accurately simulates the exact instruction path of modern CPUs: TLB → MMU Page Table → L1 Cache → L2 Cache.
- Hardware Cache Rescue: Supports spatial locality rescues where L1 capacity misses are caught by larger L2 cache block lines.
- Advanced Virtual Memory: Page-based virtual memory utilizing the Clock (Second-Chance) Algorithm with explicit Dirty Bit tracking and Disk Write-Back telemetry.
- Granular Telemetry: Real-time calculation of System AMAT (Average Memory Access Time), External Fragmentation, Internal Fragmentation, and Allocation Success Rates.

<br>

# Table of Contents

1. Memory Layout and Assumptions
2. Allocation Strategy Implementations
3. Cache Hierarchy and the Hardware Pipeline
4. Virtual Memory & Page Replacement
5. Address Translation Flow
6. Educational Simplifications

<br>

# Memory Layout and Assumptions

## Physical Memory Structure

The simulator treats physical memory as a contiguous array of bytes, managed via an intrusive tracking list to simulate heap metadata.

```bash
struct MemoryBlock {
    size_t start_address;  // Starting address of the block     
    size_t size;           // Total padded size in bytes   
    bool is_free;          // Availability status     
    int block_id;          // Unique identifier for allocated blocks     
    size_t requested_size; // The exact byte size the user asked for 
}; 
```
## Key Assumptions

### Word-Aligned Access
Memory operations are modeled on logical byte boundaries.

### Fixed Memory Pool
Total physical memory size is fixed at ```init memory``` execution.

### Internal vs External Fragmentation

External Fragmentation is calculated by analyzing isolated free-block islands.

Internal Fragmentation is calculated dynamically by comparing ```size``` vs ```requested_size```.

<br>

# Allocation Strategy Implementations

VortexMem supports four distinct memory allocation strategies to allow for direct comparative benchmarking.

## 1. Sequential Fit Algorithms (First, Best, Worst)

These algorithms parse the active memory blocks sequentially to find a valid space. They allocate the exact ```requested_size```, resulting in 0% Internal Fragmentation.

### First-Fit
- Fast allocation.
- Selects the first valid block.
- Susceptible to front-loaded external fragmentation.

### Best-Fit
- Scans the entire list to find the tightest fit.
- Minimizes wasted space.
- Can create microscopic, unusable free blocks (splinters).

### Worst-Fit
- Scans the entire list to find the largest free space.
- Maximizes leftover contiguous space after splitting.

## 2. Buddy System Allocator

A highly advanced power-of-two memory allocator designed to eliminate external fragmentation through mathematical block splitting.

### Strategy

Memory is continually halved into "buddies" until the smallest $2^k$ block that can fit the ```requested_size``` is found.

### Internal Fragmentation

High. A request for 65 bytes will allocate a 128-byte block, wasting 63 bytes internally.

### Deallocation (Coalescing)

When a block is freed, it checks if its exact mathematical buddy is also free. If so, they instantly merge back into a $2^{k+1}$ block, bubbling up to the highest possible contiguous size in $O(\log n)$ time.

<br>

# Cache Hierarchy and the Hardware Pipeline

Unlike standard academic simulators, VortexMem forces memory requests down a strict, sequentially dependent hardware pipeline.

## Pipeline Topology

```CPU Request -> TLB    -> MMU Page Table   -> L1 Data Cache    -> L2 Data Cache -> Main Memory ```

## Cache Configurations

### TLB (Translation Lookaside Buffer)
Caches Virtual-to-Physical page mappings to bypass the MMU Page Table.

### L1 Cache
Small, fast, tightly packed block lines (e.g., 32 bytes per line).

### L2 Cache
Larger, slightly slower, wider block lines (e.g., 64 bytes per line).

## The "L2 Rescue" Mechanic (Spatial Locality)

Because L2 cache lines are wider than L1 cache lines, reading ```0x1000``` pulls 32 bytes into L1, but 64 bytes into L2.

A subsequent read to ``0x1020`` will result in:

- An L1 Miss (out of bounds for the 32-byte line)
- An L2 Hit (within the 64-byte line)

This accurately simulates modern processor spatial locality.

<br>

# Virtual Memory & Page Replacement

VortexMem implements a production-grade virtual memory management unit (MMU) featuring Dirty Bit tracking.

## The Clock (Second-Chance) Algorithm

Instead of standard LRU queues, VortexMem uses a cyclic Clock algorithm for page eviction:

1. Every physical frame has a ```referenced``` bit.
2. When the OS needs to evict a page to load a new one, a hardware "clock hand" sweeps through the frames.
3. If a frame's reference bit is ```1```, the OS gives it a "second chance", sets the bit to ```0```, and moves on.
4. The first frame it finds with a ```0``` is selected as the eviction victim.

## Dirty Bits & Disk Write-Backs

### Clean Pages
If a page was only read (```read 0x1000```), it is cleanly overwritten during eviction.

### Dirty Pages
If a page was modified (```write 0x1000```), its dirty bit is set to true.

When the Clock algorithm evicts a dirty page, VortexMem registers a Disk Write-Back to simulate flushing the modified data to the storage swap file.

<br>

# Address Translation Flow

The pipeline strictly adheres to this execution sequence on every CLI ```read``` or ```write``` command:

### 1. TLB Lookup
Mask the Virtual Address to extract the Virtual Page Number (VPN). Check if the TLB holds the corresponding Physical Frame.

### 2. Page Table Walk (On TLB Miss)
The MMU queries the OS Page Table.

### 3. Page Fault Resolution (On Invalid Page)
If the page is not in RAM, the OS executes the Clock algorithm, evicts a page (triggering write-backs if dirty), and loads the new page.

### 4. Physical Data Lookup
The resulting Physical Address is passed to the L1 Cache.

### 5. Cache Cascading
If L1 misses, the Physical Address falls through to L2.

If L2 misses, the block is fetched from Main Memory (RAM) and populated upward.

<br>

# Educational Simplifications

While VortexMem surpasses standard reference implementations, it retains the following simplifications for terminal-based educational testing:

### No Cache Coherency Protocols

Single-threaded execution assumes a single CPU core, rendering protocols like MESI unnecessary.

### Abstract Timing Models

AMAT is calculated using predefined cycle multipliers rather than simulating real-time clock cycles.

### Unprotected Memory Space

There is no isolation between simulated processes or kernel/user mode boundaries.