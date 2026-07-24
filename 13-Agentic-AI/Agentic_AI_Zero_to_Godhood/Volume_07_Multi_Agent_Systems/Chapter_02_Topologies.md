# Chapter 02 - Topologies

## What you will master

- The six recurring multi-agent topologies: orchestrator-workers, hierarchical trees, pipelines, peer/debate, swarm/handoff, and blackboard.
- The coordination cost and communication cost profile of each topology, and why these costs, not capability, usually decide the winner.
- How to map topology to task structure: dependency graphs, read/write mix, and the shape of the deliverable.
- Why production systems as of early 2026 overwhelmingly converge on shallow orchestrator-workers, and what would have to change for the others to win.

## 1. Topology is a cost model, not an org chart

A topology answers three questions: who decides what work exists, who talks to whom, and where state lives.
Each answer has a cost.
Deciding costs tokens in the decider's context (planning, delegation, monitoring).
Talking costs tokens at both ends plus information loss at every summarization boundary.
State placement determines who must re-read what, and how much of the system a single failure can corrupt.

Evaluate every topology by summing these costs against the task's actual dependency structure.
A topology that mirrors the task graph pays the minimum; a topology that fights it pays coordination overhead in tokens and reliability.
Two general laws hold across all of them.
First, communication paths scale badly: full peer-to-peer among N agents has N(N-1)/2 potential channels, which is why unconstrained "group chats" degrade fast beyond three or four agents.
Second, every edge in the communication graph is a lossy compression step, so deep or chatty topologies suffer telephone-game degradation (Chapter 03).

## 2. Orchestrator-workers

### 2.1 Shape

One orchestrator agent owns the goal, decomposes it into subtasks, spawns worker agents with clean contexts, collects their results, and synthesizes the deliverable.
Workers do not talk to each other; the communication graph is a star.
Workers are typically disposable: created for one subtask, terminated after returning a result.

```text
            +-> worker A (clean context) --+
orchestrator+-> worker B (clean context) --+-> orchestrator synthesis -> result
            +-> worker C (clean context) --+
```

### 2.2 Costs

Coordination cost is concentrated in the orchestrator: it pays for planning, for writing precise subtask specs, and for holding all results during synthesis.
Communication cost is 2N edges (spec out, result in) with exactly one summarization boundary per worker, the minimum possible for parallel work.
The orchestrator context is the scaling bottleneck: N results must fit alongside the plan, which caps practical fan-out or forces results into external artifacts.

### 2.3 When it wins and fails

It wins when subtasks are independent and results compose by aggregation: research, parallel evaluation, search, batch analysis.
Anthropic's research system is exactly this shape, with the empirically observed detail that the hard engineering is in the orchestrator's delegation prompts: early versions spawned duplicate workers because subtask specs like "research the semiconductor shortage" were too vague, so specs had to state objective, output format, tool guidance, and effort budget.
It fails when subtasks are secretly coupled, because workers cannot see each other's decisions; the topology has no channel for them even to detect the conflict.
It also fails when the orchestrator under-invests in synthesis, rubber-stamping worker outputs into an incoherent whole.

## 3. Hierarchical trees

### 3.1 Shape

Orchestrator-workers, recursively: workers themselves spawn sub-workers.
Depth greater than two.

### 3.2 Costs

Each level adds a summarization boundary, so information reaching the root has been compressed d times at depth d.
Coordination cost multiplies: every internal node pays orchestrator costs.
Failure attribution gets hard: a wrong answer at the root may originate at any leaf, filtered through multiple summaries.
Latency compounds because each level's fan-out waits on the slowest child.

### 3.3 When it wins and fails

Legitimate use: tasks whose natural structure is a tree with genuinely independent branches, such as researching an industry by sector, then by company, where leaf detail truly does not need to cross branches.
Also legitimate: capping fan-out per node when a flat star would exceed the orchestrator's context (a two-level tree as sharded aggregation, the map-reduce shape).
It fails as a default because most tasks are not trees, and because the double compression destroys the cross-branch details that synthesis needs.
Practical guidance as of early 2026: production systems rarely go beyond depth two, and frameworks like Claude Code deliberately prevent subagents from spawning their own subagents, which forces the topology to stay shallow and keeps traces debuggable.
The downside of the cap is that a leaf that discovers it needs decomposition must return and ask the root, adding a round trip.

## 4. Pipelines

### 4.1 Shape

Agents in sequence, each consuming the previous agent's output artifact: spec -> plan -> implement -> test -> review.
The communication graph is a path; state is the artifact flowing along it.

### 4.2 Costs

No parallelism within one item, so wall-clock time is the sum of stages, though throughput parallelism across items is natural (stage k works on item i+1 while stage k+1 works on item i).
Communication cost is one boundary per stage, but the boundaries are load-bearing: everything a later stage needs must survive every earlier boundary.
Reliability follows the sequential compounding law from Chapter 01, and a mid-pipeline error contaminates everything downstream.
Coordination cost is low: the topology itself encodes the plan, and no agent needs to reason about delegation.

### 4.3 When it wins and fails

It wins when the task genuinely is a staged transformation with different context requirements per stage: ingest, transform, verify, publish.
It maps to the "workflow" side of the workflow-vs-agent distinction from Volume 04: use a pipeline when the decomposition is known in advance and stable, because fixed structure is cheaper and more predictable than an orchestrator re-deriving it per task.
It fails when stages need to iterate with each other; a linear pipeline cannot send work backward without bolting on loops, at which point you are building a state machine and should use an explicit graph framework (LangGraph-style, Volume 08) rather than pretending linearity.
The classic failure is the waterfall trap: ChatDev-style software-company pipelines inherit the known weaknesses of waterfall development, because misunderstandings in the spec stage are discovered only at the test stage, after every intermediate stage has amplified them (Chapter 07).

## 5. Peer and debate

### 5.1 Shape

Multiple agents with symmetric standing exchange messages: proposals, critiques, votes.
Variants include multi-agent debate (agents argue toward a judged answer), group chat (AutoGen-style, a shared conversation with a speaker-selection policy), and society-of-mind role-play.

### 5.2 Costs

Communication cost is the worst of any topology: rounds times agents times message length, all appended to every participant's context.
Coordination is emergent rather than owned, which means termination, decision authority, and tie-breaking all need explicit policy or the conversation wanders.
Known failure dynamics include sycophantic convergence (agents agree with the majority rather than the evidence), degeneration into repeated restatement, and shared blind spots when all agents are the same base model, which undermines the independence assumption that makes debate attractive in theory.

### 5.3 When it wins and fails

The honest reading of the research as of early 2026: debate can improve accuracy on tasks with a verifiable answer, but much of the gain is explained by spending more inference and sampling diverse chains, and self-consistency voting over independent samples often matches it at lower complexity.
Where peer review earns its place in production is the narrow two-agent form: a generator and an adversarial critic with a fresh context, one or two rounds, explicit rubric, hard termination.
That form is really a verification pattern (Chapter 04) rather than open-ended collaboration.
Full peer collaboration among three or more agents on open-ended work has essentially no production track record worth copying, and its cost profile explains why.

## 6. Swarm and handoff

### 6.1 Shape

One conversation, many possible operators: control of the ongoing interaction is handed from agent to agent, each with its own instructions and tool subset.
The user-facing state (the conversation) is continuous; the active policy changes.
This is the model popularized by OpenAI's experimental Swarm library (late 2024) and productionized in the OpenAI Agents SDK's handoffs (2025); a triage agent routes to a refunds agent, which may hand off to a human-escalation agent.

### 6.2 Costs

Communication cost is near zero because agents share the conversation history rather than summarizing to each other; a handoff is a pointer transfer, not a message.
The cost shows up as context accumulation: every agent inherits the full history, relevant or not, so long multi-hop sessions carry growing baggage.
Coordination cost is the routing decision itself, and the failure modes are routing loops (A hands to B hands back to A), handoffs that drop tool state, and responsibility gaps where every agent believes another agent owns the request.

### 6.3 When it wins and fails

It wins for conversational products whose surface is genuinely modal: support flows, sales flows, tiered escalation, where each mode needs different tools and guardrails and where sharing full history is a feature, not a leak.
It is better understood as dynamic prompt-and-toolset switching within one logical agent than as multi-agent collaboration, and designing it that way (small agent count, explicit routing table, loop limits) keeps it reliable.
It fails for task-parallel work, since there is only one thread of control, and it fails when agent count grows: each added specialist multiplies routing surface, and misroutes are the dominant observed error class.

## 7. Blackboard

### 7.1 Shape

Agents do not address each other at all; they read from and write to a shared workspace (the blackboard), and a control component decides which agent runs next based on blackboard state.
The pattern predates LLMs by decades: the Hearsay-II speech understanding system (1970s) coined it, with independent knowledge sources opportunistically contributing partial hypotheses.

### 7.2 Costs

Communication cost is centralized into the shared store: no pairwise channels, no telephone game, and late-joining agents get full state by reading.
The costs move into contention and coherence: concurrent writers can clobber or contradict each other, so you need write discipline (single writer per section, append-only logs, or explicit locking), and every agent pays to re-read a growing blackboard unless you structure it for selective reads.
Control becomes its own design problem: something must decide activation order, and that something is either a fixed scheduler (cheap, rigid) or an LLM controller (flexible, another fallible agent).

### 7.3 When it wins and fails

The modern incarnation is mundane and effective: the filesystem as blackboard.
Parallel coding agents sharing a repo, a plan file, and a task queue directory are a blackboard system, with git as the coherence mechanism (Chapter 03).
The pattern wins when contributions are incremental and opportunistic, when agents genuinely need each other's partial results, and when you want to add or remove agents without rewiring channels.
It fails without write discipline, and it degrades the "clean context" benefit: agents that read the whole blackboard re-import the very clutter that context isolation was meant to avoid, so selective reading conventions matter as much as the store itself.

## 8. Mapping topology to task structure

The decision procedure: draw the task's dependency graph first, then pick the topology that matches its shape.

- Independent subtasks, results compose by aggregation: orchestrator-workers.
  This covers parallelizable research, batch evaluation, and search, and it is the default topology for read-heavy work.
- Independent subtasks but too many for one context: two-level tree (sharded aggregation), never deeper without a specific argument.
- Known, stable, staged transformation: pipeline, implemented as a workflow rather than free agents.
- Sequential decisions with dense coupling, such as most coding on one change: single agent, per Chapter 01; the correct multi-agent move is read-only helper subagents, not parallel writers.
- Coding across genuinely separate changes: fleet of independent single agents, one worktree each, which is orchestrator-workers at the task level (Chapter 07).
- Verification of any of the above: attach a generator-critic pair; two agents, fresh critic context, fixed rounds.
- Modal conversation with distinct tool/guardrail regimes: swarm/handoff with a small routing table.
- Many opportunistic contributors over a long-lived shared artifact: blackboard with single-writer discipline per region.

Two meta-rules govern all mappings.
Prefer the shallowest topology that fits, because every added level or channel is a summarization boundary and a failure surface.
Prefer static structure (workflow, routing table, fixed pipeline) over dynamic structure (LLM-decided delegation) wherever the task shape is known in advance, because static structure costs no planning tokens and cannot hallucinate a bad decomposition; spend dynamic flexibility only on the parts of the task that are actually unpredictable.
The downside of shallow-and-static is reduced adaptability on novel task shapes, which is exactly where you escalate to an orchestrator that plans.

## 9. Worked example

Task: "produce a competitive analysis of five vendors, with a recommendation".
Dependency graph: five independent research branches (read-heavy), one synthesis (write, global coherence), one review.
Topology: orchestrator-workers with fan-out five for research; the orchestrator itself writes the synthesis in one context to keep the recommendation coherent; a single critic agent with a fresh context reviews the draft against a rubric; no tree, no debate, no pipeline.
Cost sketch: five parallel worker runs dominate token spend; the star topology gives five summarization boundaries; the single-writer synthesis avoids merge incoherence; the critic adds one bounded round.

Counter-example: "refactor the authentication module and update its callers".
Dependency graph: one densely coupled write cluster; the callers depend on interface decisions made during the refactor.
Topology: single agent, optionally spawning read-only search subagents to find callers; any parallel-writer topology here fights the task graph and pays for it in merge conflicts and inconsistent interfaces.

## Exercises

1. For each of the six topologies, write down its communication graph, count the summarization boundaries on the path from raw evidence to final answer, and rank them by expected information loss.
2. Design a two-level tree for "summarize the year's activity across 40 repositories" that respects a 50k-token orchestrator budget; specify the per-node fan-out and the result schema at each level.
3. Take an AutoGen-style group chat transcript (or generate one among three agents on a planning task) and annotate every message as new-information, restatement, or social-glue; compute the ratio and compare against a two-agent generator-critic run on the same task.
4. Implement a minimal handoff router in Python: three system prompts, a routing function returning the next agent id, a loop limit, and a test that provokes and then catches a routing loop.
5. Build a filesystem blackboard for two concurrent agents (a producer appending findings, a consumer compiling a digest) and demonstrate one write-collision failure and one discipline (append-only files plus a single compiled output owner) that eliminates it.

## Godhood check

- For each of the six topologies, state in one sentence where its coordination cost lives and where its communication cost lives.
- Explain why worker-to-worker channels are absent in orchestrator-workers and what class of failure this makes undetectable.
- Explain why production frameworks forbid subagents from spawning subagents, and give the one legitimate reason to use depth two anyway.
- Given "peer debate improved accuracy in paper X", name the two confounds you would check before crediting the topology.
- State why handoff systems are best understood as one logical agent, and name the dominant error class as specialist count grows.
- Describe the modern blackboard incarnation and the write discipline that keeps it coherent.
- Recite the two meta-rules for topology selection and the downside of each.
