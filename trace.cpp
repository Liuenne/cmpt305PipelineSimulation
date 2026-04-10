/*
 * The main goal of this implementation is to read the trace in the simplest
 * possible way while still matching the assignment format exactly. Each line
 * is split into the instruction PC, instruction type, and any dependency PCs.
 * Keeping this logic here makes it easier to test parsing on its own and helps
 * the simulator avoid doing string work inside the main cycle loop.
 */

#include "trace.h"

#include <sstream>
#include <string>
#include <vector>

bool parse_trace_line(const std::string& line, instruction& inst) {
    std::stringstream ss(line);
    std::string token;

    if (!std::getline(ss, token, ',')) return false;
    inst.pc = std::stoull(token, nullptr, 16);

    if (!std::getline(ss, token, ',')) return false;
    inst.type = static_cast<InstType>(std::stoi(token));

    inst.dep_pc.clear();
    while (std::getline(ss, token, ',')) {
        if (!token.empty()) {
            inst.dep_pc.push_back(std::stoull(token, nullptr, 16));
        }
    }

    return true;
}

TraceReader::TraceReader(const std::string& path) : file_(path) {}

bool TraceReader::is_open() const {
    return file_.is_open();
}

bool TraceReader::skip_instructions(long long count) {
    std::string line;
    for (long long i = 0; i < count; ++i) {
        if (!std::getline(file_, line)) return false;
    }
    return true;
}

bool TraceReader::read_next(instruction& inst) {
    std::string line;
    if (!std::getline(file_, line)) return false;
    return parse_trace_line(line, inst);
}
