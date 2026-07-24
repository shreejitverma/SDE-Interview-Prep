# Volume 13 - Coding Agents and Computer Use

Software engineering was the first domain where agents crossed from demo to daily infrastructure.
This volume explains why that happened, dissects the systems that did it, extends the analysis to the harder surfaces (browser and desktop), and ends by having you build and measure a coding agent of your own.

Knowledge is current as of early 2026.
Product details, benchmark scores, and API version strings will rot; the architectural arguments are written to outlast them, and claims that will age are date-stamped where they appear.

## Chapters

| # | Chapter | One-line summary |
|---|---------|------------------|
| 01 | [Why Coding Agents Lead](Chapter_01_Why_Coding_Agents_Lead.md) | Verifiable rewards, rich tool ecosystems, economic pull, and RL trainability explain why coding led, plus the 2021-2026 trajectory from autocomplete to autonomous engineers. |
| 02 | [Anatomy of a Coding Agent](Chapter_02_Anatomy_Of_A_Coding_Agent.md) | The five-component skeleton dissected through Claude Code: system prompt, tool surface, permission model, CLAUDE.md, hooks, skills, plan mode, subagents, plus why terminal-first won and how the field compares. |
| 03 | [SWE Agents and Scaffolds](Chapter_03_SWE_Agents_and_Scaffolds.md) | SWE-bench in depth, SWE-agent's agent-computer-interface insight, Agentless and scaffold minimalism, Devin's ambition, the scaffold-decay thesis, repository navigation, and long-horizon task management. |
| 04 | [Browser Agents](Chapter_04_Browser_Agents.md) | DOM versus accessibility tree versus vision, the Playwright and CDP stack, WebArena and BrowseComp-style evaluation, the canonical failure modes, the early-2026 landscape, and why indirect prompt injection is unsolved. |
| 05 | [Computer Use](Chapter_05_Computer_Use.md) | The screenshot-action loop, coordinate versus element grounding, the Anthropic computer use API lineage, OSWorld, the latency and reliability realities, when to choose it over an API, and virtual desktop infrastructure. |
| 06 | [Async Agents and Fleets](Chapter_06_Async_Agents_and_Fleets.md) | From pairing to delegation: background and cloud execution, git worktree parallelism, CI-triggered agents, the verification pyramid for reviewing agent code, fleet operations, and the agent-manager role. |
| 07 | [Build Your Own Coding Agent](Chapter_07_Build_Your_Own_Coding_Agent.md) | Capstone: a runnable ~300-line terminal coding agent with tools, permissions, transcripts, and compaction; the hardening pass; and a fail-to-pass eval harness with tamper detection. |

## Reading order and dependencies

Read 01 through 03 in order; they build one argument about why the domain works and how the systems evolved.
Chapters 04 and 05 are independent of each other and both depend on 02's harness vocabulary.
Chapter 06 assumes 02 and 03.
Chapter 07 assumes 02, 03, and 06, and is best done with a terminal open.

Cross-volume dependencies: Volume 3 for the agent loop and tool-use mechanics, Volume 6 for memory and context engineering, Volume 10 for evaluation methodology, and Volume 11 for the security material this volume repeatedly defers to.
