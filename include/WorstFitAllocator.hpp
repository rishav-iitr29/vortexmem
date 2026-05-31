#ifndef WORST_FIT_ALLOCATOR_HPP
#define WORST_FIT_ALLOCATOR_HPP

#include "BaseAllocator.hpp"

class WorstFitAllocator : public BaseAllocator {
public:
    using BaseAllocator::BaseAllocator;
    bool allocate(int id, size_t size) override;
};

#endif