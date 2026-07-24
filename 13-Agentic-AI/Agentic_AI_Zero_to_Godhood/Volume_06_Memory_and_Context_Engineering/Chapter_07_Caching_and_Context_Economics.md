# Chapter 07 - Caching and Context Economics

## What you will master

- Why the KV cache exists, what it stores, and why prefix identity is the unit of reuse.
- Prompt caching mechanics across providers: explicit breakpoints, TTLs, and automatic caching, as of early 2026.
- Structuring contexts for cache hits: the stable prefix, append-only history, and the placement of anything that churns.
- Cost modeling for long-running agents: why uncached loops scale quadratically in spend and what caching changes.
- The latency dividend of cache hits and the equivalent machinery in self-hosted serving (vLLM prefix caching, SGLang RadixAttention).
- Measuring cache performance from API usage fields and building the dashboard that keeps it honest.
- When caching stops being an optimization and starts dictating architecture: compaction timing, memory placement, and multi-agent topology.

## 7.1 Why caching exists: the KV cache

During prefill, a transformer computes key and value tensors for every input token at every layer; during decoding, each new token attends over all of them.
The KV cache is that stored computation, and its size and cost scale with input length, which is why long contexts are expensive in both latency and money.
The provider-side insight behind prompt caching: two requests whose input token sequences share an identical prefix share identical KV entries for that prefix, because each token's keys and values depend only on the tokens before it.
So the provider can store the prefix's KV state, and a later request that matches the prefix exactly can skip recomputing it, paying only for the novel suffix.

Two hard consequences follow from the mechanism, and every practice in this chapter derives from them:

- Identity is exact and positional: one changed token invalidates everything after it; there is no fuzzy matching, and "almost the same prompt" is a full-price prompt from the point of divergence.
- Reuse is prefix-shaped: content can only be cached up to the first difference, so the order of your context sections (Chapter 02) is also your cache policy, whether you meant it or not.

Agents are the ideal caching customer by construction: the loop re-sends system prompt, tool definitions, and an ever-growing history on every turn, so consecutive calls share a massive prefix that grows by one turn each step.

## 7.2 Provider mechanics as of early 2026

The shapes below are date-stamped early 2026; verify current pricing and limits in provider docs before modeling, because these are exactly the numbers that rot.

Anthropic (explicit caching): you mark cache breakpoints with `cache_control: {"type": "ephemeral"}` on content blocks; everything from the start of the prompt through the breakpoint is the cacheable prefix.
Up to 4 breakpoints per request; minimum cacheable prefix length is model-dependent (on the order of 1024 tokens for larger models, 2048 for smaller ones); the cache key covers the full prefix including tools and system content, and the platform checks breakpoints against previously cached prefixes.
Pricing shape: writing a 5-minute-TTL cache entry costs a premium over base input (1.25x), reading costs a small fraction (0.1x); a 1-hour TTL is available at a higher write premium (2x); TTLs refresh on use.
The asymmetry defines the economics: at 0.1x reads, a prefix re-used even a handful of times within the TTL pays for its write premium many times over, and a prefix used only once costs you 25 percent extra for nothing.

OpenAI (automatic caching): prompts beyond a minimum length (1024 tokens) are cached automatically on exact prefix match, with cached input tokens discounted (a 50 percent discount on cached input was the announced shape in 2024; some later models carry deeper cached-input discounts); no breakpoints to place, no write premium, cache lifetime is short and managed by the platform, with routing that tries to hit the same cache.
Gemini offered both implicit caching and an explicit cached-content API with storage billed by token-hours (shape as of 2025).

The engineering difference is who bears the thinking: automatic caching means you cannot get the accounting wrong but also cannot buy a longer TTL or reason precisely about placement; explicit caching gives you leverage and hands you a new class of bug, the silently-missing cache, which costs real money while functioning perfectly.

## 7.3 Structuring context for cache hits

The rules all reduce to one principle: sort your context by change frequency, most stable first, and never let anything volatile sit upstream of anything stable.

1. Stable prefix: system prompt, then tool definitions, then injected memory files, all byte-identical across calls within a session; a session-start timestamp is fine, a per-call timestamp in the system prompt is a self-inflicted 100 percent miss rate, and the same goes for request ids, random example ordering, or "helpfully" refreshed dynamic sections.
2. Append-only history: the message list must only grow at the tail during normal turns; each turn then hits the cache for the entire previous context and pays full price only for the newest messages, which converts the loop's quadratic full-price token flow into quadratic-at-0.1x plus linear novel tokens.
3. Volatile content goes last: the todo re-injection anchor (Chapter 04), retrieved snippets for the current step, and per-turn reminders belong in the churning suffix after the last breakpoint, which is exactly where recency position wants them anyway; caching and attention mechanics agree here, which is convenient.
4. Determinism is a cache property: serialize tools and memory in a canonical order; a dict that iterates differently across processes is a cache-buster invisible in diffs.
5. Place explicit breakpoints at the stability boundaries: end of tools, end of system-plus-memory, and rolling at the recent end of history; with a 4-breakpoint budget, that allocation covers the standard agent shape.

Every mutation of history is a cache event, and this is where earlier chapters connect: tool-result clearing and compaction (Chapter 03) rewrite the middle of the sequence and invalidate everything after the edit point, so the next call after compaction re-prefills the new context at full price.
This does not make compaction wrong; it makes compaction timing an economic decision (Section 7.6).

A concrete Anthropic-shaped request illustrating the layout (API shape current as of early 2026; check docs before copying):

```python
response = client.messages.create(
    model=MODEL,
    system=[
        {"type": "text", "text": SYSTEM_PROMPT},          # stable all session
        {"type": "text", "text": MEMORY_FILES,            # stable all session
         "cache_control": {"type": "ephemeral"}},         # breakpoint 1: end of system+memory
    ],
    tools=TOOLS,                                          # canonical order, stable
    messages=[
        *history[:-1],                                    # append-only; last old message
                                                          # carries breakpoint 2 (rolling)
        current_turn,                                      # novel suffix: new user msg or
                                                          # tool results + todo anchor
    ],
)
```

The rolling breakpoint on the most recent stable message is the detail people miss: on the next call, the platform matches the longest previously cached prefix at or before your breakpoints, so advancing the breakpoint each turn caches the newly appended turn for the turn after it.
The todo anchor and any per-turn reminder text live inside `current_turn`, after every breakpoint, so their churn costs only their own tokens.

## 7.4 Cost modeling for long-running agents

Model the loop before optimizing it; the arithmetic is short and the conclusions are large.
Let a run have T turns, a stable prefix of P tokens, and an average of d new tokens appended per turn (messages plus tool results).

Uncached input spend is proportional to the sum over turns of (P + t*d), which is T*P + d*T^2/2: quadratic in run length, with every historical token re-billed at full price every turn.
With ideal caching (append-only, no invalidation, reads at fraction r of base price, r = 0.1 for Anthropic as of early 2026), each turn pays full price only for d novel tokens plus r times the re-read history, so spend is roughly r*(T*P + d*T^2/2) + (1-r)*(P + d*T): the quadratic term survives but shrunk by 10x, and for realistic run lengths total input spend drops by 5-10x depending on the prefix-to-novel ratio.
Worked instance to internalize the shape: P = 10k, d = 3k, T = 100 gives about 16M uncached input tokens versus roughly 1.9M effective full-price-equivalent tokens with clean caching; run the numbers for your own agent, and do not quote anyone else's ratio, including this one, without measuring.

The model also prices your design choices:

- A cache-busting bug costs the difference between the two curves, which is why misses are a budget incident, not a nit.
- Compaction trades a one-time re-prefill of the new (smaller) context against a permanently smaller history term; compacting too often burns re-prefills, too rarely burns window and quality (Chapter 03), and the model tells you the break-even for your P, d, and threshold.
- Output tokens, priced several times input and never cached, plus tool execution and latency costs, sit outside this model; for tool-heavy agents input dominates spend, but check your own mix before believing that.

TTL management is the remaining lever: a 5-minute TTL refreshed on use covers an active loop for free, but an agent that waits on a human approval for an hour comes back to a cold cache and one full-price re-prefill; the 1h-TTL write premium is worth it exactly when expected idle gaps exceed the short TTL and the prefix is large, and a scheduled keep-warm ping is the cruder alternative with its own cost.

### Latency is the second dividend

Prefill work is where time-to-first-token goes on long contexts, and a cache hit skips exactly that work, so caching buys latency as well as money.
Anthropic's documentation has described large latency reductions on long cached prompts, and the effect is mechanical: the hit turns a 100k-token prefill into a lookup plus a short novel-suffix prefill.
For interactive agents this often matters more than the invoice: per-turn responsiveness stays flat as the session grows instead of degrading linearly, which directly shapes how long a session users will tolerate.
The corollary bites in the other direction: the first turn after a compaction or a TTL expiry is not just expensive but visibly slow, so schedule compaction at moments where a pause is acceptable (phase boundaries again) rather than mid-interaction.

### Self-hosted serving: the same physics, your hardware

If you serve open-weight models, prompt caching is not a billing feature but an engineering feature you turn on: vLLM ships automatic prefix caching that reuses KV blocks across requests sharing a prefix, and SGLang's RadixAttention organizes cached prefixes in a radix tree for reuse across a request population (both shipping and widely used as of 2025-2026).
The economics translate from dollars into GPU-seconds and KV-cache memory: cached prefixes occupy HBM, so the trade is hit rate against batch capacity, and eviction policy (typically LRU over cache blocks) replaces TTLs.
The context-structuring rules of Section 7.3 apply unchanged, and one new lever appears: you control scheduling, so routing requests that share a prefix to the same replica (cache-aware routing) is yours to implement, where hosted APIs do it for you invisibly.
The honest cost is operational: KV memory pressure, fragmentation, and eviction tuning become your pager's problem, which is a real part of the build-versus-buy inference decision (Volume 12).

## 7.5 Measuring cache performance

Providers report the split per response, and everything else is built from it: Anthropic's usage block reports `cache_creation_input_tokens`, `cache_read_input_tokens`, and uncached `input_tokens` (field shapes as of early 2026); OpenAI reports `cached_tokens` inside `prompt_tokens_details`.

Track three numbers per agent per day, and alert on the third:

- Hit rate by token: cache-read tokens divided by total input tokens; healthy agent loops with the Section 7.3 layout run high (the large majority of input tokens read from cache); chat products with cold users run far lower.
- Effective input price: blended per-token spend versus the uncached counterfactual; this is the number finance understands and the one that justifies the engineering.
- Write-without-read anomalies and hit-rate cliffs: a sudden drop in hit rate after a deploy is almost always an accidental cache-buster (a reordered tool list, a new timestamp, a changed serialization), and it will not show up anywhere else because behavior is unchanged.

The most useful debugging technique is the same one as Chapter 02's: capture two consecutive assembled requests and diff them byte-for-byte; the first differing byte is where your cache dies, and it is routinely somewhere embarrassing.
Fold these metrics into the per-section context dashboard of Chapter 02, because composition and caching are the same investigation: the stacked-area chart tells you what you are paying attention with, the cache split tells you what you are paying cash for.

## 7.6 When caching changes architecture

Below a certain scale caching is a discount; above it, it is a design force with opinions.
The decisions it reaches into:

- Compaction policy: since every compaction is a cache flush, prefer fewer, larger compactions at phase boundaries over continuous trimming, and co-schedule tool-result clearing with compaction rather than clearing eagerly every turn; server-side context editing (Chapter 03) makes eager clearing easy, which makes this trade easy to get wrong.
- Memory placement: content injected into the stable prefix (memory files, hot notes) is nearly free to re-read but expensive to update mid-session, because an update is a flush; JIT-retrieved content arriving as fresh tool results costs full price once and never busts the prefix; the Chapter 04 hybrid rule thus has a cache column too, and it mostly agrees with the attention column: stable and hot goes early, volatile and cold arrives late.
- System prompt economics: a large, high-quality stable prefix (rich instructions, canonical examples, full tool docs) costs its write once per TTL window and 0.1x thereafter, so caching partially repeals Chapter 01's per-token tax for stable content within an active session; the attention tax remains in full, which is why "cache makes big prompts free" is half true and the wrong half gets quoted.
- Multi-agent topology: sub-agents with distinct system prompts share no prefix with their orchestrator, so every sub-agent spawn is a cold prefill; shared-prefix designs (same system prompt and tools, role selected in the suffix) trade prompt specialization for cache reuse, and fan-out patterns that re-send one large corpus to N workers should restructure so the corpus sits in a cached shared prefix rather than N paid copies (Volume 07 picks this up).
- Session infrastructure: resume-after-idle (Chapter 06) lands on a cold cache, so the first turn after resume is priced like a new session; batch and offline pipelines that replay transcripts (evals, memory extraction) should be laid out to share prefixes deliberately, and batch APIs with their own discounts often beat cache gymnastics there.

The honest counterweight: cache-driven design has a failure mode of ossification, where teams refuse prompt improvements, memory updates, or overdue compactions because they flush the cache.
The discipline is to treat cache efficiency as a constraint with a price, not a virtue: compute what the flush costs, compare it with what the change buys, and remember that a 0.1x read rate means even a daily full flush is usually noise next to a quality regression.
Quality decisions outrank cache decisions; the point of this chapter is to make that ranking a calculation instead of a slogan.

## Exercises

1. Instrument an existing agent to log the usage split per call, compute token hit rate and effective input price over a week of runs, and produce the uncached counterfactual; state your measured savings multiple.
2. Find a cache-buster: diff two consecutive assembled requests from any agent you run byte-for-byte, identify the first divergence, fix it, and measure the hit-rate change; if you find none, introduce one (a per-call timestamp), quantify its cost over a 50-turn session, and remove it.
3. Build the cost model of Section 7.4 as a small script parameterized by P, d, T, r, and write premium; plot uncached versus cached spend curves for your real agent's parameters, then add compaction (threshold, retained tail) and find the compaction frequency that minimizes total spend for a 200-turn run.
4. Design breakpoint placement for an Anthropic-shaped agent with system prompt, 20 tools, injected memory, and growing history under the 4-breakpoint budget; justify each placement by change frequency, then implement it and verify with usage fields that history turns read from cache.
5. Measure the idle-gap trade: run the same 30-turn session with a 10-minute human pause in the middle under (a) short TTL, (b) long TTL, and (c) a keep-warm ping, and report total input spend for each.
6. Restructure a fan-out task (one 50k-token corpus, 10 parallel analysis calls) from per-call corpus copies to a shared cached prefix, and report the spend difference and any quality change.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Explain what the KV cache stores, why prefix identity is exact and positional, and derive from the mechanism why section ordering is cache policy.
- Contrast explicit-breakpoint and automatic caching models, including the write-premium/read-discount asymmetry and the silently-missing-cache failure mode, with early-2026 shapes date-stamped.
- Recite the stable-prefix and append-only rules, name four real cache-busters, and say where volatile re-injected content must sit and why attention mechanics agree.
- Write down the quadratic cost model for an agent loop, show what caching does to it, and use it to price a compaction policy and a TTL choice.
- Name the usage fields that measure caching, the three dashboard numbers worth alerting on, and the byte-diff debugging technique.
- Give three examples where caching legitimately changes architecture, and state the ossification counter-principle that keeps quality decisions ranked above cache decisions.
