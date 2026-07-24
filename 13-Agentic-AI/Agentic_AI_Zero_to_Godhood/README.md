# Agentic AI: Zero to Godhood

A complete, first-principles-to-frontier curriculum for agentic AI engineering.
The goal is the same as the other Zero to Godhood tracks in this repository: total mastery, not survey-level familiarity.
By the end you should be able to design, build, evaluate, secure, and operate production agent systems, and to read frontier research without translation.

## How this track is organized

Fourteen volumes, ordered as a dependency graph.
Each volume is a directory of chapter files.
Each chapter is written to be self-contained but assumes the volumes before it.
Appendices hold reference material: glossary, paper list, benchmark index, interview drills, and a pattern library.

| Volume | Title | What you master |
|--------|-------|-----------------|
| 01 | LLM Foundations | Transformers, tokenization, training, RLHF, scaling laws, inference mechanics |
| 02 | Working With LLMs | Prompting, sampling, structured output, embeddings, context windows, APIs |
| 03 | Tool Use and the Agent Loop | Function calling, the core agent loop, error handling, agentic control flow |
| 04 | Agent Architectures | ReAct, planning, reflection, orchestrator patterns, workflow vs agent design |
| 05 | RAG and Knowledge Systems | Retrieval, vector databases, chunking, hybrid search, agentic RAG |
| 06 | Memory and Context Engineering | Context management, compaction, short and long-term memory, state |
| 07 | Multi-Agent Systems | Orchestration topologies, communication, handoffs, agent-to-agent protocols |
| 08 | Frameworks and SDKs | Claude Agent SDK, OpenAI Agents SDK, LangGraph, CrewAI, AutoGen, smolagents, Pydantic AI |
| 09 | Model Context Protocol | MCP architecture, servers, clients, transports, security, ecosystem |
| 10 | Evaluation and Observability | Evals, LLM-as-judge, tracing, benchmarks (SWE-bench, tau-bench, GAIA) |
| 11 | Safety, Security, Alignment | Prompt injection, sandboxing, guardrails, alignment for agents |
| 12 | Production Engineering | Deployment, cost, latency, caching, scaling, reliability, infra |
| 13 | Coding Agents and Computer Use | SWE agents, browser agents, computer use, harness design |
| 14 | Frontier and Capstones | RL for agents, reasoning models, research frontier, capstone builds |

## Learning path

Phase 1 - Foundations (Volumes 01-02).
Understand what the model actually is before orchestrating it.
Everything in agent engineering bottoms out in how transformers, context windows, and sampling work.

Phase 2 - The core craft (Volumes 03-06).
The agent loop, architectures, retrieval, and context engineering.
This is the daily working knowledge of an agent engineer.

Phase 3 - Systems (Volumes 07-09).
Multi-agent coordination, the framework landscape, and MCP as the interoperability layer.

Phase 4 - Rigor (Volumes 10-12).
Evaluation, security, and production operations.
This is what separates demos from systems that survive contact with users.

Phase 5 - Frontier (Volumes 13-14).
Coding agents and computer use are the most advanced deployed agent category.
Volume 14 takes you to the edge of current research and closes with capstone projects.

## Ground rules baked into this track

Build everything once from scratch before reaching for a framework.
The agent loop is about a hundred lines of code; you should have written it yourself.
Prefer simple, inspectable designs; add autonomy only when it earns its cost.
Evaluate before you believe; every claim about agent quality needs an eval behind it.
Treat security as a design input, not a patch; agents are a new attack surface.

## Conventions

Code examples use Python and the provider-neutral parts use pseudocode.
Provider-specific examples favor the Anthropic and OpenAI APIs since they anchor the ecosystem.
Every chapter ends with exercises and a "Godhood check": questions you must answer cold before moving on.
Sources and papers are collected in Appendix B rather than scattered per chapter.
