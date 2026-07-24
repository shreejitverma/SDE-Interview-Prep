# Chapter 01 - Why Retrieval

## What you will master

- Why language models cannot know everything, and the precise mechanics of what they do know.
- The difference between parametric and non-parametric knowledge, and why the distinction drives system design.
- How hallucination actually arises at the token level, and why retrieval reduces but does not eliminate it.
- The lineage from the original RAG paper (Lewis et al., 2020) to modern retrieval-augmented agents.
- A decision framework for choosing between RAG, fine-tuning, long context, and live search tools.

## 1. The knowledge problem

A large language model is a function from a token sequence to a probability distribution over the next token.
Everything the model "knows" was baked into its weights during pretraining and post-training.
This creates three structural gaps that no amount of scale fixes.

First, the knowledge cutoff.
Training data has a collection date, and the world keeps moving after it.
As of early 2026, frontier models typically ship with cutoffs somewhere between six and eighteen months before their release date, because data pipelines, training runs, and safety evaluations take time.
A model with a March 2025 cutoff cannot know about a library version released in June 2025, no matter how confidently it answers.

Second, private data.
Your company's design docs, ticket history, customer records, and internal wikis were never in any training corpus.
The model has zero parametric knowledge of them, and the only way to get that knowledge into an answer is to put it into the context window at inference time.

Third, the long tail.
Even for public data inside the cutoff, models memorize unevenly.
Facts that appear thousands of times in the corpus (the capital of France) are stored redundantly and recalled reliably.
Facts that appear a handful of times (the default timeout of an obscure config flag) are stored weakly, if at all.
Research on memorization consistently shows recall correlates with duplication count in the training data, which means the long tail of rare facts is exactly where parametric knowledge is least trustworthy.

Retrieval is the general answer to all three gaps.
Instead of asking the model to recall, you fetch the relevant text from an external store and ask the model to read.
Reading is a much easier task than recall, and models are dramatically better at it.

## 2. Parametric versus non-parametric knowledge

The vocabulary comes from the machine learning literature and is worth using precisely.

Parametric knowledge is information encoded in the model weights.
It is fast to access (no extra inference-time work), compressed, and interpolative, meaning the model can blend related facts smoothly.
It is also frozen at training time, impossible to attribute to a source, expensive to update (you must train), and unreliable in the long tail.

Non-parametric knowledge is information stored outside the weights - documents, databases, indexes - and injected into the context at inference time.
It is updatable in real time (write a new document, it is instantly available), attributable (you know which chunk produced the claim), and auditable.
It costs context tokens, adds retrieval latency, and introduces a new failure mode: retrieving the wrong thing.

The design consequence is a division of labor.
Use parametric knowledge for language competence, reasoning patterns, common-sense facts, and domain vocabulary.
Use non-parametric knowledge for anything that is private, fresh, rare, or must be cited.
Almost every production system that answers questions about specific data is a hybrid: the model's parameters provide the reading and reasoning skill, the retrieval layer provides the facts.

## 3. Hallucination mechanics

"Hallucination" is a behavioral label, not a mechanism.
The mechanism is ordinary next-token prediction operating without sufficient grounding.

Consider what happens when a model is asked "What is the return type of `parse_config` in our codebase?" with no retrieval.
The model has no parametric knowledge of your codebase.
But the training objective never rewarded saying "I do not know" as strongly as producing plausible text, and the sampling process must emit some token.
The distribution over next tokens is shaped by every `parse_config` function the model has ever seen, so it emits a statistically plausible answer: probably `dict` or a dataclass name.
The output is fluent, confident, and fabricated, because fluency and confidence are properties of the language model, not of the underlying knowledge.

Three specific mechanisms are worth internalizing.

Plausibility substitution.
When the true fact is absent or weakly stored, the model substitutes the most probable completion given the surface form of the question.
This is why hallucinated citations look real: the model generates author names, years, and journal names that match the distribution of real citations.

Confabulation under commitment.
Autoregressive generation cannot backtrack.
Once the model has emitted "The function returns a", it must continue, and the continuation will be locally coherent even if the premise was wrong.
Errors early in a generation propagate and get elaborated, not corrected.

Sycophantic agreement.
Post-training on human feedback rewards answers users rate highly, and users rate confident, agreeable answers highly.
This biases models toward asserting rather than hedging, which converts uncertainty into confident error.

Retrieval attacks the root cause: it changes the task from recall to reading comprehension.
When the answer is present in the context, the probability mass concentrates on tokens copied or paraphrased from that context, and the model's strong in-context abilities take over.
But note the honest limits.
Retrieval does not help if the retriever returns the wrong passage, and the model may then confidently ground its answer in irrelevant text.
Retrieval does not fully prevent the model from blending context with parametric priors, especially when the two conflict; studies of knowledge conflicts show models sometimes prefer their parametric answer over contradicting context.
And retrieval does nothing about reasoning errors on top of correct facts.
Grounding reduces fabrication of facts; it does not guarantee faithful synthesis.
This is why Volume 05 dedicates a full chapter to evaluation: you must measure retrieval quality and generation faithfulness separately.

## 4. Grounding, attribution, and why enterprises care

Grounding means every factual claim in the output is supported by retrieved evidence available in the context.
Attribution means the system can point at the specific evidence for each claim.
These are distinct: an answer can be grounded but unattributed (correct, no citations) or attributed but ungrounded (citations that do not actually support the claim).

For consumer chat, grounding is a quality feature.
For enterprise and regulated domains, it is a hard requirement.
A legal research tool that fabricates case law, a medical assistant that invents dosages, or an internal support bot that misstates policy creates liability, not just user annoyance.
Retrieval-based systems are the standard architecture in these domains because they make the evidence inspectable: you can log which chunks were retrieved, show citations to users, and audit failures after the fact.
Parametric answers offer none of that; there is no way to ask a weight matrix for its sources.

## 5. Lineage: from open-domain QA to the RAG paper to agents

Retrieval-augmented generation has a longer history than the current hype cycle suggests, and knowing the lineage helps you evaluate claims about novelty.

Open-domain question answering systems in the pre-neural era (TREC QA tracks, IBM Watson circa 2011) already followed the retrieve-then-read pattern with lexical search and hand-engineered readers.
DrQA (Chen et al., 2017) modernized this with a TF-IDF retriever feeding a neural reading-comprehension model over Wikipedia.
ORQA (Lee et al., 2019) and REALM (Guu et al., 2020) made the retriever itself learned and dense, with REALM jointly pretraining the retriever and the language model.
DPR, dense passage retrieval (Karpukhin et al., 2020), showed that a simple dual-encoder trained on question-passage pairs beats BM25 on several QA benchmarks, and became the standard dense retriever recipe.

The paper that named the pattern is "Retrieval-Augmented Generation for Knowledge-Intensive NLP Tasks" (Lewis et al., 2020, from Facebook AI Research).
Its specific contribution is often misremembered.
RAG in that paper is a jointly trained model: a DPR retriever plus a BART generator, with the retrieved passages treated as a latent variable marginalized over during training, in two variants (RAG-Sequence and RAG-Token).
The retriever's query encoder was fine-tuned end to end with the generator.
What the industry now calls RAG - frozen off-the-shelf LLM, frozen off-the-shelf embedding model, passages pasted into a prompt - is architecturally closer to "in-context retrieval augmentation" and shares only the name and the high-level idea.
This matters practically: the original paper's joint training solved the retriever-generator mismatch problem that modern frozen pipelines still suffer from, and modern systems compensate with reranking, query rewriting, and prompt engineering instead.

From roughly 2022 onward, the frozen-pipeline pattern exploded because instruction-tuned LLMs made the generator a commodity and vector databases made the retriever a product.
From roughly 2024 onward, the frontier moved again: instead of a fixed retrieve-then-generate pipeline, agents call search as a tool, decide when and what to retrieve, issue multiple queries, and iterate.
That evolution is the subject of Chapter 07.
The stable principle across all eras is unchanged: separate the knowledge store from the reasoner, and let the reasoner consult the store at inference time.

## 6. The alternatives, stated fairly

Retrieval is one of four ways to get knowledge into a model's output.
Each is the right answer for some workloads.

### 6.1 Fine-tuning

Fine-tuning continues training on your own data, updating weights (fully or via adapters such as LoRA).
What it is good at: teaching behavior - style, format, tone, domain vocabulary, tool-use patterns, and task-specific skills.
What it is bad at: injecting facts.
Knowledge injection via fine-tuning is unreliable because a fact seen a few hundred times in fine-tuning competes with billions of pretraining tokens, and empirical studies repeatedly find fine-tuned models still hallucinate on the injected facts while sometimes degrading on general ability (catastrophic forgetting).
Fine-tuning also freezes the knowledge again: every data update requires a new training run, evaluation pass, and deployment.
There is no attribution and no per-user access control - once a fact is in the weights, every user of that model can potentially elicit it, which is a real data-governance problem.
The honest summary as of early 2026: fine-tune for form, retrieve for facts.

### 6.2 Long context

Context windows grew from 4K tokens (2022) to 100K-200K (2023) to models advertising one million or more tokens (Gemini 1.5 and successors, 2024 onward).
The naive conclusion is that you can skip retrieval and paste the whole corpus into the prompt.
Sometimes you genuinely can, and when the corpus fits comfortably, it is the simplest and often the highest-quality option because the model sees everything with full cross-document attention.
The limits are concrete.
Cost: attention-based inference cost scales with context length, and paying for hundreds of thousands of input tokens on every request is orders of magnitude more expensive than retrieving a few thousand relevant tokens; prompt caching reduces but does not eliminate this.
Latency: prefill time grows with context length, so million-token prompts add seconds to time-to-first-token.
Quality: models do not attend uniformly across long contexts; "needle in a haystack" tests look solved, but harder multi-fact benchmarks (RULER, NoLiMa, and similar, 2024-2025) show effective context - the length at which reasoning quality holds - is well below the advertised maximum for most models.
Scale: most real corpora (a company wiki, a codebase's history, a document archive) are millions to billions of tokens and simply do not fit.
The practical role of long context is not to replace retrieval but to relax it: you can retrieve larger chunks, more documents, and whole files rather than fragments, which makes the retriever's precision requirements much softer.

### 6.3 Live search tools

Giving the model a web-search or API tool is retrieval where the index is the live internet or a live system of record.
This is the right choice when freshness is paramount (news, prices, stock levels, current documentation) and when you cannot or should not maintain your own index.
The costs: you do not control ranking quality, results can be ad-laden or SEO-spammed, latency is variable, and you inherit the search provider's coverage and terms.
For private data, a search tool over your own index and classic RAG converge; the difference is merely whether the model or a fixed pipeline decides when to query.

### 6.4 Classic RAG

Maintain your own index over your own corpus, retrieve top-k passages per query, and generate with them in context.
Strengths: fresh (index updates are cheap), attributable, access-controllable (filter chunks by the caller's permissions at query time), and economical (only relevant tokens hit the context).
Weaknesses: you now operate a pipeline - parsing, chunking, embedding, indexing, and retrieval each have failure modes, and total answer quality is upper-bounded by retrieval quality.
RAG is the default for question answering over private corpora at any nontrivial scale, and the rest of this volume is about doing it well.

## 7. A decision framework

Ask these questions in order.

1. Is the knowledge about behavior (style, format, procedure) rather than facts?
   If yes, fine-tune or prompt-engineer; retrieval does not teach behavior.
2. Does the total relevant corpus fit in a fraction of the context window at acceptable cost and latency, including under prompt caching?
   If yes, just include it; do not build a retrieval pipeline you do not need.
3. Must answers reflect data that changes between requests, or data you do not host?
   If yes, use live search or API tools for that portion.
4. Is the corpus private, large, access-controlled, or citation-critical?
   If yes, build RAG over your own index.
5. Do queries require multi-step lookup, comparison across documents, or exploration?
   If yes, expose retrieval as a tool to an agent rather than a single-shot pipeline (Chapter 07).

These combine rather than exclude.
A production assistant in early 2026 commonly uses a fine-tuned or well-prompted model (behavior), retrieval over internal docs (private facts), a web-search tool (freshness), and long context to hold generous retrieved material (relaxed precision).
The framework tells you where each mechanism carries which load, so you can debug the right layer when quality drops.

A note on cost asymmetry, because it decides many arguments.
Embedding and indexing a corpus is a one-time cost plus incremental updates.
Long-context stuffing is a per-request cost.
If a corpus is queried thousands of times, retrieval amortizes its pipeline cost quickly; if it is queried three times ever, building a pipeline is over-engineering and stuffing or manual selection wins.
Query volume, corpus size, and freshness requirements - not fashion - should drive the choice.

## 8. What retrieval does not solve

Close the chapter with the failure modes retrieval leaves open, because Chapters 02 through 06 exist to address them.

- Garbage in: if parsing mangles a PDF table, retrieval faithfully serves mangled text (Chapter 02).
- Retrieval miss: if the relevant chunk is not in the top-k, the generator cannot use it, and recall depends on chunking, embeddings, and index parameters (Chapters 02, 03).
- Semantic gap: embedding similarity is not relevance; lexically distinct but relevant passages get missed, and hybrid retrieval plus reranking exist for this reason (Chapter 05).
- Synthesis miss: the right passage is in context and the model still answers wrongly or unfaithfully, which only generation-side evaluation catches (Chapter 06).
- Multi-hop questions: single-shot retrieval cannot answer questions whose evidence must be found sequentially (Chapter 07).

## Exercises

1. Pick a library that released a major version after your favorite model's knowledge cutoff.
   Ask the model three API questions without retrieval, then with the changelog pasted into context.
   Classify each ungrounded answer as correct-from-parameters, plausibility substitution, or refusal.
2. Take ten factual questions about your own codebase or company docs.
   Answer them with (a) no context, (b) the full relevant document in context, (c) a single retrieved paragraph.
   Score correctness and note where the paragraph was insufficient - this is your first retrieval-granularity observation.
3. Write a one-page decision memo for a real system you know: should its knowledge live in fine-tuning, long context, live search, or RAG?
   Apply the five questions from Section 7 explicitly, and state the downside of your chosen option.
4. Read the abstract and Section 2 of Lewis et al. (2020).
   Write three sentences on what the original RAG architecture trained end to end that modern frozen pipelines do not, and which modern technique compensates for each gap.
5. Construct a knowledge-conflict probe: paste a context passage that contradicts a well-known fact and ask the model a question it answers.
   Observe whether the model follows the context or its parameters, and test how instruction wording changes the outcome.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Explain the token-level mechanism of hallucination and why retrieval converts recall into reading comprehension.
- Define parametric and non-parametric knowledge and give two properties each that the other lacks.
- State what the Lewis et al. 2020 RAG paper actually trained, and how modern frozen-pipeline RAG differs.
- Argue both sides of "long context kills RAG" with concrete cost, latency, and effective-context evidence, and state where each side is right.
- Given a workload description (corpus size, freshness, privacy, query volume), choose among fine-tuning, long context, search tools, and RAG, and name the main failure mode of your choice.
