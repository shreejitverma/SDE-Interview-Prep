---
role: AI-Engineer
aliases: [AI/ML Engineer, ML Engineer, Machine Learning Engineer]
tags: [role-hub, ai-engineer]
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# AI Engineer - Preparation Hub

> **Target Companies:** Google DeepMind, OpenAI, Anthropic, Meta AI (FAIR), NVIDIA, xAI, Cohere
> **Target Levels:** Senior ML Engineer / AI Engineer / Research Engineer

---

## What AI Engineer Interviews Test

| Round | Weight | What They Want |
|-------|--------|----------------|
| **ML Theory & Fundamentals** | 30% | Deep understanding of transformers, attention, training, optimization |
| **ML Systems Design** | 25% | Serving infra, training pipelines, data pipelines, MLOps |
| **Coding** | 25% | DSA + ML implementation (custom attention, loss functions, data loaders) |
| **Research Depth** | 15% | Paper reading ability, staying current, understanding SOTA |
| **Behavioral** | 5% | Collaboration, ambiguity tolerance, research taste |

---

## Study Plan

→ [[Study-Plan|Detailed Week-by-Week Study Plan]]

### Quick Priority Matrix

| Topic | Priority | Your Level | Target Level | Vault Resource |
|-------|----------|-----------|-------------|----------------|
| Transformer Architecture | Critical | | 5/5 | [[13-Agentic-AI/Agentic_AI_Zero_to_Godhood]] |
| Attention Mechanisms | Critical | | 5/5 | |
| Training & Optimization | Critical | | 4/5 | |
| LLMs (GPT, Claude, Gemini) | Critical | | 5/5 | [[13-Agentic-AI/Agentic_AI_Zero_to_Godhood]] |
| RAG & Agentic Systems | Critical | | 4/5 | [[13-Agentic-AI/Agentic_AI_Zero_to_Godhood]] |
| ML System Design | High | | 4/5 | |
| Distributed Training | High | | 4/5 | |
| RLHF / Alignment | High | | 4/5 | |
| Coding (Python + DSA) | High | | 4/5 | [[03-Data-Structures-Algorithms/README\|03-Data-Structures-Algorithms]] |
| MLOps / Serving | Medium | | 3/5 | |
| Classical ML | Medium | | 3/5 | |

---

## Target Companies

```dataview
TABLE WITHOUT ID
  file.link AS "Company", industry AS "Industry", status AS "Status"
FROM "16-Interview-Command-Center/02-Companies"
WHERE contains(target_roles, "AI-Engineer")
SORT file.name ASC
```

## Active Interviews

```dataview
TABLE WITHOUT ID
  company AS "Company", level AS "Level", stage AS "Stage",
  confidence + "/5" AS "Conf", next_deadline AS "Deadline"
FROM "16-Interview-Command-Center/03-Pipeline"
WHERE role = "AI-Engineer" AND stage != "rejected" AND stage != "withdrawn"
SORT next_deadline ASC
```

---

## Role Resources
- → [[Skill-Matrix]] | → [[Question-Bank]] | → [[Common-Patterns]] | → [[Resources]]

## Key Vault Links
| Domain | Link |
|--------|------|
| Agentic AI Zero to Godhood | [[13-Agentic-AI/Agentic_AI_Zero_to_Godhood]] |
| System Design | [[04-System-Design/README\|04-System-Design]] |
| Python | [[02-Programming-Languages/Python]] |
| DSA | [[03-Data-Structures-Algorithms/README\|03-Data-Structures-Algorithms]] |
