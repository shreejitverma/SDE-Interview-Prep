# Chapter 07 - Case Studies

## What you will master

- Anthropic's multi-agent research system in full: architecture, prompt engineering lessons, economics, evaluation, and production engineering.
- The deep research product category, and the instructive split between orchestrated multi-agent and RL-trained single-agent designs.
- ChatDev and MetaGPT as the canonical role-play software teams: what they claimed, what held up, and what the field learned from them.
- Fleet patterns for coding agents: task-level parallelism, worktree isolation, and the human as merge coordinator.
- The cross-case synthesis: which architectural choices recur in systems that work.

Every system described here is date-stamped to its publication or deployment era; treat the specifics as history and the recurring choices as the durable content.

## 1. Anthropic's multi-agent research system

### 1.1 The system

Anthropic published a detailed engineering account (June 2025) of the multi-agent system behind their Research feature, and it remains the best-documented production multi-agent deployment as of early 2026.
The architecture is the orchestrator-workers star of Chapter 02: a lead agent (Opus-class model at the time) interprets the user's query, plans a decomposition, and spawns worker subagents (Sonnet-class) that search in parallel, each with a clean context and its own tool budget.
Workers act as intelligent filters: they iterate on searches, evaluate sources, and return condensed findings rather than raw pages, which is context isolation doing its job as a compression function (Chapter 01).
The lead synthesizes findings, spawns further workers if gaps remain, and a dedicated citation agent runs at the end to attach claims to sources, which is a verification subagent in the sense of Chapter 04, specialized to the one property the product must not get wrong.
Memory and artifacts handle the long-horizon problem: the lead persists its plan externally so that context compaction cannot destroy it, and subagent outputs can flow through external storage with lightweight references rather than through the lead's window.

### 1.2 The reported results and the honest reading

The headline result: the multi-agent system outperformed a single-agent Opus-class baseline by 90.2% on their internal research eval (their number, their eval, mid-2025 models).
The accompanying analysis is the honest part and the reason this case study leads the chapter: in their evals, token usage alone explained roughly 80% of performance variance, with tool-call count and model choice covering most of the rest.
The architecture, on their own account, is primarily a mechanism for spending more tokens effectively in parallel on parallelizable work, and they state directly that multi-agent systems burn roughly 15x chat-level tokens and therefore only fit tasks whose value carries that cost.
They are equally direct about scope limits: domains requiring tight coupling and shared context, such as most coding, fit the architecture poorly, which is the Chapter 01 reconciliation stated by the multi-agent camp itself.

### 1.3 The engineering lessons

Their published lessons map almost one-to-one onto the preceding chapters, which is why this volume is structured as it is.
Vague delegation failed: early leads spawned workers with instructions like "research the semiconductor shortage", producing duplicated and misdirected work, and the fix was the full delegation contract of Chapter 04: objective, output format, tool guidance, source guidance, and explicit task boundaries per worker.
Effort had to be taught: models did not infer proportional investment, so prompts encode scaling rules from one worker with a few tool calls for simple facts up to ten-plus workers with divided responsibilities for complex queries, and fan-out is capped rather than left to enthusiasm.
Tool descriptions were load-bearing enough that they used an agent to test and rewrite tool descriptions, reporting that the rewritten descriptions cut task completion time substantially for subsequent agents (their figure was around a 40% reduction).
Extended thinking was used as a controllable planning scratchpad in both lead and workers.
Evaluation was end-state rather than trajectory: LLM-as-judge with a rubric (factual accuracy, citation accuracy, completeness, source quality, tool efficiency) scoring the deliverable, since valid research paths vary too much for step-matching, supplemented with small-N human review that caught judge blind spots such as workers preferring SEO-optimized sources over primary ones.
Production engineering was distributed-systems work: checkpointing for resume instead of restart, retry logic around stateful long-running workers, and rainbow deployments so prompt and harness updates would not break in-flight agents, all Chapter 06 mitigations in production form.

## 2. Deep research products: two architectures, one category

The deep research category (query in, cited multi-page report out, minutes of latency) emerged across 2024-2025: Gemini Deep Research shipped December 2024, OpenAI's Deep Research followed February 2025, Anthropic's Research arrived spring 2025, with Perplexity and xAI shipping equivalents in the same window.
The category is the natural habitat of multi-agent design because the task is read-heavy, breadth-first, and value-dense enough to justify the token multiplier (Chapter 01's rubric answered affirmatively on every question).

The instructive fact is that the category's leaders did not converge on one architecture.
Anthropic's system is explicit orchestration: hand-engineered lead-and-workers structure, prompt-encoded delegation rules.
OpenAI's Deep Research was described by OpenAI as an o3-based model trained end-to-end with reinforcement learning on browsing-and-synthesis tasks: the long-horizon search behavior lives substantially in the weights, and the system presents as a single very capable agent rather than an orchestrated team.
Both produce competitive products in the same category, which yields the deepest lesson in this volume: explicit multi-agent orchestration and training-time compute are partially substitutable ways to buy the same capability, and the orchestration you hand-build today is a bet that model training will not subsume it tomorrow.
The practical corollaries: orchestration is available to everyone immediately and is inspectable and steerable, while trained-in capability requires frontier-lab resources but ships with lower per-task ceremony; and as trained long-horizon competence improves, the break-even point for hand-built orchestration keeps moving toward simpler structures (the same moving frontier flagged in Chapter 01).
Date-stamp: this substitution argument reflects early 2026; revisit it whenever a model generation visibly absorbs a coordination behavior you currently prompt for.

## 3. ChatDev and MetaGPT: the role-play software companies

### 3.1 What they were

ChatDev (Qian et al., 2023-2024) instantiated a virtual software company: CEO, CTO, programmer, reviewer, tester agents proceeding through a waterfall of phase-scoped chat pairs, from requirements through coding to testing and documentation.
MetaGPT (Hong et al., 2023) encoded standard operating procedures: product manager, architect, engineer roles that communicate primarily through structured artifacts (PRDs, design documents, interface specs) rather than free chat, an explicit application of message-discipline that Chapter 03 would endorse.
Both papers reported striking efficiency figures for their era, ChatDev producing small applications in minutes at well under a dollar of API cost, and MetaGPT reporting strong pass rates on function-level coding benchmarks (HumanEval-class) relative to contemporary baselines.

### 3.2 The honest results

Independent scrutiny was less kind than the launch numbers.
The MAST study (Chapter 06) measured correctness on realistic programming tasks and found ChatDev as low as roughly 25%, with popular multi-agent frameworks broadly failing large fractions of their target tasks.
The generated software that succeeded was overwhelmingly toy-scale: single-file games and utilities, not systems with real integration surface.
Function-level benchmark wins, meanwhile, do not evidence the multi-agent structure at all, since single strong models score comparably on such benchmarks without any team; the role-play was never isolated as the active ingredient with an ablation strong enough to survive later model generations.
And the waterfall pipeline exhibited exactly the Chapter 02 prediction: requirement misunderstandings surfaced at the test phase after every intermediate role had amplified them, with agent chatter adding telephone-game loss between phases.

### 3.3 What the field kept

The fair epitaph is that these systems were valuable probes, and three of their findings survive in modern practice.
Structured intermediate artifacts beat free chat: MetaGPT's document-mediated communication measurably reduced incoherent-chatter failures relative to conversational teams, and it prefigured the schema-and-artifact discipline of Chapter 03.
Phase separation with different contexts per phase is real value, but it is workflow design, not sociology: the same benefit ships today as pipelines and subagents without the company metaphor.
And the negative finding was the most useful: anthropomorphic role decomposition (CEO, CTO) partitions by human org chart rather than by context or decision structure, and it is precisely the "team of AI employees" trap Chapter 01 opened this volume by dismantling.
As of early 2026, no major production coding product uses the virtual-company pattern; the pattern's descendants are the sober pipeline and the verification pair.

## 4. Fleet patterns for coding agents

### 4.1 The pattern

The multi-agent pattern that actually took over coding is parallelism across tasks rather than within one: N independent agents, N independent tickets, one worktree or sandbox each, converging on review.
This is the Chapter 01 resolution applied at fleet scale: each ticket is internally write-heavy and coupled, so each gets a single sequential agent, while the fleet as a whole is embarrassingly parallel because tickets are chosen to be decision-independent.
The 2025-era product landscape converged on it from every direction: Claude Code running multiple local or cloud sessions in parallel worktrees, OpenAI's Codex cloud agents (May 2025) running each task in an isolated container, Devin marketing parallel task sessions, Cursor adding background agents, and GitHub-integrated agents picking up issues as independent assignments.

### 4.2 The mechanics

Isolation: one worktree or container per agent (Chapter 03's mechanics), parameterized ports and build caches, and disposable environments so a wedged agent is deleted rather than debugged.
Assignment: the partition is the ticket backlog, and partition quality is ticket hygiene; tickets that secretly share a decision surface (two tickets touching one interface) reintroduce every conflicting-decision failure of Chapter 06, so fleet operators curate independence at triage time, which is decision hoisting performed by humans.
Verification: CI is the non-LLM verifier with real teeth, and fleets lean on it hard; a branch that fails tests never reaches a human, and review subagents pre-screen diffs before human attention.
Convergence: merges serialize through review, which makes the human reviewer the fleet's bottleneck and its actual rate limiter; the practical fleet size is set not by compute but by how many diffs per day the owning humans can responsibly review, a constraint that dominates fleet economics as of early 2026.

### 4.3 The economics and the honest limits

The fleet pattern pays because its parallelism is real (independent tickets), its token multiplier is roughly linear in accomplished work rather than in coordination overhead, and its verification is cheap and mechanical (CI) rather than model-based.
Its limits are equally structural.
Throughput moves to the review bottleneck rather than disappearing, and teams that respond by rubber-stamping have removed the load-bearing verifier (Chapter 06 mode 14 at organizational scale).
Backlog independence is a finite resource: most codebases contain a core where everything couples, and fleet parallelism stops at its edge.
And per-ticket quality still depends entirely on single-agent competence; the fleet multiplies whatever that is, including the failure rate.

## 5. Cross-case synthesis

Reading the four cases against each other yields the recurring choices of systems that work, and they are this volume in miniature.
Working systems are shallow: one orchestration level (Anthropic), or zero with parallelism at the task boundary (fleets); the deep hierarchy appears only in the case study that did not hold up.
Working systems parallelize reads and serialize writes: parallel searchers with a single synthesizing writer, parallel tickets with serialized merges; the failed pattern parallelized coupled writing behind a role-play facade.
Working systems put verification outside the generator, and prefer mechanical verifiers where they exist: citation agents, CI gates, rubric judges; the failed pattern let the team grade its own homework.
Working systems spend engineering effort on boundaries (delegation contracts, artifact schemas, ticket hygiene) and treat the agents themselves as commodity loops.
And every working system's authors published its costs: the 15x multiplier, the review bottleneck; distrust any case study that reports capability without a cost ledger.

## Exercises

1. Reconstruct Anthropic's research architecture from memory as a diagram with every component labeled by its chapter concept (orchestrator, delegation contract, quarantine boundary, verification subagent, checkpoint), then check against section 1.
2. Run the substitution argument concretely: pick one coordination behavior you would prompt into an orchestrator today, and write down the observable evidence that would tell you a new model generation has absorbed it, making your orchestration removable.
3. Design the ablation that ChatDev-era papers lacked: an experiment isolating whether role decomposition, phase structure, or plain extra inference explains a multi-agent coding system's wins; specify baselines, tasks, and the confound each baseline removes.
4. Specify a five-agent coding fleet for a repository you know: ticket-independence criteria at triage, worktree and environment provisioning, CI gate policy, and a measurement plan for the review bottleneck; state the fleet size at which your plan saturates.
5. Write the one-page cost ledger for a deep-research feature at your own company: expected token multiplier versus single-agent baseline, latency, engineering cost of the orchestration and its observability, and the query-value threshold at which the architecture breaks even.

## Godhood check

- Describe Anthropic's research system end to end and state, with numbers, both the headline result and the token-economics caveats they published alongside it.
- Explain what the 80%-of-variance-from-token-usage finding implies about how much of "multi-agent" performance is architecture versus compute.
- State the two rival architectures in the deep research category and argue the substitution thesis in both directions.
- Give the honest account of ChatDev and MetaGPT: the claims, the MAST-measured reality, and the three findings the field kept.
- Explain why fleet patterns parallelize at the ticket boundary and identify the fleet's true rate limiter.
- List the five recurring choices of working systems from the synthesis and, for each, name the case that violated it and paid.
