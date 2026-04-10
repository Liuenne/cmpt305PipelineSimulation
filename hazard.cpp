/*
 * The purpose of this implementation is to keep hazard bookkeeping small and
 * predictable. Structural hazards are tracked with simple per-cycle busy flags,
 * data hazards are handled by remembering when the latest dynamic instance of
 * a PC becomes ready, and control hazards are handled by delaying fetch after
 * a branch resolves. This keeps the checks straightforward for integration
 * into the simulator loop later on.
 */

#include "hazard.h"

void reset_structural_hazards(HazardState& state) {
    state.int_ex_busy = false;
    state.fp_ex_busy = false;
    state.branch_ex_busy = false;
    state.load_mem_busy = false;
    state.store_mem_busy = false;
}

bool can_fetch(const HazardState& state, int cycle) {
    return cycle >= state.fetch_resume_cycle;
}

bool dependencies_ready(const instruction& inst, const HazardState& state, int cycle) {
    for (uint64_t dep_pc : inst.dep_pc) {
        auto it = state.last_ready_cycle.find(dep_pc);
        if (it != state.last_ready_cycle.end() && it->second >= cycle) {
            return false;
        }
    }
    return true;
}

bool can_enter_ex(const instruction& inst, const HazardState& state, int cycle) {
    if (!dependencies_ready(inst, state, cycle)) return false;

    switch (inst.type) {
        case INT: return !state.int_ex_busy;
        case FP: return !state.fp_ex_busy;
        case BRANCH: return !state.branch_ex_busy;
        case LOAD:
        case STORE:
            return true;
        default:
            return false;
    }
}

bool can_enter_mem(const instruction& inst, const HazardState& state, int) {
    switch (inst.type) {
        case LOAD: return !state.load_mem_busy;
        case STORE: return !state.store_mem_busy;
        default: return true;
    }
}

void reserve_ex_unit(const instruction& inst, HazardState& state) {
    switch (inst.type) {
        case INT: state.int_ex_busy = true; break;
        case FP: state.fp_ex_busy = true; break;
        case BRANCH: state.branch_ex_busy = true; break;
        default: break;
    }
}

void reserve_mem_port(const instruction& inst, HazardState& state) {
    switch (inst.type) {
        case LOAD: state.load_mem_busy = true; break;
        case STORE: state.store_mem_busy = true; break;
        default: break;
    }
}

void mark_instruction_ready(const instruction& inst, HazardState& state, int ready_cycle) {
    state.last_ready_cycle[inst.pc] = ready_cycle;
}

void mark_branch_resolved(HazardState& state, int ex_finish_cycle) {
    state.fetch_resume_cycle = ex_finish_cycle + 1;
}
