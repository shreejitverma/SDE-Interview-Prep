# 📊 Retrospective Dashboard

> Post-interview analysis hub. Track patterns across all your interviews.

---

## 📈 Overall Performance

```dataview
TABLE WITHOUT ID
  length(filter(rows, (r) => r.outcome = "pass")) AS "Pass",
  length(filter(rows, (r) => r.outcome = "fail")) AS "Fail",
  length(filter(rows, (r) => r.outcome = "waitlisted")) AS "Waitlisted",
  length(rows) AS "Total"
FROM "16-Interview-Command-Center/04-Retrospectives"
WHERE outcome != null AND !contains(file.name, "Dashboard") AND !contains(file.name, "Strengths") AND !contains(file.name, "Weaknesses")
```

## 📝 Recent Retrospectives

```dataview
TABLE WITHOUT ID
  file.link AS "Retro",
  company AS "Company",
  role AS "Role",
  round AS "Round",
  outcome AS "Outcome",
  performance + "/5" AS "Perf",
  difficulty + "/5" AS "Diff",
  date AS "Date"
FROM "16-Interview-Command-Center/04-Retrospectives"
WHERE outcome != null AND !contains(file.name, "Dashboard") AND !contains(file.name, "Strengths") AND !contains(file.name, "Weaknesses")
SORT date DESC
```

## 🎯 Performance by Role

```dataview
TABLE WITHOUT ID
  role AS "Role",
  round(average(rows.performance), 1) AS "Avg Perf",
  round(average(rows.difficulty), 1) AS "Avg Diff",
  length(rows) AS "Count"
FROM "16-Interview-Command-Center/04-Retrospectives"
WHERE performance != null AND !contains(file.name, "Dashboard") AND !contains(file.name, "Strengths") AND !contains(file.name, "Weaknesses")
GROUP BY role
```

## 🔍 Pattern Analysis
- → [[Patterns/Strengths|✅ Emerging Strengths]]
- → [[Patterns/Weaknesses|❌ Recurring Weaknesses]]

---

> **After every interview:** Create a retrospective using `_Templates/Retrospective` within 1 hour while memory is fresh.
