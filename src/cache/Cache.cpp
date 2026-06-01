#include "../../include/Cache.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>

Cache::Cache(std::string name, size_t assoc, size_t block_sz, size_t sets_num, EvictionPolicy pol)
    : cache_name(name), associativity(assoc), block_size(block_sz), num_sets(sets_num), policy(pol) {
    
    // Calculate bit widths using log2()
    offset_bits = static_cast<size_t>(std::log2(block_size));
    index_bits = static_cast<size_t>(std::log2(num_sets));
    index_mask = num_sets - 1;

    // initialize internal sets and ways
    sets.resize(num_sets);
    for (auto& set : sets) {
        set.lines.resize(associativity);
    }
}

void Cache::parse_address(size_t address, size_t& tag, size_t& index, size_t& offset) const {
    offset = address & (block_size - 1);
    index = (address >> offset_bits) & index_mask;
    tag = address >> (offset_bits + index_bits);
}