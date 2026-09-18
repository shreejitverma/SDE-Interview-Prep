---
role: Quant-Dev
aliases: [Quantitative Developer, Quant Software Engineer]
tags: [role-hub, quant-dev]
---

# 📊 Quantitative Developer — Preparation Hub

> **Target Companies:** Citadel, Two Sigma, Jane Street, DE Shaw, Jump Trading, Tower Research, HRT, Optiver, IMC
> **Target Levels:** Mid to Senior Quant Dev / VP

---

## 🎯 What Quant Dev Interviews Test

| Round | Weight | What They Want |
|-------|--------|----------------|
| **C++ Coding** | 35% | Low-latency C++, STL mastery, memory management, templates |
| **System Design / Architecture** | 25% | Trading system design, order management, market data infra |
| **Math / Probability** | 20% | Probability, statistics, mental math, brain teasers |
| **Domain Knowledge** | 15% | Market microstructure, order types, FIX protocol, exchange connectivity |
| **Behavioral** | 5% | Teamwork under pressure, fast-paced environment fit |

---

## 📚 Study Plan

→ [[Study-Plan|📋 Detailed Week-by-Week Study Plan]]

### Quick Priority Matrix

| Topic | Priority | Your Level | Target Level | Vault Resource |
|-------|----------|-----------|-------------|----------------|
| Modern C++ (11/14/17/20) | 🔴 Critical | | 5/5 | [[02-Programming-Languages/C++]] |
| Lock-Free / Wait-Free | 🔴 Critical | | 5/5 | [[14-Low-Latency-Systems/08 - Low-Latency Programming]] |
| Memory Models & Atomics | 🔴 Critical | | 4/5 | [[14-Low-Latency-Systems/08 - Low-Latency Programming]] |
| Order Book Implementation | 🔴 Critical | | 5/5 | [[05-Quantitative-Finance/02-Quant-Dev]] |
| Probability & Statistics | 🔴 Critical | | 4/5 | [[05-Quantitative-Finance/01-Mathematics]] |
| Market Microstructure | 🟡 High | | 4/5 | [[14-Low-Latency-Systems/01 - Market & Microstructure Fundamentals]] |
| Networking (TCP/UDP/Multicast) | 🟡 High | | 4/5 | [[14-Low-Latency-Systems/06 - Networking]] |
| Hardware Sympathy | 🟡 High | | 4/5 | [[14-Low-Latency-Systems/04 - Hardware Mechanical Sympathy]] |
| OS & Kernel Tuning | 🟢 Medium | | 3/5 | [[14-Low-Latency-Systems/05 - OS & Kernel Tuning]] |
| FIX / SBE Protocols | 🟢 Medium | | 3/5 | [[14-Low-Latency-Systems/10 - Protocols & Codecs]] |

---

## 🏢 Target Companies

```dataview
TABLE WITHOUT ID
  file.link AS "Company",
  industry AS "Industry",
  status AS "Status"
FROM "16-Interview-Command-Center/02-Companies"
WHERE contains(target_roles, "Quant-Dev")
SORT file.name ASC
```

## 📊 Active Interviews

```dataview
TABLE WITHOUT ID
  company AS "Company", level AS "Level", stage AS "Stage",
  confidence + "/5" AS "Conf", next_deadline AS "Deadline"
FROM "16-Interview-Command-Center/03-Pipeline"
WHERE role = "Quant-Dev" AND stage != "rejected" AND stage != "withdrawn"
SORT next_deadline ASC
```

---

## 🔗 Role Resources
- → [[Skill-Matrix|📊 Self-Assessment Skill Matrix]]
- → [[Question-Bank|🧩 Curated Question Bank]]
- → [[Common-Patterns|🔄 Common Patterns & Frameworks]]
- → [[Resources|📚 Resources & Links]]

## 📖 Key Vault Links
| Domain | Link |
|--------|------|
| Quant Finance | [[05-Quantitative-Finance]] |
| Low Latency Systems | [[14-Low-Latency-Systems]] |
| LL Interview Bank | [[14-Low-Latency-Systems/Interview/interview]] |
| LL Question Bank | [[14-Low-Latency-Systems/Interview/question-bank-answers]] |
| Matching Engine | [[14-Low-Latency-Systems/03 - Matching Engine Internals]] |
| C++ Gold Standard | [[03-Data-Structures-Algorithms/04-Gold-Standard-Cpp-Patterns]] |
| Performance Engineering | [[12-Performance-Engineering]] |
