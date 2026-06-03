#ifndef MEMORY_BLOCK_HPP
#define MEMORY_BLOCK_HPP

#include <cstddef>

struct MemoryBlock {
    size_t start_address;
    size_t size;
    size_t requested_size = 0;
    bool is_free;
    int block_id; // -1 if the block is free
};

#endif