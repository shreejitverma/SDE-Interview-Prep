# Chapter 03 - Cost Engineering

## What you will master

- The token economics of agent loops, and why context accumulation makes session cost grow roughly quadratically in turns.
- Prompt caching as the single biggest cost lever, and how to structure prompts and traffic to earn cache hits.
- Model routing by task difficulty, output-token discipline, and batch APIs as the remaining structural levers.
- Cost observability: attributing spend to sessions, users, and features so cost is an engineering metric rather than a monthly surprise.
- Unit economics for agent products: framing cost per successful task and designing pricing that survives heavy users.

## 1. Why agents are expensive in a specific way

LLM APIs price per token, with input tokens and output tokens priced separately and output tokens costing several times more per token than input tokens at every major provider as of early 2026.
Exact prices change frequently and differ per model tier, so this chapter reasons in mechanisms and ratios; look up current numbers when you need them, and never hard-code them into intuition.

Three structural facts make agents expensive relative to chatbots of similar apparent activity.

First, the loop multiplies calls: a single user request can trigger tens of model calls, and each call is billed in full.

Second, context accumulates: every loop iteration re-sends the system prompt, tool definitions, and the entire conversation so far, so late calls in a session are far larger than early ones.

Third, agents read a lot: tool results (search pages, file contents, API responses) enter the context as input tokens, and a session that reads twenty documents pays for every token of every document, possibly on every subsequent turn.

The result is that agent cost is dominated by input tokens in accumulated context, which surprises teams whose intuition was trained on chat products where output tokens dominate.
This is also why the two biggest levers, caching and context discipline, both target input tokens.

## 2. The quadratic-ish cost of context accumulation

Model the loop.
Let the fixed prefix (system prompt plus tool definitions) be S tokens, and let each turn append on average d tokens of new material (model output plus tool results).
Turn k re-sends the prefix and all prior turns, so its input size is roughly S + (k - 1) * d.
Summing over n turns:

```
total input tokens ~ n * S + d * n * (n - 1) / 2
```

The second term is quadratic in n, and for real sessions it dominates quickly.
A concrete instance: S = 5,000, d = 2,000, n = 30 gives roughly 150k prefix tokens plus 870k accumulated-context tokens, so more than a million input tokens for one session whose visible output might be a few thousand tokens.
Doubling session length roughly quadruples the accumulation term, which is why long sessions are disproportionately expensive and why per-session token ceilings are a budget necessity, not just a safety measure.

Output tokens, by contrast, are linear: roughly n times the average generation length, plus reasoning tokens where extended thinking is enabled.
Reasoning tokens are billed as output tokens, and a step that thinks for thousands of tokens pays the premium output rate for them, so thinking budgets are cost budgets exactly as they are latency budgets.

Every context-management technique from Volume 06 is therefore also a cost technique, with the cost lens making the trade-offs concrete.
Compaction (summarizing older turns) caps the accumulation term at the cost of summary-model calls and information loss.
Tool-result truncation and retrieval-on-demand (store the document, give the model a handle, fetch slices when needed) keep bulk data out of the resent context.
Scoped subagents reset the context for subtasks, converting one long quadratic session into several short ones, at the price of losing shared context across the boundary.
The quadratic formula tells you when each is worth it: the savings from capping context at C tokens instead of letting it grow scales with the area between the two curves, so long sessions justify aggressive management and short sessions justify none.

## 3. Prompt caching: the biggest lever

Prompt caching lets the provider reuse the computed state of a prompt prefix across requests, charging a much lower rate for cached input tokens than for uncached ones.
As of early 2026, all major providers offer it: Anthropic exposes explicit cache_control breakpoints with cache-write costing somewhat more than base input and cache-read costing a small fraction of base input, while OpenAI applies automatic prefix caching with a discount on cached tokens; mechanisms and ratios differ, so check current documentation, but the shape is stable.
For an agent loop, the accumulated context of turn k is exactly the context of turn k-1 plus an append, which is the ideal caching pattern: with correct prompt structure, every turn after the first reads almost its entire input from cache.
This is why caching routinely cuts agent input cost by the large majority, more than any other single technique, and why cache hit rate belongs on your main dashboard.

Earning cache hits is an engineering discipline with specific rules.

Structure prompts append-only.
Caching matches prefixes, so any change to early tokens invalidates everything after it.
Put stable content first (system prompt, tool definitions, few-shot examples), never rewrite history in place, and append new turns at the end.
A common self-inflicted wound is injecting a timestamp, a request id, or "current date" at the top of the system prompt, which invalidates the entire cache every request; put volatile values at the end of the prompt or in the latest user message.

Do not shuffle tool definitions.
Tool lists serialized in nondeterministic order (iterating a hash map) change the prefix every request; sort them and keep them stable across a session.

Mind the TTL and traffic pattern.
Cache entries expire after minutes of disuse at default tiers (Anthropic's default is five minutes as of early 2026, with a longer-TTL option at a higher write price), so a session with long gaps between turns pays cache-write again after each gap.
Interactive sessions inside the TTL hit reliably; sporadic background jobs may not, and for those you should compute whether the write premium is worth it at all.

Respect minimum cacheable sizes and breakpoint limits where the provider has them, and place explicit breakpoints (where required) at the boundaries that actually recur: end of system prompt, end of tool definitions, end of conversation history.

Compaction interacts destructively with caching: rewriting history to shrink it invalidates the cached prefix, so a compaction event pays full input price once.
The resolution is to compact rarely and at natural boundaries (for example, when context crosses a threshold), accepting one expensive turn to make all subsequent turns cheap again, rather than continuously rewriting history.

The trade-offs of aggressive caching, stated plainly.
Cache-write premiums mean a prefix used once costs more than an uncached call, so caching short one-shot requests loses money.
Designing prompts for prefix stability constrains prompt engineering (you cannot cheaply personalize the top of the system prompt per request).
And cache behavior is provider-specific enough that a multi-provider abstraction layer must carry per-provider caching logic or forfeit the discount.

## 4. Model routing by task difficulty

The price spread between model tiers is large: as of early 2026 the gap between a small fast model and a frontier model at the same provider is commonly one to two orders of magnitude per token.
Routing work to the cheapest model that meets the quality bar is therefore the second structural lever, and it is the same tiering decision as in latency engineering (Chapter 02, section 5), evaluated on a second axis.

The routing candidates, in rough order of confidence: classification and routing itself, extraction and reformatting of tool output, compaction summaries, guardrail checks, draft generation that a stronger model or a human will review, and high-volume simple end-user tasks identified from traffic analysis.
Keep on the frontier tier: multi-step planning, delicate tool orchestration, tasks with high failure cost, and final synthesis where quality is the product.

The routing mechanism matters less than the evaluation discipline.
Static routing by step type is simple and predictable; learned routers that estimate difficulty per request squeeze out more savings but add a model that can itself be wrong, and their failure mode (hard task routed to weak model) is silent quality loss.
Whatever the mechanism, every routed task class needs its own eval, and the honest accounting must include the retry cost: if the small model fails validation 15 percent of the time and failures are retried on the big model, your effective cost is small-model price plus 15 percent of big-model price plus added latency, which can erase the saving for borderline task classes.
Run the arithmetic per task class rather than assuming routing always wins.

A related lever is right-sizing the default: many products launch on the frontier model out of caution and never revisit, while their measured traffic is dominated by tasks a mid-tier model handles at equal quality.
A quarterly re-evaluation of the default tier against your eval suite is one of the highest-return recurring rituals in cost engineering, and model price-performance improves fast enough (as of early 2026, roughly annual tier turnover) that last year's routing conclusions are stale.

## 5. Output-token discipline

Output tokens carry a multiple of the input price and all of the decode latency, so unnecessary generation is the purest waste in the system.
The recurring sources and their fixes:

- Verbose habits: models pad with preamble, restatement, and summary unless instructed otherwise; system-prompt directives toward concision, with format examples, measurably cut output length at no quality cost for tool-facing text.
- Redundant echo: models re-quoting large tool results or restating the plan every turn; instruct against it and structure prompts so the information is referenced, not repeated.
- Over-generation in artifacts: regenerating a whole file to change three lines; edit-style tools (diff or patch application) cut artifact output by an order of magnitude for iterative work, at the cost of a more complex tool contract and occasional malformed patches.
- Unbounded reasoning: thinking budgets set high globally when only a few step types need deep reasoning; set budgets per step type, not per product.
- Missing max_tokens hygiene: runaway generations (loops, degenerate repetition) billed to the ceiling; set realistic per-call ceilings so pathology is bounded.

The trade-off to respect: for reasoning-heavy steps, output tokens are where quality comes from, and squeezing chain-of-thought to save cost is a false economy that shows up as failed tasks and retries.
Discipline means eliminating tokens that carry no information, not rationing the tokens that do.

## 6. Batch APIs for offline work

Major providers offer batch processing (as of early 2026, both Anthropic's Message Batches and OpenAI's Batch API) with a substantial discount, commonly around half price, in exchange for asynchronous completion within a window on the order of hours rather than seconds.
The mechanism behind the discount is scheduling freedom: the provider fills idle capacity with batch work, so you are being paid to be preemptible.

Agent workloads with a surprising amount of batch-eligible volume: evaluation runs (often the largest single line item for a serious team), embedding and summarization pipelines, nightly data enrichment, report generation, offline scoring of production traces by LLM judges, and backfills after prompt changes.
The design consequence is architectural: build offline pipelines against the batch interface from the start, with job submission, polling or webhook completion, and partial-failure handling, because retrofitting synchronous pipelines later is tedious.
The limits: nothing user-facing or latency-sensitive qualifies, per-request results can arrive in any order, and a batch that lands at the deadline boundary needs your pipeline to tolerate the full window, so deadlines in your own system must be set against the provider's window, not against typical completion times.

## 7. Cost observability

You cannot engineer what you attribute to a single monthly invoice line.
The goal is cost as a first-class engineering metric: visible per session, aggregable per user and feature, and alertable on anomaly, with the same seriousness as latency.

The instrumentation, bottom-up.
Record per model call: model id, input tokens, cached-read and cache-write tokens, output tokens, and reasoning tokens, all of which providers return in the API response as of early 2026, so this is logging discipline rather than estimation.
Tag every call with session id, user or tenant id, feature or workflow id, step type, and prompt version, propagated through your orchestrator the same way trace context propagates (Volume 10's tracing stack should carry these fields already; cost is one more span attribute).
Multiply by a price table you maintain in configuration, versioned and dated, because provider prices change and historical analysis needs the price that applied at the time.

The views that earn their keep.
Cost per session, as a distribution and not an average, because agent cost is heavy-tailed and the p99 session often costs two orders of magnitude more than the median; the tail is where bugs live (loops, retry storms, runaway context growth).
Cost per user and per tenant, which feeds abuse detection, fair-use enforcement, and pricing design.
Cost per feature and per step type, which tells you where the routing and caching work of sections 3 and 4 should aim.
Cache hit rate and cache savings, computed from cached-token counts, so a regression in prompt structure (someone adds a timestamp to the system prompt) shows up as a cost alert within hours instead of on the invoice.
Cost per prompt version, so an eval-passing prompt change that doubles token usage is caught at canary.

Alert on rate anomalies (spend per hour deviating from seasonal baseline), on per-session ceiling breaches, and on cache hit-rate drops.
Enforce budgets in code: per-session token ceilings that terminate gracefully with partial results, per-tenant quotas, and a global circuit breaker on spend rate, because an agent bug that loops is a literal money printer running in reverse until something stops it.

## 8. Unit economics for agent products

The business-facing synthesis of all of the above is a small set of numbers that determine whether the product can exist.

Define cost per successful task, not cost per session: total model plus infrastructure spend on a task class divided by successful completions, so failures and retries are charged to the tasks that eventually succeed.
This is the honest unit cost, and it moves with quality: a reliability improvement that cuts retries reduces unit cost even at identical per-call prices, which is how reliability work shows up on the P&L.

Compare unit cost to unit value.
For internal automation, value is the loaded cost of the human time replaced or augmented, which for most white-collar tasks is orders of magnitude above typical model cost per task as of early 2026; this gap is why "expensive" agent sessions are usually still wildly positive-ROI, and why capping quality to save cents is often the wrong trade.
For consumer and per-seat products the arithmetic is harsher: flat-rate subscriptions meet heavy-tailed usage, and the p99 user can consume hundreds of times the median user's tokens, so a flat price that clears the average can still lose money on exactly the users who love the product most.

The pricing shapes that survive this distribution, with their downsides.
Usage-based pricing (per task, per token, or credits) aligns price with cost and survives heavy users, but transfers cost anxiety to the customer and suppresses usage of the very feature you want adopted.
Flat rate with a fair-use cap or throttle keeps the simple price while bounding tail loss, at the cost of the cap being a visible disappointment to power users.
Hybrid (flat base plus metered overage) is the emerging default for serious agent products as of early 2026 because it preserves predictability for the median and coverage for the tail, at the cost of billing complexity.
Whatever the shape, the internal requirement is the same: per-user cost telemetry from section 7, margin computed per cohort, and an explicit answer to "what does our heaviest plausible user cost us."

Two forward-looking notes for planning.
Per-token prices for equivalent capability have fallen steeply and repeatedly since 2023, so unit economics that are marginal today often clear next year at constant product behavior; build the cost telemetry so you can re-run the margin math the day a price or tier changes.
In the opposite direction, capability growth raises ambition: products keep expanding session length and autonomy to consume the savings, so cost engineering is a permanent function, not a launch-phase task.

## Exercises

1. Derive the total-input-token formula from section 2, then extend it to include a compaction policy that summarizes history down to c tokens whenever context exceeds C tokens. Plot (or tabulate) session cost versus turn count for no-compaction, C = 50k, and C = 100k, using a stated price ratio between input and output tokens.
2. Take the same model and add prompt caching: cache-read at one tenth of base input price, cache-write at 1.25 times base, five-minute TTL, and a compaction event that invalidates the cache. Compute the cache-adjusted cost of a 30-turn interactive session and of the same session with ten-minute gaps between turns, and state the break-even gap length.
3. Design the cost-attribution schema for an agent platform: the fields on every model-call record, the rollup tables, and the five alerts you would ship first. Specify how a prompt-version cost regression would surface, and how quickly.
4. A workflow step is handled by the frontier model at a given quality; the small model costs one twentieth as much but fails validation 12 percent of the time, with failures retried on the frontier model. Write the expected-cost expression for the routed configuration, compute the saving, then state the failure rate at which routing stops paying, including a latency penalty of your choosing in the accounting.
5. Model the unit economics of a flat-rate agent subscription: assume a log-normal usage distribution (choose parameters), a per-token cost, and a monthly price. Find the fraction of users who are margin-negative and total margin across the base, then redesign as flat-plus-overage and show the new margin picture. State every assumption explicitly.

## Godhood check

You have mastered this chapter when you can derive why agent session cost grows quadratically-ish with turns, and use the formula to decide when compaction, truncation, and subagent-scoping each pay.
You can state the prompt-structure rules that earn cache hits, explain the TTL and cache-write economics well enough to compute break-even for a given traffic pattern, and diagnose a cache hit-rate regression from telemetry.
You can run the honest arithmetic for a model-routing decision including retry costs, and name the workload classes that belong on batch APIs and why the discount exists.
You can design cost observability that attributes spend to session, user, feature, and prompt version, and you can build the unit-economics model (cost per successful task versus unit value, tail-user exposure, pricing shape) for a real product without fabricating a single number you have not measured or explicitly assumed.
