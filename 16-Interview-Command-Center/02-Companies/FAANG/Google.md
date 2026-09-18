---
company: Google
industry: FAANG
hq_location: Mountain View, CA
remote_policy: Hybrid (3 days in office)
target_roles: [SDE, AI-Engineer]
engineering_blog: "https://blog.google/technology/"
careers_page: "https://careers.google.com"
glassdoor_rating: 4.4
status: researching
tags: [company, faang]
---

# Google

## 🏢 Company Overview
**Industry:** FAANG | **HQ:** Mountain View, CA | **Remote:** Hybrid

### Mission & Values
"Organize the world's information and make it universally accessible and useful." Focus on 10x thinking, user focus, technical excellence.

### Engineering Culture
- Promotion committee system (no manager-only decisions)
- Design docs (go/designdoc) for all major projects
- Code reviews via Critique, monorepo, Blaze/Bazel builds
- 20% time (historically), strong IC track to L10 (Distinguished Engineer / Fellow)

---

## 💻 Tech Stack
| Layer | Technologies |
|-------|-------------|
| Languages | C++, Java, Python, Go |
| Infrastructure | Borg (→Kubernetes), Colossus, Spanner, Bigtable |
| Data | MapReduce/Flume, BigQuery, Pub/Sub |
| ML/AI | TensorFlow, JAX, TPUs, Gemini |

---

## 📋 Interview Process

### Stages
| # | Stage | Format | Duration |
|---|-------|--------|----------|
| 1 | Recruiter Screen | Phone | 30min |
| 2 | Technical Phone Screen | Google Meet + shared doc | 45min |
| 3 | Onsite (Virtual or In-Person) | 4-5 rounds | 4-5hrs |
| 4 | Hiring Committee Review | Packet review | 1-3 weeks |
| 5 | Team Match | Conversations with teams | 1-2 weeks |

### Onsite Breakdown (SDE L5)
- **2× Coding:** LeetCode medium-hard, clean code, edge cases
- **1× System Design:** Large-scale distributed systems
- **1× Behavioral (Googleyness & Leadership):** Culture fit, collaboration, ambiguity

### What They Test
- [x] DSA / Coding
- [x] System Design
- [x] Behavioral / Culture (Googleyness)
- [x] Leadership (for L5+)
- [ ] Take-Home

### Tips & Known Patterns
- Interviewers submit independent feedback — no groupthink
- Hiring committee values "clear strong hires" — aim for at least 2 "Strong Hire" signals
- System design: always start with requirements and do back-of-envelope calculations
- Googleyness: "intellectual humility," "thriving in ambiguity," "doing the right thing"
- Code in Google Docs (no autocomplete!) — practice without IDE

---

## 💰 Compensation
**Levels.fyi:** https://www.levels.fyi/companies/google/salaries
| Level | Base | Stock (4yr) | Bonus | TC |
|-------|------|-------------|-------|-----|
| L4 (SWE III) | $155k | $200k | 15% | ~$250k |
| L5 (Senior) | $185k | $350k | 15% | ~$370k |
| L6 (Staff) | $220k | $600k+ | 15% | ~$530k+ |

---

## 📊 My Applications
```dataview
TABLE role, stage, confidence, next_deadline
FROM "16-Interview-Command-Center/03-Pipeline"
WHERE company = "Google"
SORT next_deadline ASC
```

## 📝 My Retrospectives
```dataview
TABLE round, outcome, performance, date
FROM "16-Interview-Command-Center/04-Retrospectives"
WHERE company = "Google"
SORT date DESC
```
