---
type: playbook
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://github.com/shreejitverma/agents, https://github.com/shreejitverma/fleet-ops, https://github.com/shreejitverma/dotfiles-nix, https://github.com/shreejitverma/firstmate, https://github.com/kunchenguid/no-mistakes, https://github.com/kunchenguid/quota-axi]
---

# Cheat sheet - one page before walking in

## Opening line

"I run a personal agentic harness: a supervisor agent routes work by task class and live quota to workers in isolated worktrees, and nothing reaches GitHub except through a local nine-step gate; the tools are mostly open-source forks, and the policy and operations layer is mine."

## The whiteboard

```text
 [1 Captain] --chat--> [2 firstmate] <-- [3 Policy: manuals, routing, guard]
                          ^    |    \
       [4 quota-axi] -----+    |     +--> [5 tasks-axi backlog]
                               v
                  [6 treehouse worktree]
                               v
                  [7 Workers: Claude, Grok, Gemini] --> [8 AXI tools]
                               | git push no-mistakes
                               v
  [9 no-mistakes: intent rebase review test document lint push pr ci]
                               v
  [10 GitHub: attested PR, CI, required check] --> merge only on my word
  [11 dotfiles-nix + fleet-ops underneath]   [12 gnhf, wheelhouse on the side]
```

Full Mermaid version and narration: [Pitches](Pitches.md).

## The 12 components in one line each

| # | Component | One line | Mine? |
| --- | --- | --- | --- |
| 1 | [firstmate](../components/firstmate.md) | Supervisor agent distro: spawns, steers, and tears down workers; never edits code | upstream; 6 fork commits and routing config mine |
| 2 | [no-mistakes](../components/no-mistakes.md) | Local Git proxy gate: fixed 9 steps, verified-SHA push, attested PR | upstream; config and mandate mine |
| 3 | [treehouse](../components/treehouse.md) | Pooled worktrees at detached HEAD with leases and landed-work proofs | upstream |
| 4 | [tasks-axi](../components/tasks-axi.md) | Structured, locked edits to a human-readable `backlog.md` | upstream |
| 5 | [quota-axi](../components/quota-axi.md) | Per-subscription remaining, runway, and spendPriority; data only | upstream; policy that consumes it mine |
| 6 | [agents](../components/agents.md) | One rule source compiled into every tool's manual, plus guard and lint hooks | mine (hooks adapted from MIT ECC) |
| 7 | AXI tools: [axi](../components/axi.md), [gh-axi](../components/gh-axi.md), [chrome-devtools-axi](../components/chrome-devtools-axi.md), [lavish-axi](../components/lavish-axi.md) | Token-efficient agent CLIs for GitHub, real-browser checks, and visual review | upstream |
| 8 | [gnhf](../components/gnhf.md) | Overnight loop: fresh agent per iteration, commit on success, reset on failure | upstream |
| 9 | [compact-adviser](../components/compact-adviser.md) | Suggests `/compact` at task boundaries; disabled for unattended workers | upstream; key scoping mine |
| 10 | [wheelhouse](../components/wheelhouse.md) | Owner decision queue on GitHub Issues and Actions | upstream; fleet list mine |
| 11 | [dotfiles-nix](../components/dotfiles-nix.md) | Nix flake for the machine plus `ic-link`, `ic-doctor`, `sync-forks` | fork; 45 commits mine |
| 12 | [fleet-ops](../components/fleet-ops.md) | 22-fork manifest, bootstrap, doctor, server-side sync | mine |

## 10 numbers worth remembering (all verified 2026-09-23)

| Number | What | Source |
| --- | --- | --- |
| 9 | Fixed gate steps: intent, rebase, review, test, document, lint, push, pr, ci | `no-mistakes/internal/types/types.go:52-60` |
| 228 / 17 | Changes through the gate, across 17 repos | `no-mistakes stats` |
| 58% | Changes where a mistake was caught and fixed (133 of 228) | `no-mistakes stats` |
| 1,385 / 69% | Mistakes reported by the gate / share fixed; 879 fixes came from review | `no-mistakes stats` |
| 22 / 20 | Forks declared / fast-forwarded daily at 10:00 | `.fleet/manifest.yaml`, `dotfiles-nix/nix/home/darwin.nix:79-96` |
| 3 x 3 | Routing tiers by task class, three providers per peer tier | `agents/ROUTING.md:9-22` |
| 54% vs 2.7 h | Claude weekly headroom vs actual runway, bound by the five-hour window | quota-axi snapshot, [quota-axi](../components/quota-axi.md) section 3 |
| 51 | Guard and post-edit hook tests (31 + 20), re-run by me: OK | `agents/claude/hooks/test_*.py` |
| 3.5 h | First scheduled sync under Background QoS, fixed with Standard QoS | `dotfiles-nix/nix/home/darwin.nix:89-94`, `b790d5f` |
| 6 / +73 -19 | My firstmate fork commits and net lines; upstream is 31 commits ahead | `git diff --shortstat` vs merge base |

Bonus if asked about tests: dotfiles-nix sandboxed suites 380 checks; firstmate suites 55 + 21 + 36 checks; all passing when run on 2026-09-23.

## 5 trade-offs to defend

1. **Scripts and files over a daemon and database (firstmate).**
   Upside: restart-proof, introspectable, zero-token supervision.
   Downside: shell edge cases (my SIGPIPE and Bash 3.2 fixes), paid for with about 220 test files.
2. **Review findings never auto-fix (`auto_fix.review: 0`).**
   Upside: intent-changing edits are never made silently.
   Downside: latency; review was the longest step at 226.7 s in agents PR #9.
3. **Guard fails open, and allows ask-level commands when no human is present.**
   Upside: pipelines never wedge and a hook bug never blocks all work.
   Downside: some unattended risk; mitigated by keeping always-deny rules fail-closed even on parse failure.
4. **Unknown quota evidence poisons a scope.**
   Upside: never route into a window that might be empty.
   Downside: Gemini via `agy` is permanently unranked because its windows lack cycle data.
5. **Forks with fast-forward-only sync.**
   Upside: control, patchability, reviewed installs, no clobbering.
   Downside: divergence work (firstmate is manual-sync and 31 behind) and dependence on one upstream author.

## 5 gaps and bridges

| Gap | Bridge (one sentence) |
| --- | --- |
| No AWS CDK, CloudFormation, or CodeBuild | Same gate as a CDK Pipelines `CodePipeline`: CodeBuild steps running the repo's lint and test commands, `cdk diff` and change sets as evidence, a `ManualApprovalStep` before prod. |
| No OpenFin | OpenFin is a Chromium desktop runtime like Electron with managed windows, a message bus, and FDC3 interop; the Electron discipline I can show (context isolation, narrow preload bridge) carries over. |
| No Java | My Python is a functional core with immutable dataclasses and an I/O shell, tested with unittest; cover Java from prior experience. |
| Frontend code is upstream, not mine | I read, run, and test Electron 42, React 19, Tailwind v4, and `ws` servers, and can design a blotter feed: sequence-numbered deltas, snapshots, per-instrument conflation, reconnect with resubscribe. |
| Single user, single machine, no markets domain | The control design (limits before dispatch, one audited release path, golden-source sync) is the same one markets tech uses; team scale is untested and I would pilot before widening. |

## Three stories, one line each

1. The gate caught my guard hook failing open on Claude's own commit format, then caught a regression in the fix (agents PR #8).
2. My firstmate patch ended fast-forward sync, so I made it a manual-sync fork with the reason in the manifest, and upstream now arrives through gated merge PRs (fleet-ops PR #4, firstmate PRs #1 to #3).
3. Claude hit its five-hour limit mid-delivery, Grok covered two PRs then ran out of credits, I waited for the reset rather than skip the gate, and wrote runway gating into policy that evening (2026-09-18).

Details: [STAR-Stories](STAR-Stories.md).

## Never say

- "I built firstmate" or "I wrote no-mistakes".
- Any benchmark number as mine; the AXI and programbench numbers are upstream's, and I say so.
- "It is secure"; say "it limits blast radius; worktrees and the guard are not a sandbox".
