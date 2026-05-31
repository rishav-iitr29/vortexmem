#ifndef FIRST_FIT_ALLOCATOR_HPP
#define FIRST_FIT_ALLOCATOR_HPP

#include "BaseAllocator.hpp"

class FirstFitAllocator : public BaseAllocator {
public:
    using BaseAllocator::BaseAllocator;
    bool allocate(int id, size_t size) override;
};

#endif