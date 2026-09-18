---
tags: [prep-guide, full-stack, system-design, trade-surveillance, react, fastapi, bank-of-america]
---

# 🏗️ Full-Stack Trade Surveillance & Supervisory Platform Architecture

> **Target Role:** Quant / Python Developer — Supervisory Systems & OpEx (Nnaemeka Ezugwu)
> **Core Focus:** End-to-End Surveillance System Design, High-Throughput Event Streaming, Alert Triage UX, React/TypeScript & FastAPI Implementation

---

## 🏛️ Part 1: High-Level Platform Architecture

Global trade surveillance requires ingesting high-velocity order events (new orders, cancels, modifies, fills) across multiple lines of business (Equities, Fixed Income, FX, Derivatives), evaluating compliance rules with sub-second latency, and serving an interactive, zero-latency dashboard for supervisory officers.

```
┌────────────────────────────────────────────────────────────────────────────────────────┐
│                        Global Markets Trading Infrastructure                           │
│   Equities OMS       │   Fixed Income Gateways   │   FX & Commodities Execution Logs   │
└──────────┬───────────┴─────────────┬─────────────┴─────────────────┬───────────────────┘
           │ FIX / Binary Feed       │ ITCH / OUCH Drop Copy         │ Proprietary FIX
           ▼                         ▼                               ▼
┌────────────────────────────────────────────────────────────────────────────────────────┐
│                        Enterprise Ingestion Tier (Apache Kafka)                        │
│   Topics: market.orders.v1 | market.executions.v1 | market.quotes.bbo.v1               │
└────────────────────────────────────────────┬───────────────────────────────────────────┘
                                             │ High-Throughput Consumer Group
                                             ▼
┌────────────────────────────────────────────────────────────────────────────────────────┐
│                     Python Surveillance Detection Engine (Worker Tier)                  │
│                                                                                        │
│   - Complex Event Processing (CEP)          - Sliding Time Windows (100ms - 5min)      │
│   - Spoofing & Layering Rule Matcher        - Wash Trade Account Identifier            │
│   - Order-to-Trade Ratio (OTR) Tracker      - False-Positive ML Pre-Filter (scikit)   │
└────────────────────────────────────────────┬───────────────────────────────────────────┘
                                             │ Emits Qualified Compliance Alerts
                                             ▼
┌────────────────────────────────────────────────────────────────────────────────────────┐
│                        Data & Audit Persistence Tier                                    │
│   PostgreSQL / Oracle (Transactional Alerts) │ ClickHouse / Time-Series (Order Events) │
└────────────────────────────────────────────┬───────────────────────────────────────────┘
                                             │ Async SQLAlchemy / Connection Pool
                                             ▼
┌────────────────────────────────────────────────────────────────────────────────────────┐
│                       FastAPI Application & WebSocket Server                            │
│                                                                                        │
│   - RESTful Alert Management APIs           - Real-time Alert Broadcasts (WebSockets)  │
│   - Role-Based Access Control (RBAC)       - Full Tamper-Proof Audit Logging (SIA/IAI)│
└────────────────────────────────────────────┬───────────────────────────────────────────┘
                                             │ HTTP / WSS
                                             ▼
┌────────────────────────────────────────────────────────────────────────────────────────┐
│                     Compliance Officer Dashboard (React + TypeScript)                  │
│                                                                                        │
│   - Virtualized Alert Queue (AG Grid)       - Interactive Trade Reconstruction Chart  │
│   - One-Click Triage & Case Escalation      - Keyboard Shortcut Workflow (J/K/A/C)     │
└────────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 🐍 Part 2: Python Surveillance Detection Engine Implementation

### 2.1 Complete Sliding-Window Wash Trade Detector

```python
"""
surveillance_engine.py - Algorithmic Market Abuse Detection Engine
Focus: Wash Trading & Pre-Arranged Crosses across trading accounts.
"""
from dataclasses import dataclass
from datetime import datetime, timedelta
from typing import List, Dict, Optional
import uuid
import logging
from pydantic import BaseModel, Field

logger = logging.getLogger("surveillance.engine")

class OrderSide(str):
    BUY = "BUY"
    SELL = "SELL"

class ExecutionEvent(BaseModel):
    execution_id: str
    order_id: str
    trader_id: str
    account_id: str
    beneficial_owner_id: str  # Critical for wash trade detection
    symbol: str
    side: str
    price: float
    quantity: int
    executed_at: datetime

class SurveillanceAlert(BaseModel):
    alert_id: str = Field(default_factory=lambda: str(uuid.uuid4()))
    rule_name: str
    severity: str  # HIGH, MEDIUM, LOW
    symbol: str
    trader_id: str
    account_id: str
    timestamp: datetime
    description: str
    metadata: Dict

class WashTradeDetector:
    """
    Detects wash trading by tracking opposing buy and sell executions on the same instrument
    for the same beneficial owner within a tight time window (e.g. 5 seconds) where net economic
    risk transfer is zero.
    """
    def __init__(self, time_window_seconds: int = 5, price_tolerance_pct: float = 0.001):
        self.time_window = timedelta(seconds=time_window_seconds)
        self.price_tolerance = price_tolerance_pct
        # Buffer of recent executions grouped by (symbol, beneficial_owner_id)
        self.history: Dict[str, List[ExecutionEvent]] = {}

    def process_execution(self, event: ExecutionEvent) -> Optional[SurveillanceAlert]:
        key = f"{event.symbol}:{event.beneficial_owner_id}"
        now = event.executed_at

        # Prune events older than sliding time window
        recent_events = self.history.get(key, [])
        recent_events = [e for e in recent_events if now - e.executed_at <= self.time_window]
        
        # Check for matching opposing execution
        for past_event in recent_events:
            if past_event.side != event.side:
                # Check price proximity
                price_diff_pct = abs(past_event.price - event.price) / past_event.price
                if price_diff_pct <= self.price_tolerance:
                    # Potential wash trade detected!
                    matched_qty = min(past_event.quantity, event.quantity)
                    alert = SurveillanceAlert(
                        rule_name="WASH_TRADING_SAME_BENEFICIAL_OWNER",
                        severity="HIGH",
                        symbol=event.symbol,
                        trader_id=event.trader_id,
                        account_id=event.account_id,
                        timestamp=now,
                        description=(
                            f"Suspected Wash Trade detected on {event.symbol}: "
                            f"{event.side} {event.quantity} shares @ {event.price:.2f} matched with prior "
                            f"{past_event.side} {past_event.quantity} shares @ {past_event.price:.2f} "
                            f"within {(now - past_event.executed_at).total_seconds():.2f}s "
                            f"for Beneficial Owner {event.beneficial_owner_id}."
                        ),
                        metadata={
                            "incoming_execution_id": event.execution_id,
                            "matched_execution_id": past_event.execution_id,
                            "matched_quantity": matched_qty,
                            "price_variance_pct": price_diff_pct,
                            "time_delta_seconds": (now - past_event.executed_at).total_seconds()
                        }
                    )
                    logger.warning(f"ALERT GENERATED: {alert.description}")
                    return alert

        # Add current event to buffer
        recent_events.append(event)
        self.history[key] = recent_events
        return None
```

---

## 💾 Part 3: Database Design for Audit Remediation & Lineage

To satisfy **SIA and IAI audit standards**, every alert generated must have an **immutable audit trail** tracking which analyst viewed it, when it was escalated, and what justification notes were entered.

### 3.1 PostgreSQL Enterprise Surveillance Schema

```sql
-- Schema for Global Supervisory Platform (BofA OpEx Migration)

-- 1. Core Alerts Table
CREATE TABLE surveillance_alerts (
    alert_id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    rule_code VARCHAR(64) NOT NULL,
    rule_category VARCHAR(32) NOT NULL, -- MARKET_ABUSE, WASH_TRADE, SPOOFING, INSIDER
    severity VARCHAR(16) NOT NULL CHECK (severity IN ('LOW', 'MEDIUM', 'HIGH', 'CRITICAL')),
    status VARCHAR(32) NOT NULL DEFAULT 'NEW' CHECK (status IN ('NEW', 'ASSIGNED', 'IN_REVIEW', 'ESCALATED', 'CLOSED_FALSE_POSITIVE', 'CLOSED_RESOLVED')),
    symbol VARCHAR(32) NOT NULL,
    desk_id VARCHAR(32) NOT NULL,
    trader_id VARCHAR(64) NOT NULL,
    account_id VARCHAR(64) NOT NULL,
    triggered_at TIMESTAMPTZ NOT NULL,
    assigned_to VARCHAR(64),
    summary_text TEXT NOT NULL,
    alert_payload JSONB NOT NULL, -- Machine-readable trade evidence
    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- Fast query indices for Compliance UI
CREATE INDEX idx_alerts_status_severity ON surveillance_alerts (status, severity, triggered_at DESC);
CREATE INDEX idx_alerts_trader_symbol ON surveillance_alerts (trader_id, symbol, triggered_at DESC);
CREATE INDEX idx_alerts_payload_gin ON surveillance_alerts USING GIN (alert_payload);

-- 2. Tamper-Proof Audit Trail Table (Mandated by IAI Audit Controls)
CREATE TABLE alert_audit_log (
    audit_id BIGSERIAL PRIMARY KEY,
    alert_id UUID NOT NULL REFERENCES surveillance_alerts(alert_id) ON DELETE RESTRICT,
    action_taken VARCHAR(64) NOT NULL, -- STATUS_CHANGE, NOTE_ADDED, CASE_ESCALATED
    performed_by VARCHAR(64) NOT NULL,
    previous_state JSONB,
    new_state JSONB,
    justification_comment TEXT NOT NULL,
    ip_address VARCHAR(45),
    action_timestamp TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_audit_alert_id ON alert_audit_log (alert_id, action_timestamp ASC);
```

---

## 🖥️ Part 4: React / TypeScript Compliance Queue Component

Compliance officers review hundreds of alerts per shift. The frontend must feature **keyboard navigation**, **virtual scrolling**, and **instant optimistic state updates**.

```tsx
// SurveillanceAlertQueue.tsx - Production React Component with Virtualized Triage
import React, { useState, useEffect, useCallback } from 'react';

interface Alert {
  alert_id: string;
  rule_code: string;
  severity: 'LOW' | 'MEDIUM' | 'HIGH' | 'CRITICAL';
  status: string;
  symbol: string;
  trader_id: string;
  triggered_at: string;
  summary_text: string;
}

export const SurveillanceAlertQueue: React.FC = () => {
  const [alerts, setAlerts] = useState<Alert[]>([]);
  const [selectedIndex, setSelectedIndex] = useState<number>(0);
  const [filterSeverity, setFilterSeverity] = useState<string>('ALL');

  // Keyboard shortcut listener (J = down, K = up, A = Escalate, C = Close False Positive)
  const handleKeyDown = useCallback((e: KeyboardEvent) => {
    if (e.key === 'j' || e.key === 'ArrowDown') {
      setSelectedIndex(prev => Math.min(prev + 1, alerts.length - 1));
    } else if (e.key === 'k' || e.key === 'ArrowUp') {
      setSelectedIndex(prev => Math.max(prev - 1, 0));
    } else if (e.key === 'c') {
      // Fast close as false positive
      triageAlert(alerts[selectedIndex]?.alert_id, 'CLOSED_FALSE_POSITIVE', 'Automated rapid triage via hotkey');
    } else if (e.key === 'a') {
      // Fast escalate
      triageAlert(alerts[selectedIndex]?.alert_id, 'ESCALATED', 'Escalated to Level 2 Investigation');
    }
  }, [alerts, selectedIndex]);

  useEffect(() => {
    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [handleKeyDown]);

  const triageAlert = async (alertId: string, newStatus: string, comment: string) => {
    if (!alertId) return;
    try {
      await fetch(`/api/v1/surveillance/alerts/${alertId}/triage`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ status: newStatus, justification: comment })
      });
      // Optimistic local state update
      setAlerts(prev => prev.map(a => a.alert_id === alertId ? { ...a, status: newStatus } : a));
    } catch (err) {
      console.error("Triage action failed", err);
    }
  };

  return (
    <div className="flex h-screen bg-gray-100 dark:bg-gray-950 text-gray-900 dark:text-gray-100">
      {/* Alert Feed Panel */}
      <div className="w-1/2 border-r border-gray-300 dark:border-gray-800 flex flex-col">
        <header className="p-4 border-b border-gray-300 dark:border-gray-800 flex justify-between items-center bg-white dark:bg-gray-900">
          <div>
            <h1 className="text-lg font-bold">BofA Global Supervisory Queue</h1>
            <p className="text-xs text-gray-500">Hotkeys: [J/K] Navigate | [A] Escalate | [C] Close FP</p>
          </div>
          <span className="px-2 py-1 text-xs font-semibold rounded bg-blue-100 text-blue-800 dark:bg-blue-900 dark:text-blue-200">
            {alerts.filter(a => a.status === 'NEW').length} Pending Alerts
          </span>
        </header>

        <div className="flex-1 overflow-y-auto divide-y divide-gray-200 dark:divide-gray-800">
          {alerts.map((alert, idx) => (
            <div
              key={alert.alert_id}
              onClick={() => setSelectedIndex(idx)}
              className={`p-4 cursor-pointer transition-colors ${
                idx === selectedIndex 
                  ? 'bg-blue-50 dark:bg-blue-950/40 border-l-4 border-blue-600' 
                  : 'hover:bg-gray-50 dark:hover:bg-gray-900'
              }`}
            >
              <div className="flex justify-between items-center mb-1">
                <span className={`text-xs px-2 py-0.5 rounded font-bold ${
                  alert.severity === 'HIGH' ? 'bg-red-100 text-red-700' : 'bg-yellow-100 text-yellow-700'
                }`}>
                  {alert.severity}
                </span>
                <span className="text-xs text-gray-400">{alert.triggered_at}</span>
              </div>
              <div className="font-semibold text-sm">{alert.rule_code} — {alert.symbol}</div>
              <div className="text-xs text-gray-500 truncate mt-1">{alert.summary_text}</div>
            </div>
          ))}
        </div>
      </div>

      {/* Detail & Trade Timeline Reconstruction Panel */}
      <div className="w-1/2 p-6 flex flex-col bg-white dark:bg-gray-900">
        {alerts[selectedIndex] ? (
          <div>
            <div className="flex justify-between items-start mb-4">
              <div>
                <h2 className="text-xl font-bold">{alerts[selectedIndex].rule_code}</h2>
                <div className="text-sm text-gray-500">Trader: {alerts[selectedIndex].trader_id} | Security: {alerts[selectedIndex].symbol}</div>
              </div>
              <div className="space-x-2">
                <button 
                  onClick={() => triageAlert(alerts[selectedIndex].alert_id, 'CLOSED_FALSE_POSITIVE', 'Hotkey quick close')}
                  className="px-3 py-1.5 bg-gray-200 hover:bg-gray-300 dark:bg-gray-800 dark:hover:bg-gray-700 rounded text-sm font-medium"
                >
                  Dismiss (False Positive)
                </button>
                <button 
                  onClick={() => triageAlert(alerts[selectedIndex].alert_id, 'ESCALATED', 'Level 2 Case Created')}
                  className="px-3 py-1.5 bg-red-600 hover:bg-red-700 text-white rounded text-sm font-medium"
                >
                  Escalate Case
                </button>
              </div>
            </div>

            <div className="p-4 bg-gray-50 dark:bg-gray-800 rounded-lg mb-6">
              <h3 className="text-xs font-bold text-gray-400 uppercase tracking-wider mb-2">Evidence Summary</h3>
              <p className="text-sm leading-relaxed">{alerts[selectedIndex].summary_text}</p>
            </div>

            {/* Audit Trail Note Box */}
            <div>
              <label className="block text-xs font-bold text-gray-400 uppercase mb-2">Audit Compliance Note (Mandated by IAI)</label>
              <textarea 
                rows={3}
                placeholder="Enter mandatory supervisory justification before dispositioning..."
                className="w-full p-3 border rounded border-gray-300 dark:border-gray-700 bg-transparent text-sm focus:ring-2 focus:ring-blue-500"
              />
            </div>
          </div>
        ) : (
          <div className="flex h-full items-center justify-center text-gray-400">Select an alert to inspect evidence</div>
        )}
      </div>
    </div>
  );
};
```
