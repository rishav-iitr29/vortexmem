#ifndef TELEMETRY_HPP
#define TELEMETRY_HPP

#include <cstddef>
#include <vector>
#include "../include/Config.hpp"
#include "../include/Cache.hpp"
#include "../include/BaseAllocator.hpp"

using namespace std;

class Telemetry {
public:
    // Calculates total cycles and average access time based on cache miss rates
    static double calculate_amat(const Cache& l1, const Cache& l2, size_t page_faults) {
        double l1_miss_rate = l1.get_misses() / max(1.0, static_cast<double>(l1.get_hits() + l1.get_misses()));
        double l2_miss_rate = l2.get_misses() / max(1.0, static_cast<double>(l2.get_hits() + l2.get_misses()));

        // The core AMAT equation
        double amat = config::HIT_TIME_L1 + l1_miss_rate * (config::HIT_TIME_L2 + l2_miss_rate * config::MISS_PENALTY_RAM);
        
        // Add extreme penalty for disk swap thrashing (Page Faults averaged over total accesses)
        size_t total_accesses = l1.get_hits() + l1.get_misses();
        if (total_accesses > 0) {
            double pf_rate = static_cast<double>(page_faults) / total_accesses;
            amat += (pf_rate * config::PAGE_FAULT_PENALTY);
        }
        
        return amat;
    }

    // Calculates External Fragmentation % (Free space divided into multiple small blocks)
    static double calculate_external_fragmentation(const list<MemoryBlock>& blocks) {
        size_t total_free = 0;
        size_t largest_free = 0;

        for (const auto& block : blocks) {
            if (block.is_free) {
                total_free += block.size;
                if (block.size > largest_free) {
                    largest_free = block.size;
                }
            }
        }

        if (total_free == 0) return 0.0;
        
        // External fragmentation = (Total Free - Largest Free Block) / Total Free
        return ((static_cast<double>(total_free - largest_free)) / total_free) * 100.0;
    }
};

#endif