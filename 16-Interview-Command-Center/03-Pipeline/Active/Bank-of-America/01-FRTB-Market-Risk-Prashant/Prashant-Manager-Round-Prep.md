---
tags: [prep-guide, manager-round, prashant-raghuvanshi, bank-of-america, frtb]
---

# 👔 Manager Round Strategy — Prashant Raghuvanshi (FRTB / Market Risk)

> **Interviewer:** Prashant Raghuvanshi — Engineering Manager / VP, Global Markets Technology
> **Team Focus:** Fundamental Review of the Trading Book (FRTB), Market Risk Calculations, Regulatory Capital
> **Candidate Positioning:** Senior Quant Python Developer & Full-Stack Engineer who bridges quantitative finance and scalable software architecture.

---

## 🎯 Understanding Prashant's Objectives & Pain Points

Prashant is not looking for an academic researcher who writes slow math scripts, nor a generic web developer who doesn't know what a basis point is. He needs someone who can:
1. **Deliver Regulatory Compliance On Time:** FRTB has rigid regulatory deadlines set by the Federal Reserve and OCC. Delays or inaccurate capital calculations incur severe regulatory penalties.
2. **Handle Scale & Performance:** Hundreds of thousands of trading positions, millions of sensitivity Greeks, complex correlation aggregation across 3 scenarios, executed overnight and exposed via intraday APIs.
3. **Investigate Discrepancies Independently:** When Front Office P&L doesn't match Risk P&L (PLAT test failure), or capital spikes by 40% overnight, he needs an engineer who can dive into curves, SQL tables, and code to pinpoint the root cause without hand-holding.
4. **Build Clean Full-Stack User Interfaces:** Risk managers need interactive React/Angular dashboards to drill down into bucket sensitivities and sign off on daily numbers.

---

## 🎙️ Core Questions & High-Impact Response Frameworks

### 1. "Tell me about your background and how you've worked with Python in risk or capital markets."
> **Strategic Angle:** Position yourself at the exact intersection of Python Software Engineering, Quant Analytics, and Full-Stack Delivery.

**Response Structure:**
- **Hook:** "I am a Senior Python and Full-Stack Developer specializing in capital markets and risk technology. Over the past several years, my focus has been on building high-throughput quantitative calculation engines, RESTful microservices, and risk visualization platforms."
- **Technical Meat:** "On the backend, I heavily utilize Python (FastAPI, NumPy, Polars/Pandas) to design architectures that transform raw trade positions and market curves into actionable risk sensitivities—such as Delta, Vega, and Curvature—and aggregate them into regulatory capital metrics."
- **Full-Stack Connection:** "I pair this with strong SQL data modeling for time-series and snapshot reconciliation, and modern frontend frameworks like React to give risk managers intuitive drill-down dashboards."
- **Why this role:** "When I saw your team's mandate around the FRTB implementation and market risk calculations, it felt like an exact match for what I do best: bridging complex quant requirements into robust, audit-ready software."

---

### 2. "Walk me through how you would architect our FRTB SBM calculation engine."
> **Strategic Angle:** Demonstrate end-to-end systems thinking—API design, vectorization, caching, and regulatory correctness.

**Response Points:**
1. **Ingestion & Data Normalization:**
   - Asynchronous ingest from trade booking systems and market data feeds.
   - Pydantic schema validation ensuring every trade has designated risk class, bucket, tenor, and sensitivity values.
2. **Vectorized Aggregation Engine (NumPy):**
   - Intra-bucket aggregation: Compute $WS_k = RW_k \cdot s_k$. Construct correlation matrix $\rho_{kl}$ and evaluate $K_b = \sqrt{WS^T \cdot \rho \cdot WS}$.
   - Cross-bucket aggregation: Form vector of bounded bucket sums $S_b$ and evaluate cross-bucket correlations $\gamma_{bc}$.
   - Multi-scenario parallel execution: Concurrently evaluate Medium, High ($1.25\rho$), and Low ($0.75\rho$) correlation regimes and apply the regulatory $\max()$ operator.
3. **API & Intraday Delivery:**
   - FastAPI microservice with async worker pool.
   - Redis caching for static curve parameters and supervisory risk weight tables.
4. **Audit & Lineage:**
   - Persist intermediate bucket results, active correlation scenario, and calculation timestamps in PostgreSQL/Oracle for MRM (Model Risk Management) auditability.

---

### 3. "If yesterday's SBM capital was $120M and today it jumps to $195M, how do you systematically debug the discrepancy?"
> **Strategic Angle:** Show calm, methodological debugging rather than panic.

**Step-by-Step Investigation Flow:**
1. **Scenario Shift Verification:** Check which correlation scenario was active yesterday vs today. Often, a portfolio boundary shift from Medium to High or Low correlation triggers a sudden non-linear jump.
2. **Decomposition by Risk Class & Bucket:** Isolate which of the 7 risk classes drove the variance. If GIRR jumped by $60M, drill into GIRR buckets (e.g., USD 10Y curve).
3. **Inspect Top Delta/Vega Contributors:** Run a SQL variance query joining $T$ and $T-1$ sensitivities:
   $$\Delta \text{WS} = \text{WS}_T - \text{WS}_{T-1}$$
   Identify whether a new large trade was executed, a hedge was unwound, or a block trade was booked with erroneous notional.
4. **Market Data & Curve Calibration Check:** Verify if the underlying yield curve or implied vol surface experienced an anomaly or bad quote from market data providers.
5. **Static Data & Bucket Mapping:** Ensure no positions had missing ratings that defaulted to a punitive fallback bucket (e.g., unrated corporate treated with high risk weights).

---

### 4. "How do you handle collaboration between Quants, Front Office Traders, and Risk Managers?"
> **Strategic Angle:** Demonstrate emotional intelligence, diplomacy, and technical translation capability.

**Key Themes:**
- **Quants** care about mathematical purity and pricing accuracy; developers care about latency, testability, memory management, and uptime. You bridge this by establishing clear API contracts (Pydantic/JSON schemas) and benchmarking quant Python models before productionizing.
- **Traders** need fast execution and don't tolerate system downtime or delayed EOD runs. You communicate in terms of business impact and clear SLAs.
- **Risk Managers & Compliance** care about auditability, lineage, and explainability. You provide detailed drill-downs and never deliver "black-box" numbers.

---

## 🙋 6 Strategic Questions to Ask Prashant

Asking these questions proves you already think like an insider on his team:

1. **"Regarding your FRTB program: Are you currently running in parallel-run mode alongside Basel 2.5, or are you focused on optimizing the go-live pipelines for Day 1 compliance?"**
2. **"For the risk sensitivities—are they pre-calculated by front-office pricing engines and fed into your team for aggregation, or does your Python layer invoke pricing libraries directly via C++/Python bindings?"**
3. **"How does the team handle the P&L Attribution Test (PLAT) workflow between front office hypothetical P&L and your risk-theoretic models? What does the alerting mechanism look like when a desk approaches Amber/Red zones?"**
4. **"On the full-stack side: What does the frontend stack look like for risk managers reviewing the capital numbers? Is it primarily React with internal design libraries, and what level of intraday recalculation do they have access to?"**
5. **"What is the single biggest technical bottleneck the team is currently working to resolve—is it calculation latency during batch runs, data reconciliation volume, or regulatory reporting format changes?"**
6. **"How is the team structured between core engineering and quantitative risk analysts in your day-to-day Agile sprints?"**
