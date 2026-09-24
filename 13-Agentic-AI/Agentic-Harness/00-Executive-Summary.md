---
type: concept
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: ["https://github.com/shreejitverma/agents", "https://github.com/shreejitverma/fleet-ops", "https://github.com/shreejitverma/dotfiles-nix", "https://github.com/kunchenguid/dotfiles-mac-nix", "https://github.com/kunchenguid/firstmate", "https://github.com/shreejitverma/firstmate", "https://github.com/kunchenguid/no-mistakes", "https://github.com/kunchenguid/treehouse", "https://github.com/kunchenguid/gnhf", "https://github.com/kunchenguid/axi", "https://github.com/kunchenguid/gh-axi", "https://github.com/kunchenguid/chrome-devtools-axi", "https://github.com/kunchenguid/lavish-axi", "https://github.com/kunchenguid/tasks-axi", "https://github.com/kunchenguid/quota-axi", "https://github.com/kunchenguid/compact-adviser", "https://github.com/ImZoomBoy/wheelhouse", "https://github.com/affaan-m/ecc"]
---

# Agentic Harness - executive summary

Evidence was measured read-only on 2026-09-23 from the local clones under `~/github`, the live symlinks under `~`, and the component notes in [components/](components/).
Citations use `repo/path:line` with the repo name relative to `~/github`; the fleet control repo is checked out at `~/github/.fleet` and is cited as `.fleet/...`.
Anything not checked against a file or a command is marked (unverified).

## 1. TL;DR

The Agentic Harness is a personal, local software-delivery system in which AI coding agents do most of the implementation and deterministic tooling does the control.
One supervisor session (the "first mate", Claude Code running inside `~/github/firstmate`) takes requests from the human, files them in a backlog, picks a model per task from live subscription quota, and launches worker agents ("crewmates") in isolated git worktrees.
Every worker ships only through a local pre-publication gate (`no-mistakes`) that runs review, tests, docs, lint, a verified push, the PR, and CI babysitting, and every agent runs under version-controlled safety hooks and one generated operating manual per tool.
Most tool code is upstream open source by Kun Chen (`kunchenguid/*`); what I built is the integration layer: routing policy, the manual generator, the machine and fleet configuration, fork patches, and the operations that keep 22 forks current.

## 2. Why it exists

| Problem without the harness | What the harness does | Evidence |
| --- | --- | --- |
| Five parallel agents turn the human into a tab-juggler who loses track of state | One conversational supervisor; workers never address the human; all state on disk | `firstmate/README.md:29-30`, `firstmate/AGENTS.md:38-40` |
| Agents collide in one checkout | Each task gets a pooled, detached-HEAD worktree; spawn refuses the primary checkout | `firstmate/bin/fm-spawn.sh:3843`, `firstmate/AGENTS.md:330` |
| Three paid subscriptions, each with overlapping windows, are spent by habit | Tiered routing by task class, then by quota-axi runway and `spendPriority` | `agents/ROUTING.md:9-22` |
| Agent diffs reach `origin` unreviewed | `no-mistakes` is the only ship path; bare `git push` is forbidden by rule | `agents/ROUTING.md:29` |
| Agents run destructive commands or weaken lint configs | `guard.py` PreToolUse hook: always-deny, ask-if-attended, deny config edits when unattended | `agents/claude/hooks/guard.py:59-65`, [agents](components/agents.md) |
| Four tools read four instruction files that drift | One source (`CORE.md` + `ROUTING.md`) generates every manual; CI fails on drift | `agents/bin/build-manuals:27-62` |
| About twenty forks go stale or get clobbered | Manifest-driven, fast-forward-only daily sync with rebuilds | `dotfiles-nix/files/bin/sync-forks:195-249`, `.fleet/manifest.yaml:11-341` |

## 3. The five layers

| Layer | Components | One-line role |
| --- | --- | --- |
| Control plane | [firstmate](components/firstmate.md), [tasks-axi](components/tasks-axi.md), [quota-axi](components/quota-axi.md), `firstmate/config/crew-dispatch.json` | Decide what to do, with which model, and track it durably |
| Execution | Claude Code, Grok Build, Gemini via `agy` workers; [treehouse](components/treehouse.md); [gnhf](components/gnhf.md); [compact-adviser](components/compact-adviser.md) | Do the work in isolation; overnight loops; context-pressure advice |
| Agent tooling | [axi](components/axi.md) SDK, [gh-axi](components/gh-axi.md), [chrome-devtools-axi](components/chrome-devtools-axi.md), [lavish-axi](components/lavish-axi.md), [skills](components/skills-catalog.md), six Claude subagents | Token-efficient CLIs and on-demand know-how for agents |
| Gate | [no-mistakes](components/no-mistakes.md), `guard.py`, `post_edit.py`, upstream `require-no-mistakes` CI check, [wheelhouse](components/wheelhouse.md) downstream | Nothing reaches GitHub without review, tests, and attestation |
| Machine config | [agents](components/agents.md), [claude-code-config](components/claude-code-config.md), [dotfiles-nix](components/dotfiles-nix.md), [fleet-ops](components/fleet-ops.md) | Reproducible machine, one manual per tool, fleet inventory and sync |

The diagrams are in [01-Architecture-and-Diagrams](01-Architecture-and-Diagrams.md).

## 4. Built versus adopted (honest authorship)

Authorship was checked with `git log --format=%an upstream/<branch>..HEAD` in each fork and `git log` in the two repos without an upstream.

| What | Who built it | My contribution | Evidence |
| --- | --- | --- | --- |
| firstmate (supervisor distro, about 190 bash scripts) | Upstream (Kun Chen) | 6 non-merge fork commits (2 hand-written, 4 created by the no-mistakes pipeline's fix steps), 8 files, +73/-19; the private `config/crew-dispatch.json` | `git log --no-merges upstream/main..HEAD` in `~/github/firstmate`; `firstmate/.gitignore:13` |
| no-mistakes, treehouse, gnhf, axi, gh-axi, chrome-devtools-axi, lavish-axi, tasks-axi, quota-axi, compact-adviser | Upstream (Kun Chen and contributors) | 0 fork commits; install recipes, skill links, aliases, config, and policy around them | ahead count 0 for all ten (section 5) |
| wheelhouse (GitHub Actions decision queue) | Upstream history (103 of 117 ahead commits by Kun Chen, carried from the original repo) | 14 commits: fleet repo list and docs | `git log --format=%an upstream/main..HEAD` in `~/github/wheelhouse` |
| dotfiles-nix | Upstream base: 8 commits, small macOS-only Nix flake | All 45 fork commits: `ic-link`, `ic-doctor`, `sync-forks`, Linux and WSL support, zsh layer, tests, CI, 12 owned skills | `git rev-list --left-right --count upstream/main...HEAD` = `2 45` |
| agents (manual generator, hooks, subagents) | Me (no upstream); hooks and skills adapted from `affaan-m/ecc` (MIT), attributed in source | 44 commits | `agents/claude/hooks/guard.py:32-35` |
| fleet-ops (`~/github/.fleet`) | Me (no upstream) | 29 commits, 7 merged PRs | `git log` in `~/github/.fleet` |
| Nine apps and benchmarks (baby-menu, short-pipe, justroll, autopreso, presize, trial-by-combat, org-bench, superpowers-bench, programbench-bench) | Upstream | 0 fork commits; tracked and synced only | [apps-and-benchmarks-overview](components/apps-and-benchmarks-overview.md) |

The one-sentence version for an interviewer: "I did not write the agent tools; I designed and operate the system that composes them, and I own its routing policy, safety hooks, config generation, and fleet operations."

## 5. Headline numbers (measured)

| Metric | Value | How measured |
| --- | --- | --- |
| Repos in the harness | 24: 22 forks in the fleet manifest plus 2 of my own (`agents`, `fleet-ops`) | `grep -c '^- name:' .fleet/manifest.yaml` = 22 |
| Forks synced daily | 20 `sync: true`, 2 `sync: false` (firstmate, dotfiles-nix) | `.fleet/manifest.yaml:38`, `:293` |
| Forks with my commits | 3 of 22: firstmate (6 non-merge), dotfiles-nix (45), wheelhouse (14) | `git log upstream/<branch>..HEAD` per repo |
| Skills in `~/.agents/skills` | 31: 9 from forks, 12 owned by dotfiles-nix (`ship` plus 11 ECC-derived), 10 third-party (vercel-labs) | `ls ~/.agents/skills \| wc -l` = 31; [skills-catalog](components/skills-catalog.md) |
| Claude subagents and rules | 6 subagents, 2 path-scoped rules, 2 hooks | `ls ~/.claude/agents ~/.claude/rules` |
| Routing rules | 6 rules plus a default array | `firstmate/config/crew-dispatch.json:2-56` |
| Gate steps | 9, fixed order | `no-mistakes/internal/pipeline/steps/common.go:361-376` |
| Health checks | `ic-doctor` exit 0, 67 ok, 1 warn; `fleet doctor.sh` 130 ok, 1 warn | [dotfiles-nix](components/dotfiles-nix.md), [fleet-ops](components/fleet-ops.md) (run 2026-09-23) |
| Hook tests | 51 unittest cases pass (31 guard, 20 post-edit) | [agents](components/agents.md) |
| dotfiles-nix suites | 6 suites, 380 checks, all pass | [dotfiles-nix](components/dotfiles-nix.md) |

No latency, cost, or productivity numbers are claimed, because none were measured.

## 6. What I found wrong (good interview material)

Being able to name the system's defects is stronger evidence of ownership than a clean story.

| Defect | Impact | Evidence |
| --- | --- | --- |
| Server-side fleet sync uses `out=$(gh repo sync ...); status=$?` under GitHub's `bash -e` | The first failing repo silently aborts the loop; the `diverged` and `FAILED` branches are unreachable; failed on 2026-09-22 and 2026-09-23 | `.fleet/.github/workflows/fleet-sync.yml:29-30` |
| Local `sync-forks` exits 0 even when repos fail | Anything reading exit status alone misses failures; the signal is the `failed:[ ]` log line | `dotfiles-nix/files/bin/sync-forks:252-262` |
| Claude Code rewrote `~/.claude/settings.json` through its symlink | The committed `model` pin disappeared from the live file; visible only as a dirty tree | `git diff claude/settings.json` in `~/github/agents` |
| `tag.gpgsign true` in my git config | 4 treehouse upstream tests fail locally because they call plain `git tag` without isolating global config | [treehouse](components/treehouse.md) section 9 |
| Gemini via `agy` reports no cycle start | Its `spendPriority` and runway are always `unknown`, so Gemini can never win a quota ranking | [quota-axi](components/quota-axi.md) section 6 |
| Contradictory Grok reading | quota-axi reports Grok `exhausted_now` limited by `credits`, while the dispatch skill says prepaid credits must never be read as exhaustion; which rule wins is (unverified) | `firstmate/.agents/skills/quota-array-dispatch/SKILL.md:93` |
| A non-empty `TYPESAFE_API_KEY` line exists in the firstmate home `.env` | The zsh wrapper deliberately keeps the key out of `fm`, but this line would still opt firstmate into typed dispatch (value not read) | `dotfiles-nix/files/zsh/ic-workflow.zsh:528-536`, `firstmate/bin/fm-dispatch-resolve.sh:8-12` |
| `gh-axi` home view swallows `gh` failures | Prints `0 open` with exit 0 on failure (upstream bug) | `gh-axi/src/commands/home.ts:47-62` |

## 7. Mapping to a full-stack trading-technology role

| Role expectation | Where the harness shows it | Honest gap |
| --- | --- | --- |
| Own full SDLC and STLC | Intake, backlog, isolated build, gated test, PR, CI, merge, teardown, all scripted | Personal scale, one operator |
| Unit test coverage and code quality | Hook suites in CI, dotfiles-nix sandboxed suites, gate Test and Lint steps, `post_edit.py` lint feedback | Coverage percentages were not measured |
| Enterprise and regulatory release compliance | Every PR carries a machine-readable no-mistakes attestation bound to the head SHA, and a required GitHub check verifies it | Not a certified control; review is "probabilistic evidence" (`no-mistakes/docs/src/content/docs/reference/pipeline-steps.md:92`) |
| CI/CD and infrastructure as code | Nix flake declares the machine; generated manuals with drift checks; GitHub Actions for sync and CI | No AWS CDK, CloudFormation, or CodeBuild anywhere in these repos |
| Distributed, reliable pipelines | Idempotent sync with retries, crash-replayable backlog transitions, fast-forward-only publication, lease-anchored pushes | Single machine, not a distributed system |
| React, websockets, Electron, Tailwind | Upstream apps I run and read: baby-menu (Electron, React 19, Tailwind v4), autopreso and lavish-axi (Express plus WebSocket servers) | No OpenFin, no Java; I did not write that app code |

## 8. The 60-second pitch

1. "I run AI coding agents like a small engineering team, and I built the control system around them rather than the agents themselves."
2. "A supervisor agent takes my request, files it in a backlog, and picks a model tier from live quota: frontier model for design and root-cause work, a balanced pool for well-specified work, cheap models for mechanical work."
3. "Each worker gets its own git worktree, runs under safety hooks that deny force-pushes and hook bypasses, and can ship only through a local gate that reviews, tests, documents, lints, pushes a verified SHA, opens the PR, and babysits CI."
4. "One source file generates the operating manual for Claude, Grok, Gemini, and Codex, with a CI check that fails on drift, and a Nix flake plus a fleet manifest rebuild the whole machine and keep 22 forks fast-forwarded daily."
5. "The design rule I care about is the one from firstmate: exact logic lives in deterministic scripts, judgment lives in the agent, and the two never mix."

## 9. Reading order

1. [01-Architecture-and-Diagrams](01-Architecture-and-Diagrams.md) for the pictures and the whiteboard script.
2. [02-Inventory](02-Inventory.md) for every repo, its upstream, and my delta.
3. [03-End-to-End-Lifecycle](03-End-to-End-Lifecycle.md) for one task traced through real files.
4. [04-Model-Routing-and-Quota](04-Model-Routing-and-Quota.md) for tiers, `spendPriority`, and where each rule lives.
5. The component notes under [components/](components/) for depth on any single tool.
