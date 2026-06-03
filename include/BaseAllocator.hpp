#ifndef BASE_ALLOCATOR_HPP
#define BASE_ALLOCATOR_HPP

#include <list>
#include <string>
#include "MemoryBlock.hpp"

using namespace std;

class BaseAllocator {
protected:
    size_t total_size;
    list<MemoryBlock> blocks; // O(1) splits and merges once located

    size_t total_allocations = 0;
    size_t successful_allocations = 0;
    size_t failed_allocations = 0;

public:
    explicit BaseAllocator(size_t size);
    virtual ~BaseAllocator() = default;

    virtual bool allocate(int id, size_t size) = 0;
    virtual bool deallocate(int id);
    void coalesce();
    void debug_dump() const;
    
    void print_advanced_stats() const;


    const list<MemoryBlock>& get_blocks() const { return blocks; }
};

#endif