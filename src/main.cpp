#include <iostream>
#include <iomanip>
#include "../include/PageTable.hpp"

int main() {
    std::cout << "Testing Strategy: VIRTUAL PAGING & CLOCK\n";

    // Setup: 4KB Pages, but only 16KB of Physical RAM (Exactly 4 Frames available)
    size_t PAGE_SIZE = 4096;
    size_t PHYS_MEM = 16384; 
    
    PageTable pt(PAGE_SIZE, PHYS_MEM);

    std::cout << "--- 1. Bootstrapping & Filling Physical Memory ---\n";
    // We will access 4 distinct Virtual Pages. This will fill all 4 physical frames.
    pt.access(1 * PAGE_SIZE, false); // VPN 1 (Read)
    pt.access(2 * PAGE_SIZE, true);  // VPN 2 (Write - Marks as Dirty)
    pt.access(3 * PAGE_SIZE, false); // VPN 3 (Read)
    pt.access(4 * PAGE_SIZE, false); // VPN 4 (Read)
    
    std::cout << "RAM is now 100% full (4/4 frames occupied).\n";

    std::cout << "\n--- 2. Setting up the Second Chance ---\n";
    std::cout << "Accessing VPN 2 again to refresh its 'Referenced' bit...\n";
    pt.access(2 * PAGE_SIZE, false); // A temporal hit

    std::cout << "\n--- 3. Forcing a Clock Eviction ---\n";
    std::cout << "Accessing VPN 5 (Address 0x5000)... This will trigger a Page Fault\n";
    // Clock hand starts at Frame 0 (VPN 1). VPN 1 is referenced, so it gets cleared.
    // Clock hand moves to Frame 1 (VPN 2). VPN 2 is referenced, so it gets cleared.
    // Clock hand moves to Frame 2 (VPN 3). VPN 3 is referenced, so it gets cleared.
    // Clock hand moves to Frame 3 (VPN 4). VPN 4 is referenced, so it gets cleared.
    // Clock hand loops to Frame 0 (VPN 1). VPN 1 is now not referenced.
    pt.access(5 * PAGE_SIZE, false); 

    std::cout << "\n--- Final Paging Telemetry Report ---\n";
    pt.print_stats();

    return 0;
}