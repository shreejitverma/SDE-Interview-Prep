# Chapter 07 - Agentic RAG and Beyond

## What you will master

- The architectural shift from single-shot retrieve-then-generate to agent-driven retrieval, and what the model gains by controlling the loop.
- Query planning, iterative search, and the multi-hop question class that fixed pipelines cannot answer.
- The research lineage of self-correcting retrieval: Self-RAG and CRAG, and what survives of them in practice.
- GraphRAG and knowledge graphs: what graph structure buys, what it costs, and the query classes that justify it.
- Code-aware retrieval: why production coding agents mostly grep, and when embeddings still earn a place.
- File-system-as-context and the general trend toward navigable rather than pre-retrieved knowledge.
- The "RAG is dead" debate, stated fairly enough to argue both sides.

## 1. From pipeline to agent

Classic RAG is a fixed dataflow: one query in, one retrieval, one generation, done.
The pipeline decides nothing; every query gets the same treatment regardless of whether it needs zero retrievals or seven.
Agentic RAG inverts the control flow: retrieval becomes a tool (or several), and the model decides when to search, what to search for, whether the results suffice, and whether to search again.
This is the same shift Volume 03 described for tool use generally, applied to knowledge access.

What the model gains by holding the loop.
Adaptive effort: trivial questions skip retrieval, hard ones get many searches; a fixed pipeline pays the same cost for both, and mis-serves both tails.
Query reformulation with feedback: the agent sees the results of search one before writing search two, so a vocabulary mismatch is a recoverable event rather than a silent miss - the fixed pipeline's rewriting (Chapter 05) is a guess made blind.
Multi-hop composition: "which of our customers is affected by the CVE we discussed in the March incident review" requires finding the incident review, extracting the CVE, then searching customer deployments; the second query cannot be written until the first result is read.
Verification behavior: an agent can retrieve, draft, notice an unsupported claim, and retrieve again to check it.

What it costs, stated honestly.
Latency and tokens multiply with loop iterations, and the model may loop unproductively (repeating near-identical queries is a documented failure pattern that needs loop budgets and dedup).
Nondeterminism replaces the pipeline's reproducibility, complicating evaluation - you are now evaluating trajectories, not a function (Volume 10's territory).
And the quality floor drops: a fixed pipeline never forgets to search, while an agent sometimes answers from parameters when it should have looked, so production systems often keep a mandatory first retrieval and let the agent iterate beyond it.
As of early 2026 the pragmatic spectrum runs: fixed pipeline for high-volume homogeneous queries where latency is precious, single-agent tool-loop retrieval for assistant products, and planned multi-step research for deep tasks - and one product can route between all three.

## 2. Query planning and iterative search

Two planning styles dominate.
Decomposition-first: the model reads the question, emits a plan of sub-queries (compare A and B becomes lookup A, lookup B), executes them (often in parallel), and synthesizes; this suits questions whose structure is visible upfront, and it maps to the plan-then-execute patterns of Volume 04.
Interleaved reasoning and search: the model alternates thought, search, and reading, deciding each next query from accumulated evidence; the research lineage runs from ReAct (2022) through iterative retrieval work like IRCoT and FLARE (2023), and by 2025 the pattern is native to frontier models trained for agentic search, needing orchestration rather than exotic prompting.
Interleaving handles hidden structure - you do not know what to search next until you read - at the cost of serial latency, since each search depends on the last.

Engineering the loop, the parts that matter in practice.
Give the searcher tools with honest descriptions and distinct purposes (semantic search, keyword search, fetch-by-id, list-structure) so the model can choose the right probe; a single blended "search" tool hides the choice the model is best placed to make.
Budget the loop explicitly: max searches, max tokens of accumulated evidence, and a required stop-and-answer; models overshoot without limits and undershoot with harsh ones, so tune on traces.
Deduplicate and compress accumulated evidence between iterations, or the context fills with near-copies (Volume 06's compaction techniques apply directly).
Deep-research products (the OpenAI, Google, Anthropic, and Perplexity offerings of 2025) are this same loop scaled up: minutes of budget, dozens to hundreds of retrievals, explicit synthesis with citations - proof that the pattern holds far past chat-scale, if you pay for it.

## 3. Self-correcting retrieval: Self-RAG and CRAG

Two 2023-2024 research directions made the correction loop explicit, and both are worth knowing as idea sources even though neither is deployed verbatim at scale as of early 2026.

Self-RAG (Asai et al., 2023) fine-tunes a model to emit reflection tokens during generation: whether retrieval is needed at this point, whether a retrieved passage is relevant, whether the generated segment is supported by it, and whether the segment is useful.
The generation process branches on these self-assessments - retrieve on demand, discard irrelevant passages, prefer supported continuations.
The durable ideas: retrieval-on-demand rather than always-retrieve, and inline self-assessment of support.
The reason it did not ship as-is: it requires training a bespoke model, and frontier instruction-following made the same behaviors promptable - modern agentic systems get retrieval-on-demand from tool choice and support-checking from judge or verification steps, without special tokens.

CRAG, corrective RAG (Yan et al., 2024), keeps the generator frozen and adds a lightweight retrieval evaluator: score the retrieved set as correct, incorrect, or ambiguous; on correct, refine the passages (decompose-then-recompose to strip noise); on incorrect, discard and escalate to web search; on ambiguous, blend both.
The durable ideas: grade the evidence before generating, and have a fallback retrieval source when the primary store fails.
Both papers converge on one principle that did survive everywhere: do not trust a single retrieval pass; assess it, and act on the assessment - which is exactly what an agent loop does with the model itself as the assessor.

## 4. GraphRAG and knowledge graphs

Vector retrieval answers "find text similar to this query", and some questions are not that shape.
"What themes recur across all incident reports this year" has no local passage to find - the answer is a property of the whole corpus.
"How is entity A connected to entity C" may require joining facts stated in documents that share no vocabulary.
Graph-augmented RAG targets exactly these global and relational query classes.

Microsoft's GraphRAG (2024) is the reference design.
Index time: an LLM extracts entities and relations from every chunk, builds a graph, detects hierarchical communities (Leiden algorithm), and writes an LLM summary for each community at each level.
Query time, global mode: answer corpus-level questions map-reduce style over community summaries rather than chunks; local mode: start from entities matched in the query and expand along edges to assemble a neighborhood context.
What it buys: genuinely better answers on sensemaking and whole-corpus questions, and multi-hop joins that similarity search structurally misses.
What it costs, and the costs are the story: index-time LLM extraction over every chunk is orders of magnitude more expensive than embedding; extraction quality bounds everything (missed or wrongly merged entities silently corrupt paths); incremental update of communities and summaries under a changing corpus is genuinely hard; and evaluation of "sensemaking" answers is weaker ground than QA metrics.
Later variants (LazyGraphRAG, deferring extraction to query time; the LightRAG lineage; and property-graph implementations in LlamaIndex and Neo4j stacks) attack the cost side.
Decision rule as of early 2026: adopt graph structure when your query mix demonstrably contains global-summary or multi-hop-relational questions that flat retrieval fails on your evals, and when you can tolerate the index economics; do not adopt it because the architecture diagram looks smarter.
Where a curated knowledge graph already exists as a governed asset (biomedical, financial compliance), exposing it as a query tool to an agent is a much cheaper win than building a graph from raw text.

## 5. Code-aware retrieval: why coding agents mostly grep

Coding agents are the most-deployed agent category, and their retrieval design choice is instructive: as of early 2026, the leading agentic coding tools (Claude Code among them) rely primarily on grep/ripgrep, glob, and file reading, not embedding indexes; embedding-based code search exists in the ecosystem (Cursor's codebase indexing, Sourcegraph's hybrid search) but the agentic loop itself leans lexical.

The reasons are structural, not fashion.
Code queries are exact-token queries: an agent looking for `parse_config` wants occurrences of that literal symbol, and Chapter 05 already established that exact rare tokens are lexical search's home turf while embeddings blur identifiers.
Code has native navigable structure - imports, call sites, directory layout, symbol definitions - so retrieval-by-navigation (grep for the symbol, open the file, follow the import) exploits ground-truth relationships that similarity scores only approximate.
The agent loop converts grep's weakness into strength: a zero-hit grep returns instantly and the model reformulates, so imperfect recall per probe is fine when probes are cheap, fast, and iterated - whereas an embedding index over a codebase must be built, kept in sync with every edit (agents edit constantly, and a stale index is actively misleading mid-session), and still loses on exact matches.
Freshness is decisive: `git grep` is always correct about the working tree by construction.
Where embeddings still earn a place in code: natural-language questions over unfamiliar code ("where is retry logic handled"), cross-repository discovery at organization scale, and finding conceptually similar code with different naming - legitimately semantic queries, best served alongside, not instead of, lexical tools.
The honest synthesis: give a code agent grep, glob, file read, and optionally semantic search as separate tools, and observe that a strong model uses grep first and semantic search rarely - a result worth internalizing because it generalizes: when the corpus has exact identifiers, cheap probes, and native structure, agentic lexical navigation beats one-shot semantic retrieval.

## 6. File-system-as-context and navigable knowledge

Generalize the coding-agent lesson.
A file system is itself a knowledge interface: directory listings are a table of contents, file names carry metadata, and read/search tools are retrieval primitives.
By 2025 this became an explicit design pattern beyond code: agent harnesses mount corpora as browsable file trees, memory systems (Anthropic's memory tooling among them) persist knowledge as files the agent lists, reads, and edits, and "context engineering" guidance from multiple labs recommends letting agents pull context via tools rather than pushing pre-retrieved chunks.
Why it works: navigation preserves provenance and structure (the agent knows where a fact lives and what sits next to it), supports progressive disclosure (read the summary, open the detail only if needed - cheap tokens), and turns retrieval into an inspectable trajectory rather than an opaque top-k.
Why it does not replace indexes: navigation is serial and slow over large corpora (an agent cannot list-and-read its way through a million documents), discovery of the unknown-unknown document still needs search, and token costs of browsing add up; the mature design is hierarchical - search tools (lexical and semantic) to land in the right region, navigation tools to explore it precisely.
Note the convergence: this is Chapter 02's small-to-big idea reborn with the model, rather than the pipeline, doing the expansion.

## 7. The "RAG is dead" debate, stated fairly

The claim recurs every time context windows grow, and both sides hold real evidence; a senior engineer should be able to argue either.

The case against RAG-as-you-knew-it.
Million-token contexts plus prompt caching let entire corpora ride along at tolerable marginal cost for some workloads, deleting the retrieval pipeline and its failure modes outright.
Agentic search replaces the embedding pipeline's core role: a model with grep-like and web-search tools finds its own evidence iteratively, and coding agents prove it at scale daily.
The chunking-embedding-top-k apparatus is a lossy compression bolted on to fit 4K-token windows that no longer constrain us, and every stage of it (Chapters 02-05) is a place to silently lose the answer.

The case for retrieval's persistence.
Corpora outrun contexts: enterprise knowledge bases, log archives, and the web are orders of magnitude beyond any window, and Chapter 01's effective-context evidence (RULER-class results) shows reasoning quality degrades well before advertised limits.
Economics: shipping a million tokens per request is orders of magnitude costlier than retrieving ten thousand relevant ones, caching or not, and at production QPS this is the whole ballgame.
Latency: prefill on huge contexts adds seconds that interactive products cannot spend.
Access control and freshness: per-user filtered retrieval and instantly updatable indexes have no long-context equivalent - you cannot cache a context per permission set per day at scale.
And agentic search is retrieval: the agent's search tool is backed by exactly the indexes, hybrid ranking, and evaluation discipline of Chapters 02-06; the loop changed, the substrate did not.

The synthesis this volume commits to.
What is dying is a specific 2023 artifact: the mandatory, fixed, single-shot chunk-embed-top-k-stuff pipeline as the only way to connect models to knowledge.
What is thriving is retrieval as a layered capability - indexes and ranking (the substrate), exposed as tools (the interface), driven by a model in a loop (the controller), with long context as the buffer that makes generous evidence affordable.
Every layer of this volume remains load-bearing in that stack; what changed is who calls it.

## Exercises

1. Build a two-tool agent (semantic search plus keyword search over the same corpus) with a loop budget of five searches.
   Run ten multi-hop questions against it and against your Chapter 05 fixed pipeline; compare answer quality, total tokens, and wall-clock latency, and classify each win by mechanism.
2. Instrument the agent's trajectories and find one unproductive loop (repeated near-identical queries).
   Fix it two ways - a dedup instruction and a hard budget - and report which degrades answer quality less.
3. Implement CRAG's spirit without training: a judge prompt that grades a retrieved set correct/ambiguous/incorrect, with web search as the incorrect-branch fallback.
   Measure how often each branch fires on your golden set and whether end-to-end correctness improves.
4. Run entity and relation extraction over 200 chunks of a corpus, build the graph, and answer one global question ("what are the recurring themes") with map-reduce over community or cluster summaries versus top-k chunks.
   Report the quality difference and the full token cost of building the graph.
5. Take a real codebase and answer five questions ("where is X validated", "what calls Y") using only grep and file reads, logging every probe.
   Then answer the same five with embedding search over AST chunks (Chapter 02).
   Tabulate probes, tokens, and correctness, and identify which question types favored which method.
6. Write the strongest one-page memo you can for "our product should drop its RAG pipeline for long context plus agentic search", then the strongest rebuttal.
   End with the routing policy you would actually ship.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Explain what an agent controlling the retrieval loop gains over a fixed pipeline, and the three costs it pays, with the production middle grounds.
- Distinguish decomposition-first from interleaved search, and name the question class that forces interleaving.
- State the durable idea each of Self-RAG and CRAG contributed, and why neither ships verbatim while both ship in spirit.
- Name the two query classes that justify GraphRAG, walk through its index-time pipeline, and recite its cost structure honestly.
- Explain mechanically why coding agents grep instead of embedding, and the three conditions under which that lesson generalizes to non-code corpora.
- Argue "RAG is dead" and its rebuttal each for two minutes without strawmanning, and state the layered synthesis in three sentences.
