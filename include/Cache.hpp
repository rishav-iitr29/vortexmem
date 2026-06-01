#ifndef CACHE_HPP
#define CACHE_HPP

#include <cstddef>
#include <vector>
#include <list>
#include <unordered_map>
#include <string>

enum class EvictionPolicy { FIFO, LRU };

struct CacheLine {
    bool valid = false;
    bool dirty = false;
    size_t tag = 0;
};

struct CacheSet {
    std::vector<CacheLine> lines;
    
    // data structures for O(1) eviction policies
    std::list<size_t> policy_list; //stores tags //Front is always the eviction target.
    std::unordered_map<size_t, std::list<size_t>::iterator> policy_map; // Fast iterator lookup
};

class Cache {
private:
    std::string cache_name;
    size_t associativity;
    size_t block_size;
    size_t num_sets;
    EvictionPolicy policy;

    size_t offset_bits;
    size_t index_bits;
    size_t index_mask;

    std::vector<CacheSet> sets;

    size_t hit_count = 0;
    size_t miss_count = 0;

    void parse_address(size_t address, size_t& tag, size_t& index, size_t& offset) const;
    void update_policy_on_hit(CacheSet& set, size_t tag);
    void update_policy_on_insert(CacheSet& set, size_t tag);

public:
    Cache(std::string name, size_t assoc, size_t block_sz, size_t sets_num, EvictionPolicy pol);
    
    // return true on hit, false on miss
    bool access(size_t address, bool is_write);
    
    size_t get_hits() const { return hit_count; }
    size_t get_misses() const { return miss_count; }
    double get_hit_rate() const;
    void print_stats() const;
};

#endif