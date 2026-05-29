#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstddef>

namespace config {
    // Memory Settings
    inline constexpr size_t DEFAULT_MEMORY_SIZE = 1024 * 1024; // 1024 KB
    inline constexpr size_t PAGE_SIZE = 4096;                  // 4 KB Pages

    // Latency Penalty Weights (in Clock Cycles)
    inline constexpr size_t HIT_TIME_L1 = 1;
    inline constexpr size_t HIT_TIME_L2 = 5;
    inline constexpr size_t MISS_PENALTY_RAM = 100;
    inline constexpr size_t PAGE_FAULT_PENALTY = 10000;
}

#endif // CONFIG_HPP