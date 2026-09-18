# 🏦 Bank of America Core Values & Behavioral Competencies

> **Master Guide:** Mapping Bank of America's core values to your STAR stories for Global Markets Technology interviews.

---

## 🏛️ Bank of America's 4 Core Values

| # | Value | What It Means in Global Markets Tech | Your Target Story |
|:--|:------|:-------------------------------------|:------------------|
| 1 | **Deliver Together** | Seamless collaboration between front-office traders, quant researchers, risk managers, and core software engineers. No siloed finger-pointing. | [[03-Pipeline/Active/Bank-of-America/Shared-BofA-Prep/BofA-Behavioral-STAR-Bank#Story 3: Bridging the Gap Between Quants and Software Developers\|Story 3: Quant-IT Pairing]] |
| 2 | **Act Responsibly** | Deep respect for regulatory frameworks (Fed, SEC, OCC, Basel III.1), zero tolerance for calculation shortcuts, rock-solid audit trails, and strict risk controls. | [[03-Pipeline/Active/Bank-of-America/Shared-BofA-Prep/BofA-Behavioral-STAR-Bank#Story 1: High-Pressure Regulatory / Audit Deadline (SIA/IAI Remediation)\|Story 1: Audit Remediation (IAI)]] |
| 3 | **Realize the Power of Our People** | Mentoring junior developers, respecting diverse technical viewpoints, fostering psychological safety when bugs or breaks occur. | Mentoring & Pair Programming |
| 4 | **Trust the Team** | Extreme ownership, transparency when calculations diverge, accountability during production incidents, and reliable execution without micromanagement. | [[03-Pipeline/Active/Bank-of-America/Shared-BofA-Prep/BofA-Behavioral-STAR-Bank#Story 2: Investigating a Mysterious Risk / P&L Discrepancy Break\|Story 2: P&L Break Investigation]] |

---

## 🎯 Top 5 Behavioral Questions at Bank of America

### 1. "Tell me about a time you had to deliver under a strict regulatory or statutory deadline."
- **Key Theme:** Act Responsibly & Deliver Together.
- **Reference:** [[03-Pipeline/Active/Bank-of-America/Shared-BofA-Prep/BofA-Behavioral-STAR-Bank#Story 1: High-Pressure Regulatory / Audit Deadline (SIA/IAI Remediation)|Story 1: IAI Audit Remediation]].
- **Golden Rule:** Emphasize that you *never sacrificed testing or code quality* to meet the date; you prioritized scope and parallelized work.

### 2. "How do you handle disagreements with quantitative researchers or business users who don't understand software constraints?"
- **Key Theme:** Deliver Together.
- **Reference:** [[03-Pipeline/Active/Bank-of-America/Shared-BofA-Prep/BofA-Behavioral-STAR-Bank#Story 3: Bridging the Gap Between Quants and Software Developers|Story 3: Quant-IT Collaboration]].
- **Golden Rule:** Never say "the quants wrote bad code." Frame it as: *"Quants optimize for mathematical expressiveness and research iteration; my role as an engineer is to preserve that mathematical integrity while bringing production qualities like vectorization, typing, and horizontal scalability."*

### 3. "Describe a situation where you identified a risk or discrepancy that others missed."
- **Key Theme:** Act Responsibly & Trust the Team.
- **Reference:** [[03-Pipeline/Active/Bank-of-America/Shared-BofA-Prep/BofA-Behavioral-STAR-Bank#Story 2: Investigating a Mysterious Risk / P&L Discrepancy Break|Story 2: Bermudan Swaption Curve Break]].
- **Golden Rule:** Focus on root-cause analysis, data reconciliation queries, and preventing future recurrences with automated pre-batch checks.

### 4. "How do you ensure that systems you build are audit-ready and compliant with internal governance?"
- **Key Theme:** Act Responsibly.
- **Answer Points:**
  - Pydantic schema validation on all API endpoints.
  - Immutable bi-temporal data tables for sensitivity and trade snapshots.
  - Comprehensive Pytest test suites simulating edge-case market shocks.
  - Structured JSON audit logging recording user ID, timestamp, and rationale for any manual override.

### 5. "Tell me about a time you replaced or modernized an existing legacy tool."
- **Key Theme:** Operational Excellence (OpEx) & Trust the Team.
- **Reference:** [[03-Pipeline/Active/Bank-of-America/Shared-BofA-Prep/BofA-Behavioral-STAR-Bank#Story 4: Retiring a Legacy / Vendor Platform with Zero Business Disruption|Story 4: Vendor Platform Retirement]].
- **Golden Rule:** Emphasize the shadow/dual-run methodology and automated delta reconciliation between old and new outputs.
