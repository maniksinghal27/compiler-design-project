import React, { useState } from 'react';
import { TACInstruction, Quad, Triple, IndirectTriple, BackpatchLog } from '../types';
import { Layers, Table, GitCommit, ArrowRight, Play, RotateCcw } from 'lucide-react';

interface Props {
  tac: TACInstruction[];
  quads: Quad[];
  triples: Triple[];
  indirectTriples: IndirectTriple[];
  backpatchLogs: BackpatchLog[];
  selectedQuadId?: number;
  onSelectQuad?: (id: number) => void;
}

export const IRExplorer: React.FC<Props> = ({
  tac,
  quads,
  triples,
  indirectTriples,
  backpatchLogs,
  selectedQuadId,
  onSelectQuad,
}) => {
  const [activeTab, setActiveTab] = useState<'tac' | 'quads' | 'triples' | 'indirect' | 'backpatch'>('tac');
  const [bpReplayIndex, setBpReplayIndex] = useState<number>(-1);

  return (
    <div style={{ display: 'flex', flexDirection: 'column', height: '100%', width: '100%', background: 'var(--bg-secondary)', overflow: 'hidden' }}>
      {/* Tab Navigation */}
      <div
        style={{
          display: 'flex',
          borderBottom: '1px solid var(--border-color)',
          background: 'var(--bg-tertiary)',
          padding: '0 12px',
          overflowX: 'auto',
          flexShrink: 0,
        }}
      >
        <button
          onClick={() => setActiveTab('tac')}
          style={{
            padding: '11px 16px',
            fontSize: '13.5px',
            fontWeight: 600,
            background: 'transparent',
            color: activeTab === 'tac' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            borderBottom: activeTab === 'tac' ? '2px solid var(--accent-blue)' : 'none',
            whiteSpace: 'nowrap',
          }}
        >
          Three-Address Code (TAC)
        </button>
        <button
          onClick={() => setActiveTab('quads')}
          style={{
            padding: '11px 16px',
            fontSize: '13.5px',
            fontWeight: 600,
            background: 'transparent',
            color: activeTab === 'quads' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            borderBottom: activeTab === 'quads' ? '2px solid var(--accent-blue)' : 'none',
            whiteSpace: 'nowrap',
          }}
        >
          Quadruples ({quads.length})
        </button>
        <button
          onClick={() => setActiveTab('triples')}
          style={{
            padding: '11px 16px',
            fontSize: '13.5px',
            fontWeight: 600,
            background: 'transparent',
            color: activeTab === 'triples' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            borderBottom: activeTab === 'triples' ? '2px solid var(--accent-blue)' : 'none',
            whiteSpace: 'nowrap',
          }}
        >
          Triples
        </button>
        <button
          onClick={() => setActiveTab('indirect')}
          style={{
            padding: '11px 16px',
            fontSize: '13.5px',
            fontWeight: 600,
            background: 'transparent',
            color: activeTab === 'indirect' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            borderBottom: activeTab === 'indirect' ? '2px solid var(--accent-blue)' : 'none',
            whiteSpace: 'nowrap',
          }}
        >
          Indirect Triples
        </button>
        <button
          onClick={() => setActiveTab('backpatch')}
          style={{
            padding: '11px 16px',
            fontSize: '13.5px',
            fontWeight: 600,
            background: 'transparent',
            color: activeTab === 'backpatch' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            borderBottom: activeTab === 'backpatch' ? '2px solid var(--accent-blue)' : 'none',
            whiteSpace: 'nowrap',
          }}
        >
          Backpatch Inspector ({backpatchLogs.length})
        </button>
      </div>

      {/* Tab Contents */}
      <div style={{ flex: 1, minHeight: 0, overflowY: 'auto', padding: '16px' }}>
        {/* TAC VIEW */}
        {activeTab === 'tac' && (
          <div className="mono" style={{ fontSize: '14px', lineHeight: 1.8 }}>
            {tac.map((inst) => {
              const isSelected = selectedQuadId === inst.id;
              return (
                <div
                  key={inst.id}
                  onClick={() => onSelectQuad && onSelectQuad(inst.id)}
                  style={{
                    padding: '5px 10px',
                    borderRadius: 'var(--radius-sm)',
                    background: isSelected ? 'var(--bg-hover)' : 'transparent',
                    borderLeft: isSelected ? '3px solid var(--accent-blue)' : '3px solid transparent',
                    cursor: 'pointer',
                    display: 'flex',
                    alignItems: 'baseline',
                    gap: 16,
                  }}
                >
                  <span style={{ color: 'var(--accent-cyan)', width: 44, textAlign: 'right', fontWeight: 600 }}>
                    {inst.label}:
                  </span>
                  <span style={{ color: 'var(--text-primary)' }}>{inst.text}</span>
                  <span style={{ color: 'var(--text-muted)', fontSize: '12px', marginLeft: 'auto' }}>
                    line {inst.line}
                  </span>
                </div>
              );
            })}
          </div>
        )}

        {/* QUADRUPLES TABLE */}
        {activeTab === 'quads' && (
          <div style={{ overflowX: 'auto' }}>
            <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13.5px', textAlign: 'left' }} className="mono">
              <thead>
                <tr style={{ borderBottom: '1px solid var(--border-color)', color: 'var(--text-muted)', background: 'var(--bg-tertiary)' }}>
                  <th style={{ padding: '8px 12px' }}>#</th>
                  <th style={{ padding: '8px 12px' }}>Op</th>
                  <th style={{ padding: '8px 12px' }}>Arg1</th>
                  <th style={{ padding: '8px 12px' }}>Arg2</th>
                  <th style={{ padding: '8px 12px' }}>Result</th>
                  <th style={{ padding: '8px 12px' }}>Src Line</th>
                </tr>
              </thead>
              <tbody>
                {quads.map((q) => {
                  const isSelected = selectedQuadId === q.id;
                  return (
                    <tr
                      key={q.id}
                      onClick={() => onSelectQuad && onSelectQuad(q.id)}
                      style={{
                        borderBottom: '1px solid var(--border-color)',
                        background: isSelected ? 'var(--bg-hover)' : 'transparent',
                        cursor: 'pointer',
                      }}
                    >
                      <td style={{ padding: '8px 12px', color: 'var(--accent-blue)', fontWeight: 600 }}>{q.id}</td>
                      <td style={{ padding: '8px 12px', color: 'var(--accent-purple)', fontWeight: 600 }}>{q.op}</td>
                      <td style={{ padding: '8px 12px', color: 'var(--text-primary)' }}>{q.arg1 || '—'}</td>
                      <td style={{ padding: '8px 12px', color: 'var(--text-primary)' }}>{q.arg2 || '—'}</td>
                      <td style={{ padding: '8px 12px', color: 'var(--accent-green)', fontWeight: 600 }}>{q.result || '—'}</td>
                      <td style={{ padding: '8px 12px', color: 'var(--text-muted)' }}>{q.line || 1}</td>
                    </tr>
                  );
                })}
              </tbody>
            </table>
          </div>
        )}

        {/* TRIPLES TABLE */}
        {activeTab === 'triples' && (
          <div style={{ overflowX: 'auto' }}>
            <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13.5px', textAlign: 'left' }} className="mono">
              <thead>
                <tr style={{ borderBottom: '1px solid var(--border-color)', color: 'var(--text-muted)', background: 'var(--bg-tertiary)' }}>
                  <th style={{ padding: '8px 12px' }}>Index</th>
                  <th style={{ padding: '8px 12px' }}>Op</th>
                  <th style={{ padding: '8px 12px' }}>Arg1</th>
                  <th style={{ padding: '8px 12px' }}>Arg2</th>
                </tr>
              </thead>
              <tbody>
                {triples.map((t) => (
                  <tr key={t.index} style={{ borderBottom: '1px solid var(--border-color)' }}>
                    <td style={{ padding: '8px 12px', color: 'var(--accent-cyan)', fontWeight: 600 }}>({t.index})</td>
                    <td style={{ padding: '8px 12px', color: 'var(--accent-purple)', fontWeight: 600 }}>{t.op}</td>
                    <td style={{ padding: '8px 12px', color: 'var(--text-primary)' }}>{t.arg1 || '—'}</td>
                    <td style={{ padding: '8px 12px', color: 'var(--text-primary)' }}>{t.arg2 || '—'}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        )}

        {/* INDIRECT TRIPLES */}
        {activeTab === 'indirect' && (
          <div style={{ display: 'grid', gridTemplateColumns: 'minmax(200px, 1fr) 2fr', gap: 20 }}>
            <div>
              <div style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: 10 }}>
                Execution Order Pointer Table
              </div>
              <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13px' }} className="mono">
                <thead>
                  <tr style={{ borderBottom: '1px solid var(--border-color)', color: 'var(--text-muted)', background: 'var(--bg-tertiary)' }}>
                    <th style={{ padding: '6px 10px' }}>Order</th>
                    <th style={{ padding: '6px 10px' }}>Pointer</th>
                  </tr>
                </thead>
                <tbody>
                  {indirectTriples.map((it) => (
                    <tr key={it.order} style={{ borderBottom: '1px solid var(--border-color)' }}>
                      <td style={{ padding: '6px 10px', color: 'var(--text-muted)' }}>{it.order}</td>
                      <td style={{ padding: '6px 10px', color: 'var(--accent-blue)', fontWeight: 600 }}>({it.triple_pointer})</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>

            <div>
              <div style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: 10 }}>
                Underlying Triples Table
              </div>
              <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13px' }} className="mono">
                <thead>
                  <tr style={{ borderBottom: '1px solid var(--border-color)', color: 'var(--text-muted)', background: 'var(--bg-tertiary)' }}>
                    <th style={{ padding: '6px 10px' }}>Idx</th>
                    <th style={{ padding: '6px 10px' }}>Op</th>
                    <th style={{ padding: '6px 10px' }}>Arg1</th>
                    <th style={{ padding: '6px 10px' }}>Arg2</th>
                  </tr>
                </thead>
                <tbody>
                  {indirectTriples.map((it) => (
                    <tr key={it.order} style={{ borderBottom: '1px solid var(--border-color)' }}>
                      <td style={{ padding: '6px 10px', color: 'var(--accent-cyan)', fontWeight: 600 }}>({it.triple.index})</td>
                      <td style={{ padding: '6px 10px', color: 'var(--accent-purple)', fontWeight: 600 }}>{it.triple.op}</td>
                      <td style={{ padding: '6px 10px' }}>{it.triple.arg1 || '—'}</td>
                      <td style={{ padding: '6px 10px' }}>{it.triple.arg2 || '—'}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>
        )}

        {/* BACKPATCH INSPECTOR */}
        {activeTab === 'backpatch' && (
          <div>
            <div style={{ display: 'flex', gap: 10, marginBottom: 16, alignItems: 'center' }}>
              <span style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)' }}>
                Step-Through Backpatching Trace ({backpatchLogs.length} events)
              </span>
              <button
                className="btn btn-secondary"
                onClick={() => setBpReplayIndex((prev) => Math.min(prev + 1, backpatchLogs.length - 1))}
                disabled={bpReplayIndex >= backpatchLogs.length - 1}
              >
                <ArrowRight size={15} /> Step Event
              </button>
              <button
                className="btn btn-secondary"
                onClick={() => setBpReplayIndex(-1)}
              >
                <RotateCcw size={15} /> Reset
              </button>
            </div>

            <div style={{ display: 'flex', flexDirection: 'column', gap: 8 }}>
              {backpatchLogs.map((log, idx) => {
                const isActive = bpReplayIndex === idx;
                const isPassed = bpReplayIndex >= idx;
                const isBackpatch = log.event === 'backpatch';

                return (
                  <div
                    key={idx}
                    onClick={() => {
                      setBpReplayIndex(idx);
                      if (onSelectQuad) onSelectQuad(log.from_quad);
                    }}
                    style={{
                      padding: '10px 14px',
                      borderRadius: 'var(--radius-sm)',
                      background: isActive
                        ? 'rgba(59, 130, 246, 0.15)'
                        : isPassed
                        ? 'var(--bg-tertiary)'
                        : 'transparent',
                      border: isActive ? '1px solid var(--accent-blue)' : '1px solid var(--border-color)',
                      cursor: 'pointer',
                      display: 'flex',
                      alignItems: 'center',
                      gap: 14,
                      fontSize: '13px',
                    }}
                    className="mono"
                  >
                    <span className={`badge ${isBackpatch ? 'badge-green' : 'badge-blue'}`}>
                      {log.event.toUpperCase()}
                    </span>
                    <span style={{ color: 'var(--text-primary)', flex: 1 }}>{log.desc}</span>
                    {log.target_quad >= 0 && (
                      <span style={{ color: 'var(--accent-amber)', fontSize: '12px', fontWeight: 600 }}>
                        Target: L{log.target_quad}
                      </span>
                    )}
                  </div>
                );
              })}
            </div>
          </div>
        )}
      </div>
    </div>
  );
};
