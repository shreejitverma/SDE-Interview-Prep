# Chapter 04 - Subagents in Practice

## What you will master

- The subagent pattern precisely defined: a disposable clean-context agent invoked as a tool by a parent.
- Prompt design for subagents, which differs from top-level prompting because the subagent lacks everything the parent knows.
- Parallel fan-out with synthesis: budgeting, spawning, collecting, and merging.
- Verification subagents: using fresh context as an independence mechanism for adversarial review.
- Claude Code's subagents and its Agent/Task tool as a production case study, with the design decisions and their reasons.

## 1. The pattern, precisely

A subagent is an agent invoked by another agent through the tool interface: the parent emits a tool call whose arguments are the subtask description, a fresh agent loop runs to completion in its own context, and its final message returns as the tool result.
Three properties define the pattern and distinguish it from the general multi-agent zoo.
The context is clean: the subagent sees its system prompt, the task description, and nothing else of the parent's history.
The lifetime is the task: the subagent is created for one delegation and discarded after returning, holding no state between invocations.
The interface is the tool contract: one request in, one result out, no mid-flight chatter, which makes the parent's trace read as an ordinary tool call and keeps the topology a star.

Because the subagent is just a tool, everything Volume 03 established about tools applies: it needs a description that tells the parent when to use it, its result must be parseable, and its failures must surface as structured errors rather than silence.
The pattern captures both fundamental wins from Chapter 01 in their simplest form: isolation, because the subagent's exploration never touches the parent's window, and parallelism, because independent tool calls can run concurrently.
Its limits are equally structural: no shared context means the subagent cannot know anything the parent forgot to say, and one-shot results mean the parent cannot steer mid-task; both limits drive the prompt-design discipline of the next section.

## 2. Prompt design for subagents

### 2.1 The core fact: they know nothing

The parent has been working for an hour: it knows the user's goal, the constraints discovered along the way, the conventions of the repository, and the three approaches already rejected.
The subagent knows none of this.
Every subagent failure post-mortem eventually reduces to the same finding: the parent assumed shared context that did not exist.
Anthropic's engineering account of their research system reports exactly this: early orchestrators delegated with short vague instructions like "research the semiconductor shortage", and subagents duplicated each other's work, misinterpreted scope, and wandered, until delegation prompts were made explicit and detailed.

### 2.2 The delegation contract

A production-grade subagent task description states five things.

1. Objective: what question to answer or artifact to produce, phrased so success is checkable by the subagent itself.
2. Context: the minimum background the subagent needs, stated explicitly, including relevant constraints and decisions already made; if a fact matters, quote it, do not allude to it.
3. Output format: the exact result schema (Chapter 03), because the parent will parse this mechanically during synthesis.
4. Effort budget: roughly how many tool calls or how much depth this subtask deserves, because the subagent has no way to infer proportionality from a task description alone; Anthropic's system encodes explicit scaling rules such as simple fact-finding warranting one agent with a handful of tool calls, and complex research warranting multiple subagents with divided responsibilities.
5. Boundaries: what is out of scope, what tools or sources to prefer or avoid, and what to do on failure (return `status: failed` with what was learned, never fabricate).

The template as pseudocode:

```text
OBJECTIVE: Find every caller of AuthClient.refresh() outside tests.
CONTEXT: We are renaming refresh() to renew(); the repo is a Python monorepo
  rooted at /src; the public API surface is /src/authlib only.
OUTPUT: JSON per result schema v1: findings[] of {path, line, quote}, gaps[].
BUDGET: read-only; aim for under 15 tool calls; grep before reading files.
BOUNDARIES: do not edit anything; ignore /vendor; if the count exceeds 50,
  return the count plus the module-level distribution instead of every site.
```

The cost of this rigor is real: detailed delegation prompts are long, and writing them consumes orchestrator tokens and planning attention.
The alternative costs more: a vague delegation that misfires wastes the entire subagent run and often poisons synthesis with confidently wrong results.

### 2.3 The subagent's own system prompt

When you define reusable subagent types (a code-searcher, a test-runner, a fact-checker), the system prompt carries the stable half of the contract: role, method, tool guidance, output schema, and refusal rules, so per-call descriptions carry only the variable half.
Keep subagent system prompts narrow on purpose: a subagent that does one thing with three tools outperforms a general assistant handed the same task, because the narrow prompt removes degrees of freedom that only produce variance.
The trade-off is proliferation: a fleet of hyper-specialized subagent types becomes its own maintenance surface, and the parent's choice among twenty similar specialists becomes a new error source, so consolidate types until each has a clearly distinct trigger condition.

## 3. Parallel fan-out with synthesis

### 3.1 Decompose

Fan-out begins with the parent writing the partition, and the partition quality bounds everything downstream.
Good partitions have three properties: decision-independence between shards (Chapter 01), comparable size so stragglers do not dominate latency, and collectively-exhaustive coverage stated explicitly so dropped work is detectable.
Have the orchestrator write the partition down as an artifact before spawning; a written task list makes duplicated and dropped shards visible in review, and it survives orchestrator compaction.

### 3.2 Spawn

Issue the subagent tool calls in one batch so they run concurrently; a parent that spawns serially is paying multi-agent costs for single-agent latency.
Cap the fan-out deliberately.
The binding constraints are the parent's synthesis capacity (N results must fit in its window alongside the plan), rate limits and burst cost, and the empirical observation that marginal shards add less value than they cost once coverage saturates; Anthropic's guidance embeds effort scaling for this reason, with even complex tasks capped around ten to twenty subagents rather than hundreds.

### 3.3 Collect

Collection is where distributed-systems hygiene enters.
Set per-subagent timeouts and treat timeout as `status: failed` with a synthetic result, so one hung worker cannot stall the batch.
Validate every result against the schema before synthesis, and retry once with the validation error appended; discard on second failure and record the gap.
Never let a failed shard silently vanish: synthesis must receive the full shard list with per-shard status, or the deliverable will claim coverage it lacks.

### 3.4 Synthesize

Synthesis is a first-class reasoning task, not concatenation, and under-investing in it is the most common fan-out failure.
The synthesizer must deduplicate overlapping findings, reconcile contradictions (contradiction between workers is signal, and should be surfaced or adjudicated, never averaged away), weigh evidence quality using the claim-evidence pairs, and explicitly enumerate uncovered gaps from the shard statuses.
For large N, synthesize hierarchically in one extra level (map-reduce, Chapter 02) rather than forcing one context to hold everything, and accept the extra summarization boundary as the price.
When the deliverable needs citations or verifiable claims, run a final verification pass over the synthesized draft against the collected evidence, which is where the next section's pattern connects.

## 4. Verification subagents

### 4.1 Why fresh context verifies better

A model reviewing its own work inside the same context is anchored: its reasoning, assumptions, and blind spots are all present and actively attended.
A verifier subagent receives only the artifact and the spec, so it must re-derive judgments independently; the generator's rationalizations are quarantined away (Chapter 03).
This exploits the generator-verifier asymmetry: checking work against a spec is usually an easier task than producing it, so even a same-strength model in a fresh context catches real defects, and a cheaper model often suffices.
The honest caveat: same-model verification shares model-level blind spots, so fresh context buys independence from the trajectory, not from the weights; for high stakes, vary the model or add non-LLM verifiers (tests, type checkers, linters), which dominate LLM review wherever they apply.

### 4.2 Designing the adversarial reviewer

Give the verifier an adversarial role and a rubric, not an open question.
"Review this" yields pleasantries; "find reasons this fails the spec, checking each rubric item, and return findings as {severity, location, claim, evidence}" yields defects.
Withhold the generator's reasoning and self-assessment.
Bound the loop: generate, verify, revise once against the findings, verify once more, then stop or escalate; unbounded generate-verify loops oscillate and burn budget, because verifier findings at low severities are partly noise and the generator will chase them indefinitely.
Calibrate severity in the rubric so the parent can apply a merge threshold mechanically (block on high, note mediums, ignore lows), which converts review from vibes into a gate.

### 4.3 Where verification subagents pay

They pay wherever errors are expensive and specs are checkable: code review before merge, fact-and-citation checking of research output, policy compliance of user-facing text, and evaluation of other agents' transcripts (the LLM-as-judge machinery of Volume 10 is this pattern industrialized).
They do not pay for taste-heavy judgments without a rubric, where reviewer variance swamps signal.

## 5. Case study: Claude Code subagents and the Agent tool

Claude Code (Anthropic's CLI coding agent) ships the subagent pattern as a first-class feature, and its design choices, current as of late 2025, map one-to-one onto this chapter's principles.

### 5.1 The mechanism

The main agent has an Agent tool (historically named Task) that spawns a subagent: the tool call carries the task description, the subagent runs a full agent loop in a separate context with its own tool access, and only its final report returns to the parent as the tool result.
Users can define custom subagent types as Markdown files with YAML frontmatter in `.claude/agents/` (project) or `~/.claude/agents/` (user), each specifying a name, a description of when to use it, an optional tool allowlist, and a system prompt.
The description field is load-bearing: the parent model reads it to decide delegation, so it is written as a trigger condition, exactly the tool-description discipline of Volume 03.

### 5.2 The design decisions and their reasons

Isolation is total: the subagent does not see the parent's conversation, which forces the delegation-contract discipline of section 2 and preserves the parent's window for the main line of work; the documented cost is exactly the one this volume predicts, that vague delegations fail, and users are advised to write detailed task descriptions.
Return is final-message-only: no mid-flight steering, keeping the interface a tool contract and the trace readable; the cost is that a subagent heading in a wrong direction cannot be corrected, only discarded.
Nesting is forbidden: subagents cannot spawn subagents, capping the topology at depth two by construction, which bounds token blowup and keeps failure attribution tractable (Chapter 02's shallow-topology rule enforced in the harness).
Parallelism is supported: the parent can run multiple subagents concurrently, and the dominant production use is parallel read-only exploration, such as searching a large codebase along several hypotheses at once while the parent stays clean for the edit.
Tool allowlists per subagent type implement least privilege: a reviewer subagent with read-only tools cannot damage the repo regardless of prompt confusion, which is quarantine of capability rather than of information.

### 5.3 What the case study teaches

The feature is conservative on every axis where this volume documents failure: shallow, star-shaped, synchronous, schema-light but contract-heavy, least-privilege by default.
That conservatism is the lesson: a production harness with maximal usage data chose the narrowest viable version of multi-agency, and it chose it primarily for context isolation, with parallel research as the secondary win, matching Chapter 01's honest accounting.
The pattern's residual weaknesses are also visible in practice: parents sometimes under-delegate (doing bulk exploration inline and bloating their own context) or over-delegate (spawning a subagent for a two-command job, paying spawn overhead and summary loss for nothing), and calibrating that judgment remains prompt engineering on the parent side.

## 6. Implementation sketch

A minimal but real subagent harness in provider-neutral Python-shaped pseudocode:

```python
def run_subagent(task: str, system_prompt: str, tools: list, budget: int) -> dict:
    ctx = [system(system_prompt), user(task)]
    for _ in range(budget):
        msg = llm(ctx, tools=tools)
        if msg.tool_calls:
            ctx += [msg] + [execute(tc) for tc in msg.tool_calls]
        else:
            return validate_or_retry(msg.text, RESULT_SCHEMA, ctx)
    return {"status": "failed", "gaps": ["budget exhausted"], "findings": []}

def fan_out(parent_ctx, shards: list[str]) -> list[dict]:
    futures = [spawn(run_subagent, shard, SEARCHER_PROMPT, READ_TOOLS, 15)
               for shard in shards]
    return [f.result(timeout=300) if not f.timed_out
            else {"status": "failed", "gaps": ["timeout"], "findings": []}
            for f in futures]
```

The two functions encode the chapter: clean context per task, hard budget, schema validation with one retry, batch spawn, timeout-as-failure, and no shard ever silently missing from the returned list.
Everything else (better prompts, better synthesis, tracing) layers on top without changing this skeleton.

## Exercises

1. Write the full delegation contract (all five parts) for the task "determine whether our public Python SDK has any breaking changes against the last released version", then have a fresh-context agent execute only your text and grade how much it had to guess.
2. Take a vague delegation you wrote and run it through five independent subagent executions; classify the divergence in their interpretations, then rewrite the contract and measure the divergence again.
3. Build the section 6 harness against a real LLM API, run a three-shard fan-out on a research question, and make the collector inject one forced timeout; verify the synthesis output names the gap.
4. Construct a generator-verifier pair for SQL migration scripts: generator writes the migration, verifier gets only the schema and the migration with an adversarial rubric (data loss, lock duration, reversibility); measure defects caught with and without quarantining the generator's reasoning.
5. Define a custom Claude Code subagent (or equivalent in your harness) for read-only dependency auditing with a minimal tool allowlist, and write the description field so the parent triggers it exactly when a dependency question arises and never otherwise.

## Godhood check

- Define the subagent pattern by its three structural properties and explain what each buys and each forbids.
- Recite the five parts of the delegation contract and, for each, name the observed failure when it is omitted.
- Explain why effort budgets must be stated explicitly to subagents and how Anthropic's system encodes proportionality.
- Walk the four fan-out stages and identify the failure absorbed at each (bad partition, serial spawn, silent shard loss, concatenation-as-synthesis).
- Explain precisely what fresh-context verification makes independent, what it does not, and the two escalations when weight-level blind spots matter.
- List four conservative design choices in Claude Code's subagent feature and map each to the failure mode it forecloses.
- State the over-delegation and under-delegation failure modes and the cost signature of each.
