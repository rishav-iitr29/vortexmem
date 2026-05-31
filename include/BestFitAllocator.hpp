#ifndef BEST_FIT_ALLOCATOR_HPP
#define BEST_FIT_ALLOCATOR_HPP

#include "BaseAllocator.hpp"

class BestFitAllocator : public BaseAllocator {
public:
    using BaseAllocator::BaseAllocator;
    bool allocate(int id, size_t size) override;
};

#endif 