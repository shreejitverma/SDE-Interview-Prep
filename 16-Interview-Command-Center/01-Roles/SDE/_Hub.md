---
role: SDE
aliases: [Software Engineer, SWE]
tags: [role-hub, sde]
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Software Engineer - Preparation Hub

> **Target Companies:** Google, Meta, Amazon, Apple, Microsoft, Netflix, Stripe, Uber
> **Target Levels:** L4–L6 (or equivalent)

---

## What SDE Interviews Test

| Round | Weight | What They Want |
|-------|--------|----------------|
| **Coding (2 rounds)** | 40% | Clean, optimal code. Edge cases. Clear communication. |
| **System Design (1-2 rounds)** | 30% | Scalable architecture. Trade-offs. Deep dives. |
| **Behavioral (1 round)** | 20% | Leadership, conflict resolution, impact stories. |
| **Culture Fit** | 10% | Company values alignment, collaboration signals. |

---

## Study Plan

→ [[Study-Plan|Detailed Week-by-Week Study Plan]]

### Quick Priority Matrix

| Topic | Priority | Your Level | Target Level | Vault Resource |
|-------|----------|-----------|-------------|----------------|
| Arrays/Strings | Critical | | 5/5 | [[03-Data-Structures-Algorithms/01-Topics]] |
| Trees/Graphs | Critical | | 5/5 | [[03-Data-Structures-Algorithms/01-Topics]] |
| Dynamic Programming | Critical | | 4/5 | [[03-Data-Structures-Algorithms/01-Topics]] |
| System Design (HLD) | Critical | | 5/5 | [[04-System-Design/00-Concepts]] |
| System Design (LLD) | High | | 4/5 | [[04-System-Design/01-LLD]] |
| Concurrency | High | | 4/5 | [[01-CS-Foundations/README\|01-CS-Foundations]] |
| OOP / Design Patterns | High | | 4/5 | [[04-System-Design/03-Design-Patterns]] |
| OS / Networking | Medium | | 3/5 | [[01-CS-Foundations/README\|01-CS-Foundations]] |
| Behavioral (STAR) | Critical | | 5/5 | [[05-Behavioral/_Story-Index]] |

---

## Target Companies

```dataview
TABLE WITHOUT ID
  file.link AS "Company",
  industry AS "Industry",
  status AS "Status"
FROM "16-Interview-Command-Center/02-Companies"
WHERE contains(target_roles, "SDE")
SORT file.name ASC
```

## My Active SDE Interviews

```dataview
TABLE WITHOUT ID
  company AS "Company",
  level AS "Level",
  stage AS "Stage",
  confidence + "/5" AS "Confidence",
  next_action_date AS "Deadline"
FROM "16-Interview-Command-Center/03-Pipeline"
WHERE role = "SDE" AND stage != "rejected" AND stage != "withdrawn"
SORT next_action_date ASC
```

## SDE Retrospectives

```dataview
TABLE WITHOUT ID
  company AS "Company",
  round AS "Round",
  outcome AS "Outcome",
  performance + "/5" AS "Perf",
  date AS "Date"
FROM "16-Interview-Command-Center/04-Retrospectives"
WHERE role = "SDE"
SORT date DESC
LIMIT 10
```

---

## Role Resources
- → [[Skill-Matrix|Self-Assessment Skill Matrix]]
- → [[Question-Bank|Curated Question Bank]]
- → [[Common-Patterns|Common Patterns & Frameworks]]
- → [[Resources|Resources & Links]]

---

## Key Vault Links
| Domain | Link |
|--------|------|
| DSA Gold Standard | [[03-Data-Structures-Algorithms/04-Gold-Standard-Cpp-Patterns]] |
| System Design Case Studies | [[04-System-Design/02-Case-Studies]] |
| Design Patterns | [[04-System-Design/03-Design-Patterns]] |
| Behavioral Guide | [[06-Interview-Prep/01-Behavioral/star_method]] |
| Programming Languages | [[02-Programming-Languages/README\|02-Programming-Languages]] |
