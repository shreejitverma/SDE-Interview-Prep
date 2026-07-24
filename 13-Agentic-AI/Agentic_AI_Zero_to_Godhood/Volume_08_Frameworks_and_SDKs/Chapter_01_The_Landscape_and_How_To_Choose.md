# Chapter 01 - The Landscape and How To Choose

Knowledge in this chapter is current as of early 2026.
Framework versions, product names, and vendor strategies churn faster than any other layer of the agent stack, so treat specific names as dated snapshots and the selection principles as durable.

## What you will master

- A complete map of the agent framework landscape as of early 2026, organized by category rather than by hype.
- The three-way decision between raw provider APIs, an existing framework, and building your own thin layer.
- The abstraction tax: what frameworks actually cost you in debugging, prompt visibility, and upgrade churn.
- The distinct forms of lock-in and how to price each one before committing.
- A concrete evaluation checklist you can apply to any framework, including ones that do not exist yet.
- The doctrine this track teaches: start with the raw API, add a framework only when you can name the specific problem it solves.

## The map as of early 2026

The landscape sorts into six categories, and confusion usually comes from comparing tools across categories as if they competed directly.

### Category 1: provider agent SDKs

These are agent harnesses shipped by the model vendors themselves, tuned for their own models.

- Claude Agent SDK (Anthropic): the harness extracted from Claude Code, with a full agent loop, filesystem and shell tools, subagents, hooks, and MCP support; covered in Chapter 02.
- OpenAI Agents SDK (OpenAI): the production successor to the experimental Swarm library, built on agents, handoffs, guardrails, and sessions, tightly coupled to the Responses API; covered in Chapter 03.
- Google Agent Development Kit (ADK): Google's entry, integrated with Vertex AI and the A2A protocol; covered in Chapter 05.

The provider SDKs are the newest category and the fastest-moving one, because vendors realized in 2024-2025 that the harness around the model matters nearly as much as the model.
Their advantage is that the vendor co-designs the harness and the model, so model-specific behaviors like interleaved thinking, prompt caching, and server-side tools work without translation.
Their disadvantage is the obvious one: they are gravity wells pulling you toward one provider.

### Category 2: orchestration frameworks

These are provider-neutral libraries whose core abstraction is a graph, workflow, or team of agents.

- LangGraph (LangChain Inc.): explicit state machines with checkpointing, human-in-the-loop interrupts, and durable execution; the most widely deployed in this category as of early 2026; covered in Chapter 04.
- CrewAI: role-based agent teams plus a lower-level Flows API; covered in Chapter 05.
- Microsoft Agent Framework: the convergence of AutoGen and Semantic Kernel announced in late 2025, aimed at enterprise .NET and Python shops; covered in Chapter 05.
- Mastra: a TypeScript-first workflow and agent framework; covered in Chapter 05.

### Category 3: minimalist and opinionated libraries

- smolagents (Hugging Face): a deliberately tiny library centered on code-acting agents, where the model writes Python instead of emitting JSON tool calls.
- Pydantic AI: type-safe agents with dependency injection, from the team behind Pydantic.

These reject the kitchen-sink approach and bet that a small, sharp abstraction beats a large configurable one.

### Category 4: the TypeScript application layer

- Vercel AI SDK: the dominant way to put model calls and tool loops inside TypeScript web applications, with first-class streaming and UI hooks.
- Mastra straddles categories 2 and 4.

This category exists because web product teams live in TypeScript and their concerns (streaming to React, edge deployment) differ from Python backend concerns.

### Category 5: the plumbing layer

Not agent frameworks at all, but the infrastructure agents sit on.

- LiteLLM, OpenRouter, and other gateways that normalize many providers behind one API.
- instructor, outlines, and guidance for structured and constrained generation.

Covered in Chapter 06, because you will use plumbing regardless of which framework you pick, and often instead of one.

### Category 6: adjacent but not agent frameworks

- LlamaIndex: retrieval-centric, with agent features added later; the retrieval material lives in Volume 05.
- DSPy: prompt optimization as compilation; a genuinely different paradigm that belongs to evaluation and optimization discussions more than to agent orchestration.
- Haystack: pipeline-oriented NLP with agent additions, common in enterprise search deployments.

Knowing what a tool is not for is as valuable as knowing what it is for, and misclassifying these three causes real architecture mistakes.

## The three-way decision

Every agent project makes this decision, explicitly or by default.

### Option A: raw provider API

You call the Messages API or Responses API directly, write the agent loop yourself, and own every line between your code and the model.
Volume 03 taught you that the core loop is roughly a hundred lines: send messages, check for tool calls, execute tools, append results, repeat until the model stops asking.

Advantages:

- Total prompt visibility: every token that reaches the model is one you constructed.
- Zero abstraction layers to debug through when behavior is wrong.
- Immediate access to new provider features on release day, not after a framework wrapper ships.
- Minimal dependency surface, which matters for security review and supply-chain audits.

Disadvantages:

- You write and maintain the boring parts: retries, streaming assembly, tool schemas, transcript persistence, context compaction.
- Multi-provider support is entirely your problem.
- Your team invents idioms that new hires cannot look up in public docs.

### Option B: an existing framework

Advantages:

- Solved infrastructure: checkpointing, tracing hooks, session storage, streaming plumbing arrive on day one.
- Shared vocabulary: a new engineer who knows LangGraph is productive in a LangGraph codebase quickly.
- Community pressure has already found many of the sharp edges you would otherwise find in production.

Disadvantages:

- The abstraction tax, detailed below.
- The framework's opinions become your constraints, and fighting a framework is worse than not using one.
- Version churn: the agent framework ecosystem broke APIs repeatedly between 2023 and 2026, and there is no reason to believe it has stopped.

### Option C: build your own thin layer

After you have built two or three agents on raw APIs, the shared code factors naturally into a small internal library: a tool registry, a loop, hooks, persistence.
Chapter 07 builds exactly this in about three hundred lines.

Advantages:

- You keep raw-API transparency while removing raw-API repetition.
- The abstraction fits your problems exactly, because it was extracted from your problems.

Disadvantages:

- You own it forever: docs, onboarding, and the bugs.
- It is easy to drift into rebuilding LangGraph badly if you do not hold the line on scope.
- Solo maintainers leaving is a real organizational risk for internal frameworks.

The honest summary: Option A optimizes for understanding and control, Option B optimizes for time-to-first-demo and shared infrastructure, Option C optimizes for long-run fit at the cost of ownership burden.

## The abstraction tax

Every framework charges these costs, and the invoice arrives during incidents, not during the demo.

### Debugging through layers

When an agent misbehaves, the question is always "what exactly did the model see, and what exactly did it return".
With a raw API, the answer is one dictionary you constructed.
With a framework, the answer is buried under the framework's message transformation, its retry logic, its context assembly, and possibly its own injected system prompt content.
A bug hunt that takes ten minutes with raw API logs can take hours when you first have to learn how the framework assembles requests.
The severity of this tax varies: frameworks with first-class tracing (LangGraph with LangSmith, OpenAI Agents SDK with built-in traces) reduce it substantially, and frameworks without it make you pay full price.

### Prompt opacity

Frameworks inject text you did not write: tool-use instructions, formatting scaffolds, agent role preambles, handoff descriptions.
Injected prompt text is part of your product's behavior, and text you have not read is behavior you have not reviewed.
Early LangChain was the canonical offender, with agent prompts hidden in package internals; the ecosystem improved, but every framework still injects something.
The test to run on any framework: can you dump the exact final request payload for any given call in one line of configuration?
If not, walk away.

### Version churn and migration cost

Between 2023 and 2026 the field saw LangChain deprecate its original agent classes, Swarm die in favor of the Agents SDK, AutoGen fork into AG2 and then converge into Microsoft Agent Framework, and multiple 0.x libraries break APIs monthly.
Each migration costs engineering weeks and re-validation of agent behavior, because behavior is sensitive to prompt and loop changes in ways ordinary library upgrades are not.
Price this in: assume at least one significant migration per year of a framework's life in your stack.

### Dependency weight

An agent framework typically pulls in dozens of transitive dependencies.
Each is attack surface, license review, container size, and cold-start latency.
For agents that execute tools with real permissions, supply-chain risk is not theoretical; Volume 11 treats this in depth.

### Cognitive overhead

Your engineers now debug two systems: your agent and the framework.
The framework's mental model (graphs, crews, handoffs) must be learned on top of the agent loop itself, and engineers who learn the framework first often never learn the loop underneath, which cripples their debugging ability permanently.
This is the pedagogical reason this track put frameworks in Volume 08 rather than Volume 03.

## Lock-in, priced by type

Lock-in is not binary; it comes in forms with very different exit costs.

### API-surface lock-in

Your code imports the framework's types everywhere.
Exit cost is a mechanical rewrite, painful but estimable.
Mitigation: keep framework imports at the edges, keep your tools and prompts as plain functions and strings that any harness could host.

### Platform lock-in

The framework works standalone but its best features (durable execution, deployment, monitoring) live in a paid platform: LangGraph Platform, AgentKit's hosted pieces, Vertex Agent Engine.
Exit cost includes rebuilding operational infrastructure, not just code.
This is the modern open-core pattern, and it is a business-model fact, not a scandal; you must simply price it.

### Provider lock-in

Provider SDKs bind you to one model vendor's API and pricing.
Exit cost is re-tuning every prompt and re-running every eval on a new model family, which is usually larger than people estimate and smaller than vendors fear.
Note the asymmetry: provider-neutral frameworks reduce this lock-in but charge the lowest-common-denominator tax described in Chapter 06.

### Data and trace lock-in

Your eval history, traces, and fine-tuning data accumulate inside a vendor's observability product.
Exit cost grows silently over time; check export capabilities before the data exists, not after.

### Conceptual lock-in

The subtlest form: your team's architecture starts to mirror the framework's concepts, and you cannot imagine the system otherwise.
A team that thinks in crews will propose crew-shaped solutions to problems that need a simple pipeline.
The only mitigation is having built agents without the framework at least once.

## Evaluation criteria

Apply this checklist to any framework, current or future.

1. Transparency: can you log the exact request and response payloads for every model call without patching the framework?
2. Escape hatches: can you drop to the raw provider API for one call without leaving the framework?
3. Loop control: can you intercept, veto, and modify each step of the agent loop (hooks, middleware, interrupts)?
4. State model: where does conversation and agent state live, who owns persistence, and can you bring your own store?
5. Provider feature passthrough: does prompt caching, extended thinking, and server-side tool use survive the abstraction, or get silently dropped?
6. Failure behavior: what happens on tool errors, malformed model output, and rate limits, and can you change it?
7. Testing story: can you run the agent against a fake model in unit tests without network access?
8. Churn record: read the changelog for the past year and count the breaking changes.
9. Business model: what is free, what is paid, and what happens to you if the company pivots or dies?
10. Dependency surface: run a dependency audit before the proof of concept, because after it you will not.

Notice what is not on the list: GitHub stars, benchmark demos, and launch-week excitement, which are the three most common actual selection criteria and the three least predictive of long-term fit.

## The doctrine: raw API first, framework only for a reason

This track's position, stated plainly.

Build your first version of any agent on the raw provider API.
The loop is a hundred lines and you wrote it in Volume 03; the marginal cost is small and the understanding is permanent.
Adopt a framework only when you can complete the sentence "we are adopting X specifically because we need Y", where Y is a concrete capability like durable checkpointed execution, human-in-the-loop interrupts with resumability, or an enterprise-mandated platform.
"Everyone uses it" and "we might need it later" do not complete the sentence.

The reasoning behind the doctrine:

- Agent behavior debugging is prompt and transcript debugging, and frameworks obscure both by default.
- The expensive parts of agent engineering (evals, prompts, tool design, security) are not what frameworks provide; frameworks provide the cheap part.
- Migration off a bad early framework choice costs more than the weeks the framework saved.
- Understanding compounds: engineers who built the loop can adopt any framework in days, while the reverse transfer does not happen.

The doctrine's own downside, stated honestly: it is slower to first demo, it can produce in-house code that is worse than mature framework code for genuinely hard problems like durable execution, and it can calcify into not-invented-here culture if applied without judgment.
Durable execution in particular is a place where "just build it" is bad advice; correct checkpoint-and-resume with side-effect idempotency is a distributed-systems problem, not a loop feature.

### A decision table

| Situation | Recommendation |
|-----------|----------------|
| Learning, prototyping, or single-agent product | Raw API, own loop |
| Committed to one model vendor, want batteries included | That vendor's agent SDK |
| Long-running workflows needing checkpoints and human approval gates | LangGraph or equivalent durable-execution framework |
| Enterprise Microsoft estate | Microsoft Agent Framework |
| TypeScript web product with streaming UI | Vercel AI SDK, add Mastra if orchestration grows |
| Strong typing culture, Python | Pydantic AI |
| Research on code-acting agents | smolagents |
| Three or more internal agents with shared patterns | Extract your own thin layer (Chapter 07) |

## Exercises

1. Take an agent you built in Volume 03 or 04 on the raw API and write down every piece of infrastructure code in it that is not business logic; classify each piece as "a framework would provide this" or "this is specific to my problem".
2. Pick two frameworks from different categories above and run the ten-point evaluation checklist against both using only their documentation and changelogs; write a one-page comparison.
3. For one framework, find the exact mechanism (config flag, callback, environment variable) that dumps the final request payload sent to the model; if you cannot find one within thirty minutes, document what you tried.
4. Estimate, in engineering days, the cost of migrating a five-tool agent from the OpenAI Agents SDK to the Claude Agent SDK; list the assumptions your estimate depends on.
5. Write the sentence "we are adopting X specifically because we need Y" for a real or hypothetical project three times, with three different X and Y; then argue against each one.

## Godhood check

Answer these cold before moving on.

- Name the six categories of the early-2026 landscape and place any framework you know into exactly one of them.
- What are the five components of the abstraction tax, and during which phase of a project does each one bite?
- Distinguish API-surface lock-in, platform lock-in, provider lock-in, and data lock-in, with the exit cost of each.
- Why does this track teach the loop before any framework, in one sentence about debugging ability?
- Which single evaluation criterion would you check first on an unknown framework, and why is transparency the usual answer?
- Give one situation where the raw-API-first doctrine is wrong, and explain why durable execution is the standard counterexample.
