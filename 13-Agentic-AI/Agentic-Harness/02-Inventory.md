---
type: concept
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: ["https://github.com/shreejitverma/fleet-ops", "https://github.com/shreejitverma/agents", "https://github.com/shreejitverma/dotfiles-nix", "https://github.com/kunchenguid/dotfiles-mac-nix", "https://github.com/kunchenguid/firstmate", "https://github.com/kunchenguid/no-mistakes", "https://github.com/kunchenguid/treehouse", "https://github.com/kunchenguid/gnhf", "https://github.com/ImZoomBoy/wheelhouse", "https://github.com/kunchenguid/axi", "https://github.com/kunchenguid/gh-axi", "https://github.com/kunchenguid/chrome-devtools-axi", "https://github.com/kunchenguid/lavish-axi", "https://github.com/kunchenguid/tasks-axi", "https://github.com/kunchenguid/quota-axi", "https://github.com/kunchenguid/compact-adviser", "https://github.com/kunchenguid/baby-menu", "https://github.com/kunchenguid/short-pipe", "https://github.com/kunchenguid/justroll", "https://github.com/kunchenguid/autopreso", "https://github.com/kunchenguid/presize", "https://github.com/kunchenguid/trial-by-combat", "https://github.com/kunchenguid/org-bench", "https://github.com/kunchenguid/superpowers-bench", "https://github.com/kunchenguid/programbench-bench"]
---

# Agentic Harness - inventory

Every repo in the harness, re-verified on 2026-09-23 with `git remote -v`, `git rev-list --left-right --count upstream/<branch>...HEAD`, `git log --format=%an upstream/<branch>..HEAD`, `git status --porcelain`, `command -v`, and the fleet manifest `.fleet/manifest.yaml`.
Ahead and behind counts are against the local `upstream/*` remote-tracking refs, which component agents refreshed with `git fetch upstream` earlier on 2026-09-23 for firstmate, gnhf, treehouse, wheelhouse, and no-mistakes; the others may be stale by up to one day, but the daily 10:00 sync had already run.
Every fork's `origin` is `github.com/shreejitverma/<repo>`.
"My commits" means commits authored under my name; commits whose subject starts with `no-mistakes(<step>):` were written by the gate's fix steps under my identity.

## 1. Harness repos

| Repo | Path | Upstream | Fork delta (behind / ahead) and my commits | Language and toolchain | Binaries on PATH | Skills shipped | Purpose | Layer |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| [firstmate](components/firstmate.md) | `~/github/firstmate` | `kunchenguid/firstmate` | 31 / 11; 6 non-merge commits are mine (2 hand-written, 4 gate fix commits) plus 5 merges; +73/-19 in 8 files; `sync: false` | bash (`toolchain: [bash]`) | none; `fm` is `alias fm='firstmate'` for a zsh function (`dotfiles-nix/files/zsh/ic-workflow.zsh:518-526`) | `stow` (public, linked by `ic-link`); 22 internal entries in `.agents/skills` | Supervisor distro: first mate, crewmate spawn, watcher, merge, teardown | Control plane |
| [tasks-axi](components/tasks-axi.md) | `~/github/tasks-axi` | `kunchenguid/tasks-axi` | 0 / 0; none | Node, pnpm, `axi-sdk-js` | `/opt/homebrew/bin/tasks-axi` (npm link to clone) | `tasks-axi` | Markdown backlog CLI with locks, holds, `ready` derivation | Control plane |
| [quota-axi](components/quota-axi.md) | `~/github/quota-axi` | `kunchenguid/quota-axi` | 0 / 0; none | Node, pnpm, `axi-sdk-js` | `/opt/homebrew/bin/quota-axi` (npm link) | `quota-axi` | Subscription windows, runway, `spendPriority`; data only | Control plane |
| [treehouse](components/treehouse.md) | `~/github/treehouse` | `kunchenguid/treehouse` | 1 / 0 (missing `1185dc6`); none | Go, Nix flake | `~/go/bin/treehouse` (`make build` plus explicit install) | none | Pooled detached-HEAD git worktrees with leases | Execution |
| [gnhf](components/gnhf.md) | `~/github/gnhf` | `kunchenguid/gnhf` | 0 / 0; none | Node, pnpm | `/opt/homebrew/bin/gnhf` (npm link) | `gnhf` | Unattended iterate-commit-or-reset loop | Execution |
| [compact-adviser](components/compact-adviser.md) | `~/github/compact-adviser` | `kunchenguid/compact-adviser` | 0 / 0; none | Node plugin (`kind: plugin`) | none; loaded by Claude Code from a directory marketplace | none (plugin command `/compact-adviser`) | Tells an interactive session when `/compact` is safe | Execution |
| [axi](components/axi.md) | `~/github/axi` | `kunchenguid/axi` | 0 / 0; none | Node library, pnpm | none (CLIs resolve `axi-sdk-js` from npm, not this clone) | `axi` | 10 agent-CLI principles plus `axi-sdk-js` | Agent tooling |
| [gh-axi](components/gh-axi.md) | `~/github/gh-axi` | `kunchenguid/gh-axi` | 0 / 0; none | Node, pnpm, wraps `gh` | `/opt/homebrew/bin/gh-axi` (npm link) | `gh-axi` | Agent-shaped GitHub CLI (TOON output) | Agent tooling |
| [chrome-devtools-axi](components/chrome-devtools-axi.md) | `~/github/chrome-devtools-axi` | `kunchenguid/chrome-devtools-axi` | 0 / 0; none | Node, pnpm, bridge to `chrome-devtools-mcp` | `/opt/homebrew/bin/chrome-devtools-axi` (npm link) | `chrome-devtools-axi` | Real-browser verification with stale-ref detection | Agent tooling |
| [lavish-axi](components/lavish-axi.md) | `~/github/lavish-axi` | `kunchenguid/lavish-axi` | 0 / 0; none | Node, pnpm, Express plus WebSocket server | `/opt/homebrew/bin/lavish-axi` (npm link) | `lavish` | HTML review boards the human annotates; agent polls feedback | Agent tooling |
| [no-mistakes](components/no-mistakes.md) | `~/github/no-mistakes` | `kunchenguid/no-mistakes` | 1 / 0 (missing release 1.82.0); none | Go, SQLite, launchd daemon | `~/go/bin/no-mistakes` | `no-mistakes` | Local Git proxy gate: 9-step pipeline to PR and CI | Gate |
| [wheelhouse](components/wheelhouse.md) | `~/github/wheelhouse` | `ImZoomBoy/wheelhouse` | 0 / 117; 14 are mine (fleet list and docs), 103 are Kun Chen's history from the original repo | Python, GitHub Actions | none (runs only on GitHub) | none | Decision-card queue for other people's PRs and issues on my forks | Gate (downstream, on GitHub) |
| [dotfiles-nix](components/dotfiles-nix.md) | `~/github/dotfiles-nix` | `kunchenguid/dotfiles-mac-nix` | 2 / 45; all 45 are mine; the 2 behind are re-applied by content (ancestry only); `sync: false`; 1 untracked file (`backlog.md`) | Nix flake, nix-darwin, Home Manager, bash, zsh | `ic-link`, `ic-doctor`, `sync-forks`, `up`, `gprune`, `note`, `scratch`, `sysinfo` from `files/bin` | 12 owned: `ship` plus 11 ECC-derived | Declares the machine; symlink farm; health check; daily fork sync | Machine config |
| [agents](components/agents.md) | `~/github/agents` | none (my repo) | 44 commits, 43 under my name and 1 under an earlier author name; 1 modified file (`claude/settings.json`) | bash generator, Python hooks | none (`bin/build-manuals` runs by path) | none (6 subagents, 2 rules, 2 hooks) | One source to per-tool manuals; Claude settings, hooks, subagents | Machine config |
| [fleet-ops](components/fleet-ops.md) | `~/github/.fleet` | none (my repo) | 29 commits, all mine, 7 merged PRs | bash, awk, YAML, GitHub Actions | none; `fleet-sync`, `fleet-doctor`, `fleet-status`, `fleet-cd` come from generated `aliases.zsh` | none | 22-entry fork manifest, bootstrap, doctor, server-side sync | Machine config |

## 2. Apps and benchmarks (tracked, not part of the runtime)

All nine are forks with 0 behind and 0 ahead and no commits by me; they are synced daily and run or studied, not authored.

| Repo | Path | Upstream | Fork delta | Toolchain (manifest) | Binaries on PATH | Purpose | Role |
| --- | --- | --- | --- | --- | --- | --- | --- |
| [baby-menu](components/baby-menu.md) | `~/github/baby-menu` | `kunchenguid/baby-menu` | 0 / 0 | node >= 22.12, pnpm 11.1.1 | none (installed as a Homebrew cask) | macOS tray app hosting ACP agents; Electron 42, React 19, Tailwind v4 | Dogfood product |
| [short-pipe](components/short-pipe.md) | `~/github/short-pipe` | `kunchenguid/short-pipe` | 0 / 0 | node, pnpm 11.1.1 | none | Electron video app with an embedded read-only agent | Dogfood product |
| [justroll](components/justroll.md) | `~/github/justroll` | `kunchenguid/justroll` | 0 / 0 | node >= 20, pnpm 11.1.1 | `/opt/homebrew/bin/justroll` | Ink (React) terminal CLI | Dogfood product |
| [autopreso](components/autopreso.md) | `~/github/autopreso` | `kunchenguid/autopreso` | 0 / 0 | node >= 24, npm | `/opt/homebrew/bin/autopreso` | Express 5 plus `ws` live whiteboard | Dogfood product |
| [presize](components/presize.md) | `~/github/presize` | `kunchenguid/presize` | 0 / 0 | node 20, pnpm 8.6.2 | none | Qwik plus React islands image resizer | Legacy app |
| [trial-by-combat](components/trial-by-combat.md) | `~/github/trial-by-combat` | `kunchenguid/trial-by-combat` | 0 / 0 | node, npm | none | Agent arena with websocket spectators | Benchmark |
| [org-bench](components/org-bench.md) | `~/github/org-bench` | `kunchenguid/org-bench` | 0 / 0; 2 dirty paths (lockfile), so sync skips it daily | node, npm | none | Agent team topology benchmark | Benchmark |
| [superpowers-bench](components/superpowers-bench.md) | `~/github/superpowers-bench` | `kunchenguid/superpowers-bench` | 0 / 0 on `master` | node, npm | none | Skill-discovery benchmark | Benchmark |
| [programbench-bench](components/programbench-bench.md) | `~/github/programbench-bench` | `kunchenguid/programbench-bench` | 0 / 0 | bash, python, docker | none | Paired A/B studies of harness choices | Benchmark |

## 3. Non-repo inventory

| Item | Count or value | Evidence |
| --- | --- | --- |
| Fleet manifest entries | 22 (20 `sync: true`, 2 `sync: false`) | `.fleet/manifest.yaml:11-341`, `:38`, `:293` |
| `repos.txt` (server-side sync list) | 20 names, equal to the `sync: true` set | `wc -l .fleet/repos.txt` |
| Skills in `~/.agents/skills` | 31 (9 fork, 12 owned, 10 third-party) | [skills-catalog](components/skills-catalog.md) |
| Claude subagents | 6, all `claude-opus-5-5` at high effort; only `cpp-build-resolver` can write | `ls ~/.claude/agents`, `agents/tools/claude.md:28-30` |
| Claude rules | `cpp.md`, `python.md` | `ls ~/.claude/rules` |
| Claude hooks | `guard.py` (PreToolUse: Bash, Write, Edit, MultiEdit), `post_edit.py` (PostToolUse) | [claude-code-config](components/claude-code-config.md) |
| Generated manuals | `CLAUDE.md`, `GROK.md`, `GEMINI.md`, `AGENTS.md` | `agents/bin/build-manuals:27-31` |
| Live manual links | `~/.claude/CLAUDE.md`, `~/.grok/AGENTS.md`, `~/.gemini/AGENTS.md`, `~/AGENTS.md` all resolve into `~/github/agents` | `readlink` on 2026-09-23 |
| Model CLIs | `claude` (zsh wrapper around `~/.local/bin/claude`), `grok` (wrapper), `agy` at `~/.local/bin/agy` | `command -v`, `dotfiles-nix/files/zsh/ic-workflow.zsh:548-549` |
| Scheduled jobs | launchd `org.nix-community.home.sync-forks` at 10:00; GitHub cron `0 14 * * *` in fleet-ops; wheelhouse `scan-backstop` hourly cron | `dotfiles-nix/nix/home/darwin.nix:79-96`, [fleet-ops](components/fleet-ops.md), [wheelhouse](components/wheelhouse.md) |
| Daemons | `com.kunchenguid.no-mistakes.daemon.*` (launchd); lavish-axi server on port 4387 and chrome-devtools-axi bridge on 9224, both started on demand | [no-mistakes](components/no-mistakes.md), [lavish-axi](components/lavish-axi.md), [chrome-devtools-axi](components/chrome-devtools-axi.md) |

## 4. Install mechanisms, and why they differ

| Family | Install | Why | Evidence |
| --- | --- | --- | --- |
| Go (no-mistakes, treehouse) | `make build` then `install` into `$HOME/go/bin` | Upstream `make install` restarts the no-mistakes daemon mid-run, and treehouse's reads an unset `$(GOPATH)` under launchd | `.fleet/manifest.yaml:49-50`, `:169-170` |
| Node CLIs | `pnpm install --frozen-lockfile && pnpm run build`, then `npm link` | A fast-forward plus rebuild is live immediately, with no publish step | [fleet-ops](components/fleet-ops.md) section 3 |
| firstmate | nothing; the clone is the product | It is instructions plus scripts, launched by `fm` | `.fleet/manifest.yaml:26-39` |
| compact-adviser | `claude plugin marketplace update`, Grok plugin reinstall | Claude reads hooks live from the clone; Grok copies plugins | `.fleet/manifest.yaml:336-337` |
| dotfiles-nix | `sudo darwin-rebuild switch` via the `rebuild` alias | nix-darwin activation needs root; flake update and brew stay as the user | `dotfiles-nix/nix/home/darwin.nix:25-29` |
| wheelhouse | nothing locally | It runs as GitHub Actions in the fork | `.fleet/manifest.yaml:313-326` |

## 5. Interview angle

**Q: How much of this did you write?**
The runtime tools are upstream open source, and I say so first.
I own the composition: the routing policy and its encoding, the manual generator and safety hooks (`agents`), the machine and symlink layer (45 commits in `dotfiles-nix`), the fleet inventory and sync (`fleet-ops`), and six fork patches in firstmate, four of which the gate itself wrote while fixing review, docs, and CI findings.

**Q: Why fork at all instead of installing releases?**
A fork gives me a place to carry patches (firstmate), a pinned and reviewable source for everything my agents execute, and a fast-forward-only update path that rebuilds from source daily; the cost is 22 repos to keep current, which is why the manifest and the doctor exist.
