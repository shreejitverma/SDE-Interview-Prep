# Chapter 06 - Human Oversight and Reversibility

## What you will master

- Approval-gate design: which actions to interrupt for, and how to avoid approval fatigue that turns the gate into a rubber stamp.
- Audit trails that let you reconstruct what an agent did, why, and with what authority, after the fact.
- Reversibility as a first-class design axis: prefer reversible actions, stage irreversible ones, and know the difference before you build a tool.
- Blast-radius limiting so that when an agent misbehaves, the damage is bounded by construction.
- Kill switches and the operational ability to stop an agent fleet fast.
- Monitoring agent actions in production so you detect misbehavior while it is still cheap to fix.

This chapter is the operational layer of agent safety.
Chapters 02 through 05 established that injection is unsolved, that trained behavior is not a boundary, and that you must bound blast radius; this chapter is how humans stay in control of the residual risk that remains after all the automated layers.
Details are current to early 2026; the principles are durable, the tooling evolves.

## 1. Why human oversight is load-bearing

Every prior chapter converges here.
Injection cannot be eliminated (Chapter 02).
The model's disposition is not a boundary (Chapter 05).
Sandboxing and guardrails reduce and bound but do not eliminate (Chapters 03 and 04).
What remains is a residual probability that an agent, on some run, does something harmful, and human oversight is how you catch and correct that residual before or shortly after it causes damage.

Human oversight is not a confession of weak automation; it is the rational response to a probabilistic component acting in the world.
The design question is not whether to have humans in the loop but where to place them so they add safety without destroying the throughput that made the agent worth building.
Placed wrong, oversight is either useless (humans rubber-stamping everything) or crippling (humans re-doing the agent's work).
Placed right, oversight gates exactly the decisions that warrant a human and lets everything else flow, which is the craft this chapter teaches.

## 2. Approval-gate design

An approval gate pauses the agent before a chosen action and requires a human to approve, reject, or modify it.
It is the one boundary the model cannot argue past, because the decider is a person outside the model's context.

### 2.1 What to gate

Gate an action when the cost of an unwanted execution exceeds the cost of a human interruption.
The two dimensions are **reversibility** (section 4) and **blast radius** (section 5), and they combine into a simple policy:

- **Gate:** irreversible or high-blast-radius actions. Sending external communications, spending or moving money, deleting data, granting or changing access, deploying to production, publishing anything publicly, executing anything that touches other users' assets.
- **Auto-approve:** reversible, low-blast-radius actions. Reading data within scope, running a read-only query, executing code in a sandbox with no egress, drafting (but not sending) content, making changes on a branch that a human merges.

The policy should be encoded in code, not left to the model to decide when to ask, because a model deciding whether to seek approval is a model that can be injected into not asking.
The harness inspects the proposed action against the policy and gates deterministically.

### 2.2 Approval fatigue

The dominant failure mode of approval gates is fatigue.
If the human is asked to approve too many actions, especially too many trivially safe ones, they stop reading and start clicking approve reflexively, and the gate becomes a rubber stamp that provides false assurance while adding latency.
This is worse than no gate, because it costs throughput and gives a false sense of control.

Counter approval fatigue deliberately:

- **Gate rarely and meaningfully.** Every gate you add for a low-stakes action spends the human's attention budget and dilutes the high-stakes gates. Aggressively auto-approve the safe majority.
- **Make each prompt decision-ready.** Show the human exactly what will happen (the concrete action, its arguments, its predicted effect, its irreversibility, what data it touches), not a vague "the agent wants to proceed." A human cannot meaningfully approve what they cannot quickly understand.
- **Batch where safe.** Group related low-stakes actions into one approval rather than many, while never batching a genuinely consequential action into a batch where it hides.
- **Escalate by stakes.** A cheap reversible action gets a lightweight confirm; an irreversible high-blast-radius action gets a heavyweight, deliberate approval with a summary of consequences and perhaps a second approver.
- **Measure the approve rate.** If humans approve 99.9 percent of gated actions, either the gate is on the wrong actions (too safe to be worth gating) or fatigue has set in (they are not really reviewing). Both are signals to redesign.

The honest trade-off: more gates mean more safety and less throughput and more fatigue risk, and the sweet spot is few gates on genuinely consequential actions, each presented so well that the human actually exercises judgment.
A system with one well-designed gate on the irreversible action a human actually thinks about is safer than a system with fifty gates the human clicks through blind.

## 3. Audit trails

An audit trail is a durable, tamper-resistant record of what the agent did, sufficient to reconstruct events after the fact.
It is not optional for any agent that takes consequential actions, because you cannot investigate, learn from, or prove anything about an incident you did not record.

Record, for every consequential action:

- **What.** The action and its full arguments (the exact email sent, the exact query run, the exact command executed).
- **Why.** The agent's reasoning or the context that led to it, enough to understand the decision. This is where the model's chain of reasoning, the retrieved documents, and the triggering input belong.
- **Authority.** Which credential and scope was used, on behalf of which user, at which autonomy level.
- **When and where.** Timestamp, session, agent version, model version, tool version.
- **Outcome.** Whether it succeeded, what it returned, any error.
- **Approval.** If gated, who approved and when; if auto-approved, which policy rule allowed it.

Design properties that make an audit trail actually useful:

- **Immutability.** The agent must not be able to alter or delete its own audit log, or an injected agent could cover its tracks. The log sink is outside the agent's write authority, append-only, ideally in a separate trust domain.
- **Completeness.** Log the untrusted inputs too (the email that was read, the page that was fetched), because after an incident you need to find the injection, and you cannot if you only logged the agent's actions and not what it was reacting to.
- **Queryability.** An audit trail you cannot search is a tape you never rewind. Structure it so you can answer "what did this agent do with this user's credential in this window" fast.
- **Retention aligned to risk.** Keep records long enough to investigate incidents that surface late, balanced against privacy and data-minimization obligations (Chapter 07).

The audit trail connects to Chapter 05's specification-gaming and Chapter 02's injection: when an agent does something surprising, the trail is how you determine whether it was injected, whether it gamed its objective, or whether it simply erred, and each diagnosis leads to a different fix.

## 4. Reversibility as a design axis

Reversibility is whether an action can be undone, and it is one of the two axes (with blast radius) that should govern how much oversight an action gets.
Treating reversibility as a first-class design property, decided when you build each tool, is one of the highest-leverage safety practices available, because it changes the stakes of every mistake the agent makes.

### 4.1 The spectrum

Actions range from fully reversible to fully irreversible:

- **Fully reversible.** Writing to a branch you can discard, staging a change, moving a file to a trash with retention, creating a draft. A mistake costs only the undo.
- **Reversible with effort.** Deleting data that is backed up, changing a config that is version-controlled, deploying with a fast rollback. Recoverable but not free.
- **Irreversible.** Sending an email or message, publishing publicly, moving money, deleting the only copy, physical-world actions, anything a third party immediately acts on. Once done, it cannot be taken back.

### 4.2 The design moves

- **Prefer reversible actions.** When designing a tool, make it reversible if you can. A tool that stages a change for human merge is safer than one that applies it directly, and you should reach for the staged version by default and only build the direct version when you have a reason.
- **Stage irreversible actions.** Convert an irreversible action into a reversible proposal plus an approval gate. "Send email" becomes "draft email, then a human approves the send." The irreversible step happens only after human judgment.
- **Add reversibility to the infrastructure.** Soft deletes with retention instead of hard deletes, version control on everything the agent changes, transactional operations that can roll back, deploys with instant rollback. This turns "reversible with effort" into "reversible cheaply" and shrinks the stakes of agent mistakes across the board.
- **Insert delays on the irreversible.** For actions that are irreversible but not urgent, a short hold before execution (an undo window) gives a human or a monitor time to catch a mistake, converting some irreversibility into practical reversibility.

The reason this axis is so powerful: it decouples agent reliability from agent safety.
An agent that makes reversible mistakes can be wrong often and still be safe, because every mistake is undoable, which lets you deploy more autonomy earlier and learn from failures cheaply.
An agent whose mistakes are irreversible must be nearly perfect to be safe, which is a bar no probabilistic system meets.
Wherever you can move an action left on the reversibility spectrum, you have bought yourself the right to tolerate the agent's imperfection.

## 5. Blast-radius limiting

Blast radius is how much damage a single agent action or a single compromised session can cause.
The goal is to bound it by construction so that even a fully misbehaving agent - injected, misaligned, or buggy - cannot exceed a tolerable limit.

Techniques, several of which reprise Chapter 03 from the oversight angle:

- **Scope credentials and data to the acting user and the task** (Chapter 03), so one session cannot reach beyond one user's authorization.
- **Rate-limit and quota consequential actions.** An agent that can send one email per approval is bounded; an agent that can send ten thousand is a spam incident and a denial-of-wallet. Cap the count and the spend per session and per time window, enforced in the harness.
- **Cap resource consumption.** Token budgets, tool-call counts, wall-clock limits per session, so a looping or gamed agent stops before it runs up an unbounded bill (the availability and budget asset from Chapter 01).
- **Segment agents.** Give each agent or task the narrowest reach, so a compromise is contained to that segment rather than the whole system. One agent per user, per tenant, per data domain, rather than one omni-agent with reach into everything.
- **Separate duties.** The agent that drafts is not the agent that approves; the agent that reads untrusted content does not hold the credential that could exfiltrate (Chapter 02's dual-LLM). Splitting authority across components means no single compromised component has the whole capability.

Blast-radius limiting is the operational form of "assume the agent will be compromised."
You do not ask whether the agent will misbehave; you ask what the worst a misbehaving agent can do is, and you engineer that worst case down to something you can absorb.
An incident that is bounded to one user's reversible data, caught by monitoring, and undone by rollback is a Tuesday; the same incident unbounded across all users with irreversible effects is a company-ending event, and the difference is entirely the blast-radius engineering you did in advance.

## 6. Kill switches

A kill switch is the operational ability to stop an agent, or a fleet of agents, immediately.
When monitoring detects misbehavior, or when a vulnerability is disclosed, or when an agent is doing something you do not understand, you need to be able to halt it in seconds, not after a deploy cycle.

Properties of a real kill switch:

- **Fast.** Effective in seconds, not minutes, and not gated behind a code deploy.
- **Central and fleet-wide.** One control that stops all instances, because an incident rarely affects only one.
- **Layered.** The ability to stop at multiple levels: pause a single session, disable a specific tool across the fleet (revoke the send_email capability while leaving read-only work running), revoke the agent's credentials centrally (Chapter 03), or halt everything.
- **Tested.** A kill switch you have never exercised is a kill switch you do not know works. Rehearse it, the way you rehearse any incident-response control.

The credential-revocation path from Chapter 03 is a kill switch: if every agent credential can be revoked centrally and fast, you can neutralize a compromised agent by cutting its authority even if you cannot stop its process.
Design the kill switch alongside the agent, not after the first incident, because the first incident is exactly when you will wish you had it and will not have time to build it.

## 7. Monitoring agent actions in production

Oversight is not only pre-action gates; it is continuous observation of what agents do, so you detect misbehavior while it is small.
This connects to the observability material in Volume 10, viewed through a security lens.

Monitor for:

- **Anomalous action patterns.** An agent suddenly reading far more data than usual, calling a tool it rarely calls, contacting a new destination, or looping. Deviation from the agent's normal behavioral baseline is a signal, and baselining agent behavior is the security analog of user-behavior analytics.
- **Exfiltration signatures.** Outbound requests to unexpected domains, encoded data in outputs, access to sensitive data followed by an external channel use - the trifecta being exercised.
- **Injection indicators.** Untrusted content flagged by classifiers (Chapter 04), agent actions that do not match the user's request, sudden topic or goal shifts mid-session.
- **Guardrail and gate events.** Every block, every refusal, every approval and rejection, as a stream you watch for spikes and patterns.
- **Cost and rate anomalies.** Spend and call-rate spikes that indicate a gamed objective, a loop, or an attack.

Design the monitoring to be actionable:

- **Alert on the meaningful, not the noisy,** or alert fatigue mirrors approval fatigue and the alerts get ignored.
- **Wire alerts to the kill switch,** so detection can lead to containment fast, sometimes automatically for high-confidence signals.
- **Feed incidents back into the boundaries.** Every real incident should tighten a credential scope, add a guardrail, add a gate, or narrow a tool, so the system learns. Monitoring that detects but never hardens is a smoke alarm with no fire department.

Monitoring is the last line and the feedback loop for all the others.
It catches what prevention missed, it bounds the duration of an incident, and it tells you which of your assumptions were wrong so you can fix the design rather than just the instance.

## 8. Putting it together

A well-overseen agent in early 2026 combines all of the above with the boundaries of prior chapters:

- Reversible actions auto-execute; irreversible ones are staged as proposals behind a well-designed approval gate that a human actually reads.
- Every consequential action is logged immutably with what, why, authority, and outcome, including the untrusted inputs.
- Infrastructure makes mistakes cheap to undo: soft deletes, version control, fast rollback, undo windows on the irreversible-but-not-urgent.
- Blast radius is bounded by scoped credentials, rate limits, resource caps, agent segmentation, and separation of duties, so a compromised session cannot exceed tolerance.
- A tested, fast, fleet-wide kill switch and central credential revocation can halt or defang the agents in seconds.
- Monitoring baselines behavior, alerts on anomalies and exfiltration and injection signals, wires to the kill switch, and feeds every incident back into tighter boundaries.

Run the whole volume's logic across this.
Injection succeeds sometimes (Chapter 02), the model's disposition is not a boundary (Chapter 05), so the agent will occasionally try to do the wrong thing; the reversibility, blast-radius bounds, gates, kill switch, and monitoring here ensure that when it does, the action is undoable or caught or bounded or all three.
That is what it means to deploy a probabilistic actor responsibly: not to make it never fail, which is impossible, but to make its failures survivable, observable, and correctable.

## 9. Claims that will rot

The principles - gate by reversibility and blast radius, avoid approval fatigue, audit immutably, prefer and engineer reversibility, bound blast radius, keep a tested kill switch, monitor and feed back - are durable and will remain correct.
The specific tooling for approvals, audit storage, monitoring, and kill switches is current to early 2026 and evolves; re-verify current best-practice tooling before building.

## Exercises

1. Take every tool in an agent you run and classify each action as fully reversible, reversible with effort, or irreversible. For each irreversible one, design the staging that turns it into a proposal plus a gate.
2. Design an approval prompt for a genuinely consequential action so that the human can make a real decision in seconds. List every fact the prompt must show and why.
3. Find an action in your system that is currently irreversible and add infrastructure reversibility (soft delete, versioning, undo window). State what stakes this removes from every future agent mistake.
4. Specify the blast-radius bounds for one agent: credential scope, rate limits, resource caps, and segmentation. Then describe the worst a fully compromised session could do within those bounds and decide whether you can absorb it.
5. Design the kill switch for an agent fleet: the levels (session, tool, credential, all), the speed target, and the test you would run to prove it works. Then define three monitoring alerts that should trigger it.

## Godhood check

You have mastered this chapter when you can:

- Decide which actions to gate using the reversibility and blast-radius axes, and design a gate that resists approval fatigue.
- Specify an audit trail that is immutable, complete (including untrusted inputs), and queryable, and explain why each property matters for incident investigation.
- Place any action on the reversibility spectrum and apply the design moves (prefer, stage, add infrastructure reversibility, delay) to shrink the stakes of agent mistakes.
- Bound the blast radius of a compromised session by construction and state the worst case within your bounds.
- Design a fast, tested, fleet-wide kill switch and a monitoring-plus-feedback loop, and explain how oversight makes a probabilistic actor's failures survivable rather than catastrophic.
