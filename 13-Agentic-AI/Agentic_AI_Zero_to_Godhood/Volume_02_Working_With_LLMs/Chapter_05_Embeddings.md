# Chapter 05: Embeddings

## What you will master

- What an embedding actually is: a learned map from text to a point in a vector space where geometry approximates meaning.
- The contrastive-training intuition that explains why embeddings behave the way they do, including their blind spots.
- Cosine similarity, dot product, and Euclidean distance, and when the choice matters.
- Dimensionality trade-offs and Matryoshka embeddings: how one model serves many sizes.
- Practical uses well beyond RAG: deduplication, clustering, routing, semantic caching, anomaly detection, and evals.
- How to select an embedding model as of early 2026, and the benchmark caveats that keep you honest.

## 1. What an embedding is

An embedding model is a function `f: text -> R^d` mapping a string (a word, a sentence, a document chunk) to a dense vector of `d` floats, typically 256 to 4096 dimensions.
The training objective arranges the space so that semantically similar texts land near each other and dissimilar texts land far apart.
"Near" is measured by a similarity function, almost always cosine similarity in practice.

Three properties define what you can and cannot do with this:

- Embeddings are fixed-size regardless of input length, so they are lossy summaries; a 4,000-token document and a 5-word query both become one point.
  Compression is the feature and the limitation: fine-grained detail within long inputs is averaged away.
- Similarity is task-relative, not absolute.
  "Similar" for a retrieval-trained model means "likely to answer the same query", which differs from stylistic similarity, authorship similarity, or sentiment similarity.
  A model trained for retrieval will happily place "I loved this phone" near "I hated this phone" because both answer "what do reviews say about this phone".
- The space is only meaningful within one model.
  Vectors from different models, or different versions of the same model, are mutually incomparable; a model upgrade means re-embedding your entire corpus, which is a real migration cost to plan for.

Distinguish embedding models from the decoder LLMs of earlier chapters.
Most modern embedding models are encoder-style or adapted decoder architectures fine-tuned for representation, they run one forward pass with no autoregressive loop, and they are orders of magnitude cheaper per input than generation.
As of early 2026, embedding a million tokens costs cents (for example OpenAI's `text-embedding-3-small` listed at 0.02 dollars per million tokens), which is why embedding-heavy architectures are economically attractive.

## 2. Contrastive training intuition

Nearly every strong embedding model since SBERT (Reimers and Gurevych, 2019) and the E5/GTE/BGE lineage is trained with a contrastive objective, and understanding it explains the behavior you observe in production.

The recipe:

1. Assemble pairs of texts that should be close: query and its relevant passage, question and its answer, two paraphrases, title and body.
   Sources include search click logs, mined web pairs, QA datasets, and increasingly LLM-synthesized pairs.
2. For each positive pair, gather negatives: random texts (easy negatives) and, critically, hard negatives that are topically close but wrong (retrieved by an earlier model, then filtered).
3. Train with a loss such as InfoNCE: pull the positive pair's vectors together and push apart the anchor from all negatives in the batch, with temperature scaling.
   In-batch negatives make every other example in a large batch a free negative, which is why embedding training uses very large batch sizes.

What this explains:

- Why hard negatives matter: a model trained only on easy negatives learns topic detection, not relevance; it will retrieve any passage about your topic rather than the one answering your question.
  The quality gap between embedding models is substantially a hard-negative-mining gap.
- Why asymmetric prefixes exist: queries and documents come from different distributions (short and interrogative versus long and declarative), so many models train with instruction prefixes like `query:` and `passage:` (E5) or accept a task instruction string (BGE, Voyage, Cohere).
  Omitting the prefix the model was trained with silently degrades retrieval quality, and this is one of the most common embedding bugs in the wild.
- Why negation and numbers are weak: "contains gluten" and "gluten-free" share almost all tokens and topical context, so contrastive pressure to separate them is weak unless the training pairs specifically target it.
  Benchmarks probing negation and instruction-following (for example the FollowIR and NevIR lines of work) show even strong models fail here; do not rely on embeddings alone for logic-sensitive matching.
- Why out-of-domain performance drops: the geometry is shaped by the training pair distribution; legal, medical, or codebase-specific similarity may not match web-scale similarity, which is the motivation for domain-tuned models (code embedders, finance and legal variants) and for fine-tuning.

## 3. Similarity measures

Given vectors `u` and `v`:

- Cosine similarity: `cos(u, v) = (u . v) / (|u| |v|)`, in [-1, 1]; measures angle only.
- Dot product: `u . v`; angle and magnitude together.
- Euclidean distance: `|u - v|`; for unit-normalized vectors it is a monotone transform of cosine (`|u - v|^2 = 2 - 2 cos(u, v)`), so rankings are identical.

Most provider models ship unit-normalized vectors (OpenAI's do), collapsing the three into one ranking; the practical guidance is to normalize on ingestion and use cosine or dot product interchangeably, letting your vector store use the cheaper dot product.
The choice matters only when magnitude is meaningful: some models encode a confidence-like or length-like signal in magnitude, and maximum-inner-product search intentionally exploits unnormalized dot products (for example in recommendation systems where popularity lives in the norm).

Numbers to internalize so scores do not mislead you:

- Absolute cosine values are model-specific and mean nothing across models; one model's "0.8" is another's "0.55".
  Thresholds must be calibrated per model on your own data, never copied from a blog post.
- Score distributions are typically compressed into a narrow band (often roughly 0.2 to 0.9 rather than the full range), and the useful signal is the ranking and the gaps, not the raw value.
- High-dimensional spaces concentrate: random unrelated texts still score well above zero, so "positive similarity" is not evidence of relatedness; only calibrated thresholds and relative comparisons are.

## 4. Dimensionality and Matryoshka embeddings

Dimensionality trades quality against storage, memory, and search latency.
Vector search cost scales linearly with `d` per comparison, and index memory scales linearly with `d` per vector; at a hundred million vectors, the difference between 3072 and 768 dimensions is the difference between a cluster and a single machine.

Matryoshka Representation Learning (Kusupati et al., 2022) made this trade-off dynamic.
The training objective applies the contrastive loss not only to the full vector but also to its prefixes (first 64, 128, 256, ... dimensions), forcing the most important information into the earliest coordinates, like nested dolls.
A Matryoshka-trained model therefore lets you truncate vectors to any supported prefix length, renormalize, and still get a usable embedding, with quality degrading gracefully rather than catastrophically.

This is now mainstream: OpenAI's `text-embedding-3` family exposes it as a `dimensions` parameter, and Nomic, Voyage, Cohere, and most open-weight leaders train this way as of early 2026.

```python
from openai import OpenAI
import numpy as np

client = OpenAI()

resp = client.embeddings.create(
    model="text-embedding-3-large",
    input=["How do I rotate my API keys?"],
    dimensions=256,   # Matryoshka truncation, server-side
)
v = np.array(resp.data[0].embedding)
print(v.shape)        # (256,)
```

Engineering patterns this unlocks:

- Adaptive retrieval (coarse-to-fine): search the whole corpus with short vectors (fast, small index), then re-rank the top few hundred candidates with full-length vectors or a reranker.
  This typically preserves nearly all quality at a fraction of the search cost.
- One corpus, many budgets: store full vectors once; serve truncated views to memory-constrained deployments without re-embedding.
- Note the interaction with quantization: truncation composes with scalar or binary quantization of each dimension, and the two together (short binary vectors for the first pass) are the basis of most 2025-era cheap-retrieval stacks.
  The trade-off is that each compression step loses recall, and only measurement on your data tells you how much you can afford.

## 5. Practical uses beyond RAG

Retrieval-augmented generation (Volume 05) is the famous use, but embeddings are a general-purpose semantic primitive, and several non-RAG uses have better cost-benefit than most teams realize.

### 5.1 Near-duplicate detection and deduplication

Exact hashing catches identical strings; embeddings catch paraphrases, reformatted copies, and translations.
Pattern: embed every item, index it, and flag pairs above a calibrated similarity threshold for merge or suppression.
Production examples: deduplicating support tickets before triage, collapsing repeated bug reports, cleaning training and eval datasets (near-duplicate leakage between train and test is a classic eval-inflation bug), and news clustering.
Trade-off: thresholding is delicate near the boundary between "duplicate" and "same topic"; a two-stage design (loose embedding threshold, then an LLM judge on candidates) buys precision with a bounded number of expensive calls.

### 5.2 Clustering and corpus cartography

Embed a corpus, run k-means or HDBSCAN, and label each cluster (an LLM summarizing a sample of members makes good labels).
This answers "what is in this data" for support logs, user feedback, agent conversation transcripts, and eval failures.
Clustering agent-failure transcripts by embedding is one of the fastest ways to find systematic failure modes, and it reappears in Volume 10 as an observability technique.
Caveat: cosine-based clustering inherits every bias of the embedding space; clusters reflect what the model considers similar, so inspect before trusting.

### 5.3 Semantic routing and intent classification

A router decides which pipeline, prompt, tool, or model handles an input.
The embedding approach: embed a handful of exemplar utterances per route at build time; at request time, embed the input and pick the route with highest similarity (nearest-centroid or k-NN), with a threshold below which you fall back to an LLM classifier.
Compared with an LLM-call router, this costs microseconds and effectively nothing per request, and it is trivially updatable by editing exemplars; the cost is lower ceiling accuracy on subtle intents and the negation blindness noted above.
A common production compromise: embedding router for the easy 90 percent, LLM router for the uncertain band, measured by the similarity margin.

### 5.4 Semantic caching

Exact-match response caches miss almost always in natural-language systems because users phrase the same question differently.
A semantic cache embeds incoming queries and returns the cached response when a previous query is similar beyond a threshold.
Done well, this cuts both latency and spend on high-traffic assistants with recurring questions.
The risks are serious and must be named: false-positive cache hits serve a wrong answer confidently (tune thresholds conservatively and scope caches per user or per context where answers depend on state), and time-sensitive answers need TTLs.
Semantic caching of tool results and sub-agent results, not just final answers, is the higher-leverage variant inside agent systems.

### 5.5 Anomaly and drift detection

Embed production inputs over time; a drift in the centroid or a rise in distance-to-nearest-training-cluster signals distribution shift (new user intents, a new attack pattern, an upstream product change) before task metrics move.
The same trick applies to outputs: responses that embed far from the historical response distribution are worth sampling for review.
This is cheap, unsupervised, and one of the few early-warning signals available for LLM systems.

### 5.6 Eval support

Embedding similarity between model output and a reference answer is a weak but fast grader: useful as a first-pass filter or a regression tripwire, dangerous as a final judge because surface-similar wrong answers score high (and correct paraphrases can score low).
Use it to cheaply rank which cases deserve LLM-judge or human attention, not to declare success.

## 6. Selecting an embedding model as of early 2026

The landscape (knowledge as of early 2026; verify before committing, this rots fast):

- Hosted proprietary: OpenAI `text-embedding-3-small` and `-3-large` (cheap, solid, Matryoshka `dimensions` parameter); Voyage AI (`voyage-3` family, strong retrieval quality, code- and domain-specific variants; Anthropic's documentation has pointed to Voyage as its recommended embedding partner since Anthropic offers no first-party embedding API, and Voyage was acquired by MongoDB in 2025); Cohere `embed-v4` (multimodal text-plus-image, Matryoshka, int8 and binary output types); Google `gemini-embedding-001` (top MTEB scores at launch in 2025).
- Open-weight: BGE family (BAAI, including `bge-m3` for multilingual and multi-vector), E5 and GTE lineages, Nomic `nomic-embed-text` (fully open with training data), Qwen3-Embedding and other LLM-derived embedders which led MTEB in late 2025, and small classics (`all-MiniLM-L6-v2`) that remain unbeatable per dollar for undemanding tasks.

Decision criteria, in the order that actually matters:

1. Quality on your task: build a small retrieval eval from your own data (50 to 200 queries with labeled relevant documents; an LLM can draft the labels for you to verify) and measure recall@k and nDCG.
   This one afternoon of work dominates every leaderboard consultation.
2. Benchmark priors, used skeptically: MTEB (Massive Text Embedding Benchmark) is the standard leaderboard, but treat it as a shortlist generator, not a verdict.
   Known caveats: many models train on data overlapping MTEB tasks (contamination), rankings compress at the top, and average scores hide per-task variance; MTEB's own maintainers introduced harder multilingual and contamination-aware versions (MMTEB, MTEB v2, 2025) in response.
3. Operational constraints: license and data-residency (open-weight models can run in your VPC; hosted APIs ship your text to a vendor), latency and throughput, max input length (512-token encoders force aggressive chunking; 8k to 32k context embedders like `text-embedding-3`, `bge-m3`, and Voyage models permit larger chunks), and language and modality coverage.
4. Cost at your scale: price per million tokens for embedding is usually negligible next to storage and search cost of the resulting vectors, which is where dimensionality and quantization decisions dominate; do the arithmetic for your corpus size before choosing 3072 dimensions.
5. Migration story: whichever you choose, you will re-embed eventually; keep raw text canonical, treat vectors as a derived cache with the model ID stamped on every record, and design ingestion to support a full rebuild.

Two closing cautions.
First, embeddings versus rerankers: a cross-encoder reranker (Cohere Rerank, Voyage rerank, open-weight `bge-reranker`) reads query and document together and beats any embedding on precision; the standard architecture is embeddings for recall, reranker for the top 50 to 100, and this pairing usually outperforms a better embedding model alone.
Second, embeddings versus LLM judgment: for one-off comparisons of a handful of texts, just asking a cheap LLM is often more accurate than cosine similarity; embeddings win when the comparison count is large (n squared pair checks, corpus-scale search), which is precisely where per-comparison cost dominates.

## Exercises

1. Calibrate a similarity threshold: embed 200 pairs of texts you label as duplicate or distinct (mine them from any public dataset), plot the two score distributions, and pick an operating point from the ROC curve.
   Repeat with a second embedding model and observe how the threshold refuses to transfer.
2. Demonstrate the prefix bug: take an E5-style open model and measure retrieval recall@10 on 50 queries with and without the required `query:` and `passage:` prefixes.
   Report the gap.
3. Build the Matryoshka cost curve: embed a 10k-document corpus at 1536, 512, 256, and 64 dimensions (truncate and renormalize), measure recall@10 against full-dimension results, and plot quality versus index size.
   Then add a two-stage coarse-to-fine search and show where it lands on the same plot.
4. Probe the blind spots: construct 30 negation pairs ("with X" versus "without X") and 30 numeric pairs ("under 50 dollars" versus "over 500 dollars"); measure how often the contradictory pair scores higher than a true paraphrase.
   Write down the implications for a compliance-search product.
5. Build a semantic router: pick 5 intents, 8 exemplars each, implement nearest-centroid routing with a fallback threshold, and evaluate against an LLM-classifier router on 200 held-out utterances.
   Report accuracy, p95 latency, and cost per 1k requests for both.
6. Ship a semantic cache prototype: wrap any LLM endpoint with an embedding cache (FAISS or a vector store), replay a day of realistic traffic with paraphrase variation, and measure hit rate, latency saved, and, most importantly, false-hit rate at your chosen threshold.

## Godhood check

You have mastered this chapter when you can do the following from memory:

- Explain contrastive training (positives, in-batch negatives, hard negatives, InfoNCE) and use it to predict three concrete production behaviors: prefix sensitivity, negation blindness, and out-of-domain degradation.
- State the relationship between cosine, dot product, and Euclidean distance on normalized vectors, and explain why absolute similarity scores are meaningless across models and must be calibrated per model.
- Explain how Matryoshka training reorders information across dimensions, and design a two-stage adaptive retrieval system that exploits it, including where quantization composes.
- Name five non-RAG embedding applications, sketch the architecture of each, and identify the dominant failure mode of each (threshold fragility, cluster bias, false cache hits, and so on).
- Walk through a defensible model-selection process for a new project, including why your own 100-query eval outranks MTEB, what MTEB contamination means, and what the re-embedding migration plan looks like.
- Argue when a cross-encoder reranker or a plain LLM call beats an embedding comparison, and place all three on a cost-versus-precision curve.
