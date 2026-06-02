#include <iostream>
#include <thread>
#include <mutex>
#include "../include/ScriptRunner.hpp"
#include "../include/BuddyAllocator.hpp"
#include "../third_party/httplib.h"

// Global synchronization
std::mutex sim_mutex;
ScriptRunner runner;

void start_web_server() {
    httplib::Server svr;

    // Serve the static frontend files from the frontend directory
    svr.set_mount_point("/", "../frontend");

    // Serve the live memory state JSON
    svr.Get("/api/snapshot", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content(runner.get_snapshot(sim_mutex).dump(), "application/json");
    });

    std::cout << "Dashboard Live at: http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./vortexmem <workload_file.txt>\n";
        return 1;
    }
    std::string workload_file = argv[1];

    // Launch the web server on a background thread
    std::thread server_thread(start_web_server);
    
    // Give the server a second to boot up
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Starting Simulation in 3 seconds...\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Run the Buddy System simulation on the main thread
    std::cout << "[Executing Buddy System Workload]\n";
    runner.execute_workload(workload_file, std::make_unique<BuddyAllocator>(1024), sim_mutex);

    std::cout << "Simulation Complete.\n";
    server_thread.join();
    return 0;
}