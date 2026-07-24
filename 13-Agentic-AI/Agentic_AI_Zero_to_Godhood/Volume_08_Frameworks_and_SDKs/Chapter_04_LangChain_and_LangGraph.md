# Chapter 04 - LangChain and LangGraph

Knowledge in this chapter is current as of early 2026.
The LangChain ecosystem has reinvented itself more times than any other project in this volume, so this chapter is as much a history lesson as an API tour; the history is what makes the current design intelligible.

## What you will master

- LangChain's history: the 2022-2023 boom, the abstraction backlash, and the pivot that produced LangGraph.
- LCEL, what problem it solved, and why the ecosystem moved past it for agents.
- LangGraph's core model: graph state machines, reducers, checkpointing, interrupts, and durable execution.
- The commercial layer: LangGraph Platform and LangSmith, and how the open-core boundary runs.
- The 1.0 era: what stabilized in late 2025 and what the create_agent abstraction signals.
- An honest account of the criticism, what was fixed, what remains, and exactly where LangGraph is the right answer.

## History: boom, backlash, pivot

LangChain launched in October 2022, weeks before ChatGPT, and became the fastest-adopted library of the first LLM wave.
Its original offer: chains (prebuilt sequences like retrieval-then-answer), a zoo of integrations (every model, vector store, and loader), and early agent executors built on the ReAct pattern from Volume 04.
For a field with no idioms yet, prebuilt everything was exactly what thousands of teams wanted, and integrations remain LangChain's most defensible asset to this day.

The backlash arrived through 2023 and is worth studying because it is the abstraction tax from Chapter 01 documented in public.
The recurring complaints: too many layers of indirection for what was ultimately string assembly and an API call, prompts buried inside package internals so users did not know what their own app said to the model, unstable APIs breaking monthly, and abstractions that made simple things easy but hard things harder.
Several high-profile engineering blogs described removing LangChain and shrinking their code while gaining control; whatever you think of the specifics, the pattern (framework great at demo scale, painful at production scale) became the cautionary tale of the era.

LangChain Inc. responded with three moves rather than denial.
First, LCEL (mid 2023) replaced opaque chain classes with explicit composition.
Second, the package was split (early 2024) into langchain-core, provider packages, and community integrations, taming the dependency sprawl.
Third, and decisively, LangGraph (announced January 2024) abandoned the "agent as a black-box executor" model entirely in favor of explicit state machines, which is a public admission that the original agent abstraction was wrong.
Judge the ecosystem by LangGraph, not by 2023 LangChain; but remember the history when evaluating any young framework's claims.

## LCEL in one section

LangChain Expression Language composes components with a pipe operator into a runnable pipeline.

```python
# Python, LangChain LCEL, shape current since 2023.
chain = prompt | model | output_parser
result = chain.invoke({"topic": "checkpointing"})
```

Every runnable gets invoke, batch, stream, and async variants for free, which standardized streaming and parallelism across the ecosystem.
LCEL is genuinely good at what it is for: linear or branching data-flow pipelines known at authoring time, such as retrieval pipelines.
It is wrong for agents, because an agent loop is cyclic (model, tools, model again, until done) and LCEL pipelines are acyclic by construction.
That gap between DAG composition and cyclic control flow is precisely the hole LangGraph fills; by the 1.0 era, LangChain's own guidance steers agent work to LangGraph and treats heavy LCEL chaining as legacy style.

## LangGraph: the core model

LangGraph models an agent as a state machine: a typed state schema, nodes that are functions transforming state, and edges (fixed or conditional) deciding what runs next, with cycles allowed.

```python
# Python, LangGraph, shape current as of 2025.
from typing import Annotated, TypedDict
from langgraph.graph import StateGraph, START, END
from langgraph.graph.message import add_messages

class State(TypedDict):
    messages: Annotated[list, add_messages]

def call_model(state: State):
    return {"messages": [llm_with_tools.invoke(state["messages"])]}

def should_continue(state: State):
    last = state["messages"][-1]
    return "tools" if last.tool_calls else END

graph = StateGraph(State)
graph.add_node("model", call_model)
graph.add_node("tools", tool_node)
graph.add_edge(START, "model")
graph.add_conditional_edges("model", should_continue)
graph.add_edge("tools", "model")
app = graph.compile()
```

The load-bearing ideas:

- State is explicit and typed, and every node's output is merged into it by reducers; the Annotated add_messages reducer, for example, appends rather than overwrites, which is how conversation history accumulates without nodes knowing about each other.
- Control flow is data: the graph is an inspectable, drawable object, so "what can happen next" is a property you can read rather than behavior you must infer from prompts.
- The model still makes decisions, but only at points you designed (conditional edges reading model output), which is the workflow-versus-agent dial from Volume 04 made concrete: you choose per-edge how much autonomy to grant.

The cost of explicitness is ceremony: the hundred-line raw loop becomes schema, nodes, edges, and compilation, and simple agents feel over-engineered.
LangGraph's own answer is a prebuilt ReAct-style agent constructor for the simple case, with the graph API underneath when you outgrow it.

## Checkpointing and durable execution

This is the feature that justifies LangGraph's existence, so slow down here.
A checkpointer persists the full graph state after every step (super-step) to a backing store: in-memory for tests, SQLite for local work, Postgres for production.

```python
# Python, LangGraph, shape current as of 2025.
from langgraph.checkpoint.postgres import PostgresSaver

app = graph.compile(checkpointer=checkpointer)
config = {"configurable": {"thread_id": "case-8841"}}
app.invoke({"messages": [("user", "Start the refund process.")]}, config)
```

What checkpointing buys, each a distinct capability:

- Threads: state is stored per thread id, so multi-turn memory across process restarts is free.
- Fault tolerance: a crash mid-workflow resumes from the last checkpoint instead of restarting, which is what "durable execution" means; for a twenty-step agent where step fifteen fails, this is the difference between a retry and a re-bill.
- Human-in-the-loop: because state is durable, the graph can pause indefinitely; an interrupt inside a node stops execution, a human reviews hours or days later, and execution resumes with their input via a Command, with no process waiting in the meantime.
- Time travel: past checkpoints can be inspected, and execution can be forked from any historical state, which turns "what if the agent had chosen differently" from speculation into a runnable experiment.

The engineering fine print, which is why Chapter 01 warned against hand-rolling this: resuming replays or re-enters work after the last checkpoint, so side effects need idempotency; state must be serializable, which constrains what you put in it; and checkpoint storage becomes real operational load (retention, migration, size) at scale.
Durable execution is a distributed-systems contract, not a convenience flag, and LangGraph's implementation of it is the strongest argument for adopting the framework.

## Streaming, subgraphs, and multi-agent shapes

LangGraph streams at several granularities: state updates per node, token streams from inside nodes, and custom events, which UIs need to show progress on long runs.
Graphs compose: a compiled graph can be a node in a parent graph, giving you subagents with their own internal state.
The multi-agent patterns from Volume 07 map directly: supervisor topologies are a router node with conditional edges to specialist nodes, swarm-style handoffs exist as a prebuilt library, and hierarchical teams are subgraphs; LangGraph does not pick an orchestration ideology, it gives you the graph and lets you encode one.

## The commercial layer: LangSmith and LangGraph Platform

LangSmith is the observability and eval product: tracing for any LLM app (LangChain-based or not), datasets, LLM-as-judge evals, and prompt management; it is close to vendor-neutral in what it ingests and is the company's actual moat.
LangGraph Platform is managed deployment for graphs: a runtime exposing your graph as an API with persistence, task queues, cron, and horizontal scaling handled, plus Studio, a visual debugger over threads and time travel.
The open-core line as of early 2026: the LangGraph library and its checkpointers are MIT open source and self-hostable, while the managed runtime, Studio, and LangSmith are commercial.
Priced in Chapter 01 terms: adopting the library is API-surface lock-in you can reason about, adopting the platform is platform lock-in, and pouring traces into LangSmith is data lock-in; each is a separate decision, and conflating them is how teams overpay.

## The 1.0 era

In October 2025, langchain 1.0 and langgraph 1.0 shipped, with the company signaling API stability after three years of churn.
The headline changes: langchain 1.0 was slimmed around a create_agent entry point that runs on the LangGraph runtime underneath, legacy chains and agent executors moved out to a legacy package, and content-block message formats standardized access to reasoning, citations, and multimodal outputs across providers.
The signal to read: the original LangChain surface is now effectively a facade over LangGraph, completing the pivot that began in 2024; the company's strategic center is graphs plus LangSmith, not chains.
Version 1.0 promises matter because churn was the ecosystem's worst tax, but a stability promise is evidence, not proof; check the changelog record since 1.0 before believing it fully.

## Honest criticism and where it shines

What remains true in the criticism as of early 2026:

- Documentation sprawl: three generations of APIs coexist in search results, and the top result for a LangChain question is frequently deprecated; budget real time for doc archaeology.
- Abstraction depth: even LangGraph carries LangChain message types, reducers, and configuration conventions underneath, and debugging still sometimes means reading framework source.
- Ceremony for simple cases: a single-agent tool loop is more code and more concepts in LangGraph than with a raw API, and teams that will never need checkpointing pay graph tax for nothing.
- Ecosystem gravity: the integration zoo tempts you to adopt LangChain wrappers for things (a vector store client, an API call) that are two lines of plain Python.

Where it genuinely shines, and where this track recommends it without hedging:

- Long-running, stateful workflows where crashing and restarting from zero is unacceptable.
- Human-in-the-loop approval gates with pauses measured in hours or days.
- Regulated or high-stakes flows that need replayable, auditable execution history.
- Complex orchestration whose control flow you want visible as a graph rather than latent in prompts.
- Teams that need LangSmith-grade tracing and evals regardless of which agent library they use; LangSmith adoption does not require LangChain adoption.

The compressed verdict: LangGraph is the strongest durable-execution and human-in-the-loop story in open source as of early 2026, and it charges the highest ceremony and doc-archaeology tax in this volume; adopt it when you need what checkpointing buys, and not before.

## Exercises

1. Build the tool-loop graph from this chapter's code sketch on the raw LangGraph API, then rebuild the same behavior with the prebuilt agent constructor, and diff the two in lines of code and in what you can inspect.
2. Add a Postgres or SQLite checkpointer, kill the process mid-run between two tool calls, and resume the thread; document exactly what re-executed and what did not, and derive the idempotency rule your tools must follow.
3. Implement a human approval gate: interrupt before a "send_email" node, resume once with approval and once with an edit to the draft in state, and confirm both paths.
4. Use time travel to fork a finished thread from the checkpoint before its final decision, inject a different tool result, and compare outcomes.
5. Reproduce one classic 2023-era criticism: pick any simple two-step pipeline, write it in LCEL and in plain Python, and present both to a colleague for a debuggability verdict.
6. Instrument a non-LangChain agent (your Volume 03 loop) with LangSmith tracing, proving to yourself that the observability layer is separable from the framework.

## Godhood check

Answer these cold before moving on.

- Reconstruct the timeline: what did 2023's backlash attack, and which three moves answered it?
- Why is LCEL structurally incapable of expressing an agent, in one sentence about cycles?
- Name the four distinct capabilities checkpointing buys and the operational contract (idempotency, serializability) it demands in return.
- How does a conditional edge implement the workflow-versus-agent autonomy dial from Volume 04?
- Where exactly does the open-source boundary run between LangGraph, LangGraph Platform, and LangSmith, and which lock-in type does each represent?
- State the compressed verdict: the one workload class where LangGraph is the default answer, and the one tax you accept for it.
