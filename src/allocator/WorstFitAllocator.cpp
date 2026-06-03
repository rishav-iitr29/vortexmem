#include "WorstFitAllocator.hpp"
using namespace std;

bool WorstFitAllocator::allocate(int id, size_t size) {
    total_allocations++;
    auto worst_match = blocks.end();
    size_t max_size = 0;

    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (it->is_free && it->size >= size) {
            if (it->size > max_size) {
                max_size = it->size;
                worst_match = it;
            }
        }
    }

    if (worst_match != blocks.end()) {
        if (worst_match->size > size) {
            MemoryBlock leftover{worst_match->start_address + size, worst_match->size - size, true, -1};
            blocks.insert(next(worst_match), leftover);
        }
        worst_match->size = size;
        worst_match->is_free = false;
        worst_match->block_id = id;
        worst_match->requested_size = size;
        successful_allocations++;
        return true;
    }
    failed_allocations++;
    return false;
}