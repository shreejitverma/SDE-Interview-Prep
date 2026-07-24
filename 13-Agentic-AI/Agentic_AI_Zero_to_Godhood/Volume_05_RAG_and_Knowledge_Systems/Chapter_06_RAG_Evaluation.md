# Chapter 06 - RAG Evaluation

## What you will master

- Why RAG must be evaluated as two coupled subsystems, retrieval and generation, with separate metrics for each.
- Retrieval metrics in real detail: recall@k, precision@k, MRR, and nDCG, including when each is the right headline number.
- Generation metrics: faithfulness, answer relevance, and their relationship to correctness.
- LLM-as-judge evaluation in the RAGAS style: what it measures, how it breaks, and how to calibrate it.
- Building golden datasets that stay useful: sourcing questions, labeling evidence, and maintaining them as the corpus drifts.
- A systematic debugging procedure that localizes any bad answer to retrieval miss, ranking miss, or synthesis miss.

## 1. Why RAG evaluation is a two-subsystem problem

A RAG answer is the composition of two functions: retrieve(query) produces context, and generate(query, context) produces the answer.
End-to-end answer quality confounds them: a wrong answer can come from missing evidence, badly ranked evidence, or a model that ignored good evidence.
These failure modes have different owners and different fixes - chunking and index parameters versus fusion and reranking versus prompts and model choice - so a single end-to-end score cannot tell you what to change.
The entire discipline of this chapter reduces to one rule: measure retrieval against labeled evidence and generation against retrieved context, separately, and only then look at end-to-end correctness.

A second framing rule: retrieval quality upper-bounds answer quality.
If the evidence is not in the context, no prompt engineering will produce a grounded correct answer; the generator can only get it right by parametric luck, which is worse than failure because it passes silently and unreproducibly.
This is why retrieval metrics come first, both in this chapter and in any debugging session.

## 2. Retrieval metrics

All retrieval metrics assume a query set with relevance labels: for each query, which chunks (or documents) count as relevant evidence.
Section 4 covers how to get those labels; here we assume them.

### 2.1 Recall@k

Recall@k is the fraction of relevant items that appear in the top k results, averaged over queries.
For RAG it is the primary first-stage metric, because the generator sees exactly k items: evidence outside the top k does not exist as far as the answer is concerned.
Choose k to match what the generator actually receives (recall@10 if you pass 10 chunks), and additionally track recall@100 for the pre-rerank candidate stage, since the reranker can only promote what stage one surfaced.
A useful simplification for QA-style datasets with one evidence chunk per query is hit rate@k: the fraction of queries whose single gold chunk appears in the top k.
Downside of recall: it ignores ranking within the top k and says nothing about how much junk rides along.

### 2.2 Precision@k and why it matters less than you expect, but not zero

Precision@k is the fraction of the top k that is relevant.
Long-context models tolerate irrelevant passages better than short-context models did, which demoted precision from a primary metric.
It still matters at the margin: irrelevant context costs tokens and money, can distract the model toward plausible-but-wrong evidence (measured distraction effects are real, especially for near-topic hard negatives), and dilutes citation quality.
Track it as a secondary metric; alarm on sharp drops rather than optimizing it directly.

### 2.3 MRR: mean reciprocal rank

For each query, take the rank of the first relevant result and score 1/rank; average over queries.
MRR@10 of 0.5 means the first relevant hit sits at rank 2 on average.
MRR is the right metric when one good hit suffices and position matters - "find the policy document" - and it is sensitive exactly where users and generators are sensitive, at the top ranks.
Its blind spot: it only sees the first relevant item, so it cannot distinguish a system that finds one evidence chunk from one that finds all five, which makes it wrong for multi-evidence questions.

### 2.4 nDCG: normalized discounted cumulative gain

DCG@k sums each result's graded relevance discounted by log2(rank + 1), so relevant items high in the list contribute more; nDCG divides by the ideal DCG (the score of the perfect ordering) to normalize to [0, 1].
Two properties earn nDCG its status as the standard ranking metric: it handles graded relevance (a chunk can be partially relevant, label 1 of 3, rather than binary), and it rewards putting the most relevant items highest, which is precisely what a reranker is for.
Use nDCG@10 as the headline metric for the post-rerank list; a reranker that raises nDCG@10 without raising recall@10 is doing its job of reordering, and one that raises neither is not paying for its latency.
Downside: graded labels cost more to collect and judge consistency on grades is harder than on binary relevance.

### 2.5 Reading the metrics together

The stage-wise pattern to internalize: recall@100 measures candidate generation, recall@k and nDCG@k measure what the generator receives, and the deltas between stages localize problems.
High recall@100 with low recall@10 indicts fusion or reranking.
Low recall@100 indicts chunking, embeddings, query transformation, or the index itself, and no downstream stage can fix it.

## 3. Generation metrics

Given retrieved context, the generator can fail in ways retrieval metrics never see.
The two core measurements are faithfulness and answer relevance, and they are deliberately defined relative to different references.

Faithfulness (also called groundedness): is every factual claim in the answer supported by the retrieved context.
The canonical measurement decomposes the answer into atomic claims, then checks each claim for support in the context, scoring the supported fraction.
Faithfulness is measured against the context, not against the truth: an answer can be faithful to retrieved-but-wrong context, which is a retrieval or corpus problem, not a generation problem.
This separation is the point - it tells you which subsystem to fix.

Answer relevance: does the answer actually address the question asked, regardless of truth.
A model that answers a related-but-different question, or pads with accurate-but-beside-the-point material, fails relevance while passing faithfulness.
One implementation (used by RAGAS) reverse-generates questions from the answer and measures embedding similarity to the original question; simpler implementations directly ask a judge model to rate on a rubric.

Correctness: does the answer match a gold reference answer.
This is the metric everyone wants and the most expensive to have, because it needs reference answers, and free-text comparison needs either humans or a judge model.
The classical automatic metrics (exact match, token F1, ROUGE, BLEU) correlate poorly with correctness for long-form answers and survive mainly in short-answer QA benchmarks; as of early 2026 the working standard for long-form correctness is LLM-judged comparison against a reference, with human audit.

Two auxiliary measurements complete the picture.
Context relevance judges whether the retrieved chunks were pertinent to the question, an LLM-judged proxy for precision that needs no labels.
Citation accuracy checks that each cited source actually supports the sentence citing it, which matters wherever citations are a product feature; attribution benchmarks and judge prompts for this are standard.

## 4. LLM-as-judge and the RAGAS style

RAGAS (2023) popularized a specific move: compute the RAG metric suite - faithfulness, answer relevance, context relevance, and later context recall against a reference - using an LLM as the annotator, so evaluation needs no human labels per run.
The mechanics are worth knowing beyond the library: each metric is a structured prompt chain (extract claims, verify claims against context, output verdicts) executed by a judge model, and the framework aggregates verdicts into scores.
TruLens's "RAG triad" (context relevance, groundedness, answer relevance) is the same idea with different packaging; DeepEval, Phoenix, and most observability platforms ship equivalent judge metrics as of early 2026.

What judges are good for: scaling evaluation across thousands of examples, catching regressions in CI, and triaging which examples deserve human eyes.
How judges break, concretely.
Judge models exhibit position bias (preferring the first-presented candidate in pairwise setups), verbosity bias (longer answers score higher), and self-preference (favoring outputs from their own model family) - all documented in the LLM-as-judge literature from 2023 onward.
Claim extraction is lossy: subtle implications and numeric reasoning errors slip through claim decomposition.
Judge scores drift when the judge model version changes, so pin judge model versions and re-baseline when you upgrade.
And judge metrics are themselves uncalibrated: a faithfulness of 0.9 means nothing until you have checked what human reviewers say about a sample of answers the judge scored 0.9.

The calibration discipline that makes judges trustworthy: sample 50-100 judge verdicts, have humans label the same examples blind, compute agreement (Cohen's kappa or simple accuracy against humans), and iterate on the judge prompt until agreement is acceptable for your stakes; re-audit periodically and after any judge or prompt change.
Treat the judge as a measurement instrument that requires calibration against ground truth, not as ground truth itself.

## 5. Building golden datasets

Automated metrics are only as good as the dataset under them, and dataset construction is where most evaluation programs succeed or fail.

Sourcing questions.
The best source is production: real user queries, sampled across intents and difficulty, deduplicated and anonymized.
Before launch, mine proxies: support tickets, FAQ pages, search logs from the old system, and questions domain experts actually get asked.
Synthetic generation - prompting an LLM with a chunk to produce questions answerable from it - scales cheaply and is built into RAGAS-style tooling, but it has a systematic bias: synthetic questions are phrased close to the source text, overestimating retrieval quality relative to real users who do not know the document's vocabulary.
Use synthetic data to bootstrap and stress-test coverage, then replace it with production queries as they accumulate, and never report synthetic-only numbers as if they predicted production.

Labeling evidence and answers.
For each question record: the gold evidence (chunk or passage identifiers that suffice to answer), a reference answer written or verified by someone who knows the domain, and metadata (intent class, difficulty, required-evidence count, source documents).
Include the hard classes deliberately: multi-evidence questions, questions whose answer is "the corpus does not contain this" (unanswerable questions are essential for measuring abstention and are almost always forgotten), time-sensitive questions, and near-miss distractor cases where a plausible wrong document exists.
Size guidance from practice: even 50-100 well-labeled examples beat zero and catch gross regressions; a few hundred stratified examples support real comparisons; thousands are needed only when chasing small deltas.

Maintenance, the neglected half.
Golden datasets rot in three ways: the corpus changes (gold evidence chunks are deleted or re-chunked, so pin evidence by content, not by chunk id, and re-resolve on re-index), the product changes (new intents appear that the set does not cover), and the team overfits to the set (repeated tuning against the same examples).
Countermeasures: version the dataset alongside the pipeline config (Chapter 02's versioning discipline), add a rolling sample of fresh production queries each cycle, hold out a slice that is never used for tuning, and review label quality when metrics move suspiciously.

## 6. Debugging RAG failures systematically

The point of the whole apparatus is fast, correct blame assignment when an answer is bad.
The procedure below turns "the bot said something wrong" into a specific subsystem fix, and it should be runnable per-example from your logs.

Prerequisite: log everything per request - raw query, rewritten query, per-arm candidates with scores, fused list, reranked list, final context with token counts, prompt, and answer.
Without stage-level logs the procedure is impossible, which is the operational argument for the staged pipeline of Chapter 05.

Step 1: is the evidence in the corpus at all.
Search manually (grep, direct lookup) for the fact.
If absent, this is a corpus or ingestion gap - fix ingestion coverage or parsing (Chapter 02), not retrieval parameters.
If present in the source but mangled in the index, it is a parsing or chunking defect.

Step 2: did stage-one retrieval surface it.
Check the candidate lists for the gold evidence.
If absent from both arms, classify further: query-side (would the rewritten or expanded query match; test by querying with the document's own phrasing), embedding-side (does the chunk embed far from any reasonable query; inspect nearest neighbors of the chunk), or chunking-side (is the fact split across a boundary, interpretable only with missing context).
This is the retrieval miss, and its fixes live in Chapters 02, 03, and 05: contextual enrichment, hybrid arms, chunk boundaries, query rewriting.

Step 3: did it survive fusion and reranking into the final context.
If the evidence was in candidates but not in the final top k, it is a ranking miss: fusion starved it or the reranker demoted it.
Inspect the reranker's score for the gold pair; a systematically low score on your domain indicates reranker mismatch (Chapter 05).

Step 4: with the evidence in context, did the model answer correctly and faithfully.
If the answer contradicts or ignores in-context evidence, it is a synthesis miss: the generation subsystem failed despite correct inputs.
Sub-diagnose: position effects (evidence buried mid-context), conflict with parametric priors (Chapter 01's knowledge-conflict behavior), prompt defects (instructions that do not demand grounding or permit abstention), or plain model capability, testable by swapping models on the frozen context.
Fixes: context ordering, grounding instructions with required citations, abstention instructions, model upgrade - and note these fixes are cheap to A/B because the retrieval side is frozen during the experiment.

Aggregate the per-example verdicts into a failure taxonomy dashboard: percent corpus gap, percent retrieval miss, percent ranking miss, percent synthesis miss.
This distribution is the single most decision-relevant artifact your evaluation program produces, because it tells you where the next engineering week goes, and teams that skip it reliably tune the stage that was not broken.

## 7. Evaluation in CI and production

Offline evaluation on the golden set runs in CI: every change to chunking, embeddings, prompts, or models runs the suite, with regression gates on recall@k, nDCG@10, faithfulness, and correctness-by-judge.
Two practical notes: judge-based metrics have run-to-run variance, so gate on deltas beyond noise (measure the noise by re-running the judge on identical outputs), and keep the suite fast enough that engineers actually run it (a few hundred examples with cached retrieval baselines).
Online, production monitoring closes the loop: sample live traffic into the judge pipeline asynchronously, track faithfulness and context-relevance trends, alert on drift, and route judge-flagged failures into the debugging procedure of Section 6, with the best of them graduating into the golden set.
User signals (thumbs, reformulation rate, escalation to human support) are noisy but free; treat them as triggers for judge evaluation rather than metrics in themselves.

## Exercises

1. Build a golden set of 60 questions over a corpus you control: 40 from real or realistic user queries, 10 multi-evidence, 5 unanswerable, 5 with deliberate near-miss distractor documents.
   Label gold evidence by content and write reference answers.
2. Instrument a pipeline from Chapter 05 to log every stage, then compute recall@100, recall@10, MRR@10, and nDCG@10 on your golden set.
   Change one variable (chunk size, or reranker on/off) and report which metrics moved and why.
3. Implement faithfulness scoring yourself: claim extraction prompt, per-claim verification prompt, aggregation.
   Run it on 30 answers, then hand-label the same 30 and report your judge's agreement rate; revise the prompts once and report the change.
4. Take 20 bad answers (real or induced by degrading the pipeline) and run the four-step debugging procedure on each.
   Produce the failure-taxonomy distribution and a one-paragraph engineering recommendation based on it.
5. Quantify judge noise: run your faithfulness judge five times on the same 30 answers and report per-example score variance.
   Set a CI regression threshold that this noise cannot trip.
6. Demonstrate the synthetic-question bias: generate 20 synthetic questions from chunks, measure recall@10, and compare against your 40 realistic questions on the same pipeline.
   Explain the gap in terms of vocabulary overlap.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Explain why end-to-end accuracy alone cannot localize RAG failures, and name the two subsystems with their metric families.
- Define recall@k, MRR, and nDCG precisely, compute them by hand on a small example, and choose the right headline metric for a given product.
- Define faithfulness and answer relevance, state what reference each is measured against, and explain why an answer can be faithful yet wrong.
- Describe how RAGAS-style judge metrics work mechanically, list three documented judge biases, and give the calibration procedure that makes judge scores trustworthy.
- Design a golden dataset including the four hard classes most teams forget, and name the three ways such datasets rot.
- Run the four-step blame-assignment procedure on a bad answer from logs, and explain what the aggregate failure taxonomy buys an engineering team.
