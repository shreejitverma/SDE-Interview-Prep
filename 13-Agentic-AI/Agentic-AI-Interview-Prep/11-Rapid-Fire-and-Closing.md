---
type: playbook
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://www.anthropic.com/engineering/building-effective-agents, https://modelcontextprotocol.io/specification, https://genai.owasp.org/llm-top-10/]
---

# Rapid fire and closing

Quick definitions to have ready, questions to ask the interviewer, and a checklist for the last hour.
Each definition should take one breath to say.

## Prompting and generation

| Term | One-line definition |
| --- | --- |
| Chain-of-thought | Step-by-step reasoning before answering |
| Self-consistency | Sample several reasoning paths and take a majority vote |
| Few-shot versus zero-shot | With versus without examples in the prompt |
| Structured outputs | JSON-schema-constrained generation |
| Constrained decoding | Masking invalid tokens with a grammar so output always parses |
| Hallucination | Fluent but unsupported output; mitigate with grounding, citations, verification, and abstention |
| Temperature | Scales logits; lower is more deterministic |
| Top-p | Sample from the smallest token set with cumulative probability at least p |
| System prompt | Highest-priority instructions that set role, rules, and tools |
| Instruction hierarchy | Trained priority: system above user above tool output |

## Agents

| Term | One-line definition |
| --- | --- |
| Agent | An LLM that chooses its own tool calls and when to stop, in a loop |
| Workflow | LLM calls orchestrated along predefined code paths |
| ReAct | Interleaved reasoning, action, and observation |
| Tool calling | Model emits a structured call; your code executes it and returns the result |
| `tool_choice` | Whether the model may, must, or must not call a tool, or a specific one |
| Subagent | A delegated agent with its own clean context that returns a condensed result |
| Handoff | Transfer of conversation control to another agent |
| Orchestrator-workers | A lead model decomposes a task and delegates to workers |
| Evaluator-optimizer | Generator plus critic in a loop |
| Plan-and-execute | Plan up front, execute steps, re-plan on failure |
| Reflexion | Self-critique after failure stored as memory for the next attempt |
| Computer use | An agent operating a GUI via screenshots and mouse and keyboard actions |
| Harness | The runtime around an agent: tools, permissions, hooks, context management |
| Agent skill | A folder of instructions and scripts loaded on demand (progressive disclosure) |
| Human-in-the-loop | Pausing for approval before an action, then resuming from saved state |

## Context and memory

| Term | One-line definition |
| --- | --- |
| Context engineering | Curating every token the model sees at each step |
| Context rot | Accuracy falls as context grows, even within the window |
| Compaction | Summarizing context so a long task can continue |
| Prompt caching | Reusing the KV cache for an identical prompt prefix |
| Episodic memory | Records of past interactions and trajectories |
| Semantic memory | Facts and preferences |
| Procedural memory | Learned skills and instructions |
| Memory poisoning | Planted instructions recalled in a later session |

## Retrieval

| Term | One-line definition |
| --- | --- |
| Embedding | A dense vector that represents meaning |
| BM25 | Sparse keyword ranking with term-frequency saturation and length normalization |
| Hybrid search | Dense plus sparse, usually fused with RRF |
| RRF | Sum of 1/(k + rank) across ranked lists, k = 60 |
| Reranker | A cross-encoder that rescores query-document pairs jointly |
| HNSW | Layered proximity graph for approximate nearest-neighbor search |
| IVF / PQ | Cluster-then-search / compress vectors into codebook indices |
| HyDE | Embed a hypothetical answer instead of the query |
| Contextual retrieval | Prepend document context to each chunk before indexing |
| GraphRAG | Entity graph and community summaries for global questions |
| Agentic RAG | Retrieval as a tool the agent controls and iterates on |
| Faithfulness | Every claim in the answer is supported by the retrieved context |

## Protocols

| Term | One-line definition |
| --- | --- |
| MCP | Open protocol connecting LLM apps to tools and data over JSON-RPC 2.0 |
| MCP primitives | Server: tools, resources, prompts; client: sampling, roots, elicitation |
| Streamable HTTP | MCP's remote transport, replacing HTTP+SSE |
| A2A | Agent-to-agent protocol with Agent Cards and tasks, under the Linux Foundation |
| Agent Card | JSON descriptor of an agent's capabilities and endpoint |

## Evaluation and operations

| Term | One-line definition |
| --- | --- |
| pass@k | Probability at least one of k tries succeeds |
| pass^k | Probability all k tries succeed; the reliability metric |
| LLM-as-judge | A model grading outputs against a rubric; calibrate against humans |
| Trajectory eval | Grading the sequence of actions, not only the outcome |
| Shadow mode | The agent runs alongside production without acting |
| Idempotency | Repeating an operation has the same effect; critical for retries |
| Circuit breaker | Stop calling a failing dependency until it recovers |
| LLM gateway | One front door for all model calls: routing, budgets, caching, logging |

## Models and inference

| Term | One-line definition |
| --- | --- |
| KV cache | Stored keys and values of earlier tokens so decode does not recompute them |
| PagedAttention | KV cache managed in pages for high-throughput batching |
| Prefill / decode | Parallel prompt processing (TTFT) / one-token-at-a-time generation (TPOT) |
| Speculative decoding | Draft model proposes, big model verifies in parallel |
| Quantization | Lower-precision weights for less memory and faster decode |
| GGUF | llama.cpp's quantized model file format |
| LoRA / QLoRA | Low-rank adapter fine-tuning / LoRA on a 4-bit base model |
| Distillation | Train a small model on a large model's outputs |
| Mixture of experts | Each token routed to a few expert blocks; compute of active, memory of total |
| RLVR | Reinforcement learning with verifiable rewards, such as passing tests |

## Security

| Term | One-line definition |
| --- | --- |
| Prompt injection | Untrusted text that overrides the agent's instructions |
| Lethal trifecta | Private data plus untrusted content plus an exfiltration channel |
| Excessive agency | More tools or permissions than the task needs |
| Tool poisoning | Malicious instructions hidden in a tool's description |
| Rug pull | A tool definition that changes after approval |
| Confused deputy | A privileged component tricked into acting for the wrong party |
| Guardrail | A check on inputs, outputs, or actions |

---

## Questions to ask them

Pick two or three that fit the interviewer's role.

**For engineers:**

1. "Are your agents in production today? What's the biggest reliability issue you're fighting?"
2. "How do you evaluate agent quality before shipping? Is there an eval suite in CI, and who owns it?"
3. "Workflows versus autonomous agents: where on that spectrum are your use cases?"
4. "Which model providers and frameworks do you use, and do you self-host any models?"
5. "How do you handle prompt injection and permissions for tools that take actions?"

**For managers:**

6. "What would success look like for this role in the first 90 days?"
7. "How do you decide which problems get an agent and which get a simpler solution?"
8. "What does the path from prototype to production look like here, and where does it usually stall?"

**For leadership:**

9. "Where do you expect the agent roadmap to create the most value in the next year?"
10. "How do you think about build versus buy for models, frameworks, and evaluation tooling?"

---

## Last-hour checklist

- [ ] Say the 90-second and 30-second pitches out loud once each ([Q1](01-Pitch-and-Resume-Deep-Dives.md)).
- [ ] Every **[fill in]** you might mention is either filled or off-limits.
- [ ] Re-read [Claims to avoid](01-Pitch-and-Resume-Deep-Dives.md).
- [ ] Write the agent loop skeleton on paper in under 5 minutes ([Q55](09-Coding-Round.md)).
- [ ] Recite the 9-step design frame ([System Design](08-System-Design.md)).
- [ ] Recite the pass@k versus pass^k difference and the 0.95^20 = 36% compounding number.
- [ ] Recite the cost formula and why input tokens grow quadratically.
- [ ] Know the company's stack, product, and one recent engineering post.
- [ ] Pick your three questions to ask.
- [ ] Water, charger, quiet room, and a blank sheet for diagrams.
