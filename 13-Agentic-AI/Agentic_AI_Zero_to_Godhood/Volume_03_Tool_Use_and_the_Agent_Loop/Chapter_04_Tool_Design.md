# Chapter 04 - Tool Design

## What you will master

- The agent-computer interface (ACI) as a discipline equal in importance to prompt engineering, and why interface quality often matters more than model choice.
- Naming and description ergonomics: writing tool definitions the way you would write documentation for a talented new hire with no institutional memory.
- The granularity decision: one polyvalent tool versus many narrow tools, and the criteria that actually decide it.
- Returning errors as observations that steer the model toward recovery instead of dead ends.
- Token-efficient tool output: pagination, truncation, filtering, and response-format options.
- Concrete lessons from two heavily studied tool surfaces: SWE-agent and Claude Code.

## 4.1 The agent-computer interface

Human-computer interface design asks how to present a system to human perception and motor abilities.
Agent-computer interface design asks the same question for a language model, and the answer differs because the user differs.

The term ACI was popularized by the SWE-agent work out of Princeton, published in 2024, which demonstrated something the field has re-confirmed ever since: holding the model constant and improving only the interface produced large gains on software-engineering tasks.
The model was never the whole system; the model plus its interface was.

The differences between the model-user and the human-user drive everything in this chapter.

The model has no persistent visual field; anything not in the transcript does not exist for it, so every observation must be self-contained.
The model pays per token to perceive; a verbose output is not merely noisy but expensive, and it dilutes attention over the tokens that matter.
The model cannot poke around idly to build intuition; each exploration step costs a full turn, so interfaces should minimize the turns needed to locate information.
The model is text-native and format-agile but has no out-of-band channel; the tool result is the entire sensory experience of the action.

Anthropic's engineering guidance states the resulting discipline plainly: give tool design the same prompt-engineering care as system prompts, because the tool definitions and results are prompts, just prompts you write once and pay for on every turn.

A useful drill from that same guidance: before shipping a tool, imagine being the model.
You see only the definition and the conversation so far; is it obvious when to use this tool rather than its neighbors, what each argument means, and what you will get back.
If a smart human contractor would need to ask a clarifying question, the model will guess instead, and guesses become bugs.

## 4.2 Naming and description ergonomics

The model chooses tools by reading names and descriptions, so those strings are your control surface.

### Names

Use verb_noun names that state the action: `read_file`, `search_code`, `create_ticket`, `send_email`.
Avoid abbreviations and internal jargon; `get_cust_acct_v2` forces the model to infer what you could have said.
Make sibling tools lexically parallel, because parallel names teach the pattern: `read_file`, `write_file`, `edit_file` is a system the model can extrapolate; `read_file`, `save`, `apply_patch` is three facts to memorize.
Never ship two tools whose names suggest overlapping purposes without descriptions that draw the boundary, because overlap converts selection into a coin flip.

### Descriptions

The description answers four questions, in roughly this order: what the tool does, when to use it (and when to use something else instead), what the arguments mean, and what comes back including limits.

A weak description:

```json
{"name": "search", "description": "Searches the database."}
```

A strong description:

```json
{
  "name": "search_orders",
  "description": "Search customer orders by keyword, status, or date range. Returns at most 20 orders as a compact table of id, date, status, and total. Use this to locate orders when you do not have an order id; if you already have an id, use get_order instead, which returns full detail. Results are sorted newest first."
}
```

The strong version resolves the selection boundary against a sibling, sets result-shape expectations, and preempts a wasted turn of the model calling search when it should have called get.

Two further rules, both learned repeatedly and expensively by the field.

State trigger conditions explicitly, not just capabilities, because as of early 2026 the strongest models are conservative tool users and prescriptive "call this when..." language measurably improves appropriate-use rates.
Do not use aggressive imperatives like "you MUST always use this tool"; on current instruction-faithful models this overtriggers, and the tool fires on tasks where it is wrong.
Calibration beats volume.

### Argument ergonomics

Every argument gets its own description, with format and an example inline: `"City name, optionally with country, e.g. 'Paris, France'"`.
Prefer arguments the model can produce from what it naturally has in context; a tool that needs an opaque `internal_shard_key` the model has never seen forces a hallucination.
Where an identifier is required, provide a discovery tool that returns it, and mention that tool in the description of the one that consumes it.
Default aggressively in the executor so optional arguments are truly optional, and say what the default is.

## 4.3 Granularity: one polyvalent tool versus many narrow tools

The single most consequential ACI decision is how much capability to pack per tool.

At one extreme sits the fully polyvalent tool: `bash`, one string in, one string out, capable of nearly anything.
At the other sits the fully decomposed surface: `list_directory`, `read_file`, `count_lines`, each doing one thing with typed arguments.

Neither extreme wins in general, and the criteria that decide are worth holding explicitly.

Polyvalent tools win on coverage and definition cost.
One bash tool covers a thousand operations you did not anticipate, costs a few hundred tokens of definition, and exploits the model's deep pretraining familiarity with shell idioms.
Frontier models as of early 2026 are genuinely good at shell one-liners, and that competence is free capability.

Narrow tools win on four specific properties, and these properties, not tidiness, are the reasons to pay for them.

Gating: a dedicated `delete_records` tool with typed arguments can be wrapped in an approval gate; a `bash` string containing an `rm` buried in a pipeline cannot be reliably gated by inspection.
Invariants: a dedicated `edit_file` tool can refuse to edit a file the agent has not read since it last changed, enforcing a read-before-write staleness check no shell command can enforce.
Observability and rendering: typed calls can be logged, audited, and rendered as rich UI, while opaque strings can only be echoed.
Scheduling: the harness can mark `read_file` and `search` as parallel-safe and run them concurrently, while bash strings must be serialized because any one of them might mutate state.

The synthesis used by the strongest production agents, Claude Code among them, is a deliberate hybrid: keep a polyvalent bash tool for the long tail, and promote an operation to a dedicated tool when it needs gating, invariants, rendering, or parallel scheduling, or when it is frequent enough that a purpose-built interface saves tokens and errors.
Promotion, not proliferation: each added tool grows every request by its definition and grows the selection problem the model must solve, and past a few dozen well-differentiated tools, selection accuracy degrades and deferred-loading patterns become necessary.

A related consolidation heuristic from Anthropic's guidance: if your agent routinely chains the same three calls in sequence to get one meaningful thing done, ship one tool that does the sequence, because you pay a full model turn per call and intermediate results pollute context.
`schedule_event` that finds a slot and books it beats `list_availability` plus `check_conflicts` plus `create_event`.

## 4.4 Errors as observations

Chapter 3 established the mechanic: tool failures return as results with `is_error: true` rather than raising.
This section is about the content of those results, because error text is steering input, and most error text is written as if for a stack-trace archaeologist rather than for an actor deciding what to do next.

The quality ladder for an error observation, worst to best.

Level 0, silent lie: return an empty success; the model builds on a false world model and the trajectory derails invisibly.
Level 1, bare signal: `"Error"`; the model knows only that something failed and will often retry the identical call.
Level 2, diagnosis: `"Error: file not found: /app/src/main.py"`; the model can reason toward a fix.
Level 3, diagnosis plus remedy: `"Error: file not found: /app/src/main.py. Nearest match: /app/src/main/app.py. Use search to locate files by name."`; the model's next action is nearly determined, and it is the right one.

Aim for level 3 wherever the executor can compute a remedy cheaply: nearest-match suggestions for bad paths, valid-enum listings for bad values, "did you mean" for close misspellings, and the name of the discovery tool for missing identifiers.
This is the same design sense as good compiler diagnostics, and it pays the same dividend: fewer round trips to a working state.

Match error format to error audience.
Truncate stack traces to the frames that carry meaning; a 200-line traceback is mostly noise tax.
Preserve the machine-relevant parts verbatim - exception type, message, failing line - because the model reasons over exact strings.

Finally, validate arguments before side effects, and report all violations at once.
An executor that fails on the first bad argument of three teaches the model one constraint per turn; one that reports all three converges in a single retry.

## 4.5 Token-efficient output

Every byte a tool returns is paid for on the turn it arrives and on every subsequent turn of the trajectory, because the transcript is resent.
Output design is therefore economics, and the budget mindset from Anthropic's tool-writing guidance is the right one: an agent's context is a finite resource and tool results are spending it.

### Filtering beats truncating

The best truncation is the one you never perform because the tool asked for what mattered.
Give search tools result caps and narrowing parameters.
Give list tools filters on the fields callers actually filter by.
Return compact projections by default - id, name, status - rather than full records, with a detail tool for drill-down.

### Pagination

When a result legitimately exceeds a page, paginate with an explicit, actionable continuation: state what was returned, what remains, and exactly how to get the next page.

```
Showing orders 1-20 of 143 matching status=refunded.
To continue, call search_orders with the same arguments and page=2.
```

The continuation instruction matters: an output that ends silently at 20 items teaches the model there were 20 items.

### Truncation

When you must cut, cut honestly and instructively: say that truncation happened, how much is missing, and the narrower call that avoids it.
Truncate at semantic boundaries - whole lines, whole records - never mid-JSON, because a syntactically broken observation invites a parsing hallucination.
Prefer head-plus-tail truncation for logs, since failures concentrate at the ends.

### Response formats

A tool can offer response-format options when different consumers need different fidelity: a `detail` parameter taking `concise` or `full` lets the same tool serve a quick scan and a deep read.
Concise formats also mean choosing representations by token efficiency: a compact aligned table or CSV block beats pretty-printed JSON with repeated keys for homogeneous rows, while JSON wins for nested or irregular data.
Numeric line-number prefixes on file reads, as in Chapter 3's `read_file`, are a format choice too: they cost a few tokens per line and buy the model the ability to reference and edit by line reliably.

### Structured signals travel free

Exit codes, match counts, and durations are cheap tokens with high information density; include them.
`"(exit code: 1)"` after empty stderr converts a mystery into a fact.

## 4.6 Case study: SWE-agent

The SWE-agent paper (Yang et al., 2024) is the cleanest published demonstration that ACI design moves capability, because it changed only the interface on a fixed model and measured the difference on SWE-bench.
Its lessons generalize far beyond coding agents.

Lesson one: replace the terminal's implicit state with explicit, compact views.
SWE-agent's file viewer shows a window of about 100 lines with line numbers, plus current position and total length, instead of dumping whole files or relying on interactive pagers, which agents handle poorly because pagers assume a human with keys to press.
Interactive-by-design programs are ACI poison; every tool should complete in one shot and return.

Lesson two: make search output decision-shaped.
Their search returns file-level summaries and caps results, having found that flooding the context with every raw match harmed performance; the agent needs enough to choose where to look next, not the whole haystack.

Lesson three: put guardrails in the tool, not the prompt.
SWE-agent's editor runs a linter on every edit and rejects edits that introduce syntax errors, returning the lint message as the observation.
Malformed-edit failures, previously a dominant error mode, became recoverable single turns.
The general principle: when a class of model error is mechanically detectable, detect it in the executor and bounce it back as a level-3 error observation, because prevention inside the tool is cheaper than instruction inside the prompt.

Lesson four: feedback on state changes must be explicit.
After an edit, SWE-agent shows the edited region with surrounding context, so the model sees what actually happened rather than trusting what it intended.
Action-effect visibility closes the perception loop; silent success is almost as harmful as silent failure.

## 4.7 Case study: Claude Code

Claude Code's tool surface, observable in the product as of early 2026, is a masterclass in the hybrid granularity strategy and is worth reading as a design document.

Its core surface is small - on the order of a dozen tools - and each one earns its slot by one of the promotion criteria from section 4.3.

`Read` returns line-numbered content with pagination, and images render as images; line numbers exist because `Edit` consumes them indirectly.
`Edit` performs exact string replacement and fails informatively unless the target string is unique in the file, forcing the model to include enough context to disambiguate; it also enforces read-before-edit, a staleness invariant only a dedicated tool can hold.
`Write` overwrites whole files and is described as the fallback when Edit is inappropriate, an explicit selection boundary between siblings.
`Glob` and `Grep` exist separately from `Bash` even though the shell could do both, because as read-only tools they are parallel-safe and their outputs can be shaped and capped.
`Bash` remains for everything else, with a description that actively redirects file reading and searching to the dedicated tools, and with a per-invocation natural-language `description` argument the model fills in, so the human approving a command sees intent alongside the string.
Slow operations run in the background with a monitor tool, keeping the loop responsive instead of blocking a turn on a long build.

Meta-lessons worth stealing.

The tool count stayed small while the product grew, and new capability preferentially landed as descriptions, sub-features of existing tools, or the bash long tail rather than as new top-level tools; the selection problem is treated as a budget.
Descriptions carry the routing logic between overlapping tools, so the model, not a router, resolves overlap, and the descriptions are written to make that resolution unambiguous.
The permission system keys off tool identity and typed arguments - reads auto-approved, writes and commands gated - which is only possible because the granularity decisions made the dangerous operations legible.
And the tools return concise, structured results tuned over many iterations, because at agent scale every wasted token in a tool result is multiplied by millions of turns.

## 4.8 A design checklist

Before shipping a tool, walk this list.

- Name states the action; siblings are lexically parallel.
- Description answers what, when, when-not, arguments, and result shape, with trigger conditions.
- Every argument has a description with format and example; optional arguments have stated defaults.
- Closed value sets are enums; identifiers have a discovery path.
- The tool completes in one shot; nothing interactive, nothing that assumes a follow-up keystroke.
- Success output is compact, self-contained, and includes structured signals; large outputs paginate with actionable continuations.
- Failure output diagnoses and, where cheap, prescribes; all argument violations report at once; nothing fails silently.
- Mechanically detectable model mistakes are caught in the executor and returned as observations.
- Side-effecting operations are legible to gates: dedicated tools, typed arguments.
- The tool has been tested by reading only its definition and asking whether you, cold, would use it correctly - and then by watching a model actually use it on a dozen transcripts.

That final item is the real test.
Tool design is empirical: transcripts of a model misusing a tool are the ground truth that outranks every principle in this chapter, and the fastest ACI improvement loop is read transcripts, fix the interface, rerun.

## Exercises

1. Take the bash-only agent from Chapter 3 and design, on paper, the minimal set of dedicated tools you would promote for a code-review agent, justifying each promotion by gating, invariants, rendering, or scheduling.
2. Rewrite these two real-world-style descriptions to checklist standard: `{"name": "db", "description": "Query the db"}` and `{"name": "update", "description": "Updates a record with new values"}`, inventing plausible sibling tools and drawing the selection boundaries.
3. Implement pagination and honest truncation for the `search` tool from Chapter 3, then run the TODO-report task and compare turn counts and token usage against the capped-at-50 version.
4. Design level-3 error observations for five failures of a `create_calendar_event` tool: unknown attendee, slot conflict, past date, permission denied, and malformed timezone.
5. Reproduce a miniature SWE-agent lesson: give a model a file-edit task with a raw overwrite tool, then with an Edit-style unique-match tool plus lint-on-edit, and compare failure modes across ten runs each.
6. Audit any agent framework's default tool surface against the checklist in section 4.8 and write up the three worst violations you find, with concrete fixes.
7. Take one week of transcripts from any agent you run, find the single most common tool misuse, and fix it purely through interface changes - no prompt edits - measuring the before and after rate.

## Godhood check

You have mastered this chapter when you can do the following without reference material.

- Argue, with the SWE-agent evidence, why interface quality rivals model quality, and predict which interface defects will cost the most capability.
- Write a tool definition that resolves what, when, when-not, arguments, and result shape in under 150 words, and critique someone else's in one pass.
- Decide polyvalent versus dedicated for a proposed capability using the four promotion criteria, and defend the decision against both the tidiness instinct and the bash-maximalist instinct.
- Design level-3 error observations for a new tool's failure modes without seeing them occur first.
- Choose pagination, truncation, projection, and format strategies for a tool from its expected data shape, and estimate the per-trajectory token consequences.
- Read ten transcripts of an agent using a tool surface and produce a ranked, concrete list of interface fixes.
