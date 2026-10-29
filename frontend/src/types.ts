export interface TokenRecord {
  index: number;
  type: string;
  lexeme: string;
  line: number;
  col: number;
}

export interface Diagnostic {
  severity: 'error' | 'warning' | 'info';
  message: string;
  line: number;
  col: number;
}

export interface Quad {
  id: number;
  op: string;
  arg1: string;
  arg2: string;
  result: string;
  line?: number;
  col?: number;
}

export interface TACInstruction {
  id: number;
  label: string;
  text: string;
  raw_quad: Quad;
  line: number;
  col: number;
}

export interface Triple {
  index: number;
  op: string;
  arg1: string;
  arg2: string;
  line?: number;
}

export interface IndirectTriple {
  order: number;
  triple_pointer: number;
  triple: Triple;
}

export interface BackpatchLog {
  event: string;
  from_quad: number;
  target_quad: number;
  desc: string;
}

export interface CFGBlock {
  id: string;
  index: number;
  start_quad: number;
  end_quad: number;
  instructions: Quad[];
  predecessors: string[];
  successors: string[];
  is_entry: boolean;
}

export interface CFGEdge {
  source: string;
  target: string;
  kind: 'true' | 'false' | 'goto' | 'fallthrough';
  label: string;
  is_backedge?: boolean;
}

export interface NaturalLoop {
  header: string;
  latch: string;
  blocks: string[];
}

export interface CFGData {
  blocks: CFGBlock[];
  edges: CFGEdge[];
  loops: NaturalLoop[];
}

export interface LiveRange {
  start: number;
  end: number;
  duration: number;
}

export interface RegisterAnalysis {
  k_registers: number;
  available_registers: string[];
  allocations: Record<string, string>;
  spills: Array<{ variable: string; reason: string }>;
  live_ranges: Record<string, LiveRange>;
  interference_nodes: Array<{ id: string; is_temp: boolean; assigned_reg: string }>;
  interference_edges: Array<{ source: string; target: string }>;
  peak_live: number;
}

export interface CompilationResult {
  success: boolean;
  duration_ms: number;
  diagnostics: Diagnostic[];
  tokens: TokenRecord[];
  quads: Quad[];
  tac: TACInstruction[];
  triples: Triple[];
  indirect_triples: IndirectTriple[];
  backpatch_logs: BackpatchLog[];
  cfg: CFGData;
  registers: RegisterAnalysis;
}

export interface VMState {
  pc: number;
  steps: number;
  current_instruction: Quad | null;
  status: string;
  is_halted: boolean;
  can_step_back: boolean;
  user_variables: Record<string, number>;
  temporary_variables: Record<string, number>;
  memory: Record<string, number>;
  console_output: string[];
  raw_output: Array<{ pc: number; text: string }>;
}

export interface OptimizationLog {
  pass: string;
  instruction_id: number;
  desc: string;
}

export interface OptimizationResult {
  success: boolean;
  original_quads: Quad[];
  optimized_quads: Quad[];
  optimized_tac: TACInstruction[];
  optimized_cfg: CFGData;
  optimized_registers: RegisterAnalysis;
  logs: OptimizationLog[];
  stats: {
    original_count: number;
    optimized_count: number;
    transformations: number;
  };
}
