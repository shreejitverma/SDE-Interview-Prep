---
type: concept
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# CLI Design and Agent-Friendly Interfaces (AXI)

> [!summary] Scope
> Owns command-line interface design principles for human engineers and autonomous AI agents.
> Codifies clig.dev standards (exit codes, stdout vs stderr separation, isatty, NO_COLOR, signals) alongside the 10 AXI principles (token-efficient TOON output, minimal default schemas, content truncation, precomputed aggregates, definitive empty states, structured errors, ambient context, content-first, contextual disclosure, and concise help).
> Reaches Use through Decide at scale, establishing standards for internal tooling across the vault.

## Related

- Prerequisites: [[Pipelines-Subshells-Redirection-and-Process-Substitution]]
- Next: [[Command-Line-Decision-Records]]
- See also: [[13-Agentic-AI/Agentic-Harness/components/axi|axi]], [[02-Programming-Languages/Python/Python_Zero_to_Godhood/Chapter_47_Command_Line_Interfaces_argparse_cmd_shlex|Chapter_47_Command_Line_Interfaces_argparse_cmd_shlex]]
- Lab: `code/01-Command-Line/axi_cli_tool/`
