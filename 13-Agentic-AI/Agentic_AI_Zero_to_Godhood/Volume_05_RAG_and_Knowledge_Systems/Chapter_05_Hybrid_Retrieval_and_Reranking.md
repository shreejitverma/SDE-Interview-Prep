# Chapter 05 - Hybrid Retrieval and Reranking

## What you will master

- Why lexical search (BM25) still matters in the embedding era, with the specific query classes it wins.
- Reciprocal rank fusion and score-based fusion for combining retrievers.
- Cross-encoder rerankers: what they compute that bi-encoders cannot, and what they cost.
- Late interaction (ColBERT): the architecture between bi-encoders and cross-encoders.
- Query understanding: rewriting, decomposition, expansion, and HyDE.
- How to assemble these into a multi-stage pipeline, with working code and honest latency accounting.

## 1. Why BM25 still matters

BM25 is a probabilistic-relevance ranking function over term frequencies: a document scores highly when it contains the query's terms, with each term weighted by inverse document frequency (rare terms count more), term-frequency saturation (the tenth occurrence adds less than the second), and length normalization (long documents do not win by volume).
It has two free parameters (k1 for saturation, b for length normalization) whose defaults (roughly 1.2 and 0.75) work broadly.
It requires no training, no GPU, and an inverted index makes it fast at any corpus size.

Dense embeddings beat BM25 on paraphrase and concept matching: "how do I make my container smaller" matches a document about reducing Docker image size with zero shared terms.
BM25 beats dense embeddings on exact-token queries, and these are common in technical corpora: error codes ("ORA-01555"), identifiers (`get_user_by_email`), product SKUs, ticket numbers, version strings, legal citations, and names.
Embedding models compress text through a subword tokenizer into a fixed vector; a rare identifier contributes almost nothing distinctive to the vector, while for BM25 that same rare term is the highest-IDF, highest-weight signal available.
Dense retrieval also degrades out of domain - an embedder trained mostly on web prose has weaker geometry for niche jargon - while BM25's term matching is domain-agnostic by construction.
The BEIR benchmark (2021) made this concrete: BM25 remained a strong baseline that many dense models failed to beat across heterogeneous domains, and while dense and hybrid models have improved since, the structural point stands as of early 2026: the two methods fail on disjoint query classes, which is precisely why combining them works.

## 2. Fusion: combining retrievers

Run both retrievers, then merge the ranked lists.
The obstacle is that BM25 scores and cosine similarities live on incomparable scales, and score distributions shift per query.

Reciprocal rank fusion (RRF) sidesteps scales entirely by using only ranks.
Each document's fused score is the sum over lists of 1 / (k + rank_in_list), with k conventionally 60.
The constant k damps the dominance of top ranks: with k=60, ranks 1 and 2 score 0.0164 and 0.0161, so a document ranked moderately in both lists can beat one ranked first in one list and absent from the other.
Pros: no tuning, no score normalization, robust across retrievers of different types; this is why it is the default fusion in Elasticsearch, OpenSearch, and most RAG stacks.
Cons: it discards score magnitude, so a dense hit with overwhelming similarity counts the same as a marginal rank-1; when you have calibrated scores, weighted score fusion (normalize per list, then convex combination with a tuned alpha) can outperform RRF, at the price of per-corpus tuning that drifts.
Practical rule: start with RRF; move to tuned weighted fusion only when an evaluation set proves the gain.

## 3. Cross-encoder reranking

A bi-encoder (the standard embedding model) encodes query and document independently into vectors; relevance is a dot product of two summaries computed in ignorance of each other.
A cross-encoder concatenates query and document into one input and runs full transformer attention across the pair, letting every query token attend to every document token, and outputs a relevance score directly.
This captures exactly what bi-encoders lose: precise term correspondence, negation, conditionals ("does NOT apply when..."), and which entity plays which role.

The cost structure dictates the architecture.
A bi-encoder embeds the corpus once offline; query cost is one embedding plus ANN search.
A cross-encoder must run one full forward pass per query-document pair at query time, with nothing precomputable.
So cross-encoders cannot search a corpus, but they can re-score a candidate list: retrieve 50-200 candidates cheaply with hybrid first-stage retrieval, then rerank them with the cross-encoder and keep the top 5-20.
This first-stage-recall, second-stage-precision split is the same architecture web search settled on decades ago, rediscovered for RAG.

Options as of early 2026: open models (the BGE reranker family, Jina rerankers, MixedBread rerankers, and others on the standard MTEB-style leaderboards) that you host yourself, and API rerankers (Cohere Rerank being the most established, with Voyage and others competing).
LLM-as-reranker - prompting a general model to score or order candidates (pointwise scoring, or listwise as in RankGPT-style work) - is the quality ceiling but the highest latency and cost; it is most defensible inside agents that already pay LLM latency per step.
Honest cost accounting: reranking 100 candidates through a hosted cross-encoder adds tens to a few hundred milliseconds and a per-query fee; through a self-hosted GPU it adds a batch forward pass and an infrastructure bill.
The evaluation question is always: does reranking lift answer-level quality enough to pay its latency, at your k; measure nDCG@10 before and after (Chapter 06), because on easy corpora the first stage may already saturate.

## 4. Late interaction: ColBERT

ColBERT (Khattab and Zaharia, 2020; ColBERTv2, 2022) sits deliberately between the two architectures.
Encode the document into one vector per token (not one per document) offline; encode the query into one vector per token at query time.
Score with MaxSim: for each query token, take the maximum similarity against all document token vectors, and sum over query tokens.
This preserves token-level matching - the query token "ORA-01555" can find its exact counterpart vector - while keeping document encoding precomputable, unlike a cross-encoder.

Trade-offs.
Storage explodes: hundreds of vectors per document instead of one, mitigated by ColBERTv2's residual compression and by the PLAID engine's centroid-based candidate generation, but still several times the footprint of single-vector indexes.
Infrastructure is nonstandard: mainstream vector databases are built for single-vector search, and although multi-vector support has been appearing (Vespa has long supported it natively; Qdrant and others added multi-vector types in the 2024-2025 era), operational maturity lags single-vector paths.
Quality: late-interaction models punch above their parameter count on out-of-domain retrieval (a headline BEIR result for ColBERTv2), and the architecture found a second life in document-image retrieval (ColPali and successors, 2024 onward) where token-level patches make visual grounding tractable.
Position it as: better recall-stage quality than bi-encoders, cheaper query-time than cross-encoders, at the price of storage and less-trodden infrastructure; most teams still get further, sooner, with hybrid-plus-cross-encoder, which is the well-paved road.

## 5. Query understanding

The query the user typed is often not the best query to search with, and everything upstream of retrieval multiplies or destroys downstream quality.

Query rewriting.
In conversation, queries are elliptical: "what about the enterprise tier?" is unsearchable without the preceding turns.
Rewrite conversational queries into standalone queries with a fast LLM before retrieval; this is the single highest-value query transformation in chat products and is essentially mandatory for conversational RAG.

Query decomposition.
Comparative and multi-part questions ("compare the retention policies of product A and B") retrieve poorly as one query because the embedding averages both intents.
Decompose into sub-queries, retrieve per sub-query, and merge (dedup, then fuse); this overlaps with agentic planning, covered fully in Chapter 07.

Query expansion.
Classical expansion adds synonyms or related terms (thesaurus-based, or pseudo-relevance feedback: assume the top results are relevant, mine their salient terms, requery).
It mainly helps lexical retrieval, where vocabulary mismatch is the core weakness; dense retrieval partly obsoletes it but identifier-heavy corpora still benefit from expanding known aliases (error code to error name).

HyDE (Hypothetical Document Embeddings, Gao et al., 2022).
Ask an LLM to write a hypothetical answer to the query, embed that fake answer, and search with its vector.
Rationale: a hypothetical answer is distributionally closer to real answer passages than a terse question is, so answer-to-answer similarity beats question-to-answer similarity, especially zero-shot with no tuned retriever.
Costs and risks: one LLM call of latency per query, and if the model hallucinates a wrong-topic answer the retrieval follows it off a cliff; modern instruction-tuned embedders that encode queries and passages asymmetrically have narrowed HyDE's advantage, so as of early 2026 treat it as a technique to evaluate on hard corpora rather than a default.

The routing consideration that ties these together: each transformation adds latency and an LLM dependency, so production systems classify queries first (cheap heuristics or a small model) and apply expensive understanding only where it pays - rewrite always in chat, decompose only multi-intent queries, HyDE rarely and by evidence.

## 6. The multi-stage pipeline, assembled

The consensus architecture as of early 2026, stable enough to teach as a principle.

Stage 0, query understanding: rewrite (conversation), optionally decompose or expand; budget tens to a few hundred milliseconds with a fast model.
Stage 1, candidate generation for recall: dense ANN top-100 and BM25 top-100 in parallel, both with hard metadata and ACL filters applied.
Stage 2, fusion: RRF into one candidate list, dedup near-identical chunks (overlapping chunk hygiene from Chapter 02 matters here).
Stage 3, reranking for precision: cross-encoder over the fused top-100, keep top-k for generation (k typically 5-20 with long-context models).
Stage 4, assembly: small-to-big expansion to parent sections where configured, source formatting with citations, and token-budget enforcement.

Each stage exists to correct a specific weakness of the previous one, and each is independently measurable: recall@100 for stage 1, nDCG@10 after stage 3, answer faithfulness after generation (Chapter 06).
That per-stage measurability is the real argument for the architecture: when quality drops you can localize the failure instead of shaking the whole pipeline.

```python
# Multi-stage hybrid retrieval with RRF fusion and cross-encoder reranking.
# Abstract interfaces; bind them to your stores (Chapter 04) and models.
from collections import defaultdict

RRF_K = 60

def rrf_fuse(ranked_lists: list[list[str]], k: int = RRF_K) -> list[str]:
    scores: dict[str, float] = defaultdict(float)
    for ranking in ranked_lists:
        for rank, doc_id in enumerate(ranking, start=1):
            scores[doc_id] += 1.0 / (k + rank)
    return sorted(scores, key=scores.get, reverse=True)

def retrieve(query: str, history: list[str], filters: dict,
             dense, bm25, reranker, store, llm,
             n_candidates: int = 100, k_final: int = 10) -> list[dict]:
    # Stage 0: make the query standalone (conversational rewrite).
    if history:
        query = llm.rewrite_standalone(query=query, history=history)

    # Stage 1: parallel candidate generation, hard filters in both arms.
    dense_ids = dense.search(query, top_k=n_candidates, filters=filters)
    lexical_ids = bm25.search(query, top_k=n_candidates, filters=filters)

    # Stage 2: rank-based fusion, then dedup by parent document region.
    fused = rrf_fuse([dense_ids, lexical_ids])[:n_candidates]
    candidates = store.fetch(fused)
    candidates = dedup_overlapping(candidates)

    # Stage 3: cross-encoder precision pass.
    scores = reranker.score(query, [c["text"] for c in candidates])
    ranked = [c for _, c in sorted(zip(scores, candidates),
                                   key=lambda p: p[0], reverse=True)]

    # Stage 4: expand to parents and enforce the token budget.
    results = [store.parent_or_self(c) for c in ranked[:k_final]]
    return enforce_token_budget(results, max_tokens=8000)
```

Failure modes of the pipeline itself, to watch in production.
Rewrite drift: the standalone rewrite subtly changes the question; log both forms and eyeball diffs.
Fusion starvation: one retriever returns garbage confidently and RRF dilutes the good list; per-arm recall metrics catch this.
Reranker domain mismatch: a general reranker can demote correct domain passages; evaluate the reranker on your golden set before trusting it, and remember the reranker caps your quality once first-stage recall is high.
Latency stacking: rewrite plus two retrievals plus reranking sums to noticeable seconds; run stages concurrently where possible and cache aggressively (embedding cache keyed by query hash, rewrite cache keyed by conversation state).

## Exercises

1. Build BM25 (rank_bm25 or Elasticsearch) and dense retrieval over one corpus.
   Construct 30 queries: 10 paraphrase-style, 10 containing exact identifiers or error codes, 10 mixed.
   Report recall@10 per retriever per class, and confirm or refute the disjoint-failure claim on your data.
2. Implement RRF and a normalized weighted-sum fusion with alpha swept over {0.3, 0.5, 0.7}.
   Compare against each single retriever on the 30 queries; report when tuned fusion beats RRF and by how much.
3. Add an open cross-encoder reranker over fused top-100.
   Measure nDCG@10 before and after, and the added p50/p99 latency at batch sizes 16 and 100; write three sentences on whether it pays for this corpus.
4. Implement HyDE with a small fast model and compare top-10 hit rate against direct dense retrieval on your 30 queries.
   Find and document at least one query where the hypothetical answer led retrieval astray.
5. Implement conversational rewrite: take five multi-turn conversations, retrieve with the raw final turn versus the rewritten standalone query, and count retrieval misses fixed and introduced.
6. Score MaxSim by hand: given a 4-token query and two 10-token documents with provided per-token vectors, compute ColBERT-style scores and explain which token correspondences drove the winner.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Explain the mechanism behind BM25's advantage on rare exact tokens, down to IDF weighting and subword tokenization.
- Write the RRF formula from memory, explain the role of k, and state when weighted score fusion is worth its tuning cost.
- Explain what a cross-encoder computes that a bi-encoder structurally cannot, and why that forces the two-stage recall/precision architecture.
- Place ColBERT precisely between the two on the quality/cost/storage axes and name its infrastructure catch.
- Choose query transformations (rewrite, decompose, expand, HyDE) for a given product with latency budgets, and defend each inclusion and exclusion.
- Draw the five-stage pipeline, name the specific weakness each stage corrects, and name the metric that isolates each stage's failure.
