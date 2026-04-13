#include "instruction.h"
#include "stat.h"
#include "depth_config.h"
#include "hazard.h"
#include "trace.h"
#include <iostream>
#include <string>
#include "simulator.h"

// prints how to use the program if arguments are wrong
void print_usage(const char* program) {
    std::cerr << "Usage: " << program
              << " <trace_file> <start_inst> <inst_count> <D>\n";
}

int main(int argc, char* argv[]) {
    // need exactly 4 arguments after the program name
    if (argc != 5) {
        print_usage(argv[0]);
        return 1;
    }

    std::string trace_path = argv[1];

    long long start_inst = 0;
    long long inst_count = 0;
    int depth = 0;

    // convert the command line arguments from strings to numbers
    try {
        start_inst = std::stoll(argv[2]);
        inst_count = std::stoll(argv[3]);
        depth = std::stoi(argv[4]);
    } catch (const std::exception&) {
        print_usage(argv[0]);
        return 1;
    }

    // make sure all the values are in a valid range
    if (start_inst < 1 || inst_count < 1 || depth < 1 || depth > 4) {
        print_usage(argv[0]);
        return 1;
    }

    SimulationResult result;
    std::string error_message;

    if (!run_simulation(trace_path, start_inst, inst_count, depth, result, error_message)) {
        std::cerr << error_message << "\n";
        return 1;
    }

    print_stats(result.run_stats, depth, start_inst, result.loaded_instructions);
    return 0;
}