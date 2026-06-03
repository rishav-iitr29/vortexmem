#ifndef CLI_HPP
#define CLI_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include "../../include/BaseAllocator.hpp"
#include "../../include/FirstFitAllocator.hpp"
#include "../../include/BuddyAllocator.hpp"
#include "../../include/Cache.hpp"
#include "../../include/PageTable.hpp"
#include "../../include/Telemetry.hpp"

using namespace std;

class CLI {
private:
    unique_ptr<BaseAllocator> allocator;
    unique_ptr<Cache> tlb;
    unique_ptr<Cache> l1;
    unique_ptr<Cache> l2;
    unique_ptr<PageTable> pt;

    vector<string> split_command(const string& str) {
        vector<string> tokens;
        stringstream ss(str);
        string token;
        while (ss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }

    void print_help() {
        cout << "\n--- VortexMem CLI Commands ---\n";
        cout << "init memory <size> <first|buddy> - Initialize physical memory\n";
        cout << "init pipeline                    - Boot Cache & Virtual Paging\n";
        cout << "malloc <id> <size>               - Allocate memory\n";
        cout << "free <id>                        - Free memory\n";
        cout << "read <hex_addr>                  - Read memory (triggers caches)\n";
        cout << "write <hex_addr>                 - Write memory (triggers caches)\n";
        cout << "dump                             - Show memory layout\n";
        cout << "stats                            - Show advanced metrics & AMAT\n";
        cout << "exit                             - Close simulator\n";
    }

public:
    void run() {
        cout << " VortexMem Command Line Interface\n";
        cout << " Type 'help' for commands.\n";

        string input;
        while (true) {
            cout << "vortex> ";
            getline(cin, input);
            
            if (input.empty()) continue;
            
            vector<string> args = split_command(input);
            string cmd = args[0];

            if (cmd == "exit") {
                cout << "Shutting down VortexMem...\n";
                break;
            } 
            else if (cmd == "help") {
                print_help();
            }
            else if (cmd == "init" && args.size() >= 4 && args[1] == "memory") {
                size_t size = stoull(args[2]);
                string type = args[3];
                if (type == "first") allocator = make_unique<FirstFitAllocator>(size);
                else if (type == "buddy") allocator = make_unique<BuddyAllocator>(size);
                cout << "System RAM initialized: " << size << " Bytes (" << type << "-fit)\n";
            }
            else if (cmd == "init" && args.size() >= 2 && args[1] == "pipeline") {
                tlb = make_unique<Cache>("TLB", 4, 64, 1, EvictionPolicy::LRU);
                l1 = make_unique<Cache>("L1", 1, 32, 4, EvictionPolicy::FIFO);
                l2 = make_unique<Cache>("L2", 2, 64, 8, EvictionPolicy::LRU);
                pt = make_unique<PageTable>(4096, 16384);
                cout << "Hardware Pipeline (TLB, L1, L2, Paging) booted.\n";
            }
            else if (cmd == "malloc" && args.size() >= 3) {
                if (!allocator) { cout << "Error: Run 'init memory' first.\n"; continue; }
                int id = stoi(args[1]);
                size_t size = stoull(args[2]);
                if (!allocator->allocate(id, size)) cout << "Allocation Failed: Out of Memory!\n";
            }
            else if (cmd == "free" && args.size() >= 2) {
                if (!allocator) { cout << "Error: Run 'init memory' first.\n"; continue; }
                int id = stoi(args[1]);
                if (!allocator->deallocate(id)) cout << "Free Failed: ID not found.\n";
                else cout << "Block ID " << id << " freed.\n";
            }
            else if ((cmd == "read" || cmd == "write") && args.size() >= 2) {
                if (!pt || !l1) { cout << "Error: Run 'init pipeline' first.\n"; continue; }
                size_t v_addr = stoull(args[1], nullptr, 16);
                bool is_write = (cmd == "write");

                size_t p_addr = pt->access(v_addr, is_write);
                if (!tlb->access(p_addr, is_write)) {
                    if (!l1->access(p_addr, is_write)) {
                        l2->access(p_addr, is_write);
                    }
                }
                cout << "Memory operation complete.\n";
            }
            else if (cmd == "dump") {
                if (allocator) allocator->debug_dump();
            }
            else if (cmd == "stats") {
                if (allocator) allocator->print_advanced_stats();
                if (l1 && pt) {
                    cout << "--- Hardware Pipeline Metrics ---\n";
                    cout << "System AMAT: " << Telemetry::calculate_amat(*l1, *l2, pt->get_page_faults()) << " cycles\n";
                    pt->print_stats();
                    tlb->print_stats();
                    l1->print_stats();
                    l2->print_stats();
                    cout << "\n";
                }
            }
            else {
                cout << "Unknown command. Type 'help'.\n";
            }
        }
    }
};

#endif