# Volume 12 - Production Engineering

How to run an agent in front of real users without it being slow, expensive, unreliable, or quietly wrong.
This volume is the engineering discipline between a demo that works and a system that keeps working.

The spine of the argument runs across all seven chapters.
Per-step reliability compounds over a trajectory, so agents fail in ways single-call systems do not, and shipping requires SLOs that cover semantic failure and autonomy scoped to measured reliability (Chapter 01).
The three resources you spend against that gap are time, money, and capacity, each with agent-specific structure: latency is dominated by loop iterations rather than single calls (Chapter 02), cost grows roughly quadratically in session length because context accumulates (Chapter 03), and capacity is a provider-granted token budget rather than a server count (Chapter 06).
Holding it together are the failure-handling patterns re-derived for probabilistic systems (Chapter 04), a deployment substrate matched to the session shape and side-effect profile (Chapter 05), and the operational practice that keeps quality from drifting once real traffic arrives (Chapter 07).

Content is current to early 2026.
Provider limits, model lineups, pricing structures, deprecation windows, and serving-stack capabilities move fast, so date-stamped claims should be re-verified before you rely on them, and no pricing or benchmark figures in this volume should be treated as current numbers.

## Chapters

- **[Chapter 01 - From Demo to Production](Chapter_01_From_Demo_To_Production.md)** - Why the agent demo-to-production gap is wider than for any previous class of software, the compounding math of per-step reliability, the recurring failure modes, SLOs for semantic failure, and the maturity ladder that scopes autonomy to measured reliability.
- **[Chapter 02 - Latency Engineering](Chapter_02_Latency_Engineering.md)** - Where time actually goes in an agent request (queueing, prefill, decode, tools, orchestration), why time-to-first-token and completion time are separate problems, streaming and its limits inside a loop, model tiering and parallelism as structural wins, and the psychology of perceived latency.
- **[Chapter 03 - Cost Engineering](Chapter_03_Cost_Engineering.md)** - The token economics of agent loops and the quadratic growth of session cost, prompt caching as the dominant lever, routing and output discipline and batch APIs, cost observability attributed to sessions and features, and unit economics that survive heavy users.
- **[Chapter 04 - Reliability Patterns](Chapter_04_Reliability_Patterns.md)** - Failure classification, retries with full jitter and session-scoped budgets, deadline propagation for composable timeouts, idempotency keys derived from trajectory position, the degradation ladder with circuit breakers, queue-based decoupling, and the honest limits of exactly-once for side-effecting tools.
- **[Chapter 05 - Deployment Architectures](Chapter_05_Deployment_Architectures.md)** - The fork between request-scoped and long-running agents, serverless constraints and where agents do not fit them, durable execution and deterministic replay, queue-and-worker designs, sandbox infrastructure, session state storage, and multi-tenant isolation.
- **[Chapter 06 - Capacity and Quotas](Chapter_06_Capacity_and_Quotas.md)** - Rate-limit shapes (RPM, TPM, ITPM, OTPM, concurrency) and which binds an agent workload, quota tiers and dedicated capacity, priority-ordered load shedding, capacity planning arithmetic for spiky heavy-tailed demand, multi-provider strategies and their real ongoing costs, and self-hosted open-weight serving as a capacity lever.
- **[Chapter 07 - Operations](Chapter_07_Operations.md)** - Four-layer monitoring where quality and safety signals stand beside uptime, incident response for model misbehavior including the "what changed" triage and frozen-eval replay, model version migrations with pinning and migration evals, prompt change management with canary and rollback, on-call design for AI systems, and postmortems whose root cause is a distribution rather than a defect.

## How to read this volume

Read Chapter 01 first; it establishes the compounding-reliability argument and the SLO vocabulary that every later chapter uses.
Chapters 02, 03, and 06 form the resource-economics group (time, money, capacity) and are best read in that order, since capacity planning consumes the token telemetry that cost engineering builds.
Chapters 04 and 05 are the systems group and can be read in either order, though 04's failure taxonomy makes 05's architecture trade-offs easier to evaluate.
Chapter 07 is best read last, once you know what there is to monitor, roll back, and postmortem.

## Related volumes

- Volume 03 covers the agent loop whose iteration count drives latency, cost, and capacity alike.
- Volume 06 covers context engineering and compaction, the direct lever on the quadratic cost growth of Chapter 03.
- Volume 07 covers multi-agent systems, whose fan-out multiplies concurrency and spend.
- Volume 10 covers evaluation and observability, the measurement substrate this volume operates on top of; Chapters 01, 05, and 07 there map most directly onto Chapter 07 here.
- Volume 11 covers safety and security, which defines the controls whose firing this volume instruments and whose incidents take a different response path.
- Volume 13 covers coding agents and computer use, the deployments that stress sandbox infrastructure and long-session architectures hardest.
