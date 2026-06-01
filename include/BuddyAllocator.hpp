#ifndef BUDDY_ALLOCATOR_HPP
#define BUDDY_ALLOCATOR_HPP

#include "BaseAllocator.hpp"

class BuddyAllocator : public BaseAllocator {
private:
    // Helper to calculate the next highest power of two
    size_t next_power_of_two(size_t size) const;

public:
    explicit BuddyAllocator(size_t size);
    bool allocate(int id, size_t size) override;
    bool deallocate(int id) override;
    
    // to highlight power-of-two blocks
    void debug_dump() const;
};

#endif