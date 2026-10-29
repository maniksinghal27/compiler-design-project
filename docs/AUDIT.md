# Compiler Studio - Baseline Audit Report

## 1. Environment & Tooling Verification
- **Host OS**: Windows 11 with Ubuntu WSL2 (`/usr/bin/gcc`, `/usr/bin/flex`, `/usr/bin/bison`, `/usr/bin/make`, `/usr/bin/python3`).
- **WinFlexBison**: Available at `C:\winflexbison\win_bison.exe` and `flex.exe`.
- **Node.js**: v24.13.0, npm 11.6.2 (Windows native).
- **Python**: Windows Python 3.14 + Flask 3.1.3 + flask-cors installed.
- **Build Reproducibility**: Tested `make clean && make` via GCC/Flex/Bison in WSL. Compiler builds cleanly with zero warnings/errors.
- **Review 1 Baseline Test**:
  - Input: nested while loop (`input.txt`).
  - Output: `1, 2, 4, 6, 9, 12`.
  - Memory: `i = 4, sum = 12, j = 3`.
  - Backpatch log and Quadruple table generated and validated.

## 2. Identified Bugs & Architecture Limitations in Baseline C Compiler
1. **Unsafe Fixed Array Capacities**:
   - `quad_table[500]`
   - `bp_logs[200]`
   - `symbol_table[100]`
   - Risk of silent buffer overflow or immediate `exit(EXIT_FAILURE)`.
2. **Global State Retention**:
   - `quad_count`, `temp_var_count`, `bp_log_count`, `sym_count` are global variables never reset between compilation runs. Repeated calls in process would accumulate corrupted state.
3. **Lexer Line/Column Tracking Missing**:
   - `yytext` consumed without tracking `yylineno` or column offsets. Lexical errors printed to stdout without proper exit codes or line numbers.
4. **No Structured JSON Output**:
   - Compiler only writes human-formatted text blocks to stdout (`--- THREE-ADDRESS CODE (TAC) ---`, etc.).
   - Needs `--json` mode emitting a single, strictly valid JSON schema containing:
     - Lexed tokens (token, lexeme, line, col)
     - Diagnostics / syntax errors with line/col
     - Quads (id, op, arg1, arg2, result, line)
     - TAC instructions formatted
     - Backpatch events trace (`makelist`, `merge`, `backpatch`)
     - Canonical symbols / variables
5. **No Triples or Indirect Triples**:
   - Currently only Quadruples are generated.
6. **No Basic Block Partitioning or CFG Extraction**:
   - No leader algorithm or graph edge structures in C code.
7. **No Optimizer Passes**:
   - No constant folding, propagation, dead code elimination, CSE, or unreachable code elimination.
8. **Virtual Simulator Limitations**:
   - Integer only, fixed step limit, no reverse stepping, no step-by-step telemetry, undefined variables silently default to 0 without warning/tracking.
9. **No Liveness / Register Allocation**:
   - No interference graph or Chaitin-Briggs graph coloring allocator.

## 3. Transformation Strategy
- Upgrade `lexer.l` and `parser.y` to:
  - Track line and column numbers using `yylloc`.
  - Add reset API for clean runs.
  - Support a `--json` CLI flag to emit clean structured JSON containing the entire compilation artifact.
  - Support Triples and Indirect Triples generation.
  - Implement leader algorithm and Basic Block/CFG analysis.
  - Retain backwards-compatible text output when `--json` is omitted.
- Build clean Python backend (`backend/server.py`):
  - Expose REST API: `/api/compile`, `/api/execute`, `/api/optimize`, `/api/analyze/registers`, `/api/samples`, `/api/tests/run`, `/api/health`.
  - Implement full Virtual Execution Machine with reversible step-by-step history, memory store, and safety limits.
  - Implement complete deterministic Optimization Pipeline (Constant Folding, Constant Propagation, DCE, CSE, Unreachable Code Elimination).
  - Implement Chaitin-Briggs K-register graph-coloring allocator with interference matrix and spill analysis.
- Build modern React 18 + Vite + TypeScript frontend (`frontend/`):
  - Monaco editor with custom language syntax highlighting, error markers, diagnostics panel.
  - Cytoscape.js interactive Control-Flow Graph with pan/zoom/legend/block inspector and animated loop header detection.
  - IR Explorer with synchronized tabs (TAC, Quads, Triples, Indirect Triples, Backpatch Inspector with step-through replay).
  - Optimization Lab with side-by-side diff view and pass manager.
  - Virtual Execution Machine with Step Forward / Backward / Play / Pause / Variable inspector.
  - Register & Metrics Studio with live range table, interference graph, and before/after complexity reduction.
  - Test Bench with automated test runner (14+ test suites).
