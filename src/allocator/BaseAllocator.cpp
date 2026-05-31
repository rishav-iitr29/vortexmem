#include "BaseAllocator.hpp"
#include <iostream>

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