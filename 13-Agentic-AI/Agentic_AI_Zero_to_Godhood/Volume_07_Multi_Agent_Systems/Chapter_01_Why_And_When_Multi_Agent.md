# Chapter 01 - Why and When Multi-Agent

## What you will master

- The two real technical wins of multi-agent systems: context isolation and parallelism.
- Why the "team of AI employees" framing is a marketing metaphor that actively misleads system designers.
- The token cost multipliers of multi-agent architectures and how to reason about when the spend pays for itself.
- How errors compound across agents and why coordination adds failure modes that single agents do not have.
- The Cognition "Don't Build Multi-Agents" argument, the Anthropic multi-agent research system, and the task-structure lens that reconciles them.
- A decision rubric you can apply to any task before reaching for multiple agents.

## 1. Strip the metaphor first

The phrase "multi-agent system" imports fifty years of connotations from distributed AI, organizational theory, and science fiction.
Most of those connotations are wrong for LLM agents, and designs built on them fail in predictable ways.

An LLM agent is a loop: a model, a context window, and a set of tools, iterating until a stop condition.
A "multi-agent system" is therefore nothing more mystical than multiple such loops whose contexts are (partially or fully) separate, plus some mechanism for moving information between them.
That is the entire ontology.
There is no manager with judgment, no employee with initiative, no team with shared understanding.
There are context windows, token streams, and the plumbing you build between them.

This deflationary framing matters because it changes what questions you ask.
The anthropomorphic framing asks "what roles should my team have?"
The technical framing asks "what is the optimal partitioning of context, and which parts of this task can proceed without knowledge of each other?"
The second question has answers you can verify; the first produces org charts of prompts that look impressive and perform poorly.
The downside of the deflationary framing is that role names ("planner", "reviewer") are still useful shorthand for prompt specialization, so you keep the vocabulary while refusing the metaphysics.

## 2. The two real wins

As of early 2026, after several years of production multi-agent systems, the honest accounting is that multi-agent architectures deliver exactly two fundamental benefits.
Everything else claimed for them is either a special case of these two or is achievable more cheaply with a single agent.

### 2.1 Context isolation

A single agent doing a large task accumulates everything into one context window: the plan, every tool result, every dead end, every intermediate artifact.
This causes three problems covered in depth in Volume 06.
First, context windows are finite, and long tasks exhaust them, forcing lossy compaction.
Second, model attention degrades on long contexts; irrelevant history distracts from the current step, a failure mode often called context rot or context distraction.
Third, cost per step grows with context length, because every step re-reads the whole prefix (mitigated but not eliminated by prompt caching).

Spawning a subagent gives a subtask a clean context containing only what that subtask needs.
The subagent can burn fifty tool calls and two hundred thousand tokens exploring, then return a five hundred token summary.
The parent pays attention cost only for the summary.
This is context isolation: the subagent acts as a compression function from a large working set to a small result.
It is the single most defensible reason to use more than one agent, and note that it does not require any "team" - a single orchestrator spawning disposable workers captures it fully.

The trade-off is information loss at the boundary.
The summary is lossy by construction, and if the parent later needs a detail the subagent discarded, that detail is gone unless the subagent externalized it to a file or store.
Chapter 03 treats this boundary problem in detail.

### 2.2 Parallelism

A single agent is sequential: one context, one token stream, one action at a time.
When a task decomposes into independent subtasks, N subagents can execute them concurrently, cutting wall-clock time by up to a factor of N.
Anthropic reported that parallelizing search across subagents, each also issuing parallel tool calls, cut research time for complex queries by up to 90% in their multi-agent research system (published mid-2025).

Parallelism only pays when the subtasks are actually independent.
"Independent" here means: the correct execution of subtask A does not depend on decisions made during subtask B.
Breadth-first research over separate sources is independent.
Editing two functions that share an interface is not, even though the files differ.
Misjudging independence is the root cause of most multi-agent coding failures, as Chapter 06 documents.

### 2.3 What is not a fundamental win

Specialization ("a security expert agent, a performance expert agent") is usually achievable with one agent and conditional prompting, or one agent invoked serially with different system prompts.
Splitting prompts across agents only becomes necessary when the specialized contexts are too large to coexist or the work should run in parallel, which reduces specialization back to isolation and parallelism.
Robustness through redundancy (multiple agents voting) is real but is better understood as sampling the same model several times, which does not require an agent architecture at all.
"Emergent collaboration" between peer agents has, as of early 2026, essentially no evidence of outperforming a well-engineered orchestrator on production tasks, and the debate-style results from research settings are mixed.

## 3. The cost side of the ledger

### 3.1 Token multipliers

Anthropic published concrete multipliers from their production data (mid-2025): agents use roughly 4x the tokens of chat interactions, and multi-agent systems use roughly 15x the tokens of chat.
The 15x number is worth internalizing.
A multi-agent architecture is a decision to spend an order of magnitude more money per task.
The same analysis found that in their research eval, token usage by itself explained the large majority of performance variance, meaning much of the multi-agent gain is "spend more inference on the problem" rather than architecture magic.
Three factors together (token budget, tool call count, model choice) explained most of the variance.

This gives you the honest framing: multi-agent is one mechanism for scaling test-time compute, competing with alternatives like longer single-agent runs, best-of-N sampling, and reasoning models with extended thinking.
Choose it when its specific shape (parallel clean contexts) fits the task, not because more agents feel more powerful.

### 3.2 Error compounding

Let p be the per-step success probability of an agent step.
A sequential chain of n dependent steps succeeds with probability roughly p^n under an independence assumption.
At p = 0.95 and n = 20, that is about 0.36.
Real systems do better than the naive formula because agents recover from errors, but the qualitative point stands: reliability decays with dependent step count.

Multi-agent systems change this calculus in both directions.
Parallel independent subtasks do not compound with each other, which helps.
But every handoff is itself a step that can fail: the parent can mis-specify the subtask, the child can misinterpret it, the summary can drop the crucial fact, and the synthesis can mis-merge results.
Coordination is not free reliability plumbing; it is additional surface area for the same class of errors, executed by the same fallible models.
A multi-agent system with poor interfaces is strictly less reliable than the single agent it replaced.

### 3.3 Latency and operational cost

Parallel fan-out reduces wall-clock latency but multiplies concurrent load, rate-limit pressure, and burst cost.
Multi-agent traces are harder to debug: a failure surfaces in the synthesis but originates three subagents and one summary boundary earlier.
Stateful multi-agent systems need checkpointing, retry semantics, and deployment strategies for long-running work; Anthropic described using rainbow deployments to avoid breaking in-flight agents when updating prompts.
Budget engineering time for observability (Volume 10) before you scale out agents.

## 4. The 2025 argument: Cognition vs Anthropic

Two influential essays published within weeks of each other in mid-2025 appeared to give opposite advice, and reconciling them is the best single exercise for understanding this volume.

### 4.1 Cognition: "Don't Build Multi-Agents"

Cognition (the Devin company) argued from their experience with long-running coding agents.
Their core principles: share context, and share full agent traces rather than summarized messages; and recognize that every action carries implicit decisions, so parallel agents acting on partial views make conflicting implicit decisions that poison the final result.
Their canonical failure example: split "build a Flappy Bird clone" into two parallel subtasks (background, bird), and the two subagents make incompatible visual-style decisions that no synthesis step can cleanly merge.
Their recommendation: a single-threaded linear agent, with context compression by a dedicated summarization model when tasks exceed the window, in preference to parallel multi-agent decomposition.
The essay explicitly framed 2025-era multi-agent orchestration as fragile and premature for their domain.

### 4.2 Anthropic: "How we built our multi-agent research system"

Anthropic described the production system behind their Research feature: a lead orchestrator agent that plans, spawns parallel search subagents, and synthesizes their findings, with a citation pass at the end.
They reported that the multi-agent system (Opus-class lead with Sonnet-class subagents, model generation of mid-2025) outperformed a single-agent baseline by 90.2% on their internal research eval.
They were candid about the cost: roughly 15x chat-level token usage, meaning the architecture only makes sense for tasks whose value supports that spend.
They also stated plainly that some domains requiring shared context and many inter-dependent decisions, such as most coding tasks, are a poor fit for parallel multi-agent decomposition as of mid-2025.

### 4.3 The reconciliation: read-write task structure

The two essays are not in conflict once you classify tasks by their dependency structure.

Research is read-heavy and embarrassingly parallel at the subtask level.
Subagents read from independent sources, subtask results are facts that compose by union, and conflicts between findings are themselves useful signal for the synthesizer.
Lossy summaries are acceptable because the deliverable is itself a summary.

Coding is write-heavy with a densely connected dependency graph.
Every edit embeds implicit decisions (naming, interfaces, style, architecture) that other edits must be consistent with.
Parallel agents with partial views make divergent implicit decisions, and merging divergent decisions is often harder than doing the work once.
Lossy summaries are unacceptable because the "detail" dropped may be the exact interface another agent needs.

So the rule: parallelize reads, serialize writes, and when you must parallelize writes, partition the write surface so that no two agents touch overlapping decisions, and make the shared interfaces explicit before fan-out (Chapter 03 and Chapter 07 cover the mechanics, including git worktrees).
Cognition is right for tightly coupled write-heavy work; Anthropic is right for decomposable read-heavy work; both said as much in their own texts, and the apparent controversy was mostly headline compression.

Date-stamp on this reconciliation: it reflects the state of models and tooling as of early 2026.
As models get better at following detailed specifications, the minimum viable interface for parallel write work shrinks, and the coupled-task boundary will keep moving.

## 5. A decision rubric

Apply these questions in order before adopting a multi-agent design.

1. Can a single agent with good context engineering (compaction, external memory, sub-task files) do this task within budget?
   If yes, stop; single agents are cheaper, more debuggable, and more reliable per step.
2. Does the task exceed what one context window can hold even with compaction, or does it involve heavy exploration whose intermediate state the final answer does not need?
   If yes, context isolation via subagents is justified even without parallelism.
3. Does the task decompose into subtasks that are independent in the decision sense, not merely the file sense?
   If yes, parallel fan-out is justified; if you cannot write down the subtask interfaces precisely, the answer is no.
4. Is the task read-heavy (research, review, evaluation, search) or write-heavy (code, documents with global coherence)?
   Read-heavy tolerates lossy boundaries; write-heavy demands explicit shared interfaces or serialization.
5. Does the value of the task support roughly an order of magnitude more token spend than a single-agent attempt?
   If not, spend the tokens on a better single-agent attempt (stronger model, more thinking, more retries).
6. Can you evaluate the end result automatically or with an LLM judge?
   Multi-agent systems are nondeterministic in more dimensions; without an eval you cannot tell whether the architecture helps (Volume 10).
7. Do you have tracing that attributes a bad final answer to the responsible subagent and handoff?
   If not, build that first; you will need it within the first week.

A useful summary heuristic from the field: the number of agents should be a consequence of the task's structure, never a design goal.
Systems designed as "a team of seven agents" almost always contain five agents' worth of ceremony and two agents' worth of work.

## 6. What multi-agent systems look like when they work

To ground the rest of the volume, here is the shape of the systems that demonstrably pay their way as of early 2026.

- An orchestrator spawning parallel, disposable, clean-context research workers, with structured result payloads and a synthesis step (Anthropic Research, most deep-research products; Chapter 07).
- A primary coding agent spawning read-only exploration subagents to search a large codebase, keeping its own context clean for the write work (Claude Code subagents; Chapter 04).
- A generator agent plus an independent verifier agent with a fresh context, exploiting the generator-verifier gap: checking is cheaper and less biased when the checker did not produce the work (Chapter 04).
- Fleets of independent coding agents on fully independent tasks (different tickets, different worktrees), which is parallelism across tasks rather than within one (Chapter 07).

Notice what is absent: free-form peer collaboration, agents debating at length, deep hierarchies of managers.
Those appear in research papers and demos; the production wins are shallow topologies with narrow, explicit interfaces.

## Exercises

1. Take a task you recently did with a single agent that required more than 30 tool calls.
   Partition its steps into "reads" and "writes", draw the decision-dependency graph, and determine the maximum safe fan-out.
2. Compute the naive reliability of a 12-step sequential pipeline at per-step success rates of 0.90, 0.95, and 0.99, then explain two mechanisms real agents use to beat the naive number.
3. Write a one-page adversarial critique of the Anthropic 90.2% improvement figure: list every methodological question you would ask before accepting it as evidence for multi-agent architectures in general rather than for token scaling on research tasks.
4. Reproduce Cognition's Flappy Bird failure in miniature: give two parallel LLM calls each half of a two-part creative task with no shared style spec, then repeat with an explicit shared spec, and compare coherence of the merged result.
5. Apply the seven-question rubric to three tasks: "summarize 200 customer interviews", "migrate a service from REST to gRPC", "monitor 50 RSS feeds and file daily digests".
   Justify a single-agent or multi-agent verdict for each.

## Godhood check

You are ready for the rest of this volume if you can answer these cold.

- State the only two fundamental benefits of multi-agent architectures and explain why specialization is not a third.
- Quote the approximate token multipliers Anthropic reported for agents vs chat and multi-agent vs chat, and explain what most of the multi-agent performance gain was attributable to.
- Explain the difference between file-level independence and decision-level independence, with a coding example where they diverge.
- Summarize Cognition's two core principles and construct the task classification that makes their advice and Anthropic's simultaneously correct.
- Given a task description, walk the seven-question rubric out loud and defend a verdict, including the token-economics question.
- Explain why every handoff between agents should be counted as a fallible step when estimating system reliability.
