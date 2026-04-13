#pragma once

#include "stat.h"

#include <string>

struct SimulationResult {
    stats run_stats;
    long long loaded_instructions = 0;
};

bool run_simulation(const std::string& trace_path,
                    long long start_inst,
                    long long inst_count,
                    int depth,
                    SimulationResult& result,
                    std::string& error_message);
