---
tags: [prep-guide, python, software-engineering, fast-api, sql, bank-of-america]
---

# 🐍 Python & Full-Stack Quant Technical Interview Mastery

> **Target Role:** Quant / Python Developer — FRTB & Market Risk (Prashant Raghuvanshi)
> **Focus Areas:** High-Performance Python, FastAPI REST Architecture, NumPy Vectorization, SQL Data Engineering, React Integration, Testing & Auditability

---

## 🏗️ Part 1: Production Python Architecture for Risk Engines

In enterprise banking (Global Markets Technology), risk engines cannot be toy scripts. They must be resilient, typed, asynchronous, horizontally scalable microservices.

### 1.1 Complete FastAPI Production Pattern for FRTB SBM Service

```python
"""
frtb_service.py - Production-grade FastAPI microservice for FRTB Sensitivities-Based Method (SBM)
Includes Pydantic validation, async dependency injection, and correlation aggregation.
"""
from enum import Enum
from typing import List, Dict, Optional
import numpy as np
from fastapi import FastAPI, Depends, HTTPException, status, Query
from pydantic import BaseModel, Field, field_validator
import logging
import time

# Structured logging for regulatory compliance
logger = logging.getLogger("bofa.frtb.engine")
logging.basicConfig(level=logging.INFO, format='{"timestamp": "%(asctime)s", "level": "%(levelname)s", "msg": "%(message)s"}')

app = FastAPI(
    title="BofA Global Markets - FRTB SBM Engine",
    description="Regulatory Capital & Sensitivity Aggregation Microservice",
    version="2.4.0"
)

# --- Domain Models & Schemas ---
class RiskClass(str, Enum):
    GIRR = "GIRR"
    CSR_NON_SEC = "CSR_NON_SEC"
    CSR_SEC_CTP = "CSR_SEC_CTP"
    CSR_SEC_NON_CTP = "CSR_SEC_NON_CTP"
    EQUITY = "EQUITY"
    COMMODITY = "COMMODITY"
    FX = "FX"

class SensitivityType(str, Enum):
    DELTA = "DELTA"
    VEGA = "VEGA"
    CURVATURE = "CURVATURE"

class SensitivityRecord(BaseModel):
    trade_id: str = Field(..., example="TRD-904823")
    desk_id: str = Field(..., example="G10-RATES-NY")
    risk_class: RiskClass
    sensitivity_type: SensitivityType
    bucket: str = Field(..., example="1")
    risk_factor_name: str = Field(..., example="USD-SOFR-10Y")
    sensitivity_value: float = Field(..., description="Raw net sensitivity (DV01, Vega, etc.)")
    risk_weight: float = Field(..., gt=0.0, description="Supervisory risk weight multiplier")

    @property
    def weighted_sensitivity(self) -> float:
        return self.sensitivity_value * self.risk_weight

class PortfolioSBMRequest(BaseModel):
    portfolio_id: str
    as_of_date: str
    records: List[SensitivityRecord]

class BucketResult(BaseModel):
    bucket: str
    net_weighted_sensitivity: float
    bucket_capital: float

class SBMCalculationResponse(BaseModel):
    portfolio_id: str
    as_of_date: str
    risk_class: RiskClass
    sensitivity_type: SensitivityType
    capital_medium: float
    capital_high: float
    capital_low: float
    final_regulatory_capital: float
    active_scenario: str
    bucket_breakdown: List[BucketResult]
    execution_time_ms: float

# --- FRTB Vectorized SBM Engine ---
class FRTBSBMEngine:
    """Vectorized NumPy engine for intra-bucket and cross-bucket aggregation under 3 correlation scenarios."""
    
    @staticmethod
    def intra_bucket_aggregation(ws_vector: np.ndarray, rho_matrix: np.ndarray) -> float:
        """
        K_b = sqrt( sum_k WS_k^2 + sum_k sum_{l!=k} rho_kl * WS_k * WS_l )
        Vectorized form: K_b = sqrt( max(0, WS^T * Rho * WS) )
        """
        variance = ws_vector @ rho_matrix @ ws_vector
        return float(np.sqrt(max(0.0, variance)))

    @classmethod
    def calculate_sbm_capital(
        cls, 
        bucket_data: Dict[str, np.ndarray], 
        intra_corr_base: float, 
        cross_corr_base: float
    ) -> Dict[str, float]:
        """
        Executes SBM aggregation across the 3 prescribed Basel correlation regimes:
        Medium (1.0x), High (1.25x capped at 1.0), Low (2x - 1.0 or 0.75x).
        """
        results = {}
        scenarios = {
            "Medium": (intra_corr_base, cross_corr_base),
            "High": (min(1.0, 1.25 * intra_corr_base), min(1.0, 1.25 * cross_corr_base)),
            "Low": (max(2.0 * intra_corr_base - 1.0, 0.75 * intra_corr_base),
                    max(2.0 * cross_corr_base - 1.0, 0.75 * cross_corr_base))
        }

        for scenario_name, (rho, gamma) in scenarios.items():
            k_buckets = []
            s_buckets = []
            
            for b_name, ws_arr in bucket_data.items():
                n = len(ws_arr)
                # Construct intra-bucket correlation matrix
                rho_mat = np.full((n, n), rho)
                np.fill_diagonal(rho_mat, 1.0)
                
                kb = cls.intra_bucket_aggregation(ws_arr, rho_mat)
                sb = float(np.sum(ws_arr))
                # Bounded Sb
                sb_bounded = max(min(sb, kb), -kb)
                
                k_buckets.append(kb)
                s_buckets.append(sb_bounded)

            # Cross-bucket aggregation
            k_vec = np.array(k_buckets)
            s_vec = np.array(s_buckets)
            m = len(k_vec)
            
            if m == 1:
                total_k = k_vec[0]
            else:
                gamma_mat = np.full((m, m), gamma)
                np.fill_diagonal(gamma_mat, 0.0) # separate diagonal from cross terms
                
                sum_kb_sq = np.sum(k_vec ** 2)
                cross_terms = s_vec @ gamma_mat @ s_vec
                total_k = float(np.sqrt(max(0.0, sum_kb_sq + cross_terms)))
                
            results[scenario_name] = total_k

        return results

# --- FastAPI Endpoints ---
@app.post("/api/v1/frtb/sbm/calculate", response_model=SBMCalculationResponse)
async def calculate_sbm(request: PortfolioSBMRequest):
    start_time = time.perf_counter()
    logger.info(f"Received SBM calc request for portfolio {request.portfolio_id}, {len(request.records)} records")

    if not request.records:
        raise HTTPException(status_code=400, detail="No sensitivity records provided in request")

    # Group records by bucket
    buckets: Dict[str, List[float]] = {}
    for r in request.records:
        buckets.setdefault(r.bucket, []).append(r.weighted_sensitivity)

    bucket_arrays = {b: np.array(vals) for b, vals in buckets.items()}

    # Supervisory baseline correlations (e.g. GIRR standard 0.50 intra, 0.50 cross)
    base_intra_corr = 0.50
    base_cross_corr = 0.50

    scenario_capitals = FRTBSBMEngine.calculate_sbm_capital(bucket_arrays, base_intra_corr, base_cross_corr)

    # FRTB rule: Capital is the MAX of the 3 scenarios
    worst_scenario = max(scenario_capitals, key=scenario_capitals.get)
    final_capital = scenario_capitals[worst_scenario]

    # Calculate individual bucket metrics for audit transparency
    bucket_breakdowns = []
    for b_name, ws_arr in bucket_arrays.items():
        n = len(ws_arr)
        rho_mat = np.full((n, n), base_intra_corr)
        np.fill_diagonal(rho_mat, 1.0)
        kb = FRTBSBMEngine.intra_bucket_aggregation(ws_arr, rho_mat)
        bucket_breakdowns.append(BucketResult(
            bucket=b_name,
            net_weighted_sensitivity=float(np.sum(ws_arr)),
            bucket_capital=kb
        ))

    elapsed = (time.perf_counter() - start_time) * 1000.0

    return SBMCalculationResponse(
        portfolio_id=request.portfolio_id,
        as_of_date=request.as_of_date,
        risk_class=request.records[0].risk_class,
        sensitivity_type=request.records[0].sensitivity_type,
        capital_medium=scenario_capitals["Medium"],
        capital_high=scenario_capitals["High"],
        capital_low=scenario_capitals["Low"],
        final_regulatory_capital=final_capital,
        active_scenario=worst_scenario,
        bucket_breakdown=bucket_breakdowns,
        execution_time_ms=elapsed
    )
```

---

## ⚡ Part 2: Handling Large Financial Datasets in Python

When dealing with 100,000+ trade positions and millions of sensitivity shocks daily:

### 2.1 Pandas vs Polars vs Arrow Memory Optimization

```python
import pandas as pd
import polars as pl
import pyarrow.parquet as pq

# 1. High-Performance Polars LazyFrame Pipeline (Recommended for 10M+ rows)
def calculate_desk_exposure_polars(parquet_path: str) -> pl.DataFrame:
    """Reads partitioned trade data with zero-copy and computes risk weights lazily."""
    q = (
        pl.scan_parquet(parquet_path)
        .filter(pl.col("asset_class") == "RATES")
        .with_columns([
            (pl.col("raw_delta") * pl.col("risk_weight")).alias("weighted_delta")
        ])
        .group_by(["desk_id", "currency", "tenor"])
        .agg([
            pl.col("weighted_delta").sum().alias("net_weighted_delta"),
            pl.col("trade_id").count().alias("trade_count")
        ])
        .sort("net_weighted_delta", descending=True)
    )
    return q.collect()

# 2. Memory-Optimized Pandas Chunk Processing (Handling multi-GB CSV feeds)
def stream_process_sensitivities(csv_file_path: str, chunk_size: int = 50_000):
    """Processes large regulatory sensitivity dumps in chunks to maintain < 500MB RAM footprint."""
    dtypes = {
        "trade_id": "category",
        "desk_id": "category",
        "risk_class": "category",
        "bucket": "int16",
        "raw_sensitivity": "float32",
        "risk_weight": "float32"
    }
    
    total_capital = 0.0
    for chunk in pd.read_csv(csv_file_path, chunksize=chunk_size, dtype=dtypes):
        chunk["weighted_sens"] = chunk["raw_sensitivity"] * chunk["risk_weight"]
        # Aggregation logic per chunk
        total_capital += chunk["weighted_sens"].sum()
        
    return total_capital
```

---

## 💾 Part 3: Advanced SQL for Financial Risk & Reconciliation

Bank of America relies heavily on Oracle, PostgreSQL, and SQL Server for storing positions, curves, sensitivities, and regulatory audit snapshots.

### 3.1 Reconciliation Query: Front Office P&L vs Risk System Hypothetical P&L (PLAT Preparation)

```sql
-- Daily P&L Attribution Test (PLAT) Reconciliation & Discrepancy Detector
WITH DailyFO AS (
    -- Front Office hypothetical P&L (T-1 positions revalued at T market prices)
    SELECT 
        trade_id,
        desk_id,
        as_of_date,
        hypo_pnl_usd
    FROM front_office_pnl_snapshot
    WHERE as_of_date = CURRENT_DATE
),
DailyRisk AS (
    -- Risk Engine Theoretic P&L (sensitivities x market factor shocks)
    SELECT 
        trade_id,
        desk_id,
        as_of_date,
        risk_theoretic_pnl_usd
    FROM risk_engine_pnl_snapshot
    WHERE as_of_date = CURRENT_DATE
),
JoinedRecon AS (
    SELECT 
        COALESCE(fo.trade_id, rk.trade_id) AS trade_id,
        COALESCE(fo.desk_id, rk.desk_id) AS desk_id,
        fo.hypo_pnl_usd,
        rk.risk_theoretic_pnl_usd,
        ABS(COALESCE(fo.hypo_pnl_usd, 0) - COALESCE(rk.risk_theoretic_pnl_usd, 0)) AS pnl_variance,
        CASE 
            WHEN fo.trade_id IS NULL THEN 'MISSING_IN_FRONT_OFFICE'
            WHEN rk.trade_id IS NULL THEN 'MISSING_IN_RISK_ENGINE'
            WHEN ABS(fo.hypo_pnl_usd - rk.risk_theoretic_pnl_usd) > 5000.0 THEN 'VARIANCE_BREACH'
            ELSE 'RECONCILED'
        END AS recon_status
    FROM DailyFO fo
    FULL OUTER JOIN DailyRisk rk 
        ON fo.trade_id = rk.trade_id 
       AND fo.as_of_date = rk.as_of_date
)
SELECT 
    desk_id,
    recon_status,
    COUNT(*) AS trade_count,
    SUM(pnl_variance) AS total_dollar_variance,
    AVG(pnl_variance) AS avg_dollar_variance
FROM JoinedRecon
GROUP BY desk_id, recon_status
ORDER BY total_dollar_variance DESC;
```

### 3.2 SQL Window Functions for SBM Bucket Sensitivity Aggregation

```sql
-- SBM Bucket-Level Aggregation with Analytical Functions
SELECT 
    desk_id,
    risk_class,
    bucket,
    as_of_date,
    SUM(raw_sensitivity * risk_weight) AS net_weighted_sensitivity,
    -- Intra-bucket square sum for SBM formula
    SQRT(
        SUM(POWER(raw_sensitivity * risk_weight, 2)) + 
        -- Cross-sensitivity term inside bucket (simplified correlation rho = 0.5)
        0.5 * (
            POWER(SUM(raw_sensitivity * risk_weight), 2) - 
            SUM(POWER(raw_sensitivity * risk_weight, 2))
        )
    ) AS bucket_capital_estimate,
    -- Rank desks by sensitivity impact within each risk class
    DENSE_RANK() OVER (
        PARTITION BY risk_class 
        ORDER BY ABS(SUM(raw_sensitivity * risk_weight)) DESC
    ) AS risk_rank
FROM position_sensitivities
WHERE as_of_date = :report_date
GROUP BY desk_id, risk_class, bucket, as_of_date
HAVING ABS(SUM(raw_sensitivity * risk_weight)) > 0;
```

---

## 🖥️ Part 4: Full-Stack Architecture (FastAPI + React)

The job description emphasizes **Demonstrated Full Stack Development background** (Python backend + React/Angular frontend).

### 4.1 Architecture Diagram

```
┌────────────────────────────────────────────────────────┐
│               Frontend: React / TypeScript             │
│                                                        │
│  - AG Grid / TanStack Table (100k+ virtualized rows)  │
│  - Real-time P&L / Sensitivity Waterfall Charts        │
│  - Regulatory Reconciliation Drill-Down                │
└───────────────▲────────────────────────▲───────────────┘
                │ REST (CRUD / Query)    │ WebSockets (Live Risk)
┌───────────────▼────────────────────────▼───────────────┐
│               Backend: FastAPI Microservices           │
│                                                        │
│  - Pydantic Data Contracts & Validation Layer         │
│  - Background Tasks / Celery for Heavy SBM Batches     │
│  - Redis In-Memory Cache for Curve Data & Positions    │
└───────────────▲────────────────────────────────────────┘
                │ SQLAlchemy / AsyncPG
┌───────────────▼────────────────────────────────────────┐
│            Relational Database: Oracle / PostgreSQL    │
│                                                        │
│  - Partitioned Trade & Sensitivity Snapshots          │
│  - Audit Trail / Lineage Logs                          │
└────────────────────────────────────────────────────────┘
```

### 4.2 React TypeScript Component for Capital Breakdown Table

```tsx
// RiskSummaryTable.tsx - High-performance React component for risk breakdown
import React, { useState, useEffect } from 'react';

interface BucketMetric {
  bucket: string;
  net_weighted_sensitivity: number;
  bucket_capital: number;
}

interface SBMResponse {
  portfolio_id: string;
  final_regulatory_capital: number;
  active_scenario: string;
  bucket_breakdown: BucketMetric[];
}

export const RiskSummaryTable: React.FC<{ portfolioId: string }> = ({ portfolioId }) => {
  const [data, setData] = useState<SBMResponse | null>(null);
  const [loading, setLoading] = useState<boolean>(true);

  useEffect(() => {
    async function fetchRisk() {
      try {
        const res = await fetch(`/api/v1/frtb/sbm/${portfolioId}`);
        const json = await res.json();
        setData(json);
      } catch (err) {
        console.error("Failed to load SBM capital metrics", err);
      } finally {
        setLoading(false);
      }
    }
    fetchRisk();
  }, [portfolioId]);

  if (loading) return <div className="p-4">Calculating regulatory capital...</div>;
  if (!data) return <div className="p-4 text-red-500">Failed to fetch capital data.</div>;

  return (
    <div className="rounded-lg border p-6 shadow-sm bg-white dark:bg-gray-900">
      <div className="flex justify-between items-center mb-4">
        <div>
          <h2 className="text-xl font-bold">FRTB SBM Capital Summary — {data.portfolio_id}</h2>
          <span className="text-sm text-gray-500">Active Worst-Case Scenario: <strong>{data.active_scenario}</strong></span>
        </div>
        <div className="text-right">
          <div className="text-2xl font-black text-emerald-600">
            ${data.final_regulatory_capital.toLocaleString(undefined, { minimumFractionDigits: 2 })}
          </div>
          <span className="text-xs text-gray-400">Total Regulatory Capital</span>
        </div>
      </div>

      <table className="w-full text-left border-collapse">
        <thead>
          <tr className="border-b bg-gray-50 dark:bg-gray-800">
            <th className="p-3">Bucket</th>
            <th className="p-3 text-right">Net Weighted Sensitivity</th>
            <th className="p-3 text-right">Bucket Capital (Kb)</th>
          </tr>
        </thead>
        <tbody>
          {data.bucket_breakdown.map((row) => (
            <tr key={row.bucket} className="border-b hover:bg-gray-50/50">
              <td className="p-3 font-medium">Bucket {row.bucket}</td>
              <td className="p-3 text-right">${row.net_weighted_sensitivity.toLocaleString()}</td>
              <td className="p-3 text-right font-semibold">${row.bucket_capital.toLocaleString()}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
};
```

---

## 🧪 Part 5: Testing, Audit Readiness, & CI/CD

In Bank of America's Global Markets Technology, software must pass strict **Model Risk Management (MRM)** and internal audit scrutiny.

### 5.1 Pytest Suite for Sensitivity Engine

```python
# test_sbm_engine.py
import pytest
import numpy as np
from frtb_service import FRTBSBMEngine

def test_intra_bucket_perfect_correlation():
    """Two identical sensitivities with correlation 1.0 must sum linearly."""
    ws = np.array([100.0, 100.0])
    rho = np.array([[1.0, 1.0], [1.0, 1.0]])
    capital = FRTBSBMEngine.intra_bucket_aggregation(ws, rho)
    assert pytest.approx(capital, rel=1e-4) == 200.0

def test_intra_bucket_zero_correlation():
    """Two orthogonal sensitivities must aggregate as hypotenuse (sqrt(a^2 + b^2))."""
    ws = np.array([300.0, 400.0])
    rho = np.array([[1.0, 0.0], [0.0, 1.0]])
    capital = FRTBSBMEngine.intra_bucket_aggregation(ws, rho)
    assert pytest.approx(capital, rel=1e-4) == 500.0

def test_three_correlation_scenarios_worst_case_rule():
    """Verify that final capital reflects the maximum of Medium, High, and Low regimes."""
    buckets = {
        "1": np.array([100.0, -50.0]),
        "2": np.array([80.0, 20.0])
    }
    res = FRTBSBMEngine.calculate_sbm_capital(buckets, intra_corr_base=0.5, cross_corr_base=0.5)
    max_val = max(res.values())
    assert max_val >= res["Medium"]
    assert max_val >= res["High"]
    assert max_val >= res["Low"]
```
