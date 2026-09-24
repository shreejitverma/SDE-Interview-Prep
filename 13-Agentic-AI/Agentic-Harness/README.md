---
type: moc
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://github.com/shreejitverma/agents, https://github.com/shreejitverma/fleet-ops, https://github.com/shreejitverma/dotfiles-nix, https://github.com/shreejitverma/firstmate]
---

# Agentic Harness

The complete, source-cited documentation of my personal agentic engineering harness: every repository, CLI, skill, hook, subagent, and config file, and how they connect.
Everything was verified read-only against the local clones under `~/github` and the live config under `~` on 2026-09-23.
Citations use `repo/path:line` relative to `~/github`.

## Authorship, stated once

Most components are open-source tools written upstream (mostly `kunchenguid/*`; `wheelhouse` is from `ImZoomBoy/wheelhouse`) that I forked and run daily.
My own work is the integration layer: the `agents` manual generator and guard hooks, the model routing policy, `fleet-ops`, the `dotfiles-nix` fork additions, the firstmate fork patches, and operating the whole system.
Each component note has a "Fork delta" section that separates the two with commit hashes.

## Reading order for tonight

1. [Cheat sheet](interview/Cheat-Sheet.md) - the one page to walk in with.
2. [Executive summary](00-Executive-Summary.md) - what the harness is and why it exists.
3. [Architecture and diagrams](01-Architecture-and-Diagrams.md) - six diagrams, ending with the 12-box whiteboard.
4. [End-to-end lifecycle](03-End-to-End-Lifecycle.md) - one task traced through the real files.
5. [Pitches](interview/Pitches.md) - rehearse the 30-second, 2-minute, and 10-minute versions aloud.
6. [JD mapping](interview/JD-Mapping.md) - each job requirement, the evidence, the gap, and the bridge.
7. [Safety and quality gates](06-Safety-and-Quality-Gates.md) - the release-compliance story.
8. [Question bank](interview/Question-Bank.md) and [STAR stories](interview/STAR-Stories.md).

## Cross-cutting notes

| Note | Covers |
| --- | --- |
| [00 Executive summary](00-Executive-Summary.md) | Layers, what I built versus adopted, measured headline numbers |
| [01 Architecture and diagrams](01-Architecture-and-Diagrams.md) | Context, container, sequence, config flow, routing flowchart, whiteboard |
| [02 Inventory](02-Inventory.md) | Every repo: upstream, fork delta, toolchain, binaries, skills, layer |
| [03 End-to-end lifecycle](03-End-to-End-Lifecycle.md) | One task from request to merged PR, plus the overnight and context-pressure paths |
| [04 Model routing and quota](04-Model-Routing-and-Quota.md) | Tiers, fallback chain, spendPriority, quota windows |
| [05 Configuration topology](05-Configuration-Topology.md) | Sources of truth, generated files, symlinks, change propagation |
| [06 Safety and quality gates](06-Safety-and-Quality-Gates.md) | Guard hooks, reviewer subagents, the no-mistakes gate, audit trail |
| [07 Fleet operations](07-Fleet-Operations.md) | Bootstrap, doctor, daily fork sync, adding a repo |
| [08 Design principles and trade-offs](08-Design-Principles-and-Tradeoffs.md) | Each principle, its rejected alternative, and its weakness |
| [09 Glossary](09-Glossary.md) | Every term, defined and linked |

## Diagram atlas

Fifty-two more diagrams, each with a "How to explain it" script to say aloud while drawing:

- [A - System and deployment](diagrams/A-System-and-Deployment.md): layers, deployment on one Mac, trust boundaries, authorship map, scaling-out proposal.
- [B - Orchestration](diagrams/B-Orchestration.md): who writes where, dispatch, supervision, teardown, treehouse leases, gnhf, wheelhouse.
- [C - Gate, safety, and compliance](diagrams/C-Gate-Safety-and-Compliance.md): no-mistakes state machine, guard.py decision tree, bank release controls, threat model.
- [D - Tools, configuration, and fleet](diagrams/D-Tools-Config-and-Fleet.md): AXI calls, quota pipeline, backlog model, manual generation, fork sync.

All notes and all 98 diagrams are compiled into `Agentic-Harness-Interview-Guide.pdf` in this folder (local only; `*.pdf` is gitignored).

## Component notes

| Layer | Components |
| --- | --- |
| Control plane | [firstmate](components/firstmate.md), [gnhf](components/gnhf.md), [wheelhouse](components/wheelhouse.md) |
| Execution isolation | [treehouse](components/treehouse.md) |
| Ship gate | [no-mistakes](components/no-mistakes.md) |
| AXI tooling | [axi](components/axi.md), [gh-axi](components/gh-axi.md), [chrome-devtools-axi](components/chrome-devtools-axi.md), [lavish-axi](components/lavish-axi.md), [tasks-axi](components/tasks-axi.md), [quota-axi](components/quota-axi.md), [compact-adviser](components/compact-adviser.md) |
| Machine config and manuals | [agents](components/agents.md), [Claude Code config](components/claude-code-config.md), [dotfiles-nix](components/dotfiles-nix.md), [fleet-ops](components/fleet-ops.md), [skills catalog](components/skills-catalog.md) |
| Apps and benchmarks | [overview](components/apps-and-benchmarks-overview.md), [baby-menu](components/baby-menu.md), [short-pipe](components/short-pipe.md), [justroll](components/justroll.md), [autopreso](components/autopreso.md), [presize](components/presize.md), [trial-by-combat](components/trial-by-combat.md), [org-bench](components/org-bench.md), [superpowers-bench](components/superpowers-bench.md), [programbench-bench](components/programbench-bench.md) |

## Interview pack

See the [interview pack index](interview/README.md).

<!-- moc:start (generated by tools/build_mocs.py; edits inside are overwritten) -->
## Also in this folder

**Sections**

- [components](components/README.md)
- [diagrams](diagrams/README.md)

<!-- moc:end -->
