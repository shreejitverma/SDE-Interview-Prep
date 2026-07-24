# Volume 07 - Multi-Agent Systems

Multi-agent architectures stripped of the team metaphor: when multiple agent loops genuinely beat one, how to wire them, how they fail, and what the production record actually shows.
Claims about the fast-moving parts (protocols, products, published systems) are date-stamped as of early 2026.

## Chapters

| Chapter | Title | One-line summary |
|---------|-------|------------------|
| 01 | [Why and When Multi-Agent](Chapter_01_Why_And_When_Multi_Agent.md) | Context isolation and parallelism are the only fundamental wins; token multipliers, error compounding, the Cognition-vs-Anthropic debate reconciled by task structure, and a seven-question decision rubric. |
| 02 | [Topologies](Chapter_02_Topologies.md) | Orchestrator-workers, trees, pipelines, peer/debate, swarm/handoff, and blackboard, each analyzed by coordination and communication cost and mapped to the task's dependency graph. |
| 03 | [Communication and Shared State](Chapter_03_Communication_and_Shared_State.md) | Message passing vs shared artifacts, result schemas, filesystem workspaces and git worktrees, damping the telephone game, structured handoff payloads, and context quarantine. |
| 04 | [Subagents in Practice](Chapter_04_Subagents_In_Practice.md) | The clean-context subagent pattern in depth: the five-part delegation contract, parallel fan-out with synthesis, fresh-context adversarial verification, and Claude Code's Agent tool as case study. |
| 05 | [Interoperability Protocols](Chapter_05_Interoperability_Protocols.md) | MCP as the agent-to-tool layer that shipped, A2A agent cards and task lifecycles, ACP's consolidation, adoption reality as of early 2026, and a verdict on premature standardization. |
| 06 | [Failure Modes](Chapter_06_Failure_Modes.md) | The fourteen-mode MAST taxonomy, miscoordination case studies of duplicated, dropped, and conflicting work, cost blowups, trace debugging, and the mitigation toolkit with its costs. |
| 07 | [Case Studies](Chapter_07_Case_Studies.md) | Anthropic's research system with its 15x economics, deep-research rivals as an orchestration-vs-training natural experiment, the honest ChatDev/MetaGPT record, and coding-agent fleet patterns. |

## Position in the track

This volume assumes the agent loop (Volume 03), architectures (Volume 04), and context engineering (Volume 06), and it feeds directly into the framework implementations of Volume 08, the MCP deep dive of Volume 09, and the system-level evaluation of Volume 10.
