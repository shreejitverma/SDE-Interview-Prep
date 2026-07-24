# Chapter 01 - Why MCP Exists

## What you will master

- The NxM integration problem that MCP was designed to collapse into N+M.
- The USB-C analogy, what it gets right, and precisely where it breaks down.
- The history of the protocol from Anthropic's open-source release in November 2024 through industry-wide adoption in 2025 and stewardship changes into 2026.
- A clear statement of what MCP is not: not an agent-to-agent protocol, not an agent framework, not a model API, not a replacement for writing code.
- The economic and organizational forces that make a context protocol viable now when earlier plugin ecosystems failed.

All spec details in this volume are stated as of early 2026, against the dated MCP spec revisions 2024-11-05, 2025-03-26, and 2025-06-18, with notes where the November 2025 revision changed things.

## 1. The integration problem before MCP

By late 2024, every serious LLM application needed the same thing: a way to give the model access to data and actions that live outside the model.
Tool use (covered in Volume 03) solved the mechanics of this at the API level: the model emits a structured call, the harness executes it, the result goes back into context.
What tool use did not solve was distribution.

Consider the state of the world in mid-2024.
Suppose there are N AI applications: Claude Desktop, ChatGPT, Cursor, Zed, a custom internal agent, a customer support bot.
Suppose there are M systems those applications want to reach: GitHub, Slack, Postgres, Google Drive, Jira, an internal ticketing API.
Without a shared protocol, every pairing requires bespoke glue code: N times M integrations.
Each integration re-implements the same concerns: authentication, schema definition, error handling, result formatting, pagination, rate limiting.
Each is maintained by whichever team needed it first, drifts independently, and breaks independently.

The NxM problem is not hypothetical; it is exactly what happened with LLM plugins before MCP.
OpenAI's ChatGPT plugins (announced March 2023) defined a plugin manifest plus an OpenAPI spec, but the plugin only worked inside ChatGPT.
LangChain accumulated hundreds of tool integrations, but they only worked inside LangChain's abstractions.
Every framework and every host application grew its own incompatible tool catalog.
An integration written for one host was worthless in every other host.

MCP's core move is to standardize the interface between the AI application and the integration, turning NxM into N+M.
Each AI application implements the protocol once as a client.
Each system-of-record implements the protocol once as a server.
Any compliant client can then use any compliant server, with no pairwise work.
This is the same structural move that ODBC made for databases, LSP made for editor language tooling, and HTTP made for document retrieval.

The LSP comparison is the most instructive because MCP's designers cited it explicitly.
Before the Language Server Protocol, every editor implemented language intelligence for every language: another NxM.
LSP defined a JSON-RPC protocol between an editor (client) and a language server, and within a few years every serious editor and language had one implementation each.
MCP borrows from LSP the JSON-RPC 2.0 base, the client-server split, the initialization handshake with capability negotiation, and the philosophy that the protocol describes capabilities rather than specific features.
The trade-off LSP accepted, and MCP inherits, is lowest-common-denominator pressure: a protocol serving many hosts cannot expose every host-specific feature, so hosts differentiate through optional capabilities, which fragments support in practice.

## 2. The USB-C analogy and its limits

Anthropic's own launch material described MCP as "a USB-C port for AI applications."
The analogy earns its popularity because it captures three real properties.

First, standardized connection: any device with the port can connect to any peripheral with the plug, without either vendor knowing about the other in advance.
Second, capability negotiation: USB-C devices negotiate what they can do over the wire (power delivery wattage, alternate modes such as DisplayPort), and MCP clients and servers negotiate capabilities at initialization rather than assuming a fixed feature set.
Third, ecosystem leverage: once the connector is standard, peripheral makers invest because the addressable market is every host, not one.

The analogy misleads in at least four ways, and you should be able to articulate each.

One: USB-C connects hardware with fixed, well-specified electrical behavior, while MCP connects a probabilistic model to arbitrary code.
Plugging in a USB-C drive cannot change what your keyboard types; connecting an MCP server injects text into a model's context and can absolutely change what the model does next.
This is why Volume 09 dedicates an entire chapter to security: the "port" carries instructions, not just data.

Two: USB-C negotiation is symmetric and mechanical, while MCP interoperability is only as good as the model's ability to use what it is given.
A tool with a poorly written description "connects" fine at the protocol level and still fails in practice because the model misuses it.
Protocol compatibility does not guarantee semantic usability; this gap has no analog in USB-C.

Three: USB-C has a certification program and compliance testing, while MCP as of early 2026 has no conformance certification.
Anything that speaks roughly the right JSON-RPC is "an MCP server," and quality varies wildly across the thousands of community servers.
The analogy suggests a level of interchangeability that the ecosystem does not yet enforce.

Four: cost. Plugging in a USB device is free at the host; attaching an MCP server is not, because every tool definition consumes context-window tokens on every request, and every tool result consumes more.
Attach twenty servers with fifteen tools each and you have spent tens of thousands of tokens before the user says anything.
This context economics problem (Volume 06) drives the gateway and code-mode patterns discussed in Chapter 07.

Use the analogy to explain MCP's purpose in one sentence; do not use it to reason about MCP's failure modes.

## 3. History: from launch to industry standard

Dates matter here because the ecosystem moved fast and many written claims rot within months.

November 25, 2024: Anthropic open-sourced MCP with the first dated spec revision, 2024-11-05.
The launch included the specification, TypeScript and Python SDKs, a set of reference servers (filesystem, GitHub, Slack, Postgres, Puppeteer, and others), and support in Claude Desktop.
Initial reception was muted; the launch looked to many like a vendor plugin system with extra steps, and only Claude Desktop could act as a host.

Early adopters through late 2024 and early 2025 were developer tools: Zed, Cursor, Windsurf (then Codeium), Replit, Sourcegraph, and Cline added client support because coding assistants had the most acute need for standardized context access.
This mattered strategically: developer tools are where integration pain is felt daily and where users will tolerate rough edges.

March 2025 was the inflection point.
OpenAI announced MCP support, first in the Agents SDK, with ChatGPT desktop and the Responses API following.
A protocol authored by one frontier lab being adopted by its chief competitor converted MCP from "Anthropic's plugin system" into a candidate industry standard, because it removed the primary reason to wait.
In April 2025, Google DeepMind announced Gemini would support MCP, with Demis Hassabis publicly calling it a rapidly emerging open standard.
Microsoft followed through 2025: MCP support in Copilot Studio, in VS Code's agent mode, and an announcement at Build in May 2025 of OS-level MCP integration in Windows 11.

The specification itself evolved in parallel, using date-based revisions rather than semantic versions.
Revision 2025-03-26 brought OAuth 2.1-based authorization, the streamable HTTP transport replacing HTTP+SSE, tool annotations, and audio content support.
Revision 2025-06-18 brought structured tool output, elicitation, resource links in tool results, a tightened OAuth model classifying MCP servers as resource servers with mandatory resource indicators, and removed the short-lived JSON-RPC batching feature.
A further revision landed in November 2025, adding among other things support for long-running asynchronous task patterns and an extension mechanism; treat the details of that revision as newer and less settled than the three revisions this volume covers deeply.
Chapter 02 covers the versioning scheme and negotiation mechanics.

Governance also shifted.
MCP began as an Anthropic-controlled open-source project with an informal steering group and public SEP (specification enhancement proposal) process.
In December 2025, Anthropic announced the donation of MCP to the Linux Foundation, placing it under neutral governance.
The ecosystem context makes the motive legible: Google had donated its Agent2Agent (A2A) protocol to the Linux Foundation in mid-2025, and enterprises are reluctant to standardize on infrastructure controlled by a single competitor of their other vendors.
Neutral stewardship trades slower, committee-driven evolution for credibility as a durable standard; that is the standard trade and Anthropic took it deliberately.

By early 2026 the ecosystem numbers are large but should be quoted as orders of magnitude: thousands of community servers, an official registry (in preview since September 2025), and client support in effectively every major AI application.
Quote magnitudes, not counts; any specific count is stale before it is published.

## 4. What MCP is not

Precision about scope prevents most beginner architecture mistakes.

MCP is not an agent-to-agent protocol.
It standardizes how one application (the host) reaches context and capabilities, a hub-and-spoke shape with the host at the hub.
Two agents that both speak MCP cannot discover each other, negotiate tasks, or exchange messages peer-to-peer through MCP; that is the problem Google's A2A protocol and similar efforts target.
The confusion is common because an MCP server can wrap an agent, exposing "ask the research agent" as a tool, but the protocol semantics remain request-response from a single host's perspective, with the November 2025 task additions easing, not removing, this constraint.

MCP is not an agent framework.
It has no opinion on planning, memory, orchestration, retries, or the agent loop.
LangGraph, the OpenAI Agents SDK, and the Claude Agent SDK are frameworks; each of them can consume MCP servers as a tool source.
Choosing MCP is orthogonal to choosing a framework, and "should we use MCP or LangGraph" is a category error you should be able to correct in one sentence.

MCP is not a model API.
It does not carry prompts to a model provider; it carries context and capabilities to an application that owns its own model access.
The one deliberate exception is sampling (Chapter 04), where a server requests a completion through the client, and even there the client owns the model relationship.

MCP is not a data plane for bulk transfer.
It moves context-sized payloads over JSON-RPC; it is the wrong tool for streaming gigabytes, and servers that need bulk transfer should return references (URLs, resource links) rather than content.

MCP is not automatically better than writing code.
For a single application with three integrations owned by one team, direct function calls are simpler, faster, cheaper in tokens, and easier to debug; the protocol pays for itself only when the integration matrix or the organizational boundary grows.
Chapter 07 treats this debate fully and fairly.

## 5. Why this succeeded where plugins failed

It is worth closing with the causal analysis, because "open standard" alone does not explain adoption; plenty of open standards die.

First, timing: by late 2024 tool use was reliable enough in frontier models that integrations actually worked, which was not true in the ChatGPT plugin era of early 2023.
Second, the local-first wedge: the stdio transport let developers run servers as local subprocesses with zero deployment, zero auth, and full filesystem access, which made the first-hour experience trivial and seeded thousands of servers before remote deployment was even specified well.
Third, the right abstraction level: MCP standardizes capability exchange, not agent behavior, so it composed with every framework instead of competing with them.
Fourth, competitor adoption: OpenAI's March 2025 endorsement resolved the standards-war uncertainty early, before the ecosystem fragmented.
Fifth, real reference implementations: SDKs and dozens of runnable servers shipped on day one, so the spec was never paper-only.

The honest counterweights: the security model shipped late and incidents in 2025 were real (Chapter 07); context-window costs of naive multi-server setups are severe (Chapters 03 and 07); and the protocol's rapid revision cadence through 2025 imposed churn on early implementers.
A senior engineer should hold both facts: MCP won the standard, and the standard is still paying down debt from the speed of its own adoption.

## Exercises

1. Write out the integration matrix for your own organization: list the AI-facing applications (N) and the systems they need (M), count the bespoke integrations that exist today, and compute what N+M would be under MCP.
2. Explain the USB-C analogy to a colleague, then immediately present the four ways it breaks down, and note which breakdown they found most surprising.
3. Take one integration you have written directly against a provider tool-use API and sketch, on paper only, how it would split into an MCP client side and server side; identify which code disappears and which code merely moves.
4. Read the changelog sections of the 2025-03-26 and 2025-06-18 spec revisions on modelcontextprotocol.io and write a five-line summary of what each revision added and removed.
5. Find one public argument that MCP is unnecessary (the "just write code" position) and one that it is essential, and write a paragraph steelmanning each; keep this, because Chapter 07 will ask you to revisit it.
6. Identify one system in your stack that should not get an MCP server, and defend the exclusion in terms of token cost, security exposure, or organizational ownership.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- State the NxM problem and the N+M solution in under a minute, with a concrete example from your own stack.
- Give the launch date, the three core spec revisions this volume covers, and the rough timeline of OpenAI, Google, and Microsoft adoption.
- Explain why LSP is the better structural analogy than USB-C, and name the lowest-common-denominator trade-off both protocols share.
- Correct, in one sentence each, the three most common category errors: MCP as agent-to-agent protocol, MCP as framework, MCP as model API.
- Argue both sides of "MCP versus just write code" and state the matrix conditions (integration count, organizational boundaries) under which each side wins.
- Explain why the Linux Foundation donation happened and what enterprises gain and lose from neutral governance.
