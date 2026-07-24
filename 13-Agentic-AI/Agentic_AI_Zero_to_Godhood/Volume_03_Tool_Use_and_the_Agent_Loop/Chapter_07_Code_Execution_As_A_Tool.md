# Chapter 07 - Code Execution As A Tool

## What you will master

- Why a code interpreter is the highest-leverage single tool you can give an agent, and the threat model that comes bundled with it.
- The CodeAct pattern: acting by writing programs instead of emitting one JSON tool call per action, and the evidence for when it wins.
- The sandboxing spectrum: bare subprocess, Docker containers, gVisor, Firecracker microVMs, and WebAssembly, with the isolation and performance trade-offs of each.
- Cloud sandbox services - E2B, Modal, Daytona, and provider-hosted execution - and how to choose among them.
- State persistence between executions: REPL sessions, container reuse, and filesystem workspaces.
- Code execution as the answer to tool proliferation, including programmatic tool calling and MCP code mode.

Vendor capabilities, product names, and API shapes in this chapter are date-stamped as of early 2026 and will rot; the isolation principles will not.

## 7.1 The most powerful tool

Chapter 3's agent gained most of its capability the moment it got a shell, and this chapter is about taking that observation seriously.

A code execution tool - a Python REPL, a shell, a scratch VM - is qualitatively different from every other tool because it is a tool factory.
An agent with `get_weather` can get weather; an agent with an interpreter can write a weather client, and a parser for its output, and a plot of the result, none of which you anticipated.
The model's pretraining distribution is saturated with code, so its competence at expressing intent as programs generally exceeds its competence at any bespoke calling convention you invent.
And code composes: loops, conditionals, retries, and intermediate variables come for free in the language, where the JSON tool-call protocol from Chapter 2 must buy each of them with a full model round trip.

The price is symmetric and must be stated with the same emphasis.
Arbitrary code execution is the maximal capability grant, so it is the maximal attack surface: a prompt-injected agent with an interpreter and network egress is a remote-code-execution vulnerability you built on purpose.
The entire discipline of this chapter is enjoying the leverage while containing the blast radius, and the containment tool is the sandbox, not the prompt.
Prompts are advisory; kernels are not.

## 7.2 The REPL tool

The minimal code execution tool is a stateful interpreter exposed through the standard tool protocol:

```json
{
  "name": "python",
  "description": "Execute Python code in a persistent session and return stdout, stderr, and the value of the final expression. Variables, imports, and definitions persist across calls. Use print() for intermediate values. Execution times out after 120 seconds.",
  "input_schema": {
    "type": "object",
    "properties": {
      "code": {"type": "string", "description": "Python source to execute."}
    },
    "required": ["code"]
  }
}
```

Three design points elevate a REPL tool from demo to dependable, and all three are Chapter 4 ACI discipline applied to a special case.

Statefulness must be explicit in the description, because the model's strategy differs completely between a persistent session, where it builds up state incrementally, and a fresh-process-per-call tool, where every snippet must be self-contained; ambiguity here produces the classic bug of the model referencing a variable that died with the previous process.
Output must capture everything the model needs to perceive the run: stdout, stderr, the repr of the final expression, and rich outputs like plots delivered as files or images, with the truncation and pagination discipline of Chapter 4 applied to runaway prints.
Errors must come back whole where it counts: exception type, message, and the relevant traceback frames, because the model debugs by reading exactly what a human reads, and code-plus-error-plus-retry is the single most reliable self-correction loop in all of agentics, per Chapter 5.

## 7.3 The CodeAct pattern

The default protocol of Chapter 2 has the model emit one structured call per action.
The CodeAct pattern, named by Wang et al. in their 2024 paper "Executable Code Actions Elicit Better LLM Agents", inverts the ratio: the model's action space is executable code, and tools become functions callable from that code.

Concretely, instead of three round trips -

```
tool_use: get_user(id=42)          -> observation
tool_use: get_orders(user=42)      -> observation
tool_use: refund(order=oldest)     -> observation
```

- the model emits one program:

```python
user = get_user(42)
orders = get_orders(user_id=user.id)
oldest = min(orders, key=lambda o: o.date)
if oldest.total < 100:
    result = refund(order_id=oldest.id)
print(oldest.id, result.status)
```

The wins are structural, not stylistic.
Control flow is native: the conditional above costs zero extra turns, where the JSON protocol pays a model call per branch decision.
Intermediate data stays out of the context window: the full orders list lives in an interpreter variable, and only the two printed values return to the transcript, which matters enormously when intermediates are large - the difference between a 50-row projection and a 100,000-row dataset flowing through the model's context.
Latency and cost collapse from N round trips to one for composable sequences.
And the original paper reported materially higher task success rates for code actions over JSON and text actions across their benchmark suite, a result the industry's subsequent convergence on code-capable agents has broadly borne out.

The losses are equally structural, and they map exactly onto Chapter 4's reasons for dedicated tools.
A program is opaque to the harness at dispatch time: you can gate, log, and render a typed `refund` call, but a refund buried in twenty lines of Python defeats argument-level policy, so approval gates degrade to all-or-nothing on the whole snippet.
Errors arrive later and tangled: a JSON call fails at one well-defined point, while a program can fail at line 14 having already committed lines 1 through 13, resurrecting the partial-side-effect problem of Chapter 5 inside a single action.
And per-call observability dissolves into whatever the program chooses to print.

The synthesis, which is where production systems landed as of early 2026: use code actions for read-and-compute work - querying, filtering, transforming, analyzing - and require consequential side effects to exit the sandbox only through gated, typed tools.
The sandbox boundary makes this enforceable: pure computation is free inside, and the dangerous verbs are only reachable as audited function calls whose implementations live outside.

## 7.4 The sandboxing spectrum

Everything below assumes the question "what happens when the agent runs hostile or merely wrong code", and the options form a spectrum of isolation strength against startup cost and operational complexity.
Capabilities and defaults here are as of early 2026.

### Bare subprocess

Chapter 3's `subprocess.run` shares your user account, filesystem, network, and credentials with the model's output.
It is acceptable for local development by the person reading the transcript in real time, and for nothing else.
Treat it as the pedagogical baseline it was.

### Docker containers

A container gives the code its own filesystem, process namespace, and resource limits via namespaces and cgroups, while sharing the host kernel.
Startup is subsecond with a warm image, the ecosystem is universal, and for the common threat model - buggy code, filesystem mess, runaway resource use - it is a sound default.
The shared kernel is the ceiling: container escape via kernel vulnerability is a real, recurring class, so hardened deployments add seccomp and capability drops, run non-root with read-only root filesystems, and above all constrain egress, because for agents the likeliest catastrophe is not kernel escape but prompt-injected exfiltration through a perfectly legal HTTPS request.
Network policy is part of the sandbox, not an accessory to it.

### gVisor

gVisor, Google's open-source user-space kernel, interposes on syscalls so guest code talks to a reimplemented kernel surface rather than the host kernel, shrinking the host's exposed attack surface dramatically while keeping container-like ergonomics and startup times.
The cost is syscall interposition overhead - negligible for compute-bound work, noticeable for syscall-heavy and I/O-heavy workloads - and occasional compatibility gaps in the long tail of syscalls.
It is the middle of the spectrum: much stronger than plain containers, much lighter than full VMs, and it underpins serverless platforms including Modal as of early 2026.

### Firecracker microVMs

Firecracker, the AWS-built open-source virtual machine monitor behind Lambda and Fargate, runs each workload in a true hardware-virtualized VM with its own guest kernel, stripped of legacy device emulation so that boot lands in the low hundreds of milliseconds with a memory overhead of a few megabytes per VM.
This is the strongest practical isolation tier for multi-tenant agent code: an escape requires defeating hardware virtualization, not a kernel bug.
The costs are operational - you manage kernels, images, snapshots, and a KVM-capable host - which is precisely the burden the cloud sandbox services of the next section exist to absorb; E2B, notably, runs on Firecracker.

### WebAssembly

Wasm runtimes offer deny-by-default capability isolation with millisecond instantiation, attractive for embedding untrusted plugins in-process.
For general agent code execution as of early 2026 it remains the constrained option: the Python-and-native-packages ecosystem agents lean on ports incompletely, so Wasm shines for narrow, language-controlled extension points rather than open-ended interpreters.

### Choosing

The decision procedure compresses to three questions.
Whose code is effectively running - your one internal agent, or anything shaped by untrusted input - which sets the tier: containers for the former, gVisor or microVMs once adversarial input or multi-tenancy enters.
What does a breach reach - which is about the credentials and network paths inside the sandbox, and is governed by least privilege and egress control at every tier.
And what latency can you pay - warm-pool engineering matters more than cold-start benchmarks, since every serious platform keeps pre-provisioned sandboxes so the agent-perceived startup is milliseconds regardless of tier.

## 7.5 Cloud sandboxes

By early 2026 running your own sandbox fleet became a choice rather than a necessity, with a mature market of sandbox-as-a-service offerings.
Representative options, date-stamped:

E2B provides Firecracker-backed sandboxes purpose-built for AI agents, with SDKs that create a sandbox in roughly 150 milliseconds, execute code and shell commands, mount files, expose ports, and pause and resume sandbox state; its open-source core also permits self-hosting.
Modal provides gVisor-isolated serverless compute with a Python-native API, strong batch and GPU support, and aggressive cold-start engineering including filesystem and memory snapshotting; it is as much a general compute platform as an agent sandbox, which is a strength when your agent's workloads include heavy data or model jobs.
Daytona provides fast-provisioning sandboxes targeted at agent code execution with per-sandbox filesystem, git, and process APIs, positioning on sub-100-millisecond starts and agent-native ergonomics.
Model providers ship hosted execution as an API feature: the Anthropic API's server-side code execution tool (as of early 2026, tool type `code_execution_20260120` and successors) runs Python in an Anthropic-managed container with files-in, files-out and no client-side loop at all, and OpenAI offers the equivalent in its tool suite.

The build-versus-buy calculus: provider-hosted execution is the least engineering for the common case but binds you to the provider's runtime, packages, network policy, and limits; sandbox services give runtime control with someone else operating the isolation layer, and per-second pricing that beats self-managed fleets until utilization is high and steady; self-hosting on Firecracker or gVisor wins on data locality, custom images, compliance, and marginal cost at scale, and costs you an infrastructure team.
The interface discipline that keeps the choice reversible: hide execution behind your own `execute(code) -> observation` boundary in the harness, so the sandbox vendor is a configuration detail, not an architecture.

## 7.6 State persistence between executions

Statelessness is the enemy of multi-step computation, and sandbox state has three layers worth separate treatment.

Interpreter state - variables, imports, loaded dataframes - persists naturally in a live kernel process, Jupyter-style; this is the cheapest continuity and the most fragile, dying with the process and growing unboundedly if never reset.
The harness should expose the reset: a `restart` capability in the tool (as the standard bash tool's schema includes) plus an honest observation when a session died, so the model rebuilds state instead of referencing ghosts.
Filesystem state - written files, installed packages, cloned repos - persists with the sandbox instance and survives interpreter restarts; provider-hosted execution exposes this as container reuse via an id (the Anthropic API returns a container id that subsequent requests can pass to resume the same filesystem, with a bounded lifetime, as of early 2026), and sandbox services expose it as pause-and-resume of the whole instance.
Cross-session state - artifacts that must outlive any sandbox - belongs in external stores by explicit tool action: object storage, git remotes, databases; the checkpoint discipline of Chapter 6 applies unchanged, with one addition, that the sandbox's relevant filesystem state either be part of the checkpoint (snapshot ids) or be re-establishable from the transcript (a setup script the model can rerun).

The design smell to avoid is implicit state coupling: a trajectory that only works because sandbox 7's `/tmp` happens to contain last Tuesday's files is a trajectory that cannot be resumed, forked, or debugged, which quietly forfeits everything Chapter 6 built.

## 7.7 Code execution versus tool proliferation

Chapter 4 left a tension unresolved: every added tool costs definition tokens on every request and dilutes selection accuracy, yet capability keeps demanding more tools.
Code execution is the structural answer, and by early 2026 it had crystallized into a named pattern across the industry.

The observation, popularized in Anthropic's late-2025 engineering writing on code execution with MCP and echoed by Cloudflare's "code mode" work: when an agent has hundreds of tools - typical once MCP servers are attached - loading every schema into context and round-tripping every call through the model stops scaling, with tool definitions consuming tens of thousands of tokens before the task begins.
The remedy is to present tools as a code API instead of as context: generate typed function stubs for the tool surface on a filesystem the sandbox can see, let the model discover them the way programmers do - list the directory, read the signature it needs - and call them from code, so only the tools actually used ever cost tokens, and intermediate results flow between tools inside the runtime rather than through the transcript.

The same idea ships as a first-party API feature in programmatic tool calling on the Anthropic API (as of early 2026): declare your custom tools with an `allowed_callers` field naming the code execution tool, and the model's sandboxed script can invoke your client-side tools mid-execution, with the container pausing while your harness executes each call and resumes with the result, and only the script's final output entering the model's context.
The through-line from section 7.3 holds: this is CodeAct with the harness still owning tool execution, so the gating and audit properties of typed tools survive inside the code-mode world - the script composes the calls, but each consequential call still crosses an inspectable boundary.

When to reach for which, as a closing heuristic: a handful of tools and short chains - plain tool calling, simplest and most observable; many tools or large intermediates - code mode, paying its complexity for token and latency collapse; consequential side effects - typed gated tools always, whichever mode composes them.

## 7.8 Security posture, consolidated

The rules of this chapter compressed into the checklist you should be able to recite.

Assume the code is hostile, because via prompt injection it eventually will be; the sandbox is the control, and prompts are not controls.
Isolate to the tier the threat model demands: containers for trusted-input internal agents, gVisor or Firecracker for anything touching untrusted input or other tenants.
Default-deny egress and allowlist what the task needs, because exfiltration through legitimate protocols is the top practical risk.
Keep secrets out of the sandbox: inject at the boundary via proxy or broker so code never holds long-lived credentials, a pattern provider platforms now implement natively with vaulted credentials substituted at egress as of early 2026.
Cap resources - CPU, memory, disk, processes, wall clock - so the failure mode of bad code is a bounded observation, per Chapter 5.
Gate consequential side effects through typed tools even in code mode, preserving argument-level policy.
And log executions with their inputs and outputs, because Volume 10's observability starts with having the data.

## Exercises

1. Replace the Chapter 3 bash tool with a persistent Python REPL tool running in a subprocess-managed kernel, including stdout, stderr, final-expression capture, timeout, and an explicit restart command with an honest death observation; demonstrate state persisting across three calls and surviving a deliberate crash-and-restart.
2. Containerize it: run the interpreter in a Docker container with a non-root user, read-only root filesystem, a writable /workspace mount, memory and CPU limits, and no network; verify each restriction with a probe task that tries to violate it, and keep the transcripts.
3. Reproduce a CodeAct comparison: give the same multi-step data task - fetch, filter, aggregate, report over a large CSV - to the JSON-tools agent and to the REPL agent, and measure turns, total tokens, wall time, and context growth for each.
4. Implement the hybrid boundary: expose `read_records` and `compute` freely inside the sandbox, but make `send_report` a typed external tool behind Chapter 6's ask-gate, and confirm the model composes all three with the gate firing exactly once.
5. Port exercise 2 to one cloud sandbox (E2B, Modal, or Daytona) behind your own `execute()` interface, then switch to a second vendor by changing only configuration; document every place the abstraction leaked, with vendor details date-stamped.
6. Build filesystem continuity: persist a sandbox or container id in the Chapter 6 checkpoint, resume a data-analysis trajectory after killing the harness, and implement the verify-on-resume probe that detects when the sandbox was lost and instructs the model to re-establish state from its own earlier setup commands.
7. Demonstrate the injection threat concretely and safely: plant an instruction in a data file that tells the agent to POST a local file to an external URL, run once with unrestricted egress in a throwaway sandbox with a canary token, and once with default-deny egress; write up the kill chain and which controls broke it.
8. Generate a code-mode API for five tools of your own as typed Python stubs on the sandbox filesystem, have the model discover and compose them from code, and compare context cost against loading all five schemas the Chapter 2 way.

## Godhood check

You have mastered this chapter when you can do the following without reference material.

- Argue both halves of the code-execution bargain - tool factory and attack surface - and name the control that addresses each risk you list.
- Specify a production-quality REPL tool: statefulness contract, output capture, error fidelity, restart semantics, and the ACI details that make each reliable.
- Explain the CodeAct pattern, its three structural wins and three structural losses, and state the hybrid synthesis that production systems converged on.
- Place bare process, Docker, gVisor, Firecracker, and Wasm on the isolation spectrum, with the mechanism, cost, and breaking threat model of each tier.
- Run the build-versus-buy decision for sandboxing a given product, and defend the `execute()` abstraction that keeps the decision reversible.
- Explain how code mode and programmatic tool calling resolve tool proliferation, and why typed gated tools still survive inside them for consequential actions.
- Recite the eight-point security posture and apply it as a review checklist to an agent design you have never seen before.
