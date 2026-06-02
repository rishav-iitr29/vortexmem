#ifndef SCRIPT_RUNNER_HPP
#define SCRIPT_RUNNER_HPP

#include <string>
#include <memory>
#include "BaseAllocator.hpp"
#include "Cache.hpp"
#include "PageTable.hpp"

class ScriptRunner {
public:
    ScriptRunner() = default;
    ~ScriptRunner() = default;

    void execute_workload(const std::string& filename, std::unique_ptr<BaseAllocator> allocator);
};

#endif