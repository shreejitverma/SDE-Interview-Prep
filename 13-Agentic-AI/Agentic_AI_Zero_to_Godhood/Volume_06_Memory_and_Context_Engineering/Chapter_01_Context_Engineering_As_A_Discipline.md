# Chapter 01 - Context Engineering as a Discipline

## What you will master

- Why the industry shifted from "prompt engineering" to "context engineering" during 2024-2025, and what the new term actually adds.
- The model of context as a finite resource with diminishing and eventually negative marginal returns.
- Context rot: what degrades as context grows, what the evidence says, and why advertised window sizes overstate usable capacity.
- The attention budget framing and the "smallest set of high-signal tokens" principle.
- The core strategies that follow from these constraints: curation, just-in-time retrieval, compaction, structured notes, and sub-agent isolation.
- How to reason about context decisions the way you reason about memory hierarchies in systems engineering.

## 1.1 From prompt engineering to context engineering

Prompt engineering, as practiced from roughly 2020 to 2023, was the craft of writing a single block of instructions that coaxed good behavior out of a model in one shot.
Its unit of analysis was the prompt string: phrasing, few-shot examples, role assignments, output format constraints.
This framing fit the dominant usage pattern of the time, which was single-turn or short multi-turn chat.

Agents broke that framing.
An agent runs in a loop: it receives instructions, calls tools, observes results, and iterates, often for dozens or hundreds of turns.
At each turn, the model does not see "a prompt"; it sees a full context assembled from many sources: system prompt, tool definitions, conversation history, tool results, retrieved documents, and working notes.
The engineering question stops being "what should I write" and becomes "what should be in the window at this moment, and what should not."

Context engineering is the discipline of curating and maintaining the optimal set of tokens in the model's context at each step of an agentic loop.
The term entered mainstream vocabulary in mid-2025; Anthropic published "Effective context engineering for AI agents" in September 2025, and similar guidance appeared from other labs and from practitioners such as the LangChain and Manus teams around the same period.
Date-stamp: this chapter describes the consensus as of early 2026.

The distinction is not cosmetic, and it is worth being precise about what changed:

- Prompt engineering optimizes a static artifact; context engineering optimizes a dynamic process that runs across turns.
- Prompt engineering is mostly about instructions; context engineering is mostly about information selection, budgeting, and lifecycle.
- Prompt engineering fails visibly when the wording is bad; context engineering fails subtly when the window fills with low-value tokens and quality decays mid-task.
- Prompt engineering is something you do once at design time; context engineering includes runtime machinery: retrieval, compaction, note-taking, cache-aware assembly.

A useful mental model: prompt engineering is to context engineering what writing a good function is to designing a memory hierarchy.
The first is still necessary; it is simply no longer the binding constraint.

### What did not change

System prompt quality still matters enormously, and Chapter 02 dissects it.
Few-shot examples still work, and diverse canonical examples usually beat exhaustive rule lists.
The change is that these artifacts now live inside a budget, and every token they consume competes with task-relevant information.

## 1.2 Context as a finite resource

Advertised context windows grew fast: 4k tokens was typical in early 2023, 128k became common in 2024, and by 2025 several frontier models advertised 1M-token windows (Gemini 1.5 Pro and successors, GPT-4.1, and Claude Sonnet 4 in beta, as of late 2025).
It is tempting to conclude that context is no longer scarce.
That conclusion is wrong for three separate reasons: attention, cost, and latency.

### Attention is the real budget

Transformers implement attention as pairwise token interactions, so the number of relationships the architecture must represent grows quadratically with context length even though modern implementations compute it efficiently.
Models are trained on a distribution of sequence lengths in which short sequences vastly outnumber long ones, so they have less training signal for long-range dependency patterns.
Position-encoding schemes are commonly extended by interpolation or extrapolation beyond the lengths dominant in training, which preserves basic function but degrades precision.
The practical consequence is that a model's ability to attend accurately to any given token declines as the total number of tokens grows.

Anthropic's 2025 guidance names this budget explicitly: LLMs have a limited "attention budget," and every token in context draws down that budget.
Treat context like RAM under memory pressure, not like disk: it is fast, it is scarce, and filling it with cold data evicts nothing automatically but degrades everything.

### Diminishing and negative marginal returns

The value curve of added context is not flat and is not even monotonic.
The first tokens of task-relevant context deliver enormous value: without the task description, tool definitions, and key constraints, the agent cannot function at all.
Additional relevant tokens continue to help, but each increment helps less, because the model must now discriminate signal against a larger background.
Beyond some point, which varies by model and task, adding tokens reduces accuracy: distractors pull attention, near-duplicate content creates interference, and stale information contradicts fresh information.
This is the sense in which marginal returns become negative, and it is why "just stuff everything in, the window is big" is an engineering error rather than a lazy but safe default.

### Cost and latency scale with tokens even when quality does not

Every input token is billed on every model call, and an agent loop re-sends the growing history each turn, so raw token count compounds quadratically over a long run unless caching and compaction intervene (Chapter 07 covers the economics).
Prefill latency also grows with input length, so bloated contexts make agents slower even when they do not make them dumber.
A team that ignores context discipline typically discovers it first as a cost problem, then as a latency problem, and only later realizes it was also a quality problem all along.

## 1.3 Context rot

"Context rot" names the empirical finding that model performance degrades as input length grows, even on tasks the model performs perfectly at short lengths.
Chroma published a technical report under that name in July 2025, evaluating a broad set of frontier models (their report covered 18 models including GPT-4.1, Claude 4, and Gemini 2.5 variants) and found non-uniform degradation with input length on tasks deliberately designed to be simple.
Related evidence accumulated across several years:

- "Lost in the Middle" (Liu et al., 2023) showed U-shaped position bias: models retrieve information at the beginning and end of long contexts far better than information in the middle.
- Needle-in-a-haystack tests, popularized in 2023-2024, showed near-perfect literal retrieval for many models, which created a misleading impression of long-context competence.
- The NoLiMa benchmark (2025) removed literal word overlap between question and needle, forcing semantic rather than lexical matching, and reported that many models that ace literal needle tests degrade sharply by 32k tokens.
- Multi-needle, distractor-laden, and reasoning-over-context variants consistently show worse degradation than single literal retrieval.

The mechanism summary that practitioners should internalize:

- Retrieval of an exact string from long context is the easiest case and overstates capability.
- Degradation grows with semantic distance between query and target, with the number of plausible distractors, and with the amount of reasoning required over retrieved content.
- Structural features of the haystack matter; ordering and coherence of surrounding text measurably change retrieval success, which tells you the model is not doing clean random access.
- Long, irrelevant context is not neutral filler; it is an active tax on everything else in the window.

Do not memorize specific percentages from these studies; the numbers rot as models improve, and by early 2026 newer models degrade later and more gently than 2024 models did.
The stable lesson is the shape of the curve, not its coefficients: usable context is smaller than advertised context, and the gap widens with task difficulty.

### Practical symptoms in agents

Context rot in a real agent rarely looks like a benchmark chart; it looks like behavioral drift late in a session:

- The agent re-asks questions that were answered fifty turns ago.
- It violates a constraint stated in the system prompt, because the constraint is now buried under 100k tokens of tool output.
- It fixates on an early failed approach that still sits verbatim in history.
- It confuses two similar entities, such as two files with near-identical names whose contents both appear in the window.
- Summaries it produces of its own work drop the most important decision and keep trivia.

When you see these symptoms, the fix is almost never "use a bigger model" and almost always "put fewer, better tokens in the window."

## 1.4 The smallest set of high-signal tokens

Anthropic's 2025 guidance compresses the discipline into one sentence: find the smallest possible set of high-signal tokens that maximize the likelihood of the desired outcome.
Every clause of that sentence is load-bearing.

- "Smallest possible set": minimality is a goal, not a side effect, because of the attention budget and cost scaling above.
- "High-signal": tokens are ranked by expected influence on the outcome; a token that the model will not use is negative value, not zero value, because it still draws attention and money.
- "Likelihood of the desired outcome": the objective is task success, not context elegance; if an extra 2k tokens of examples reliably lifts success rate, they earn their place.

This principle generates concrete design rules:

1. Calibrate system prompts to the right altitude: specific enough to guide behavior, general enough to avoid a brittle if-else lattice of hardcoded edge cases; both failure modes waste tokens and generalize poorly.
2. Prefer canonical few-shot examples over exhaustive rules; a small diverse set of examples is usually a denser encoding of intent than paragraphs of prose.
3. Keep tool sets minimal and non-overlapping; if a human engineer cannot say which of two tools applies, the model cannot either, and both definitions cost budget on every call (Chapter 02).
4. Retrieve just in time instead of pre-loading everything; maintain lightweight identifiers (paths, queries, links) and load content when needed (Chapter 04).
5. Evict aggressively: clear stale tool results, compact history, and move durable facts out of the window into external memory (Chapters 03 and 04).
6. Isolate subtasks in sub-agents so that deep exploration burns a disposable context, returning only a distilled result to the orchestrator (Volume 07 treats multi-agent designs in depth).

### The counter-pressure: do not starve the model

Minimality has a failure mode of its own, and honest treatment requires naming it.
An over-pruned context produces an agent that hallucinates missing details, re-derives known facts at high token cost, or asks the user questions it should already know the answer to.
Retrieval that happens just in time is slower than information already in the window, and each retrieval step is itself a chance to fetch the wrong thing.
The discipline is not "fewer tokens always"; it is deliberate allocation under a budget, with the same trade-off character as cache sizing: too small thrashes, too large pollutes.
When in doubt, err toward including information whose absence would force a guess, and toward excluding information the model could re-fetch cheaply and reliably.

## 1.5 An engineering vocabulary for context decisions

Senior engineers already own the right mental tools; they just need mapping.

- Context window = RAM: fast, scarce, contended; everything else must be paged in and out deliberately.
- External memory (files, databases, vector stores) = disk: cheap and durable, but access costs a round trip and a relevance judgment.
- Retrieval = page-in; compaction and clearing = eviction; the compaction summary = a lossy write-back.
- Prompt cache = a physical optimization layer that rewards stable prefixes, exactly as CPU caches reward locality (Chapter 07).
- The system prompt = the resident kernel: always mapped, so its size is a permanent tax and its quality a permanent lever.
- Sub-agents = processes with private address spaces communicating through a narrow IPC channel of summaries.

The mapping also imports the classic failure modes.
Thrashing appears as an agent that repeatedly re-reads the same file because nothing durable was kept.
Fragmentation appears as a window full of half-relevant fragments none of which is complete enough to act on.
Leaks appear as append-only histories where every tool result lives forever.
Cache pollution appears as speculative pre-loading of documents "in case they help."

## 1.6 Where each strategy applies

The strategies introduced above are the subject of the rest of this volume; here is the decision map.

| Situation | Primary strategy | Chapter |
|---|---|---|
| Designing what goes in the window at all | Anatomy, budgeting, position effects | 02 |
| Window approaching its limit mid-task | Compaction, summarization, tool-result clearing | 03 |
| Task state that must outlive the window | Scratchpads, memory files, file-system-as-memory | 04 |
| Knowledge that must outlive the session | Long-term memory systems | 05 |
| Runs that must survive crashes and restarts | State, checkpointing, event sourcing | 06 |
| Paying for the same prefix thousands of times | Prompt caching and context economics | 07 |

One cross-cutting note on evaluation: every technique in this volume changes agent behavior, so every adoption decision should be gated on task-level evals, not vibes.
Volume 10 covers evaluation infrastructure; the minimal bar here is a fixed set of representative tasks scored before and after each context-strategy change.

## 1.7 A worked contrast: prompt thinking vs context thinking

Consider an agent that triages production incidents from logs, dashboards, and runbooks.

The prompt-engineering instinct says: write a great system prompt, paste in the runbook, paste in recent logs, and ask for a diagnosis.
This works in a demo with one small runbook and a hundred log lines.
In production, runbooks total 200k tokens, logs arrive at thousands of lines per minute, and incidents take dozens of investigative steps; the paste-everything design exceeds any window and rots long before it hits the hard limit.

The context-engineering design of the same agent:

- System prompt: role, severity taxonomy, escalation rules, and three canonical worked triages as few-shot examples; no runbook content.
- Tools: `search_runbooks(query)`, `query_logs(service, window, filter)`, `get_dashboard(service)`, each returning bounded, paginated results.
- Working notes: the agent maintains a structured incident file (symptoms observed, hypotheses ruled out, current best hypothesis, evidence links) outside the window and re-reads it at each phase boundary.
- Compaction: raw log excerpts are cleared from history once their conclusions are recorded in the notes; only conclusions and pointers survive.
- Budget: the assembler enforces per-section token ceilings, so a noisy log query cannot crowd out the runbook excerpt that actually matters.

The second design is more code and more moving parts; that is the honest cost.
What it buys is an agent whose quality at turn 80 resembles its quality at turn 8, whose cost grows roughly linearly rather than quadratically, and whose behavior can be debugged by inspecting an assembled context rather than guessing.

## 1.8 The trajectory: from curation toward learned management

As of early 2026, most context engineering is explicit engineering: humans design the budgets, the compaction triggers, and the retrieval policies.
The direction of travel is toward models doing more of this themselves: models trained to take notes, to decide what to retrieve, to manage their own memory directories (the memory-tool pattern of Chapter 04), and to operate for long horizons across compaction boundaries.
Anthropic's late-2025 releases of a memory tool and server-side context editing, and comparable moves elsewhere, are early instances of the platform absorbing patterns that practitioners first built by hand.
The durable skill is therefore not any specific trick but the underlying resource model: whoever understands the budget can evaluate each new mechanism as it arrives.

## Exercises

1. Take an existing single-prompt application you have built and re-express it as a context-engineering design: list every token source, assign each a budget, and mark each as static, per-session, or per-turn.
2. Build a minimal context-rot probe: place ten factual statements at controlled depths inside filler text of 1k, 10k, 50k, and 100k tokens, ask a model questions requiring each fact, and plot accuracy against depth and total length; repeat with paraphrased (non-literal) questions and compare.
3. Audit a real agent transcript of at least 50 turns: classify every token span as instruction, tool definition, tool result, retrieved content, or dialogue, compute the fraction of the final context that plausibly influenced the final answer, and identify the three largest wastes.
4. Take one over-long system prompt (yours or a public one) and rewrite it at the right altitude: replace exhaustive rules with canonical examples, cut anything the model would not act on, and measure the token reduction; then eval both versions on a fixed task set and report whether quality moved.
5. Write a one-page memory-hierarchy design for an agent in your domain: what lives permanently in the window, what is retrieved just in time, what is compacted, and what is written to external memory; justify each placement by access frequency and staleness.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Explain to a skeptical colleague why a 1M-token window does not make context engineering obsolete, using attention budget, cost scaling, and context-rot evidence as three independent arguments.
- State the "smallest set of high-signal tokens" principle and derive at least five concrete design rules from it.
- Name the failure mode of over-minimization and describe how you would detect it in an eval.
- Given symptoms of a misbehaving long-running agent, diagnose whether the cause is context rot, missing information, or bad instructions, and say what evidence would distinguish them.
- Map context window, retrieval, compaction, and prompt cache onto a systems memory hierarchy and use the mapping to predict a failure mode of a proposed agent design.
