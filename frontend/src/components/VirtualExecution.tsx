import React, { useState } from 'react';
import { VMState, Quad } from '../types';
import { Play, Pause, StepForward, StepBack, RotateCcw, AlertTriangle, Terminal, Cpu } from 'lucide-react';

interface Props {
  vmState: VMState | null;
  quads: Quad[];
  onStepForward: () => void;
  onStepBackward: () => void;
  onRunAll: () => void;
  onReset: () => void;
  isLoading: boolean;
}

export const VirtualExecution: React.FC<Props> = ({
  vmState,
  quads,
  onStepForward,
  onStepBackward,
  onRunAll,
  onReset,
  isLoading,
}) => {
  return (
    <div style={{ display: 'flex', flexDirection: 'column', height: '100%', width: '100%', background: 'var(--bg-primary)', overflow: 'hidden' }}>
      {/* Control Toolbar */}
      <div
        style={{
          padding: '12px 18px',
          borderBottom: '1px solid var(--border-color)',
          background: 'var(--bg-secondary)',
          display: 'flex',
          gap: 12,
          alignItems: 'center',
          flexShrink: 0,
        }}
      >
        <button className="btn btn-primary" onClick={onRunAll} disabled={isLoading || vmState?.is_halted}>
          <Play size={15} fill="currentColor" /> Run to Completion
        </button>
        <button className="btn btn-secondary" onClick={onStepForward} disabled={isLoading || vmState?.is_halted}>
          <StepForward size={15} /> Step Forward
        </button>
        <button className="btn btn-secondary" onClick={onStepBackward} disabled={isLoading || !vmState?.can_step_back}>
          <StepBack size={15} /> Step Backward (Reversible)
        </button>
        <button className="btn btn-secondary" onClick={onReset} disabled={isLoading}>
          <RotateCcw size={15} /> Reset
        </button>

        <div style={{ marginLeft: 'auto', display: 'flex', gap: 14, alignItems: 'center', fontSize: '13px' }}>
          <span style={{ color: 'var(--text-muted)' }}>Status:</span>
          <span
            className={`badge ${
              vmState?.status === 'COMPLETED'
                ? 'badge-green'
                : vmState?.status === 'LIMIT_EXCEEDED'
                ? 'badge-red'
                : 'badge-blue'
            }`}
          >
            {vmState?.status || 'READY'}
          </span>
          <span style={{ color: 'var(--text-muted)' }}>Steps:</span>
          <span style={{ fontWeight: 700 }}>{vmState?.steps || 0}</span>
          <span style={{ color: 'var(--text-muted)' }}>PC:</span>
          <span className="mono" style={{ color: 'var(--accent-cyan)', fontWeight: 700 }}>L{vmState?.pc ?? 0}</span>
        </div>
      </div>

      {/* Main Execution View */}
      <div style={{ display: 'grid', gridTemplateColumns: '1.2fr 1fr', flex: 1, minHeight: 0, overflow: 'hidden' }}>
        {/* Left: TAC with Program Counter indicator */}
        <div style={{ borderRight: '1px solid var(--border-color)', display: 'flex', flexDirection: 'column', minHeight: 0 }}>
          <div style={{ padding: '10px 14px', background: 'var(--bg-tertiary)', borderBottom: '1px solid var(--border-color)', fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)' }}>
            INTERMEDIATE CODE & EXECUTION TRACE
          </div>
          <div style={{ flex: 1, minHeight: 0, overflowY: 'auto', padding: '14px', background: 'var(--bg-secondary)' }} className="mono">
            {quads.map((q) => {
              const isCurrent = vmState?.pc === q.id;
              let text = '';
              if (q.op === '=') text = `${q.result} = ${q.arg1}`;
              else if (q.op === 'print') text = `print ${q.arg1}`;
              else if (q.op === 'print_str') text = `print ${q.arg1}`;
              else if (q.op === 'goto') text = `goto L${q.result}`;
              else if (['<', '>', '<=', '>=', '==', '!='].includes(q.op))
                text = `if ${q.arg1} ${q.op} ${q.arg2} goto L${q.result}`;
              else text = `${q.result} = ${q.arg1} ${q.op} ${q.arg2}`;

              return (
                <div
                  key={q.id}
                  style={{
                    padding: '5px 10px',
                    borderRadius: 'var(--radius-sm)',
                    background: isCurrent ? 'rgba(59, 130, 246, 0.25)' : 'transparent',
                    borderLeft: isCurrent ? '4px solid var(--accent-blue)' : '4px solid transparent',
                    display: 'flex',
                    alignItems: 'center',
                    gap: 14,
                    fontSize: '13.5px',
                  }}
                >
                  <span style={{ color: isCurrent ? 'var(--accent-blue)' : 'var(--text-muted)', width: 24, fontSize: '14px' }}>
                    {isCurrent ? '▶' : ''}
                  </span>
                  <span style={{ color: 'var(--accent-cyan)', width: 44, fontWeight: 600 }}>L{q.id}:</span>
                  <span style={{ color: isCurrent ? '#ffffff' : 'var(--text-primary)', fontWeight: isCurrent ? 700 : 400 }}>
                    {text}
                  </span>
                </div>
              );
            })}
          </div>
        </div>

        {/* Right: Runtime Memory & Console Output */}
        <div style={{ display: 'grid', gridTemplateRows: '1fr 1fr', minHeight: 0, overflow: 'hidden' }}>
          {/* Virtual Variable Store */}
          <div style={{ borderBottom: '1px solid var(--border-color)', padding: '14px', minHeight: 0, overflowY: 'auto' }}>
            <div style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: 10, display: 'flex', alignItems: 'center', gap: 7 }}>
              <Cpu size={15} /> VIRTUAL VARIABLE STORE (MEMORY MAP)
            </div>
            {Object.keys(vmState?.user_variables || {}).length === 0 ? (
              <div style={{ fontSize: '13px', color: 'var(--text-muted)', fontStyle: 'italic' }}>
                No variables initialized yet.
              </div>
            ) : (
              <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13px' }} className="mono">
                <thead>
                  <tr style={{ borderBottom: '1px solid var(--border-color)', color: 'var(--text-muted)', background: 'var(--bg-tertiary)' }}>
                    <th style={{ padding: '6px 10px', textAlign: 'left' }}>Variable</th>
                    <th style={{ padding: '6px 10px', textAlign: 'left' }}>Current Value</th>
                    <th style={{ padding: '6px 10px', textAlign: 'left' }}>Type</th>
                  </tr>
                </thead>
                <tbody>
                  {Object.entries(vmState?.user_variables || {}).map(([name, val]) => (
                    <tr key={name} style={{ borderBottom: '1px solid var(--border-color)' }}>
                      <td style={{ padding: '7px 10px', color: 'var(--accent-cyan)', fontWeight: 600 }}>{name}</td>
                      <td style={{ padding: '7px 10px', color: 'var(--accent-green)', fontWeight: 700, fontSize: '14px' }}>{val}</td>
                      <td style={{ padding: '7px 10px', color: 'var(--text-muted)' }}>int (32-bit)</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            )}
          </div>

          {/* Console Output */}
          <div style={{ padding: '14px', display: 'flex', flexDirection: 'column', background: '#0a0c10', minHeight: 0 }}>
            <div style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: 10, display: 'flex', alignItems: 'center', gap: 7 }}>
              <Terminal size={15} /> RUNTIME CONSOLE OUTPUT
            </div>
            <div
              style={{
                flex: 1,
                minHeight: 0,
                overflowY: 'auto',
                fontSize: '13px',
                lineHeight: 1.7,
                color: 'var(--accent-green)',
                fontFamily: 'var(--font-mono)',
              }}
            >
              {vmState?.console_output && vmState.console_output.length > 0 ? (
                vmState.console_output.map((out, idx) => (
                  <div key={idx} style={{ display: 'flex', gap: 10 }}>
                    <span style={{ color: 'var(--text-muted)' }}>&gt;</span>
                    <span style={{ fontWeight: 600 }}>{out}</span>
                  </div>
                ))
              ) : (
                <span style={{ color: 'var(--text-muted)' }}>No console output produced yet.</span>
              )}
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};
