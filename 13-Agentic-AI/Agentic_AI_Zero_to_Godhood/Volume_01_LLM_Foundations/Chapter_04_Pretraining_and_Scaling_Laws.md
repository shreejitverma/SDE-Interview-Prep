# Chapter 04 - Pretraining and Scaling Laws

## What you will master

- The web-scale data pipeline: crawling, extraction, filtering, deduplication, and quality classification, with the major public corpora as landmarks.
- Pretraining objectives beyond plain causal LM, including fill-in-the-middle and why the masked-LM branch lost for generation.
- FLOPs accounting: the 6ND approximation, where it comes from, and how to size training runs on the back of an envelope.
- Kaplan versus Chinchilla scaling laws: what each actually claimed, why they disagreed, and how inference economics changed the optimum again.
- The data wall, synthetic data, curriculum and annealing, and multi-epoch training.
- A precise account of what pretraining gives you and what it structurally cannot, which is the contract every later volume builds on.

## 1. What pretraining is

Pretraining is the phase that turns an initialized transformer into a general model of text: next-token prediction over a corpus of trillions of tokens, run once at enormous cost, producing the "base model".
Everything agents do ultimately draws on capabilities laid down here, and nothing later (post-training, prompting, RAG, tools) adds knowledge at anything like this scale; later phases mostly shape, select, and extend what pretraining built.
As of early 2026, frontier pretraining runs are estimated in the 10^25 to 10^26 FLOPs range on corpora in the low tens of trillions of tokens; exact figures for frontier models are no longer disclosed, so treat these as order-of-magnitude anchors.

## 2. Data pipelines

The unglamorous truth of pretraining is that data engineering dominates outcomes at fixed compute.
The pipeline below is the consensus shape, documented publicly by the RefinedWeb (2023), Dolma (2024), and FineWeb (2024) papers.

### 2.1 Acquisition

The backbone is Common Crawl, a nonprofit web crawl publishing multi-billion-page snapshots since 2008; essentially every documented open corpus starts from it.
Labs supplement with code (GitHub-derived corpora, most famously The Stack from BigCode), academic text (arXiv, papers), books, reference works, and forums, plus licensed and proprietary sources that are not public knowledge.
Legal status of web training data remains contested as of early 2026, with active litigation and a growing licensing market; this book does not track the litigation, but you should know the input supply is not legally settled.

### 2.2 Extraction and language identification

Raw crawl data is WARC files of HTML; extraction strips boilerplate (navigation, ads, cookie banners) to recover the main text, using tools like trafilatura or resiliparse.
Extraction quality is a quietly huge lever: FineWeb attributed a meaningful share of its gains over prior corpora to better extraction alone.
Language identification (fastText classifiers, typically) then routes or drops documents; a threshold here silently sets the model's multilingual profile, connecting directly to the tokenizer economics of Chapter 03.

### 2.3 Quality filtering

Two families, usually combined.
Heuristic filters encode "does this look like real text": document length bounds, symbol-to-word ratios, fraction of lines ending in punctuation, repetition ratios, boilerplate line patterns; the C4 (2019) and Gopher (2021) rule sets are the canonical public examples.
Model-based filters score documents with a trained classifier; the target has evolved from "resembles curated reference text" (GPT-3 era) to "educational value" scored by an LLM-judged classifier, the approach FineWeb-Edu (2024) showed produces strong benchmark gains at fixed compute.
The explicit trade-off: aggressive quality filtering shrinks the corpus and narrows its distribution, risking loss of dialectal, informal, and low-resource text, and encoding the filter designer's notion of "quality" into everything downstream.
Filtering is where a model's implicit values enter earliest, well before any alignment training.

### 2.4 Deduplication

Web data is massively duplicated: mirrors, syndication, templated pages, quotes.
Exact dedup hashes normalized documents; near-dedup uses MinHash over shingles with locality-sensitive hashing to find and drop close variants at billion-document scale.
Why it matters: duplicated data wastes compute on repeated gradient signal, amplifies whatever is duplicated, and drives verbatim memorization, which is simultaneously a privacy, copyright, and eval-contamination problem ("Deduplicating Training Data Makes Language Models Better", Lee et al., 2022).
Contamination checking, removing benchmark test sets from training data, is the same machinery pointed at eval integrity, and its imperfection is why you should always ask "was this benchmark in the training set" when reading capability claims; Volume 10 returns to this.

### 2.5 Mixing

The final corpus is a weighted mixture over sources (web, code, academic, books, multilingual), and the weights are consequential model-design decisions: the code fraction, in particular, is widely credited with affecting not just coding skill but general reasoning, though clean public ablations are scarcer than the folklore is confident.
Mixture weights are typically tuned via small proxy models, extrapolating by the scaling machinery of Section 4.

## 3. Objectives

The dominant objective is exactly Chapter 01's: causal next-token prediction with cross-entropy loss over the mixture.

Fill-in-the-middle (FIM, Bavarian et al., 2022) deserves specific attention because coding agents depend on it.
Plain causal LM only conditions on a left prefix, but real editing needs generation conditioned on both sides of a cursor.
FIM rearranges a fraction of training documents into prefix, suffix, middle order with sentinel tokens, so the model learns to generate a middle given both sides, at essentially no cost to left-to-right quality.
Every serious code model trains with FIM or a variant, and inline completion in IDEs is FIM at inference.

The masked-LM branch (BERT: predict masked-out tokens using both directions) won understanding benchmarks in 2018-2019 but lost the generative race: masking supervises only the masked fraction of positions (typically 15 percent) versus causal LM's every-position signal, and a bidirectional encoder has no natural sequential generation story.
Span corruption (T5) sits in between and survives in encoder-decoder models.
The lasting takeaway: objectives are judged by supervision density and by alignment between the pretraining task and the deployment task, and causal LM wins both for generation.

## 4. FLOPs accounting

You should be able to size a training run on a napkin, and the tool is the 6ND rule.

For a dense transformer with N parameters trained on D tokens, total training compute is approximately:

```
C ~= 6 * N * D   FLOPs
```

Where it comes from: a forward pass through a weight matrix costs about 2 FLOPs per parameter per token (one multiply, one add per weight); the backward pass computes gradients with respect to both activations and weights, costing roughly twice the forward pass; total 2 + 4 = 6 FLOPs per parameter per token.
The rule ignores attention's quadratic score computation, which is a good approximation while context length is much smaller than d_model times a modest factor, and degrades for very long contexts.
For MoE models, N in the compute formula is active parameters per token, not total parameters, which is the entire point of MoE (Chapter 02).

Worked example, arithmetic only.
A 70B dense model on 15T tokens: C ~= 6 * 7e10 * 1.5e13 ~= 6.3e24 FLOPs.
An H100 delivers on the order of 1e15 dense BF16 FLOPs/s peak; at a realistic 40 percent utilization, that is 4e14 FLOPs/s, so about 1.6e10 GPU-seconds, roughly 4.4 million H100-hours, meaning about 26 weeks on a 1000-GPU cluster.
Every number above is public-order-of-magnitude, and the point is the method: parameters and tokens in, wall-clock and dollars out.
Utilization (MFU, model FLOPs utilization) is the honesty term: real training runs at roughly 30-50 percent of peak due to communication, memory-bandwidth limits, and pipeline bubbles, and reported MFU is a primary systems-engineering scorecard.

## 5. Scaling laws

### 5.1 Kaplan 2020

"Scaling Laws for Neural Language Models" (Kaplan et al., OpenAI, 2020) established that loss falls as a power law in each of parameters N, data D, and compute C over many orders of magnitude, smooth and predictable.
Its headline allocation claim: at a fixed compute budget, grow N much faster than D; big models are so sample-efficient that data can lag far behind.
This licensed the GPT-3 shape: 175B parameters on about 300B tokens.
The predictability result, loss versus compute is forecastable, is the deepest legacy: it turned model development into an engineering discipline where small runs calibrate a frontier run, and it is why labs can commit nine-figure budgets to a single training run with justified confidence in the loss it will reach.

### 5.2 Chinchilla 2022

"Training Compute-Optimal Large Language Models" (Hoffmann et al., DeepMind, 2022) redid the allocation study with a crucial methodological fix: Kaplan's sweeps had used a fixed learning-rate schedule length, undertraining the smaller models and biasing the fit toward "parameters matter more".
With schedules tuned per run, the corrected optimum is that N and D should scale together, roughly equally, landing near:

```
D_optimal ~= 20 * N    (tokens per parameter, at compute-optimal)
```

The demonstration: Chinchilla, 70B parameters on 1.4T tokens, outperformed Gopher, 280B on 300B tokens, at the same training compute.
The field-level correction was immediate: models of the GPT-3 era were revealed as badly undertrained on data, and post-2022 frontier models shifted to vastly larger corpora rather than maximal parameter counts.

### 5.3 The inference-aware correction

Chinchilla optimizes loss per unit of training compute, but training happens once and inference happens forever.
A smaller model trained far beyond its Chinchilla-optimal token count reaches nearly the same loss while being permanently cheaper to serve, and once expected inference volume enters the objective, the optimum shifts hard toward small-and-overtrained.
Llama made this the open-model norm: Llama 1/2/3 trained 7B-70B-class models into the trillions of tokens, with Llama 3 8B at around 15T tokens, hundreds of tokens per parameter, wildly "suboptimal" by Chinchilla and exactly right for deployment economics.
For agent builders this is why capable small models keep appearing: the Haiku/mini/flash tier of every provider exists because inference-aware scaling says to buy small-model quality with extra training tokens.
The stable lesson across all three rounds: scaling "laws" are empirical fits under stated assumptions, and each revision came from changing an assumption (schedule tuning, then the objective itself), not from the power-law form failing.

### 5.4 Emergence, briefly

Some capabilities appear abruptly with scale on specific benchmarks (emergent abilities, Wei et al., 2022), while later analysis (Schaeffer et al., 2023) showed many such jumps are artifacts of discontinuous metrics like exact match over smoothly improving underlying likelihoods.
The safe operational posture: loss improves smoothly and predictably; task-level capability thresholds remain genuinely hard to forecast, and you should not bet a product on a capability the current model tier does not demonstrably have.

## 6. The data wall, synthetic data, and multi-epoch training

Frontier corpora in the low tens of trillions of tokens are within an order of magnitude of plausible estimates of usable high-quality public text, a constraint discussed publicly since about 2022 (Villalobos et al., "Will we run out of data?").
As of early 2026 the binding constraint is quality-weighted: raw web tokens are not exhausted, but tokens that improve a frontier model are increasingly scarce.
The responses in play:

- Multi-epoch training: repeating data was long taboo, but Muennighoff et al. (2023) showed roughly up to 4 epochs behaves close to fresh data before returns decay sharply, buying a small multiplier, not an escape.
- Synthetic data: model-generated text, filtered and verified, now a major real input; the Phi series (Microsoft, 2023-2024) built competitive small models substantially on synthetic textbook-style data, and reasoning traces for post-training (Chapters 05-06) are dominantly synthetic.
The known hazard is distribution narrowing and self-feedback ("model collapse" in the recursive limit, Shumailov et al., 2023), which is why verification and grounding in checkable domains (code that runs, math that verifies) is where synthetic data works best; this foreshadows the RL-on-verifiable-rewards story of Chapter 06.
- New modalities and sources: video, audio, and licensed private corpora expand supply but change the distribution rather than extending it neutrally.
- Spending compute elsewhere: if pretraining data saturates, put marginal compute into post-training and test-time reasoning, which is a compact summary of the field's 2024-2025 pivot.

## 7. Curriculum and annealing

Modern pretraining is staged rather than uniform, and two techniques matter.

Learning-rate annealing: the LR follows warmup then a long decay (cosine, or trapezoidal/WSD schedules that hold flat and decay late); the decay phase is where loss drops fastest, and WSD-style schedules let labs branch a single run into multiple decay endpoints cheaply.
Data curriculum and midtraining: the data mixture shifts over training, with the now-standard move being to concentrate the highest-quality data (curated text, textbooks, code, math, sometimes early instruction-format data) in the final annealing phase, where the low learning rate imprints it strongly; Llama 3 and OLMo 2 (2024) both document versions of this.
Long-context extension is also typically staged at the end: most of the run happens at short context (4K-8K) for throughput, then a brief phase at long context with RoPE frequency rescaling (Chapter 02) produces the shipped 128K-1M window.
The trade-off in all staging: order effects create sensitivity to when data is seen, forgetting of early-seen distributions is real, and staging decisions are among the least public, least reproducible parts of frontier recipes.
The boundary between "late pretraining" and "post-training" has genuinely blurred, which is context for Chapter 05.

## 8. What pretraining gives you, and what it cannot

This section is the contract between this volume and everything after it; keep it literally in mind when debugging agents.

What the base model has:

- Broad world knowledge and linguistic competence, compressed from the corpus, with a hard cutoff at the end of its data.
- Transferable representations and in-context learning: shown a pattern in the prompt, it continues the pattern, which is the raw substrate of all prompting.
- Latent skills present in the corpus (translation, code, arithmetic-up-to-tokenization, style imitation) at whatever depth the data and scale purchased.
- Calibrated next-token uncertainty over its distribution, a property post-training partially destroys (documented in the GPT-4 technical report, 2023).

What pretraining structurally cannot give:

- Assistant behavior: a base model continues documents; ask it a question and a plausible continuation may be more questions.
The gap is behavioral, not knowledge-based, and closing it is Chapter 05's subject.
- Truthfulness as an objective: likelihood rewards plausibility under the corpus, including confident falsehoods; hallucination is the objective working as specified on out-of-support queries.
- Knowledge past the cutoff, or private knowledge: retrieval (Volume 05) and tools (Volume 03) exist precisely because pretraining cannot cover them.
- Reliable multi-step goal pursuit: nothing in the objective rewards plan coherence over long horizons; the agent loop and post-training supply the scaffolding.
- Guarantees of any kind: no property of the output (format, safety, factuality) is enforced by pretraining; every guarantee an agent system offers is built downstream, by post-training, constrained decoding, or verification.

The clean mental model: pretraining buys the capability prior; post-training selects and shapes behavior from that prior; scaffolding (tools, retrieval, verification) covers what neither can.
When an agent fails, your first diagnostic question should be which of the three layers the failure belongs to, because the fixes live in different places and different budgets.

## Exercises

1. Reproduce a miniature pipeline: take 200 pages from a Common Crawl WET sample, apply three Gopher-style heuristic filters and MinHash near-dedup (a small Python implementation is fine), and report the survival rate at each stage with two example documents killed by each filter.
2. Derive the 6ND rule from per-matrix-multiply FLOPs counting for one transformer block, state the attention term it drops, and compute the context length at which the dropped term reaches 10 percent of the counted term for d_model = 8192.
3. Napkin-size three runs: compute total FLOPs, H100-hours at 40 percent MFU, and calendar time on 512 GPUs for (a) 8B on 15T tokens, (b) 70B on 15T tokens, (c) 400B on 15T tokens; then state which is Chinchilla-optimal at its compute budget and which you would train for a high-volume serving business, with the rationale.
4. Read the Chinchilla paper's Approach 3 and write half a page on the methodological flaw it identified in Kaplan's sweep and why fixed-schedule undertraining biases the fitted exponents.
5. Using D = 20N, compute the compute-optimal token counts for 8B, 70B, and 400B models, compare to the roughly 15T tokens of Llama 3, and compute the tokens-per-parameter ratio for Llama 3 8B; explain the gap in one paragraph of inference economics.
6. Design an annealing-phase data mix for a hypothetical coding-agent base model: name five source categories, assign percentage weights, state what you upweight in the final 10 percent of training and why, and name the failure mode your mix risks.
7. For each of five recurring agent failures (wrong API hallucinated, stale knowledge, ignores output-format instruction, arithmetic slip, gives up mid-task), assign the failure to pretraining, post-training, or scaffolding, and name the cheapest fix at the correct layer.

## Godhood check

You are ready for Chapter 05 when you can do all of the following without notes.

- Draw the data pipeline end to end (crawl, extract, language-ID, filter, dedup, mix) and give the reason each stage exists plus one trade-off it introduces.
- Explain why deduplication improves models and how the same machinery relates to benchmark contamination.
- State the FIM objective, why it exists, and why masked LM lost the generative race on supervision density and task alignment.
- Derive 6ND, state its blind spot, and size a named training run to GPU-hours within an order of magnitude.
- Give the Kaplan claim, the Chinchilla correction with its methodological cause, the 20 tokens-per-parameter rule, and the inference-aware reason modern models violate it deliberately.
- Explain the quality-weighted data wall and the three main responses (multi-epoch limits, verified synthetic data, compute reallocation to post-training and test time).
- Recite the pretraining contract: four things the base model has, five things it structurally lacks, and the three-layer diagnostic (capability prior, behavior shaping, scaffolding) for locating agent failures.
