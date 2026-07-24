# Chapter 03 - Compaction and Summarization

## What you will master

- The three responses to a filling window: truncation, summarization, and compaction, and when each is correct.
- The compaction contract: exactly what must survive (decisions, constraints, unresolved threads) and what may be discarded.
- Tool-result clearing as the cheapest first-line intervention.
- Implementation patterns with runnable code: thresholds, split points, summary prompts, verification.
- Claude Code's compaction behavior as a production case study.
- Server-side context editing APIs (Anthropic's 2025 context-management beta) and how they change the architecture.
- The failure modes: summary drift, lost constraints, compaction loops, and how to test against them.

## 3.1 The problem: the window fills before the task ends

Any agent that runs long enough hits the wall: the assembled context approaches the model's limit, or more precisely the effective limit you set below it (Chapter 02).
At that point every design must answer one question: which tokens die so the task can live.
There are exactly three families of answer, and mature systems use all three at different layers.

- Truncation: delete tokens by position or age, keeping no residue beyond a marker.
- Summarization: replace a span of tokens with a shorter model-generated description of it.
- Compaction: the engineered combination: summarize what has irreplaceable meaning, truncate what does not, preserve verbatim what must not be paraphrased, and leave pointers for what can be re-fetched.

The terminology in the field is loose; this volume uses "compaction" for the whole orchestrated operation, following the usage popularized by Claude Code and Anthropic's 2025 guidance.

## 3.2 Truncation: cheap, fast, and dangerous

Truncation strategies in increasing order of sophistication:

- Head truncation (drop the oldest messages): almost always wrong for agents, because the oldest content includes the task definition and early decisions.
- Tail-preserving sliding window (keep system prompt plus the last N turns): the default in many chat frameworks; acceptable for chitchat, destructive for multi-step work because mid-session decisions vanish silently.
- Middle-out truncation (keep head and tail, cut the middle): aligns with the U-shaped attention curve and is the least-bad pure truncation, since the sagging middle is the least-attended region anyway.
- Per-item truncation (cap each tool result at K tokens at insertion time): not really a limit response but a prophylactic; it bounds the growth rate and should be on by default in every agent.

Truncation's virtues are real: zero model calls, zero latency, zero risk of a bad paraphrase, perfectly predictable token savings.
Its vice is silence: nothing marks what was lost, so the model does not know it does not know.
Minimum professional standard: never truncate without leaving an explicit marker such as `[earlier 34 messages removed; task began: "migrate billing to v2 schema"]`, because an agent that knows history is missing can re-fetch or ask, while an agent that does not will confabulate.

## 3.3 Summarization: paying tokens and risk for meaning

Summarization spends one model call to compress a span into prose.
Its economics are attractive: a 60k-token history often compresses to 1-3k tokens of summary, a 20-50x reduction.
Its risks are the whole subject of careful design:

- Lossiness is unbounded and unmarked; the summary reads confident whether or not it dropped the key constraint.
- Errors become authoritative: after compaction the summary sits early in the window with primacy position (Chapter 02), so a wrong summary outvotes the truth.
- Iterated summarization compounds: summarizing a summary loses second-order detail, and long sessions may compact many times.

The single highest-leverage decision is what the summary prompt demands, which brings us to the contract.

## 3.4 The compaction contract: what must survive

Treat compaction as a serialization boundary with a schema, not as "write a summary."
The categories that must survive, in priority order, with the reason each is irreplaceable:

1. The task and its acceptance criteria: without it, the agent literally cannot finish; verbatim over paraphrase, because paraphrased goals drift.
2. Decisions made and their rationale: "we chose approach B because A failed on X" prevents re-litigating settled questions and re-attempting known failures.
3. Constraints and invariants: user-stated restrictions ("do not touch prod", "keep the public API stable"), discovered restrictions ("tests require Node 20"), and policy rules; a lost constraint is the most dangerous single loss because it converts a safe agent into an unsafe one.
4. Unresolved threads: open questions, deferred subtasks, known-broken states, promises made to the user; these are the agent's return addresses, and losing one means silent non-delivery.
5. State-of-the-world facts that are expensive or impossible to re-derive: what was already changed, created, sent, or deleted; especially anything with side effects, because the world will not tell you twice.
6. Pointers to everything else: file paths touched, commands that produced key results, document IDs; a pointer is a few tokens and converts "lost" into "re-fetchable."

What may be discarded, again with reasons:

- Raw tool output whose conclusions are captured: the file content is on disk, the conclusion is in category 2 or 5.
- Dead-end exploration beyond a one-line "ruled out X because Y": the one line is category 2; the transcript of the dead end is noise.
- Politeness, acknowledgments, and duplicated restatements: zero information content.
- Intermediate reasoning that led to a recorded decision: the decision is the residue; exception: if the user may challenge the decision later, keep a pointer to where the reasoning happened.

Two categories require verbatim preservation and must be exempt from paraphrase: user-stated constraints and exact identifiers (paths, IDs, version numbers, commands, error strings).
Summaries paraphrase, and a paraphrased path or a softened constraint is a bug factory.

## 3.5 Tool-result clearing: the cheapest first move

Before summarizing anything, clear old tool results, because (Chapter 02) they are the bulkiest, fastest-decaying, most-recoverable content in the window.
The operation: replace a tool result's content with a short residue, keeping the message structure intact so the conversation remains well-formed.

```python
CLEAR_RESIDUE = "[tool result cleared to save context; re-run the tool if needed]"

def clear_old_tool_results(messages, keep_last_n=3, min_size_tokens=500, protect=frozenset()):
    tool_msgs = [i for i, m in enumerate(messages) if m["role"] == "tool"]
    for i in tool_msgs[:-keep_last_n] if keep_last_n else tool_msgs:
        m = messages[i]
        if m.get("id") in protect:
            continue                      # e.g. the result the current step depends on
        if count_tokens(m["content"]) >= min_size_tokens:
            m["content"] = f"{CLEAR_RESIDUE} (was {m['tool_name']}, {count_tokens(m['content'])} tokens)"
    return messages
```

Design notes that separate a toy from a production implementation:

- Keep the last N results untouched; the model is usually acting on them right now.
- Clear only results above a size floor; clearing a 40-token result saves nothing and destroys information.
- Protect results that later messages explicitly reference and that have no durable copy elsewhere.
- Record the tool name and original size in the residue so the model can decide whether re-running is worth it.
- Never clear results of side-effecting tools whose output is the only record of what happened (a payment confirmation, a migration log); those belong in category 5 of the contract and should be summarized into durable notes before clearing.

Anthropic's platform absorbed exactly this pattern as a server-side feature in late 2025 (Section 3.8).

## 3.6 Implementing full compaction

A production compaction pipeline has five stages: trigger, split, summarize, verify, reassemble.

### Trigger

Fire on a threshold of the effective window, not the advertised one, and leave headroom for the summary call itself and for the next few turns.

```python
def should_compact(used_tokens, effective_window, headroom_frac=0.20):
    return used_tokens >= effective_window * (1 - headroom_frac)
```

Thresholds in the field cluster around 70-90 percent as of 2025-2026; lower thresholds compact more often (more summary risk, better per-call quality and cache behavior), higher thresholds compact rarely (less summary risk, worse late-window quality).
Also support a manual trigger; operators and users often know a phase boundary just ended, which is the ideal compaction moment because open loops are fewest.

### Split

Choose a split point: everything before it is compacted, everything after survives verbatim.
Split at a message boundary, never inside a tool-call/tool-result pair, and prefer a semantic boundary such as the completion of a subtask.
Keep the most recent K turns verbatim regardless, because recency is where the action is.

### Summarize

Summarize the head with a schema-directed prompt; the schema is the contract of Section 3.4 made executable.

```python
COMPACTION_PROMPT = """Summarize the conversation so far for your own future use.
You will lose access to everything except this summary and the recent messages, so capture:

1. TASK: the user's goal and acceptance criteria, quoting the user verbatim where stated.
2. DECISIONS: each decision made, with its rationale, as a bullet list.
3. CONSTRAINTS: every restriction stated by the user or discovered during work, verbatim.
4. OPEN THREADS: unresolved questions, deferred work, anything promised but not delivered.
5. WORLD STATE: everything already changed or created (files, systems, messages), with exact paths and identifiers.
6. POINTERS: files, commands, and sources consulted, so content can be re-fetched.

Be dense and factual. Do not editorialize. Preserve exact identifiers verbatim."""
```

Run the summary with the same model family the agent uses, or a cheaper model if your evals show parity; the cheap-model temptation is real and sometimes fine for mechanical sessions, but constraint capture is exactly where small models slip, so eval before economizing.

### Verify

Trust but verify, mechanically where possible:

- Check that every file path and identifier that appears in protected categories of the head also appears in the summary or the survivors; regex-level checking catches a large fraction of pointer loss.
- Check the summary is within its token ceiling; runaway summaries defeat the purpose.
- Optionally run a checklist critique pass ("does this summary state the task? list constraints?"), which costs a small call and catches structural omissions.

### Reassemble

The new context: system prompt, then the summary (clearly framed as "summary of earlier conversation"), then the verbatim recent tail, then the current turn.
Persist the pre-compaction transcript to storage before discarding anything from the window; the window is lossy, the transcript on disk is not, and Chapter 06 builds on the transcript as the source of truth.

## 3.7 Case study: Claude Code's compaction

Claude Code (Anthropic's coding agent, behavior described as of late 2025 to early 2026) is the most widely observed production compaction system and illustrates every principle above.

- It tracks context usage continuously and surfaces it to the user as a percentage.
- When usage approaches the window limit, it auto-compacts: the conversation is summarized and the session continues seamlessly with the summary plus recent messages; users can also trigger `/compact` manually, optionally with instructions about what to emphasize.
- Its compaction summary is schema-directed, emphasizing the task, recent work, key decisions, files touched, and next steps, which is a direct instance of the Section 3.4 contract.
- Separately from full compaction, it clears or truncates old bulky tool results (the Section 3.5 pattern), which extends runway between full compactions.
- The full pre-compaction transcript persists in the session log on disk, so `--resume` and history inspection survive compaction; the window is treated as a cache over the transcript, not as the record.
- Users are advised to compact at natural phase boundaries (after a plan is agreed, after a bug is fixed) because summaries are best when open loops are fewest; the same advice falls out of Section 3.6.

Observable failure modes, reported by users throughout 2025 and instructive for your own designs: occasionally a constraint stated early ("use spaces not tabs", "never commit directly") weakens after compaction, and occasionally the agent re-reads files it had already analyzed because the analysis was compacted to a pointer.
Both are the predicted losses of a lossy boundary, and both are mitigated the same way in Claude Code and in your systems: put durable rules in memory files outside the window (CLAUDE.md, Chapter 04) so they are re-injected fresh every session and survive every compaction.

## 3.8 Context editing APIs: the platform absorbs the pattern

In late 2025 Anthropic shipped server-side context management in public beta (beta header `context-management-2025-06-27`, current as of early 2026), with a strategy named `clear_tool_uses_20250919` that clears older tool results server-side when a token trigger is crossed, replacing them with a placeholder the model can see.
Configuration includes the trigger threshold, how many recent tool uses to keep, and exclusions for tools that must never be cleared; cleared content stops counting toward input tokens on subsequent calls.
Anthropic reported internal evaluations where context editing combined with the memory tool improved agentic task performance substantially and cut token consumption sharply on long-horizon tasks; treat the direction as informative and re-measure on your workload rather than quoting their numbers as yours.

Architectural consequences of moving clearing server-side:

- You stop shipping and maintaining the clearing code, and the platform applies it consistently even when your orchestrator is naive.
- You lose fine-grained policy: protection rules like "never clear the migration log" must be expressible in the API's exclusion vocabulary or they do not exist.
- Cache interaction is real: editing the history invalidates the prompt-cache prefix from the edit point onward, so server-side clearing trades cache hits for window space; the same trade exists client-side, but server-side makes it easier to forget (Chapter 07 quantifies when each side wins).
- Compaction proper (summarize-and-replace) remained largely a client-side or harness-level concern as of early 2026; the API clears, the harness summarizes, and OpenAI's Responses API similarly handled truncation policies while leaving semantic summarization to the application layer.

The lesson generalizes: platforms keep absorbing the mechanical layers (clearing, truncation, caching) while the semantic layer (what must survive, the contract) remains your responsibility, because only the application knows what a "decision" or a "constraint" is in its domain.

## 3.9 Failure modes and how to test compaction

Compaction bugs are silent by construction, so testing must be adversarial.

- Summary drift: iterated compaction mutates the task description; test by running a scripted 200-turn session with 5+ compactions and diffing the task statement across summaries; verbatim-quote requirements in the prompt suppress drift.
- Lost constraints: seed sessions with N explicit constraints early, run past compaction, then present temptations to violate each; measure violation rate before and after compaction; this is the single most important compaction eval.
- Lost open threads: promise M deliverables early, complete only some, compact, and check the agent still knows the remainder.
- Compaction loops: if the post-compaction context is still near the threshold (huge system prompt, huge recent tail, or a summary ceiling set too high), the system compacts every turn and burns its budget on summaries; enforce that compaction reclaims a minimum fraction of the window or escalate to a hard failure.
- Split-point corruption: splitting inside a tool-call pair produces malformed conversations that some APIs reject and others accept with degraded behavior; property-test the splitter on adversarial message sequences.
- Poisoned summaries: a prompt-injected instruction in some tool result can survive into the summary in laundered, authoritative form; run injection-seeded sessions and inspect summaries for the payload (Volume 11 treats this class fully).

The honest trade-off summary for this chapter: truncation is free and silent, summarization is expensive and risky, and compaction done well is engineering-heavy but is the only approach that preserves task integrity across long horizons; the cost is roughly a few hundred lines of orchestrator code plus an eval suite, and skipping the eval suite converts the whole mechanism into a liability.

## Exercises

1. Implement `clear_old_tool_results` with protection rules and size floors, run it against a recorded 50+ turn agent transcript, and report tokens reclaimed versus information lost, judged by whether the agent's next action would change.
2. Implement the full five-stage compaction pipeline (trigger, split, summarize, verify, reassemble) for a chat-with-tools agent, using the schema prompt from Section 3.6, and demonstrate a 100+ turn session that stays under a 32k effective window.
3. Build the lost-constraint eval: 10 sessions, 5 seeded constraints each, temptations after compaction; report per-constraint violation rates with and without verbatim-quoting in the summary prompt.
4. Reproduce a compaction loop deliberately (oversized system prompt plus low threshold), observe the pathology, then implement and demonstrate the minimum-reclamation guard.
5. If you have Anthropic API access, enable the context-management beta with `clear_tool_uses_20250919` on a long tool-heavy session, and compare tokens billed and cache hit rates against your client-side clearing implementation; write up which policy controls you missed.
6. Design the compaction contract for an agent in your own domain: enumerate its categories 1-6 concretely (what is a "decision" in your domain, what identifiers must survive verbatim), and turn it into a summary prompt and a mechanical verifier.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Given a filling window, choose among truncation, summarization, tool-result clearing, and full compaction, and justify the choice by recoverability, decay rate, and risk.
- Recite the six survival categories of the compaction contract and explain why constraints and world-state facts are the two most dangerous to lose.
- Sketch the five-stage compaction pipeline with correct split-point rules and a schema-directed summary prompt.
- Describe Claude Code's compaction behavior and extract three transferable design decisions from it, including why durable rules belong in memory files rather than in the summary.
- Explain what Anthropic's 2025 context-editing API does and does not do, and articulate the cache-invalidation trade-off it introduces.
- Name five compaction failure modes and the specific adversarial test for each.
