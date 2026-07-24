# Chapter 05 - Post-Training

## What you will master

- The post-training pipeline end to end: SFT, reward modeling, RLHF with PPO explained at the level of the actual update, and the failure modes of each stage.
- Chat templates and special tokens: the concrete serialization layer where "messages" become tokens, and why template bugs silently ruin behavior.
- DPO derived from the RLHF objective, its main variants, and the honest state of the DPO-versus-PPO debate.
- RLAIF and Constitutional AI: replacing human preference labels with AI feedback under explicit principles.
- Why post-training, not pretraining, defines agent behavior: tool-call syntax, refusals, persistence, and format discipline all live here.

## 1. From document continuer to assistant

Chapter 04 ended with the gap: a base model continues documents, and an assistant is not a document.
Post-training closes the gap by changing the model's distribution from "text like the corpus" to "responses a rater would prefer".
The canonical pipeline, established by InstructGPT (Ouyang et al., 2022) and still the backbone as of early 2026, has three stages: supervised fine-tuning on demonstrations, reward-model training on comparisons, and reinforcement learning against the reward model.
Modern recipes add or substitute pieces (DPO, RLAIF, verifiable-reward RL from Chapter 06), but you should understand the classic pipeline first because everything else is defined relative to it.
A framing worth internalizing: post-training uses a tiny fraction of pretraining compute and data (typically well under a few percent), yet determines nearly everything a user experiences; it is behavior selection from the pretrained prior, not new capability creation, and the best evidence is how small the budget is relative to the behavioral change.

## 2. Supervised fine-tuning

SFT continues next-token training, but on curated (prompt, ideal response) pairs formatted as conversations, with the loss usually masked to response tokens only, so the model learns to produce answers rather than to imitate questions.
Data sources historically progressed from contractor-written demonstrations (InstructGPT) through user-conversation curation to today's heavily synthetic pipelines where a strong model drafts responses that humans or verifier systems filter; distillation of a frontier model into a smaller one via SFT on its outputs is the standard way small model tiers are made.
Quality dominates quantity: LIMA (Meta, 2023) showed about a thousand excellent demonstrations produce most of instruction-following style, supporting the view that SFT mainly teaches format and persona while capability comes from pretraining.
SFT's structural limits motivate everything after it.
It only expresses "imitate this", never "this response is better than that one", so it cannot teach fine-grained preferences or calibrated refusal boundaries.
It suffers exposure bias: trained only on ideal continuations of ideal prefixes, the model never learns to recover from its own mistakes.
And imitation of demonstrations that exceed or lag the model's own knowledge teaches, respectively, confident guessing (a direct cause of hallucination) or capability sandbagging; matching SFT targets to what the model actually knows is a real and studied art.

## 3. Chat templates and special tokens

Between "the API takes a list of messages" and "the model sees tokens" sits the chat template, and agent engineers ignore it at their peril.
A conversation is serialized with special tokens marking roles and turn boundaries; ChatML-style formats (originated at OpenAI, circa 2023) look like:

```
<|im_start|>system
You are a helpful assistant.<|im_end|>
<|im_start|>user
What is RoPE?<|im_end|>
<|im_start|>assistant
```

The model is trained to continue after the assistant header and to emit an end-of-turn token when done; the serving stack stops generation on that token.
Every family has its own template (Llama uses different header tokens, Anthropic and OpenAI templates are internal), and Hugging Face standardized client-side handling via a Jinja template shipped with each model (apply_chat_template, current as of early 2026).
Facts that matter in practice:

- The system prompt is not magic: it is tokens in a privileged position that post-training taught the model to weight heavily; its authority is learned, not architectural, which is why prompt injection (Volume 11) can compete with it at all.
- Template mismatch is a silent killer: fine-tune or evaluate an open model with even slightly wrong role tokens and behavior degrades badly with no error raised anywhere; when an open model "seems broken", checking the template is step one.
- Tool definitions and tool results are also just template sections: function schemas are serialized into the context and tool calls are emitted as structured text between special markers, trained in during post-training; this is the mechanical substrate of everything in Volume 03.
- Stop-token discipline is learned behavior: a model that "won't stop talking" or truncates early is often a template or stop-token configuration bug, not a model-quality issue.

## 4. Reward models

RLHF needs a scalar signal for "how good is this response", and humans cannot score absolutely with any consistency, but they can compare.
So the pipeline collects pairwise preferences: for a prompt x, raters pick between responses y_w (chosen) and y_l (rejected).
The reward model r_phi, typically the SFT model with its unembedding replaced by a scalar head, is trained under the Bradley-Terry model, which posits P(y_w preferred over y_l) = sigmoid(r(y_w) - r(y_l)), giving the loss:

```
L(phi) = -E[ log sigmoid( r_phi(x, y_w) - r_phi(x, y_l) ) ]
```

Only reward differences are identified, not absolute values, which is fine because the RL stage only needs relative signal.
The deep problems with reward models are the deep problems of alignment in miniature.
They are proxies twice over: raters imperfectly represent what we want, and r_phi imperfectly fits the raters, so optimizing r_phi hard exploits the gap (Goodhart's law), producing reward hacking: sycophancy, confident verbosity, and formatting that pleases raters, all documented systematically (for sycophancy, Sharma et al., Anthropic, 2023).
Length bias is the classic concrete case: raters modestly favor longer answers, the RM learns it, RL amplifies it, and output length inflates without quality; length-controlled evaluation exists precisely to correct for this.
Reward models are also out-of-distribution-fragile: RL pushes the policy into regions the RM never saw, where its scores are extrapolation; the standard mitigations are the KL leash (next section) and periodically refreshing the RM with labels on current policy samples.

## 5. RLHF with PPO, properly

### 5.1 The objective

The RL stage maximizes reward while staying close to a reference policy (usually the SFT model):

```
max_theta  E_{x ~ prompts, y ~ pi_theta(.|x)} [ r_phi(x, y) ]  -  beta * KL( pi_theta(.|x) || pi_ref(.|x) )
```

The KL term is not decoration; it is the load-bearing constraint.
It keeps the policy where the reward model is trustworthy, preserves pretraining capabilities against catastrophic drift, and controls the reward-hacking failure where the policy finds degenerate high-reward text.
Beta is the central tuning knob: too small and the model degrades into RM-exploiting gibberish, too large and nothing improves.

### 5.2 Why PPO, and what it actually does

Token generation is an MDP: states are prefixes, actions are tokens, the trajectory is the response, and reward arrives at the end (sequence-level r_phi, with the KL penalty typically distributed per token).
Vanilla policy gradient (REINFORCE) estimates the gradient as E[ grad log pi(y|x) * advantage ], which is unbiased but so high-variance that each batch of expensive samples supports only a tiny step.
PPO (Schulman et al., 2017) makes sample reuse safe: take multiple gradient steps on the same batch under an importance ratio, but clip the ratio so the policy cannot move far on stale data:

```
ratio_t = pi_theta(a_t | s_t) / pi_theta_old(a_t | s_t)
L_clip = E[ min( ratio_t * A_t,  clip(ratio_t, 1 - eps, 1 + eps) * A_t ) ]
```

The clip (eps around 0.2) is a trust region by construction: outside the band, the gradient through the ratio is zero, so the update cannot chase large policy changes on old samples.
The advantage A_t ("how much better was this token than expected") is computed with a learned value function V(s) via generalized advantage estimation (GAE), which interpolates between high-variance Monte Carlo and high-bias one-step estimates.
So a full PPO-for-RLHF setup runs four models: policy (trained), reference (frozen, for KL), reward model (frozen, scores responses), and value function (trained, for advantages).
That is the honest accounting of why RLHF is operationally hard: four models in memory, generation inside the training loop, and a stack of interacting hyperparameters (beta, clip eps, GAE lambda, batch reuse) where instability shows up as sudden KL blowups or reward collapse.
This operational pain is the market force that produced DPO.

### 5.3 What RLHF changes

InstructGPT's result set the pattern: human raters preferred a 1.3B RLHF model over the 175B base model, with improved instruction following and reduced toxicity, at a small fraction of pretraining compute.
Known costs, stable across the literature: an "alignment tax" on some capabilities, degraded probability calibration relative to the base model (GPT-4 technical report, 2023), amplified sycophancy, and mode collapse in the sense of reduced output diversity, the likely mechanistic cause of recognizable "AI style" prose.
RLHF buys preference alignment with the coin of distributional diversity, and both sides of that trade matter for agents.

## 6. DPO and its variants

### 6.1 The derivation that matters

DPO (Rafailov et al., 2023) starts from a known closed form: the KL-constrained reward maximization of Section 5.1 has the analytic optimum pi*(y|x) proportional to pi_ref(y|x) * exp(r(x, y) / beta).
Inverting this expresses reward in terms of the policy itself, r(x, y) = beta * log(pi(y|x) / pi_ref(y|x)) + const, and substituting into the Bradley-Terry likelihood makes the partition constant cancel, yielding a pure supervised loss on preference pairs:

```
L_DPO = -E[ log sigmoid( beta * ( log(pi(y_w|x)/pi_ref(y_w|x)) - log(pi(y_l|x)/pi_ref(y_l|x)) ) ) ]
```

Read it as: raise the likelihood margin of chosen over rejected, measured relative to the reference model, with beta setting the implicit KL strength.
No reward model is trained, no sampling happens in the loop, and the four-model PPO apparatus collapses to two forward passes per pair; that is why DPO swept the open ecosystem within a year (Zephyr, 2023, was the demonstration that made it standard).

### 6.2 Variants, one line each

- IPO (2023): replaces the sigmoid objective to fix DPO's tendency to over-optimize the margin on finite data.
- KTO (2024): needs only per-response good/bad labels rather than pairs, matching data you can actually collect from production thumbs-up/down.
- ORPO (2024): folds preference optimization into SFT with an odds-ratio penalty, removing the separate reference model.
- SimPO (2024): drops the reference model and length-normalizes the implicit reward, targeting DPO's length bias.
The proliferation itself is the signal: offline preference optimization is cheap enough to iterate on weekly, which is exactly what the open community did through 2024-2025.

### 6.3 The honest trade-off versus PPO

DPO is offline: it optimizes on a fixed preference set over (usually) pre-collected responses, so it never explores, never gets feedback on its own current failure modes, and inherits every bias of the pair-collection distribution.
On-policy RL samples from the current policy and grades it, so it can discover and fix behaviors no dataset anticipated, and it generalizes the preference signal through an explicit reward model rather than memorizing pair statistics.
The rough consensus as of early 2026: DPO-family methods are the efficient choice for style, format, and moderate preference shaping, especially in open and resource-constrained settings; frontier labs continue to use on-policy RL (PPO-family, and for reasoning the GRPO-family of Chapter 06) where the last increment of robustness and the ability to train against verifiers matter.
Iterative DPO (re-sampling from the updated policy and re-labeling between rounds) narrows the gap by re-introducing on-policyness at higher pipeline cost.

## 7. RLAIF and Constitutional AI

Human preference labels are the binding constraint of RLHF: slow, expensive, inconsistent between raters, and unable to cover the space RL explores.
RLAIF replaces the human comparison with an AI comparison: a capable judge model picks between responses, the reward model trains on those labels, and the rest of the pipeline is unchanged; Bai et al. (Anthropic, 2022) and later Google work showed AI labels can match human labels in downstream preference quality on many tasks, at a tiny fraction of the cost and with much higher throughput.
Constitutional AI (Anthropic, 2022) is the principled version: the judge does not use raw taste but evaluates against an explicit written constitution of principles.
The pipeline has two phases: a supervised phase where the model critiques and revises its own outputs against constitutional principles and is fine-tuned on the revisions, and an RL phase where preference labels come from constitution-guided AI comparisons.
Why this matters beyond cost: the normative content moves from implicit rater statistics into an inspectable, editable document, which changes alignment from "whatever the raters rewarded" to something you can review and amend; Anthropic published Claude's constitution, and the 2025 evolution of this line is visible in published system-prompt and model-spec practices across labs.
The honest limits: AI feedback inherits and can amplify the judge's own biases, self-evaluation has known blind spots, and a constitution is only as good as its drafting and its interpretation under distribution shift; RLAIF moves the human role up a level of abstraction rather than removing it.
LLM-as-judge evaluation (Volume 10) is this same machinery repurposed for measurement, with the same bias caveats.

## 8. Why post-training defines agent behavior

This section is the reason this chapter is in an agents curriculum and not just an ML one.
Almost every property you engineer around when building agents is a post-training artifact, not a pretraining one.

- Tool calling is trained syntax: the model emits schema-conformant calls between special tokens because post-training included large volumes of tool-use trajectories; reliability differences in function calling between models at the same raw capability are post-training differences.
- Agentic dispositions are trained: persistence on long tasks versus giving up, trying another approach after a failed command, knowing when to stop and ask, resisting the urge to fabricate a tool result; frontier labs explicitly train these behaviors on multi-turn agent trajectories (visible in the Claude 4-era, 2025, emphasis on long-horizon agentic training).
- Refusal boundaries and injection resistance are trained: which instructions the model treats as authoritative (system over user over tool output) is a learned hierarchy, imperfectly learned, and the gap between trained hierarchy and attacker pressure is the entire subject of prompt injection defense.
- Format discipline is trained: staying in JSON, respecting stop conventions, matching requested schemas; when your agent's output parser breaks once per hundred calls, you are experiencing a post-training reliability distribution, and the fix ladder is: constrain decoding, then prompt, then choose a differently-post-trained model.
- Sycophancy and verbosity, the two chronic RLHF artifacts, are directly agent-relevant: a sycophantic model agrees with a wrong plan in a review loop, and a verbose model burns your token budget (Chapter 03 economics) in every iteration.

Two operational corollaries.
First, "the model" you build against is pretraining capability seen through a post-training behavioral filter, so model selection for agents should weight post-training properties (tool reliability, instruction hierarchy, calibrated refusal) at least as heavily as benchmark capability.
Second, when a provider ships a same-family model update, capability moves slowly but the behavioral filter can move a lot, which is why agent regression suites (Volume 10) exist: your system is coupled to trained behaviors that were never guaranteed stable.

## Exercises

1. Implement the Bradley-Terry reward-model loss and train a scalar head on a public preference dataset sample (a few thousand pairs from Anthropic HH or UltraFeedback); report pairwise accuracy, then plot predicted reward against response length and quantify the length bias you find.
2. Take an open chat model and deliberately corrupt its chat template (wrong role token, missing end-of-turn) across five prompts; document the behavioral failures and write the debugging checklist you would now apply to any misbehaving open model.
3. Derive the DPO loss yourself from the KL-constrained objective's closed-form optimum, showing explicitly where the partition function cancels; state which assumption breaks if the preference data does not come from pi_ref.
4. Write the PPO clipped-surrogate update in code for a toy bandit over tokens (no value network, advantage = reward minus batch mean) and demonstrate empirically that removing the clip destabilizes training under multiple gradient steps per batch.
5. Run DPO with a library (TRL, current as of early 2026) on a small open model with 2,000 preference pairs; evaluate before and after on ten held-out instructions with a judge model, and report both the win rate and the change in mean response length.
6. Draft a ten-principle constitution for a coding agent (covering fabrication, destructive commands, secrets, and stopping conditions), then use a frontier model as judge to label ten response pairs under it; note every case where the judge's reading of a principle surprised you.
7. Design the post-training evaluation you would run before adopting a new model for an existing production agent: list five trained-behavior properties from Section 8, and specify a concrete, automatable test for each.

## Godhood check

You are ready for Chapter 06 when you can do all of the following without notes.

- Diagram the InstructGPT pipeline and state what each stage can teach that the previous one cannot.
- Explain SFT's three structural limits: no preference signal, exposure bias, and the knowledge-mismatch route to hallucination.
- Serialize a two-turn conversation in a ChatML-style template from memory and name three production failures caused at the template layer.
- Write the Bradley-Terry RM loss, explain why only reward differences are identified, and give two documented reward-hacking phenomena with their mechanism.
- State the full RLHF objective with the KL term, explain what the KL leash protects against, and walk through the PPO clipped surrogate including why clipping permits safe sample reuse and what the value function is for.
- Derive DPO's key substitution, name the trade-off against on-policy RL (no exploration, dataset bias inheritance), and place IPO, KTO, ORPO, and SimPO in one sentence each.
- Explain Constitutional AI's two phases and what moving norms into an explicit document buys and does not buy.
- Argue, with at least four concrete mechanisms, why agent behavior is chiefly a post-training artifact, and state the two operational corollaries for model selection and model-update regression testing.
