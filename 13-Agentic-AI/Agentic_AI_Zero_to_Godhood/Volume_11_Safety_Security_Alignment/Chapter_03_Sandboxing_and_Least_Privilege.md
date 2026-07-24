# Chapter 03 - Sandboxing and Least Privilege

## What you will master

- Why least privilege is the load-bearing defense for agents: since you cannot stop the model from being fooled, you constrain what a fooled model can reach.
- Permission systems for agents: allowlists, approval prompts, and autonomy levels, and how to choose the granularity that matches the trust in the context.
- Filesystem and network isolation, and the concrete containment ladder from process sandboxes to containers to microVMs, with the security-versus-overhead trade at each rung.
- Egress control as the specific defense against the trifecta's external-communication leg.
- Credential handling that assumes compromise: short-lived tokens, scoped keys, secret managers, and never handing the model a standing superuser credential.
- The organizing principle: capability follows trust in the context.

This chapter is the constructive counterpart to Chapter 02.
Injection is unsolved, so this is where you make that survivable by bounding blast radius.
Details are current to early 2026; the containment technologies named here are stable, but their exact security properties and defaults evolve, so re-verify before relying on a specific claim.

## 1. The principle: capability follows trust in the context

State the organizing rule first, because every technique in this chapter is an instance of it.

**An agent's capabilities in any given context should be no greater than the trust level of the least-trusted content in that context.**

The moment an agent's context includes untrusted content - a fetched web page, a received email, a third-party tool result - you must treat the agent, for that context, as potentially controlled by the author of that content.
Its capabilities in that context should therefore be exactly the capabilities you would grant that author.
You would not give a stranger who emails your user the ability to read your secrets and make arbitrary network calls, so the agent that reads that stranger's email must not have those capabilities either while that email is in its context.

This reframes least privilege for agents specifically.
Classic least privilege says give each component the minimum authority it needs.
The agent version adds a time-and-context dimension: the minimum authority depends on what is in the context right now, because the effective actor might be whoever wrote the untrusted content.
A single agent that reads trusted internal data in one phase and untrusted web content in another should not hold both capabilities in a context that mixes them.
This is why the strong architectures in Chapter 02 split contexts, and why the strong architectures in this chapter split credentials by phase.

## 2. Permission systems

The harness, not the model, decides what actions are permitted, because the harness is code and the model is not a trust boundary (Chapter 01).
There are three primitives, and mature systems use all three.

### 2.1 Allowlists

An allowlist enumerates exactly what is permitted and denies everything else.
Denylists (block these bad things) are the wrong default, because you cannot enumerate all bad things and attackers live in what you forgot; allowlists fail closed.

Allowlist at several layers:

- **Which tools exist.** The agent can only call tools you registered. This is the coarsest and most important lever: an agent with no send_email tool cannot send email no matter how it is injected.
- **Which arguments are legal.** Within a tool, constrain the arguments. A file-read tool restricted to a specific directory. An HTTP tool restricted to specific domains. A shell tool restricted to a fixed set of commands, or replaced entirely by narrow purpose-built tools.
- **Which resources are reachable.** Which files, which hosts, which database rows, scoped by the acting user's authorization, not the agent's.

The design discipline is to make tools narrow and specific rather than general and powerful.
A general `run_shell(command)` tool is an arbitrary-code-execution primitive handed to a probabilistic component that reads attacker text; a specific `run_tests()` tool that executes a fixed command is far safer and usually sufficient.
The cost of narrow tools is more of them and less flexibility, and sometimes you genuinely need a shell, which is exactly when the sandboxing in section 3 becomes mandatory rather than optional.

### 2.2 Approval prompts

For actions too consequential to automate, the harness pauses and asks a human to approve before executing.
This inserts a human trust boundary in front of the dangerous action, which is the only kind of boundary the model cannot argue past.

The engineering questions, developed fully in Chapter 06, are which actions require approval and how to avoid approval fatigue.
The short version: gate irreversible and high-blast-radius actions (spending money, deleting data, sending external communications, granting access) and auto-approve reversible, low-impact ones (reading a file, running a read-only query in a sandbox).
The failure mode is prompting for everything, which trains the human to click approve reflexively, converting the gate into a rubber stamp that provides false assurance.

### 2.3 Autonomy levels

Rather than a per-action decision, define discrete autonomy levels the operator selects based on trust and stakes.
A common ladder:

- **Suggest only.** The agent proposes actions; a human executes each. Highest safety, lowest throughput. Appropriate for high-stakes or early-deployment settings.
- **Approve each action.** The agent executes after per-action human approval. The approval-gate model above.
- **Approve risky actions.** The agent auto-executes low-risk actions and gates only the risky ones. The common production balance.
- **Autonomous within a sandbox.** The agent runs freely inside strong containment (section 3) with tight egress and credentials, and humans review after the fact via audit trails (Chapter 06). Appropriate only when the sandbox genuinely bounds the blast radius.
- **Fully autonomous.** No gates. Appropriate only when the agent provably cannot cause harm exceeding your tolerance, which in practice means the trifecta is broken by construction.

The level should track the trust in the context and the stakes of the actions, and it should be able to drop automatically when untrusted content enters the context.
A defensible pattern: an agent runs at "approve risky actions" normally, but the harness demotes it to "suggest only" for the remainder of a session once it ingests content from an untrusted source, because the effective actor may now be the content's author.

## 3. Isolation: the containment ladder

When an agent runs code, executes shell commands, or processes untrusted files, you assume that execution can be hostile - because injection can make it hostile - and you contain it.
There is a ladder of containment strength, each rung stronger and heavier than the last.
Pick the lowest rung that bounds your actual blast radius, because stronger isolation costs latency, complexity, and operational overhead.

### 3.1 Process-level sandboxing

The lightest containment: run the risky code as a separate OS process with restricted privileges.
Mechanisms include OS sandboxing primitives (seccomp-bpf to restrict syscalls on Linux, AppArmor or SELinux profiles, the macOS sandbox), dropping privileges to an unprivileged user, `chroot` or mount namespaces to limit the visible filesystem, and resource limits (rlimits, cgroups) to cap CPU, memory, and process count.

Strength: cheap and fast, no virtualization overhead.
Weakness: it shares the host kernel, so a kernel vulnerability or a misconfigured profile is a full escape, and building a correct syscall policy by hand is error-prone.
Appropriate when the code is semi-trusted and you want defense in depth rather than a hard security boundary against determined attackers.

### 3.2 Containers (Docker and friends)

Containers package the process with its dependencies and isolate it using kernel namespaces and cgroups.
They are the default unit of agent execution isolation in early 2026 because the tooling is ubiquitous and the developer experience is good.

Security posture: better than a bare process, because namespaces isolate the filesystem, network, and process views, and you can drop capabilities, run as non-root, mount filesystems read-only, and disable networking.
But a standard container shares the host kernel, so the security boundary is the kernel's namespace and cgroup implementation, and container escapes via kernel vulnerabilities are a real and recurring class.
A default Docker container is a convenience boundary, not a hostile-code boundary; hardening (drop all capabilities, no-new-privileges, read-only root, seccomp profile, non-root user, no host mounts) narrows the gap but does not close it against a kernel exploit.

Treat containers as a strong operational boundary and a moderate security boundary, and reach for the next rung when you must run genuinely untrusted code.

### 3.3 User-space kernels: gVisor

gVisor, from Google, interposes a user-space kernel between the sandboxed process and the host kernel.
The sandboxed process's syscalls are intercepted and handled by gVisor's reimplementation of the kernel interface, so the process almost never touches the real host kernel directly, shrinking the attack surface dramatically.

Trade-off: stronger isolation than a plain container with a container-like developer experience, at the cost of some performance overhead (syscall interception is not free) and imperfect compatibility (some syscalls and workloads are unsupported or slow).
It is a strong choice when you need better-than-container isolation without full virtualization, and it is used in practice for running untrusted code such as agent-generated programs.

### 3.4 MicroVMs: Firecracker

Firecracker, from AWS, runs each workload in a lightweight virtual machine with its own guest kernel, using hardware virtualization, but stripped down for fast boot (tens of milliseconds) and low memory overhead.
Because each microVM has its own kernel and is isolated by the hypervisor and hardware virtualization extensions, the boundary is far stronger than kernel-namespace isolation: an escape requires breaking the hypervisor or the hardware boundary, a much higher bar than a kernel-namespace bug.

Firecracker is what powers serverless platforms that run arbitrary customer code (AWS Lambda, Fly.io, and code-execution backends for agent platforms) precisely because it gives VM-grade isolation at near-container speed.
Trade-off: more operational complexity than a container, a real (if small) resource overhead per VM, and the need to manage guest images and kernels.
This is the right rung when the agent runs genuinely untrusted code and a container escape is an unacceptable risk.

### 3.5 Choosing a rung

The decision is a function of how untrusted the executed code is and how much a breach would cost:

- Semi-trusted internal code, defense in depth wanted: process sandbox or hardened container.
- Untrusted code, moderate stakes, want good DX: gVisor.
- Untrusted code, high stakes, escape is unacceptable: Firecracker microVM.
- Full VMs or physical isolation: only when regulatory or extreme-risk requirements demand it, since the overhead is large.

Whatever the rung, containment is only half the job; a sandbox that can still reach the network or hold live credentials leaks despite perfect isolation, which is why sections 4 and 5 exist.

## 4. Egress control

Egress control governs what the agent can send out, and it is the specific, decisive defense against the trifecta's third leg.
If the agent cannot reach an attacker-controlled destination, injection cannot exfiltrate even if it fully controls the model.

Techniques, from coarse to fine:

- **No network at all.** The strongest option. If the sandboxed task does not need the network, disable it entirely (`--network none` on a container, no interface in the VM). Injection has no channel out.
- **Egress allowlist.** Permit outbound connections only to a specific set of hosts required for the task, deny everything else, enforced by an egress proxy or firewall the agent cannot reconfigure. The attacker cannot reach `attacker.example` because it is not on the list.
- **Egress proxy with inspection.** Route all outbound traffic through a proxy that logs, filters, and can block based on destination, content, or data patterns. This is also where you scan for exfiltration shapes (encoded secrets, unexpected domains).
- **DNS control.** Restrict or monitor DNS resolution, since DNS itself is an exfiltration channel (encoding data in subdomain lookups). An egress allowlist that ignores DNS is incomplete.

The enforcement point must be outside the agent's control.
An egress rule the agent can rewrite by calling a tool is not a control.
This means the proxy, firewall, or network namespace configuration lives in the harness or infrastructure layer, and the agent has no tool that can modify it.

Egress control interacts with the trifecta directly: it is the mechanism by which you remove leg 3.
Remember from Chapter 02 that leg 3 includes rendering surfaces, so egress control at the network layer must be paired with output filtering at the rendering layer (Chapter 02 section 6.2); a network egress allowlist does not stop a Markdown image URL that the user's own browser fetches outside the sandbox.
Close both, or you have closed neither.

## 5. Credential handling

The agent's credentials are assets (Chapter 01) and are the second half of blast-radius control.
Even with perfect isolation and egress control, an agent holding a standing superuser token that it can be injected into misusing is a live threat, because the token grants authority the sandbox does not revoke.
Handle credentials assuming the agent will, at some point, be fully controlled by an attacker.

### 5.1 Never hand the model a standing superuser credential

The cardinal rule.
Do not give the agent a long-lived, broadly scoped API key, database superuser, or admin token, because injection turns that credential into the attacker's credential.
Every credential the agent can access is a credential the attacker can use if they win the injection contest, and Chapter 02 says they sometimes will.

### 5.2 Short-lived tokens

Use credentials that expire quickly - minutes, not months.
Short-lived tokens (OAuth access tokens with brief lifetimes, STS session credentials on AWS, workload identity tokens) bound the window in which a stolen credential is useful and force re-authorization, which is a natural place to re-check policy.
The cost is token-refresh plumbing and handling expiry mid-task, which is minor compared to the risk it removes.

### 5.3 Scoped keys

Every credential should carry the minimum scope for the task at hand.
A token that can read one user's calendar, not all calendars.
A database role that can read the three tables the task needs, not the whole schema, and cannot write.
Scope by the acting user's authorization, so the agent can never reach data the user themselves could not, which defends against the malicious-user adversary from Chapter 01.

The anti-pattern is the agent holding one powerful service credential and using it on behalf of every user, because then a low-privilege user (or an injection in their content) can reach high-privilege data through the agent - a confused-deputy escalation. Bind the credential to the request's authorization context instead.

### 5.4 Secret managers and no secrets in the context

Store credentials in a secret manager (HashiCorp Vault, AWS Secrets Manager, cloud KMS-backed stores), fetch them at the point of use in the harness, and inject them into the tool call's execution, never into the model's context.
The model does not need to see the API key to use a tool that needs the key; the harness holds the key and the model holds a reference.
A secret that never enters the context cannot be exfiltrated by an injection that controls the model's output, which closes an entire leak path by construction.
This is the same principle as parameterizing credentials out of the reasoning channel: the powerful component (harness) holds the secret, the instructable component (model) never sees it.

### 5.5 Rotation, revocation, and audit

Assume compromise and plan for it.
Rotate credentials on a schedule and immediately on any suspected incident.
Ensure every credential can be revoked centrally and fast, so a detected compromise can be contained in seconds.
Log every credential use with enough context to reconstruct what the agent did with it (Chapter 06 audit trails), because you cannot investigate what you did not record.

## 6. Putting it together: a defense-in-depth stack

A well-secured code-executing agent as of early 2026 layers all of the above, and the layering is the point, because each layer assumes the others sometimes fail.

- The agent's tools are a narrow allowlist; there is no general shell, or if there is, it runs only inside the sandbox.
- Risky and irreversible actions require human approval; autonomy level drops automatically when untrusted content enters the context.
- Code execution happens in a Firecracker microVM or gVisor sandbox, not on the host, with resource limits.
- The sandbox has no network, or egress restricted to an allowlist enforced by a proxy the agent cannot reconfigure.
- Rendered output is filtered for auto-fetch vectors, closing the rendering-surface external channel.
- Credentials are short-lived, scoped to the acting user, fetched from a secret manager at point of use, and never placed in the model's context.
- Everything is logged for audit, and every credential can be revoked centrally in seconds.

Run the trifecta test against this stack.
Leg 1 is scoped to the acting user so injection cannot reach others' data.
Leg 3 is closed at both the network layer (egress) and the rendering layer (output filter).
Even a fully injected model is contained: it can act only within a narrow, sandboxed, egress-controlled, short-credentialed envelope, and a human gates anything irreversible.
This is what "make injection survivable" looks like in concrete engineering, and it is the constructive answer to Chapter 02's unsolvability.

## 7. Claims that will rot

The principle - capability follows trust in the context - and the containment ladder's shape are stable and durable.
The specific security properties, defaults, performance figures, and even the existence of particular products (Docker hardening flags, gVisor and Firecracker capabilities, cloud secret managers) are current to early 2026 and evolve; re-verify a claim before you build a boundary on it.
Container-escape and hypervisor-escape research is continuous, so the relative strength of the rungs is stable but the absolute safety of any one is a moving target.

## Exercises

1. Take an agent with a general `run_shell` tool and redesign it with a narrow allowlist of purpose-built tools. List which shell uses you replaced and which genuinely require a sandboxed shell.
2. For a code-executing agent, choose a containment rung (process sandbox, container, gVisor, Firecracker) and justify it in terms of how untrusted the code is and the cost of an escape. State the overhead you accept.
3. Design the egress policy for an agent that must call exactly two external APIs and nothing else. Specify the enforcement point and prove the agent cannot reconfigure it.
4. Find a place in a system you run where the model's context contains, or could contain, a credential. Redesign so the harness holds the credential and the model holds only a reference.
5. Write the autonomy-level demotion rule for an agent: define the trigger (untrusted content ingested), the new level, and how the harness enforces it for the rest of the session.

## Godhood check

You have mastered this chapter when you can:

- State the "capability follows trust in the context" principle and use it to decide an agent's permitted actions given a description of its current context.
- Choose among allowlists, approval prompts, and autonomy levels for a given action and defend the granularity against approval fatigue on one side and unbounded blast radius on the other.
- Place a workload on the containment ladder from process sandbox to Firecracker, naming the security-versus-overhead trade at the rung you pick and the escape class you are accepting.
- Explain egress control as the removal of the trifecta's third leg, including why it must be enforced outside the agent and paired with rendering-layer output filtering.
- Design credential handling that survives full agent compromise: short-lived, scoped to the acting user, secret-manager-held, and never in the model's context, and explain why each property matters.
