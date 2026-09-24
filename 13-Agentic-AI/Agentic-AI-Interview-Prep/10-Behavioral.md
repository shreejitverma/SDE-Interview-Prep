---
type: playbook
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: []
---

# Behavioral

STAR format: Situation, Task, Action, Result.
For AI roles, interviewers listen for three extra things: judgment about when not to use AI, honesty about failures, and measurement instead of anecdotes.

## Rules for every story

- 60-90 seconds spoken; Situation and Task in two sentences, most of the time on Action.
- Say "I," not "we," for the actions you took; name what the team did separately.
- End with a number and a lesson; if there is no measured number, say what you observed and how you would measure it now.
- Have one story ready per theme below, and practice each out loud at least twice.
- The three harness stories in [STAR Stories](../Agentic-Harness/interview/STAR-Stories.md) are fully evidenced (commits, PRs, gate run IDs); prefer them when the question allows.

## Story map

| Theme | Primary story | Backup |
| --- | --- | --- |
| AI system failed or misbehaved | LogiNext tool hallucinated file paths (Q64) | Gate caught my hook failing open ([Story 1](../Agentic-Harness/interview/STAR-Stories.md)) |
| Drove adoption | LogiNext debugging tool (Q65) | BofA AI/ML campaign |
| Led a team | 12 engineers at LogiNext (Q66) | **[fill in]** |
| Speed versus quality | **[fill in]** (Q67) | Harness quota exhaustion mid-delivery ([Story 3](../Agentic-Harness/interview/STAR-Stories.md)) |
| Production incident | **[fill in: trading stack]** | Scheduled sync took 3.5 hours (harness backup story B1) |
| Change management | Fork that stopped being a mirror ([Story 2](../Agentic-Harness/interview/STAR-Stories.md)) | **[fill in]** |

---

## Q64. Tell me about a time an AI system you built failed or misbehaved.

**Primary: the LogiNext debugging tool hallucinated file paths.**

- **Situation:** the tool proposed root causes and fixes from logs and stack traces; early users reported fixes that referenced files and functions that did not exist.
- **Task:** restore trust without killing adoption.
- **Action:** grounded answers in retrieved code, required every claim to cite a retrieved chunk, validated cited paths against the repository before showing the answer, and showed "no grounded answer" instead of guessing.
  **[fill in: how you detected it, how you measured the hallucination rate before and after]**
- **Result:** **[fill in: hallucinated-path rate before and after, adoption trend]**.
- **Lesson:** the model's fluency is not evidence; verification in code is.

**Backup: small local models failing at tool calls in the Sovereign platform.**
Fix: constrained JSON-schema decoding, fewer tools per step, and validation errors fed back for retries.
**[fill in: tool-call success rate before and after]**

---

## Q65. How did you drive adoption of an AI tool?

Use the LogiNext debugging tool (company-wide) or the BofA AI/ML campaign (64 use cases, 2,500+ employees).

- Adoption answers should cover: who the first users were and why, how you lowered friction (where the tool lived in their workflow), how you handled skeptics, and what usage metric you tracked.
- A strong line: "I measured weekly active users and time-to-resolution, not demo reactions."
- **[fill in: the adoption curve, the one change that most increased usage]**

---

## Q66. Tell me about leading a team.

Use the 12-engineer team at LogiNext: conflict resolution, how you prioritized, a technical decision you owned.

- Pick **one** decision with a real trade-off (for example, rewriting a routing component versus patching it) and explain how you decided and what happened.
- Include one moment of disagreement and how it resolved.
- **[fill in]**

---

## Q67. A time you made a trade-off between speed and quality.

- The best answers show that you protected what could not be undone (correctness, data, safety) and traded what could (polish, scope).
- Harness example: during a quota exhaustion mid-delivery, work fell back to a different model, and the gate still applied the same checks, so speed changed but the quality bar did not ([Story 3](../Agentic-Harness/interview/STAR-Stories.md)).
- **[fill in: a trading or LogiNext example with a deadline]**

---

## Q68. How do you stay current in a field that changes monthly?

- Papers (selectively, via summaries and citations from people I trust), engineering blogs from the model labs, protocol and framework changelogs, and building things.
- Concrete proof: I run my own agent harness daily and keep its 22 forks synced, so I see breaking changes as they land; for example, the MCP Python SDK renamed `FastMCP` to `MCPServer` in 2.x, which I hit while testing examples for this prep.
- I keep a written log of what changed and what it means for my systems.

---

## Q69. When would you not use an agent?

- When a deterministic script or a single LLM call suffices.
- When actions are high-stakes and cannot be verified.
- When latency budgets are tight.
- When there is no way to evaluate success.

Saying this unprompted signals maturity.
A good closing line: "I start with the simplest thing that works and add autonomy only where I can verify its output."

---

## Expansion questions (B1-B10)

**B1. Tell me about a time you disagreed with a technical decision.**
Show that you disagreed with data, committed once decided, and followed up on the outcome.

**B2. Tell me about a time you were wrong.**
Pick a real technical misjudgment; spend most of the time on how you found out and what you changed in your process.
The harness defects table (for example, `sync-forks` exiting 0 on failure) is a ready source; see the [Executive Summary](../Agentic-Harness/00-Executive-Summary.md).

**B3. Tell me about working with ambiguous requirements.**
Show how you turned ambiguity into a written spec with a measurable success criterion before building.

**B4. A stakeholder wants "an AI agent" for a problem a script would solve. What do you do?**
Ask what outcome they need, prototype the simplest solution, and show the comparison on cost, reliability, and latency; keep the door open for an agent where the problem really is open-ended.

**B5. Tell me about a production incident you handled.**
Timeline, detection, mitigation first, root cause second, and the systemic fix (test, alert, or guard) that prevents recurrence.
**[fill in: one trading-stack incident]**

**B6. How do you mentor engineers who are new to LLM systems?**
Pair on evals first: once someone has seen a model fail on real cases, they design more defensively.

**B7. Describe a time you had to influence without authority.**
Use data and a small working prototype rather than argument; the gate and generated-manual approach in the harness is an example of making the right path the easy path.

**B8. Have you ever raised an ethical or safety concern about an AI feature?**
Describe the concern, how you raised it, and the concrete control that resulted (approval step, data minimization, audit log).
**[fill in]**

**B9. How do you prioritize when everything is urgent?**
Rank by irreversibility and blast radius first, then by value per effort; write the ranking down and share it.

**B10. Why this company?**
Tie their product and stage to your production-discipline pitch; name one specific thing about their agents, customers, or engineering blog.
**[fill in per company]**
