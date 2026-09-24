---
type: playbook
track: [ai-eng, sde, low-latency]
level:
status: draft
last_reviewed:
sources: [https://arxiv.org/abs/1706.03762, https://arxiv.org/abs/2104.09864, https://arxiv.org/abs/2309.06180, https://arxiv.org/abs/2205.14135, https://arxiv.org/abs/2211.17192, https://arxiv.org/abs/2210.17323, https://arxiv.org/abs/2306.00978, https://arxiv.org/abs/2106.09685, https://arxiv.org/abs/2305.14314, https://arxiv.org/abs/2305.18290, https://github.com/ggml-org/llama.cpp]
---

# LLM fundamentals and inference

Expect a few questions here, and expect "how much memory" follow-ups.
Quantization is on the resume, so the numbers in F1-F3 must be fluent.

---

## Q39. How does a transformer work, briefly?

- Tokens are embedded and given positional information (usually rotary position embeddings, RoPE, now).
- Stacked layers of multi-head self-attention, `softmax(Q K^T / sqrt(d)) V`, plus feed-forward networks (often gated, often mixture-of-experts), with residual connections and normalization.
- Decoder-only models predict the next token autoregressively, with a causal mask so each token sees only earlier tokens.
- Attention is O(n^2) in sequence length for compute; the KV cache makes each new decode step O(n).
- Grouped-query attention (GQA) shares key and value heads across query heads to shrink the KV cache.

Go deeper: [The Transformer From Scratch](../Agentic_AI_Zero_to_Godhood/Volume_01_LLM_Foundations/Chapter_02_The_Transformer_From_Scratch.md).

---

## Q40. What is the KV cache?

- Cached key and value tensors from previous tokens, so each new token only computes its own query against them.
- It makes decoding **memory-bandwidth-bound**: each step reads all weights and the whole cache to produce one token.
- **PagedAttention** (vLLM) manages the KV cache in fixed-size pages, like virtual memory, which removes fragmentation and allows high batch throughput and prefix sharing.
- **Prompt caching** reuses the KV cache for shared prefixes across requests.
  This is huge for agents: the system prompt and tool definitions are stable, and each step's history is a prefix of the next step's, so cost and time-to-first-token drop sharply.

**Worked size:** `kv_bytes_per_token = 2 (K and V) * layers * kv_heads * head_dim * bytes_per_value`.

| Model shape | Per token (FP16) | 8K context | 128K context |
| --- | --- | --- | --- |
| 8B class: 32 layers, 8 KV heads, head_dim 128 | 128 KiB | 1 GiB | 15.6 GiB |
| 70B class: 80 layers, 8 KV heads, head_dim 128 | 320 KiB | 2.5 GiB | 39 GiB |

Per sequence; multiply by the batch size.
This is why long-context serving is limited by memory, not by compute.

Go deeper: [Inference Mechanics](../Agentic_AI_Zero_to_Godhood/Volume_01_LLM_Foundations/Chapter_07_Inference_Mechanics.md).

---

## Q41. What are quantization, GGUF, AWQ, and GPTQ?

- Quantization reduces weight precision from FP16 to INT8 or INT4, cutting weight memory about 2-4x with a small quality loss; it also speeds decoding, which is bandwidth-bound.
- **GPTQ:** post-training, layer-by-layer quantization that uses approximate second-order information to minimize output error.
- **AWQ:** activation-aware; finds the small fraction of salient weight channels (by activation magnitude) and protects them with scaling.
- **GGUF:** the llama.cpp file format, with k-quants (for example `Q4_K_M`, about 4.85 bits per weight) aimed at CPU and Apple Silicon, plus GPU offload.
- Other formats to recognize: FP8 on recent GPUs, bitsandbytes NF4 (used by QLoRA), and KV-cache quantization.
- **Rough weight memory:** `params_B * bits_per_weight / 8` GB.

| Model | FP16 | 8-bit | Q4_K_M (about 4.85 bpw) |
| --- | --- | --- | --- |
| 8B | 16 GB | 8 GB | 4.9 GB |
| 70B | 141 GB | 71 GB | about 43 GB |

Add the KV cache (Q40) and 1-2 GB of runtime overhead.

- **The hardware-aware layer** in my platform: detect VRAM and RAM, then choose the model size and quantization that fit with headroom, then cap the context length to what is left (see R3 in [Pitch and Resume](01-Pitch-and-Resume-Deep-Dives.md)).
- **Serving:** llama.cpp, Ollama (a wrapper around llama.cpp), vLLM, SGLang, TGI, TensorRT-LLM.

---

## Q42. Other inference optimizations?

| Technique | What it does | Helps |
| --- | --- | --- |
| Speculative decoding | A small draft model proposes several tokens; the big model verifies them in one parallel pass; output distribution is unchanged | Latency |
| Continuous batching | New requests join the batch at each decode step instead of waiting | Throughput |
| FlashAttention | Tiled, IO-aware exact attention that avoids materializing the n x n matrix | Speed and memory at long context |
| Tensor and pipeline parallelism | Split layers or matrices across GPUs | Fitting large models |
| Prefix caching | Reuse KV for shared prefixes | TTFT, cost |
| Distillation | Train a small model on a large model's outputs | Cost, latency |
| Structured or constrained decoding | Mask invalid tokens with a grammar or JSON schema (Outlines, XGrammar, llama.cpp GBNF) | Guarantees parseable tool calls |

Constrained decoding is especially relevant for small local models, which otherwise produce malformed tool calls.
It guarantees syntax, not correct semantics, so still validate arguments.

---

## Q43. What are sampling parameters?

- **Temperature:** divides the logits before softmax; lower is more deterministic.
- **Top-p (nucleus):** sample from the smallest set of tokens whose cumulative probability is at least p.
- **Top-k:** sample from the k most likely tokens.
- **Min-p:** keep tokens with probability at least min_p times the top token's; common in local inference.
- For agents and tool calling, use low temperature for reliability; use higher temperature for brainstorming or for diversity when voting.
- Reasoning models often fix or restrict sampling parameters; check the provider's docs.

Go deeper: [Sampling and Decoding](../Agentic_AI_Zero_to_Godhood/Volume_02_Working_With_LLMs/Chapter_02_Sampling_and_Decoding.md).

---

## Q44. What are reasoning models and extended thinking, and when do you use them in agents?

- Models trained (often with reinforcement learning on verifiable rewards) to produce long internal reasoning before answering.
- Good for complex planning, math, code, and multi-step tool decisions.
- Cost: latency and output tokens; thinking tokens are billed as output.
- Pattern: a reasoning model for the orchestrator or planner, fast cheap models for workers, routing, and extraction.
- Interleaved thinking between tool calls improves multi-step decisions, because the model reasons about each observation.
- Control the thinking budget per request; more thinking is not always better on simple steps.

Go deeper: [Reasoning Models](../Agentic_AI_Zero_to_Godhood/Volume_01_LLM_Foundations/Chapter_06_Reasoning_Models.md), [Test Time Compute](../Agentic_AI_Zero_to_Godhood/Volume_14_Frontier_and_Capstones/Chapter_02_Test_Time_Compute.md).

---

## Q45. How do you choose a model for an agent?

- Capability on **your** eval set, not public leaderboards.
- Tool-calling reliability (measured as pass^k on your tasks).
- Context length and how well quality holds at that length.
- Latency (TTFT and output tokens per second) and cost.
- Data residency (on-prem versus API) and vendor terms.
- **Model cascade or routing:** try the cheap model first and escalate on low confidence or failed verification.

My harness applies this as policy: tasks are classified into three tiers (frontier reasoning, standard coding, mechanical), then the model within a tier is chosen by live quota headroom; frontier work never trades down for quota (see [Model Routing and Quota](../Agentic-Harness/04-Model-Routing-and-Quota.md)).

---

## Q46. How are models trained for agentic behavior?

- **Pretraining** on large text and code corpora.
- **Supervised fine-tuning** on instructions and tool-use trajectories.
- **Preference optimization:** RLHF with a reward model, or DPO directly on preference pairs.
- **Reinforcement learning with verifiable rewards** (tests pass, the answer checks out) in agentic environments with real tools.
- For my own adaptation: LoRA or QLoRA fine-tuning of small models on successful trajectories, which distills a larger model's behavior.

Go deeper: [Post Training](../Agentic_AI_Zero_to_Godhood/Volume_01_LLM_Foundations/Chapter_05_Post_Training.md), [RL For Agents](../Agentic_AI_Zero_to_Godhood/Volume_14_Frontier_and_Capstones/Chapter_01_RL_For_Agents.md).

---

## Expansion questions (F1-F10)

**F1. What are prefill and decode, and why do they behave differently?**
Prefill processes the whole prompt in parallel and is compute-bound; it determines time to first token (TTFT).
Decode generates one token at a time and is memory-bandwidth-bound; it determines time per output token (TPOT).
Agents are often prefill-heavy (long histories, short outputs), which is why prompt caching helps them so much.

**F2. Estimate decode speed for a model on a single GPU.**
Upper bound for batch size 1: memory bandwidth divided by bytes read per token (roughly the weight size).
An 8B model at 4 bits (about 5 GB) on a GPU with 1 TB/s bandwidth tops out near 200 tokens per second; real numbers are lower.

**F3. Why does quantization speed up decoding?**
Decode is bandwidth-bound, so fewer bytes per weight means fewer bytes moved per token.

**F4. What is a mixture-of-experts model, and why does it matter for serving?**
Each token is routed to a few expert feed-forward blocks, so compute per token tracks the active parameters, but memory must hold all parameters.
A 47B-total, 13B-active model computes like a 13B but needs memory like a 47B.

**F5. What is tokenization, and why should an engineer care?**
Text is split into subword tokens (BPE or similar); costs, limits, and some failures (counting letters, arithmetic on long numbers) come from tokenization.
Different providers tokenize differently, so token counts do not transfer.

**F6. What is the context window, and what is "effective" context?**
The maximum tokens per request; effective context is how much the model uses well, which is often smaller and must be measured on your task.

**F7. What is LoRA, and why is it cheap?**
Freeze the base weights and learn a low-rank update `W + B A` with rank r much smaller than the matrix dimensions; only A and B train, and adapters are small and swappable.
QLoRA does this on a 4-bit quantized base model.

**F8. When would you self-host instead of calling an API?**
Data residency or confidentiality requirements, predictable high volume, latency control, or a fine-tuned small model.
The costs: GPUs, operations, and usually a quality gap to frontier models.

**F9. What is the difference between vLLM and llama.cpp?**
vLLM targets high-throughput GPU serving with PagedAttention and continuous batching; llama.cpp targets portable local inference on CPUs, Apple Silicon, and consumer GPUs with GGUF quantization.

**F10. Why is "temperature 0" not deterministic on hosted APIs?**
Batch composition changes the order of floating-point reductions, mixture-of-experts routing can differ with batch content, and model aliases can change underneath you.
