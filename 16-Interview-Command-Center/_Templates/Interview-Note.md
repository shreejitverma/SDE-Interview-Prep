---
company: "<% tp.system.prompt('Company name?') %>"
role: "<% tp.system.suggester(['SDE', 'Quant-Dev', 'Quant-Research', 'AI-Engineer', 'Low-Latency'], ['SDE', 'Quant-Dev', 'Quant-Research', 'AI-Engineer', 'Low-Latency']) %>"
level: "<% tp.system.prompt('Level (e.g., L5, VP, Senior)?') %>"
stage: "<% tp.system.suggester(['applied', 'recruiter-call', 'phone-screen', 'technical', 'onsite', 'team-match', 'offer', 'rejected', 'withdrawn'], ['applied', 'recruiter-call', 'phone-screen', 'technical', 'onsite', 'team-match', 'offer', 'rejected', 'withdrawn']) %>"
date_applied: <% tp.date.now("YYYY-MM-DD") %>
next_action: ""
next_deadline: ""
referral: ""
recruiter: ""
recruiter_email: ""
confidence: 3
priority: "<% tp.system.suggester(['high', 'medium', 'low'], ['high', 'medium', 'low']) %>"
salary_range: ""
location: ""
remote: false
tags:
  - interview
  - active
---

# <% tp.file.title %>

> **Company:** `= this.company` | **Role:** `= this.role` | **Level:** `= this.level`
> **Stage:** `= this.stage` | **Priority:** `= this.priority` | **Confidence:** `= this.confidence`/5

---

## Interview Timeline

| Date | Round | Interviewer | Format | Duration | Status |
|------|-------|-------------|--------|----------|--------|
| | | | | | |

## Pre-Interview Prep

### Key Topics to Review
- [ ] 
- [ ] 
- [ ] 

### Company-Specific Prep
- [ ] Review company profile: `= "[[" + this.company + "]]"`
- [ ] Research recent engineering blog posts
- [ ] Prepare company-specific "Why here?" answer
- [ ] Review Glassdoor/Blind interview reports

### Questions to Ask Interviewer
1. 
2. 
3. 

## Interview Notes

### Round 1
**Date:**
**Interviewer:**
**Questions Asked:**

**My Approach:**

**How It Went:**

---

## Post-Interview

### Gut Feel (immediately after)
- Energy level: /5
- How I think it went: /5
- Would I want to work here: /5

### Next Steps
- [ ] Send thank-you email
- [ ] Update stage in frontmatter
- [ ] Create retrospective note
- [ ] Update study plan based on gaps

## Related
- Role Hub: `= "[[01-Roles/" + this.role + "/_Hub]]"`
- Company Profile: `= "[[02-Companies/" + this.company + "]]"`
