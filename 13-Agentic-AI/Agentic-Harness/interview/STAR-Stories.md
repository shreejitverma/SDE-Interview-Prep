---
type: playbook
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://github.com/shreejitverma/agents, https://github.com/shreejitverma/fleet-ops, https://github.com/shreejitverma/dotfiles-nix, https://github.com/shreejitverma/firstmate, https://github.com/kunchenguid/firstmate, https://github.com/kunchenguid/no-mistakes, https://github.com/kunchenguid/quota-axi]
---

# STAR stories - three evidenced incidents from the harness

Every story below is reconstructed from evidence on disk: commit hashes, PR numbers (read with `gh-axi pr list`, read-only), the gate's own SQLite state (read with `sqlite3 -readonly ~/.no-mistakes/state.sqlite`), and step logs under `~/.no-mistakes/logs/`.
Where a causal link is inferred from timing rather than stated in a record, the story says so.
Each story ends with a 60-second spoken version.

| # | Story | Lens it serves | Core evidence |
| --- | --- | --- | --- |
| 1 | The gate caught fail-open bypasses in my own security hook, then a regression in the fix | code quality, security, testing | agents PR #8, run `01M368PN3APV4Z3HQE4W87D719`, commits `ee57f59`, `8d05cab`, `9e92de5` |
| 2 | Handling a fork that stopped being a mirror (firstmate) | change management, release process | firstmate PRs #1 to #3, fleet-ops PR #4 (`db9a398`), commits `157d3d82`, `ce99d183` |
| 3 | Quota exhaustion mid-delivery, fallback, and the policy it produced | reliability, cost, incident response | gate runs on 2026-09-18, fleet-ops PR #3, agents PR #3, dotfiles-nix PR #14, agents `5230b78`, `dcfab28` |
| B1 | Scheduled sync took 3.5 hours; a reinstall collided with the gate daemon | operations | dotfiles-nix `b790d5f` (PR #11), fleet-ops `a12d58d` (PR #2) |
| B2 | Review caught a parser-breaking pipe in a manifest entry | config as code | fleet-ops `b0b990a` (PR #7) |

## Story 1 - the gate caught my security hook failing open

**Situation.**
On 2026-09-23 I added a PreToolUse guard hook to my `agents` repo, adapted from the MIT-licensed ECC project (`ee57f59`, PR #8 "feat(claude): add guard hook, review subagents, and path-scoped rules").
Its job is to always deny `--no-verify`, force-pushes to shared branches, and similar irreversible commands on every shell call any Claude session makes (`agents/claude/hooks/guard.py:59-65`).

**Task.**
Ship it through my own rule, the no-mistakes gate, and make sure the hook actually blocks what it claims, including the exact command shapes an agent emits.

**Action.**
- Pushed through the gate (run `01M368PN3APV4Z3HQE4W87D719`); review findings park for a decision because my config sets `auto_fix.review` to 0.
- Round 1 of review returned three warnings and one info finding (decoded from the run's `step_rounds.findings_json`).
  The most serious: a `$(...)` substitution containing a heredoc with an apostrophe made the lexer raise, and the hook failed open; the finding noted this is Claude Code's standard commit-message form, so `git commit --no-verify -m "$(cat <<'EOF' ...)"` would pass.
  Another: closing a file descriptor (`>&-`) made the guard skip classifying the next command.
  A third: if the hook script path was missing, the hook errored instead of failing open as intended.
- The findings were selected for fixing (the run records the selection source as `user`), and the review step's fix loop produced `8d05cab` "Fix guard lexer bypasses and fail open on missing script" (+192/-46 across `guard.py`, `test_guard.py`, `settings.json`).
- Round 2 reviewed the fix itself and found a regression it introduced: an empty quoted word caused an index error, and `$((1<<3))` inside a multi-line substitution was read as a heredoc, both failing open and letting a trailing `git commit --no-verify` through.
- The next fix, `9e92de5` "Harden guard parser and deny unparseable always-block commands" (+90/-5 with 55 new test lines), changed the policy: when parsing fails, a crude separator split still denies any always-block marker.

**Result.**
- Later review rounds reported no findings; test, document, lint, push, PR, and CI completed and PR #8 merged at 00:56 the same night (`6d4dcf8`).
- The suite now has 51 cases; I re-ran it on 2026-09-23: "Ran 51 tests ... OK".
- Live proof while writing these notes: the hook denied one of my own test commands because it could not parse a heredoc and the command contained `git commit --no-verify`, the exact `9e92de5` behavior.

**What I learned.**
Review the fix, not just the change; the second round found a bug the first fix created.
A fail-open safety tool must keep its highest-severity rules fail-closed.

**60-second version.**
> I wrote a guard hook that blocks things like `--no-verify` for every shell command my agents run, and shipped it through my own review gate.
> The gate's review found that Claude's standard way of writing a commit message, a heredoc with an apostrophe inside a command substitution, crashed my lexer, and my hook failed open, so `--no-verify` would have gone straight through.
> I fixed it with tests, and the next review round found that my fix introduced two new fail-open paths.
> So I changed the policy: if the command cannot be parsed, the always-deny rules still apply.
> The lesson I took: gate the fix too, and keep the top tier of a fail-open tool fail-closed.

## Story 2 - a fork that stopped being a mirror (firstmate)

**Situation.**
My fleet of 22 forks is kept current by a daily job that only fast-forwards and never merges or force-pushes (`dotfiles-nix/files/bin/sync-forks:195-249`).
On 2026-09-19 at 15:37 my first fork-specific change to firstmate merged: PR #1 "fix(bin): validate crew-dispatch harnesses against fm_control_harnesses without the typed key" (`e4ed7634`).
The bug it fixed: a hard-coded fallback list never gained `gemini`, so my own routing config was reported invalid at every session start (`9c19c22b` message).

**Task.**
Keep my patch, keep taking upstream, and stop the automation from mishandling a fork that can no longer fast-forward.

**Action.**
- While PR #1 was in the gate, its CI step found an unrelated intermittent failure: a Herdr behavior test saw "printf: write error: Broken pipe".
  The root cause was a lookup that returned from inside a process-substitution read loop, closing the pipe while the producer still wrote; the fix drains the producer and adds a deterministic regression test that fails before and passes after (`157d3d82`, `bin/fm-control-lib.sh`, `tests/fm-control.test.sh`).
- Four minutes after the merge I shipped fleet-ops PR #4 (`db9a398`, 15:41): firstmate becomes `sync: false`, `repos.txt` is regenerated so the server-side job stops touching it, and the README records why and how upstream now arrives.
  The commit message states the reason: the fork "can never fast-forward again", the local sync "would report it as ahead and then diverged every day, and the server-side loop would fail on it".
- Upstream intake became a deliberate, gated merge PR: PR #2 "feat(bin): sync fork with upstream firstmate (22 commits through 3fcbc6c)", merged with a merge commit (`2d30a0d9`, `21b61244`), because squash-merging a sync PR discards the upstream parent and re-conflicts forever (`dotfiles-nix/AGENTS.md:37-45`).
- PR #3 took one more upstream commit and made two tests independent of my shell; the gate's CI step then caught that my own new comment, an apostrophe inside a heredoc inside `$(...)`, broke stock macOS Bash 3.2 parsing, and fixed it (`ce99d183`).

**Result.**
- The fork carries exactly 6 non-merge commits of mine, 8 files, +73/-19 lines, all shipped through the gate as PRs #1 to #3 (`git diff --shortstat` against the merge base, 2026-09-23).
- The daily sync keeps running green for the other 20 forks; firstmate is documented as the exception with its reason in `.fleet/manifest.yaml:26-39`.
- Honest open item: upstream is 31 commits ahead of my fork as of 2026-09-23, so the next sync PR is due.

**What I learned.**
Automation should detect "this is no longer a mirror" and hand the decision to a human, not guess; and divergence is a policy decision that must be written next to the config it changes.

**60-second version.**
> My tool forks are synced automatically, but only by fast-forward, so nothing of mine is ever overwritten.
> When I merged my first real patch to the supervisor tool, that fork could never fast-forward again, so within minutes I marked it as a manual-sync repo, with the reason recorded in the manifest, and upstream now arrives through a reviewed merge PR.
> Along the way the gate's CI step caught an intermittent broken-pipe race in the upstream code, which I root-caused and fixed with a deterministic regression test.
> In a bank this is the same problem as a vendor library you have patched: pin it, record why, and take upgrades as reviewed changes.

## Story 3 - quota exhaustion mid-delivery (2026-09-18)

**Situation.**
On the morning of 2026-09-18 I had three changes in flight, all wiring Grok into the harness: fleet-ops `feat/grok-health-check`, agents `feat/grok-personal-layer`, and dotfiles-nix `feat/grok-cross-tool-link`.
All three were in the no-mistakes gate with Claude as the pipeline agent.

**Task.**
Get the three changes through review, test, and CI without skipping the gate, while my primary model was unavailable.

**Action.**
- 07:47: all three runs failed at review within the same second (`agent_invocations` rows with `exit_status` error).
  The review log shows the cause: "You've hit your session limit", with a reset at 11:30am (`~/.no-mistakes/logs/01M2T5FMMF5E2Z0SRMZD3BP6MH/review.log`), the five-hour Claude window, not the weekly one.
- 08:19: I pinned Grok to its native binary and recorded that the gate "uses agent: grok with that binary while Claude quota is exhausted" (`agents` commit `5230b78`, 08:19:09).
- 08:19:35 onward: 17 gate agent invocations used Grok across the three branches (16 completed on `grok-4.6` for review, test, test-fix, housekeeping, and PR; one errored), grouped from `agent_invocations`, where Claude has 1,113 invocations in total.
- Two runs completed: fleet-ops PR #3 "feat: check xAI grok identity and AGENTS.md in fleet-doctor" and agents PR #3 "feat(grok): add a dedicated personal layer for Grok Build".
- 08:45: the third run failed at the document step with "API error (status 402 Payment Required): Grok Build usage balance exhausted"; the fallback provider ran out too.
- I did not bypass the gate; I waited for the Claude reset and re-ran at 11:33, and that run completed, producing dotfiles-nix PR #14 "feat(ic): wire Grok's own agent layer into ic-link and ic-doctor" (merged 2026-09-19).

**Result.**
- All three changes shipped through the full gate; nothing was pushed around it.
- The same evening (17:26) I committed the routing policy as code in `agents` `dcfab28`: drop candidates whose runway is exhausted or would end before the task finishes, and "Before any long run, size it against the limiting window, not the headline percentage ... a single long gate run can drain a whole week of Grok credits" (`agents/ROUTING.md:17-20`).
  The link from the incident to that wording is inferred from timing and content; the commit message does not cite the incident.
- The operational switch is now documented where it is used: "Switch to `grok` only while Claude has no runway (the coding quota fallback); restore `auto` after" (`~/.no-mistakes/config.yaml:7`).
- Grok credits were still `exhausted_now` on 2026-09-23 (quota-axi snapshot in [quota-axi](../components/quota-axi.md)), so today Claude is the only viable gate agent; that is a known single point of failure.

**What I learned.**
Headline percentage lies; the tightest window and the burn rate decide whether a task can finish.
Fallback capacity is only real if you know its runway too, and the right response to losing both is to wait, not to skip the control.

**60-second version.**
> One morning three of my changes were in my release gate when my primary model hit its five-hour limit, and all three reviews failed at once.
> I switched the gate's agent to my fallback provider; two changes completed with PRs, and then the fallback ran out of credits mid-run.
> I did not skip the gate; I waited for the reset at 11:30 and re-ran, and the third change shipped that afternoon.
> That evening I wrote the lesson into my routing policy: gate every long run on runway against the tightest window, not the headline percentage.
> In markets terms, it is checking the limit that actually binds before you route the order.

## Backup B1 - two operations incidents in the first week of the fleet sync

- First scheduled run on 2026-08-12 took 3.5 hours: launchd treated the agent as Background, "a make install crawled for 75 minutes, a pnpm build for 87", and two fetches hung about 17 minutes (`dotfiles-nix/nix/home/darwin.nix:89-94`, commit `b790d5f`, PR #11).
  Fix: `ProcessType = "Standard"` plus one fetch retry after 30 s.
- The same day at 13:55 a sync marked no-mistakes failed: upstream's `make install` restarts the gate daemon, and the daemon "rightly refuses to stop while a pipeline run is active" (`a12d58d`, fleet-ops PR #2).
  Fix: build and install the binary only, and let launchd `KeepAlive` pick up the new binary on its next natural restart.
- Spoken: "Both bugs came from treating a build job like a background chore and an install like a restart; I fixed each at the root and documented the reason in the config it changed."

## Backup B2 - review caught a parser-breaking pipe (fleet-ops PR #7)

- Adding the compact-adviser plugin, my manifest install string used `|| true`; `bootstrap.sh` splits manifest fields on `|`, so the pipe would have shifted the binaries field on a fresh machine.
- The gate's review step caught it and produced `b0b990a` "Remove pipe from compact-adviser install command for bootstrap parsing", which also wrote the constraint into the entry: "The install string must contain no '|', because bootstrap.sh splits manifest fields on it" (`.fleet/manifest.yaml:337`).
- Spoken: "A one-character config bug that would only have failed on a fresh machine rebuild, caught before merge because config changes go through the same gate as code."

## Stories I looked for but could not evidence

- A production outage prevented by the gate in a team setting: none; the harness has one user, so do not claim team impact.
- A cost number in dollars saved by routing: not measured; say "I measure runway and spendPriority, not dollars".

Related: [Pitches](Pitches.md), [Question-Bank](Question-Bank.md), [no-mistakes](../components/no-mistakes.md), [fleet-ops](../components/fleet-ops.md).
