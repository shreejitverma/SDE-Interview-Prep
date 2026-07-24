# Chapter 02 - Anatomy of a Coding Agent

## What you will master

- The complete internal architecture of a modern coding agent, using Claude Code as the reference implementation.
- The system prompt as an operating manual: what goes in it, in what order, and why every section earns its tokens.
- The tool surface: bash, read/edit/write, glob/grep, web tools, and the design logic for when an action deserves a dedicated tool versus a bash command.
- The permission model: modes, allow/deny rules, and the trust gradient from read-only to full autonomy.
- The extension mechanisms: CLAUDE.md memory, hooks, slash commands, skills, plan mode, and subagents, and which problem each one solves.
- Why the terminal-first form factor won the 2025 land rush, stated as an argument you can defend.
- A calibrated comparison of Claude Code, Cursor, Codex CLI, Gemini CLI, Aider, and OpenCode as of early 2026.

Date-stamp: product details in this chapter describe early 2026; the architecture concepts are stable, the feature lists are not.

## 1. The reference architecture

Strip any modern coding agent to its skeleton and you find the same five components:

1. A **model** capable of interleaved reasoning and tool use.
2. A **loop**: send conversation plus tool results to the model, execute the tool calls it returns, append results, repeat until the model stops calling tools.
3. A **tool surface**: the fixed set of actions the loop will execute.
4. A **policy layer**: permissions, sandboxing, and gates deciding which tool calls execute automatically, which require approval, and which are refused.
5. A **context manager**: what enters the prompt (system prompt, memory files, tool results) and what leaves it (compaction, truncation, elision).

Everything else - IDE panels, checkpoint systems, MCP servers, cloud execution - is elaboration on these five.
Claude Code is the cleanest public expression of the skeleton, which is why this chapter dissects it rather than a more featureful IDE product.
Chapter 7 has you build the skeleton yourself in about 300 lines, and it will feel familiar because this chapter is the map.

## 2. The system prompt as an operating manual

A coding agent's system prompt is not a personality blurb; it is an operating manual for an employee who reads the manual freshly on every task.
Claude Code's system prompt (observable because the client sends it via the API, and largely reproduced in the open Claude Agent SDK) is organized into functional sections, each of which exists because its absence caused a measurable failure mode:

- **Identity and tone rules**: the agent is a CLI tool; output is rendered in a terminal; be concise; no emoji unless asked.
Terminal rendering is why the prompt bans heavy markdown and long preambles - verbosity in a terminal is friction, not polish.
- **Proactiveness calibration**: do the thing the user asked, including obvious follow-ups, but do not surprise the user with unrequested actions.
This sentence-level tuning is among the highest-leverage text in the prompt, because over-proactive agents commit unwanted changes and under-proactive agents stop and ask constantly.
- **Convention-following rules**: mimic existing code style, check that a library is already a dependency before using it, never assume a framework.
These rules encode the difference between a contractor who reads the codebase and one who pastes from their last job.
- **Task management instructions**: use the todo-list tool for multi-step work, mark items complete as you go.
This externalizes plan state so it survives context pressure and is visible to the user.
- **Tool usage policy**: when to prefer specialized tools over bash, when to batch independent calls in parallel, when to delegate to subagents.
- **Safety rules**: refuse malware, never commit unless asked, never push force to shared branches, ask before destructive operations.
- **Environment context**: injected at runtime - working directory, git status, platform, date, model id.
This grounds the model in facts it would otherwise hallucinate.
- **Memory**: the contents of CLAUDE.md files (section 5) are appended into context with instructions to obey them.

Two design lessons generalize.
First, system prompts are debugged, not authored: nearly every rule corresponds to a real observed failure, and prompt diffs between versions read like a bug tracker.
Second, ordering matters less than specificity: vague rules ("be careful with git") underperform concrete rules ("never update the git config; never skip hooks with --no-verify") because models follow the letter of instructions increasingly literally as they improve.
The trade-off: a long manual costs tokens on every request and risks rule collisions; production prompts are in constant tension between coverage and weight, and prompt caching (stable prefix first, volatile context last) is what makes the weight affordable.

## 3. The tool surface

Claude Code's core tools, as of early 2026:

| Tool | Function | Why it is a dedicated tool |
|---|---|---|
| Bash | Run shell commands, foreground or background | Universal escape hatch; the permission system's main choke point |
| Read | Read a file (text, images, PDFs, notebooks), with offset/limit | Adds line numbers for reliable editing; enforces read-before-edit |
| Edit | Exact string replacement in a file | Diff-sized changes; fails loudly on ambiguity; enables staleness checks |
| Write | Create or overwrite a whole file | Overwrite requires prior read, preventing blind clobbering |
| Glob | Filename pattern matching | Fast, parallel-safe discovery without shell quoting hazards |
| Grep | Regex content search (ripgrep) | Structured output modes; safe to run in parallel |
| WebFetch / WebSearch | Retrieve docs and search results | Marks the trust boundary where untrusted web text enters context |
| Task | Spawn a subagent with its own context window | Context isolation for large searches; parallelism |
| TodoWrite | Maintain the visible task list | Plan persistence and user visibility |
| AskUserQuestion | Ask the user a structured question | Blocks the loop for input; renders as UI rather than prose |

The deep design question is why Read, Edit, Grep, and Glob exist at all when bash has cat, sed, grep, and find.
The answer, which Volume 3 introduced and this chapter makes concrete, is that a dedicated tool gives the harness a typed hook that an opaque bash string cannot provide:

- **Gating**: the permission layer can auto-approve Read while prompting for Bash, because the tool name carries semantics; `bash -c "cat file"` and `bash -c "rm -rf /"` are the same shape to the harness.
- **Invariants**: Edit can refuse to modify a file the model has not read in its current state, eliminating a whole class of stale-write bugs; sed cannot.
- **Rendering**: Edit calls render as diffs in the UI; Write renders as a file creation; bash output is just text.
- **Reliability**: Edit's exact-match-or-fail semantics turned out to be dramatically more reliable for models than line-number edits or unified-diff application, both of which fail silently when the model's line arithmetic drifts.
The string-replacement edit format is one of the quiet load-bearing discoveries of the 2024-2025 agent era, and every major agent converged on some variant of it.
- **Scheduling**: read-only tools are marked parallel-safe, so the harness can execute a batch of Grep/Read calls concurrently while serializing Bash.

The rule of thumb from Anthropic's own agent-design guidance: start with bash for breadth, promote an action to a dedicated tool when you need to gate, render, audit, or parallelize it.

## 4. The permission model

The permission layer is what makes it sane to run a shell-wielding agent on your laptop.
Claude Code's model has three interlocking parts.

**Modes** set the global posture:

- Default: read-only tools run freely; edits and commands prompt for approval.
- Accept-edits: file edits auto-approve; commands still prompt.
- Plan mode: the agent may only read and analyze; it produces a plan the user approves before any mutation (section 7).
- Bypass-permissions (also known as the dangerously-skip-permissions flag): everything auto-approves; intended for containers and CI, not laptops.

**Rules** refine the posture with allow/deny/ask lists in settings files, matched per tool and argument pattern, for example allowing `Bash(npm test:*)` while denying `Read(./.env)` and everything under secrets directories.
Rules compose across scopes - enterprise policy, user, project, local - with deny taking precedence, so an organization can forbid what a project cannot re-enable.

**Prompts** are the interactive fallback: when neither rules nor mode decide, the user is shown the exact command or diff and chooses allow-once, allow-always (which writes a rule), or deny with feedback.
Deny-with-feedback matters because the denial reason goes back to the model as a tool result, letting it adapt rather than stall.

The design tension is real and should be named: every prompt is friction, and friction pushes users toward bypass mode, which converts a permission system into theater.
The mitigations are rule learning (allow-always), sandboxed execution for low-risk commands, and containerized full-autonomy modes where bypass is actually safe.
Volume 11 treats the adversarial side - prompt injection attempting to launder malicious commands through the permission surface - and why argument-pattern matching alone is insufficient against a creative attacker.

## 5. CLAUDE.md: memory as files

Claude Code's persistent memory is deliberately low-tech: markdown files loaded into context at session start.

- Enterprise policy file: organization-wide, highest precedence.
- `~/.claude/CLAUDE.md`: user-global preferences across all projects.
- `<repo>/CLAUDE.md`: project conventions, build commands, architecture notes, committed to git and shared with the team.
- `CLAUDE.local.md` and nested per-directory files: personal or scoped overrides, loaded on demand.

The content that earns its place is the content the agent cannot cheaply rediscover: build and test commands, non-obvious invariants, style decisions, "do not touch" zones, and corrections the user is tired of repeating.
The `#` shortcut appends a remembered fact to the file mid-session, closing the loop from correction to memory.

Why files rather than a vector database?
Because files are inspectable, editable, versioned, and shared through the same git workflow as code, and because a few kilobytes of curated instructions beat retrieval over an uncurated pile for the steering use case.
The trade-off is that everything loads whether or not it is relevant, which taxes the context budget; large organizations hit this and must ruthlessly prune, or push detail into skills that load on demand (section 6).
Volume 6 covers the general memory design space; CLAUDE.md is its simplest workable point.

## 6. Hooks, slash commands, and skills

Three extension mechanisms cover three different gaps.

**Hooks** are user-defined shell commands that the harness (not the model) executes at lifecycle events: PreToolUse (can block or rewrite a tool call), PostToolUse (can run formatters or tests after edits), UserPromptSubmit, SessionStart, Stop, and others.
Hooks exist because instructions are probabilistic but policy must be deterministic: "run the formatter after every edit" as a prompt instruction is followed usually; as a PostToolUse hook it is followed always.
Use hooks for anything that must happen every time: lint gates, audit logging, blocking edits to protected paths, notifications.

**Slash commands** are parameterized prompt templates stored as markdown files in `.claude/commands/`, invoked explicitly by the user (for example `/review-pr 123`).
They capture repeatable workflows a human triggers deliberately.

**Skills** (introduced across Anthropic surfaces in late 2025) are folders with a SKILL.md manifest plus optional scripts and resources, loaded progressively: the model sees only name and description by default and reads the full skill when relevant.
Skills solve the context-economics problem that CLAUDE.md creates: expertise that would bloat every session loads only when the task calls for it, and the model itself decides when, unlike slash commands which the user decides.

The composition rule: memory for always-true facts, hooks for must-happen policy, slash commands for user-triggered workflows, skills for on-demand expertise.

## 7. Plan mode and subagents

**Plan mode** separates deliberation from mutation.
The agent researches the codebase read-only, writes a plan, and presents it for approval; on acceptance the mode drops to normal execution.
It exists because the cost curve of correction is steep: redirecting a wrong plan costs one message, unwinding wrong edits costs a review and a revert.
Use it for multi-file changes, unfamiliar codebases, and anything irreversible; skip it for one-line fixes where planning overhead exceeds task cost.

**Subagents** (the Task tool) spawn child agent instances with their own context windows, tool restrictions, and optionally different models and system prompts (custom subagents live in `.claude/agents/`).
The primary value is context economics, not anthropomorphic teamwork: a search that would flood the main context with twenty files of exploration comes back as a three-paragraph report, and several such explorations can run in parallel.
The cost is latency, tokens, and the telephone-game risk: a subagent knows only what its launch prompt says, so under-specified delegation returns confident irrelevance.
Volume 7 treats multi-agent architecture generally; the practical guidance here is to delegate self-contained, report-shaped work and keep tightly coupled edits in the main loop.

## 8. Why terminal-first won

In February 2025 the obvious form factor was the IDE: Copilot lived in VS Code and Cursor was a VS Code fork.
Claude Code shipped as a terminal program, and by late 2025 every major lab had shipped a terminal agent (Codex CLI, Gemini CLI) and every IDE product had added a terminal-style agent panel.
The reasons the terminal bet paid off:

- **The terminal is where the tools already are.**
An agent's actions are commands; running them where commands run means zero integration surface, and the agent inherits the user's real environment, dotfiles, and credentials rather than a simulation of them.
- **Composability.**
A CLI pipes: it can be scripted, cron-scheduled, invoked from CI, wrapped in other tools, and run headless with a print flag.
This made the same binary the substrate for the automation wave of Chapter 6; an IDE panel cannot be a build step.
- **Editor neutrality.**
Vim, JetBrains, VS Code, and Emacs users all have terminals; a terminal agent addresses the whole market without porting UI, and it follows developers to servers over SSH.
- **It matches delegation, not supervision.**
IDE inline completion optimizes watching every keystroke; a conversation-plus-diff surface optimizes describing intent and reviewing results.
As models improved, the economic center of gravity moved from supervision to delegation, and the terminal's low-bandwidth UI stopped being a weakness and became honest signaling about where the work happens.
- **Model-first competition.**
A thin harness makes the model the product; labs with frontier models rationally chose the surface with the least product between the user and the weights.

The honest counterpoint: terminals are hostile to rich diff review, image handling, and non-expert users, which is why the terminal core grew IDE extensions, web frontends, and desktop wrappers rather than remaining pure.
The stable synthesis as of early 2026: agent logic lives in a headless, scriptable core; surfaces (terminal, IDE, web, CI) are thin clients over it.
The Claude Agent SDK - the Claude Code harness packaged as a library - is that architecture made explicit.

## 9. The field, compared

Snapshot as of early 2026; expect drift.

**Cursor** (Anysphere): the leading AI-native IDE, a VS Code fork.
Strengths: best-in-class tab completion driven by custom autocomplete models, inline edit, an agent mode with checkpoints, background agents, and codebase embeddings for retrieval; multi-model (users pick Anthropic, OpenAI, Google, or Cursor's own models).
Trade-offs: value concentrates in the supervised editing loop; the fork must chase upstream VS Code; retrieval-by-embedding is a different navigation philosophy than agentic grep (Chapter 3 compares them).
Choose it when developers live in the IDE and want the tightest human-in-the-loop editing.

**Codex CLI and Codex cloud** (OpenAI): open-source terminal agent (rewritten in Rust in mid-2025) plus a hosted agent that runs tasks in cloud containers from a web UI or GitHub.
Strengths: tight integration with OpenAI reasoning models tuned for it, clean sandboxing story, and the cloud surface pioneered fire-and-forget delegated tasks at scale.
Trade-offs: the local CLI's extension ecosystem is thinner than Claude Code's; the product surface reorganized repeatedly during 2025, which taxed users.

**Gemini CLI** (Google): open-source (Apache-2.0) terminal agent over the Gemini models.
Strengths: aggressive free tier, huge context windows, and Google-ecosystem integration; the open license made it a popular base for forks.
Trade-offs: later to agentic-coding RL polish; enterprise controls and ecosystem depth trail as of this writing.

**Aider** (open source, 2023 onward): the original git-native terminal pair programmer.
Strengths: model-agnostic, transparent, efficient; its repository map (tree-sitter-derived symbol graph ranked by relevance) is an influential middle path between embeddings and raw grep; excellent for edit-loop workflows on a budget.
Trade-offs: philosophically human-driven - the user curates which files enter context - so it scales less naturally to long autonomous runs; smaller tool surface, no subagent or permission machinery.

**OpenCode** (open source, 2025): terminal agent emphasizing provider neutrality and a polished TUI, with a client-server design that allows remote driving.
Strengths: no lock-in, active community, works with local models.
Trade-offs: inherits whatever model it is pointed at, so reliability tracks the chosen model; enterprise governance is DIY.

**Claude Code** (Anthropic): the reference implementation this chapter dissected.
Strengths: deepest extension mechanism stack (hooks, skills, subagents, MCP client and server modes, SDK), a permission model refined by the largest agentic-coding deployment, and first-party models trained against the same harness.
Trade-offs: single-provider by design; terminal-first UX still asks more of non-expert users than an IDE; the pace of feature addition periodically outruns documentation.

The durable takeaway is not the ranking, which rots, but the axes: form factor (IDE versus terminal versus cloud), model coupling (first-party versus agnostic), navigation strategy (embeddings versus agentic search), extension depth, and governance controls.
Evaluate any new entrant on those five axes and you will not need a review site.

## Exercises

1. Run any coding agent with request logging enabled (or read the Claude Agent SDK's exported prompt), extract the system prompt, and annotate ten distinct rules with the failure mode each one plausibly prevents.
2. Write permission rules for a repository you own that allow the full test-and-lint loop to run unprompted while denying reads of secret files and all git push variants; then attempt three commands that should be blocked and verify they are.
3. Implement a PreToolUse hook that blocks any Bash call containing `git commit` unless a marker file exists, and a PostToolUse hook that runs your formatter after every Edit; document one behavior difference between enforcing the formatter via hook versus via system-prompt instruction.
4. Take a workflow you repeat weekly and implement it twice: as a slash command and as a skill; measure context tokens consumed in a session that does not use it, and state which mechanism was correct and why.
5. Give the same three-task benchmark (a bug fix, a multi-file refactor, a test-writing task) to Claude Code and one competitor from section 9; score each on edits correct, commands run, tokens or dollars spent, and interventions needed, and write a one-page comparison against the five axes.
6. Design the tool surface for a database-administration agent: name five dedicated tools you would promote out of bash, and justify each with one of gate, render, audit, invariant, or parallelize.

## Godhood check

You have mastered this chapter when you can:

- Draw the five-component skeleton from memory and place any named feature of any coding agent into the correct component in seconds.
- Explain why exact-string-replacement editing beat diff application and line-number editing, and what invariant a dedicated Edit tool can enforce that bash cannot.
- Recite the permission trust gradient from plan mode to bypass mode and argue where a new command pattern belongs, including the friction-versus-theater trade-off.
- State the composition rule for memory, hooks, slash commands, and skills, and correctly classify six real workflow needs into the four mechanisms.
- Deliver the terminal-first argument and its honest counterpoint, ending with the headless-core-plus-thin-surfaces synthesis.
- Compare any two coding agents on the five durable axes without mentioning a benchmark score.
