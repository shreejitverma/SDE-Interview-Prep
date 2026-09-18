---
date: <% tp.date.now("YYYY-MM-DD") %>
partner: "<% tp.system.prompt('Mock partner (name or AI)?') %>"
role: "<% tp.system.suggester(['SDE', 'Quant-Dev', 'Quant-Research', 'AI-Engineer', 'Low-Latency'], ['SDE', 'Quant-Dev', 'Quant-Research', 'AI-Engineer', 'Low-Latency']) %>"
type: "<% tp.system.suggester(['coding', 'system-design', 'behavioral', 'math/brainteaser', 'domain-specific', 'full-loop'], ['coding', 'system-design', 'behavioral', 'math-brainteaser', 'domain-specific', 'full-loop']) %>"
target_company: ""
duration_minutes: 45
overall_score: 3
would_hire: "lean-yes"
tags:
  - mock-interview
---

# 🎤 Mock Interview: <% tp.file.title %>

> **Partner:** `= this.partner` | **Role:** `= this.role` | **Type:** `= this.type`
> **Target Company:** `= this.target_company` | **Duration:** `= this.duration_minutes` min
> **Overall Score:** `= this.overall_score`/5 | **Hire Decision:** `= this.would_hire`

---

## 📋 Scoring Rubric

| Dimension | Score (1-5) | Notes |
|-----------|-------------|-------|
| Problem Solving | | |
| Code Quality | | |
| Communication | | |
| Edge Cases | | |
| Time Management | | |
| Optimality | | |

## 📝 Questions & Performance

### Question 1
**Problem:**
**Difficulty:** /5
**My Solution:**
**Optimal Solution:**
**Gap:**

## 🗣️ Feedback Received
### Strengths
- 

### Areas for Improvement
- 

## 📋 Action Items
- [ ] 
- [ ] 
