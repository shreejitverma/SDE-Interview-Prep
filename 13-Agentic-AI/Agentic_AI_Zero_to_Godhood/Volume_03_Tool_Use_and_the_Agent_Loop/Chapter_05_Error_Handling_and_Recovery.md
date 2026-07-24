# Chapter 05 - Error Handling and Recovery

## What you will master

- A working taxonomy of agent failure: transport failures, tool failures, model failures, and trajectory failures, and why each layer needs a different response.
- Retry-with-feedback as the fundamental recovery primitive, and how to make feedback that actually converges.
- Validation layers: schema, semantic, and precondition checks, and where each belongs in the harness.
- Detecting and breaking the pathological trajectories: repetition loops, thrashing, premature surrender, and false success claims.
- Budget guards: turns, tokens, dollars, and wall clock, plus what to do when a budget fires.
- Graceful degradation: designing the agent to fail partially and legibly instead of totally and silently.

## 5.1 Why agent error handling is its own discipline

Conventional software fails by raising: an exception propagates, a handler catches or the process dies, and the failure is a control-flow event.
Agents add a second, stranger failure surface: the system can be mechanically healthy - every API call succeeding, every tool returning - while the trajectory is failing, because the decision-maker in the loop is a stochastic model.

This forces a layered view.
A layer-appropriate response to a network timeout is an exponential-backoff retry that the model never learns about.
A layer-appropriate response to a hallucinated file path is an error observation the model must learn about, because only the model can revise its plan.
Confusing the layers produces the two classic harness bugs: surfacing transport noise to the model, which wastes turns on problems the model cannot fix, and hiding tool failures from the model, which lets it build on a false world.

The taxonomy in this chapter proceeds from the outside in: transport, tool call, model behavior, trajectory.

## 5.2 Layer 1: transport failures

These are failures of the API call itself: rate limits (429), server errors (500), overload (529), and network drops.
The model has nothing to do with them and should never see them.

Handle them in the harness with standard distributed-systems hygiene.
Retry 429 and 5xx with exponential backoff and jitter; the official SDKs as of early 2026 do a default two retries for you, which you should raise for long-running agents.
Respect `retry-after` headers when present.
Treat 4xx request errors as bugs in your harness, not retryable events: a 400 from a malformed messages array will not fix itself, and retrying it burns quota while hiding the defect.
Distinguish one 400 specially: an over-limit context error is a trajectory-level event, and the correct response is context compaction or checkpoint-and-summarize, not a retry.

One subtlety specific to agents: a transport retry replays a model call, not a tool execution, so it is always safe.
Never build a retry layer that replays tool executions on network failure, because the failure may have occurred after the side effect landed.
If a tool's transport can fail after commit, the tool needs idempotency keys, which is a tool-design concern from Chapter 4, not a retry-policy concern.

## 5.3 Layer 2: bad tool calls

The model emitted a call your executor cannot or should not run as-is.
The subspecies, in increasing order of subtlety:

Malformed structure: arguments that do not parse or violate the schema.
Strict schema modes (Chapter 2) largely eliminate this class at the source, which is why you should enable them; what remains arrives when strictness is off or unsupported.
Unknown tool: the model names a tool that does not exist, typically a plausible neighbor of one that does, such as `read` for `read_file`.
Hallucinated arguments: schema-valid values that reference nonexistent entities - invented file paths, fabricated ticket ids, misremembered branch names.
This is the dominant subspecies in practice and the one no grammar can prevent, because the schema cannot know which strings denote real objects.
Invalid semantics: real entities, impossible operation - editing a file never read, refunding more than was paid, scheduling a meeting in the past.

All four get the same shape of response, established in Chapters 3 and 4: never crash, never silently drop, return an `is_error` observation at level 3 quality - diagnosis plus remedy.
For unknown tools, list the valid tool names.
For hallucinated references, say the entity does not exist and name the discovery tool that finds real ones.
For semantic violations, state the violated precondition and the action that satisfies it.

The empirical justification for investing here: with informative feedback, frontier models as of early 2026 recover from a bad call in one turn at high rates, while with bare "Error" feedback they frequently retry the same call verbatim, because from the model's perspective nothing distinguishes the failed attempt from a transient fault.
Feedback quality is the recovery rate.

## 5.4 Retry with feedback, formalized

The primitive underlying all of layer 2 and much of layer 3 is a loop-within-the-loop:

```
attempt = 0
while attempt < max_attempts:
    call = model_output()
    problem = validate(call)
    if problem is None:
        return execute(call)
    return_observation(problem.diagnosis + problem.remedy)
    attempt += 1
escalate()
```

Three engineering points make the difference between a retry loop that converges and one that oscillates.

Feedback must be differential.
If the second attempt fails differently, say what changed and what still fails; if it fails identically, say explicitly "this is the same error as before", because the model may not notice the repetition across the noise of intervening tokens.

Retry budgets must be per-problem, not global.
Three attempts at one validation failure is a reasonable spend; a global counter that lets one stubborn failure exhaust the budget meant for the whole task is not.
Track attempts keyed by the failing operation, and escalate that operation specifically when its budget exhausts.

Escalation must exist.
When retries exhaust, the harness needs a defined next step: try an alternative strategy, drop the subtask and continue degraded (section 5.8), or surface to a human with the failure history attached.
A retry loop without an escalation edge is just a slower crash.

## 5.5 Validation layers

Validation is cheapest at the earliest layer that can express the check, and a well-built harness stacks three.

Schema validation, at the API boundary.
Enable strict tool schemas so structurally invalid calls cannot be emitted; keep a harness-side schema check anyway for providers and modes where strictness is unavailable, and to defend against your own definition drift.

Semantic validation, in the executor, before side effects.
Existence checks on referenced entities, range and consistency checks the schema could not express, permission checks against the agent's authority scope from Chapter 1.
Report every violation in one observation rather than one per turn, as argued in Chapter 4.

Precondition and staleness validation, in the harness state.
This layer holds cross-call invariants: the file being edited was read after its last modification, the record being updated was fetched this session, the migration being applied was generated against the current schema version.
These checks live in the harness because only the harness sees the whole trajectory; no single tool can know what another tool read.
Claude Code's read-before-edit rule, discussed in Chapter 4, is exactly such an invariant, and it exists because the failure it prevents - blind-writing over content changed since the model last saw it - is otherwise silent and severe.

A fourth pseudo-layer deserves mention: post-execution verification.
The strongest agent domains are those with cheap ground truth - run the test suite, type-check the code, re-fetch the record - and a harness can require verification tool calls after mutating operations, or the system prompt can demand it, as Chapter 3's did with "verify after you modify".
Verification converts model self-assessment, which is unreliable, into environmental fact, which is not.

## 5.6 Layer 3: pathological trajectories

Above single calls sits the trajectory, and trajectories have their own failure modes, each requiring detection machinery in the harness because the model inside a failing trajectory usually cannot see the failure.

### Repetition loops

The signature: the same tool call, or a trivially permuted variant, issued again and again with non-improving results.
Detection: hash each (tool, normalized arguments) pair and count recent repeats within a sliding window; three identical calls with error results is a firing condition.
Response, in escalating order: inject a system-level observation naming the repetition and instructing a strategy change; then forbid the specific call at the dispatcher, bouncing it with "this exact call has failed 3 times; it is now blocked; try a different approach"; then escalate out of the loop.
Cause, worth knowing: repetition is usually context-driven - the failing attempt and its error dominate recent context and the model pattern-matches into reissuing it - which is why the injected observation must be loud enough to compete.

### Thrashing

The signature: mutually undoing actions - edit A, revert A, edit A again - or oscillation between two strategies without progress.
Detection is harder than repetition because individual calls differ; practical proxies include no-net-change detection on the environment (same file hashes as N turns ago) and progress heuristics per domain (test-failure count not decreasing over a window).
Response: same escalation ladder, but the injected observation should summarize the oscillation explicitly, because thrash is invisible from inside.

### Premature surrender

The signature: the model declares the task impossible or asks an unnecessary question while viable moves remain.
This mode anticorrelates with the loop modes: prompts and models tuned against runaway loops surrender more, and vice versa, so treat the pair as a calibration axis rather than two independent bugs.
Detection is heuristic: an end_turn whose final text contains apology-and-inability patterns while the turn budget is largely unspent.
Response: one nudge is legitimate - return an observation asking for a concrete enumeration of untried approaches and permission-free next steps - but hard-forbidding surrender produces confabulated work, which is worse than an honest stop.

### False success claims

The signature: the model reports completion while the environment disagrees - tests never run, file never written, the claimed output nowhere in the transcript.
This is the most dangerous mode because it exits the loop through the front door.
Detection is verification: on end_turn, before accepting the result, the harness runs domain checks - does the claimed artifact exist, do the tests pass, does the diff apply.
Response on verification failure: reopen the loop with the discrepancy as an observation: "you reported the tests passing, but no test command appears in the transcript; run them now".
Prompt-side mitigation stacks with this: instructions requiring every progress claim to be backed by a tool result in the same session measurably reduce fabricated status reports on current models as of early 2026, but the harness check remains the backstop because prompt compliance is probabilistic.

## 5.7 Budget guards

Every autonomous loop needs hard limits that fire independently of model judgment, because the failure modes above all share one property: they consume resources until something external stops them.
The four budgets, and their design points:

Max turns is the coarse guard from Chapter 3, cheap and essential; size it generously relative to honest task length, because a too-tight turn budget converts recoverable trajectories into failures.
Token and cost budgets track `usage` per response, accumulate input and output tokens, price them, and cap the spend; this is the budget that maps to money and the one to enforce per-task and per-tenant in production.
Wall-clock budgets bound latency-sensitive contexts and runaway tool executions; enforce per-tool-call timeouts (Chapter 3's bash timeout) separately from whole-trajectory deadlines.
Action budgets cap specific dangerous operations - at most N external emails, at most N deploy attempts - and are really authority limits from Chapter 1 wearing a budget costume.

Two rules govern what happens when a budget fires.

First, fire softly before firing hard: at a threshold such as 80 percent, inject an observation telling the model its remaining budget and instructing it to prioritize and wrap up; models given a visible countdown finish gracefully at usefully higher rates than models guillotined mid-thought.
The Anthropic API's task-budget feature (beta as of early 2026) implements exactly this pattern server-side - a token budget the model sees counting down - which is evidence of how central the pattern has become; the harness-side version works on any provider.
Second, when the hard limit fires, spend one final bounded turn on salvage: ask the model, with tools disabled via `tool_choice: none`, to summarize state, what was accomplished, and what remains, and persist that summary with the checkpoint (Chapter 6).
Budget death with a handoff note is degradation; budget death without one is data loss.

## 5.8 Graceful degradation

The final discipline is designing failure as a first-class outcome with useful partial value, rather than a binary.

Degradation ladders: for each agent, write down the ordered fallbacks before shipping.
A research agent that cannot access the paywalled source falls back to open sources and flags the gap; a coding agent that cannot make the full test suite pass delivers the subset of passing changes plus a precise description of the remaining failure; a data agent that cannot compute the exact metric delivers the approximation and labels it.
The ladder belongs partly in the prompt, so the model knows degraded success is acceptable and preferred to fabrication, and partly in the harness escalation edges, so exhausted retries route to the next rung instead of to a stack trace.

Legible partiality: a degraded result must say it is degraded, what is missing, and why, in machine-checkable form where downstream automation consumes it.
The worst degradation is the silent kind, indistinguishable from full success until someone relies on the missing part.

Fail toward safety asymmetrically: when uncertain between a destructive completion and an incomplete stop, stop.
This asymmetry should be explicit in prompts and enforced by gates on the destructive tools, because the cost function of agent errors is rarely symmetric, and the harness, not the model, is the final holder of that asymmetry.

The through-line of this chapter is worth stating once, plainly.
Reliability in agents is not achieved by making the model never err; it is achieved by building a harness in which nearly every error is observed, fed back, bounded, and survivable - and by reserving the unsurvivable errors for gates that never let them execute.

## Exercises

1. Extend the Chapter 3 agent with the full layered handler: SDK-level transport retries, level-3 observations for all four bad-call subspecies, and a per-problem retry budget of three with an escalation message; demonstrate each path with a contrived failing tool.
2. Implement repetition detection with a (tool, normalized-args) hash window and the three-step escalation ladder; verify it by pointing the agent at an impossible task, such as making a network call in an offline sandbox, and capture the transcript at each escalation stage.
3. Add token and cost accounting with a soft warning at 80 percent and a hard stop with a salvage turn; confirm the salvage summary survives in your checkpoint format.
4. Build a false-success detector for a coding task: on end_turn, the harness greps the transcript for a passing test command and reopens the loop with a discrepancy observation if absent; measure how often it fires across twenty runs.
5. Write the degradation ladder for an agent that compiles a weekly competitor-pricing report from web sources, including what each rung delivers and how partiality is labeled.
6. Design idempotency for a `send_invoice` tool such that transport-level ambiguity (timeout after possible commit) can never double-send; specify the key, the storage, and the observation returned on a replay.
7. Take a real failed trajectory from any agent you run, classify every error in it against this chapter's taxonomy, and identify the single earliest point where a harness mechanism from this chapter would have changed the outcome.

## Godhood check

You have mastered this chapter when you can do the following without reference material.

- Classify any observed agent failure into transport, tool-call, model-behavior, or trajectory layers within seconds, and name the layer-appropriate response.
- Explain why transport errors must be hidden from the model and tool errors must be shown to it, and what goes wrong in each direction of confusion.
- Write retry-with-feedback with differential feedback, per-problem budgets, and a real escalation edge, from memory.
- Place a given validation check at the correct layer - schema, semantic, precondition, or post-execution - and justify the placement by what information that layer uniquely holds.
- Implement detectors for repetition, thrashing, premature surrender, and false success, and describe the calibration tension between the loop modes and the surrender mode.
- Design the four budget guards with soft warnings and salvage turns, and write a degradation ladder that makes partial failure legible and preferable to fabrication.
