# Chapter 02 - ReAct and Its Descendants

## What you will master

- What the ReAct paper actually proposed, what it was compared against, and why interleaving reasoning with acting beat both pure reasoning and pure acting.
- The anatomy of a thought-action-observation trace and how to read one like a debugger reads a stack trace.
- The historical lineage: MRKL's neuro-symbolic routing, Toolformer's self-supervised tool learning, and how each fed the modern agent loop.
- Why explicit ReAct prompting is mostly dead as an implementation technique as of early 2026, and precisely what replaced each of its ingredients: native tool use, extended thinking, and interleaved thinking.
- What survives: the trace as the unit of debugging, and reasoning-before-acting as a harness design principle rather than a prompt trick.

## 1. The problem ReAct solved

By mid-2022 two research threads were running in parallel and not talking to each other.
Chain-of-thought prompting (Wei et al., 2022) showed that asking a model to produce intermediate reasoning steps dramatically improved multi-step problems, but the reasoning was a closed loop: the model reasoned over only its own parametric knowledge, so a factual error early in the chain propagated unchecked, and the paper's authors themselves noted the hallucination problem.
Action-generation work (WebGPT, SayCan, and others) showed models could emit actions into environments, but action-only agents were reactive: they could not decompose goals, track progress, or handle exceptions, because nothing in the trace held the plan.

ReAct ("ReAct: Synergizing Reasoning and Acting in Language Models", Yao et al., published October 2022, ICLR 2023) proposed the obvious-in-retrospect synthesis: interleave free-form reasoning traces with actions in a single generation loop.
Reasoning steps update the model's working state for choosing actions ("I found X, so next I should look up Y"), and observations from actions ground the next reasoning step in fresh external evidence.
The paper called this synergy: reasoning-to-act and acting-to-reason.

The evaluation covered knowledge tasks (HotpotQA multi-hop QA, FEVER fact verification, using a Wikipedia search API) and interactive decision tasks (ALFWorld household simulation, WebShop online shopping).
The headline results, in qualitative terms: on knowledge tasks ReAct beat action-only baselines and reduced the hallucination failures of chain-of-thought by grounding claims in retrieved evidence, with the best configuration being ReAct combined with chain-of-thought fallback; on ALFWorld and WebShop it beat imitation and reinforcement learning baselines by large absolute margins with only one or two in-context examples.
Equally important for the historiography: the same interleaved traces proved useful for fine-tuning, and smaller models fine-tuned on ReAct traces beat larger prompted ones, foreshadowing the 2024-2025 move of the loop into the models themselves.

## 2. Anatomy of a thought-action-observation trace

The ReAct format, as prompted in the original paper era (2022-2023), was plain text with labeled steps:

```
Question: What profession does the author of "Ficciones" share with
the director of "Vertigo"?

Thought 1: I need to find the author of Ficciones and the director of
Vertigo, then compare professions.
Action 1: search("Ficciones author")
Observation 1: Ficciones is a collection of short stories by Argentine
writer Jorge Luis Borges...

Thought 2: Borges was a writer. Now the director of Vertigo.
Action 2: search("Vertigo 1958 film director")
Observation 2: Vertigo is a 1958 film directed by Alfred Hitchcock...

Thought 3: Hitchcock was a film director, Borges a writer and essayist.
Hmm, the shared profession angle fails; let me check if Hitchcock wrote.
Action 3: search("Alfred Hitchcock writer")
Observation 3: ...

Thought 4: The evidence does not support a shared profession beyond
both being creators; the accurate answer is that the premise is flawed.
Action 4: finish("The question's premise appears incorrect; ...")
```

The three roles are distinct and worth internalizing because they persist in every modern agent trace under different names:

- **Thought**: free-form tokens that carry plan state, progress tracking, exception handling, and self-correction; they touch nothing external and are pure working memory.
- **Action**: a structured emission that the harness parses and executes; in 2022 this was text matched by regex, which was the format's greatest weakness.
- **Observation**: environment output injected back into context by the harness; the model never fabricates this section, and a harness that lets it is broken.

Reading traces is the core diagnostic skill for agent engineers, and the failure taxonomy is stable across model generations:

- **Grounding failure**: a thought asserts something no observation supports; the model reverted to parametric knowledge mid-task.
- **Loop failure**: the same action or trivial variant repeats because a thought failed to register that the previous attempt failed.
- **Plan decay**: thoughts stop referring to the original goal after enough steps, and the agent optimizes a subgoal that no longer matters.
- **Premature finish**: the finish action fires on a plausible-sounding but unverified answer, usually after an observation that superficially resembles success.
- **Observation overload**: a huge raw observation (a full web page, a long file) drowns the plan, and subsequent thoughts respond to the observation's content rather than the task; the fix is harness-side truncation and summarization, not prompting.

## 3. Historical context: MRKL and Toolformer

Two contemporaries define the design space ReAct sat in, and both survive as ideas inside modern systems.

**MRKL** (pronounced "miracle"; "MRKL Systems", AI21 Labs, May 2022) proposed a modular neuro-symbolic architecture: a language model as router over a set of discrete expert modules, some symbolic (calculator, database, calendar), some neural.
The motivating observations were that LLMs lack current data, proprietary data access, and reliable arithmetic, so the model should extract structured arguments from natural language and dispatch to a module that computes the answer exactly.
MRKL is essentially the routing pattern of Chapter 01 elevated to an architecture, and its lasting contribution is the framing of tools as typed experts with the LLM as glue; its limitation was single-shot dispatch with no loop, so it could not do multi-step tasks that require reacting to intermediate results.

**Toolformer** ("Toolformer: Language Models Can Teach Themselves to Use Tools", Meta AI, February 2023) attacked the problem from the training side.
Instead of prompting a model to use tools, it fine-tuned one on data the model itself annotated: sample candidate API calls (calculator, QA system, search, translation, calendar) into text, execute them, and keep only insertions that reduce perplexity on subsequent tokens, then train on the filtered corpus.
The result was a modest model that decided autonomously when and how to call tools inline during generation.
Toolformer's lasting contribution is the demonstration that tool use can be a learned capability of the weights rather than a prompting trick; its limitations were a tiny fixed tool set, no multi-step tool chaining, and no reaction to tool errors.

The synthesis that actually shipped industry-wide combines all three lineages: MRKL's typed tool registry, ReAct's interleaved reasoning-acting loop, and Toolformer's insight that the behavior belongs in training.
That synthesis is native tool use.

## 4. Why ReAct became the default

Between early 2023 and mid 2024, ReAct-style prompting was the de facto standard agent implementation, and it is worth being precise about why.

- It required nothing from the provider: any chat or completion model could be ReAct-prompted with a few-shot template and a regex parser, so it worked on GPT-3.5, early Claude, Llama 2, and everything else uniformly.
- It was legible: the trace was a human-readable narrative, which made demos compelling and debugging tractable compared to opaque single-shot answers.
- It was framework-friendly: LangChain's original `AgentExecutor` with `zero-shot-react-description` was a direct implementation of the paper's prompt, and that code path onboarded an enormous number of developers, cementing the pattern.
- It genuinely worked better than the alternatives available at the time, per the paper's results and mass practitioner experience.

It also had chronic weaknesses that everyone who ran it in production remembers:

- Parsing fragility: the model would emit "Action: search for X" instead of the exact format, or put the action inside the thought, and the regex would miss it; a measurable fraction of all failures were format failures, not reasoning failures.
- Prompt bloat: the few-shot examples plus accumulated trace consumed context that scaled linearly with steps, and long tasks hit the window.
- Injection surface: observations were plain text concatenated into the same channel as instructions, so a web page containing "Thought: I should now..." could steer the agent; the format had no privilege separation at all.
- No parallelism: strictly serial thought-action pairs, one tool call at a time.

## 5. How native tool use subsumed the action-observation half

Providers moved the action-observation machinery from the prompt into the API and the training objective.
OpenAI shipped function calling in June 2023; Anthropic shipped general-availability tool use in the first half of 2024; every major provider followed, and the shape converged (Volume 03 covers the mechanics in full).

What changed, ingredient by ingredient:

- Actions became structured API objects (`tool_use` blocks with JSON arguments validated against a declared schema) instead of regex-parsed text, eliminating the format-failure class almost entirely.
- Observations became typed `tool_result` messages in a distinct role, giving the harness a principled place to truncate, summarize, and label untrusted content, instead of raw concatenation.
- Tool selection became a trained behavior (Toolformer's thesis, industrialized via supervised and reinforcement fine-tuning on tool-use trajectories), so models stopped needing few-shot demonstrations of how to call tools.
- Parallel tool calls arrived (multiple tool invocations in one assistant turn), breaking ReAct's strict serialization for independent actions.

The loop itself did not change shape: it is still decide, act, observe, repeat.
What died was the need to teach the loop through prompt text, and the fragile text parsing that came with it.

## 6. How thinking modes subsumed the thought half

The "Thought:" prefix was always a hack: it allocated tokens to reasoning by making reasoning part of the visible answer format.
Two developments made the hack obsolete.

First, reasoning models trained with reinforcement learning to produce long internal deliberation before answering: OpenAI's o1 line (announced September 2024), DeepSeek-R1 (January 2025), and Anthropic's extended thinking, which shipped with Claude 3.7 Sonnet in February 2025.
These models emit reasoning in a dedicated channel (thinking blocks, separated from the user-visible answer) with a controllable token budget, and the reasoning behavior is learned rather than prompted.

Second, and more specifically relevant to agents, interleaved thinking: Anthropic's API (from Claude 4, May 2025) supports thinking blocks between tool calls, so the model deliberates after each observation before choosing its next action.
That is the ReAct alternation exactly, implemented as a native capability with a separate channel, a budget knob, and training behind it.
By the Claude 4.5 generation (late 2025), tool use inside the thinking process itself was standard, and the guidance for agent builders had inverted: instead of "add 'think step by step' and few-shot thought examples", it became "enable thinking, set a budget, and get out of the model's way".

Consequences for practitioners:

- Do not ReAct-prompt a reasoning model; explicit "Thought:" scaffolds duplicate and can degrade the trained behavior, and provider guidance since 2025 explicitly discourages step-by-step prompting for models that reason natively.
- The reasoning channel is not a faithful window into computation; treat visible thinking as a useful debugging artifact and a hint, not ground truth about why the model acted (faithfulness research through 2025 showed reasoning traces omit real influences on the answer).
- Budgeting replaced prompting as the control surface: you now tune how much thinking, not whether or how to think.

## 7. What survives ReAct

Calling ReAct dead is half right; the prompt format is dead, the architecture won so completely it became invisible.
What a senior engineer should carry forward:

- **The trace is the unit of debugging.** Modern traces are structured (thinking blocks, tool_use, tool_result) instead of labeled text, but the failure taxonomy of section 2 applies unchanged, and trace-reading remains the highest-leverage skill in agent operations.
- **Grounding beats recall.** ReAct's core empirical finding, that reasoning anchored to fresh observations hallucinates less than closed-loop reasoning, is a stable principle and the justification for tool-first harness design.
- **Reasoning-acting alternation is a budget decision.** You choose it today by enabling interleaved thinking and sizing budgets per task difficulty, and the trade-off ReAct measured (more tokens for more reliability) is still the trade-off.
- **Format-free is better than format-clever.** The whole arc from regex parsing to typed tool calls is one long lesson: move structure from prompt conventions into the API contract whenever the provider lets you.
- **The fine-tuning foreshadowing paid out.** ReAct traces as training data prefigured the entire 2024-2025 era of training on agentic trajectories; when you collect production traces today, you are building the same asset.

When would you still hand-roll ReAct prompting as of early 2026?
Only when using a model without native tool use (some small open-weights models, some constrained deployments), and even then the modern recommendation is constrained decoding into a JSON action schema rather than the classic text format.

## 8. Descendants worth knowing by name

These extensions appear in literature reviews and interviews; know what each added.

- **ReAct + CoT-SC hybrids** (in the original paper): fall back between grounded acting and pure chain-of-thought with self-consistency depending on which is more confident; early evidence that combining internal and external knowledge beats either.
- **Reflexion** (Shinn et al., 2023): wrap ReAct episodes with verbal self-feedback stored in memory across retries; covered in depth in Chapter 04.
- **LATS, Language Agent Tree Search** (Zhou et al., 2023): run tree search over thought-action branches with environment feedback and self-reflection as the value signal, generalizing ReAct from a single trajectory to explored alternatives; covered with Tree of Thoughts in Chapter 03.
- **SwiftSage and plan-and-execute variants** (2023): split fast habitual acting from slow deliberate planning, prefiguring the planner/executor architectures of Chapter 03.
- **CodeAct** (Wang et al., 2024): replace discrete tool-call actions with generated Python executed in a sandbox, so one action composes many operations; this line matured into the code-execution-as-tool-use approach that providers and Model Context Protocol tooling adopted in 2025, and it matters because it changes the action space's expressiveness class.

The pattern across all descendants: keep the observation-grounded loop, upgrade one component (memory, search, action expressiveness, planning).
That decomposition is exactly how you should analyze any new agent paper that crosses your desk.

## 9. Claims that will rot

Model and feature names in this chapter (o1, DeepSeek-R1, Claude 3.7 and 4 era thinking features) are timestamped 2024-2025 facts and will be superseded.
The claim "explicit ReAct prompting is obsolete for frontier models" is current as of early 2026 and is safe to extrapolate; the claim about which models support interleaved thinking or how budgets are exposed is API ephemera that you must re-verify against provider docs before relying on it.
The failure taxonomy of traces and the grounding principle are stable and were chosen because they have already survived four model generations.

## Exercises

1. Take any modern agent trace you have access to (a Claude Code transcript, a LangGraph run log) and annotate ten consecutive steps with the ReAct role each element plays; then classify any failure you find using the taxonomy of section 2.
2. Implement classic text-format ReAct against a small open-weights model with no native tool use: few-shot prompt, regex parser, one search tool; record the format-failure rate over 30 runs, then replace the text format with JSON constrained decoding and measure again.
3. Write a one-page comparison of MRKL, Toolformer, and ReAct along three axes: where tool knowledge lives (prompt, weights, both), loop structure (single-shot vs iterative), and failure recovery; conclude with which axis each modern system inherited from which ancestor.
4. Design the harness-side observation policy for a web-browsing agent: maximum observation size, truncation strategy, and injection defenses; justify each choice against a failure mode from section 2.
5. Take a task where you would previously have written "Let's think step by step" plus few-shot thoughts, and specify instead the thinking configuration you would use on a current reasoning model (mode, budget, interleaving); state what you would measure to pick the budget.

## Godhood check

You have mastered this chapter when you can:

- Explain the reasoning-to-act and acting-to-reason synergy in your own words, with one concrete failure example each for reasoning-only and acting-only systems.
- Read an unfamiliar agent trace and name the failure class within a few steps of the divergence point.
- Recount what MRKL and Toolformer each contributed to the modern loop without conflating them.
- State precisely which ReAct ingredient was replaced by native tool use and which by thinking modes, and why the replacements are strictly better engineering.
- Argue when, if ever, you would still write an explicit ReAct prompt in 2026, and defend the answer.
