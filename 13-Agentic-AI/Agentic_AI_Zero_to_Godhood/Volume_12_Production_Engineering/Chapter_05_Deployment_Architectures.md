# Chapter 05 - Deployment Architectures

## What you will master

- The architectural fork between request-scoped and long-running agents, and how it drives every downstream infrastructure choice.
- Serverless constraints (execution time limits, statelessness, cold starts) and where agents do and do not fit inside them.
- Durable execution engines in the Temporal style, and why deterministic replay is the natural substrate for agent loops.
- Queue-and-worker architectures, sandbox infrastructure for code-executing agents as of early 2026, session state storage, and multi-tenant isolation.
- How to pick an architecture by matching session duration, side-effect profile, and tenancy requirements against each option's failure modes.

## 1. The shape of the workload

Deployment architecture follows workload shape, so characterize the workload before choosing infrastructure.
Agent sessions differ from web requests on four axes, and each axis eliminates some architectures.

Duration: sessions run seconds to hours, with heavy-tailed distributions; any platform with a hard execution ceiling must either fit the tail or support checkpoint-and-resume.
State: a session's context, tool results, and pending intents are expensive to lose mid-flight, because replaying a half-finished trajectory re-pays tokens and may re-fire side effects; the architecture must define where session state lives and how it survives process death.
I/O profile: sessions are overwhelmingly wait-time (model calls, tool calls) with brief bursts of local compute, which means process-per-session wastes memory on idle waiting and pushes serious deployments toward async concurrency or externalized waiting.
Blast radius: sessions execute model-authored actions, and code-executing agents run model-authored code, so the architecture must answer "what can a compromised or confused session touch" with infrastructure, not with prompt text (Volume 11 gives the threat model; this chapter gives the containment machinery).

## 2. Request-scoped versus long-running agents

The first fork: does the agent live inside one request-response cycle, or does it outlive it?

### 2.1 Request-scoped agents

The agent loop runs inside the handler of a synchronous request: the user sends a message, the loop executes its steps, the response returns, and no agent state survives except what you persist to the conversation store.
This is the right architecture when sessions are short (seconds to roughly a minute), interactive, and tolerable to lose on failure, because it is radically simpler: no schedulers, no workers, no resume logic; scaling is stateless-horizontal behind a load balancer; a crash costs one turn, and the user retries.
Most chat-plus-tools products ship this way and should.
Its limits are exactly its simplicity: the session is hostage to the connection (a dropped websocket or a client tab close kills the trajectory), the platform's request timeout caps trajectory length, and there is no story for work that continues after the user leaves.
Teams outgrow it the day the product roadmap says "run this in the background" or the trace data shows sessions bumping the timeout, and the migration is architectural, not incremental, so recognize the trigger early.

### 2.2 Long-running agents

The agent's lifetime is decoupled from any request: work is accepted, executed asynchronously by infrastructure that survives client disconnects and process restarts, and results are delivered by notification, polling, or a reconnecting stream.
Everything in the rest of this chapter (queues, durable execution, sandboxes, session stores) exists to serve this shape.
The cost of the decoupling is the whole distributed-systems bill: durable state, at-least-once execution with the idempotency machinery of Chapter 04, progress streaming across a queue boundary, and an operational surface with many more moving parts.
The middle ground worth naming is the hybrid: interactive turns run request-scoped for latency, and the moment a turn's plan exceeds a duration threshold the orchestrator promotes it to a background job with the same session id, which keeps the common path simple and reserves the heavy machinery for the sessions that need it.
The hybrid's downside is two code paths through the loop, which must be kept behaviorally identical or bugs will live only in the less-traveled one.

## 3. Serverless constraints

Functions-as-a-service platforms (AWS Lambda and its peers) are attractive for agents because agent traffic is spiky and serverless bills per use, but the constraints are structural, not incidental.

Execution time limits: as of early 2026, Lambda caps invocations at 15 minutes, and edge platforms are far tighter; notably, several platforms (Cloudflare Workers among them) meter CPU time rather than wall-clock time, which suits agents unusually well because a session that spends 10 minutes awaiting model responses consumes only seconds of CPU.
A request-scoped agent with short sessions fits comfortably; a long-horizon agent hits the ceiling mid-trajectory, and "checkpoint before the deadline and re-invoke" is a hand-rolled durable-execution engine, at which point you should use a real one (section 5).

Statelessness: any invocation may land on a fresh instance, so all session state must externalize to a store between steps (section 7), adding a read-write round trip per step; this is the correct discipline anyway, so it is a forcing function more than a penalty.

Cold starts: hundreds of milliseconds to seconds of init on scale-up, hitting exactly when traffic spikes; tolerable inside a multi-second agent turn, painful for the sub-second guardrail and routing calls, which argues for keeping tiny fast paths on provisioned capacity.

Connection shapes: long-lived streaming from a function is platform-dependent and capped; the robust pattern routes streams through a dedicated gateway layer while functions do the step work.

The honest summary: serverless is a good execution substrate for individual steps and short sessions, and a poor substrate for being the memory or the scheduler of long trajectories; the architectures that work pair serverless step-executors with an external scheduler or durable engine that owns the trajectory.

## 4. Queues and workers

The workhorse architecture for long-running agents: an API tier accepts and validates work, writes it to a durable queue, and a fleet of workers pulls sessions, runs the loop, and persists results.
Chapter 04 covered the reliability semantics (visibility timeouts, heartbeats, dead-letter queues, poison messages); here, the deployment concerns.

Worker sizing follows the I/O-heavy profile: sessions spend most wall-clock time awaiting model and tool responses, so async workers running tens of concurrent sessions per process are the efficient shape, with per-worker concurrency capped by memory (contexts are large) and by your provider rate-limit allocation (Chapter 06) rather than by CPU.
Scale on queue depth and age, not CPU: the autoscaling signal that matters is "oldest message age exceeds the latency SLO for its priority class," because CPU on an I/O-bound fleet is uninformative.
Separate queues per priority and per workload class (interactive-promoted, scheduled, batch), because a single queue lets a batch backfill starve user-visible work, and per-class queues also give you per-class concurrency limits, which is how you enforce that evals never consume the rate limit budget of production traffic.
Drain and deploy: workers must finish or checkpoint in-flight sessions on SIGTERM, so deployment rollouts need termination grace periods sized to the checkpoint interval, not to the session length; this is one of the strongest practical arguments for checkpointing even in a plain queue-worker design.

The limitation that queues alone do not fix: the worker crash mid-session still loses in-memory trajectory state, and recovery replays the whole session from the queue message unless the loop checkpoints its own progress.
Hand-rolled checkpointing (persist context after every step, resume from last checkpoint on redelivery) works and is widely deployed, but it is exactly the problem durable execution engines solve properly.

## 5. Durable execution engines

Durable execution (Temporal is the canonical engine; Restate, Inngest, and cloud-native options like AWS Step Functions occupy the same space as of early 2026) makes a workflow's progress survive any process death by recording every step's inputs and outputs in an event history and reconstructing state by deterministic replay.
The programming model splits code into workflow logic, which must be deterministic and is replayed freely, and activities, which perform real-world I/O, execute at-least-once, and have their results recorded so replay reuses recorded results instead of re-executing.

The fit with agent loops is unusually clean, which is why this pairing became a default pattern for serious agent backends by 2025.
The agent loop is the workflow; every model call and every tool call is an activity.
A worker crash at step 23 resumes by replaying the history: steps 1 through 22 return their recorded results instantly with no re-execution and no re-billed tokens, and execution continues at step 23.
Timers, retries with backoff, and heartbeats are engine primitives rather than your code.
Human-in-the-loop gates fall out naturally: a workflow awaiting an approval signal consumes no worker resources and can wait days, which makes the maturity-ladder checkpoints of Chapter 01 nearly free to implement.
And the event history doubles as a perfect trajectory audit log.

The costs and sharp edges, stated with equal weight.
Determinism discipline: workflow code cannot use wall-clock time, randomness, or direct I/O (all must go through engine APIs or activities), and violating this produces replay divergence errors that are unfamiliar and unpleasant to debug; model calls are nondeterministic by nature and must always be activities, never inlined.
History size limits: engines cap event history, and chatty agent loops with large payloads hit the caps, so the standard pattern stores large contexts and tool results in a blob store and passes references through the history, plus continue-as-new to reset history for very long sessions; this is boilerplate you will write.
Versioning: changing workflow code while sessions are in flight requires the engine's versioning machinery, because replay of an old history against new code diverges; this makes prompt and loop changes a deployment-coordination problem you did not have before.
Operational weight: a self-hosted Temporal cluster is a serious distributed system to run (managed offerings shift this to a bill), and the whole apparatus is overkill for request-scoped products.
The decision rule: adopt durable execution when sessions are long or expensive enough that replaying them on failure is unacceptable, when human-gated pauses are core to the product, or when side-effect bookkeeping (Chapter 04, section 9) would otherwise be hand-rolled; stay on plain queue-workers below that line.

## 6. Sandbox infrastructure for code-executing agents

An agent that writes and runs code needs an execution environment that assumes the code is hostile, because model-authored code is untrusted by construction: it can be wrong, and under prompt injection it can be adversarial.
The isolation requirement is a hard security boundary (VM or equivalent), not a shared-kernel container alone, plus egress control (the exfiltration path runs through the network), resource quotas (CPU, memory, disk, wall-clock), and disposability (fresh environment per session, destroyed after).

The landscape as of early 2026, by mechanism rather than by marketing.
E2B provides purpose-built agent sandboxes on microVM-class isolation with SDK-first ergonomics (create a sandbox, run code, read files, destroy), sub-second-to-seconds start times from templates, and persistence options for pause-and-resume; it is the most direct "sandbox as a product" option.
Modal is a serverless compute platform (originally for data and ML workloads) whose fast container-plus-gVisor-style isolation, image caching, and Python-native APIs make it a strong general executor for agent-spawned jobs, especially compute-heavy ones like test suites and data processing.
Fly Machines are fast-launching Firecracker microVMs with full-VM flexibility (any image, any listening ports, real regions), suited to per-session sandboxes that need to look like real servers, at the cost of doing more assembly (snapshotting, pooling, egress policy) yourself.
Cloudflare Workers plus Containers pair V8-isolate-speed control logic at the edge with container-backed sandboxes for arbitrary code, attractive when your orchestrator already lives at the edge; the container side of the platform is the newest of the group and its ergonomics are correspondingly less settled.
Roll-your-own on Firecracker or gVisor buys maximum control and minimum marginal cost at scale, and pays a permanent platform-engineering tax (image pipelines, pool management, egress proxies, patching); it is the right answer for large fleets and the wrong answer for a team of five.

The engineering concerns common to every choice.
Cold start versus pooling: boot times range from sub-second to tens of seconds depending on image size and platform, and a warm pool converts that to near-zero at the price of paying for idle capacity (the Chapter 02 speculation trade, again).
Snapshot and resume: long agent sessions want to pause the sandbox with the session and resume it later; platform support for filesystem or memory snapshots varies and often defines the choice for session-centric products.
Egress policy is the security load-bearing wall: default-deny with an allowlist proxy, because a sandbox that can POST anywhere makes every secret in its environment exfiltrable the moment injection succeeds; correspondingly, keep real secrets out of sandboxes entirely and broker privileged calls through your API tier.
Cost model: per-second billing of idle-but-open sandboxes quietly dominates spend for long sessions, so aggressive idle-pause policies are a cost feature.

## 7. Session state storage

Long-running architectures externalize session state; the design question is what the state is and where each part lives.

Inventory the state first: the conversation and trajectory (messages, tool calls and results, the append-only record), working artifacts (files, notebooks, sandbox filesystems), control state (step index, pending intents, idempotency records, budgets consumed), and derived memory (summaries, embeddings, extracted facts; Volume 06's territory).

The storage mapping that recurs across production systems.
Trajectory and control state belong in a transactional document or relational store (Postgres with JSONB is the boring, correct default), because control state needs the atomicity that Chapter 04's outbox and idempotency patterns require, and trajectories need ordered, append-only writes with read-back on resume; if you use a durable execution engine, it owns control state and much of the trajectory, and the store holds what the history should not (large payloads, by reference).
Large artifacts belong in object storage with references in the trajectory, both for cost and because history and row-size limits punish inline blobs.
Hot working state (the streaming buffer, the live context assembly) can live in cache-tier stores (Redis-class) with the durable store as source of truth, an optimization to add when read latency on resume actually hurts, not before.
Derived memory belongs in whatever serves its query pattern (vector store, search index), and must be rebuildable from the trajectory, which keeps it out of the recovery-critical path.

Two disciplines matter more than the store choice.
Write ordering: persist the intent before the side effect and the outcome after it (the Chapter 04 contract), and persist the trajectory append before acknowledging the step, so that the store is always at or behind reality by a known, recoverable margin, never ahead in an ambiguous way.
Retention and privacy: trajectories are conversation data with everything that implies (PII, tenant confidentiality, deletion obligations); define retention windows, encrypt per tenant where the product warrants it, and make deletion actually reach object storage, caches, and derived indexes, because "we deleted the row" is not deletion.

## 8. Multi-tenant isolation

Agent platforms concentrate risk in one process: a worker holds tenant A's context in memory while executing model-authored actions that could touch tenant B's data.
Isolation must therefore be enforced at every layer where tenants could meet, and the enforcement must not depend on the model behaving.

Data plane: every query to the session store, memory store, and artifact store carries the tenant id from an authenticated context, enforced by the storage layer (row-level security or per-tenant schemas or databases), never assembled from model output; a tool that accepts "which customer" as a model-provided string and queries with it is a cross-tenant read vulnerability by design.
Tool credentials: tools execute with the tenant's scoped credentials injected by the orchestrator per session, not with a platform-wide service account, so a confused agent is limited to the blast radius of one tenant's permissions; this is the single highest-value isolation decision in the architecture.
Execution plane: sandboxes are per session and never shared across tenants; for non-sandboxed workers, tenant contexts share a process, so treat worker memory as a boundary you must not let tools or logging cross (no global mutable state keyed by anything but session id, and log scrubbing per tenant).
Model plane: prompts must never mix tenant contexts (a shared few-shot cache accidentally containing tenant A's example in tenant B's prompt is a breach), and per-tenant prompt caches at the provider are isolated by construction as of early 2026, but your own context-assembly caches are not unless you key them by tenant.
Noisy neighbors: per-tenant concurrency limits, rate-limit allocations, and cost quotas (Chapter 06) are isolation too, because one tenant's runaway batch consuming the shared provider quota is a cross-tenant availability failure.

Enterprise deployments push isolation further along a spectrum: shared everything with logical isolation (the default above), per-tenant queues and worker pools (bounded blast radius, higher idle cost), through single-tenant deployments (maximum isolation, maximum operational multiplication); price each rung honestly, because every step right multiplies operational surface by tenant count.

## 9. Choosing an architecture

A decision sequence that covers most real products.

1. If sessions are interactive and reliably under about a minute, ship request-scoped stateless services, persist conversations, and stop; every additional pattern in this chapter is a cost you do not yet need.
2. When background work or longer trajectories arrive, add the queue-and-worker tier with per-class queues and hand-rolled per-step checkpointing, promoting sessions across the hybrid boundary rather than rewriting the interactive path.
3. When session loss becomes expensive (long trajectories, human-gated pauses, side-effect bookkeeping), put the loop on a durable execution engine, with model and tool calls as activities and large payloads referenced from blob storage.
4. If the agent executes code, add per-session sandboxes on a platform chosen by your start-time, snapshot, and control requirements (E2B for product-shaped sandboxing, Modal for compute-heavy jobs, Fly Machines for VM-shaped flexibility, Cloudflare for edge-centric stacks, Firecracker-your-own at fleet scale, as the landscape stands in early 2026), with default-deny egress and no real secrets inside.
5. Apply tenancy isolation from the first multi-tenant day: tenant-scoped credentials for tools, storage-layer tenant enforcement, and per-tenant quotas; retrofitting isolation is the most painful migration on this list.
6. Revisit annually: platform limits, sandbox start times, and engine ergonomics are all moving as of early 2026, and an architecture chosen against last year's constraints may be carrying unnecessary weight.

## Exercises

1. Take a concrete product (an agent that researches and drafts responses to customer tickets, with a human approval gate before sending) and produce two full architecture diagrams: request-scoped-plus-queue-hybrid, and durable-execution-based. Annotate every arrow with what is persisted, what is replayed on crash, and where tokens are re-billed on recovery, then write the one-paragraph recommendation.
2. Design the checkpoint format for a hand-rolled queue-worker agent: exactly what is persisted after each step, the resume algorithm on redelivery, and the interaction with the idempotency keys of Chapter 04. Identify the crash windows where a step re-executes and show your format makes them safe.
3. Write the workflow-versus-activity partition for an agent loop on a Temporal-style engine: list ten operations the loop performs (model call, tool call, compaction, budget check, approval wait, and so on) and classify each as workflow logic or activity, with a one-sentence justification citing determinism or I/O.
4. Specify the sandbox policy for a code-executing data-analysis agent: platform choice with rationale against two alternatives, image contents, resource quotas, egress allowlist, secret-brokering design, idle-pause policy, and the per-session cost drivers you would monitor.
5. Threat-model tenancy for a shared-worker deployment: enumerate five concrete cross-tenant failure paths (data plane, credentials, prompt assembly, caches, quotas), and for each, name the enforcing layer and write the test that would catch a regression.

## Godhood check

You have mastered this chapter when you can classify any agent product into request-scoped, hybrid, queue-worker, or durable-execution territory from its session-duration distribution and side-effect profile, and defend the classification against both the simpler and the heavier alternative.
You can state the serverless constraints from memory (time ceilings, statelessness, cold starts, CPU-versus-wall-clock billing) and say which agent components they fit.
You can explain deterministic replay, why model calls must be activities, what continue-as-new and payload-by-reference exist to solve, and what the determinism and versioning disciplines cost.
You can design a sandbox setup with a real isolation boundary and default-deny egress on a current platform, map every category of session state to its correct store with the write-ordering discipline intact, and enumerate the tenant-isolation enforcement points without leaning on model behavior for any of them.
