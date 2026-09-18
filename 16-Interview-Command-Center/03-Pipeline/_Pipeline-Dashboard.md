# 📊 Interview Pipeline Dashboard

> **Active interviews tracked here.** Create new interviews using the `_Templates/Interview-Note` template.

---

## 🔴 Requiring Immediate Action
```dataview
TABLE WITHOUT ID
  company AS "Company",
  role AS "Role",
  stage AS "Stage",
  next_action AS "Action Needed",
  next_deadline AS "Deadline"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE next_deadline != null AND next_deadline <= date(today) + dur(3 days)
SORT next_deadline ASC
```

## 📋 Full Pipeline by Stage

### 🎯 Applied
```dataview
TABLE WITHOUT ID
  file.link AS "Interview / Track",
  company AS "Company",
  role AS "Role",
  manager AS "Manager",
  salary_range AS "Rate",
  date_applied AS "Applied",
  priority AS "Priority"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE stage = "applied"
SORT date_applied DESC
```


### 📞 Recruiter Call
```dataview
TABLE WITHOUT ID company AS "Company", role AS "Role", level AS "Level", next_deadline AS "Next Step"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE stage = "recruiter-call" OR stage = "recruiter-screen"
SORT next_deadline ASC
```

### 💻 Online Assessment / Coding Challenge (OA)
```dataview
TABLE WITHOUT ID
  file.link AS "Interview / Track",
  company AS "Company",
  role AS "Role",
  next_action AS "Assessment Scope",
  next_deadline AS "Deadline",
  priority AS "Priority"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE stage = "technical-assessment"
SORT next_deadline ASC
```


### 📱 Phone Screen
```dataview
TABLE WITHOUT ID company AS "Company", role AS "Role", level AS "Level", confidence + "/5" AS "Confidence"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE stage = "phone-screen"
SORT next_deadline ASC
```

### 🎯 Active Interviews & Final Rounds
```dataview
TABLE WITHOUT ID
  file.link AS "Interview / Track",
  company AS "Company",
  role AS "Role",
  stage AS "Stage",
  next_action AS "Next Action",
  next_deadline AS "Target Date",
  priority AS "Priority"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE stage = "Interview" OR stage = "Final Round" OR stage = "technical" OR stage = "onsite"
SORT next_deadline ASC
```

### 📞 Recruiter Screen & Outreach
```dataview
TABLE WITHOUT ID
  file.link AS "Track",
  company AS "Company",
  role AS "Role",
  next_action AS "Action Needed",
  priority AS "Priority"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
WHERE stage = "recruiter-screen" OR stage = "applied" OR stage = "outreach"
SORT priority ASC
```

---

## 📈 Pipeline Stats

```dataview
TABLE WITHOUT ID
  length(rows) AS "Count",
  key AS "Stage"
FROM "16-Interview-Command-Center/03-Pipeline/Active"
GROUP BY stage AS key
```

## 🗄️ Archived & Post-Mortem Bench (Recent Top Funds)

```dataview
TABLE WITHOUT ID
  file.link AS "Company Tracker",
  company AS "Company",
  role AS "Role",
  track AS "Track",
  date_rejected AS "Outcome Date",
  rejection_reason AS "Outcome / Status"
FROM "16-Interview-Command-Center/03-Pipeline/Archive"
SORT date_rejected DESC
```

