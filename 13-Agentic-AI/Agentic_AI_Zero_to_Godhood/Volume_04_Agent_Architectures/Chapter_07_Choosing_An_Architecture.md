# Chapter 07 - Choosing an Architecture

## What you will master

- A decision framework that turns the patterns of Chapters 01-06 into a repeatable selection procedure rather than taste.
- The three governing axes: task complexity, verifiability, and risk tolerance, and how their combinations map to architecture families.
- Cost ceilings and latency budgets as hard constraints that prune the design space before quality arguments begin.
- Four worked case studies (customer support bot, deep research, coding agent, data pipeline copilot), each carried from requirements through the matrix to a defended architecture.
- The revisit triggers: how to notice that a past architecture decision has been invalidated by model progress or workload drift.

## 1. From patterns to decisions

Chapters 01-06 gave you a vocabulary: the workflow ladder, the agent loop, planning modes, reflection loops, graph runtimes, and the harness that carries them all.
This chapter gives you the procedure for choosing among them, because in practice architectures are chosen badly for predictable reasons: resume-driven complexity, framework defaults mistaken for decisions, and the demo-to-production trap where the architecture that impressed in a demo is the one that pages you at scale.

The procedure, in outline, is: characterize the task on three axes, apply the cost and latency constraints as filters, select the simplest architecture family consistent with what survives, then choose the specific mechanisms (planning, reflection, durability) that the task's failure modes demand.
Every step is elaborated below, and the case studies run the full procedure end to end.

## 2. The three axes

### 2.1 Task complexity

Complexity here means path unpredictability, not difficulty: how much does the sequence of steps vary across inputs, and can it be enumerated at design time?

- **Low**: every input follows essentially one path; a fixed decomposition covers the traffic; classification, extraction, templated generation, single-lookup QA.
- **Medium**: inputs cluster into a handful of paths, or the path is fixed but some steps have input-dependent internals; multi-category support, document processing with a few formats, retrieval with query-type variation.
- **High**: the step count and identity are genuinely input-dependent and unenumerable; multi-file code changes, open-ended research, debugging, computer use.

Complexity sets the floor on the ladder: low complexity is served by single calls and chains, medium by routing and orchestrator-workers, high by agent loops.
Overshooting the floor buys nothing and costs everything downstream, which was Chapter 01's doctrine; undershooting it produces the 15-branch workflow that grows a new branch per incident, which is the equally real opposite failure.

### 2.2 Verifiability

Verifiability means: how cheaply and soundly can an output be checked without a human?

- **Mechanically verifiable**: tests pass, code compiles, schema validates, the reservation exists; a sound verifier is available (Chapter 03's LLM-Modulo condition).
- **Judgeable**: no sound verifier, but an LLM judge with a rubric correlates acceptably with human judgment; summaries, drafts, research syntheses.
- **Opaque**: neither; quality is only visible to a domain expert or only over time; strategic advice, nuanced policy answers.

Verifiability is the most decision-relevant axis and the most commonly ignored.
It determines whether reflection loops will work at all (Chapter 04: external signal or no gain), whether an agent can safely run long autonomous stretches (only if the environment tells it when it is wrong), and whether best-of-n or search patterns pay (only with a ranker worth trusting).
The design heuristic with the highest yield in this entire volume: before choosing an architecture, spend effort making the task more verifiable, because every point of verifiability bought (a test harness, a validator, a simulator, a rubric) expands the set of architectures that work and shrinks the human review bill.

### 2.3 Risk tolerance

Risk means the cost of an undetected bad output times the probability it escapes, weighted by reversibility.

- **Low stakes / reversible**: a bad draft, a wrong internal summary; the user catches and regenerates; errors cost seconds.
- **Medium stakes**: wrong information to a customer, a bad but reviewable code change; errors cost trust or rework but pass through a catchable stage.
- **High stakes / irreversible**: money moves, emails send, data deletes, production changes apply; errors cost real damage and cannot be recalled.

Risk does not select the architecture family; it selects the gates bolted onto it: where humans approve, which tools are mutation-gated, what runs in sandboxes, what requires plan review (Chapter 03), and what is simply not given to the agent at all.
The critical interaction with the other axes: high risk plus opaque verifiability is the forbidden quadrant for autonomy; no architecture in this volume makes that combination safe, and the honest designs either add verifiability, add a mandatory human gate, or decline to automate the final step.

## 3. The matrix

Complexity times verifiability, with risk choosing the gates within each cell:

| | Mechanically verifiable | Judgeable | Opaque |
|---|---|---|---|
| **Low complexity** | Single call or short chain with code gates; verify every output; automate fully at low risk | Single call plus judge-sampled QA; human spot checks scale with risk | Single call, human reviews all output; automation limited to drafting |
| **Medium complexity** | Routing or orchestrator-workers with verifiers per branch; evaluator-optimizer where the verifier is the evaluator | Routing plus judge; escalation route mandatory; human review on judge-flagged and sampled outputs | Workflow drafts, human owns every decision; the system is an assistant, not an agent |
| **High complexity** | Agent loop with environment feedback; long autonomy is safe in proportion to verifier soundness; plan mode plus gates by risk | Agent loop with judge checkpoints and tight budgets; human reviews the artifact, not every step | Do not build an autonomous agent here; decompose the task until parts land in better cells, or keep a human in the loop throughout |

Reading the matrix correctly: it prescribes families and gates, not implementations; within any cell, Chapter 01's ladder still says to take the simplest member that meets the bar, and Chapters 03-06 supply the mechanisms (planning, reflection, durability, harness structure) that the cell's failure modes call for.

## 4. Cost ceilings and latency budgets

The matrix assumes economics permit the chosen cell; often they do not, and constraints should be applied first because they prune fastest.

Cost discipline:

- Price the unit of work, not the token: compute expected tokens per completed task, including retries, reflection rounds, and abandoned trajectories, times price, against the value of the completed task; a support resolution worth a few dollars supports a very different token budget than a code change worth hours of engineer time.
- Know the multipliers by heart: chaining multiplies by steps; voting and self-consistency by n; evaluator-optimizer by roughly rounds times two; agent loops by trajectory length, which is high-variance; multi-agent orchestration historically runs an order of magnitude or more over single-call chat (Chapter 01, current as of early 2026).
- Cost engineering levers, in the order to pull them: cache the stable prompt prefix; route easy traffic to small models; cap budgets per run with best-so-far semantics; only then micro-optimize prompts.
- A ceiling is a design input, not an aspiration: if the ceiling forbids the matrix cell the task wants, the honest moves are narrowing the task, raising verifiability so cheaper architectures suffice, or concluding the automation is not yet economic; quietly degrading quality to fit the ceiling and hoping is the dishonest move, and it is common.

Latency discipline:

- Classify the interaction contract first: interactive-synchronous (seconds; a human is waiting mid-flow), interactive-streaming (first token fast, total time flexible; chat and coding sessions), and asynchronous (minutes to hours; research jobs, batch pipelines, fire-and-forget tasks).
- The contract prunes hard: interactive-synchronous excludes deep agent loops, multi-round reflection, and heavyweight planning regardless of their quality benefits; asynchronous contracts unlock everything, which is why the strongest agent products of the 2025-2026 era moved long work into explicitly asynchronous surfaces rather than making users watch.
- Streaming changes the effective budget: visible thinking, visible plan updates, and visible tool activity buy patience; the same 90-second task feels broken as a silent request and fine as a narrated one, so perceived latency is partly a harness design output (Chapter 06).
- Parallelization (Chapter 01) is the main latency lever inside a fixed architecture: sectioning and DAG-scheduled workers convert serial token time into wall-clock savings at unchanged token cost.

## 5. Case study: customer support bot

**Requirements**: answer product questions, handle order status, process refunds within policy; thousands of conversations daily; seconds-scale responses; errors visible to customers, refunds move money; cost per conversation must stay well under the value of deflecting a human ticket.

**Axes**: complexity medium (a handful of stable intents, each with a mostly fixed path); verifiability mixed by branch (order status is mechanically verifiable against the order system, policy answers are judgeable against a knowledge base, refund eligibility is mechanically checkable in code); risk tiered (answers medium, refunds high and partially irreversible).

**Decision**: a routing workflow, not an agent.
A cheap classifier routes intents; each route is a short chain with its own tools and prompt: retrieval-grounded answering for product questions, a lookup-then-summarize chain for order status, and a refund route where eligibility is computed by code from policy rules, with the model handling extraction and communication only.
Risk gates by tier: refunds above a threshold or failing clean eligibility go to a human queue; the "none of the above" route (Chapter 01's escape hatch) goes to a human, not to improvisation.
Reflection is limited to a judge-based QA sample offline (Chapter 04's hierarchy: the refund verifier is code, so the LLM judge is reserved for the judgeable branch); no runtime evaluator-optimizer, because latency and cost forbid it and single-pass quality on routed, narrow prompts meets the bar.

**Why not an agent loop**: the paths are enumerable, so autonomy buys nothing; a loop adds latency variance and an unbounded blast-radius surface exactly where risk is highest; and at this volume, workflow determinism converts evals into regression tests per route, which is how the system stays shippable weekly.
**The named downside**: the workflow will ossify at the edges; novel intents arrive as misroutes, and the maintenance contract is watching route distribution drift (Chapter 01) and periodically re-deriving whether a current-generation model could collapse routes into fewer, more general ones.

## 6. Case study: deep research

**Requirements**: multi-source research briefs on open questions; minutes-to-tens-of-minutes acceptable, asynchronous surface; output quality is judgeable but not mechanically verifiable; wrong facts embarrass, but a human reads the brief before acting; per-task value high enough to absorb heavy token spend.

**Axes**: complexity high (sources and follow-ups are input-dependent and unenumerable); verifiability judgeable at best (rubrics for coverage, sourcing, and internal consistency; no soundness); risk medium and human-buffered.

**Decision**: orchestrator-workers with agentic workers, on an asynchronous surface.
A lead planner decomposes the question into research threads (Chapter 01's dynamic decomposition; Chapter 03's DAG when threads are independent); parallel worker loops search, read, and extract with per-worker budgets; a synthesis pass composes the brief with citations required inline.
Reflection is deployed where Chapter 04 says it pays: a fresh-context judge scores the draft against a coverage-and-sourcing rubric with required evidence quotes, one or two evaluator-optimizer rounds, best-so-far kept; citation checking is pushed up the hierarchy to mechanical verification (do the sources exist and contain the quoted claims), because that is the one property here that can be made sound.
Latency is wall-clock-managed by parallel workers; cost is managed by per-thread token caps and an orchestrator instructed to scale worker count with question breadth (the overspawn failure of Chapter 01).

**Why not a single agent loop**: one loop serially exploring ten threads blows the wall-clock budget and accumulates a context that degrades synthesis; sectioned parallelism is the latency lever, and thread isolation keeps each worker's context clean.
**The named downside**: this is the most expensive architecture in the volume, its decomposition quality gates everything (a bad thread split poisons all workers), and the judge is the weakest link; the mitigation is the mechanical citation check plus human readership, and the residual risk of a fluent, well-cited, subtly wrong synthesis is real and should be stated to users.

## 7. Case study: coding agent

**Requirements**: implement changes across a real codebase from natural-language requests; sessions are interactive-streaming; the repository has tests and a compiler; changes are reviewed before merge but the agent edits files and runs commands live; task value is high.

**Axes**: complexity high (files touched and steps taken are unenumerable); verifiability the best in this volume (compiler, tests, linters are sound-enough verifiers with fast feedback); risk medium at edit time (workspace changes are revertible via version control) spiking high at command execution (arbitrary shell is irreversible).

**Decision**: the thin agent loop over a strong model, with the harness carrying the architecture (Chapter 06 throughout).
A single tool loop with curated tools (read, search, edit, execute) plus wired-in verification commands; extended and interleaved thinking as the deliberation mechanism rather than external search scaffolding (Chapter 02); a lightweight todo-list plan maintained and re-shown by the harness, with plan mode gating exploration from mutation on larger tasks (Chapter 03); reflection driven exclusively by external signals, test failures and compiler errors fed verbatim into retries (Chapter 04's ordering, applied); risk handled by the permission asymmetry, read free, mutation and execution gated with graduated consent, plus sandboxing for anything network-touching.
Durability (Chapter 05) is warranted selectively: interactive sessions live in process, while long asynchronous runs (overnight refactors, CI-triggered fixes) justify checkpointed execution and are exactly the workloads that earn a durable runtime.

**Why this and not plan-heavy orchestration**: the environment's verifiability is so strong that the loop self-corrects at step granularity, which substitutes for a priori planning; the 2023-2026 record (Chapter 06) shows elaborate multi-agent coding scaffolds depreciating against thin harnesses as models improved, and this task sits squarely where that lesson applies.
**The named downside**: trajectory cost is high-variance and rabbit holes are real; budgets, drift detection against the todo list, and the second-failure escalation rule (Chapter 03) are the containment, and the human review before merge remains the final gate that makes medium risk honest.

## 8. Case study: data pipeline copilot

**Requirements**: help data engineers build and modify ETL pipelines: generate SQL and transformation code, explain lineage, propose fixes for failing jobs; outputs execute against warehouses where a bad mutation can corrupt production tables; users are experts; interactive-streaming sessions; moderate volume.

**Axes**: complexity medium (requests cluster: generate, explain, diagnose, modify; each with mostly stable shape); verifiability strong for the generated artifacts (SQL parses, dry-runs, schema checks, row-count sanity comparisons; a staging environment can execute everything) but weak for intent (whether the transformation is the one the user meant is opaque to machines); risk sharply bimodal: reads and staging runs are low, production mutations are high and only partially reversible.

**Decision**: a workflow skeleton with one agentic joint, the hybrid Chapter 01 called the honest middle position.
Routing across the four request types; generation and modification routes run a verifier-guided loop (Chapter 03's LLM-Modulo made concrete: propose, then parse, dry-run, and diff row counts in staging, then repair from verifier output, budget two repairs, then surface); diagnosis gets a bounded agent loop with read-only tools over logs, lineage metadata, and query history, because failure investigation is the one genuinely unenumerable path in the product; explanation is a single retrieval-grounded call.
The risk design does the remaining work: the copilot never mutates production; it produces artifacts (a migration script, a PR into the pipeline repository) that the expert user applies through their existing deployment gate, which converts the high-risk cell into a human-gated medium one and keeps the forbidden quadrant (high stakes, opaque intent) permanently human-owned.

**Why not a full agent with warehouse write access**: the verifiability that is strong here covers correctness of form, not correctness of intent, and intent errors on production data are the catastrophic case; no reflection loop fixes an opaque-verifiability problem (Chapter 04), so the architecture routes around it structurally.
**The named downside**: the artifact-handoff design caps the automation ceiling, and users will ask for one-click apply; the principled path to granting it is narrowing scope (idempotent, reversible operation classes only) and buying more verifiability (automated backfill diffs, snapshot rollback), not relaxing the gate because the model got better.

## 9. Revisit triggers

An architecture decision is a dated claim about model capability, workload shape, and economics, and all three drift; mature teams schedule the revisit instead of waiting for pain.

- **Model-generation trigger**: on every major model upgrade, re-run the decision, not just the evals; the bitter-lesson discipline of Chapter 06 applies at architecture scale, and the specific question is which scaffolding, routes, and decomposition the new model makes deletable.
- **Route-distribution trigger**: workflow escape-hatch and misroute rates trending up mean the enumerable-paths assumption is decaying toward agent territory.
- **Budget-burn trigger**: agent trajectories lengthening or reflection rounds saturating their caps mean the task got harder or the harness degraded; investigate before raising caps.
- **Verifiability trigger**: new verifiers (a test suite that did not exist, a staging environment, a simulator) re-open the matrix; a cell move from judgeable to mechanically verifiable is the cheapest architecture upgrade available and is routinely left unclaimed.
- **The half-life heuristic**: as of early 2026, architecture decisions in this space have had a useful half-life of roughly one model generation; write the decision down with its assumptions explicitly (this task is medium-complexity because the paths are X, Y, Z), so the future revisit is a diff against stated assumptions rather than archaeology.

## 10. Claims that will rot

The case-study decisions encode early-2026 model capabilities, prices, and latencies; every one of them should be re-derived, not copied, at reading time, and the multi-agent cost multiplier and autonomy-length claims are the fastest-moving inputs.
The three axes, the matrix's structure, the forbidden quadrant, the constraint-first pruning order, and the revisit triggers are the durable framework; they are deliberately stated in terms of task properties and verification soundness, which do not rot, rather than model names, which do.

## Exercises

1. Run the full procedure on a task from your own work: characterize all three axes with evidence, apply your real cost ceiling and latency contract, select the cell and the family, and write the one-page decision memo including the named downside and the revisit triggers with dates.
2. Take the support-bot case study and change one requirement: refunds are now fully reversible for 24 hours; re-derive the risk tier, the gates, and whether the architecture family changes, and justify each delta.
3. Find the forbidden quadrant in your organization: identify one process someone wants to automate that is high-stakes and opaque-verifiability, and write the two concrete investments (a verifier, a gate) that would move it to a buildable cell.
4. Price the deep-research architecture: estimate tokens for a 6-worker run with two evaluator rounds at current prices for a frontier model and a mid-tier model, and determine the per-brief value at which each configuration breaks even.
5. Audit an existing system you know against the matrix: state which cell it was built for, which cell its current traffic actually occupies, and which revisit trigger should have fired already.
6. Argue the strongest case against this chapter's framework: name a real task where the three axes mislead (for example, where perceived risk rather than actual risk governs, or where verifiability is gameable), and propose the amendment the framework needs.

## Godhood check

You have mastered this chapter when you can:

- Characterize any task on complexity, verifiability, and risk in under five minutes, with evidence rather than adjectives.
- Reproduce the matrix from memory, including the forbidden quadrant and why no architecture in this volume makes it safe.
- Apply cost and latency pruning before quality arguments, and compute the token multiplier of any composed architecture on sight.
- Defend each case study's decision against its strongest alternative, and state the named downside of your own preferred design unprompted.
- Write an architecture decision as a dated, assumption-explicit document with revisit triggers, and identify in someone else's system which trigger has already fired.
