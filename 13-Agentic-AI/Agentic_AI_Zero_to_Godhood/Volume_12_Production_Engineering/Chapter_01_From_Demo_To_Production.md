# Chapter 01 - From Demo to Production

## What you will master

- Why the demo-to-production gap for agents is wider than for any previous class of software, stated in terms of error compounding math.
- How per-step reliability multiplies over a trajectory, and what 0.99^n actually implies for autonomy budgets.
- The recurring failure modes that kill agent products in the wild, categorized so you can design against each one.
- How to define SLOs for systems whose failures are semantic, not just operational.
- How to scope autonomy to measured reliability, and the maturity ladder from copilot to fully autonomous.

## 1. The reliability gap

A demo is a single trajectory chosen by the person giving the demo.
Production is the distribution of all trajectories chosen by users who did not read your prompt, do not share your assumptions, and will find every edge in your tool schema within a week.
Traditional software has this gap too, but agents widen it for a structural reason: the core execution engine is probabilistic.
A deterministic service that passes its tests will pass them again tomorrow; a model that succeeded on a task at temperature 0 can still fail on a paraphrase of the same task, because the input distribution, not the random seed, is the dominant source of variance.

This produces a predictable emotional arc in teams building agent products.
The prototype works on twenty hand-picked tasks in an afternoon, which feels like ninety percent of the work.
The remaining ninety percent of the work is the gap between "works on tasks we thought of" and "degrades gracefully on tasks we did not."
As of early 2026 this arc is well documented across the industry: the median agent project that dies, dies in this second phase, not because the model was too weak but because the team never built the machinery to measure and manage semantic failure at scale.

The correct mental model is that a demo demonstrates capability while production requires reliability, and capability and reliability are different axes.
A model can be capable of a task, meaning the task is inside its competence envelope on a good sample, while being unreliable at it, meaning the success rate across the real input distribution is far below what the product needs.
Most agent engineering is the work of converting capability into reliability through decomposition, verification, retries, guardrails, and scoping.

## 2. Error compounding: the 0.99^n problem

Agents execute multi-step trajectories, and errors compound multiplicatively across steps when steps are independent.
If each step succeeds with probability p, an n-step trajectory that requires every step to succeed completes with probability p^n.

```
p = 0.99: p^10 ~ 0.904, p^20 ~ 0.818, p^50 ~ 0.605, p^100 ~ 0.366
p = 0.95: p^10 ~ 0.599, p^20 ~ 0.358, p^50 ~ 0.077
p = 0.999: p^100 ~ 0.905, p^500 ~ 0.607
```

Read those numbers slowly, because they set the physics of the field.
A step reliability of 99 percent, which sounds excellent, gives you a coin flip somewhere between 50 and 100 steps.
A step reliability of 95 percent, which is where many tool-calling steps actually sit on messy real inputs, means a 20-step workflow fails almost two times out of three.
To run 100-step trajectories at 90 percent task success you need roughly three nines per step, which no raw model call delivers on open-ended inputs as of early 2026.

Three things rescue agents from this arithmetic, and all of production agent engineering is some combination of them.

First, steps are not independent and failures are not absorbing.
A capable model detects many of its own errors from tool output and recovers, which converts per-step reliability into per-step "reliability after local retry," a much higher number.
This is why the ReAct-style loop with observable tool results outperforms open-loop planning: the environment provides an error signal, and recovery turns a multiplicative chain into something closer to a random walk with a restoring force.
The downside is that recovery consumes tokens, latency, and sometimes side effects, so recovery is a budget, not a free lunch.

Second, you can shorten n.
Decomposing a workflow so that any single autonomous run is 5 to 15 steps, with checkpoints where state is verified or a human confirms, keeps you on the friendly part of the exponential curve.
This is the deep reason why "smaller scoped agents composed by an orchestrator" beats "one heroic agent with 200 tools" in production, independent of any prompt-quality argument.

Third, you can raise p for the steps that matter with verification.
A step whose output is checked by a validator, a test suite, a schema check, or a second model has effective reliability p_step + (1 - p_step) * p_catch * p_retry_success.
Cheap deterministic verifiers (does the file compile, does the API return 200, does the JSON validate) are the highest-leverage reliability investment in the entire stack because they are near-perfect catchers at near-zero cost.

The compounding math also explains why long-horizon autonomy improved so sharply between 2023 and 2026: small improvements in per-step reliability produce superlinear improvements in feasible trajectory length.
Moving p from 0.98 to 0.995 does not feel like a big model improvement on a single-turn benchmark, but it moves the 50 percent task-completion horizon from roughly 34 steps to roughly 138 steps.
METR's time-horizon measurements, which track the length of tasks models can complete at a fixed success rate, are the cleanest public evidence of this effect; the measured horizon has been growing at a steady exponential, roughly doubling every several months through 2025.

## 3. Why agent products fail in the wild

Post-mortems of failed agent deployments cluster into a small set of causes.
Design against each one explicitly.

### 3.1 Distribution shift from demo inputs

The team tuned prompts against a curated task set, then real users arrived with ambiguous requests, missing context, adversarial phrasing, other languages, and inputs of wildly different length.
Mitigation: collect real traffic early behind a feedback flag, build the eval set from production traces rather than imagination, and treat the eval set as a living artifact (Volume 10 covers the mechanics).

### 3.2 Compounding without checkpoints

The product promised end-to-end autonomy on workflows of 30-plus steps with no intermediate verification, so even good per-step reliability produced regular end-to-end failure, and each failure burned user trust disproportionately.
Mitigation: shorten autonomous segments, verify at boundaries, and make partial progress durable so a failure loses one segment, not the whole job.

### 3.3 Silent semantic failure

The agent completed trajectories that looked successful, returned confident summaries, and was wrong: it edited the wrong record, cited a hallucinated policy, or marked a task done that it never finished.
This is the most dangerous class because operational monitoring shows green while the product corrodes.
Mitigation: independent verification of outcomes rather than trusting the agent's self-report, sampled human review, and quality metrics in the SLO set (section 4).

### 3.4 Cost and latency discovered late

The demo ran one session at a time on a fast model with a short context.
Production sessions accumulated 100k-token contexts, retried on failures, and fanned out to subagents, producing per-session costs and multi-minute latencies that the business model could not absorb.
Mitigation: cost and latency budgets per session defined before launch, enforced in code, with the engineering disciplines of Chapters 02 and 03.

### 3.5 Side-effect incidents

The agent had write access to something real (email, payments, production infrastructure, customer records) and a failure became an incident rather than a bad answer.
One public-facing incident of this class can end a product, because the trust asymmetry is brutal: users forgive a wrong answer far more easily than a wrong action.
Mitigation: irreversibility-scoped permissions, confirmation gates on destructive actions, idempotency (Chapter 04), and sandboxing (Chapter 05).

### 3.6 Nobody owned the model as a dependency

The provider deprecated a model version, changed behavior in an update, or had an outage, and the team had no pinning strategy, no migration evals, and no fallback.
Mitigation: treat the model like any other critical vendor dependency, with version pinning, upgrade testing, and multi-provider contingency (Chapters 06 and 07).

### 3.7 The product asked for more trust than it had earned

Autonomy was set by ambition rather than by measured reliability, users got burned, and they retreated to doing the task manually while the agent feature rotted.
Mitigation is the entire point of sections 5 and 6: autonomy must be earned by measurement, and it can be granted incrementally.

## 4. SLOs for agent systems

Classical SLOs cover availability, latency, and error rate, where "error" means an operational failure like a 5xx.
Agent systems need those plus a second layer, because an agent can be up, fast, and returning 200s while being wrong.

A production agent SLO set has four layers.

### 4.1 Operational SLOs

These are inherited from normal service engineering and are still required.
Examples: availability of the agent endpoint, p50 and p95 time-to-first-token, p95 session completion time, provider error rate after retries, queue wait time for background jobs.
The agent-specific twist is that latency distributions are heavy-tailed and multi-modal (one-tool-call sessions versus 40-step sessions), so define latency SLOs per session class, not globally, or the numbers will be meaningless.

### 4.2 Trajectory-health SLOs

These measure whether the loop itself is behaving, independent of task correctness.
Useful indicators: fraction of sessions terminating in a final answer versus hitting the step or token ceiling, tool-call failure rate by tool, loop-detection triggers (same tool with same arguments repeatedly), context-window exhaustion rate, and fraction of sessions requiring model fallback.
These are computable in real time from traces without any labeling, which makes them your fastest-alerting quality proxies.

### 4.3 Quality SLOs

These measure semantic success and require either automated grading or sampled human labels.
Examples: task success rate on a continuously-scored sample of production traffic, hallucinated-citation rate for research agents, unnecessary-action rate for side-effecting agents, and escalation correctness (did the agent hand off when it should have).
Quality SLOs are measured on samples with confidence intervals, lag real time by minutes to days, and are noisier than operational metrics; accept this rather than pretending a proxy metric is the real thing.
The standard architecture is an online grader (an LLM judge with a rubric, calibrated against periodic human labels) scoring a fixed sample rate of production sessions, with the human-agreement rate of the judge itself tracked as a meta-metric.

### 4.4 Safety SLOs

These bound the worst cases rather than the average: rate of guardrail triggers, rate of confirmed harmful or policy-violating actions, prompt-injection detection rate on canary probes, and time-to-kill for a misbehaving deployment.
Safety SLOs are the ones you design to never page because the underlying event should be near zero; when they do fire, they are incidents, not degradations.

### 4.5 Error budgets with semantic errors

Error-budget policy needs one modification for agents: a semantic-quality regression consumes budget the same way downtime does.
If the task-success SLO is 90 percent and a prompt change drops the measured rate to 84 percent, that is a budget-burning event that should freeze further prompt and model changes until quality recovers, exactly as an availability burn freezes risky deploys.
Teams that exempt prompt changes from error-budget discipline ship quality regressions continuously and discover them from churn data months later.

## 5. Scoping autonomy to reliability

The central design decision of an agent product is not which model to use but how much autonomy to grant, and the correct answer is a function of two measured quantities: per-step reliability on your task distribution, and the cost of an uncaught failure.

Formalize it as a simple expected-value frame.
Let p_task be measured end-to-end success, C_fail the cost of an uncaught failure (including trust damage, which dominates for user-facing actions), C_review the cost of human review, and V the value of successful automation.
Full autonomy is rational when p_task * V - (1 - p_task) * C_fail exceeds the human-in-the-loop alternative V - C_review, which rearranges to a required success rate that rises with C_fail.
The numbers going into this are estimates, but forcing the estimate exposes the real disagreements: most "should the agent do this alone" debates are actually disagreements about C_fail.

Practical corollaries.

Reversible actions tolerate far lower reliability than irreversible ones, so partition your tool surface by reversibility and gate only the irreversible subset.
Drafting an email needs maybe 70 percent quality to be useful because the human edits it; sending the email needs something like 99-plus percent because the failure is public.
The same agent can therefore be autonomous for reads and drafts while requiring confirmation for sends and deletes, and this asymmetric design is almost always better than a uniform autonomy level.

Reliability is per-task-class, not global.
An agent might be 97 percent reliable at data lookup, 85 percent at multi-system workflows, and 60 percent at open-ended analysis, and autonomy should be scoped per class, with a router or explicit task typing deciding which regime a request falls into.

Autonomy scoping is dynamic.
Confidence signals (verifier failures, tool errors, judge scores, the model's own calibrated uncertainty where available) should be able to demote a session from autonomous to supervised mid-flight, which is cheaper than either always supervising or never supervising.

## 6. The maturity ladder

Deploy along a ladder, promoting a capability one rung at a time when measurement supports it.
The ladder mirrors how autonomous-driving levels work, and for the same reason: trust is granted against evidence, per capability, not globally.

**Level 0 - Tool.**
The model produces artifacts on explicit request with no loop and no side effects; a human uses or discards the output.
Reliability bar: low, because the human is the executor.

**Level 1 - Copilot.**
The system proposes actions in context (suggested replies, code completions, drafted changes) and the human approves each one.
The product metric is acceptance rate, which doubles as your reliability measurement for the next rung.
Downside of staying here: approval fatigue caps the value, and users start rubber-stamping, which silently degrades the safety property you think you have.

**Level 2 - Supervised agent.**
The agent executes multi-step trajectories but checkpoints at defined boundaries: plan approval before execution, confirmation before irreversible actions, review before submission.
This is where most serious agent products sit as of early 2026, and it is the rung where you accumulate the trajectory data that justifies promotion.

**Level 3 - Bounded autonomy.**
The agent completes whole task classes without per-task review, inside hard guardrails: budget ceilings, tool allowlists, sandboxed side effects, and sampled after-the-fact review instead of before-the-fact approval.
Promotion to this rung should be per task class and backed by a written case: measured success rate over a defined sample, worst observed failure, and the containment story for that failure.

**Level 4 - Autonomous with escalation.**
The agent owns an ongoing responsibility (triage this queue, keep this dashboard green, maintain this dependency set), runs continuously, and escalates to humans on defined uncertainty or risk triggers.
The engineering center of gravity shifts from prompting to operations: durable execution, monitoring, incident response, and escalation quality become the product.

**Level 5 - Fully autonomous.**
No routine human oversight within the domain.
As of early 2026 this is appropriate only for domains where failures are cheap, contained, and automatically detectable, such as ephemeral sandboxed environments; treating it as the default goal is the most common strategic error in the field.

Two rules govern movement on the ladder.
Promotion requires evidence from the current rung, gathered by instrumentation you built before you needed it.
Demotion must be instant and cheap: a feature flag that drops a task class from level 3 to level 2 is your fastest incident mitigation, and if demotion requires a code deploy you have built the ladder wrong.

## 7. A production readiness checklist

Before granting an agent capability real users or real side effects, you should be able to answer yes to each of these.

- There is an eval suite built from realistic traffic that gates prompt, tool, and model changes.
- End-to-end success rate is measured, with a number, on the target task distribution, not on a demo set.
- Autonomous segments are short enough that the compounding math works at the measured per-step reliability.
- Every irreversible action is gated, idempotent, or sandboxed, and you can enumerate which of the three applies to each tool.
- Cost and latency per session have budgets enforced in code, with alerting on distribution shift.
- Operational, trajectory-health, quality, and safety SLOs exist and at least the first two alert automatically.
- There is a kill switch and a demotion flag reachable in under a minute by the on-call engineer.
- The model version is pinned, and there is a tested path to the fallback model.
- Someone is on call for the system, and they have a runbook for the top five failure modes.

The rest of this volume is the engineering behind each line of that checklist: latency (Chapter 02), cost (Chapter 03), reliability patterns (Chapter 04), deployment architecture (Chapter 05), capacity (Chapter 06), and operations (Chapter 07).

## Exercises

1. Compute the maximum autonomous trajectory length for a 90 percent end-to-end success target at per-step reliabilities of 0.95, 0.99, and 0.999, then recompute assuming a verifier catches 80 percent of step failures and retried steps succeed at the base rate. Derive the closed form for effective per-step reliability with one retry before computing.
2. Take an agent workflow you know (or design a plausible 25-step support-ticket resolution flow) and partition it into autonomous segments with verification checkpoints so that each segment has at least 95 percent expected completion at p = 0.98 per step. Justify each checkpoint placement in one sentence.
3. Write an SLO document for a customer-facing research agent: at least two operational SLOs, two trajectory-health SLOs, two quality SLOs with their measurement method and sampling rate, and one safety SLO with its incident threshold. State the error-budget policy for a quality regression.
4. For a calendar-management agent with tools for reading events, drafting invites, sending invites, and deleting events, assign each tool a maturity-ladder level and a required measured success rate, using the expected-value frame from section 5. Make your C_fail estimates explicit.
5. Pick a publicized agent or LLM product failure and map it to the failure taxonomy in section 3, identifying which checklist items from section 7 would have prevented or contained it.

## Godhood check

You have mastered this chapter when you can do the following without notes.
Derive the p^n compounding curve and explain the three mechanisms (recovery, decomposition, verification) that beat it, including the cost each one pays.
Design a four-layer SLO set for an agent product and explain why quality SLOs must be sampled and lagged rather than real-time.
Argue for a specific autonomy level for a specific tool using measured reliability and an explicit failure-cost estimate, and identify the rung on the maturity ladder where a given product should launch.
Explain why acceptance-rate data from a copilot deployment is the correct evidence for promoting to supervised autonomy, and why demotion paths must be faster than promotion paths.
