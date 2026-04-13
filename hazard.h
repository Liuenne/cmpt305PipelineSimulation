/*
 * This file groups the hazard-related checks into one place so the simulator
 * can ask simple questions like "can this instruction move forward now?"
 * without hard-coding all of the hazard rules into the main loop. The goal is
 * to keep the cycle logic readable while still covering the three hazard types
 * required by the assignment: structural, data, and control hazards.
 */

#pragma once

#include "instruction.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

struct HazardState {
    bool int_ex_busy = false;
    bool fp_ex_busy = false;
    bool branch_ex_busy = false;
    bool load_mem_busy = false;
    bool store_mem_busy = false;

    bool fetch_blocked = false;
    int fetch_resume_cycle = 0;

    // Maps a dynamic instruction index to the cycle when it became ready.
    std::unordered_map<long long, int> last_ready_cycle;
};

void reset_structural_hazards(HazardState& state);

bool can_fetch(const HazardState& state, int cycle);

void block_fetch_until_branch_resolves(HazardState& state);

bool dependencies_ready(const instruction& inst, const HazardState& state, int cycle);

bool can_enter_ex(const instruction& inst, const HazardState& state, int cycle);

bool can_enter_mem(const instruction& inst, const HazardState& state, int cycle);

void reserve_ex_unit(const instruction& inst, HazardState& state);

void reserve_mem_port(const instruction& inst, HazardState& state);

void mark_instruction_ready(const instruction& inst, HazardState& state, int ready_cycle);

void mark_branch_resolved(HazardState& state, int ex_finish_cycle);
