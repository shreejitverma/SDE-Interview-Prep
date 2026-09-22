<%*
const ACTIVE = "16-Interview-Command-Center/03-Pipeline/";
const apps = app.vault.getMarkdownFiles()
  .filter(f => f.path.startsWith(ACTIVE) && app.metadataCache.getFileCache(f)?.frontmatter?.stage)
  .sort((a, b) => a.basename.localeCompare(b.basename));
const appFile = await tp.system.suggester(["(no application)", ...apps.map(f => f.basename)], [null, ...apps], false, "Which application?");
const afm = appFile ? (app.metadataCache.getFileCache(appFile)?.frontmatter ?? {}) : {};
-%>
---
application: "[[<% appFile ? appFile.basename : "" %>]]"
company: "<% afm.company ?? tp.system.prompt('Company name?') %>"
role: "<% tp.system.suggester(['SDE', 'Quant-Dev', 'Quant-Research', 'AI-Engineer', 'Low-Latency'], ['SDE', 'Quant-Dev', 'Quant-Research', 'AI-Engineer', 'Low-Latency']) %>"
round: "<% tp.system.prompt('Round (e.g., Phone Screen, Onsite R1, System Design)?') %>"
date: <% tp.date.now("YYYY-MM-DD") %>
outcome: "<% tp.system.suggester(['pass', 'fail', 'waitlisted', 'unknown'], ['pass', 'fail', 'waitlisted', 'unknown']) %>"
interviewer_name: ""
difficulty: 3
performance: 3
topics_tested:
  - 
weak_topics: []
categories:
  - "<% tp.system.suggester(['coding', 'system-design', 'behavioral', 'math', 'brain-teaser', 'low-latency', 'ml-theory', 'quant-finance', 'culture-fit'], ['coding', 'system-design', 'behavioral', 'math', 'brain-teaser', 'low-latency', 'ml-theory', 'quant-finance', 'culture-fit']) %>"
tags:
  - retro
---

# Retrospective: <% tp.file.title %>

> **Company:** `= this.company` | **Role:** `= this.role` | **Round:** `= this.round`
> **Date:** `= this.date` | **Outcome:** `= this.outcome`
> **Difficulty:** `= this.difficulty`/5 | **Performance:** `= this.performance`/5

---

## Questions Asked

### Question 1
**Topic:** 
**Difficulty:** /5
**Time Given:** min | **Time Used:** min

**Problem Statement:**


**Expected Solution:**


**My Approach:**


### Question 2
*(Copy the block above for additional questions)*

---

## ✓ What Went Well
- 
- 
- 

## ✗ What Went Wrong
- 
- 
- 

## Key Learnings
1. 
2. 
3. 

## Knowledge Gaps Exposed
| Gap | Severity (1-5) | Study Resource | Target Date |
|-----|----------------|----------------|-------------|
| | | | |

## Action Items
- [ ] 
- [ ] 
- [ ] 

## Time Analysis
| Question | Allotted | Actual | Verdict |
|----------|----------|--------|---------|
| Q1 | min | min | ✓/✗ |
| Q2 | min | min | ✓/✗ |

## If I Could Redo This Interview
> What would I do differently?


## Related
- Interview Note: `= "[[03-Pipeline/Active/" + this.company + "-" + this.role + "]]"`
- Company Profile: `= "[[02-Companies/" + this.company + "]]"`
