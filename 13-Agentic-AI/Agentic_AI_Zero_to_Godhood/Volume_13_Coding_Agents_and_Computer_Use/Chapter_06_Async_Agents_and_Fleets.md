# Chapter 06 - Async Agents and Fleets

## What you will master

- The shift from interactive pairing to delegation, and the specific capability thresholds that made it viable.
- Background and cloud execution models: Codex cloud, Claude Code on the web and in GitHub Actions, Devin-style hosted engineers, and Cursor background agents.
- Parallel exploration with git worktrees: the mechanics, the coordination rules, and when parallelism is waste.
- CI-triggered agents: the patterns that work (issue-to-PR, review, flaky-test triage) and the ones that reliably annoy people.
- Review gates for agent-written code: the verification pyramid, what humans should actually review, and how to keep review from becoming the bottleneck.
- Fleet management: task queues, concurrency limits, cost controls, observability, and failure isolation.
- The agent-manager role: the concrete skills that replace typing as the scarce input.

Date-stamp: product surfaces described here are as of early 2026 and move quickly; the operating patterns are the durable content.

## 1. Why delegation became possible

Interactive pairing - human watching, agent acting, human correcting every minute - was the correct 2024 posture because agent coherence measured in minutes.
Delegation requires a different property: the agent must stay on task, unsupervised, for long enough that the round trip of assigning and reviewing is cheaper than doing the work.

Three thresholds had to be crossed.

**Coherence half-life.**
An agent must sustain work longer than the human's attention cost of monitoring it.
METR's time-horizon measurements - the length of task an agent completes at 50 percent reliability - showed roughly a doubling every seven months through 2024-2025, moving from minutes to hours of human-equivalent task length.
Once the number exceeded the ten-to-thirty-minute range, watching became more expensive than reviewing.

**Verification at the boundary.**
Delegation is only safe if the output can be checked without redoing it.
Software already had the instrument: the pull request, backed by CI.
The delegation pattern that works is not "agent edits my working tree while I look away" but "agent produces a reviewable artifact that CI has already judged."

**Environment isolation.**
Unsupervised agents need somewhere to run that is not your laptop mid-flight.
Containers, cloud sandboxes, and worktrees provided it.

When all three held, the economics inverted.
In the pairing regime, the human is the bottleneck and one agent saturates them.
In the delegation regime, the bottleneck moves to task specification and review, and one human can keep several agents busy.
That single sentence is the whole chapter.

## 2. Execution surfaces

Five surfaces, ordered by increasing distance from the developer's machine.

**Local background agents.**
The agent runs on your machine, in a separate worktree or container, while you do something else.
Cursor's background agents and any `claude -p` invocation launched in a terminal tab are examples.
Advantages: your environment, your credentials, no infrastructure, instant setup.
Disadvantages: competes for local resources, dies with your laptop lid, and cannot be triggered by external events.

**Cloud-hosted agent runs.**
The task runs in a provider-managed container: Codex cloud (OpenAI, from May 2025), Claude Code on the web and in the desktop and mobile clients (2025), Devin's hosted workspaces (2024 onward), and Cursor's cloud-backed agents.
The interaction is fire-and-forget: describe the task, close the tab, come back to a diff or a PR.
Advantages: parallelism limited only by budget, no local resource contention, works from a phone, and a uniform environment that removes "works on my machine."
Disadvantages: environment setup must be codified (a container image or setup script), repository and credential access must be granted deliberately, and network-restricted sandboxes surprise agents that expect to install packages.

**Repository-event-triggered agents.**
The agent runs inside CI in response to a repository event: Claude Code's GitHub Action responding to an `@claude` mention on an issue or PR, GitHub Copilot's coding agent assigned to an issue, or a bespoke workflow.
Advantages: zero new interface - the team's existing issue and PR flow is the control plane - and full auditability through the Actions log.
Disadvantages: CI minutes and API costs are now triggered by anyone who can comment, which is a permission and budget surface most teams under-think on day one.

**Scheduled and monitoring agents.**
Cron-triggered runs: nightly dependency upgrades, weekly flaky-test triage, dashboard-driven investigations.
This is where hosted scheduling (Anthropic's Managed Agents deployments and equivalents) or plain CI schedules apply.
Advantages: catches slow-accumulating work that nobody prioritizes.
Disadvantages: unattended runs on a schedule are the easiest way to generate noise nobody reads; every scheduled agent needs an owner and a kill switch.

**Autonomous fleets.**
Many agents working from a queue on decomposed work, with automated verification and human review only at the gate.
This is the frontier as of early 2026 and section 7 is about operating it.

The architectural point tying these together: they are the *same agent* behind different triggers and environments.
The Claude Agent SDK exists precisely to make the harness embeddable, so the terminal, the web, the GitHub Action, and your own queue worker all drive identical logic.
Design your own automation the same way - one agent entry point, many triggers - or you will maintain five divergent behaviors.

## 3. Parallel exploration with git worktrees

Git worktrees let one repository have multiple working directories on different branches simultaneously, sharing one object database.

```bash
git worktree add ../proj-auth-fix -b agent/auth-fix
git worktree add ../proj-perf     -b agent/perf
git worktree list
git worktree remove ../proj-auth-fix
```

Each worktree is a full checkout an agent can own exclusively: it can edit, build, run tests, and commit without any other agent seeing intermediate state.
Compared to the alternatives, worktrees hit a sweet spot - branches alone force serialization because one directory holds one checkout; full repository clones duplicate history and lose shared objects; containers give stronger isolation but cost setup time and complicate access to local toolchains.

Two genuinely different uses:

**Parallel decomposition** - several independent tasks, one worktree each.
The rule that makes it work: tasks must not touch overlapping files.
Overlap converts parallelism into merge conflicts, and resolving agent-generated conflicts across three branches costs more than doing the work serially.
Decompose by module boundary, not by convenience.

**Parallel attempts** - the same task, several worktrees, different approaches or different models, then pick the winner.
This is test-time compute at the workflow level, and it is rational exactly when verification is cheap and strong relative to generation.
If a robust test suite decides the winner, running three attempts and keeping the best is a good trade at 3x token cost.
If the winner must be chosen by human judgment, you have tripled your review burden, which is usually the actual bottleneck - so do not.

Operational hygiene, learned the hard way by everyone who tries this:

- Nail down per-worktree isolation of everything stateful: node_modules and virtualenvs (each worktree needs its own), ports (assign explicitly or agents fight over 3000), database and cache names, and any global config the tests write.
- Give each worktree a distinct branch name that encodes the task, so a stale worktree is identifiable a week later.
- Set a concurrency ceiling based on machine resources, not enthusiasm; four agents each running a test suite will saturate a laptop.
- Clean up: `git worktree prune` and explicit removal, or you accumulate dozens of stale directories and branches.
- Do not exceed the number of results you can actually review.

That last rule is the one that separates people who benefit from parallelism from people who generate a review backlog.

## 4. CI-triggered agents

Patterns that work, roughly in order of value delivered per unit of trouble:

**Automated code review.**
An agent reviews every PR against the repository's conventions and posts findings as comments.
This works because review is read-only, findings are cheap to ignore, and the agent has genuine advantages - it never gets tired, it reads the whole diff, and it knows the project's CLAUDE.md conventions.
The failure mode is volume: an agent that comments on every nit trains reviewers to ignore it.
Tune for precision over recall in the posting step, even though Chapter 3's lesson about severity filters says to have the model *find* everything and filter downstream.

**Issue to draft PR.**
A labeled or mentioned issue triggers an agent that produces a draft PR.
This works when issues are well specified and the change is localized.
It fails on ambiguous issues, and the honest measurement is not "PRs opened" but "PRs merged without a human rewriting them."
Track that ratio or you are measuring activity, not value.

**Flaky-test triage.**
On CI failure, an agent reruns, bisects, classifies the failure as flake or real, and either files an issue or annotates the run.
High value, low risk, and it targets exactly the work humans procrastinate on.

**Dependency and migration sweeps.**
Scheduled agents that upgrade a dependency, fix the resulting breakage, and open a PR with the test run attached.
The verification is strong (the tests either pass or they do not), so this is one of the best fits for full automation.

**Failure investigation.**
On a production alert or CI failure, an agent gathers logs, correlates recent commits, and posts a first-pass hypothesis before a human opens the laptop.
Value is in time-to-context, not in the fix.

Patterns that reliably annoy people:

- Agents that auto-merge without human approval, in any repository anyone cares about.
- Agents that respond to every comment, turning a PR thread into a conversation with a machine.
- Agents that open PRs nobody asked for.
- Agents whose failures are silent, so the team slowly learns the automation is unreliable and stops looking at it.

Three controls belong on every CI-triggered agent from day one: **who can trigger it** (mention-based triggers are a permission surface - restrict to collaborators or the automation will be used to burn your API budget), **a per-run and per-day cost ceiling**, and **an owner** who receives failures.

## 5. Review gates

The delegation bargain is that a human reviews the output.
The engineering question is how to keep that review cheap enough to stay worth it.

The answer is a **verification pyramid**: layer checks from cheapest and most automatic to most expensive and most human, and never spend a human on something a machine could have caught.

1. **Mechanical**: formatter, linter, type checker, build.
Free, instant, and non-negotiable - a PR that fails these should never reach a human.
2. **Tests**: the existing suite, plus tests for the new behavior.
The single most important gate; require that the agent's change comes with a test that fails before it and passes after.
3. **Coverage and change-shape heuristics**: did the diff touch files it had no business touching, did it delete tests, did it grow by ten times the estimate.
These catch the reward-hacking behaviors from Chapter 1 - deleted assertions, weakened tests, hard-coded returns - and they are cheap to automate.
4. **Automated review agent**: a second agent, ideally with a different model, reviewing the diff for bugs and convention violations.
Independent context is what makes this useful; a reviewer that shares the author's context shares its blind spots.
5. **Security scanning**: dependency changes, new network calls, new file writes, credential-shaped strings, and permission changes deserve automated flags.
6. **Human review**: everything above has already run, so the human is spending attention on the questions only a human answers.

What humans should actually review, given the pyramid did its job:

- **Is this the right change?** Tests confirm the code does what it does; only a human confirms it is what should have been built.
- **Architecture and blast radius.** Does this fit the system, and what breaks if it is wrong.
- **The parts the tests do not cover.** Error paths, concurrency, edge cases, and anything with real-world side effects.
- **Security-sensitive surfaces.** Auth, input handling, secrets, permissions - always human-reviewed regardless of test results.
- **Deletions and test changes.** Anything that removes a check is a higher-scrutiny change than anything that adds code.

Practices that keep review tractable:

- **Small diffs.** Instruct agents to keep changes focused and split unrelated work; a 2,000-line agent PR gets rubber-stamped, which is worse than no review.
- **Agent-authored PR descriptions that explain intent and list what was verified**, including the commands run.
The description is the agent's argument for its own work, and a weak argument is itself a signal.
- **Transcript links.** Reviewers who can see how the agent reached the change catch reasoning errors that the diff hides.
- **A stated confidence and open-questions section**, which is remarkably effective at directing reviewer attention.

The failure mode to fear is **review capitulation**: agents produce more than humans can meaningfully read, reviewers start approving on vibes, and the verification layer that justified delegation quietly stops existing.
The defense is to treat review capacity as the binding constraint on fleet size, and to keep pushing checks down the pyramid so human attention stays scarce and well spent.

## 6. Managing a fleet

Once several agents run concurrently, you have a distributed system, and the usual disciplines apply.

**Task queue.**
Work items with a specification, an owner, a priority, a state (queued, running, needs-review, done, failed), and a result pointer.
It can be a GitHub Projects board, a backlog file, or a real queue; what matters is that the state is explicit and one item is owned by one agent at a time.
Task specification quality dominates everything - an under-specified item is how you get three hours of confident irrelevance.
A good spec states the goal, the acceptance criteria, the files or modules in scope, and what not to touch.

**Concurrency and resource limits.**
Cap concurrent agents by whichever binds first: API rate limits, CI runner capacity, local CPU and memory, or - almost always in practice - human review throughput.

**Cost controls.**
Per-task token or dollar budgets that terminate a runaway, per-day org budgets, and alerting on outliers.
Task budgets (a declared ceiling the model is aware of so it paces itself and wraps up gracefully) are strictly better than hard truncation, which leaves work half-finished.
Track cost per merged PR, not cost per run; the former is the number that tells you whether the fleet is economic.

**Observability.**
Per-run: full transcript, tool calls, token usage, wall-clock, and outcome.
Aggregate: success rate by task type, review-rejection rate, cost per merged change, mean time to review.
Volume 10 covers agent observability properly; the fleet-specific addition is that you need a *comparison* view - which task types succeed, which models are winning, where cost concentrates.

**Failure isolation.**
One agent's failure must not corrupt shared state.
Separate worktrees or containers, branches rather than direct pushes to main, and no shared mutable resources (databases, ports, caches) without explicit per-agent namespacing.
Assume any given run may end in a broken intermediate state and design so that abandoning it is free.

**Idempotency and resumption.**
Long runs die - network blips, rate limits, restarts.
Design tasks so re-running is safe, and prefer checkpointing (commits, plan files, notes on disk) over hoping a single process survives.

**Kill switches.**
A documented way to stop everything, and a per-agent stop.
Anyone on the team should be able to hit it without asking permission.

## 7. The agent-manager role

When typing stops being the bottleneck, the scarce skills change.
The engineer who gets the most out of a fleet is doing five things well.

**Task decomposition.**
Cutting work into pieces that are independently specifiable, independently verifiable, and non-overlapping.
This is the highest-leverage skill and it is the same skill that makes a good tech lead; it just now applies at a finer granularity and higher frequency.

**Specification.**
Writing a task description with enough context, acceptance criteria, and scope boundaries that an agent with no tribal knowledge can execute it.
The compounding version of this skill is investing in CLAUDE.md, skills, and documentation so that context is reusable rather than re-typed per task.

**Verification design.**
Deciding what evidence would prove the task is done, and building it - tests, assertions, checks - often *before* dispatching the agent.
An agent-manager who writes the acceptance test first gets dramatically better results than one who reviews prose afterward.

**Review triage.**
Knowing which of five incoming PRs deserves ten minutes and which deserves thirty seconds, and being disciplined about not reading what the pyramid already checked.

**Portfolio management.**
Deciding how many agents to run, on what, and when to stop.
Recognizing sunk cost - an agent three hours into the wrong approach should be killed and re-specified, not nursed.

Two honest caveats to close on.

First, this role is not universally available or universally desirable.
It suits work that decomposes cleanly and verifies cheaply, and it degrades badly on exploratory, research-shaped, or deeply interconnected work where the specification *is* the hard part and cannot be written in advance.
Plenty of high-value engineering is that shape, and for it, interactive pairing remains correct.

Second, the skill atrophy concern is real: an engineer who only reviews stops building the model of the codebase that makes their review good.
The mitigation practiced by teams that have run fleets for a while is deliberate: keep doing some work by hand, especially in unfamiliar or critical areas, and treat deep familiarity with the system as a maintained asset rather than an accident of past labor.

## Exercises

1. Set up three git worktrees for three genuinely independent tasks in a repository you own, run agents in parallel, and record: wall-clock versus your serial estimate, merge conflicts encountered, and total review time; state the concurrency level your review throughput actually supports.
2. Run the same task in three worktrees with three different prompts or models, then write the automated selection criterion you would use to pick a winner without reading all three; if you cannot write one, explain what that implies about parallel attempts for this task type.
3. Implement a repository-event-triggered review agent (GitHub Action or equivalent) with a per-run cost ceiling and a collaborator-only trigger; run it on ten historical PRs and measure precision of its comments against what reviewers actually flagged.
4. Build the change-shape heuristics from the verification pyramid's third layer: a script that fails a PR if it deletes tests, weakens assertions, or touches files outside a declared scope; test it against three deliberately reward-hacking diffs you write yourself.
5. Instrument a week of agent runs and produce the fleet dashboard: success rate by task type, cost per merged PR, review-rejection rate, and mean time from dispatch to merge; identify the one metric that would most change your operating decisions.
6. Write task specifications for five items from your backlog at three levels of detail (one line, one paragraph, full spec with acceptance criteria and scope), dispatch each level to an agent, and quantify the relationship between specification effort and rework.
7. Design and document a kill-switch and incident procedure for an agent fleet with repository write access: what stops runs, what revokes credentials, how you determine blast radius, and how you decide whether to revert.

## Godhood check

You have mastered this chapter when you can:

- Name the three thresholds that made delegation viable and explain why the bottleneck moves from typing to specification and review.
- Place any agent execution surface - local background, cloud, event-triggered, scheduled, fleet - on the distance axis and state its characteristic advantage and its characteristic failure.
- Set up parallel worktrees correctly from memory, including the per-worktree isolation checklist, and state the file-overlap rule and the review-capacity rule.
- Distinguish parallel decomposition from parallel attempts and give the verification-strength condition under which each is rational.
- Reproduce the six-layer verification pyramid, say what humans should review after it runs, and explain review capitulation and its defense.
- Enumerate the seven fleet-management concerns and design controls for each on a concrete team.
- Describe the five agent-manager skills and argue honestly about where the role does not apply and how skill atrophy is mitigated.
