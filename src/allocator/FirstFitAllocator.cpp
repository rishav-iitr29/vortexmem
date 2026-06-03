#include "FirstFitAllocator.hpp"
using namespace std;

bool FirstFitAllocator::allocate(int id, size_t size) {
    total_allocations++;
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (it->is_free && it->size >= size) {
            if (it->size > size) {
                MemoryBlock leftover{it->start_address + size, it->size - size, true, -1};
                blocks.insert(next(it), leftover);
            }
            it->size = size;
            it->is_free = false;
            it->block_id = id;
            successful_allocations++;
            return true;
        }
    }
    failed_allocations++;
    return false;
}