# Chapter 03 - Tokenization

## What you will master

- Why models operate on subword tokens rather than characters or words, and what that choice trades away.
- The BPE algorithm executed by hand on a worked example, plus byte-level BPE as used by GPT-class models.
- WordPiece and the SentencePiece/Unigram approach, and how they differ from BPE in objective and behavior.
- Vocabulary size trade-offs and why vocabularies grew from 32K to 100K-256K across model generations.
- The tokenization root causes behind famous model failures: arithmetic, spelling, code indentation, multilingual cost, and glitch tokens.
- Token economics for agent builders: how tokenizer behavior turns directly into latency and dollars.

## 1. Why subwords

A language model needs a finite vocabulary of symbols to predict over.
The two pure options both fail.

Word-level vocabularies explode (every name, typo, and inflection is a new type), cannot represent unseen words at all, and waste capacity on rare words seen too few times to learn.
Character-level vocabularies are tiny and never see an out-of-vocabulary symbol, but sequences become 4-5x longer in English, which under quadratic attention and finite context is expensive, and the model must relearn from scratch that "t-h-e" is a unit.

Subword tokenization interpolates: frequent strings become single tokens, rare strings decompose into smaller known pieces, and in the limit any input falls back to characters or bytes.
Nothing is ever out of vocabulary, common text is short, and vocabulary size is a chosen hyperparameter.
The cost, developed in Section 5, is that the model sees text through an arbitrary, frequency-driven segmentation, and a whole family of failures traces to exactly that.

Terminology used throughout: a token is an element of the vocabulary; tokenization maps a string to a token-id sequence; as a rough English-only rule of thumb for GPT-class tokenizers, one token averages about 4 characters or 0.75 words.
That rule of thumb is English-specific, and Section 5.4 shows how badly it breaks elsewhere.

## 2. Byte-pair encoding

### The algorithm

BPE (introduced for NMT by Sennrich et al., 2016, adapting a 1994 compression scheme) learns a vocabulary by greedy frequency merging.

1. Start with a base vocabulary of individual characters (or bytes) and represent every word in the training corpus as a sequence of base symbols.
2. Count all adjacent symbol pairs across the corpus.
3. Merge the most frequent pair into a new single symbol, and add that merge rule to an ordered list.
4. Repeat until the vocabulary reaches the target size.

Encoding new text replays the learned merge rules in order of learned priority.
Decoding is trivial concatenation, which is a real virtue of BPE: detokenization is exact and unambiguous.

### Worked example

Take the classic toy corpus, with word frequencies, and words split into characters plus an end-of-word marker "_":

```
l o w _        (5)
l o w e r _    (2)
n e w e s t _  (6)
w i d e s t _  (3)
```

Iteration 1: count pairs.
The pair (e, s) occurs in "newest" (6) and "widest" (3), total 9, the maximum.
Merge to "es".

Iteration 2: (es, t) now occurs 9 times.
Merge to "est".

Iteration 3: (est, _) occurs 9 times.
Merge to "est_".

Iteration 4: (l, o) occurs in "low" (5) and "lower" (2), total 7.
Merge to "lo".

Iteration 5: (lo, w) occurs 7 times.
Merge to "low".

After five merges the learned subwords include "est_" and "low", so "lowest", a word never seen in training, encodes as "low" + "est_": two known units with meaningful statistics.
This is the entire point of BPE: compositional coverage of unseen words from frequent fragments.
Notice also the arbitrariness: "widest" ends up as "w i d est_", so "wid" is not a unit even though "low" is, purely because of corpus frequencies.
Every real tokenizer is full of such frequency accidents, and the model has to live with them.

### Byte-level BPE

GPT-2 (2019) introduced byte-level BPE, now standard: the base alphabet is the 256 byte values, and merges are learned over UTF-8 bytes.
Any string in any language, plus arbitrary binary junk, tokenizes without an unknown-token escape hatch, which matters enormously for robustness on web data.
The cost is that a character outside the merge table can cost multiple tokens (one per byte), and byte-level merges can split inside a UTF-8 multi-byte character, so a token boundary can fall in the middle of a Unicode code point.
Practical detail: GPT-class tokenizers include the leading space in tokens, so " world" and "world" are different tokens with different ids, which is why prompts that end with a trailing space often degrade completions: the model is pushed off the natural " word" token path.

## 3. WordPiece, Unigram, and SentencePiece

### WordPiece

WordPiece (used by BERT, 2018) differs from BPE in the merge criterion: instead of merging the most frequent pair, it merges the pair that most increases the training-data likelihood under a unigram model over the current vocabulary, which normalizes pair frequency by the frequencies of the parts.
Effect: BPE favors raw-frequent pairs; WordPiece favors pairs that co-occur more than chance predicts.
BERT-style WordPiece also marks word-internal continuation pieces with "##", as in "playing" -> "play", "##ing".

### Unigram language model tokenization

The Unigram method (Kudo, 2018) inverts the construction: start from a large candidate vocabulary, assign each piece a probability, and iteratively prune pieces whose removal least damages the corpus likelihood, using EM to fit piece probabilities.
Encoding picks the segmentation with the highest product of piece probabilities via Viterbi.
The practically important consequence is that Unigram is probabilistic over segmentations, which enables subword regularization: sampling different valid segmentations of the same text during training as a robustness augmentation.
BPE has a deterministic-by-merge-order segmentation and needed a separate trick (BPE-dropout, 2020) to get the same effect.

### SentencePiece

SentencePiece (Kudo and Richardson, 2018) is a library, not an algorithm; it implements both BPE and Unigram.
Its distinctive design choice is treating the input as a raw character stream with no pre-tokenization on whitespace: spaces are converted to a visible meta symbol (U+2581, the underscore-like block) and participate in learning like any character.
This makes tokenization fully reversible and language-agnostic, which is why models targeting languages without spaces (Japanese, Chinese, Thai) and most open models (Llama 1 and 2, T5, Gemma lineage) adopted it.
As of early 2026 the ecosystem splits roughly into tiktoken-style byte-level BPE (OpenAI lineage, Llama 3 onward) and SentencePiece (T5, Llama 1/2, Gemma lineage), and you should check which you are dealing with before reasoning about token counts.

## 4. Vocabulary size trade-offs

Vocabulary size V is a real design decision with pressure in both directions.

Larger V means shorter sequences: more text fits in a fixed context window, attention does less work per document, and each forward pass covers more characters, all of which reduce cost per unit of text.
Larger V also gives frequent words and code idioms dedicated tokens with dedicated learned representations.

The costs of larger V:

- The embedding and unembedding matrices scale as V times d_model, and for small models this becomes a dominant parameter share, stealing capacity from layers.
- Rare tokens are seen few times and remain undertrained, which is the direct cause of glitch tokens (Section 5.5).
- The final softmax over V grows linearly in compute and in logit memory.

The historical trajectory reflects the balance shifting as models grew: GPT-2/GPT-3 used about 50K, Llama 1 and 2 used 32K, GPT-4 lineage moved to about 100K, Llama 3 to 128K, and Gemma and several 2024-2025 models to 256K.
The push upward came from two directions: model sizes grew so the embedding-share cost shrank in relative terms, and multilingual plus code coverage rewards a bigger merge table far more than English prose does.

## 5. Why tokenization causes weird failures

This section is the practical payoff of the chapter.
A large fraction of "the model is stupid" reports are really "the model cannot see what you think it sees", because the model perceives token ids, not characters.

### 5.1 Arithmetic

Number tokenization is frequency-driven and therefore inconsistent: in GPT-2-era vocabularies, "17" might be one token while "171" splits as "17" + "1" and "1234567" splits into irregular chunks.
Digit alignment, the thing column-wise addition depends on, is invisible when numbers are chunked irregularly, so the model must memorize arithmetic over an inconsistent chunking rather than learn a positional algorithm.
Mitigations adopted since: Llama-era tokenizers force single-digit tokens or fixed three-digit groups for numbers, which measurably improves arithmetic; and frontier systems increasingly route real math through code execution tools, which is the correct engineering answer.
For agents, the rule is simple: never let the model do load-bearing arithmetic in its head; give it a calculator or interpreter tool, both because of tokenization and because next-token prediction gives no carry-checking guarantee.

### 5.2 Spelling and character-level tasks

"How many r's are in strawberry" became the canonical 2024 example of frontier models failing a trivial task.
The cause is direct: "strawberry" is one or two tokens, and the model has no runtime access to the character decomposition of a token id; knowing token 302 contains three r's is a memorized fact, not an observation.
The same mechanism breaks reversing strings, counting letters, acrostics, precise rhyming, and character-offset manipulation.
Reasoning-trained models (Chapter 06) do better by spelling the word out letter by letter in their thinking, which converts the task into one over letter tokens the model can see.
For agents: any character-precise transformation (regex construction over exact strings, fixed-width formats, checksums) should be done in a tool, not by generation.

### 5.3 Code and indentation

Whitespace is syntax in Python and YAML, and tokenizers handle runs of spaces in learned, uneven chunks.
GPT-2's tokenizer had no multi-space tokens, so an 8-space indent cost 8 tokens and code was cripplingly expensive; modern tokenizers learned dedicated tokens for common indent runs, which fixed the cost but means indentation arithmetic happens over irregular units.
Consequences you will observe when building coding agents: off-by-one indent errors when generating deeply nested code, tabs versus spaces being entirely different token sequences, and diffs or exact-match edits failing over invisible whitespace differences.
This is one reason string-replacement edit tools in coding agents (Volume 13) demand exact literal matches and why agents are instructed to re-read files rather than trust remembered formatting.

### 5.4 Multilingual inefficiency

A tokenizer trained on English-heavy data gives English short encodings and everything else long ones.
On GPT-class tokenizers of the 2023 era, the same content in Burmese, Amharic, or Khmer could cost several times the tokens of its English equivalent, with the ratio exceeding 10x in the worst-studied cases; Latin-script European languages typically sat at 1.5-2.5x.
This is simultaneously a cost multiplier, a latency multiplier, an effective-context reducer, and a quality tax (fragmented text is harder to model) for non-English users.
Vocabulary growth to 128K-256K in 2024-2025 models was substantially about closing this gap.
If you build agents for non-English markets, measure token ratios on your actual language mix with the actual tokenizer before pricing anything; this is a one-line script and it will change your cost model.

### 5.5 Glitch tokens

If a string is frequent enough in tokenizer training data but its documents are later filtered out of model training data, the token exists but its embedding is never meaningfully updated.
The famous examples are the "SolidGoldMagikarp" family, discovered in GPT-2/GPT-3 vocabularies (published analysis in 2023): Reddit usernames and log artifacts that as tokens caused models to misbehave bizarrely, from being unable to repeat the string to producing unrelated or hostile output.
Root cause: tokenizer corpus and training corpus were different, so some embeddings are effectively untrained noise the network never learned to handle.
Modern pipelines reduce this by aligning corpora and pruning near-dead tokens, but the class of bug is permanent in kind: any pipeline where the vocabulary and the training distribution drift apart can mint undertrained tokens.

### 5.6 Boundary and adversarial effects

Token boundaries create seams with security consequences: a blocked word can be smuggled past naive string filters by forcing an unusual segmentation, or conversely, safety-relevant patterns can fail to match because the model never sees the surface string.
Prompt-injection defense (Volume 11) must therefore operate on decoded text and model behavior, never on raw token patterns.

## 6. Token economics for agent builders

Everything an agent does is metered in tokens, so tokenizer behavior is a first-order line item.
Claims here are stated as of early 2026; check current pricing pages before quoting numbers, but the structural points are stable.

The billing structure that matters:

- Input (prompt) tokens and output (completion) tokens are priced separately, and output tokens are typically several times more expensive than input tokens because decode is the expensive serving phase (Chapter 07 explains why).
- Agents are token-hungry by construction: each loop iteration typically resends the system prompt, tool definitions, and accumulated history, so naive context handling makes cost quadratic-ish in conversation length; prompt caching (Chapter 07) and context compaction (Volume 06) exist to fight exactly this.
- Reasoning models add thinking tokens billed as output, which can multiply cost per step; Chapter 06 covers when that spend is justified.

Concrete practices that follow directly from tokenizer mechanics:

- Measure, never estimate: count tokens with the model's own tokenizer (tiktoken for OpenAI-lineage models; Anthropic exposes a count-tokens API endpoint since its tokenizer is not published).
Estimating with the wrong tokenizer routinely misprices by tens of percent.
- Design tool outputs for token economy: compact tables beat pretty-printed JSON (every quote, brace, and space costs), truncate large results with an escape hatch to fetch more, and strip decorative formatting from anything an agent will read in a loop.
- Budget context by token, not by "message count": a single pasted log file can dwarf fifty conversation turns.
- Mind stop-sequence and formatting choices: forcing rigid output formats that fight the tokenizer's natural segmentation (fixed-width padding, exotic delimiters) costs tokens and accuracy simultaneously.
- Verbosity is a controllable cost: system-prompt instructions toward terse output are among the highest-ROI cost optimizations available, since they cut the expensive token class (output) directly.

A worked order-of-magnitude example, arithmetic only, no vendor prices baked in.
Suppose an agent loop carries a 6,000-token system-plus-tools prefix, accumulates 2,000 tokens of history per step, runs 10 steps, and emits 500 output tokens per step.
Without caching, total input tokens are roughly sum over steps of (6000 + step * 2000), about 170K input tokens, plus 5K output tokens, and the input side dominates the meter even at a several-fold lower per-token price.
Cache the shared prefix and compact the history, and the same task can drop by a large integer factor.
This arithmetic, not model choice, is usually the difference between a viable and a non-viable agent product, and it is why Chapters 06 and 07 of this volume plus all of Volume 06 keep returning to context economics.

## Exercises

1. Execute the BPE worked example of Section 2 entirely by hand for eight merges, writing the merge table and the final segmentation of every corpus word, then verify by implementing the trainer in about 50 lines of Python.
2. Using your trained toy tokenizer, encode "lowest", "newer", and "wider", and explain each segmentation from the merge table, including one case where the segmentation is linguistically wrong but frequency-correct.
3. Install tiktoken and compare token counts across an English paragraph, its machine translation into two non-Latin-script languages, a JSON blob, the same data as a compact TSV, and a 40-line Python file, using both an older (GPT-2) and newer (GPT-4-era) encoding; tabulate the ratios and write three sentences on what changed between generations.
4. Reproduce a character-blindness failure and its fix: ask any current model to count letter occurrences in five words directly, then again with the instruction to first write the word letter by letter separated by spaces; report accuracy in both conditions and explain the mechanism in tokenizer terms.
5. Take a real tool output from any agent you use (a search result blob or API response), and produce a token-minimized redesign preserving all load-bearing information; measure the savings with a real tokenizer and state the percentage.
6. Write a Unigram-tokenizer Viterbi encoder: given a piece vocabulary with log probabilities, segment an input string optimally; demonstrate on a small vocabulary where greedy longest-match and Viterbi disagree.
7. Estimate the embedding-parameter share for V = 32K versus V = 256K at d_model = 2048 (a small model) and d_model = 8192 (a large model), with tied embeddings, and use the four numbers to explain why large vocabularies became affordable as models grew.

## Godhood check

You are ready for Chapter 04 when you can do all of the following without notes.

- Argue the subword compromise from both directions: what breaks with word-level and with character-level vocabularies.
- Run BPE by hand on a toy corpus and state what byte-level BPE adds and what it complicates.
- Contrast BPE, WordPiece, and Unigram by their selection criteria in one sentence each, and say what SentencePiece actually is.
- Give both directions of the vocabulary-size trade-off and the historical trajectory from 32K to 256K with its two drivers.
- For each of arithmetic, spelling, code indentation, multilingual cost, and glitch tokens: state the failure, the tokenizer-level root cause, and the engineering mitigation.
- Explain why output tokens price higher than input tokens (you may forward-reference Chapter 07) and compute the token bill of a multi-step agent loop from its structure.
- State the three tokenizer-driven rules you will apply to every agent you build: count with the real tokenizer, design tool output for token economy, and route character-precise or arithmetic work to tools.
