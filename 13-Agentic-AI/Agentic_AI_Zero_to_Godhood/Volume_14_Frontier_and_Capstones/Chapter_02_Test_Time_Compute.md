# Chapter 02 - Test-Time Compute

## What you will master

- The test-time scaling axis: why spending more compute at inference became a first-class capability lever alongside pretraining scale.
- Sequential scaling: longer chains of thought, what actually improves with thinking length, and where it saturates or backfires.
- Parallel scaling: best-of-n, self-consistency, and the selection problem that determines whether parallel samples are worth anything.
- Search: beam search over reasoning steps, MCTS-style lookahead, and why tree search has underdelivered for LLM reasoning relative to its Go-era reputation.
- Verifier models: outcome reward models versus process reward models, how PRMs are trained, and their failure modes.
- Compute-optimal test-time strategies: matching the method to the difficulty, and the substitution curve between model size and inference compute.
- Interleaved thinking in agent loops: how extended thinking interacts with tool use, and what it changes about harness design.
- The economic framing: intelligence per dollar per second, and how to reason about test-time compute as a product and infrastructure decision.

## 1. The third scaling axis

Volume 01 covered the pretraining scaling laws: loss falls predictably with parameters, data, and training compute.
By 2024 the field had internalized two axes (pretraining scale, post-training quality) and then o1 made a third one legible: capability rises with the compute spent at inference on a single problem.
OpenAI's o1 release (September 2024) showed log-linear accuracy gains on math and code benchmarks as thinking-token budgets grew, and DeepSeek-R1 (January 2025) replicated the phenomenon openly.

The conceptual shift matters more than any specific model.
Before: a model's capability was a fixed property, and you bought more capability by buying a bigger model.
After: capability is a curve over inference spend, and every product decision becomes a point on that curve.
This is the single most consequential change to agent economics since function calling, because agents are exactly the workloads where per-task inference spend varies by orders of magnitude.

A necessary caution before the mechanics: test-time compute is not free capability.
It buys the most on tasks with verifiable or at least checkable structure (math, code, logic, constrained retrieval), much less on open-ended generation, and the gains come with latency and cost that scale linearly-to-worse with the spend.
Every section below should be read with "on which task distribution, at what cost" attached.

## 2. Sequential scaling: longer thinking

The simplest way to spend more inference compute is to let the model generate more tokens before answering.
Chain-of-thought prompting (2022) was the manual version; reasoning models made it trained behavior, with RL (Chapter 01) teaching the model to use long chains productively: decomposing, checking intermediate results, backtracking, and trying alternative approaches.

What actually improves with thinking length:

- Multi-step problems where errors are locally detectable: arithmetic, symbolic manipulation, code tracing, constraint satisfaction.
- Problems that benefit from enumerating cases before committing.
- Self-correction, but only in models RL-trained for it; base models mostly do not fix their own errors by generating more, they elaborate on them.

Where it saturates or backfires:

- Accuracy versus thinking length is concave, and beyond a task-dependent point more thinking yields nothing; you pay latency for zero capability.
- Overthinking is a measured phenomenon: on easy problems, reasoning models sometimes talk themselves out of correct first answers, and several 2025 papers document non-monotonic accuracy curves.
- Thinking tokens are context: a 40,000-token reasoning trace consumes window and can crowd out task material in agent settings, interacting with everything you learned in Volume 06.
- Latency is sequential and irreducible: 30,000 thinking tokens at typical decode speeds is tens of seconds to minutes, and no parallelism helps a single chain.

The provider surface, date-stamped early 2026: Anthropic exposes extended thinking with a token budget parameter (API shape current as of 2025), OpenAI exposes reasoning-effort levels rather than raw budgets, and Google's Gemini line exposes thinking budgets; all three converged on "the developer buys a capability tier per request," which is the correct abstraction and likely to persist even as the parameter names churn.
Adaptive thinking - the model or a router deciding budget per query rather than the developer fixing it - shipped in various forms through 2025 and is the clear direction, because fixed budgets waste money on easy inputs and starve hard ones.

## 3. Parallel scaling: sampling with selection

The second axis is sampling n independent completions and selecting one.
The capability question decomposes cleanly into coverage and selection, and keeping those separate will save you from most confusions in this literature.

Coverage: does the correct answer appear anywhere in n samples?
This is pass@n, and it rises impressively with n on many tasks; a model may have pass@1 of 30 percent but pass@100 above 80 percent on competition math or coding tasks (order of magnitude from the 2024-2025 literature, not a durable number).
High pass@n at low pass@1 means the model "knows" the answer in distributional terms but cannot reliably commit to it, which is precisely the gap that RL training (Chapter 01) and selection methods (this section) attack from opposite ends.

Selection: can you find the right sample without an oracle?
The methods, in ascending sophistication:

- Majority voting (self-consistency, 2022): sample n chains, take the most common final answer; requires answers to be canonicalizable (numbers, multiple choice), free of any extra model, and surprisingly strong; useless for open-ended outputs where no two samples match.
- Best-of-n with a verifier: score each sample with a checker (unit tests for code) or a learned reward model, pick the top; as good as the verifier, and only as good as the verifier.
- LLM-as-judge selection: a model ranks the candidates; flexible, works for open-ended outputs, inherits every judge bias from Volume 10 (position bias, verbosity bias, self-preference).
- Generative self-verification: the model checks each candidate in a fresh context; generation-verification gap determines whether this helps, and for many tasks models verify better than they generate, which is why it often does.

The selection ceiling is the fundamental law here: parallel sampling capability equals coverage times selection accuracy, and with imperfect selectors the realized gain is far below pass@n.
When a leaderboard entry says "with 64 samples," your first question is what the selector was, and your second is whether the selector had access to ground truth (a test suite) that production traffic will not have.

Parallel scaling's operational virtues, which explain its production popularity:

- Latency is one generation, not n, given parallel capacity; this is the opposite profile from sequential scaling.
- Failures are independent-ish, so it derisks stochastic wrong turns.
- It composes with everything else: you can parallel-sample entire agent trajectories, which is exactly what heavy modes of frontier agent products do (multiple attempts at a coding task, best one kept, shipped in various 2025-era products).

## 4. Search: best-of-n's smarter siblings

Best-of-n commits to full samples before evaluating.
Search evaluates partial progress and reallocates compute toward promising branches, which should be strictly better and in practice is only sometimes better.

Beam search over reasoning steps: maintain k partial chains, extend each, score partial states with a process verifier (Section 5), keep the best k, repeat.
This buys early pruning of doomed branches at the cost of needing a stepwise scorer and of beam collapse, where superficially-appealing-but-wrong steps dominate the beam because the scorer is imperfect exactly where it matters.

MCTS-style methods: build a tree over reasoning or action steps, balance exploration and exploitation with UCB-style rules, back up value estimates from rollouts or a value model.
The Go-era intuition says this should be transformative; the honest early-2026 reading of the literature is that it has not been, for identifiable reasons:

- No cheap simulator: in Go, rollouts are free and the value signal is exact; in reasoning, every node expansion is an LLM call and every evaluation is another one, so the tree is starved.
- No natural state merge: distinct token prefixes are distinct states even when semantically identical, so the tree branches into redundancy.
- Value estimation is the same hard problem as selection; MCTS launders it through more machinery without solving it.
- Long-chain sequential thinking captures much of the same benefit implicitly, because a trained model backtracking inside one chain is doing depth-first search in token space without any external tree.

Where explicit search does pay, as of early 2026: agentic settings with real environment feedback at intermediate steps (a failing test is a free, exact process signal), theorem proving against a proof checker (every step machine-verified, the ideal case), and offline data generation for training, where you can spend absurd compute per problem because it amortizes into weights.
The stable principle: search helps in proportion to the quality and cheapness of intermediate evaluation, and token-space reasoning search mostly lacks both.

## 5. Verifiers: ORMs and PRMs

Selection and search both reduce to evaluation, so the evaluator is the load-bearing component of the whole test-time stack.

Outcome reward models (ORMs) score complete solutions.
Training data is straightforward when tasks have checkable answers: sample solutions, label by final correctness, train a classifier.
Weakness: an ORM credits lucky wrong reasoning that stumbles onto the right answer and cannot localize errors, which makes it useless for guiding search.

Process reward models (PRMs) score individual reasoning steps.
OpenAI's "Let's Verify Step by Step" (2023) is the canonical reference: human-labeled step correctness on math (the PRM800K dataset), with the PRM beating an ORM as a best-of-n selector on competition math.
Because human step labels are brutally expensive, the field moved to automated labeling, principally Monte Carlo estimation (Math-Shepherd style, 2024): a step's value is estimated by rolling out many completions from that step and measuring how often they reach a correct final answer.

PRM failure modes you must know before trusting one:

- Domain narrowness: PRMs trained on math grade math-shaped text; transfer to code review or agent action-selection is poor without retraining, and general-purpose PRMs remained an open problem through 2025.
- Value-versus-correctness confusion: Monte Carlo labels measure "can the model recover from here," not "is this step valid," so a strong policy makes wrong steps look fine.
- Gameability: optimize hard against a PRM (in search or in RL) and you rediscover Goodhart; generators learn step styles the PRM loves.
- Distribution shift: a PRM trained on one model's chains degrades on another model's chains, an annoying practical coupling.

A notable 2025 development, date-stamped: DeepSeek-R1's authors reported deliberately not using PRMs in their RL pipeline, citing reward hacking and label cost, and relying on outcome rewards plus emergent self-checking instead.
The field's tentative synthesis as of early 2026: PRMs earn their keep as test-time selectors and search guides in narrow verifiable domains, while training-time reward has consolidated around outcome verification; treat any stronger claim in either direction as speculation.

## 6. Compute-optimal test-time strategies

Given a fixed inference budget, how should you spend it?
The reference result is Snell et al. (2024), "Scaling LLM Test-Time Compute Optimally Can Be More Effective than Scaling Model Parameters," and its qualitative findings have held up well enough to teach as principles:

- Difficulty determines the best method: on easy problems, sequential refinement (revising one chain) wins; on hard problems, parallel exploration wins because the model's first attempt is likely in the wrong basin entirely.
- Adaptive allocation beats any fixed strategy: estimating difficulty and routing the budget accordingly outperformed uniform best-of-n at equal compute by large factors.
- The size-versus-inference substitution is real but bounded: a smaller model with generous test-time compute can match a much larger model's pass@1 on some distributions, and cannot on others; if the small model's coverage is near zero on a task, no amount of sampling or search recovers it, because you cannot select what you never generate.

Translating this into engineering practice for agent systems:

- Build the router: classify incoming tasks by expected difficulty (a cheap model call, or historical telemetry per task type) and assign thinking budgets and sample counts per class; this is the test-time analogue of the model-routing you built in Volume 12.
- Exploit asymmetric verification: when a task has a cheap checker, crank parallel sampling aggressively, because your selection is near-perfect; when it does not, prefer sequential thinking, because your selector would be the bottleneck anyway.
- Cascade: attempt cheap-and-fast first, escalate to expensive modes on failure signals (failed tests, low judge scores, explicit model uncertainty); cascades dominate fixed policies in cost-capability space in almost every production report through 2025.
- Cap and monitor: every escalation policy needs a budget ceiling per task and telemetry on realized spend, or a pathological input class will quietly own your margins.

## 7. Interleaved thinking in agent loops

Reasoning models plus tool use produce a distinctive loop shape: think, call a tool, observe, think about the observation, call again.
Anthropic shipped this as interleaved thinking in 2025 (API shape current as of then); other providers expose equivalents.
This changes agent engineering in specific ways.

What it buys:

- Deliberate tool choice: thinking before each call replaces reflexive calling, measurably reducing wasted calls on hard tasks.
- Observation digestion: thinking after a tool result lets the model reconcile surprises (a failing test, an unexpected file) before acting, which is where naive agents historically derailed.
- Plan revision mid-trajectory rather than only at the start, subsuming much of the explicit reflection machinery you built in Volume 04.

What it costs and complicates:

- Context pressure: thinking blocks accumulate across turns; providers differ in whether prior-turn thinking persists in context or is dropped, and your compaction strategy (Volume 06) must account for whichever holds, because as of early 2026 this behavior is provider-specific and version-specific; check current docs rather than trusting this page.
- Latency stacking: per-step thinking multiplied by many steps can turn a 30-second task into ten minutes; per-step budget caps matter more than per-request ones.
- Observability: thinking traces are gold for debugging trajectories and also raise the redaction and storage questions from Volume 10; some providers return signed or encrypted thinking blocks, constraining what your tracing layer can do.
- Prompt-injection surface: thinking that reasons about untrusted tool output is still conditioning on it; interleaved thinking does not change any of Volume 11's threat model, a point worth stating because "the model will reason its way past the injection" is a real and wrong assumption in the wild.

Harness design guidance: treat per-step thinking budget as a tunable on the same footing as tool timeout, set it per tool-call type (high before irreversible actions, low before cheap reads), and A/B it against your eval suite, because the optimal setting is empirically task-distribution-specific.

## 8. The economics: intelligence per dollar per second

Test-time compute collapses the model-quality question into a portfolio question: for this task, at this latency tolerance, at this budget, which point on which model's inference-scaling curve maximizes utility?
Three framings carry most of the reasoning weight.

Cost per solved task, not cost per token.
A mode that costs 8x per attempt but doubles solve rate on tasks worth hundreds of dollars is cheap; the same mode on tasks worth cents is absurd.
Compute cost per solved task equals cost per attempt divided by solve rate, plus the expected cost of failure handling, and this quantity - not benchmark accuracy - is what you should optimize and report internally.
The ARC-AGI episode is the canonical cautionary tale, date-stamped: o3's December 2024 high-compute result spent thousands of dollars of inference per task at the extreme setting; a capability demonstration, not a product configuration, and a permanent reminder to read the compute axis of any announcement.

The latency-value surface.
Interactive products have seconds; agentic background work has minutes to hours; offline pipelines have days.
Test-time compute monetizes best where latency tolerance is high, which is a structural reason the frontier pushed toward asynchronous, long-running agents through 2025: the async form factor is what makes large inference spend sellable at all.

Deflation and the option value of scaffolds.
Inference cost per unit of capability fell rapidly and repeatedly through 2024-2025 (order of magnitude per year on many workloads, via model efficiency, distillation, and hardware; direction robust, rate not to be extrapolated naively).
Consequence one: a test-time strategy that is marginally uneconomic today may be comfortably economic in twelve months, so build the scaffolds and gate them behind budget flags.
Consequence two: capability gains from scaffolding compound with capability gains from models, so the teams that master budget-aware orchestration get the full product of both curves, which is the closing argument of this chapter and, in a sense, of the whole track.

## Exercises

1. For a task family you care about (for example LeetCode-style problems or SQL generation), measure pass@1, pass@8, and pass@32 for one model, then implement majority voting and best-of-n with a test-based verifier and report realized accuracy at equal sample counts.
   State the coverage-versus-selection decomposition of your results explicitly.
2. Using one provider's thinking-budget or reasoning-effort control, sweep three budget levels over a mixed-difficulty eval set and plot accuracy and latency per level; identify the saturation point and at least one overthinking case in the transcripts.
3. Design a compute-router on paper: task classes, difficulty signals, the escalation ladder (budgets, sample counts, model tiers), the per-task cost cap, and the telemetry you would log to tune it; then implement the simplest two-rung version and measure cost per solved task against a fixed-policy baseline.
4. Write a one-page analysis of why MCTS underperformed expectations for LLM reasoning, arguing from simulator cost, state identity, and evaluator quality; then describe one agentic setting where its assumptions are satisfied and search genuinely pays.
5. Train or prompt a simple step scorer (an LLM judge with a step-grading rubric) and use it for beam search on math word problems; document one concrete instance where the scorer's error steered the beam wrong, and classify it against the PRM failure modes in Section 5.
6. Compute, for a real or hypothetical product, the full cost-per-solved-task table across three inference modes, including failure-handling cost; write the recommendation memo you would send to the team, with the downside of your recommended mode named.

## Godhood check

You are at godhood level for this chapter when you can do the following without notes.

- Draw the three scaling axes and place sequential thinking, parallel sampling, and search on the test-time axis, with the latency and cost profile of each.
- Decompose any sampling-based result into coverage and selection, and interrogate a leaderboard claim by asking the two selector questions from Section 3.
- Explain ORMs versus PRMs, how PRM labels are made without humans, four PRM failure modes, and the early-2026 state of PRMs in training versus test-time roles.
- State the compute-optimal findings (difficulty-dependence, adaptive allocation, bounded size-substitution) and turn them into a concrete router-plus-cascade design for a given product.
- Reason about interleaved thinking's effect on context management, latency, observability, and injection surface, and specify per-step budgets in a harness.
- Frame any test-time compute decision as cost per solved task on a latency-value surface, and explain, with the ARC-AGI example, how to read the compute axis of a capability announcement.
- Say precisely which claims in this chapter are stable principles and which are early-2026 snapshots that you must re-verify before relying on them.
