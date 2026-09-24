---
type: playbook
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://github.com/shreejitverma/agents, https://github.com/shreejitverma/fleet-ops, https://github.com/shreejitverma/dotfiles-nix, https://github.com/shreejitverma/firstmate, https://github.com/kunchenguid/no-mistakes, https://github.com/kunchenguid/baby-menu, https://github.com/kunchenguid/short-pipe, https://github.com/kunchenguid/autopreso, https://github.com/kunchenguid/trial-by-combat, https://github.com/kunchenguid/lavish-axi]
---

# JD mapping - the harness against a Global Markets full-stack SWE role

Each section takes one JD requirement and gives three things: evidence from the harness with cites, how to say it in one or two sentences, and an honest gap with a bridge.
"Mine" means code or configuration I authored; "upstream" means code in a fork that I run, configure, and test but did not write.
Frontend claims were re-verified with `grep` on 2026-09-23 against each repo's `package.json` and source (commands at the end).

## Summary table

| JD requirement | Strength | Mine or upstream | One-line proof |
| --- | --- | --- | --- |
| SDLC and STLC ownership | strong | mine (policy) plus upstream (gate) | 7-step ship loop and a 9-step gate on every change |
| Unit test coverage | strong for scripts, no coverage metric | mine | 51 hook tests, 380 sandboxed script checks |
| Enterprise and regulatory release compliance | strong analogue | upstream gate, my policy | SHA-bound attestation, human-decided review findings |
| CI/CD incl. AWS CDK, CloudFormation, CodeBuild | CI yes, AWS no | mine (CI) | GitHub Actions only; AWS is a gap with a concrete bridge |
| Distributed reliable pipelines | moderate (single machine) | both | idempotent ff-only sync, leases, locks, crash replay |
| Full stack UI: React, ES6+, websockets, OpenFin, Electron, Tailwind | strong for reading and testing upstream code, OpenFin absent | upstream | Electron 42, React 19, Tailwind v4, `ws` servers verified by grep |
| OO language (Python or Java) | Python yes, Java no | mine | 1,204-line Python guard with dataclasses and 51 unittest cases |
| Code quality and efficiency | strong | both | 1,385 findings reported by the gate, TOON token-efficient CLIs |
| Partnering with business | weak in code | mine (process) | intent-first briefs, PRs with Intent and Risk sections |
| Global Markets exposure | not in harness | n/a | bridge by analogy and prior experience |

## 1. Full SDLC and STLC ownership

Evidence:
- The `ship` skill (mine, in dotfiles-nix) defines one loop for every change: 0 check headroom, 1 track the task, 2 isolate the stream, 3 implement, 4 verify end to end, 5 gate the ship, 6 close out (`dotfiles-nix/files/skills/ship/SKILL.md:15-84`).
- The gate enforces a fixed test lifecycle in code: intent, rebase, review, test, document, lint, push, pr, ci (`no-mistakes/internal/types/types.go:52-60`); a repo can only add gates after rebase, review, test, document, or lint (`no-mistakes/internal/types/types.go:137`).
- Requirements traceability: the worker passes `--intent` taken only from my own words in the brief, so the gate reviews the diff against the stated requirement (`firstmate/bin/fm-dod-lib.sh:285-311`).
- Closure evidence: `tasks-axi done --pr` accepts only a canonical PR URL (`tasks-axi/src/pr-url.ts:16-31`).
- Real record: 228 changes across 17 repos went through the gate (`no-mistakes stats`, 2026-09-23); the `agents` repo alone has PRs #1 to #9, all gated.

Say it:
"Every change I make, human or agent, goes through the same lifecycle: tracked, isolated in a worktree, verified end to end, then a fixed gate of review, test, docs, lint, push, PR, and CI before anyone can merge it."

Gap and bridge:
- Gap: no formal STLC artifacts such as a test plan document, traceability matrix, or UAT sign-off.
- Bridge: "The brief's intent section is my requirement, the gate's test evidence is my execution record, and the PR body carries Intent, Testing, and Risk sections; in a bank I would link each to the Jira story and the release ticket."

## 2. Unit test coverage

Evidence (mine):
- `agents` hooks: 51 unittest cases (31 guard, 20 post-edit); I re-ran them on 2026-09-23: "Ran 51 tests ... OK".
  `SettingsHookTest` executes the exact hook command strings from `settings.json` against a temp `HOME`, so a broken settings line fails CI (`agents/claude/hooks/test_post_edit.py:204-257`).
- `dotfiles-nix`: six sandboxed bash suites that run the real script with fake `HOME`, stub executables, and local bare git remotes: 75 + 105 + 15 + 66 + 84 + 35 = 380 checks, all passing on 2026-09-23 (see [dotfiles-nix](../components/dotfiles-nix.md) section 9).
- `firstmate` fork: the SIGPIPE race fix shipped with a deterministic regression test, `test_harness_lookup_drains_its_producer`, which fails before the fix and passes after (commit `157d3d82`).
- Test-quality rule enforced by the gate skill: tests that only grep implementation source are banned (`~/.agents/skills/no-mistakes/SKILL.md:66-80`).

Evidence (upstream, run by me): gnhf 763 passed with 1 environment-dependent failure; tasks-axi 444 passed; baby-menu 554 passed; autopreso 237 passed (component notes, 2026-09-23).

Say it:
"I test scripts that touch real state by running the real script against a sandbox, fake home, stub binaries, and local bare remotes, and asserting resulting state, never grepping the source."

Gap and bridge:
- Gap: no coverage percentage is measured in any of my CI jobs.
- Bridge: "For Python I would add `pytest --cov` with a branch-coverage floor on changed lines, and for the React side Vitest's `--coverage`; I treat the floor as a ratchet, not a target, because mutation-free coverage is easy to game."

## 3. Enterprise and regulatory release compliance

Evidence:
- Single publication path: the global rule "Ship through `no-mistakes`, always" is compiled into every tool's manual (`agents/CORE.md:59-60`).
- Evidence bound to the artifact: the push step rewrites the PR attestation before pushing, pushes an exact verified SHA with a lease anchored to the verified remote SHA, then re-reads the remote with `ls-remote` (`no-mistakes/internal/pipeline/steps/push.go:170-205`).
- Required check: five upstream app repos call the shared `require-no-mistakes` action pinned to a SHA (`baby-menu/.github/workflows/no-mistakes-required.yml:58`).
- Segregation of duties analogue: review findings never auto-fix in my config (`auto_fix.review: 0` in `~/.no-mistakes/config.yaml`), and merges happen only on my explicit word (`firstmate/AGENTS.md:25-42`).
- Tamper resistance: security-relevant repo settings are read only from the trusted default branch (`no-mistakes/docs/src/content/docs/reference/repo-config.md:37-83`), and my guard denies an unattended agent editing an existing `.no-mistakes.yaml` or lint config (`agents/claude/hooks/guard.py:1016-1100`).
- Signed history: SSH commit signing is on by default on this Mac (`dotfiles-nix/nix/home/darwin.nix:60`, `dotfiles-nix/nix/home/common.nix:74-77`).
- Upstream example of a regulated-grade release: baby-menu signs, notarizes, and verifies `codesign --verify --deep --strict` before publishing (`baby-menu/.github/workflows/release-please.yml:229`, `:293`).

Say it:
"Every change carries machine-verifiable evidence bound to its exact commit: it was reviewed, tested, and documented before it was published, and a required check can refuse anything that skipped the gate."

Gap and bridge:
- Gap: no change-advisory board, no ticket linkage, no formal four-eyes approval by a second human.
- Bridge: "The attestation is the automated half of a change record; in a bank I would attach it to the ServiceNow or Jira change, require a second approver on the PR, and keep the gate's step logs as retained audit evidence."
- Be candid: the gate's own docs say review is "probabilistic evidence, not a security or compliance certification" (`no-mistakes/docs/src/content/docs/reference/pipeline-steps.md:92`).

## 4. CI/CD including AWS CDK, CloudFormation, CodeBuild

Evidence:
- CI I wrote: `agents/.github/workflows/ci.yml` (shellcheck, `build-manuals --check`, unittest, pinned ruff) and `dotfiles-nix/.github/workflows/ci.yml` (shellcheck plus six sandboxed suites on `macos-latest`, added in `ba94692`).
- Infrastructure as code for the workstation: a Nix flake with pinned inputs declares the Mac and four Linux or WSL profiles (`dotfiles-nix/flake.nix:45-67`); a 22-entry manifest declares the fleet (`.fleet/manifest.yaml`).
- Drift detection: `bin/build-manuals --check` byte-compares generated manuals in CI (`agents/bin/build-manuals:79-121`), and `ic-doctor` verifies the live machine read-only, exiting 1 on any FAIL (`dotfiles-nix/files/bin/ic-doctor:503-508`).
- CD: the daily sync is continuous delivery of tools to my machine, fast-forward-only and rebuild on change (`dotfiles-nix/files/bin/sync-forks:195-249`).
- The gate can emit a CI workflow that mirrors its lint and test commands (`no-mistakes ci-workflow`, used for this vault's `.github/workflows/ci.yml`).

Say it:
"I treat configuration as code with drift detection: generated artifacts are committed, CI byte-compares them, and a read-only doctor verifies the live environment matches."

Gap and bridge:
- Gap: no AWS CDK, CloudFormation, or CodeBuild anywhere in the harness (`git grep` for `aws-cdk`, `cloudformation`, `codebuild`, `codepipeline` returned nothing in `agents`, `.fleet`, `dotfiles-nix`, `firstmate`, `no-mistakes`, `treehouse`, `gnhf`; the one hit in baby-menu was `xcodebuild`).
- Bridge, a design I can whiteboard (not implemented):
  1. A CDK app defines the pipeline with CDK Pipelines (`pipelines.CodePipeline`), sourcing from GitHub through a CodeStar connection.
  2. A synth `ShellStep` runs `cdk synth`; the pipeline is self-mutating, so pipeline changes also go through review.
  3. CodeBuild projects replace the gate's steps: lint, unit tests with CodeBuild report groups, SAST, and dependency audit, each with a `buildspec.yml` that runs the same commands as `.no-mistakes.yaml`.
  4. `cdk diff` output and the CloudFormation change set are attached to the change record as review evidence, like the gate's attestation.
  5. A `ManualApprovalStep` before production is the four-eyes control; separate AWS accounts per stage.
  6. CloudFormation drift detection plays the role `ic-doctor` plays on my machine.
- Say: "The shape I built locally, one publication path, evidence bound to a SHA, drift detection, maps one to one onto CodePipeline plus CodeBuild plus CloudFormation change sets; the services are new to me, the control design is not."

## 5. Distributed, reliable pipelines

Evidence:
- Idempotent convergence: `sync-forks` fast-forwards only when ahead is 0, reports diverged and ahead-only repos without touching them, retries a fetch once after 30 s, and never force-pushes (`dotfiles-nix/files/bin/sync-forks:169-249`).
- Crash-safe state machines: no-mistakes serializes runs per repo and branch with a mutex and recovers parked gates after a daemon crash (`no-mistakes/internal/daemon/manager.go:1274-1281`, [no-mistakes](../components/no-mistakes.md) section 8).
- Leases and ownership: treehouse keeps a locked JSON state file with atomic temp-file rename, durable leases, and quarantines slots whose state was corrupt (`treehouse/internal/pool/state.go:326-329`); 11 slots on this Mac carry that quarantine marker.
- Concurrency control: tasks-axi uses an `O_EXCL` lockfile with a 2.5 s timeout, compare-before-write, and atomic rename (`tasks-axi/src/backends/lock.ts:23-192`).
- Durable wakes: firstmate queues wakes on disk and acknowledges them only after handling (`firstmate/bin/fm-wake-drain.sh`, [firstmate](../components/firstmate.md) section 3).
- Server-side redundancy: a GitHub Actions job syncs the same forks while the laptop is off (`.fleet/.github/workflows/fleet-sync.yml`).

Say it:
"Every unattended job I run is idempotent, logs every decision, isolates failures per item, and fails closed on uncertainty; the bugs I found were exactly where one of those properties was missing."

Gap and bridge:
- Gap: this is a single machine plus GitHub Actions, not a multi-node streaming system.
- Honest defect I found: the server-side sync loop runs under `bash -e`, so the first failing `gh repo sync` aborts the step and skips every later repo (`.fleet/.github/workflows/fleet-sync.yml:29-30`); runs failed on 2026-09-22 and 2026-09-23.
- Bridge: "The same patterns carry to a trade or market-data pipeline: at-least-once delivery with idempotent consumers, leases like SQS visibility timeouts, per-key serialization like Kafka partitions, and a dead-letter path instead of aborting the batch."

## 6. Full stack and UI: React, ES6+, websockets, OpenFin, Electron, Tailwind

Verified by grep on 2026-09-23 (all upstream code in forks I run and test):

| Tech | Where | Evidence |
| --- | --- | --- |
| Electron | baby-menu 42.2.0, short-pipe 41.3 | `baby-menu/package.json:59`, `short-pipe/package.json:62` |
| Electron security model | context isolation, no Node in renderer, sandbox | `baby-menu/src/main/popover.ts:80-81`, `short-pipe/src/main/index.ts:86-88` |
| React | 19.2.6 in baby-menu, 19 in short-pipe and justroll (Ink), 19.2.0 via import map in autopreso, 18.3.1 in lavish-axi, 18.2.0 in presize | `baby-menu/package.json:44`, `short-pipe/package.json:51`, `justroll/package.json:56`, `autopreso/public/index.html:16`, `lavish-axi/package.json:55`, `presize/apps/web/package.json:28` |
| Tailwind | v4 in baby-menu, v4 browser build in lavish-axi, v3 in presize | `baby-menu/package.json:47`, `lavish-axi/package.json:34`, `presize/apps/web/package.json:30` |
| Websockets | `ws` servers in autopreso, trial-by-combat, lavish-axi | `autopreso/src/server.js:37`, `trial-by-combat/src/server.js:44`, `lavish-axi/src/server.js:1856` |
| ES6+ | ESM Node CLIs and TypeScript across the AXI tools | [axi](../components/axi.md) |
| OpenFin | absent | `git grep -il openfin` returned 0 files in all seven app repos and in `agents`, `.fleet`, `dotfiles-nix` |

How the harness itself uses the frontend stack:
- `chrome-devtools-axi` drives real Chrome for UI verification, with stale-element refs failing loudly as `STALE_REF` (`chrome-devtools-axi/src/uid-freshness.ts:16-42`).
- `lavish-axi` serves HTML review artifacts from one local Express plus `ws` server and returns element-anchored feedback.

Say it:
"I read, run, and test Electron, React 19, Tailwind v4, and websocket codebases daily, and I can walk through the Electron security model and a websocket protocol design in the terms a trading desktop needs."

Gap and bridge:
- Gap: I did not author these frontends, and there is no OpenFin code anywhere.
- OpenFin bridge: OpenFin is a Chromium-based desktop runtime like Electron, aimed at financial desktops, adding managed window layouts, an inter-app messaging bus and channels, and FDC3 interop between apps from different vendors (from public docs, not used in the harness).
  "The Electron discipline transfers directly: renderer never gets Node, all privileged calls go through a narrow typed bridge (`baby-menu/src/preload/index.ts:80`), and entitlement-sensitive actions live in the main or service process."
- Websocket bridge for market data: autopreso broadcasts full state and coalesces bursts into one next turn (`autopreso/src/transcript-turn-queue.js:1`), trial-by-combat heartbeats every 15 s (`trial-by-combat/src/server.js:130`).
  "For a blotter I would keep a typed message union but send sequence-numbered deltas with periodic snapshots, conflate per instrument when the UI falls behind, and resubscribe with last-seen sequence on reconnect."

## 7. Object-oriented language (Python or Java)

Evidence (mine):
- `agents/claude/hooks/guard.py`: 1,204 lines of Python, a bash-subset lexer and classifier with frozen dataclasses for value types (`guard.py:84-92`, `:442-443`) and pure decision functions; `post_edit.py` 165 lines.
- Tests are `unittest.TestCase` classes grouped by concern: `ClassifierTest`, `LexerTest`, `PresenceTest`, `DecisionTest`, `ProcessTest`, `SettingsHookTest` (`agents/claude/hooks/test_guard.py:197-392`).
- Lint and format: ruff 0.16.1 check and format clean, pinned in CI.

Say it:
"My Python is small, typed, and tested: immutable value objects, pure classification functions, and a thin I/O shell, which is the same functional-core, imperative-shell split I would use in a Java service."

Gap and bridge:
- Gap: zero `.java` files in any harness or app repo (`git ls-files '*.java'` returned 0 in 16 repos).
- Bridge: cover Java from prior experience; frame design in language-neutral terms (interfaces at boundaries, immutable value types, dependency injection for testability).

## 8. Code quality and efficiency

Evidence:
- Gate record: 1,385 mistakes reported, 69 percent fixed, 879 fixes from the review step (`no-mistakes stats`, 2026-09-23).
- Immediate feedback: `post_edit.py` runs ruff or clang-format after every edit and returns findings to the agent in the same turn (`agents/claude/hooks/post_edit.py:86-118`).
- No drift by construction: one rule source, a duplicate-rule check that aborts the build on a verbatim copy (`agents/bin/build-manuals:79-121`).
- Token efficiency: every agent-facing CLI is built on an SDK that emits compact TOON with counts and next-step hints (`axi/packages/axi-sdk-js/src/errors.ts:12-18` for the exit-code contract); upstream benchmarks report `gh-axi` at 100 percent task success vs 87 percent for the GitHub MCP server (`axi/README.md:37-45`, upstream numbers, not re-run).
- Compute efficiency: firstmate's watcher is bash that wakes the model only on actionable events, so idle supervision costs zero tokens (`firstmate/docs/supervision-protocols/claude.md:1-9`).

Say it:
"Quality is enforced at three distances: on every edit by a lint hook, on every change by the gate, and on the whole system by doctors and drift checks."

Gap and bridge: no static-analysis dashboard or trend metrics; I would export the gate's per-step findings to a team dashboard.

## 9. Partnering with the business

Evidence:
- Intent first: the brief separates my words (`## Captain's intent`) from implementation detail (`## Firstmate spec`), and only my words drive the gate's review (`firstmate/AGENTS.md:552-558`).
- Outcome language: "Talk in outcomes, not mechanics"; every captain-facing message must translate internal state into outcome, consequence, and next decision (`firstmate/AGENTS.md:482-483`).
- Decision queues: wheelhouse turns every owner-only decision into one issue card with checkboxes, and model output is advisory until a human ticks it (`wheelhouse/README.md:5-11`).
- Explicit trade-offs: the routing policy states that sending Tier 2 work to Grok or Gemini "trades some quality for subscription utilization" (`agents/ROUTING.md:13`).

Say it:
"I design systems so the non-engineer's intent is captured verbatim and verified against, and every decision that needs a human is surfaced as one explicit, auditable choice."

Gap and bridge: the harness has one stakeholder, me; cover business partnering with prior experience and frame the harness as the tool that lets a small team respond faster to trader requests.

## 10. Global Markets exposure

Evidence in the harness: none directly; do not stretch it.

Bridges by analogy, useful only after stating the gap:
- quota-axi is a limit service: it publishes remaining capacity and runway per window and never decides, like a credit or risk-limit service that an order router consults (`quota-axi/README.md:685`).
- The routing gates resemble pre-trade checks: drop anything that cannot complete within its limit, then rank survivors (`agents/ROUTING.md:17-18`).
- Fast-forward-only sync resembles golden-source reference data: consumers never edit the source, divergence is reported, never auto-merged.
- The gate's attestation resembles a release evidence pack for model or pricing library changes.

Say it:
"The harness is not markets code, but its control design, limits consulted before dispatch, one audited release path, and golden-source syncing, is the same design markets technology uses."

## Commands used to verify frontend and gap claims

```sh
cd ~/github
grep -n -E '"(react|electron|tailwindcss|ws|express|ink)"' baby-menu/package.json short-pipe/package.json justroll/package.json autopreso/package.json trial-by-combat/package.json lavish-axi/package.json
grep -n "WebSocketServer(" autopreso/src/server.js trial-by-combat/src/server.js lavish-axi/src/server.js
for r in baby-menu short-pipe justroll autopreso presize trial-by-combat org-bench; do git -C $r grep -il openfin -- . | wc -l; done
for r in agents .fleet dotfiles-nix firstmate no-mistakes; do git -C $r grep -il -E 'aws-cdk|cloudformation|codebuild|codepipeline' -- . | wc -l; done
```

Related: [Pitches](Pitches.md), [Question-Bank](Question-Bank.md), [apps and benchmarks overview](../components/apps-and-benchmarks-overview.md).
