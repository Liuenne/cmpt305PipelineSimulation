#pragma once
#include "instruction.h"

double get_freq(int D) {
    switch(D) {
        case 1: return 2.0;
        case 2: return 1.5;
        case 3: return 1.0;
        default: return 1.0;
    }
}

void depth_config(instruction& inst, int D) {
    inst.ex_cycles = 1;
    inst.mem_cycles = 1;
    if (inst.type == FP && (D == 2 || D == 4)) {
        inst.ex_cycles = 2;
    }

    if (inst.type == LOAD && (D == 3 || D == 4)) {
        inst.mem_cycles = 3;
    }
}