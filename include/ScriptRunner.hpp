#ifndef SCRIPT_RUNNER_HPP
#define SCRIPT_RUNNER_HPP

#include <string>
#include <memory>
#include <mutex>
#include "BaseAllocator.hpp"
#include "Cache.hpp"
#include "PageTable.hpp"
#include "../third_party/json.hpp"

using namespace std;

class ScriptRunner {
public:
    unique_ptr<BaseAllocator> allocator;
    double current_amat = 0.0;
    double current_ext_frag = 0.0;
    size_t current_l1_hits = 0;

    ScriptRunner() = default;
    ~ScriptRunner() = default;

    void execute_workload(const string& filename, unique_ptr<BaseAllocator> allocator, mutex& sim_mutex);

    nlohmann::json get_snapshot(mutex& sim_mutex) const;
};

#endif