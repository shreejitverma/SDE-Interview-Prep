# Chapter 07 - Inference Mechanics

## What you will master

- The KV cache: what is stored, the exact size formula, why it dominates serving memory, and how architecture choices from Chapter 02 exist to shrink it.
- Prefill versus decode: why one phase is compute-bound and the other memory-bandwidth-bound, via arithmetic intensity.
- Continuous batching and PagedAttention: how modern serving engines keep GPUs busy across ragged requests.
- Speculative decoding: trading cheap parallel verification for sequential generation, exactly and losslessly.
- Quantization of weights and KV cache: the formats, the methods, and the honest quality caveats.
- Why latency and cost behave the way they do: TTFT versus TPOT, the input/output price asymmetry, and the arithmetic every agent budget rests on.
- Prompt caching mechanics: prefix rules, cache lifetimes, pricing shape, and the prompt-structure discipline it imposes on agent design.

## 1. Why inference mechanics is an agents topic

An agent is a loop that repeatedly sends a growing context to a model and waits for tokens; its latency, cost, and feasible context are set by the serving physics in this chapter.
Every later production decision (Volume 12) is a corollary of four facts: generation is sequential, the KV cache grows with context, decode is memory-bandwidth-bound, and identical prefixes are recomputable-or-cacheable work.
Numbers for specific hardware and prices below are order-of-magnitude anchors stated as of early 2026; the structural relationships are the durable content.

## 2. The KV cache

### 2.1 What and why

Causal attention at position t needs the keys and values of all positions up to t (Chapter 02).
Without caching, generating each new token would recompute K and V for the whole prefix, making generation quadratic-times-depth in length; the KV cache stores every layer's keys and values once, so each new token computes attention against stored tensors and appends its own K and V.
The cache is pure classic space-for-time: generation becomes linear in sequence length, at the price of memory that grows with every token held in context.

### 2.2 The size formula

```
bytes = 2 * n_layers * n_kv_heads * d_head * seq_len * bytes_per_value
```

The leading 2 is K plus V; n_kv_heads is the GQA-reduced head count, which is where Chapter 02's architecture meets this chapter's economics.
Worked example, Llama-3-70B-class geometry: 80 layers, 8 KV heads, d_head 128, FP16 (2 bytes).
Per token: 2 * 80 * 8 * 128 * 2 = 327,680 bytes, roughly 0.33 MB.
A 128K-token context therefore holds roughly 42 GB of KV cache, comparable to half the model's own FP16 weight footprint, for one request.
Under full multi-head attention (64 KV heads) the same context would need roughly 335 GB, which is why GQA exists: an 8x cache reduction is the difference between serving long contexts and not.
The consequences cascade: cache size caps concurrent batch size, batch size determines throughput, and throughput determines cost per token; this single formula is the causal root of most provider pricing structure, including why long-context requests are disproportionately expensive to serve.

### 2.3 Architectural countermeasures, revisited

Chapter 02's variants are now legible as KV-cache engineering: MQA and GQA shrink n_kv_heads; sliding-window layers cap seq_len per layer at the window size; MLA (multi-head latent attention, DeepSeek-V2/V3, 2024) compresses K and V into a low-rank latent per token and reconstructs heads at compute time, cutting cache by an order of magnitude at some extra compute; hybrid and state-space blocks (Mamba-family layers in 2024-2025 hybrids) replace attention's growing cache with constant-size state in some layers.
The pattern to internalize: as of early 2026, inference memory economics, not modeling quality, is the dominant force reshaping attention architecture.

## 3. Prefill versus decode

### 3.1 The two phases

Prefill processes the entire prompt in one pass: every prompt token's activations and KV entries are computed in parallel, exactly like a training forward pass, ending with the first generated token.
Decode then generates one token at a time: each step is a full forward pass for a single token, attending against the cache, appending to it, and sampling.
The phases have opposite hardware personalities, and every serving system is organized around that opposition.

### 3.2 Arithmetic intensity

Arithmetic intensity is FLOPs performed per byte moved from memory; a GPU has a fixed ratio of compute throughput to memory bandwidth (an H100 SXM: on the order of 1e15 BF16 FLOPs/s against about 3.35e12 bytes/s of HBM bandwidth, a ratio near 300 FLOPs per byte).
Kernels far below the ratio are memory-bound; far above, compute-bound.
Prefill does large matrix-matrix multiplies: thousands of prompt tokens reuse each loaded weight, intensity is high, and the GPU runs near its compute peak; prefill cost therefore scales with prompt length, and long prompts take visibly longer to reach the first token.
Decode without batching does matrix-vector multiplies: every weight byte and every KV-cache byte is loaded to produce a single token, yielding an intensity around 1-2 FLOPs per byte, two orders of magnitude below the compute-bound threshold.
Decode speed is therefore set by bytes moved, not math: a naive per-token latency floor is roughly (weight bytes + KV bytes) / memory bandwidth, and the compute units idle.
This asymmetry is the single most explanatory fact in LLM serving: it is why output tokens cost more than input tokens, why batching is the core throughput lever, why weight and KV quantization speed up decode directly, and why GQA and MLA are worth their quality risk.

### 3.3 Batching decode

Batching B decode requests reuses each loaded weight B times, multiplying weight-side intensity by B and moving decode toward compute-bound; per-token cost falls dramatically with batch size until KV-cache traffic (which does not amortize across requests, since each request has its own cache) or memory capacity becomes the new binding constraint.
This is why high-utilization providers can price tokens low, why your self-hosted single-user deployment will look shockingly inefficient next to provider pricing, and why the KV cache's memory footprint (Section 2) is the real limiter of the batching remedy.

## 4. Continuous batching and PagedAttention

Static batching (assemble B requests, run all to completion) fails for LLMs because requests are ragged: generation lengths differ wildly, so finished requests idle their slots while the longest request runs, and arriving requests wait for the whole batch.
Continuous batching (introduced as iteration-level scheduling in Orca, 2022) reschedules at every decode step: completed sequences leave the batch immediately, waiting requests join immediately, and their prefill is interleaved with ongoing decodes.
The gain is throughput at high concurrency, with a trade-off surface you will meet in production: mixing prefill work into decode steps creates latency jitter for in-flight requests, managed by chunked-prefill policies that split long prompts into slices.

PagedAttention (vLLM, 2023) fixed the memory side: contiguous per-request KV allocations forced worst-case reservation and fragmented memory, so vLLM manages the cache like virtual memory, in fixed-size blocks (tens of tokens each) mapped through a block table per sequence.
Fragmentation collapses, memory serves actual rather than worst-case lengths, batch sizes rise, and identical prefixes can share physical blocks copy-on-write, which is the in-engine ancestor of the provider-level prompt caching of Section 8.
As of early 2026, continuous batching with paged or similar block-managed caches is the baseline in every serious engine (vLLM, TensorRT-LLM, SGLang, and provider-internal stacks); a further standard refinement is disaggregated serving, running prefill and decode on separate GPU pools sized to their different bottlenecks.

## 5. Speculative decoding

Decode is memory-bound: the weights are loaded anyway, and the compute units are idle (Section 3).
Speculative decoding spends that idle compute to shorten the sequential path.
A cheap drafter proposes k tokens autoregressively; the target model then scores all k positions in one parallel forward pass (parallel scoring is cheap, like prefill); accepted tokens are kept up to the first rejection, and a rejection-sampling correction guarantees the output distribution is exactly the target model's.
Losslessness is the crucial property: this is not approximation, it is a latency optimization with provably identical output distribution (Leviathan et al. and Chen et al., 2023).
Expected speedup rises with the acceptance rate, which depends on drafter-target agreement; typical production gains are around 2-3x on per-request latency.
Design space as of early 2026: separate small draft models; self-speculative approaches with extra decoding heads on the target model (Medusa, 2023; EAGLE, 2024, the strongest open family); and n-gram or retrieval drafters that copy from the prompt, which work startlingly well for editing and RAG workloads where output substantially echoes input.
The trade-offs: wasted compute on rejected tokens (harmful exactly when the system is compute-bound, so speculation and large-batch serving partially conflict), drafter maintenance alongside every target-model update, and workload sensitivity, since acceptance rates fall on unpredictable text; agent workloads, full of boilerplate tool-call syntax and echoed code, are on the favorable end.

## 6. Quantization

### 6.1 Weights

Quantization stores parameters in fewer bits and dequantizes on the fly; since decode time is bytes moved over bandwidth, halving weight bytes directly speeds decode and halves weight memory, independent of any accuracy consideration.
The practical ladder as of early 2026: BF16/FP16 as the reference; FP8 as the near-free serving default on Hopper/Blackwell-class hardware (hardware-accelerated, typically negligible quality loss); INT8 weight quantization as a mature safe point; 4-bit weight-only (GPTQ and AWQ as the standard post-training methods, plus the GGUF k-quant family in local inference) as the aggressive tier where quality loss becomes measurable but often acceptable; below 4 bits, degradation grows quickly and unevenly.
Post-training quantization needs only a small calibration set: GPTQ minimizes layerwise reconstruction error with second-order information, AWJ scales channels to protect the activation-salient weights; both run in hours, which is why the open ecosystem quantizes everything within days of release.
The honest caveats: perplexity understates the damage, with instruction following, long-context retrieval, and multi-step agentic reliability degrading before perplexity visibly moves; quality loss concentrates unevenly (outlier channels, specific capabilities); and a quantized model is a different model that must re-pass your agent's regression evals (Volume 10), not a free lunch.

### 6.2 KV cache

The same logic applies to the cache, which at long context outweighs the weights (Section 2): FP8 KV storage is now routine in serving engines with small measured loss, INT4 KV roughly quarters cache traffic and capacity at a real but often tolerable cost, and attention is empirically more sensitive to key precision than value precision, so asymmetric schemes quantize K less aggressively than V.
KV quantization composes multiplicatively with GQA: 8x fewer heads times 4x fewer bytes is a 32x reduction against the FP16 full-MHA baseline, which is the kind of arithmetic that turns million-token contexts from impossible to merely expensive.

## 7. Why latency and cost behave the way they do

### 7.1 The latency model

Two metrics with different physics: TTFT (time to first token) is dominated by queueing plus prefill, so it grows roughly linearly with prompt length; TPOT (time per output token, the steady decode rate) is set by memory bandwidth, batch load, and speculation luck, and is roughly independent of prompt length once decoding.
Total latency = TTFT + output_tokens * TPOT.
Consequences you will design around: long contexts hurt the start of the response, not the streaming rate; output length is the lever on total time you control most directly (Chapter 03's verbosity economics, now with a latency face); streaming exists because TTFT plus visible progress is psychologically acceptable where the same total latency unstreamed is not; and an agent's end-to-end latency is the sum over loop steps of (TTFT_i + generation_i), so step count and per-step prompt length multiply, which is why chatty multi-step loops feel slow even on fast models.

### 7.2 The cost model

Providers price input and output tokens separately with output typically 3-5x input (spot-check current price pages; the ratio, not the number, is the stable fact).
The ratio is grounded in Section 3: an input token is one slice of a high-intensity parallel prefill, an output token is a full memory-bound sequential pass, so their marginal costs genuinely differ by multiples.
For agents, recall Chapter 03's loop arithmetic: resending a growing history makes cumulative input tokens scale roughly quadratically in steps, so despite the per-token price ratio, input spend usually dominates agent bills, and the two structural remedies are prompt caching (next section) and context compaction (Volume 06).
Self-hosting arithmetic differs: you pay for GPU-hours, so cost per token is set by achieved utilization, and the provider-versus-self-host decision is largely "can you keep a batch full"; Volume 12 does this arithmetic in full.

## 8. Prompt caching

### 8.1 The mechanics

Two requests sharing an identical token prefix share identical KV-cache contents for that prefix, because causal attention makes position t's K and V depend only on tokens up to t.
Prompt caching stores the prefix's KV tensors and, on a matching later request, skips that portion of prefill entirely; the effect is large because agent requests are extreme prefix-repeaters, resending the same system prompt, tool definitions, and conversation head every step.
The identity requirement is exact and token-level: any divergence point invalidates everything after it, so caching is a prefix technology, and one changed byte early in the prompt (a timestamp, a random id, a reordered tool definition) destroys all downstream reuse.

### 8.2 Provider shape, as of early 2026

Anthropic exposes explicit cache_control breakpoints marking prefix boundaries, with a default cache lifetime on the order of minutes (refreshed on use, and an optional longer paid tier), cache writes priced at a premium over base input (1.25x for the default tier), and cache reads priced at 0.1x base input; OpenAI ships automatic prefix caching with a roughly 0.5x read discount and no write premium; Google offers both implicit caching and explicit cached-content objects with storage-time billing.
Check current pricing pages before quoting any of these figures; the design axes (explicit versus automatic, read discount, write premium, lifetime) are the stable structure.
The arithmetic that matters: at a 0.1x read price, a cached 20K-token agent prefix costs the input-token equivalent of 2K tokens per step, a 10x cut on the dominant cost line of Chapter 03's loop example, and TTFT falls in proportion to the skipped prefill, so caching is simultaneously the top cost and top latency optimization for agents.

### 8.3 The discipline it imposes

Prompt caching converts prompt layout from a style question into a performance contract, and the rules follow mechanically from prefix identity:

- Order by stability: system prompt and tool definitions first, stable conversation history next, volatile per-step material (latest tool results, current query) last.
- Never put entropy early: timestamps, request ids, or "current date" lines near the top of the prompt silently zero the cache; inject volatile facts at the end or via a late message.
- Append, do not rewrite: an agent loop that edits earlier history (resummarizing, reordering messages) breaks its own prefix; compaction strategies (Volume 06) must be cache-aware, compacting rarely and at deliberate breakpoints rather than continuously.
- Keep tool definitions byte-stable across steps and deployments; serialization nondeterminism (dict ordering, float formatting) is a classic silent cache-killer.
- Mind lifetimes: cache expiry on the order of minutes means bursty agent loops ride the cache while slow human-in-the-loop sessions repeatedly miss it, which changes the economics of the two usage patterns.

The through-line of this chapter, and the reason it anchors the production volumes: the KV cache made generation linear, its memory cost made batching and architecture bend around it, its bandwidth cost priced output over input, its prefix structure made caching possible, and an agent engineer who has internalized this one data structure can derive most serving behavior, most pricing structure, and most prompt-layout best practices from first principles.

## Exercises

1. Compute per-token and 128K-context KV-cache sizes for three real open geometries (an 8B, a 70B, and one MLA or sliding-window model), using each model's published layer count, KV-head count, and head dimension; present a table in FP16 and FP8 and identify which architecture choice buys the largest reduction for each model.
2. Derive the naive single-request decode latency floor for a 70B FP16 model on hardware with 3.35e12 bytes/s of bandwidth (weights plus a 32K-token cache per step), then recompute at batch size 32 assuming weight traffic amortizes and KV traffic does not; state which term dominates in each regime.
3. Measure prefill-versus-decode empirically on any local model (llama.cpp or vLLM): plot TTFT against prompt lengths of 128, 1K, 8K, 32K tokens at fixed output length, and TPOT against the same, then explain both curves with the arithmetic-intensity argument.
4. Implement toy speculative decoding with two sizes of one open model family (for example 1B drafting for 8B): draft k=4 tokens, verify with the target in one pass, apply the acceptance rule, and report acceptance rate and wall-clock speedup on prose versus code prompts.
5. Quantize one open model to INT8 and 4-bit with a standard tool (AWQ or GPTQ pipelines, current as of early 2026), then evaluate all three variants not on perplexity but on 20 structured tool-call generations, and report the format-validity rate at each precision.
6. Instrument a real multi-step agent session: log per-step input tokens, output tokens, TTFT, and total latency; then compute the total bill with and without prompt caching using a current provider price page, and identify the single prompt-layout change that most improves the cached fraction.
7. Audit any agent prompt you have shipped against the five cache-discipline rules of Section 8.3, find every cache-breaking element, and produce the reordered layout with an estimate of tokens moved from full-price to cached-price per step.

## Godhood check

You are ready for Volume 02 when you can do all of the following without notes.

- Write the KV-cache size formula from memory, compute it for a named model geometry, and explain the causal chain from cache size to batch size to token price.
- Explain arithmetic intensity, classify prefill and decode against a stated hardware FLOPs-per-byte ratio, and state what batching changes and what it cannot amortize.
- Describe continuous batching and PagedAttention as solutions to raggedness and fragmentation respectively, including one trade-off each (latency jitter; block-management complexity).
- Explain why speculative decoding is lossless, what determines its speedup, and why it conflicts with compute-bound large-batch serving.
- Give the weight-quantization ladder with its two honest caveats (perplexity understates agentic damage; re-evaluate before shipping), and the GQA-times-KV-quant composition arithmetic.
- Decompose latency into TTFT and TPOT, attribute each to its bottleneck, and explain the input/output price ratio from serving physics.
- State the exact-prefix rule of prompt caching, the read/write/lifetime pricing axes, and recite the five prompt-layout disciplines with the mechanism behind each.
- Trace one full causal story from "attention needs past keys and values" to "put your timestamp at the bottom of the prompt" without skipping a link.
