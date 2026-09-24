---
type: concept
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://github.com/kunchenguid/axi, https://github.com/kunchenguid/treehouse, https://github.com/kunchenguid/no-mistakes, https://github.com/kunchenguid/firstmate, https://github.com/kunchenguid/quota-axi, https://github.com/kunchenguid/chrome-devtools-axi, https://github.com/kunchenguid/gh-axi, https://github.com/shreejitverma/agents, https://github.com/shreejitverma/fleet-ops, https://github.com/shreejitverma/dotfiles-nix]
---

# Design principles and trade-offs

Evidence was read from disk on 2026-09-23; citations use `repo/path:line` relative to `~/github`.
Authorship matters for this note: the tools (axi, treehouse, no-mistakes, firstmate, quota-axi, and the other CLIs) and most of their design principles are upstream work, largely by kunchenguid.
The user's decisions are which tools to adopt, how they are wired and configured, the routing policy, the manual generator, and the fleet operations; each section says which is which.

## 1. TL;DR

The harness makes eight bets: agent-shaped CLIs, one isolated worktree per task, a single gated path to the remote, generated per-tool manuals, quota-aware routing, several model vendors, local CLIs instead of MCP servers where possible, and deterministic scripts for mechanics with agents only for judgment.
Each bet buys correctness or throughput and pays for it somewhere: token-format lock-in, disk and bookkeeping, latency, config sprawl, routing that can stall on unknown data, quality variance, a larger shell blast radius, and a very large bash surface.
The honest summary is that the local controls are strong and well tested, while the remote, the identity model, and several config files are still conventions rather than enforced controls.

## 2. Summary table

| # | Principle | Whose decision | Trade-off | Rejected alternative | Known weakness (measured) |
|---|---|---|---|---|---|
| 1 | Agent-first CLIs (AXI) | upstream principles; user compiles them into every manual | tokens and turns saved vs a niche output format and one SDK dependency | raw human CLIs and MCP tool schemas | `gh-axi` home view swallows `gh` failures and prints `0 open` |
| 2 | One worktree per task | upstream treehouse; firstmate mandates it | isolation vs disk, pool state, and lease bookkeeping | branches in one checkout; containers | not a sandbox; 11 leased slots recovered from a corrupt state file |
| 3 | Gated shipping | upstream no-mistakes; user makes it the only ship path | quality and evidence vs minutes of latency per PR | CI-only checks; pre-commit hooks | gate not required on the remote; attestation is unsigned |
| 4 | Generated manuals | user's `agents` repo | one rule source vs a build step and symlink fragility | four hand-kept manuals | paraphrased duplicates pass the check; live settings drifted |
| 5 | Quota-aware routing | user's policy; upstream quota-axi supplies data | uses expiring allowance vs routing that can stall | fixed vendor; habit | Gemini has unknown pace and can never rank; model pins live in four files |
| 6 | Multi-vendor models | user | resilience and fit vs uneven quality and three manuals | single vendor | only Claude can run the gate today |
| 7 | Local CLIs over MCP servers | upstream tools; user runs zero user MCP servers | lean context vs full shell access | MCP servers in context | `chrome-devtools-axi` itself spawns `chrome-devtools-mcp@latest` unpinned |
| 8 | Scripts for mechanics, agents for judgment | upstream firstmate | exact, zero-token control vs a huge shell surface | LLM-driven orchestration; a daemon with a database | contract is 11,589 words against a stated 9,000-word ceiling |

## 3. The principles in detail

### 3.1 Agent-first CLIs (AXI)

What: ten principles for CLIs whose user is an LLM, from token-efficient TOON output to definitive empty states and structured exit codes (`axi/principles.yaml:13-40`), implemented once in `axi-sdk-js` and used by gh-axi, chrome-devtools-axi, lavish-axi, tasks-axi, and quota-axi ([axi](components/axi.md)).
The user's part: the principles are compiled into every generated manual and the `axi` skill is linked into every agent's skill directory ([05 Configuration topology](05-Configuration-Topology.md)).
Why: every byte of stdout is billed input, and every ambiguous answer costs another turn; upstream's published GitHub benchmark reports gh-axi at 100% success and $0.050 per task versus 86% and $0.054 for plain `gh` and 87% and $0.148 for the GitHub MCP server (`axi/README.md:37-45`, Claude Sonnet 4.6; upstream numbers, not re-run).
Trade-off: TOON is a young format that humans and generic tools read less easily than JSON, and every CLI depends on one SDK.
Rejected: wrapping human CLIs directly, or loading MCP tool schemas into context.
Known weaknesses:
- A real upstream defect: `gh-axi` home view catches `gh` failures with `.catch(() => [])` (`gh-axi/src/commands/home.ts:50`, `:61`), so a broken `gh` prints `issues: 0 open` and exits 0 - a violation of principle 5 ([gh-axi](components/gh-axi.md)).
- Manual drift: the user's manual summarizes principle 6 without upstream's newer "fail loud on unknown flags" (`agents/CORE.md:153` vs `axi/principles.yaml:29`).
- Principle 7 (ambient context) is not adopted: the optional SessionStart hooks are "Not currently enabled here" (`dotfiles-nix/README.md:524-525`).

### 3.2 One isolated worktree per task

What: treehouse hands out a pooled git worktree at detached HEAD, never to two holders, and never recycles one it cannot prove is clean (`treehouse/VISION.md:3-18`); firstmate types `treehouse get` into every crew window and verifies isolation before launch (`firstmate/bin/fm-spawn.sh:3843-3902`).
Why: parallel agents in one checkout overwrite each other, and hand-managed worktrees leak.
Trade-off: disk per slot, a pool state file with leases, and one more tool that can fail.
Rejected: branch switching in one checkout (serializes everything), and containers or VMs (stronger isolation, heavier setup, not needed for trusted local code).
Known weaknesses:
- "Treehouse isolates working directories and lifecycle ownership; it is not a security sandbox" (`treehouse/VISION.md:13`).
- On 2026-09-23 the machine had 14 pools and 23 leased slots, 11 marked "recovered: state file was corrupt or truncated"; whether they hold unlanded work was not inspected ([treehouse](components/treehouse.md)).
- The local fork is 1 commit behind `1185dc6` "reuse a pooled worktree only for the clone that owns it", which matters when firstmate primary and secondmate clones share a pool (`git -C treehouse log HEAD..upstream/main`).
- 4 treehouse Go tests fail locally because the user's global `tag.gpgsign = true` leaks into tests that call plain `git tag` ([treehouse](components/treehouse.md)).

### 3.3 Gated shipping

What: no-mistakes is a local git proxy; pushing to the `no-mistakes` remote runs a fixed nine-step pipeline in a disposable worktree and forwards only a green branch (`no-mistakes/internal/types/types.go:131-133`).
The user's part: `agents/CORE.md:59` makes it the only ship path for every tool, the gate agent is configured in `~/.no-mistakes/config.yaml`, and firstmate briefs require it ([06 Safety and quality gates](06-Safety-and-Quality-Gates.md)).
Why: CI runs after a branch is already public and costs a round trip per fix; the gate fixes mechanical problems before publication and attaches evidence to the PR.
Trade-off: latency; on `agents` PR #9 review took 226.7 s, test 128.3 s, and CI 166.5 s ([no-mistakes](components/no-mistakes.md)), and `auto_fix.review: 0` makes every review finding wait for a decision.
Rejected: CI-only checks (late feedback), pre-commit hooks (bypassable with `--no-verify`, which the guard blocks, and too slow for agent review).
Known weaknesses: none of the user's own repos has branch protection, so the gate is not required on the remote; the PR attestation is "not a cryptographic signature" (`no-mistakes/.github/actions/require-no-mistakes/README.md:121-142`); AI review is "probabilistic evidence" (`no-mistakes/docs/src/content/docs/reference/pipeline-steps.md:92`).

### 3.4 Generated per-tool manuals

What: `agents/bin/build-manuals` renders `CORE.md` + `ROUTING.md` + a tuning file into one manual per tool and fails CI if a committed manual is stale or a tuning file restates a shared rule (`agents/bin/build-manuals:48-62`, `:79-121`, `:135-140`).
This is entirely the user's work.
Why: Claude, Grok, Gemini, and Codex each read a different file; four hand-kept copies drift and contradict each other.
Trade-off: a build step, symlinks that depend on checkout paths, and generated files committed at the repo root, which a session inside the repo loads twice (`agents/README.md:43-47`).
Rejected: hand-maintained manuals; a single shared file (tools need different tuning, and one tool loading another's manual is the exact fault `ic-doctor` fails on).
Known weaknesses: the duplicate check catches verbatim lines only (`agents/bin/build-manuals:73-78`); Claude Code rewrote the linked `settings.json` and dropped the model pin (uncommitted diff on 2026-09-23); a stale comment still says no symlink points at `AGENTS.md` (`agents/bin/build-manuals:28-30`).

### 3.5 Quota-aware routing

What: route by fit first, then by quota: classify the task into a tier, drop candidates whose runway is `exhausted_now` or too short, then take the highest `spendPriority`; Tier 1 never trades down by quota (`agents/ROUTING.md:9-22`).
The user wrote the policy and encoded it in the local `firstmate/config/crew-dispatch.json` (6 rules plus a default); quota-axi (upstream) supplies the data and deliberately never routes (`~/.agents/skills/quota-axi/SKILL.md:37`).
`spendPriority` is the cycle-weighted mean over a scope's windows of `percentRemaining / timeRemainingPercent - burnMultiple`, positive when allowance is on track to expire unused (`quota-axi/src/pace.ts:308-319`).
Why: subscriptions reset on fixed clocks, so unused allowance is lost, and a task dispatched onto a nearly exhausted window stalls mid-run.
Trade-off: more moving parts at every intake, and routing that sometimes cannot decide.
Rejected: one vendor for everything; picking by habit; round-robin.
Known weaknesses:
- Gemini through agy reports `unknown` pace, and the policy says unknown "never ranks above a peer with known viable evidence" (`agents/ROUTING.md:18`), so Gemini can never win a Tier 2 balance ([quota-axi](components/quota-axi.md)).
- The 2026-09-23 snapshot had Claude projected to exhaust its five-hour window within hours, Grok `exhausted_now` on credits, and Gemini unknown, so a long Tier 2 task had no candidate both ranked and provably feasible; the skill escalates rather than guesses ([firstmate](components/firstmate.md), section 7).
- Unresolved rule conflict: quota-axi reports Grok `exhausted_now` limited by credits, while `firstmate/.agents/skills/quota-array-dispatch/SKILL.md:93` says Grok prepaid credits "are unrelated to paid-window headroom; never read them as exhaustion".
- The Opus pin appears in `agents/claude/settings.json` and four times in `crew-dispatch.json`, and the Grok pin in three files; nothing checks they agree.

### 3.6 Multiple model vendors

What: Claude Code Max, Grok Build, and Gemini through Antigravity, each with its own manual (`agents/ROUTING.md:9`).
Why: vendor outages and quota windows are independent, and some tasks fit one vendor better (live information to Grok, multimodal to Gemini).
Trade-off: the manual itself admits the Tier 2 peers are not equal: "Opus is the strongest of the three rather than a true peer, so a quota win for Grok or Gemini here trades some quality for subscription utilization" (`agents/ROUTING.md:13`).
Rejected: a single vendor (simpler, one failure domain).
Known weakness: the gate itself is single-vendor in practice; `agent: auto` resolves to Claude, the Grok fallback was credit-exhausted, and the Gemini path needs `acpx`, which `no-mistakes doctor` did not find ([no-mistakes](components/no-mistakes.md), section 6).

### 3.7 Local CLIs over MCP servers where applicable

What: `~/.claude.json` defines zero MCP servers ([claude-code-config](components/claude-code-config.md)); GitHub, browser, backlog, and quota work goes through local CLIs, and the tool skills are thin pointers that tell the agent to read the live `--help` (`~/.agents/skills/gh-axi/SKILL.md:18-24`).
Why: MCP tool schemas occupy context on every turn, while a CLI costs tokens only when called, and a skill that defers to `--help` cannot drift from the binary.
Upstream's browser benchmark reports chrome-devtools-axi at $0.074 and 4.5 turns per task versus $0.101 and 6.2 turns for raw `chrome-devtools-mcp` (`axi/README.md:23-33`; upstream numbers, not re-run).
Trade-off: a CLI needs shell access, which is a far larger authority than a narrowly scoped MCP tool; the guard (L1 in note 06) is the only brake.
Known weaknesses: this is "over MCP servers in context", not "without MCP": `chrome-devtools-axi` runs a local bridge that spawns `npx -y chrome-devtools-mcp@latest` (`chrome-devtools-axi/src/bridge.ts:657`), an unpinned download at run time; session MCP tools still arrive through claude.ai connectors and plugins.

### 3.8 Scripts own mechanics, agents own judgment

What: "Logic that can be exact lives in deterministic scripts; work that requires understanding lives in an agent; the two never mix" (`firstmate/VISION.md:34`); firstmate has about 190 helper scripts, a zero-token bash watcher, and on-disk state so a killed session loses nothing ([firstmate](components/firstmate.md)).
This is upstream's design; the user adopted it and contributed fork fixes to that shell surface (a SIGPIPE race and Bash 3.2 parsing).
Why: locks, isolation checks, merge guards, and backlog transitions must behave the same every time and cost no tokens.
Trade-off: a very large bash surface with edge cases, paid for with 220 test files and sharded CI.
Rejected: letting the LLM orchestrate directly (non-deterministic, expensive), or a daemon with a database (less introspectable, harder for the agent to repair).
Known weakness: the always-loaded contract is 11,589 words by `wc -w` against a stated 9,000-word ceiling (`firstmate/VISION.md:39`); the project's counting method was not found, so whether it is over budget is (unverified).

### 3.9 Supporting decisions

- **Fast-forward-only fork fleet** (user): forks stay pristine mirrors and divergence is reported, never auto-merged ([07 Fleet operations](07-Fleet-Operations.md)); the cost is that fork-carrying repos lag upstream (firstmate is 31 behind).
- **Unattended by default, human at the merge** (user configuration of upstream tools): crewmates and gate agents run with permission prompts bypassed so nothing deadlocks, and the merge stays human; the cost is weak least privilege during the run ([06 Safety and quality gates](06-Safety-and-Quality-Gates.md)).
- **Symlinks into working trees instead of copies** (user): live edits and one reviewed copy, at the price of tools writing into tracked files ([05 Configuration topology](05-Configuration-Topology.md)).

## 4. Limitations and what I would build next

Ordered by risk reduced per unit of effort.

1. **Make the gate mandatory on the remote.**
   Add branch protection or a ruleset on `main` for `agents`, `fleet-ops`, `dotfiles-nix`, and the forks, requiring the `PR must be raised via no-mistakes` check and CI; today the GitHub API reports "Branch not protected" for all four.
2. **Sign the attestation and separate identities.**
   Pipeline fix commits carry the user's identity and signing key; a bot identity for gate commits plus upstream's planned signed attestations (`nm-signed-attestations-r1`, `no-mistakes/.github/actions/require-no-mistakes/README.md:140-142`) would make "who wrote this" auditable.
3. **Least privilege for unattended agents.**
   Switch firstmate crews from `bypass` to `auto` permission mode (`firstmate/docs/configuration.md:375`), add guard tests for `--dangerously-skip-permissions`, and drop `skipDangerousModePermissionPrompt` from the shared settings.
4. **Version and de-duplicate configuration.**
   Put `~/.no-mistakes/config.yaml` and a redacted `crew-dispatch.json` in the `agents` repo, generate every model pin from one file, and point Claude Code's writable settings at an untracked layer.
5. **Fix the fleet's silent failures.**
   Make `fleet-sync.yml` survive a failing repo under `bash -e`, make `sync-forks` exit non-zero on failure, and have `fleet-doctor` read the last server-side run.
6. **Stage upstream updates.**
   Today new upstream code is installed automatically every morning; pin CLIs to reviewed release tags or run upstream deltas through a scout review before reinstalling, and pin `chrome-devtools-mcp` to a version.
7. **Resolve routing blind spots.**
   Settle the Grok credits rule, measure Gemini runway (or give it an explicit prior), and add a check that fails when the dispatch file names a model the live catalogs do not list.
8. **Exercise the supervisor for real.**
   The primary firstmate home has no registered projects or backlog yet; register the day-to-day repos so the dispatch, merge, and teardown paths run on real work, not only in tests.
9. **Close test hygiene gaps.**
   Isolate global git config in treehouse's tests (`GIT_CONFIG_GLOBAL=/dev/null` in helpers, untested), and fix the environment-dependent gnhf cursor test.
10. **Measure the harness itself.**
    Track cost, turns, and gate outcomes per task in the decision ledger; the programbench-bench data in this fleet shows a process rule (a TDD arm) can cost more and score lower (mean 48.8 vs 52.4, p = 3.1e-08 on 192 paired tasks), so harness rules deserve the same evidence ([programbench-bench](components/programbench-bench.md)).

Mapping to the target role: the same shape carries to an enterprise release pipeline, for example the gate stages as CodeBuild steps in a CDK-defined pipeline with required approvals, but this harness has no AWS CDK, CloudFormation, CodeBuild, Java, or OpenFin code, and that should be said plainly ([apps-and-benchmarks-overview](components/apps-and-benchmarks-overview.md)).

## 5. Interview angle

**Q: What is the single most important design decision?**
One gated publication path with evidence bound to the commit, because it turns "the agent says it tested" into a checkable record; everything else (routing, isolation, manuals) makes agents faster, while the gate makes them safe to trust.

**Q: What would you do differently?**
Start from the remote: branch protection and required checks first, then local convenience, because a local control that can be skipped is a convention.

**Q: Where did measurement change a decision?**
The launchd sync was moved to Standard QoS after the first run took 3.5 hours (`dotfiles-nix/nix/home/darwin.nix:89-94`), and the Grok manual ceiling was set from a measured load rather than the documented cap (`agents/bin/build-manuals:33-44`).

## Related

- [Agentic Harness index](README.md)
- [00 Executive summary](00-Executive-Summary.md)
- [01 Architecture and diagrams](01-Architecture-and-Diagrams.md)
- [02 Inventory](02-Inventory.md)
- [03 End-to-end lifecycle](03-End-to-End-Lifecycle.md)
- [04 Model routing and quota](04-Model-Routing-and-Quota.md)
- [05 Configuration topology](05-Configuration-Topology.md)
- [06 Safety and quality gates](06-Safety-and-Quality-Gates.md)
- [07 Fleet operations](07-Fleet-Operations.md)
- [09 Glossary](09-Glossary.md)
- Components: [axi](components/axi.md), [treehouse](components/treehouse.md), [no-mistakes](components/no-mistakes.md), [agents](components/agents.md), [quota-axi](components/quota-axi.md), [firstmate](components/firstmate.md), [chrome-devtools-axi](components/chrome-devtools-axi.md), [gh-axi](components/gh-axi.md)
