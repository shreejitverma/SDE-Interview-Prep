# Chapter 03 - Vector Search Internals

## What you will master

- What an embedding space is geometrically, and what similarity metrics actually compute.
- Exact nearest neighbor search, its true cost, and why brute force is underrated.
- Approximate nearest neighbor (ANN) families: inverted file (IVF) partitioning and graph-based search.
- The HNSW algorithm in real detail: skip-list intuition, layer construction, search, and its parameters.
- Compression: product quantization, scalar quantization, and binary quantization, with their accuracy costs.
- How to reason about the recall/latency/memory triangle, and how to size a system honestly.

## 1. Embedding spaces

An embedding model maps text to a point in R^d, with d typically between 256 and 4,096 for text models in use as of early 2026.
The training objective (contrastive learning on positive pairs, for most retrieval embedders) shapes the space so that semantically related texts land close together under a chosen metric.
Three metrics cover practice.

Cosine similarity measures the angle between vectors and ignores magnitude.
Dot product measures angle and magnitude together; some models train with dot product so that vector norm encodes something like confidence or salience.
Euclidean (L2) distance measures straight-line distance.
For unit-normalized vectors the three are monotonically related (cosine equals dot product, and L2 distance is a decreasing function of both), so ranking order is identical.
Practical rule: use the metric the embedding model was trained with, which the model card states; when in doubt, normalize to unit length and use cosine or dot product interchangeably.

Two geometric facts matter for engineering intuition.
First, high-dimensional spaces are counterintuitive: distances concentrate, meaning the ratio between the nearest and farthest neighbor distances shrinks as d grows, which is one reason exact tree structures (KD-trees) that work in low dimensions degrade to near-linear scans in high dimensions.
Second, embedding spaces are anisotropic in practice: vectors occupy a narrow cone rather than the full sphere, and average pairwise similarity is well above zero, so absolute similarity values are not comparable across models and thresholds must be calibrated empirically per model.

A note on matryoshka embeddings, common by 2024-2025: models trained with Matryoshka Representation Learning produce vectors whose prefixes (first 256 of 1,536 dimensions, say) are themselves usable lower-fidelity embeddings.
This enables a cheap coarse search on truncated vectors followed by re-scoring on full vectors, and it makes dimension a tunable cost knob rather than a fixed property.

## 2. Exact search, and when brute force is fine

Exact k-nearest-neighbor search over n vectors of dimension d costs O(n * d) similarity computations per query plus an O(n log k) or heap-based selection.
This sounds disqualifying and usually is not.
A dot product over d=1,024 floats is about 1,024 multiply-accumulates; modern CPUs with SIMD do this in tens of nanoseconds, and GPUs do millions of such products in parallel.
Concretely, a brute-force scan over 100,000 vectors at d=1,024 in float32 touches about 400 MB of memory and completes in the low tens of milliseconds on a single modern CPU core with SIMD, and far faster with multiple cores or a GPU - order-of-magnitude figures, not benchmarks, but stable ones.

Therefore: below roughly one million vectors, brute force is often the right engineering answer.
It gives perfect recall by construction, zero index build time, instant consistency on insert and delete, trivially correct metadata filtering (just scan the filtered subset), and no parameters to tune.
Every ANN structure trades away some of each of those properties to gain query speed.
The professional habit is to compute the brute-force cost for your actual n and d before reaching for ANN, and to treat ANN as an optimization with a measured payoff, not a default.
NumPy one-liner for the baseline: `scores = corpus @ query; top = np.argpartition(-scores, k)[:k]`.

## 3. The ANN problem and its families

ANN search accepts imperfect recall - returning most, not all, of the true nearest neighbors - in exchange for sublinear query cost.
Recall@k here means: of the true k nearest neighbors, what fraction did the index return.
Two index families dominate practice as of early 2026: partition-based (IVF) and graph-based (HNSW and descendants such as DiskANN/Vamana).
Hashing-based methods (LSH) are historically important but rarely competitive for dense text embeddings today, and tree-based methods do not survive high dimensionality.

### 3.1 IVF: inverted file indexes

Training: run k-means over a sample of the corpus to produce nlist centroid vectors (common heuristic: nlist near sqrt(n)).
Indexing: assign each vector to its nearest centroid; each centroid owns an inverted list of its members.
Query: find the nprobe nearest centroids to the query, then exhaustively scan only those lists.
Cost: nlist centroid comparisons plus roughly (nprobe / nlist) * n vector comparisons, so with nlist=4,096 and nprobe=64 you scan about 1.6 percent of the corpus.

The recall failure mode is geometric: a true neighbor can live in a cell whose centroid is not among the nprobe closest to the query, especially for queries near cell boundaries.
Raising nprobe buys recall linearly in query cost, which gives IVF a smooth, easily understood recall/latency dial.
Other properties: index build is fast (one k-means pass plus assignment), memory overhead is small, inserts are cheap (append to a list), but heavy inserts and drift degrade the centroids, requiring periodic retraining.
IVF composes naturally with compression (Section 5), which is why "IVF-PQ" is the classic billion-scale recipe in FAISS and in GPU-accelerated libraries.

### 3.2 Graph-based search, and the intuition before HNSW

Build a graph where each vector is a node connected to a small set of near neighbors.
Query by greedy traversal: start somewhere, repeatedly move to the neighbor closest to the query, stop when no neighbor improves.
Greedy traversal on a pure nearest-neighbor graph gets stuck in local minima and takes long routes across the space.
Two ideas fix this.
Diverse edges: when selecting a node's neighbors, prefer a spread of directions rather than a tight cluster of mutual neighbors (this is the relative-neighborhood-style pruning heuristic used by HNSW and Vamana), which gives the graph long-range shortcuts and better navigability.
Beam search: instead of tracking one current node, keep a bounded priority list of the best ef candidates seen and expand them; larger ef explores more and misses less.

## 4. HNSW in detail

Hierarchical Navigable Small World (Malkov and Yashunin, 2016) is the dominant in-memory ANN index, implemented in hnswlib, FAISS, Lucene (hence Elasticsearch and OpenSearch), pgvector, Qdrant, Weaviate, Milvus, and most others.

### 4.1 Structure

HNSW is a multi-layer graph, with the skip-list analogy taken literally.
Layer 0 contains every vector, with up to M_max0 edges per node (conventionally 2*M).
Each higher layer contains an exponentially shrinking random subset of nodes (each node's top layer is drawn from a geometric distribution controlled by a normalization factor mL, typically 1/ln(M)), with up to M edges per node.
Upper layers are sparse long-range maps for coarse routing; layer 0 is the dense map where the real search happens.

### 4.2 Search

Start at the entry point (a node in the topmost layer).
On each layer above 0, run greedy search with ef=1: move to the closest neighbor until no improvement, then descend a layer from that node.
On layer 0, run beam search with a candidate list of size ef (called efSearch or ef at query time): maintain a min-heap of candidates to expand and a bounded max-heap of the best ef results found; expand the closest unexpanded candidate, examine its neighbors, and stop when the closest remaining candidate is farther than the worst of the current best ef.
Return the top k of the result heap.
Complexity is empirically O(log n) distance computations for fixed recall, with the constant governed by M and ef.

### 4.3 Construction

Insert vectors one at a time.
Draw the new node's top layer l from the geometric distribution.
Route greedily from the global entry point down to layer l+1, then on each layer from l down to 0: run a beam search with width efConstruction to collect candidate neighbors, select up to M of them with the diversity heuristic (prefer a candidate only if it is closer to the new node than to all already-selected neighbors, which prunes redundant same-direction edges), connect bidirectionally, and shrink any neighbor that now exceeds its degree bound by re-running the selection heuristic on its edge list.
Construction is essentially n searches, so build time is O(n log n) distance computations and is very sensitive to efConstruction.

### 4.4 Parameters and their trade-offs

M (edges per node, typically 8-64): higher M means better recall and better robustness on hard (clustered, high-dimensional) data, at the cost of memory (index overhead is roughly 8 to 12 bytes per edge, so M=16 costs on the order of 100-plus bytes per vector at layer 0) and slower construction.
efConstruction (typically 100-500): higher values build a higher-quality graph; this is one-time cost, so err generous for static corpora.
ef at query time (must be at least k, typically 50-400): the main recall/latency dial; recall rises with ef with diminishing returns while latency rises roughly linearly.
The tuning method is not to memorize numbers but to sweep ef against a ground-truth set (computed by brute force on a sample) and plot the recall/latency curve for your data.

### 4.5 Operational sharp edges

Deletes: HNSW has no cheap delete; implementations mark nodes as deleted and skip them at query time, which degrades the graph until a rebuild or compaction (Qdrant, Lucene segment merges, and others each handle this differently, but the underlying cost is inherent).
Filtered search: applying a metadata filter during traversal removes nodes from the graph the search must route through; at low filter selectivity the graph fragments and greedy routing fails, which is why engines either switch to brute force under selective filters, or use filter-aware traversal (Qdrant), or partition indexes by tenant.
Concurrency: inserts mutate shared adjacency lists, so concurrent build-and-serve needs locking or segment-based designs.
Memory residency: classic HNSW assumes the graph and vectors fit in RAM; random access during traversal destroys naive disk paging, which is exactly the problem DiskANN/Vamana addresses by laying out a flat graph for SSD-friendly access with compressed vectors in RAM for routing.

## 5. Quantization: trading precision for memory

Float32 vectors at d=1,024 cost 4 KB each; 100 million of them is 400 GB before index overhead, which is why compression is not optional at scale.

### 5.1 Scalar quantization

Map each float32 dimension independently to int8 (or float16) using per-dimension or per-vector min/max or quantile calibration.
4x compression to int8 with typically small recall loss on text embeddings, cheap to implement, and SIMD-friendly for distance computation.
This is the boring, reliable first step and many engines (Qdrant, Milvus, pgvector via halfvec for fp16) expose it directly.

### 5.2 Binary quantization

Keep one bit per dimension (sign of each component), giving 32x compression and letting Hamming distance (XOR plus popcount) stand in for similarity.
Accuracy loss is significant as a standalone representation, but two facts rescue it.
High-dimensional embeddings, especially matryoshka-trained ones, preserve neighborhood structure surprisingly well under sign quantization.
And binary search is used as a coarse first stage: retrieve an oversampled candidate set (say 4x the target k) with Hamming distance, then rescore those candidates with full-precision or int8 vectors fetched from slower storage.
This two-stage oversample-and-rescore pattern is how engines advertise large speed and memory wins with modest recall cost; the honest caveat is that quality varies by embedding model, so validate per model.

### 5.3 Product quantization

PQ compresses a vector by splitting it into m subvectors (for example, 1,024 dimensions into m=64 subvectors of 16 dimensions each) and quantizing each subvector separately against its own codebook of 256 centroids learned by k-means.
The vector is then stored as m one-byte codes: 64 bytes instead of 4,096, a 64x compression.
Distance computation uses asymmetric distance computation (ADC): for an incoming query, precompute a table of distances from each query subvector to all 256 centroids of that subspace (m * 256 entries), after which the approximate distance to any database vector is m table lookups and additions - no floating-point multiplies against the database at all.
Refinements matter in practice: OPQ learns a rotation of the space before splitting so that subspaces carry balanced information, and IVF-PQ encodes residuals relative to the coarse IVF centroid rather than raw vectors, which tightens the quantization error.
Trade-offs: PQ achieves the largest compression ratios but distorts distances the most, ranking quality within close neighbors suffers, and rescoring the top candidates with exact vectors (kept on disk) is standard to recover accuracy.
PQ also adds training complexity: codebooks must be trained on representative data and retrained on drift.

### 5.4 Choosing a compression level

A defensible default ladder as of early 2026: float32 or float16 up to a few million vectors; int8 scalar quantization when RAM pressure appears; binary-plus-rescore or IVF-PQ-plus-rescore at tens to hundreds of millions; DiskANN-style SSD-resident designs when RAM cost dominates even compressed.
At every rung, the acceptance test is the same: recall@k against brute-force ground truth on your corpus and your query distribution, not the paper's.

## 6. The recall/latency/memory triangle

Every configuration choice in this chapter is a point in a three-dimensional trade space, and vendors quote the corner that flatters them.
Discipline for reasoning about it:

State the recall target first (for RAG with a reranker downstream, recall@100 of the first stage matters more than recall@10; without a reranker, recall@k at your actual k is the number).
Then measure latency at that recall, at your real filter selectivity, under concurrent load - single-query latency on an idle machine is the most commonly quoted and least representative figure.
Then price the memory: vectors plus graph edges plus metadata, times replication.

Worked example of the reasoning, with round numbers.
10 million chunks, d=1,024.
Float32 vectors: 40 GB; HNSW edges at M=16: a few GB more; feasible on one large machine, so HNSW uncompressed is simplest.
At 100 million chunks: 400 GB float32 becomes 100 GB at int8 or about 13 GB binary; now quantization or disk-resident indexes are structural decisions, not tuning.
At 100 thousand chunks: 400 MB; brute force ends the conversation.
The order of magnitude of n, not fashion, picks the architecture.

A final honesty note: ANN benchmarks (ann-benchmarks.com and successors) are run on standard datasets (SIFT, GloVe, deep learning embeddings) whose geometry differs from your embedding model's output on your corpus.
Use them to shortlist algorithms, never to skip your own recall measurement.

## Exercises

1. Implement brute-force top-k with NumPy over 100 thousand random-projected and real embedding vectors.
   Measure latency at d in {256, 1024} and verify the linear scaling in n and d.
2. Build ground truth with brute force, then index the same vectors with hnswlib.
   Sweep ef over {16, 32, 64, 128, 256} and plot recall@10 versus mean and p99 latency; repeat at M=8 and M=32 and explain the differences.
3. Implement IVF from scratch: k-means for nlist=256 centroids, inverted lists, and nprobe-controlled search.
   Plot recall versus fraction of corpus scanned and find the nprobe where you match HNSW at equal latency, if you can.
4. Implement scalar int8 quantization and binary quantization with oversample-and-rescore (4x oversampling, full-precision rescoring).
   Report recall@10 and memory per vector for float32, int8, and binary on a real embedding model's output.
5. Implement the PQ ADC distance path: train codebooks with k-means on subvectors, encode the corpus, and answer queries via lookup tables.
   Measure ranking correlation (Spearman) between PQ distances and exact distances for the top-100 of each query.
6. Reproduce the filtered-search failure: attach a random label to each vector, filter to 1 percent selectivity, and compare HNSW filtered recall against brute force over the filtered subset.
   Explain the mechanism in terms of graph connectivity.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Derive the cost of brute-force search for given n and d, and state the corpus size below which you would not build an ANN index, with reasoning.
- Explain IVF's recall failure mode geometrically and what nprobe trades for what.
- Walk through HNSW search and insertion on a whiteboard, including the layer distribution, the beam search on layer 0, and the neighbor-diversity heuristic, and state what M, efConstruction, and ef each control.
- Explain why deletes and selective filters hurt graph indexes, and name two engineering responses.
- Describe PQ end to end - subvectors, codebooks, ADC lookup tables, residual encoding under IVF - and say when you would pick it over scalar or binary quantization.
- Given a corpus size, dimension, QPS, and recall target, sketch a defensible index configuration and its memory footprint, and name the measurement that would validate it.
