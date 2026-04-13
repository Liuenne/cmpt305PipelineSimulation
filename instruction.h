#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>
#include <vector>

enum InstType { INT=1, FP=2, BRANCH=3, LOAD=4, STORE=5 };

struct instruction {
    uint64_t pc = 0;
    InstType type = INT;
    std::vector<uint64_t> dep_pc;

    long long cycle_if = -1;
    long long cycle_id = -1;
    long long cycle_ex = -1;
    long long cycle_mem = -1;
    long long cycle_wb = -1;

    int ex_cycles = 1;
    int mem_cycles = 1;
};

#endif
