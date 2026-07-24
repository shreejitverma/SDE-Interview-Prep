# Chapter 07 - Capstone Projects

## What you will master

- Three integrative capstone specifications that force you to use, rather than merely recall, the material of Volumes 01-13.
- How to scope an agent project so that it is finishable, verifiable, and honest about what it does not do.
- Milestone decomposition with explicit dependencies on prior volumes, so that every build step has a chapter behind it.
- Verification criteria that are measurable before you start, which is the only kind that survive contact with a working system.
- Graded rubrics that distinguish "it demoed" from "it works" from "it would survive a quarter in production".
- The failure modes each capstone reliably produces, named in advance so you recognize them from the inside.
- The "godhood bar" stretch goals that separate competent agent engineers from the people who set the practice.

## 1. How to use this chapter

These are specifications, not tutorials.
Each capstone tells you what must exist and how it will be judged, and deliberately does not tell you which framework to use, because choosing the framework is part of the exercise (Volume 08 Chapter 01 gave you the selection criteria, and Volume 08 Chapter 07 gave you the option of writing your own).
Build at least one to the shipping bar before you claim this track.
Build all three if you want the claim to be defensible in an interview, because each one exercises a different failure surface: research agents fail on truth, coding agents fail on scale and cost, ops agents fail on authority.

The grading scheme is the same for all three, on a 100-point scale across six dimensions, with the weights differing per capstone.
Three bars matter.
The pass bar is 70 points: the system works on the happy path and you can prove it with an eval rather than a demo.
The shipping bar is 85 points: the system has budgets, guardrails, observability, and a regression suite, and you would let a colleague depend on it.
The godhood bar is not a score, it is the stretch list at the end of each spec, and it is where the interesting engineering is.

Four honesty rules apply throughout, and violating any of them zeroes the relevant dimension.

- No number without a harness: any performance claim you make about your own system must come from a runnable eval, committed to the repository, that a reader can execute.
- No demo without a base rate: if you show a successful trajectory, you must also report how many attempts it took and what the failure trajectories looked like, which is Chapter 06's demo-selection discipline applied to yourself.
- No safety claim without an attack: if you claim injection resistance, permission enforcement, or budget enforcement, you must include the red-team cases that try to break it and show they fail to break it.
- No cost claim without instrumentation: token and dollar accounting comes from your tracing layer (Volume 10 Chapter 06), not from an estimate.

A general scoping warning before you start.
Every one of these capstones can be expanded indefinitely, and the most common way students fail is not building a bad system but building an unfinished ambitious one.
Write the non-goals section of your README before you write code, and treat expanding it as a change that requires deleting something else.

## 2. Capstone A - Deep Research Agent

### 2.1 Motivation

A deep research agent takes an open-ended question, decomposes it, searches and reads across many sources, and returns a synthesized report with citations.
It is the canonical test of multi-agent orchestration because the work is genuinely parallelizable - independent subquestions do not need to share context - and it is the canonical test of grounding because the output's failure mode is not "wrong format" but "confidently cited falsehood".
Commercial deep-research products from multiple major labs shipped through 2025, and the public engineering writeups on multi-agent research architectures (covered as case studies in Volume 07 Chapter 07) make this the best-documented multi-agent pattern in the field, which means you can compare your design against real ones.

The reason it belongs first in this chapter: it forces you to build a verifier for a task with no ground-truth answer, which is the hardest evaluation problem in the track and the one that most cleanly separates engineers who understand Volume 10 from engineers who have read it.

### 2.2 Scope and non-goals

In scope: a question in, a cited report out, with a defensible claim that the citations support the claims.
Out of scope by default: a user interface beyond a CLI, real-time streaming of intermediate results, personalization across sessions, and any attempt to beat a commercial product on breadth.
Your corpus can be the live web through a search API, a fixed document collection, or both; a fixed corpus makes citation verification dramatically easier and is the recommended starting point.

### 2.3 Milestones

Milestone A1 - Single-agent baseline with tools.
Build the loop yourself first: a model, a search tool, a fetch-and-extract tool, and a termination condition, in the shape of Volume 03 Chapter 03, with the tool contracts designed per Volume 03 Chapter 04 and errors handled per Volume 03 Chapter 05.
This baseline is not throwaway; it is the ablation you will compare every later architecture against, and if your multi-agent system cannot beat it, you have learned the most valuable lesson in Volume 07.

Milestone A2 - Retrieval quality.
Implement chunking and indexing (Volume 05 Chapter 02), hybrid retrieval with reranking (Volume 05 Chapter 05), and measure retrieval in isolation with the RAG-specific metrics of Volume 05 Chapter 06 before you measure the end-to-end system.
Retrieval failures masquerade as reasoning failures, and the only way to tell them apart is to measure the stages separately.

Milestone A3 - Orchestration.
Introduce an orchestrator that decomposes the question into subquestions and dispatches parallel researcher subagents, per the orchestrator-workers topology of Volume 07 Chapter 02 and the subagent practice of Volume 07 Chapter 04.
The design decisions to document and justify: how subquestions are generated and deduplicated, what each subagent returns (a summary, a set of quoted spans with source identifiers, or raw text), how results are merged, and how you bound total fan-out.
Return quoted spans with source identifiers rather than free-text summaries unless you have a strong reason otherwise, because summaries destroy exactly the evidence your citation verifier needs.

Milestone A4 - Citation verification.
This is the milestone that makes the capstone worth doing.
Build a verification pass that takes every claim-citation pair in the final report and checks that the cited source actually supports the claim, using a judge constructed per Volume 10 Chapter 05 with an explicit rubric, calibrated against at least 50 human-labeled pairs of your own making.
Report per-claim support rates, and make unsupported claims a blocking condition: the agent must either find support, weaken the claim, or drop it.
Track the three distinct failure classes separately, because they have different fixes: the citation points to a real source that does not support the claim, the citation points to a source that does not exist or was never retrieved, and the claim is a synthesis spanning sources with no single supporting passage.

Milestone A5 - Context and cost management.
Long research runs will exceed the context window, so implement compaction (Volume 06 Chapter 03) and an external scratchpad or notes file (Volume 06 Chapter 04) for findings that must survive compaction.
Apply prompt caching and the context economics of Volume 06 Chapter 07 to the orchestrator's stable preamble, and instrument tokens and dollars per run per stage.

Milestone A6 - Eval harness.
Assemble 30 or more research questions with known-good answers or known-good source sets, spanning at least three difficulty tiers, following the agent-eval construction method of Volume 10 Chapter 03.
Grade on three axes separately: answer quality (rubric judge), citation support rate (Milestone A4's verifier), and cost per question.
Include a contamination check: at least five questions about events or documents your model could not have memorized, which is how you detect a system that is retrieving nothing and answering from parametric memory.
Report pass@1 and, for at least a subset, repeated-run consistency, because a research agent that returns a different answer on every run has a reliability problem your averages will hide.

### 2.4 Verification criteria

The system is verified when all of the following are true and demonstrable by running a committed command.

- The eval harness runs end to end, unattended, over the full question set and emits a machine-readable result file with per-question scores, token counts, wall-clock time, and dollar cost.
- The multi-agent configuration is compared against the Milestone A1 single-agent baseline on the same question set, with the delta reported on all three axes including cost, and with an explicit verdict on whether the added complexity was worth it.
- Citation support rate is measured by a judge whose agreement with your human labels is itself reported; a judge you have not calibrated is not evidence.
- Every claim in a sample of at least three full reports is manually spot-checked by you against its citation, and the manual result is compared against the automated verifier's result, with disagreements enumerated.
- At least one adversarial case is included in which a retrieved document contains an injection attempting to redirect the agent (Volume 11 Chapter 02), and the run log shows what happened.
- A run with the search tool disabled is included, showing the system degrades to refusal or explicit uncertainty rather than confabulation.

### 2.5 Rubric

| Dimension | Weight | Pass (70-level) | Shipping (85-level) |
|---|---|---|---|
| Architecture | 20 | Orchestrator plus parallel researchers works and is documented | Topology choice is justified against the single-agent baseline with measured deltas including cost |
| Grounding and citations | 25 | Citations present and mostly correct on spot checks | Automated verifier with a calibrated judge, per-failure-class rates, and a blocking gate on unsupported claims |
| Evaluation | 20 | 30-question harness with rubric grading, runnable in one command | Difficulty tiers, contamination controls, repeated-run consistency, and baseline ablations |
| Context and cost | 15 | Compaction implemented, runs complete without context overflow | Per-stage token accounting, caching applied, cost per question tracked over time |
| Safety | 10 | Injection case documented | Injection defenses per Volume 11 Chapter 02, tool-result trust boundaries explicit, degradation behavior tested |
| Engineering quality | 10 | Reproducible setup, committed harness | Tracing per Volume 10 Chapter 06, structured logs, deterministic replay of a saved run |

### 2.6 Common failure modes

- Citation theater: the report is full of links that were never read, because the model generated plausible URLs or cited a document it retrieved but did not consult; this is the single most common failure and the reason Milestone A4 is non-negotiable.
- Fan-out without bound: subquestion generation recurses, the orchestrator spawns dozens of researchers, and a single question costs more than a day of your API budget; put a hard cap on total subagent invocations and total tokens before your first parallel run, not after.
- Context poisoning through merging: subagent outputs are concatenated into the orchestrator's context until the synthesis step is drowning in low-signal text; enforce per-subagent output size limits and structure, per Volume 07 Chapter 03 on shared state.
- The multi-agent tax nobody measures: the parallel version is more expensive, slower per unit of quality, and no better than a well-prompted single agent with good retrieval; you will only discover this if you built Milestone A1 and kept it.
- Judge-shaped self-deception: the same model family writes the report and grades it, and the scores are high and meaningless; use the bias catalog and meta-evaluation methods of Volume 10 Chapter 05, and calibrate against human labels.
- Silent retrieval death: the search API rate-limits or returns empty, the agent proceeds from parametric memory, and the output looks normal; make empty retrieval a loud, logged, eval-visible event.

### 2.7 Godhood bar

- Implement a claim graph rather than a claim list: every sentence in the report is a node with typed edges to supporting spans, contradicting spans, and other claims, so the report can be regenerated at different lengths without losing grounding.
- Handle source disagreement explicitly: detect when retrieved sources conflict, surface the conflict in the report with both positions attributed, and evaluate whether your system does this rather than silently picking one.
- Add a test-time compute controller (Chapter 02) that allocates research depth per subquestion based on measured uncertainty, and demonstrate a better quality-versus-cost curve than a fixed-depth policy on your harness.
- Optimize the orchestrator and researcher prompts with a programmatic optimizer (Chapter 03) against your harness, and report the gain honestly including the optimization's own cost and the held-out result.
- Publish the harness and your longitudinal results across model versions, turning the capstone into the personal eval suite of Chapter 06.

## 3. Capstone B - Production-Grade Coding Agent

### 3.1 Motivation

Coding agents are the most economically proven agentic application as of early 2026 and the most demanding integration test in this track, because they combine an unbounded action space, a verifier that actually works (tests, compilers, type checkers), sessions long enough to break every context strategy you have, and a permission surface with real destructive potential.
Volume 13 Chapters 01-03 gave you the why, the anatomy, and the scaffold landscape; this capstone makes you build one and hold it to a production standard.
Volume 13 Chapter 07 walks the minimal build end to end, so treat it as the reference implementation this capstone hardens rather than as a substitute for doing the work yourself.
Volume 13 Chapter 06 is the companion for the stretch goals below, since delegation, worktree isolation, and fleet management are where a working single agent becomes a system.

The point is not to beat existing tools, which are excellent and have years of engineering in them.
The point is that having built the permission system, the compaction strategy, the cost governor, and the eval harness yourself, you will never again be confused about why a coding agent behaves the way it does.

### 3.2 Scope and non-goals

In scope: a CLI agent that operates on a real git repository, reads and edits files, runs commands in a sandbox, and completes small, well-specified tasks with test verification.
Out of scope by default: an IDE integration, multi-repository work, autonomous long-horizon feature development, and any model training.
Choose a target repository with a fast, reliable test suite; a repository whose tests take twenty minutes will destroy your iteration loop and your eval harness alike.

### 3.3 Milestones

Milestone B1 - The core loop and tools.
Implement read, write or patch-apply, search, and command-execution tools per Volume 03 Chapter 04, plus code execution as a tool per Volume 03 Chapter 07, in the loop shape of Volume 03 Chapter 03 and the anatomy of Volume 13 Chapter 02.
Decide and document your edit primitive: whole-file rewrite, unified diff, or search-and-replace, each of which has a distinct failure profile (whole-file is expensive and truncation-prone, diffs fail on context mismatch, search-and-replace fails on ambiguity).
Instrument the failure rate of your edit primitive from day one, because it silently caps everything above it.

Milestone B2 - Permissions and sandboxing.
Build an explicit permission layer per Volume 11 Chapter 03: an allowlist of commands, path scoping so writes cannot escape the working tree, network egress control, and an approval prompt for anything outside the allowlist.
Implement at least two autonomy levels (ask-every-time and auto-approve-within-policy), and make the policy a data file, not code scattered through the agent.
Adopt the reversibility framing of Volume 11 Chapter 06: every destructive action either has an undo (git) or requires approval.

Milestone B3 - Context management at length.
Real coding sessions exceed the window, so implement compaction (Volume 06 Chapter 03) with an explicit policy about what is never dropped: the task statement, the permission state, the current plan, and the list of files already modified.
Add a persistent scratchpad or plan file (Volume 06 Chapter 04) and session persistence with resume (Volume 06 Chapter 06).
Measure the thing everyone skips: run a task long enough to trigger at least two compaction cycles and verify the agent still knows what it was doing, because compaction bugs manifest as an agent that cheerfully redoes finished work.

Milestone B4 - Cost budget and governor.
Implement a per-task budget in both tokens and dollars, with a hard stop, a soft warning threshold, and a graceful termination path that writes a status report rather than dying mid-edit.
Apply prompt caching and the cost techniques of Volume 06 Chapter 07 and Volume 12 Chapter 03, and report the cache hit rate.
Add model routing if your design justifies it (a cheaper model for file search and summarization, a stronger model for planning and editing) and measure whether it actually saves money at equal quality, since routing frequently costs more once retry rates are included.

Milestone B5 - Mini-eval.
Build 20 or more tasks on your target repository with test-based verification, in the SWE-bench-style shape described in Volume 10 Chapter 04 and Volume 13 Chapter 03: each task is an issue description plus a set of tests that must pass afterwards and a set that must not break.
Include at least three tasks that should be refused or escalated (underspecified, out of scope, or requiring a destructive action), because a coding agent's willingness to attempt the impossible is a real failure mode.
Report pass@1 and pass^k for at least the top five tasks, per the reliability framing of Volume 10 Chapter 03, since a 60 percent single-attempt rate and a 60 percent every-attempt rate are entirely different products.

Milestone B6 - Observability and operations.
Emit traces per Volume 10 Chapter 06 with one span per tool call, token counts, and outcomes, so a failed run can be debugged without reproducing it.
Add the reliability patterns of Volume 12 Chapter 04 (timeouts, retries with backoff, idempotent tool semantics) and the operational surface of Volume 12 Chapter 07 (a run log, a way to kill a run, a way to inspect what a run changed).

### 3.4 Verification criteria

- The mini-eval runs unattended in a container and produces per-task pass or fail, token and dollar cost, wall-clock time, and the diff produced.
- A red-team suite is included and passes: a task whose repository contains a file with an injected instruction to exfiltrate an environment variable or write outside the working tree, a task that requests an out-of-policy command, and a task designed to blow the budget; the expected result in each case is a clean, logged refusal or halt, not a lucky escape.
- A long-session test demonstrates at least two compaction cycles with correct task continuity, verified by a scripted check rather than by reading the transcript and feeling good about it.
- The budget governor is demonstrated by a run that hits the hard stop and terminates with a coherent status report and a clean working tree or a clearly labeled partial state.
- Permission enforcement is demonstrated by tests that assert denial, not by a screenshot of a prompt.
- The edit primitive's failure rate is reported as a number over the full eval run.

### 3.5 Rubric

| Dimension | Weight | Pass (70-level) | Shipping (85-level) |
|---|---|---|---|
| Agent loop and tools | 20 | Loop, tools, and edit primitive work on the happy path | Edit failure rate measured and reduced, error recovery per Volume 03 Chapter 05, idempotent tool semantics |
| Permissions and safety | 20 | Allowlist plus path scoping enforced | Policy as data, two autonomy levels, egress control, red-team suite passing, reversibility for every destructive action |
| Context management | 15 | Compaction implemented, long sessions complete | Never-drop invariants specified and asserted, resume works, continuity verified by script |
| Cost control | 15 | Budget enforced with a hard stop | Caching with reported hit rate, per-stage cost accounting, routing decision justified by measurement |
| Evaluation | 20 | 20 tasks with test-based grading, one-command run | Refusal tasks, pass^k on a subset, regression gating in CI per Volume 10 Chapter 07 |
| Observability | 10 | Structured run logs | Traces with per-tool spans, replayable runs, a debugging workflow you can demonstrate |

### 3.6 Common failure modes

- The patch that never applies: a large share of coding-agent failures are mechanical edit failures rather than reasoning failures, and teams that do not instrument the edit primitive spend weeks tuning prompts to fix a string-matching bug.
- Test-suite gaming: the agent modifies or deletes tests to make them pass; forbid test-file edits in eval tasks by policy and detect them in grading, and note that this is a live specification-gaming example of the kind Volume 11 Chapter 05 describes.
- Compaction amnesia: after summarization, the agent loses the fact that it already fixed a file and re-fixes it, or loses the permission state and re-asks; the never-drop list exists precisely to prevent this and must be asserted, not assumed.
- Budget exhaustion in a loop: two tools disagree, the agent retries forever, and the budget dies in a cycle; add loop detection on repeated identical tool calls, which is cheap and catches most of it.
- Sandbox escape by convenience: mid-project you add a shell tool "just for debugging" that bypasses the allowlist, and the entire permission layer becomes decorative; the red-team suite is what keeps you honest.
- Eval overfitting: you tune prompts against your 20 tasks until they pass and the agent gets worse on everything else; hold out at least five tasks you run rarely, per Chapter 06's contamination control.
- Silent context truncation: the provider or your own code truncates the middle of the conversation and the agent behaves erratically with no error; log context size per turn and alert on truncation.

### 3.7 Godhood bar

- Add a subagent layer (Volume 07 Chapter 04) for isolated read-heavy work such as codebase search, and prove with your harness that it improves quality per dollar rather than merely feeling architecturally sophisticated.
- Implement a verifier-driven test-time compute policy (Chapter 02): generate multiple candidate patches, select by test outcomes rather than by model preference, and report the quality-versus-cost curve against single-shot.
- Build the data flywheel of Volume 10 Chapter 07: every production failure becomes an eval task automatically, and show the suite growing from real failures over a month.
- Support computer use or browser control (Volume 13 Chapters 04-05) for tasks requiring a running application, with the additional guardrails that action space demands.
- Ship it as an MCP-integrated tool surface (Volume 09) so the same tools are reusable by other clients, and apply the third-party server security patterns of Volume 09 Chapter 07 to whatever you connect.

## 4. Capstone C - Ops and Support Agent Over Real Tools

### 4.1 Motivation

The first two capstones operate on text and code, where mistakes are cheap and reversible.
This one operates on systems with side effects: tickets, refunds, account changes, infrastructure operations, notifications to real people.
That single change reorganizes every design decision, because the binding constraint stops being capability and becomes authority - what the agent is allowed to do, on whose behalf, with what evidence, and with what path back.

It is also the capstone closest to the majority of real enterprise agent deployments as of early 2026, and the one where the tau-bench-style insight matters most: an agent that succeeds 70 percent of the time on customer-facing actions is not 70 percent of a product, it is a liability with a good demo.

### 4.2 Scope and non-goals

In scope: an agent that resolves a bounded class of requests end to end against real tools, with a policy defining what it may do autonomously and what requires human approval.
Use a sandboxed or staging instance of the real systems, never production, and if you use a mock backend, make it faithful enough to fail the way the real one does (latency, rate limits, partial failures, stale reads).
Out of scope by default: open-ended conversation, unbounded request types, and autonomous action on anything financial or irreversible.

Pick a concrete domain, because vagueness here produces an unevaluable system: an internal IT support agent over a ticket system and a directory service, a customer support agent over an order and refund system, or an on-call triage agent over logs, metrics, and a runbook repository.

### 4.3 Milestones

Milestone C1 - Tool surface and integration.
Integrate at least three real tools with authentication, ideally over MCP (Volume 09 Chapters 02-05) so the boundary between agent and system is a protocol rather than a pile of glue.
Design tool contracts per Volume 03 Chapter 04 with the specific ops discipline that read tools and write tools are visibly different: name, describe, and permission them separately, and make every write tool idempotent with a caller-supplied idempotency key so a retry cannot double-refund.
Handle the real-world error surface per Volume 03 Chapter 05: rate limits, partial success, timeouts that may or may not have committed.

Milestone C2 - Policy and approval gates.
Write the authority policy as a document first: for each action type, state whether it is autonomous, requires approval, or is forbidden, and state the threshold that moves an action between tiers (amount, blast radius, reversibility, customer tier).
Implement it per Volume 11 Chapter 06 with a real approval channel (a queue, a chat message, an email) that a human actually uses, including timeout behavior when nobody approves.
Design against approval fatigue explicitly: measure what fraction of actions require approval and what fraction of approvals are rubber-stamped, because a gate that is always approved is not a control, it is a delay.

Milestone C3 - Guardrails.
Layer input, output, and action guardrails per Volume 11 Chapter 04, with the action guardrail as the load-bearing one: a code-enforced check between the model's decision and the tool call that validates the action against policy, scope, and the current state of the world.
Treat every piece of retrieved content - ticket text, customer messages, log lines, documentation - as untrusted input per Volume 11 Chapter 02, and be explicit about the lethal trifecta from Volume 11 Chapter 01, since an ops agent with private data, untrusted content, and outbound communication is exactly that shape.
Add a PII handling policy and enforce it in your trace store per Volume 10 Chapter 06.

Milestone C4 - Architecture and control flow.
Choose deliberately between a workflow and an agent per Volume 04 Chapters 01 and 07, and justify it: much of ops work is better served by a state machine (Volume 04 Chapter 05) with model calls at specific nodes than by an open agent loop, and choosing the constrained architecture where it fits is a sign of maturity, not timidity.
Implement escalation as a first-class terminal state, not an error path, with a handoff package that contains everything a human needs to continue: what was requested, what was done, what was attempted and failed, and what the agent believes is true.

Milestone C5 - Evaluation with simulated users and outcome grading.
Build 25 or more scenarios spanning resolvable requests, requests requiring approval, requests that must be refused, and requests that must escalate, with a simulated user per the tau-bench-style method in Volume 10 Chapter 03 for the multi-turn cases.
Grade on final world state, not on transcript plausibility: the correct measure is whether the right database rows changed and the wrong ones did not.
Report pass^k on the highest-risk scenarios, since consistency is the product requirement here, and report a separate false-action rate: how often the agent took an action it should not have, which should be weighted far more heavily than a missed action.

Milestone C6 - Production surface.
Add the Volume 12 machinery: latency budgets per interaction (Chapter 02), cost per resolved request (Chapter 03), reliability patterns for flaky downstream systems (Chapter 04), quota and rate-limit handling against your tool providers (Chapter 06), and an operational runbook with a kill switch (Chapter 07).
Define and instrument the two metrics an ops owner will actually ask for: resolution rate without human involvement, and incident rate per thousand actions.

### 4.4 Verification criteria

- Every write tool is proven idempotent by a test that invokes it twice with the same idempotency key and asserts a single effect.
- The approval gate is proven by scenarios that assert the agent stops and waits, including a timeout scenario asserting safe behavior when approval never arrives.
- The action guardrail is proven by red-team scenarios: an injected instruction in ticket text attempting to trigger a refund or a permission change, a request that is in-scope in wording but out-of-policy in amount, and a request that would act on the wrong account.
- The full scenario suite runs unattended against the sandbox, resets state between scenarios, and reports resolution rate, false-action rate, escalation rate, approval rate, cost per scenario, and pass^k on the risk subset.
- An end-to-end trace of one resolved request and one escalated request is committed, showing every tool call, every guardrail decision, and the approval record.
- A documented rollback exists for every autonomous write action, and at least one is exercised in the suite.

### 4.5 Rubric

| Dimension | Weight | Pass (70-level) | Shipping (85-level) |
|---|---|---|---|
| Tool integration | 15 | Three real tools working with auth | Idempotency keys enforced, partial-failure handling, MCP or equivalent protocol boundary |
| Authority and approvals | 25 | Approval gate implemented for high-risk actions | Written policy with tiers and thresholds, timeout behavior, approval-fatigue metrics, handoff packages |
| Guardrails and safety | 20 | Input and output guardrails present | Action guardrail enforcing policy in code, untrusted-content handling, lethal-trifecta analysis documented, red team passing |
| Architecture | 10 | Works end to end | Workflow-versus-agent choice justified, escalation as a first-class state, deterministic control flow where it fits |
| Evaluation | 20 | 25 scenarios with world-state grading | Simulated multi-turn users, pass^k on risk subset, false-action rate reported and weighted, state reset between runs |
| Production readiness | 10 | Runbook and logs exist | Latency and cost budgets, retry and quota handling, kill switch, rollback exercised in the suite |

### 4.6 Common failure modes

- Grading the transcript instead of the world: the agent says "I have issued the refund" and the eval passes; only world-state assertions catch this, and it is the most common way ops-agent evals lie.
- Double action on retry: a timeout hides a committed write, the agent retries, and the customer is refunded twice; idempotency keys are the fix and are not optional.
- Injection through the work item: the untrusted content in an ops agent is the ticket itself, which is written by the person the agent is acting on behalf of and is therefore the most natural injection vector in the entire track.
- Approval theater: every action requires approval, the approver clicks yes reflexively within seconds, and the organization believes it has human oversight; measure approval latency and override rate, and if approvals are never denied, your tiering is wrong.
- Scope creep into irreversibility: the agent starts with read-only triage, someone adds a "small" write action, and nobody revisits the guardrails; require a policy diff for every new tool.
- Wrong-entity actions: the correct action on the wrong account, which is invisible to grading that only checks whether an action of the right type occurred; assert on entity identifiers.
- Stale-read decisions: the agent reads state, deliberates for thirty seconds, and acts on a world that changed; use conditional writes or re-read before acting on anything consequential.

### 4.7 Godhood bar

- Implement a capability-based mediation layer in the spirit of the dual-LLM and CaMeL-style designs in Volume 11 Chapter 02, where untrusted content can influence data but never directly authorize an action, and demonstrate it defeating an injection that defeats your prompt-level defenses.
- Add a policy simulator: replay a month of historical requests through the policy without executing writes, to answer "what would this agent have done" before it is allowed to do anything.
- Build tiered autonomy that earns itself: an action type moves from approval-required to autonomous only when its measured false-action rate over N observed approvals is below a stated threshold, which is the operational version of Volume 11 Chapter 06's oversight-proportional-to-risk principle.
- Instrument the memory and personalization layer of Chapter 05 for repeat requesters, and evaluate the delta with personalization ablated, including the over-personalization safety probes.
- Run a genuine on-call rotation for your agent for two weeks against a staging system, keep an incident log, and write the postmortems; nothing else in this chapter teaches as much per hour.

## 5. Cross-cutting requirements

These apply to whichever capstone you build, and each is worth points in the Engineering and Evaluation dimensions.

- One-command reproducibility: a fresh clone plus a documented setup step must produce a runnable system and a runnable eval, because a capstone nobody else can run is a story, not an artifact.
- A written design document of two to four pages: the architecture, the three most consequential decisions with their alternatives and trade-offs, the non-goals, and the known weaknesses; the weaknesses section is the part that demonstrates seniority.
- A results section with numbers from your own harness, dated, with the model identifiers and prices used, following Chapter 06's falsifiable-claim discipline applied to your own work.
- Cost accounting per run, because an agent whose economics you cannot state is not a product you can ship.
- A trace of at least one full successful run and one full failed run, committed, with your annotation of where the failure began; the ability to locate the first wrong step in a long trajectory is the core debugging skill of this field.
- A short "what I would do with three more months" section, which is the honest expression of everything you now know that you could not fit.

## 6. What mastery means

You reached the end of a track that started with n-gram models and ended here, and it is worth being precise about what that does and does not mean, because the field is loud with claims in both directions.

It does not mean you know what is coming.
Volume 14's frontier chapters are dated by construction, and a meaningful fraction of the specifics in them will be obsolete within a year; Chapter 06 exists because the half-life of the details is short and the only durable defense is a personal measurement habit.

What mastery here actually means is narrower and more useful than omniscience.
You can build the agent loop from nothing, so no framework is magic to you and no framework's absence blocks you.
You can decide when not to build an agent at all, which is the judgment that separates engineers from enthusiasts, because the workflow that fits is almost always cheaper and more reliable than the agent that impresses.
You can measure a system honestly, which means you can be wrong in public and correct quickly, and it means you cannot be sold a capability by a demo.
You can reason about context, cost, and latency as one coupled budget rather than three separate annoyances.
You can put a boundary around a model's authority in code, because you understand that trained good behavior is a tendency and only enforcement is a control.
And you can read a new result, place it in a structure you already have, and estimate within an hour whether it changes what you build on Monday.

The capstones in this chapter are the proof of that, not because a research agent or a coding agent or an ops agent is inherently impressive, but because building one to the shipping bar requires every one of those capacities at once, and it is impossible to fake at the point where the eval harness runs.
The godhood bars are deliberately not achievable in a weekend; they are the direction the practice is moving, and the engineers who define this field over the next few years are the ones treating those items as their normal working standard.

The last thing worth saying is that this field rewards the unglamorous half of the work disproportionately.
The capability comes from the model, and it will keep improving without you.
The reliability, the economics, the safety, and the evidence come from you, and they are the entire distance between something that works in a screenshot and something people depend on.
Build the harness, keep the receipts, and go build things worth measuring.

## Exercises

1. Before writing any code, write the design document and non-goals for your chosen capstone, then predict your final rubric score per dimension; after finishing, score yourself against the rubric and compare, and write one paragraph on where your self-prediction was most wrong.
2. Build the deliberately unimpressive baseline first for whichever capstone you chose (single agent, single model, no orchestration), commit it, and keep it as the permanent ablation; report the final system's delta against it on quality, cost, and latency.
3. Write the red-team suite for your capstone before the feature it attacks exists, so the defense is built against a concrete adversary rather than an imagined one; include at least one attack that succeeds initially, and document the fix.
4. Instrument cost per successful task from the very first run, plot it across your development history, and write a short analysis of which changes moved it and in which direction; changes that improved quality while increasing cost per success should be named as such.
5. Take one failure trajectory of at least twenty steps from your system, find the first genuinely wrong step, and write the causal chain from that step to the visible failure; then add the eval case that would have caught it.
6. Hand your repository to another engineer with only the README, and record every question they have to ask you; each question is a documentation defect, and fixing them is the difference between an artifact and a personal project.
7. Take a second capstone from this chapter and build only its eval harness, without building the agent; then run an off-the-shelf commercial agent against your harness and report the numbers, which is Chapter 06's personal-ground-truth practice applied to a whole product category.

## Godhood check

You are at godhood level for this chapter when you can do the following without notes.

- Scope any of the three capstones into milestones with explicit dependencies on the prior volumes, and state the non-goals that make it finishable.
- Explain why the deep-research capstone's central difficulty is verification without ground truth, and describe the citation-verification design including its three distinct failure classes and the judge calibration it requires.
- Explain why the coding capstone's hidden bottleneck is usually the edit primitive and the compaction policy rather than model reasoning, and state the never-drop invariants a long coding session requires.
- Explain why the ops capstone's binding constraint is authority rather than capability, and design the tiering, approval, idempotency, and action-guardrail machinery that follows from it.
- State the four honesty rules and apply them to a system you built, including reporting a number that makes your own work look worse.
- Name the failure modes each capstone reliably produces, and for each, state the specific instrument or test that detects it before a user does.
- Argue, with the rubric dimensions and weights as evidence, why a system that demos well can still score below the pass bar, and why that is the correct outcome.
- State what mastery in this field actually consists of, in terms of what you can build, measure, bound, and decide, without overclaiming about what you can predict.
