---
type: playbook
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://langchain-ai.github.io/langgraph/, https://docs.langchain.com/, https://openai.github.io/openai-agents-python/, https://docs.claude.com/en/api/agent-sdk/overview, https://docs.crewai.com/, https://dspy.ai/, https://www.anthropic.com/engineering/building-effective-agents, https://arxiv.org/abs/2104.09864]
---

# More frequently asked questions

Commonly asked questions that the topic notes did not already cover, numbered A1-A64.
They fall into eight groups: fundamentals openers, prompting in production, building agents day to day, framework specifics, product and business, using AI coding tools yourself, opinion questions, and quick-fire follow-ups.
Framework APIs named here were checked on 2026-09-23 by installing the packages (LangGraph 1.2, LangChain 1.4, OpenAI Agents SDK 0.22, Claude Agent SDK 0.2, CrewAI 1.6, DSPy 3.3); the LangGraph example in A33 was run.

## The ones most likely to come up

If time is short, prepare these first: **A1, A3, A11, A12, A15, A18, A20, A24, A33, A34, A41, A42, A49, A50, A55, A56**.

---

## 1. Fundamentals openers (A1-A10)

Interviewers often warm up with these; a crisp answer sets the tone.

**A1. What are the main limitations of LLMs?**
They hallucinate fluent but unsupported claims, their knowledge stops at a training cutoff, they are weak at exact arithmetic and long counting, their outputs vary run to run, they follow injected instructions in untrusted text, and quality degrades over long contexts.
Every production pattern (RAG, tools, verification, guardrails, evals) exists to compensate for one of these.

**A2. Why do LLMs hallucinate?**
They are trained to produce likely continuations, not true ones; when the answer is not well represented in training data or context, a plausible-sounding guess still scores well.
Training that rewards guessing over abstaining makes it worse.
Mitigate with grounding in retrieved sources, citations checked in code, tools for facts and math, and explicit permission to say "I don't know."

**A3. What is grounding?**
Tying the model's output to evidence it was given (retrieved documents, tool results, database rows) rather than to its parametric memory, and making that link checkable through citations.

**A4. Encoder, decoder, encoder-decoder: what is the difference?**
Encoder-only models (BERT) read the whole input bidirectionally and are used for embeddings, classification, and reranking.
Decoder-only models (GPT, Claude, Llama) generate left to right with a causal mask and power chat and agents.
Encoder-decoder models (T5) encode an input and decode an output, used for translation and summarization.

**A5. Why did transformers replace RNNs?**
Attention connects any two positions in one step, so long-range dependencies do not fade, and training parallelizes across the sequence instead of stepping token by token.
The cost is quadratic attention in sequence length.

**A6. How do models know token order?**
Through positional information; most current models use rotary position embeddings (RoPE), which rotate query and key vectors by position-dependent angles so their dot product depends on relative position.
Context-extension methods rescale those rotations.

**A7. How are embedding models trained?**
Contrastively: pull a query toward its matching passage and push it away from other passages in the batch (in-batch negatives) and from deliberately hard negatives, usually with an InfoNCE-style loss.
That is why they capture "relevant to" rather than "identical to."

**A8. Is temperature 0 the same as greedy decoding?**
In intent, yes: always pick the most likely token.
In practice hosted APIs are still not fully deterministic (batching changes floating-point order, mixture-of-experts routing varies), so do not rely on it for reproducibility (F10 in [Fundamentals](06-LLM-Fundamentals-and-Inference.md)).

**A9. What happens when a request exceeds the context window?**
The API rejects it, or a framework silently truncates, which is worse because the dropped part may be the instructions.
Count tokens before sending, and compact or retrieve instead of truncating blindly.

**A10. Why are tokens, not words, the unit of cost and limits?**
Models read and write subword tokens; English averages roughly 3-4 characters per token, code and non-English text use more tokens per word, and each provider's tokenizer differs.
Estimate with the provider's counter, not word counts.

---

## 2. Prompting in production (A11-A17)

**A11. How do you iterate on a prompt?**
Treat it like code with tests: build a small eval set first, change one thing at a time, run the eval with several trials per case, read failing transcripts, and keep the change only if the pass rate improves beyond noise.
"It looked better on three examples" is not evidence.

**A12. How do you version and deploy prompts?**
Store prompts in version control next to the code that calls them, tie each release to a model version, run the eval suite in CI on every change, and log the prompt version with every request so traces and regressions can be attributed.
Roll out through shadow mode or a canary like any other change.

**A13. How do you choose few-shot examples?**
A few diverse, canonical examples that show the output format and the hard distinctions, not a list of every edge case.
For varied inputs, retrieve the most similar labeled examples per request (dynamic few-shot), and watch the token cost.

**A14. How do you get reliable JSON out of a model?**
Use the provider's structured output or strict tool schemas, or constrained decoding for local models; then validate with Pydantic and retry with the error message on failure (Q60 in [Coding Round](09-Coding-Round.md)).
Keep schemas small and flat; deeply nested optional fields are where models slip.

**A15. How do you handle a document longer than the context window?**
Retrieve only the relevant parts (RAG); or map-reduce (summarize or extract per chunk, then combine); or refine (walk the chunks carrying a running summary); or give the agent tools to read sections on demand.
Pick by task: extraction suits map-reduce, question answering suits retrieval, holistic judgment suits refine or hierarchical summaries.

**A16. What is the difference between prompt injection and jailbreaking?**
Jailbreaking is a user trying to get the model to break its safety rules; prompt injection is untrusted content (a web page, an email, a tool result) overriding the application's instructions.
For agents, injection is the bigger risk because the attacker never talks to the agent directly (Q34 in [Safety](05-Safety-and-Security.md)).

**A17. The model keeps drifting from the required format or tone. What do you do?**
Show the format with an example rather than describing it, put format rules near the end of the prompt, use structured output where possible, and add a cheap output check that retries or repairs.
If drift persists across prompts, consider a small fine-tune for format.

---

## 3. Building agents day to day (A18-A30)

**A18. The agent works in development and fails in production. How do you debug it?**
Pull the failing traces and compare them with development runs: input distribution (real users are messier), context size, tool latency and errors, rate limits, model version or alias changes, and missing permissions or data.
Reproduce by replaying the production trace against the dev build, then add the case to the eval set.

**A19. How do you handle provider rate limits (HTTP 429)?**
Respect `retry-after` headers, back off with jitter (Q59), cap concurrency per provider with a token bucket (K2), queue low-priority work, and fail over to another model or provider through a gateway.
Track tokens-per-minute as well as requests-per-minute.

**A20. How do you stream agent output to users?**
Stream text tokens as they arrive, show tool activity as status events ("searching orders..."), and assemble tool-call arguments from fragments before executing (K10).
Server-sent events suit most web UIs; handle client disconnects by cancelling the run or letting it continue in the background.

**A21. How do you handle model upgrades and deprecations?**
Pin exact model versions, not aliases; when a new model ships, run the eval suite against it, compare cost and latency, fix prompt regressions, and roll out behind a flag.
Keep the previous model as a fallback until the new one has run in production.

**A22. What are the levels of autonomy for an agent?**
A useful scale: (1) suggest, a human acts; (2) act after human approval each time; (3) act autonomously on low-risk actions, approval for high-risk; (4) act autonomously with after-the-fact review; (5) fully autonomous.
Move up one level at a time as measured error rates allow, and set the level per action type, not per agent.

**A23. How is an agent different from RPA?**
RPA replays fixed, scripted steps against user interfaces and breaks when the interface changes; an agent decides its steps at runtime from goals and observations.
Many good systems combine them: deterministic automation for the fixed path, an agent for exceptions.

**A24. Two tools return conflicting information. What should the agent do?**
Prefer the source of record defined in the tool descriptions, check freshness timestamps, and when the conflict matters, say so and escalate rather than pick silently.
Design tools to return provenance and timestamps so the model can reason about it.

**A25. What is semantic caching, and when is it dangerous?**
Returning a cached answer for a new query that is close in embedding space (K7).
It is dangerous when near-identical questions need different answers (different user, account, region, or date), so scope keys by those fields, tune the threshold, and never cache personalized or action-taking responses.

**A26. How do you build background or long-running agents?**
Run them as jobs from a queue with durable checkpoints, report progress through events or a status endpoint, notify on completion or when approval is needed, and make every step idempotent so a crashed worker can resume.
Budgets and deadlines matter more because no one is watching.

**A27. How do you make an agent multi-tenant?**
Tenant identity comes from authentication and is bound to the session server-side; every tool, retrieval filter, memory store, cache key, and budget is scoped by it.
Test for cross-tenant leaks explicitly.

**A28. How do you test a new tool before giving it to the agent?**
Unit-test the tool as ordinary code, then run agent evals on tasks that should and should not use it, and read transcripts for misuse; adjust the name, description, and output until selection accuracy is good.

**A29. How do you keep PII out of prompts and logs?**
Minimize: send only the fields the task needs; redact or tokenize identifiers before the model call where possible; log metadata separately from content with redaction and retention limits; use providers and regions that meet your data agreements.

**A30. Should the agent query structured data with SQL or through RAG?**
Structured questions (totals, filters, joins) go through SQL or an API tool against a read-only, permission-scoped source; RAG is for unstructured text.
Embedding table rows and hoping retrieval does arithmetic gives wrong answers.

---

## 4. Framework specifics (A31-A40)

Asked when the company uses the framework; read its current docs the night before.

**A31. What is the difference between LangChain and LangGraph?**
LangGraph is the low-level runtime: explicit state graphs, checkpointing, interrupts, streaming, durable execution.
LangChain is the higher-level library of model and tool integrations; since 1.0 its `create_agent` builds a tool-calling agent on the LangGraph runtime and adds middleware (for example human-in-the-loop and summarization middleware).
Use `create_agent` for a standard loop, and LangGraph directly when you need custom control flow.

**A32. Explain LangGraph's core concepts.**
- **State:** a typed dict shared by all nodes; reducers (for example `add_messages`) define how updates merge.
- **Nodes:** functions that read state and return a partial update.
- **Edges:** fixed or conditional transitions; `START` and `END` mark entry and exit.
- **Checkpointer:** saves state after every step under a `thread_id` (in-memory, SQLite, Postgres), which gives memory, resume, and time travel.
- **`interrupt()` and `Command(resume=...)`:** pause for a human and resume later with their input.
- **`Send`:** fan out to parallel branches (map-reduce).
- **Subgraphs and streaming modes** for composition and progressive output.

**A33. Sketch a human approval step in LangGraph.**
Run on 2026-09-23 with LangGraph 1.2.12: it pauses at the interrupt and resumes to `refunded`.

```python
from typing import TypedDict

from langgraph.checkpoint.memory import InMemorySaver
from langgraph.graph import END, START, StateGraph
from langgraph.types import Command, interrupt


class State(TypedDict):
    order_id: str
    amount: float
    status: str


def propose(state: State) -> dict:
    return {"status": "proposed"}


def approve(state: State) -> dict:
    if state["amount"] <= 100:                       # policy in code: small refunds auto-approve
        return {"status": "approved"}
    decision = interrupt({"question": f"Refund {state['amount']} for {state['order_id']}?"})
    return {"status": "approved" if decision == "yes" else "rejected"}


def execute(state: State) -> dict:
    return {"status": "refunded" if state["status"] == "approved" else state["status"]}


builder = StateGraph(State)
builder.add_node("propose", propose)
builder.add_node("approve", approve)
builder.add_node("execute", execute)
builder.add_edge(START, "propose")
builder.add_edge("propose", "approve")
builder.add_edge("approve", "execute")
builder.add_edge("execute", END)
graph = builder.compile(checkpointer=InMemorySaver())   # checkpointer is required for interrupt

config = {"configurable": {"thread_id": "refund-42"}}
first = graph.invoke({"order_id": "ORD-42", "amount": 420.0, "status": ""}, config)
print(first["__interrupt__"][0].value)               # paused; state is saved under the thread id
final = graph.invoke(Command(resume="yes"), config)  # a human approves, possibly hours later
print(final["status"])
```

Policy stays in code (the $100 threshold); the pause survives restarts if the checkpointer is durable (Postgres), because resume only needs the `thread_id`.

**A34. Explain the OpenAI Agents SDK's core concepts.**
`Agent` (instructions, tools, handoffs, guardrails, `output_type` for structured output), `Runner.run` to execute the loop, `@function_tool` to turn a Python function into a tool, handoffs to transfer control between agents, input and output guardrails that can trip and halt a run, sessions for conversation memory, and built-in tracing.

**A35. Explain the Claude Agent SDK.**
It exposes the Claude Code harness as a library: `query()` for one-shot runs and `ClaudeSDKClient` for interactive sessions, configured with `ClaudeAgentOptions` (allowed and disallowed tools, permission mode, system prompt, MCP servers, hooks, subagent definitions, max turns, budget, sandbox settings).
Custom tools are defined with `@tool` and served in-process with `create_sdk_mcp_server`.
Its strengths are the built-in file, shell, and search tools, subagents, hooks, and context compaction; it suits coding and file-system agents.

**A36. Explain CrewAI's model.**
Agents are defined by role, goal, and backstory; tasks have a description and expected output and are assigned to agents; a crew runs tasks with a sequential or hierarchical (manager-led) process; Flows add event-driven control for production pipelines.
Fast to prototype; less control over the inner loop.

**A37. What is DSPy?**
"Programming, not prompting": you declare signatures (typed inputs and outputs) and compose modules (`Predict`, `ChainOfThought`, `ReAct`), then an optimizer (`BootstrapFewShot`, `MIPROv2`, `GEPA`) tunes prompts and few-shot examples against a metric on your data.
It makes prompt changes measurable and portable across models; the cost is a learning curve and the need for a good metric and dataset.

**A38. When would you drop a framework and write your own loop?**
When the agent is simple, when debugging through the framework costs more than it saves, when you need control over every token for cost or latency, or when the framework's abstractions fight your state model.
Keep what is hard to build well yourself: tracing, durable checkpoints, and evals.

**A39. How would you migrate an agent from one framework to another?**
Freeze behavior first with an eval suite and recorded traces; keep tools and prompts framework-independent behind thin adapters; port, then compare pass rates, cost, and latency on the same suite before switching traffic.

**A40. LlamaIndex versus LangChain?**
LlamaIndex centers on data: ingestion, indexing, and query engines for RAG over documents; LangChain and LangGraph center on orchestration and integrations.
Teams often use LlamaIndex for retrieval inside an agent orchestrated elsewhere.

---

## 5. Product and business (A41-A48)

**A41. How do you measure the ROI of an agent?**
Compare against the current process on the same work: time or cost per task, quality or error rate, throughput, and what humans do with the freed time; subtract model, infrastructure, maintenance, and review costs.
Report cost per successful task, and count the human review time the agent still needs.

**A42. How do you A/B test an LLM feature?**
Randomize by user or session, not by request, so experiences stay consistent; pick a primary metric tied to the outcome (task completion, resolution, retention) plus guardrail metrics (cost, latency, complaints, safety flags); run long enough for novelty effects to fade.
Offline evals decide what is worth testing; the A/B test decides what ships.

**A43. What metrics would you track for an LLM product in production?**
Task success or resolution rate, user feedback and edits to outputs, escalation rate, latency percentiles, cost per task, error and refusal rates, safety incidents, and retention of users of the feature.

**A44. How do you gather requirements for an AI feature?**
Start from the workflow and the decision being supported, collect 30-50 real examples with what a good output looks like, define what an unacceptable error is, and agree on the success metric and the fallback when the model is wrong, before choosing a model.

**A45. Build or buy?**
Buy commodity capabilities (models, vector databases, tracing) and build what differentiates you or touches your proprietary data and workflows: tools, evals, domain logic, and guardrails.
Revisit as the market moves; lock-in risk is highest in orchestration frameworks and proprietary agent platforms.

**A46. How do you explain model limitations to non-technical stakeholders?**
Show real failures from their own data, give error rates as "about 1 in 20 cases," and pair each limitation with the control that manages it (review step, confidence threshold, audit log).

**A47. When would you kill an AI project?**
When evals plateau below the bar the workflow needs, when the cost per successful task exceeds the manual process, when a deterministic solution matches it, or when the risk of errors cannot be contained by review or reversibility.
Decide the kill criteria at the start.

**A48. How do costs change as usage scales?**
Token costs grow linearly with tasks and quadratically with steps per task (Q48 in [Cost](07-Cost-and-Latency.md)); caching, smaller models for easy steps, batch APIs, and self-hosting at high steady volume bend the curve.
Watch outlier tasks that loop; a few runaway tasks can dominate the bill.

---

## 6. Using AI coding tools yourself (A49-A54)

Almost every engineering loop now asks some version of these.

**A49. How do you use coding agents in your own work?**
I run them as a supervised team: a supervisor agent files and routes tasks, workers run in isolated git worktrees, and nothing ships without passing a gate that reviews, tests, lints, and opens the PR; I make the merge decision.
Details and evidence: [Agentic Harness](../Agentic-Harness/README.md), and the pitches in [Pitches](../Agentic-Harness/interview/Pitches.md).

**A50. How do you review AI-generated code?**
Like a capable but unfamiliar contributor's code: read the diff against the intent, look hardest at tests and configs the agent touched (weakened tests are a classic failure), run it, and check edge cases and error handling, where agents cut corners.
Automated review catches some issues; the human owns the merge.

**A51. How do you keep quality high when agents write a lot of the code?**
Make verification cheap and mandatory: tests that the agent must make pass, lint and type checks, a pre-merge gate the agent cannot skip, and hooks that block destructive commands and edits to quality configs.
Small, reviewable changes beat large ones.

**A52. How do you write instructions files (`CLAUDE.md`, `AGENTS.md`) for coding agents?**
Keep them short and specific to the repo: build and test commands, architecture notes, conventions, and hard rules.
I generate mine from one source for every tool, with a CI check that fails on drift (see the [Executive Summary](../Agentic-Harness/00-Executive-Summary.md)).

**A53. What are the risks of AI-assisted coding?**
Subtle bugs that pass shallow tests, security issues (injection, secrets in code), license and provenance questions, skill atrophy, and large diffs nobody fully understands.
Controls: tests, review, gates, secret scanning, and keeping humans responsible for design decisions.

**A54. What tasks do you not give to a coding agent?**
Ambiguous design decisions without a spec, security-critical changes without close review, and anything whose correctness cannot be tested.
I also route by difficulty: frontier models for design and root-cause work, cheaper models for mechanical changes.

---

## 7. Opinion and trend questions (A55-A60)

There is no single right answer; interviewers are testing judgment and whether you are grounded in real use.

**A55. What is the biggest limitation of agents today?**
Reliability over long horizons: small per-step error rates compound, and agents still declare success without verifying.
The practical frontier is verification, evaluation, and recovery, not raw model capability.

**A56. Where do you see agents going in the next one to two years?**
Longer autonomous runs in verifiable domains (coding, data work), more background and asynchronous agents, protocol standardization (MCP, A2A) making tools and agents composable, and more emphasis on evaluation, permissions, and audit as they touch real systems.
Say what you would bet on and why, briefly.

**A57. Tell me about a recent paper or post you found useful.**
Prepare one real example you can discuss for three minutes: the problem, the method, the result, and how it changed what you build.
Anthropic's "Building Effective Agents" and the ReAct paper are safe starting points, but a recent one of your own choosing is stronger.
**[fill in]**

**A58. Is MCP overhyped?**
It solves a real integration problem and has broad adoption, but it is plumbing, not intelligence: each connected server adds tool definitions to the context and a security surface.
For coding agents, plain CLIs are often cheaper in tokens; I use whichever gives the agent the smallest, safest interface.

**A59. Open-weight or closed models?**
Closed frontier models usually lead on hard reasoning and tool use; open-weight models win on data residency, cost at steady volume, fine-tuning, and control.
Many production systems mix them: frontier models for planning, small open models for high-volume steps.

**A60. Will fine-tuning matter as models improve?**
Less for general capability, still a lot for cost (distilling into small models), latency, strict formats, and domain behavior.
Retrieval and tools will keep carrying knowledge, and fine-tuning will keep carrying behavior.

---

## 8. Quick-fire follow-ups (A61-A64)

**A61. What is the difference between an embedding model and a reranker?**
An embedding model encodes query and document separately (fast, indexable); a reranker scores them together (slower, more accurate), so it only sees the top candidates.

**A62. What is the difference between guardrails and evals?**
Guardrails check each live request and can block it; evals measure quality offline or on samples and inform changes.
Many checks serve as both.

**A63. What is the difference between memory and RAG?**
RAG retrieves from documents that exist independently of the agent; memory stores what the agent learned from its own interactions, which it writes, updates, and must be able to forget.

**A64. What is an agent trajectory?**
The full sequence of model outputs, tool calls, and observations in one run; it is what you trace, evaluate, replay, and train on.
