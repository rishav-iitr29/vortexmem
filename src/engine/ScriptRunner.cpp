#include "../../include/ScriptRunner.hpp"
#include "../../include/Telemetry.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>

using namespace std;

void ScriptRunner::execute_workload(const string& filename, unique_ptr<BaseAllocator> allocator, mutex& sim_mutex) {
    this->allocator = move(allocator);
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

        {
            // lock the data: The web server cannot read while this block is executing
            lock_guard<mutex> lock(sim_mutex);

            if (command == "ALLOC") {
                int id; size_t size; char comma;
                ss >> id >> comma >> size;
                allocator->allocate(id, size);
            } else if (command == "FREE") {
                int id; ss >> id;
                allocator->deallocate(id);
            } else if (command == "READ" || command == "WRITE") {
                string hex_addr; ss >> hex_addr;
                size_t v_addr = stoull(hex_addr, nullptr, 16);
                bool is_write = (command == "WRITE");

                size_t p_addr = pt.access(v_addr, is_write);
                if (!tlb.access(p_addr, is_write)) {
                    if (!l1.access(p_addr, is_write)) l2.access(p_addr, is_write);
                }
            }

            // Update live telemetry
            current_amat = Telemetry::calculate_amat(l1, l2, pt.get_page_faults());
            current_ext_frag = Telemetry::calculate_external_fragmentation(allocator->get_blocks());
            current_l1_hits = l1.get_hit_rate();
        }

        // unlock and sleep: give the web browser 1.5 seconds to fetch and render the new frame
        this_thread::sleep_for(chrono::milliseconds(1500));
    }
}

nlohmann::json ScriptRunner::get_snapshot(mutex& sim_mutex) const {
    lock_guard<mutex> lock(sim_mutex); // Lock while reading to prevent crashes
    
    nlohmann::json j;
    j["metrics"] = {
        {"amat", current_amat},
        {"fragmentation", current_ext_frag},
        {"l1_hit_rate", current_l1_hits}
    };
    
    j["blocks"] = nlohmann::json::array();
    if (allocator) {
        for (const auto& b : allocator->get_blocks()) {
            j["blocks"].push_back({
                {"start", b.start_address},
                {"size", b.size},
                {"is_free", b.is_free},
                {"id", b.block_id}
            });
        }
    }
    return j;
}