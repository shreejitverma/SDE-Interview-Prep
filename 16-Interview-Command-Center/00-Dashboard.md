---
type: moc
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Interview Command Center

> *Last updated: `= date(today)`*
> Your mission control for clearing every interview across all target roles.

---

## Pipeline

[[03-Pipeline/_Pipeline-Dashboard|Full dashboard]] - [[03-Pipeline/Pipeline-Board|Board]] - [[03-Pipeline/_Inbox-Review|Inbox review]] - [[03-Pipeline/_Pipeline-Stats|Statistics]]

### Active

```dataview
TABLE WITHOUT ID
  file.link AS "Application",
  company AS "Company",
  stage AS "Stage",
  next_action AS "Next action",
  next_action_date AS "Due",
  priority AS "Priority"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE stage AND type != "round" AND !contains(list("rejected", "withdrawn", "ghosted"), stage)
SORT choice(priority = "high", 1, choice(priority = "medium", 2, 3)) ASC, next_action_date ASC
```

### Overdue follow-ups

```dataview
TABLE WITHOUT ID file.link AS "Application", next_action AS "Next action", next_action_date AS "Was due"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE stage AND type != "round" AND next_action_date AND date(next_action_date) < date(today)
  AND !contains(list("offer", "rejected", "withdrawn", "ghosted"), stage)
SORT next_action_date ASC
```

---

## Quick actions

Run these from the command palette (QuickAdd) or the QuickAdd ribbon icon.

| Macro | What it does |
| :--- | :--- |
| New application | Creates a tracker from `_Templates/Application` in `03-Pipeline/Active/<Company>/`. |
| Log interview round | Picks an application, files a round note next to it, optionally moves its stage, and adds a Timeline line. |
| Post-interview retro | Creates a retrospective linked to the application; fill `weak_topics`. |
| New STAR story | Creates a behavioral story in `05-Behavioral/Stories/`. |
| Weekly review | Creates this week's review with live pipeline tables. |

Email ingestion runs daily at 09:00; see [[03-Pipeline/Gmail-Sync-Guide|Gmail sync]].

---

## Weak topics from retrospectives

The topics you listed under `weak_topics` in retrospectives, most frequent first; review these before the next onsite.

```dataview
TABLE WITHOUT ID topic AS "Topic", length(rows) AS "Times", min(rows.date) AS "First seen", max(rows.date) AS "Last seen"
FROM "16-Interview-Command-Center/04-Retrospectives"
FLATTEN weak_topics AS topic
WHERE topic
GROUP BY topic
SORT length(rows) DESC
```

## Questions asked in interviews

Every list item tagged `#question/<topic>` in a round note.

```dataview
TABLE WITHOUT ID item.text AS "Question", file.link AS "Round"
FROM "16-Interview-Command-Center/03-Pipeline"
FLATTEN file.lists AS item
WHERE type = "round" AND any(item.tags, (t) => startswith(t, "#question"))
SORT file.name DESC
LIMIT 50
```

## Review queue

Knowledge notes due for review by status and last review date: [[00-Start-Here/Review-Queue|Review queue]].

---

## Role Hubs

| Role | Hub | Readiness |
|------|-----|-----------|
| Software Engineer | [[01-Roles/SDE/_Hub]] | → See Skill Matrix |
| Quantitative Developer | [[01-Roles/Quant-Dev/_Hub]] | → See Skill Matrix |
| Quantitative Researcher | [[01-Roles/Quant-Research/_Hub]] | → See Skill Matrix |
| AI Engineer | [[01-Roles/AI-Engineer/_Hub]] | → See Skill Matrix |
| Low Latency Systems | [[01-Roles/Low-Latency/_Hub]] | → See Skill Matrix |


---

## Company Intelligence

> [[02-Companies/_Company-Index|Full Company Index]]

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

## Recent Retrospectives

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

## Performance Trends

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

## Upcoming Deadlines (Next 7 Days)

```dataview
TABLE WITHOUT ID
  company AS "Company",
  role AS "Role",
  next_action AS "Action",
  next_action_date AS "Deadline"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE stage AND type != "round" AND next_action_date != null AND next_action_date <= date(today) + dur(7 days)
SORT next_action_date ASC
```

---

## Vault Knowledge Base

| Domain | Location | For Roles |
|--------|----------|-----------|
| CS Foundations | [[01-CS-Foundations/README\|01-CS-Foundations]] | All |
| DSA & Patterns | [[03-Data-Structures-Algorithms/README\|03-Data-Structures-Algorithms]] | SDE, Quant Dev |
| System Design | [[04-System-Design/README\|04-System-Design]] | SDE, AI Eng, Low Latency |
| Quant Finance | [[05-Quantitative-Finance/README\|05-Quantitative-Finance]] | Quant Dev, Quant Research |
| Behavioral | [[06-Interview-Prep/README\|06-Interview-Prep]] | All |
| Performance Engineering | [[12-Performance-Engineering/README\|12-Performance-Engineering]] | Low Latency, SDE |
| Agentic AI | [[13-Agentic-AI/README\|13-Agentic-AI]] | AI Engineer |
| Low Latency Systems | [[14-Low-Latency-Systems/00 Home\|14-Low-Latency-Systems]] | Low Latency, Quant Dev |
| Technical Whitepapers | [[15-Technical-Whitepapers/README\|15-Technical-Whitepapers]] | All |

---

> *"The impediment to action advances action. What stands in the way becomes the way." - Marcus Aurelius*

<!-- moc:start (generated by tools/build_mocs.py; edits inside are overwritten) -->
## Also in this folder

**Sections**

- [Roles](01-Roles/README.md)
- [Templates](_Templates/README.md)

**Notes**

- [The Ultimate Interview Coaching Prompt](Coaching-Prompt.md)

<!-- moc:end -->
