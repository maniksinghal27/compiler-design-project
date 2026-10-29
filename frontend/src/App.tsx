import React, { useState, useEffect, useRef } from 'react';
import Editor from '@monaco-editor/react';
import {
  Code,
  Layers,
  GitMerge,
  Sparkles,
  Play,
  Cpu,
  CheckSquare,
  Sun,
  Moon,
  AlertCircle,
  FileCode,
  CheckCircle2,
  PanelRightClose,
  PanelRightOpen,
  PanelBottomClose,
  PanelBottomOpen,
  RotateCcw,
  Layout,
  RefreshCw,
} from 'lucide-react';

import { CFGStudio } from './components/CFGStudio';
import { IRExplorer } from './components/IRExplorer';
import { OptimizationLab } from './components/OptimizationLab';
import { VirtualExecution } from './components/VirtualExecution';
import { RegisterStudio } from './components/RegisterStudio';
import { TestBench } from './components/TestBench';

import { CompilationResult, OptimizationResult, VMState } from './types';

export const App: React.FC = () => {
  const [theme, setTheme] = useState<'dark' | 'light'>(() => {
    return (localStorage.getItem('cs_theme') as 'dark' | 'light') || 'dark';
  });

  // Core 7 workspaces
  const [activeTab, setActiveTab] = useState<
    'source' | 'ir' | 'cfg' | 'optimization' | 'execution' | 'registers' | 'testbench'
  >('source');

  // Source & Samples
  const [samples, setSamples] = useState<Record<string, { title: string; code: string; description: string }>>({});
  const [selectedSample, setSelectedSample] = useState<string>('test_nested.c');
  const [code, setCode] = useState<string>('');

  // Compiler state
  const [compilationResult, setCompilationResult] = useState<CompilationResult | null>(null);
  const [isCompiling, setIsCompiling] = useState<boolean>(false);
  const [isRunningCode, setIsRunningCode] = useState<boolean>(false);
  const [selectedQuadId, setSelectedQuadId] = useState<number | undefined>(undefined);

  // Optimizer state
  const [optResult, setOptResult] = useState<OptimizationResult | null>(null);
  const [isOptimizing, setIsOptimizing] = useState<boolean>(false);

  // Virtual Machine state
  const [vmState, setVmState] = useState<VMState | null>(null);
  const [isVmRunning, setIsVmRunning] = useState<boolean>(false);

  // Layout Preferences State (Persisted)
  const [rightPanelWidth, setRightPanelWidth] = useState<number>(() => {
    const saved = localStorage.getItem('cs_right_panel_width');
    return saved ? parseInt(saved, 10) : 380;
  });
  const [bottomPanelHeight, setBottomPanelHeight] = useState<number>(() => {
    const saved = localStorage.getItem('cs_bottom_panel_height');
    return saved ? parseInt(saved, 10) : 260;
  });
  const [panelPlacement, setPanelPlacement] = useState<'right' | 'bottom'>(() => {
    return (localStorage.getItem('cs_panel_placement') as 'right' | 'bottom') || 'right';
  });
  const [panelCollapsed, setPanelCollapsed] = useState<boolean>(() => {
    return localStorage.getItem('cs_panel_collapsed') === 'true';
  });
  const [activeSideSubTab, setActiveSideSubTab] = useState<'diagnostics' | 'tokens'>('diagnostics');

  // Resizing state
  const [isDragging, setIsDragging] = useState<boolean>(false);
  const splitRef = useRef<HTMLDivElement>(null);

  // Fetch samples on load
  useEffect(() => {
    fetch('/api/samples')
      .then((res) => res.json())
      .then((data) => {
        setSamples(data);
        if (data['test_nested.c']) {
          setCode(data['test_nested.c'].code);
        }
      })
      .catch((e) => console.error('Failed to load samples:', e));
  }, []);

  // Theme switch effect
  useEffect(() => {
    document.documentElement.setAttribute('data-theme', theme);
    localStorage.setItem('cs_theme', theme);
  }, [theme]);

  // Persist layout choices
  useEffect(() => {
    localStorage.setItem('cs_right_panel_width', rightPanelWidth.toString());
  }, [rightPanelWidth]);

  useEffect(() => {
    localStorage.setItem('cs_bottom_panel_height', bottomPanelHeight.toString());
  }, [bottomPanelHeight]);

  useEffect(() => {
    localStorage.setItem('cs_panel_placement', panelPlacement);
  }, [panelPlacement]);

  useEffect(() => {
    localStorage.setItem('cs_panel_collapsed', panelCollapsed.toString());
  }, [panelCollapsed]);

  // Load sample code
  const handleSelectSample = (sampleKey: string) => {
    setSelectedSample(sampleKey);
    if (samples[sampleKey]) {
      setCode(samples[sampleKey].code);
    }
  };

  // Compile helper returning data
  const performCompile = async (): Promise<CompilationResult | null> => {
    setIsCompiling(true);
    try {
      const res = await fetch('/api/compile', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ code }),
      });
      const data: CompilationResult = await res.json();
      setCompilationResult(data);
      setOptResult(null); // reset stale optimization
      return data;
    } catch (e) {
      console.error(e);
      return null;
    } finally {
      setIsCompiling(false);
    }
  };

  // Compile Button action
  const handleCompile = async () => {
    const data = await performCompile();
    if (data && data.quads && data.quads.length > 0) {
      // Reset VM state with fresh quads
      const vmRes = await fetch('/api/execute/reset', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ quads: data.quads }),
      });
      const vmData = await vmRes.json();
      setVmState(vmData.state);
    }
  };

  // Run Code Action directly from Source Workshop
  const handleRunCode = async () => {
    if (isCompiling || isRunningCode) return;
    setIsRunningCode(true);

    try {
      const data = await performCompile();

      if (!data) {
        setIsRunningCode(false);
        return;
      }

      // If compilation failed with errors, stay in Source Workshop to display diagnostics
      if (!data.success || !data.quads || data.quads.length === 0) {
        // Expand panel if collapsed so user sees the diagnostic immediately
        if (panelCollapsed) setPanelCollapsed(false);
        setActiveSideSubTab('diagnostics');
        setIsRunningCode(false);
        return;
      }

      // Initialize Virtual Machine with freshly compiled quads
      const resetRes = await fetch('/api/execute/reset', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ quads: data.quads }),
      });
      const resetData = await resetRes.json();
      setVmState(resetData.state);

      // Execute program immediately to show actual execution results
      const execRes = await fetch('/api/execute', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ quads: data.quads }),
      });
      const execData = await execRes.json();
      setVmState(execData.state);

      // Seamlessly navigate to Virtual Execution view
      setActiveTab('execution');
    } catch (err) {
      console.error('Error during Run Code workflow:', err);
    } finally {
      setIsRunningCode(false);
    }
  };

  // Reset layout to sensible defaults
  const handleResetLayout = () => {
    setRightPanelWidth(380);
    setBottomPanelHeight(260);
    setPanelPlacement('right');
    setPanelCollapsed(false);
  };

  // Handle panel divider drag resizing
  const startResizing = (e: React.MouseEvent) => {
    e.preventDefault();
    setIsDragging(true);

    const startX = e.clientX;
    const startY = e.clientY;
    const initialWidth = rightPanelWidth;
    const initialHeight = bottomPanelHeight;

    const onMouseMove = (moveEvent: MouseEvent) => {
      if (panelPlacement === 'right') {
        const deltaX = startX - moveEvent.clientX;
        const newWidth = Math.max(260, Math.min(window.innerWidth - 350, initialWidth + deltaX));
        setRightPanelWidth(newWidth);
      } else {
        const deltaY = startY - moveEvent.clientY;
        const newHeight = Math.max(160, Math.min(window.innerHeight - 250, initialHeight + deltaY));
        setBottomPanelHeight(newHeight);
      }
    };

    const onMouseUp = () => {
      setIsDragging(false);
      window.removeEventListener('mousemove', onMouseMove);
      window.removeEventListener('mouseup', onMouseUp);
    };

    window.addEventListener('mousemove', onMouseMove);
    window.addEventListener('mouseup', onMouseUp);
  };

  // Optimization Handler
  const handleApplyOptimization = async (passes: string[]) => {
    if (!compilationResult || !compilationResult.quads) return;
    setIsOptimizing(true);
    try {
      const res = await fetch('/api/optimize', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ quads: compilationResult.quads, passes }),
      });
      const data: OptimizationResult = await res.json();
      setOptResult(data);
    } catch (e) {
      console.error(e);
    } finally {
      setIsOptimizing(false);
    }
  };

  // VM Stepping
  const handleVmStep = async (direction: 'forward' | 'backward') => {
    try {
      const res = await fetch('/api/execute/step', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ direction }),
      });
      const data = await res.json();
      setVmState(data.state);
    } catch (e) {
      console.error(e);
    }
  };

  const handleVmRunAll = async () => {
    setIsVmRunning(true);
    try {
      const res = await fetch('/api/execute', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ quads: compilationResult?.quads }),
      });
      const data = await res.json();
      setVmState(data.state);
    } catch (e) {
      console.error(e);
    } finally {
      setIsVmRunning(false);
    }
  };

  const handleVmReset = async () => {
    try {
      const res = await fetch('/api/execute/reset', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ quads: compilationResult?.quads }),
      });
      const data = await res.json();
      setVmState(data.state);
    } catch (e) {
      console.error(e);
    }
  };

  return (
    <div style={{ display: 'flex', flexDirection: 'column', height: '100vh', width: '100vw' }}>
      {/* Top Application Header - Cleaned Branding */}
      <header
        style={{
          height: '52px',
          background: 'var(--bg-secondary)',
          borderBottom: '1px solid var(--border-color)',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'space-between',
          padding: '0 20px',
          flexShrink: 0,
        }}
      >
        <div style={{ display: 'flex', alignItems: 'center', gap: 12 }}>
          <Code size={22} style={{ color: 'var(--accent-blue)' }} />
          <span style={{ fontWeight: 700, fontSize: '17px', letterSpacing: '-0.2px' }}>
            Compiler Studio
          </span>
        </div>

        {/* Global Compile Status & Actions */}
        <div style={{ display: 'flex', alignItems: 'center', gap: 12 }}>
          {compilationResult && (
            <div style={{ display: 'flex', alignItems: 'center', gap: 8, fontSize: '13px' }}>
              {compilationResult.success ? (
                <span className="badge badge-green" style={{ display: 'inline-flex', gap: 6, alignItems: 'center' }}>
                  <CheckCircle2 size={14} /> Compiled ({compilationResult.duration_ms}ms)
                </span>
              ) : (
                <span className="badge badge-red" style={{ display: 'inline-flex', gap: 6, alignItems: 'center' }}>
                  <AlertCircle size={14} /> {compilationResult.diagnostics.length} Diagnostic Error(s)
                </span>
              )}
            </div>
          )}

          {/* Run Code Button */}
          <button
            className="btn btn-success"
            onClick={handleRunCode}
            disabled={isCompiling || isRunningCode}
            title="Compile code and automatically run in Virtual Execution"
          >
            {isRunningCode ? <RefreshCw size={15} className="spin" /> : <Play size={15} fill="currentColor" />}
            <span>Run Code</span>
          </button>

          {/* Compile Button */}
          <button
            className="btn btn-primary"
            onClick={handleCompile}
            disabled={isCompiling || isRunningCode}
            title="Compile source into TAC and Quadruples"
          >
            {isCompiling ? <RefreshCw size={15} className="spin" /> : <Code size={15} />}
            <span>Compile</span>
          </button>

          {/* Theme Switcher */}
          <button
            className="btn-icon"
            onClick={() => setTheme(theme === 'dark' ? 'light' : 'dark')}
            title="Toggle theme"
          >
            {theme === 'dark' ? <Sun size={16} /> : <Moon size={16} />}
          </button>
        </div>
      </header>

      {/* Main Workspace Navigation Bar - Exactly the 7 Core Workspaces */}
      <nav
        style={{
          height: '42px',
          background: 'var(--bg-tertiary)',
          borderBottom: '1px solid var(--border-color)',
          display: 'flex',
          padding: '0 12px',
          gap: 6,
          alignItems: 'center',
          overflowX: 'auto',
          flexShrink: 0,
        }}
      >
        <button
          onClick={() => setActiveTab('source')}
          style={{
            padding: '7px 14px',
            borderRadius: 'var(--radius-sm)',
            fontSize: '13.5px',
            fontWeight: 600,
            display: 'flex',
            alignItems: 'center',
            gap: 7,
            background: activeTab === 'source' ? 'var(--bg-secondary)' : 'transparent',
            color: activeTab === 'source' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            whiteSpace: 'nowrap',
          }}
        >
          <FileCode size={16} /> Source Workshop
        </button>

        <button
          onClick={() => setActiveTab('ir')}
          style={{
            padding: '7px 14px',
            borderRadius: 'var(--radius-sm)',
            fontSize: '13.5px',
            fontWeight: 600,
            display: 'flex',
            alignItems: 'center',
            gap: 7,
            background: activeTab === 'ir' ? 'var(--bg-secondary)' : 'transparent',
            color: activeTab === 'ir' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            whiteSpace: 'nowrap',
          }}
        >
          <Layers size={16} /> Intermediate Representations
        </button>

        <button
          onClick={() => setActiveTab('cfg')}
          style={{
            padding: '7px 14px',
            borderRadius: 'var(--radius-sm)',
            fontSize: '13.5px',
            fontWeight: 600,
            display: 'flex',
            alignItems: 'center',
            gap: 7,
            background: activeTab === 'cfg' ? 'var(--bg-secondary)' : 'transparent',
            color: activeTab === 'cfg' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            whiteSpace: 'nowrap',
          }}
        >
          <GitMerge size={16} /> Control Flow Graph Studio
        </button>

        <button
          onClick={() => setActiveTab('optimization')}
          style={{
            padding: '7px 14px',
            borderRadius: 'var(--radius-sm)',
            fontSize: '13.5px',
            fontWeight: 600,
            display: 'flex',
            alignItems: 'center',
            gap: 7,
            background: activeTab === 'optimization' ? 'var(--bg-secondary)' : 'transparent',
            color: activeTab === 'optimization' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            whiteSpace: 'nowrap',
          }}
        >
          <Sparkles size={16} /> Optimization
        </button>

        <button
          onClick={() => setActiveTab('execution')}
          style={{
            padding: '7px 14px',
            borderRadius: 'var(--radius-sm)',
            fontSize: '13.5px',
            fontWeight: 600,
            display: 'flex',
            alignItems: 'center',
            gap: 7,
            background: activeTab === 'execution' ? 'var(--bg-secondary)' : 'transparent',
            color: activeTab === 'execution' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            whiteSpace: 'nowrap',
          }}
        >
          <Play size={16} /> Virtual Execution
        </button>

        <button
          onClick={() => setActiveTab('registers')}
          style={{
            padding: '7px 14px',
            borderRadius: 'var(--radius-sm)',
            fontSize: '13.5px',
            fontWeight: 600,
            display: 'flex',
            alignItems: 'center',
            gap: 7,
            background: activeTab === 'registers' ? 'var(--bg-secondary)' : 'transparent',
            color: activeTab === 'registers' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            whiteSpace: 'nowrap',
          }}
        >
          <Cpu size={16} /> Register & Liveness Analysis
        </button>

        <button
          onClick={() => setActiveTab('testbench')}
          style={{
            padding: '7px 14px',
            borderRadius: 'var(--radius-sm)',
            fontSize: '13.5px',
            fontWeight: 600,
            display: 'flex',
            alignItems: 'center',
            gap: 7,
            background: activeTab === 'testbench' ? 'var(--bg-secondary)' : 'transparent',
            color: activeTab === 'testbench' ? 'var(--accent-blue)' : 'var(--text-secondary)',
            whiteSpace: 'nowrap',
          }}
        >
          <CheckSquare size={16} /> Test Bench
        </button>
      </nav>

      {/* Main Workspace Body */}
      <main style={{ flex: 1, minHeight: 0, minWidth: 0, overflow: 'hidden', display: 'flex' }}>
        {/* WORKSPACE 1: SOURCE WORKSHOP (Resizable & Movable Panels) */}
        {activeTab === 'source' && (
          <div
            ref={splitRef}
            style={{
              display: 'flex',
              flexDirection: panelPlacement === 'right' ? 'row' : 'column',
              width: '100%',
              height: '100%',
              minWidth: 0,
              minHeight: 0,
              overflow: 'hidden',
            }}
          >
            {/* Editor Area */}
            <div
              style={{
                flex: 1,
                minWidth: 0,
                minHeight: 0,
                display: 'flex',
                flexDirection: 'column',
                overflow: 'hidden',
              }}
            >
              {/* Presets and Editor Toolbar */}
              <div
                style={{
                  padding: '9px 16px',
                  background: 'var(--bg-secondary)',
                  borderBottom: '1px solid var(--border-color)',
                  display: 'flex',
                  alignItems: 'center',
                  justifyContent: 'space-between',
                  gap: 12,
                  flexShrink: 0,
                }}
              >
                <div style={{ display: 'flex', alignItems: 'center', gap: 10 }}>
                  <span style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)' }}>
                    Sample Preset:
                  </span>
                  <select
                    value={selectedSample}
                    onChange={(e) => handleSelectSample(e.target.value)}
                    style={{
                      padding: '5px 10px',
                      borderRadius: 'var(--radius-sm)',
                      background: 'var(--bg-tertiary)',
                      color: 'var(--text-primary)',
                      border: '1px solid var(--border-color)',
                      fontSize: '13px',
                      fontFamily: 'var(--font-sans)',
                    }}
                  >
                    {Object.entries(samples).map(([k, s]) => (
                      <option key={k} value={k}>
                        {s.title} ({k})
                      </option>
                    ))}
                  </select>
                </div>

                {/* Workspace Layout Controls */}
                <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
                  <button
                    className="btn-icon"
                    onClick={() => setPanelPlacement(panelPlacement === 'right' ? 'bottom' : 'right')}
                    title={panelPlacement === 'right' ? 'Move diagnostics to bottom' : 'Move diagnostics to right side'}
                  >
                    <Layout size={15} />
                  </button>
                  <button
                    className="btn-icon"
                    onClick={() => setPanelCollapsed(!panelCollapsed)}
                    title={panelCollapsed ? 'Expand diagnostics panel' : 'Collapse diagnostics panel'}
                  >
                    {panelPlacement === 'right' ? (
                      panelCollapsed ? <PanelRightOpen size={15} /> : <PanelRightClose size={15} />
                    ) : (
                      panelCollapsed ? <PanelBottomOpen size={15} /> : <PanelBottomClose size={15} />
                    )}
                  </button>
                  <button
                    className="btn-icon"
                    onClick={handleResetLayout}
                    title="Reset workspace layout to default"
                  >
                    <RotateCcw size={15} />
                  </button>
                </div>
              </div>

              {/* Monaco Editor Container */}
              <div style={{ flex: 1, minHeight: 0, minWidth: 0, position: 'relative' }}>
                <Editor
                  height="100%"
                  defaultLanguage="c"
                  theme={theme === 'dark' ? 'vs-dark' : 'light'}
                  value={code}
                  onChange={(val) => setCode(val || '')}
                  options={{
                    fontSize: 15,
                    lineHeight: 24,
                    fontFamily: 'JetBrains Mono, monospace',
                    minimap: { enabled: false },
                    scrollBeyondLastLine: false,
                    lineNumbers: 'on',
                    tabSize: 4,
                    renderLineHighlight: 'all',
                    cursorBlinking: 'smooth',
                  }}
                />
              </div>
            </div>

            {/* Resizer Handle */}
            {!panelCollapsed && (
              <div
                className={panelPlacement === 'right' ? 'split-resizer-horizontal' : 'split-resizer-vertical'}
                onMouseDown={startResizing}
                title="Drag to resize panel"
              />
            )}

            {/* COMPILER DIAGNOSTICS & TOKENS PANEL */}
            {!panelCollapsed && (
              <div
                style={{
                  width: panelPlacement === 'right' ? `${rightPanelWidth}px` : '100%',
                  height: panelPlacement === 'bottom' ? `${bottomPanelHeight}px` : '100%',
                  minWidth: panelPlacement === 'right' ? '260px' : '100%',
                  minHeight: panelPlacement === 'bottom' ? '160px' : '100%',
                  background: 'var(--bg-secondary)',
                  display: 'flex',
                  flexDirection: 'column',
                  flexShrink: 0,
                  overflow: 'hidden',
                }}
              >
                {/* Panel Tab Strip */}
                <div
                  style={{
                    background: 'var(--bg-tertiary)',
                    borderBottom: '1px solid var(--border-color)',
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'space-between',
                    padding: '0 8px',
                    flexShrink: 0,
                  }}
                >
                  <div style={{ display: 'flex' }}>
                    <button
                      onClick={() => setActiveSideSubTab('diagnostics')}
                      style={{
                        padding: '10px 14px',
                        fontSize: '13px',
                        fontWeight: 600,
                        background: 'transparent',
                        color: activeSideSubTab === 'diagnostics' ? 'var(--accent-blue)' : 'var(--text-secondary)',
                        borderBottom: activeSideSubTab === 'diagnostics' ? '2px solid var(--accent-blue)' : '2px solid transparent',
                        display: 'flex',
                        alignItems: 'center',
                        gap: 6,
                      }}
                    >
                      <AlertCircle size={15} />
                      <span>Diagnostics ({compilationResult?.diagnostics?.length || 0})</span>
                    </button>
                    <button
                      onClick={() => setActiveSideSubTab('tokens')}
                      style={{
                        padding: '10px 14px',
                        fontSize: '13px',
                        fontWeight: 600,
                        background: 'transparent',
                        color: activeSideSubTab === 'tokens' ? 'var(--accent-blue)' : 'var(--text-secondary)',
                        borderBottom: activeSideSubTab === 'tokens' ? '2px solid var(--accent-blue)' : '2px solid transparent',
                        display: 'flex',
                        alignItems: 'center',
                        gap: 6,
                      }}
                    >
                      <Layers size={15} />
                      <span>Tokens ({compilationResult?.tokens?.length || 0})</span>
                    </button>
                  </div>

                  <button
                    className="btn-icon"
                    onClick={() => setPanelCollapsed(true)}
                    title="Collapse panel"
                  >
                    {panelPlacement === 'right' ? <PanelRightClose size={14} /> : <PanelBottomClose size={14} />}
                  </button>
                </div>

                {/* Subtab Contents with Independent Scrolling */}
                <div style={{ flex: 1, minHeight: 0, overflowY: 'auto', padding: '14px' }}>
                  {activeSideSubTab === 'diagnostics' && (
                    <div>
                      {compilationResult?.diagnostics && compilationResult.diagnostics.length > 0 ? (
                        <div style={{ display: 'flex', flexDirection: 'column', gap: 10 }}>
                          {compilationResult.diagnostics.map((d, i) => (
                            <div
                              key={i}
                              style={{
                                padding: '10px 12px',
                                background: 'rgba(239, 68, 68, 0.1)',
                                borderLeft: '4px solid var(--accent-red)',
                                borderRadius: 'var(--radius-sm)',
                                fontSize: '13px',
                              }}
                            >
                              <div style={{ fontWeight: 600, color: 'var(--accent-red)', marginBottom: 4 }}>
                                {d.severity.toUpperCase()} at line {d.line}, col {d.col}
                              </div>
                              <div style={{ color: 'var(--text-primary)', lineHeight: 1.4 }}>{d.message}</div>
                            </div>
                          ))}
                        </div>
                      ) : (
                        <div style={{ fontSize: '13.5px', color: 'var(--accent-green)', display: 'flex', gap: 8, alignItems: 'center', padding: '8px 0' }}>
                          <CheckCircle2 size={16} /> Zero diagnostic errors. Source is valid.
                        </div>
                      )}
                    </div>
                  )}

                  {activeSideSubTab === 'tokens' && (
                    <div>
                      {compilationResult?.tokens && compilationResult.tokens.length > 0 ? (
                        <div style={{ display: 'flex', flexDirection: 'column', gap: 5 }} className="mono">
                          {compilationResult.tokens.map((t) => (
                            <div
                              key={t.index}
                              style={{
                                fontSize: '12.5px',
                                display: 'flex',
                                justifyContent: 'space-between',
                                padding: '4px 8px',
                                background: 'var(--bg-tertiary)',
                                borderRadius: 'var(--radius-sm)',
                                alignItems: 'center',
                              }}
                            >
                              <span style={{ color: 'var(--accent-cyan)', fontWeight: 600 }}>{t.type}</span>
                              <span style={{ color: 'var(--text-primary)' }}>{t.lexeme}</span>
                              <span style={{ color: 'var(--text-muted)', fontSize: '11.5px' }}>L{t.line}:{t.col}</span>
                            </div>
                          ))}
                        </div>
                      ) : (
                        <div style={{ fontSize: '13px', color: 'var(--text-muted)' }}>
                          Click Compile or Run Code to inspect the token stream.
                        </div>
                      )}
                    </div>
                  )}
                </div>
              </div>
            )}
          </div>
        )}

        {/* WORKSPACE 2: INTERMEDIATE CODE EXPLORER */}
        {activeTab === 'ir' && (
          compilationResult ? (
            <IRExplorer
              tac={compilationResult.tac}
              quads={compilationResult.quads}
              triples={compilationResult.triples}
              indirectTriples={compilationResult.indirect_triples}
              backpatchLogs={compilationResult.backpatch_logs}
              selectedQuadId={selectedQuadId}
              onSelectQuad={(id) => setSelectedQuadId(id)}
            />
          ) : (
            <div style={{ flex: 1, display: 'flex', alignItems: 'center', justifyContent: 'center', color: 'var(--text-muted)', fontSize: '14px' }}>
              Compile source code in Source Workshop to view Intermediate Representations.
            </div>
          )
        )}

        {/* WORKSPACE 3: CFG STUDIO */}
        {activeTab === 'cfg' && (
          compilationResult ? (
            <CFGStudio
              cfg={compilationResult.cfg}
              activeQuadId={selectedQuadId}
              onSelectBlock={(bId) => {}}
            />
          ) : (
            <div style={{ flex: 1, display: 'flex', alignItems: 'center', justifyContent: 'center', color: 'var(--text-muted)', fontSize: '14px' }}>
              Compile source code in Source Workshop to construct the Control-Flow Graph.
            </div>
          )
        )}

        {/* WORKSPACE 4: OPTIMIZATION LAB */}
        {activeTab === 'optimization' && (
          compilationResult ? (
            <OptimizationLab
              quads={compilationResult.quads}
              onApplyOptimization={handleApplyOptimization}
              optResult={optResult}
              isLoading={isOptimizing}
            />
          ) : (
            <div style={{ flex: 1, display: 'flex', alignItems: 'center', justifyContent: 'center', color: 'var(--text-muted)', fontSize: '14px' }}>
              Compile source code in Source Workshop to apply optimization passes.
            </div>
          )
        )}

        {/* WORKSPACE 5: VIRTUAL EXECUTION */}
        {activeTab === 'execution' && (
          compilationResult ? (
            <VirtualExecution
              vmState={vmState}
              quads={compilationResult.quads}
              onStepForward={() => handleVmStep('forward')}
              onStepBackward={() => handleVmStep('backward')}
              onRunAll={handleVmRunAll}
              onReset={handleVmReset}
              isLoading={isVmRunning}
            />
          ) : (
            <div style={{ flex: 1, display: 'flex', alignItems: 'center', justifyContent: 'center', color: 'var(--text-muted)', fontSize: '14px' }}>
              Compile and run source code to launch the Virtual Execution Machine.
            </div>
          )
        )}

        {/* WORKSPACE 6: REGISTER STUDIO */}
        {activeTab === 'registers' && (
          compilationResult ? (
            <RegisterStudio
              analysis={compilationResult.registers}
              quadCount={compilationResult.quads.length}
            />
          ) : (
            <div style={{ flex: 1, display: 'flex', alignItems: 'center', justifyContent: 'center', color: 'var(--text-muted)', fontSize: '14px' }}>
              Compile source code to analyze variable live ranges and register interference.
            </div>
          )
        )}

        {/* WORKSPACE 7: TEST BENCH */}
        {activeTab === 'testbench' && <TestBench />}
      </main>
    </div>
  );
};
