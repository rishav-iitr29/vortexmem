#include <iostream>
#include <memory>
#include "../include/BaseAllocator.hpp"
#include "../include/FirstFitAllocator.hpp"
#include "../include/BestFitAllocator.hpp"
#include "../include/WorstFitAllocator.hpp"

using namespace std;

void run_test(unique_ptr<BaseAllocator> alloc, const string& type) {
    cout << "\n--- Testing " << type << " ---\n";
    alloc->allocate(1, 200);
    alloc->allocate(2, 100);
    alloc->allocate(3, 200);
    alloc->deallocate(2); // Leave an isolated 100B hole in the middle
    alloc->debug_dump();

    cout << "Allocating 50B to ID 4:\n";
    alloc->allocate(4, 50);
    alloc->debug_dump();

    cout << "Clean up all remaining blocks:\n";
    alloc->deallocate(1);
    alloc->deallocate(3);
    alloc->deallocate(4);
    alloc->debug_dump();
}

int main() {
    run_test(make_unique<FirstFitAllocator>(1024), "FIRST-FIT");
    run_test(make_unique<BestFitAllocator>(1024), "BEST-FIT");
    run_test(make_unique<WorstFitAllocator>(1024), "WORST-FIT");
    return 0;
}