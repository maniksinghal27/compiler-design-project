import React, { useEffect, useRef } from 'react';
import cytoscape from 'cytoscape';
import { CFGData } from '../types';
import { Maximize2, ZoomIn, ZoomOut, RefreshCw } from 'lucide-react';

interface Props {
  cfg: CFGData;
  activeQuadId?: number;
  onSelectBlock?: (blockId: string) => void;
  selectedBlockId?: string;
}

export const CFGStudio: React.FC<Props> = ({
  cfg,
  activeQuadId,
  onSelectBlock,
  selectedBlockId,
}) => {
  const containerRef = useRef<HTMLDivElement>(null);
  const cyRef = useRef<cytoscape.Core | null>(null);

  useEffect(() => {
    if (!containerRef.current || !cfg.blocks || cfg.blocks.length === 0) return;

    // Convert CFG blocks to Cytoscape nodes
    const elements: any[] = [];

    // Identify which blocks are loop headers
    const loopHeaders = new Set(cfg.loops.map((l) => l.header));
    const loopBlocks = new Set(cfg.loops.flatMap((l) => l.blocks));

    cfg.blocks.forEach((b) => {
      // Build block label
      const lines = [
        `${b.id} [L${b.start_quad}..L${b.end_quad}]`,
        '------------------',
        ...b.instructions.map((q) => {
          if (q.op === '=') return `${q.result} = ${q.arg1}`;
          if (q.op === 'print') return `print ${q.arg1}`;
          if (q.op === 'print_str') return `print ${q.arg1}`;
          if (q.op === 'goto') return `goto L${q.result}`;
          if (['<', '>', '<=', '>=', '==', '!='].includes(q.op))
            return `if ${q.arg1} ${q.op} ${q.arg2} goto L${q.result}`;
          return `${q.result} = ${q.arg1} ${q.op} ${q.arg2}`;
        }),
      ];

      const isHeader = loopHeaders.has(b.id);
      const isLoop = loopBlocks.has(b.id);

      elements.push({
        data: {
          id: b.id,
          label: lines.join('\n'),
          blockId: b.id,
          isHeader,
          isLoop,
        },
      });
    });

    // Edges
    cfg.edges.forEach((e, idx) => {
      elements.push({
        data: {
          id: `edge_${idx}_${e.source}_${e.target}`,
          source: e.source,
          target: e.target,
          label: e.label,
          kind: e.kind,
          is_backedge: e.is_backedge,
        },
      });
    });

    const cy = cytoscape({
      container: containerRef.current,
      elements,
      style: [
        {
          selector: 'node',
          style: {
            shape: 'round-rectangle',
            'background-color': '#1a202c',
            'border-width': 2,
            'border-color': '#3b82f6',
            label: 'data(label)',
            color: '#e2e8f0',
            'font-family': 'JetBrains Mono, monospace',
            'font-size': '12.5px',
            'line-height': 1.4,
            'text-wrap': 'wrap',
            'text-valign': 'center',
            'text-halign': 'center',
            width: 'label',
            height: 'label',
            padding: '16px',
          },
        },
        {
          selector: 'node[?isHeader]',
          style: {
            'border-color': '#f59e0b',
            'border-width': 3,
          },
        },
        {
          selector: 'node:selected',
          style: {
            'border-color': '#10b981',
            'border-width': 4,
            'background-color': '#243042',
          },
        },
        {
          selector: 'edge',
          style: {
            width: 2.5,
            'curve-style': 'bezier',
            'target-arrow-shape': 'triangle',
            'arrow-scale': 1.3,
            label: 'data(label)',
            'font-size': '11.5px',
            'font-family': 'JetBrains Mono, monospace',
            'text-rotation': 'autorotate',
            'text-margin-y': -8,
            color: '#94a3b8',
          },
        },
        {
          selector: 'edge[kind = "true"]',
          style: {
            'line-color': '#10b981',
            'target-arrow-color': '#10b981',
            color: '#10b981',
          },
        },
        {
          selector: 'edge[kind = "false"]',
          style: {
            'line-color': '#ef4444',
            'target-arrow-color': '#ef4444',
            color: '#ef4444',
          },
        },
        {
          selector: 'edge[kind = "goto"]',
          style: {
            'line-color': '#3b82f6',
            'target-arrow-color': '#3b82f6',
            color: '#3b82f6',
          },
        },
        {
          selector: 'edge[kind = "fallthrough"]',
          style: {
            'line-color': '#64748b',
            'target-arrow-color': '#64748b',
            color: '#94a3b8',
          },
        },
        {
          selector: 'edge[?is_backedge]',
          style: {
            'line-style': 'dashed',
            'line-color': '#f59e0b',
            'target-arrow-color': '#f59e0b',
            width: 3,
            color: '#f59e0b',
          },
        },
      ],
      layout: {
        name: 'breadthfirst',
        directed: true,
        padding: 40,
        spacingFactor: 1.5,
      },
    });

    cy.on('tap', 'node', (evt) => {
      const node = evt.target;
      if (onSelectBlock) {
        onSelectBlock(node.data('blockId'));
      }
    });

    cyRef.current = cy;

    return () => {
      cy.destroy();
    };
  }, [cfg]);

  // Highlight block corresponding to active Quad or selected block
  useEffect(() => {
    if (!cyRef.current) return;
    const cy = cyRef.current;

    cy.nodes().unselect();

    let targetBlockId = selectedBlockId;
    if (!targetBlockId && activeQuadId !== undefined && cfg.blocks) {
      const found = cfg.blocks.find(
        (b) => activeQuadId >= b.start_quad && activeQuadId <= b.end_quad
      );
      if (found) targetBlockId = found.id;
    }

    if (targetBlockId) {
      const node = cy.getElementById(targetBlockId);
      if (node) {
        node.select();
      }
    }
  }, [selectedBlockId, activeQuadId, cfg]);

  const handleFit = () => cyRef.current?.fit(undefined, 30);
  const handleZoomIn = () => cyRef.current?.zoom(cyRef.current.zoom() * 1.25);
  const handleZoomOut = () => cyRef.current?.zoom(cyRef.current.zoom() * 0.8);
  const handleResetLayout = () => {
    cyRef.current?.layout({ name: 'breadthfirst', directed: true, padding: 40, spacingFactor: 1.5 }).run();
  };

  if (!cfg.blocks || cfg.blocks.length === 0) {
    return (
      <div style={{ height: '100%', display: 'flex', alignItems: 'center', justifyContent: 'center', color: 'var(--text-muted)' }}>
        No Control-Flow Graph available. Compile source code first.
      </div>
    );
  }

  return (
    <div style={{ position: 'relative', width: '100%', height: '100%', background: 'var(--bg-primary)' }}>
      {/* Canvas */}
      <div ref={containerRef} style={{ width: '100%', height: '100%' }} />

      {/* Graph Toolbar Controls */}
      <div
        style={{
          position: 'absolute',
          top: 16,
          right: 16,
          display: 'flex',
          gap: 6,
          background: 'var(--bg-secondary)',
          padding: '6px 8px',
          borderRadius: 'var(--radius-md)',
          border: '1px solid var(--border-color)',
          boxShadow: '0 4px 12px rgba(0,0,0,0.3)',
          zIndex: 10,
        }}
      >
        <button className="btn btn-secondary" onClick={handleZoomIn} title="Zoom In">
          <ZoomIn size={14} />
        </button>
        <button className="btn btn-secondary" onClick={handleZoomOut} title="Zoom Out">
          <ZoomOut size={14} />
        </button>
        <button className="btn btn-secondary" onClick={handleFit} title="Fit to Screen">
          <Maximize2 size={14} />
        </button>
        <button className="btn btn-secondary" onClick={handleResetLayout} title="Reset Layout">
          <RefreshCw size={14} />
        </button>
      </div>

      {/* Legend */}
      <div
        style={{
          position: 'absolute',
          bottom: 16,
          left: 16,
          background: 'var(--bg-secondary)',
          padding: '10px 14px',
          borderRadius: 'var(--radius-md)',
          border: '1px solid var(--border-color)',
          fontSize: '11px',
          color: 'var(--text-secondary)',
          boxShadow: '0 4px 12px rgba(0,0,0,0.3)',
          zIndex: 10,
        }}
      >
        <div style={{ fontWeight: 600, color: 'var(--text-primary)', marginBottom: 6 }}>Edge Semantics</div>
        <div style={{ display: 'flex', flexDirection: 'column', gap: 4 }}>
          <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
            <span style={{ width: 12, height: 3, background: '#10b981', display: 'inline-block' }} />
            <span>TRUE (Branch Taken)</span>
          </div>
          <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
            <span style={{ width: 12, height: 3, background: '#ef4444', display: 'inline-block' }} />
            <span>FALSE (Branch Fallthrough)</span>
          </div>
          <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
            <span style={{ width: 12, height: 3, background: '#3b82f6', display: 'inline-block' }} />
            <span>GOTO (Unconditional Jump)</span>
          </div>
          <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
            <span style={{ width: 12, height: 3, background: '#f59e0b', borderTop: '2px dashed #f59e0b', display: 'inline-block' }} />
            <span>Natural Loop Back-Edge</span>
          </div>
        </div>
      </div>
    </div>
  );
};
