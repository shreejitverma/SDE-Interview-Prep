# Chapter 01 - What Is An Agent

## What you will master

- Why "agent" has no settled definition, and why the definitional fights are really fights about money, safety, and product positioning.
- The spectrum from hardcoded workflow to fully autonomous agent, and where real production systems actually sit on it.
- Anthropic's operational framing: an agent is a model using tools in a loop, and why this definition won by being useful rather than by being philosophically complete.
- The augmented LLM as the atomic building block: a model plus retrieval, tools, and memory.
- A practical taxonomy of autonomy degrees you can use to scope a system before writing code.
- The discipline of not building an agent: when a workflow, a single prompt, or a plain script beats an agent on every axis that matters.

## 1.1 The definition problem and its politics

Ask five practitioners what an agent is and you will get five answers, each shaped by what its author sells or fears.

This is not a sign that the field is confused about the technology.
It is a sign that "agent" is a word doing economic and rhetorical work, not just technical work.

Consider the incentives behind each common definition.

Framework vendors tend to define agents structurally: an agent is a thing with a planner, a memory module, and a tool registry.
This definition is convenient because it makes the framework's abstractions look load-bearing.
The downside is that it imports architecture into the definition, so a fifty-line loop that outperforms the framework does not count as an agent.

Marketing teams define agents by aspiration: an agent is software that does a job a human used to do.
This definition sells, but it is unfalsifiable and covers everything from a cron job to a coworker.

AI safety researchers define agents by goal-directedness: a system that pursues objectives over time, models its environment, and selects actions to achieve outcomes.
This definition is precise about what makes systems dangerous, but it classifies a thermostat as a weak agent and says little about how to build software this quarter.

Academic multi-agent-systems literature, which predates LLMs by decades, defines agents by properties: autonomy, reactivity, proactiveness, and social ability.
The terminology is rigorous, but it was designed for BDI architectures and robotic control, and it maps awkwardly onto a stochastic text model calling a weather API.

The practical resolution, and the one this volume adopts, is to stop asking "what is an agent" as an ontological question and ask it as an engineering question: what property of my system changes when I call it an agent.
The answer is control flow ownership.
In a workflow, your code decides what happens next.
In an agent, the model decides what happens next.
Everything else - memory, planning, multi-agent topologies - is elaboration on top of that single inversion of control.

The trade-off in adopting this narrow definition is that it excludes some systems people call agentic, such as a fixed retrieval-then-summarize pipeline.
That exclusion is deliberate.
A definition that includes everything predicts nothing about cost, latency, failure modes, or testing strategy, and those predictions are the entire point of having a definition.

## 1.2 The spectrum from workflow to agent

Anthropic's engineering guidance, first published in the "Building effective agents" essay in December 2024, drew a line that the industry largely adopted: distinguish workflows from agents.

Workflows are systems where LLMs and tools are orchestrated through predefined code paths.
Agents are systems where the LLM dynamically directs its own processes and tool usage, maintaining control over how it accomplishes tasks.

This is a spectrum, not a binary, and it is worth walking through the rungs in order of increasing model control.

### Rung 0: single augmented call

One prompt, one response, possibly with retrieval context or a forced tool call.
Your code owns everything: what goes in, what the output must look like, what happens next.
Classification, extraction, summarization, and translation live here.
Most LLM value shipped to production as of early 2026 still lives here, which is worth remembering when agent hype peaks.

### Rung 1: prompt chaining

A fixed sequence of LLM calls, where each step's output feeds the next step's input, often with programmatic checks between steps.
Example: generate an outline, validate it against length constraints in code, then write the document from the outline.
The decomposition trades latency for accuracy: each call does an easier task than the whole.
The model has zero control over sequencing.

### Rung 2: routing

An LLM classifies the input and your code dispatches to one of several specialized downstream paths.
Example: a support system that routes refund requests, technical questions, and account issues to different prompts, tools, or models.
The model makes exactly one decision, from a closed menu you wrote.

### Rung 3: parallelization

Your code fans a task out across multiple simultaneous LLM calls and aggregates programmatically.
Two subspecies matter: sectioning, where independent subtasks run in parallel, and voting, where the same task runs multiple times for diverse or higher-confidence output.
Control flow is still entirely yours.

### Rung 4: orchestrator-workers

A central LLM dynamically decomposes a task into subtasks it could not have enumerated in advance, delegates them to worker LLM calls, and synthesizes the results.
This is the first rung where the shape of the computation is decided at runtime by a model.
The number of subtasks, their content, and their ordering are model outputs.

### Rung 5: evaluator-optimizer

One LLM produces work, another evaluates it against criteria, and the loop repeats until the evaluator is satisfied or a budget runs out.
The model now controls loop termination within your outer bounds.

### Rung 6: the agent

The model receives a goal, a tool set, and an environment.
It plans, acts through tools, observes real results from the environment, adjusts, and continues until it judges the task complete or a hard limit stops it.
Your code executes tools and enforces budgets; the model owns the trajectory.

Two observations about this ladder matter more than the ladder itself.

First, each rung up buys generality and costs predictability.
A rung-1 chain has bounded cost, bounded latency, and unit-testable steps.
A rung-6 agent has none of those properties without deliberate engineering, which is what Chapters 5 and 6 of this volume are about.

Second, rungs compose.
A production agent is frequently a rung-6 loop whose individual tools are rung-1 chains, sitting behind a rung-2 router that decides whether the request needs an agent at all.
Reaching for composition before reaching for more autonomy is usually the right instinct.

## 1.3 Models using tools in a loop

The definition this track builds on is the one Anthropic's engineers converged on and popularized through 2024 and 2025: an agent is a model using tools in a loop.

Written as pseudocode, the entire concept is:

```
env_state = initial_task
while not done:
    action = model(env_state, history)
    observation = execute(action)
    history.append(action, observation)
    done = stop_condition(action, history)
```

The definition earns its keep through what it refuses to include.

It does not mention planning, because planning is just tokens the model emits before acting, not a separate module.
It does not mention memory, because within a task the message history is the memory, and across tasks memory is just another tool.
It does not mention reflection, self-critique, or reasoning strategies, because those are prompting patterns expressible inside the loop, not architecture.
It does not mention multi-agent systems, because a subagent is just a tool whose implementation happens to contain another loop.

This minimalism is a strong empirical claim: most of the capability lives in the model, and the harness should be thin.
The claim has held up well.
Claude Code, one of the most capable agents deployed as of early 2026, is at its core a single-threaded loop of exactly this shape, with capability coming from the model, the tool design, and the prompts rather than from orchestration machinery.
The bitter-lesson reading is that elaborate scaffolding built to compensate for model weaknesses tends to become dead weight one model generation later, while a thin loop rides each model improvement for free.

The trade-off of the thin-loop philosophy is that it concentrates trust in the model.
When the model is not capable enough for the domain, a thin loop fails openly and repeatedly, and you must either add scaffolding, constrain the task, or wait for a better model.
Knowing which of those three to choose is a judgment skill this volume tries to train.

## 1.4 The augmented LLM

The building block underneath every rung of the ladder is what Anthropic's essay called the augmented LLM: a base model enhanced with three capabilities.

Retrieval lets the model pull relevant knowledge into context on demand instead of relying on training data.
Tools let the model act on the world and observe fresh, ground-truth results.
Memory lets state survive beyond a single context window, whether as conversation history, files, or an external store.

Each augmentation changes the failure characteristics of the system, not just its capabilities.

Retrieval converts "the model does not know" into "the retriever did not find", which is a measurable, improvable engineering problem.
Tools convert hallucination into verifiable error: a model that claims a test passed can be contradicted by the actual exit code in the transcript.
Memory converts context-window exhaustion into a cache-management problem, with all the invalidation bugs that implies.

A point that will recur throughout this volume: the interface quality of each augmentation matters as much as its existence.
A tool with a vague description gets misused.
A retriever that returns 40,000 tokens of marginal context poisons the loop it was meant to help.
Anthropic's guidance is to spend as much design effort on tool definitions as on the prompts themselves, and Chapter 4 takes that instruction seriously.

## 1.5 Degrees of autonomy

"Autonomous" is not a property a system has or lacks; it is a budget you grant along several independent axes.
Scoping an agent means choosing a point on each axis explicitly.

### Axis 1: action authority

What can the agent do without a human in the loop.
Levels, roughly: read-only observation, reversible writes in a sandbox, reversible writes in production, irreversible actions with approval, irreversible actions without approval.
A coding agent that can read a repo and propose a diff sits two full levels below one that can push to main, even if the loop code is identical.

### Axis 2: temporal scope

How long the agent runs before a human sees anything.
Levels: single response, single task of minutes, session of hours, standing process that runs on a schedule or reacts to events indefinitely.
Cost variance and failure blast radius grow superlinearly with temporal scope, because errors compound across turns.

### Axis 3: goal ownership

Who decides what the agent works on.
Levels: human specifies each task, human specifies an outcome and the agent decomposes it, agent proposes tasks for approval, agent selects its own objectives within a charter.
Nearly all production systems as of early 2026 sit at the first two levels, and the systems at the third and fourth levels are mostly research artifacts and demos.

### Axis 4: resource authority

What the agent may spend: tokens, dollars, API rate limits, compute, and third-party quota.
An agent without an explicit resource budget has an implicit one, namely everything, and it will eventually find that budget for you.

Writing down a system's position on all four axes takes ten minutes and prevents the most common failure of agent projects, which is building level-4 authority infrastructure for a level-1 problem or, worse, granting level-4 authority to a system tested at level 1.

## 1.6 When not to build an agent

Anthropic's guidance opens with advice that most agent content buries: find the simplest solution possible, and only increase complexity when demonstrably needed.
Agents trade latency, cost, and predictability for capability on open-ended tasks, and that trade is frequently bad.

Use the following four gates, and require a yes on all of them before building an agent.

### Gate 1: complexity

Is the task genuinely open-ended, with steps that cannot be enumerated in advance.
If a human expert could write down the procedure as a flowchart, encode the flowchart as a workflow.
Workflows are cheaper, faster, testable step-by-step, and debuggable by reading code instead of transcripts.
"Turn a design doc into a working PR" clears this gate; "extract the invoice total from this PDF" does not.

### Gate 2: value

Does the outcome justify agent economics.
An agent burning several dollars of tokens across dozens of turns is easily two orders of magnitude more expensive than a single call.
A task worth fifty cents to solve cannot carry that cost; a task that replaces an hour of engineer time can.
Latency has the same shape: interactive users tolerate seconds, and agents routinely take minutes.

### Gate 3: viability

Is the current model actually capable at this task class, with the tools you can realistically provide.
The honest test is empirical: run twenty representative tasks through a thin prototype loop before committing to the project.
If the model succeeds at two of twenty, no amount of harness engineering will save the product, because harnesses amplify model capability rather than create it.

### Gate 4: cost of error

Can mistakes be caught cheaply and reversed.
Agents in domains with cheap verification, such as code with a test suite, thrive because the loop can self-correct against ground truth.
Agents in domains where errors are expensive, silent, or irreversible, such as sending external emails or mutating financial records, need human gates that erode the autonomy you were paying for.
If every action needs review, a workflow that drafts for human approval delivers the same value with far less machinery.

Two anti-patterns deserve explicit names because they recur constantly.

The resume-driven agent is a system built as an agent because agents are the prestigious shape in the current cycle, where a router plus three chains would outperform it on every metric.
The demo-shaped agent is a system whose autonomy demos well on curated tasks but whose gates 3 and 4 were never honestly evaluated, and which therefore ships with an unbounded error budget its owners discover in production.

## 1.7 A worked example

Task: customers email support asking to change their subscription plan.

The agent-shaped temptation: an autonomous support agent with tools for reading the account, modifying the subscription, issuing refunds, and replying to the customer.

Walking the gates: complexity is low, because plan changes follow an enumerable procedure with perhaps a dozen branches.
Value per task is modest.
Viability is fine, which is seductive.
Cost of error is high, because wrong charges and wrong emails are customer-visible and partially irreversible.

The correct build is a rung-2 router plus rung-1 chains: classify the request, extract the desired plan change with a schema-constrained call, look up the account in plain code, draft the reply and the change summary with a model call, and gate execution on one human click for anything touching billing.
The model makes two constrained decisions; your code owns everything else.
This system is boring, and boring is the highest compliment infrastructure can receive.

Now change one assumption: the requests are not plan changes but arbitrary technical problems requiring investigation across logs, docs, and the customer's configuration.
Complexity is now genuinely open-ended, verification is cheap because a proposed fix can be tested against the customer's reported symptoms, and value per resolution is high.
The gates now say agent, and the rest of this volume is about building it properly.

## 1.8 Vocabulary you will need

The following terms are used precisely for the rest of the volume.

Harness: the code you write around the model - the loop, tool executors, budgets, and safety gates.
Turn: one model call plus the execution of whatever tool calls it emitted.
Trajectory: the full sequence of turns for one task, as recorded in the message history.
Observation: any information returned to the model as a result of its action, including errors.
Stop condition: the predicate that ends the loop, whether model-initiated, budget-initiated, or human-initiated.
Environment: everything the tools can read or mutate, from a filesystem to a browser to a production database.

## Exercises

1. Take three systems currently marketed as "AI agents" and place each on the rung ladder from section 1.2, citing the specific control-flow evidence for your placement.
2. Write the four-axis autonomy scoping from section 1.5 for a coding assistant that reviews pull requests and pushes fix-up commits when its confidence is high.
3. Pick a task from your own work and run it through the four gates of section 1.6 in writing, reaching an explicit build or do-not-build verdict.
4. The pseudocode loop in section 1.3 has a subtle design decision: the stop condition inspects both the last action and the whole history. List three concrete stop conditions that require the history, not just the last action.
5. Argue the strongest possible case that the "models using tools in a loop" definition is too narrow, using a real system as your counterexample, then write the rebuttal.
6. Design the router-plus-chains version of the subscription-change system from section 1.7 as a diagram with every model call, code step, and human gate labeled.

## Godhood check

You have mastered this chapter when you can do the following without reference material.

- State three competing definitions of "agent", name the incentive behind each, and explain what the control-flow-ownership definition predicts that they do not.
- Reconstruct the workflow-to-agent ladder from memory with a concrete example per rung, and explain what each rung trades for what.
- Write the minimal agent loop as pseudocode and defend, one by one, why planning, memory, and multi-agent structure are absent from it.
- Scope any proposed agent on the four autonomy axes in under ten minutes, producing limits concrete enough to implement.
- Kill a bad agent project in a design review using the four gates, and specify the simpler system that should be built instead.
- Explain why cheap verification is the single strongest predictor of agent success in a domain, with two examples on each side.
