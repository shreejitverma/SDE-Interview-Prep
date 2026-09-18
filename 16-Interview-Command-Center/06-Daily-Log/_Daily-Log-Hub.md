# 📅 Daily Log

> Track your daily interview preparation. Use the `_Templates/Daily-Log` template for each day.
> Navigate via the Calendar plugin in the sidebar.

---

## This Week's Logs
```dataview
TABLE WITHOUT ID
  file.link AS "Day",
  energy + "/5" AS "Energy",
  focus + "/5" AS "Focus",
  total_problems AS "Problems",
  total_study_hours + "h" AS "Study"
FROM "16-Interview-Command-Center/06-Daily-Log"
WHERE date != null AND date >= date(today) - dur(7 days)
SORT date DESC
```

## Weekly Reviews
```dataview
TABLE WITHOUT ID
  file.link AS "Week",
  total_problems_solved AS "Problems",
  total_study_hours + "h" AS "Hours",
  interviews_done AS "Interviews"
FROM "16-Interview-Command-Center/06-Daily-Log"
WHERE contains(tags, "weekly-review")
SORT file.name DESC
LIMIT 8
```
