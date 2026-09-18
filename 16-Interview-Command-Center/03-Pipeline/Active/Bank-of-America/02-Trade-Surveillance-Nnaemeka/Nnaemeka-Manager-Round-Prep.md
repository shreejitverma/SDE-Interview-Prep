---
tags: [prep-guide, manager-round, nnaemeka-ezugwu, bank-of-america, trade-surveillance]
---

# 👔 Manager Round Strategy — Nnaemeka Ezugwu (Trade Surveillance & OpEx)

> **Interviewer:** Nnaemeka Ezugwu — Engineering Manager / VP, Global Markets Supervisory Technology
> **Project Scope:** Operational Excellence (OpEx) — Retiring Vendor Surveillance Platform & Cross-LOB Supervisory Consolidation (1/3 of space funding)
> **Candidate Positioning:** Senior Python / Full-Stack Engineer who specializes in high-throughput event processing, compliance UI workflows, and zero-downtime platform migrations.

---

## 🎯 Understanding Nnaemeka's Priorities & Pressures

Nnaemeka leads the **single largest and most critical project in this division**, commanding **one-third (33%) of the space's annual budget**. His team faces high-visibility pressure from both Global Markets executive leadership and regulatory audit bodies:

1. **Vendor Platform Retirement:** Decommissioning expensive vendor software (e.g., NICE Actimize / Nasdaq SMARTS) requires 100% feature and alert parity. Dropping a single real market abuse alert during cutover could trigger a catastrophic regulatory enforcement action.
2. **Alert Volume & Analyst Ergonomics:** Compliance officers across Equities, Rates, FX, and Commodities are drowning in thousands of alerts daily. Nnaemeka needs an engineer who builds **fast, intuitive, keyboard-driven frontends** that reduce triage time per alert while preventing analyst burnout.
3. **Audit Item Remediation (SIA / IAI):** Regulatory deadlines on audit findings are non-negotiable. Nnaemeka values an engineer who can quickly pick up SIA (Self-Identified) and IAI (Internal Audit Identified) tasks, write bulletproof tests, document lineage, and get sign-off from auditors.
4. **Cross-LOB Diplomacy:** Different trading desks have different order conventions. Standardizing alert definitions across disparate business lines requires strong communication and structured domain modeling.

---

## 🎙️ Core Questions & High-Impact Response Frameworks

### 1. "Tell me about your experience and how your background fits this surveillance modernization project."
> **Strategic Angle:** Position yourself as a full-stack engineer who understands both high-velocity streaming backends and high-efficiency compliance frontends.

**Response Outline:**
- **Hook:** "I am a Senior Python and Full-Stack Software Engineer with extensive experience building data-intensive applications in capital markets and financial technology."
- **Backend Strength:** "On the backend, I specialize in architecting asynchronous Python microservices using FastAPI, Celery, and Kafka event streaming to ingest high-frequency trade and order feeds, applying sliding-window detection algorithms for market abuse, and persisting immutable audit trails."
- **Frontend Strength:** "On the frontend, I focus on building React and TypeScript applications that eliminate UI latency—using table virtualization, optimistic state updates, and keyboard shortcuts—so operations analysts can triage thousands of daily records efficiently."
- **Connection to OpEx:** "What excites me about your team is the scale of the OpEx consolidation: retiring a commercial vendor platform and replacing it with an agile, high-performance in-house supervisory platform that serves multiple lines of business."

---

### 2. "How would you approach migrating from a vendor surveillance platform to our internal supervisory platform without risking regulatory compliance gaps?"
> **Strategic Angle:** Walk through a battle-tested, risk-mitigated phased rollout.

**Response Points:**
1. **Rule Inventory & Parity Catalog:** Create an exhaustive mapping of every existing vendor surveillance rule, threshold, input feed, and alert output format.
2. **Dual-Run / Shadow Phase (3-6 Months):**
   - Stream production order, cancel, and fill events into both the vendor tool and the internal Python engine in parallel.
   - Run automated daily reconciliation scripts to compare alert sets:
     $$\text{Delta} = \text{Alerts}_{\text{Vendor}} \triangle \text{Alerts}_{\text{InHouse}}$$
3. **Discrepancy Investigation & Threshold Calibration:**
   - For any discrepancy where the vendor fired an alert and the internal platform didn't, determine whether it was a timing artifact, data feed difference, or rule sensitivity gap.
   - Tune thresholds to eliminate false positives while guaranteeing 100% capture of genuine suspicious activity.
4. **Independent Audit & Compliance Sign-Off:**
   - Present quantitative parity metrics to Compliance and Corporate Audit.
   - Execute staged cutover desk by desk (e.g., Cash Equities first, then FX, then Fixed Income), culminating in formal vendor decommissioning.

---

### 3. "Surveillance analysts face extreme alert fatigue. How do you address this from both an engineering and UX perspective?"
> **Strategic Angle:** Show that you think about the user experience of the compliance officer, not just raw database tables.

**Dual-Pronged Strategy:**
- **Backend Algorithmic Filtering:**
  - Introduce **Alert Aggregation / Clustering:** Instead of firing 50 discrete alerts for 50 rapid order cancellations by the same trading bot, aggregate them into a single parent incident with an interactive sub-event timeline.
  - Implement dynamic thresholding based on historical trader baselines and volatility regimes rather than rigid, static limits.
- **Frontend Ergonomics (React UX):**
  - Use virtualized lists (AG Grid or TanStack Virtual) to render thousands of rows smoothly with zero DOM lag.
  - Implement full keyboard shortcuts (J/K navigation, A to escalate, C to dismiss) to cut triage time from 45 seconds down to 5 seconds per alert.
  - Interactive trade reconstruction timeline displaying bid-ask book depth alongside the trader's execution timestamps to give instant contextual evidence.

---

### 4. "How do you handle audit items like SIA (Self-Identified) or IAI (Internal Audit Identified) tasks under tight deadlines?"
> **Strategic Angle:** Demonstrate accountability, speed, and strict audit readiness.

**Key Themes:**
- "I treat audit items with the highest priority because an open IAI is a regulatory liability for the firm."
- "When resolving an audit item, I begin with a rigorous Root Cause Analysis (RCA) to understand why the supervisory gap existed. Then, I design a clean, modular solution with automated regression tests that specifically replicate the failure scenario."
- "Crucially, I document the end-to-end data lineage and provide clear before-and-after evidence so Corporate Audit can verify and close the finding on their first inspection."

---

## 🙋 6 Strategic Questions to Ask Nnaemeka

Asking these questions demonstrates executive presence and immediate project readiness:

1. **"What is the targeted timeline for fully decommissioning the vendor surveillance platform, and what phase of the migration is the team currently executing?"**
2. **"Which lines of business have already been onboarded to the new supervisory platform, and which asset classes are on the roadmap for this fiscal year?"**
3. **"How are the detection rules currently configured—are they expressed as Python code, SQL rules, or do you have a domain-specific rule engine that compliance officers can configure?"**
4. **"What does the daily alert volume look like across Global Markets, and what is your target reduction in false positive rates with the new in-house platform?"**
5. **"Regarding the SIA and IAI audit remediation work: Does the team have dedicated sprints for audit remediation, or are audit items integrated into the normal Agile product backlog?"**
6. **"What is the biggest operational hurdle you've encountered so far in replacing the vendor platform—is it data feed normalization across LOBs or user adoption from compliance teams?"**
