#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>
#include <vector>

enum InstType { INT=1, FP=2, BRANCH=3, LOAD=4, STORE=5 };

struct instruction {
    uint64_t pc;
    InstType type;
    std::vector<uint64_t> dep_pc; //dependant instructions
    
    long long cycle_if, cycle_id, cycle_ex, cycle_mem, cycle_wb;
    int ex_cycles =1; //cycles spent in EX
    int mem_cycles =1; //cycles spent in MEM
};

#endif
