# Chapter 03 - The Agent Loop From Scratch

## What you will master

- Building a complete, working agent against the Anthropic API in roughly 130 lines of Python, with no framework.
- The four responsibilities every harness has: the loop, tool dispatch, message accumulation, and stop conditions.
- Iterating the minimal agent into something usable: streaming output, multiple tools, and a real tool surface of bash, file read, file write, and search.
- Where every kind of production concern will later attach to this skeleton, so the rest of the volume has a concrete anchor.
- The debugging instincts that come only from having written the loop yourself instead of importing it.

This is the keystone chapter of the entire track.
Everything before it was preparation and everything after it is refinement.
Do not read this chapter; run it.

## 3.1 Ground rules

The code targets the Anthropic Messages API with the official `anthropic` Python SDK, and the API shapes shown are current as of early 2026.

Setup, assuming Python 3.11 or newer:

```bash
pip install anthropic
export ANTHROPIC_API_KEY=...
```

The examples use the model id `claude-opus-4-8`, current as of early 2026; substitute the current strong model when you read this later, because model ids rot faster than any other detail in this volume.

One safety note before the first run: this agent executes shell commands the model chooses.
Run it in a directory you do not mind changing, ideally inside a container or throwaway VM, and read Chapter 7 for real sandboxing.
The examples deliberately omit sandboxing so the loop stays visible; that omission is pedagogical, not a recommendation.

## 3.2 Version 1: a complete agent in about 130 lines

The first version has one tool, bash, which is enough to make it a real agent: with a shell it can inspect files, run programs, install packages, and verify its own work.

```python
"""agent_v1.py - a minimal but complete agent: one model, one tool, one loop."""

import json
import subprocess

import anthropic

MODEL = "claude-opus-4-8"  # current as of early 2026
MAX_TURNS = 30

SYSTEM_PROMPT = """You are a capable software engineering agent working in a Unix shell.

Work step by step: inspect before you modify, and verify after you modify.
When the task is complete, verify the result with a command, then summarize
what you did in plain text and stop calling tools."""

TOOLS = [
    {
        "name": "bash",
        "description": (
            "Run a shell command in a persistent working directory and return "
            "its stdout and stderr. Use this to inspect files, run programs, "
            "and verify your work. Commands time out after 60 seconds."
        ),
        "input_schema": {
            "type": "object",
            "properties": {
                "command": {
                    "type": "string",
                    "description": "The shell command to execute.",
                }
            },
            "required": ["command"],
        },
    }
]


def run_bash(command: str) -> str:
    """Execute a shell command and format the outcome as an observation."""
    try:
        result = subprocess.run(
            command,
            shell=True,
            capture_output=True,
            text=True,
            timeout=60,
        )
    except subprocess.TimeoutExpired:
        return "Error: command timed out after 60 seconds."
    output = ""
    if result.stdout:
        output += result.stdout
    if result.stderr:
        output += ("\n--- stderr ---\n" + result.stderr)
    if result.returncode != 0:
        output += f"\n(exit code: {result.returncode})"
    if not output.strip():
        output = "(command produced no output)"
    return output[:10_000]  # crude truncation; Chapter 4 does this properly


def execute_tool(name: str, tool_input: dict) -> tuple[str, bool]:
    """Dispatch a tool call. Returns (observation, is_error)."""
    if name == "bash":
        try:
            return run_bash(tool_input["command"]), False
        except Exception as exc:  # noqa: BLE001 - errors become observations
            return f"Error: {exc}", True
    return f"Error: unknown tool '{name}'", True


def run_agent(task: str) -> str:
    client = anthropic.Anthropic()
    messages = [{"role": "user", "content": task}]

    for turn in range(MAX_TURNS):
        response = client.messages.create(
            model=MODEL,
            max_tokens=8192,
            system=SYSTEM_PROMPT,
            tools=TOOLS,
            messages=messages,
        )

        # Always append the assistant turn verbatim, tool_use blocks included.
        messages.append({"role": "assistant", "content": response.content})

        # Surface the model's narration.
        for block in response.content:
            if block.type == "text" and block.text.strip():
                print(f"\n[agent] {block.text}")

        if response.stop_reason != "tool_use":
            # end_turn, max_tokens, refusal: the loop is over either way.
            return final_text(response)

        # Execute every tool call in this turn and return all results together.
        results = []
        for block in response.content:
            if block.type != "tool_use":
                continue
            print(f"[tool ] {block.name} {json.dumps(block.input)[:200]}")
            observation, is_error = execute_tool(block.name, block.input)
            print(f"[obs  ] {observation[:200]}")
            results.append(
                {
                    "type": "tool_result",
                    "tool_use_id": block.id,
                    "content": observation,
                    "is_error": is_error,
                }
            )
        messages.append({"role": "user", "content": results})

    return "Stopped: reached the maximum number of turns."


def final_text(response) -> str:
    return "\n".join(b.text for b in response.content if b.type == "text")


if __name__ == "__main__":
    import sys

    task = " ".join(sys.argv[1:]) or "Count the lines of Python code in this directory."
    print(f"[task ] {task}")
    print(f"\n[done ] {run_agent(task)}")
```

Run it:

```bash
python agent_v1.py "Create a file called fib.py containing a fibonacci function, then run it to print the first 10 numbers, and fix any errors you hit."
```

An illustrative transcript, abbreviated, of the kind of trajectory this produces:

```
[task ] Create a file called fib.py ...
[agent] I'll create the file first, then run it to verify.
[tool ] bash {"command": "cat > fib.py << 'EOF'\ndef fib(n): ..."}
[obs  ] (command produced no output)
[tool ] bash {"command": "python fib.py"}
[obs  ] 0 1 1 2 3 5 8 13 21 34
[agent] Done. fib.py defines fib(n) and printing the first 10 numbers works.
[done ] Done. fib.py defines fib(n) and printing the first 10 numbers works.
```

That is a genuine agent: it planned, acted, observed real output, and decided for itself when it was finished.

## 3.3 Anatomy of what you just built

The 130 lines contain exactly four responsibilities, and every agent harness you will ever read - including ones with a thousand times the code - contains the same four.

### The loop

The `for turn in range(MAX_TURNS)` loop alternates model calls with tool executions.
Its shape encodes the protocol from Chapter 2: sample, check stop_reason, execute, append, repeat.
The turn cap is not decoration; it is the outermost budget guard, and without it a confused model looping on a failing command would spend money until you noticed.

### Tool dispatch

`execute_tool` maps a name string to a Python function and converts every possible failure into a string observation plus an `is_error` flag.
Note the deliberate asymmetry: the harness never raises on tool failure, because an exception that escapes the loop kills the trajectory, while an error returned as an observation gives the model a chance to recover.
This single design decision - errors are observations, not exceptions - is the seed of all of Chapter 5.

### Message accumulation

The `messages` list is the agent's entire memory, and both append sites obey the wire-format contracts from Chapter 2.
The assistant message is appended verbatim with its tool_use blocks, and all tool results for a turn travel together in one user message.
The SDK accepts its own response block objects inside the messages list, so no manual re-serialization is needed.
Notice what this implies about state: to checkpoint this agent you serialize one list, and to resume it you deserialize one list, a property Chapter 6 exploits.

### Stop conditions

The loop ends in one of three ways: the model stops requesting tools (`stop_reason` is anything but `tool_use`), the turn budget runs out, or an unhandled harness crash, which we have tried to make impossible.
Version 1 treats `end_turn`, `max_tokens`, and `refusal` identically by returning whatever text is present, which is honest but crude; distinguishing them properly is an exercise below and a topic of Chapters 5 and 6.

## 3.4 Version 2: streaming

Version 1 is silent for the many seconds each model call takes, which is unacceptable for anything interactive.
The fix is streaming: consume the response as server-sent events and print text deltas as they arrive.
The SDK wraps this in a context manager, and crucially, `get_final_message()` returns the same complete message object version 1 used, so the loop logic does not change at all.

Replace the `client.messages.create` call and narration block with:

```python
        with client.messages.stream(
            model=MODEL,
            max_tokens=8192,
            system=SYSTEM_PROMPT,
            tools=TOOLS,
            messages=messages,
        ) as stream:
            for text in stream.text_stream:
                print(text, end="", flush=True)
            response = stream.get_final_message()
        print()

        messages.append({"role": "assistant", "content": response.content})
```

Everything downstream - stop_reason handling, dispatch, accumulation - is untouched.
This is the payoff of separating the four responsibilities: transport concerns changed, and the loop did not.

Streaming also changes what you can observe.
The event stream carries `content_block_start` events that announce a tool call before its arguments finish generating, and `input_json_delta` events carrying argument fragments, which is how real agent UIs show "running bash..." with the command materializing live.
For long outputs, streaming is not just UX but a requirement: large `max_tokens` values on non-streaming requests risk HTTP timeouts, so production harnesses stream by default.

## 3.5 Version 3: a real tool surface

A bash-only agent works, but Chapter 4 will argue that dedicated tools for common operations are safer, cheaper, and easier for the model to use well.
Version 3 adds three: `read_file`, `write_file`, and `search`.

Add the definitions:

```python
TOOLS = [
    # ... the bash tool from version 1 ...
    {
        "name": "read_file",
        "description": (
            "Read a text file and return its contents with line numbers. "
            "Prefer this over 'cat' so you can reference line numbers later. "
            "Large files are returned in pages; pass 'offset' to continue."
        ),
        "input_schema": {
            "type": "object",
            "properties": {
                "path": {"type": "string", "description": "File path, relative or absolute."},
                "offset": {
                    "type": "integer",
                    "description": "1-based line to start from. Default 1.",
                },
            },
            "required": ["path"],
        },
    },
    {
        "name": "write_file",
        "description": (
            "Create or overwrite a file with the given content. "
            "Creates parent directories as needed. "
            "For small edits to existing files prefer targeted commands via bash."
        ),
        "input_schema": {
            "type": "object",
            "properties": {
                "path": {"type": "string", "description": "File path to write."},
                "content": {"type": "string", "description": "Full file content."},
            },
            "required": ["path", "content"],
        },
    },
    {
        "name": "search",
        "description": (
            "Search file contents for a regex pattern under a directory. "
            "Returns matching lines as path:line:text, capped at 50 matches. "
            "Use this to locate code before reading whole files."
        ),
        "input_schema": {
            "type": "object",
            "properties": {
                "pattern": {"type": "string", "description": "Regular expression to find."},
                "directory": {
                    "type": "string",
                    "description": "Directory to search. Default '.'",
                },
            },
            "required": ["pattern"],
        },
    },
]
```

And the implementations plus the extended dispatcher:

```python
import pathlib
import re

PAGE_LINES = 500


def read_file(path: str, offset: int = 1) -> str:
    p = pathlib.Path(path)
    if not p.is_file():
        return f"Error: no such file: {path}"
    lines = p.read_text(errors="replace").splitlines()
    if offset > len(lines):
        return f"Error: offset {offset} is past end of file ({len(lines)} lines)."
    page = lines[offset - 1 : offset - 1 + PAGE_LINES]
    body = "\n".join(f"{i + offset:>6}\t{line}" for i, line in enumerate(page))
    remaining = len(lines) - (offset - 1 + len(page))
    if remaining > 0:
        body += f"\n... {remaining} more lines; call read_file with offset={offset + len(page)}."
    return body


def write_file(path: str, content: str) -> str:
    p = pathlib.Path(path)
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(content)
    return f"Wrote {len(content)} bytes to {path}."


def search(pattern: str, directory: str = ".") -> str:
    try:
        rx = re.compile(pattern)
    except re.error as exc:
        return f"Error: invalid regex: {exc}"
    matches = []
    for p in sorted(pathlib.Path(directory).rglob("*")):
        if not p.is_file() or p.stat().st_size > 1_000_000:
            continue
        try:
            text = p.read_text(errors="replace")
        except OSError:
            continue
        for i, line in enumerate(text.splitlines(), 1):
            if rx.search(line):
                matches.append(f"{p}:{i}:{line.strip()[:200]}")
                if len(matches) >= 50:
                    return "\n".join(matches) + "\n... capped at 50 matches; narrow the pattern."
    return "\n".join(matches) if matches else "No matches."


def execute_tool(name: str, tool_input: dict) -> tuple[str, bool]:
    handlers = {
        "bash": lambda a: run_bash(a["command"]),
        "read_file": lambda a: read_file(a["path"], a.get("offset", 1)),
        "write_file": lambda a: write_file(a["path"], a["content"]),
        "search": lambda a: search(a["pattern"], a.get("directory", ".")),
    }
    handler = handlers.get(name)
    if handler is None:
        return f"Error: unknown tool '{name}'", True
    try:
        return handler(tool_input), False
    except KeyError as exc:
        return f"Error: missing required argument {exc}", True
    except Exception as exc:  # noqa: BLE001
        return f"Error: {type(exc).__name__}: {exc}", True
```

Three design details in this code are load-bearing, and each previews a later chapter.

The `read_file` output is line-numbered and paginated with an explicit continuation instruction, which is Chapter 4's token-efficiency and pagination guidance in miniature.
The `search` output is capped with a message telling the model how to narrow, converting an overflow into a steerable observation rather than a context bomb.
The dispatcher validates presence of required arguments and turns a missing one into an `is_error` observation naming the argument, which is exactly the retry-with-feedback pattern Chapter 5 formalizes.

With this surface, try a meaningfully harder task:

```bash
python agent_v3.py "Find every TODO comment in this repository, and write a markdown report todos.md grouping them by file with line numbers."
```

Watch the trajectory: a good model will search first, read selectively, write once, and verify by reading its own report back.
When it instead reads whole files it did not need, the fix is usually a sharper tool description, not more code, and noticing that is the beginning of ACI intuition.

## 3.6 What is deliberately missing

You now hold a complete agent skeleton, and it is worth naming what it lacks so the rest of the volume has hooks.

It has no sandbox, so the model's commands run with your privileges; Chapter 7 fixes this.
It has one budget guard, turns, and no cost or wall-clock budget; Chapter 5 adds them.
It cannot be interrupted, steered mid-task, checkpointed, or resumed; Chapter 6 adds all four.
It retries nothing and detects no loops, so a model stuck repeating a failing command burns the whole turn budget; Chapter 5 again.
Its tool outputs are crudely truncated at a character count rather than thoughtfully summarized; Chapter 4.
It keeps the full transcript forever, so very long tasks will eventually exhaust the context window; Volume 06 treats context management in depth.

None of these gaps require restructuring the skeleton.
Every one of them attaches at a seam you can already point to: budgets wrap the loop, gates wrap the dispatcher, checkpoints serialize the messages list, and context management rewrites it.
That is the argument for having built this by hand: you now know where everything goes.

## 3.7 SDK note: the tool runner

The Anthropic SDKs ship a beta helper, the tool runner, that implements this same loop for you: decorate Python functions with `@beta_tool`, pass them to `client.beta.messages.tool_runner(...)`, and iterate until done, with hooks for intervening between turns.
As of early 2026 it is a reasonable default for production custom-tool agents, precisely because what it automates is what you just wrote.
This track had you write the loop anyway, for the same reason systems courses have you implement malloc: you will debug, extend, and distrust these systems far better for having built one.
Use the runner when it fits; never let it be a mystery.

## Exercises

1. Type in and run version 1, then version 3, on your own machine against a real task in a scratch repository; keep the transcripts.
2. Extend the loop to distinguish stop reasons: on `max_tokens`, retry the turn once with a doubled cap; on `refusal`, stop immediately with a distinct message; on turn exhaustion, have the harness ask the model for a summary of progress so far and return that.
3. Add an `edit_file` tool that replaces an exact string in a file and errors informatively when the string is absent or ambiguous, then observe whether the model starts preferring it over write_file for small changes.
4. Add per-trajectory cost tracking by accumulating `response.usage` across turns, printing input tokens, output tokens, and an estimated dollar cost at exit.
5. Make tool execution concurrent for parallel tool calls using a thread pool, while keeping all results in one user message; then explain in a comment why this is safe for search and read_file but questionable for bash.
6. Break the harness deliberately three ways - drop the tool_use block from an appended assistant message, split parallel results across two user messages, and mismatch a tool_use_id - and record the API error or behavior change each produces.
7. Rewrite version 3 on the SDK tool runner and compare line count, behavior on the same task, and where your custom stop-reason handling from exercise 2 has to live.

## Godhood check

You have mastered this chapter when you can do the following without reference material.

- Write a working agent loop against the Anthropic API from a blank file in under thirty minutes, including dispatch, accumulation, and stop conditions.
- Name the four responsibilities of a harness and point to the exact lines implementing each in code you wrote.
- Explain why errors must become observations rather than exceptions, and what breaks in recovery when this rule is violated.
- Add streaming to a non-streaming loop without touching dispatch or accumulation logic.
- Predict, before running, how a given tool description change will alter the model's tool selection on a concrete task, and verify by experiment.
- List the six things the skeleton deliberately lacks and state, for each, the seam in the code where it will attach.
