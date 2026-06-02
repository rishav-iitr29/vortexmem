#include <iostream>
#include <memory>
#include "../include/ScriptRunner.hpp"
#include "../include/FirstFitAllocator.hpp"
#include "../include/BuddyAllocator.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./vortexmem <workload_file.txt>\n";
        return 1;
    }
    
    string workload_file = argv[1];

    cout << "--- Comparative Analysis Report ---\n";
    cout << "Workload: " << workload_file << "\n\n";

    cout << "[Strategy: First-Fit]\n";
    ScriptRunner runner1;
    runner1.execute_workload(workload_file, make_unique<FirstFitAllocator>(1048576)); // 1MB

    cout << "\n[Strategy: Buddy System]\n";
    ScriptRunner runner2;
    runner2.execute_workload(workload_file, make_unique<BuddyAllocator>(1048576)); // 1MB

    return 0;
}