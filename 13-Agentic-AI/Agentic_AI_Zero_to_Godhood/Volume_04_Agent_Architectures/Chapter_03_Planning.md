# Chapter 03 - Planning

## What you will master

- The plan-then-execute architecture and how it differs from step-at-a-time reactive agents in cost, latency, and failure profile.
- Task decomposition as the load-bearing skill, and the data structures plans live in: ordered todo lists, dependency DAGs, and hierarchical task networks.
- Replanning on failure: triggers, scope, and the plan-stability versus plan-freshness trade.
- Search-based deliberation: Tree of Thoughts, LATS, and when spending tokens on exploring alternatives pays.
- LLM-Modulo and verifier-guided planning: why the planning literature insists LLMs cannot plan alone, and what the generate-test loop with sound verifiers buys.
- Plan mode in coding agents as the industrial synthesis, and a hard-nosed account of when explicit planning helps versus when it ossifies.

## 1. Why planning is a separate concern

The Chapter 01 agent loop decides one step at a time: look at history, pick the next action.
This is maximally adaptive and minimally foresighted, and its failure modes are exactly the foresight failures: taking a locally sensible first step that forecloses the right path, doing work in an order that forces rework, and losing the goal across a long trajectory (plan decay, in Chapter 02's taxonomy).

Planning inserts deliberation about the whole trajectory before or during execution.
The stable principle: planning converts trajectory-level errors, which are expensive because you discover them after spending many steps, into plan-level errors, which are cheap because you can catch them by reading a few hundred tokens.
Everything in this chapter is a different answer to three questions: when do you plan, what shape does the plan take, and who checks it.

## 2. Plan-then-execute

The simplest planning architecture separates the loop into two phases with different contracts.

```
def plan_then_execute(goal, tools):
    plan = planner_llm(goal, tools_summary(tools))   # produces step list
    results = []
    for step in plan.steps:
        outcome = executor(step, tools, context=results)
        results.append(outcome)
        if outcome.failed:
            plan = replanner_llm(goal, plan, results)   # section 4
    return synthesize(goal, results)
```

The pattern's lineage runs from classical AI planning through 2023 framework implementations (LangChain popularized a "plan-and-execute" agent, following the ReWOO and Plan-and-Solve papers) into today's coding-agent plan modes.

What separating the phases buys:

- The planner can be a stronger, more expensive model invoked once, while executors are cheaper models or plain code invoked per step; ReWOO (2023) made the cost argument explicit by showing plans with placeholder variables let you skip re-invoking the planner between steps.
- The plan is a reviewable artifact; a human or a verifier can approve it before any side effects occur, which is the single cheapest safety gate in agent engineering.
- Executors run with narrow contexts (their step plus dependencies), resisting the context bloat that degrades long single-loop runs.

What it costs:

- The plan is made under maximal ignorance, before any environment feedback exists; every fact the planner assumed and got wrong is baked into the step list.
- Rigid execution of a wrong plan is worse than no plan; hence replanning (section 4) is not optional in real deployments.
- Two prompts, two failure surfaces, and a serialization boundary between them that loses nuance ("step 3: fix the bug" carries far less than the planner knew).

Use plan-then-execute when the environment is mostly predictable from the goal description, steps are expensive or irreversible enough to justify up-front review, or cost demands cheap executors.
Prefer the reactive loop when the first observation is likely to invalidate any plan you could write, such as exploratory debugging in an unknown codebase.

## 3. Plan data structures

The plan's representation determines what the system can do with it; this is a real data-structure decision, not formatting taste.

### 3.1 Ordered todo lists

A flat sequence of steps, each with a description and a status (pending, in progress, done, skipped, failed).

This is the dominant industrial representation as of early 2026: Claude Code maintains an explicit todo list it updates as it works, and OpenAI and Google coding agents converged on visible checklist artifacts in the same era.
Its virtues are legibility (humans audit it at a glance), trivial progress tracking, and a subtle but large one: the list re-injected into context each turn acts as goal-restatement, directly countering plan decay on long trajectories.
Its limit is expressiveness: no parallelism, no dependencies beyond adjacency, and no conditionals, so it under-serves tasks with genuinely branching structure.

```
plan = [
    {"id": 1, "step": "Locate the failing test and reproduce it", "status": "done"},
    {"id": 2, "step": "Identify root cause in parser module", "status": "in_progress"},
    {"id": 3, "step": "Implement fix with regression test", "status": "pending"},
    {"id": 4, "step": "Run full test suite", "status": "pending"},
]
```

### 3.2 Dependency DAGs

Steps as nodes, dependencies as edges; anything with no unmet dependencies is runnable, and independent branches run in parallel.

This is the representation behind LLMCompiler-style systems (2023 era) that plan tool calls as a dataflow graph for parallel execution, behind orchestrator-workers decompositions (Chapter 01), and behind multi-agent task dispatch (Volume 07).
The wins are parallel speedup and explicit dependency reasoning; the costs are that models generate invalid graphs (cycles, missing edges, false independence claims), so you need code-level validation (topological sort or reject), and that humans review graphs much more slowly than lists.
Adopt a DAG only when parallelism or dependency-correctness is worth that overhead; a linear list is a degenerate DAG, and most tasks are nearly linear.

```
plan = {
  "fetch_schema":   {"deps": []},
  "fetch_examples": {"deps": []},
  "draft_queries":  {"deps": ["fetch_schema", "fetch_examples"]},
  "run_queries":    {"deps": ["draft_queries"]},
  "write_report":   {"deps": ["run_queries"]},
}
```

### 3.3 Hierarchical plans

Goals decompose into subgoals recursively, and only leaves are executable; classical AI called this hierarchical task network planning.
In LLM systems this appears as outline-then-expand (plan the chapters, then plan each chapter's sections) and as orchestrators whose workers are themselves planners.
Hierarchy is how you keep any single planning context small, at the price of coherence risk across branches: sibling subplans made independently can conflict, and only a synthesis or review step catches it.

A practical rule that covers most cases: represent plans as flat todo lists by default, upgrade to a DAG only when you will actually execute in parallel, and add hierarchy only when a single plan no longer fits in one review.

## 4. Replanning on failure

A plan meets reality one step at a time, and the replanning policy decides what happens when reality wins.

Triggers, from cheap to expensive:

- A step fails mechanically (tool error, test failure) after in-step retries are exhausted.
- A step succeeds but its outcome contradicts a plan assumption (the file the plan says to edit does not exist).
- Drift detection: periodic comparison of progress against plan, catching the slow failures no single step triggers.
- Human interrupt: the user redirects mid-run, which in interactive agents is the most common trigger of all.

Scope, the more consequential decision:

- **Local repair**: regenerate only the failed step and its dependents, keeping the rest; cheap and stable, but blind to the possibility that the failure falsifies the whole approach.
- **Full replan**: rebuild the plan from the goal plus everything learned; maximally adaptive, but expensive, and it discards plan stability, which matters when humans approved or are tracking the plan.
- **Escalate**: some failures should end autonomy, not trigger cleverness; a plan that fails twice on the same step is evidence about the goal or environment, and the correct move is often surfacing that evidence to a human.

The trade to hold in your head is stability versus freshness.
Replan too eagerly and the agent thrashes: each minor surprise rewrites the plan, no step sequence survives long enough to accomplish anything, and any human approval of the plan becomes meaningless.
Replan too reluctantly and the agent grinds through a falsified plan, producing confident, coherent, wrong work.
A robust default used in practice: local repair on mechanical failures, full replan only when an assumption is explicitly contradicted, escalate on the second full replan.

## 5. Search-based deliberation: Tree of Thoughts and LATS

The architectures so far commit to one plan and repair it; search-based approaches explore alternatives before committing.

**Tree of Thoughts** (Yao et al., May 2023) generalizes chain-of-thought into search: nodes are partial solutions ("thoughts"), the model proposes multiple children per node, a model-based evaluation scores or votes on states, and classical search (breadth-first, depth-first, with pruning) explores the tree.
Its showcase tasks were ones where a greedy left-to-right generation predictably fails and where partial states are checkable: the Game of 24, creative writing under constraints, mini crosswords; on Game of 24 the paper reported success rates far above chain-of-thought (roughly an order of magnitude better for GPT-4 at the time).
The honest reading of ToT for practitioners: it is a token-for-reliability trade with multiplicative cost (branching factor times depth times evaluation calls), it requires a meaningful state evaluator to beat sampling, and its published wins are on puzzle-shaped tasks with verifiable intermediate states rather than open-ended work.

**LATS** (Language Agent Tree Search, Zhou et al., 2023) extends the idea from reasoning to acting: Monte-Carlo-style tree search over thought-action trajectories, using real environment feedback and self-reflection as the value signal.
It matters conceptually as the maximal point on the deliberation spectrum: single trajectory (ReAct) to single plan (plan-then-execute) to many plans (ToT) to many executed trajectories (LATS), with cost rising steeply at each step.

Two developments since bound the practical relevance of explicit tree search, as of early 2026:

- Reasoning models trained with reinforcement learning internalized much of the explore-evaluate-backtrack behavior inside the thinking channel; the o1 and R1 lineages visibly try approaches, check, and revise within one call, which captures a large share of ToT's benefit at far lower orchestration complexity.
- Where explicit search survives industrially, it is almost always in the presence of a cheap external verifier: best-of-n sampling against a test suite, or search over candidate patches ranked by execution results, which is verifier-guided generation (section 6) more than tree search proper.

Reach for explicit search when all three hold: single-pass failure rate is high, intermediate or final states are cheaply checkable, and per-task value absorbs a 10x-100x token multiplier.
Otherwise let the model's internal deliberation carry the load.

## 6. LLM-Modulo and verifier-guided planning

A parallel research thread, associated most strongly with Subbarao Kambhampati's group, spent 2022-2024 documenting that LLMs prompted to produce formal plans (Blocksworld and other PDDL-style benchmarks) perform poorly, that apparent planning competence often reflects retrieval of familiar patterns, and that self-critique does not fix it because the model is no better as a verifier of plans than as a generator of them.
The position paper title states the stance: "LLMs Can't Plan, But Can Help Planning in LLM-Modulo Frameworks" (2024).

The **LLM-Modulo** proposal: use the LLM as a generator of candidate plans and plan critiques inside a loop where sound external critics hold the correctness authority.
Critics can be formal (a PDDL validator like VAL, a model checker, a simulator) or hard-coded domain checks; the LLM proposes, critics verify and produce error feedback, the LLM repairs, and only critic-approved plans execute.
This is generate-and-test with the LLM on the generate side only, and its guarantee is exactly the guarantee of the weakest critic that must approve.

Why this matters beyond the academic dispute:

- It is the intellectual justification for the most reliable pattern in applied agent engineering: pair the model with ground truth.
  Test suites for coding agents, type checkers, schema validators, simulators for robotics, dry-run modes for infrastructure changes are all LLM-Modulo critics in production clothing.
- It names the failure of pure self-correction precisely (Chapter 04 takes this further): a generator checking its own work with the same faculties that produced the errors adds correlation, not verification.
- It gives you the design question to ask of any planning system: who verifies, and is the verifier sound?
  If the answer is "another LLM with the same blind spots", you have improved vibes, not reliability.

The caveat the critique itself carries: the strong negative results were measured on formal planning benchmarks with older models, and reasoning-model generations (2024-2025) improved measurably on such tasks; but the architectural lesson, that soundness must come from outside the generator, does not depend on the exact capability level and is the part to keep.

## 7. Plan mode in coding agents

Coding agents are where planning theory met the largest user base, and the synthesis they converged on is instructive.

Claude Code's plan mode (2025 era) is a harness-level state: the agent is restricted to read-only exploration, produces an explicit plan, presents it for approval, and only after approval enters execution with mutation rights.
Other coding agents of the same generation converged on close analogues (proposed-plan review steps, checklist artifacts that persist across the session).
Note the ingredients, each traceable to a section above:

- Plan-then-execute with a human as the plan verifier (section 2 plus LLM-Modulo's insistence on an external critic, with the human as the sound critic of intent).
- The plan as a todo-list artifact that persists, is updated as steps complete, and is re-shown, countering plan decay (section 3.1).
- Replanning as a first-class interaction: the user edits or rejects the plan, and mid-run failures surface as plan updates rather than silent thrashing (section 4).
- A permission asymmetry that makes planning cheap and execution gated: reading is safe and free, writing requires the approved plan, which operationalizes "trajectory errors are expensive, plan errors are cheap".

Two practical observations from this class of systems, current as of early 2026:

- Plan quality is dominated by exploration quality; time the agent spends reading the codebase before planning correlates with plan usefulness far more than planner prompt sophistication does, which is the acting-to-reason synergy of Chapter 02 restated.
- Users approve plans they barely read; the human gate degrades into rubber-stamping (Chapter 04's theme) unless plans are short, concrete, and highlight the risky steps, so plan presentation is a legitimate engineering surface, not cosmetics.

## 8. When explicit planning helps, and when it ossifies

The synthesis, stated as conditions rather than slogans.

Explicit planning earns its cost when:

- Steps are expensive, slow, or irreversible, so plan-level review has real option value; database migrations, infrastructure changes, and long compute jobs qualify.
- The task has known structure the model reliably articulates; decomposition is the easy part and execution the hard part.
- Multiple parties (humans, subagents) must coordinate; the plan is the coordination contract, and a DAG plan literally is the dispatch schedule.
- Trajectories are long enough that goal restatement fights context decay; the checklist pays for itself purely as memory.
- An external verifier for plans exists; then planning plus verification converts an unreliable generator into a bounded-error system.

Explicit planning ossifies, and you should resist it, when:

- The first observation predictably invalidates any a priori plan; exploratory debugging and open-ended research planned in detail up front produce theater, then thrashing.
- The plan becomes a commitment device against learning; agents demonstrably exhibit plan-following bias, continuing a written plan past contradicting evidence, and the more elaborate the plan, the stronger the pull.
- Planning granularity outruns knowledge: fifteen-step plans written in ignorance contain fabricated specifics, each a chance to be confidently wrong; plans should get vaguer where knowledge ends ("investigate X, then decide between A and B") rather than pretend precision.
- The task fits in one model pass; planning a task the model completes reliably in one call is pure overhead, the ladder of Chapter 01 in miniature.

The default that falls out for general-purpose agents: plan lightly and adaptively.
A short, coarse todo list that is cheap to make, cheap to revise, and continuously visible; heavy machinery (DAGs, search, formal verifiers) reserved for the tasks whose structure and stakes justify it.

## 9. Claims that will rot

Benchmark-flavored claims (LLM performance on Blocksworld-style planning, ToT's Game of 24 margins) are frozen observations from the 2023-2024 literature on models of that era; capability on such tasks has improved since and will keep moving.
Product descriptions (Claude Code plan mode, checklist behaviors of specific coding agents) are early-2026 snapshots of fast-moving harnesses.
The stable content is the architecture space (react, plan-execute, search, generate-verify), the data-structure trade-offs, the replanning policy space, and the principle that soundness must come from outside the generator; build your judgment on those.

## Exercises

1. Take a real multi-step task you performed recently and write it three times: as a todo list, as a dependency DAG, and as a two-level hierarchy; identify which representation exposed a parallelism or ordering fact the others hid.
2. Implement plan-then-execute with local repair for a toy environment (e.g. file operations in a sandbox directory); inject a failure at step 3 of 5 and verify your replanner repairs locally without rewriting completed steps, then inject a contradiction of a plan assumption and verify it triggers a full replan.
3. Specify the replanning policy (triggers, scope, escalation) for an agent that manages cloud infrastructure changes; justify each choice by naming the incident it prevents.
4. Build a minimal verifier-guided planner for SQL: the LLM proposes a query plan, a critic checks it by running EXPLAIN and schema validation, and only critic-approved queries execute; measure how many generator errors the critic catches over 20 tasks.
5. Design the plan-presentation format for a coding agent's approval gate under the constraint that users spend under 15 seconds reading; decide what to surface, what to fold, and how to mark irreversible steps, and defend the design against the rubber-stamping failure.
6. Argue, in one page, whether an agent doing exploratory data analysis should have a plan mode; take a position and steelman the opposite one.

## Godhood check

You have mastered this chapter when you can:

- Place react-only, plan-then-execute, tree search, and generate-verify on a single deliberation-cost spectrum and name the task property that selects each.
- Choose a plan data structure for a described task in under a minute and state what the chosen structure cannot express.
- Write a replanning policy from scratch, including the escalation rule, and explain the stability-freshness trade it navigates.
- Summarize the LLM-Modulo position fairly, including its evidence, its caveats, and the production patterns that embody it.
- Diagnose a failing planning agent as thrashing, ossified, over-granular, or under-verified from its traces alone.
