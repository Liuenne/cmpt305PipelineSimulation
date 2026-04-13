#include "instruction.h"
#include "stat.h"
#include "depth_config.h"
#include "hazard.h"
#include "trace.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <trace_file> \n";
        return 1;
    }

    std::string trace_file = argv[1];
    long long start_inst = std::stoll(argv[2]);
    long long insts = std::stoll(argv[3]);
    int depth = std::stoi(argv[4]);

    TraceReader reader(trace_file);
    if (!reader.is_open()) {
        std::cerr << "Failed to open trace file: " << trace_file << std::endl;
        return 1;
    }

    if (!reader.skip_instructions(start_inst - 1)) {
        std::cerr << "Failed to skip instructions: " << start_inst << std::endl;
        return 1;
    }

    HazardState hazard;
    stats s;

    long long prev_EX_end = -1;
    long long prev_MEM_end = -1;
    long long fetch_slot[2] = {0}; //next fetch cycle for each slot
    long long fetch_index = 0;

    long long simulated = 0;
    instruction inst;

    while (simulated < insts && reader.read_next(inst)) {
        depth_config(inst, depth);

        long long slot = fetch_index & 2;
        fetch_index++;
        
    }
}