import React from 'react';
import { RegisterAnalysis } from '../types';
import { Layers, Activity, Server, AlertCircle } from 'lucide-react';

interface Props {
  analysis: RegisterAnalysis;
  quadCount: number;
}

export const RegisterStudio: React.FC<Props> = ({ analysis, quadCount }) => {
  return (
    <div style={{ display: 'flex', flexDirection: 'column', height: '100%', width: '100%', background: 'var(--bg-primary)', overflowY: 'auto', padding: '18px' }}>
      {/* Header Metric Cards */}
      <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(220px, 1fr))', gap: 14, marginBottom: 18 }}>
        <div style={{ background: 'var(--bg-secondary)', padding: '14px', borderRadius: 'var(--radius-sm)', border: '1px solid var(--border-color)' }}>
          <div style={{ fontSize: '12px', color: 'var(--text-muted)', marginBottom: 6, fontWeight: 600 }}>AVAILABLE REGISTERS</div>
          <div style={{ fontSize: '24px', fontWeight: 700, color: 'var(--accent-blue)' }}>K = {analysis.k_registers}</div>
          <div style={{ fontSize: '12.5px', color: 'var(--text-secondary)', marginTop: 4 }}>{analysis.available_registers.join(', ')}</div>
        </div>

        <div style={{ background: 'var(--bg-secondary)', padding: '14px', borderRadius: 'var(--radius-sm)', border: '1px solid var(--border-color)' }}>
          <div style={{ fontSize: '12px', color: 'var(--text-muted)', marginBottom: 6, fontWeight: 600 }}>PEAK LIVE VARIABLES</div>
          <div style={{ fontSize: '24px', fontWeight: 700, color: 'var(--accent-cyan)' }}>{analysis.peak_live}</div>
          <div style={{ fontSize: '12.5px', color: 'var(--text-secondary)', marginTop: 4 }}>Simultaneous live ranges</div>
        </div>

        <div style={{ background: 'var(--bg-secondary)', padding: '14px', borderRadius: 'var(--radius-sm)', border: '1px solid var(--border-color)' }}>
          <div style={{ fontSize: '12px', color: 'var(--text-muted)', marginBottom: 6, fontWeight: 600 }}>REGISTER ALLOCATIONS</div>
          <div style={{ fontSize: '24px', fontWeight: 700, color: 'var(--accent-green)' }}>
            {Object.keys(analysis.allocations).length}
          </div>
          <div style={{ fontSize: '12.5px', color: 'var(--text-secondary)', marginTop: 4 }}>Successfully colored</div>
        </div>

        <div style={{ background: 'var(--bg-secondary)', padding: '14px', borderRadius: 'var(--radius-sm)', border: '1px solid var(--border-color)' }}>
          <div style={{ fontSize: '12px', color: 'var(--text-muted)', marginBottom: 6, fontWeight: 600 }}>SPILL CANDIDATES</div>
          <div style={{ fontSize: '24px', fontWeight: 700, color: analysis.spills.length > 0 ? 'var(--accent-red)' : 'var(--accent-green)' }}>
            {analysis.spills.length}
          </div>
          <div style={{ fontSize: '12.5px', color: 'var(--text-secondary)', marginTop: 4 }}>Spilled to memory stack</div>
        </div>
      </div>

      <div style={{ display: 'grid', gridTemplateColumns: '1.2fr 1fr', gap: 18 }}>
        {/* Liveness Table & Coloring */}
        <div style={{ background: 'var(--bg-secondary)', padding: '16px', borderRadius: 'var(--radius-sm)', border: '1px solid var(--border-color)' }}>
          <div style={{ fontSize: '13.5px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: 14, display: 'flex', alignItems: 'center', gap: 8 }}>
            <Activity size={16} /> LIVE RANGES & CHAITIN-BRIGGS ALLOCATION
          </div>
          <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13px' }} className="mono">
            <thead>
              <tr style={{ borderBottom: '1px solid var(--border-color)', color: 'var(--text-muted)', background: 'var(--bg-tertiary)' }}>
                <th style={{ padding: '8px 10px', textAlign: 'left' }}>Variable</th>
                <th style={{ padding: '8px 10px', textAlign: 'left' }}>Type</th>
                <th style={{ padding: '8px 10px', textAlign: 'left' }}>Live Range</th>
                <th style={{ padding: '8px 10px', textAlign: 'left' }}>Assigned Hardware Reg</th>
              </tr>
            </thead>
            <tbody>
              {Object.entries(analysis.live_ranges).map(([v, range]) => {
                const reg = analysis.allocations[v];
                const isTemp = v.startsWith('t');
                return (
                  <tr key={v} style={{ borderBottom: '1px solid var(--border-color)' }}>
                    <td style={{ padding: '8px 10px', fontWeight: 600, color: isTemp ? 'var(--accent-purple)' : 'var(--accent-cyan)' }}>
                      {v}
                    </td>
                    <td style={{ padding: '8px 10px', color: 'var(--text-muted)' }}>
                      {isTemp ? 'Compiler Temp' : 'Program Var'}
                    </td>
                    <td style={{ padding: '8px 10px', color: 'var(--text-primary)' }}>
                      [inst {range.start} → {range.end}] ({range.duration} insts)
                    </td>
                    <td style={{ padding: '8px 10px' }}>
                      {reg ? (
                        <span className="badge badge-green">{reg}</span>
                      ) : (
                        <span className="badge badge-red">SPILL</span>
                      )}
                    </td>
                  </tr>
                );
              })}
            </tbody>
          </table>
        </div>

        {/* Interference Edges Matrix */}
        <div style={{ background: 'var(--bg-secondary)', padding: '16px', borderRadius: 'var(--radius-sm)', border: '1px solid var(--border-color)' }}>
          <div style={{ fontSize: '13.5px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: 14, display: 'flex', alignItems: 'center', gap: 8 }}>
            <Server size={16} /> REGISTER INTERFERENCE GRAPH EDGES ({analysis.interference_edges.length} edges)
          </div>
          <div style={{ fontSize: '13px', color: 'var(--text-muted)', marginBottom: 12 }}>
            Variables that are simultaneously live cannot share the same hardware register:
          </div>
          <div style={{ maxHeight: '340px', overflowY: 'auto', display: 'flex', flexWrap: 'wrap', gap: 8 }}>
            {analysis.interference_edges.map((e, idx) => (
              <div
                key={idx}
                className="mono"
                style={{
                  padding: '6px 10px',
                  background: 'var(--bg-tertiary)',
                  borderRadius: 'var(--radius-sm)',
                  fontSize: '12px',
                  border: '1px solid var(--border-color)',
                  display: 'flex',
                  alignItems: 'center',
                  gap: 8,
                }}
              >
                <span style={{ color: 'var(--accent-cyan)', fontWeight: 600 }}>{e.source}</span>
                <span style={{ color: 'var(--accent-amber)' }}> interferes with</span>
                <span style={{ color: 'var(--accent-cyan)', fontWeight: 600 }}>{e.target}</span>
              </div>
            ))}
          </div>
        </div>
      </div>
    </div>
  );
};
