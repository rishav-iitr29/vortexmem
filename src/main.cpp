#include <iostream>
#include "../include/Cache.hpp"

using namespace std;

int main() {
    cout << " Testing Strategy: CACHE HIERARCHY\n";

    // Setup Hierarchy Layers
    Cache tlb("TLB (Fully)", 4, 64, 1, EvictionPolicy::LRU);      // 4 way fully associative
    Cache l1("L1 Cache",     1, 32, 4, EvictionPolicy::FIFO);     // Direct Mapped (1), 4 sets
    Cache l2("L2 Cache",     2, 64, 8, EvictionPolicy::LRU);      // 2 Way Set Associative, 8 sets

    // Address Stream Trace Array
    vector<size_t> addresses = {0x1000, 0x1004, 0x1000, 0x2000, 0x1000};

    cout << "Streaming Address Accesses through Cascade Pipeline:\n\n";
    for (size_t addr : addresses) {
        cout << "Accessing Address: 0x" << hex << addr << dec << "\n";
        
        // Cascade lookup flow
        if (!tlb.access(addr, false)) {
            if (!l1.access(addr, false)) {
                l2.access(addr, false);
            }
        }
    }

    cout << "\n Final Simulation Telemetry Report \n";
    tlb.print_stats();
    l1.print_stats();
    l2.print_stats();

    return 0;
}