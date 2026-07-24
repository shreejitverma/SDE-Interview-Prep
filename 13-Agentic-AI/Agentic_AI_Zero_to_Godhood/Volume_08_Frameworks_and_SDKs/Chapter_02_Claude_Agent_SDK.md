# Chapter 02 - Claude Agent SDK

Knowledge in this chapter is current as of early 2026.
The SDK evolved quickly through 2025; specific option names and defaults may have shifted, so verify against the current Anthropic documentation before writing production code.

## What you will master

- What the Claude Agent SDK actually is: the Claude Code harness, extracted and made programmable.
- The agent loop the SDK runs for you, and how it differs from the bare loop you built in Volume 03.
- The built-in tool suite: shell, file operations, search, and web access, and why a filesystem-centric toolset generalizes beyond coding.
- MCP integration, including in-process MCP servers for exposing your own Python or TypeScript functions as tools.
- Subagents, hooks, the permission system, and session management.
- The Python and TypeScript API shapes, and the decision criteria for when this SDK is the right choice.

## Lineage: from Claude Code to a general agent harness

Claude Code shipped in early 2025 as a terminal coding agent.
Its architecture was a general agent harness wearing a coding costume: a loop, a filesystem, a shell, search tools, and a permission gate.
Anthropic first exposed this harness programmatically as the Claude Code SDK, then renamed it the Claude Agent SDK in late 2025, signaling that the harness was for agents in general, not just coding.
The rename mattered conceptually: Anthropic's public position became that the primitives a coding agent needs (act on a real environment, verify results, iterate) are the primitives most agents need.

This lineage explains the SDK's personality.
Where the OpenAI Agents SDK gives you abstractions and asks you to bring tools, the Claude Agent SDK gives you a working autonomous agent on line one and asks you to constrain it.
You subtract capability with permissions rather than add capability from zero.
That inversion is the single most important thing to understand about it.

## The agent loop you get

The SDK runs the full loop from Volume 03 internally: assemble context, call the model, execute requested tools, feed results back, repeat until the model produces a final answer or a limit is hit.
On top of the bare loop it adds the machinery Claude Code needed in production:

- Automatic context management, including compaction of long transcripts so sessions can exceed the context window.
- Tool execution with a permission gate in front of every side-effecting call.
- Streaming of intermediate events (assistant text, tool calls, tool results) so a host application can render progress.
- Error handling and retry behavior for transient API failures.
- Session persistence, so a conversation can be resumed by id.

The trade-off is the standard abstraction tax from Chapter 01: you get a production loop for free, and in exchange the loop's internals (compaction triggers, retry policy, exact prompt scaffolding) are Anthropic's decisions, inspectable but not primarily yours.
The SDK also injects a substantial system prompt of its own when you opt into the Claude Code preset, and you must read the docs on system prompt behavior to know exactly what your agent has been told.

## The built-in tools

The default toolset is inherited from Claude Code; names here are the tool names as of late 2025.

- Bash: run shell commands, the universal escape hatch to every CLI on the machine.
- Read, Write, Edit: file operations, with Edit doing exact string replacement rather than regenerating whole files.
- Glob and Grep: filename and content search, the agent's primary way to orient in a large directory tree.
- WebSearch and WebFetch: search the web and fetch page content.
- Task: spawn a subagent (covered below).
- Various coding-specific helpers (notebook editing and similar) that matter less outside coding.

The design insight worth internalizing: the filesystem is a general-purpose agent workspace, not a coding detail.
An agent researching a market can write notes to files, grep them later, and treat the directory as external memory; this is the "agent workspace" pattern from Volume 06 with the SDK providing it natively.
Bash generalizes similarly: any task with a CLI (cloud provider operations, data processing, video encoding) is in reach without writing a custom tool.
The cost of this generality is risk: Bash plus Write is arbitrary code execution by construction, which is why the permission system is not optional decoration but the core safety mechanism, and why Volume 11's sandboxing material applies in full.

## MCP integration

The SDK is a first-class MCP client, which Volume 09 covers protocol-deep; here is the framework-level view.
You attach MCP servers through configuration, and their tools appear in the agent's toolset with names prefixed by the server, in the pattern mcp__servername__toolname.

Three connection styles matter:

- External servers over stdio: the SDK launches a subprocess (any language) speaking MCP.
- Remote servers over HTTP transports: connect to servers running elsewhere.
- In-process SDK servers: define tools as functions in your own Python or TypeScript process, wrapped in an MCP server object, with no subprocess and no serialization boundary.

The in-process style is how you add custom tools idiomatically.
In Python, the shape as of late 2025 was a tool decorator plus a server constructor.

```python
# Python, Claude Agent SDK, shape current as of late 2025.
from claude_agent_sdk import tool, create_sdk_mcp_server, ClaudeAgentOptions, query

@tool("lookup_order", "Look up an order by id", {"order_id": str})
async def lookup_order(args):
    order = await db.get_order(args["order_id"])
    return {"content": [{"type": "text", "text": str(order)}]}

server = create_sdk_mcp_server(name="shop", version="1.0.0", tools=[lookup_order])

options = ClaudeAgentOptions(
    mcp_servers={"shop": server},
    allowed_tools=["mcp__shop__lookup_order"],
)
```

The trade-off of routing custom tools through MCP rather than a plain function registry: you get protocol uniformity (the same tool can later move out of process, or be shared with other MCP clients) and pay a small ceremony cost and the MCP result format.

## Subagents

The Task tool lets the agent spawn a subagent: a fresh context window, optionally a restricted toolset, and a prompt describing its job; the subagent runs its own loop and returns a single result message to the parent.
You can also define named subagents with their own system prompts and tool restrictions, either as markdown files in a project's agent directory or programmatically in options.

Why this matters, connecting to Volumes 06 and 07:

- Context isolation: a subagent can burn fifty thousand tokens reading files, and the parent receives only the distilled answer, which is the context-quarantine pattern.
- Least privilege: a research subagent can be granted read-only tools even when the parent can write.
- Parallelism: multiple subagents can run concurrently on independent shards of a problem.

The costs are the ones Volume 07 taught: subagents cannot see the parent's conversation, so poorly specified tasks produce confidently wrong results, and token spend multiplies.
The SDK's subagent model is deliberately hierarchical (parent spawns, child returns, no peer-to-peer chatter), which avoids most multi-agent coordination pathologies at the price of expressiveness.

## Hooks

Hooks are user-supplied functions the loop calls at defined lifecycle points, and they are the SDK's mechanism for deterministic control in an otherwise model-driven loop.
Key hook events as of late 2025 include PreToolUse (before a tool runs, with power to allow, deny, or modify), PostToolUse (after a tool runs), and events around session lifecycle and compaction.

```python
# Python, illustrative shape: a PreToolUse hook that blocks dangerous commands.
async def block_rm(input_data, tool_use_id, context):
    if input_data["tool_name"] == "Bash":
        cmd = input_data["tool_input"].get("command", "")
        if "rm -rf" in cmd:
            return {
                "hookSpecificOutput": {
                    "hookEventName": "PreToolUse",
                    "permissionDecision": "deny",
                    "permissionDecisionReason": "Destructive command blocked by policy.",
                }
            }
    return {}
```

The design principle: hooks turn "please do not do X" from a prompt suggestion into an enforced invariant.
Prompts are advisory and probabilistic; hooks are code and certain.
Anything security-critical belongs in a hook or permission rule, never only in the prompt, a rule Volume 11 elevates to doctrine.

## Permissions

The permission system decides, for every tool call, whether it runs.
The layers, roughly in order of evaluation:

- Permission modes: default (ask for risky actions), acceptEdits (auto-approve file edits), plan (analyze without executing), and bypassPermissions (approve everything, for sandboxed environments only).
- Allow and deny rules: patterns over tools and arguments, such as allowing Bash(git status) while denying Bash(git push), configurable in settings files or options.
- The canUseTool callback: a programmatic decision function invoked when no rule settles the question, which is how a host application implements interactive approval UI.
- Hooks, as above, which can veto anything.

The layering exists because different actors need different control points: administrators set static rules, applications make dynamic decisions, and end users answer prompts.
The sharp edge: permission configuration is security configuration, and bypassPermissions outside a disposable sandbox converts every prompt injection in fetched web content into arbitrary code execution on your machine.

## Sessions

Each conversation is a session with an id; the SDK persists transcripts and can resume a session later, continuing with full context.
Forking a session lets you branch alternative continuations from a common prefix, useful for exploring options or for evals that share an expensive setup.
Long sessions trigger automatic compaction, which summarizes older turns to reclaim context; you gain unbounded session length and lose verbatim recall of compacted detail, exactly the trade-off Volume 06 analyzed.

## The Python API shape

Two entry points, shapes current as of late 2025.

query: a one-shot async generator, best for fire-and-forget tasks.

```python
# Python, Claude Agent SDK, shape current as of late 2025.
import anyio
from claude_agent_sdk import query, ClaudeAgentOptions

async def main():
    options = ClaudeAgentOptions(
        system_prompt="You are a code review assistant.",
        allowed_tools=["Read", "Grep", "Glob"],
        max_turns=10,
        cwd="/path/to/repo",
    )
    async for message in query(prompt="Review the diff in HEAD for bugs.", options=options):
        print(message)

anyio.run(main)
```

ClaudeSDKClient: a stateful client for multi-turn conversations, interrupts, and hooks, used as an async context manager with methods to send follow-up messages and stream responses.
The messages you iterate are typed: assistant messages containing text and tool-use blocks, tool results, and a final result message carrying the outcome, cost, and session id.
One operational note: the Python SDK drives the bundled Claude Code runtime under the hood, so it carries a Node.js runtime dependency; this surprises people doing slim container builds.

## The TypeScript API shape

The TypeScript package (published under the Anthropic npm scope as the claude-agent-sdk) mirrors the Python surface: a query function taking a prompt and an options object, returning an async iterable of messages, with the same options vocabulary (allowedTools, mcpServers, permissionMode, hooks, resume).

```typescript
// TypeScript, Claude Agent SDK, shape current as of late 2025.
import { query } from "@anthropic-ai/claude-agent-sdk";

for await (const message of query({
  prompt: "Summarize the TODO comments in this repo.",
  options: { allowedTools: ["Grep", "Read"], maxTurns: 8 },
})) {
  if (message.type === "result") console.log(message);
}
```

Feature parity between the two languages was close but not perfect through 2025; check current docs for the language you deploy.

## When it is the right choice

Choose the Claude Agent SDK when:

- You are committed to Claude models, so provider lock-in is a decision already made rather than a new cost.
- Your agent benefits from a real environment: filesystem, shell, and long-horizon autonomous work.
- You want Claude Code's production-hardened loop, compaction, and permission machinery without rebuilding them.
- Your custom integrations are or will be MCP servers, making the SDK a natural host.

Prefer something else when:

- You need multi-provider routing or model portability, where a neutral framework or your own layer wins.
- Your agent is a thin, latency-sensitive API feature (a classifier, an extractor), where the full harness is heavy machinery for a small job and the raw Messages API is simpler and faster.
- You need explicit graph-shaped orchestration with checkpointed, replayable state transitions, which is LangGraph's territory.
- You cannot tolerate the injected scaffolding and runtime weight, and want to own every token, which is the Chapter 07 path.

The honest downside summary: it is the most capable off-the-shelf harness as of early 2026, and it is also a strongly opinionated, single-provider system whose power (Bash, Write, autonomy) is exactly what makes it dangerous to configure carelessly.

## Exercises

1. Install the Python SDK and run a read-only agent (Read, Grep, Glob only) against a repository you know well; ask it a question whose answer you can verify, and inspect every message type it streams.
2. Write a PreToolUse hook that logs every tool call to a JSONL file with timestamp, tool name, and inputs; then aim the agent at a small task and reconstruct its behavior purely from your log.
3. Build an in-process MCP server exposing one tool backed by a real data source you own, and confirm the agent can discover and use it; then deny it via allowed_tools and observe the failure mode.
4. Configure a subagent with a restricted toolset and delegate a research task to it; compare parent context growth with and without the subagent for the same task.
5. Deliberately create a permission conflict: an allow rule, a deny rule, and a canUseTool callback that disagree about the same Bash command; determine the actual precedence empirically and write it down.
6. Measure the wall-clock and token cost difference between the same small task run through the SDK versus a hand-rolled Volume 03 loop on the raw Messages API; explain where the difference comes from.

## Godhood check

Answer these cold before moving on.

- Explain "you subtract capability rather than add it" and why that inversion follows from the SDK's Claude Code lineage.
- Why does a filesystem-plus-shell toolset generalize beyond coding, and what pattern from Volume 06 does the workspace implement?
- Name the three ways to attach MCP tools and the trade-off of the in-process style.
- Why must security constraints live in hooks or permission rules rather than in the system prompt?
- What does the parent agent see of a subagent's work, and which two costs does subagent delegation always incur?
- Give two situations where this SDK is the wrong choice even for a Claude-committed team, and say what you would use instead in each.
