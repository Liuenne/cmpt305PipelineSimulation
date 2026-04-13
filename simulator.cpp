#include "simulator.h"
#include "depth_config.h"
#include "hazard.h"
#include "trace.h"

#include <cstdlib>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace {

// pipeline stages in order
enum class Stage {
    IF_STAGE,
    ID_STAGE,
    EX_STAGE,
    MEM_STAGE,
    WB_STAGE
};

// represents an instruction currently in the pipeline
struct ActiveInstruction {
    instruction inst;
    Stage stage = Stage::IF_STAGE;
    int remaining_cycles = 1;
};

// returns true if debug mode is turned on
bool debug_enabled() {
    const char* d = std::getenv("PIPELINE_DEBUG");
    return d && d[0] != '\0' && d[0] != '0';
}

// counts how many instructions are in a specific stage
int count_stage(const std::deque<ActiveInstruction>& pipe, Stage s) {
    int count = 0;
    for (int i = 0; i < (int)pipe.size(); i++) {
        if (pipe[i].stage == s) {
            count++;
        }
    }
    return count;
}

// adds up all retired instructions across all types
long long retired_count(const stats& st) {
    return st.count[INT] + st.count[FP] + st.count[BRANCH] +
           st.count[LOAD] + st.count[STORE];
}

// reads instructions from the trace file into a vector
bool load_trace_window(const std::string& path,
                       long long start,
                       long long total,
                       int depth,
                       std::vector<instruction>& insts,
                       std::string& err) {

    TraceReader reader(path);

    if (!reader.is_open()) {
        err = "Cannot open file";
        return false;
    }

    if (!reader.skip_instructions(start - 1)) {
        err = "Skip failed";
        return false;
    }

    insts.clear();

    instruction inst;

    while ((long long)insts.size() < total && reader.read_next(inst)) {
        depth_config(inst, depth);
        insts.push_back(inst);
    }

    if ((long long)insts.size() != total) {
        err = "Trace ended early";
        return false;
    }

    return true;
}

// tick down remaining cycles for all instructions in the pipeline
void decrement_cycles(std::deque<ActiveInstruction>& pipe) {
    for (int i = 0; i < (int)pipe.size(); i++) {
        if (pipe[i].remaining_cycles > 0) {
            pipe[i].remaining_cycles--;
        }
    }
}

// remove finished instructions from the front of the pipeline
void retire(std::deque<ActiveInstruction>& pipe, stats& st) {
    int retired = 0;

    while (!pipe.empty() &&
           retired < 2 &&
           pipe.front().stage == Stage::WB_STAGE &&
           pipe.front().remaining_cycles == 0) {

        record_inst(pipe.front().inst, st);
        pipe.pop_front();
        retired++;
    }
}

// move instructions from EX to MEM and MEM to WB
void advance_ex_mem(std::deque<ActiveInstruction>& pipe,
                    HazardState& hz,
                    int cycle) {

    bool ex_block = false;
    bool mem_block = false;

    for (int i = 0; i < (int)pipe.size(); i++) {

        // MEM -> WB
        if (pipe[i].stage == Stage::MEM_STAGE) {

            if (pipe[i].remaining_cycles == 0 && !mem_block) {

                if (pipe[i].inst.type == LOAD || pipe[i].inst.type == STORE) {
                    mark_instruction_ready(pipe[i].inst, hz, cycle - 1);
                }

                pipe[i].stage = Stage::WB_STAGE;
                pipe[i].inst.cycle_wb = cycle;
                pipe[i].remaining_cycles = 1;

            } else {
                if (pipe[i].inst.type == LOAD || pipe[i].inst.type == STORE) {
                    reserve_mem_port(pipe[i].inst, hz);
                }
                mem_block = true;
            }
        }

        // EX -> MEM
        else if (pipe[i].stage == Stage::EX_STAGE) {

            if (pipe[i].remaining_cycles == 0 &&
                !ex_block &&
                !mem_block &&
                can_enter_mem(pipe[i].inst, hz, cycle)) {

                if (pipe[i].inst.type == INT || pipe[i].inst.type == FP || pipe[i].inst.type == BRANCH) {
                    mark_instruction_ready(pipe[i].inst, hz, cycle - 1);
                }

                if (pipe[i].inst.type == BRANCH) {
                    mark_branch_resolved(hz, cycle - 1);
                }

                pipe[i].stage = Stage::MEM_STAGE;
                pipe[i].inst.cycle_mem = cycle;
                pipe[i].remaining_cycles = pipe[i].inst.mem_cycles;

                reserve_mem_port(pipe[i].inst, hz);

            } else {
                ex_block = true;
            }
        }
    }
}

// move instructions from ID to EX
void advance_id(std::deque<ActiveInstruction>& pipe,
                HazardState& hz,
                int cycle) {

    bool blocked = false;

    for (int i = 0; i < (int)pipe.size(); i++) {

        if (pipe[i].stage == Stage::ID_STAGE) {

            if (pipe[i].remaining_cycles == 0 &&
                !blocked &&
                can_enter_ex(pipe[i].inst, hz, cycle)) {

                pipe[i].stage = Stage::EX_STAGE;
                pipe[i].inst.cycle_ex = cycle;
                pipe[i].remaining_cycles = pipe[i].inst.ex_cycles;

                reserve_ex_unit(pipe[i].inst, hz);

            } else {
                blocked = true;
            }
        }
    }
}

// move instructions from IF to ID
void advance_if(std::deque<ActiveInstruction>& pipe, int cycle) {

    int moved = 0;
    bool blocked = false;

    for (int i = 0; i < (int)pipe.size(); i++) {

        if (pipe[i].stage == Stage::IF_STAGE) {

            if (pipe[i].remaining_cycles == 0 && !blocked && moved < 2) {

                pipe[i].stage = Stage::ID_STAGE;
                pipe[i].inst.cycle_id = cycle;
                pipe[i].remaining_cycles = 1;

                moved++;

            } else {
                blocked = true;
            }
        }
    }
}

// fetch new instructions into the pipeline
void fetch(std::deque<ActiveInstruction>& pipe,
           const std::vector<instruction>& insts,
           size_t& next,
           HazardState& hz,
           int cycle) {

    if (!can_fetch(hz, cycle)) return;

    int slots = 2 - count_stage(pipe, Stage::IF_STAGE);

    while (slots > 0 && next < insts.size()) {

        ActiveInstruction a;
        a.inst = insts[next];
        a.stage = Stage::IF_STAGE;
        a.inst.cycle_if = cycle;
        a.remaining_cycles = 1;

        pipe.push_back(a);

        next++;
        slots--;

        // stop fetching after a branch until it resolves
        if (a.inst.type == BRANCH) {
            block_fetch_until_branch_resolves(hz);
            break;
        }
    }
}

} // namespace

// main simulation loop
bool run_simulation(const std::string& path,
                    long long start,
                    long long total,
                    int depth,
                    SimulationResult& result,
                    std::string& err) {

    std::vector<instruction> insts;

    if (!load_trace_window(path, start, total, depth, insts, err)) {
        return false;
    }

    result = {};
    result.loaded_instructions = insts.size();

    std::deque<ActiveInstruction> pipe;
    HazardState hz;

    size_t next = 0;
    int cycle = 0;

    // keep running until all instructions have retired
    while (retired_count(result.run_stats) < result.loaded_instructions) {

        retire(pipe, result.run_stats);
        reset_structural_hazards(hz);

        advance_ex_mem(pipe, hz, cycle);
        advance_id(pipe, hz, cycle);
        advance_if(pipe, cycle);

        fetch(pipe, insts, next, hz, cycle);

        decrement_cycles(pipe);

        cycle++;
    }

    finalize_stats(result.run_stats, depth);
    return true;
}