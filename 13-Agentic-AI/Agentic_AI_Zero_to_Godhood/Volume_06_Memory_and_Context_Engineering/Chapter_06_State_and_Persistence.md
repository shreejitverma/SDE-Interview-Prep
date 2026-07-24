# Chapter 06 - State and Persistence

## What you will master

- The taxonomy of agent state: in-window state, session state, and durable state, and the boundaries between them.
- Checkpointing agent runs: what a checkpoint contains, when to take one, and how frameworks implement it.
- Resumability after crash, interrupt, or human pause, including the side-effect problem that makes naive replay wrong.
- Event-sourced designs: the transcript as an append-only log and derived state as projections.
- The transcript as source of truth and its relationship to the lossy window.
- Multi-session continuity: resuming, forking, and long-lived agents.
- Versioning state schemas so month-old checkpoints still load.

## 6.1 The state taxonomy

Every agent system carries state at three distances from the model, and most persistence bugs come from confusing them.

In-window state is whatever the assembled context currently says: the model's only working memory, lossy by construction (Chapter 03), and gone the moment the process exits.
Session state is everything the orchestrator holds about the current run: the full message list, tool-call records, todo state, scratchpad files, pending approvals, loop counters, and accumulated cost.
Durable state is everything meant to outlive the run: memory files and long-term memory (Chapters 04-05), artifacts produced (code, documents, database rows), and the persisted record of the run itself.

The boundaries generate the design rules.
The window is a cache over session state: compaction (Chapter 03) evicts from the cache, and nothing evicted should exist only in the cache.
Session state is a working set over durable state: a crash may lose the working set, and the system's correctness is defined by what it can reconstruct from durable state alone.
Writing this sentence down for your own system - "after losing all session state, we can reconstruct X from Y" - is the single most clarifying persistence exercise available.

## 6.2 What a checkpoint contains

A checkpoint is a snapshot of session state sufficient to resume the run.
Enumerating its contents forces every implicit dependency into the open:

1. The message list: every user, assistant, and tool message in order, in provider-neutral form.
2. Agent-loop bookkeeping: current step, pending tool calls not yet executed, retry counters, spend so far against budgets.
3. Scratchpad and todo state: either the bytes or authoritative pointers to files that are themselves durable.
4. Environment bindings: working directory, sandbox or container identity, environment variables the run assumed; often the hardest part, because a container that died cannot be un-died, and the checkpoint must record enough to rebuild an equivalent one.
5. External-world cursors: which items of a work queue are done, which API resources were created, idempotency keys issued; this is the side-effect ledger of Section 6.4.
6. Identity and config: model, tool versions, prompt version, schema version (Section 6.7).

What a checkpoint deliberately excludes: the assembled window (it is derivable from the message list plus the assembly code), raw caches, and anything re-fetchable at acceptable cost; a checkpoint is small because it stores decisions and cursors, not the world.

When to checkpoint, with the trade-offs stated: after every turn is the simple, robust default and is cheap when checkpoints are deltas; at tool boundaries (specifically, after recording the intent to call a side-effecting tool and again after its result) is the minimum for exactly-once reasoning; at phase boundaries only is cheaper still but loses up to a phase of work on crash.
Frameworks made this a commodity: LangGraph's checkpointer persists graph state per super-step to a pluggable backend, which buys time travel and interrupt-resume; Temporal-style durable execution takes the strongest position, journaling every workflow step so the process itself is restartable by replay.
As of early 2026, the practical baseline for a production agent is per-turn checkpointing to a database keyed by session id.

## 6.3 Resumability: crash, interrupt, and human pause

Three interruption classes look similar and demand different handling.

Crash (process death, OOM, provider outage mid-call): resume from the last checkpoint; the open question is always the in-flight operation - was the model call or tool call that was executing when the process died actually completed on the other side?
For model calls the answer is cheap: re-issue; generation is side-effect-free, and the cost is tokens.
For tool calls the answer is Section 6.4.

Interrupt (deliberate stop: user hits stop, budget cap trips, a policy gate fires): the orchestrator gets to finish the current step cleanly and checkpoint at a consistent boundary, which is why interrupt handling should be built as "reach the next checkpointable boundary, then stop," never as process kill.

Human pause (approval gates, clarification requests): structurally an interrupt with an unbounded gap; the checkpoint may be resumed hours or weeks later, which raises staleness: the world moves while the run sleeps.
A resumed run should re-validate its critical assumptions - re-read files it plans to edit, re-check queue state - rather than trusting a week-old picture; the cheap implementation is a resume hook that re-runs a small validation step before the loop continues.
LangGraph's interrupt primitive and similar human-in-the-loop mechanisms in other frameworks (current as of 2025-2026) exist precisely to make the pause a first-class checkpointed state rather than a hung process.

### Resume mechanics

Resuming is reassembly, not memory: load the checkpoint, rebuild session state, re-assemble the window from the message list through the normal pipeline (including compaction if the history warrants it), rebuild or reattach the environment, and continue the loop at the recorded step.
Two subtleties bite in practice.
First, environment reattachment: if the sandbox died, files the agent created in it are gone unless the checkpoint captured them or they lived on durable mounts; decide which before the first crash, not after.
Second, model drift: a run resumed weeks later may execute on a newer model snapshot; record the model id in the checkpoint and decide policy explicitly (pin when reproducibility matters, float when improvements matter).

## 6.4 The side-effect problem

Replay is safe for pure computation and dangerous for the world: a resumed run must not send the email twice, apply the migration twice, or charge the card twice.
The toolkit is classic distributed-systems material applied at the tool boundary:

- Classify every tool as pure (read-only), idempotent-effectful (safe to repeat: overwrite-style writes), or non-idempotent-effectful (sends, charges, creates).
- For non-idempotent tools, write an intent record to the checkpoint before executing, with a generated idempotency key; pass the key to the downstream API where supported; on resume, an intent without a result triggers a lookup ("did this key execute?") rather than a blind re-run.
- Where the downstream API supports no idempotency, bracket the call with your own ledger and accept that the residual race (crash between the API's commit and your ledger write) needs either reconciliation or human review; say so in the design doc rather than pretending it away.

This is the checkpoint content that matters most, because everything else in a resumed run is recoverable and this is not.

## 6.5 Event sourcing: the transcript as log

The agent loop is naturally event-sourced, and leaning into that is the cleanest persistence architecture available.
The design: the append-only event log is the source of truth - user messages, assistant messages, tool calls, tool results, plus orchestrator events (compaction performed, checkpoint taken, budget updated, human approval granted) - and everything else is a projection derived by folding over the log.

```python
# Events are appended, never mutated.
# {"seq": 41, "ts": "...", "type": "tool_result", "call_id": "c17", "content": ...}
# {"seq": 42, "ts": "...", "type": "compaction", "replaced_seqs": [3, 30], "summary": "..."}

def project_window(events, assembler):
    """The context window is a lossy projection of the log."""
    messages = fold_messages(events)          # apply compactions, clears
    return assembler(messages)                # budgets, ordering (Chapter 02)

def project_costs(events):
    return sum(e["usage"]["total_tokens"] for e in events if e["type"] == "model_call")
```

What this buys, concretely:

- Crash recovery is replay: session state is a fold over the log, so the checkpoint can shrink to "log position plus side-effect ledger."
- Compaction becomes non-destructive: the compaction event records the summary and the range it replaces, the window projection applies it, and the underlying events remain for audit and for re-compaction with a better prompt later.
- Debugging and evals get their substrate: any past step can be re-assembled exactly as the model saw it, which is the capability Chapter 02's dissection exercise and Volume 10's eval replay both require.
- Forking is cheap: branch the log at any sequence number to explore "what if the agent had done X," which is the mechanism behind time-travel debugging in LangGraph-style frameworks.

The costs, honestly: logs grow without bound and need retention policies; projections must be deterministic or replay diverges (beware folding logic that consults the current clock or current file system); and the discipline is easy to erode - one convenient in-place mutation of history and the guarantees are gone.
Concurrency also needs a rule: a single writer per session log is the simple, correct default, with multi-agent designs giving each agent its own log rather than interleaving writers (Volume 07).

## 6.6 Transcript as source of truth, and multi-session continuity

Claude Code is the accessible production example of this architecture as of 2025-2026: every session appends to a JSONL transcript on disk, the window is a lossy view over it (compaction summarizes, but the transcript keeps everything), and `--resume` / `--continue` rebuild a live session from the stored transcript, across process restarts and machine reboots.
The generalizable principle: the transcript is the database and the window is a cache; systems that treat the window as the record are unrecoverable by construction.

Multi-session continuity is then a spectrum of how much context a new session inherits:

- Cold start plus durable memory: each session starts fresh, inheriting only memory files and long-term memory (Chapters 04-05); simplest, and right for independent tasks.
- Resume: continue the same log; right for one task interrupted.
- Summary handoff: a new session starts with a compaction-style summary of a previous session plus pointers into its transcript; right when the old log is too long or too stale to resume wholesale, and it is exactly Chapter 03's contract applied at the session boundary.
- Fork: branch an existing log to try an alternative; right for exploration and evals.

Long-lived agents (weeks of continuous operation) combine all four: the log rolls with periodic summary handoffs, durable memory accumulates the distillate, and the working session stays bounded; as of early 2026 this composition - not any single mechanism - is how long-horizon agents actually ship.

## 6.7 Versioning state schemas

A checkpoint written in January must load in March, after the orchestrator changed; otherwise persistence rots into a museum of unreadable snapshots.
The rules are ordinary data engineering, applied with agent-specific care:

- Tag everything: every checkpoint, event, and memory record carries a schema version; untagged state is version zero forever and you must still handle it.
- Prefer additive evolution: new optional fields with defaults load old data for free; renames and semantic changes are the expensive class.
- Migrate on read for long-tail data, migrate in batch for hot data; on-read migration keeps old sessions loadable indefinitely at the cost of carrying every migration function forever.
- Version the semantics, not just the shape: the subtle breakages are a changed tool contract (old tool-call records replaying against new tool behavior), a changed compaction summary format, or a changed prompt whose old assistant messages now violate new invariants; record tool and prompt versions in the log so replay can detect mismatch and either adapt or refuse.
- Test the promise: keep a corpus of frozen checkpoints from every released version and load them in CI; a persistence guarantee without a compatibility test suite is a hope.

The trade-off of strong versioning discipline is drag on iteration speed - every schema change now has a migration cost - and teams should scope the guarantee deliberately: many products promise resumability for 30 days, not forever, and delete beyond it, which converts an unbounded compatibility burden into a bounded one and doubles as a retention policy (Chapter 05's privacy obligations approve).

## Exercises

1. Write the reconstruction statement for an agent you own - "after losing all session state we can reconstruct X from Y" - then kill the process mid-run and test it; document every gap between the statement and reality.
2. Implement per-turn checkpointing for a tool-using agent: define the checkpoint schema of Section 6.2, persist to SQLite keyed by session id, and demonstrate crash-resume completing a 20-step task with the process killed at three random points.
3. Add the side-effect ledger: classify your tools into the three effect classes, implement intent records with idempotency keys for the non-idempotent ones, and prove by test that a crash between intent and result does not double-execute.
4. Refactor the same agent to event sourcing: append-only log, window and cost projections, compaction as an event; then implement fork and demonstrate two branches of one session diverging from sequence number N.
5. Build summary handoff: end a long session, generate a Chapter 03 contract summary plus transcript pointers, start a new session from it, and eval task continuity against a full resume on five multi-day scenarios.
6. Break your own schema: rename a checkpoint field, then implement version tagging and on-read migration so a pre-rename checkpoint still resumes; add the frozen-checkpoint CI test that would have caught the break.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Define the three state distances, state the cache relationships between them, and give the reconstruction statement that defines correctness under crash.
- Enumerate the six contents of a checkpoint, defend what is excluded, and choose a checkpoint frequency with its trade-off.
- Distinguish crash, interrupt, and pause, and explain staleness re-validation on resume and the environment-reattachment problem.
- Explain why replay endangers side effects, classify tools by effect, and describe the intent-record and idempotency-key protocol including its residual race.
- Sketch an event-sourced agent with window and cost projections, list the three things it buys and the three disciplines it demands, and explain compaction-as-event.
- Describe transcript-as-source-of-truth with Claude Code as the example, place the four continuity modes on their spectrum, and state the schema-versioning rules including scoped guarantees.
