# Chapter 05 - State Machines and Graphs

## What you will master

- How to model an agentic system as a state machine or graph: states, nodes, edges, conditional routing, and cycles, and what each element corresponds to in the patterns of Chapters 01-04.
- Why LangGraph chose the graph shape, what problems that choice actually solves, and what it costs.
- Checkpointing and durable execution: persistence of state between steps, resumability after crashes, time travel, and human-in-the-loop interrupts as first-class graph operations.
- The honest comparison with plain code control flow, and the specific conditions under which the graph abstraction earns its keep.

## 1. Why control flow became the battleground

Every pattern in this volume is, mechanically, control flow around model calls: sequence (chaining), branch (routing), fan-out and join (parallelization), loop (agents, evaluator-optimizer).
General-purpose languages already express all of these, so any framework that introduces a new control-flow formalism owes you an answer to one question: what does the formalism give that `if`, `for`, and function calls do not?

The serious answers are not about expressiveness, since a Turing-complete language cannot be out-expressed.
They are about reification: when control flow is data (a graph object) rather than code, the system can inspect it, draw it, persist mid-execution state at well-defined boundaries, resume it on another machine, replay it, and modify it at runtime.
Whether those capabilities are worth the indirection is the entire debate of this chapter, and the correct answer is workload-dependent, not ideological.

## 2. Agent systems as state machines

A state machine model of an agentic system has three parts.

- **State**: the data that fully determines what happens next; for an agent this is typically the message history plus any accumulated artifacts (plan, draft, retrieved documents, tool outputs, counters like retry budgets).
- **Transitions**: functions that take state and produce updated state; a model call, a tool execution, a code gate.
- **Transition logic**: the rule choosing which transition fires next, which may be fixed (always go to step B), conditional on state (if tests failed, go to repair), or decided by a model (the agent loop's "which tool next" is transition logic delegated to the LLM).

Two modeling insights make this more than notation.

First, the workflow/agent distinction of Chapter 01 becomes precise: in a workflow the transition logic is code, and in an agent some transition logic is a model call; hybrid systems are state machines where specific transitions delegate their choice to the model, and you can point at exactly which ones.

Second, the state machine forces you to name your state, and unnamed state is where agent bugs live.
A plain-code agent accumulates implicit state in local variables, closures, and the conversation list; the state-machine discipline of declaring a state schema makes visible what the system knows, what each step may read and write, and what must survive a crash.
You can apply this discipline in plain code with a typed state object and get much of the benefit without any framework; that observation recurs in section 7.

## 3. The graph model: nodes, edges, conditional routing

The graph view generalizes the state machine transition table into an explicit directed graph.

- **Nodes** are units of work: a model call, a tool invocation, a deterministic function, or a subgraph; each node receives the current state and returns an update to it.
- **Edges** are successor relations: a normal edge says "after node A, run node B"; fan-out is multiple edges from one node, and a join node waits for all incoming branches.
- **Conditional edges** attach a routing function to a node: after the node runs, the function inspects state and returns which successor to take; this is where `if` statements live in graph form, and where an LLM router (Chapter 01) plugs in.
- **Cycles** are edges pointing backward: generate, then evaluate, then conditionally either exit or return to generate is the evaluator-optimizer pattern as a three-node cyclic graph; the agent loop itself is a two-node cycle between "call model" and "execute tools" with a conditional edge that exits when the model stops requesting tools.

Pseudocode for the shape, deliberately framework-neutral:

```
graph = Graph(state_schema={"messages": list, "draft": str, "verdict": str,
                            "rounds": int})

graph.add_node("generate", generate_fn)
graph.add_node("evaluate", evaluate_fn)

graph.add_edge(START, "generate")
graph.add_edge("generate", "evaluate")
graph.add_conditional_edge(
    "evaluate",
    lambda s: "done" if s["verdict"] == "pass" or s["rounds"] >= 3
              else "again",
    {"done": END, "again": "generate"},
)

app = graph.compile(checkpointer=persistent_store)
result = app.invoke(initial_state, thread_id="task-42")
```

Note what became explicit that plain code leaves implicit: the full state schema, every possible path through the system, and the exact boundaries (node completions) at which state is well-defined and persistable.
Note also what became worse: a five-line `while` loop is now fifteen lines of registration calls, the logic is scattered across named callbacks, and reading execution order requires reconstructing the graph in your head or rendering it.

## 4. Why LangGraph chose this shape

LangGraph (released by the LangChain team in early 2024, with steady growth through 2025-2026) is the highest-profile bet on the graph formalism, and its design rationale is documented enough to study rather than guess.

The context it was born from matters: LangChain's original `AgentExecutor` was a closed loop with fixed internals, and the most common serious complaint was inability to control what happened inside the loop (custom stopping rules, forced tool ordering, human gates mid-run, state beyond the message list).
LangGraph's answer was to expose the loop as a user-defined graph: you build the nodes and edges yourself, and the framework's value shifts from "we run the agent for you" to "we run your explicit state machine with services attached."

The services are the actual product, and each one exploits the reified graph:

- **Checkpointing**: after every node (a "super-step"), the framework persists the full state to a pluggable backend keyed by thread id; this single mechanism yields crash resumption, conversation persistence across sessions, and time travel (fork execution from any historical checkpoint with modified state), none of which plain code gets without hand-building the same machinery.
- **Human-in-the-loop interrupts**: because state is durable at node boundaries, execution can stop at a designated point, wait indefinitely (hours, days) for human input at essentially zero cost, and resume exactly where it stopped; approval gates become a graph primitive rather than a blocking thread or a hand-rolled queue.
- **Streaming and observability**: the graph runtime knows node identity and boundaries, so it can stream per-node events and token deltas, and render the execution path; traces come structured for free because structure was declared up front.
- **Deterministic replay of parallelism**: fan-out/join with defined state-merge semantics (reducers that specify how concurrent updates combine) is easy to get subtly wrong in ad hoc async code; the framework makes merge behavior declarative.

The same reasoning appears outside LangGraph, which is evidence the shape is not one team's taste: durable-workflow engines (Temporal and its relatives) reify control flow for exactly the same resumability reasons, and several agent runtimes of the 2025 era converged on graph-or-workflow cores under different vocabulary.
The general law: you reify control flow precisely when executions are long-lived, interruptible, and must survive process death; agents with human gates and multi-minute tool calls fit that description, which is why the formalism found its niche here.

The costs, which LangGraph-style designs accept knowingly:

- Indirection tax: logic fragments into callbacks wired by strings; stack traces route through the runtime; debugging shifts from stepping through code to inspecting state snapshots.
- Framework lock-in at the architecture level, not just the import level: a system designed as a graph with checkpoint-dependent interrupts does not port to plain code by mechanical translation.
- Abstraction churn risk: agent frameworks in 2023-2026 revised APIs rapidly, and the more of your control flow lives in framework vocabulary, the more you pay per revision (Volume 08 expands on framework selection).

## 5. Checkpointing and durable execution, properly

Durable execution deserves depth because it is the graph abstraction's strongest justification and the least understood by newcomers.

The core contract: execution state is persisted at every step boundary, so the effective unit of failure is one step, not the whole run.
For an agent, the difference is material in three concrete scenarios.

- **Crash and redeploy**: a 40-step coding-agent run at step 37 survives a pod eviction or a deploy; on restart, the runtime loads the last checkpoint and re-executes only the in-flight step; without durability you either rerun 37 steps (paying tokens and side effects twice) or lose the run.
- **Long waits without resources**: an approval gate that a human answers tomorrow holds only a database row, not a process, a thread, or a context window in memory.
- **Time travel as a debugging and product surface**: forking from checkpoint 12 with an edited state lets you ask "what would the agent have done had the tool returned X", which is the closest thing agent engineering has to a reproducible experiment on a nondeterministic system; several products expose the same mechanism to users as "edit and rerun from here".

What durability demands in exchange, and where the sharp edges are:

- **Determinism discipline at replay**: if recovery re-executes a step, that step's side effects must be idempotent, or effects must be recorded and skipped on replay (the Temporal-style event-sourcing solution); an agent step that sends an email is not naturally idempotent, and the framework does not absolve you of designing for exactly-once effects.
- **Serialization boundaries**: everything in state must serialize; the moment a node stashes a live client handle or an open file in state, checkpointing breaks subtly or loudly.
- **State size economics**: checkpointing full message histories every step multiplies storage and serialization cost by trajectory length; production systems checkpoint deltas or prune state, which reintroduces the context-curation problem in persistence clothing.
- **Schema migration**: checkpoints outlive code versions; resuming a week-old thread against a changed state schema or changed prompts is a real compatibility problem that plain stateless code never has.

The honest summary: durable execution converts agent runs from processes into data, with all the operational benefits and all the schema-and-migration obligations that data has.

## 6. Cycles, guards, and the shape of real agent graphs

Production agent graphs are small; the value density is in the edges, not node count.
Recurring shapes worth having in your pattern vocabulary:

- **The tool loop**: model node, tool node, conditional edge exiting when no tool calls are requested; the minimal agent, two nodes and one cycle.
- **Gated mutation**: read-only subgraph, interrupt node for approval, mutation subgraph; the plan-mode shape of Chapter 03 as a graph.
- **Retry with counter guard**: any cycle in a durable graph must carry an explicit budget in state, checked by the routing function; an unguarded cycle in a durable runtime does not crash and burn, it durably persists forever, spending money.
- **Subgraph delegation**: a node that is itself a compiled graph with its own state, the orchestrator-workers pattern with isolation enforced by the state schema rather than by convention.
- **Escalation edge**: every conditional router gets a terminal "give up and surface to human" target; graphs make the absence of this edge visible in review, which is a small real advantage of drawing the system.

The state-schema design rule that prevents most graph-system bugs: nodes should declare narrow read and write sets, and shared-everything state (every node reads and writes one blob) reproduces in graph form the same spooky action that made the plain-code agent hard to reason about, while paying the indirection tax on top.

## 7. Graphs versus plain code, without religion

The comparison, condition by condition rather than by slogan.

Plain code control flow (functions, loops, `async`, a typed state object, your language's error handling) is the right default because:

- The ladder of Chapter 01 says most systems should be simple workflows, and a prompt chain as five function calls needs no runtime to interpret it.
- Debuggers, profilers, type checkers, and code review all work natively; every graph runtime rebuilds worse versions of these.
- There is nothing a graph can express that code cannot; specifically, checkpointing at boundaries can be hand-built with a state table and an explicit step enum when you need only that one feature.

The graph abstraction earns its keep when several of these hold simultaneously:

- Runs are long-lived and must survive process death, redeploys, or infrastructure churn; durability is the killer feature, and wanting it badly is the strongest single signal to adopt a runtime rather than hand-roll.
- Human-in-the-loop pauses of unbounded duration are part of the product, not an edge case.
- You need time travel or forked replays for debugging, evals, or user-facing edit-and-rerun.
- Multiple teams touch the topology, and the drawn graph is a real coordination artifact reviewed in design discussions.
- You operate many heterogeneous agent workflows on shared infrastructure, and uniform checkpointing, streaming, and tracing across all of them amortizes the framework tax.

And the anti-signals, where adopting a graph framework is a mistake being made at scale as of early 2026:

- Short synchronous request-response flows (a routed RAG pipeline answering in seconds) gain nothing from durability and pay latency, dependency weight, and debugging indirection for it.
- A single tool-loop agent with one approval gate; fifty lines of plain code with one persisted state row is easier to own than a runtime.
- Teams adopting the framework for the drawing rather than the runtime semantics; if the graph is only documentation, draw a diagram and write code.

A middle path used by experienced teams deserves mention: write the system as plain code around an explicit, serializable state object with named steps, which keeps the state-machine discipline and leaves a cheap migration path to a durable runtime later if the workload earns it; the discipline transfers, the framework is swappable.

## 8. Claims that will rot

LangGraph's specific API vocabulary, its checkpointer backends, and its position among frameworks are early-2026 facts moving quickly; verify against current docs before building, and treat Volume 08 as the framework-comparison home.
The durable-execution contract, the idempotency and serialization obligations, and the reify-when-long-lived law are stable engineering content borrowed from a decade of workflow-engine practice and safe to build judgment on.
The claim that most agent systems do not need a graph runtime is a judgment about the early-2026 workload distribution; as agents take on longer-horizon work, the fraction of workloads that justify durability will grow, and this chapter's decision conditions, not its bottom-line ratio, are the part to keep.

## Exercises

1. Take the evaluator-optimizer loop you built for Chapter 04 and re-express it as a graph on paper: state schema, nodes, edges, conditional routing functions, and the cycle guard; identify which state fields each node reads and writes.
2. Implement the minimal two-node tool loop twice: once in plain Python (a `while` loop with a typed state dataclass) and once in a graph framework of your choice; measure lines of code, then kill each process mid-run and report what recovery takes in each version.
3. Design the checkpoint schema for a coding agent whose state includes a 200-message history: decide what is checkpointed per step versus referenced externally, estimate storage per 50-step run at both designs, and state your pruning rule.
4. An agent step calls a payment API; write the idempotency design that makes this step safe under durable-execution replay, covering the idempotency key, the effect record, and the replay-skip logic.
5. Write the one-page architecture memo arguing for or against adopting a graph runtime for a specific system you know, using only the conditions in section 7; have a colleague attack the weakest condition.

## Godhood check

You have mastered this chapter when you can:

- Translate any pattern from Chapters 01-04 into a graph (nodes, edges, conditional routing, guards) on a whiteboard without hesitation.
- Explain reification as the real content of the graph choice, and list the four services (durability, interrupts, observability, replay) it enables.
- State the durable-execution contract and its three obligations (idempotency, serializability, schema migration) with a concrete failure example for each.
- Argue both sides of graphs-versus-code for a given workload using conditions, not preferences, and name the middle path.
- Spot the unguarded cycle, the shared-everything state blob, and the missing escalation edge in someone else's agent graph within minutes of reading it.
