# Chapter 03 - Communication and Shared State

## What you will master

- The two fundamental channels between agents: message passing and shared artifacts, and the cost model of each.
- Result schemas: designing the structured payload a subagent returns so the parent can trust and compose it.
- Filesystem and workspace sharing, including git worktrees as the isolation mechanism for parallel code agents.
- The telephone-game problem: how information degrades across summarization boundaries and the engineering that limits the loss.
- Structured handoff payloads for control-transfer topologies.
- Context quarantine: deciding what must never cross an agent boundary.

## 1. Two channels, one trade-off

Every mechanism for moving information between agents reduces to one of two channels.
Message passing: agent A produces tokens that are placed into agent B's context.
Shared artifacts: agent A writes to a store (file, database, object store) and agent B reads some of it later.

The trade-off between them is push vs pull of context cost.
A message forces its full cost into the recipient's context immediately, whether or not the recipient needs all of it.
An artifact costs the recipient nothing until read, and can be read selectively (a file path plus a grep beats a pasted file), but requires the recipient to know it exists and to spend actions retrieving it.
Messages are ephemeral and ordered; artifacts are durable and random-access.
Messages disappear from the system when the recipient's context is compacted; artifacts survive agent death, which makes them the only channel suitable for checkpointing and for results too large to summarize.

The practical synthesis used by every serious system as of early 2026: pass small structured summaries as messages, and pass bulk content as artifact references inside those messages.
Anthropic's research system engineering notes describe exactly this evolution: subagents storing outputs externally and passing lightweight references to the coordinator, instead of piping everything through the orchestrator's context, precisely to avoid the information loss of multi-stage summarization.
The downside of reference passing is dangling references and stale reads, which is why artifact stores need naming conventions and immutability rules (section 5).

## 2. Message passing done properly

### 2.1 The message is a prompt

A message from agent A to agent B is, mechanically, prompt text for B.
Everything Volume 02 taught about prompts applies: B will weight what is salient, misread what is ambiguous, and ignore what is buried.
So inter-agent messages are written for the reader model, not for a human: front-load the objective, use explicit structure, avoid pronouns whose referents live only in A's context, and never assume B knows why the message was sent.
The most common inter-agent bug is A writing a message that is only interpretable given A's context, then B, lacking that context, interpreting it differently; this is Cognition's "actions carry implicit decisions" problem surfacing in the channel itself.

### 2.2 Full traces vs summaries

There is a spectrum of message fidelity.
At one end, share the full trace: B receives A's entire message-and-tool history, as Cognition recommends for coupled work; zero information loss, maximum context cost, and only feasible when the combined trace fits a window.
At the other end, share a conclusion: one sentence of result, minimum cost, maximum loss.
Production systems pick points between: structured summaries with claims plus evidence pointers, or summaries plus on-demand access to the raw trace stored as an artifact.
The correct fidelity is set by the coupling of the work: coupled decisions need decisions-and-reasons transmitted, aggregatable facts need only claims-and-sources.

### 2.3 Synchronous vs asynchronous

Synchronous request-response (parent blocks on child) is simple and is what subagent tools implement; the parent's context cleanly contains spawn then result.
Asynchronous messaging (queues, callbacks, agents notifying each other mid-flight) buys pipelining and steering but imports the classic distributed-systems problems: ordering, idempotency, at-least-once duplicates, and the new one, deciding which in-flight context an arriving message should be injected into.
As of early 2026, most production systems remain synchronous at the orchestration layer because the debugging cost of async agent systems is severe; adopt async only when latency requirements force it, and then keep the message types few and schematized.

## 3. Result schemas

A subagent result is an interface, and interfaces deserve schemas.
Free-text results invite the parent to misparse, over-trust, or silently drop findings; schemas make omissions visible and composition mechanical.

A battle-tested minimal schema for research-style workers:

```json
{
  "task_id": "vendor-analysis-03",
  "status": "complete | partial | failed",
  "summary": "3-5 sentences, the claim-level result",
  "findings": [
    {
      "claim": "one atomic factual claim",
      "evidence": "quote or data point",
      "source": "url or artifact path",
      "confidence": "high | medium | low"
    }
  ],
  "artifacts": ["scratch/vendor3_notes.md"],
  "gaps": ["what was not covered and why"],
  "cost": {"tool_calls": 18, "approx_tokens": 42000}
}
```

Design rules behind each field.
`status` must be tri-state, because "partial" is the common real outcome and forcing it into complete/failed corrupts synthesis either way.
`findings` are atomic claims with evidence and source, because synthesis needs to weigh and cite claims individually, and because an LLM judge or citation pass can verify claim-evidence pairs but cannot verify prose.
`confidence` is self-reported and therefore weak evidence, but it is cheap and it usefully flags where the parent should verify; do not treat it as calibrated.
`gaps` is the anti-silent-failure field: a worker that ran out of budget must say what it skipped, or the parent will assume coverage that does not exist, one of the dropped-work failure modes in Chapter 06.
`cost` enables the orchestrator to enforce budgets and enables you to attribute spend in traces.
Enforce the schema with structured output (Volume 02) or validate-and-retry; a schema the model can silently violate is a suggestion, not an interface.
The downside of schemas is that they can truncate genuinely novel observations that fit no field, so include one free-text `notes` escape hatch and review what accumulates there.

## 4. Shared filesystems and workspaces

### 4.1 The filesystem as the agent-native store

For tool-using agents, the filesystem is the most ergonomic shared store: models are heavily trained on file operations, paths are stable references, grep and head enable selective reads, and durability plus inspectability come free.
Standard workspace conventions that recur across systems: a plan or task file the orchestrator owns; a scratch directory per agent for private notes; a results directory where each worker writes exactly its own deliverable; and a read-mostly source area.
The convention doing the real work is ownership: every path has exactly one writer, so no coherence protocol is needed.

### 4.2 Git worktrees for parallel code agents

Parallel coding agents sharing one checkout is a coherence disaster: they fight over the index, overwrite working-tree files, and see each other's half-finished edits.
Git worktrees solve this at the VCS layer: one repository, N working directories, each on its own branch.

```bash
git worktree add ../proj-agent-a feature/agent-a
git worktree add ../proj-agent-b feature/agent-b
# each agent runs with cwd set to its own worktree
git worktree list
git worktree remove ../proj-agent-a
```

Each agent gets full-repo context, complete isolation of uncommitted state, and a branch whose diff is exactly that agent's work, which makes review and attribution trivial.
Merging branches afterward is where the decision-coupling from Chapter 01 reappears: worktrees isolate files, not decisions, so two agents can still produce textually mergeable but semantically incompatible changes.
Worktrees are therefore the right mechanism only after you have partitioned tasks at the decision level; they are widely recommended for coding-agent fleets (Claude Code documentation among others, 2025) precisely for cross-task parallelism, not for splitting one change.
Operational costs: each worktree needs its own build artifacts and dependency installs, ports and dev servers collide unless parameterized, and stale worktrees leak disk, so fleet tooling should create and destroy them programmatically.

### 4.3 Databases and stores beyond the filesystem

Long-lived multi-agent products outgrow flat files: concurrent digests want a real queue, session state wants a KV store, and knowledge wants the retrieval systems of Volume 05.
The design rule does not change: single writer per logical region, append-preferred, schema at every boundary.
What changes is that you must hand agents ergonomic tools for the store, because a model fumbling a bespoke query API loses more reliability than the store gains in consistency.

## 5. The telephone game and how to dampen it

### 5.1 The failure

Chain k summarization boundaries and multiply their retention rates: even 90% fidelity per hop leaves roughly 59% after five hops.
Worse than uniform loss, summarization loss is biased: models preserve conclusions and drop qualifiers, preserve the salient and drop the load-bearing detail, and normalize surprising findings toward the expected.
So deep topologies do not just know less at the root; they are systematically overconfident and under-caveated at the root.

### 5.2 Engineering the loss down

- Minimize hops: shallow topologies (Chapter 02); every level you delete is a boundary you do not pay.
- Ship evidence with claims: the schema's claim-evidence-source triple lets any downstream reader re-derive or spot-check without the intermediate contexts.
- Keep raw traces as artifacts: summaries for flow, full trace on disk, so a suspicious synthesis step can pull the primary record instead of trusting the chain.
- Quote, do not paraphrase, for anything load-bearing: exact error messages, exact interface signatures, exact numbers; paraphrase is where corruption enters.
- Make omission explicit: the `gaps` field, and orchestrator prompts that demand it; unknown-and-declared is recoverable, unknown-and-silent is not.
- Verify at the point of use: a citation or fact-check pass at the end (as in Anthropic's citation subagent) catches chain corruption where it matters, at the deliverable.

The residual trade-off: every dampening technique spends tokens (evidence is bigger than claims, raw traces cost storage and retrieval actions), so apply full rigor to load-bearing facts and allow lossy summaries for color.

## 6. Structured handoff payloads

Handoff topologies (Chapter 02) transfer control, and the payload transferred determines whether the next agent can actually continue.
A handoff that passes only the conversation history forces the receiver to re-derive intent from raw transcript; a handoff that passes only "user needs refunds" drops the constraints already discovered.
The robust shape is history plus a structured handoff note:

```json
{
  "from": "triage",
  "to": "refunds",
  "reason": "order not received, past carrier SLA",
  "established_facts": {"order_id": "A-1873", "delivery_sla_days": 7, "days_elapsed": 12},
  "already_tried": ["carrier trace lookup"],
  "open_questions": ["customer preference: refund vs reship"],
  "constraints": ["customer is enterprise tier: no automated denial"]
}
```

`established_facts` prevents the receiver re-asking the user known answers, the most user-visible handoff failure.
`already_tried` prevents duplicated actions, which for side-effecting tools (refunds, emails) is a correctness issue, not just waste.
`constraints` carries policy decisions made upstream that the receiver's own prompt does not contain.
The same shape serves sequential pipelines: each stage emits its artifact plus a handoff note of decisions made and assumptions taken, which is precisely the "share decisions, not just artifacts" remedy for implicit-decision divergence.

## 7. Context quarantine

Context quarantine is the deliberate use of agent boundaries to keep material out of a context, and it is the inverse skill of communication: deciding what must not flow.

Four things worth quarantining.
Bulk: exploration transcripts, big files, and search noise stay in the worker that produced them; the parent gets conclusions and references, which is the core context-isolation win of Chapter 01.
Bias: a verifier agent should not receive the generator's reasoning, only the artifact and the spec, because seeing the reasoning anchors the verifier into the same blind spots; fresh-context review is quarantine used for independence (Chapter 04).
Poison: untrusted content (web pages, third-party documents, tool outputs from external systems) can carry prompt injection; processing it in a low-privilege quarantined agent that returns only schema-validated extractions keeps injected instructions away from the agent that holds powerful tools, a pattern treated fully in Volume 11.
Secrets: credentials and sensitive data should be reachable only by the narrow agent whose task requires them, so a compromised or confused peripheral agent cannot exfiltrate what it never saw.

Quarantine has a real cost: it is precisely the information loss this chapter has been fighting, applied on purpose.
The discipline is to make quarantine decisions explicitly per boundary (what crosses, what must not, and why) rather than letting them fall out of whatever the summarization happened to keep.

## 8. A worked boundary design

Task: orchestrator plus three parallel workers auditing a codebase for deprecated API usage, then one fixer agent per confirmed site.
Channel design: workers are read-only and return the schema of section 3, with findings as file-path-plus-line claims and exact code quotes as evidence; raw grep transcripts stay quarantined in the workers.
Store design: workers write nothing; the orchestrator owns a single `audit.md` artifact compiled from validated results, so there is one writer.
Fix phase: the orchestrator partitions confirmed sites into decision-independent groups (one module each), spawns fixers in separate git worktrees, and hands each a payload containing its sites, the exact replacement rule as a quoted spec, and the constraint list; fixers return branch names plus a handoff note of assumptions.
Verification: a fresh-context reviewer diffs each branch against the quoted rule before merge.
Every mechanism in this chapter appears once, and each was chosen against a named failure: schema against misparse, quotes against paraphrase corruption, single writer against clobbering, worktrees against checkout conflicts, decision-level partitioning against semantic merge conflicts, quarantined review against anchored verification.

## Exercises

1. Take a real subagent result you have seen (or generate one) as free prose, then re-express it in the section 3 schema; list every fact that had no field and every field the prose had silently omitted.
2. Build the telephone game empirically: summarize a dense technical document through four sequential LLM summarization hops, then diff hop 4 against hop 0 for dropped qualifiers, altered numbers, and confidence inflation.
3. Script a two-worktree fleet: create worktrees, run two independent edits (real or simulated), merge both branches, and then construct a case where the merge is textually clean but semantically broken; state the partitioning rule that would have prevented it.
4. Design the handoff payload schema for a three-agent support flow (triage, billing, cancellation) and write the test conversation that catches a re-asking failure when `established_facts` is removed.
5. Implement a quarantine wrapper: a function that sends untrusted web content to a tool-less LLM call returning a fixed extraction schema, validates it, and rejects on schema violation; feed it a page containing an injected instruction and confirm the instruction cannot reach the parent.

## Godhood check

- State the cost asymmetry between message passing and shared artifacts, and the hybrid pattern production systems converge on.
- Explain why an inter-agent message must be written as a prompt for a reader with zero shared context, and name the bug class when it is not.
- Recite the minimal worker result schema and justify `status`, `gaps`, and `cost` individually by the failure each prevents.
- Explain what git worktrees do and do not isolate, and therefore which parallelization decision must precede their use.
- Give the compounding arithmetic of summarization loss and three concrete dampening techniques with their token cost.
- List the four categories worth quarantining and the distinct risk each quarantine addresses.
- Explain why "single writer per path" eliminates the need for a coherence protocol, and what you give up by enforcing it.
