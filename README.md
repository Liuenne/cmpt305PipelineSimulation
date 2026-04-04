# CMPT 305 Pipeline Simulation

A pipeline simulator for a 2-wide superscalar in-order processor. Simulates a 5-stage pipeline (IF, ID, EX, MEM, WB) with four pipeline depth configurations.

# How to Compile
```
make proj
```

---
## How to run
```
./proj <trace_file><start_inst><inst_count><D>
```

| Argument | Description |
|----------|-------------|
| `trace_file` | Path to the decompressed trace file |
| `start_inst` | Instruction number to start simulation from (1-indexed) |
| `inst_count` | Number of instructions to simulate |
| `D` | Pipeline depth configuration (1, 2, 3, or 4) |

Example:
```
./proj srv_0 10000000 1000000 2
```