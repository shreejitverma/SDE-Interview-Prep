# Chapter 01 - From N-Grams to Neural Language Models

## What you will master

- The language modeling objective stated precisely, and why it is a compression problem in disguise.
- N-gram models, their smoothing machinery, and the exact reasons they hit a wall.
- Neural language models from Bengio's 2003 formulation through RNNs, LSTMs, and seq2seq with attention.
- Why next-token prediction, a seemingly trivial objective, produces general-purpose capability.
- The GPT lineage and the wider frontier-model timeline through 2025, date-stamped so you can tell principle from ephemera.

## 1. The language modeling objective

A language model assigns a probability to a sequence of tokens.
Formally, for a sequence w_1, w_2, ..., w_T, the model estimates P(w_1, ..., w_T).
The chain rule of probability factorizes this exactly, with no approximation:

```
P(w_1, ..., w_T) = P(w_1) * P(w_2 | w_1) * P(w_3 | w_1, w_2) * ... * P(w_T | w_1, ..., w_{T-1})
```

This factorization turns the joint distribution over all possible texts into a product of next-token conditionals.
Every language model you will ever use, from a bigram counter to Claude, is an estimator of P(next token | all previous tokens).
The entire field disagrees only about how to represent the conditioning context and how to parameterize the conditional.

Training minimizes the negative log-likelihood of the data, which is the cross-entropy between the empirical distribution and the model:

```
L = - (1/T) * sum_t log P_model(w_t | w_1, ..., w_{t-1})
```

Perplexity is exp(L), the effective branching factor: a perplexity of 20 means the model is, on average, as uncertain as a uniform choice over 20 tokens.
Perplexity is the oldest metric in the field and still the most honest one for pretraining, because it cannot be gamed by formatting tricks the way benchmark accuracy can.
Its downside is that it correlates imperfectly with downstream usefulness, especially after post-training deliberately moves the model away from the pretraining distribution.

### The compression view

Shannon's source coding theorem links prediction and compression: a model with cross-entropy H bits per token can compress text to H bits per token with arithmetic coding.
A better language model is, literally, a better compressor of human text.
This is not a metaphor; it is an identity, and it explains why scale helps.
Compressing Wikipedia well requires knowing facts; compressing GitHub well requires knowing how programs behave; compressing dialogue well requires a model of what speakers want.
The objective never mentions facts, programs, or intent, but the loss cannot go below a floor without them.
Hold onto this framing, because Section 4 builds on it.

## 2. N-gram models

The n-gram model makes a Markov assumption: the next token depends only on the previous n-1 tokens.

```
P(w_t | w_1, ..., w_{t-1}) ~= P(w_t | w_{t-n+1}, ..., w_{t-1})
```

Estimation is counting:

```
P(w_t | context) = count(context, w_t) / count(context)
```

A trigram model over a large corpus can be built in an afternoon with a hash map, and until roughly 2012 this family powered production speech recognition and machine translation.
Google's 2007 "large language models in machine translation" work used 5-gram models trained on trillions of tokens, which is a useful reminder that "train on the whole web" predates neural networks.

### Smoothing

Raw counts fail catastrophically: any n-gram unseen in training gets probability zero, and one zero annihilates the whole product.
Smoothing redistributes probability mass to unseen events, and the ladder of techniques is worth knowing because the ideas recur elsewhere.

Laplace (add-one) smoothing adds one to every count.
It is trivially simple but badly miscalibrated for large vocabularies, because it steals far too much mass from seen events.

Backoff (Katz) uses the trigram estimate when the trigram was seen, otherwise falls back to the bigram, then the unigram.
Interpolation (Jelinek-Mercer) always mixes all orders with learned weights, which is usually better than hard backoff.

Kneser-Ney smoothing, the practical state of the art for n-grams, has an idea that survives into the neural era: estimate a word's backoff probability from how many distinct contexts it appears in, not how often it appears.
"Francisco" is frequent but appears almost only after "San", so it should get low probability in novel contexts, even though its raw unigram count is high.
Continuation probability is a primitive form of distributional generalization, which neural embeddings later do properly.

### Why n-grams hit a wall

The failure modes are structural, not fixable by more data.

- Combinatorial sparsity: the number of possible n-grams grows as V^n, so for any realistic vocabulary V most 5-grams that will occur in test data have never occurred in any training corpus, no matter how large.
- No parameter sharing: "the cat sat on the" and "the dog sat on the" are unrelated events to an n-gram model; nothing learned about cats transfers to dogs.
- Hard context ceiling: dependencies longer than n-1 tokens are invisible by construction, so agreement, coreference, and topic coherence beyond a few words are unmodelable.
- Memory scales with data: the model is the count table, so more data means a bigger model with no abstraction.

The lesson to carry forward: generalization requires representing words and contexts in a space where similarity is meaningful, so that evidence about one sequence informs predictions about related sequences.
That is exactly what embeddings provide.

## 3. Neural language models

### Bengio 2003: the feedforward neural LM

Bengio et al. (2003) introduced the architecture that defines the neural approach: map each vocabulary item to a learned dense vector (an embedding), concatenate the embeddings of the last n-1 words, and pass them through a feedforward network with a softmax over the vocabulary.
Two ideas here are permanent.
First, the embedding table shares parameters across contexts, so "cat" and "dog" can end up with nearby vectors and the model generalizes across them; this directly fixes the n-gram sharing failure.
Second, the softmax over the full vocabulary, trained with cross-entropy, is still how every modern LLM produces its output distribution.
The architecture kept the fixed context window of n-grams, so the context ceiling remained.

Word2vec (Mikolov et al., 2013) later showed that embeddings trained on simple objectives capture striking regularities, popularized by the king - man + woman ~= queen example.
Word2vec is not a language model in the generative sense, but it demonstrated that distributional structure is learnable at scale and cheap to learn, which made the field take embeddings seriously as the substrate for everything that followed.

### RNNs: unbounded context in principle

The recurrent neural network (Elman 1990, applied to language modeling by Mikolov et al., 2010) removes the fixed window.
At each step the RNN consumes one token and updates a hidden state:

```
h_t = tanh(W_hh * h_{t-1} + W_xh * x_t + b)
P(w_t+1 | w_1..t) = softmax(W_out * h_t)
```

The hidden state is a fixed-size summary of the entire prefix, so in principle the context is unbounded.
In practice two problems bite.

The first is vanishing and exploding gradients.
Backpropagation through time multiplies Jacobians across steps, so gradient norms shrink or blow up exponentially with distance.
Exploding gradients are treatable with clipping; vanishing gradients mean long-range dependencies receive essentially no learning signal.

The second is the fixed-size bottleneck: every fact about a 10,000-token prefix must be squeezed into one hidden vector, and information is overwritten as new tokens arrive.
This is a lossy, recency-biased compression with no way to retrieve arbitrary earlier detail.

### LSTMs and GRUs

The LSTM (Hochreiter and Schmidhuber, 1997) attacks the vanishing gradient with an additive cell state and multiplicative gates.
The cell state c_t is updated by addition rather than repeated matrix multiplication, giving gradients a protected path backward in time:

```
f_t = sigmoid(W_f [h_{t-1}, x_t])        # forget gate
i_t = sigmoid(W_i [h_{t-1}, x_t])        # input gate
c_t = f_t * c_{t-1} + i_t * tanh(W_c [h_{t-1}, x_t])
o_t = sigmoid(W_o [h_{t-1}, x_t])        # output gate
h_t = o_t * tanh(c_t)
```

The GRU (Cho et al., 2014) is a cheaper two-gate variant with similar practical performance.
LSTMs powered the 2014-2017 era: Google's neural machine translation system, early neural speech systems, and character-level LMs that could generate plausible C code and Wikipedia markup (Karpathy's 2015 "Unreasonable Effectiveness of RNNs" post is the era's best artifact).

LSTMs mitigated gradient decay but kept two structural limits.
The fixed-size state bottleneck remained: gates decide what to forget, but something must be forgotten.
And computation is inherently sequential: h_t cannot be computed before h_{t-1}, so training cannot parallelize across the time dimension, which caps the corpus size you can practically train on.
The transformer's core economic advantage, before any modeling advantage, is that it removed this sequential dependency during training.

### Seq2seq and the birth of attention

Sequence-to-sequence models (Sutskever et al., 2014) encode a source sentence into one vector and decode the target from it, and they made neural machine translation competitive.
They also made the bottleneck vivid: translation quality collapsed on long sentences because one vector cannot hold a paragraph.

Bahdanau et al. (2014) introduced attention as a fix: at each decoding step, compute a weighted average over all encoder hidden states, with weights derived from a learned relevance score between the decoder state and each encoder state.
The decoder now retrieves from the whole source instead of relying on a single compressed summary.
Attention was born as a patch on RNNs.
The transformer's 2017 insight, covered in Chapter 02, was that the patch was the load-bearing part: with attention providing content-based retrieval over the whole context, the recurrence could be deleted entirely.

## 4. Why next-token prediction is so powerful

It is worth being precise about why such a simple objective produces general capability, because sloppy versions of this argument ("it just predicts the next word, so it cannot reason") and equally sloppy counter-arguments are both common.

First, the objective is a universal task container.
Any task whose input and output can be serialized as text is an instance of next-token prediction: translation, summarization, question answering, code synthesis, and tool-call emission are all "predict the continuation of this prefix".
The objective does not need to change per task; only the prefix does.
This is why one pretrained model plus prompting replaced a decade of per-task architectures.

Second, the loss floor forces modeling of the generative process.
Text is produced by people who have knowledge, goals, and reasoning; predicting their output optimally requires modeling those latent causes, per the compression identity of Section 1.
To predict the next token of a chess transcript at the ceiling you must model chess; to predict the last line of a proof you must verify the proof holds.
How deeply current models actually model these latent causes, versus exploiting shallower statistical structure, is a live empirical question; the point here is about the direction the objective pushes, not a claim of achieved perfection.

Third, the objective yields dense supervision at web scale.
Every token of every document is a labeled training example, with no annotation cost.
No other objective in machine learning combines this label density with this data availability, and Chapter 04 shows that data volume is half of the scaling-law story.

Fourth, in-context learning falls out.
Pretraining corpora contain endless documents where a pattern is established and then continued: lists, tables, Q&A pages, parallel translations.
A model good at continuing such documents can be steered by demonstrations at inference time with no weight updates.
This was observed as a surprise in GPT-2 and became the central interface in GPT-3 ("Language Models are Few-Shot Learners", 2020).

The honest caveats, which Chapter 04 and Chapter 05 develop:

- Likelihood training imitates the data distribution, including its errors, biases, and confident-sounding falsehoods; nothing in the objective rewards truth over truthiness.
- The objective is myopic per token, and there is a train-test mismatch called exposure bias: training always conditions on real prefixes, generation conditions on the model's own possibly-flawed output.
- A raw pretrained model is a document continuer, not an assistant; asked a question, it may respond with three more questions, because that is a plausible document.
Post-training exists to close that gap.

## 5. The GPT lineage and the frontier timeline through 2025

Dates and orders of magnitude below are stated as of early 2026.
Parameter counts are cited only where the developer disclosed them; frontier labs stopped disclosing around 2023.

### The GPT line

- GPT-1 (OpenAI, June 2018): a 117M-parameter decoder-only transformer pretrained on BooksCorpus, then fine-tuned per task.
The paper's thesis was generative pretraining as the universal initialization, beating task-specific architectures.
- GPT-2 (February 2019): 1.5B parameters, trained on WebText.
The headline result was zero-shot task performance: translation and summarization emerging without any fine-tuning, purely from the pretraining objective.
The staged release over misuse concerns was the field's first mainstream safety controversy.
- GPT-3 (May 2020): 175B parameters, roughly 300B training tokens.
It established in-context few-shot learning as an interface and validated the Kaplan scaling laws in public.
The API-only release created the "model as a service" business model.
- Codex (2021): GPT-3 fine-tuned on code, powering GitHub Copilot; the proof that code generation was commercially real, and the ancestor of every coding agent in Volume 13.
- InstructGPT (January 2022): the RLHF paper.
A 1.3B instruction-tuned model was preferred by humans over the 175B raw GPT-3, the clearest early evidence that post-training quality can beat raw scale for usefulness.
- ChatGPT (November 30, 2022): productized RLHF chat on the GPT-3.5 series.
Technically incremental, historically pivotal; it made conversational LLMs a consumer category in weeks.
- GPT-4 (March 2023): multimodal input, large capability jump, and a technical report that disclosed neither parameter count nor architecture, marking the industry's turn to secrecy.
- GPT-4 Turbo (late 2023) and GPT-4o (May 2024): longer context, lower price, native multimodality, and much lower latency; the era's theme was making frontier capability cheap and fast rather than just bigger.
- o1 (September 2024): the first production reasoning model, trained with RL to produce long private chains of thought; covered in depth in Chapter 06.
- o3 (announced December 2024, released 2025) and GPT-5 (August 2025): continued the reasoning line; GPT-5 unified fast and reasoning modes behind a router.

### The Claude line

- Claude 1 (Anthropic, March 2023) introduced Constitutional AI training to production.
- Claude 2 (July 2023) pushed long context (100K tokens) before it was standard.
- Claude 3 family (March 2024): Haiku, Sonnet, Opus; the tiered small/medium/large family became the industry norm.
- Claude 3.5 Sonnet (June 2024) plus computer use (October 2024): the first frontier-lab agent controlling a real desktop, foreshadowing Volume 13.
- Claude 3.7 Sonnet (February 2025): extended thinking with a controllable thinking budget.
- Claude 4 Opus and Sonnet (May 2025), then Sonnet 4.5 and Opus 4.5 later in 2025: emphasis on agentic coding, long-horizon task persistence, and interleaved thinking between tool calls.

### The open-weight line

- Llama 1 (Meta, February 2023) leaked and ignited the open-weight ecosystem; Llama 2 (July 2023) made the weights commercially licensed.
- Mistral 7B (September 2023) and Mixtral 8x7B (December 2023) mainstreamed sliding-window attention and mixture-of-experts respectively at open scale.
- Llama 3 and 3.1 (2024) brought open weights to 405B dense parameters and near-frontier quality.
- Qwen (Alibaba) and DeepSeek releases through 2024-2025 made China-origin open weights competitive; DeepSeek-V3 (December 2024) demonstrated frontier-adjacent quality at unusually low disclosed training cost, and DeepSeek-R1 (January 2025) open-sourced an o1-class reasoning model, both covered later in this volume.

### Google's line, briefly

Google invented the transformer (2017) and ran the encoder branch (BERT, 2018; T5, 2019) that dominated NLP benchmarks before decoder-only scaling won.
PaLM (2022), Gemini 1 (December 2023), Gemini 1.5 with million-token context (2024), and Gemini 2.x (2025) mark its convergence onto the same decoder-only, long-context, reasoning-model trajectory as everyone else.

### What to remember from the timeline

The stable pattern across all lineages: pretraining scale-ups from 2018 to 2023, then a pivot to post-training, inference efficiency, and test-time compute from 2024 onward as pretraining returns flattened and data limits approached.
Model names and leaderboard positions in this section will rot; that pattern, and the objective from Section 1, will not.

## Exercises

1. Derive the chain-rule factorization for a three-token sequence and explain why it is exact rather than an approximation, and where the approximation enters for an n-gram model.
2. Train a trigram character-level model with add-one smoothing on a book from Project Gutenberg (pure Python, a dict of counts is enough).
Sample 500 characters from it, compute its per-character perplexity on a held-out chapter, and identify three generated artifacts that demonstrate the Markov horizon.
3. Implement Kneser-Ney continuation counts for your corpus and show one concrete word (like "Francisco") whose continuation probability diverges sharply from its unigram probability.
4. Write the backpropagation-through-time gradient for a linear RNN h_t = W h_{t-1} + x_t and show that the gradient of the loss at step T with respect to h_1 contains W^(T-1).
State the spectral condition on W under which the gradient vanishes.
5. Explain the LSTM cell-state update as a mitigation of your answer to exercise 4, and explain why the mitigation still cannot give the model random access to token 3 of a 10,000-token prefix.
6. Take any task you have shipped with an LLM (extraction, code review, summarization) and write out explicitly how it reduces to next-token prediction, including where the task specification lives in the token sequence.
7. Compute, from the compression identity, the compressed size in bytes of a 1M-token corpus under a model with 1.2 bits per token cross-entropy, and compare it to gzip's typical ratio on English text (measure gzip yourself; do not trust a remembered number).

## Godhood check

You are ready for Chapter 02 when you can do all of the following without notes.

- State the language modeling objective and the chain-rule factorization, and explain the exact location of the Markov approximation in an n-gram model.
- Explain why smoothing is necessary, and what Kneser-Ney's context-diversity idea anticipates about embeddings.
- Give the two structural failures of n-grams (sparsity without sharing, hard context ceiling) and map each to the neural mechanism that fixed it.
- Explain vanishing gradients through the repeated-Jacobian argument and how the LSTM's additive cell state addresses it.
- Name the two limits that survived into LSTMs (state bottleneck, sequential training) and which one the transformer's parallelism addressed first.
- Argue both directions of the next-token-prediction debate: why the objective pushes toward modeling latent causes of text, and what the objective genuinely does not provide (truthfulness, non-myopia, assistant behavior).
- Reconstruct the GPT lineage with approximate dates from 2018 to 2025 and state the field-level pivot that happened around 2024.
