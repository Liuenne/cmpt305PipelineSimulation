#pragma once

#ifndef STAT_H
#define STAT_H

#include "instruction.h"
#include "depth_config.h"
#include <iostream>
#include <iomanip>

struct stats {
    long long total_cycles = 0;
    double exec_time = 0.0; //ms
    int count[6] = {0}; //count of each instruction type
};

inline void record_inst(const instruction& inst, stats& s) {
    s.count[inst.type]++;

    if (inst.cycle_wb + 1 > s.total_cycles) {
        s.total_cycles = inst.cycle_wb + 1;
    }
}

inline void finalize_stats(stats& s, int D) {
    double freq = get_freq(D);
    double cycle_time_seconds = 1.0 / (freq * 1e9);
    s.exec_time = s.total_cycles * cycle_time_seconds * 1e3; // ms
}

inline void print_stats(const stats& s, int D, long long start_inst, long long inst_count) {
    long long total = 0;
    for (int i = 1; i <= 5; i++) total += s.count[i];

    std::cout << "========================================\n";
    std::cout << "D = " << D
              << " start = " << start_inst
              << " count = " << inst_count << "\n";
    std::cout << "Total Cycles: " << s.total_cycles << "\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Exec Time (ms): " << s.exec_time << "\n";
    std::cout << "----------------------------------------\n";

    if (total == 0) {
        std::cout << "No instructions executed." << std::endl;
        return;
    }

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "INT: " << s.count[INT]    << "(" << 100.0 * s.count[INT]    / total << "%)\n";
    std::cout << "FP: " << s.count[FP]     << "(" << 100.0 * s.count[FP]     / total << "%)\n";
    std::cout << "BRANCH: " << s.count[BRANCH] << "(" << 100.0 * s.count[BRANCH] / total << "%)\n";
    std::cout << "LOAD: " << s.count[LOAD]   << "(" << 100.0 * s.count[LOAD]   / total << "%)\n";
    std::cout << "STORE: " << s.count[STORE]  << "(" << 100.0 * s.count[STORE]  / total << "%)\n";
    std::cout << "========================================\n";
}
#endif
