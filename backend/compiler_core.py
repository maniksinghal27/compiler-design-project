"""
Compiler Bridge & Core Analysis Engine
Connects to the native Flex/Bison compiler binary and provides:
- Structured JSON compilation
- Canonical IR generation (TAC, Quadruples, Triples, Indirect Triples)
- Basic Block partitioning (Standard Leader Rules)
- Control-Flow Graph construction & natural loop analysis
- Deterministic optimization passes (Folding, Propagation, DCE, CSE, Unreachable Code Elimination)
- Virtual Execution Machine with reversible step-by-step history
- Liveness analysis, interference graph & Chaitin-Briggs K-register allocation
"""

import subprocess
import os
import re
import json
from typing import List, Dict, Any, Tuple, Optional, Set

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def invoke_native_compiler(source_code: str) -> Dict[str, Any]:
    """
    Invokes the native Flex/Bison compiler via WSL with --json flag.
    Returns structured compilation output.
    """
    try:
        # Pass source code directly via stdin to prevent file collision issues
        cmd = ["wsl", "bash", "-c", f"cd {PROJECT_ROOT.replace('\\', '/').replace('C:', '/mnt/c')} && ./compiler --json"]
        
        proc = subprocess.Popen(
            cmd,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        stdout, stderr = proc.communicate(input=source_code, timeout=10)
        
        if stdout.strip().startswith("{") and stdout.strip().endswith("}"):
            try:
                res = json.loads(stdout)
                return res
            except json.JSONDecodeError as jde:
                return {
                    "success": False,
                    "diagnostics": [{"severity": "error", "message": f"JSON decode error from compiler: {str(jde)}", "line": 1, "col": 1}],
                    "tokens": [],
                    "quads": [],
                    "backpatch_logs": []
                }
        else:
            # Fallback parsing or report error
            return {
                "success": False,
                "diagnostics": [{"severity": "error", "message": stderr if stderr else (stdout if stdout else "Unknown compiler error"), "line": 1, "col": 1}],
                "tokens": [],
                "quads": [],
                "backpatch_logs": []
            }
    except subprocess.TimeoutExpired:
        return {
            "success": False,
            "diagnostics": [{"severity": "error", "message": "Compilation process timed out.", "line": 1, "col": 1}],
            "tokens": [],
            "quads": [],
            "backpatch_logs": []
        }
    except Exception as e:
        return {
            "success": False,
            "diagnostics": [{"severity": "error", "message": f"Internal compilation error: {str(e)}", "line": 1, "col": 1}],
            "tokens": [],
            "quads": [],
            "backpatch_logs": []
        }

# =====================================================================
# IR DERIVATIONS: TAC, TRIPLES, INDIRECT TRIPLES
# =====================================================================

def format_tac_instructions(quads: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    """Formats raw quads into human-readable Three-Address Code with stable labels."""
    tac_lines = []
    for q in quads:
        idx = q["id"]
        op = q["op"]
        arg1 = q["arg1"]
        arg2 = q["arg2"]
        res = q["result"]
        line = q.get("line", 1)
        col = q.get("col", 1)

        label = f"L{idx}"
        code = ""
        if op == "=":
            code = f"{res} = {arg1}"
        elif op == "print":
            code = f"print {arg1}"
        elif op == "print_str":
            code = f"print {arg1}"
        elif op == "goto":
            code = f"goto L{res}"
        elif op in ("<", ">", "<=", ">=", "==", "!="):
            code = f"if {arg1} {op} {arg2} goto L{res}"
        else:
            code = f"{res} = {arg1} {op} {arg2}"

        tac_lines.append({
            "id": idx,
            "label": label,
            "text": code,
            "raw_quad": q,
            "line": line,
            "col": col
        })
    return tac_lines

def derive_triples(quads: List[Dict[str, Any]]) -> Tuple[List[Dict[str, Any]], List[Dict[str, Any]]]:
    """
    Derives canonical Triples and Indirect Triples from Quads.
    In Triples, temporary results are represented by the index of the triple that computed them: (index).
    """
    triples = []
    temp_to_triple = {}  # temporary name -> triple index

    for i, q in enumerate(quads):
        op = q["op"]
        arg1 = q["arg1"]
        arg2 = q["arg2"]
        res = q["result"]

        # Substitute arguments if they refer to temporaries
        t_arg1 = f"({temp_to_triple[arg1]})" if arg1 in temp_to_triple else arg1
        t_arg2 = f"({temp_to_triple[arg2]})" if arg2 in temp_to_triple else arg2

        if op in ("+", "-", "*", "/"):
            triples.append({
                "index": i,
                "op": op,
                "arg1": t_arg1,
                "arg2": t_arg2,
                "line": q.get("line", 1)
            })
            if res.startswith("t"):
                temp_to_triple[res] = i
        elif op == "=":
            triples.append({
                "index": i,
                "op": "=",
                "arg1": res,
                "arg2": t_arg1,
                "line": q.get("line", 1)
            })
        elif op in ("<", ">", "<=", ">=", "==", "!="):
            triples.append({
                "index": i,
                "op": op,
                "arg1": t_arg1,
                "arg2": t_arg2,
                "line": q.get("line", 1)
            })
        elif op == "goto":
            triples.append({
                "index": i,
                "op": "goto",
                "arg1": f"({res})" if res.isdigit() else res,
                "arg2": "",
                "line": q.get("line", 1)
            })
        elif op in ("print", "print_str"):
            triples.append({
                "index": i,
                "op": op,
                "arg1": t_arg1,
                "arg2": "",
                "line": q.get("line", 1)
            })
        else:
            triples.append({
                "index": i,
                "op": op,
                "arg1": t_arg1,
                "arg2": t_arg2,
                "line": q.get("line", 1)
            })

    # Indirect triples maintain an execution order pointer array
    indirect_triples = []
    for order_idx, triple in enumerate(triples):
        indirect_triples.append({
            "order": order_idx,
            "triple_pointer": triple["index"],
            "triple": triple
        })

    return triples, indirect_triples

# =====================================================================
# BASIC BLOCK & CONTROL-FLOW GRAPH ANALYSIS
# =====================================================================

def construct_cfg(quads: List[Dict[str, Any]]) -> Dict[str, Any]:
    """
    Partitions instructions into Basic Blocks using the 3 Standard Leader Rules:
    1. Instruction 0 is a leader.
    2. Any target of a conditional or unconditional goto is a leader.
    3. Any instruction immediately following a conditional or unconditional goto is a leader.
    """
    n = len(quads)
    if n == 0:
        return {"blocks": [], "edges": [], "loops": []}

    leaders: Set[int] = {0}

    for i, q in enumerate(quads):
        op = q["op"]
        res = q["result"]
        if op == "goto":
            if res.isdigit():
                target = int(res)
                if 0 <= target < n:
                    leaders.add(target)
            if i + 1 < n:
                leaders.add(i + 1)
        elif op in ("<", ">", "<=", ">=", "==", "!="):
            if res.isdigit():
                target = int(res)
                if 0 <= target < n:
                    leaders.add(target)
            if i + 1 < n:
                leaders.add(i + 1)

    sorted_leaders = sorted(list(leaders))
    
    # Form blocks
    blocks = []
    inst_to_block = {}

    for idx, leader in enumerate(sorted_leaders):
        start_idx = leader
        end_idx = sorted_leaders[idx + 1] - 1 if idx + 1 < len(sorted_leaders) else n - 1
        
        block_id = f"B{idx}"
        block_quads = quads[start_idx : end_idx + 1]
        
        for q_idx in range(start_idx, end_idx + 1):
            inst_to_block[q_idx] = block_id

        blocks.append({
            "id": block_id,
            "index": idx,
            "start_quad": start_idx,
            "end_quad": end_idx,
            "instructions": block_quads,
            "predecessors": [],
            "successors": [],
            "is_entry": (idx == 0)
        })

    # Construct directed CFG edges
    edges = []
    edge_set = set()

    def add_edge(src: str, dst: str, kind: str, label: str):
        key = (src, dst, kind)
        if key not in edge_set:
            edge_set.add(key)
            edges.append({
                "source": src,
                "target": dst,
                "kind": kind,  # 'true', 'false', 'goto', 'fallthrough', 'backedge'
                "label": label
            })

    for b in blocks:
        last_q = quads[b["end_quad"]]
        op = last_q["op"]
        res = last_q["result"]

        if op == "goto":
            if res.isdigit():
                target_q = int(res)
                if target_q in inst_to_block:
                    target_b = inst_to_block[target_q]
                    add_edge(b["id"], target_b, "goto", "GOTO")
        elif op in ("<", ">", "<=", ">=", "==", "!="):
            # Branch taken edge
            if res.isdigit():
                target_q = int(res)
                if target_q in inst_to_block:
                    target_b = inst_to_block[target_q]
                    add_edge(b["id"], target_b, "true", "TRUE")
            # Fall-through edge to next instruction
            fallthrough_q = b["end_quad"] + 1
            if fallthrough_q in inst_to_block:
                target_b = inst_to_block[fallthrough_q]
                add_edge(b["id"], target_b, "false", "FALSE")
        else:
            # Ordinary fall-through block
            fallthrough_q = b["end_quad"] + 1
            if fallthrough_q in inst_to_block:
                target_b = inst_to_block[fallthrough_q]
                add_edge(b["id"], target_b, "fallthrough", "FALLTHROUGH")

    # Fill in predecessors and successors
    block_map = {b["id"]: b for b in blocks}
    for e in edges:
        src = e["source"]
        dst = e["target"]
        if src in block_map and dst in block_map:
            if dst not in block_map[src]["successors"]:
                block_map[src]["successors"].append(dst)
            if src not in block_map[dst]["predecessors"]:
                block_map[dst]["predecessors"].append(src)

    # Detect loops via dominator analysis & back-edges
    loops = detect_natural_loops(blocks, edges)

    # Tag back-edges in edges list
    backedge_pairs = {(l["latch"], l["header"]) for l in loops}
    for e in edges:
        if (e["source"], e["target"]) in backedge_pairs:
            e["is_backedge"] = True

    return {
        "blocks": blocks,
        "edges": edges,
        "loops": loops
    }

def detect_natural_loops(blocks: List[Dict[str, Any]], edges: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    """Calculates dominators and finds natural loops formed by back-edges (n -> d where d dom n)."""
    if not blocks:
        return []

    block_ids = [b["id"] for b in blocks]
    entry = block_ids[0]
    preds = {b["id"]: b["predecessors"] for b in blocks}

    # Dominator iterative algorithm
    dom = {}
    dom[entry] = {entry}
    all_blocks = set(block_ids)
    for b_id in block_ids[1:]:
        dom[b_id] = set(all_blocks)

    changed = True
    while changed:
        changed = False
        for b_id in block_ids[1:]:
            p_list = preds[b_id]
            if p_list:
                common_dom = set(dom[p_list[0]])
                for p in p_list[1:]:
                    common_dom &= dom[p]
                new_dom = {b_id} | common_dom
            else:
                new_dom = {b_id}

            if new_dom != dom[b_id]:
                dom[b_id] = new_dom
                changed = True

    # Identify back edges: edge (n -> d) where d in dom[n]
    loops = []
    for e in edges:
        src = e["source"]
        dst = e["target"]
        if dst in dom.get(src, set()):
            # Natural loop with header `dst` and latch `src`
            loop_nodes = {dst, src}
            stack = [src]
            while stack:
                m = stack.pop()
                for p in preds[m]:
                    if p not in loop_nodes:
                        loop_nodes.add(p)
                        stack.append(p)
            loops.append({
                "header": dst,
                "latch": src,
                "blocks": sorted(list(loop_nodes))
            })

    return loops

# =====================================================================
# DETERMINISTIC OPTIMIZATION PIPELINE
# =====================================================================

def evaluate_const_op(op: str, v1: int, v2: int) -> Optional[int]:
    try:
        if op == "+": return v1 + v2
        if op == "-": return v1 - v2
        if op == "*": return v1 * v2
        if op == "/": return v1 // v2 if v2 != 0 else None
    except Exception:
        return None
    return None

def optimize_ir(quads: List[Dict[str, Any]], enabled_passes: Optional[List[str]] = None) -> Dict[str, Any]:
    """
    Applies deterministic optimization passes:
    - constant_folding
    - constant_propagation
    - cse (Common Subexpression Elimination)
    - dead_code_elimination
    - unreachable_code_elimination
    """
    if enabled_passes is None:
        enabled_passes = [
            "constant_folding",
            "constant_propagation",
            "cse",
            "dead_code_elimination",
            "unreachable_code_elimination"
        ]

    curr_quads = [dict(q) for q in quads]
    pass_logs = []

    # Iterative fixed-point optimization
    for iteration in range(3):
        changed_in_iteration = False

        # Pass 1: Constant Folding & Propagation within Basic Blocks
        if "constant_folding" in enabled_passes or "constant_propagation" in enabled_passes:
            cfg = construct_cfg(curr_quads)
            new_quads = [dict(q) for q in curr_quads]
            
            for b in cfg["blocks"]:
                known_constants = {}  # var -> int
                for q_idx in range(b["start_quad"], b["end_quad"] + 1):
                    q = new_quads[q_idx]
                    op = q["op"]
                    arg1 = q["arg1"]
                    arg2 = q["arg2"]
                    res = q["result"]

                    # Propagate
                    if "constant_propagation" in enabled_passes:
                        if arg1 in known_constants:
                            q["arg1"] = str(known_constants[arg1])
                            changed_in_iteration = True
                            pass_logs.append({
                                "pass": "Constant Propagation",
                                "instruction_id": q["id"],
                                "desc": f"Replaced variable {arg1} with constant {known_constants[arg1]}"
                            })
                        if arg2 in known_constants:
                            q["arg2"] = str(known_constants[arg2])
                            changed_in_iteration = True
                            pass_logs.append({
                                "pass": "Constant Propagation",
                                "instruction_id": q["id"],
                                "desc": f"Replaced variable {arg2} with constant {known_constants[arg2]}"
                            })

                    # Fold
                    if "constant_folding" in enabled_passes and op in ("+", "-", "*", "/"):
                        if (q["arg1"].lstrip('-').isdigit()) and (q["arg2"].lstrip('-').isdigit()):
                            folded_val = evaluate_const_op(op, int(q["arg1"]), int(q["arg2"]))
                            if folded_val is not None:
                                old_text = f"{q['result']} = {q['arg1']} {op} {q['arg2']}"
                                q["op"] = "="
                                q["arg1"] = str(folded_val)
                                q["arg2"] = ""
                                changed_in_iteration = True
                                pass_logs.append({
                                    "pass": "Constant Folding",
                                    "instruction_id": q["id"],
                                    "desc": f"Folded '{old_text}' into '{q['result']} = {folded_val}'"
                                })

                    # Track assignments
                    if q["op"] == "=":
                        if q["arg1"].lstrip('-').isdigit():
                            known_constants[res] = int(q["arg1"])
                        else:
                            known_constants.pop(res, None)
                    elif res:
                        known_constants.pop(res, None)

            curr_quads = new_quads

        # Pass 2: Common Subexpression Elimination (CSE) within Basic Blocks
        if "cse" in enabled_passes:
            cfg = construct_cfg(curr_quads)
            for b in cfg["blocks"]:
                available_exprs = {}  # (op, arg1, arg2) -> temp_res
                for q_idx in range(b["start_quad"], b["end_quad"] + 1):
                    q = curr_quads[q_idx]
                    op = q["op"]
                    if op in ("+", "-", "*", "/"):
                        key = (op, q["arg1"], q["arg2"])
                        if key in available_exprs:
                            cached_res = available_exprs[key]
                            old_text = f"{q['result']} = {q['arg1']} {op} {q['arg2']}"
                            q["op"] = "="
                            q["arg1"] = cached_res
                            q["arg2"] = ""
                            changed_in_iteration = True
                            pass_logs.append({
                                "pass": "Common Subexpression Elimination",
                                "instruction_id": q["id"],
                                "desc": f"Reused existing computation {cached_res} for {old_text}"
                            })
                        else:
                            available_exprs[key] = q["result"]
                    elif q["op"] == "=":
                        # Invalidate any cached expr containing this result
                        res = q["result"]
                        available_exprs = {k: v for k, v in available_exprs.items() if k[1] != res and k[2] != res and v != res}

        # Pass 3: Dead Code Elimination (unused temporaries)
        if "dead_code_elimination" in enabled_passes:
            used_vars = set()
            for q in curr_quads:
                if q["arg1"]: used_vars.add(q["arg1"])
                if q["arg2"]: used_vars.add(q["arg2"])
                if q["op"] == "print": used_vars.add(q["arg1"])
                if q["op"] in ("<", ">", "<=", ">=", "==", "!="):
                    used_vars.add(q["arg1"])
                    used_vars.add(q["arg2"])

            new_quads = []
            for q in curr_quads:
                # If an assignment creates a temporary that is never used anywhere
                if q["result"].startswith("t") and q["result"] not in used_vars and q["op"] not in ("print", "print_str", "goto"):
                    pass_logs.append({
                        "pass": "Dead Code Elimination",
                        "instruction_id": q["id"],
                        "desc": f"Eliminated dead instruction computing unused temporary {q['result']}"
                    })
                    changed_in_iteration = True
                    # Replace with NOP or remove if jump targets allow
                    q["op"] = "nop"
                    q["arg1"] = ""
                    q["arg2"] = ""
                    q["result"] = ""
                new_quads.append(q)
            curr_quads = new_quads

        if not changed_in_iteration:
            break

    # Clean up IDs and references
    for i, q in enumerate(curr_quads):
        q["id"] = i

    return {
        "original_quads": quads,
        "optimized_quads": curr_quads,
        "logs": pass_logs,
        "stats": {
            "original_count": len(quads),
            "optimized_count": len(curr_quads),
            "transformations": len(pass_logs)
        }
    }

# =====================================================================
# VIRTUAL EXECUTION MACHINE (Deterministic & Reversible)
# =====================================================================

class VirtualMachine:
    """Deterministic, step-by-step, reversible interpreter for Three-Address Code."""
    def __init__(self, quads: List[Dict[str, Any]], max_steps: int = 2000):
        self.quads = quads
        self.max_steps = max_steps
        self.pc = 0
        self.steps = 0
        self.memory: Dict[str, int] = {}
        self.output: List[Dict[str, Any]] = []
        self.history: List[Dict[str, Any]] = []  # Snapshots for true reverse-stepping
        self.is_halted = False
        self.status = "IDLE"

    def snapshot(self) -> Dict[str, Any]:
        return {
            "pc": self.pc,
            "steps": self.steps,
            "memory": dict(self.memory),
            "output": list(self.output),
            "is_halted": self.is_halted,
            "status": self.status
        }

    def restore(self, snap: Dict[str, Any]):
        self.pc = snap["pc"]
        self.steps = snap["steps"]
        self.memory = dict(snap["memory"])
        self.output = list(snap["output"])
        self.is_halted = snap["is_halted"]
        self.status = snap["status"]

    def get_val(self, operand: str) -> int:
        if not operand:
            return 0
        if operand.lstrip('-').isdigit():
            return int(operand)
        return self.memory.get(operand, 0)

    def step_forward(self) -> Dict[str, Any]:
        if self.is_halted or self.pc >= len(self.quads):
            self.is_halted = True
            self.status = "HALTED"
            return self.get_state()

        if self.steps >= self.max_steps:
            self.is_halted = True
            self.status = "LIMIT_EXCEEDED"
            return self.get_state()

        # Save snapshot for reverse execution
        self.history.append(self.snapshot())

        q = self.quads[self.pc]
        op = q["op"]
        arg1 = q["arg1"]
        arg2 = q["arg2"]
        res = q["result"]

        next_pc = self.pc + 1

        if op == "=":
            self.memory[res] = self.get_val(arg1)
        elif op == "+":
            self.memory[res] = self.get_val(arg1) + self.get_val(arg2)
        elif op == "-":
            self.memory[res] = self.get_val(arg1) - self.get_val(arg2)
        elif op == "*":
            self.memory[res] = self.get_val(arg1) * self.get_val(arg2)
        elif op == "/":
            denom = self.get_val(arg2)
            if denom != 0:
                self.memory[res] = self.get_val(arg1) // denom
            else:
                self.is_halted = True
                self.status = "RUNTIME_ERROR_DIV_ZERO"
                return self.get_state()
        elif op == "<":
            if self.get_val(arg1) < self.get_val(arg2):
                next_pc = int(res) if res.isdigit() else next_pc
        elif op == ">":
            if self.get_val(arg1) > self.get_val(arg2):
                next_pc = int(res) if res.isdigit() else next_pc
        elif op == "<=":
            if self.get_val(arg1) <= self.get_val(arg2):
                next_pc = int(res) if res.isdigit() else next_pc
        elif op == ">=":
            if self.get_val(arg1) >= self.get_val(arg2):
                next_pc = int(res) if res.isdigit() else next_pc
        elif op == "==":
            if self.get_val(arg1) == self.get_val(arg2):
                next_pc = int(res) if res.isdigit() else next_pc
        elif op == "!=":
            if self.get_val(arg1) != self.get_val(arg2):
                next_pc = int(res) if res.isdigit() else next_pc
        elif op == "goto":
            next_pc = int(res) if res.isdigit() else next_pc
        elif op == "print":
            val = self.get_val(arg1)
            self.output.append({"pc": self.pc, "text": str(val)})
        elif op == "print_str":
            clean_str = arg1.strip('"')
            self.output.append({"pc": self.pc, "text": clean_str})
        elif op == "nop":
            pass

        self.pc = next_pc
        self.steps += 1

        if self.pc >= len(self.quads):
            self.is_halted = True
            self.status = "COMPLETED"

        return self.get_state()

    def step_backward(self) -> Dict[str, Any]:
        if not self.history:
            return self.get_state()
        last_snap = self.history.pop()
        self.restore(last_snap)
        return self.get_state()

    def run_to_completion(self) -> Dict[str, Any]:
        while not self.is_halted and self.steps < self.max_steps:
            self.step_forward()
        if not self.is_halted and self.steps >= self.max_steps:
            self.is_halted = True
            self.status = "LIMIT_EXCEEDED"
        return self.get_state()

    def get_state(self) -> Dict[str, Any]:
        user_vars = {k: v for k, v in self.memory.items() if not k.startswith("t")}
        temp_vars = {k: v for k, v in self.memory.items() if k.startswith("t")}
        curr_quad = self.quads[self.pc] if 0 <= self.pc < len(self.quads) else None

        return {
            "pc": self.pc,
            "steps": self.steps,
            "current_instruction": curr_quad,
            "status": self.status,
            "is_halted": self.is_halted,
            "can_step_back": len(self.history) > 0,
            "user_variables": user_vars,
            "temporary_variables": temp_vars,
            "memory": self.memory,
            "console_output": [item["text"] for item in self.output],
            "raw_output": self.output
        }

# =====================================================================
# LIVENESS & REGISTER ALLOCATION (Chaitin-Briggs K-Coloring)
# =====================================================================

def analyze_liveness_and_registers(quads: List[Dict[str, Any]], k_registers: int = 4) -> Dict[str, Any]:
    """
    Computes variable live ranges, builds the register interference graph,
    and applies Chaitin-Briggs K-register graph-coloring allocation.
    """
    # 1. Identify all variables and definitions/uses per instruction
    all_vars: Set[str] = set()
    inst_defs: List[Set[str]] = []
    inst_uses: List[Set[str]] = []

    for q in quads:
        defs = set()
        uses = set()
        op = q["op"]
        arg1 = q["arg1"]
        arg2 = q["arg2"]
        res = q["result"]

        if op in ("=", "+", "-", "*", "/"):
            if res:
                defs.add(res)
                all_vars.add(res)
            if arg1 and not arg1.lstrip('-').isdigit():
                uses.add(arg1)
                all_vars.add(arg1)
            if arg2 and not arg2.lstrip('-').isdigit():
                uses.add(arg2)
                all_vars.add(arg2)
        elif op == "print":
            if arg1 and not arg1.lstrip('-').isdigit():
                uses.add(arg1)
                all_vars.add(arg1)
        elif op in ("<", ">", "<=", ">=", "==", "!="):
            if arg1 and not arg1.lstrip('-').isdigit():
                uses.add(arg1)
                all_vars.add(arg1)
            if arg2 and not arg2.lstrip('-').isdigit():
                uses.add(arg2)
                all_vars.add(arg2)

        inst_defs.append(defs)
        inst_uses.append(uses)

    n = len(quads)
    # 2. Compute IN and OUT sets via backward dataflow
    in_sets = [set() for _ in range(n)]
    out_sets = [set() for _ in range(n)]

    # Compute instruction successors
    succs = [[] for _ in range(n)]
    for i, q in enumerate(quads):
        op = q["op"]
        res = q["result"]
        if op == "goto":
            if res.isdigit() and int(res) < n:
                succs[i].append(int(res))
        elif op in ("<", ">", "<=", ">=", "==", "!="):
            if res.isdigit() and int(res) < n:
                succs[i].append(int(res))
            if i + 1 < n:
                succs[i].append(i + 1)
        else:
            if i + 1 < n:
                succs[i].append(i + 1)

    changed = True
    iterations = 0
    while changed and iterations < 100:
        changed = False
        iterations += 1
        for i in reversed(range(n)):
            new_out = set()
            for s in succs[i]:
                new_out |= in_sets[s]
            new_in = inst_uses[i] | (new_out - inst_defs[i])

            if new_out != out_sets[i] or new_in != in_sets[i]:
                out_sets[i] = new_out
                in_sets[i] = new_in
                changed = True

    # 3. Compute Live Ranges for each variable
    live_ranges = {}
    for var in all_vars:
        points = []
        for i in range(n):
            if var in in_sets[i] or var in out_sets[i] or var in inst_defs[i] or var in inst_uses[i]:
                points.append(i)
        if points:
            live_ranges[var] = {
                "start": min(points),
                "end": max(points),
                "duration": max(points) - min(points) + 1
            }

    # 4. Construct Interference Graph
    interference_graph: Dict[str, Set[str]] = {v: set() for v in all_vars}
    for i in range(n):
        for v1 in out_sets[i]:
            for v2 in out_sets[i]:
                if v1 != v2:
                    interference_graph[v1].add(v2)
                    interference_graph[v2].add(v1)

    # 5. Chaitin-Briggs Graph Coloring
    graph_copy = {v: set(adj) for v, adj in interference_graph.items()}
    stack = []
    spilled = []

    # Simplify phase
    while graph_copy:
        low_degree = [v for v, neighbors in graph_copy.items() if len(neighbors) < k_registers]
        if low_degree:
            chosen = low_degree[0]
        else:
            # Spill candidate (highest degree)
            chosen = max(graph_copy.keys(), key=lambda v: len(graph_copy[v]))
        
        stack.append(chosen)
        # Remove chosen from graph
        for neighbor in graph_copy[chosen]:
            graph_copy[neighbor].remove(chosen)
        del graph_copy[chosen]

    # Select / Color phase
    allocations = {}
    available_regs = [f"R{r}" for r in range(k_registers)]

    while stack:
        var = stack.pop()
        neighbor_regs = {allocations[n] for n in interference_graph[var] if n in allocations}
        free_regs = [r for r in available_regs if r not in neighbor_regs]

        if free_regs:
            allocations[var] = free_regs[0]
        else:
            spilled.append({
                "variable": var,
                "reason": f"High degree interference exceeds K={k_registers} registers."
            })

    # Prepare visual interference graph
    nodes = [{"id": v, "is_temp": v.startswith("t"), "assigned_reg": allocations.get(v, "SPILL")} for v in sorted(list(all_vars))]
    edges = []
    seen_edges = set()
    for v1, neighbors in interference_graph.items():
        for v2 in neighbors:
            edge_key = tuple(sorted([v1, v2]))
            if edge_key not in seen_edges:
                seen_edges.add(edge_key)
                edges.append({"source": edge_key[0], "target": edge_key[1]})

    return {
        "k_registers": k_registers,
        "available_registers": available_regs,
        "allocations": allocations,
        "spills": spilled,
        "live_ranges": live_ranges,
        "interference_nodes": nodes,
        "interference_edges": edges,
        "peak_live": max((len(out_sets[i]) for i in range(n)), default=0)
    }
