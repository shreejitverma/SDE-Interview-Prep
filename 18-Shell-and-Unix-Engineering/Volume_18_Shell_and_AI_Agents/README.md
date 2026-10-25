---
type: moc
track: [sde, distinguished, ai-eng]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 18 - Shell and AI Agents

Coding agents act on the world mostly through a shell.
This volume covers how agent harnesses execute commands, quoting across tool boundaries, safety guards and sandboxes, and how to design commands that agents use well.

## Prerequisites

- [Volume 09 - Robust and Defensive Scripting](../Volume_09_Robust_and_Defensive_Scripting/README.md)
- [Volume 13 - Shell Security](../Volume_13_Shell_Security/README.md)

## Chapters

1. [How Coding Agents Execute Shell](Chapter_01_How_Coding_Agents_Execute_Shell.md) - Persistent versus fresh shells, working directory and environment state, timeouts, and output truncation.
2. [Quoting Across Tool Boundaries](Chapter_02_Quoting_Across_Tool_Boundaries.md) - JSON to shell, nested quoting, here-documents, and avoiding injection through generated commands.
3. [Command Safety Guards and Hooks](Chapter_03_Command_Safety_Guards_and_Hooks.md) - Pre-execution policy checks, deny and ask lists, and how they are bypassed.
4. [Sandboxing Agent Shells](Chapter_04_Sandboxing_Agent_Shells.md) - macOS Seatbelt, Linux Landlock, containers, and network restrictions.
5. [Designing Commands for Agents](Chapter_05_Designing_Commands_for_Agents.md) - Idempotency, dry runs, structured output, definitive empty states, and the AXI principles.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v18-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
