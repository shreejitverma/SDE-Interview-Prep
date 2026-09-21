---
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: solid
last_reviewed:
sources: []
---

# Review queue

Spaced review of the vault's knowledge notes, one list per track.
A note is due when it has never been reviewed, or when its last review is older than its interval: `draft` 7 days, `solid` 21 days, `canonical` 60 days.
`seed` notes are not written yet and never appear here.
After reviewing a note, set its `last_reviewed` property to today; it leaves the queue until its interval passes again.
Weak topics from your retrospectives are listed on the [[16-Interview-Command-Center/00-Dashboard|dashboard]]; review those first before an onsite.

## Software engineering

```dataview
TABLE WITHOUT ID file.link AS "Note", status AS "Status", level AS "Level", last_reviewed AS "Last reviewed"
FROM ""
WHERE contains(track, "sde") AND type != "moc" AND status AND status != "seed"
  AND !contains(file.path, "/_archive/") AND !contains(file.path, "16-Interview-Command-Center/")
  AND (!last_reviewed OR date(today) - date(last_reviewed) > choice(status = "canonical", dur(60 days), choice(status = "solid", dur(21 days), dur(7 days))))
SORT last_reviewed ASC, level ASC, file.name ASC
LIMIT 15
```

## Quant developer

```dataview
TABLE WITHOUT ID file.link AS "Note", status AS "Status", level AS "Level", last_reviewed AS "Last reviewed"
FROM ""
WHERE contains(track, "quant-dev") AND type != "moc" AND status AND status != "seed"
  AND !contains(file.path, "/_archive/") AND !contains(file.path, "16-Interview-Command-Center/")
  AND (!last_reviewed OR date(today) - date(last_reviewed) > choice(status = "canonical", dur(60 days), choice(status = "solid", dur(21 days), dur(7 days))))
SORT last_reviewed ASC, level ASC, file.name ASC
LIMIT 15
```

## Quant researcher

```dataview
TABLE WITHOUT ID file.link AS "Note", status AS "Status", level AS "Level", last_reviewed AS "Last reviewed"
FROM ""
WHERE contains(track, "quant-research") AND type != "moc" AND status AND status != "seed"
  AND !contains(file.path, "/_archive/") AND !contains(file.path, "16-Interview-Command-Center/")
  AND (!last_reviewed OR date(today) - date(last_reviewed) > choice(status = "canonical", dur(60 days), choice(status = "solid", dur(21 days), dur(7 days))))
SORT last_reviewed ASC, level ASC, file.name ASC
LIMIT 15
```

## Low-latency systems

```dataview
TABLE WITHOUT ID file.link AS "Note", status AS "Status", level AS "Level", last_reviewed AS "Last reviewed"
FROM ""
WHERE contains(track, "low-latency") AND type != "moc" AND status AND status != "seed"
  AND !contains(file.path, "/_archive/") AND !contains(file.path, "16-Interview-Command-Center/")
  AND (!last_reviewed OR date(today) - date(last_reviewed) > choice(status = "canonical", dur(60 days), choice(status = "solid", dur(21 days), dur(7 days))))
SORT last_reviewed ASC, level ASC, file.name ASC
LIMIT 15
```

## AI engineering

```dataview
TABLE WITHOUT ID file.link AS "Note", status AS "Status", level AS "Level", last_reviewed AS "Last reviewed"
FROM ""
WHERE contains(track, "ai-eng") AND type != "moc" AND status AND status != "seed"
  AND !contains(file.path, "/_archive/") AND !contains(file.path, "16-Interview-Command-Center/")
  AND (!last_reviewed OR date(today) - date(last_reviewed) > choice(status = "canonical", dur(60 days), choice(status = "solid", dur(21 days), dur(7 days))))
SORT last_reviewed ASC, level ASC, file.name ASC
LIMIT 15
```

## Staff and distinguished

```dataview
TABLE WITHOUT ID file.link AS "Note", status AS "Status", level AS "Level", last_reviewed AS "Last reviewed"
FROM ""
WHERE contains(track, "distinguished") AND type != "moc" AND status AND status != "seed"
  AND !contains(file.path, "/_archive/") AND !contains(file.path, "16-Interview-Command-Center/")
  AND (!last_reviewed OR date(today) - date(last_reviewed) > choice(status = "canonical", dur(60 days), choice(status = "solid", dur(21 days), dur(7 days))))
SORT last_reviewed ASC, level ASC, file.name ASC
LIMIT 15
```
