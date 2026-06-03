#include "../../include/BuddyAllocator.hpp"
#include <iostream>
#include <cmath>

// The total size for a Buddy Allocator MUST be a power of 2 (for eg- 1024B)

BuddyAllocator::BuddyAllocator(size_t size) : BaseAllocator(size) {
}

size_t BuddyAllocator::next_power_of_two(size_t size) const {
    if (size == 0) return 1;
    size_t p = 1;
    while (p < size) {
        p <<= 1;
    }
    return p;
}

bool BuddyAllocator::allocate(int id, size_t size) {
    total_allocations++;
    size_t target_size = next_power_of_two(size);
    
    while (true) {
        auto target_block = blocks.end();
        auto split_candidate = blocks.end();

        // Scan the block map for exact matches or candidates to split
        for (auto it = blocks.begin(); it != blocks.end(); ++it) {
            if (it->is_free) {
                if (it->size == target_size) {
                    target_block = it;
                    break; // got perfect fit
                }
                // track the smallest free block still larger than target
                if (it->size > target_size && (split_candidate == blocks.end() || it->size < split_candidate->size)) {
                    split_candidate = it;
                }
            }
        }

        // Case 1: An exact power of 2 block size is ready
        if (target_block != blocks.end()) {
            target_block->is_free = false;
            target_block->block_id = id;
            successful_allocations++;
            return true;
        }

        // Case 2: No exact match, but a larger block can be split in half
        if (split_candidate != blocks.end()) {
            size_t new_size = split_candidate->size / 2;
            
            // Define the right-hand companion buddy block
            MemoryBlock buddy{
                split_candidate->start_address + new_size,
                new_size,
                true,
                -1
            };
            
            // Shrink the left hand block down
            split_candidate->size = new_size;
            
            // Insert the new buddy node right next to it in the list
            blocks.insert(std::next(split_candidate), buddy);
            
            // Loop back around
            continue;
        }

        // Case 3: Completely out of memory bounds
        failed_allocations++;
        return false;
    }
}

bool BuddyAllocator::deallocate(int id) {
    auto target = blocks.end();
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (!it->is_free && it->block_id == id) {
            target = it;
            break;
        }
    }

    if (target == blocks.end()) return false; // ID not found

    // Free the block
    target->is_free = true;
    target->block_id = -1;

    // Recursively merging loop using Bitwise XOR 
    while (true) {
        size_t buddy_address = target->start_address ^ target->size;
        auto buddy = blocks.end();

        for (auto it = blocks.begin(); it != blocks.end(); ++it) {
            if (it->start_address == buddy_address && it->size == target->size && it->is_free) {
                buddy = it;
                break;
            }
        }

        // If a free buddy exists, merge them
        if (buddy != blocks.end()) {
            // Determine which block comes first numerically in memory space
            if (buddy->start_address < target->start_address) {
                buddy->size *= 2;
                blocks.erase(target);
                target = buddy; // Target shifts to the lower address head node
            } else {
                target->size *= 2;
                blocks.erase(buddy);
            }
            // Continue iterating up the tree to see if this larger unified block can merge
            continue;
        }

        break; // No more free buddies to merge
    }
    return true;
}

void BuddyAllocator::debug_dump() const {
    std::cout << "Buddy Layout- ";
    for (const auto& block : blocks) {
        std::cout << (block.is_free ? "[Free: " : "[Alloc ID " + std::to_string(block.block_id) + ": ")
                  << block.size << "B, Addr: " << block.start_address << "] -> ";
    }
    std::cout << "END\n";
}