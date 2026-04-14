# CMPT 305 Pipeline Simulation

A pipeline simulator for a 2-wide superscalar in-order processor. Simulates a 5-stage pipeline (IF, ID, EX, MEM, WB) with four pipeline depth configurations.

# How to Compile
```
make proj
```

---
## How to run
```
./proj <trace_file> <start_inst> <inst_count> <D>
```

| Argument | Description |
|----------|-------------|
| `trace_file` | Path to the decompressed trace file |
| `start_inst` | Instruction number to start simulation from (1-indexed) |
| `inst_count` | Number of instructions to simulate |
| `D` | Pipeline depth configuration (1, 2, 3, or 4) |

Example:
```
./proj traces/srv_0 10000000 1000000 2
```

## Batch Experiments
To help with the 72 required runs, you can use:
```bash
./run_experiments.sh
```

This writes a tab-separated summary into the `results/` directory. By default it
uses the three provided traces, the six required starting points, and
`inst_count=1000000`. You can override the trace directory if needed:
```bash
TRACE_DIR=/path/to/traces ./run_experiments.sh
```
