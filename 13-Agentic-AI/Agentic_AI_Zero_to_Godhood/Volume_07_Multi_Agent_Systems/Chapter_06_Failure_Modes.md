# Chapter 06 - Failure Modes

## What you will master

- The MAST taxonomy from the Berkeley multi-agent failure study: specification failures, inter-agent misalignment, and verification failures, with all fourteen modes.
- Miscoordination case studies: how duplicated work, dropped work, and conflicting decisions actually arise in traces.
- Cost blowup dynamics: delegation loops, runaway fan-out, and context-inflation spirals.
- How to debug distributed agent traces when the symptom is far from the cause.
- The mitigation toolkit: narrow interfaces, single-writer discipline, checkpoints, budgets, and structural caps, with the cost of each.

## 1. Why multi-agent failure deserves its own taxonomy

Single-agent failures (hallucination, tool misuse, context rot) are covered across Volumes 03 and 06, and multi-agent systems inherit all of them.
What multi-agent systems add is a second stratum: failures of the composition, where every individual agent behaves locally-reasonably and the system still fails.
This mirrors distributed systems generally: process crashes are old news, and the interesting bugs live in the protocols between processes.
The stratum matters practically because its failures are invisible to per-agent evals: each agent scores fine in isolation, and the defect only exists in the interaction, which is why Volume 10's system-level evaluation is non-negotiable for multi-agent work.

The empirical anchor for this chapter is the MAST study ("Why Do Multi-Agent LLM Systems Fail?", Cemri et al., Berkeley, 2025), which analyzed traces from seven popular multi-agent frameworks across more than two hundred tasks, derived a fourteen-mode taxonomy through grounded coding with human annotators, and validated an LLM-based annotator against them at high inter-rater agreement.
Two headline findings frame everything below.
First, failure was not rare: several popular systems failed on a large fraction of their own target tasks, with correctness for ChatDev as low as roughly 25% on the study's programming tasks.
Second, no single category dominated: specification problems, inter-agent misalignment, and verification problems all contributed substantially, meaning there is no one fix, only a discipline.

## 2. The MAST taxonomy

MAST groups fourteen failure modes into three categories aligned with the phases of a multi-agent run: how the system was specified, how the agents interacted, and how the result was checked.

### 2.1 Specification and system design failures

These are defects in the contract given to the agents, present before the first token is generated.

1. Disobey task specification: an agent ignores stated requirements of the task; often the specification was buried, ambiguous, or contradicted by the role prompt.
2. Disobey role specification: an agent acts outside its assigned role, such as a reviewer rewriting code; role prompts under-constrain and the base model's helpfulness fills the gap.
3. Step repetition: the system re-executes completed steps, commonly because progress state lives only in conversation history that no one consults reliably.
4. Loss of conversation history: context truncation or reset silently discards decisions, and later steps proceed on a stale view.
5. Unaware of termination conditions: agents continue past the point where stopping criteria were met, because no component owns the halt decision.

The category label is honest about blame: these read as agent misbehavior but are mostly design defects, and MAST's authors emphasize that better prompts and structure remove many of them, while others require architectural state management rather than prompt patches.

### 2.2 Inter-agent misalignment

These arise in the interaction itself; each maps directly onto machinery from Chapter 03.

6. Conversation reset: an agent restarts a dialogue, discarding accumulated agreement.
7. Fail to ask for clarification: an agent proceeds on a guess where the protocol allowed asking; the guess embeds an implicit decision that diverges from the delegator's intent (Cognition's central failure, formalized).
8. Task derailment: the collective drifts from the objective through locally plausible turns, with no component holding the goal fixed.
9. Information withholding: an agent possesses a fact the task needs and does not transmit it, usually because nothing in its output schema asked for it, the schema-omission failure Chapter 03's `gaps` field targets.
10. Ignored other agent's input: a message arrives and has no effect on the recipient's subsequent actions; the telephone game's degenerate case where fidelity is zero.
11. Reasoning-action mismatch: an agent's stated reasoning and its emitted action diverge, which corrupts any coordination that trusted the stated reasoning.

### 2.3 Task verification and termination failures

12. Premature termination: the system declares completion with objectives unmet, typically because "done" was self-reported rather than checked.
13. No or incomplete verification: verification was skipped or covered only part of the deliverable, such as compiling code without running it.
14. Incorrect verification: the verifier ran and approved a defective result; rubber-stamp reviewers, weak rubrics, and verifier-generator shared blind spots (Chapter 04) all land here.

The category's practical weight is high because verification is the system's last line of defense: every earlier failure mode is survivable if verification catches it, and fatal if verification is mode 13 or 14.

## 3. Miscoordination case studies

### 3.1 Duplicated work

Signature: two workers return overlapping findings, or worse, two agents perform the same side effect twice.
Anthropic's research-system retrospective reports the benign form directly: early orchestrator prompts spawned subagents with vague overlapping scopes, and multiple agents researched the same ground, pure token waste.
The malignant form involves non-idempotent tools: two agents both file the ticket, send the email, or apply the migration, because "who owns this action" was never assigned.
Root causes: partitions defined by topic instead of by mutually exclusive scope, retries without idempotency keys, and orchestrators that re-delegate after a timeout while the timed-out worker is still running.
The fix cluster: written partitions with explicit exclusions (Chapter 04), idempotency keys on every side-effecting tool, and single ownership of each external action.

### 3.2 Dropped work

Signature: the final deliverable silently lacks coverage; nothing failed loudly.
Mechanism one: a shard fails or times out and synthesis proceeds over the survivors without noting the hole (the silent-shard failure Chapter 04's collector discipline exists to prevent).
Mechanism two: a worker completes but its `partial` status is treated as `complete` because the schema forced binary status.
Mechanism three: the orchestrator's plan itself omitted a region, and no component ever compared plan coverage against task scope.
Dropped work is more dangerous than duplicated work because its cost is a wrong deliverable rather than a wasted budget, and it is invisible without either coverage accounting or verification against the original scope.

### 3.3 Conflicting decisions

Signature: parts are individually fine and jointly incoherent, the merged Flappy Bird of Chapter 01.
In coding traces this appears as parallel branches with incompatible interface assumptions that merge textually clean (Chapter 03's worktree caveat); in research it appears as sections written under different implicit definitions of the same term.
Root cause is always the same: a decision that should have been made once, upstream, and transmitted, was instead made independently N times downstream.
The fix is decision hoisting: identify decision-coupled dimensions before fan-out, decide them in the orchestrator, and transmit them as explicit constraints in every delegation contract.

### 3.4 A composite trace

A realistic compound failure, assembled from the patterns above, of the kind MAST-style annotation surfaces.
An orchestrator splits "audit and fix logging across services" into per-service fixers; the partition looks disjoint (mode: none yet).
The delegation omits the target log schema (specification gap), so each fixer picks a format (fail-to-ask, mode 7, times five).
One fixer times out; the orchestrator respawns it, and the original completes later, double-applying edits (step repetition, mode 3, plus duplicated side effects).
The reviewer checks that services compile but not that formats agree (incomplete verification, mode 13).
The system reports success; the observability pipeline downstream breaks on mixed formats.
Every individual agent behaved plausibly; the composition failed four ways; and note that one upstream sentence (the schema in the contract) would have prevented three of the four.

## 4. Cost blowups

Multi-agent cost failures deserve their own section because their blast radius is financial and immediate.

Delegation loops: agent A delegates to B, whose result dissatisfies A, which re-delegates; without a retry cap this oscillates indefinitely, and with peer topologies it can cycle through more parties (A to B to C to A).
Runaway fan-out: an orchestrator prompted to be thorough spawns at the maximum every time, including for trivial queries; Anthropic's effort-scaling rules (explicit agent and tool-call budgets per task complexity tier) exist precisely because models do not infer proportional effort by default.
Context inflation spirals: chatty topologies append every exchange to every participant, so cost per round grows superlinearly in round count; group-chat architectures hit this hardest (Chapter 02).
Depth blowup: recursive spawning multiplies budgets geometrically, which is why production harnesses forbid nesting outright.
The mitigations are all budget mechanics: hard per-agent token and tool-call ceilings enforced by the harness (not the prompt), fan-out caps, retry caps, depth caps, and a per-task cost circuit breaker that halts the whole run at a spend threshold and surfaces to a human.
The trade-off of hard budgets is truncated work on genuinely large tasks, which is why budgets must pair with `partial` status reporting so truncation is visible rather than silent.

## 5. Debugging distributed agent traces

The defining difficulty: the symptom appears at the deliverable, and the cause lives several agents and several summarization boundaries upstream, in a context that no longer exists.
Treat debugging as an evidence problem and solve it with instrumentation before the first incident, not after.

Prerequisites (the observability of Volume 10 applied to composition).
Persist every agent's full trace (messages, tool calls, results), keyed by a run id and a parent-child spawn tree, so the whole execution reconstructs as one tree.
Persist every inter-agent payload verbatim at both ends: what the parent sent, what the child received, what the child returned, what the parent parsed; boundary corruption is only provable with both sides recorded.
Record schema-validation outcomes, retries, timeouts, and budget exhaustions as first-class events, because these are exactly the silent contributors to dropped work.

The diagnostic procedure that follows from the taxonomy.
Start at the defect in the deliverable and identify which claim or artifact region is wrong.
Walk the spawn tree backward from synthesis to the worker that produced that region, checking at each boundary whether the defect already existed in the payload (content failure inside an agent) or appeared across the boundary (communication failure between agents).
If the defect existed in a worker's output, debug that worker as a single agent with Volume 03's methods.
If it appeared at a boundary, diff the sent and parsed payloads; the usual finds are summarization loss, schema coercion, and ignored-input (mode 10).
If no single agent or boundary holds it, you have a composition defect (conflicting decisions, dropped coverage), and the artifact to inspect is the orchestrator's partition and delegation contracts.
Finally, automate the triage: an LLM annotator applying the MAST taxonomy to traces (as the MAST authors did) turns failure analysis from artisanal reading into a labeled distribution you can track across releases, which tells you whether a mitigation actually moved the histogram.

## 6. The mitigation toolkit

Each mitigation below forecloses specific modes; each has a cost; deploy them as a set, not a menu of one.

- Narrow interfaces: schemas on every boundary, explicit delegation contracts, quoted load-bearing facts (Chapters 03 and 04).
  Forecloses modes 1, 7, 9, and most boundary corruption; costs prompt length and schema maintenance, and can truncate novel observations without an escape-hatch field.
- Single writer: every artifact, path, and external side effect has exactly one owning agent.
  Forecloses duplicated side effects and write conflicts; costs serialization of writes, which caps parallelism on write-heavy work, and that cap is correct (Chapter 01).
- Decision hoisting: shared decisions made once in the orchestrator and transmitted as constraints.
  Forecloses conflicting decisions; costs upfront planning effort and can over-constrain workers when the orchestrator decides badly, so hoist only genuinely shared dimensions.
- Checkpoints: durable externalized state (plan file, completed-shard ledger, artifact store) at every phase boundary, enabling resume instead of restart.
  Forecloses step repetition, loss-of-history, and full-cost reruns after crashes (Anthropic cites checkpoint-and-resume among their production necessities for long-running agents); costs storage machinery and the discipline of making state authoritative outside any context window.
- Coverage accounting: the partition written as a ledger, every shard's terminal status recorded, synthesis required to enumerate the ledger.
  Forecloses dropped work; costs a little orchestration rigidity.
- Independent verification with teeth: fresh-context verifier, rubric, severity gate, plus non-LLM verifiers wherever they exist (Chapter 04).
  Forecloses modes 12 through 14 to the extent the rubric is right; costs an extra run per deliverable and a false-positive burden at strict thresholds.
- Structural caps: depth, fan-out, retries, budgets, circuit breakers, enforced by the harness.
  Forecloses blowups categorically; costs truncation on the tail of large legitimate tasks.

The meta-mitigation is architectural conservatism: every cap and interface above gets cheaper to apply as the topology gets shallower, which is the deep reason Chapters 01 and 02 kept recommending the smallest structure that fits the task.

## Exercises

1. Memorize the three MAST categories, then reconstruct all fourteen modes from memory and check yourself against section 2; repeat until perfect, because trace triage requires the taxonomy at recall speed.
2. Take any multi-agent framework demo (or your Chapter 04 harness) and deliberately induce three modes: bury a requirement to induce mode 1, remove the `gaps` field to induce mode 9, and self-report completion to induce mode 12; capture the traces.
3. Annotate the three traces from exercise 2 with an LLM annotator prompted with the taxonomy, and measure its agreement with your own labels.
4. Design the coverage ledger for a ten-shard research fan-out: schema, writer, and the synthesis-time assertion that fails loudly when a shard is unaccounted for; implement it in the Chapter 04 harness.
5. Write the incident report for section 3.4's composite trace as if it happened in production: timeline, contributing modes by number, the single highest-leverage fix, and the mitigation set that prevents recurrence.

## Godhood check

- Name the three MAST categories and at least twelve of the fourteen modes, with a one-line mechanism for each.
- Explain why per-agent evals cannot detect composition failures, and what evaluation level can.
- Distinguish duplicated work's benign and malignant forms and give the specific mitigation for each.
- Explain why dropped work is more dangerous than duplicated work and name the two mechanisms that make it silent.
- Define decision hoisting and identify, for a given fan-out task, which dimensions must be hoisted.
- Walk the boundary-diff debugging procedure from deliverable defect to root cause, naming what evidence must have been persisted for each step to be possible.
- For every mitigation in the toolkit, state the failure modes it forecloses and its explicit cost, without notes.
