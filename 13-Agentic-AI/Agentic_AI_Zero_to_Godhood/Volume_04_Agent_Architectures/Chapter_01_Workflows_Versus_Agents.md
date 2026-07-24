# Chapter 01 - Workflows Versus Agents

## What you will master

- The precise distinction between a workflow and an agent, and why the industry conflated them for two years.
- The composable-patterns taxonomy popularized by Anthropic's "Building Effective Agents" essay (December 2024): prompt chaining, routing, parallelization, orchestrator-workers, evaluator-optimizer.
- Pseudocode implementations of each pattern that you could port to any provider SDK in an afternoon.
- The simplicity-first doctrine: why you should climb this ladder from the bottom and stop at the first rung that solves your problem.
- A concrete checklist for deciding when a deterministic workflow beats an autonomous agent.

## 1. Definitions that actually cut

The word "agent" was applied to everything from a single prompt template to a fleet of self-replicating processes, which made most 2023-2024 architecture discussions useless.
Anthropic's December 2024 essay "Building Effective Agents" gave the field a vocabulary that stuck, and this chapter follows it.

A **workflow** is a system where LLM calls and tools are orchestrated through predefined code paths.
The developer decides the control flow at design time; the model fills in content within a structure it does not control.

An **agent** is a system where the LLM dynamically directs its own process and tool usage, deciding at runtime which step comes next and when it is done.
The developer supplies tools, an environment, and a goal; the model owns the control flow.

The distinction is about who holds the program counter, not about how many LLM calls occur or how impressive the output looks.
A ten-step pipeline with fixed ordering is a workflow no matter how sophisticated each step is.
A single loop where the model chooses tools until it declares success is an agent even if it usually finishes in two iterations.

Both are instances of what the essay calls **agentic systems**, built from one shared primitive: the **augmented LLM**, meaning a model call equipped with tools, retrieval, and memory.
Every pattern in this chapter is a different way of wiring augmented LLM calls together.

The distinction matters operationally because the two families fail differently.
Workflows fail like normal software: a step produces bad output, and you can find the step, inspect its inputs, and fix its prompt.
Agents fail like employees: they misunderstand goals, go down rabbit holes, declare premature victory, and their failures are path-dependent and harder to reproduce.
Choosing between them is choosing which failure mode you want to debug for the life of the system.

## 2. The augmented LLM building block

Before the patterns, fix the primitive in your mind.

```
def augmented_llm(prompt, context, tools) -> Response:
    # One model call that may:
    #  - read retrieved documents supplied in context
    #  - request tool calls, which the harness executes
    #  - return structured output (JSON) or free text
    ...
```

Everything below composes this block.
The essay's core claim, which held up through 2025, is that the most successful production systems were not built on complex frameworks but on simple, composable arrangements of this primitive.
Frameworks (LangGraph, the OpenAI Agents SDK, Claude Agent SDK, and others; Volume 08 covers them) can help, but they add layers of abstraction that obscure the underlying prompts and responses, and the essay's advice is to start by using LLM APIs directly, since many patterns are a few dozen lines of code.

## 3. Pattern: prompt chaining

Prompt chaining decomposes a task into a fixed sequence of LLM calls, where each call processes the output of the previous one.
You can insert programmatic gates between steps that validate intermediate output and abort or retry early.

Use it when the task decomposes cleanly into fixed subtasks known at design time.
The trade is latency for accuracy: each hop adds a round trip, but each individual call gets a simpler job, which raises per-step reliability.

Canonical examples: generate marketing copy then translate it; write a document outline, check the outline against constraints in code, then write the document from the outline.

```
def chain(input):
    outline = llm("Write an outline for: " + input)
    if not gate_ok(outline):          # deterministic check, e.g. section count
        outline = llm("Fix this outline to satisfy X: " + outline)
    draft = llm("Write the document following this outline: " + outline)
    final = llm("Tighten the prose, keep all facts: " + draft)
    return final
```

Design notes that separate good chains from bad ones:

- Gates should be code, not model calls, wherever the property is mechanically checkable; a regex or schema validation is free and never hallucinates.
- Each step's prompt should be independently testable with recorded inputs; a chain is only as debuggable as its least observable link.
- Resist chains longer than four or five steps; error compounds multiplicatively, and if step accuracy is 95 percent, a seven-step chain is below 70 percent end to end unless gates catch failures.
- The downside of chaining is rigidity: if real inputs vary in structure, a fixed decomposition forces every input through the same shape, and the wrong decomposition is worse than none.

## 4. Pattern: routing

Routing classifies an input first, then dispatches it to a specialized downstream path: a different prompt, a different toolset, or a different model.

Use it when inputs fall into distinct categories that are better handled separately, and when classification is easier than handling.
It enables separation of concerns: each route's prompt is optimized for one input type instead of one prompt trying to handle everything, which usually degrades performance on every category.

Canonical examples: customer support triage (refund vs technical vs general questions), and cost tiering (route easy questions to a small cheap model, hard ones to a frontier model).

```
def route(input):
    category = llm_classify(input, labels=["refund", "technical", "general"])
    handler = {
        "refund":    refund_workflow,      # tools: order lookup, refund API
        "technical": technical_workflow,   # tools: docs search, diagnostics
        "general":   general_prompt,       # no tools, cheap model
    }[category]
    return handler(input)
```

Design notes:

- The classifier can be a small model or even embeddings plus nearest-centroid; it needs to be accurate, not eloquent.
- Always include an escape route for "none of the above"; the most damaging routing bug is a confident misclassification that sends a legal threat to the FAQ bot.
- Log the routing decision as a first-class field; route distribution drift is your earliest signal that user behavior changed.
- The downside is added latency and a new failure point; if categories are fuzzy or overlapping, routing pushes errors upstream where they are hardest to recover from.

## 5. Pattern: parallelization

Parallelization runs multiple LLM calls simultaneously and aggregates the results in code.
The essay identifies two variants with different purposes.

### 5.1 Sectioning

Sectioning splits a task into independent subtasks that run in parallel.

Canonical examples: reviewing a large document by running one call per section; implementing guardrails by running the content check and the task itself as separate parallel calls, so neither prompt is diluted by the other's instructions.

```
def sectioned_review(document):
    sections = split(document)
    reviews = parallel_map(lambda s: llm("Review this section: " + s), sections)
    return llm("Merge these section reviews into one report: " + join(reviews))
```

### 5.2 Voting

Voting runs the same task multiple times to get diverse outputs, then aggregates by majority, intersection, or threshold.

Canonical examples: several calls reviewing code for vulnerabilities where any single flag escalates; scoring content where you require k-of-n agreement to act, trading false positives against false negatives via the threshold.

```
def vote_vulnerable(code, n=5, threshold=2):
    verdicts = parallel_map(lambda _: llm_bool("Is this code vulnerable? " + code),
                            range(n))
    return sum(verdicts) >= threshold
```

Design notes:

- Sectioning only works when subtasks are genuinely independent; hidden cross-section dependencies (a variable defined in section 1, used in section 4) make parallel review confidently wrong, so decide independence in code before splitting.
- Voting buys reliability with linear cost multiplication; five votes is five times the tokens, so reserve it for decisions where an error is expensive relative to inference.
- Aggregation is a real design problem: majority vote suits classification, union suits recall-critical detection, and a final LLM merge call suits prose, but that merge call is itself a failure point.
- Sampling temperature and prompt variation across voters matter; identical deterministic calls vote identically and buy nothing.

## 6. Pattern: orchestrator-workers

An orchestrator LLM dynamically decomposes a task into subtasks, delegates each to worker LLM calls, and synthesizes their results.

This differs from parallelization in one crucial way: the subtasks are not predefined but determined by the orchestrator at runtime based on the specific input.
That makes it the bridge pattern between workflows and agents; the topology is fixed (one orchestrator, N workers, one synthesis) but the decomposition is dynamic.

Canonical examples: coding tasks where the set of files needing changes depends on the request; research tasks that gather and analyze information from multiple sources decided on the fly.
Anthropic's own multi-agent research system (described publicly in June 2025) is this pattern at scale: a lead agent plans and spawns parallel search subagents, then synthesizes.

```
def orchestrate(task):
    plan = llm_json("Break this task into independent subtasks with "
                    "clear deliverables: " + task)      # returns list of specs
    results = parallel_map(lambda spec: worker_llm(spec, tools=worker_tools),
                           plan.subtasks)
    return llm("Synthesize these results into the final answer for the "
               "original task: " + task + join(results))
```

Design notes:

- The orchestrator's hardest job is writing worker instructions; vague specs produce workers that duplicate work or drift, so force the plan into a schema with objective, inputs, and expected output per subtask.
- Workers should not talk to each other; if they need to, your decomposition is wrong or you actually need a sequential chain.
- Cap worker count and give the orchestrator explicit guidance on effort scaling; orchestrators otherwise overspawn on simple queries, and multi-agent token costs run an order of magnitude above single-call chat.
- The downside is compounding indirection: a wrong decomposition dooms every worker, and debugging requires tracing three layers instead of one.
- Volume 07 treats the full multi-agent generalization; here it is enough to see it as a workflow with one dynamic joint.

## 7. Pattern: evaluator-optimizer

One LLM call generates a response; another evaluates it against criteria and provides feedback; the generator revises; the loop repeats until the evaluator accepts or a budget runs out.

Use it when clear evaluation criteria exist and iterative refinement provides measurable value, meaning: an LLM can articulate useful critiques of the output, and revisions given those critiques actually improve it.
The classic litmus test is whether a human editor could improve the output by giving notes; literary translation and complex search with completeness requirements are the essay's examples.

```
def evaluate_optimize(task, max_rounds=3):
    draft = llm_generate(task)
    for _ in range(max_rounds):
        review = llm_evaluate(task, draft)   # returns verdict + specific critiques
        if review.verdict == "pass":
            return draft
        draft = llm_generate(task, feedback=review.critiques)
    return draft   # best effort; surface that budget was exhausted
```

Design notes:

- The evaluator prompt must demand specific, actionable critiques against explicit criteria; "make it better" feedback produces oscillation, not improvement.
- Separate the evaluator's context from the generator's; an evaluator that saw the generator's reasoning tends to rubber-stamp it (Chapter 04 dissects this failure at length).
- Always cap rounds and keep the best-so-far draft; unbounded refinement loops are the pattern's signature production incident.
- The downside is cost multiplication with diminishing returns; empirically most of the gain arrives in the first revision, so budgets beyond two or three rounds rarely pay.
- This pattern is the workflow-shaped ancestor of reflection in agents, covered in depth in Chapter 04.

## 8. Autonomous agents

An agent begins with a command from or discussion with a human, then plans and operates independently: an LLM in a loop, using tools based on environmental feedback, until it judges the task complete or hits a stopping condition.

```
def agent(goal, tools, max_steps=50):
    history = [goal]
    for _ in range(max_steps):
        action = llm_decide(history, tools)   # native tool use call
        if action.type == "finish":
            return action.result
        observation = execute(action, environment)
        history.append((action, observation))
    return escalate_to_human(history)
```

The two properties that make this work, per the essay and everything the field learned since:

- **Ground truth from the environment.** The agent must get real feedback each step (tool results, code execution output, test failures), because without it the loop is just the model talking to itself.
- **Checkpoints and stopping conditions.** Max iterations, budget caps, and human gates for irreversible actions are not optional hardening; they are part of the definition of a responsible deployment.

Agents suit open-ended problems where you cannot predict the number of steps or hardcode a path: coding tasks spanning many files, computer-use tasks, deep research.
The cost is higher spend, higher latency, and compounding-error risk, which is why the essay recommends extensive sandboxed testing and guardrails before trusting one.

The essay's much-quoted line applies: agents are "just LLMs using tools based on environmental feedback in a loop."
The sophistication lives in the tools, the environment, and the harness (Chapter 06), not in the loop.

## 9. The simplicity-first doctrine

The essay's central recommendation, validated repeatedly in production since: find the simplest solution possible, and only increase complexity when demonstrably needed.
Agentic systems trade latency and cost for task performance, and you should ask whether that trade is worth it for your task before making it.

Operationally, climb this ladder and stop at the first rung that meets your quality bar:

1. A single augmented LLM call with good retrieval and in-context examples.
2. Prompt chaining, if the task has a fixed decomposition.
3. Routing, if inputs cluster into types.
4. Parallelization, if independence or voting helps.
5. Orchestrator-workers, if decomposition is input-dependent.
6. Evaluator-optimizer, if critique measurably improves output.
7. A full agent loop, if the path is genuinely unpredictable.

Reasons this ordering is not just aesthetic conservatism:

- Every rung down is cheaper to run, faster to respond, easier to test, and easier to explain to the person paged at 3 a.m.
- Deterministic control flow means deterministic evals; you can regression-test step 3 in isolation, which you cannot do for an emergent trajectory.
- Model upgrades favor simple systems; a thin harness over a better model improves for free, while a complex scaffold built around an old model's weaknesses becomes a straitjacket (this is the "bitter lesson" applied to harness design, expanded in Chapter 06).

The doctrine has a genuine downside worth naming: teams that internalize "avoid agents" sometimes ship brittle 15-branch workflows for tasks that a 2025-class model handles fine in a simple loop, and the maintenance cost of encoded-by-hand control flow can exceed the inference cost it saved.
Simplicity-first means simplest system that works, not maximally deterministic system.

## 10. When a deterministic workflow beats an autonomous agent

Prefer a workflow when most of these hold:

- The decomposition is stable across inputs; every request goes through essentially the same steps.
- Intermediate outputs are mechanically checkable, so code gates catch errors cheaply.
- Latency or cost budgets are tight; workflows let you use small models per step and parallelize predictably.
- Errors are expensive and audits are required; fixed paths give you replayable, explainable traces.
- Volume is high; a 2 percent flakiness rate that is tolerable at 100 runs a day is an incident stream at 100,000.

Prefer an agent when most of these hold:

- The step count and step identity vary per input and cannot be enumerated at design time.
- The environment provides cheap, reliable ground truth (tests, compilers, API responses) that the loop can react to.
- A human reviews or gates the final output, so autonomy risk is bounded.
- Task value per run is high enough to absorb 10x-100x token cost and minutes of latency.

The honest middle position, common in mature systems: a workflow skeleton with agentic joints.
Fixed stages for the predictable parts, and a bounded agent loop inside the one stage where the path genuinely varies.
Most production "agents" as of early 2026 are exactly this shape.

## 11. Claims that will rot

The taxonomy (chaining, routing, parallelization, orchestrator-workers, evaluator-optimizer, agents) is a stable way to think and is worth memorizing.
The placement of the workflow/agent frontier is not stable: each model generation moves tasks from "needs scaffolding" to "single call suffices" and from "needs a workflow" to "agent handles it."
Statements in this chapter about what models can or cannot do reliably describe early 2026 and should be re-derived, not trusted, a year later.

## Exercises

1. Take a task you currently solve with one large prompt and rewrite it as a three-step chain with one code gate; measure end-to-end accuracy of both on 20 examples and report which won and why.
2. Implement the voting pattern for a yes/no classification you care about with n=1, 3, and 5 samples; plot accuracy versus token cost and find the knee.
3. Write the orchestrator prompt for a "summarize this codebase" task, including the subtask schema; then list three ways a bad decomposition would poison the workers, and one code-level check per failure.
4. Take the agent pseudocode in section 8 and add the minimal set of checkpoints you would demand before letting it call a tool that sends email; justify each.
5. For each of the five workflow patterns, name one production task at your current or last job where it is the correct stopping rung, and one where applying it would be over-engineering.

## Godhood check

You have mastered this chapter when you can:

- State the workflow/agent distinction in one sentence about control flow, and defend why call count is irrelevant to it.
- Reproduce all five workflow patterns from memory in pseudocode, each with its trigger condition and its named downside.
- Explain why orchestrator-workers is the bridge pattern between the two families.
- Argue both directions of the simplicity-first doctrine, including the failure mode of over-applying it.
- Given a novel task description, place it on the ladder in under a minute and articulate the evidence that would move it up or down a rung.
