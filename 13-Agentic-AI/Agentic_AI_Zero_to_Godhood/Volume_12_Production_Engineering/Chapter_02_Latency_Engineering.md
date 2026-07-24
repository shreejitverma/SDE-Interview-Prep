# Chapter 02 - Latency Engineering

## What you will master

- The full anatomy of where time goes in an agent request: queueing, prefill, decode, tool execution, and orchestration overhead.
- Why time-to-first-token and completion time are different engineering problems with different levers.
- Streaming as a UX mitigation, and its limits inside multi-step agent loops.
- Model tiering, parallel tool calls, and speculative execution as structural latency wins.
- The psychology of perceived latency and how to spend engineering effort where users actually feel it.

## 1. Why agent latency is a different problem

A chatbot request is one model call.
An agent session is a chain of model calls interleaved with tool executions, so its latency is a sum over a variable-length loop, and every term in the sum has its own distribution.
This changes the engineering problem in three ways.

First, tail behavior compounds.
If each of 10 sequential model calls has a well-behaved p50 but a fat p95, the session p50 already contains several per-call tail events, because the probability that all 10 calls avoid their own tail is small.
Sequential composition means session latency is governed by per-step tails, not per-step medians, which is why per-call p95 is the number to engineer against.

Second, context grows across the loop.
Each iteration re-sends the accumulated conversation, so prefill cost rises roughly linearly with step index, and total prefill work across a session grows roughly quadratically in the number of steps (Chapter 03 does the token economics; here it costs time, not just money).
Prompt caching flattens much of this, but only if you engineer for cache hits.

Third, some terms are outside your process entirely: provider queueing, tool-side API latency, sandbox boot times.
Latency engineering for agents is therefore a systems discipline, not a model-tuning discipline.

## 2. Where the time goes

Decompose a single loop iteration end to end.
The numbers below are orders of magnitude for hosted frontier models as of early 2026; measure your own stack rather than quoting these.

### 2.1 Client and network overhead

Request serialization, TLS, and transit are typically tens of milliseconds and rarely worth optimizing, with one exception: agents that make many small model calls from a distant region pay this repeatedly, so co-locating your orchestrator with (or near) the provider's ingress region is a cheap win.

### 2.2 Provider queueing and scheduling

Hosted inference is batched: your request waits to be admitted to a batch on a GPU replica.
Under normal load this is small; under provider load spikes it becomes the dominant and most variable term, and it is invisible in your code.
You observe it as the gap between request send and first token minus expected prefill time.
Mitigations are contractual and architectural, not algorithmic: provisioned or priority capacity tiers where offered, multi-provider fallback (Chapter 06), and admission control on your side so your own spikes do not amplify provider queueing.

### 2.3 Prefill (processing the input)

Prefill computes attention over all input tokens before the first output token can be produced.
It is compute-bound and roughly linear in input length for the ranges agents live in, so a 100k-token context can add seconds of prefill on its own.
Prompt caching is the fix: when the prefix of your request matches a cached prefix, the provider skips recomputation of that prefix, cutting time-to-first-token dramatically on cache hits, often by the large majority of prefill time.
The design consequence: structure prompts so the stable parts (system prompt, tool definitions, conversation history) form an unchanging prefix and all per-step variation appends at the end, because a single early-token change invalidates the cache from that point onward.

### 2.4 Decode (generating the output)

Decode is sequential: one forward pass per output token, memory-bandwidth-bound, at a roughly constant tokens-per-second rate for a given model and load.
Output length is therefore the dominant controllable term for completion time: a 2,000-token response takes roughly ten times the decode time of a 200-token response, at any provider, on any hardware.
Bigger models decode slower; as of early 2026 the spread between a small fast model and a frontier model is commonly several-fold in tokens per second, which is a core input to the tiering decisions of section 5.
Extended thinking multiplies this term: reasoning tokens are decoded tokens, so a step that thinks for 3,000 tokens before answering pays the full decode cost of those tokens, and thinking budgets are latency budgets.

### 2.5 Tool execution

The model emits a tool call, your orchestrator runs it, and the loop cannot continue until the result returns.
Tool latency spans five orders of magnitude: an in-process lookup is microseconds, a database query is milliseconds, a third-party API is hundreds of milliseconds to seconds, a browser automation step is seconds, and a code-execution sandbox with a cold start is seconds to tens of seconds.
Because tool time is ordinary distributed-systems time, all the ordinary machinery applies: connection pooling, caching, timeouts (Chapter 04), and pre-warming sandboxes (Chapter 05).
Teams routinely spend weeks shaving model latency while a single slow tool contributes more session time than the model does; profile before optimizing.

### 2.6 Orchestration overhead

Your own framework adds time: state serialization, database writes for durability, guardrail model calls, logging.
Each item is small, but agent loops execute them per step, and a 50ms overhead on a 30-step session is 1.5 seconds of pure self-inflicted latency.
Guardrail calls deserve special attention because they are model calls: an input classifier that costs 300ms on every step should run in parallel with the main call or be reserved for steps that need it.

### 2.7 A worked profile

A representative mid-session iteration with a 40k-token context on a frontier model, warm cache, one moderate tool call:

```
network + client            ~50 ms
provider queue              ~100 ms (highly variable)
prefill (cache hit)         ~200 ms
decode (300 tokens)         ~4 s
tool execution (API call)   ~800 ms
orchestration + persistence ~100 ms
total                       ~5.3 s
```

Multiply by 10 to 30 steps and a session is one to three minutes, which frames the real problem: agents are batch jobs wearing an interactive costume, and the engineering below is about either shortening the job or changing the costume.

## 3. TTFT versus completion time

Time-to-first-token (TTFT) and time-to-completion are separate metrics with separate levers, and conflating them wastes effort.

TTFT is queueing plus prefill plus the first decode step.
It is what makes a system feel responsive, and its levers are prompt caching, shorter contexts, faster models, priority capacity, and regional placement.

Completion time is TTFT plus full decode plus, for agents, the entire remaining loop.
Its levers are output-length discipline, fewer loop iterations, parallelism, and tiering.

The product question is which one your surface actually needs.
A chat surface needs excellent TTFT and tolerates long completion if tokens stream visibly.
An agent taking actions needs neither to be instant but needs progress visibility (section 4).
An API consumed by other software cares only about completion time, and spending effort on its TTFT is waste.
Define latency SLOs per surface in these terms: for example, p95 TTFT under 1 second for chat turns, p95 completion under 2 minutes for background research sessions, and no SLO at all on intermediate step latency.

## 4. Streaming as UX mitigation

Streaming does not reduce completion time by a single millisecond; it reduces waiting, which is what users experience.
The evidence from human-computer interaction is old and stable: perceived wait grows nonlinearly with uncertainty, and feedback resets the clock.
A response that streams its first token at 800ms and finishes at 20 seconds is experienced as faster than a response that appears whole at 10 seconds, because reading proceeds in parallel with generation and the user is never staring at a blank state.

For single model calls, streaming is table stakes: consume the provider's server-sent-events stream and render incrementally.
For agent loops, streaming is harder and more important, because the dead time between visible outputs is where sessions feel broken.
The practical patterns, in increasing order of engineering cost:

- Stream the model's tokens during every step, including intermediate reasoning summaries where the provider exposes them, so the user watches the agent think rather than watching a spinner.
- Emit structured progress events at loop boundaries: which tool is being called and why, what came back, what the agent will do next, rendered as a live activity feed.
- Stream partial artifacts: show the report section by section, the code file by file, so the user can start reviewing (and can abort early, saving the remaining cost) before the session completes.
- Make long sessions interruptible mid-stream, with user input injected into the next loop iteration; the ability to redirect a wandering agent converts latency frustration into a feeling of control.

The trade-offs are real.
Streaming complicates your API surface (websockets or SSE through every layer, including queues and workers), complicates retries (you cannot transparently retry a call whose partial output the user has seen), and constrains post-processing (you cannot run a guardrail over a complete output you have already shown; you must moderate the stream incrementally or accept retraction).
Accept these costs for user-facing surfaces; skip streaming entirely for machine-to-machine calls where nobody is watching.

## 5. Model tiering

Not every step in an agent loop needs the frontier model, and the latency (and cost) spread between tiers is large enough that routing is a structural win.

The taxonomy of steps that tolerate a smaller, faster model:

- Routing and classification: deciding which workflow, tool group, or specialist handles a request is a constrained task with a small output, well inside small-model competence.
- Extraction and reformatting: pulling fields from tool output, converting formats, summarizing a document into working notes.
- Simple tool-argument construction: turning "look up order 4521" into the obvious API call.
- Guardrail checks: input and output classification runs constantly and must be cheap and fast.
- Compaction: summarizing conversation history for context management is frequent, and its quality bar is "preserve the facts," not "be brilliant."

Reserve the frontier tier for planning, complex reasoning, delicate tool sequences, and final user-facing synthesis.
As of early 2026 every major provider ships a small-fast, medium, and frontier tier (in Anthropic's lineup, Haiku, Sonnet, and Opus class models), with the small tier typically several times faster in both TTFT and decode rate, so a loop that routes its cheap steps down a tier removes seconds per iteration.

The engineering costs of tiering, stated plainly.
Every routed step is a new failure surface: the small model mis-extracts or mis-routes at some rate, and you now need per-tier evals to know that rate.
Routing logic itself adds a decision (rule-based, classifier-based, or model-based) that can be wrong, and a wrong route either wastes a frontier call or degrades quality silently.
Prompt caches are per-model, so bouncing a conversation between tiers can forfeit cache hits; the standard resolution is to keep the main loop on one model and tier only the side calls (guardrails, extraction, compaction) that do not share the main context.
Start with the two or three step types where the quality bar is obviously low, measure, and expand; do not build a general learned router on day one.

## 6. Parallelism

The agent loop is sequential by default, but real workflows contain independent work, and extracting that parallelism is the biggest completion-time lever after loop-shortening.

### 6.1 Parallel tool calls

Modern models emit multiple tool calls in a single response when the calls are independent (as of early 2026 this is standard across major providers; Anthropic's API returns multiple tool_use blocks in one assistant message).
The orchestrator should execute these concurrently and return all results together, turning three 800ms lookups into one 800ms wall-clock step.
You must prompt for it (models under-parallelize by default; an instruction like "when multiple independent lookups are needed, request them in a single turn" measurably raises parallel usage) and your executor must actually run them concurrently rather than iterating the array.
The constraint: only parallelize independent, read-safe calls; two writes with an ordering dependency must stay sequential, and your tool metadata should mark which is which rather than trusting the model to know.

### 6.2 Parallel subagents

Fan out independent subtasks (research three competitors, review four files) to concurrent subagent sessions and join the results.
Wall-clock time drops toward the slowest branch, at the price of total token cost (each branch carries its own context), a join step that must reconcile results, and harder debugging.
Use it when branches are genuinely independent and individually simple; a fan-out whose branches need to coordinate mid-flight is a distributed-systems problem you should decline (Volume 07 covers the coordination patterns).

### 6.3 Pipelining

When a session produces multiple artifacts, start downstream work on completed artifacts while upstream generation continues: begin executing the reviewed part of a plan while the model drafts the rest, or start rendering the report's first section while the second is being written.
Pipelining trades implementation complexity for latency and creates rollback obligations if a later stage invalidates an earlier one; it pays off mainly in long, structured sessions.

## 7. Speculative and eager execution

Speculation spends compute to remove waiting from the critical path: do work before you know it is needed, keep it if it was, discard it if not.

### 7.1 Eager tool prefetch

If the router or the first tokens of a streaming response make a tool call predictable, start the read-only tool call before the model finishes asking for it, and serve the result from your prefetch when the call arrives.
Restrict this to idempotent reads; speculatively executing a write is an incident generator, and this rule should be enforced by tool metadata, not convention.
The cost is wasted tool invocations on mispredictions, which matters if the tool is rate-limited or billed per call; instrument the prediction hit rate and disable prefetch for tools where it is low.

### 7.2 Speculative next-step generation

While a slow tool executes, you can start a model call for the likely next step against the predicted tool outcome, discarding it if the real outcome differs.
This resembles branch prediction in CPUs, and like branch prediction it pays only when prediction accuracy is high and the speculated work is on the critical path; the token cost of discarded branches is real money.
In practice this is worth it for a small number of high-traffic, highly-predictable transitions (for example, a search step that almost always leads to a fetch of the top result), not as a general mechanism.

### 7.3 Warm resources

The cheapest speculation is keeping resources warm: sandbox pools booted before they are needed, database connections pooled, model sessions with their prompt-cache prefix kept alive by traffic patterns or explicit cache TTL management.
Cold sandbox boot is often the single largest fixed latency in code-executing agents, and a warm pool converts tens of seconds into hundreds of milliseconds at the cost of paying for idle capacity; size the pool from arrival-rate data, as with any pool.

### 7.4 Precomputation

Move work out of the session entirely: pre-index the documents, pre-summarize the account history nightly, pre-compute the embeddings.
Every token the agent does not need to read at request time is prefill latency removed; the trade-off is staleness and the batch-pipeline machinery to manage it.

## 8. Perceived versus actual latency

Users do not experience your latency histogram; they experience moments, and engineering effort should follow the moments.

The stable findings to design against.
Sub-second feedback feels immediate; one to a few seconds feels responsive if something visibly happens; beyond roughly ten seconds attention leaves, and the interaction becomes a check-back task.
Uncertainty dominates duration: an unexplained 15-second wait feels worse than a 60-second wait with visible progress and a rough estimate.
Trajectory matters: sessions that appear to stall (progress feed frozen during a long tool call) are rated worse than uniformly slow ones, so emit heartbeat progress during long steps.
Endings dominate memory: a session that streams smoothly and then hangs for eight silent seconds before the final answer is remembered as slow; keep the last step tight and, where possible, deliver the conclusion early and the appendix after.

The design consequences, ordered by leverage.
Acknowledge instantly: something must render within a few hundred milliseconds of user action, even if it is only the acknowledgment of the request and the first progress line.
Show real progress, not spinners: named steps ("searching the order database") beat generic animation because they carry information and demonstrate competence.
Set expectations for long work: telling the user a deep-research session takes several minutes converts an interactive wait into a scheduled delivery, and delivering via notification when done removes the wait entirely.
Choose the right mode per task: below roughly ten seconds of expected completion, keep the user in an interactive flow; above roughly a minute, default to background execution with notification, because pretending a batch job is interactive gives you the worst of both.
Deliver value early: partial answers, early drafts, and top-line conclusions ahead of supporting detail all shrink perceived latency without touching actual latency.

The honest caveat: perceived-latency work has a ceiling.
No progress feed makes a 10-minute session acceptable for a task the user expected in seconds, so UX mitigation complements, and never replaces, the structural work of sections 5 through 7.

## 9. A latency engineering workflow

1. Instrument first: per-step spans for queue, prefill (via TTFT), decode (via token counts and stream timing), each tool call, and orchestration overhead, aggregated per session class (Volume 10's tracing stack gives you this).
2. Read the profile before acting: find whether session time is dominated by decode, tool time, loop count, or queueing, because each has a different fix and intuition about which is wrong more often than not.
3. Apply levers in order of typical return: cache-friendly prompt structure, output-length discipline, loop-count reduction, parallel tool calls, tiering the cheap steps, warm pools, then speculation last.
4. Fix the tails, not the median: hunt the specific causes of p95 sessions (retry storms, cold sandboxes, one slow tool, context-overflow compactions) because SLOs and user complaints both live in the tail.
5. Re-measure after every change against a fixed eval traffic set, because several of these levers (tiering, output limits, parallelism prompts) can degrade quality, and a latency win that costs task success is a loss.

## Exercises

1. Build a latency model of a 15-step agent session as a spreadsheet or script: per-step queue, prefill with and without cache hits, decode at a chosen tokens-per-second, and a mix of fast and slow tools. Compute session p50 and p95 by Monte Carlo over plausible per-term distributions, then show the effect of (a) an 80 percent cache hit rate, (b) parallelizing the three independent tool calls, and (c) moving five steps to a model with three times the decode speed.
2. Design the streaming event schema for an agent product: the event types, their payloads, and which loop boundaries emit them. Specify how a mid-session user interruption is injected into the loop and what happens to an in-flight tool call.
3. Write the routing specification for a two-tier deployment: which step types go to the small model, the eval you would run to validate each routed step type, and the fallback behavior when the small model's output fails validation.
4. Identify the speculation opportunities in a code-review agent (fetch diff, read files, run linters, comment) and state for each: the prediction signal, the hit-rate threshold at which it pays, and why it is or is not safe to speculate.
5. Take a real agent trace (or construct one) and produce the waterfall diagram of where time went, then write the one-page latency review you would present: dominant term, top three fixes, expected effect of each, and the quality risk each fix carries.

## Godhood check

You have mastered this chapter when you can decompose any agent session's latency into queue, prefill, decode, tool, and orchestration terms from a trace, and name the correct lever for each term without guessing.
You can explain why prefill and decode scale differently, why prompt-cache-friendly prompt structure is a latency feature and not just a cost feature, and why output-token count is the dominant controllable term in completion time.
You can argue when streaming, tiering, parallel tool calls, and speculation each pay for their complexity, including the quality and correctness risks each introduces.
You can design the perceived-latency surface for both an interactive copilot and a background research agent, and justify the roughly-ten-second and roughly-one-minute mode boundaries from attention behavior rather than taste.
