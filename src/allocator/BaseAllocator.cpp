#include "BaseAllocator.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

BaseAllocator::BaseAllocator(size_t size) : total_size(size) {
    // Memory starts as a single continuous free block
    blocks.push_back({0, total_size, true, -1});
}

bool BaseAllocator::deallocate(int id) {
    for (auto& block : blocks) {
        if (!block.is_free && block.block_id == id) {
            block.is_free = true;
            block.block_id = -1;
            
            coalesce();
            return true;
        }
    }
    return false; 
}

void BaseAllocator::coalesce() {
    auto it = blocks.begin();
    while (it != blocks.end()) {
        auto next_it = next(it);
        if (next_it != blocks.end() && it->is_free && next_it->is_free) {
            // Absorb adjacent right-hand space block
            it->size += next_it->size;
            blocks.erase(next_it);
            // Don't step forward; check the newly adjoined neighbor block
        } else {
            ++it;
        }
    }
}

void BaseAllocator::debug_dump() const {
    cout << "Layout- ";
    for (const auto& block : blocks) {
        cout << (block.is_free ? "[Free: " : "[Allocated ID " + to_string(block.block_id) + ": ")
                  << block.size << "B, Addr: " << block.start_address << "] -> ";
    }
    cout << "END\n";
}

void BaseAllocator::print_advanced_stats() const {
    size_t free_memory = 0;
    size_t largest_free = 0;

    for (const auto& block : blocks) {
        if (block.is_free) {
            free_memory += block.size;
            if (block.size > largest_free) largest_free = block.size;
        }
    }

    size_t allocated_memory = total_size - free_memory;
    double utilization = (total_size > 0) ? (static_cast<double>(allocated_memory) / total_size) * 100.0 : 0.0;
    double ext_frag = (free_memory > 0) ? (static_cast<double>(free_memory - largest_free) / free_memory) * 100.0 : 0.0;
    double success_rate = (total_allocations > 0) ? (static_cast<double>(successful_allocations) / total_allocations) * 100.0 : 0.0;

    cout << "\n--- Memory Statistics ---\n";
    cout << "Total Memory: " << total_size << " B\n";
    cout << "Used Memory:  " << allocated_memory << " B\n";
    cout << "Free Memory:  " << free_memory << " B\n";
    cout << fixed << setprecision(2);
    cout << "Memory Utilization:     " << utilization << "%\n";
    cout << "External Fragmentation: " << ext_frag << "%\n";
    
    cout << "\n--- Allocation Reliability ---\n";
    cout << "Total Attempts: " << total_allocations << "\n";
    cout << "Successful:     " << successful_allocations << "\n";
    cout << "Failed:         " << failed_allocations << "\n";
    cout << "Success Rate:   " << success_rate << "%\n\n";
}