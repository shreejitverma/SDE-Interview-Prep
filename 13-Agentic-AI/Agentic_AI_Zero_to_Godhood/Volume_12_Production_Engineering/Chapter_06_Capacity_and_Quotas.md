# Chapter 06 - Capacity and Quotas

## What you will master

- The shapes of provider rate limits (RPM, TPM, ITPM, OTPM, concurrency) and how each one binds an agent workload differently.
- Quota tiers, how providers grant capacity, and how to manage the allocation as a first-class engineering resource.
- Load shedding and prioritization: deciding what not to serve when demand exceeds capacity, before the provider decides for you.
- Capacity planning for spiky, heavy-tailed agent workloads, and the arithmetic that converts product forecasts into token budgets.
- Multi-provider strategies with their real costs, and self-hosted open-weight serving (vLLM, SGLang) as a capacity lever, with the honest decision rule for when it makes sense.

## 1. Capacity is a token budget, not a server count

Classical capacity planning provisions compute you own; agent capacity planning mostly manages quota someone else grants you.
The scarce resource is tokens per unit time at a given model, and it is scarce twice over: your provider caps what you may consume, and upstream of that, frontier-model inference capacity industry-wide has been supply-constrained for most of 2024 through early 2026, which is why quotas exist at all rather than providers simply selling unlimited throughput.
The consequences frame this chapter: you must know your limits precisely, spend them deliberately (prioritization), forecast growth into them (planning), and know your options when you outgrow them (more quota, more providers, or your own serving).

## 2. Rate limit shapes

Providers enforce several limit types simultaneously, and your effective capacity is whichever binds first for your traffic mix.
The shapes below are stable across major providers as of early 2026; specific numbers per tier change often enough that you should read them from the provider dashboard, not from memory or this page.

RPM (requests per minute) caps call count regardless of size.
Agent loops are chatty (many small-to-medium calls per session), so RPM binds first for workloads with many short steps, and batching several logical questions into one call, or reducing loop iterations, is the relief valve.

TPM (tokens per minute) caps total token throughput, sometimes as a single number and increasingly split into ITPM (input tokens per minute) and OTPM (output tokens per minute), because the two consume different provider resources (prefill compute versus decode occupancy).
Agents are input-heavy (Chapter 03's accumulated contexts), so ITPM is very often the binding limit for agent products, and this is the quiet reason prompt caching matters for capacity, not just cost: at Anthropic as of early 2026, cache reads do not count against ITPM the way uncached input does (OpenAI's cached tokens likewise get favorable treatment), so a high cache hit rate can multiply your effective input capacity within the same nominal quota.
Check the current accounting rules for your provider before relying on this, because the details are provider-specific and revisable.

Concurrency limits cap simultaneous in-flight requests, binding for workloads with long generations (each stream occupies a slot for its whole decode) and for fan-out patterns that launch many parallel subagent calls at once.

Enforcement details that change your engineering.
Token limits are typically enforced against an estimate at admission (your max_tokens declaration can count against OTPM before you generate anything, at some providers), so inflated max_tokens hygiene from Chapter 03 is also a capacity issue.
Windows are short (per-minute, often with burst smoothing at finer granularity), so a spike that averages fine over an hour can still be throttled within a minute, and client-side pacing (the token bucket from Chapter 04, section 7) set just under quota converts 429 chaos into a smooth queue you control.
Limits are per model and usually per workspace or organization, which is why per-class and per-tenant allocation (section 4) has to be built by you; the provider gives one pool per model and does not know your priorities.

## 3. Quota tiers and growing your allocation

Providers grant quota in tiers that scale with account history and spend (automatic tier ladders at lower levels, sales conversations above them), and as of early 2026 the major providers all offer some form of purchased dedicated capacity at the top (provisioned throughput at cloud providers, enterprise agreements with committed throughput at the labs), where you pay for reserved capacity whether used or not in exchange for guaranteed throughput and better latency isolation from public-pool weather.

Manage the allocation as an engineering resource, with three disciplines.
Know your binding limit empirically: instrument 429 responses and provider rate-limit headers (remaining-quota headers are standard) and dashboard the utilization of each limit shape per model, because teams routinely optimize RPM while ITPM is what is throttling them.
Lead growth with quota requests: tier increases and enterprise capacity take days to quarters of lead time, so the capacity plan of section 5 must trigger requests ahead of the demand curve, and a launch that 10xes traffic without a pre-arranged quota bump is a self-inflicted outage.
Treat dedicated capacity as a portfolio decision: reserved throughput is economically a base-load purchase, and the standard shape is reserved capacity sized to your steady base with public-pool (or batch API) overflow for peaks, exactly analogous to reserved instances versus on-demand in classical cloud economics; the downside is commitment risk if your demand forecast or model choice changes, and model-tier turnover as of early 2026 is fast enough that multi-year commitments to a specific model deserve skepticism.

## 4. Load shedding and prioritization

When demand exceeds capacity, something will not be served; load shedding is choosing what, on purpose, instead of letting timeouts and 429s choose randomly.
Random degradation is the worst outcome because it spreads pain uniformly across your most and least important traffic, whereas designed shedding concentrates it where it costs least.

Build the priority scheme first.
Classify every model call by workload class (interactive user turn, background job, scheduled batch, eval run, internal experimentation) and by tenant tier where the business distinguishes them, and attach the class to the request at its source, because you cannot prioritize what you cannot identify.
A workable default ordering: interactive turns above background jobs above scheduled batch above evals above experimentation, with safety-critical calls (guardrails on traffic you are already serving) pinned at the top since shedding them silently degrades safety, not just service.

Then enforce it at the choke point.
The clean implementation is a token-budget scheduler in front of the provider: each class gets a guaranteed share of the per-minute budget plus access to unused headroom, high classes preempt admission (not in-flight calls) of lower ones, and the lowest classes are the first parked entirely when the budget tightens.
Degradation composes with the ladder of Chapter 04: before refusing interactive traffic, shed by cheapening it (shorter max_tokens, a tier-down for routable steps, disabling expensive optional capabilities), because a degraded answer usually beats a refusal, and refusal beats an unbounded queue.
For queued work, shedding means deferral with honesty (deadline-aware scheduling, and re-forecasting completion times you expose to users) rather than silent staleness.

The failure mode to design against explicitly: internal traffic eating production quota.
Eval suites and backfills are bursty, machine-driven, and quota-hungry, and the day an engineer launches a 50,000-trace judge run against the production model pool is the day interactive latency spikes for every user; separate quota pools (distinct provider workspaces or keys where supported), plus the batch API for everything offline, is cheap insurance and should be organizational policy, not etiquette.

## 5. Capacity planning for spiky agent workloads

Agent demand is spiky on every timescale: diurnal and weekly cycles like any product, launch and marketing spikes, and a per-session amplification factor that makes user-level spikes worse at the provider (one user action can trigger tens of calls over minutes, so user concurrency multiplies into call concurrency with a fan-out you must measure, not guess).
Session cost is also heavy-tailed (Chapter 03), so mean-based planning understates the load contributed by tail sessions; plan on distributions.

The planning arithmetic, step by step.
Start from product forecasts (sessions per hour at peak, by workload class), multiply by measured per-session distributions (model calls per session, input and output tokens per call, at p50 and p95) to get token and request demand per minute at peak, per model and per limit shape.
Apply the cache-adjustment (expected cache hit rate converts gross input tokens into quota-relevant uncached tokens, per the section 2 accounting) and the retry overhead (a realistic degraded-day retry multiplier, not the happy-path one).
Compare against quota per limit shape, and require headroom: a utilization target around 60 to 70 percent of the binding limit at forecast peak is a sane default for spiky traffic, because the difference between forecast peak and realized peak is exactly where incidents live, and because Chapter 04's storm dynamics need slack to drain into.
Re-run the plan on every prompt or architecture change that moves tokens per session, which is to say continuously: capacity planning for agents is a living calculation coupled to the cost telemetry of Chapter 03, not an annual spreadsheet.

Two agent-specific planning traps.
Context growth is nonlinear in session length, so a product change that extends average sessions by 30 percent can raise ITPM demand far more than 30 percent; the quadratic term from Chapter 03 shows up here as capacity, and per-session token ceilings are a capacity control as much as a cost control.
Fan-out features (parallel subagents, Chapter 02) convert latency wins into concurrency spikes, and a feature that launches eight parallel branches needs its concurrency budget checked against the concurrency limit shape before launch, not after.

## 6. Multi-provider strategies and their costs

Running against multiple model providers buys three distinct things, and it is worth being precise about which one you are buying, because they justify different investment levels.
Availability: surviving a whole-provider outage (the rung-3 fallback of Chapter 04).
Capacity: summing quotas across providers when one provider cannot or will not grant enough.
Choice: routing each task class to the best or cheapest adequate model across the market, and negotiating leverage as a side effect.

The costs are larger than teams expect, and they are mostly ongoing rather than one-time.
API divergence: request shapes, tool-calling conventions, structured-output mechanisms, and streaming formats differ; an abstraction layer (self-built, or a gateway like LiteLLM-style proxies, or a cloud aggregator like Bedrock or Vertex offering many models behind one API, as the landscape stands in early 2026) papers over syntax but cannot paper over semantics.
Behavioral divergence is the deep cost: prompts are tuned to a model, and the same prompt produces different tool-calling behavior, different verbosity, and different failure modes elsewhere, so real multi-provider readiness means maintained prompt variants and a per-provider eval suite run continuously, which roughly multiplies your prompt-engineering and eval surface by the provider count.
Divided optimization: prompt caches, quota tier progression, and provisioned-capacity economics all reward concentration, so splitting traffic dilutes each; a 50/50 split can cost measurably more per token than either provider alone at the same volume.
Operational surface: two sets of incident behaviors, deprecation calendars, and account relationships.

The honest decision rules that follow.
If you are buying availability only, the cheap version is an actively-exercised fallback for degraded mode (reduced capability is acceptable during a rare outage), not behavioral parity; size the investment to that and resist scope creep toward parity.
If you are buying capacity, first exhaust the single-provider options (tier increases, dedicated capacity, cache-driven effective-quota multiplication, batch offload), because they are cheaper than the multi-provider tax.
If you are buying choice, route at the task-class boundary with per-class evals (the Chapter 03 routing discipline extended across vendors) rather than pretending arbitrary traffic is portable.
And whatever you buy, keep one provider primary: the equal-split architecture pays every cost at full price and captures each benefit at half strength.

## 7. Self-hosted open-weight serving as a capacity lever

Serving open-weight models on hardware you control (owned, rented, or GPU-cloud) removes the quota ceiling entirely for the traffic you move there: capacity becomes a hardware-procurement problem, which at least is a problem with a known playbook.

The serving stack as of early 2026 is genuinely mature.
vLLM (built around PagedAttention for KV-cache memory efficiency and continuous batching for throughput) and SGLang (notable for RadixAttention, which shares KV-cache across requests with common prefixes, an unusually good fit for agent workloads where system prompts and tool definitions repeat across every call) are the leading open inference engines, both production-grade with OpenAI-compatible APIs, quantization support, and multi-GPU serving; TensorRT-LLM occupies the NVIDIA-optimized niche.
Open-weight model quality (the Llama, Qwen, DeepSeek, Mistral, and gpt-oss lineages, as of early 2026) sits meaningfully behind the closed frontier on hard agentic tasks but comfortably covers the routable tiers of Chapter 03: classification, extraction, compaction, guardrails, and mid-difficulty single steps.

When it makes sense, as a decision rule rather than a vibe.
The favorable conditions, all of which should be checked: sustained high-volume traffic on task classes an open-weight model demonstrably handles at your quality bar (per your evals, not leaderboards); utilization high enough to beat API economics, because self-hosted cost is capacity-shaped (you pay for the GPUs at 3 a.m.) while API cost is usage-shaped, so the crossover requires keeping the hardware busy, and bursty low-duty-cycle traffic never crosses it; a team able to own inference operations (GPU procurement or reservations, engine upgrades, model updates, capacity management, an on-call rotation for a new tier of infrastructure); or a non-economic forcing function (data residency, strict tenancy, air-gapped deployment, latency co-location) that closed APIs cannot meet, which settles the question regardless of economics.
The recurring successful pattern is hybrid: self-host the high-volume cheap tiers where open-weight quality suffices and utilization is provable, keep frontier API capacity for the hard steps, and let the Chapter 03 router draw the boundary; the recurring failure pattern is self-hosting the whole product for cost reasons, discovering the quality gap on the hard tail, and running both stacks at low utilization each.
Re-evaluate the boundary on a cadence, because both sides move: API prices per unit capability have fallen steeply and repeatedly, and open-weight quality has climbed steadily, so the crossover point migrates every few quarters and a decision made in 2024 is stale in 2026 in either direction.

## 8. The capacity operations loop

Capacity work is a loop, not a project, and it closes back into the rest of the volume.
Instrument utilization per limit shape, per model, per workload class, with 429s and rate-limit headers as ground truth (this chapter, feeding Chapter 07's dashboards).
Alert on utilization trends crossing headroom thresholds, not just on throttling after it starts.
Re-forecast on every product and prompt change that moves tokens per session (coupled to Chapter 03's telemetry).
Lead quota growth ahead of the forecast, with dedicated capacity sized to base load.
Shed by design when the forecast is wrong anyway, in priority order, with internal traffic fenced off from production quota.
And rehearse the overflow paths (fallback provider, batch deferral, tier-down degradation) on a schedule, because a capacity strategy that has never been exercised under load is, like every unexercised fallback in this volume, a hypothesis.

## Exercises

1. An agent product forecasts 600 peak sessions per hour; measured per-session medians are 14 model calls, 38k input tokens per call late-session average, 400 output tokens per call, with p95 sessions at three times median token volume, an expected 85 percent cache hit rate, and a degraded-day retry multiplier of 1.3. Compute peak RPM, ITPM (cache-adjusted), and OTPM demand, state which limit shape binds against a quota you choose to posit, and size the quota request at 65 percent target utilization. Show every step.
2. Design the token-budget scheduler for a platform with five workload classes and three tenant tiers: the allocation policy (guarantees, headroom sharing, preemption rules), the queue structure, the enforcement point in the architecture of Chapter 05, and the metrics that prove it is working during a throttling event.
3. Write the shedding runbook for a day when provider capacity drops to 40 percent of normal: the ordered list of what is parked, cheapened, or deferred, with the trigger and reversal condition for each action, and the user-facing communication for each affected surface.
4. Build the multi-provider decision memo for a concrete product: state which of the three benefits (availability, capacity, choice) you are buying, the architecture rung it implies, the ongoing costs (prompt variants, eval multiplication, cache and tier dilution) in engineer-hours per month as explicit estimates, and the single-provider alternatives you rejected and why.
5. Construct the self-hosting crossover analysis for a guardrail-and-extraction tier: pick an open-weight model class and serving engine, estimate served throughput per GPU from published engine documentation (cite what you used; do not invent benchmark numbers), posit GPU cost and utilization scenarios, and find the monthly volume at which self-hosting beats a stated API price assumption. State every assumption and mark which ones the conclusion is most sensitive to.

## Godhood check

You have mastered this chapter when you can name the rate-limit shapes from memory, say which one binds for a given traffic mix, and explain why agent workloads are usually ITPM-bound and how prompt caching multiplies effective capacity under early-2026 accounting rules.
You can run the capacity-planning arithmetic from product forecast to per-limit-shape demand with cache and retry adjustments, defend a headroom target, and name the two agent-specific traps (context nonlinearity, fan-out concurrency).
You can design priority-ordered load shedding with internal traffic fenced off, and explain why designed shedding beats random 429 pain.
You can state precisely which benefit a multi-provider setup buys in a given design and its full ongoing cost, and you can argue both sides of the self-hosting decision with the utilization-shaped-versus-usage-shaped cost framing, naming vLLM and SGLang's key mechanisms and why prefix-sharing matters specifically for agents.
