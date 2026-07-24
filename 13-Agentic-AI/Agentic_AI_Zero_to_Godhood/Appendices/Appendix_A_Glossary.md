# Appendix A: Glossary

A reference glossary of agentic AI terminology, alphabetized.
Each entry is one dense definition; cross-references to other entries are capitalized where useful.
Knowledge as of early 2026; terms tied to fast-moving products or specs are dated where it matters.

## A

**A2A (Agent2Agent protocol)** - An open protocol announced by Google in 2025 for interoperability between independent agents, in which agents advertise capabilities via agent cards and exchange tasks and artifacts over HTTP; complementary to MCP, which connects one agent to tools rather than agents to each other.
**ACI (agent-computer interface)** - The interface layer through which an agent perceives and acts on a computing environment (tool names, argument schemas, output formats, error messages); coined in the SWE-agent paper (2024), which showed that ACI design changes agent success rates as much as model choice.
**Action space** - The set of actions an agent can take at each step, defined by its available tools plus the option to respond in text; a poorly scoped action space is a leading cause of both agent failure and agent risk.
**Agent** - A system in which an LLM directs its own process and tool usage in a loop, deciding dynamically what to do next based on environment feedback, in contrast to a Workflow whose control flow is fixed in code.
**Agent card** - In A2A, a published JSON document describing an agent's identity, capabilities, endpoints, and auth requirements, used for discovery by other agents.
**Agent harness** - The non-model code surrounding an LLM that turns it into an agent: the loop, tool execution, context management, permissions, retries, and state persistence; harness quality often dominates model quality in end-to-end results.
**Agent loop** - The core cycle of agentic systems: the model receives context, emits either a message or tool calls, the harness executes tools and appends results to context, and the cycle repeats until a stop condition; everything else in agent engineering decorates this loop.
**Agentic RAG** - Retrieval driven by the agent loop rather than a fixed pipeline: the model decides whether to search, formulates and reformulates queries, inspects results, and iterates until it has enough evidence, trading latency and cost for recall and adaptivity.
**Alignment** - The property that a model's behavior matches the intentions and values of its principals; for agents this extends beyond text outputs to actions with real-world side effects.
**Ambient agent** - An agent triggered by events (incoming email, CI failures, calendar changes) rather than direct user prompts, running continuously in the background and escalating to humans only when needed.
**Approval gate** - A control point where the agent pauses before a consequential action (payment, deletion, external send) and requires explicit human confirmation before proceeding; the standard mitigation when an action is irreversible or high-blast-radius.
**Attention** - The transformer mechanism by which each token position computes a weighted mixture over other positions using query-key similarity, letting the model relate any two tokens in the context regardless of distance.
**Attention sink** - The empirical tendency of attention mass to concentrate on the first few tokens of a sequence; exploited by streaming-inference techniques that keep initial tokens resident when evicting the KV cache.
**Autoregressive generation** - Producing output one token at a time, each conditioned on all previous tokens; the reason output tokens are far more expensive in latency than input tokens, and the reason agent steps serialize.

## B

**Batch API** - Provider offering that trades latency for cost by processing requests asynchronously (typically within 24 hours at roughly half price); well suited to offline evals, bulk data extraction, and non-interactive agent runs.
**Benchmark contamination** - Leakage of benchmark problems or solutions into a model's training data, inflating scores without real capability; a chronic problem for static benchmarks and a core argument for held-out, refreshed, or execution-based evals.
**Best-of-n sampling** - Generating n candidate outputs and selecting the best via a verifier, reward model, or judge; a simple form of parallel test-time compute that helps most when a reliable selector exists.
**BM25** - A classic lexical ranking function based on term frequency and inverse document frequency; still a strong retrieval baseline, and the standard sparse half of Hybrid search because it handles exact identifiers, error codes, and rare terms that embeddings blur.
**Browser agent** - An agent that operates a web browser, either through the DOM and accessibility tree or through pixels and mouse/keyboard actions, to complete tasks on websites; evaluated by benchmarks like WebArena and BrowseComp-style tasks.
**Budget forcing** - A test-time technique that controls how long a reasoning model thinks by suppressing or forcing continuation of its thinking phase (for example appending "Wait" to extend reasoning); named in the s1 paper (2025).

## C

**Chain of thought (CoT)** - Intermediate reasoning text a model produces before its answer; prompting for it (Wei et al., 2022) markedly improves multi-step task accuracy, and training on it is the basis of modern reasoning models.
**Checkpointing** - Persisting agent state (conversation, plan, artifacts, environment) at intervals so a long-running run can resume after failure instead of restarting; a production necessity for tasks running minutes to hours.
**Chunking** - Splitting documents into retrievable units for RAG; the trade-off is that small chunks improve retrieval precision while losing surrounding context, and the main strategies are fixed-size, recursive/structural, semantic, and contextualized chunking.
**Compaction** - Replacing the oldest portion of a conversation with a model-written summary when the context window nears its limit, preserving decisions, constraints, and unresolved threads so the agent can continue indefinitely; lossy by construction, so what the compaction prompt preserves is a first-class design decision.
**Computer use** - Agent operation of a full graphical desktop through screenshots and synthesized mouse/keyboard events, first shipped as a public API capability by Anthropic in October 2024; the most general and currently least reliable actuation modality.
**Confused deputy** - A security failure where a privileged component (the agent) is tricked by an unprivileged party (attacker-controlled content) into misusing its authority; prompt injection attacks on tool-using agents are confused-deputy attacks.
**Constitutional AI (CAI)** - Anthropic's training method (Bai et al., 2022) in which a model critiques and revises its own outputs against a written set of principles, and preference labels for RL are generated by AI feedback against that constitution rather than by humans (RLAIF).
**Context engineering** - The discipline of deciding what enters the model's context window at each step (instructions, tools, retrieved knowledge, history, tool results) and what stays out; the successor framing to prompt engineering for agentic systems.
**Context poisoning** - Corruption of an agent's context by false or adversarial content (a hallucinated claim, an injected instruction, a bad tool result) that then compounds because later steps condition on it.
**Context rot** - The empirical degradation of model performance as input length grows, even well below the advertised context limit and even on simple tasks; documented systematically in a 2025 Chroma technical report, and a core motivation for compaction, retrieval, and minimal-context design.
**Context window** - The maximum number of tokens a model can attend over in one call; frontier windows are 200K to 1M+ tokens as of early 2026, but effective usable context is smaller in practice due to context rot.
**Continuous batching** - Inference-server scheduling that admits and retires requests at token granularity rather than batch granularity, greatly improving GPU utilization for mixed-length workloads; standard in vLLM-class servers.
**Cross-encoder** - A relevance model that scores a query and document jointly in one forward pass, more accurate than a Bi-encoder but too slow for first-stage search; the standard architecture for Rerankers.

## D

**Deep research agent** - An agent that autonomously conducts multi-step research (searching, reading, cross-checking, and synthesizing sources into a cited report) over minutes to hours; popularized by Google's, OpenAI's, and Anthropic's deep research products in 2024-2025.
**Distillation** - Training a smaller model to imitate a larger model's outputs or reasoning traces; DeepSeek-R1's distilled variants (2025) showed that reasoning ability transfers to small models via plain SFT on traces.
**DPO (Direct Preference Optimization)** - A post-training method (Rafailov et al., 2023) that optimizes a policy directly on preference pairs with a classification-style loss, removing the separate reward model and RL loop of RLHF at the cost of less flexibility for online or verifiable rewards.
**Dual-LLM pattern** - A prompt-injection defense (articulated by Simon Willison, 2023) in which a privileged LLM with tool access never reads untrusted content directly, delegating that reading to a quarantined LLM whose outputs are treated as data, not instructions.
**Dynamic tool discovery** - Loading tool definitions on demand (by search or namespace listing) rather than injecting all tools into every context; the standard mitigation when a system has hundreds or thousands of tools.

## E

**Egress control** - Restricting what network destinations and data an agent's environment can reach, so that even a fully compromised agent cannot exfiltrate secrets; the infrastructure-level backstop for the Lethal trifecta.
**Elicitation (MCP)** - An MCP capability by which a server asks the client to gather structured input from the user mid-operation (added to the spec in 2025), enabling interactive server flows without the server owning the UI.
**Embedding** - A dense vector representation of text (or other data) positioned so that semantic similarity maps to geometric proximity; the substrate of vector search and semantic memory.
**Environment** - Everything the agent acts on and observes: filesystem, shell, browser, APIs, databases; agent engineering treats environment design (what is visible, what is permitted, what is reversible) as equal in importance to prompt design.
**Episodic memory** - Memory of specific past events or sessions ("what happened"), as opposed to Semantic memory ("what is true"); in agents, typically implemented as stored, searchable session summaries or trajectories.
**Eval** - A repeatable measurement of system quality: a task set, a way to run the system on it, and graders (exact match, code execution, LLM-as-judge, human) producing scores; the agent-engineering analogue of a test suite.
**Evaluator-optimizer** - A workflow pattern in which one model call produces work and another grades it against criteria, looping until the grade passes or a budget is exhausted; one of the five workflow patterns in Anthropic's Building Effective Agents (2024).
**Exfiltration** - Unauthorized movement of sensitive data out of a system; for agents, typical channels are tool calls that send data externally, markdown image URLs that encode data in query strings, and writes to attacker-readable locations.
**Extended thinking** - A mode in which the model emits a long private reasoning phase before its visible answer, with the thinking budget controllable per request; Anthropic's product name for reasoning-model inference (2025-era API).

## F

**Few-shot prompting** - Including worked examples in the prompt so the model infers the task format and style from them; the most reliable prompting lever for output-format fidelity short of fine-tuning.
**Fine-tuning** - Continuing training on task-specific data to change model behavior; in agent systems it is usually the last resort after prompting, retrieval, and tool design, because it freezes behavior against a moving model landscape.
**Frontier model** - A model at or near the current capability frontier (as of early 2026: Claude, GPT, and Gemini flagship lines, plus leading open-weight models such as DeepSeek and Llama releases); "frontier" is a moving label and should always be date-stamped.
**Function calling** - The provider API mechanism (OpenAI, June 2023, followed by all major providers) by which a model emits a structured request to invoke a developer-defined function with JSON arguments, and the developer returns results; the primitive underlying all tool use.

## G

**GAIA** - A benchmark of real-world assistant questions (Mialon et al., 2023) that are easy for humans but require tool use, browsing, and multi-step reasoning from models; a standard general-assistant agent benchmark with three difficulty levels.
**Golden dataset** - A curated, human-verified set of task-answer pairs used as the trusted core of an eval; small and correct beats large and noisy, because grader noise caps the signal an eval can produce.
**Greedy decoding** - Always selecting the highest-probability next token (temperature 0); maximally deterministic in intent but not a determinism guarantee in practice due to floating-point nonassociativity and batching effects in serving.
**Grounding** - Constraining model outputs to supplied evidence (retrieved documents, tool results) and ideally citing it, as opposed to answering from parametric memory; the central quality property of RAG systems.
**GRPO (Group Relative Policy Optimization)** - An RL algorithm introduced in DeepSeekMath (2024) that replaces PPO's learned value function with a baseline computed from the mean reward of a group of sampled responses to the same prompt; cheaper than PPO and the algorithm behind DeepSeek-R1.
**Guardrail** - A runtime check outside the model that constrains inputs or outputs (input classifiers, output filters, schema validators, action policies); useful defense in depth, but insufficient alone against adaptive prompt injection.

## H

**Hallucination** - Confident model output not supported by training data or provided evidence; in agents it is more dangerous than in chat because hallucinated facts become premises for subsequent actions.
**Handoff** - A multi-agent pattern in which one agent transfers control of the conversation to another agent (rather than delegating a subtask and awaiting a result); the core primitive of OpenAI's Swarm and Agents SDK.
**HITL (human in the loop)** - System design in which humans review, approve, or correct agent actions at defined points; the dial runs from approving every action to reviewing only final output, set by the cost of an uncaught error.
**Hybrid search** - Combining lexical retrieval (BM25) and dense vector retrieval, typically fused with reciprocal rank fusion, to get both exact-term precision and semantic recall.
**HyDE (Hypothetical Document Embeddings)** - A retrieval technique (Gao et al., 2022) that asks an LLM to write a hypothetical answer document for a query and embeds that document instead of the query, bridging the query-document vocabulary gap.

## I

**Idempotency** - The property that performing an action twice has the same effect as once; agent tools with side effects should be idempotent (or accept idempotency keys) because agents retry, and retries of non-idempotent actions cause double-sends and double-charges.
**In-context learning** - A model's ability to acquire task behavior from examples and instructions in the prompt without weight updates; the emergent capability (demonstrated at scale by GPT-3, 2020) that makes prompting and agent scaffolding possible at all.
**Indirect prompt injection** - Prompt injection delivered through content the agent processes (a web page, email, document, or tool result) rather than typed by the user; identified by Greshake et al. (2023) and the primary security threat to tool-using agents.
**Instruction hierarchy** - The design principle that instructions from different sources have different authority (system > developer > user > tool results and retrieved content), and models should be trained and prompted to respect that ordering; formalized in OpenAI work of 2024 and implicit in most provider system-prompt designs.
**Instruction tuning** - Supervised fine-tuning on instruction-response pairs that converts a base next-token predictor into a model that follows directions; the first stage of modern post-training pipelines.
**Interleaved thinking** - Reasoning-model operation in which thinking blocks occur between tool calls within a single agentic turn, letting the model reason about each tool result before acting again; supported in Claude-class APIs from 2025.

## J

**Jailbreak** - An attack that manipulates a model into violating its own safety policy (produce disallowed content); distinct from Prompt injection, which manipulates an application into misusing its capabilities, though techniques overlap.
**JSON mode** - Provider inference option that constrains output to valid JSON; weaker than full schema-constrained decoding because validity of syntax does not imply conformance to your schema.
**Just-in-time retrieval** - Loading information into context at the moment it is needed (via tool calls, file reads, or searches) instead of front-loading everything; a core context-engineering strategy that trades extra steps for a smaller, fresher context.

## K

**Knowledge cutoff** - The date after which a model's training data ends; agents compensate for it with retrieval and web tools, and confusing parametric knowledge with retrieved evidence is a standard failure mode.
**KV cache** - The stored key and value tensors for all previous tokens, which make autoregressive decoding O(n) per token instead of O(n^2); its memory footprint is the binding constraint on serving throughput, and reusing it across calls is the mechanism behind Prompt caching.

## L

**Least privilege** - Granting the agent only the capabilities, credentials, and data scopes required for its current task; the single highest-leverage security control for agents because it bounds the blast radius of any successful injection.
**Lethal trifecta** - Simon Willison's name (2025) for the combination of (1) access to private data, (2) exposure to untrusted content, and (3) ability to communicate externally; an agent holding all three can be turned into a data-exfiltration engine by injection, so at least one leg should be removed by design.
**LLM-as-judge** - Using a model to grade another model's output against a rubric; scalable but biased (position bias, verbosity bias, self-preference), so judges must themselves be validated against human labels before their scores are trusted.
**LLM gateway** - A proxy layer between applications and model providers that centralizes routing, credentials, retries, fallbacks, caching, cost accounting, and logging; standard infrastructure in multi-model production deployments.
**Long-horizon task** - A task requiring many sequential, interdependent steps over minutes to hours (or days), where per-step error rates compound; METR's task-length studies (2025) track model progress specifically on this axis.
**LoRA (Low-Rank Adaptation)** - Parameter-efficient fine-tuning that trains small low-rank matrices added to frozen weights (Hu et al., 2021), reducing tuning cost by orders of magnitude and enabling many task adapters over one base model.
**Lost in the middle** - The finding (Liu et al., 2023) that models retrieve information best from the beginning and end of a long context and worst from the middle; a practical argument for putting critical instructions and evidence at context edges.

## M

**MCP (Model Context Protocol)** - An open protocol released by Anthropic in November 2024 that standardizes how applications provide tools, resources, and prompts to LLMs over JSON-RPC, so any compliant client can use any compliant server; the "USB-C of AI integrations" framing, since adopted across major vendors.
**MCP client** - The component inside a host application that maintains a one-to-one connection to an MCP server, forwarding tool calls and results.
**MCP host** - The user-facing application (IDE, chat app, agent harness) that embeds MCP clients, owns the model conversation, and enforces user consent over server capabilities.
**MCP server** - A program exposing tools, resources, and prompts over MCP, connected via stdio (local) or streamable HTTP (remote); the unit of integration and also the unit of supply-chain risk.
**Memory (agent memory)** - Mechanisms for persisting information beyond a single context window: conversation state, note files, vector stores, or structured databases; usually split into working (in-context), episodic, semantic, and procedural memory.
**Mixture of experts (MoE)** - An architecture in which each token is routed to a small subset of expert feed-forward networks, decoupling parameter count from per-token compute; used by Mixtral, DeepSeek-V3-class, and most 2025-era frontier open models.
**Multi-agent system** - A system of multiple LLM agents with distinct contexts, roles, or tools that coordinate on a task; wins when work parallelizes cleanly or contexts must be isolated, and loses to a single agent when subagents need shared context they do not have.
**Multi-turn RL** - Reinforcement learning where the unit of optimization is a whole agent trajectory (many model calls and tool interactions) rather than a single response; the training frontier for agents as of early 2026, with credit assignment across turns as the central difficulty.

## N

**Needle in a haystack** - A long-context eval that hides a fact in a large context and asks the model to retrieve it; popularized by Greg Kamradt's 2023 test, and now considered necessary but far from sufficient evidence of long-context competence.

## O

**Observability** - The ability to inspect what an agent did and why: traces of every model call, tool call, and decision, with token counts, latency, cost, and outcomes; the precondition for debugging and for building evals from production failures.
**Orchestrator-workers** - A pattern in which a lead agent decomposes a task and dispatches subtasks to worker agents (often in parallel), then synthesizes their results; the architecture of Anthropic's multi-agent research system (2025).

## P

**Parallel tool calls** - A model emitting multiple tool calls in one response for concurrent execution; a major latency win for independent reads, and a hazard for order-dependent or side-effecting operations.
**pass@k** - The probability that at least one of k sampled attempts solves a task; measures capability ceiling and rewards lucky variance.
**pass^k** - The probability that all k of k attempts solve the task, introduced by tau-bench (2024) to measure reliability; agent deployments care about pass^k because customers experience the worst run, not the best, and pass^k falls sharply with k even when pass@1 looks strong.
**PEFT (parameter-efficient fine-tuning)** - The family of tuning methods (LoRA, adapters, prompt tuning) that update a small fraction of parameters; the default economic choice for customizing open-weight models.
**Perplexity** - The exponentiated average negative log-likelihood of a corpus under a model; the classic pretraining metric, largely uninformative about agentic competence.
**Plan-and-execute** - An architecture that separates an explicit planning step (produce a task list) from execution steps (do each item, possibly replanning); improves coherence on long tasks and auditability, at the cost of staleness when early assumptions break.
**Policy** - In RL terms, the model being optimized, viewed as a mapping from states (contexts) to action distributions (token or tool choices).
**PPO (Proximal Policy Optimization)** - The clipped-objective policy-gradient algorithm (Schulman et al., 2017) used in classic RLHF; stable but heavy, since it requires a separate value model and reward model alongside the policy.
**Prefill** - The inference phase that processes the entire prompt in parallel to populate the KV cache before decoding begins; compute-bound, whereas decoding is memory-bandwidth-bound, which is why long prompts cost latency up front.
**Process reward model (PRM)** - A reward model scoring each intermediate reasoning step rather than only the final answer; shown by Let's Verify Step by Step (2023) to outperform outcome-only supervision on math.
**Prompt caching** - Provider-side reuse of the KV cache for a repeated prompt prefix, cutting cost and latency of the cached portion dramatically (order of 90 percent cost reduction on cache hits, provider-dependent); the reason agent prompts should keep stable content first and never mutate earlier turns.
**Prompt chaining** - Decomposing a task into a fixed sequence of model calls where each consumes the previous output, optionally with programmatic checks between steps; the simplest workflow pattern.
**Prompt injection** - The class of attacks in which untrusted text is crafted to be interpreted as instructions by the model, overriding developer intent; named by Simon Willison in 2022, unsolved in the general case as of early 2026, and the reason agent security must be architectural rather than purely prompt-based.

## Q

**Quantization** - Representing weights and activations at reduced precision (8-bit, 4-bit) to cut memory and increase throughput, with small accuracy loss when done well; the enabling technique for local and edge deployment of open-weight models.
**Query expansion** - Generating variants or decompositions of a search query (synonyms, sub-questions, HyDE documents) to improve retrieval recall before ranking.

## R

**RAG (retrieval-augmented generation)** - Grounding generation in retrieved documents: index a corpus, retrieve relevant chunks per query, and generate with those chunks in context; introduced as a trained architecture by Lewis et al. (2020) and now standard as an in-context pattern.
**ReAct** - The pattern of interleaving reasoning traces with actions and observations in one loop (Yao et al., 2022); the conceptual ancestor of the modern tool-use agent loop, where the reasoning now lives in native function calling and thinking phases rather than parsed text.
**Reasoning model** - A model post-trained (usually with RL on verifiable rewards) to produce long chains of thought before answering, trading inference tokens for accuracy on math, code, and planning; the o1 (2024) and DeepSeek-R1 (2025) lineage.
**Red teaming** - Adversarial testing of a system by humans or automated attackers to find failures before deployment; for agents this covers injection, excessive-agency abuse, sandbox escape, and social-engineering of approval flows.
**Reflection** - An agent critiquing its own output or trajectory and revising accordingly; helps when the model can actually detect its own errors (verifiable domains) and mostly adds cost when it cannot.
**Reflexion** - A specific framework (Shinn et al., 2023) in which an agent stores verbal self-critiques of failed attempts in episodic memory and conditions retries on them, improving over repeated trials without weight updates.
**Reranker** - A second-stage model (typically a cross-encoder) that rescores the top results of first-stage retrieval for relevance; the highest-precision-per-dollar upgrade in most RAG stacks.
**Retrieval** - Selecting relevant items from a corpus given a query, by lexical, dense, or hybrid means; in agent systems, retrieval quality upper-bounds grounded answer quality.
**Reward hacking** - A policy exploiting flaws in the reward signal instead of solving the task (deleting failing tests, hard-coding expected outputs, flattering the judge); the central failure mode of RL-trained and judge-evaluated agents.
**Reward model** - A model trained on preference data to score outputs, standing in for human judgment inside RLHF; imperfect by construction, hence over-optimizing against it degrades true quality (Goodhart's law in practice).
**RLAIF** - RLHF with AI-generated preference labels in place of human labels, as in Constitutional AI; scales label volume at the cost of inheriting the labeler model's biases.
**RLHF (reinforcement learning from human feedback)** - The post-training pipeline of supervised fine-tuning, reward-model training on human preferences, and RL optimization of the policy against that reward model; established for LLMs by InstructGPT (2022).
**RLVR (reinforcement learning with verifiable rewards)** - RL where rewards come from programmatic checks (unit tests pass, answer matches, constraints hold) rather than learned reward models; named in Tulu 3 (2024), central to DeepSeek-R1, and the dominant training recipe behind 2025-era reasoning and coding-agent gains.
**Rollout** - One sampled trajectory of a policy through an environment (for agents: a full task attempt including all tool calls), used as the unit of experience in RL and of measurement in evals.
**Router** - A component that classifies incoming requests and dispatches them to different models, prompts, or pipelines (cheap model for easy queries, agent for complex ones); the standard first lever for cost and latency.
**Rug pull (MCP)** - A supply-chain attack in which an MCP server changes its tool definitions or behavior after installation and approval, turning a trusted integration malicious; motivates version pinning and re-approval on definition change.

## S

**Sandbox** - An isolated execution environment (container, VM, or restricted process) in which agent-run code and commands are confined, with controlled filesystem, network, and secret access; mandatory for agents that execute arbitrary code.
**Scaffold** - Informal term for the harness and prompting structure around a model; "scaffolding" gains are improvements from better loops, tools, and context rather than better weights.
**Scaling laws** - Empirical power-law relationships between loss and compute, parameters, and data (Kaplan et al., 2020), refined by Chinchilla (2022) to show most large models were undertrained on data relative to size; the planning basis for pretraining investment.
**Scratchpad** - A workspace where the model writes intermediate reasoning, notes, or plans (in-context thinking, a notes file, or a todo list); externalized scratchpads survive compaction, in-context ones do not.
**Self-consistency** - Sampling multiple reasoning paths and taking a majority vote over final answers (Wang et al., 2022); a robust accuracy boost on tasks with discrete answers, at linear cost in samples.
**Semantic memory** - Stored facts and preferences ("the user deploys on Fridays", "the API base URL is X") persisted across sessions, usually as structured records or editable notes retrieved into context when relevant.
**Session** - One continuous interaction between a user (or trigger) and an agent, bounded by its own context and state; session scoping decides what memory persists and what resets.
**Skill (agent skill)** - A packaged, named set of instructions, scripts, and resources an agent loads on demand for a particular kind of task; a progressive-disclosure mechanism (folders with a manifest in Anthropic's 2025 Agent Skills design) that keeps expertise out of the base prompt until needed.
**Speculative decoding** - Accelerating inference by letting a small draft model propose several tokens that the large model verifies in one pass (Leviathan et al., 2023); exact same output distribution, significant latency savings.
**Streaming** - Delivering tokens (and tool-call deltas) to the client as they are generated; essential UX for agents, since it converts dead air into visible progress.
**Structured output** - Constraining generation to a schema, either by grammar-constrained decoding at the token level (guaranteed valid) or by prompting plus validation and retry (probabilistic); the load-bearing joint between model output and downstream code.
**Subagent** - An agent spawned by another agent with its own fresh context and a scoped task brief, returning a distilled result to its parent; the mechanism for parallelism and context isolation in orchestrator-workers systems.
**SWE-bench** - The benchmark family (Jimenez et al., 2023) of real GitHub issues where an agent must produce a repository patch that passes held-out tests; SWE-bench Verified (2024) is the human-validated 500-instance subset that became the headline coding-agent metric.
**Swarm** - Loosely, a multi-agent topology of peer agents passing control via handoffs rather than reporting to an orchestrator; concretely, the name of OpenAI's 2024 experimental framework that popularized the handoff primitive.
**Sycophancy** - Model tendency to agree with the user's stated beliefs or accept corrections even when wrong; in agents it corrupts evaluation-by-user-feedback and any judge that sees the user's opinion.
**System prompt** - The developer-authored instruction block that defines the agent's role, tools policy, constraints, and style, placed at the highest level of the instruction hierarchy; in production it is versioned, tested, and cached like code.

## T

**tau-bench** - A benchmark (Yao et al., 2024) for tool-using customer-service agents interacting with a simulated user under domain policy (airline, retail), scored on final database state and introducing pass^k for reliability; tau2-bench (2025) extends it with a telecom domain and dual-control tasks where the user also acts.
**Temperature** - The softmax scaling parameter controlling sampling randomness; low for tool arguments and structured output, moderate for prose, and irrelevant to determinism guarantees (see Greedy decoding).
**Test-time compute** - Spending more inference compute to get better answers (longer thinking, more samples, search, verification loops); the scaling axis that reasoning models industrialized starting with o1 (2024).
**Token** - The subword unit of model input and output produced by the tokenizer; all context limits, costs, and latencies are denominated in tokens, roughly 3-4 characters of English text each.
**Tokenization** - Mapping text to tokens, typically via byte-pair encoding; explains model weaknesses on character-level tasks and why numbers, code, and non-English text have different effective costs.
**Tool** - A capability exposed to the model as a named function with a description and typed parameters; the unit of agent actuation, and its description is prompt engineering with the same leverage as the system prompt.
**Tool call** - A single model-emitted invocation of a tool with concrete arguments, executed by the harness, whose result is appended to context as a tool result message.
**Tool description** - The natural-language and schema documentation of a tool that the model reads to decide when and how to call it; ambiguous descriptions are a top cause of agent failure, and adversarial descriptions are the vector for Tool poisoning.
**Tool poisoning** - An attack (documented against MCP by Invariant Labs, 2025) in which malicious instructions are hidden inside a tool's description or metadata, visible to the model but not surfaced to the user, steering the agent to exfiltrate data or misuse other tools.
**Top-p (nucleus) sampling** - Sampling restricted to the smallest token set whose cumulative probability exceeds p; the standard companion knob to temperature.
**Trace** - The recorded tree of one request's execution across model calls, tool calls, and subagents, with inputs, outputs, timings, and costs; the primary artifact of agent observability.
**Trajectory** - The full sequence of states, model outputs, tool calls, and observations for one task attempt; the unit RL optimizes, evals grade, and engineers read when debugging.
**Transformer** - The attention-based sequence architecture (Vaswani et al., 2017) underlying all current frontier LLMs; agents inherit its properties, notably parallel prefill, serial decode, and finite attention over a context window.
**Tree of thoughts** - Framing problem solving as search over a tree of intermediate reasoning states with lookahead and backtracking (Yao et al., 2023); the explicit-search ancestor of capabilities that reasoning models now partially internalize.
**Trust boundary** - The line separating content and components of different trust levels (developer instructions vs user input vs internet content vs tool output); agent security consists largely of knowing where these boundaries are and refusing to let instructions cross them upward.
**Turn** - One user-visible exchange in a conversation; a single agentic turn may internally contain many model calls and tool executions.

## U

**User simulator** - An LLM playing the human user in evals of conversational agents (as in tau-bench); enables scale, but simulator quirks become part of what is measured, so its noise must be characterized.

## V

**Vector database** - A store optimized for approximate nearest-neighbor search over embeddings (HNSW and related indexes), with metadata filtering; dedicated products (Pinecone, Weaviate, Qdrant, Milvus, Chroma) compete with pgvector-style extensions to existing databases.
**Verifier** - Any mechanism that checks a candidate solution more cheaply or reliably than producing it (unit tests, compilers, checkers, PRMs); the asymmetry between generation and verification is the engine of test-time compute and RLVR.

## W

**Workflow** - An LLM system whose control flow is fixed in code (chains, routers, fan-outs) with the model filling in steps, as opposed to an Agent that chooses its own control flow; per Anthropic's Building Effective Agents (2024), the right default when tasks are predictable.

## Z

**Zero-shot prompting** - Instructing a model to perform a task with no in-prompt examples; the baseline against which few-shot and fine-tuned approaches are judged.
