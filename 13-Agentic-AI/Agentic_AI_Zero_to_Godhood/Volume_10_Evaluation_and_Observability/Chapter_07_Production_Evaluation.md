# Chapter 07 - Production Evaluation

## What you will master

- Online evaluation as a discipline: what production traffic can tell you that offline suites cannot, and its structural limits.
- Implicit feedback signals: the full catalog from thumbs ratings to edits, retries, abandonment, and escalation, with the biases of each.
- A/B testing agents: why standard experimentation applies, and the agent-specific complications of high variance, session-level units, and non-stationary behavior.
- Regression gates in CI: wiring the offline suite into the merge and deploy path with tiered gates and governance.
- Canary deployments for prompt and model changes: why every prompt edit is a deploy, and how progressive rollout plus automated rollback contains blast radius.
- Drift monitoring: input drift, behavior drift, and dependency drift, and the proxy metrics that detect them before users do.
- The data flywheel: the pipeline that turns production failures into eval cases, which is the compounding asset of the whole volume.

## 1. What online evaluation is for

Offline evaluation (Chapters 1-5) answers "does the system pass our frozen definition of good"; online evaluation answers "is the system actually serving the live distribution well right now".
The two diverge for every reason this volume has catalogued: the offline dataset is a lagging sample of a moving distribution, some qualities exist only in interaction, and the world (users, APIs, upstream models) changes under a frozen system.

The structural limits of online evaluation, stated up front so the chapter is not read as a replacement for offline work:

- No counterfactual per run: you observe what the shipped agent did, not what a better agent would have done, so absolute quality is only estimable through experiments or judged sampling.
- Weak and biased labels: users label a small, self-selected slice of interactions, and the labels conflate agent quality with product friction and user mood.
- Post-hoc by construction: online signals arrive after users experienced the behavior; online evaluation bounds damage and detects it fast, but only offline gates prevent it.
- Ethically and commercially constrained: experiments expose real users to the worse arm, which caps how much exploration production can absorb.

The mature architecture is therefore a loop with distinct roles: offline evals gate what ships (Sections 4-5), online signals measure what shipped (Sections 2-3, 6), and the flywheel (Section 7) pumps online reality back into offline gates.

## 2. Implicit feedback signals

Explicit feedback (thumbs, stars, surveys) is the obvious signal and the weakest: response rates are low single-digit percentages in most products, responders are self-selected toward the extremes, and the same interaction draws different ratings depending on expectations the product set.
Use it, but as one noisy input, never as the metric.

Implicit signals are behaviors users emit while simply using the product, and they carry more information per unit of traffic precisely because everyone emits them:

- Edits: the user takes the agent's output and modifies it before use; edit distance and edit type (fixing facts versus adjusting tone) grade the output more honestly than any rating, and near-zero edits on adopted output is a strong success signal.
- Retries and rephrases: the user immediately re-asks the same thing differently; a reliable failure signal, detectable by intra-session semantic similarity between consecutive requests.
- Abandonment: the user quits mid-task; strong negative signal, confounded by interruptions, so it reads best as a rate compared across arms rather than per session.
- Escalation: the user asks for a human, or the agent hands off; in support products this is simultaneously a quality metric and a cost metric, and its complement (containment) is the headline number, with the caveat that containment can be gamed by making escalation hard, so pair it with downstream satisfaction.
- Acceptance in tool-shaped products: coding agents have the cleanest signals in the industry (suggestion acceptance, diff retention after a day, revert rates, CI pass rate on agent-authored PRs), and retention-after-time beats acceptance-at-a-glance because it measures whether the work survived scrutiny.
- Follow-through: did the user act on the answer (clicked the cited link, sent the drafted email, executed the plan); the strongest satisfaction proxy where the product can observe it.
- Conversation-shape signals: turns to resolution, corrective phrasings ("no, I meant"), sentiment trajectory across the session; individually weak, useful in aggregate, and extractable by an LLM classifier over transcripts, which is judge machinery (Chapter 5) applied online and inherits all of its calibration obligations.

Two disciplines make signals usable.
First, validate each proxy against ground truth on a labeled sample before trusting it: measure how well edit distance or retry rate actually predicts human-judged failure in your product, because proxy validity is product-specific and shifts with UI changes.
Second, attach every signal to the trace (Chapter 6, Layer 3), because a signal that cannot be joined to what the agent actually did supports dashboards but not diagnosis.
And remember Goodhart: any single proxy elevated to a target will be gamed by your own optimization loop; watch a basket, and rotate scrutiny toward whichever number looks implausibly good.

## 3. A/B testing agents

When you can randomize, randomize: an A/B test is the only instrument in this chapter that yields causal answers about which variant is better on the live distribution.
Standard experimentation applies (random assignment, pre-registered metrics, power analysis before launch, no peeking without sequential-testing corrections), and this section covers only what agents change.

- The randomization unit is the user or session, not the request: agent quality expresses itself across multi-turn interactions, users compare experiences across their own sessions, and per-request randomization within a conversation both breaks coherence and contaminates measurement; user-level assignment with session-level metrics is the default.
- Variance is brutal: agent outcome metrics (task success, containment, edit rates) are high-variance and often heavy-tailed in cost and latency, so detectable effects at product-realistic traffic are larger than intuition from web experimentation suggests; run the power analysis honestly, and use variance-reduction machinery (covariate adjustment with pre-exposure user metrics, stratification by task category) to claw back sensitivity.
- Small-N reality: many agent products serve thousands of users, not millions, and cannot power a 2-point effect; the honest responses are longer experiments, bigger planned effects, offline-heavy decision-making with online tests reserved for the few changes big enough to measure, and interleaving designs where applicable (present both variants' outputs and observe choice), which pay the exploration cost within-user at high sensitivity but fit only selection-shaped surfaces.
- Non-stationary learning effects: users adapt to a new agent over days (novelty inflates engagement, then habits settle), so pre-registered burn-in periods and time-sliced readouts distinguish transient from durable effects.
- Guardrail metrics are mandatory: every agent experiment carries cost per session, latency percentiles, safety-event rate, and escalation rate as guardrails with pre-committed stop conditions, because an arm that wins engagement while doubling token spend or safety incidents is a loss wearing a win's clothes.
- Judge-scored online metrics: sampling live sessions from both arms and scoring them with a calibrated judge gives a quality metric denser than user feedback; it inherits every Chapter 5 obligation, and the judge must be blind to arm assignment, since a judge that can infer the variant from stylistic tells reintroduces bias at the metric layer.

## 4. Regression gates in CI

The offline suite earns its keep by running automatically at the moments of change; a suite that must be remembered is a suite that gets skipped exactly when the risky change lands.

The tiered structure that fits agent-suite economics, where a full run costs real money and minutes-to-hours:

- Tier 0, per commit: deterministic and cheap; prompt-template linting and rendering tests, tool-schema validation, grader unit tests, harness smoke test with a stub model; seconds, no tokens.
- Tier 1, per pull request: the curated regression subset, a few dozen tasks with mostly programmatic graders and a small trial count, targeting minutes and single-digit dollars; paired against the base branch on identical tasks (Chapter 3's discordant-task comparison), because pairing is what makes a small subset statistically usable.
- Tier 2, pre-deploy and nightly: the full regression suite at production trial counts, plus judge-graded criteria, plus the capability suite for tracking; the nightly run also catches drift that arrives without any commit (Section 6), which per-PR gating structurally cannot.

Governance rules that keep gates meaningful, restating Chapter 1's social failure modes as CI policy:

- The gate compares against a pinned baseline with a pre-agreed tolerance derived from measured suite variance, not from optimism; a gate whose threshold sits inside the noise band flags randomly and trains engineers to ignore it.
- Threshold and grader changes require review outside the shipping team, and land in separate commits from the change they would unblock.
- Flaky tasks are quarantined loudly and fixed on an SLA, never silently retried into passing.
- The gate's verdict is attached to the PR alongside the per-task diff (which tasks flipped, with trace links), because a red gate that names the three flipped tasks gets fixed, while a red gate that says 78 percent gets argued with.

One structural honesty note: CI gates evaluate the agent code and prompts at merge time against pinned models; if your model provider updates a pinned-alias snapshot underneath you, the merge-time gate proves nothing about tonight, which is why Tier 2 runs on a clock and not only on commits.

## 5. Canary deployments for prompt and model changes

The unit of deployment for agent products is not just code: a prompt edit, a tool-description change, a temperature adjustment, or a model-version bump each changes production behavior as much as a code deploy, and each therefore deserves deploy discipline: versioning, review, progressive rollout, and rollback.
Teams that let prompts hot-patch to 100 percent of traffic outside the deploy path relearn this after their first quiet regression.

The canary pattern transposed to agents:

1. Ship the change to a small traffic slice (commonly low single-digit percent, user-sticky so sessions stay coherent) while the incumbent serves the rest.
2. Compare canary against baseline on the online basket: implicit signals from Section 2, guardrails from Section 3, sampled judge scores, cost and latency percentiles, error and safety rates.
3. Progress through widening stages on explicit criteria, with dwell times long enough to accumulate signal at each stage; agent metrics are slow relative to error-rate metrics, so agent canaries dwell hours-to-days, not minutes.
4. Roll back automatically on guardrail breach, and treat rollback as a routine mechanism (the incumbent prompt and model are retained and instantly re-routable), not an incident.

Agent-specific complications worth naming:

- Model migrations are the heaviest canary case: a new model shifts behavior across the entire surface at once, so migrations run the full offline suite first (this is the moment the suite pays for itself, per Chapter 1), then canary longer and wider than any prompt change, with per-category readouts because migrations routinely improve the mean while regressing a category.
- Judge-scored canary comparison needs the same blindness discipline as Section 3, and model migrations tempt self-preference bias if the judge shares a family with either side.
- Sample-size limits bind again: a 2 percent canary of a small product may see too little traffic for anything but coarse guardrails, in which case widen the early stages and lean harder on the offline gate; the canary then defends mainly against catastrophic surprise rather than subtle regression, which is still worth having.

## 6. Drift monitoring

Drift is degradation without a deploy: the system is unchanged and its performance moves anyway.
Three distinct sources, each with its own detector, because conflating them wastes diagnosis time:

- Input drift: the task distribution shifts; new user cohorts, seasonal topics, a viral use case your suite never sampled; detect via distribution monitoring over request features (topic mix from a lightweight classifier, request length, language, tool-demand mix) with alerts on divergence from a trailing baseline.
- Dependency drift: the world under the agent changes; APIs alter response schemas, websites restructure, knowledge-base content goes stale, and the provider updates the model behind an alias; detect via tool-error and schema-validation rates per integration, plus a fixed probe set (a small battery of canonical requests replayed daily against production infrastructure) whose behavior change isolates dependency movement from traffic movement, since the probe inputs are constant by construction.
- Behavior drift: the agent's outputs shift for any upstream reason; detect via time series on cheap proxies that need no labels: output length, refusal rate, tool-calls per task, cost per session, escalation rate, retry rate, plus sampled judge scores as the denser, costlier layer.

Operational notes that separate working drift monitoring from dashboard theater:

- Alert on trends and level shifts, not single-day wiggles, and tune windows to each metric's natural variance; drift detection is a statistics problem, and untuned alerts train the team to ignore the channel.
- Every drift alert should route into the Section 7 pipeline: the alert's exemplar traces are the raw material for new eval cases, which is how the offline suite tracks the moving distribution instead of freezing at launch.
- The pinned-model caveat cuts both ways: pinning snapshot versions eliminates provider-side behavior drift at the cost of a periodic forced migration (Section 5); pinning is the right default, and the probe set is your early warning either way.

## 7. The data flywheel

Everything in this volume converges here: the pipeline that converts production failures into eval cases, so that every failure the system ever exhibits makes the system permanently harder to break in that way.
Teams with a running flywheel compound; teams without one fix the same class of bug quarterly under different ticket numbers.

The pipeline, stage by stage:

1. Capture: failures arrive from every channel this chapter built (negative implicit signals, low judge scores, canary breaches, drift alerts, escalations, support tickets, on-call reports); each arrives attached to its trace, which Chapter 6 made possible.
2. Triage: a human, aided by clustering over failure embeddings and the Chapter 6 failure taxonomy, deduplicates and ranks by frequency times severity; not every failure becomes a case, and triage capacity is the flywheel's rate limiter, so staff it explicitly on a rotation rather than hoping it happens.
3. Distill: the trace becomes a task: initial state extracted and re-seeded into the mock environment, instruction preserved (with the user's ambiguity intact, per Chapter 3's specification rules), content redacted and, where required, consented per Chapter 6's privacy regime, and success criteria written for what should have happened, reviewed like code.
4. Verify the case: confirm the current system actually fails the new case (reproducing the failure offline validates the distillation) and that the intended fix passes it; a case that never failed anything measures nothing.
5. File and gate: the case enters the rolling recent-failures suite immediately and graduates into the golden regression set per Chapter 2's lifecycle; from that moment, CI (Section 4) enforces that this failure class stays fixed forever.
6. Feed the other assets: the same distilled traces refresh judge calibration sets, unit-eval replay contexts, and user-simulator personas, because production is the only source that keeps every offline instrument honest.

The flywheel's health is itself measurable, and mature teams track it: time from failure observed to case merged, fraction of incidents that produced cases, and the ratio of production failures that were repeats of known classes versus novel (a falling novelty ratio means the flywheel is working; a rising repeat ratio means cases exist but fixes do not, which is an engineering-priority problem the flywheel has now made visible and undeniable).

This is the volume's closing argument in operational form.
Chapter 1 claimed evals are the bottleneck and the highest-leverage asset; the flywheel is the mechanism that makes the asset appreciate: observability captures reality, triage selects what matters, distillation freezes it into executable product spec, and CI compounds it into a ratchet.
A model upgrade, a framework rewrite, even a vendor switch can discard almost every other artifact the team built; the eval suite and the flywheel that grows it survive all of them, because they encode the one thing that does not rotate with the stack: what your product means by good.

## Exercises

1. For a product you know, catalog every implicit signal it could emit today with zero UI changes, using the Section 2 list; for the two strongest, design the validation study (sample size, labeling protocol, target correlation) that would qualify them as trusted proxies.
2. Run the power analysis for an A/B test on a support agent with 8,000 weekly active users, baseline containment of 60 percent, and a hoped-for 3-point lift; report the required duration at user-level randomization, then recompute with a pre-exposure covariate that explains 30 percent of variance, and state whether the test is worth running.
3. Design the three-tier CI gate for the Chapter 3 harness: name the exact checks in Tier 0, select the Tier 1 subset and trial count under a five-dollar per-PR budget, and write the governance paragraph covering thresholds, flake quarantine, and who can change graders.
4. Write the canary plan for migrating a production agent to a new model snapshot: offline prerequisites, stage percentages and dwell times, the full guardrail basket with breach thresholds, per-category readouts, and the rollback mechanics; one page.
5. Build the probe-set design for dependency drift: choose 15 canonical requests for an agent with four tool integrations, define expected-behavior envelopes per probe, and show how a schema change in one API surfaces in the probe dashboard before it surfaces in user metrics.
6. Take three real or realistic production failures and run them through the full flywheel: triage ranking, distilled task with seeded state and reviewed success criteria, verification that the case fails before the fix and passes after, and the suite each case lands in; then compute your time-from-failure-to-merged-case and propose the process change that would halve it.

## Godhood check

You have internalized this chapter when you can do the following without reference.

- State the four structural limits of online evaluation and the offline-gates, online-measures, flywheel-connects architecture they force.
- Recite the implicit-signal catalog with the bias of each, explain why edits and retention-after-time beat ratings, and state the proxy-validation discipline that must precede trusting any of them.
- Explain the agent-specific A/B complications: session-level randomization, variance and small-N reality, learning effects, mandatory guardrails, and blind judge scoring.
- Design the three-tier CI gate from memory, including paired comparison at Tier 1, nightly Tier 2 as the drift catcher, and the governance rules that keep thresholds meaningful.
- Argue why every prompt edit is a deploy, and walk the canary pattern with its agent-specific dwell-time and model-migration complications.
- Distinguish input, dependency, and behavior drift, name the detector for each, and explain what a fixed probe set isolates that traffic metrics cannot.
- Draw the six-stage flywheel, identify triage as its rate limiter, name its three health metrics, and make the closing argument: the eval suite and its flywheel are the assets that survive every stack rotation.
