#ifndef PAGE_TABLE_HPP
#define PAGE_TABLE_HPP

#include <cstddef>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "MemoryBlock.hpp"

using namespace std;

struct PageTableEntry {
    int frame_number = -1;
    bool valid = false;
    bool referenced = false;
    bool dirty = false;
};

class PageTable {
private:
    size_t page_size;
    size_t num_frames;
    
    // Forward map: Virtual Page Number (VPN) -> Page Table Entry
    unordered_map<size_t, PageTableEntry> table;
    
    // Reverse map: Physical Frame Number -> VPN (Needed for Clock eviction)
    vector<int> frame_mapping; 
    size_t clock_hand = 0;

    // Instrumentation metrics
    size_t page_faults = 0;
    size_t disk_writes = 0;

    // Internal clock loop
    int run_clock_eviction();

public:
    PageTable(size_t page_sz, size_t physical_memory_size);
    
    // return the translated Physical Address
    size_t access(size_t virtual_address, bool is_write);
    
    void print_stats() const;

    size_t get_page_faults() const { return page_faults; }
};

#endif
