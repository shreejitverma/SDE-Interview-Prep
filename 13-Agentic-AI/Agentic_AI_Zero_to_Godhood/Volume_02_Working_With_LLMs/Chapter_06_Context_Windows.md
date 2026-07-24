# Chapter 06: Context Windows

## What you will master

- What a context window physically is, why it has a limit, and what actually happens as you fill it.
- The cost and latency economics of long context: quadratic attention, linear KV cache, and the billing consequences.
- Long-context behavior in practice: needle-in-a-haystack and why passing it means little, lost-in-the-middle, and context rot.
- Effective versus advertised context, and how to measure the difference for your task.
- The long-context-versus-retrieval decision: when stuffing the window replaces a retrieval pipeline and when it cannot.
- Context-budget engineering: the discipline of treating tokens as a scarce, priced resource.

## 1. What a context window really is

The context window is the maximum number of tokens the model can attend over in one forward pass: input and generated output combined occupy the same window.
It is not a buffer that overflows gracefully; a request whose tokens exceed the limit is rejected (a 400 on both major APIs), and generation that reaches the limit mid-output stops with an explicit stop reason (`model_context_window_exceeded` on newer Anthropic models, distinct from the `max_tokens` cap you set yourself).

Where the limit comes from, mechanically:

- Positional encoding: the model must represent token position, and schemes like RoPE are trained over a finite range; extension tricks (position interpolation, YaRN, NTK-aware scaling) stretch that range, which is how 4k-trained architectures became 128k-plus models, but stretched positions are exactly where quality degrades first.
- Attention cost: self-attention compares every token pair, so prefill compute grows quadratically with input length, and the KV cache (stored keys and values for every token at every layer) grows linearly in memory.
  Serving a 1M-token request means holding a KV cache of tens of gigabytes per request on the provider's accelerators, which is why long context is priced and rate-limited the way it is.
- Training distribution: a model advertising 1M tokens was trained mostly on far shorter sequences, with a comparatively small long-context fine-tuning phase; capability at extreme lengths is real but thinner than at the lengths that dominate training.

The numbers as of early 2026: frontier models advertise 200k as a floor and 1M as the flagship tier (Claude Sonnet's 1M window, GPT-4.1's 1M, Gemini's 1M to 2M lineage), with output caps far smaller than input windows (tens of thousands of tokens, for example 64k to 128k).
Advertised numbers rot quickly; the mechanics above do not.

One more definitional point that trips people: the window is per-request, not per-conversation.
A stateless API re-sends history every turn, so "the conversation exceeded the context window" really means "the rendered prompt for this turn no longer fits", and the remedies (truncation, summarization, compaction) are all edits to what you choose to re-send.

## 2. The economics: cost and latency of long context

Tokens are the billing unit, and the context window is where token counts explode, so context management is cost management.

Cost mechanics to internalize:

- Input tokens are cheaper than output tokens (commonly 3x to 5x cheaper), but input volume in agentic systems is often 10x to 100x output volume, because the whole history, system prompt, and tool results are re-sent every turn.
  In mature agent deployments, input dominates the bill.
- Re-sending is re-processing: without caching, turn N of a conversation pays to re-prefill every token of turns 1 through N-1.
  Cumulative cost of a conversation therefore grows quadratically in its length, which is the single most underestimated line item in agent economics.
- Prompt caching is the counter-mechanism: both providers discount cached prefix tokens heavily (order of 90 percent off reads, with a modest write premium), converting the quadratic re-prefill into something closer to linear, but only if your prompt is engineered as a byte-stable prefix (Chapter 01 and Volume 06).
- Long-context premium tiers exist: providers have priced requests beyond a threshold (for example above 200k input tokens) at elevated per-token rates, because the serving cost is genuinely superlinear.
  Check current pricing before designing a stuff-everything architecture; the premium can erase its simplicity advantage.

Latency mechanics:

- Time-to-first-token grows with input length, since prefill must complete (quadratic compute) before generation starts; a near-window-limit request can take tens of seconds before the first byte on today's serving stacks.
- Per-token generation speed also degrades with a long KV cache, since every new token attends over everything.
- Practical consequence: interactive products have a latency budget that implies a context budget, independent of the model's advertised window; the window is a ceiling, not a target.

## 3. Long-context behavior: what degrades and how

A model that accepts 1M tokens does not use all positions equally well; capability is non-uniform across the window, and you must know the shape of that non-uniformity.

Needle-in-a-haystack (NIAH), popularized by Greg Kamradt's 2023 test against GPT-4 and Claude 2.1: insert one out-of-place fact (the needle) at varying depths in a long distractor corpus (the haystack) and ask for it.
Frontier models now ace simple NIAH near-perfectly across their windows, and vendors advertise those charts.
Why passing means little: simple NIAH is literal retrieval of an obviously distinctive string, requiring no reasoning, no integration across positions, and no resistance to semantically similar distractors.
It is a smoke test; treat a failed NIAH as disqualifying and a passed one as merely non-disqualifying.

Harder probes reveal the real frontier:

- Lost in the middle (Liu et al., 2023): accuracy as a function of the position of relevant information is U-shaped; models use the beginning and end of the context better than the middle.
  The effect persists, attenuated, in current models, and it directly dictates prompt layout: instructions and contracts at the top, the question or task restated at the bottom, and never bury the critical fact at position 60 percent.
- Multi-needle, aggregation, and reasoning variants (RULER, Hsieh et al., 2024; NoLiMa, 2025; LongBench and successors): when the task requires combining several dispersed facts, resisting near-miss distractors, or matching meaning rather than literal strings, effective context shrinks dramatically; RULER's authors found many models claiming 128k+ performed like 32k models under these conditions, and NoLiMa showed steep drops by 32k when lexical overlap is removed.
- Context rot (the term popularized by a Chroma technical report, July 2025): the general phenomenon that per-token reliability declines as input grows, even on trivially simple tasks, and declines faster when distractors are semantically close to the target or when the haystack is coherent prose rather than random shuffle.
  The operational reading: input length is itself a quality variable, and every token you add pays a small reliability tax on every other token.
- Distraction and priming: irrelevant-but-plausible content does not just get ignored at zero cost; it measurably pulls answers off course (the "distractibility" results going back to Shi et al., 2023), which is why "harmless extra context" is not harmless.

Two engineering corollaries.
First, position is a resource: place what matters where the model attends best, and re-state critical instructions near the end of very long prompts.
Second, curation beats accumulation: a smaller, higher-signal context routinely outperforms a larger, noisier one containing strictly more information; this is the empirical foundation of the entire context-engineering discipline in Volume 06.

## 4. Effective versus advertised context

Define effective context for a task as the largest input length at which the model still meets your quality bar on that task.
It is task-dependent, model-dependent, and always at or below the advertised window; the gap between the two numbers is where products silently fail.

How to measure it, concretely:

1. Take your real task (not a synthetic needle): the actual extraction, QA, code comprehension, or summarization you ship.
2. Construct instances at graded lengths (for example 8k, 32k, 64k, 128k, 200k) by embedding the same solvable core in growing amounts of realistic surrounding material, with the target's position varied (start, middle, end) as a second axis.
3. Score with your task grader across length-position cells, several samples per cell.
4. The curve typically shows a plateau then a knee; set your production context budget below the knee with margin, and re-measure per model upgrade, because effective context is exactly the kind of number that shifts across snapshots.

Publish the result internally as a number ("this pipeline is validated to 60k input tokens"), not a vibe ("the model handles long docs fine").
Auxiliary signals worth tracking in production: quality metrics bucketed by input length, and the rate of length-cap rejections and truncations, which together tell you whether you are drifting past your validated regime.

## 5. When long context replaces retrieval, and when it cannot

The perennial architecture question: with million-token windows, do you still need a retrieval pipeline, or do you just include everything?

Cases where window-stuffing genuinely wins:

- The corpus fits comfortably within effective (not advertised) context: a single contract, one repository subsystem, a day of logs, a handful of papers.
  Retrieval adds a lossy, failure-prone stage; if everything fits, inclusion is strictly more faithful and massively simpler to build and debug.
- The task is holistic: cross-document synthesis, whole-codebase refactoring surveys, "read all of this and find the inconsistencies".
  Retrieval presupposes you can name what is relevant before reading; holistic tasks violate that premise, and chunk-by-chunk processing destroys the global view.
- The workload re-reads the same corpus repeatedly with a stable prefix: prompt caching amortizes the corpus to near-zero marginal cost, making the stuffed-window design cheap after the first request; this pattern (cache the corpus, vary the question) is a legitimate RAG replacement for corpora up to the cache-friendly size.

Cases where retrieval remains structurally necessary:

- Scale: corpora beyond the window by orders of magnitude (enterprise knowledge bases, the web, years of tickets); no window growth changes the asymptotics, and a million tokens is a few thousand pages, not a company's documents.
- Cost and latency floors: even below the window limit, paying for 800k mostly-irrelevant tokens per request, at long-context premium rates and multi-second prefill, loses to a retrieval stage that delivers 5k relevant tokens, unless caching fully applies.
- Quality under distraction: per section 3, filling the window with weakly relevant material taxes accuracy on the relevant part; retrieval is not just a cost optimization but a signal-to-noise optimization.
- Freshness and access control: retrieval indexes update incrementally and can enforce per-user document permissions at query time; a stuffed static context can do neither cleanly.

The synthesis that reflects actual practice as of early 2026: the dichotomy is false, and production systems blend the two.
Retrieval got coarser (fetch whole documents or large sections, not 300-token fragments, because the window can afford it), agentic retrieval emerged (the model uses search tools iteratively, reading what it decides it needs, rather than receiving one-shot top-k), and long context serves as the working set while retrieval serves as the storage hierarchy.
Think memory hierarchy, not either-or: the window is RAM, the corpus plus retrieval is disk, and Volume 05 and Volume 06 build on exactly this framing.

## 6. Context-budget engineering

Treat the window as a priced, finite resource with an explicit budget allocation, the way you treat memory in embedded systems.

A working budget for a 200k-window agent turn might allocate: system prompt and tool schemas 5k; retrieved or attached working documents 40k; conversation history after compaction 30k; current tool results 20k; headroom for output and thinking 32k; slack 20k.
The absolute numbers matter less than the practice: every category is measured, capped, and owned by a mechanism (truncation rule, summarizer, eviction policy), so growth in one category cannot silently starve another.

Mechanisms you will implement or configure (developed fully in Volume 06):

- Counting: use the provider's token-counting endpoint (Anthropic `count_tokens`; OpenAI tokenizers) rather than character heuristics; budgets enforced in characters drift by 2x across content types.
- Truncation policies: oldest-first turn dropping is the crude baseline; keep-first-and-last (protect the system prompt and recent turns, drop the middle) respects the U-curve; per-item caps stop one giant tool result from evicting everything else.
- Summarization and compaction: replace evicted history with a model-written summary; provider-side compaction (Anthropic's server-side compaction, beta as of early 2026) and framework auto-compaction do this for you, at the cost of lossy memory whose losses you do not control; anything that must survive verbatim (IDs, code, decisions) needs an explicit home outside the summarized region.
- Structural placement: stable content first for cache efficiency, critical instructions at the edges for the U-curve, volatile content last; these two constraints (cache wants stability at the front, attention favors the edges) jointly determine good prompt layout.
- Observability: log input tokens, cached tokens, and output tokens per request, bucketed by category; a context-budget regression (a tool suddenly returning 10x output) should page someone before the bill does.

## Exercises

1. Measure the latency curve: send prompts of 1k, 10k, 50k, 100k, and 200k input tokens to one model, 10 samples each, and plot time-to-first-token and total time.
   Compute the implied context budget for a product requiring sub-2-second first tokens.
2. Reproduce lost-in-the-middle: build a QA task where the answer-bearing sentence is placed at 10 relative depths within 50k tokens of realistic distractor text; plot accuracy versus depth, then repeat with the question restated at the end of the prompt and compare.
3. Measure your effective context: pick a real task from your work, construct graded-length instances per section 4, and produce the length-quality curve with a marked knee.
   State the production budget you would set and defend the margin.
4. Quantify the re-send tax: run a 30-turn synthetic agent conversation stateless with caching disabled versus enabled (or simulate from token counts and published prices); plot cumulative cost for both and identify where the curves diverge.
5. Run the stuff-versus-retrieve bake-off: take a 300k-token document set and 40 questions; compare (a) full stuffing where it fits on a 1M model, (b) top-k chunk retrieval into a 10k context, and (c) agentic retrieval where the model greps and reads on demand.
   Report accuracy, cost per question, and p95 latency for each.
6. Implement a context budgeter: a function that takes system prompt, history, tool results, and attachments, enforces per-category caps with keep-first-and-last truncation and a summarization fallback, and emits a token-count report per category.
   Wire it into any agent loop and verify no request ever exceeds your configured budget.

## Godhood check

You have mastered this chapter when you can do the following unaided:

- Explain, at the level of positional encodings, quadratic attention, and KV-cache memory, why context windows have limits, why prefill dominates long-context latency, and why providers price long context at a premium.
- Derive why stateless multi-turn cost grows quadratically with conversation length, and show exactly how prefix caching changes the curve and what prompt property it demands.
- Describe NIAH and articulate three specific reasons a perfect NIAH score fails to predict production long-context quality, citing the harder probe families (multi-needle aggregation, lexical-overlap removal, distractor similarity) and the lost-in-the-middle position effect.
- Define effective context, sketch the measurement protocol for a given task, and explain why the number must be re-established on every model upgrade.
- Argue both sides of long-context-versus-retrieval with the four structural limits of window-stuffing (scale, cost, distraction, freshness/permissions) and the three cases where stuffing wins, then present the memory-hierarchy synthesis.
- Design a full context budget for a 200k-window agent, naming every category, its cap, its eviction mechanism, and the two placement constraints (cache stability and edge attention) that determine layout.
