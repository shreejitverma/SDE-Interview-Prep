---
type: playbook
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://github.com/shreejitverma/agents, https://github.com/shreejitverma/fleet-ops, https://github.com/shreejitverma/dotfiles-nix, https://github.com/shreejitverma/firstmate, https://github.com/kunchenguid/firstmate, https://github.com/kunchenguid/no-mistakes, https://github.com/kunchenguid/treehouse, https://github.com/kunchenguid/quota-axi, https://github.com/kunchenguid/tasks-axi, https://github.com/kunchenguid/gnhf, https://github.com/kunchenguid/axi, https://github.com/kunchenguid/chrome-devtools-axi, https://github.com/kunchenguid/programbench-bench]
---

# Question bank - 49 questions with model answers

Answers are written to be said in 30 to 90 seconds.
Each one is grounded in a component note or a command I ran on 2026-09-23; cites use `repo/path:line` relative to `~/github`.
Authorship reminder: the tools are mostly upstream forks (Kun Chen, `kunchenguid/*`); the policy, integration, hooks, generator, dotfiles additions, fleet ops, and six firstmate patches are mine.

## A. Architecture

**Q1. Walk me through the architecture.**
I talk to one supervisor agent, firstmate, which never edits code (`firstmate/AGENTS.md:25-42`).
It files a backlog row with tasks-axi, picks a model using my routing rules plus quota evidence from quota-axi, and spawns a worker agent in its own treehouse worktree.
The worker changes code and publishes only by pushing to a local no-mistakes remote, which runs a fixed nine-step gate and opens an attested PR (`no-mistakes/internal/types/types.go:52-60`).
When CI is green the supervisor asks me to merge, merges only on my word, and tears down the worktree after proving the work landed.
Underneath, my dotfiles-nix fork and fleet-ops install, link, sync, and health-check all of it.

**Q2. What exactly does the supervisor decide, and what does it never do?**
It decides judgment calls: which project, whether a task is a code change or a research scout, which tier and model, and when to escalate to me.
It never writes to a project, never merges without my explicit word, and never tears down unlanded work (`firstmate/AGENTS.md:25-42`).
The design rule is "Logic that can be exact lives in deterministic scripts; work that requires understanding lives in an agent; the two never mix" (`firstmate/VISION.md:34`).

**Q3. Why a local Git proxy gate instead of relying on CI?**
CI runs after the branch is public and costs a round trip per fix; the gate runs before publication in a disposable worktree, fixes mechanical problems itself, and only publishes a green branch.
It also makes "passed" mean the same thing in every repo, because step order is fixed in code and a repo can only add gates, never remove core steps (`no-mistakes/internal/types/types.go:137`).
CI still runs afterward for broad regression; the gate's CI step polls it and can auto-fix up to the configured limit.

**Q4. How do parallel agents avoid stepping on each other?**
Each task gets its own treehouse worktree at detached HEAD, and spawn waits until the pane's working directory is a distinct worktree before launching the agent (`firstmate/bin/fm-spawn.sh:3843-3902`).
Detached HEAD makes pooled slots interchangeable because Git refuses one branch in two worktrees; the task creates its own branch after it owns the slot.
Slots are recycled only if idle, unleased, clean, and merged, all checked under an exclusive lock (`treehouse/internal/pool/pool.go:433-457`).

**Q5. Where does state live, and what happens if the supervisor session dies?**
Every durable fact is on disk: backlog, briefs, append-only status files, task metadata, and a durable wake queue acknowledged only after handling (see [firstmate](../components/firstmate.md) section 3).
A new session takes a per-home lock, drains the wake queue, and replays half-finished backlog transitions from marker files (`firstmate/bin/fm-teardown.sh:15-23`).
The gate keeps its own SQLite state and recovers parked runs after a daemon crash.

**Q6. How does model routing work?**
Two steps (`agents/ROUTING.md:9-22`).
Step one classifies the task: Tier 1 frontier reasoning gets the strongest model only; Tier 2 standard coding and Tier 3 mechanical work each have three peers across Claude, Grok, and Gemini.
Step two reads quota-axi: drop any candidate whose runway is exhausted or would end before the task finishes, then take the highest `spendPriority` among survivors; unknown evidence stays eligible but never outranks known evidence (`agents/ROUTING.md:17-18`).
Tier 1 never trades down by quota; it falls back in a fixed order and stops if nothing has runway.
Inside firstmate the same rules live in a local `config/crew-dispatch.json`, and spawn refuses to launch without an explicit harness when that file exists (`firstmate/bin/fm-spawn.sh:2088-2089`).

**Q7. What is AXI and why do you care?**
AXI is a set of ten design principles for CLIs whose main user is an agent, plus a small SDK that implements them (see [axi](../components/axi.md)).
The principles that matter most: compact TOON output, explicit empty states, pre-computed counts, structured errors with exit code 2 for validation errors (`axi/packages/axi-sdk-js/src/errors.ts:12-18`), and next-step hints.
Every CLI my agents call for GitHub, browser, review, backlog, and quota is built on it, and the principles are compiled into every tool's manual.

**Q8. How do you keep four AI tools following the same rules?**
One source of truth in my `agents` repo: `CORE.md` plus `ROUTING.md`, plus a thin per-tool tuning file, compiled by a bash generator into `CLAUDE.md`, `GROK.md`, `GEMINI.md`, and a neutral `AGENTS.md` (`agents/bin/build-manuals:27-62`).
CI byte-compares the generated manuals and fails on drift, and the build aborts if a tuning file copies a core rule verbatim (`agents/bin/build-manuals:79-121`).
`ic-link` symlinks each manual into the path that tool reads, so one edit changes every tool together.

## B. Why not X

**Q9. Why not a single agent doing everything?**
A single long session degrades as context grows, serializes independent work, and mixes the roles of doer and checker.
Separating a read-only supervisor from workers gives me separation of duties, and the gate is a third party the worker cannot skip.
For long unattended work I use gnhf, which restarts the agent every iteration and carries only a small `notes.md` forward (`gnhf/src/templates/iteration-prompt.ts:43-61`).

**Q10. Why not MCP for everything?**
My Claude config defines zero MCP servers (`~/.claude.json` counted by key); agents reach tools through shell CLIs.
A CLI costs no schema tokens until called, composes with pipes and exit codes, and can be tested like any program.
Upstream's benchmark reports `chrome-devtools-axi` at 79,141 average input tokens per task versus 184,711 for the raw MCP server it wraps (`chrome-devtools-axi/README.md:27-37`, upstream numbers, not re-run by me).
MCP is not wrong: that same tool keeps one MCP session inside a detached bridge process, so MCP is an implementation detail behind a token-efficient CLI.

**Q11. Why not LangGraph or CrewAI?**
Those are libraries for building agent applications in code, with explicit graphs or role definitions.
My workers are already complete coding agents with their own tool loops (Claude Code, Grok Build, Gemini via `agy`); what I needed was process-level orchestration: worktrees, terminals, a backlog, quota evidence, and a release gate.
That layer is mostly deterministic, so it lives in tested scripts, and swapping the worker vendor is a flag (`fm-spawn.sh --harness`).
If I were building an in-product assistant, for example a trade-support bot with an explicit state machine, a graph library would be a reasonable choice.

**Q12. Why forks instead of installing released packages?**
Three reasons: I can read and patch the code (six firstmate fixes shipped on my fork), I install from a clone I have reviewed rather than a moving registry, and I control when upstream changes arrive.
Every fork is declared in one manifest, and a daily job fast-forwards only when my fork has no local commits, reports divergence instead of merging, and never force-pushes (`dotfiles-nix/files/bin/sync-forks:195-249`).
The cost is divergence management: firstmate carries my commits, so it is `sync: false` and takes upstream through deliberate merge PRs (`.fleet/manifest.yaml:26-39`).

**Q13. Why is so much of this bash?**
The orchestration is glue over git, tmux, and CLIs, and bash is the native language of that glue.
The trade-off is a large surface of shell edge cases; my own firstmate patches fixed a SIGPIPE race and a Bash 3.2 heredoc parse failure (`157d3d82`, `ce99d183`).
Upstream pays for it with about 220 test files and sharded CI, and I ran three suites locally: 55, 21, and 36 checks, all passing (see [firstmate](../components/firstmate.md) section 9).

**Q14. Why build a decision queue on GitHub Issues instead of a service?**
wheelhouse (upstream, configured by me) has no server or database: issues are the queue, labels are the state machine, Actions are the workers, and GitHub's audit log records every decision (`wheelhouse/README.md:5-11`).
The cost is GitHub's eventual consistency and schedule delay: the hourly scan actually ran every three to six hours on my fork.

**Q15. Why Nix for your machine?**
The flake pins every input and declares the Mac plus four Linux and WSL profiles, so a fresh machine converges from one command (`dotfiles-nix/flake.nix:45-67`).
Invariants Nix cannot check itself, like the checkout path that out-of-store symlinks depend on, are asserted at evaluation time and again in shell guards (`dotfiles-nix/nix/user.nix:23-32`).

## C. Failure and recovery

**Q16. What happens when a model subscription runs out mid-task?**
It happened on 2026-09-18: three gated runs failed at review with Claude's "You've hit your session limit" (`~/.no-mistakes/logs/01M2T5FMMF5E2Z0SRMZD3BP6MH/review.log`).
I switched the gate agent to Grok, two runs completed with PRs, and the third hit Grok's own 402 "usage balance exhausted"; it completed on Claude after the 11:30 reset (full story in [STAR-Stories](STAR-Stories.md) story 3).
The lesson is now policy: gate long runs on runway, not headline percentage, because "a single long gate run can drain a whole week of Grok credits" (`agents/ROUTING.md:20`).

**Q17. What if a worker crashes or stalls?**
The zero-token watcher classifies status lines in bash and wakes the supervisor on `stale:` or `signal:` events (`firstmate/docs/supervision-protocols/claude.md:1-9`).
The supervisor can interrupt, exit, or relaunch the worker through a verified control script that never tears down work.
Teardown refuses a dirty or unlanded worktree unless I explicitly authorize discarding it (`firstmate/AGENTS.md:34-37`).

**Q18. What if the gate's daemon dies mid-run?**
Runs are serialized per repo and branch (`no-mistakes/internal/daemon/manager.go:1274-1281`), and on restart the daemon recovers crashed runs, reconciles parked gates, and keeps auto-fix attempt counts durable ([no-mistakes](../components/no-mistakes.md) section 8).
Database writes happen after the remote settles, so a partial failure is re-entrant.

**Q19. What if a state file is corrupted?**
treehouse rebuilds entries from disk and marks every recovered slot as leased with the holder "recovered: state file was corrupt or truncated; verify before reuse" (`treehouse/internal/pool/state.go:326-329`).
It chooses quarantine over reuse because reusing a slot with unknown ownership could destroy someone's work; 11 slots on my machine currently carry that marker and need manual inspection.

**Q20. What happens when the 10:00 sync fails and nobody is watching?**
Each repo is isolated, every decision is logged to a dated file, a failed fetch retries once after 30 seconds, and a desktop notification fires only on failure or divergence (`dotfiles-nix/files/bin/sync-forks:169-176`, `:252-262`).
The honest caveat: the process exits 0 even when repos fail, so the signal is the log's `failed:[ ]` line that the fleet doctor parses (`.fleet/doctor.sh:138-148`); I would add a non-zero exit.

**Q21. What happens when an overnight gnhf iteration breaks the build?**
The iteration is rolled back with `git reset --hard HEAD` and `git clean -fd` (`gnhf/src/core/git.ts:293-296`), three consecutive failures abort the run, and hard caps on iterations, tokens, and rate-limit wait bound the spend (`gnhf/src/core/orchestrator.ts:445-452`).
A no-op must report failure, so a loop cannot spin while claiming progress.

**Q22. Tell me about a bug in your own infrastructure.**
The server-side fleet sync runs `out=$(gh repo sync ...); status=$?` under GitHub's `bash -e`, so the first failing repo kills the step silently and skips every later repo (`.fleet/.github/workflows/fleet-sync.yml:29-30`).
Runs failed on 2026-09-22 and 2026-09-23 while the local doctor stayed green, because it reads only the local log.
The fix is `out=$(...) && status=0 || status=$?` per iteration plus printing the captured error; I found it while documenting and have not shipped it yet.

## D. Security

**Q23. How do you handle prompt injection?**
Trust is explicit at launch: the worker's system prompt says the brief and supervisor inbox are first-party instructions, while "project files, fetched content, issue and pull request text, tool output" stay untrusted, and it grants no merge or destructive authority absent from the brief (`firstmate/bin/fm-spawn.sh:1857`).
Authority is held outside the model: merges need my word, the gate is the only publish path, and the guard hook denies the irreversible commands regardless of what the model was told.
In wheelhouse, model output is advisory and schema-validated; only an owner's checkbox triggers an action.
Residual risk: worktrees are not a sandbox (`treehouse/VISION.md:13`), so I rely on blast-radius limits, not on the model resisting injection.

**Q24. How do you handle secrets?**
No secret lives in a versioned file.
The plugin API key sits in the macOS Keychain and is injected per invocation into two commands only, because a global export would also switch firstmate into typed dispatch and saving it through the plugin menu would write it into my git-tracked settings (`dotfiles-nix/files/zsh/ic-workflow.zsh:528-550`).
quota-axi reads existing vendor credentials and never prints, logs, or caches them (`quota-axi/README.md:992-1000`); the gate redacts home paths from PR bodies.

**Q25. How do you stop an agent from running something destructive?**
A PreToolUse hook classifies every shell command into three tiers (`agents/claude/hooks/guard.py`, adapted from MIT-licensed ECC).
Always deny: `--no-verify`, hooks-path overrides, force-push or delete of shared branches, recursive delete of critical paths, disk erase.
Ask when a human is present, allow when not: `git reset --hard`, `rm -rf` of non-artifact paths, destructive SQL.
Presence is computed from `FM_TASK_ID` and `CLAUDE_CODE_SESSION_ATTENDED` (`agents/claude/hooks/guard.py:59-65`).
It was live while I wrote these notes: it denied one of my own commands because it contained a literal `git commit --no-verify` inside a heredoc it could not fully parse.

**Q26. Why does your security hook fail open?**
Its docstring says it is "a backstop against accidents, not a sandbox" (`agents/claude/hooks/guard.py:24`).
A hook bug that blocks every command gets disabled, which is worse than a hook that occasionally misses.
The compromise: if parsing fails, a crude separator split still denies any always-block marker, so only lower tiers fail open (`9e92de5`).

**Q27. How do you stop an agent from making a failing check pass by weakening it?**
The guard denies an unattended agent editing an existing lint, format, or gate config such as `ruff.toml` or `.no-mistakes.yaml`, while still allowing creation of a new one (`agents/claude/hooks/guard.py:1016-1100`).
The gate reads security-relevant repo settings only from the default branch, so a feature branch cannot relax its own gate (`no-mistakes/docs/src/content/docs/reference/repo-config.md:37-83`).

**Q28. What supply-chain risks did you consider?**
Tools are installed from forks I control, with lockfile-frozen installs (`pnpm install --frozen-lockfile`) in the manifest.
Upstream repos pin the shared required-check action by SHA (`baby-menu/.github/workflows/no-mistakes-required.yml:58`).
wheelhouse holds any fork CI run that touches workflows or actions for manual review and fails closed, because approving it could run attacker code with repository secrets (`wheelhouse/AGENTS.md:24-30`).

**Q29. Workers run with `--dangerously-skip-permissions`. Isn't that reckless?**
Unattended agents cannot answer prompts, so firstmate defaults to bypass mode (`firstmate/docs/configuration.md:370-379`).
Safety comes from structure instead: isolated worktrees, the always-deny guard tier, the gate as the only publish path, and merge authority kept with me.
One gap I state openly: I have not tested whether a hook `deny` is honored in bypass mode when the gate launches Claude, so that belongs on my verification list.

## E. Cost control

**Q30. How do you control spend across three subscriptions?**
Classify first, so expensive models only see work that needs them; Fable's separate weekly window is spent only on Tier 1 (`agents/ROUTING.md:11`).
Then use quota evidence to spend use-it-or-lose-it allowance first and drop anything that would run out mid-task.
Supporting controls: gnhf caps, a zero-token watcher, token-efficient CLIs, and firstmate disabling the compact adviser for unattended workers (`firstmate/bin/fm-spawn.sh:4727`).

**Q31. Explain spendPriority.**
Per window: `gap = percentRemaining / timeRemainingPercent - burnMultiple`; per scope, the cycle-length-weighted mean of gaps, clamped to plus or minus 100 (`quota-axi/src/pace.ts:308-382`).
Positive means allowance will be forfeited at reset at the current burn, so spend there first; negative means overdrawn.
Weighting by cycle stops a five-hour window from swamping a weekly one; the note reproduces the tool's 0.3399 for Claude by hand (see [quota-axi](../components/quota-axi.md) section 3).

**Q32. Why is percent remaining not enough?**
Percent answers "how much is left"; runway answers "how long at this burn".
In a real snapshot Claude showed 54 percent remaining, bound by the weekly window, but only about 9,777 seconds of runway, bound by the five-hour window (see [quota-axi](../components/quota-axi.md) section 3).

**Q33. How do tool outputs affect cost?**
Every stdout byte is input tokens and every ambiguous answer is another turn.
Upstream's GitHub benchmark reports `gh-axi` at 100 percent success and $0.050 per task versus 87 percent and $0.148 for the GitHub MCP server (`axi/README.md:37-45`, upstream numbers).
My job was to make those CLIs the default path, through skills and manuals.

## F. Scaling

**Q34. How would this scale to a team of 50 engineers?**
Split per-engineer from shared.
Per engineer: supervisor, worktree pool, and quota view stay local.
Shared: the rule source becomes a team repo with per-team overlays compiled by the same generator; the gate config lives in each repo with an org-level required check; guard policy ships as managed settings; a model gateway enforces per-team budgets instead of personal subscriptions.
I have not run it at that scale, so I would pilot with one team and measure gate rescue rate and review latency before widening.

**Q35. What changes for a regulated bank?**
Approved models and data paths only (no code to unapproved endpoints), secrets from the bank's vault, audit retention of gate logs and attestations, integration with the change system, a second human approver on every PR, and agents never approving their own work.
The existing design already separates doer, checker, and approver; the bank version makes the approver a second person.

**Q36. It is one machine. How would you make it distributed?**
Move the state that is already on disk into shared services: backlog to a ticket system, run state to a database with per-branch leases, wakes to a queue with at-least-once delivery.
The idempotency, lease, and fail-closed patterns already in treehouse, tasks-axi, and the gate carry over unchanged.

**Q37. How would you roll it out without disrupting a team?**
Start with the gate as an optional push target, because `origin` stays untouched and adoption is opt-in (`no-mistakes/docs/src/content/docs/concepts/gate-model.md:72`).
Then make the attestation check required on one repo, then add supervision for volunteers.

## G. Observability

**Q38. How do you know what the agents are doing?**
Append-only status files per task, supervisor wakes, per-step gate logs, and the gate's SQLite telemetry of every agent invocation.
`no-mistakes stats --agents` showed 407 review invocations averaging 4m2s on my machine, and `no-mistakes stats` showed 228 changes, 133 with a mistake caught and fixed.

**Q39. What would you add first?**
Export gate telemetry and sync outcomes to a central time series with alerts, because today failure signals are desktop notifications and log lines.
The server-side sync failure in Q22 went unnoticed locally for exactly that reason.

**Q40. How do you detect configuration drift?**
Generated manuals are byte-compared in CI (`agents/bin/build-manuals --check`), `ic-doctor` verifies every link, hook script, and manual on the live machine and exits 1 on any FAIL (`dotfiles-nix/files/bin/ic-doctor:503-508`), and the fleet doctor verifies remotes, identities, binaries, and the last sync.
It works: the live `settings.json` currently shows a dropped model pin as a dirty tree, which ic-doctor reported as its one warning.

## H. Testing an agent system

**Q41. How do you test an agent system?**
Split it.
Deterministic parts get hermetic unit tests: firstmate suites use fake `tmux`, fake `curl`, and temp homes.
Hooks are tested by running the exact command string from settings (`agents/claude/hooks/test_post_edit.py:204-257`).
Agent behavior is evaluated statistically: the gate ships local eval commands (`no-mistakes eval capture`, `miss`, `relabel`, `report`), and harness choices are judged by paired benchmarks.

**Q42. How do you test scripts whose job is to install things or push to GitHub?**
Run the real script against a sandbox: fake `HOME`, stub executables first on `PATH` that record their calls, and local bare Git remotes so fetches and pushes are real but offline.
Assert resulting state, not source text; the one real-install test runs in Docker (see [dotfiles-nix](../components/dotfiles-nix.md) section 9).

**Q43. How do you decide whether a harness practice actually helps?**
Controlled, paired measurement.
programbench-bench (upstream) held the model fixed and found a mandated TDD skill scored 48.8 versus 52.4 without it, at $1.73 versus $1.12 per task, Wilcoxon p = 3.1e-08 over 192 paired tasks (`programbench-bench/blog/does-tdd-help-coding-agents/index.md:30`); I recomputed those numbers from the committed CSVs.
I read benchmarks with n = 1 per cell, like org-bench, as anecdotes.

**Q44. How do you deal with flaky tests?**
Find the timing dependency and make it deterministic in the test.
My firstmate SIGPIPE fix is the example: an intermittent "Broken pipe" came from returning inside a process-substitution read loop, and the regression test reproduces the CI shape deterministically with SIGPIPE ignored and a delayed producer (`157d3d82`).
I also flag environment-dependent failures instead of rewriting assertions, like treehouse tests that fail only because my global Git config signs tags.

## I. Determinism

**Q45. How do you get deterministic behavior from nondeterministic models?**
Keep the model's output small and structured, and keep control flow in code.
The gate and gnhf call Claude with a JSON schema (`gnhf/src/core/agents/claude.ts:180-191`), invalid review output is never treated as clean and is retried up to three times, and step order is fixed in code.
Decisions with consequences, like pushing, merging, and resetting, are deterministic code with preconditions.

**Q46. How do you make sure what was pushed is what was reviewed?**
The push step requires the head to equal or descend from the durable review-approved commit (`no-mistakes/internal/pipeline/steps/push.go:346`), pushes that exact SHA, and re-reads the remote with `ls-remote` afterward (`push.go:170-205`).
A CI repair publishes only if it provably descends from the reviewed head; otherwise it goes back through review.

## J. Reflection

**Q47. What would you change first?**
Fix the server-side sync loop so one failure cannot hide the rest, make `sync-forks` exit non-zero on failures, and test whether hook denials hold in bypass mode.
Then reconcile the settings drift, and measure coverage on the Python hooks.

**Q48. What is the biggest weakness of this setup?**
Concentration: most tools come from one upstream author, and my fork can lag (firstmate is 31 commits behind upstream as of 2026-09-23).
Forks plus fast-forward sync mitigate it, and the gate and manuals are the parts I would keep if I swapped any tool.

**Q49. What did building this teach you that applies here?**
Controls belong in the pipeline, not in people's memory; evidence must be bound to the exact artifact; and unattended jobs must fail loudly and per item.
Those are the same properties a Global Markets release process needs.

Related: [Pitches](Pitches.md), [JD-Mapping](JD-Mapping.md), [STAR-Stories](STAR-Stories.md), [Cheat-Sheet](Cheat-Sheet.md).
