# 🎯 Interview Command Center

> *Last updated: `= date(today)`*
> Your mission control for clearing every interview across all target roles.

---

## 🔥 Active Pipeline Overview

### By Stage
```dataview
TABLE WITHOUT ID
  file.link AS "Interview / Track",
  company AS "Company",
  role AS "Role",
  manager AS "Manager",
  stage AS "Stage",
  salary_range AS "Rate",
  confidence + "/5" AS "Confidence",
  priority AS "Priority"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE stage != null AND stage != "rejected" AND stage != "withdrawn"
SORT choice(priority, "high", 1, "medium", 2, "low", 3) ASC, next_deadline ASC
```


### 📊 Pipeline Stats
```dataview
TABLE WITHOUT ID
  length(rows) AS "Count",
  key AS "Stage"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
GROUP BY stage AS key
```

---

## 🚀 Quick Actions

| Action | Link |
|--------|------|
| ➕ New Interview | Use Templater → `_Templates/Interview-Note` |
| 📬 Ingest Gmail Emails | [[03-Pipeline/Gmail-Sync-Guide|Gmail Job Sync & Prompts]] |
| 📝 New Retrospective | Use Templater → `_Templates/Retrospective` |
| 🏢 New Company | Use Templater → `_Templates/Company-Profile` |
| 📖 New Story | Use Templater → `_Templates/Behavioral-Story` |
| 📅 Today's Log | Use Templater → `_Templates/Daily-Log` |
| 📊 Weekly Review | Use Templater → `_Templates/Weekly-Review` |


---

## 🎯 Role Hubs

| Role | Hub | Readiness | Active Interviews |
|------|-----|-----------|-------------------|
| 💻 Software Engineer | [[01-Roles/SDE/_Hub]] | → See Skill Matrix | [[03-Pipeline/Active/Goldman-Sachs/Engineering/Goldman-Sachs-Engineering-Tracker\|Goldman Sachs]], [[03-Pipeline/Active/ATT-Labs/ATT-Labs-Distributed-Systems-Tracker\|AT&T Labs (Distributed Systems)]] |
| 📊 Quantitative Developer | [[01-Roles/Quant-Dev/_Hub]] | → See Skill Matrix | [[03-Pipeline/Active/Bank-of-America/01-FRTB-Market-Risk-Prashant/BofA-FRTB-Interview-Tracker\|BofA FRTB]] & [[03-Pipeline/Active/Bank-of-America/02-Trade-Surveillance-Nnaemeka/BofA-Surveillance-Interview-Tracker\|Surveillance]], [[03-Pipeline/Active/DRW/DRW-FICC-Tools-Developer-Tracker\|DRW (FICC Tools)]], [[03-Pipeline/Active/Fidelity/Fidelity-Principal-Quant-Dev-Tracker\|Fidelity (Principal Quant Dev)]], [[03-Pipeline/Active/Morgan-Stanley/FID-Strats/Morgan-Stanley-FID-Strats-Tracker\|Morgan Stanley]], [[03-Pipeline/Active/Barclays/Barclays-Quant-Technology-Tracker\|Barclays]] |
| 🧮 Quantitative Researcher | [[01-Roles/Quant-Research/_Hub]] | → See Skill Matrix | [[03-Pipeline/Active/Teza-Technologies/Teza-Quant-Tracker\|Teza Technologies]] |
| 🤖 AI Engineer | [[01-Roles/AI-Engineer/_Hub]] | → See Skill Matrix | → See [[03-Pipeline/Active/BHFT/BHFT-CPP-LLM-Tracker\|BHFT (C++ / LLM)]] |
| ⚡ Low Latency Systems | [[01-Roles/Low-Latency/_Hub]] | → See Skill Matrix | [[03-Pipeline/Active/BHFT/BHFT-CPP-LLM-Tracker\|BHFT]] (C++ / LLM), [[03-Pipeline/Active/Talan/Talan-CPP-Market-Data-Tracker\|Talan]] (C++ Market Data), [[03-Pipeline/Active/Sagarsoft/Market-Data-C++/Sagarsoft-Senior-CPP-Tracker\|Sagarsoft]] (C++ Market Data) |


---

## 🏢 Company Intelligence

> [[02-Companies/_Company-Index|📋 Full Company Index]]

### Recently Updated Companies
```dataview
TABLE WITHOUT ID
  file.link AS "Company",
  industry AS "Industry",
  status AS "Status"
FROM "16-Interview-Command-Center/02-Companies"
WHERE file.name != "_Company-Index"
SORT file.mtime DESC
LIMIT 10
```

---

## 📝 Recent Retrospectives

```dataview
TABLE WITHOUT ID
  file.link AS "Retro",
  company AS "Company",
  role AS "Role",
  round AS "Round",
  outcome AS "Outcome",
  performance + "/5" AS "Perf",
  date AS "Date"
FROM "16-Interview-Command-Center/04-Retrospectives"
WHERE file.name != "_Retro-Dashboard" AND file.name != "Strengths" AND file.name != "Weaknesses"
SORT date DESC
LIMIT 10
```

---

## 📈 Performance Trends

### Win Rate by Role
```dataview
TABLE WITHOUT ID
  role AS "Role",
  length(filter(rows, (r) => r.outcome = "pass")) AS "Passes",
  length(filter(rows, (r) => r.outcome = "fail")) AS "Fails",
  length(rows) AS "Total"
FROM "16-Interview-Command-Center/04-Retrospectives"
WHERE outcome != null AND file.name != "_Retro-Dashboard" AND file.name != "Strengths" AND file.name != "Weaknesses"
GROUP BY role
```

---

## 🗓️ Upcoming Deadlines (Next 7 Days)

```dataview
TABLE WITHOUT ID
  company AS "Company",
  role AS "Role",
  next_action AS "Action",
  next_deadline AS "Deadline"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE next_deadline != null AND next_deadline <= date(today) + dur(7 days)
SORT next_deadline ASC
```

---

## 🔗 Vault Knowledge Base

| Domain | Location | For Roles |
|--------|----------|-----------|
| CS Foundations | [[01-CS-Foundations]] | All |
| DSA & Patterns | [[03-Data-Structures-Algorithms]] | SDE, Quant Dev |
| System Design | [[04-System-Design]] | SDE, AI Eng, Low Latency |
| Quant Finance | [[05-Quantitative-Finance]] | Quant Dev, Quant Research |
| Behavioral | [[06-Interview-Prep]] | All |
| Performance Engineering | [[12-Performance-Engineering]] | Low Latency, SDE |
| Agentic AI | [[13-Agentic-AI]] | AI Engineer |
| Low Latency Systems | [[14-Low-Latency-Systems]] | Low Latency, Quant Dev |
| Technical Whitepapers | [[15-Technical-Whitepapers]] | All |

---

> *"The impediment to action advances action. What stands in the way becomes the way." — Marcus Aurelius*
