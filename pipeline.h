#include <cstdint>

#ifndef PIPELINE_H
#define PIPELINE_H

enum InstType { INT=1, FP=2, MEM=3, LOAD=4, STORE=5 };

struct instruction {
    uint64_t pc;
    InstType type;
    
    int cycle_if, cycle_id, cycle_ex, cycle_mem, cycle_wb;
    int ex_cycles;
    int mem_cycles;
};

#endif