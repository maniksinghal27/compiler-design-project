# COMPILER STUDIO
## Compiler Design & Control-Flow Intelligence Suite

**Course**: Compiler Design Lab (`BCSE307P`)  
**Academic Objective**: Design and Development of a Compiler for Structured Control Flow Using Three-Address Code and Backpatching.

---

## 1. Overview & Architecture

**Compiler Studio** is an interactive, fully functional compiler engineering and visualization workbench. It bridges an authentic **Flex & Bison (C)** compiler backend with an advanced **React 18 + TypeScript** desktop-grade interface.

### Highlights
- **100% Genuine Compiler**: The pipeline relies entirely on the Flex lexer (`lexer.l`), Bison parser (`parser.y`), and GCC-compiled native binary. No simulated or faked ASTs.
- **Structured Control-Flow & Backpatching**: Complete syntax-directed translation for `while` loops, `for` loops, and `if/else` conditionals utilizing `makelist()`, `merge()`, and `backpatch()`.
- **4 Canonical IR Representations**: Three-Address Code (TAC), Quadruples, Triples, and Indirect Triples.
- **Interactive Control-Flow Graph (CFG)**: Automatic Basic Block partitioning via the 3 standard leader rules, directed edges (taken, fallthrough, goto), natural loop detection via dominator trees, and interactive Cytoscape.js canvas.
- **Deterministic Optimizer**: Configurable passes for Constant Folding, Constant Propagation, Common Subexpression Elimination (CSE), and Dead Code Elimination (DCE).
- **Reversible Virtual Execution Machine**: Step forward, step backward, run to completion, inspect variable memory store, and automatic step-limit loop protection.
- **Liveness Analysis & Chaitin-Briggs Register Allocation**: Computes variable live ranges, constructs interference graphs, and applies K-register graph-coloring (`K=4`).
- **Comprehensive Test Bench**: 10 automated test suites verifying regression benchmarks, conditionals, loops, optimization passes, and diagnostics.

---

## 2. Quick Start & Launch Instructions

### Prerequisites
- Windows 10/11 with Ubuntu WSL2 (with `gcc`, `flex`, `bison`, `make`)
- Python 3.10+ (with `flask` and `flask-cors`)
- Node.js 18+ and npm

### One-Click Launch (Windows)
Double-click `launch_studio.bat` or run in terminal:
```cmd
launch_studio.bat
```
Then open your web browser at:
```
http://127.0.0.1:5000
```

### Manual Build & Run
1. **Rebuild the Native Compiler**:
   ```bash
   wsl bash -c "cd /mnt/c/Users/asmin/compiler-design-project && make clean && make"
   ```
2. **Build the Frontend**:
   ```bash
   cd frontend
   npm run build
   cd ..
   ```
3. **Start the Studio Backend**:
   ```bash
   python backend/server.py
   ```
4. Access `http://127.0.0.1:5000`.

---

## 3. Presets & Regression Benchmark (Review 1)

The suite contains presets for all mandatory language constructs under the **Source Workspace**:
- `test_nested.c`: Review 1 nested while-loop benchmark.
- `test_if_else.c`: Multi-path branching conditionals.
- `test_for_loops.c`: Structured `for` loops with step expressions.
- `test_infinite.c`: Infinite loop step-limit safeguard test.
- `test_optimizable.c`: Constant folding and CSE test.

### Review 1 Regression Verification:
```c
i = 1;
sum = 0;
while (i <= 3) {
    j = 1;
    while (j <= 2) {
        sum = sum + i;
        print(sum);
        j = j + 1;
    }
    i = i + 1;
}
```
- **Console Outputs**: `1, 2, 4, 6, 9, 12`
- **Final Memory State**: `i = 4, sum = 12, j = 3`
- **Result**: Validated across native CLI, Virtual Machine, and Test Bench.

---

## 4. Workspaces & Navigation Guide

1. **Source Workspace**: Monaco editor with syntax highlighting, line numbers, error markers, presets, and token streams.
2. **Intermediate Reps (IR)**: Tabbed view of TAC with symbolic labels, Quadruple table, Triples table, Indirect Triples pointer table, and interactive step-through Backpatch Inspector.
3. **Control-Flow Graph Studio**: Zoom, pan, fit, reset Cytoscape graph with colored edge semantics (Green = Taken, Red = Fallthrough, Blue = Goto, Dashed Amber = Loop Back-Edge).
4. **Optimization Lab**: Checkbox pass manager with side-by-side original vs optimized TAC comparison and transformation audit logs.
5. **Virtual Execution**: Single-step forward, single-step backward (reversible execution snapshot deltas), live variable store, and runtime console output.
6. **Register & Liveness Studio**: Variable live ranges, register interference matrix, and K-register graph-coloring allocations with spill candidates.
7. **Test Bench**: Automated test runner executing 10 comprehensive regression and boundary tests.
8. **Reference & Viva Guide**: Context-free grammar (BNF), algorithm breakdowns, and viva exam preparation notes.
