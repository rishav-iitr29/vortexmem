#include "../../include/Cache.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

Cache::Cache(string name, size_t assoc, size_t block_sz, size_t sets_num, EvictionPolicy pol)
    : cache_name(name), associativity(assoc), block_size(block_sz), num_sets(sets_num), policy(pol) {
    
    // Calculate bit widths using log2()
    offset_bits = static_cast<size_t>(log2(block_size));
    index_bits = static_cast<size_t>(log2(num_sets));
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


void Cache::update_policy_on_hit(CacheSet& set, size_t tag) {
    if (policy == EvictionPolicy::LRU) {
        auto it = set.policy_map[tag];
        set.policy_list.splice(set.policy_list.end(), set.policy_list, it);
    }
    // FIFO ignores hits and the original loading queue structure remains unaltered
}

void Cache::update_policy_on_insert(CacheSet& set, size_t tag) {
    // insert new tag into the tracking system
    set.policy_list.push_back(tag);
    set.policy_map[tag] = prev(set.policy_list.end());
}

bool Cache::access(size_t address, bool is_write) {
    size_t tag, index, offset;
    parse_address(address, tag, index, offset);

    CacheSet& set = sets[index];

    // 1. Check for a Cache Hit
    for (size_t i = 0; i < set.lines.size(); ++i) {
        if (set.lines[i].valid && set.lines[i].tag == tag) {
            hit_count++;
            if (is_write) set.lines[i].dirty = true;
            update_policy_on_hit(set, tag);
            return true;
        }
    }

    // 2. Cache Miss - Search for an empty line in the set
    miss_count++;
    for (size_t i = 0; i < set.lines.size(); ++i) {
        if (!set.lines[i].valid) {
            set.lines[i].valid = true;
            set.lines[i].tag = tag;
            set.lines[i].dirty = is_write;
            update_policy_on_insert(set, tag);
            return false;
        }
    }

    // 3. Cache Miss + Set is Full - evict a line based on policy
    size_t victim_tag = set.policy_list.front(); // target oldest entry (FIFO) or least recently used (LRU)
    set.policy_list.pop_front();
    set.policy_map.erase(victim_tag);

    // Find the victim line inside the set array to overwrite it
    for (size_t i = 0; i < set.lines.size(); ++i) {
        if (set.lines[i].valid && set.lines[i].tag == victim_tag) {
            // Overwrite line parameters
            set.lines[i].tag = tag;
            set.lines[i].dirty = is_write;
            update_policy_on_insert(set, tag);
            return false;
        }
    }

    return false;
}

double Cache::get_hit_rate() const {
    size_t total = hit_count + miss_count;
    return (total == 0) ? 0.0 : (static_cast<double>(hit_count) / total) * 100.0;
}

void Cache::print_stats() const {
    cout << left << setw(12) << cache_name 
        << " -> Hits: " << setw(6) << hit_count 
        << " Misses: " << setw(6) << miss_count 
        << " Hit Rate: " << fixed << setprecision(2) << get_hit_rate() << "%\n";
}