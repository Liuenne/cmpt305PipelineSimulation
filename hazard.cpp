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
    return !state.fetch_blocked && cycle >= state.fetch_resume_cycle;
}

void block_fetch_until_branch_resolves(HazardState& state) {
    state.fetch_blocked = true;
}

bool dependencies_ready(const instruction& inst, const HazardState& state, int cycle) {
    for (long long dep_index : inst.dep_indices) {
        auto it = state.ready_cycle_by_index.find(dep_index);
        if (it == state.ready_cycle_by_index.end() || it->second >= cycle) {
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
    if (inst.dynamic_index >= 0) {
        state.ready_cycle_by_index[inst.dynamic_index] = ready_cycle;
    }
}

void mark_branch_resolved(HazardState& state, int ex_finish_cycle) {
    state.fetch_blocked = false;
    state.fetch_resume_cycle = ex_finish_cycle + 1;
}
