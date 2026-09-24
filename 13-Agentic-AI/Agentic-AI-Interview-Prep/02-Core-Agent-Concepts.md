---
type: playbook
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://www.anthropic.com/engineering/building-effective-agents, https://www.anthropic.com/engineering/effective-context-engineering-for-ai-agents, https://www.anthropic.com/engineering/writing-tools-for-agents, https://www.anthropic.com/engineering/multi-agent-research-system, https://arxiv.org/abs/2210.03629, https://arxiv.org/abs/2303.11366, https://arxiv.org/abs/2305.10601, https://arxiv.org/abs/2312.04511, https://modelcontextprotocol.io/specification, https://github.com/a2aproject/A2A]
---

# Core agent concepts

The most-asked technical questions.
Q7, Q8, Q10, Q11, Q12, Q14, and Q17 are near-certain; know them cold.

---

## Q7. What is an AI agent? How is it different from a chatbot or a workflow?

- An **agent** is an LLM that **dynamically decides its own control flow**: which tools to call, in what order, and when to stop, in a loop driven by feedback from the environment.
- A **workflow** orchestrates LLMs and tools through **predefined code paths**; the developer owns the control flow.
- A **chatbot** is single-turn or multi-turn text generation with no autonomous actions.
- Rule of thumb: use the simplest thing that works.
  Start with a single LLM call, then a workflow, then an agent.
  Agents trade cost, latency, and predictability for flexibility.

| Question to ask | If yes | If no |
| --- | --- | --- |
| Can the steps be enumerated in advance? | Workflow | Agent |
| Is the path different for most inputs? | Agent | Workflow |
| Can each step's output be verified automatically? | Agent is safer | Keep a human in the loop |
| Is the latency budget under a few seconds? | Single call or short workflow | Agent is possible |
| Are actions irreversible or high-cost? | Workflow with approval gates | Agent with guardrails |

Go deeper: [What Is An Agent](../Agentic_AI_Zero_to_Godhood/Volume_03_Tool_Use_and_the_Agent_Loop/Chapter_01_What_Is_An_Agent.md), [Workflows Versus Agents](../Agentic_AI_Zero_to_Godhood/Volume_04_Agent_Architectures/Chapter_01_Workflows_Versus_Agents.md).

---

## Q8. Describe the basic agent loop.

```text
while not done and steps < max_steps:
    response = llm(system_prompt, history, tool_schemas)
    if response has tool_calls:
        for call in tool_calls:
            result = execute(call)          # validate args, sandbox, timeout
            history.append(tool_result(call.id, result))
    else:
        done = True                         # final answer
```

Key components:

- **Model:** reasoning and decisions.
- **Tools:** actions and observations.
- **Memory and context:** what the model sees at each step.
- **Orchestration:** the loop, stop conditions, error handling.
- **Guardrails:** permissions, validation, budgets.

**Stop conditions to name** (interviewers probe this):

1. The model returns no tool calls (a final answer), signaled by the stop reason (for example `end_turn` versus `tool_use` in the Anthropic API).
2. Step, token, time, or spend budget exhausted.
3. Loop detection: the same call with the same arguments repeated.
4. A tool reports a terminal state (task submitted, human approval required).
5. Output truncated by `max_tokens`: continue or fail explicitly; never treat a truncated reply as final.

The tested implementation is in [Coding Round Q55](09-Coding-Round.md).

Go deeper: [The Agent Loop From Scratch](../Agentic_AI_Zero_to_Godhood/Volume_03_Tool_Use_and_the_Agent_Loop/Chapter_03_The_Agent_Loop_From_Scratch.md).

---

## Q9. What is ReAct?

- **Reason + Act** (Yao et al., 2022): the model interleaves a **Thought** (reasoning), an **Action** (tool call), and an **Observation** (tool result).
- Grounding reasoning in real observations reduces hallucination compared with pure chain-of-thought, and lets the model recover from a wrong first step.
- Native function calling in modern APIs is essentially ReAct built into the model and the API; reasoning models add interleaved thinking between tool calls.

A trace you can sketch on a whiteboard:

```text
Thought: The user wants last month's refund total; I need the orders table.
Action: sql_query({"query": "SELECT SUM(amount) FROM refunds WHERE month = '2026-08'"})
Observation: {"sum": 18230.50}
Thought: I have the number; answer with the source.
Answer: $18,230.50 in refunds in August 2026 (refunds table).
```

**Descendants to name:** Reflexion (self-critique stored as memory), plan-and-execute, LATS (tree search over ReAct trajectories).

Go deeper: [ReAct and Its Descendants](../Agentic_AI_Zero_to_Godhood/Volume_04_Agent_Architectures/Chapter_02_ReAct_and_Its_Descendants.md).

---

## Q10. How does function or tool calling work under the hood?

1. You pass tool definitions: name, description, and a JSON Schema for the parameters.
2. The model is trained to emit a structured tool-call block instead of text when a tool fits.
3. **Your code** executes the tool; the model never executes anything itself.
4. You return the result with a matching call ID.
5. The model continues from there, calling more tools or answering.

One round trip in the Anthropic Messages format:

```json
[
  {"role": "user", "content": "Weather in Paris?"},
  {"role": "assistant", "content": [
    {"type": "tool_use", "id": "toolu_01", "name": "get_weather", "input": {"city": "Paris"}}]},
  {"role": "user", "content": [
    {"type": "tool_result", "tool_use_id": "toolu_01", "content": "Sunny, 22C"}]}
]
```

Important details:

- **Parallel tool calls:** the model may emit several calls in one turn; return all results in the next message, each with its ID.
- **`tool_choice`:** `auto` (model decides), `any` or `required` (must call some tool), a specific tool, or `none`.
- **Argument validation:** the model can produce invalid JSON or hallucinated arguments; validate with Pydantic and return the error as a tool result so the model can self-correct.
- **Strict mode or constrained decoding:** some APIs can guarantee schema-valid arguments; validation of semantics is still your job.
- **Tool definitions cost tokens** on every call; they belong in the cached prefix.

Go deeper: [Function Calling Mechanics](../Agentic_AI_Zero_to_Godhood/Volume_03_Tool_Use_and_the_Agent_Loop/Chapter_02_Function_Calling_Mechanics.md).

---

## Q11. What makes a good tool design?

Treat tools as an interface for the model, like an API designed for a capable junior engineer who cannot ask questions.

- Clear names and descriptions, including **when** to use the tool and **when not to**.
- Small parameter sets, with enums where possible.
- Return concise, high-signal output.
  Never dump 50 KB of raw JSON; paginate, filter, or summarize, and offer a `detail` or `verbose` flag.
- Actionable error messages ("date must be YYYY-MM-DD", not "400").
- Idempotency for anything with side effects.
- Consolidate: one `search_orders(filters)` beats five narrow tools.
- Fewer tools improves selection accuracy; namespace related tools (`orders_search`, `orders_refund`).
- Return semantic identifiers (names, slugs) rather than opaque UUIDs where the model must reason about them.
- Mnemonic: **"poka-yoke" your tools**; make wrong usage hard (for example, require absolute paths so the model cannot get the working directory wrong).

**How to improve tools empirically:** build a small eval of realistic tasks, run the agent, read the transcripts, and change descriptions and outputs where the model misused a tool; measure again.

Go deeper: [Tool Design](../Agentic_AI_Zero_to_Godhood/Volume_03_Tool_Use_and_the_Agent_Loop/Chapter_04_Tool_Design.md).

---

## Q12. What is the Model Context Protocol (MCP)?

Expect depth here; it is on the resume.

- An open protocol, introduced by Anthropic in November 2024, that standardizes how LLM applications connect to tools and data; the "USB-C for AI integrations."
- It turns the M x N integration problem (every app times every tool) into M + N.
- **Architecture:**
  - **Host:** the application (an IDE, a chat client, an agent).
  - **Client:** one per server connection, living inside the host.
  - **Server:** exposes capabilities.
- **Wire format:** JSON-RPC 2.0, stateful sessions.
- **Transports:** stdio for local servers, **Streamable HTTP** for remote servers (it replaced the older HTTP+SSE transport in the 2025-03-26 revision).
- **Server primitives:**
  - **Tools:** model-controlled actions.
  - **Resources:** application-controlled data, readable by URI.
  - **Prompts:** user-controlled templates.
- **Client primitives:**
  - **Sampling:** the server asks the host's LLM to generate.
  - **Roots:** filesystem boundaries the server may operate in.
  - **Elicitation:** the server asks the user for input mid-operation.
- **Lifecycle:** `initialize` with capability negotiation, then `tools/list`, `tools/call`, `resources/read`, and so on, with `notifications/tools/list_changed` when lists change.
- **Auth:** remote servers use OAuth 2.1-based authorization; the server acts as a resource server and tokens must be audience-bound to it.
- **Security issues to name:**
  - Tool poisoning: malicious instructions hidden in tool descriptions.
  - Prompt injection through tool results.
  - Over-permissioned servers.
  - "Rug pulls," where tool definitions change after approval.
  - Confused-deputy and token-passthrough problems (a server forwarding the client's token upstream).
- **SDK detail that shows hands-on use:** in the Python SDK 1.x the high-level server class was `FastMCP`; in 2.x it is `MCPServer`, and only a `ToolError` message reaches the model (any other exception is masked as "Error executing tool").
  See [Coding Round Q61](09-Coding-Round.md).
- **When not to use MCP:** for a single in-process tool, a plain function is simpler; for coding agents, a CLI can be cheaper in tokens (see harness [Question Bank Q10](../Agentic-Harness/interview/Question-Bank.md)).

The spec is versioned by date and moves quickly; check the current revision on [modelcontextprotocol.io](https://modelcontextprotocol.io/specification) before the interview.

Go deeper: the whole [Model Context Protocol volume](../Agentic_AI_Zero_to_Godhood/Volume_09_Model_Context_Protocol/README.md).

---

## Q13. MCP versus A2A?

- **MCP** connects an agent to tools and context (vertical).
- **A2A (Agent2Agent)**, launched by Google in April 2025 and now a Linux Foundation project, lets agents talk to **other agents** (horizontal).
  It uses **Agent Cards** (JSON capability descriptors, served at a well-known URL) for discovery, plus **tasks** with lifecycle states, messages, artifacts, and streaming.
- They are complementary: an agent can be an A2A server to its peers while being an MCP client to its tools.
- The key difference: an MCP tool is a function with a schema; an A2A agent is opaque, stateful, and may take a long time or ask for more input.

Go deeper: [Interoperability Protocols](../Agentic_AI_Zero_to_Godhood/Volume_07_Multi_Agent_Systems/Chapter_05_Interoperability_Protocols.md).

---

## Q14. Explain the common agentic patterns.

From Anthropic's "Building Effective Agents," which is widely referenced:

| # | Pattern | Shape | Use when | Example |
| --- | --- | --- | --- | --- |
| 1 | Prompt chaining | Sequential steps with checks between | Task splits into fixed subtasks | Outline, check outline, write |
| 2 | Routing | Classify, then dispatch to a specialist | Distinct input categories | Support triage: refund, tech, FAQ |
| 3 | Parallelization | Sectioning (split) or voting (N runs) | Independent parts, or confidence needed | Guardrail check in parallel with answer |
| 4 | Orchestrator-workers | Lead LLM decomposes dynamically | Subtasks not known in advance | Multi-file code change |
| 5 | Evaluator-optimizer | Generator plus critic loop | Clear evaluation criteria exist | Translation refinement |
| 6 | Autonomous agent | Open-ended tool loop | Open problems, verifiable environment | Coding agent with tests |

The first five are workflows; only the sixth is an agent in the strict sense.

---

## Q15. What is planning in agents? Name the approaches.

- **Implicit planning** (ReAct, step by step).
- **Plan-and-execute:** plan up front, execute, re-plan on failure.
  Cheaper, because a smaller model can execute the steps.
- **Tree of Thoughts and search:** explore branches and evaluate them; LATS applies Monte Carlo tree search to agent trajectories.
- **Reflexion:** self-critique after failure, stored as memory for the next attempt.
- **LLMCompiler:** plan a DAG of tool calls and execute independent calls in parallel.
- **Todo-list planning** (used by coding agents): the agent maintains an explicit checklist in context or a file, updating it as it works.

The trade-off: up-front plans are efficient but brittle; reactive planning adapts but can wander.
In practice: plan, execute with checkpoints, and re-plan when an observation contradicts the plan.

Go deeper: [Planning](../Agentic_AI_Zero_to_Godhood/Volume_04_Agent_Architectures/Chapter_03_Planning.md), [Reflection and Self Critique](../Agentic_AI_Zero_to_Godhood/Volume_04_Agent_Architectures/Chapter_04_Reflection_and_Self_Critique.md).

---

## Q16. What types of memory do agents have?

- **Short-term or working memory:** the context window (conversation, scratchpad, tool results).
- **Long-term memory:**
  - **Episodic:** past interactions and trajectories.
  - **Semantic:** facts and user preferences.
  - **Procedural:** learned skills, prompts, instructions (for example, skill files loaded on demand).
- **Implementation:** vector stores, key-value or relational stores, knowledge graphs, file-based memory (a directory the agent reads and writes).
- **Hard problems:**
  - What to write: salience and deduplication.
  - When to retrieve: always, on a trigger, or as a tool the agent calls.
  - Memory conflicts and staleness: keep timestamps and sources; newer or better-sourced facts win.
  - Privacy and deletion: a user must be able to see and delete what is stored.
  - Memory poisoning: an attacker plants instructions that are recalled later; treat recalled memory as untrusted data.

Tie this to the ChromaDB semantic memory in the Sovereign platform (see R4 in [Pitch and Resume](01-Pitch-and-Resume-Deep-Dives.md)).

Go deeper: [Long-Term Memory](../Agentic_AI_Zero_to_Godhood/Volume_06_Memory_and_Context_Engineering/Chapter_05_Long_Term_Memory.md).

---

## Q17. What is context engineering, and why does it matter more than prompt engineering?

- Context engineering curates the **whole** set of tokens the model sees at each step: system prompt, tool definitions, retrieved documents, history, memory, and tool results.
- **Context rot:** accuracy degrades as context grows, even well within the window limit; attention is a finite budget.
- **Techniques:**
  - Compaction: summarize old turns, keep decisions and open issues, drop raw tool output.
  - Just-in-time retrieval: give the agent search tools and lightweight references (file paths, IDs) instead of stuffing everything in.
  - Structured note-taking to external files (a progress file, a todo list).
  - Subagents with clean contexts that return condensed results.
  - Tool-result truncation and clearing of stale tool results.
  - Stable content first, for prompt caching.
  - System prompts at the "right altitude": specific enough to guide, general enough not to hard-code brittle if-else logic.

A context budget for one step, as a whiteboard sketch:

| Slice | Tokens | Cached? |
| --- | --- | --- |
| System prompt and policies | 2,000 | Yes |
| Tool definitions | 3,000 | Yes |
| Memory and retrieved docs | 6,000 | Partly |
| Conversation and tool history | 20,000 (compact above 40,000) | Prefix yes |
| Current tool result | capped at 4,000 | No |

Go deeper: [Context Engineering As A Discipline](../Agentic_AI_Zero_to_Godhood/Volume_06_Memory_and_Context_Engineering/Chapter_01_Context_Engineering_As_A_Discipline.md), [Compaction and Summarization](../Agentic_AI_Zero_to_Godhood/Volume_06_Memory_and_Context_Engineering/Chapter_03_Compaction_and_Summarization.md).

---

## Q18. Single-agent versus multi-agent: when would you use multiple agents?

**Use multi-agent when:**

- The task parallelizes well (broad research across many sources).
- Subtasks need different tools, permissions, or models.
- One context window cannot hold everything, so subagents compress their findings.

**Costs:**

- Many times more tokens.
  Anthropic reported that agents use about 4x the tokens of chat and its multi-agent research system about 15x.
- Coordination failures and duplicated work.
- Harder debugging and non-deterministic interactions.

**Default:** a single agent with good tools.
Anthropic's multi-agent research system beat a single agent by 90.2% on its internal breadth-first research eval, at much higher token cost.
Coding tasks, which are tightly coupled, often do better with a single agent, or with subagents used only for read-only exploration.

Go deeper: [Why And When Multi Agent](../Agentic_AI_Zero_to_Godhood/Volume_07_Multi_Agent_Systems/Chapter_01_Why_And_When_Multi_Agent.md).

---

## Q19. Multi-agent architectures?

| Topology | How control flows | Strength | Main failure |
| --- | --- | --- | --- |
| Supervisor or orchestrator-worker | Hierarchical delegation | Clear ownership | Supervisor is a bottleneck |
| Handoffs or swarm | Peers transfer control | Natural for triage | Lost context in handoffs |
| Network or blackboard | Shared state all agents read and write | Flexible | Write conflicts |
| Sequential pipeline | Fixed order | Predictable | Brittle to upstream errors |
| Debate or critic | Agents argue, a judge decides | Catches errors | Cost, convergence to consensus |

**Failure modes:** infinite delegation loops, lost context in handoffs, conflicting actions, agents agreeing with each other instead of checking.
**Mitigations:** clear role contracts, structured handoff payloads (goal, constraints, findings so far, what "done" means), a depth limit, a single writer for shared state.
My harness is the concrete example: the supervisor never writes code, each worker owns one worktree, and the gate is a third party the worker cannot skip.

Go deeper: [Topologies](../Agentic_AI_Zero_to_Godhood/Volume_07_Multi_Agent_Systems/Chapter_02_Topologies.md), [Failure Modes](../Agentic_AI_Zero_to_Godhood/Volume_07_Multi_Agent_Systems/Chapter_06_Failure_Modes.md).

---

## Q20. What are the main frameworks, and how do they compare?

| Framework | Model | Best for | Watch out for |
| --- | --- | --- | --- |
| LangGraph | Graph or state machine, checkpointers, `interrupt` | Durable, inspectable control flow, human-in-the-loop | Boilerplate; learning curve |
| CrewAI | Role-based crews and flows | Fast prototypes | Less control over the loop |
| Microsoft Agent Framework (successor to AutoGen and Semantic Kernel) | Conversational multi-agent, workflows | .NET and Azure shops | Churn during the merge |
| OpenAI Agents SDK | Agents, handoffs, guardrails, tracing | OpenAI-centric apps | Provider coupling |
| Claude Agent SDK | The harness behind Claude Code: tools, subagents, hooks, MCP, compaction | Coding and file-system agents | Opinionated about its loop |
| LlamaIndex | Data and RAG-centric agents | Document-heavy apps | Abstraction depth |
| Pydantic AI | Type-safe, lightweight | Python teams that want typed outputs | Smaller ecosystem |
| Google ADK | Agent Development Kit, A2A-native | Gemini and Vertex AI | Google-centric |

A good answer:

> "Frameworks help with state, persistence, and tracing.
> For core logic I prefer thin abstractions I understand, because debugging through heavy framework layers is costly.
> I'd pick LangGraph when I need durable, inspectable state machines with human approval steps, and a plain loop when the agent is simple."

Frameworks change fast; check what the company uses before the interview.

Go deeper: [The Landscape and How To Choose](../Agentic_AI_Zero_to_Godhood/Volume_08_Frameworks_and_SDKs/Chapter_01_The_Landscape_and_How_To_Choose.md), [Build Your Own Framework](../Agentic_AI_Zero_to_Godhood/Volume_08_Frameworks_and_SDKs/Chapter_07_Build_Your_Own_Framework.md).

---

## Expansion questions (C1-C12)

**C1. How many tools can one agent handle?**
Selection accuracy falls as the tool list grows and descriptions start to overlap; there is no fixed limit, so measure it on your eval set.
Mitigations: consolidate tools, namespace them, route to a subset per task, or load tools on demand (a tool-search tool that returns relevant definitions, or MCP servers enabled per task).

**C2. How do you handle a tool that takes ten minutes?**
Do not block the loop.
Use a job pattern: `start_job` returns an ID immediately, and `get_job_status` (or a notification) returns the result later; persist the job ID so a restarted agent can resume.
Set a timeout per poll and a total deadline.

**C3. How do you handle dependent versus independent tool calls?**
Independent calls run in parallel (`asyncio.gather` with per-call timeouts, see [Q62](09-Coding-Round.md)); dependent calls run in sequence across turns.
The model decides which calls to emit together, so the tool descriptions should make dependencies clear.

**C4. Structured output versus tool calling for extraction?**
If you only need data in a shape, use structured output (JSON-schema-constrained response); if the model must choose between actions, use tools.
A common trick is a single "respond" tool whose schema is the output format, forced with `tool_choice`.

**C5. What is the instruction hierarchy?**
Models are trained to weight system (or developer) instructions above user messages, and user messages above tool results.
It reduces but does not remove prompt injection; policy that must hold still lives in code.

**C6. What is a subagent, and how does it differ from a handoff?**
A subagent is called like a function: it gets a task, works in its own context, and returns a condensed result; control returns to the caller.
A handoff transfers control of the conversation to another agent, which then talks to the user.

**C7. What are agent skills?**
Packaged instructions and scripts in a folder (for example a `SKILL.md` with a name and description) that an agent loads on demand.
Only the short metadata sits in context; the body loads when the skill is relevant, which is progressive disclosure applied to procedural memory.

**C8. How do you write a good system prompt for an agent?**
State the role, the goal, the tools and when to use each, the constraints, and what "done" looks like, with a few canonical examples rather than a list of edge cases.
Keep it stable so it caches, and version it like code.

**C9. How do long-running agents keep working across context resets?**
Compaction plus external state: a progress file, a todo list, git history, and tests as the source of truth; each new session reads them first.
My overnight loop does exactly this: a fresh agent each iteration, with only a notes file carried forward.

**C10. When would you use computer use or a browser agent instead of an API tool?**
Only when no API exists; screen-based agents are slower, costlier, and more fragile.
Prefer accessibility trees or DOM snapshots over raw screenshots when available.

**C11. What is the difference between a harness and a framework?**
A framework is a library for building agents in code; a harness is the runtime around a complete agent: its tools, permissions, hooks, context management, and sandbox.
Claude Code is a harness; the Claude Agent SDK exposes that harness as a library.

**C12. How do you make an agent ask for clarification instead of guessing?**
Give it an explicit `ask_user` tool or instruction, define when ambiguity is material (it changes the action taken), and evaluate on ambiguous prompts where asking is the correct outcome.

Go deeper: [Harness Design](../Agentic_AI_Zero_to_Godhood/Volume_04_Agent_Architectures/Chapter_06_Harness_Design.md), [Agentic Control Flow](../Agentic_AI_Zero_to_Godhood/Volume_03_Tool_Use_and_the_Agent_Loop/Chapter_06_Agentic_Control_Flow.md).
