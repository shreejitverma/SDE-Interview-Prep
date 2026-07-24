# Chapter 03 - OpenAI Agents SDK

Knowledge in this chapter is current as of early 2026.
OpenAI reorganized its agent product surface twice in two years (Assistants API to Responses API, Swarm to Agents SDK, then the AgentKit umbrella), so expect further renames and verify shapes against current docs.

## What you will master

- The lineage from Swarm to the Agents SDK, and why the intermediate experiment mattered.
- The four core primitives: agents, handoffs, guardrails, and sessions, and the philosophy of keeping the primitive set tiny.
- The Responses API and its built-in server-side tools: web search, file search, computer use, and code execution.
- Built-in tracing and why OpenAI made observability a default rather than an add-on.
- The AgentKit context of late 2025: Agent Builder, ChatKit, and evals, and how the SDK fits inside it.
- When this SDK is the right choice and where its multi-agent model strains.

## Lineage: Swarm was the thesis, the SDK is the product

In October 2024 OpenAI released Swarm, an explicitly experimental, education-only Python library.
Swarm's thesis was radical minimalism: an agent is a model plus instructions plus tools, and multi-agent coordination is just one agent handing the conversation to another.
No graphs, no message buses, no roles, no crews; a handoff was simply a tool call that returned another agent.
Swarm was tiny, stateless between calls, and pointedly not for production.

In March 2025 OpenAI shipped the Agents SDK as Swarm's production successor, keeping the thesis and adding what production required: sessions, guardrails, tracing, structured outputs, and provider flexibility.
The lineage matters because it explains the SDK's design center: it is the least-abstracted mainstream framework, betting that models are now smart enough that heavy orchestration scaffolding is mostly legacy compensation for weaker models.
That bet is the exact opposite of LangGraph's bet, and holding both in your head is the point of this volume.

The SDK is Python-first with a TypeScript sibling (published under the openai scope as an agents package) that mirrors the same primitives.

## Primitive 1: agents

An agent is a model configuration: name, instructions, model, tools, and optionally typed output.

```python
# Python, OpenAI Agents SDK, shape current as of 2025.
from agents import Agent, Runner, function_tool

@function_tool
def get_weather(city: str) -> str:
    """Return current weather for a city."""
    return fetch_weather(city)

agent = Agent(
    name="Assistant",
    instructions="You are a concise assistant.",
    model="gpt-5",
    tools=[get_weather],
)

result = Runner.run_sync(agent, "What is the weather in Pune?")
print(result.final_output)
```

Notable design choices:

- function_tool builds the JSON schema from the Python signature and docstring, so the type hints you write for humans are the contract the model sees; lying type hints produce lying schemas.
- The Runner owns the loop (call model, run tools, repeat), with a max_turns cap; the agent object is pure configuration, which makes agents cheap to define, compose, and test.
- output_type accepts a Pydantic model, turning the final answer into validated structured data rather than prose, which is how you make agents composable with ordinary software.
- Instructions can be a function of runtime context, enabling per-user or per-tenant prompts without defining new agents.

The SDK also supports non-OpenAI models through LiteLLM integration, which is genuinely useful and also clearly a secondary path; the polish and the built-in tools assume OpenAI models.

## Primitive 2: handoffs

A handoff transfers control of the conversation to another agent, implemented as a tool the model can call.

```python
# Python, OpenAI Agents SDK, shape current as of 2025.
from agents import Agent, handoff

billing = Agent(name="Billing", instructions="Handle billing questions.")
refunds = Agent(name="Refunds", instructions="Handle refund requests.")

triage = Agent(
    name="Triage",
    instructions="Route the user to the right specialist.",
    handoffs=[billing, handoff(refunds)],
)
```

Semantics worth being precise about:

- A handoff is a transfer, not a delegation: the receiving agent takes over the conversation and, by default, sees the prior history; contrast this with Claude Agent SDK subagents, which get a fresh context and return a result to a parent that stays in charge.
- Because the receiving agent inherits history, handoffs preserve conversational continuity (good for support flows) and propagate context pollution (bad when the history is long or noisy); input filters exist to prune what the next agent sees.
- The model decides when to hand off, which means routing quality is prompt quality; the SDK exposes a recommended prompt prefix that teaches the model how handoffs work.

Handoffs are the SDK's whole answer to multi-agent orchestration, plus the ability to wrap an agent as a callable tool for hierarchical patterns.
The honesty required here: this covers routing and specialist patterns cleanly, and it does not natively express arbitrary graphs, fan-out with joins, or cyclic workflows, for which teams either write ordinary Python control flow around Runner calls (the SDK's official advice) or pick a graph framework.

## Primitive 3: guardrails

Guardrails are validation functions that run alongside the agent: input guardrails check the user's message, output guardrails check the agent's answer, and a tripwire aborts the run with an exception when validation fails.
A common pattern runs a small fast model as a classifier inside the guardrail while the expensive main agent starts in parallel, so a jailbreak or off-topic request is killed cheaply before the big model burns many tokens.

```python
# Python, OpenAI Agents SDK, illustrative shape.
from agents import Agent, GuardrailFunctionOutput, input_guardrail

@input_guardrail
async def block_homework(ctx, agent, user_input):
    verdict = await classify(user_input)
    return GuardrailFunctionOutput(
        output_info=verdict,
        tripwire_triggered=verdict.is_homework,
    )
```

The design position: safety checks are first-class citizens of the run, not middleware you remember to add.
The limitation to keep in view, ahead of Volume 11: guardrails as implemented are policy checks on inputs and outputs, and they do not sandbox tool execution; a guardrail that approves a message does nothing about what a tool later does with real permissions.

## Primitive 4: sessions

Early Swarm was stateless and you carried history yourself; the SDK added sessions as pluggable conversation memory.
A session object (SQLite-backed in the box, with other backends available) stores history under a session id, and the Runner automatically prepends it and appends new turns.

```python
# Python, OpenAI Agents SDK, shape current as of 2025.
from agents import Agent, Runner, SQLiteSession

session = SQLiteSession("user_123")
result = Runner.run_sync(agent, "My name is Shreejit.", session=session)
result = Runner.run_sync(agent, "What is my name?", session=session)
```

This is short-term conversational memory in Volume 06's taxonomy; long-term memory across sessions remains your problem.
The trade-off of automatic history management is the usual one: convenience now, and unbounded context growth later unless you prune, summarize, or filter, which the session abstraction lets you do but does not do for you.

## The Responses API and built-in tools

The Agents SDK sits on the Responses API, which OpenAI introduced in March 2025 as the successor to Chat Completions and the Assistants API for agentic use.
The Responses API's distinguishing feature is server-side tools: capabilities that execute inside OpenAI's infrastructure during the model's turn, without a round trip to your code.

- Web search: the model searches and reads current web content, returning citations.
- File search: managed retrieval over vector stores you upload, which is hosted RAG in Volume 05's terms.
- Computer use: a model-driven mouse-and-keyboard loop for operating browsers and desktops, surfaced in the SDK as a computer tool with your code supplying the environment.
- Code interpreter: sandboxed code execution hosted by OpenAI.

Why server-side tools matter architecturally: each one deletes a subsystem you would otherwise build (search integration, retrieval pipeline, sandbox), collapses multi-round-trip loops into one API call, and moves that capability's cost, latency, limits, and data handling onto OpenAI's terms.
This is the deepest form of provider lock-in in the chapter: migrating an agent that leans on file search and web search means rebuilding retrieval and search from parts, not just changing an API client.
The Responses API also maintains server-side conversation state (previous response chaining), which reduces token resending but places your transcript on the provider's side, with the data-governance implications Volume 12 examines.

## Tracing

The SDK traces every run by default: agent spans, model calls, tool executions, handoffs, and guardrail decisions, viewable in OpenAI's dashboard, with hooks for exporting to third-party observability backends.
Making tracing opt-out rather than opt-in was a lesson learned from the ecosystem's first two years, in which nobody could answer "what did the model actually see" during incidents; Volume 10 builds on this.
The flip side: default tracing to a vendor dashboard is telemetry flowing to the vendor, and regulated environments need to configure or disable it deliberately rather than discover it later.

## The AgentKit context of late 2025

At DevDay in October 2025, OpenAI wrapped its agent stack under the AgentKit umbrella:

- Agent Builder: a visual canvas for composing multi-step agent workflows, targeting faster iteration and non-specialist builders.
- ChatKit: embeddable, customizable chat UI components for shipping agent frontends.
- Expanded evals: datasets, trace grading, automated prompt optimization, tied into the same platform.
- Connector infrastructure for hooking agents to data sources, alongside MCP support in the Responses API.

Read the strategy plainly: the SDK is the code-level layer of a vertically integrated agent platform spanning model, tools, orchestration, UI, and evaluation.
That integration is genuinely productive, and it is the platform lock-in pattern from Chapter 01 executed deliberately; the pieces are designed to be better together, which is another way of saying they are designed to be hard to leave.

## When it is the right choice

Choose the OpenAI Agents SDK when:

- You are committed to OpenAI models and want the shortest path from idea to a traced, guarded, multi-agent app.
- Your multi-agent needs fit routing and specialist handoffs, which is most support, triage, and copilot workloads.
- Server-side web search, file search, or computer use replaces subsystems you would rather not build.
- You value a primitive set small enough to hold in your head; this is the framework closest in spirit to this track's build-it-yourself doctrine.

Prefer something else when:

- You need durable, checkpointed, resumable long-running workflows with human approval gates; the SDK's run model is not a durable execution engine, and LangGraph or temporal-style infrastructure fits better.
- You need provider portability as a hard requirement; the LiteLLM path works but you lose the built-in tools that justify the SDK.
- Your orchestration is graph-shaped with fan-out, joins, and cycles; you will end up writing the graph in ad hoc Python around the Runner anyway.
- Your agent lives on a real machine with filesystem and shell as its primary environment, where the Claude Agent SDK's harness is the stronger starting point.

The honest downside summary: it is the cleanest mainstream SDK design as of early 2026, its simplicity is real but partly transfers complexity to your surrounding Python code, and its gravitational pull toward the OpenAI platform is the strongest in the field precisely because the integrated pieces are good.

## Exercises

1. Build a three-agent triage system (triage, billing, refunds) with handoffs, and log the full item history of a run; identify the exact item where control transferred and what history the second agent saw.
2. Reimplement the same triage behavior as a single agent with a routing tool and ordinary if-else in your code; compare debuggability, latency, and token cost against the handoff version, and write down which you would ship.
3. Add an input guardrail using a small fast model to block off-topic requests, and measure how much it adds to latency for allowed requests versus how much it saves for blocked ones.
4. Use file search over a handful of your own documents, then design on paper the migration plan to self-hosted retrieval; list every capability you would need to rebuild.
5. Run the same agent with a SQLiteSession across twenty turns and plot per-turn input token counts; then add a history filter and show the effect.
6. Wire the SDK's tracing to a non-OpenAI observability backend and verify a complete trace of a guarded, multi-handoff run appears there.

## Godhood check

Answer these cold before moving on.

- What was Swarm's thesis, and which framework in this volume represents the opposite bet?
- Name the four primitives and state precisely what each one owns.
- Explain handoff-as-transfer versus subagent-as-delegation, including who holds the conversation and what context each pattern propagates.
- What do server-side tools delete from your architecture, and what do they add to your exit cost?
- Why are guardrails not a sandbox, and which volume's material closes that gap?
- Give the two workload shapes where this SDK strains, and what you would reach for in each case.
