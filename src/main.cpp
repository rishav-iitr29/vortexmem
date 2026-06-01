#include <iostream>
#include "../include/BuddyAllocator.hpp"

int main() {
    std::cout << " Testing Strategy: BUDDY SYSTEM\n";
    
    BuddyAllocator heap(1024); // Must be a power of two base
    heap.debug_dump();

    std::cout << "\n Step 1: Requesting Odd Sizes (Triggers Power-of-2 Padding) \n";
    std::cout << "Allocating 70B to ID 1 (should snap to 128B)..." << std::endl;
    heap.allocate(1, 70);
    
    std::cout << "Allocating 200B to ID 2 (should snap to 256B)..." << std::endl;
    heap.allocate(2, 200);
    heap.debug_dump();

    std::cout << "\n Step 2: Isolating Buddy Segments \n";
    std::cout << "Allocating 128B to ID 3..." << std::endl;
    heap.allocate(3, 128);
    heap.debug_dump();

    std::cout << "\n Step 3: Sequential Deallocations (Cascading Merges) \n";
    std::cout << "Freeing ID 1..." << std::endl;
    heap.deallocate(1);
    heap.debug_dump();

    std::cout << "Freeing ID 3 (Should cascade merge with ID 1's remnants)..." << std::endl;
    heap.deallocate(3);
    heap.debug_dump();

    std::cout << "Freeing ID 2 (Full structural collapse back to 1024B)..." << std::endl;
    heap.deallocate(2);
    heap.debug_dump();

    return 0;
}