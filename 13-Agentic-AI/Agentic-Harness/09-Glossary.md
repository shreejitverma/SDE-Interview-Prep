---
type: concept
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://github.com/kunchenguid/firstmate, https://github.com/kunchenguid/no-mistakes, https://github.com/kunchenguid/axi, https://github.com/kunchenguid/quota-axi, https://github.com/kunchenguid/treehouse, https://github.com/kunchenguid/tasks-axi, https://github.com/shreejitverma/agents, https://github.com/shreejitverma/fleet-ops, https://github.com/shreejitverma/dotfiles-nix]
---

# Glossary

Every term used across the Agentic Harness notes, with a one-to-two sentence definition and the note that covers it.
Definitions follow the source repos as read on 2026-09-23; where a term is the user's own coinage or configuration, the definition says so.

## Roles and orchestration

| Term | Definition | Covered in |
|---|---|---|
| Agentic harness | The whole personal system: agent CLIs, the tools they call, the manuals and hooks that constrain them, the gate, and the fleet that keeps it current. | [05](05-Configuration-Topology.md) |
| Harness (agent harness) | An agent CLI that runs a model with tools, such as Claude Code, Grok Build, Codex, or `agy`; firstmate selects one per crewmate with `--harness`. | [firstmate](components/firstmate.md) |
| firstmate | An upstream "agent distro": a cloned directory of `AGENTS.md`, skills, and about 190 helper scripts that turns one agent session into a supervisor of parallel workers. | [firstmate](components/firstmate.md) |
| Captain | firstmate's name for the human operator, the only party who can authorize merges and discards. | [firstmate](components/firstmate.md) |
| First mate | The primary agent session launched in `~/github/firstmate` (by `fm`); it reads projects but never writes to them, and dispatches, steers, and tears down crewmates. | [firstmate](components/firstmate.md), [06](06-Safety-and-Quality-Gates.md) |
| Crewmate | A worker agent in its own tmux window and treehouse worktree; a ship changes a project and delivers a PR or branch, a scout only writes a report. | [firstmate](components/firstmate.md) |
| Secondmate | A persistent crewmate with its own isolated `FM_HOME` and charter; it inherits the primary's dispatch and permission settings. | [firstmate](components/firstmate.md) |
| `fm` | The user's zsh alias for `firstmate()`, which `cd`s into the firstmate checkout and launches `${FM_DEFAULT_HARNESS:-claude}` with `command`, bypassing the key-injecting wrapper. | [07](07-Fleet-Operations.md), [dotfiles-nix](components/dotfiles-nix.md) |
| `FM_HOME` | Selects which home's `data/`, `state/`, `config/`, and `projects/` firstmate scripts use; scripts always come from the code root. | [firstmate](components/firstmate.md) |
| `FM_TASK_ID` | Environment variable firstmate exports into every crew pane; the guard treats it as "no human present". | [06](06-Safety-and-Quality-Gates.md) |
| Brief | `data/<id>/brief.md`, the task instructions for a crewmate, with a `Captain's intent` section passed verbatim to the gate as `--intent`. | [firstmate](components/firstmate.md) |
| Delivery mode | Per-task delivery path: `no-mistakes`, `direct-PR`, or `local-only`. | [firstmate](components/firstmate.md) |
| `yolo` posture | A captain-approved per-project relaxation that lets firstmate merge without asking each time; the only standing relaxation of merge authority. | [06](06-Safety-and-Quality-Gates.md) |
| Definition of Done | The status line a worker must end with, for example `done [at=<epoch>]: PR <url> checks green` for a no-mistakes ship. | [firstmate](components/firstmate.md) |
| Watcher | `fm-watch.sh`, a zero-token bash loop that wakes the first mate only on actionable events (signal, stale, check, heartbeat). | [firstmate](components/firstmate.md) |
| `/updatefirstmate` | firstmate skill that updates the running home and every secondmate to the latest origin through a guarded path; the second half of taking upstream updates for this `sync: false` fork. | [07](07-Fleet-Operations.md) |
| gnhf | "Good night, have fun": an upstream Node loop that runs `claude -p` repeatedly against one objective, commits each successful iteration, and rolls back failures. | [gnhf](components/gnhf.md) |
| Stop condition | A gnhf limit (iterations, tokens, rate-limit wait, or a natural-language condition) that ends an unattended run. | [gnhf](components/gnhf.md) |
| wheelhouse | A GitHub Issues and Actions decision queue on the user's fork, where each open issue is one owner decision about another fleet repo; not part of the local runtime. | [wheelhouse](components/wheelhouse.md) |
| Decision card | A wheelhouse issue with checkboxes that a workflow executes and closes after a successful resolving action. | [wheelhouse](components/wheelhouse.md) |

## Routing and quota

| Term | Definition | Covered in |
|---|---|---|
| Tier | The user's task classes: Tier 1 frontier reasoning (Fable only while Claude has runway), Tier 2 well-specified coding (Opus 5.5, Grok 4.7, Gemini 3.1 Pro as peers), Tier 3 mechanical work (Haiku, Grok 4.5, Gemini Flash). | [08](08-Design-Principles-and-Tradeoffs.md) |
| Two-step routing | The user's policy in `agents/ROUTING.md`: classify by fit first, then pick within the class by quota. | [08](08-Design-Principles-and-Tradeoffs.md) |
| `crew-dispatch.json` | The user's local, gitignored firstmate file with natural-language `when` rules, each mapping to a profile or peer array of harness, model, and effort. | [05](05-Configuration-Topology.md) |
| Typed dispatch resolution | Opt-in firstmate path, enabled by `TYPESAFE_API_KEY` in `.env`, that asks TypeSafe's Jev model which rule matches and does the quota math in `jq`. | [firstmate](components/firstmate.md) |
| `quota-array-dispatch` | firstmate skill that resolves a peer array: eligibility, reasoning-class, and runway gates, then the highest known `spendPriority`, never downgrading Tier 1. | [firstmate](components/firstmate.md) |
| quota-axi | Upstream CLI that reads quota windows for many AI subscriptions from local auth sources and reports remaining percentage, runway, and `spendPriority`; it never routes or mints credentials. | [quota-axi](components/quota-axi.md) |
| Window | One quota limit with its own reset clock, for example Claude's five-hour session window, seven-day window, and separate Fable weekly window. | [quota-axi](components/quota-axi.md) |
| Scope | A quota-axi grouping of windows that bound one kind of usage, such as Claude `all_models` or `model:fable`; its effective remaining is the minimum across its windows. | [quota-axi](components/quota-axi.md) |
| Runway | quota-axi's projection for a scope: `exhausted_now`, `projected_exhaustion` (at current burn, before reset), `through_reset`, or `unknown`. | [quota-axi](components/quota-axi.md) |
| `spendPriority` | quota-axi's selection scalar: the cycle-weighted mean of `percentRemaining / timeRemainingPercent - burnMultiple` over a scope's windows; positive means allowance is on track to expire unused. | [08](08-Design-Principles-and-Tradeoffs.md) |
| Burn multiple | Fraction of a window used divided by fraction of its cycle elapsed; above 1 means spending faster than the reset clock. | [quota-axi](components/quota-axi.md) |
| Fable | The strongest Claude model in this setup, with its own weekly window; the policy spends it only on Tier 1. | [08](08-Design-Principles-and-Tradeoffs.md) |
| Effort | Reasoning budget per request: `low`, `medium`, `high`, `xhigh`, `max`; the user's floor for intelligence-sensitive work is `high`. | [05](05-Configuration-Topology.md) |
| `agy` | Antigravity CLI, the only surface through which quota-axi can measure Gemini, so Gemini crewmates launch through it. | [quota-axi](components/quota-axi.md) |

## Agent runtime building blocks

| Term | Definition | Covered in |
|---|---|---|
| Skill | A directory with a `SKILL.md` whose frontmatter `description` is the trigger; the runtime loads the body only when the description matches the task. | [skills-catalog](components/skills-catalog.md) |
| Subagent | A Claude Code agent definition (`~/.claude/agents/*.md`) with its own tools, model, effort, and preloaded skills; the user's six are review-focused and pinned to Opus 5.5 at high effort. | [06](06-Safety-and-Quality-Gates.md) |
| Rule (path-scoped) | A Claude Code Markdown file with `paths:` globs that loads only when a matching file is read, such as `cpp.md` and `python.md`. | [agents](components/agents.md) |
| Hook | A command Claude Code runs at a lifecycle event: PreToolUse can allow, ask, or deny a tool call; PostToolUse can feed stderr back; SessionStart and Stop drive firstmate's startup and watcher. | [06](06-Safety-and-Quality-Gates.md) |
| `guard.py` | The user's PreToolUse hook (adapted from ECC, MIT) that always denies hook bypass, shared-branch force-push, and critical `rm`, and asks or allows other destructive commands depending on presence. | [06](06-Safety-and-Quality-Gates.md) |
| `post_edit.py` | The user's PostToolUse hook that runs the project's own ruff or clang-format checks after an edit and shows findings to the model. | [06](06-Safety-and-Quality-Gates.md) |
| Human present (attended) | The guard's test: `FM_TASK_ID` unset and `CLAUDE_CODE_SESSION_ATTENDED=1`; unattended runs auto-allow ask-class commands and deny protected config edits. | [06](06-Safety-and-Quality-Gates.md) |
| Bypass permission mode | Launching Claude with `--dangerously-skip-permissions`; the default for firstmate crewmates and for no-mistakes gate agents. | [06](06-Safety-and-Quality-Gates.md) |
| MCP | Model Context Protocol: a server exposes tools whose schemas load into the model's context; the user runs no user-level MCP servers and prefers local CLIs. | [08](08-Design-Principles-and-Tradeoffs.md) |
| Plugin and marketplace | Claude Code extension bundles, installed from a marketplace; the user enables `vercel` and `compact-adviser` (from a local directory marketplace). | [claude-code-config](components/claude-code-config.md) |
| compact-adviser | Upstream plugin that tells the user when a session is at a safe point to `/compact`, judged by TypeSafe's Jev model; disabled for every firstmate crewmate. | [compact-adviser](components/compact-adviser.md) |
| Untrusted content | firstmate's label for project files, fetched pages, issue and PR text, and tool output: data to read, never instructions to follow. | [06](06-Safety-and-Quality-Gates.md) |

## Tools and interfaces

| Term | Definition | Covered in |
|---|---|---|
| AXI | Agent eXperience Interface: upstream's ten principles for CLIs whose primary user is an LLM, from token-efficient output to structured errors and content-first entry points. | [axi](components/axi.md), [08](08-Design-Principles-and-Tradeoffs.md) |
| TOON | Token-Oriented Object Notation, the compact tabular text format AXI CLIs print instead of JSON, cited upstream as about 40% fewer tokens. | [axi](components/axi.md) |
| `axi-sdk-js` | Upstream Node runtime (`runAxiCli`) that implements the AXI contract once: dispatch, TOON output, and exit codes 0, 1, and 2. | [axi](components/axi.md) |
| gh-axi | Upstream agent-shaped wrapper around `gh` for issues, PRs, CI runs, and releases; it defaults to the `origin` remote, so fork checkouts need `--repo`. | [gh-axi](components/gh-axi.md) |
| chrome-devtools-axi | Upstream browser CLI in front of a detached local bridge that holds one MCP session to `chrome-devtools-mcp`, used for real-browser verification. | [chrome-devtools-axi](components/chrome-devtools-axi.md) |
| `STALE_REF` | chrome-devtools-axi's error when an element reference is from an older page generation, instead of silently clicking the wrong element. | [chrome-devtools-axi](components/chrome-devtools-axi.md) |
| lavish-axi | Upstream CLI and local server that turn an agent-written HTML file into a review surface the human can annotate; `share` publishes to a third-party host, public by default. | [lavish-axi](components/lavish-axi.md) |
| Artifact | In this harness, a rich HTML page an agent writes for human review (lavish), or evidence the no-mistakes test step collects. | [lavish-axi](components/lavish-axi.md), [06](06-Safety-and-Quality-Gates.md) |
| tasks-axi | Upstream CLI that makes small, locked, atomic edits to a Markdown `backlog.md`, with dependencies, holds, a ready queue, and PR links on completion. | [tasks-axi](components/tasks-axi.md) |
| Backlog | The Markdown task queue (`In flight`, `Queued`, `Done`) that tasks-axi edits; firstmate keeps one per home. | [tasks-axi](components/tasks-axi.md) |
| Hold | A structured tasks-axi block on a task (kinds captain, external, load, parked, future) that removes it from the ready queue. | [tasks-axi](components/tasks-axi.md) |
| Ready | A derived tasks-axi state: queued, not blocked by an unfinished task, and not held. | [tasks-axi](components/tasks-axi.md) |
| stow | Skill (from firstmate) that sweeps a session for durable knowledge and files it in tiered, decaying notes, never secrets. | [skills-catalog](components/skills-catalog.md) |
| ship | The user's own skill encoding the direct-work loop: quota check, backlog item, worktree, implement and verify, no-mistakes gate, record the PR, stow. | [skills-catalog](components/skills-catalog.md) |
| ECC | `affaan-m/ecc`, an MIT-licensed source the user adapted for the guard, the post-edit hook, and eleven engineering skills, with attribution. | [skills-catalog](components/skills-catalog.md) |

## Isolation and shipping

| Term | Definition | Covered in |
|---|---|---|
| Worktree | A second working directory attached to the same git repository, so parallel tasks do not share files or branches. | [treehouse](components/treehouse.md) |
| treehouse | Upstream Go CLI that keeps a per-repo pool of reusable worktrees and hands one out at detached HEAD with `treehouse get`, reclaiming it with `treehouse return`. | [treehouse](components/treehouse.md) |
| Lease | A durable treehouse claim on a slot that survives with no process inside; firstmate uses it for secondmate homes. | [treehouse](components/treehouse.md) |
| Gate | In no-mistakes, the local bare repository at `~/.no-mistakes/repos/<id>.git` that a branch is pushed to; more generally, any check a change must pass before publication. | [no-mistakes](components/no-mistakes.md), [06](06-Safety-and-Quality-Gates.md) |
| no-mistakes | Upstream Go tool and daemon that validates a pushed branch in a disposable worktree through a fixed pipeline and forwards only a green branch to the real remote and a PR. | [no-mistakes](components/no-mistakes.md) |
| Pipeline steps | The fixed no-mistakes order: intent, rebase, review, test, document, lint, push, pr, ci. | [06](06-Safety-and-Quality-Gates.md) |
| Intent | The author's goal for a change, passed with `--intent` or inferred from local agent transcripts, and fed to the review, test, and PR agents. | [no-mistakes](components/no-mistakes.md) |
| Auto-fix | A no-mistakes step fixing its own findings up to a budget; the user allows 3 for mechanical steps and 0 for review, so review findings always wait for a decision. | [06](06-Safety-and-Quality-Gates.md) |
| Attestation | A `no-mistakes-pipeline-attestation:v1` comment in the PR body naming the steps run against the head SHA; a declaration, not a cryptographic signature. | [06](06-Safety-and-Quality-Gates.md) |
| `require-no-mistakes` | Upstream composite GitHub Action that checks a PR body for the signature and a current attestation; a contributor guardrail, not a forgery-proof boundary. | [06](06-Safety-and-Quality-Gates.md) |
| Trusted default-branch config | no-mistakes reads gate controls in `.no-mistakes.yaml` only from the default branch, so a feature branch cannot weaken its own gate. | [05](05-Configuration-Topology.md), [06](06-Safety-and-Quality-Gates.md) |
| Bare push | A plain `git push` to a real remote outside the no-mistakes pipeline; forbidden by the user's manuals but not blocked by any remote control. | [06](06-Safety-and-Quality-Gates.md) |
| Pwn request | An attack where a fork PR changes CI workflows to run with the base repo's secrets; wheelhouse holds such CI approvals for manual review. | [06](06-Safety-and-Quality-Gates.md) |

## Configuration and fleet

| Term | Definition | Covered in |
|---|---|---|
| `agents` repo | The user's own repo holding the manual sources, the generator, Claude settings, subagents, rules, and hooks. | [agents](components/agents.md), [05](05-Configuration-Topology.md) |
| `CORE.md`, `ROUTING.md`, tuning file | The shared rule source, the shared routing policy, and the per-tool additive file (`tools/<tool>.md`) that `build-manuals` compiles. | [05](05-Configuration-Topology.md) |
| Generated manual | `CLAUDE.md`, `GROK.md`, `GEMINI.md`, or `AGENTS.md` rendered by `bin/build-manuals`; never hand-edited, byte-checked in CI. | [05](05-Configuration-Topology.md) |
| `OPINIONS.md`, `VOICE.md` | Personal reference files in the `agents` repo that agents read on demand, linked into the home directory. | [agents](components/agents.md) |
| Symlink farm | The set of links `ic-link` creates from `~/.claude`, `~/.grok`, `~/.gemini`, `~/.codex`, `~/.agents/skills`, and `~` into git checkouts. | [05](05-Configuration-Topology.md) |
| `ic-link` | The user's idempotent script in dotfiles-nix that creates or repairs every harness symlink and refuses to point one tool at another tool's manual. | [05](05-Configuration-Topology.md) |
| `ic-doctor` | The user's read-only seven-section toolchain health check; exits 1 on any FAIL. | [07](07-Fleet-Operations.md) |
| Fleet | The 22 GitHub forks under `~/github` declared in the fleet-ops manifest; unrelated to firstmate's internal use of the word for its crew. | [07](07-Fleet-Operations.md) |
| Manifest | `~/github/.fleet/manifest.yaml`, the user's single inventory of fleet repos with upstream, branch, install command, binaries, aliases, and `sync` flag. | [07](07-Fleet-Operations.md), [fleet-ops](components/fleet-ops.md) |
| `fleet-doctor` | Alias for `.fleet/doctor.sh`, the manifest-driven read-only audit of clones, identity, binaries, aliases, sync status, Grok, and CLI smoke tests. | [07](07-Fleet-Operations.md) |
| `sync-forks` | The user's daily launchd job that fast-forwards every `sync: true` fork from upstream, reinstalls it, and pushes the fork, never merging or forcing. | [07](07-Fleet-Operations.md) |
| Fast-forward-only (ff-only) | Updating a branch only when it is a strict ancestor of the new tip, so no merge commit or rewrite ever happens. | [07](07-Fleet-Operations.md) |
| Diverged, ahead-only | `sync-forks` outcomes: behind and ahead means diverged and left untouched; ahead but not behind means unpushed local work that is never published automatically. | [07](07-Fleet-Operations.md) |
| `sync: false` | Manifest flag for forks that carry their own commits (dotfiles-nix, firstmate); they take upstream through reviewed merge PRs instead. | [07](07-Fleet-Operations.md) |
| Nix flake | A pinned, declarative Nix project; dotfiles-nix uses one to declare the Mac with nix-darwin and the user environment with Home Manager. | [dotfiles-nix](components/dotfiles-nix.md) |
| `mkOutOfStoreSymlink` | Home Manager function that links to a live checkout instead of an immutable store copy, trading immutability for edit-without-rebuild. | [dotfiles-nix](components/dotfiles-nix.md) |
| launchd agent | A macOS per-user scheduled job; `org.nix-community.home.sync-forks` runs `sync-forks` at 10:00 with `RunAtLoad`. | [07](07-Fleet-Operations.md) |
| Keychain wrapper | The user's zsh functions that inject the one harness API key from the macOS Keychain into `claude` and `grok` per invocation, never globally. | [06](06-Safety-and-Quality-Gates.md) |

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
- [08 Design principles and trade-offs](08-Design-Principles-and-Tradeoffs.md)
