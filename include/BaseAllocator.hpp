#ifndef BASE_ALLOCATOR_HPP
#define BASE_ALLOCATOR_HPP

#include <list>
#include "MemoryBlock.hpp"

using namespace std;

class BaseAllocator {
protected:
    size_t total_size;
    list<MemoryBlock> blocks; // O(1) splits and merges once located

public:
    explicit BaseAllocator(size_t size);
    virtual ~BaseAllocator() = default;

    virtual bool allocate(int id, size_t size) = 0;
    
    virtual bool deallocate(int id);
    void coalesce();

    void debug_dump() const;
};

#endif