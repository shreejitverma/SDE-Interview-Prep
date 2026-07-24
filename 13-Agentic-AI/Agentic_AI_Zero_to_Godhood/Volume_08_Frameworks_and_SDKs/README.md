# Volume 08 - Frameworks and SDKs

The framework landscape as of early 2026: what each major agent framework actually is, what it costs, when to adopt one, and how to build your own thin layer instead.
This volume assumes you built the agent loop yourself in Volume 03; frameworks are evaluated as engineering trade-offs, not adopted as defaults.

## Chapters

| Chapter | Title | One-line summary |
|---------|-------|------------------|
| 01 | The Landscape and How To Choose | The six-category framework map, the abstraction tax, lock-in priced by type, and the raw-API-first doctrine. |
| 02 | Claude Agent SDK | The Claude Code harness made programmable: built-in tools, MCP, subagents, hooks, permissions, and sessions, where you subtract capability rather than add it. |
| 03 | OpenAI Agents SDK | From Swarm's minimalist thesis to production: agents, handoffs, guardrails, sessions, the Responses API's server-side tools, and the AgentKit platform. |
| 04 | LangChain and LangGraph | The boom-backlash-pivot history, LCEL, graph state machines with checkpointing and human-in-the-loop, the 1.0 era, and an honest verdict on durable execution. |
| 05 | The Rest of the Field | CrewAI, the AutoGen-to-Microsoft-Agent-Framework lineage, smolagents, Pydantic AI, Google ADK, Mastra, and the Vercel AI SDK, each with philosophy, sweet spot, and weaknesses. |
| 06 | The Plumbing Layer | LiteLLM, gateways like OpenRouter, instructor, outlines and guidance, the lowest-common-denominator and cache-busting costs of provider abstraction, and when to standardize on one provider. |
| 07 | Build Your Own Framework | A ~300-line reference micro-framework (provider seam, tool registry, hooks, loop, transcripts, subagents) built incrementally, plus the discipline of what never to abstract. |
