---
type: playbook
track: [ai-eng, sde, low-latency]
level:
status: draft
last_reviewed:
sources: [https://docs.anthropic.com/en/docs/build-with-claude/prompt-caching, https://www.anthropic.com/engineering/effective-context-engineering-for-ai-agents]
---

# Cost and latency

Practical and often asked.
The strongest answers do arithmetic out loud: the quadratic growth of input tokens and the effect of caching.

---

## Q47. How do you reduce agent cost and latency?

| Lever | Cost | Latency | Note |
| --- | --- | --- | --- |
| Prompt caching with a stable prefix | Large cut | Lower TTFT | Order: tools, system prompt, then history; never put timestamps at the top |
| Smaller models for easy steps | Large cut | Faster | Routing, extraction, summarization |
| Fewer steps through better tools | Cut | Faster | One consolidated tool beats five narrow ones |
| Parallel tool calls | Neutral | Faster | Independent calls only |
| Truncate or summarize tool outputs | Cut | Faster | Cap tool results; return IDs and summaries |
| Compact history | Cut | Faster | Stops quadratic growth |
| Batch APIs for offline work | Discounted | Slower | Evals, backfills, nightly jobs |
| Cache tool results | Cut | Faster | Deterministic lookups with a TTL |
| Stream output | Neutral | Better perceived | Show progress on long tasks |
| Early exit | Cut | Faster | Stop when the verifier passes |
| Speculative or parallel guardrails | Neutral | Faster | Run checks alongside generation |

Measure **cost per successful task**, not cost per call: a cheaper model that fails twice as often can cost more.

Go deeper: [Cost Engineering](../Agentic_AI_Zero_to_Godhood/Volume_12_Production_Engineering/Chapter_03_Cost_Engineering.md), [Caching and Context Economics](../Agentic_AI_Zero_to_Godhood/Volume_06_Memory_and_Context_Engineering/Chapter_07_Caching_and_Context_Economics.md).

---

## Q48. How would you estimate cost for an agent use case?

```text
cost/day = tasks/day * sum over steps of (input_tokens * input_price + output_tokens * output_price)
           adjusted for cache writes and cache reads
```

Input tokens grow each step because history accumulates.
With a fixed prefix P and d new tokens per step (tool result plus assistant output), step i reads `P + d * (i - 1)` tokens, so n steps read `n * P + d * n * (n - 1) / 2`: **quadratic in the step count** without compaction.
Bringing up that quadratic effect shows real understanding.

**Worked example** with illustrative prices ($3 per million input tokens, $15 per million output tokens, cache writes at 1.25x and cache reads at 0.1x of the input price; check current pricing):

- Prefix P = 3,000 tokens (system prompt and tools), d = 800 new tokens per step, 200 output tokens per step, n = 10 steps.
- Total input = 10 * 3,000 + 800 * 45 = **66,000 tokens**; total output = **2,000 tokens**.

| Scenario | Calculation | Cost per task | 10,000 tasks/day |
| --- | --- | --- | --- |
| No caching | 66,000 * $3/M + 2,000 * $15/M | $0.228 | $2,280 |
| Prompt caching | 10,200 written * $3.75/M + 55,800 read * $0.30/M + output $0.030 | $0.085 | $850 |

Caching cuts this task's cost by about 63%.

Growth with step count (same P and d):

| Steps | Total input tokens |
| --- | --- |
| 10 | 66,000 |
| 20 | 212,000 |
| 40 | 744,000 |

Doubling the steps from 20 to 40 multiplies input tokens by 3.5x; this is why compaction and fewer steps matter.

---

## Expansion questions (L1-L8)

**L1. Break down the latency of one agent step.**
Network plus queueing, prefill (TTFT, grows with uncached input), decode (output tokens times TPOT), then tool execution; a task is the sum over steps plus any human wait.
Decode usually dominates when outputs are long; tools dominate when they call slow APIs.

**L2. The agent takes 45 seconds per task and the target is 10. Where do you start?**
Measure first: trace one task and attribute time per span.
Then remove steps (better tools, plan-and-execute), parallelize independent tool calls, cache the prefix, shorten outputs, move easy steps to a faster model, and stream progress.
State the expected bottleneck before optimizing, then verify with the trace.

**L3. How does prompt caching work, and what breaks it?**
The provider stores the KV cache for a prefix; a later request with the byte-identical prefix reuses it.
Anything that changes early in the prompt breaks it: timestamps, per-request IDs, reordered tools, a changed system prompt.
Caches expire after a short time-to-live, so bursts of related requests benefit most.

**L4. How do you attribute cost to teams or customers?**
Route every call through an LLM gateway that tags requests with tenant, feature, and task IDs, records token usage, and enforces budgets.

**L5. When is a bigger model cheaper?**
When it finishes in fewer steps or fewer retries: cost per successful task, not per token, is the metric.

**L6. How do you set a spend limit that cannot be bypassed by the agent?**
Enforce it in the gateway or orchestrator, outside the model: count tokens and tool costs per task, and stop the loop at the limit.

**L7. Estimate the monthly bill for a support agent: 50,000 conversations, 6 steps, 4,000-token prefix, 600 new tokens per step, 250 output tokens per step, same illustrative prices, no caching.**
Input per conversation: 6 * 4,000 + 600 * 15 = 33,000 tokens, so $0.099; output: 1,500 tokens, so $0.0225; total about $0.12, or about $6,100 per month.
Then say how caching and a smaller routing model would cut it, and that the number to watch is cost per resolved ticket.

**L8. How do you trade off latency and quality in a live product?**
Set a latency budget per surface, use the fastest model that meets the quality bar on your eval, escalate to a stronger model only on failure or low confidence, and show partial results while slow work continues.

Go deeper: [Latency Engineering](../Agentic_AI_Zero_to_Godhood/Volume_12_Production_Engineering/Chapter_02_Latency_Engineering.md), [Capacity and Quotas](../Agentic_AI_Zero_to_Godhood/Volume_12_Production_Engineering/Chapter_06_Capacity_and_Quotas.md).
