# Chapter 02: Sampling and Decoding

## What you will master

- The exact path from logits to a sampled token, and where every sampling parameter intervenes.
- Temperature, top-k, top-p, and min-p: what each truncates, and when each fails.
- Logprobs: what the API exposes and the practical systems you can build on them (confidence scoring, classification, cheap evals).
- Frequency and presence penalties, repetition pathologies, and why penalties are a blunt instrument.
- Why temperature 0 does not give you determinism, what seeds actually promise, and how to engineer for reproducibility anyway.
- Concrete sampling recipes per task type, with the reasoning behind each.

## 1. From logits to tokens

At each generation step the model's final layer produces one raw score (logit) per vocabulary entry, typically 32k to 200k+ values.
Softmax converts logits to a probability distribution: `p_i = exp(z_i / T) / sum_j exp(z_j / T)`, where `T` is temperature.
The decoder then either picks the argmax (greedy decoding) or samples from a possibly-truncated version of this distribution.

The standard pipeline, in the order most inference engines apply it:

1. Compute logits for the next position.
2. Apply repetition, frequency, and presence penalties (they modify logits based on tokens already generated).
3. Apply logit bias if the caller supplied any.
4. Apply temperature scaling.
5. Apply truncation filters: top-k, then top-p, then min-p (engines differ on exact ordering, and the ordering changes results).
6. Renormalize and sample one token from what survives.
7. Append the token, check stop conditions, repeat.

Everything in this chapter is a knob on steps 2 through 6.
Note one structural fact: sampling operates on a single step's distribution and knows nothing about the future.
Beam search, which does look ahead by keeping multiple candidate sequences, is essentially absent from modern LLM APIs because it costs multiple forward passes and tends to produce degenerate, repetitive text on open-ended tasks (Holtzman et al., "The Curious Case of Neural Text Degeneration", 2019).

## 2. Temperature

Temperature divides logits before softmax.
`T < 1` sharpens the distribution (high-probability tokens gain, tails shrink); `T > 1` flattens it; `T -> 0` approaches greedy decoding; `T = 1` is the model's native distribution.

Intuition that survives contact with practice:

- Temperature does not add knowledge or creativity; it redistributes probability mass the model already assigned.
- Low temperature reduces variance across runs, not error rate on any single run.
  A model that is confidently wrong is confidently wrong at every temperature.
- High temperature disproportionately amplifies the low-quality tail, because the tail contains vastly more tokens than the head.
  This is precisely the failure top-p and min-p exist to contain.

API ranges differ and this bites people: OpenAI accepts 0 to 2 with default 1; Anthropic accepts 0 to 1 with default 1.
A "temperature 1.5" config copied from an OpenAI project is an invalid request on Anthropic.
Also note a major shift: Anthropic removed `temperature`, `top_p`, and `top_k` entirely on its newest reasoning-first models (Opus 4.7 onward, early 2026); sending them returns a 400, and behavioral steering moves to prompting and the effort parameter.
Treat sampling parameters as a capability you must check per model, not a universal contract.

## 3. Top-k

Top-k keeps only the k highest-probability tokens, renormalizes, and samples.
It is a fixed-width truncation: k=40 keeps 40 candidates whether the distribution is flat (where 40 is too few) or peaked (where 40 includes garbage).

That insensitivity to the distribution's shape is its weakness and the reason top-p largely superseded it.
It survives as a belt-and-suspenders cap in open-source stacks (a common local-inference default is k around 40 to 100 combined with top-p) and as a hard bound on worst-case candidate sets.
OpenAI's APIs do not expose top-k at all; Anthropic exposed `top_k` through the 4.6 generation with advice to use it only for advanced cases.

## 4. Top-p (nucleus sampling)

Top-p sorts tokens by probability and keeps the smallest set whose cumulative probability reaches p, then renormalizes and samples.
Introduced by Holtzman et al. (2019) as nucleus sampling, it adapts to distribution shape: a peaked distribution might keep 2 tokens at p=0.9, a flat one might keep 500.

Failure modes you should be able to name:

- With a flat distribution (genuinely open-ended text), p=0.9 still admits hundreds of mediocre candidates; top-p bounds cumulative mass, not candidate quality.
- Top-p interacts with temperature: high temperature flattens the distribution first, pushing more junk inside the nucleus.
  If you raise temperature for diversity, consider lowering p to compensate.
- Setting both temperature and top-p aggressively is the classic incoherence recipe; the standard advice from both vendors is to tune one, not both.
  Anthropic's Claude 4 generation makes this concrete: requests specifying both `temperature` and `top_p` are rejected.

## 5. Min-p

Min-p, popularized in the open-source community around 2023 to 2024 (Nguyen et al., 2024) and now standard in vLLM, llama.cpp, and most local stacks, keeps every token whose probability is at least `min_p` times the probability of the top token.
The threshold scales with model confidence: if the top token has p=0.9, min_p=0.1 keeps only tokens above 0.09; if the top token has p=0.1 (a genuinely open choice), it keeps everything above 0.01.

Why it often beats top-p at high temperature: the cutoff is relative to the head of the distribution rather than to cumulative mass, so flattening the distribution with temperature does not drag hundreds of tail tokens into the candidate set.
The practical recipe from the local-inference community is min_p between 0.05 and 0.1 with temperature 1 or higher for creative work.
The trade-off: min-p is not offered by the major hosted APIs as of early 2026 (OpenAI and Anthropic do not expose it), so recipes built on it are not portable to hosted frontier models.

## 6. Logprobs

OpenAI's APIs can return the log probability of each sampled token, plus the top alternatives at each position:

```python
from openai import OpenAI
import math

client = OpenAI()
resp = client.chat.completions.create(
    model="gpt-4.1",
    messages=[{"role": "user", "content": "Is this review positive or negative? 'The battery died in a day.' Answer with one word."}],
    logprobs=True,
    top_logprobs=5,
    max_completion_tokens=2,
)
for tok in resp.choices[0].logprobs.content:
    print(tok.token, math.exp(tok.logprob))
    for alt in tok.top_logprobs:
        print("   alt:", alt.token, math.exp(alt.logprob))
```

Anthropic's Messages API does not expose logprobs as of early 2026, which is a real capability gap to know about when designing systems below.

What logprobs buy you in practice:

- Classification with calibrated-ish confidence: constrain the answer to one token (or a small label set), read the probability of each label, and threshold.
  `exp(logprob)` of "positive" versus "negative" is a far richer signal than the sampled word alone.
- Selective answering and escalation: route low-confidence outputs to a bigger model or a human.
  Perplexity over the generated answer (mean negative logprob) is a usable, if imperfect, uncertainty proxy.
- Cheap evals and regression detection: track the logprob your model assigns to known-correct answers on a fixed suite; drops flag drift after prompt or model changes without needing a judge model.
- Hallucination heuristics: low-probability spans inside otherwise confident text correlate with fabrication; this is a heuristic, not a guarantee, because models are miscalibrated, especially after RLHF.
- Ranking without generation: score a fixed set of candidate continuations by their token logprobs instead of asking the model to pick, avoiding position bias in the prompt.

Caveats to state honestly: post-training (RLHF/RLVR) damages calibration, so raw probabilities are systematically overconfident; logprobs are per-token, so multi-token labels need summing and length normalization; and top_logprobs is capped (20 on OpenAI), so you cannot reconstruct the full distribution.

## 7. Frequency and presence penalties, and logit bias

OpenAI exposes two additive penalties, both ranging -2 to 2:

- `presence_penalty`: subtracts a constant from the logit of every token that has appeared at least once in the text so far.
  It pressures the model to introduce new tokens (topic diversity).
- `frequency_penalty`: subtracts an amount proportional to how many times a token has appeared.
  It suppresses verbatim repetition loops.

Open-source stacks add `repetition_penalty` (multiplicative, from the CTRL paper) with the same intent.
Anthropic exposes none of these; repetition control on Claude is prompting plus stop sequences.

Why penalties are a blunt instrument, explicitly:

- They operate on token identity, not meaning; penalizing "the" and "return" degrades code and prose grammar long before they stop semantic repetition.
- They cannot distinguish desirable repetition (a variable name that must stay consistent, a JSON key that appears in every element) from degenerate loops.
  A frequency penalty on structured output generation is a bug factory.
- Modern frontier models rarely need them; repetition loops were a small-model pathology.
  Default both to 0 and reach for them only with observed loops, at small magnitudes (0.1 to 0.5).

`logit_bias` (OpenAI) is the surgical alternative: a map from token ID to an additive bias between -100 and 100, where -100 effectively bans a token and +100 nearly forces it.
Classic uses: banning a specific token from ever appearing, or restricting a classifier to exactly the tokens "yes" and "no".
Its limitation is that it works on token IDs, so you must tokenize your target strings and handle the fact that a word may map to multiple tokenizations.

## 8. Why determinism is hard even at temperature 0

Temperature 0 (greedy decoding) picks the argmax at every step, which sounds deterministic.
In practice, repeated identical requests still diverge, for reasons that are worth understanding precisely:

- Floating-point non-associativity: `(a + b) + c != a + (b + c)` in floating point.
  GPU kernels sum in whatever order maximizes throughput, and that order varies with batch composition, kernel selection, and hardware.
  When two candidate tokens have near-equal logits, a 1e-7 wobble flips the argmax, and one flipped token changes the entire continuation.
- Batching non-invariance: your request is batched with strangers' requests on the provider's servers.
  Batch size and the position of your sequence in the batch change kernel tiling and reduction order, so the "same" forward pass computes slightly different numbers.
  This is the dominant cause on hosted APIs and was analyzed in detail by Thinking Machines' "Defeating Nondeterminism in LLM Inference" (2025), which showed batch-invariant kernels can restore true determinism at some throughput cost.
- Mixture-of-experts routing: expert selection can depend on how requests are grouped, adding another batch-dependent source of variance on MoE models.
- Fleet heterogeneity: different GPU generations and different inference-engine builds behind one endpoint produce different numerics.
- Deliberate floors: some providers clamp temperature to a small epsilon rather than honoring exact 0.

OpenAI's `seed` parameter plus the `system_fingerprint` response field are the honest version of this story: seed makes sampling reproducible, but only "best effort", and only when the fingerprint (backend configuration) matches between calls.
A seed cannot fix nondeterminism that originates in the forward pass itself.
Anthropic does not offer a seed parameter and documents that even temperature 0 is not fully deterministic.

Engineering consequences:

- Never build correctness on run-to-run identity of hosted model output.
  Design for semantic equivalence: validate outputs against schemas and assertions, not golden strings.
- For evals, control what you can (temperature 0 or low, fixed prompts, pinned model snapshot IDs, seed where offered) and then measure variance rather than assuming it away; report pass rates over n runs, not a single run.
- If you truly need bit-level reproducibility (research, regulated audit trails), self-host with pinned engine versions, batch size 1 or batch-invariant kernels, and pinned hardware, and accept the throughput cost.
- Cache aggressively: the only fully deterministic LLM call is the one you serve from your own response cache.

## 9. Practical sampling recipes per task type

These are starting points as of early 2026, not laws; every serious deployment should sweep around them on its own evals.
Where a hosted model does not expose a parameter, drop it and rely on the prompt.

| Task | Temperature | Top-p | Other | Reasoning |
| --- | --- | --- | --- | --- |
| Code generation | 0 to 0.3 | leave default | stop sequences for fences | Correctness dominates; variance is pure risk. |
| Structured extraction / JSON | 0 | leave default | constrained decoding if available (Chapter 04) | One canonical right answer; sampling adds only error. |
| Classification / routing | 0 | leave default | logit bias or single-token labels, read logprobs | Determinism plus confidence signal. |
| Tool-calling agent steps | 0 to 0.3 | default | rely on strict schemas | A creative tool call is a broken tool call. |
| Factual Q&A / RAG answers | 0 to 0.3 | default | | Minimize tail sampling that manufactures facts. |
| Summarization | 0.2 to 0.5 | default | | Slight variance improves phrasing without inventing content. |
| Marketing / brainstorming | 0.8 to 1.2 | 0.9 to 0.95 | or min_p 0.05 to 0.1 with T >= 1 locally | You want the distribution's breadth; contain the tail with truncation. |
| Fiction / poetry | 1.0 to 1.3 (OpenAI scale) | 0.95, or min-p locally | small presence penalty if loopy | Maximum diversity, guarded against degeneration. |
| Synthetic data generation | 0.7 to 1.1 varied per batch | varied | vary seeds/prompts too | Deliberately sweep parameters; monoculture data is the failure mode. |
| Self-consistency / majority vote | 0.7 to 1.0 | 0.9 | n samples, aggregate | Diversity across samples is the whole point (Wang et al., 2022). |
| Reasoning models (o-series, Claude with thinking) | not exposed or leave default | not exposed | control via effort/budget instead | Vendors tuned or removed sampling; the lever is reasoning effort. |

Three meta-rules:

- Tune one knob at a time, usually temperature, and hold the rest at defaults; joint sweeps of temperature and top-p mostly rediscover the diagonal.
- The prompt dominates the sampler.
  A weak prompt at a perfect temperature loses to a strong prompt at any reasonable temperature; do not use sampling parameters to compensate for specification failures.
- Log every request's sampling parameters alongside outputs.
  When quality shifts, you need to rule the sampler in or out in minutes, not by archaeology.

## 10. A worked example: confidence-gated classification

This pattern combines several ideas from the chapter: single-token labels, temperature 0, logprobs, and escalation.

```python
from openai import OpenAI
import math

client = OpenAI()

LABELS = {"A": "refund", "B": "shipping", "C": "other"}

def classify(ticket: str) -> tuple[str, float]:
    resp = client.chat.completions.create(
        model="gpt-4.1-mini",
        messages=[
            {"role": "system", "content": (
                "Classify the support ticket. Reply with exactly one letter.\n"
                "A = refund request\nB = shipping issue\nC = anything else"
            )},
            {"role": "user", "content": ticket},
        ],
        temperature=0,
        max_completion_tokens=1,
        logprobs=True,
        top_logprobs=5,
    )
    tok = resp.choices[0].logprobs.content[0]
    probs = {t.token.strip(): math.exp(t.logprob) for t in tok.top_logprobs}
    label = tok.token.strip()
    return LABELS.get(label, "other"), probs.get(label, 0.0)

label, confidence = classify("My package never arrived and tracking is stuck.")
if confidence < 0.85:
    label = "needs_human_review"  # escalate instead of guessing
```

The escalation threshold is not universal; calibrate it on a labeled validation set by plotting accuracy against confidence buckets.
On Anthropic, where logprobs are unavailable, the equivalent pattern is n-sample voting: sample the classification k times at moderate temperature and use agreement rate as the confidence proxy, paying k times the cost.

## Exercises

1. Implement the full sampling pipeline (temperature, top-k, top-p, min-p, in that order) in NumPy over a synthetic 50k-entry logit vector.
   Plot how many candidate tokens survive each filter for a peaked distribution versus a flat one, and show the case where min-p and top-p disagree most.
2. Take one open-ended prompt and one code prompt; run each 20 times at temperatures 0, 0.4, 0.8, and 1.2 against a hosted model.
   Measure distinct-output rate and (for code) test pass rate; write up where variance helped and where it only hurt.
3. Build the confidence-gated classifier above, calibrate its threshold on 100 labeled examples, and report accuracy at 100 percent coverage versus accuracy at the coverage the threshold gives you.
4. Reproduce nondeterminism: send the same prompt at temperature 0 (and fixed seed, on OpenAI) 50 times; measure how many distinct outputs appear and at which token position the first divergence occurs.
   Check whether `system_fingerprint` changed across runs.
5. Implement self-consistency for GSM8K-style math problems: sample 10 chains at temperature 0.8, extract final answers, majority-vote.
   Compare accuracy against a single temperature-0 run and compute the cost multiplier you paid for the gain.
6. Using logit bias on OpenAI, force a model to answer a yes/no question while banning the token "yes" (all its tokenizations).
   Observe and explain what the model does, and what this teaches about token-level versus semantic-level control.

## Godhood check

You have mastered this chapter when you can answer these cold:

- Walk through the logits-to-token pipeline naming where each of the seven parameters in this chapter intervenes, and explain why filter ordering changes outcomes.
- Explain the specific distribution shape where top-p admits garbage that min-p would exclude, and why hosted APIs still do not offer min-p.
- Given a task (say, tool-calling agent steps or synthetic data generation), state a full sampling configuration and defend every value, including which parameters you deliberately left at defaults.
- Give three concrete production systems buildable on logprobs, and explain what breaks in each when the provider is Anthropic.
- Explain, at the level of floating-point reduction order and batching, why two identical temperature-0 requests can differ, what `seed` plus `system_fingerprint` actually guarantee, and what a batch-invariant kernel changes.
- Argue for and against using frequency penalties on a JSON-generation workload, and name the surgical alternative.
