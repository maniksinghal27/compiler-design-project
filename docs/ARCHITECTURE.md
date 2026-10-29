# Compiler Studio: Architecture & Technical Specification

## 1. System Architecture

The **Compiler Studio** suite is structured as a decoupled, multi-tier compiler engineering platform:

```
+-------------------------------------------------------------------------------+
|                             COMPILER STUDIO UI                                |
|              (React 18 + TypeScript + Monaco Editor + Cytoscape.js)           |
+-------------------------------------------------------------------------------+
       |                                                 ^
       | HTTP POST /api/compile                          | JSON Response:
       | HTTP POST /api/execute                          | Quads, Tokens, TAC,
       | HTTP POST /api/optimize                         | Triples, Indirect Triples,
       v                                                 | CFG Nodes/Edges, Loops,
+-----------------------------------------------------+  | Register Allocations
|                    BACKEND ENGINE                   |--+
|                (Python Flask + REST API)            |
| - compiler_core.py                                  |
|   * Leader Algorithm & CFG Construction             |
|   * Deterministic Optimizer (Folding, CSE, DCE)     |
|   * Reversible Virtual Execution Machine            |
|   * Liveness & Chaitin-Briggs K-Coloring Allocator  |
+-----------------------------------------------------+
       |
       | Native Subprocess Pipe (stdin/stdout)
       v
+-----------------------------------------------------+
|              NATIVE FLEX & BISON COMPILER           |
| - lexer.l (Flex): Tokens, column/line tracking      |
| - parser.y (Bison): LALR(1) Parsing, Backpatching,  |
|   makelist(), merge(), backpatch(), --json output   |
+-----------------------------------------------------+
```

## 2. Compiler Pipeline & Data Structures

### A. Lexical & Syntax Analysis
- **Lexer (`lexer.l`)**: Tracks source coordinates (`yylloc.first_line`, `yylloc.first_column`), records full token stream into structured buffers, and safely reports unrecognized characters with line/col diagnostics.
- **Parser (`parser.y`)**: LALR(1) Bison grammar with operator precedence (`* /` over `+ -`). Produces Quadruples and manages backpatch linked-list nodes.

### B. Intermediate Representations
1. **Three-Address Code (TAC)**: Linear symbolic instruction list with stable label naming (`L0`, `L1`, etc.).
2. **Quadruples (`Quad`)**: `(op, arg1, arg2, result)`.
3. **Triples**: Result temporaries are referenced by triple indices: `(0) * b 4`, `(1) + a (0)`.
4. **Indirect Triples**: Execution order pointer table decoupled from underlying instruction storage.

### C. Backpatching Subsystem
- `makelist(int quad_num)`: Allocates new linked list node for unresolved jump destination.
- `merge(Node *l1, Node *l2)`: Concatenates unresolved jump lists.
- `backpatch(Node *list, int target_quad)`: Resolves destination in each pending quad.
- All operations log telemetry events rendered in the interactive Backpatch Inspector.

### D. Control-Flow Graph (CFG) Construction
- **Leader Identification**:
  1. Quad #0 is a leader.
  2. Any target of a conditional or unconditional branch is a leader.
  3. Any instruction immediately following a conditional or unconditional branch is a leader.
- **Dominator Tree & Natural Loop Detection**:
  - Computes dominators iteratively: `Dom(b) = {b} U (∩ Dom(p) for p in Pred(b))`.
  - Edge `(latch -> header)` is a back-edge if `header ∈ Dom(latch)`. Natural loops are extracted and color-coded.

### E. Deterministic Optimizer
- **Constant Folding**: Pre-evaluates binary operations with numeric constants.
- **Constant Propagation**: Substitutes known constants through basic block assignments.
- **Common Subexpression Elimination (CSE)**: Memoizes repeated computations within basic blocks.
- **Dead Code Elimination (DCE)**: Eliminates dead assignments to temporaries without live observers.

### F. Reversible Virtual Execution Machine
- Discrete instruction-level step execution.
- Captures snapshots `(pc, steps, memory, output)` enabling true reversible backward stepping.
- Step-limit safeguard (`1000` steps) intercepts non-terminating loops.

### G. Register Allocation (Chaitin-Briggs K-Coloring)
- Backward dataflow liveness analysis:
  - `OUT[B] = U IN[S] for S in Succ(B)`
  - `IN[B] = USE[B] U (OUT[B] - DEF[B])`
- Constructs Interference Graph: edges between variables simultaneously live at any program point.
- Applies K-register graph-coloring (`K=4` default) with spill-cost candidate selection.
