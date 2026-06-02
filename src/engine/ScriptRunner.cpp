#include "../../include/ScriptRunner.hpp"
#include "../../include/Telemetry.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

void ScriptRunner::execute_workload(const string& filename, unique_ptr<BaseAllocator> allocator) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open workload file: " << filename << "\n";
        return;
    }

    // Initialize OS Hardware/Software Pipeline
    Cache tlb("TLB", 4, 64, 1, EvictionPolicy::LRU);
    Cache l1("L1", 1, 32, 4, EvictionPolicy::FIFO);
    Cache l2("L2", 2, 64, 8, EvictionPolicy::LRU);
    PageTable pt(4096, 16384); // 4KB pages, 16KB RAM

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip comments

        stringstream ss(line);
        string command;
        ss >> command;

        if (command == "ALLOC") {
            int id; size_t size;
            char comma;
            ss >> id >> comma >> size;
            allocator->allocate(id, size);
        } 
        else if (command == "FREE") {
            int id;
            ss >> id;
            allocator->deallocate(id);
        } 
        else if (command == "READ" || command == "WRITE") {
            string hex_addr;
            ss >> hex_addr;
            size_t v_addr = stoull(hex_addr, nullptr, 16);
            bool is_write = (command == "WRITE");

            // Push through the pipeline
            size_t p_addr = pt.access(v_addr, is_write);
            if (!tlb.access(p_addr, is_write)) {
                if (!l1.access(p_addr, is_write)) {
                    l2.access(p_addr, is_write);
                }
            }
        }
    }

    // Print Comparative Telemetry
    double amat = Telemetry::calculate_amat(l1, l2, pt.get_page_faults());
    double ext_frag = Telemetry::calculate_external_fragmentation(allocator->get_blocks());

    cout << fixed << setprecision(2);
    cout << "Ext. Frag %: " << setw(6) << ext_frag << "% | "
              << "L1 Hit %: " << setw(6) << l1.get_hit_rate() << "% | "
              << "AMAT: " << amat << " cycles\n";
}