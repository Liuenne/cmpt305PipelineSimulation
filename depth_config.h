#pragma once
#include "instruction.h"

inline double get_freq(int D) {
     if (D == 1) return 1.0;
    if (D == 2) return 1.2;
    if (D == 3) return 1.7;
    if (D == 4) return 1.8;
    return 1.0; // default case
}

// sets how many cycles each stage takes based on instruction type and depth
inline void depth_config(instruction& inst, int D) {
    // start with default of 1 cycle for each stage
    inst.ex_cycles = 1;
    inst.mem_cycles = 1;
 
    // FP instructions take 2 execute cycles at depth 2 or 4
    if (inst.type == FP && (D == 2 || D == 4)) {
        inst.ex_cycles = 2;
    }
 
    // LOAD instructions take 3 memory cycles at depth 3 or 4
    if (inst.type == LOAD && (D == 3 || D == 4)) {
        inst.mem_cycles = 3;
    }
}
