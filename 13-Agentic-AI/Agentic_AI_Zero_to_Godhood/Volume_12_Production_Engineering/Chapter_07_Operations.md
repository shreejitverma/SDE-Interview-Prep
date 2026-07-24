# Chapter 07 - Operations

## What you will master

- Monitoring and alerting for agent systems, where quality metrics stand beside uptime as first-class operational signals.
- Incident response when the failure is model behavior rather than infrastructure, and the mitigations unique to AI systems.
- Model version migrations: pinning, migration evals, deprecation timelines, and running upgrades as projects rather than surprises.
- Prompt change management: treating prompts as deployable artifacts with review, canary, and rollback.
- On-call design for AI systems and a postmortem culture that works when the root cause is probabilistic.

## 1. Operations when the failure is semantic

Classical operations answers "is it up, is it fast, is it erroring."
Agent operations must also answer "is it right, is it safe, is it still behaving the way it did yesterday," and those questions have properties that break standard operational habits.
Quality signals are sampled and lagged rather than instant (Chapter 01, section 4), regressions arrive without any deploy on your side (a provider-side change, a traffic shift, a new jailbreak circulating), failures are distributional rather than binary (success rate drifting from 92 to 85 percent, not a crash), and the blast radius of a misbehaving agent includes actions taken in the world, not just errors returned.
Everything in this chapter is the standard operational discipline (monitor, respond, change-manage, learn) re-derived under those properties.
The prerequisite is the observability and evaluation stack of Volume 10: full trajectory tracing (Volume 10, Chapter 06), an eval suite with calibrated graders and judges (Volume 10, Chapters 02, 03, and 05), and the production-evaluation loop of online signals, canaries, and drift monitoring (Volume 10, Chapter 07).
This chapter assumes those exist and covers how to operate on top of them; if they do not exist, everything below degrades into guessing, because you cannot page on a signal you do not measure or roll back on a verdict you cannot compute.
The other standing dependency is Volume 11: safety events are a distinct incident class with their own detection surface, containment moves, and disclosure obligations, and this chapter's job is the operational plumbing around the controls that volume specifies, not a substitute for them.

## 2. Monitoring and alerting beyond uptime

Structure the monitoring surface as four layers, matching the SLO layers of Chapter 01, each with its own alerting character.

Operational layer: availability, latency percentiles per session class, provider error and throttle rates, queue depth and age, sandbox pool health, spend rate.
These alert in real time with classical burn-rate policies, and they are your fastest proxies for everything else, because most quality incidents begin life as an operational anomaly (a retry spike, a latency shift, a truncation-rate bump).

Trajectory-health layer: sessions hitting step or token ceilings, tool-call failure rates by tool, loop-detection triggers, fallback and degradation rung activations, context-overflow compaction rates, refusal rates.
These are computable from traces with no labeling, alert within minutes, and are the highest-value agent-specific addition to a standard dashboard; a refusal-rate step-change is very often the first visible sign of a provider-side behavior change or a prompt regression.

Quality layer: online judge scores on sampled sessions, task-success rates by task class, escalation correctness, artifact-quality checks (does the code compile, do the citations resolve).
Alert on drift with statistical care: scores are noisy, samples are small, and naive thresholds page constantly or never; the workable pattern is rolling-window comparison against a baseline with a significance test or a control chart, tuned to detect the regressions you care about (several points of success rate) within hours, not minutes.
Judge drift is a real failure mode of the monitoring itself, so the judge's agreement with periodic human labels is a meta-metric with its own alert.

Safety layer: guardrail trigger rates (the input, output, and action guardrails of Volume 11, Chapter 04), confirmed harmful-action counts, injection-canary results (seeded benign markers in untrusted content whose appearance in agent output proves an injection landed, per Volume 11, Chapter 02), approval-gate rejection rates, and anomalous tool-usage patterns (a tool suddenly called at ten times baseline, sessions touching unusually many tenants' data).
These alert at low thresholds and page immediately, because each event is potentially an incident, and the tool-usage anomaly detectors deserve emphasis: they are how you notice an agent doing something new and unwanted before a user reports it.
Note the direction of the dependency: Volume 11 decides what the controls are, and this layer is the operational instrumentation that tells you when a control fired, when it stopped firing, and when something got past it.

Two cross-cutting disciplines make the layers work.
Segment everything by model version, prompt version, task class, and tenant tier, because agent regressions are usually localized (one task class, one prompt, one model) and aggregate metrics average them into invisibility; the single most useful operational chart is quality-by-prompt-version-by-model-version over time.
And alert on distribution change, not only on threshold breach: output-length distributions, tool-call-mix distributions, and session-length distributions shifting are early warnings of behavior change even while every threshold still passes, and cheap drift detectors on those distributions catch what fixed thresholds cannot.

## 3. Incident response for model misbehavior

AI incidents come in recognizable classes, and the response machinery should be built per class: quality regressions (success rate drops), behavior changes (the agent starts doing something different, not necessarily failing), safety events (harmful action, injection success, data exposure), cost or capacity runaways (Chapter 03 and 06 territory), and provider incidents (Chapter 04 territory).
What follows is the response playbook where the model, not the infrastructure, is misbehaving.

Detection to triage.
The triage question unique to AI incidents is "what changed," and the candidate list is short and checkable in order: our prompt or code deploy (check the deploy log against the regression's start time), our traffic (new task-class mix, one big tenant onboarding, an attack), the model version (pinned versions rule this out; alias drift does not, section 4), or the provider's serving stack (same version, changed behavior; rare but real, and diagnosable by re-running a frozen eval set against the same pinned version and comparing to its recorded baseline).
That frozen-eval replay is the AI equivalent of a health-check endpoint, and having it scripted and runnable in minutes is the single most valuable incident tool you can pre-build; it cleanly separates "the model changed" from "our inputs changed."

Mitigation, in the order an on-call should reach for it.
Demote autonomy first: the feature flag that drops an agent capability down the maturity ladder (Chapter 01, section 6) is the AI-specific kill switch, and it degrades service instead of removing it, which is almost always the right first move.
Roll back the newest change: prompt version, routing config, or model selection, all of which should be config-plane rollbacks taking effect in minutes without a code deploy (section 5).
Fall back model or provider along the Chapter 04 ladder if the frozen-eval replay implicates the model or the provider.
Constrain the blast radius while diagnosing: tighten tool allowlists, lower budget ceilings, force confirmation gates on side-effecting tools, or pause the affected task class into the deferred queue.
And for safety events, add containment and accounting: suspend the capability outright using the kill switches and blast-radius controls of Volume 11, Chapter 06, then use the trajectory store to enumerate every session that touched the failure window, because "which users were affected and what actions were taken on their behalf" is answerable from traces and unanswerable without them.
Safety incidents diverge from quality incidents after that point and should hand off to the Volume 11 playbook rather than staying in this one: an injection success is an attack with a possible campaign behind it and needs adversary-side analysis (Volume 11, Chapters 01 and 02), a data-exposure event carries notification and regulatory obligations (Volume 11, Chapter 07), and both need evidence preserved before mitigation churns the state you would have wanted to inspect.
The operational rule that follows: the trigger for reclassifying a quality incident as a safety incident should be written down in advance (confirmed data crossing a tenant boundary, a completed harmful action, an injection canary firing in production), because on-call judgement under time pressure is the wrong place to invent that line.

Communication has one AI-specific wrinkle: distributional failures need distributional honesty.
"Response quality is degraded for research tasks; we have reduced autonomy while we investigate" is accurate and maintains trust; pretending a quality regression is either total outage or non-event does not.

## 4. Model version migrations

Models are dependencies with vendor-controlled lifecycles: providers ship new versions frequently and deprecate old ones on announced timelines (as of early 2026, deprecation windows at major providers have typically ranged from several months to a year or so after a successor ships, with dated model ids distinguishing snapshots).
Treating this lifecycle passively is how products wake up broken; the active posture has three parts.

Pin, always.
Production traffic runs against dated snapshot ids, never floating aliases that silently move to the newest version, because an unannounced behavior change under a floating alias is an incident with no deploy log entry on your side; the cost of pinning is that upgrades become deliberate work, which is precisely the point.

Run migrations as evaluation projects.
The migration workflow that works: run the full eval suite (Volume 10, Chapter 03 for agent-eval construction, Chapter 05 for judge calibration, since a migration diff is only as trustworthy as the grader producing it) plus a replay of recent production traffic samples against the candidate version; diff not just aggregate scores but per-task-class scores, tool-calling behavior, output-length and refusal distributions, and cost and latency (new versions change token economics and speed, not just quality); expect prompt rework, because prompts overfit to model quirks and a version change moves the quirks; canary in production with the segmentation of section 2 watching quality-by-model-version; then ramp, keeping instant rollback to the prior pinned version until the old id's deprecation date.
Budget real calendar time for this: for a serious agent product with tuned prompts, a major-version migration is weeks of work, and it lands on whatever schedule the provider's deprecation calendar dictates, so maintain a dependency calendar of announced deprecations with migration projects scheduled well before each date rather than after the reminder email.

Handle the capability temptation separately.
New versions arrive with better capability, and product pressure will push to upgrade fast; keep the migration eval gate anyway, because "better on average" and "regression-free on your task distribution" are different claims, and the second is the one your users experience.
The mature shape is a standing rotation: an always-current candidate-evaluation harness so that evaluating a new model is a routine run, not a scramble, which also keeps the Chapter 03 routing and Chapter 06 sourcing decisions continuously refreshed.

## 5. Prompt change management

Prompts are behavior-defining source code deployed to production, and the operational maturity of an agent team is visible in whether prompts are managed like code or like tribal knowledge in a dashboard textbox.
The full discipline:

Version control: prompts, tool definitions, few-shot examples, and routing configs live in the repository, reviewed in pull requests, with the same ownership and history as code; a reviewer can see the diff, and the deploy log answers "what changed at 14:03."
Evaluation gates: every prompt change runs the relevant eval suites before merge, using the tiered CI gating of Volume 10, Chapter 07 so that fast cheap suites run on every commit and the expensive full suite runs before promotion (the Chapter 01 error-budget policy makes a quality regression a merge-blocker), plus the cost check from Chapter 03, because prompt changes move token counts and cache behavior (a change that touches the stable prefix invalidates caches fleet-wide for a day, which is a cost and capacity event someone should approve knowingly).
Deployment as config: prompt versions deploy through a config plane with canary percentages and instant rollback, decoupled from code deploys, because the section 3 playbook depends on prompt rollback taking minutes; the trade-off of the decoupling is version-skew management (a prompt version must declare which code and tool versions it is compatible with, or the config plane will eventually deploy a prompt referencing a tool that is not there).
Segmented observation: the quality-by-prompt-version charts of section 2 close the loop, catching what the eval suite missed, and every canary needs a defined observation window and promotion criterion, because "we canaried it" without a decision rule is theater.
Change hygiene: one behavioral change per prompt release where feasible, because a release bundling five edits that moves a metric leaves you diffing prose under incident pressure; and a changelog note stating intent, because six months later nobody remembers why line 40 forbids the agent from apologizing twice.

Resist the anti-pattern of hotfixing prompts in production consoles.
Every provider and framework dashboard offers the temptation; the console edit skips review, evals, canary, and the deploy log, and it is how a well-meaning fix at 6 p.m. becomes a quality incident that the section 3 triage cannot attribute, because "what changed" has no entry.
Emergency prompt changes should exist as an expedited path through the same pipeline (skip the slow evals, keep the version, the log, and the rollback), never as a bypass of it.

## 6. On-call for AI systems

The pager model needs adjustment because the failure modes span two competencies: infrastructure failures (queues, workers, providers, sandboxes) that any strong backend engineer can drive, and behavior failures (quality regressions, prompt interactions, model changes) that need someone fluent in the prompts, evals, and model quirks of the product.
Small teams merge these into one rotation and accept the training cost; larger ones run a platform rotation and an AI-behavior rotation with a clear routing rule (operational-layer alerts page platform, quality- and safety-layer alerts page behavior, and either can pull the other).
Whatever the structure, three properties are non-negotiable.

The on-call can act without understanding the root cause: autonomy demotion flags, prompt and config rollback, model fallback, task-class pausing, and tool-gate tightening are all reachable in minutes from a runbook, because behavioral root-causing takes hours to days and mitigation cannot wait for it.
The runbooks are behavior-aware: alongside classical entries, the top entries are "success rate dropped for task class X" and "the agent is doing something weird," each with the section 3 triage checklist, the frozen-eval replay command, the segmentation dashboards to consult, and the mitigation ladder in order.
The alert budget is defended: quality alerts tuned too tight will page nightly on noise and train the rotation to ignore exactly the alerts that matter, so quality alerting follows the statistical discipline of section 2, low-urgency drift goes to a daily review queue rather than the pager, and every page is either actionable or gets its threshold fixed in the weekly review.

One more rhythm earns its keep for agent systems: a daily quality review, fifteen minutes, where the behavior owner scans the drift dashboards, the worst-scored sampled sessions, and the DLQ, because a human regularly reading real trajectories catches degradation modes that no metric was written for yet, and it keeps the team's model of "what the agent actually does" current, which is the raw material of every runbook above.

## 7. Postmortem culture for agent failures

Blameless postmortem practice transfers directly, with three adaptations for the probabilistic setting.

Root cause becomes root distribution.
"Why did this session fail" is often unanswerable at the single-sample level (the model sampled a bad trajectory) and the productive question is "why was the failure rate what it was, why did our layers not catch this instance, and what moves the rate or the catch probability."
A good agent postmortem therefore quantifies: the failure rate before and during the incident, the detection lag, the fraction caught by each defense layer, and the same numbers after the fix; "we fixed it" for a distributional failure means the measured rate moved, demonstrated on the eval suite and then in production telemetry, not that one bad transcript now passes.

Action items target layers, not just the trigger.
The compounding structure of agent systems (Chapter 01) means most incidents pass through several layers that each could have stopped them: the prompt allowed it, the validator missed it, the autonomy level did not require confirmation, the monitoring detected it late.
The postmortem walks the whole chain and files actions at multiple layers, with special attention to detection lag, because for distributional failures the dominant cost term is usually how long the regression ran unnoticed, and "add a drift alert on the distribution that shifted" is the most commonly warranted action item in the genre.
When the trigger was the model itself, the honest action item is often not "make the model not do that" (you do not control that layer) but "make our system safe against a model that occasionally does that," which is a systems fix, and postmortems that end with "we adjusted the prompt and it seems fine" have concluded with a hope, not a fix.

Feed the eval suite as a matter of ritual.
Every incident contributes its failing cases to the regression suite, exactly as classical postmortems add tests, which is the incident-driven arm of the data flywheel in Volume 10, Chapter 07; over time the suite becomes the accumulated scar tissue of everything that has bitten you, which is what makes the migration evals of section 4 and the gates of section 5 actually protective rather than generic.
For safety incidents the same ritual applies against the adversarial suite instead: a successful injection becomes a permanent red-team case (Volume 11, Chapter 02), because the attack that worked once is the attack a regression will silently re-enable.
Close the loop with a periodic review of postmortem trends, because agent incidents cluster (the same tool contract, the same task class, the same injection surface recurring) and the clusters, not the individual incidents, are what justify the larger architectural investments of Chapters 04 and 05.

## 8. The operational maturity checklist

A compact self-assessment; each line is covered by a section above or a prior chapter.

- Dashboards exist for all four monitoring layers, segmented by model version, prompt version, and task class, with distribution-drift alerts, not just thresholds.
- A frozen-eval replay against pinned model versions is scripted and runs in minutes.
- Autonomy demotion, prompt rollback, model fallback, and task-class pause are config-plane actions in the on-call runbook, each tested within the last quarter.
- The reclassification trigger from quality incident to safety incident is written down in advance, and the safety path preserves evidence and hands off to the Volume 11 playbook rather than being improvised.
- Production models are pinned to dated ids; a deprecation calendar exists; the last migration ran through the full eval-canary-ramp pipeline.
- Prompts live in version control behind eval and cost gates, deploy through canary with instant rollback, and the console-hotfix path does not exist.
- The pager routes operational and behavioral alerts to the right competency, quality alerts survive statistical scrutiny, and a daily quality review actually happens.
- Postmortems quantify rates and detection lag, file layered action items, and feed the regression suite; someone reviews the clusters quarterly.

## Exercises

1. Design the alert catalog for a production agent product: for each of the four layers, three alerts with signal definition, threshold or statistical test, urgency (page versus review-queue), and the runbook action it points to. Justify the statistical design of one quality alert in detail, including its expected detection lag for a five-point success-rate drop at your chosen sample rate.
2. Write the incident runbook entry for "task success rate for workflow X dropped from 91 percent to 78 percent over six hours, no deploy in the window": the triage checklist in order, the commands or dashboards for each step, the mitigation ladder with expected effect of each rung, and the criteria for escalating from quality incident to safety incident.
3. Plan a major model version migration end to end for a product with twelve prompts and four task classes: the eval and replay matrix, the behavioral diffs you will inspect beyond aggregate scores, the canary design with promotion criteria, the rollback plan, and a realistic calendar against a provider deprecation date you posit. State what you would do if the candidate wins on three task classes and regresses on the fourth.
4. Specify the prompt change-management pipeline as if writing the engineering design doc: repository layout, review requirements, eval and cost gates, the config-plane deployment mechanism with canary and rollback, the version-compatibility declaration between prompts and tools, and the expedited emergency path with exactly which safeguards it keeps.
5. Write the postmortem for a fictional but realistic incident: an agent with send-capability emailed incorrect information to roughly 2 percent of sessions for three days after a prompt change passed evals. Include the timeline, the quantified rates and detection lag, the layer-by-layer analysis of why each defense missed it, at least five action items across different layers with owners, and the eval-suite additions.

## Godhood check

You have mastered this chapter when you can design the four-layer monitoring surface from memory, explain why segmentation by model and prompt version is the highest-value operational chart, and defend the statistical design of a quality alert against both false-page and missed-regression critiques.
You can run the "what changed" triage for a behavioral incident in order, explain what the frozen-eval replay isolates, and state the mitigation ladder an on-call reaches for before root cause is known.
You can describe the full pinning-eval-canary-ramp migration workflow, why floating aliases are forbidden in production, and why prompt changes deserve the entire deployment discipline of code.
You can write an agent postmortem that quantifies distributions rather than narrating a single transcript, files action items at multiple defense layers, and feeds the regression suite, and you can articulate why "we fixed the prompt" is a hope rather than a fix.
