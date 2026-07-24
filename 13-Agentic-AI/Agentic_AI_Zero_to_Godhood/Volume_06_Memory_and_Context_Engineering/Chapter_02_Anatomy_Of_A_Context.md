# Chapter 02 - Anatomy of a Context

## What you will master

- The full composition of a real agent context: system prompt layers, tool definitions, conversation history, tool results, retrieved content, and scratchpad material.
- How each component is serialized and billed, including the hidden token costs most teams never measure.
- Practical token accounting: counting, budgeting per section, and instrumenting an assembler.
- Position effects: what the model attends to, primacy and recency, and how ordering decisions change behavior.
- How to design each layer deliberately instead of letting the context accrete.

## 2.1 The assembled context is the real program

When an agent framework makes a model call, it does not send "a conversation"; it serializes a single ordered token sequence.
Everything the model will know for this step is in that sequence; everything else does not exist for the model.
Debugging agents therefore starts with one habit: dump the exact assembled context for a failing step and read it end to end.
Most "the model is being dumb" reports dissolve on inspection into "we never sent it the thing we assumed it had" or "we sent it 40k tokens of noise around the thing."

A typical agent context, in the order the API assembles it, looks like this:

```
[system prompt]            static instructions, identity, policies
[tool definitions]         JSON schemas for every available tool
[history: user msg 1]
[history: assistant msg 1] possibly with tool calls
[history: tool result 1]   returned data, often large
[history: ...]             the loop, repeated
[current user msg or tool result]
```

Retrieved documents, memory files, and scratchpad content do not get their own privileged channel; they arrive inside one of these slots, usually as tool results or as injected sections of the system or user message.
This is worth internalizing: the model has exactly one input modality, a token sequence, and all architecture in this volume is about deciding what goes into that sequence.

## 2.2 The system prompt: layers, altitude, and ownership

Production system prompts are layered documents assembled from parts with different owners and change rates.
A representative decomposition, visible in published prompts such as Claude Code's and in most serious agent codebases as of early 2026:

1. Identity and role: who the agent is, what it is for, what tone it takes.
2. Capability instructions: how to use tools well, when to act versus ask, how to plan.
3. Policy and safety: refusal rules, data handling rules, organizational constraints.
4. Output contracts: formats, length norms, citation requirements.
5. Environment context: today's date, platform, working directory, runtime facts.
6. Injected project or user memory: CLAUDE.md-style instruction files, user preferences (Chapter 04).

Treat each layer as a separately owned artifact with its own review process, because they rot at different speeds: identity changes yearly, policies change quarterly, environment facts change every session, and injected memory changes daily.
Concatenating them naively creates two classic bugs: contradictions between layers (a policy layer forbids what a memory file requests) and stale environment facts cached from a previous session.
Resolve contradictions with an explicit precedence rule stated in the prompt itself, because the model otherwise resolves them by recency and mood.

Altitude is the key quality dimension.
Too low, and the prompt becomes a brittle rule lattice: hundreds of if-then clauses that the model pattern-matches inconsistently and that break on the first unanticipated input.
Too high, and the prompt is vacuous guidance ("be helpful and accurate") that leaves behavior underdetermined.
The right altitude states principles plus a few canonical examples, and trusts the model with the interpolation; Anthropic's 2025 context-engineering guidance makes this same point.
The downside of the principled style is less determinism on edge cases; if a behavior must be exact, encode it in code (a validator, a post-processor) rather than in prose.

Structure helps more than prose polish.
Use stable section headers, bullets for enumerable rules, and consistent terminology for the same concept throughout; models attend to structural landmarks, and so do the humans who maintain the prompt.
XML-style tags or Markdown headers both work; pick one convention per codebase and keep it.

## 2.3 Tool definitions: the most under-audited tokens in the window

Every tool is serialized into the context as a name, a description, and a JSON schema of parameters, on every single call.
Teams that meticulously trim their system prompt routinely ship 30 tools at 200-600 tokens each and never notice they are spending 10k+ tokens per step on definitions.
Count them: serialize your tool list the way your provider does and measure it, rather than guessing.

Design rules, each with its reason:

- Few tools beat many tools, because every definition taxes every call and because overlapping tools create selection ambiguity the model resolves worse than you would.
- The description is a micro-prompt; write it with the same care as the system prompt, including when to use the tool, when not to, and one example argument set if the schema is subtle.
- Parameter names carry signal; `absolute_path` outperforms `p` because the name itself instructs.
- Return shapes matter as much as input schemas: a tool that returns 50k tokens of raw JSON forces downstream compaction, so give tools `limit`, `offset`, and verbosity parameters and default them low.
- Ambiguity between tools is a bug: if `search_docs` and `query_kb` overlap, merge them or sharpen the boundary in both descriptions.

Large tool catalogs create a real tension: MCP-style ecosystems (Volume 09) can expose hundreds of tools, and loading all definitions up front is exactly the pre-loading anti-pattern of Chapter 01.
The 2025-era responses are dynamic tool loading (search for tool definitions on demand and load only matches) and code-execution bridges where the model writes code that calls tool APIs, keeping definitions out of the window.
Both trade simplicity for budget: dynamic loading adds a retrieval step that can miss, and code bridges move errors from schema validation time to runtime.

## 2.4 Conversation history and tool results: the growing middle

History is the only part of the context that grows without explicit action, so it is where budgets die.
Its composition skews heavily toward tool results in real agents; in coding-agent transcripts it is common for tool results (file reads, command output, search results) to outweigh all dialogue by an order of magnitude.
Three properties of tool results make them the primary compaction target of Chapter 03:

- They are bulky: a single file read can be 10k tokens.
- They decay fast: a directory listing from 40 turns ago is probably stale and definitely low-value.
- They are cheaply recoverable: the file is still on disk; the pointer (its path) is a sufficient residue.

Dialogue, by contrast, is compact, slow-decaying, and unrecoverable, so it deserves gentler treatment.
User messages encode requirements and corrections; assistant messages encode decisions and commitments; deleting either silently changes the contract of the session.

One subtlety senior engineers should know: assistant turns may include chain-of-thought or "thinking" blocks depending on provider and configuration, and providers differ in whether prior thinking is retained, stripped, or summarized in subsequent calls (Anthropic, for example, documents that thinking blocks from prior turns are generally not carried as billed context in its 2025-era extended-thinking API, with signature mechanisms for tool-use continuity).
Do not design a memory strategy that assumes the model can re-read its own past reasoning unless you have verified your provider actually resends it.

## 2.5 Retrieved content and scratchpads: injected, not native

Retrieved documents (RAG results, memory-file contents, search hits) enter the window through one of three doors, each with trade-offs:

1. System-prompt injection: good for content that should govern the whole session (project instructions), bad for anything dynamic because it breaks prompt caching (Chapter 07) and inflates the permanent tax.
2. User-message injection: the orchestrator wraps retrieved content in the current user turn, clearly delimited; simple and cache-friendly, but the model may treat it with user-level authority, so label provenance explicitly.
3. Tool-result injection: the agent called a retrieval tool and the content arrives as that tool's result; this is the most honest representation, keeps provenance natural, and makes the content eligible for tool-result clearing later.

Whatever the door, delimit and attribute: wrap each retrieved item with its source, timestamp, and retrieval query.
Unattributed text in the window is a prompt-injection surface (Volume 11) and an untraceable hallucination source; attributed text lets the model cite, and lets you debug.

Scratchpad content, todo lists, and working notes (Chapter 04) usually enter as tool results from read operations or as a maintained section the assembler re-injects each turn.
The re-injected-section pattern is powerful and dangerous: powerful because it pins critical state near the recency position every turn, dangerous because it silently duplicates content across cache boundaries if placed carelessly.

## 2.6 Token accounting in practice

You cannot budget what you do not measure, and token counts are cheap to measure.

Counting: use the provider's tokenizer or counting endpoint rather than heuristics when accuracy matters; Anthropic exposes a token-counting API and OpenAI ships `tiktoken` (both current as of early 2026).
The rule of thumb of roughly 3.5-4 characters per token for English prose is fine for dashboards and wrong enough for enforcement, especially for code, JSON, and non-English text, which tokenize denser or sparser.

Structural overhead is real and usually unmeasured: message framing, role markers, tool-call serialization, and JSON schema boilerplate all bill as input tokens.
Measure it once by counting an empty-ish request; teams are routinely surprised that their "small" request starts at several thousand tokens before any task content.

A minimal budgeting assembler, provider-neutral:

```python
from dataclasses import dataclass

@dataclass
class Section:
    name: str
    content: str
    priority: int          # lower number evicts last
    ceiling: int           # max tokens this section may occupy

def assemble(sections: list[Section], window_budget: int, count) -> list[Section]:
    # Enforce per-section ceilings first, then evict by priority to fit.
    for s in sections:
        while count(s.content) > s.ceiling:
            s.content = shrink(s)          # truncate, summarize, or drop items
    total = sum(count(s.content) for s in sections)
    for s in sorted(sections, key=lambda s: -s.priority):
        if total <= window_budget:
            break
        freed = count(s.content)
        s.content = residue(s)             # e.g. "[cleared: see notes/incident.md]"
        total -= freed - count(s.content)
    return sections
```

The two functions left abstract are the actual design decisions: `shrink` encodes how each section degrades gracefully (drop oldest items, summarize, truncate middle), and `residue` encodes what survives eviction (a pointer, never nothing).
Set the window budget well below the model's hard limit, both to leave room for the output and because usable context is smaller than advertised context (Chapter 01).
A common production stance as of 2025-2026 is to trigger compaction at roughly 70-90 percent of the effective window rather than riding the limit.

Instrument per-section token counts as first-class metrics on every call.
The single most useful agent dashboard panel is a stacked area chart of context composition over turns: you will see tool results eating the window in real time, and you will see exactly when compaction fires and what it reclaimed.

## 2.7 Position effects: what the model actually attends to

Position in the window changes influence; this is measured, not folklore.

- Primacy: content at the start of the context (the system prompt region) exerts strong, persistent influence; this is partly training (models are trained to follow system-position instructions) and partly attention structure.
- Recency: content at the end of the context is highly salient; the most recent tool result and user message dominate immediate behavior.
- The middle sags: "Lost in the Middle" (Liu et al., 2023) demonstrated the U-shaped retrieval curve, and later long-context studies through 2025 confirmed that mid-context material is the most likely to be ignored, with the effect growing with total length.

Design consequences, each exploitable today:

1. Put durable rules first and current focus last; the sagging middle is where bulk storage goes (old history), not where instructions go.
2. Re-state critical constraints near the end when the window is long; a one-line reminder ("remember: staging only, never prod") adjacent to the current step measurably outperforms the same rule stranded 80k tokens back; this is exactly why todo-list re-injection works as an attention anchor (Chapter 04).
3. Order retrieved documents by relevance with the best either first or last, never buried mid-pack among ten mediocre hits.
4. When the agent must compare two artifacts, place them adjacently; separation across a long span degrades comparison quality.
5. After compaction, the summary sits near the start of the new window and inherits primacy; this is a feature (decisions stay authoritative) and a hazard (summary errors become authoritative too), which is why Chapter 03 treats summary correctness as a safety property.

A caution on over-fitting to position tricks: models differ, and each generation attends longer and flatter than the last; as of early 2026 frontier models handle mid-context retrieval far better than 2023 models, but the ordering "edges beat middle" has survived every generation so far, so it remains the safe default.
The downside of relying on recency re-statement is duplication: repeated reminders consume budget and can drift out of sync with the original rule, so generate them from the same source of truth rather than hand-copying.

## 2.8 Reading a real context: a dissection exercise

Take an actual mid-task snapshot from a coding agent, which as of 2025-2026 typically looks like this by proportion (illustrative composition, not a benchmark):

- 3-8 percent: system prompt and injected instruction files.
- 5-15 percent: tool definitions.
- 5-10 percent: user and assistant dialogue.
- 60-85 percent: tool results, dominated by file reads and command output.
- 0-5 percent: explicit scratchpad or todo state.

The dissection questions to ask of any snapshot:

1. What fraction of tool-result tokens could be replaced by a pointer without changing the next action?
2. Which instructions are contradicted or superseded by later content, and does the model have any explicit signal for which wins?
3. What is the distance in tokens between the current decision point and the constraint most likely to be violated?
4. What is in the cache-stable prefix versus the churning suffix, and is anything churning that should be stable (Chapter 07)?
5. If this context were compacted right now, what would be irrecoverable, and is each such item durable somewhere outside the window?

Doing this dissection on five real transcripts teaches more context engineering than any amount of prose, which is why it is the core exercise below.

## Exercises

1. Instrument your agent (or any open-source agent such as a Claude Code, Aider, or OpenHands session log) to dump the fully assembled context at each step, and build the stacked-area composition chart of token counts by section over a 30+ turn session.
2. Measure your structural overhead: count tokens for a request with an empty system prompt and no tools, then add your real tool definitions one at a time, and produce a per-tool token price list; identify the two most expensive tools and rewrite their schemas to cost less without losing clarity.
3. Design and run a position-effect experiment: place the same critical constraint at the start, middle, and end of a 50k-token context and measure violation rates across 20 trials per position on a task that tempts violation.
4. Take a system prompt of over 2k tokens and produce a layered rewrite: separate identity, capability, policy, output contract, and environment layers; add an explicit precedence rule; measure the token delta and eval both versions.
5. Write the `shrink` and `residue` functions from Section 2.6 for three concrete section types (file-read results, search results, dialogue) and unit-test that assembly never exceeds budget and never evicts a section to empty string without a pointer residue.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Enumerate every token source in an agent context, state who owns each, how fast it changes, and how it is billed.
- Estimate within 20 percent the token cost of a tool catalog by reading its schemas, and articulate three schema changes that reduce cost without reducing capability.
- Explain the three injection doors for retrieved content and choose correctly among them for a given artifact, citing caching and provenance consequences.
- Sketch a budgeting assembler with per-section ceilings, priorities, graceful degradation, and pointer residues, and defend the eviction order.
- Describe the U-shaped attention curve, cite the line of evidence behind it, and list four ordering decisions in a real agent that should change because of it.
