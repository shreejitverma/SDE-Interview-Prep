# Chapter 06 - Reasoning Models

## What you will master

- Chain-of-thought as a mechanism: why generated intermediate tokens buy real computation, and the limits of prompting for it.
- Why RL on verifiable rewards, not more preference tuning, produced the o1/R1 class of reasoning models.
- GRPO at a working level: the group-relative advantage trick, what it removes from PPO, and why it fits verifiable-reward training.
- Test-time compute scaling: the tradeoff curve between thinking longer and training bigger, and the sampling-plus-verification family.
- Extended thinking and interleaved thinking in the Claude line, and how reasoning surfaces in provider APIs as of early 2026.
- The judgment call that matters for agent builders: when reasoning models help, when they waste tokens, and how to budget thinking.

## 1. Chain-of-thought: computation in the token stream

A transformer performs a fixed amount of computation per emitted token: one forward pass through a fixed number of layers.
A question whose answer requires many sequential steps cannot reliably be answered in a single token's worth of compute, no matter how large the model, because the serial depth of the computation exceeds the serial depth of the network.
Chain-of-thought (CoT) breaks the ceiling by externalizing intermediate state into the context: each generated token is written to the sequence, becomes input to subsequent forward passes, and thereby extends the effective serial depth of the computation without bound.
This is the correct mechanistic frame for CoT: the context window is scratch memory, and generation is iteration.

The empirical lineage: Wei et al. (2022) showed that prompting with worked examples containing intermediate reasoning steps dramatically improved math and multi-step performance in sufficiently large models; Kojima et al. (2022) showed the degenerate prompt "Let's think step by step" alone recovered much of the gain zero-shot.
Self-consistency (Wang et al., 2022) added sampling: draw many chains at nonzero temperature, take a majority vote over final answers, and accuracy rises with sample count; this was the first clean demonstration that spending more inference compute buys accuracy on reasoning tasks, the seed of everything in Section 4.

The limits of prompted CoT, which motivated training for it instead:

- Prompted chains are imitations of reasoning-shaped text from pretraining, not optimized computation; models produce fluent chains with confident wrong steps, and accuracy on the step level was never trained.
- The chain is not guaranteed faithful: models can reach answers by internal computation and then rationalize a chain post hoc, a phenomenon documented repeatedly (Turpin et al., 2023) and a live concern as of early 2026 for anyone hoping to audit reasoning by reading it.
- Error recovery is untrained: pretraining text rarely shows an author noticing a mistake mid-derivation and backtracking, so prompted models rarely do it either.
The behavioral signature of trained reasoning models, visible in R1's published traces, is precisely the appearance of backtracking, self-checking, and "wait, that is wrong" moves that prompting alone almost never produces.

## 2. RL on verifiable rewards

### 2.1 Why preference RL was not enough

Chapter 05's pipeline optimizes against a learned reward model fit to human preferences, and it has a ceiling for reasoning: human raters and learned RMs judge how good an answer looks, and plausible-but-wrong reasoning is exactly the failure mode preference signal is worst at catching.
Optimizing hard against a soft proxy invites Goodhart failures (Chapter 05, Section 4), so the KL leash must stay tight, which caps how much behavior can change.

Verifiable domains dissolve the proxy problem.
A math answer checked against ground truth, a program run against unit tests, a formal proof checked by a proof assistant: these give exact, ungameable, infinitely repeatable reward signals.
Against such a reward you can optimize as hard as you like, explore far from the reference policy, and every bit of reward is real signal rather than proxy exploitation.
This is why the field's 2024 pivot was to RL on verifiable rewards (RLVR): the constraint that had made RLHF cautious simply does not bind in checkable domains.

### 2.2 The recipe and what it produced

The recipe, in its publicly documented form: take a strong base or lightly post-trained model; collect problems with checkable answers (competition math, programming tasks with tests); sample long CoT attempts; reward correct final answers (plus minor format terms); update with a policy-gradient method; repeat at scale.
No process supervision on individual steps is required: outcome reward alone, at scale, teaches the model to produce long chains containing verification, backtracking, and decomposition, because those behaviors are what make final answers correct more often.

OpenAI's o1 (September 2024) was the first production demonstration: trained via RL to think in a long private chain before answering, with reported large gains on competition math, coding, and PhD-level science benchmarks, and with the explicit finding that accuracy scales with both train-time RL compute and test-time thinking compute.
DeepSeek-R1 (January 2025) mattered for different reasons: it was open-weight, its paper disclosed the method, and its R1-Zero ablation showed reasoning behaviors emerging from pure RL on a base model with rule-based rewards and no SFT stage at all, including spontaneous chain lengthening and self-correction.
The published R1 pipeline interleaved a small cold-start SFT (to fix readability and language mixing that pure RL produced), large-scale reasoning RL with GRPO, rejection-sampling SFT for general tasks, and a final RL round; distilled variants showed that reasoning traces transfer to small models through plain SFT.
By late 2025 every major provider shipped a reasoning line or mode (o-series and GPT-5 thinking modes, Claude extended thinking, Gemini thinking models, Qwen and DeepSeek open models), making "reasoning model" a tier, not a novelty.

### 2.3 The generalization caveat

RLVR trains on domains with verifiers, and the honest open question, still live as of early 2026, is how far the gains transfer beyond them.
Measured transfer to adjacent reasoning (science QA, some agentic tasks) is real; the pattern of strongest gains staying near math and code is also real.
For agent builders the practical reading: expect reasoning-model advantages to be largest where your task resembles verification-friendly structure (debugging against failing tests, constraint satisfaction, planning with hard requirements) and smallest on soft judgment tasks where no verifier ever existed.

## 3. GRPO at a high level

PPO for RLHF carries four models (Chapter 05), and the value network, which must estimate expected future reward from every prefix, is the expensive and unstable one.
GRPO (Group Relative Policy Optimization, introduced in DeepSeekMath, 2024, and used for R1) deletes it.

The move: for each prompt, sample a group of G responses (G on the order of tens) from the current policy, score each with the verifier, and define each response's advantage as its standardized score within the group:

```
A_i = (r_i - mean(r_1..r_G)) / std(r_1..r_G)
```

The group mean serves as the baseline that the value network used to provide: "was this attempt better than the policy's typical attempt on this same prompt".
The advantage is applied uniformly across the response's tokens, the update reuses PPO's clipped-ratio surrogate, and a KL term to a reference model is retained.
What this buys: one fewer large network in memory, no value-function fitting instability, and a natural fit to verifiable rewards, where sampling many attempts per prompt is cheap and informative.
What it costs: G forward generations per prompt per update (compute shifted from the critic to sampling), per-token credit assignment is coarser than GAE (every token in a failed attempt is punished equally, including its correct prefix), and the group baseline is noisy for small G or for prompts where all attempts fail or all succeed (zero gradient signal in the all-same case, which is why problem-difficulty curation matters in these pipelines).
Successors refining these weaknesses (DAPO, GSPO, and other 2025 variants adjusting clipping, length handling, and sequence-level ratios) exist; the group-baseline idea is the durable core, and Volume 14 returns to the algorithm family in depth.

## 4. Test-time compute scaling

The classical lever for accuracy was train-time scale (Chapter 04); reasoning models made inference-time compute a second, exchangeable lever.
The forms, in increasing structure:

- Longer chains: let the model think more before answering; o1-style results showed log-linear accuracy gains in thinking tokens on hard math within the useful range.
- Parallel sampling with majority vote (self-consistency): embarrassingly parallel, needs an extractable final answer, returns diminish as samples correlate.
- Best-of-N with a verifier or reward model: sample N, keep the best-scored; quality is capped by the selector's judgment, and with a perfect verifier (code against tests) it is a pure accuracy-for-compute pump.
- Search over steps (beam or tree search guided by process reward models scoring intermediate steps): the research frontier's version, powerful in principle, costly and less used in production as of early 2026.

Two results frame the economics.
Snell et al. (2024) showed that on many tasks, optimally allocated test-time compute with a smaller model beats a much larger model at equal total FLOPs, establishing genuine exchangeability between parameters and thinking within a task-dependent regime.
The regime matters: test-time compute rescues problems near the model's capability frontier, and no amount of sampling rescues problems far beyond it; the capability prior (Chapter 04) still sets the ceiling.
For production, the levers surface as thinking-budget or effort parameters in APIs, making "how hard should the model think" a per-request engineering decision with a direct token bill, which is exactly the framing Section 6 uses.

## 5. Reasoning in the Claude line and in provider APIs

This section is provider-specific and stated as of early 2026; API shapes will drift, the concepts less so.

Anthropic shipped extended thinking with Claude 3.7 Sonnet (February 2025): a single model with a controllable thinking budget, where the API accepts a maximum thinking-token allowance and the model emits thinking blocks before its answer.
Two design points are worth noting as engineering, not marketing.
First, one model spans the spectrum from instant answers to long deliberation via the budget knob, rather than forcing a model switch; the budget is a ceiling, not a target, so simple queries can still return quickly.
Second, with Claude 4 (May 2025) came interleaved thinking: the model can think between tool calls, reflecting on a tool result before choosing the next action, rather than only thinking once up front.
For agents this is the significant variant: an agent loop's hard decisions occur mid-trajectory (interpreting an unexpected tool result, revising a plan after a failure), exactly where interleaved thinking inserts deliberation.
Mechanically, thinking tokens are generated and billed as output tokens but are separated from the visible answer; providers variously hide, summarize, or expose raw thinking, and OpenAI's o-series similarly bills hidden reasoning tokens as output.
Two operational consequences follow: cost and latency now depend on a budget you set per request, and context management must account for thinking blocks (in the Anthropic API, prior-turn thinking is generally stripped from subsequent context rather than accumulated, with preserved-thinking variants for tool-use continuity; check current documentation before relying on details).
A caution that belongs in your threat model: visible thinking is not guaranteed faithful (Section 1), and Anthropic's own published evaluations (2025) measured meaningful unfaithfulness rates; treat exposed reasoning as a debugging aid and a UX artifact, not as ground truth about the model's computation, and never as a security boundary.

## 6. When reasoning helps agents and when it wastes tokens

Reasoning is a metered resource with a real exchange rate: thinking tokens are output-priced (the expensive class, Chapter 03), they add latency before the first visible token, and in an agent loop they recur at every step.
The engineering question is never "is the reasoning model better" but "is marginal thinking worth its marginal cost at this decision point".

Where reasoning models earn their cost:

- Planning and decomposition at the head of a complex task, where an error propagates through every subsequent step; one good plan amortizes its thinking cost across the trajectory.
- Debugging and root-cause analysis, which are search problems over hypotheses with verification available (rerun the test), the closest agentic analog to RLVR training conditions.
- Constraint-heavy generation: schema migrations, dependency resolution, scheduling, anything where many requirements must hold simultaneously and single-pass generation reliably drops one.
- Recovery points: an unexpected tool failure or contradictory evidence mid-trajectory, where interleaved thinking can prevent the cheap-but-wrong reflexive next action.
- Verification of high-stakes irreversible actions before commitment, where the asymmetry between token cost and blast radius is extreme.

Where reasoning wastes tokens:

- Extraction, classification, reformatting, and templated generation: the capability prior already contains the answer, and thinking adds latency, cost, and occasionally overthought errors; documented overthinking includes models second-guessing initially correct answers on easy problems.
- High-frequency simple decisions inside loops: an agent choosing among three obvious tool calls hundreds of times per session must not deliberate at each one; this multiplies cost by an integer factor for near-zero accuracy gain.
- Latency-bound surfaces: interactive autocomplete, voice, and real-time UX cannot absorb a thinking pause regardless of quality gains.
- Tasks beyond the capability frontier: thinking longer does not substitute for missing knowledge or missing tools; the fix is retrieval, tools, or a stronger base, not a bigger budget.

The design pattern that follows, and that Volumes 04 and 12 operationalize: route by difficulty and stakes.
Use a fast tier for the loop's routine steps, escalate to reasoning (or raise the thinking budget) at planning, debugging, and recovery points, and cap budgets so a stuck model fails fast instead of thinking in circles.
Measure rather than assume: log thinking-token spend per step against step outcomes, and let the data tell you which decision points repay deliberation; teams that skip this measurement routinely discover that a majority of their reasoning spend was purchased at steps where the fast model was already right.

## Exercises

1. Construct a task that defeats single-pass generation but yields to CoT by design: multi-digit multiplication is the classic; measure a current model's accuracy at temperature 0 with direct answering versus step-by-step on 20 problems, and explain the gap using the serial-depth argument of Section 1.
2. Implement self-consistency: sample 16 chains at temperature 0.8 for 10 competition-style math problems (drawn from a public set like MATH), majority-vote the final answers, and plot accuracy versus sample count at 1, 4, 8, 16; identify one problem where the majority is confidently wrong and diagnose why voting cannot fix it.
3. Implement the GRPO advantage computation: given a group of scored samples, compute standardized advantages, and demonstrate the two degenerate cases (all-correct and all-wrong groups yield zero learning signal); write one paragraph on what this implies for training-problem difficulty curation.
4. Build best-of-N with a perfect verifier: have a model write a function against 5 hidden unit tests, sample N attempts at temperature 0.8, and plot pass rate versus N at 1, 2, 4, 8, 16; then repeat with an LLM judge selecting the best attempt instead of the tests, and quantify the gap between perfect and learned verification.
5. Using any provider with a thinking-budget control (Anthropic extended thinking, current as of early 2026), run the same 10-problem set at three budgets (minimal, moderate, high); tabulate accuracy, total thinking tokens, and wall-clock latency, and identify the budget past which accuracy plateaued for your set.
6. Take a real multi-step agent trace (any coding-agent session you can export) and annotate each step as routine or hard; estimate the token cost of running the whole trace on a reasoning tier versus routing only the hard steps to it, and state the ratio.
7. Design the faithfulness probe: construct three prompts with an embedded bias (a hint toward a wrong answer), inspect whether the model's visible reasoning acknowledges the hint it demonstrably used, and relate your findings to why reasoning traces must not serve as a security or audit boundary.

## Godhood check

You are ready for Chapter 07 when you can do all of the following without notes.

- Explain CoT mechanistically as extending serial computation through the context, and why fixed per-token compute makes some problems unanswerable in one step.
- State the three limits of prompted CoT (imitation not optimization, faithfulness, no error recovery) and which of them RLVR training visibly fixed.
- Explain why verifiable rewards remove the Goodhart ceiling that constrains preference RL, and reconstruct the o1/R1 recipe from problem collection to policy update.
- Describe R1-Zero's significance in one sentence and the role of each stage in the published R1 pipeline.
- Write the GRPO advantage formula, state what it replaces from PPO, and give both the cost it saves and the two weaknesses it introduces.
- Name the four forms of test-time compute scaling in increasing structure, and state the regime in which test-time compute substitutes for model scale and the regime in which it cannot.
- Describe extended and interleaved thinking operationally: what the budget parameter does, how thinking tokens are billed, why interleaving matters specifically for agent loops, and why visible thinking is not a faithfulness guarantee.
- Produce, for a described agent workload, a defensible routing policy for when to invoke reasoning and at what budget, with the cost arithmetic to justify it.
