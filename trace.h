/*
 * This file exists to keep trace parsing separate from the simulator loop.
 * The idea was to give the rest of the project one small, reliable interface
 * for reading instructions from the input file without mixing file-handling
 * logic into the pipeline code. That way Member 1 can focus on cycle-by-cycle
 * movement through the pipeline, while this module is responsible for turning
 * each line of the trace into a usable instruction object.
 */

#pragma once

#include "instruction.h"
#include <fstream>
#include <string>

bool parse_trace_line(const std::string& line, instruction& inst);

class TraceReader {
public:
    explicit TraceReader(const std::string& path);

    bool is_open() const;
    bool skip_instructions(long long count);
    bool read_next(instruction& inst);

private:
    std::ifstream file_;
};
