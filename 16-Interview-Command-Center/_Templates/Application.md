<%*
const company = (await tp.system.prompt("Company")) ?? "";
const role = (await tp.system.prompt("Role (as posted)")) ?? "";
const tracks = ["sde", "quant-dev", "quant-research", "low-latency", "ai-eng"];
const track = (await tp.system.suggester(tracks, tracks, false, "Track")) ?? "sde";
const source = (await tp.system.prompt("Source (Direct Application, LinkedIn, Referral, agency name)")) ?? "";
const referrer = source.toLowerCase().includes("referral") ? ((await tp.system.prompt("Referred by")) ?? "") : "";
const today = tp.date.now("YYYY-MM-DD");
const slug = (s) => s.trim().replace(/[^A-Za-z0-9]+/g, "-").replace(/^-|-$/g, "");
const name = `${slug(company)}-${slug(role)}-Tracker`;
await tp.file.move(`16-Interview-Command-Center/03-Pipeline/Active/${slug(company)}/${name}`);
-%>
---
company: "<% company %>"
role: "<% role %>"
track: [<% track %>]
level:
source: "<% source %>"
referrer: "<% referrer %>"
applied: <% today %>
stage: applied
status: ""
next_action: "Follow up if no reply"
next_action_date: <% tp.date.now("YYYY-MM-DD", 10) %>
priority: medium
confidence: 3
comp_band: ""
recruiter: ""
recruiter_email: ""
rejection_reason: ""
links: []
tags:
  - application
---

# <% company %>: <% role %>

Schema and stages: [[_Application-Schema]].
Change `stage` in the properties above as the process moves; the board and statistics follow it.

## Rounds

```dataview
TABLE WITHOUT ID file.link AS "Round", date AS "Date", outcome AS "Outcome"
FROM "16-Interview-Command-Center/03-Pipeline"
WHERE type = "round" AND contains(string(application), this.file.name)
SORT date ASC
```

## Prep

- [ ] Read the company profile and recent engineering posts
- [ ] Match the role to a track hub and its skill matrix
- [ ] Prepare the "why this team" answer and two STAR stories that fit

## Notes

## Timeline

- <% today %> applied via <% source || "unknown source" %>
