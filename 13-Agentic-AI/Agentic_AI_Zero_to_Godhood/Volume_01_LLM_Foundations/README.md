# Volume 01 - LLM Foundations

Everything in agent engineering bottoms out in how the underlying model works; this volume builds that foundation from the language modeling objective to serving physics.
Claims that will rot (model names, prices, API shapes) are date-stamped as of early 2026 throughout.

## Chapters

- [Chapter 01 - From N-Grams to Neural Language Models](Chapter_01_From_N_Grams_to_Neural_Language_Models.md): the language modeling objective as compression, n-grams and their structural limits, RNNs and LSTMs, why next-token prediction yields general capability, and the frontier-model timeline through 2025.
- [Chapter 02 - The Transformer From Scratch](Chapter_02_The_Transformer_From_Scratch.md): attention derived from first principles with the math, Q/K/V, multi-head attention, RoPE and other positional schemes, norm placement, the modern decoder-only stack, MQA/GQA/sliding-window/MoE variants, and a runnable NumPy and PyTorch implementation.
- [Chapter 03 - Tokenization](Chapter_03_Tokenization.md): BPE, WordPiece, and Unigram/SentencePiece with worked examples, vocabulary-size trade-offs, the tokenizer root causes of arithmetic, spelling, and indentation failures, and token economics for agent builders.
- [Chapter 04 - Pretraining and Scaling Laws](Chapter_04_Pretraining_and_Scaling_Laws.md): web-scale data pipelines, training objectives, 6ND FLOPs accounting, Kaplan versus Chinchilla and the inference-aware correction, the data wall and annealing curricula, and the contract of what pretraining gives and cannot give.
- [Chapter 05 - Post-Training](Chapter_05_Post_Training.md): SFT, chat templates, reward models, RLHF with PPO explained at the level of the update, DPO and its variants, RLAIF and Constitutional AI, and why agent behavior is chiefly a post-training artifact.
- [Chapter 06 - Reasoning Models](Chapter_06_Reasoning_Models.md): chain-of-thought as serial computation in the token stream, RL on verifiable rewards and the o1/R1 recipe, GRPO, test-time compute scaling, extended and interleaved thinking in the Claude line, and when reasoning helps agents versus wastes tokens.
- [Chapter 07 - Inference Mechanics](Chapter_07_Inference_Mechanics.md): the KV cache and its size arithmetic, prefill versus decode via arithmetic intensity, continuous batching and PagedAttention, speculative decoding, quantization, the latency and cost model, and prompt caching discipline; the foundation for the production volumes.
