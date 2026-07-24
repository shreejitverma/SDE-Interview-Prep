# Chapter 06 - Agentic Control Flow

## What you will master

- A complete taxonomy of stop conditions: who can end a trajectory, when, and with what semantics.
- Turn limits and their interaction with task difficulty, and why the limit is a product decision as much as a safety one.
- Interruption and steering: letting humans redirect a running agent without corrupting the transcript.
- Human-in-the-loop approval gates: allowlists, denylists, ask policies, and where the gate sits in the harness.
- Checkpointing and resumability: the messages array as the canonical state, what else must be captured, and the replay problem for side effects.
- Pause and resume semantics, including waiting on humans and waiting on the world.
- Streaming intermediate state to users, and the UX principles that make autonomy trustworthy rather than alarming.

## 6.1 Control flow is the product

Chapters 3 through 5 built a loop that runs, recovers, and stays within budget.
This chapter is about who controls that loop from outside, and it deserves its own chapter because control flow is where the agent stops being a program and starts being a collaboration.

A user's experienced quality of an agent is only partially the quality of its final answers.
It is equally the answers to control questions: can I see what it is doing, can I stop it, can I redirect it without starting over, will it ask before doing something scary, can I close my laptop and pick the task up later.
Systems identical in model and tools diverge completely in usefulness on these axes, and as of early 2026 the mature agent products - coding agents especially - compete on control surface at least as much as on capability.

The engineering claim of this chapter: every control feature reduces to disciplined operations on the same two objects you already have, the messages array and the loop, plus one new one, a durable store for both.

## 6.2 A taxonomy of stop conditions

Enumerate who can end a trajectory and how, because each initiator needs different handling.

### Model-initiated stops

The model stops requesting tools and returns `end_turn`.
Three semantically distinct cases hide under this one stop reason, and the harness should distinguish them by inspecting the final text and the state of the task.
Completion: the task is done; verify before trusting, per Chapter 5's false-success discipline.
Clarification: the model is asking the user a question; the loop should suspend awaiting input, not terminate, and the distinction matters for session state.
Surrender: the model declares inability; route through the surrender handling of Chapter 5 before accepting.
A refinement used by mature harnesses: give the model an explicit way to signal which case it means, either a `task_complete` tool, an `ask_user` tool, or a required final-message format, because inferring intent from prose is fragile.
Promoting question-asking to a tool has a second benefit: the harness can render it as a blocking modal with options, which is exactly what Claude Code does with its question tool as of early 2026.

### Harness-initiated stops

Budget exhaustion in any of the four currencies of Chapter 5, trajectory-pathology escalation, and safety-gate denial cascades.
All harness stops should pass through the salvage-turn pattern: one final model call with tools disabled to produce a handoff summary, persisted with the checkpoint.

### Human-initiated stops

Cancellation (stop and discard), interruption (stop, keep state, await instructions), and redirection (keep running, incorporate new guidance).
These are three different operations, users want all three, and conflating them - the only button being a kill switch - is the most common control-surface defect in homegrown agents.

### Environment-initiated stops

The provider returns `refusal`, the context window fills, a required external service dies.
Context exhaustion deserves special note: as of early 2026 both provider-side features (server-side compaction on the Anthropic API, in beta) and harness-side summarization exist for it, and Volume 06 covers the trade-offs; the control-flow obligation here is only that the harness detect the condition and route to compaction rather than to a crash.

## 6.3 Turn limits as product policy

Chapter 5 framed max-turns as a safety budget; here is the product framing.

The turn limit encodes how long the agent may work unsupervised, which is an autonomy-axis decision from Chapter 1.
Interactive assistants might cap at a handful of turns and prefer to come back with questions.
Background coding agents might run hundreds of turns, checkpointing throughout.
The same agent may deserve different limits per invocation context - a quick-answer mode and a deep-work mode - and exposing that choice to the user is often better than hardcoding it.

Two implementation refinements beyond the bare counter.
Progressive disclosure: rather than one hard number, use soft thresholds where the agent must produce a progress report and the user may extend, which converts an arbitrary cutoff into a natural check-in cadence.
Difficulty-aware limits: let the model estimate task size in its first turn and negotiate a budget, bounded by a hard ceiling; models as of early 2026 are usably calibrated at coarse-grained effort estimation, and the negotiation itself documents intent.

## 6.4 Interruption and steering

The naive interruption story - kill the process - loses work and can corrupt state if a tool was mid-execution.
The correct machinery is small but must be built deliberately.

### Safe interruption points

The loop has exactly two safe seams: after a model response is fully received, and after a batch of tool results is assembled.
Interrupting between a tool_use response and its tool_result violates the wire contract from Chapter 2 - a dangling tool call - so an interrupt flag must be checked at the seams, and an interrupt arriving mid-seam waits for the seam.
For long tool executions, cancellation means cancelling the execution and then synthesizing a tool_result recording the cancellation, such as "execution interrupted by user before completion", so the transcript remains well-formed and the model, on resume, knows the action did not finish.
This synthesized-result pattern is exactly how managed agent platforms implement interrupts against pending tool calls as of early 2026, and it is the pattern to copy.

### Steering without stopping

Steering is delivering new user input into a running trajectory.
Mechanically it is an append: the new guidance becomes part of the next user message, either alongside pending tool results or as its own turn at the next seam.
Design decisions that matter:
Queue semantics - multiple steering messages queue in order and are all delivered; do not silently drop intermediate ones.
Priority - an instruction like "stop touching the database" is not a queued suggestion but an interrupt-class event; a practical design distinguishes ordinary steering (append at next seam) from urgent steering (interrupt now, synthesize results for in-flight calls, then append).
Attribution - steering text should be clearly framed as fresh user input, because models weigh recent user turns heavily, which is exactly what you want here.

The payoff of good steering is economic: redirecting a trajectory that is 70 percent right preserves the 70 percent, while the kill-and-restart alternative pays the whole cost again and often loses context that made the partial progress possible.

## 6.5 Human-in-the-loop approval gates

Gates are where the authority scoping of Chapter 1 becomes running code.
The gate sits in exactly one place: the dispatcher, between receiving a tool call and executing it, which is the only point where the harness holds a fully-formed, not-yet-executed intention.

### Policy structure

A workable policy model, used in essentially this form by Claude Code and by managed agent platforms as of early 2026, has three verdicts per tool call: allow, deny, and ask.
Allow executes silently; it is the right verdict for read-only tools and reversible operations in sandboxes.
Deny bounces the call with an observation explaining the policy, which the model treats like any other error observation and routes around.
Ask suspends the call pending a human decision, and the human's denial should carry an optional message - "denied: use the staging database instead" - because a denial with a reason steers, while a bare denial merely blocks.

Policies key on tool identity first and arguments second: `bash` might be ask-by-default while `read_file` is allow, and within `bash`, argument patterns escalate - anything matching a package-publish or force-push pattern asks even if bash were allowed.
This is only possible because granularity decisions made dangerous operations legible, which is the Chapter 4 promotion criterion coming due.

### The suspension mechanic

Ask verdicts make the approval gate the first place your loop must genuinely pause mid-turn, possibly for hours.
The clean implementation reuses the checkpoint machinery of the next section: persist the trajectory with the pending call marked, release the process, and on human decision, restore and either execute (approve) or synthesize a denial observation (deny).
Building suspension as checkpoint-restore rather than as a blocked thread is what makes approval-by-mobile-notification and multi-hour review latency possible without holding resources.

### Gate fatigue

The failure mode of approval systems is social, not technical: too many asks, and the human stops reading and clicks approve reflexively, at which point the gate provides liability without safety.
Mitigations: default-allow everything genuinely safe, batch related approvals into one decision where possible, always show intent alongside mechanism (the natural-language description argument on bash calls from Chapter 4 exists for this), and treat every ask the user approves without reading as a signal the policy is miscalibrated.
A gate the user trusts enough to actually read is worth ten gates they click through.

## 6.6 Checkpointing and resumability

Everything above assumed the trajectory can outlive the process, so build that.

### What the state actually is

The beautiful fact, established in Chapter 3: the conversational state of an agent is one serializable list.
A checkpoint is therefore:
The messages array, verbatim, including all tool_use and tool_result blocks.
The loop-position metadata: turn count, per-budget spend, pending-approval markers, retry counters from Chapter 5.
The configuration identity: system prompt, tool definitions, and model id, because resuming with silently different tools or prompt is a subtle correctness bug - the transcript's calls must remain interpretable - and because tool-list changes invalidate the prompt cache, per Chapter 2.
A schema version for the checkpoint format itself, because you will change it.

Persist at every seam - after each turn's results are assembled - which makes the write cheap (append-mostly) and the recovery loss at most one turn.

### What the state is not

The environment is not in the checkpoint.
Files written, branches created, records mutated: these live in the world, and a restored trajectory assumes the world is as the transcript left it.
This is the replay problem, and it has three practical postures.
Assume continuity: for short suspensions on a stable machine, resume blind; cheapest and usually fine for approval-gate pauses.
Verify on resume: before the first model call after restore, the harness re-runs cheap probes - do the files mentioned in recent turns still hash the same, is the branch still where it was - and injects a discrepancy observation if not, letting the model reconcile.
Re-establish: for long gaps or shared environments, inject a fresh environment summary as an observation and instruct the model to re-verify anything it depends on before proceeding.
Choose per product; but choose explicitly, because the default - silent assumption of continuity across days - is how resumed agents confidently edit files that no longer exist.

### Resumption is just construction

Given the checkpoint discipline, resume is: load messages, restore counters, re-attach the same tool surface, and re-enter the loop at the seam.
Cross-machine resume follows for free, as does trajectory forking - copy the checkpoint, vary the next steering message, run both - which turns checkpoints from a durability feature into an exploration feature.

## 6.7 Pause and resume semantics

With checkpoints in hand, distinguish the three waiting states a paused agent can be in, because users and schedulers treat them differently.

Waiting on a human: approval gates and clarification questions; the resume trigger is a user decision, latency is unbounded, and the UI should show exactly what is being waited for.
Waiting on the world: a deploy pipeline running, a rate limit cooling, a scheduled time arriving; the resume trigger is a poll or webhook, and the harness, not the model, should own the wait - burning model turns on "check if it's done yet" polling is the antipattern, and the correct design suspends and lets an external trigger resume with the outcome injected as an observation.
Waiting on nothing: a user pressed pause; the resume trigger is the user pressing resume, possibly with steering attached.

All three are the same checkpoint plus a different wake condition, which is the payoff of having built state capture properly: pause is not a feature you add, it is a wake-policy table over machinery you already have.

## 6.8 Streaming intermediate state

Users distrust a silent box that will speak in four minutes, and they are right to: without intermediate signal they can neither calibrate expectations nor intervene early, which wastes exactly the steering machinery this chapter built.

What to stream, in increasing order of implementation effort.
Text deltas: the model's narration, token by token, via the streaming transport from Chapter 3.
Action events: tool name and key arguments as each call begins, and a compact result signal as each completes - this is the "running tests..." line, and it comes from the same content_block_start events the streaming API already emits.
Progress structure: turn count against budget, current subtask if the prompt has the model maintain a plan, and elapsed cost for cost-sensitive contexts.
Semantic milestones: the model's own progress summaries, which current models produce well when the prompt asks for periodic check-ins; as of early 2026 the strongest models narrate long tool sessions usefully by default, and prompting effort has shifted from forcing narration to bounding it.

Two design cautions.
Streaming is presentation, not truth: the model's narration of what it did is a claim, and UIs that display claims as facts inherit the false-success problem of Chapter 5; where it matters, bind displayed status to verified tool results, not to prose.
And verbosity has a cost curve: every narration token is output spend and context growth, so the right volume is a product decision - a background agent should narrate milestones only, an interactive one can afford running commentary.

## 6.9 The UX of agency

Close with the principles that the mechanics above exist to serve, because they generalize across every agent product category.

Legibility before capability: users extend autonomy to agents whose behavior they can predict, and they predict from what they can see; the observability features are therefore not accessories to trust but its mechanism.
Reversibility calibrates gating: the more undoable an action, the less it needs a gate, which is why sandboxes and version control are UX features - they convert ask-verdicts into allow-verdicts by making mistakes cheap.
Interruption must be safe to be used: users who fear that stopping the agent will corrupt its work let bad trajectories run to completion; advertise that stop is safe, and make it true.
Asking is a cost, not a virtue: every question and every approval interrupts a human; spend those interrupts where the expected cost of proceeding wrong exceeds the cost of the interrupt, and not elsewhere.
And finally, control features compose into a contract: what the user can see, stop, redirect, approve, and resume is the real interface of the agent, and it should be designed, documented, and tested with the same seriousness as the tool surface, because it is the half of the product the user actually touches.

## Exercises

1. Extend the Chapter 3 agent with a `task_complete` tool and an `ask_user` tool, route end_turn through the three model-stop cases, and demonstrate a trajectory that suspends on a question and resumes with the answer.
2. Implement checkpointing at every seam to a JSON file, including budgets, retry counters, and a schema version; kill the process mid-task at randomized points and verify resume loses at most one turn.
3. Add interruption: a keypress sets a flag, the loop honors it only at seams, in-flight bash executions are cancelled with a synthesized tool_result, and the transcript stays API-valid; prove validity by resuming against the real API.
4. Build the three-verdict approval gate over the dispatcher with per-tool defaults and an argument-pattern escalation for bash; implement deny-with-message and confirm the model routes around a denial that carries a reason.
5. Implement verify-on-resume: hash every file mentioned in the last five turns at checkpoint time, re-hash at resume, and inject a discrepancy observation when they differ; test by editing a file while the agent is suspended.
6. Design and implement the streaming event feed: text deltas, action begin/end events, and budget progress, rendered as a live terminal UI; then add a "steer" input box that appends guidance at the next seam.
7. Write the control contract for an overnight refactoring agent as a one-page document: what runs unattended, what asks, what checkpoints, what the user sees at breakfast, and what resume-after-laptop-sleep does; then implement the two items from it your Chapter 3 agent most lacks.

## Godhood check

You have mastered this chapter when you can do the following without reference material.

- Enumerate the four initiators of trajectory stops and the distinct handling each requires, including the three cases hiding inside end_turn.
- Explain why the loop has exactly two safe interruption seams, and write the synthesized-tool_result pattern for cancelling an in-flight execution without corrupting the transcript.
- Design a three-verdict approval policy for a given tool surface, place the gate at the dispatcher, and argue the gate-fatigue trade-offs of each ask you include.
- State precisely what belongs in a checkpoint and what deliberately does not, and choose among the three replay postures for a given product with reasons.
- Implement pause, resume, steering, and forking as operations on the checkpoint plus wake-policy machinery, rather than as separate features.
- Argue, from the UX principles, which control features a specific agent product needs first, and defend the ordering against a capability-first counterargument.
