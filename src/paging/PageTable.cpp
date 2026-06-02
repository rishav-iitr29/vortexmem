#include "../include/PageTable.hpp"

PageTable::PageTable(size_t page_sz, size_t physical_memory_size) 
    : page_size(page_sz) {
    num_frames = physical_memory_size / page_size;
    
    // Initialize all physical frames to -1
    frame_mapping.resize(num_frames, -1); 
}

int PageTable::run_clock_eviction() {
    while (true) {
        int current_vpn = frame_mapping[clock_hand];

        // 1. If the physical frame is completely empty, just take it
        if (current_vpn == -1) {
            int allocated_frame = clock_hand;
            clock_hand = (clock_hand + 1) % num_frames;
            return allocated_frame;
        }

        // 2. If occupied, inspect the PTE of the page currently residing here
        PageTableEntry& pte = table[current_vpn];

        if (pte.referenced) {
            // Clear the bit and move the hand.
            pte.referenced = false;
            clock_hand = (clock_hand + 1) % num_frames;
        } else {
            if (pte.dirty) {
                disk_writes++; 
            }
            pte.valid = false; // Invalidate the old VPN mapping
            
            int victim_frame = clock_hand;
            clock_hand = (clock_hand + 1) % num_frames;
            return victim_frame;
        }
    }
}

size_t PageTable::access(size_t virtual_address, bool is_write) {
    size_t vpn = virtual_address / page_size;
    size_t offset = virtual_address % page_size;

    PageTableEntry& pte = table[vpn];

    if (!pte.valid) {
        // Page is not in physical memory
        page_faults++;
        
        // Find a physical frame using the Clock algorithm
        int target_frame = run_clock_eviction();
        
        // Map the new page into the evicted frame
        frame_mapping[target_frame] = vpn;
        pte.frame_number = target_frame;
        pte.valid = true;
        pte.dirty = false; // Not dirty because freshly loaded
    }

    // HIT (or just freshly loaded)
    pte.referenced = true;
    if (is_write) pte.dirty = true;

    // Construct and return the physical address
    return (pte.frame_number * page_size) + offset;
}

void PageTable::print_stats() const {
    std::cout << "[Paging Telemetry] Page Faults: " << page_faults 
              << " | Disk Write-Backs: " << disk_writes << "\n";
}