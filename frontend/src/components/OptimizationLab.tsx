import React, { useState } from 'react';
import { Quad, OptimizationResult } from '../types';
import { Sparkles, CheckCircle2, ArrowRight, Zap, RefreshCw } from 'lucide-react';

interface Props {
  quads: Quad[];
  onApplyOptimization: (passes: string[]) => void;
  optResult: OptimizationResult | null;
  isLoading: boolean;
}

export const OptimizationLab: React.FC<Props> = ({
  quads,
  onApplyOptimization,
  optResult,
  isLoading,
}) => {
  const [enabledPasses, setEnabledPasses] = useState<string[]>([
    'constant_folding',
    'constant_propagation',
    'cse',
    'dead_code_elimination',
  ]);

  const togglePass = (passKey: string) => {
    setEnabledPasses((prev) =>
      prev.includes(passKey) ? prev.filter((p) => p !== passKey) : [...prev, passKey]
    );
  };

  const handleRun = () => {
    onApplyOptimization(enabledPasses);
  };

  const passes = [
    { key: 'constant_folding', name: 'Constant Folding', desc: 'Pre-evaluates static arithmetic expressions (e.g. 2 + 3 * 4 -> 14)' },
    { key: 'constant_propagation', name: 'Constant Propagation', desc: 'Substitutes known constant values into subsequent variable uses' },
    { key: 'cse', name: 'Common Subexpression Elimination (CSE)', desc: 'Reuses earlier computed results for identical operations' },
    { key: 'dead_code_elimination', name: 'Dead Code Elimination (DCE)', desc: 'Prunes temporary instructions whose results have no live consumers' },
  ];

  return (
    <div style={{ display: 'flex', flexDirection: 'column', height: '100%', width: '100%', background: 'var(--bg-primary)', overflow: 'hidden' }}>
      {/* Header Controls */}
      <div
        style={{
          padding: '12px 18px',
          borderBottom: '1px solid var(--border-color)',
          background: 'var(--bg-secondary)',
          display: 'flex',
          justifyContent: 'space-between',
          alignItems: 'center',
          flexShrink: 0,
        }}
      >
        <div style={{ display: 'flex', alignItems: 'center', gap: 10 }}>
          <Sparkles size={20} style={{ color: 'var(--accent-amber)' }} />
          <span style={{ fontWeight: 600, fontSize: '15px' }}>Deterministic Optimization Pipeline</span>
        </div>
        <button
          className="btn btn-primary"
          onClick={handleRun}
          disabled={isLoading || quads.length === 0}
        >
          {isLoading ? <RefreshCw size={15} className="spin" /> : <Zap size={15} />}
          Apply Selected Passes
        </button>
      </div>

      <div style={{ display: 'grid', gridTemplateColumns: 'minmax(280px, 320px) 1fr', flex: 1, minHeight: 0, overflow: 'hidden' }}>
        {/* Pass Configuration Panel */}
        <div
          style={{
            borderRight: '1px solid var(--border-color)',
            background: 'var(--bg-secondary)',
            padding: '16px',
            overflowY: 'auto',
            minHeight: 0,
          }}
        >
          <div style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: 14 }}>
            CONFIGURABLE PASSES
          </div>
          <div style={{ display: 'flex', flexDirection: 'column', gap: 12 }}>
            {passes.map((p) => {
              const isChecked = enabledPasses.includes(p.key);
              return (
                <div
                  key={p.key}
                  onClick={() => togglePass(p.key)}
                  style={{
                    padding: '12px 14px',
                    borderRadius: 'var(--radius-sm)',
                    background: isChecked ? 'var(--bg-tertiary)' : 'transparent',
                    border: isChecked ? '1px solid var(--accent-blue)' : '1px solid var(--border-color)',
                    cursor: 'pointer',
                  }}
                >
                  <div style={{ display: 'flex', alignItems: 'center', gap: 10, marginBottom: 6 }}>
                    <input
                      type="checkbox"
                      checked={isChecked}
                      onChange={() => {}}
                      style={{ cursor: 'pointer', width: 16, height: 16 }}
                    />
                    <span style={{ fontSize: '14px', fontWeight: 600, color: 'var(--text-primary)' }}>
                      {p.name}
                    </span>
                  </div>
                  <div style={{ fontSize: '12px', color: 'var(--text-muted)', lineHeight: 1.45, paddingLeft: 26 }}>
                    {p.desc}
                  </div>
                </div>
              );
            })}
          </div>

          {optResult && (
            <div style={{ marginTop: 24, padding: '14px', background: 'var(--bg-tertiary)', borderRadius: 'var(--radius-sm)' }}>
              <div style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: 10 }}>
                TRANSFORMATION METRICS
              </div>
              <div style={{ fontSize: '13px', display: 'flex', flexDirection: 'column', gap: 8 }}>
                <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                  <span style={{ color: 'var(--text-muted)' }}>Original Quads:</span>
                  <span style={{ fontWeight: 700 }}>{optResult.stats.original_count}</span>
                </div>
                <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                  <span style={{ color: 'var(--text-muted)' }}>Optimized Quads:</span>
                  <span style={{ fontWeight: 700, color: 'var(--accent-green)' }}>{optResult.stats.optimized_count}</span>
                </div>
                <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                  <span style={{ color: 'var(--text-muted)' }}>Transformations:</span>
                  <span style={{ fontWeight: 700, color: 'var(--accent-amber)' }}>{optResult.stats.transformations}</span>
                </div>
              </div>
            </div>
          )}
        </div>

        {/* Side-by-Side Comparison & Transformation Logs */}
        <div style={{ display: 'flex', flexDirection: 'column', minHeight: 0, overflow: 'hidden' }}>
          {optResult ? (
            <div style={{ flex: 1, display: 'grid', gridTemplateRows: '1fr 200px', minHeight: 0, overflow: 'hidden' }}>
              {/* Side-by-Side TAC View */}
              <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', borderBottom: '1px solid var(--border-color)', minHeight: 0, overflowY: 'auto' }}>
                {/* Original */}
                <div style={{ padding: '14px', borderRight: '1px solid var(--border-color)' }}>
                  <div style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: 10 }}>
                    ORIGINAL THREE-ADDRESS CODE
                  </div>
                  <div className="mono" style={{ fontSize: '13px', lineHeight: 1.7 }}>
                    {optResult.original_quads.map((q) => (
                      <div key={q.id} style={{ color: 'var(--text-primary)', padding: '2px 0' }}>
                        <span style={{ color: 'var(--accent-cyan)', fontWeight: 600 }}>L{q.id}:</span>{' '}
                        {q.op === '=' ? `${q.result} = ${q.arg1}` : `${q.result} = ${q.arg1} ${q.op} ${q.arg2}`}
                      </div>
                    ))}
                  </div>
                </div>

                {/* Optimized */}
                <div style={{ padding: '14px', background: 'rgba(16, 185, 129, 0.03)' }}>
                  <div style={{ fontSize: '13px', fontWeight: 600, color: 'var(--accent-green)', marginBottom: 10 }}>
                    OPTIMIZED THREE-ADDRESS CODE
                  </div>
                  <div className="mono" style={{ fontSize: '13px', lineHeight: 1.7 }}>
                    {optResult.optimized_tac.map((inst) => (
                      <div key={inst.id} style={{ color: 'var(--text-primary)', padding: '2px 0' }}>
                        <span style={{ color: 'var(--accent-cyan)', fontWeight: 600 }}>{inst.label}:</span> {inst.text}
                      </div>
                    ))}
                  </div>
                </div>
              </div>

              {/* Transformation Log */}
              <div style={{ padding: '14px', overflowY: 'auto', background: 'var(--bg-secondary)', minHeight: 0 }}>
                <div style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: 10 }}>
                  TRANSFORMATION AUDIT LOG ({optResult.logs.length} actions)
                </div>
                {optResult.logs.length === 0 ? (
                  <div style={{ fontSize: '13px', color: 'var(--text-muted)' }}>
                    No transformation candidates found in current program.
                  </div>
                ) : (
                  <div style={{ display: 'flex', flexDirection: 'column', gap: 8 }}>
                    {optResult.logs.map((log, i) => (
                      <div
                        key={i}
                        style={{
                          fontSize: '13px',
                          display: 'flex',
                          alignItems: 'center',
                          gap: 12,
                          padding: '7px 10px',
                          background: 'var(--bg-tertiary)',
                          borderRadius: 'var(--radius-sm)',
                        }}
                      >
                        <CheckCircle2 size={16} style={{ color: 'var(--accent-green)' }} />
                        <span className="badge badge-amber">{log.pass}</span>
                        <span style={{ color: 'var(--text-primary)' }}>{log.desc}</span>
                        <span style={{ color: 'var(--text-muted)', marginLeft: 'auto', fontSize: '12px' }}>
                          inst #{log.instruction_id}
                        </span>
                      </div>
                    ))}
                  </div>
                )}
              </div>
            </div>
          ) : (
            <div style={{ flex: 1, display: 'flex', alignItems: 'center', justifyContent: 'center', color: 'var(--text-muted)', fontSize: '14px' }}>
              Click "Apply Selected Passes" to inspect optimized code and transformation logs.
            </div>
          )}
        </div>
      </div>
    </div>
  );
};
