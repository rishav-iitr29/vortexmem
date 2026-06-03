#include "BestFitAllocator.hpp"

using namespace std;

bool BestFitAllocator::allocate(int id, size_t size) {
    total_allocations++;

    auto best_match = blocks.end();
    size_t min_delta = -1; // act as max size_t capacity

    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (it->is_free && it->size >= size) {
            size_t delta = it->size - size;
            if (delta < min_delta) {
                min_delta = delta;
                best_match = it;
            }
        }
    }

    if (best_match != blocks.end()) {
        if (best_match->size > size) {
            MemoryBlock leftover{best_match->start_address + size, best_match->size - size, true, -1};
            blocks.insert(next(best_match), leftover);
        }
        best_match->size = size;
        best_match->is_free = false;
        best_match->block_id = id;

        best_match->requested_size = size;

        successful_allocations++;
        return true;
    }
    failed_allocations++;
    return false;
}