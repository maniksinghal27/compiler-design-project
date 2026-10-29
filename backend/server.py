"""
COMPILER STUDIO - Local Flask API Backend
Provides structured, versionable endpoints connecting the modern UI to the
native Flex/Bison compiler, CFG analyzer, Virtual Execution Machine,
Optimization pipeline, and Register Allocator.
"""

import os
import sys
import time
from typing import Dict, Any
from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS

# Add root directory to sys.path
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

from backend import compiler_core as core

app = Flask(__name__, static_folder="../frontend/dist", static_url_path="")
CORS(app)

# Preset sample programs required by prompt
SAMPLES = {
    "test_nested.c": {
        "title": "Nested Loops & Arithmetic Precedence",
        "description": "Nested while loops calculating progressive sum with print output.",
        "code": """i = 1;
sum = 0;

while (i <= 3) {
    j = 1;

    while (j <= 2) {
        sum = sum + i;
        print(sum);
        j = j + 1;
    }

    i = i + 1;
}"""
    },
    "test_if_else.c": {
        "title": "Complex Conditionals & Branching",
        "description": "Multi-path decision branching demonstrating true/false backpatching lists.",
        "code": """x = 15;
y = 20;

if (x < y) {
    print("x is smaller");
    diff = y - x;
    print(diff);
} else {
    print("x is larger or equal");
    diff = x - y;
    print(diff);
}"""
    },
    "test_for_loops.c": {
        "title": "For Loop Step Traversal",
        "description": "Structured for loop with initialization, condition, and step expressions.",
        "code": """sum = 0;
for (i = 0; i <= 5; i = i + 1) {
    sum = sum + i;
    print(sum);
}"""
    },
    "test_infinite.c": {
        "title": "Infinite Loop Detector & Step Limiting",
        "description": "Demonstrates VM step-limit safeguard when non-terminating loops occur.",
        "code": """x = 1;
while (x > 0) {
    x = x + 1;
}"""
    },
    "test_optimizable.c": {
        "title": "Constant Folding & Common Subexpressions",
        "description": "Arithmetic constants and repeated subexpressions ready for optimization.",
        "code": """a = 10 + 20;
b = 5 * 6;
c = a + b;
d = a + b;
print(c);
print(d);"""
    }
}

# In-memory virtual machine session for interactive step-by-step debugging
_active_vm = None

@app.route("/")
def index():
    if os.path.exists(os.path.join(BASE_DIR, "frontend", "dist", "index.html")):
        return send_from_directory(os.path.join(BASE_DIR, "frontend", "dist"), "index.html")
    return jsonify({
        "name": "Compiler Studio Backend API",
        "status": "Online",
        "version": "2.0.0",
        "frontend": "Frontend dev server active on port 5173"
    })

@app.route("/api/health", methods=["GET"])
def health():
    return jsonify({
        "status": "HEALTHY",
        "version": "2.0.0",
        "compiler": "Flex 2.6.4 / Bison 3.8.2 / GCC",
        "timestamp": time.time()
    })

@app.route("/api/samples", methods=["GET"])
def get_samples():
    return jsonify(SAMPLES)

@app.route("/api/compile", methods=["POST"])
def compile_code():
    data = request.get_json() or {}
    source_code = data.get("code", "")
    k_registers = int(data.get("k_registers", 4))

    start_time = time.perf_counter()
    raw_res = core.invoke_native_compiler(source_code)
    compile_duration = round((time.perf_counter() - start_time) * 1000, 2)

    if not raw_res.get("success", False) and not raw_res.get("quads"):
        return jsonify({
            "success": False,
            "duration_ms": compile_duration,
            "diagnostics": raw_res.get("diagnostics", []),
            "tokens": raw_res.get("tokens", []),
            "quads": [],
            "tac": [],
            "triples": [],
            "indirect_triples": [],
            "backpatch_logs": [],
            "cfg": {"blocks": [], "edges": [], "loops": []},
            "registers": {"allocations": {}, "spills": []}
        })

    quads = raw_res.get("quads", [])
    tac = core.format_tac_instructions(quads)
    triples, indirect_triples = core.derive_triples(quads)
    cfg = core.construct_cfg(quads)
    reg_analysis = core.analyze_liveness_and_registers(quads, k_registers=k_registers)

    # Initialize active VM with these quads
    global _active_vm
    _active_vm = core.VirtualMachine(quads)

    return jsonify({
        "success": raw_res.get("success", False),
        "duration_ms": compile_duration,
        "diagnostics": raw_res.get("diagnostics", []),
        "tokens": raw_res.get("tokens", []),
        "quads": quads,
        "tac": tac,
        "triples": triples,
        "indirect_triples": indirect_triples,
        "backpatch_logs": raw_res.get("backpatch_logs", []),
        "cfg": cfg,
        "registers": reg_analysis
    })

@app.route("/api/optimize", methods=["POST"])
def optimize():
    data = request.get_json() or {}
    quads = data.get("quads", [])
    enabled_passes = data.get("passes", None)

    opt_result = core.optimize_ir(quads, enabled_passes=enabled_passes)
    optimized_quads = opt_result["optimized_quads"]
    optimized_tac = core.format_tac_instructions(optimized_quads)
    optimized_cfg = core.construct_cfg(optimized_quads)
    opt_reg_analysis = core.analyze_liveness_and_registers(optimized_quads)

    return jsonify({
        "success": True,
        "original_quads": quads,
        "optimized_quads": optimized_quads,
        "optimized_tac": optimized_tac,
        "optimized_cfg": optimized_cfg,
        "optimized_registers": opt_reg_analysis,
        "logs": opt_result["logs"],
        "stats": opt_result["stats"]
    })

@app.route("/api/execute", methods=["POST"])
def execute_all():
    global _active_vm
    data = request.get_json() or {}
    quads = data.get("quads")
    max_steps = int(data.get("max_steps", 2000))

    if quads is not None:
        _active_vm = core.VirtualMachine(quads, max_steps=max_steps)
    elif _active_vm is None:
        return jsonify({"success": False, "error": "No program compiled or quads provided."}), 400

    state = _active_vm.run_to_completion()
    return jsonify({"success": True, "state": state})

@app.route("/api/execute/step", methods=["POST"])
def execute_step():
    global _active_vm
    data = request.get_json() or {}
    direction = data.get("direction", "forward")  # "forward" or "backward"

    if _active_vm is None:
        quads = data.get("quads", [])
        if quads:
            _active_vm = core.VirtualMachine(quads)
        else:
            return jsonify({"success": False, "error": "Virtual machine not initialized."}), 400

    if direction == "backward":
        state = _active_vm.step_backward()
    else:
        state = _active_vm.step_forward()

    return jsonify({"success": True, "state": state})

@app.route("/api/execute/reset", methods=["POST"])
def execute_reset():
    global _active_vm
    data = request.get_json() or {}
    quads = data.get("quads", [])
    if _active_vm:
        _active_vm = core.VirtualMachine(_active_vm.quads)
    elif quads:
        _active_vm = core.VirtualMachine(quads)
    else:
        return jsonify({"success": True, "state": None})

    return jsonify({"success": True, "state": _active_vm.get_state()})

@app.route("/api/tests/run", methods=["POST"])
def run_tests():
    # Dedicated test suite runner
    test_results = run_comprehensive_tests()
    return jsonify(test_results)

def run_comprehensive_tests():
    tests = [
        {
            "id": "T01",
            "name": "Nested While Loop Regression (Review 1 Benchmark)",
            "code": SAMPLES["test_nested.c"]["code"],
            "expected_vars": {"i": 4, "sum": 12, "j": 3},
            "expected_outputs": ["1", "2", "4", "6", "9", "12"]
        },
        {
            "id": "T02",
            "name": "For Loop Traversal & Step Execution",
            "code": SAMPLES["test_for_loops.c"]["code"],
            "expected_vars": {"i": 6, "sum": 15},
            "expected_outputs": ["0", "1", "3", "6", "10", "15"]
        },
        {
            "id": "T03",
            "name": "If/Else Conditional True Path",
            "code": "x = 5; if (x < 10) { ans = 1; } else { ans = 2; }",
            "expected_vars": {"x": 5, "ans": 1},
            "expected_outputs": []
        },
        {
            "id": "T04",
            "name": "If/Else Conditional False Path",
            "code": "x = 15; if (x < 10) { ans = 1; } else { ans = 2; }",
            "expected_vars": {"x": 15, "ans": 2},
            "expected_outputs": []
        },
        {
            "id": "T05",
            "name": "Arithmetic Precedence & Operators",
            "code": "r = 2 + 3 * 4; print(r);",
            "expected_vars": {"r": 14},
            "expected_outputs": ["14"]
        },
        {
            "id": "T06",
            "name": "Zero-iteration While Loop",
            "code": "i = 10; while (i < 5) { i = i + 1; }",
            "expected_vars": {"i": 10},
            "expected_outputs": []
        },
        {
            "id": "T07",
            "name": "Constant Folding Optimization Verification",
            "code": "a = 2 + 3 * 4; print(a);",
            "expected_vars": {"a": 14},
            "expected_outputs": ["14"],
            "check_optimization": True
        },
        {
            "id": "T08",
            "name": "Syntax Error Diagnostic Detection",
            "code": "x = 10; y = ",
            "expect_error": True
        },
        {
            "id": "T09",
            "name": "Lexical Error Character Reporting",
            "code": "x = @ + 1;",
            "expect_error": True
        },
        {
            "id": "T10",
            "name": "Step Limit Safeguard (Infinite Loop)",
            "code": "while (1 <= 1) { x = 1; }",
            "expect_limit": True
        }
    ]

    results = []
    passed_count = 0

    for t in tests:
        t_start = time.perf_counter()
        res = core.invoke_native_compiler(t["code"])
        duration = round((time.perf_counter() - t_start) * 1000, 2)

        if t.get("expect_error"):
            passed = not res.get("success", False)
            results.append({
                "id": t["id"],
                "name": t["name"],
                "status": "PASS" if passed else "FAIL",
                "duration_ms": duration,
                "message": "Correctly diagnosed invalid source." if passed else "Failed to report diagnostic error."
            })
            if passed: passed_count += 1
            continue

        if not res.get("success", False):
            results.append({
                "id": t["id"],
                "name": t["name"],
                "status": "FAIL",
                "duration_ms": duration,
                "message": f"Compilation failed: {res.get('diagnostics')}"
            })
            continue

        quads = res.get("quads", [])
        if t.get("check_optimization"):
            opt_res = core.optimize_ir(quads)
            quads = opt_res["optimized_quads"]

        vm = core.VirtualMachine(quads, max_steps=1000)
        state = vm.run_to_completion()

        if t.get("expect_limit"):
            passed = state["status"] == "LIMIT_EXCEEDED"
            results.append({
                "id": t["id"],
                "name": t["name"],
                "status": "PASS" if passed else "FAIL",
                "duration_ms": duration,
                "message": "VM correctly intercepted infinite loop with step-limit safeguard." if passed else "VM failed to limit steps."
            })
            if passed: passed_count += 1
            continue

        # Check vars & outputs
        passed = True
        reason = "All assertions passed."
        for var, exp_val in t.get("expected_vars", {}).items():
            act_val = state["user_variables"].get(var)
            if act_val != exp_val:
                passed = False
                reason = f"Variable '{var}' expected {exp_val}, got {act_val}"
                break

        if passed and "expected_outputs" in t:
            if state["console_output"] != t["expected_outputs"]:
                passed = False
                reason = f"Output expected {t['expected_outputs']}, got {state['console_output']}"

        results.append({
            "id": t["id"],
            "name": t["name"],
            "status": "PASS" if passed else "FAIL",
            "duration_ms": duration,
            "message": reason
        })
        if passed: passed_count += 1

    return {
        "total": len(tests),
        "passed": passed_count,
        "failed": len(tests) - passed_count,
        "results": results
    }

if __name__ == "__main__":
    print("Starting Compiler Studio API on http://127.0.0.1:5000")
    app.run(host="127.0.0.1", port=5000, debug=False)
