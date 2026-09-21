---
week: "<% tp.date.now('YYYY-[W]ww') %>"
week_start: <% tp.date.now("YYYY-MM-DD", -6) %>
date: <% tp.date.now("YYYY-MM-DD") %>
total_problems_solved: 0
total_study_hours: 0
interviews_done: 0
offers_received: 0
rejections_received: 0
tags:
  - weekly-review
---

# Weekly Review - <% tp.date.now('YYYY [Week] ww') %>

---

## This Week's Stats

| Metric | Target | Actual | Delta |
|--------|--------|--------|-------|
| Problems Solved | | `= this.total_problems_solved` | |
| Study Hours | | `= this.total_study_hours` | |
| Interviews | | `= this.interviews_done` | |
| Mock Interviews | | | |
| New Applications | | | |

## Top 3 Wins
1. 
2. 
3. 

## Top 3 Gaps Exposed
1. **Gap:** → **Action:**
2. **Gap:** → **Action:**
3. **Gap:** → **Action:**

## Pipeline update

Generated from tracker and round notes for the 7 days ending this review.

### New applications

```dataview
TABLE WITHOUT ID file.link AS "Application", company AS "Company", source AS "Source"
FROM "16-Interview-Command-Center/03-Pipeline"
WHERE applied AND date(applied) >= this.file.day - dur(7 days) AND date(applied) <= this.file.day
SORT applied ASC
```

### Rounds this week

```dataview
TABLE WITHOUT ID file.link AS "Round", company AS "Company", outcome AS "Outcome"
FROM "16-Interview-Command-Center/03-Pipeline"
WHERE type = "round" AND date(date) >= this.file.day - dur(7 days) AND date(date) <= this.file.day
SORT date ASC
```

### Stalled (next action overdue)

```dataview
TABLE WITHOUT ID file.link AS "Application", next_action AS "Next action", next_action_date AS "Was due"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE next_action_date AND date(next_action_date) < this.file.day
  AND !contains(list("offer", "rejected", "withdrawn", "ghosted"), stage)
SORT next_action_date ASC
```

Full numbers: [[_Pipeline-Stats]].

## Knowledge Consolidation
> What concepts solidified this week? What "aha" moments happened?


## Next Week's Priorities
1. [ ] 
2. [ ] 
3. [ ] 
4. [ ] 
5. [ ] 

## Confidence Trend
| Role | Last Week | This Week | Trend |
|------|-----------|-----------|-------|
| SDE | /5 | /5 |//|
| Quant Dev | /5 | /5 | |
| Quant Research | /5 | /5 | |
| AI Engineer | /5 | /5 | |
| Low Latency | /5 | /5 | |

## Strategy Adjustments
> Based on this week, what should I change in my approach?

