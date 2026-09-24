---
type: concept
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: ["https://github.com/shreejitverma/agents", "https://github.com/shreejitverma/fleet-ops", "https://github.com/shreejitverma/dotfiles-nix", "https://github.com/kunchenguid/firstmate", "https://github.com/kunchenguid/no-mistakes", "https://github.com/kunchenguid/treehouse", "https://github.com/kunchenguid/gnhf", "https://github.com/kunchenguid/tasks-axi", "https://github.com/kunchenguid/quota-axi", "https://github.com/kunchenguid/gh-axi", "https://github.com/kunchenguid/chrome-devtools-axi", "https://github.com/kunchenguid/lavish-axi", "https://github.com/kunchenguid/axi", "https://github.com/kunchenguid/compact-adviser", "https://github.com/ImZoomBoy/wheelhouse"]
---

# Agentic Harness - architecture and diagrams

Six diagrams, each followed by a short explanation, from widest to narrowest, ending with a whiteboard version to draw from memory.
Citations use `repo/path:line` relative to `~/github`; see [00-Executive-Summary](00-Executive-Summary.md) for context and [components/](components/) for depth.

## 1. System context

```mermaid
flowchart LR
  U["Captain (the human, me)"]
  subgraph H["Agentic Harness (one Mac)"]
    FM["First mate: Claude Code in ~/github/firstmate"]
    CREW["Crewmates in worktrees"]
    GATE["no-mistakes local gate"]
    CFG["Config plane: agents manuals, dotfiles-nix, fleet-ops"]
  end
  CC["Claude Code Max (Anthropic)"]
  GK["Grok Build (xAI)"]
  AG["Gemini via agy (Google Antigravity)"]
  GH["GitHub: forks, PRs, Actions"]
  TS["TypeSafe API (Jev classifier)"]
  UP["Upstream repos (kunchenguid and others)"]
  U -->|chat| FM
  FM -->|spawn, steer| CREW
  CREW -->|model calls| CC
  CREW -->|model calls| GK
  CREW -->|model calls| AG
  FM -->|quota reads via quota-axi| CC
  FM -->|quota reads via quota-axi| GK
  FM -->|quota reads via quota-axi| AG
  CREW -->|git push no-mistakes| GATE
  GATE -->|verified push, PR, CI polling via gh| GH
  CFG -->|daily ff-only sync| GH
  GH -->|upstream fetch| UP
  FM -.->|optional typed dispatch| TS
  CC -.->|compact-adviser hints| TS
```

The harness is a box on one laptop with three paid model subscriptions and GitHub as its only publication target.
The human talks only to the first mate; crewmates never address the human (`firstmate/AGENTS.md:38-40`).
Every edge that leaves the box toward GitHub goes through `no-mistakes` or the fork-sync job, which is the property that makes the rest of the design safe to automate.
The TypeSafe edges are dotted because both are optional: typed dispatch runs only when `TYPESAFE_API_KEY` is set (`firstmate/bin/fm-dispatch-resolve.sh:8-12`), and compact-adviser only in interactive sessions with its key injected (`dotfiles-nix/files/zsh/ic-workflow.zsh:538-550`).

## 2. Container view

```mermaid
flowchart TB
  subgraph CP["Control plane"]
    FMS["firstmate scripts: fm-spawn, fm-watch, fm-teardown, fm-dispatch-resolve"]
    TA["tasks-axi: data/backlog.md"]
    QA["quota-axi: windows, runway, spendPriority"]
    CD["config/crew-dispatch.json (local, gitignored)"]
    QAD["quota-array-dispatch skill"]
  end
  subgraph EX["Execution"]
    TH["treehouse: ~/.treehouse pools"]
    WK["Worker agent: claude, grok, or agy"]
    GN["gnhf overnight loop"]
    CA["compact-adviser plugin"]
  end
  subgraph TL["Agent tooling"]
    SDK["axi-sdk-js runAxiCli"]
    GHA["gh-axi"]
    CDA["chrome-devtools-axi"]
    LAV["lavish-axi"]
    SK["~/.agents/skills (31)"]
    SUB["6 Claude subagents"]
  end
  subgraph GT["Gate"]
    GRD["guard.py and post_edit.py hooks"]
    NM["no-mistakes daemon and bare gate repo"]
  end
  subgraph MC["Machine config"]
    AGM["agents: CORE.md, ROUTING.md, build-manuals"]
    DN["dotfiles-nix: flake, ic-link, ic-doctor, sync-forks"]
    FO["fleet-ops: manifest.yaml, doctor.sh"]
  end
  FMS -->|"fm-tasks-axi.sh add, start, done"| TA
  FMS -->|"one TOON read per intake"| QA
  CD -->|rules and peer arrays| FMS
  QAD -->|gates then max spendPriority| FMS
  FMS -->|"types 'treehouse get' into tmux"| TH
  FMS -->|"launch with FM_TASK_ID, COMPACT_ADVISER_DISABLE=1"| WK
  WK -->|"PreToolUse and PostToolUse"| GRD
  WK -->|"no-mistakes axi run --intent"| NM
  WK -->|"GitHub ops, e.g. gh-axi pr ready"| GHA
  FMS -->|"fallback gh-axi pr view"| GHA
  WK -->|browser verification| CDA
  WK -.->|optional review board| LAV
  WK -.->|second opinion| SUB
  NM -->|"claude -p, user settings load"| GRD
  GHA --> SDK
  CDA --> SDK
  LAV --> SDK
  TA --> SDK
  QA --> SDK
  GN -->|"claude -p per iteration"| WK
  CA -.->|interactive sessions only| WK
  AGM -->|generated manuals| WK
  DN -->|symlinks via ic-link| SK
  DN -->|symlinks via ic-link| AGM
  FO -->|manifest read by sync-forks| DN
```

Each box is one repo or one file, and each edge is a real call or file contract.
The control plane never edits projects; its only write paths are the backlog, its own `state/` files, and tmux (`firstmate/AGENTS.md:27-31`).
All five Node CLIs share `runAxiCli` from `axi-sdk-js`, which is why they share one exit-code contract: 0 success, 2 validation error, 1 otherwise ([axi](components/axi.md)).
The gate is two things at two scopes: per-tool-call hooks (`guard.py` before, `post_edit.py` after) and the per-branch pipeline (`no-mistakes`).
The machine-config layer has no runtime role; it produces the files everything else reads, which is why a broken link there fails quietly and `ic-doctor` exists ([dotfiles-nix](components/dotfiles-nix.md)).

## 3. Sequence: one task end to end

```mermaid
sequenceDiagram
  autonumber
  participant C as Captain
  participant FM as First mate
  participant TA as tasks-axi
  participant Q as quota-axi
  participant TH as treehouse
  participant W as Crewmate
  participant G as guard.py
  participant NM as no-mistakes
  participant GH as GitHub
  C->>FM: fix the flaky login test in project X
  FM->>TA: fm-tasks-axi.sh add id (Queued)
  FM->>FM: fm-brief.sh writes data/id/brief.md
  FM->>FM: fm-dispatch-resolve.sh (off, clear, or fallback)
  FM->>Q: quota-axi default TOON, once
  FM->>FM: gates then highest spendPriority
  FM->>TH: fm-spawn.sh types treehouse get
  TH-->>FM: isolated detached-HEAD slot
  FM->>W: launch harness, model, effort
  FM->>TA: tasks-axi start (In flight)
  W->>G: every Bash and edit call
  G-->>W: allow, ask, or deny
  W->>NM: no-mistakes axi run --intent
  NM->>NM: intent, rebase, review, test, document, lint
  NM->>GH: push verified SHA, open PR
  NM->>GH: poll checks until green
  W->>FM: status line done PR url checks green
  FM->>C: PR URL, risk, ask to merge
  C->>FM: merge it
  FM->>GH: fm-pr-merge.sh after live green re-check
  FM->>TH: fm-teardown.sh, treehouse return --force
  FM->>TA: tasks-axi done --pr url
```

This is the happy path for a `no-mistakes` ship with `yolo` off; [03-End-to-End-Lifecycle](03-End-to-End-Lifecycle.md) walks each numbered arrow with file and line citations.
Two details matter in an interview.
The backlog row moves to In flight only after the launch succeeds (`firstmate/bin/fm-spawn.sh:4937`), and teardown pairs worktree removal with `tasks-axi done` under one lock with a crash-replay marker (`firstmate/bin/fm-backlog-transition-lib.sh:7-20`).
The merge needs the human's word unless the project is `+yolo` (`firstmate/AGENTS.md:32-33`, `:357-358`).

## 4. Config generation flow

```mermaid
flowchart LR
  subgraph AGR["~/github/agents (my repo)"]
    CORE["CORE.md"]
    ROUT["ROUTING.md"]
    TC["tools/claude.md"]
    TG["tools/grok.md"]
    TM["tools/gemini.md"]
    BM["bin/build-manuals (--check in CI and ic-doctor)"]
    OUT1["CLAUDE.md"]
    OUT2["GROK.md"]
    OUT3["GEMINI.md"]
    OUT4["AGENTS.md (no tuning)"]
    SET["claude/settings.json, agents/, rules/, hooks/"]
  end
  CORE --> BM
  ROUT --> BM
  TC --> BM
  TG --> BM
  TM --> BM
  BM --> OUT1
  BM --> OUT2
  BM --> OUT3
  BM --> OUT4
  subgraph DNR["~/github/dotfiles-nix (my fork)"]
    IL["files/bin/ic-link"]
    OWN["files/skills: ship plus 11 ECC-derived"]
    ZSH["files/zsh/ic-workflow.zsh"]
  end
  subgraph FORKS["Tool forks"]
    FS["repo/skills/NAME"]
  end
  IL -->|symlink| H1["~/.claude/CLAUDE.md"]
  OUT1 --> H1
  IL -->|symlink| H2["~/.grok/AGENTS.md"]
  OUT2 --> H2
  IL -->|symlink| H3["~/.gemini/AGENTS.md"]
  OUT3 --> H3
  IL -->|symlink| H4["~/AGENTS.md and ~/.codex/AGENTS.md"]
  OUT4 --> H4
  IL -->|symlink| H5["~/.claude/settings.json, agents, rules"]
  SET --> H5
  FS --> AS["~/.agents/skills/NAME"]
  OWN --> AS
  IL --> AS
  AS -->|relative mirror| CS["~/.claude/skills, ~/.codex/skills, ~/.grok/skills"]
```

`render()` concatenates `CORE.md`, `ROUTING.md`, and one tuning file per tool, with a "Generated by ... Do not edit" header (`agents/bin/build-manuals:48-62`).
Before rendering, `check_no_restated_rules` rejects any tuning or subagent file that repeats a core line verbatim (`agents/bin/build-manuals:79-121`), and `--check` compares rendered output byte for byte against the committed manuals.
`ic-link` then points each tool's home at its own manual and never falls back to Claude's manual for another tool (`dotfiles-nix/files/bin/ic-link:103-135`, `:143-175`).
Hooks are not copied anywhere: `settings.json` runs them by absolute path from the `agents` clone, guarded by `[ ! -f "$f" ] ||` so a missing clone is a no-op ([claude-code-config](components/claude-code-config.md)).
The weak point is the reverse edge: Claude Code writes its own settings through the symlink, which is how the committed `model` pin was dropped from the live file ([agents](components/agents.md) section 5).

## 5. Model routing decision

```mermaid
flowchart TD
  S["New task brief"] --> OV{"Captain named a model?"}
  OV -->|yes| USE["Use the captain's choice"]
  OV -->|no| CL{"Classify the task"}
  CL -->|"Tier 1: design, root cause, concurrency, security, perf"| T1{"Claude all_models and model:fable runway OK for the horizon?"}
  T1 -->|yes| FAB["claude fable, effort high"]
  T1 -->|no| FB["Ordered fallback: Opus 5.5 1M, then grok-4.7, then agy gemini-3.1-pro-high"]
  FB --> FBQ{"Any candidate with runway?"}
  FBQ -->|yes| FBP["First viable in order"]
  FBQ -->|no| STOP["Stop and report, never downgrade the class"]
  CL -->|"Tier 2: clear-spec coding"| P2["Peers: Opus 5.5 1M, grok-4.7, agy gemini-3.1-pro-high"]
  CL -->|"Tier 3: mechanical"| P3["Peers: haiku, grok-4.5, agy gemini-3.8-flash-medium"]
  CL -->|"Live info or X research"| P4["grok-4.7, then agy gemini-3.1-pro-high"]
  CL -->|"Multimodal, huge corpus, Google platforms"| P5["agy gemini-3.1-pro-high, then Opus 5.5 1M"]
  P2 --> GATES["Gate 1 eligibility, Gate 2 reasoning class, Gate 3 runway vs completion horizon"]
  P3 --> GATES
  P4 --> GATES
  P5 --> GATES
  GATES --> RANK{"Highest known spendPriority among survivors"}
  RANK -->|"unique winner"| PICK["Dispatch it"]
  RANK -->|"tie"| TIE["Report tie to captain"]
  RANK -->|"no candidate rankable or feasible"| ESC["Escalate, never pick arbitrarily"]
```

Step 1 (fit) comes from `agents/ROUTING.md:10-16` and is encoded as rules in `firstmate/config/crew-dispatch.json:2-56`.
Step 2 (quota) comes from `agents/ROUTING.md:17-19` and is executed by the `quota-array-dispatch` skill (`firstmate/.agents/skills/quota-array-dispatch/SKILL.md:58-134`).
Tier 1 is deliberately outside `spendPriority`: a cheaper model is a quality downgrade, not a peer (`firstmate/config/crew-dispatch.json:6`).
Unknown `spendPriority` (for example every `agy` row today) keeps a candidate eligible but never ranks it above a peer with known evidence (`agents/ROUTING.md:18`).
Full detail, formula, and a worked example are in [04-Model-Routing-and-Quota](04-Model-Routing-and-Quota.md).

## 6. Whiteboard version (12 boxes, 2 minutes)

```mermaid
flowchart LR
  B1["1 Captain"] --> B2["2 First mate (Claude Code)"]
  B2 --> B3["3 Backlog (tasks-axi)"]
  B2 --> B4["4 Router (rules + quota-axi)"]
  B4 --> B5["5 Models: Claude, Grok, Gemini"]
  B2 --> B6["6 Worktree pool (treehouse)"]
  B6 --> B7["7 Crewmate agent"]
  B5 --> B7
  B7 --> B8["8 Guardrails: hooks + subagents"]
  B7 --> B9["9 Gate (no-mistakes)"]
  B9 --> B10["10 GitHub PR + CI"]
  B11["11 Config plane: generated manuals + dotfiles"] -.-> B7
  B12["12 Fleet sync: manifest + daily ff-only"] -.-> B10
```

Draw it left to right in three rows: the decision row (1 to 5), the work row (6 to 9), and the platform row (10 to 12).

### Narration script

1. Draw box 1, "Captain": "I am the only human; I talk to exactly one agent."
2. Draw box 2, "First mate": "A Claude Code session whose job description forbids it from editing code; it supervises."
3. Draw box 3, "Backlog": "Every request becomes a row in a Markdown backlog, changed only through a CLI with a lockfile and atomic rename, so crashes and races do not lose work."
4. Draw box 4, "Router": "Routing is two steps: classify the task into a tier, then pick among that tier's models using live quota runway and a use-it-or-lose-it score."
5. Draw box 5, "Models": "Three subscriptions; the frontier model is reserved for hard reasoning and never traded down to save quota."
6. Draw box 6, "Worktree pool": "Each task gets a recycled git worktree at detached HEAD, so parallel agents never share a checkout."
7. Draw box 7, "Crewmate": "The worker agent implements in that worktree, reports status by appending to a file, and a zero-token bash watcher wakes the supervisor only when something is actionable."
8. Draw box 8, "Guardrails": "Every shell and edit call passes a hook that always blocks force-push to main and hook bypasses, asks a human for risky commands when one is present, and blocks config tampering when none is; reviewer subagents give a second opinion."
9. Draw box 9, "Gate": "The worker cannot push to origin; it pushes to a local gate that reviews, tests, documents, lints, pushes an exact verified SHA, opens the PR, and watches CI."
10. Draw box 10, "GitHub": "Only green, attested branches arrive here, and I approve the merge."
11. Draw box 11 with a dotted line to 7: "All agents read one generated manual per tool, built from a single source with a drift check in CI."
12. Draw box 12 with a dotted line to 10: "A manifest of 22 forks is fast-forwarded and rebuilt daily, and a read-only doctor verifies the whole machine."
13. Close: "Deterministic scripts own anything that can be exact; agents own judgment; nothing reaches GitHub except through the gate."
