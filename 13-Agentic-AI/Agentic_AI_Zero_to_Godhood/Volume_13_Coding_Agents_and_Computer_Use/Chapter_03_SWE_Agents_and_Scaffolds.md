# Chapter 03 - SWE Agents and Scaffolds

## What you will master

- SWE-bench in full depth: construction, evaluation mechanics, variants, known flaws, and how to read a reported score critically.
- SWE-agent and the agent-computer interface (ACI) insight: why the interface to the computer matters as much as the model behind it.
- Agentless and the scaffold-minimalism argument: when a fixed pipeline beats an autonomous loop.
- Devin and the autonomous-engineer ambition: what it promised, what it demonstrated, and what the backlash taught the field.
- The scaffold-decay thesis: why elaborate scaffolds matter less every model generation, with the evidence and its limits.
- Repository navigation strategies: grep-first agentic search versus embedding-based retrieval, and the economics that decide between them.
- Long-horizon task management: the techniques that keep an agent coherent across hours of work.

Date-stamp: benchmark scores and system details here are historical record as of early 2026; the design arguments are the stable content.

## 1. SWE-bench in depth

### 1.1 Construction

SWE-bench (Jimenez et al., Princeton, October 2023) mines real GitHub history from twelve popular Python repositories, including django, sympy, scikit-learn, matplotlib, and flask.
Each task instance is built from a merged pull request that both resolved an issue and modified tests.
The instance packages: the issue text, the repository snapshot at the pre-fix commit, and two hidden test sets extracted from the fixing PR.

- **Fail-to-pass tests**: tests that failed before the fix and pass after it; they define what "resolved" means.
- **Pass-to-pass tests**: tests that passed before and must still pass; they guard against fixing the issue by breaking everything else.

The system under evaluation sees the issue and the repository, never the tests.
It must produce a patch; the harness applies the patch in a containerized environment, runs both test sets, and scores the instance resolved only if all fail-to-pass tests pass and no pass-to-pass test regresses.
The full set has 2,294 instances.

This construction is the benchmark's genius and its flaw at once.
Genius: tasks are real, contextual, and verifiable end to end, unlike HumanEval's self-contained puzzles; the score measures the job (turn an issue into a merged-quality patch), not the snippet.
Flaws, documented over 2024-2025: some issues underspecify the fix so the hidden tests demand details no system could infer; some environments were fragile; some tests are flaky; and the gold patches sit in public git history, so training-data contamination is a standing concern for every model trained after 2023.

### 1.2 Variants

- **SWE-bench Lite** (2024): 300 instances filtered for self-containedness; made evaluation affordable, at the cost of over-representing small localized fixes.
- **SWE-bench Verified** (August 2024): OpenAI paid human annotators to screen 500 instances for solvability and test fairness; it became the de facto standard leaderboard and fixed the worst underspecification noise.
- **SWE-bench Multimodal** (2024): issues with screenshots and visual assets, mostly JavaScript repositories.
- **Multi-SWE-bench and similar** (2025): extensions beyond Python to Java, TypeScript, Go, Rust, C, and others, correcting the original's single-language bias.
- **SWE-bench Live and SWE-bench Pro** (2025): continuously refreshed or held-out commercial-repository variants built specifically to resist contamination and saturation, as Verified scores crossed into the 70s and stopped discriminating between frontier systems.

### 1.3 Reading a score critically

A reported "X percent on SWE-bench" is meaningless without five qualifiers, and you should demand all five:

1. **Which split**: full, Lite, Verified, or a private subset (Devin's early number was a random 25 percent subset of the full set, which is not comparable to Verified numbers).
2. **What harness**: model-only with a minimal scaffold, or a product system with retries, test-time compute, and ensembling; pass@1 or best-of-n.
3. **What cost**: dollars and wall-clock per instance; a 2 percent gain at 10x cost is a different engineering fact than a free one.
4. **Contamination posture**: model training cutoff relative to the instances, and whether the vendor addressed memorization.
5. **Verification integrity**: whether the patch passes hidden tests only, or was also checked against reward-hacking behaviors like editing tests (the harness prevents test edits from counting, but agents can still overfit the visible reproduction script).

The trajectory worth memorizing as orders of magnitude, not precise figures: low single digits for retrieval-plus-generation baselines in late 2023; roughly 12 to 14 percent for the first agents in early 2024; roughly 40 to 55 percent on Verified by late 2024; roughly 70 to 80 percent on Verified for frontier systems by late 2025.
Two years, one and a half orders of magnitude; that slope, more than any single number, is why Chapter 1's economic flywheel turned.

## 2. SWE-agent and the ACI insight

SWE-agent (Yang, Jimenez et al., Princeton, April 2024) was the first strong open agent on SWE-bench, reaching roughly 12.5 percent on the full set with GPT-4.
Its lasting contribution is not the score but the framing: the **agent-computer interface** (ACI).

The observation: humans use interfaces designed for human perception and motor control; an LLM has different strengths (perfect recall of text in context, no mouse) and weaknesses (no persistent visual state, limited context, no muscle memory).
Giving a model a raw terminal is like giving a human a punch card reader; the interface, not the intelligence, becomes the bottleneck.
SWE-agent therefore designed each tool around the model's ergonomics:

- A **file viewer** showing a 100-line window with explicit line numbers and a scroll position indicator, instead of cat dumping 3,000 lines into context.
- A **search tool** that returns at most 50 hits and says "too many results, narrow your query" instead of flooding context, and returns a clear "no results" instead of empty output.
- An **edit command** with built-in lint checking that rejects syntactically broken edits immediately, so the model repairs errors while the intent is still fresh in context.
- **Guardrails in feedback**: every tool failure explains what went wrong and what to try, because a model, unlike a human, cannot bang the table and open the man page.

The ablations in the paper showed these interface choices moved success rates by factors, not percents, at fixed model quality.
The ACI insight generalizes far beyond SWE-bench and is restated throughout this track: when an agent fails, suspect the interface before the model.
Claude Code's Read line numbers, Edit exact-match semantics, and Grep result modes (Chapter 2) are ACI thinking in production form.

## 3. Agentless and scaffold minimalism

Agentless (Xia et al., UIUC, mid-2024) asked a heretical question: does the agent loop earn its complexity?
It replaced the autonomous loop with a fixed three-phase pipeline with no model-directed control flow at all:

1. **Localization**, hierarchical: rank suspicious files from the repository structure and issue text, then narrow to classes and functions, then to specific edit locations.
2. **Repair**: sample many candidate patches for the localized region in a constrained diff format.
3. **Validation**: filter candidates by regression tests and a model-generated reproduction test, then select by majority behavior among survivors.

At publication it resolved roughly a third of SWE-bench Lite at about 70 cents per issue, beating or matching most contemporaneous agents at a fraction of their cost, and variants of it were briefly at the top of the leaderboard.
OpenAI chose Agentless as the harness for its own model evaluations in the SWE-bench Verified release, which tells you how credible the pipeline was as a measurement instrument.

The argument it grounds is the workflow-versus-agent doctrine of Volume 4, sharpened for coding: when the task distribution is narrow and known (single-repo bug fixes with a reproducible failing behavior), a workflow captures most of the value with better cost, latency, debuggability, and variance than an agent.
The agent earns its loop only when the path genuinely varies: multi-file features, unclear reproduction, environment fights, iterative design.

The counter-argument, which subsequent history validated: pipelines cap at their designers' imagination.
Agentless could not run the reproduction it did not think to write, could not recover from a wrong localization, and could not handle tasks outside the bug-fix shape.
As models got better at directing their own process, the agent's ceiling rose past the pipeline's, and by 2025 the leaderboard was agents again.
Both halves are true and the synthesis is the one to keep: pipelines win at fixed task shapes and fixed model quality; agents win as task variance and model quality grow.

## 4. Devin and the autonomous-engineer ambition

Devin (Cognition, March 2024) was the branding event of the agentic turn: "the first AI software engineer," demonstrated planning tasks, browsing documentation, running a shell and editor in a sandboxed workspace, and reporting roughly 13.9 percent on a 25 percent random subset of SWE-bench full, against low single digits for prior published baselines.

What it actually contributed:

- **The product form factor of delegation**: a ticket in, a PR out, with an inspectable timeline of everything the agent did; Chapter 6's async agents are all descendants of this shape.
- **The workspace abstraction**: agent-owned VM with shell, editor, and browser, rather than agent-in-your-editor; this prefigured cloud execution.
- **Proof of demand**: enterprises paid meaningful money for autonomy years before autonomy was reliable, which recalibrated every lab's roadmap.

The backlash arrived within weeks: independent reviewers replayed marketing demos (notably the Upwork task video) and showed inflated impressions of autonomy, tasks that were simpler than presented, and long wall-clock times with human-invisible flailing.
The fair synthesis as of early 2026: Devin's ambition was directionally right and roughly two years early; its scores were real but non-comparable; and the backlash taught the field that autonomy theater is discoverable and expensive, pushing serious vendors toward inspectable traces and review-gated delivery.
Cognition's later acquisition of the Windsurf IDE team (mid-2025) signaled the strategic convergence: autonomous agents and interactive tools are one product spectrum, not rival species.

## 4.5. The open ecosystem and the harness-as-research-instrument

Three other systems shaped the design space enough to be worth knowing by name.

**OpenHands** (formerly OpenDevin, 2024 onward) is the most complete open agent platform: a sandboxed runtime, a browser, a Jupyter-backed code executor, an event-stream architecture where every observation and action is a typed event, and a pluggable agent abstraction so researchers can swap the policy while keeping the environment.
Its contribution is the separation of concerns most homegrown agents get wrong - environment, event log, and agent policy as three independent components - and it became the default substrate for academic agent papers that did not want to rebuild containers and tooling.

**Aider** (Chapter 2) contributed the edit-format research nobody else published: systematic comparison of whole-file rewrites, unified diffs, and search-replace blocks across models, with per-model benchmark data showing which format each model applies correctly.
That work is the empirical backing for the exact-string-replacement convergence described in Chapter 2, and it is a reminder that output format is a first-class variable, not a detail.

**AutoCodeRover, RepoUnderstander, and the localization literature** (2024-2025) attacked navigation specifically: abstract syntax tree and call-graph search instead of text search, spectrum-based fault localization borrowed from classical debugging research, and program-analysis signals as retrieval features.
These consistently improved localization precision, and they consistently mattered less over time as context windows grew and models got better at directed search - which is the scaffold-decay thesis (section 5) arriving early in one subfield.

The meta-lesson: the open harnesses are research instruments as much as products.
When you read a paper claiming an agent improvement, check whether the gain came from the policy, the environment, or the edit format, because all three move the number and only one is usually what the paper is about.

## 4.6. Test-time compute: the other axis

Everything above treats one run as the unit.
The other lever, which the 2024-2025 leaderboards quietly ran on, is spending more compute per task at inference time.

- **Sampling and reranking**: generate n candidate patches, then choose.
Selection is the hard part - majority voting over behavior (do the candidates agree on test outcomes), a model-based reranker, or execution against generated reproduction tests.
Agentless's validation phase is exactly this, and it is why it competed with agents on quality despite a fixed pipeline.
- **Multiple independent attempts with restart**: run the whole agent several times from scratch and keep the run whose patch passes the reproduction test.
This exploits run-to-run variance, which on hard instances is large.
- **Critique and repair loops**: a second pass reviews the diff against the issue and either accepts or sends it back, with independent context so the reviewer does not inherit the author's blind spots.
- **Longer horizons within one run**: more steps, more verification cycles, higher reasoning effort.

Two rules govern when this pays.
First, **test-time compute converts verification strength into quality**: if you can cheaply and reliably tell good from bad, spending n times more generates roughly the best-of-n, and if you cannot, you have spent n times more to pick randomly.
Second, **report cost alongside score or the number is meaningless** - a system at 5 percent higher resolve rate and 10x cost is a different engineering artifact than one that got there for free, and leaderboards that hide this actively mislead.
The same arithmetic reappears at the workflow level in Chapter 6's parallel attempts, and at the harness level in Chapter 7's eval design.

## 5. The scaffold-decay thesis

The 2024 leaderboard rewarded scaffold engineering: retrieval pipelines, specialized localization phases, multi-stage voting, curated tool sets.
The 2025 evidence points the other way.

- **mini-SWE-agent** (Princeton, 2025): a deliberately minimal agent of roughly one hundred lines - bash as the only tool, no ACI machinery, linear history - scores within a few points of full SWE-agent on Verified when driven by a frontier 2025 model, at a fraction of the complexity.
- Lab model cards began reporting SWE-bench scores under trivial scaffolds to advertise the model rather than the harness, and the differences between elaborate and minimal harnesses shrank each generation.
- The mechanism is the RLVR flywheel of Chapter 1: labs train models in agentic environments, so the behaviors scaffolds existed to impose - decompose, search before editing, run the tests, recover from errors - are progressively distilled into the weights.
A scaffold is a prosthetic for missing capability, and prosthetics come off as the limb heals.

State the thesis precisely, because the sloppy version is wrong.
Scaffolds that **compensate for model weakness** (forced phase orderings, elaborate prompting rituals, voting to average out unreliability) decay: expect their value to shrink every generation, and budget their engineering accordingly.
Scaffolds that **provide capabilities models cannot have** do not decay: sandboxing and permissions (the model cannot grant itself safety), context management and compaction (the window is finite regardless of intelligence), verification infrastructure (tests are external facts), credential custody, audit logging, and parallel execution.
Chapter 2's anatomy is almost entirely the second kind, which is why Claude Code got simpler in its model-facing prompting over 2025 while its policy machinery grew.

The practical corollary for your own systems: before building clever harness logic, ask which kind it is.
If it is compensation, prefer waiting a model generation or writing the minimal version; if it is capability, build it well because it will outlive many models.

## 6. Repository navigation: grep-first versus embeddings

An agent's first real problem on any task is finding the relevant code in a repository too large for context.
Two philosophies compete.

**Embedding-based retrieval** (Cursor's codebase indexing is the canonical product example): chunk the repository, embed the chunks, and at query time vector-search for relevant code.
Strengths: one round trip, works from a natural-language description with no exact terms, effective for "where is X handled" discovery in unfamiliar code, and cheap per query once the index exists.
Weaknesses: the index is stale the moment code changes and must be maintained; chunking fractures semantic units; recall is probabilistic and failures are silent - the agent does not know what the index failed to return; and infrastructure (index storage, sync, per-repo embedding cost) is a standing tax.

**Grep-first agentic search** (Claude Code's approach): no index; the agent iteratively runs glob, grep, and targeted reads, following the same trail a senior engineer follows - find the error string, find the symbol, find its callers, read the neighborhood.
Strengths: always fresh, zero infrastructure, transparent (the transcript shows exactly what was searched), composable with the agent's reasoning (each result reshapes the next query), and exact where code demands exactness - identifiers, error strings, and signatures are literal strings, which is precisely where lexical search is strong and embeddings are fuzzy.
Weaknesses: multiple model round trips per lookup, so it spends tokens and latency; it can miss code that shares no vocabulary with the query; and it degrades in codebases with poor naming discipline, where the lexical trail is cold.

The middle path deserves its own mention: **Aider's repository map** builds a tree-sitter symbol graph of the codebase and ranks the portion included in context by relevance to the current task - structural rather than semantic indexing, cheap to keep fresh, and effective at giving the model a table of contents without retrieval infrastructure.
Language-server integration (go-to-definition and find-references as tools) is the other structural option and grew through 2025.

The economics decided the mainstream outcome: as context got cheaper and models got better at directing search, the round-trip cost of agentic search fell while its freshness and transparency advantages held, and grep-first became the default for terminal agents.
Embeddings survive where they are genuinely stronger: enormous monorepos where the lexical trail is too long, cross-repo discovery, and natural-language queries from users who do not know the vocabulary.
The design rule: prefer agentic lexical search as the baseline; add structural maps when the repo is large; add embeddings only when you can name the queries that lexical search demonstrably fails.

## 7. Long-horizon task management

SWE-bench tasks resolve in minutes; the frontier as of early 2026 is tasks that take hours - migrations, feature builds, dependency upgrades across a codebase.
Long horizons fail differently: not by writing wrong code, but by losing the plot - forgetting the goal, redoing finished work, declaring premature victory, or drifting into side quests.
The techniques that hold an agent together, all of which appear in production agents and map onto Volume 6's context-engineering principles:

- **Explicit plan state**: a todo list maintained as a tool artifact (Claude Code's TodoWrite) or a plan file on disk, so the plan survives context compaction and is re-read rather than re-remembered; marking items complete is the agent's defense against redoing work.
- **Compaction with intent preservation**: when the transcript approaches the context limit, summarize history with the goal, decisions, and current state pinned; a compaction that loses the original acceptance criteria is how agents finish the wrong task confidently.
- **Filesystem as memory**: scratch notes, decision logs, and intermediate results written to files; the filesystem is unbounded, persistent, and grep-able, and re-reading a note is cheaper and more reliable than trusting a summarized memory of it.
- **Checkpointing through git**: commit at each coherent milestone so both the agent and the reviewer get bisectable progress and cheap rollback; the commit log becomes an externalized episodic memory.
- **Subagent decomposition**: farm out self-contained investigations to keep the main context on the critical path (Chapter 2, section 7), accepting the delegation-specification cost.
- **Verification cadence**: run the test suite at milestones, not only at the end, so errors are caught while the causing change is small and attributable; this is the agent analogue of continuous integration.
- **Self-audit before completion**: an explicit final phase re-reading the original request against the diff, which measurably reduces the premature-victory failure mode.

The metric that matters for this frontier is not resolve rate but **coherence half-life**: how long an agent works before requiring human course correction.
METR's measurements of task-length capability (the time horizon of tasks completed at 50 percent reliability) showed this length doubling roughly every seven months through 2024-2025; every technique above is engineering to extend it, and Chapter 6 builds the delegation infrastructure that assumes it keeps growing.

## Exercises

1. Download three SWE-bench Verified instances, read the issue, the gold patch, and the fail-to-pass tests, and classify each as fairly specified or underspecified; for one underspecified case, write the issue text that would have made it fair.
2. Run mini-SWE-agent or an equivalent minimal harness with a current frontier model on twenty SWE-bench Lite instances, then run a full-featured agent on the same twenty; report resolve rate, cost, and wall-clock, and write a paragraph on what the gap implies about scaffold decay this generation.
3. Design the ACI for an agent that operates a SQL database: specify the query tool's result-window behavior, its too-many-rows response, its error feedback, and one guardrail, justifying each choice by a model ergonomic the way SWE-agent's paper does.
4. Implement hierarchical localization from Agentless (file ranking, then function ranking) as a standalone script over a repository you know, and measure top-5 file recall against ten historical bug-fix commits; state where the pipeline's fixed structure failed.
5. Take a repository over 100k lines and answer the same five "where is X" questions three ways: grep-only by hand, an embedding search tool, and an agent doing iterative search; score correctness and time, and write the design rule you would derive from your own data.
6. Give an agent a task you estimate at two hours of human work, with instructions to maintain a plan file and commit at milestones; afterward, audit the transcript for the exact moment coherence degraded, identify which section-7 technique was missing or failed, and rerun with that fix.

## Godhood check

You have mastered this chapter when you can:

- Explain SWE-bench's construction from PR mining to fail-to-pass and pass-to-pass semantics, name the major variants, and interrogate any reported score with the five qualifiers.
- State the ACI insight in one sentence and redesign a bad tool interface on sight, citing the specific model ergonomic each change serves.
- Argue both sides of Agentless versus SWE-agent and deliver the synthesis about task variance and model quality without hedging.
- Name the two rules governing test-time compute and explain why verification strength decides whether best-of-n is worth anything.
- Distinguish compensating scaffolds from capability scaffolds, sort any proposed harness feature into the right bin, and predict which bin decays.
- Choose a repository navigation strategy for a given codebase size, change rate, and query mix, and defend the choice with the economics rather than fashion.
- List the seven long-horizon techniques from memory and diagnose which one is missing from a failing transcript.
