# Appendix D: Interview Drills

Ninety-plus interview questions for agentic AI engineering roles, organized by area, each with a model answer.
Sections mirror the volume ordering of this track, so a weak answer maps directly to the volume that fixes it.
Answers are written the way a strong candidate should speak: direct claims, mechanisms, trade-offs, and date-stamped facts where the field moves.
Six system-design prompts with worked outlines and a rapid-fire round close the appendix.
Knowledge as of early 2026.

## 1. Foundations

**Q1. What is the KV cache and why does it matter for agent workloads?**
During autoregressive decoding, each new token must attend over all previous tokens, so the model caches the key and value tensors for every prior position instead of recomputing them.
This makes per-token cost linear rather than quadratic, but the cache consumes GPU memory proportional to context length, making memory the binding constraint on serving throughput.
Agent workloads are extreme KV-cache consumers because contexts are long and grow monotonically across many calls in a loop.
Provider prompt caching is KV-cache reuse across requests, which is why agents should keep stable prompt prefixes and append-only histories: a mutated early token invalidates everything after it.

**Q2. What did Chinchilla change about how models are trained?**
Kaplan's 2020 scaling laws were read as "parameters matter most", so labs built very large models trained on relatively little data.
Hoffmann et al. (2022) showed that for a fixed compute budget, loss is minimized by scaling parameters and training tokens roughly equally, landing near 20 tokens per parameter.
Their 70B Chinchilla outperformed the 280B Gopher at the same compute, proving most large models of that era were badly undertrained.
The practical legacy is that data quantity and quality became the frontier constraint, and post-Chinchilla models are often trained far beyond compute-optimal token counts because inference cost rewards smaller models.

**Q3. Walk through the RLHF pipeline and explain why a reward model is used instead of direct human labels.**
Stage one is supervised fine-tuning on demonstration data to get an instruction-following policy.
Stage two collects human pairwise preferences over model outputs and trains a reward model to predict them.
Stage three optimizes the policy with RL (classically PPO) against the reward model, with a KL penalty tethering it to the SFT policy.
The reward model exists because RL needs millions of reward evaluations, and humans cannot label in the loop at that rate; the RM amortizes a bounded set of human judgments into an unbounded number of scores.
The cost is Goodhart risk: the policy learns to exploit RM errors, which is why over-optimization degrades real quality and why the KL term exists.

**Q4. Compare DPO and PPO-based RLHF.**
DPO collapses reward modeling and RL into a single classification-style loss directly on preference pairs, using the policy's own likelihood ratios as an implicit reward.
It is dramatically simpler and cheaper: no reward model, no rollouts, no value network, and it became the default for open-weight post-training after 2023.
PPO-style online RL retains advantages: it can use fresh on-policy samples, incorporate non-preference rewards such as verifiable checks, and shape behavior over multi-turn trajectories.
The 2025-era consensus is roughly DPO for offline preference alignment, GRPO-style online RL for reasoning and agentic training where rewards are programmatic.

**Q5. What is RLVR and why did it unlock reasoning models?**
RLVR is reinforcement learning where the reward is a programmatic verification (test passes, answer matches, constraint holds) rather than a learned reward model.
Because the reward cannot be flattered, the policy is pushed toward actually solving tasks, and long chains of thought emerge as instrumentally useful behavior.
DeepSeek-R1 (2025) demonstrated this cleanly: pure RL with verifiable rewards on a strong base model produced extended reasoning, self-checking, and backtracking without supervised reasoning traces.
The limiting factor is that it only works where verification is cheap and reliable, which is why math and code led and why fuzzy domains still rely on judges and preference signals.

**Q6. How does tokenization affect agent systems specifically?**
Tokenization sets the exchange rate between text and cost: code, JSON, numbers, and non-English text tokenize less efficiently than English prose, so verbose tool outputs are disproportionately expensive.
Character-level weaknesses (counting letters, precise string edits) trace to the model seeing subword units, which matters for agents doing exact text manipulation and is a reason to delegate such edits to tools.
Tool schemas and tool results are tokens like everything else, so schema verbosity is a real cost multiplied by every loop iteration.
Practical consequence: measure token footprints of your tools' outputs, truncate and paginate large results, and prefer compact formats for machine-to-model data.

**Q7. Why does temperature 0 not guarantee deterministic outputs?**
Temperature 0 selects the argmax token, but the logits themselves are not bit-stable across runs.
Floating-point addition is nonassociative, and serving systems change reduction orders depending on batch composition, kernel selection, and hardware, so two identical requests can produce logits differing in the last bits.
When two tokens are near-tied, those differences flip the argmax, and one flipped token changes the entire continuation.
MoE models add routing-related variance under batching.
For agents the implication is to design for distributional stability, using evals over many runs and idempotent tools, rather than assuming replayable exactness.

## 2. Working with LLMs

**Q8. What are the options for getting reliable structured output, and how do you choose?**
Three tiers exist: prompt-and-pray with a parser, provider JSON or structured-output modes, and grammar-constrained decoding that masks invalid tokens at generation time.
Constrained decoding guarantees schema-valid output and is the right choice when a downstream system consumes it directly.
Prompting plus validation and retry is more flexible and preserves model quality on tasks where tight constraints hurt reasoning, at the cost of occasional retries.
A common production shape is to let the model reason freely, then emit the final answer through a constrained tool call, separating thinking from serialization.
Always validate semantically as well: schema validity does not mean field values are correct.

**Q9. How does prompt caching work and how do you structure prompts to exploit it?**
Providers cache the KV state of a prompt prefix and reuse it when a later request presents a bit-identical prefix, cutting cost on the cached portion by around 90 percent and slashing prefill latency (Anthropic's cache pricing, current since 2024).
Caching is prefix-based, so ordering is everything: stable content first (system prompt, tool definitions, reference docs), volatile content last (latest user message).
Agent loops benefit automatically if history is append-only; any edit to earlier turns, including reordering tools or injecting a timestamp early in the prompt, invalidates the cache from that point.
This is a primary reason agent frameworks avoid mutating history and put dynamic context in the final message.

**Q10. When do you fine-tune rather than prompt?**
Prompting and retrieval are the default because they iterate in minutes, survive model upgrades, and keep behavior inspectable.
Fine-tune when the behavior is stable and high-volume enough to amortize: enforcing a house style or format at scale, distilling a large model's behavior into a cheaper one, deep domain adaptation where prompting plateaus, or shrinking latency by removing long instructions and examples.
Do not fine-tune to inject facts that change; that is retrieval's job.
The hidden cost is operational: every base-model upgrade forces a retrain and re-eval, so fine-tuning couples you to a model version.

**Q11. Explain bi-encoders versus cross-encoders in retrieval.**
A bi-encoder embeds query and document independently into vectors compared by similarity, so documents are embedded offline and search is a fast nearest-neighbor lookup over millions of items.
A cross-encoder feeds the query and document jointly through the model and scores their interaction directly, which is far more accurate but requires a forward pass per pair.
The standard architecture uses both: bi-encoder (plus BM25) retrieves a candidate set, cross-encoder reranks the top 50 to 200.
This two-stage design is the precision-recall-latency compromise nearly every serious RAG stack converges on.

**Q12. With million-token context windows, is RAG dead?**
No, for four reasons.
Cost: stuffing a corpus into every call bills you for it every call, while retrieval bills only relevant slices, and caching helps only static prefixes.
Quality: context rot is real; effective performance degrades as inputs grow even on simple tasks, so more context is not more accuracy.
Freshness and scale: corpora of gigabytes exceed any window, and retrieval indexes update without touching the model.
Access control: retrieval can enforce per-user permissions at query time, which context stuffing cannot.
Long context changes RAG's shape, enabling bigger retrieved chunks and lazier precision, rather than replacing it.

## 3. The agent loop and tool use

**Q13. Describe the core agent loop precisely.**
The harness sends the model a context containing system prompt, tool definitions, and conversation history.
The model responds with either a final message or one or more tool calls with structured arguments.
The harness executes the tool calls, appends the results to the history as tool-result messages, and calls the model again.
The loop terminates when the model responds without tool calls, or when a harness-imposed stop condition fires (max iterations, budget, timeout, approval denial).
Everything distinctive about an agent product lives in the decoration of this loop: context management, permissioning, error shaping, state persistence, and observability.

**Q14. What makes a good tool definition?**
A tool definition is prompt engineering: the model chooses and parameterizes tools entirely from names, descriptions, and schemas.
Good tools have unambiguous names, descriptions that say when to use and when not to use them, typed parameters with examples, and documented defaults.
They operate at task-level granularity (search_flights, not raw_sql_query) so one call accomplishes an intention, and they return token-efficient results with meaningful errors rather than raw dumps.
Overlapping tools are a classic failure source: if two tools plausibly fit the same intent, the model will split its behavior between them, so consolidate or sharply differentiate.
Anthropic's 2025 tool-writing guidance formalizes this: evaluate tools with agents in the loop and iterate on descriptions like you would on prompts.

**Q15. How should an agent handle tool errors?**
Return errors to the model as informative text results, not exceptions that kill the loop, because the model can often correct course if told what went wrong and what to try instead.
Distinguish error classes: malformed arguments (return validation details), transient failures (retry with backoff in the harness before involving the model), and permanent failures (tell the model so it stops retrying).
Cap repair attempts, because models can loop on the same failing call; after N failures, escalate or fail gracefully.
Design error messages the way you design tool descriptions, since they are model-facing prompts, and log every error into traces because error-recovery behavior is where agents differentiate.

**Q16. When do parallel tool calls help, and what are the hazards?**
Parallel calls collapse latency when operations are independent: reading five files, querying three APIs, running multiple searches.
The hazards are ordering and side effects: parallel writes to shared state race, and a model may parallelize calls that were logically sequential (read-then-modify).
Harness policy should permit parallelism for read-only tools and force serialization for mutating tools, or require the model to declare dependencies.
There is also an error-handling wrinkle: with five parallel calls and one failure, the harness must return all five results coherently so the model can reason about partial success.

**Q17. Workflow or agent: how do you decide?**
If the task's steps are enumerable in advance, build a workflow: fixed code calling the model at defined points is cheaper, faster, more testable, and more predictable.
Reserve agents for tasks where the path genuinely cannot be scripted, such as open-ended debugging, research, or heterogeneous user requests, because model-directed control flow is what you pay for in cost, latency, and variance.
This is the central guidance of Anthropic's Building Effective Agents (2024): find the simplest structure that meets the bar, and escalate autonomy only when measurement shows the workflow ceiling.
In practice mature systems are hybrids, workflows with agentic subroutines where flexibility earns its keep.

**Q18. Your agent has 200 tools and accuracy is degrading; what do you do?**
First, recognize the mechanism: every tool definition consumes context and adds a decision branch, and overlapping tools dilute selection accuracy.
Consolidate aggressively: merge near-duplicates, remove tools the traces show are never correctly used, and design task-level tools that replace chains of primitive ones.
Then apply progressive disclosure: group tools into namespaces and load definitions on demand via search or skill-style expansion, so each context carries only plausibly relevant tools.
A 2025-era alternative is code mode, exposing tools as a code API the model calls from an execution sandbox, which scales tool count without linear context growth.
Measure tool-selection accuracy on an eval set before and after; this is a measurable regression axis, not a matter of taste.

**Q19. Why do idempotency and reversibility matter in tool design?**
Agents retry, both inside provider infrastructure and in harness logic, so a non-idempotent action like send_email or charge_card can execute twice and cause real damage.
Design mutating tools to accept idempotency keys or to be naturally idempotent (set-state rather than increment).
Reversibility bounds blast radius: prefer soft-delete, draft-then-send, and propose-then-apply shapes so a wrong action is recoverable, and reserve irreversible actions for approval gates.
A useful discipline is classifying every tool as read, reversible-write, or irreversible-write, and attaching harness policy (parallelism, retries, approvals) to the class rather than the individual tool.

## 4. Agent architectures

**Q20. What is ReAct and why did it work?**
ReAct (Yao et al., 2022) interleaves free-text reasoning steps with actions and environment observations in one generation loop.
Reasoning before acting lets the model decompose the task, track progress, and adjust plans from observations, while acting grounds the reasoning in real information and cuts hallucination relative to reasoning-only chains.
It beat both act-only and reason-only baselines, establishing that the synergy, not either half, is the point.
Modern agents are ReAct's descendants with the parsing replaced by native function calling and the visible thoughts replaced by trained reasoning phases; the loop structure is unchanged.

**Q21. When does reflection or self-critique actually help?**
Reflection helps when the model or its environment can genuinely detect errors: failing tests, checker output, constraint violations, or a judge with real signal.
Reflexion (2023) shows the mechanism, storing verbal critiques of failed attempts and conditioning retries on them, which improves success across trials in verifiable environments.
Where no reliable error signal exists, self-critique tends to produce cosmetic revision or oscillation while multiplying cost, and models are poor at spotting their own reasoning errors unprompted.
So the design rule is: build reflection around an external verifier when one exists, and be skeptical of ungrounded "reflect and improve" steps.

**Q22. Explain the evaluator-optimizer pattern and its failure modes.**
One model call generates work, a second grades it against explicit criteria, and the generator revises with the critique until pass or budget exhaustion.
It shines when evaluation is easier than generation and criteria are articulable: prose against a style guide, code against a spec, translations against fidelity.
Failure modes: a weak evaluator produces noise that the optimizer chases; a miscalibrated one causes infinite loops, so you cap iterations; and using the same model for both roles invites shared blind spots and self-preference.
Keep rubrics concrete and per-criterion, and log both grades and revisions so you can audit whether iterations actually improve outcomes.

**Q23. Orchestrator-workers versus one strong agent: how do you choose?**
Orchestrator-workers wins when the task decomposes into independent subtasks that benefit from parallelism or from isolated contexts, breadth-first research being the canonical case; Anthropic's research system (2025) is the reference implementation.
A single agent wins when subtasks are tightly coupled, because subagents cannot see each other's context and will make conflicting decisions, which is Cognition's core argument in Don't Build Multi-Agents (2025).
The economics are stark: multi-agent research runs consume on the order of 15x the tokens of a chat interaction.
So the decision test is: is the task parallelizable with cheap-to-specify interfaces between parts, and does its value justify the token multiplier; if either answer is no, use one agent with better context engineering.

**Q24. Plan-and-execute versus fully interleaved agents.**
Plan-and-execute produces an explicit task list first, then executes items, replanning as needed; interleaved agents decide each next step from the latest observation.
Explicit plans improve long-horizon coherence, make progress legible to users, enable plan-level approval before any action, and give the agent a durable scratchpad that survives context compaction.
Their weakness is staleness: early assumptions break mid-execution, so a plan without a replanning trigger becomes a liability.
Interleaved execution is maximally adaptive but drifts on long tasks and is harder to audit.
Production coding agents converge on the hybrid: a visible todo list maintained as state, with interleaved execution inside each item.

## 5. RAG and knowledge systems

**Q25. How do you design a chunking strategy?**
Start from the retrieval unit the queries need: policy clauses, functions, table rows, and paragraphs are natural units in different corpora, so chunk along document structure rather than fixed character counts when structure exists.
Balance the tension: small chunks give precise matching but strip context; large chunks preserve context but dilute embeddings and waste tokens.
Mitigations include overlap, parent-document retrieval (match small, return large), and contextual chunking, where a model-written sentence situating the chunk in its document is prepended before embedding (Anthropic, 2024).
Then evaluate: chunking choices should be judged by retrieval recall and end-task accuracy on your query distribution, not aesthetics.

**Q26. Why does BM25 still matter in 2026?**
Embeddings compress meaning and blur exact identifiers: error codes, function names, SKUs, legal citations, and version strings all retrieve poorly on dense similarity alone.
BM25 matches those rare exact terms with high precision, costs almost nothing, and requires no training.
Hybrid retrieval, dense plus BM25 fused with reciprocal rank fusion, outperforms either alone on realistic mixed query distributions and is the standard production default.
The practical claim to make in an interview: pure-vector RAG stacks systematically fail on identifier-shaped queries, and adding BM25 is the cheapest fix in retrieval.

**Q27. What does a reranker buy you, and what does it cost?**
First-stage retrieval optimizes recall over millions of documents with cheap scoring; a cross-encoder reranker rescoring the top 50 to 200 candidates optimizes precision with full query-document interaction.
The quality lift on ranking metrics is typically the largest single upgrade available in a RAG stack, because final context slots are scarce and precision at the top is what the model actually sees.
Costs: one model inference per candidate adds tens to hundreds of milliseconds and per-query fees, and the reranker becomes another model to version and evaluate.
Use it when answer quality is retrieval-bound; skip it for latency-critical paths where first-stage precision already suffices.

**Q28. What is agentic RAG and when is the extra cost justified?**
Classic RAG retrieves once with the raw query and generates; agentic RAG puts retrieval inside the agent loop, so the model decides whether to search, rewrites queries, inspects results, and iterates until evidence suffices.
It fixes the failure modes of single-shot retrieval: vague queries, multi-hop questions whose second hop depends on the first answer, and queries needing evidence from multiple angles.
The cost is multiplied latency and tokens per question, plus a new failure mode of search loops that never converge, so budget caps are mandatory.
Justified when questions are genuinely multi-hop or high-value (research, support escalations); unjustified for high-volume simple lookup, where a tuned single-pass pipeline is faster and cheaper.

**Q29. How do you evaluate a RAG system?**
Decompose into retrieval and generation, because a wrong answer has two very different root causes.
Retrieval: recall@k and precision@k against labeled relevant documents, or judge-graded relevance when labels are scarce; this tells you whether the evidence was even present.
Generation: faithfulness (is every claim supported by the retrieved context) and answer correctness against gold answers, typically judge-graded with a rubric validated against human labels.
Track the joint failure matrix: retrieval-hit-but-wrong-answer indicts the generator or prompt; retrieval-miss indicts chunking, embedding, or query formulation.
Build the eval set from real logged queries, including unanswerable ones, because graceful refusal on missing evidence is a behavior you must measure.

**Q30. What is the chunk-context-loss problem and how is it solved?**
An isolated chunk like "the fee is 2 percent of the balance" loses its referents: which product, which document, which effective date.
Embedded without that context, it matches wrong queries and misleads generation.
Contextual retrieval (Anthropic, 2024) fixes this by having a model write a short situating context for each chunk given the whole document, prepended before embedding and indexing, with prompt caching making the per-chunk cost tolerable.
Alternatives include metadata prefixing (title, section path), parent-document retrieval, and late-chunking approaches that embed with document-level attention before splitting.
The shared principle: the retrieval representation must carry enough context to be interpretable standalone.

## 6. Memory and context engineering

**Q31. What is context rot and how do you engineer around it?**
Context rot is the degradation of model performance as input length grows, occurring well below advertised limits and even on trivially simple tasks; Chroma's 2025 report measured it systematically across models.
Mechanistically, attention is a finite budget spread over more tokens, and distractor content actively harms retrieval and reasoning, so tokens are not free even when they fit.
Engineering responses: treat context as a scarce resource with diminishing returns; retrieve just-in-time instead of front-loading; summarize or truncate tool outputs; compact history; and isolate subtasks in subagent contexts.
The operational habit is measuring task accuracy as a function of context size for your workload, then setting compaction thresholds from data rather than from the advertised window.

**Q32. Design a compaction strategy for a long-running agent.**
Trigger compaction on a context threshold measured in tokens, leaving headroom for the next several turns, rather than on failure.
Compact by having the model write a structured summary of the oldest span: decisions made and their rationale, constraints and user preferences discovered, current state of the task, unresolved threads, and exact identifiers (paths, IDs, URLs) that later steps will need.
Preserve verbatim what summaries corrupt: the system prompt, recent turns, open tool results in active use, and any artifact the user may reference exactly.
Keep durable state out of the conversation entirely where possible, in files or a todo list, because externalized state survives compaction for free.
Test compaction with evals that run tasks across multiple compaction events, since silent loss of a constraint is the characteristic bug and it only shows up downstream.

**Q33. How do you implement short-term versus long-term memory in an agent product?**
Short-term memory is the context window plus session state: conversation history, working files, and the current plan, managed by compaction and just-in-time loading.
Long-term memory persists across sessions and splits into episodic (summaries of past sessions), semantic (facts and preferences as structured records or editable notes), and procedural (learned instructions and skills).
Implementation is retrieval at heart: memories are stored with metadata, retrieved by relevance when a session starts or a topic surfaces, and written back by explicit tool calls or end-of-session extraction.
The hard problems are salience (what deserves writing), staleness (facts change, so memories need provenance and overwrite rules), and poisoning (a bad memory quietly corrupts every future session), which argues for user-visible, editable memory stores.

**Q34. Why do modern agents use the filesystem as memory?**
Files give agents unbounded, persistent, addressable storage with tools they already have: write notes, read them back, list and search.
This externalizes state from the context window, so plans, findings, and intermediate artifacts survive compaction and crashes, and the agent can re-derive its context after resumption by reading its own notes.
It is also legible: humans can inspect and correct a notes file mid-run, which is far harder with opaque in-context state.
The pattern appears throughout 2025-era practice, from todo-list files in coding agents to research agents accumulating findings in scratch documents; its weakness is that the agent must be prompted to actually maintain the files, which is a trained or instructed discipline, not a free behavior.

**Q35. Pre-load context or retrieve just-in-time: how do you decide?**
Pre-loading puts everything plausibly relevant into the prompt up front: zero extra steps, full visibility, but maximum token cost, cache pressure, and context-rot exposure.
Just-in-time gives the agent retrieval tools and lets it pull what it needs: minimal resting context and always-fresh data, but extra loop iterations, added latency, and dependence on the model's judgment about what to fetch.
Decide by stability and universality: content needed in every request and stable across a session (core instructions, key schemas) belongs up front where caching amortizes it; long-tail reference material belongs behind retrieval.
Anthropic's context-engineering guidance (2025) lands on this hybrid explicitly, and traces showing the agent repeatedly fetching the same document are the signal to promote it into the prefix.

## 7. Multi-agent systems

**Q36. When do multi-agent systems actually beat a single agent?**
When work parallelizes into subtasks with cheap-to-specify interfaces and each subtask needs deep context of its own: breadth-first research across many sources, fan-out over many files or repositories, or tournament-style generation with selection.
Also when contexts must be isolated for correctness or security: an untrusted-content reader quarantined from a privileged actor is a multi-agent security pattern, not a performance one.
They lose when subtasks are coupled, because separated contexts make coordinated decisions impossible, and the token multiplier (roughly 15x chat for Anthropic's research system) demands the task value support it.
The honest summary: multi-agent is a context-engineering tool with specific wins, not a default architecture.

**Q37. Contrast handoffs with orchestrator-workers.**
In orchestrator-workers, a lead agent owns the task, dispatches scoped subtasks to workers, and synthesizes results; control returns to the orchestrator, and workers are subordinate specialists.
In handoffs, popularized by OpenAI's Swarm and Agents SDK, control of the conversation itself transfers to another agent, which becomes the new principal facing the user; triage-to-specialist support flows are the natural fit.
Orchestration suits divisible work products; handoffs suit sequential ownership changes where exactly one agent should be responsible at a time.
Handoffs carry a context-transfer risk (what does the receiving agent know about the conversation so far), while orchestration carries a synthesis risk (the orchestrator must reconcile possibly conflicting worker outputs).

**Q38. What is the strongest argument against multi-agent designs, and what does it imply?**
Cognition's Don't Build Multi-Agents (2025) argues that parallel subagents make decisions on divergent hidden context and return conflicting work that no synthesis step can reliably reconcile, citing failures like subagents building mismatched components.
The derived principles: share full context and trace history rather than summaries when agents must coordinate, and avoid architectures where independent actors make implicit decisions.
The implication is not "never multi-agent" but a constraint: parallelize only read-heavy, loosely coupled work, keep writes and design decisions in one context, and treat every inter-agent interface as a place where intent is lost until proven otherwise.
Reconciling this with Anthropic's successful research system is instructive: research is exactly the read-heavy, decomposable workload the critique exempts.

**Q39. Walk through the economics of a multi-agent research system.**
Each subagent carries its own context (system prompt, task brief, tool results), so tokens scale with agent count times per-agent context, and Anthropic reported roughly 15x chat-level token consumption for research runs (2025).
Latency improves despite cost because subagents run in parallel; wall-clock is driven by the slowest branch plus synthesis rather than the sum of steps.
Cost control levers: cap subagent count and per-agent budgets in the orchestrator prompt, scale effort to query complexity (one subagent for simple lookups, many only for open-ended surveys), use cheaper models for workers than for the lead, and make workers return distilled findings rather than raw dumps.
The business condition is that task value must clear the token multiplier, which is why deep research monetizes and casual Q&A stays single-agent.

## 8. Model Context Protocol

**Q40. Explain MCP's architecture and primitives.**
MCP, released by Anthropic in November 2024, standardizes how applications supply context and capabilities to LLMs over JSON-RPC.
A host application (IDE, chat client, agent harness) embeds one MCP client per connected MCP server; servers expose capabilities, and the host owns the model conversation and user consent.
Server primitives: tools (model-invoked functions), resources (application-controlled data the host can inject), and prompts (user-invoked templates).
Client-side primitives flow the other way: sampling lets a server request a completion from the host's model, roots scope filesystem access, and elicitation (added 2025) lets a server request structured user input mid-flow.
The value is the N-times-M collapse: any compliant client works with any compliant server, which is why the major vendors adopted it during 2025.

**Q41. What does MCP buy over plain function calling?**
Function calling is an API mechanism between one application and one model; every integration is bespoke code inside that application.
MCP makes integrations portable artifacts: a server written once (for GitHub, Postgres, Slack) works in every compliant host without that host shipping integration code.
It also standardizes discovery (list tools at runtime), transport (stdio locally, streamable HTTP remotely), and the consent boundary (hosts mediate what servers may do).
The costs are real: protocol overhead for simple cases, tool definitions consuming context across many servers, and a supply chain of third-party servers that becomes a security surface.
The honest framing: MCP is packaging and interoperability for tool use, not a new capability for the model.

**Q42. Compare MCP transports and when you would use each.**
Stdio runs the server as a local child process communicating over stdin and stdout: zero network exposure, inherits local user permissions, ideal for filesystem, shell, and desktop integrations.
Streamable HTTP, which replaced the earlier HTTP-plus-SSE transport in the 2025 spec revisions, serves remote multi-client deployments: one endpoint handling JSON-RPC POSTs with optional server-sent event streaming for long operations and notifications.
Remote transports require real authentication, and the 2025 spec aligned this on OAuth-based authorization for HTTP servers.
Choice heuristic: stdio for anything touching the local machine, streamable HTTP for shared services, and never expose a stdio-designed server over the network without adding the auth layer it was built without.

**Q43. What are the main MCP security risks?**
Tool poisoning: malicious instructions hidden in tool descriptions, visible to the model but not surfaced in the client UI, steering the agent to exfiltrate data or misuse other tools (documented by Invariant Labs, 2025).
Rug pulls: a server changing its tool definitions or behavior after installation and approval, invalidating the user's original consent.
Confused-deputy and cross-server attacks: one malicious server's descriptions or outputs manipulating the agent's use of another server's privileged tools.
Classic supply chain: unvetted third-party servers running with user permissions, plus token theft on poorly secured remote servers.
Mitigations: pin server versions and hash definitions, re-approve on change, show full tool descriptions to users, scope credentials to least privilege, and isolate untrusted servers from sensitive ones at the harness level.

**Q44. A user connects five MCP servers and the agent slows down and misbehaves; diagnose.**
The mechanism is context bloat plus selection dilution: every server's tool definitions load into each request, consuming the attention budget and multiplying overlapping choices, so both latency and tool-selection accuracy degrade.
Diagnose from traces: measure tokens consumed by definitions and the tool-choice error rate against a small eval.
Fixes in order of leverage: disconnect unused servers, filter which tools are exposed per session, apply progressive disclosure (load definitions on demand via search or namespacing), and where the harness supports it, use code-execution-style access where the model imports a typed API instead of carrying every schema in context (the "code mode" idea, 2025).
This question is really a context-economics question wearing an MCP costume, and interviewers ask it to see whether you know that.

## 9. Evaluation and observability

**Q45. Design an eval from scratch for a new agent feature.**
Start from failure, not coverage: collect 20 to 50 real or realistic tasks, weighted toward observed and suspected failure modes, each with a definition of success.
Choose graders per task type: code-execution or state checks where outcomes are verifiable, exact match for closed answers, LLM-judge with a written rubric for open-ended quality, and validate the judge against a sample of human labels before trusting it.
Grade outcomes, not paths, wherever possible, because agents legitimately vary in approach; add trajectory checks only for properties that matter intrinsically (safety, cost, tool budget).
Run multiple trials per task and report variance, since single-run agent results are noise.
Wire it into CI as a regression gate, and keep feeding production failures back in; an eval that does not grow with the product is a snapshot, not an instrument.

**Q46. Explain pass@k versus pass^k and why the distinction matters for deployment.**
pass@k is the probability that at least one of k independent attempts succeeds; it measures the capability ceiling and rises with k.
pass^k, introduced by tau-bench (2024), is the probability that all k attempts succeed; it measures reliability and falls with k, sharply when per-trial success is mediocre, since it behaves like per-trial rate to the kth power under independence.
A system at 90 percent pass@1 is at roughly 59 percent pass^5 if trials are independent, which is what a customer running the task daily experiences within a week.
Deployment decisions should weight pass^k because users experience the worst run, not the best; pass@k is the right metric only when a verifier lets you actually select the best of k.

**Q47. What are the pitfalls of LLM-as-judge and how do you mitigate them?**
Known biases from the MT-Bench work (Zheng et al., 2023): position bias in pairwise comparisons, verbosity bias favoring longer answers, and self-preference for the judge's own model family; add rubric drift and score compression on numeric scales.
Mitigations: written rubrics with per-criterion binary or few-level judgments instead of holistic scores; swap positions and average for pairwise tests; use a judge from a different family than the system under test; require the judge to cite evidence for each judgment.
Above all, validate the judge itself: measure agreement against human labels on a calibration set, and re-validate when you change judge model or rubric.
Treat judge scores as a proxy metric with known error bars, never as ground truth, and keep a human-graded golden set as the anchor.

**Q48. Why do static benchmarks stop being informative, and what replaces them?**
Three decay mechanisms: saturation (frontier models cluster near the ceiling, compressing differences), contamination (public test sets leak into training corpora, rewarding memorization), and Goodharting (labs tune toward headline suites, so scores gain benchmark-specific skill rather than general capability).
MMLU is the archetype: from headline metric to near-meaningless at the frontier within a few years, motivating successors like MMLU-Pro, GPQA, and Humanity's Last Exam (2025).
What replaces them for agents: execution-graded environments (SWE-bench-style, OSWorld), refreshed or held-out task sets (LiveCodeBench's dated windows, private splits), reliability metrics like pass^k, and above all private domain evals built from your own traffic.
The interview-grade summary: public benchmarks orient and regression-test; they do not predict your product, and any single number without harness, date, and subset is marketing.

**Q49. What does observability mean for agents beyond standard tracing?**
The unit of observation is the trajectory: a tree spanning model calls, tool calls, subagents, and retries, with prompts, outputs, token counts, latency, and cost at every node, correlated by session and task identifiers.
Beyond plumbing, agent observability answers "why": which context the model saw when it chose a wrong tool, where a run's tokens went, at which step a long task diverged.
Aggregates that matter: cost and step-count distributions per task type, tool error rates, loop and stall detection, compaction frequency, and outcome metrics joined to traces.
The flywheel property is the real point: production traces are the raw material for eval cases and prompt fixes, so trace capture, redaction policy, and search ergonomics determine how fast you can improve the agent.
OpenTelemetry-based GenAI conventions and LLM-native platforms (LangSmith, Langfuse, Braintrust and peers) are the 2025-era tooling landscape here.

**Q50. How do offline evals and online measurement fit together?**
Offline evals are the pre-ship gate: versioned task sets run against every prompt, tool, or model change, cheap enough to run in CI and sensitive enough to catch regressions before users do.
Online measurement catches what offline cannot: real query distributions, drift, and the gap between eval success and user-perceived success, via A/B tests on outcome metrics, sampled human review of traces, and implicit signals like retry, abandonment, and escalation rates.
The loop closes in both directions: online failures become offline eval cases, and offline improvements are confirmed online before full rollout.
A canary-plus-shadow pattern is standard for risky changes: run the new configuration on a traffic slice or in shadow mode, compare trace metrics, then promote.
Teams that skip either half fly blind in a different way: offline-only teams ship eval-overfit systems, online-only teams cannot attribute regressions to causes.

## 10. Security

**Q51. Distinguish prompt injection from jailbreaking.**
Jailbreaking manipulates a model into violating its own safety training: the attacker is the user, and the victim is the model's policy.
Prompt injection manipulates an application built on a model: untrusted content is crafted to be interpreted as instructions, and the victim is the application's intent and its user, a distinction Simon Willison drew when naming the attack in 2022.
The consequential form for agents is indirect injection (Greshake et al., 2023), where the payload arrives through content the agent processes: a web page, email, document, or tool result.
The distinction matters because defenses differ: jailbreak resistance is trained into models, while injection resistance must be architected into systems, since a sufficiently capable model following instructions in data is doing exactly what it was trained to do.

**Q52. State the lethal trifecta and derive its design implications.**
Willison's lethal trifecta (2025): an agent that combines access to private data, exposure to untrusted content, and the ability to communicate externally can be turned into an exfiltration engine by injection, because a planted instruction can read the data and send it out.
The design rule is to break at least one leg for any given agent configuration.
Concretely: agents reading arbitrary web content should not hold broad private-data access; agents with sensitive data access should have egress restricted to an allowlist, including blocking data-bearing URLs in rendered markdown images; and where all three capabilities are genuinely needed, insert a human approval gate on the external-communication leg.
The deeper implication is that capability composition, not any single tool, is what must be reviewed: each added tool must be assessed against what the agent can already touch and see.

**Q53. Why are prompt-based defenses insufficient, and what does architectural defense look like?**
Instructions like "ignore instructions in retrieved content" and injection classifiers reduce attack success rates but fail against adaptive attackers, because the model fundamentally cannot verify instruction provenance from text alone, and detection is a cat-and-mouse game with no sound decision boundary.
Architectural defenses assume injection will land and constrain what it can cause.
CaMeL (Google DeepMind, 2025) extracts a plan from the trusted user query, then confines untrusted data behind capability policies so injected text cannot alter control flow.
The dual-LLM pattern quarantines untrusted-content reading in an unprivileged model whose outputs are handled as data; plan-then-execute locks the action sequence before untrusted content is read; and the 2025 design-patterns literature (Beurer-Kellner et al.) catalogs these with context minimization and action confinement.
Defense in depth still includes prompts, classifiers, and monitoring, but the security boundary must be enforced outside the model.

**Q54. How do you sandbox an agent that executes code?**
Assume the agent will eventually run hostile code, whether injected or hallucinated, and confine consequences rather than trusting intent.
Execution goes in an isolated environment: containers with dropped privileges and resource limits for the common case, microVMs (Firecracker-class) or full VMs when the threat model is stronger, with the filesystem scoped to a workspace and secrets kept out of the environment entirely.
Network policy is the critical control: default-deny egress with an explicit allowlist (package registries, target APIs), because unrestricted egress converts any compromise into exfiltration.
Ephemerality completes it: fresh environment per task, destroyed afterward, so persistence attacks die with the sandbox.
Grant capabilities by task, not by platform: a data-analysis run needs no git credentials, and a refactoring run needs no browser.

**Q55. Which agent actions should require human approval, and how do you keep the gate meaningful?**
Gate on irreversibility and blast radius, not on frequency: external communications, payments, deletions and destructive migrations, credential and permission changes, and production deployments.
Reads and reversible workspace edits should flow freely, because gating everything trains users to click approve, and a rubber-stamp gate is worse than none in that it launders responsibility.
Keep gates meaningful by showing the actual action payload (recipient and full email text, exact SQL, diff to be applied), batching low-risk approvals, and making approval decisions auditable.
Session-scoped grants ("allow tests for this session") reduce fatigue without widening standing permissions.
The design goal is that a human approves each consequential intention exactly once, with enough information to genuinely evaluate it.

**Q56. Your team wants to adopt ten community MCP servers; what is your review process?**
Treat each server as a third-party dependency running with user permissions against your data: this is supply-chain review, not feature review.
Static review: read the source or vendor attestation, hash and pin the version, and inspect every tool description for hidden instructions (tool poisoning) before allowing installation.
Capability review: enumerate what each server can read and send, check the combination against the lethal trifecta, and scope its credentials to least privilege.
Runtime controls: re-approve when tool definitions change (rug-pull detection), log all server traffic into traces, and isolate untrusted servers from sensitive ones, either in separate agent configurations or behind an MCP gateway that enforces policy.
Organizationally, maintain an internal allowlist registry, because ad hoc per-developer installation is how one poisoned weather server ends up beside the production database server.

## 11. Production engineering

**Q57. Where does latency go in a user-facing agent, and how do you budget it?**
Decompose per turn: network and queueing, prefill (proportional to prompt length), decode (proportional to output tokens, serial), tool execution, and then multiply by loop iterations, which is why agent latency is dominated by step count times per-step cost.
Levers in rough order of leverage: cut iterations (better tools and prompts that accomplish more per step), parallelize independent tool calls, cache prompt prefixes to shrink prefill, cap and trim outputs to shrink decode, and route easy requests to smaller, faster models.
Perceived latency is its own budget: stream tokens, surface tool activity as progress, and deliver a first useful signal inside a second or two even when completion takes a minute.
Set the budget from the product contract first (chat turn versus background job), then allocate it across steps, and alert on step-count and per-step latency distributions rather than means, because agents fail in the tail.

**Q58. What are the main cost levers for an agent system?**
Token diet first: trim system prompts and tool schemas, truncate and paginate tool outputs, compact history, and cap output lengths, because every wasted token recurs on every loop iteration.
Caching second: structure prompts for prefix caching (stable-first ordering, append-only history), which cuts input cost dramatically on cache hits at 2025-era provider pricing.
Model routing third: send classification, extraction, and easy turns to small models, reserving frontier models for the steps that need them, including cheap workers under an expensive orchestrator.
Batch and async fourth: anything non-interactive (evals, backfills, bulk extraction) goes through half-price batch APIs.
Finally, govern: per-session and per-task budget caps, cost attribution in traces by feature and customer, and alerts on cost-per-task drift, because a prompt regression that doubles loop count is invisible in average latency but glaring in cost.

**Q59. A new model version is out; how do you migrate a production agent safely?**
Never swap in place: pin model versions in configuration, and treat a model change like a dependency major-version bump.
Run the full offline eval suite on the new version first, comparing not just aggregate scores but per-task diffs, tool-call formats, refusal behavior, latency, and cost, because models regress unevenly and agents are sensitive to tool-calling style changes.
Expect prompt rework: instructions tuned around an old model's quirks may now be counterproductive, so re-tune before judging the model.
Then roll out progressively: shadow mode or a small traffic canary with trace comparison, expansion on clean metrics, and an instant rollback path to the pinned prior version.
Keep the old version's config reproducible for as long as the provider serves it, and watch provider deprecation timelines, since forced migrations on a deadline are the worst-case version of this process.

**Q60. How do you make a long-running agent task reliable end to end?**
Assume every component fails: model calls get rate-limited and time out, tools error, sandboxes die, and the process may be redeployed mid-task.
Retries with exponential backoff and jitter handle transient faults, but only around idempotent operations, which is why tool idempotency is a reliability feature and not just a safety one.
Checkpoint durable state (conversation, plan, workspace, todo list) at step boundaries so a crashed run resumes rather than restarts, with the externalized-state patterns (files, databases) doing double duty here.
Bound everything: max iterations, wall-clock timeout, token budget, and loop detection, so a stuck agent fails fast and legibly instead of burning quota.
Degrade gracefully: on repeated failure, escalate to a human with the trace and partial work product, because "here is what I completed and where I am stuck" preserves value that a silent crash destroys.

**Q61. How do you version and roll back the non-model parts of an agent?**
Prompts, tool definitions, rubrics, and routing policies are behavior-bearing artifacts and must be versioned like code: stored in the repository, code-reviewed, and released through the same pipeline, never edited live in a dashboard.
Every trace records the full configuration fingerprint (prompt hash, tool schema versions, model version), so any production behavior is attributable to an exact configuration.
Changes ship behind the offline eval gate, then progressive rollout, and rollback is a configuration revert, which stays cheap only if configurations are immutable and self-contained.
The subtle trap is coupling: a prompt tuned against tool-set version N may fail against N+1, so evals must run against the composed configuration, not each artifact in isolation.

**Q62. When do you choose a smaller model, and how do you keep quality?**
Choose smaller models when the step is classification, routing, extraction, summarization, or templated generation, where frontier reasoning is wasted; latency-critical interactive paths and high-volume background steps are the natural candidates.
Keep quality by verification asymmetry: let the small model produce and a checker (schema validation, rules, or occasional frontier-model audit) verify, escalating failures to the larger model, a cascade that captures most savings at little quality cost.
Distillation is the stronger version: fine-tune the small model on the large model's traces for your specific task, which for narrow tasks routinely closes most of the gap.
Measure with the same evals as the frontier path and monitor drift, because small-model quality is more sensitive to input distribution shifts.
The governing rule: route by required capability per step, not by prestige of the model name.

## 12. Frameworks and SDKs

**Q63. When should you use an agent framework, and when should you write the loop yourself?**
Write it yourself first: the loop is about a hundred lines, and the abstractions only make sense once you have felt the problems they solve.
Adopt a framework when you would otherwise rebuild real machinery badly: durable execution with checkpointing, streaming plus interrupt handling, multi-agent plumbing, or integrated tracing.
Avoid one when control flow is simple or unusual, because every framework encodes opinions about state, retries, and context assembly that are expensive to fight later.
The selection test is not whether the demo works but whether you can see and control the exact bytes sent to the model, since debugging an agent means reading its prompts.

**Q64. Contrast LangGraph's model with the OpenAI Agents SDK's model.**
LangGraph models an application as a directed graph of nodes over a typed shared state object, with conditional edges defining control flow and a checkpointer persisting state at node boundaries, which gives durable, resumable, human-interruptible processes and makes control flow explicit.
The OpenAI Agents SDK (2025, productionized from the earlier Swarm experiment) models agents as objects with instructions, tools, guardrails, and handoffs, with a built-in runner driving the loop and tracing attached by default.
The first optimizes for controllable long-running processes at the cost of expressing your app as a graph; the second optimizes for fast assembly of conversational multi-agent systems at the cost of less explicit control flow.
Rough heuristic: graph frameworks when the process is the product, agent-object SDKs when the conversation is the product.

**Q65. What does the Claude Agent SDK provide over a bare API loop?**
It packages the harness behind Claude Code (Anthropic, 2025; renamed from the Claude Code SDK) rather than wrapping the API: the loop, a filesystem and shell toolset, subagents, permission modes and hooks, MCP client support, and automatic context compaction.
The value is that the details which actually decide agent quality - tool ergonomics, compaction behavior, approval flows - arrive pre-solved and exercised by a shipped product.
The cost is coupling to one provider and to an opinionated, filesystem-centric shape of work.
Fit test: if the task looks like an agent working in a workspace with tools and human approvals, it is a strong default; if it looks like a constrained business workflow with branching rules, a graph framework fits better.

**Q66. What is the code-agent approach and what are its trade-offs?**
Instead of emitting one JSON tool call per step, the model writes code that calls tools as functions and executes it in a sandbox; CodeAct (Wang et al., 2024) argued the case and smolagents (HuggingFace, 2024) is the compact reference implementation.
Advantages: loops, conditionals, and composition happen inside one action instead of many round-trips; intermediate data stays in the sandbox instead of the context window; and tool count scales without loading every schema into every request.
Costs: you must run model-authored code in a real sandbox, failures become runtime exceptions rather than schema violations, and debuggability depends on capturing sandbox stdout and tracebacks into traces.
The 2025 "code mode" framing extends the same idea to MCP, exposing servers as a typed API the model imports on demand rather than as hundreds of resident tool definitions.

**Q67. How do you avoid framework lock-in?**
Keep the behavior-bearing artifacts outside framework classes: prompts, tool definitions, rubrics, and eval sets live as plain versioned data you own.
Implement tools as ordinary functions with a thin framework adapter, so the business logic never imports the framework.
Emit traces on an open standard (OpenTelemetry GenAI conventions) so observability survives a migration.
The justification is empirical: this framework layer has turned over roughly annually since 2023, while evals, prompts, and tool logic retain value across every rewrite.

## 13. Coding agents and computer use

**Q68. Why is the agent-computer interface as important as the model?**
The SWE-agent paper (Yang et al., 2024) coined ACI and showed that interface redesign - a windowed file viewer, an edit command that rejects syntactically invalid results, concise error output - moved success rates as much as changing models.
The mechanism is context economics plus feedback quality: an interface that dumps unstructured output burns the attention budget, and one that permits silent corrupting edits removes the agent's ability to notice its own mistakes.
Design rules follow directly: validate before committing an edit, return token-efficient views rather than whole artifacts, and make every error message immediately actionable.
The practical habit is to read what your tools return before blaming the model for underperforming.

**Q69. What edit format should a coding agent use, and why?**
Three families exist: whole-file rewrite, unified diff, and search/replace blocks.
Whole-file rewriting always applies cleanly but costs output tokens proportional to file size and risks silent truncation on long files.
Unified diffs are compact but brittle, because models miscount line numbers and context hunks.
Search/replace blocks - an exact snippet to find plus its replacement, the format popularized by aider - are the usual production compromise: compact, position-independent, and self-validating, since a failed match is a loud recoverable error rather than a corrupted file.
Whatever the format, apply edits through a tool that verifies the match and the resulting parse, and return failures to the model as text.

**Q70. How should a coding agent verify its own work?**
The defining advantage of the coding domain is a real verifier: compilers, type checkers, linters, and tests supply ground truth that most agent domains lack, which is why reflection works here and flounders elsewhere.
The loop should be narrow-then-broad: make a change, run the smallest relevant check, feed raw failure output back verbatim, iterate, then run a wider suite before declaring completion.
Guard two specific failure modes: the agent weakening or deleting tests to make them pass (review test-file diffs separately or make them read-only), and the agent claiming success without executing anything (require evidence of a run in the completion criteria).
When no tests exist, having the agent first write a failing reproduction converts an unverifiable task into a verifiable one, which is usually worth the extra steps.

**Q71. Pixels or accessibility tree: how do you choose for a computer-use agent?**
Structured access (DOM or accessibility tree) gives token-efficient, precisely addressable elements and much higher action reliability, but fails on canvas-rendered UIs, native desktop applications, and pages engineered to resist parsing.
Pixel-based control - screenshot in, mouse and keyboard out, as in Anthropic's computer use capability released in October 2024 - generalizes to anything a human can see, at the cost of heavy per-screenshot token consumption, coordinate-grounding errors, and sensitivity to resolution and rendering.
The pragmatic stack is hybrid: identify and target elements structurally where possible, use screenshots for verification and for what structure cannot express.
Choose by environment breadth: a closed set of known web applications favors structured access, arbitrary third-party desktop software forces pixels, and an available API beats both.

**Q72. Why are computer-use agents markedly less reliable than coding agents?**
Compounding error dominates: a task requiring thirty consecutive correct actions at 97 percent per-step reliability succeeds under half the time, and GUI workflows have long action chains.
Grounding is genuinely hard (predicting exact click targets), the environment is partially observable (a screenshot may capture a half-rendered state), and feedback is weak because a wrong click usually yields a plausible-looking screen rather than an error.
Coding is the contrast case: the environment is text, actions are precise, and tests give unambiguous pass or fail signal, which is why execution-graded coding benchmarks have run far ahead of OSWorld-class computer-use scores through 2025.
Mitigations are interface-level, not prompt-level: prefer APIs over GUIs, insert explicit wait-and-verify steps after each action, keep a recovery policy for unexpected states, and gate consequential actions behind human approval.

## 14. The frontier

**Q73. What is test-time compute scaling and where does it stop paying?**
Spending more inference compute - longer chains of thought, sampling many candidates, search over partial solutions, or iterative revision - buys accuracy without retraining, and the reasoning-model wave (OpenAI's o1 in 2024, DeepSeek-R1 and successors in 2025) turned it into an explicit training target rather than a prompting trick.
Two axes exist: sequential scaling (think longer) and parallel scaling (sample more and select), and parallel scaling only pays when you hold a reliable selector such as a test suite, verifier, or calibrated judge.
Limits: returns diminish roughly logarithmically in compute, latency and cost rise directly with tokens, and extended reasoning can talk a model out of a correct easy answer.
The engineering consequence is per-step effort routing, because applying maximum reasoning uniformly multiplies cost with little quality gain.

**Q74. What does METR's time-horizon work tell you, and what does it not?**
METR (2025) measures the human-expert task duration at which a model succeeds 50 percent of the time and reported that horizon doubling roughly every seven months over the period studied.
It is valuable because it denominates capability in something product teams care about - how long a task can be delegated before a human must intervene - rather than accuracy on a static set.
What it does not tell you: 50 percent reliability is far below any deployment bar and the 80 percent horizon is substantially shorter; the number depends on the task portfolio's composition; and the tasks are well-specified, while real work carries ambiguity and stakeholder context that no timer captures.
Quote it as an orientation for planning horizons, never as a law, and never extrapolate the doubling as if it were measured beyond its window.

**Q75. What is the state of continual learning for agents, and why does it matter?**
Production agents do not learn from experience by default: weights are frozen between releases, so everything "learned" lives in retrievable memory, edited instruction files, or accumulated artifacts.
The available mechanisms are memory stores (semantic, episodic, procedural), self-maintained instruction and skill files, and periodic fine-tuning or distillation on collected traces.
The unsolved problems are salience (what deserves to be remembered), conflict and staleness (an obsolete memory is worse than no memory), and evaluation (scoring whether an agent improved over a month is far harder than scoring one task).
This is the highest-leverage open area as of early 2026: every long-running agent product currently reinvents memory hygiene by hand, and the ones that do it badly degrade rather than improve with use.

**Q76. Where is agent training heading, and what should engineers build accordingly?**
The direction since 2024 is training on trajectories rather than single responses: reinforcement learning with verifiable rewards inside executable environments, so tool use, error recovery, and long-horizon planning are learned behaviors rather than prompted ones.
That shifts scarcity toward environment engineering - sandboxed, resettable, programmatically graded tasks - which is now a bottleneck comparable to data curation.
A second thread is models trained to manage their own context through compaction, note-taking, and delegation, absorbing work that harnesses currently do with heuristics.
The engineering implication is to invest where the model will not absorb you: domain evals, security architecture, data access and permissions, and product surface, while treating clever harness tricks as depreciating assets.

## 15. System design drills

**Q77. Design a deep research agent.**
Clarify requirements first: queries range from simple lookups to open-ended surveys; output is a cited report; latency tolerance is minutes; correctness and citation fidelity matter more than speed.
Architecture: orchestrator-workers.
A lead agent interprets the query, writes a research plan, and scales effort to complexity, spawning one to N subagents with explicit task briefs (objective, suggested sources, output format, budget); subagents search, read, and return distilled, source-attributed findings; the lead iterates (spawning follow-up subagents for gaps), then synthesizes; a citation pass verifies every claim maps to a source.
Key decisions to narrate: parallel subagents because research is read-only and decomposable (the multi-agent sweet spot); interleaved thinking in the lead for plan revision; search tools returning snippets-plus-URLs with a separate fetch tool, so token spend is deliberate; findings accumulated in external files so compaction cannot destroy work; effort scaling rules in the lead's prompt because token cost is the dominant economic risk (order 15x chat, per Anthropic's 2025 system).
Failure modes to address: subagents duplicating work (briefs must partition the space), source-quality collapse (instruct evaluation of source authority, prefer primary sources), unverifiable claims (the citation pass drops them), and runaway budgets (hard caps per subagent and per run).
Evaluate with judge-graded report quality rubrics (coverage, accuracy, citation fidelity) validated against human ratings, plus GAIA or BrowseComp-style end-to-end checks for regression.

**Q78. Design evaluation infrastructure for a customer-support agent.**
Requirements: the agent handles refunds, exchanges, and account questions against real APIs under a policy document; errors have direct customer and financial cost; the team ships prompt and tool changes weekly.
Layered eval design: unit-level checks (tool-call correctness on fixed scenarios, policy-compliance probes), scenario-level simulations (tau-bench-style: an LLM user simulator with personas and goals, the agent under test, grading by final state of a seeded sandbox database plus required-utterance checks), and judge-graded conversation quality (tone, resolution, escalation appropriateness) with a rubric validated against human labels.
Metrics: task success per scenario family, pass^k over repeated trials as the headline reliability number (customers experience the worst run), policy-violation rate as a hard gate, and cost and latency per resolution.
Pipeline: eval sets versioned in the repo, run in CI on every prompt, tool, or model change, with diff reports against the baseline; a nightly larger run for variance estimation.
Production loop: full tracing with redaction, weekly sampled human review, automatic conversion of escalations and user-flagged failures into new eval scenarios, and online A/B with resolution rate and CSAT as decision metrics.
Trade-offs to name: simulator noise contaminates measurement (characterize it with repeated identical runs), judge bias (different model family, calibrated), and eval-set overfitting (a held-out rotation set refreshed monthly).

**Q79. Harden a coding agent against prompt injection.**
Threat model first: the agent reads untrusted content (issue text, repository files, dependency docs, web pages) and holds capabilities (shell, file writes, git push, possibly package installation), so indirect injection can attempt exfiltration of secrets or malicious code changes; this is the lethal trifecta unless a leg is broken.
Environment: run in an ephemeral sandbox per task; default-deny egress with an allowlist (package registries, the target git remote); no secrets in the environment beyond a scoped, short-lived token for the one repository; fresh sandbox per task kills persistence.
Capability policy: reads and workspace edits are free; irreversible or externally visible actions (push, PR creation, package publish, any network call outside the allowlist) require approval gates showing the exact diff or payload.
Content handling: treat file and web content as data, with harness-level markers separating it from instructions; strip or neutralize instruction-shaped content in tool results where feasible; never render attacker-controlled markdown images (a classic exfiltration channel).
Detection and audit: log all tool calls to traces, flag anomalies (unexpected egress attempts, touching files outside task scope, encoded blobs in outputs), and run AgentDojo-style injection suites in CI so defenses are measured, not assumed.
State the honest limit: none of this makes injection impossible; it makes the worst outcome a blocked action and a flagged trace instead of a leaked secret, which is the achievable bar in 2026.

**Q80. Design an enterprise document-QA system (RAG) for 10 million documents with per-user permissions.**
Requirements: heterogeneous documents (PDFs, wikis, tickets), strict access control, answers with citations, thousands of queries per day, freshness within hours.
Ingestion: parse and normalize, chunk along document structure with contextual enrichment (model-written situating context per chunk), embed with a current commercial or strong open embedding model, and index into hybrid retrieval (vector plus BM25) with metadata (source, ACL groups, timestamps); incremental pipeline keyed on document change events for freshness.
Query path: permission filter applied at retrieval time (filter by the caller's ACL groups inside the index query, never post-filtering after retrieval, so unauthorized content never enters candidate sets), hybrid retrieve top 100 to 200, cross-encoder rerank to top 10 to 20, generate with a faithfulness-focused prompt requiring per-claim citations, and refuse when evidence is insufficient.
Agentic escalation: a router sends simple queries through the single-pass pipeline and complex or multi-hop queries to an agentic loop that can reformulate and issue multiple searches, bounded by budget; this keeps cost proportional to difficulty.
Evaluation and operations: retrieval recall on a labeled set, judge-graded faithfulness and answer quality anchored to human calibration, unanswerable-query behavior as an explicit metric, tracing from query to retrieved chunks to final claims for auditability, and a security test suite proving permission filters hold under adversarial queries.
Trade-offs to name: contextual enrichment adds meaningful ingestion cost (justify with retrieval-failure reduction), reranking adds latency (skip for the fast path), and ACL-at-retrieval requires index support and disciplined metadata, which constrains vector-store choice.

**Q81. Design an ambient agent that triages inbound issues for an engineering team.**
Requirements: the agent is triggered by events (new issue, failing CI run, support escalation) rather than by a user prompt, must be cheap enough to run on every event, produces output visible to the whole team, and tolerates minutes of latency.
Architecture: an event pipeline (webhook to durable queue to worker) invoking a workflow rather than an open-ended agent, because triage steps are enumerable - classify, gather context, propose labels and severity and owner, then comment or escalate - with an agentic subroutine only inside the context-gathering step where the path genuinely varies.
Key decisions to narrate: retrieval over past issues and recent deploys for deduplication and likely-cause hints; the agent writes proposals (labels, a clearly machine-authored comment) rather than closing or reassigning, because reversibility bounds the damage of a wrong call; an explicit uncertainty path that routes low-confidence cases to a human queue instead of guessing; and idempotency keyed on the event ID, since webhook redelivery is normal and duplicate comments are the characteristic bug.
Security: issue text is attacker-controllable content and the agent holds repository read access plus public comment ability, which is two legs of the lethal trifecta, so it gets no secret access, no egress beyond the tracker API, and no ability to modify code or CI configuration.
Operations and evaluation: per-event token budget, full tracing keyed by event, a labeled eval set built from historical human triage decisions, and human override rate as the honest quality metric, since it measures what the team actually thought of each decision.

**Q82. Design a multi-tenant agent platform serving many customers on shared infrastructure.**
Requirements: per-tenant data, credentials, and configuration; strict isolation; per-tenant cost accountability; one customer's workload must not degrade another's.
Isolation: a fresh execution sandbox per run with no cross-tenant reuse; credentials held in a per-tenant vault and injected as short-lived scoped tokens at call time rather than baked into images or prompts; retrieval indexes partitioned or ACL-filtered per tenant with the filter applied inside the query; and a standing adversarial test suite whose only job is to attempt cross-tenant retrieval and fail.
Control plane: agent configuration (prompt, tools, model version, policy) is a versioned artifact per tenant promoted through the same eval gate as code, so a customer-specific customization cannot silently alter another tenant's behavior and every production trace is attributable to an exact configuration fingerprint.
Runtime: queue-backed workers with per-tenant concurrency and token budgets so one runaway agent cannot starve the pool; checkpointed durable execution so deploys do not destroy in-flight long tasks; retries confined to idempotent tool calls.
Caching and cost: structure prompts so the cacheable prefix contains only tenant-agnostic content and all tenant data sits after the boundary, keep cache scopes tenant-isolated by construction rather than by assumption, and attribute tokens and cost per node in every trace, because cost-per-task drift is usually the first visible symptom of both prompt regressions and abuse.
Trade-offs to name: stronger isolation (VM-per-run, dedicated indexes) raises the cost floor and cold-start latency, and per-tenant configuration multiplies the eval matrix, so you need a shared baseline suite plus a small tenant-specific overlay rather than a full suite per customer.

## 16. Rapid-fire round

Short questions that appear as warm-ups or as probes between larger topics.
Answer each in one or two sentences.

**Q83. Why are output tokens more expensive in latency than input tokens?**
Input is processed in a parallel prefill pass, while output is generated serially one token at a time, so decode time scales directly with response length.

**Q84. What is the difference between a tool result and a user message?**
Structurally they both land in the conversation, but tool results carry a call identifier binding them to a specific request, and treating untrusted tool output as if it carried user authority is exactly the prompt-injection failure.

**Q85. Why does an agent that "works great" in a demo often fail in production?**
Demos sample the happy path once, while production samples the tail thousands of times, so anything measured by pass@1 on curated inputs hides the pass^k reliability that users actually experience.

**Q86. What is progressive disclosure in tool design?**
Loading only the tool definitions plausibly relevant to the current task and letting the agent discover the rest on demand, so context cost stops scaling linearly with the size of your tool catalog.

**Q87. What is a confused deputy in an agent context?**
The agent holds privileges the attacker does not, so injected content does not need to break the agent's permissions; it only needs to persuade the agent to use them.

**Q88. Why should tool results be truncated rather than passed through whole?**
Because every token in a tool result is re-sent on every subsequent loop iteration, so one 50,000-token dump is paid many times and displaces the attention budget for the actual task.

**Q89. What is the single most common cause of agents looping forever?**
A tool that fails in a way the model cannot distinguish from a transient problem, so it retries the same call indefinitely; the fix is distinguishable error classes plus a harness-level repeat detector.

**Q90. Why is "the model hallucinated" usually the wrong root cause for an agent bug?**
In a tool-using system most wrong answers trace to missing or misleading context, an ambiguous tool description, or a tool returning bad data, and traces almost always show the model reasoning correctly over bad inputs.

**Q91. What is the fastest way to reduce agent cost without touching quality?**
Reorder the prompt so all stable content precedes all volatile content, keep history append-only, and let prefix caching absorb the repeated prefill.

**Q92. Why do you evaluate outcomes rather than trajectories by default?**
Because agents legitimately reach the same correct result by different paths, so trajectory matching penalizes valid behavior; add path checks only for properties that matter in themselves, such as cost, safety, or a required approval.

**Q93. When is a summary worse than a truncation?**
When exact tokens matter - identifiers, paths, code, quoted policy - because summarization paraphrases and paraphrase silently destroys exactness that later steps depend on.

**Q94. What does "grade the judge" mean?**
Measuring your LLM judge's agreement with human labels on a calibration set before trusting its scores, and re-measuring whenever the rubric or judge model changes.

**Q95. Why is an approval gate on every action worse than a few well-placed ones?**
Approval fatigue converts human review into reflexive clicking, which removes the safety value while preserving the illusion of oversight and the appearance of shared responsibility.

**Q96. What is the practical difference between a subagent and a tool?**
A subagent has its own context window and can iterate; a tool executes once and returns, so use a subagent when the subtask needs its own exploration budget and a tool when it needs a deterministic answer.

**Q97. Why is temperature usually low but rarely zero for agents?**
Low temperature stabilizes tool selection and formatting, while exact zero adds no real determinism guarantee and can lock the agent into a repeated failing action with no path out.

## How to use these drills

Answer out loud, timed, without notes; the failure mode in interviews is knowing the material and assembling it slowly.
For each answer, hit three beats: the mechanism (why it works this way), the trade-off (what the recommended option costs), and the measurement (how you would know you were right).
Date-stamp anything that moves - model names, framework shapes, protocol revisions - and say plainly when a number is an order of magnitude rather than a measurement, because fabricated precision is the fastest way to lose a senior interviewer.
For system-design prompts, spend the first minute on requirements and the last minute on failure modes and evaluation; candidates lose these questions by designing a happy path and never saying how they would find out it was wrong.
When you cannot answer one of these cold, the corresponding volume in this track is the fix, and Appendix B is the primary-source backing for anything you want to be able to defend under follow-up.
