---
role: Quant-Research
aliases: [Quantitative Researcher, Quant Trader, Quant Analyst]
tags: [role-hub, quant-research]
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Quantitative Researcher - Preparation Hub

> **Target Companies:** Jane Street, Citadel, Two Sigma, DE Shaw, Susquehanna (SIG), Akuna Capital
> **Target Levels:** Junior to Senior Researcher

---

## What Quant Research Interviews Test

| Round | Weight | What They Want |
|-------|--------|----------------|
| **Probability & Statistics** | 35% | Deep probability, conditional expectations, distributions, hypothesis testing |
| **Brain Teasers & Mental Math** | 25% | Speed, lateral thinking, mathematical intuition |
| **Coding (Python/C++)** | 20% | Clean implementation of mathematical/statistical solutions |
| **Market Intuition & Trading Games** | 15% | Market making sims, EV calculations, information games |
| **Behavioral** | 5% | Intellectual curiosity, collaboration under pressure |

---

## Study Plan

→ [[Study-Plan|Detailed Week-by-Week Study Plan]]

### Quick Priority Matrix

| Topic | Priority | Your Level | Target Level | Vault Resource |
|-------|----------|-----------|-------------|----------------|
| Probability Theory | Critical | | 5/5 | [[05-Quantitative-Finance/01-Mathematics]] |
| Combinatorics | Critical | | 5/5 | |
| Stochastic Processes | Critical | | 4/5 | [[05-Quantitative-Finance/01-Mathematics]] |
| Brain Teasers | Critical | | 5/5 | |
| Mental Math | Critical | | 4/5 | |
| Statistics & Estimation | High | | 4/5 | |
| Linear Algebra | High | | 4/5 | |
| Coding (Python) | High | | 4/5 | [[02-Programming-Languages/Python]] |
| Market Making Concepts | High | | 4/5 | [[14-Low-Latency-Systems/01 - Market & Microstructure Fundamentals]] |
| Machine Learning | Medium | | 3/5 | |
| Stochastic Calculus | Medium | | 3/5 | |

---

## Target Companies

```dataview
TABLE WITHOUT ID
  file.link AS "Company", industry AS "Industry", status AS "Status"
FROM "16-Interview-Command-Center/02-Companies"
WHERE contains(target_roles, "Quant-Research")
SORT file.name ASC
```

## Active Interviews

```dataview
TABLE WITHOUT ID
  company AS "Company", level AS "Level", stage AS "Stage",
  confidence + "/5" AS "Conf", next_action_date AS "Deadline"
FROM "16-Interview-Command-Center/03-Pipeline"
WHERE role = "Quant-Research" AND stage != "rejected" AND stage != "withdrawn"
SORT next_action_date ASC
```

---

## Role Resources
- → [[Skill-Matrix]] | → [[Question-Bank]] | → [[Common-Patterns]] | → [[Resources]]

## Key Vault Links
| Domain | Link |
|--------|------|
| Mathematics | [[05-Quantitative-Finance/01-Mathematics]] |
| Algo Trading | [[05-Quantitative-Finance/03-Algo-Trading]] |
| Market Fundamentals | [[14-Low-Latency-Systems/01 - Market & Microstructure Fundamentals]] |
