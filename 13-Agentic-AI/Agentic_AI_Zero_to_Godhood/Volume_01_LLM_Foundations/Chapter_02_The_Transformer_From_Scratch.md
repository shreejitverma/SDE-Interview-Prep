# Chapter 02 - The Transformer From Scratch

## What you will master

- Scaled dot-product attention derived from first principles, including why the scaling factor is sqrt(d_k).
- Queries, keys, and values as a differentiable key-value store, and multi-head attention as parallel retrieval subspaces.
- Positional encodings: sinusoidal, learned, RoPE in full detail, and ALiBi, with the trade-offs among them.
- Layer normalization placement (post-LN vs pre-LN), RMSNorm, and why placement determines trainability at depth.
- The complete decoder-only stack as used by every modern frontier model.
- The efficiency variants that define current architectures: MQA, GQA, sliding-window attention, and mixture-of-experts.
- A minimal, runnable NumPy and PyTorch implementation of causal multi-head attention.

## 1. The problem attention solves

Chapter 01 ended with two unfixed problems: the RNN's fixed-size state bottleneck and its sequential training.
Attention (Vaswani et al., "Attention Is All You Need", 2017) solves both with one mechanism.
Instead of compressing the prefix into one vector, keep a representation of every position, and let each position retrieve from all others by content.
Because each position's output depends on the other positions only through matrix products, the whole sequence is processed in parallel during training.
The price is quadratic cost in sequence length and, at inference, a cache that grows with the context; Chapter 07 is largely about paying that bill.

## 2. Attention from first principles

Start from the retrieval framing.
You have a query vector q describing what a position wants, and a set of key vectors k_i each paired with a value vector v_i.
A hard lookup would return the value whose key exactly matches the query, but hard lookups are not differentiable.
The differentiable relaxation is a softmax-weighted average of all values, weighted by query-key similarity:

```
score_i = q . k_i / sqrt(d_k)
alpha = softmax(score_1, ..., score_n)
output = sum_i alpha_i * v_i
```

Dot product is the similarity function because it is cheap (one matmul for all pairs) and because learned projections can shape the space so that dot product means whatever the model needs it to mean.

### Why divide by sqrt(d_k)

Assume the components of q and k are independent with zero mean and unit variance.
Then the dot product q . k is a sum of d_k such products, so it has mean 0 and variance d_k, meaning its typical magnitude grows as sqrt(d_k).
Softmax of large-magnitude inputs saturates: one weight goes to 1, the rest to 0, and the gradient through the softmax collapses toward zero.
Dividing by sqrt(d_k) restores unit variance at initialization, keeping the softmax in its trainable regime.
This is a one-line change that materially affects whether large models train, and it is the same class of reasoning (control activation statistics at init) that motivates every normalization choice in this chapter.

### Q, K, V as learned projections

In self-attention, queries, keys, and values are all linear projections of the same input matrix X (shape: sequence length n by model dimension d_model):

```
Q = X W_Q    K = X W_K    V = X W_V
Attention(Q, K, V) = softmax(Q K^T / sqrt(d_k)) V
```

The three projections give the model three independent degrees of freedom: what each position asks for (W_Q), what each position advertises (W_K), and what each position hands over when selected (W_V).
Collapsing them (for example using X directly as all three) forces "what I am looking for" and "what I contain" into the same vector, which measurably hurts; the separation is what makes attention a general-purpose routing mechanism rather than a similarity blur.

### The causal mask

A language model must not let position t see positions greater than t, or the training objective leaks the answer.
The fix is to add negative infinity to the masked entries of the score matrix before the softmax, which zeroes their weights:

```
scores[i, j] = -inf  for all j > i
```

This mask is why decoder-only transformers can compute the loss at every position of a sequence in one forward pass: position t's prediction uses exactly the prefix up to t, so one document of length n yields n training signals simultaneously.
This "every token is a training example, in parallel" property is the economic foundation of large-scale pretraining.

## 3. Multi-head attention

A single attention pattern per layer is a bottleneck: the softmax produces one distribution over positions, so one position can implement one retrieval pattern at a time.
Multi-head attention runs h independent attention operations in parallel on projected subspaces of dimension d_k = d_model / h, then concatenates the outputs and mixes them with a final projection W_O:

```
head_i = Attention(X W_Q_i, X W_K_i, X W_V_i)
MultiHead(X) = concat(head_1, ..., head_h) W_O
```

Because d_k shrinks as h grows, multi-head costs roughly the same FLOPs as single-head at the same d_model; you are buying diversity of attention patterns, not extra compute.
Interpretability work (notably Anthropic's induction-head analyses, 2021-2022) shows heads specializing: previous-token heads, syntactic heads, and induction heads that find a previous occurrence of the current token and copy what followed it, a mechanism strongly implicated in in-context learning.
The trade-off of more heads at fixed d_model is smaller per-head dimension, which limits how much information each retrieval can carry; frontier models settle in the tens-of-heads range as a balance.

## 4. Positional encodings

Attention is permutation-equivariant: shuffle the input positions and the outputs shuffle identically, because nothing in Q K^T depends on order.
Word order carries meaning, so position must be injected explicitly.

### Sinusoidal and learned absolute positions

The original transformer added a fixed sinusoidal vector to each token embedding, with wavelengths forming a geometric progression from 2*pi to 10000*2*pi.
The stated motivation was that any fixed offset becomes a linear transformation of the encoding, making relative position easy for the model to compute.
GPT-1 through GPT-3 instead used learned absolute position embeddings: a trainable vector per position index.
Learned embeddings are simple and slightly better in-distribution, but they define nothing beyond the trained maximum length, and both absolute schemes generalize poorly past training length.

### RoPE in detail

Rotary Position Embedding (RoPE, Su et al., 2021) is the de facto standard as of early 2026, used by Llama, Qwen, DeepSeek, and most open frontier-class models.
The idea: instead of adding position to the token embedding, rotate the query and key vectors by a position-dependent angle before the dot product.

Treat each consecutive pair of dimensions (x_1, x_2), (x_3, x_4), ... as 2D planes.
In plane j, rotate by angle m * theta_j for a token at position m, where theta_j = base^(-2j/d) and base is conventionally 10000:

```
[x'_1]   [cos(m*theta_j)  -sin(m*theta_j)] [x_1]
[x'_2] = [sin(m*theta_j)   cos(m*theta_j)] [x_2]
```

The key property: the dot product between a query rotated by angle m*theta and a key rotated by angle n*theta depends only on the difference m - n, because rotations compose as R(m)^T R(n) = R(n - m).
So RoPE injects absolute position into each vector but the attention score sees only relative position, which is what language structure actually cares about.
The geometric spread of theta_j gives fast-rotating planes that resolve nearby positions precisely and slow-rotating planes that distinguish coarse long-range structure.

Practical consequences that matter for agent engineers:

- Context extension: pretrained RoPE models can be stretched to longer contexts by rescaling the rotation frequencies (position interpolation, NTK-aware scaling, and YaRN, 2023), usually with a short fine-tune; this is how many "128K context" variants of 8K-trained models were produced.
- The rotation applies to Q and K only, never V, and it commutes with the KV-cache mechanics of Chapter 07 because each cached key is stored already rotated for its absolute position.

### ALiBi and no-position approaches

ALiBi (Press et al., 2022) skips embeddings entirely and adds a linear penalty proportional to key-query distance directly to attention scores, with a different slope per head.
It extrapolates to longer sequences gracefully and is cheap, but it hard-codes a recency bias that limits precise long-range retrieval, and it lost the ecosystem battle to RoPE.
Know it because it clarifies the design space: position can live in the embeddings, in the Q/K geometry (RoPE), or in the score bias (ALiBi).

## 5. Normalization and residual structure

### The residual stream

Every sublayer (attention or MLP) is wrapped as x + Sublayer(x).
The residual stream framing, standard in interpretability work, views the stream as a shared communication bus: each sublayer reads from it, computes, and writes an additive update.
Residuals give gradients an identity path to early layers, which is the second half (after normalization) of why hundred-layer transformers train at all.

### Post-LN vs pre-LN

The 2017 transformer used post-LN: LayerNorm(x + Sublayer(x)).
Post-LN normalizes the stream after each addition, which keeps activation scales tight but places the normalization inside the gradient path, and deep post-LN transformers fail to train without careful learning-rate warmup.
Pre-LN, x + Sublayer(LayerNorm(x)), normalizes only the sublayer input and leaves the residual path untouched, giving a clean identity gradient path from loss to embeddings.
Pre-LN trains stably at great depth and is what GPT-2 onward and essentially all modern models use.
The trade-off is real: post-LN, when it can be stabilized, tends to reach slightly better final loss because pre-LN allows the residual stream norm to grow with depth, diluting later layers' relative contribution; hybrid schemes exist, but pre-LN's stability won.

### RMSNorm

LayerNorm subtracts the mean and divides by the standard deviation, then applies a learned gain and bias.
RMSNorm (Zhang and Sennrich, 2019) drops mean subtraction and the bias, dividing by the root mean square alone:

```
RMSNorm(x) = x / sqrt(mean(x^2) + eps) * g
```

It is cheaper, and ablations in Llama-class models found no quality loss, so it is now the default in open frontier-class architectures.
The lesson is general: components survive on measured contribution, and the transformer of 2025 is the 2017 design with every part swapped by exactly this kind of ablation.

## 6. The decoder-only stack, end to end

A modern decoder-only transformer, as of early 2026, is:

1. Token embedding lookup: token ids to vectors of size d_model.
2. N identical blocks, each computing (pre-norm form): x = x + Attention(Norm(x)) with causal masking and RoPE, then x = x + MLP(Norm(x)).
3. A final normalization, then an unembedding projection to vocabulary logits, often weight-tied to the embedding matrix.

The MLP is typically a SwiGLU variant (Shazeer, 2020): down(silu(gate(x)) * up(x)), with hidden width around 2.7x d_model to keep parameters comparable to the classic 4x ReLU MLP.
SwiGLU won the same way RMSNorm did: consistent small ablation gains at scale.
Roughly two thirds of a dense model's non-embedding parameters live in the MLPs, one third in attention; interpretability evidence associates MLPs with key-value-like factual storage and attention with routing, though the division is not clean.

The original 2017 model was an encoder-decoder for translation; BERT (2018) took the encoder alone for bidirectional understanding tasks.
Decoder-only won for generative frontier models because it is the simplest architecture matching the next-token objective, every token contributes a loss signal in parallel, and one stack serves both understanding and generation.
Encoder models survive where they are structurally better: embeddings and rerankers for retrieval, covered in Volume 05.

## 7. Efficiency variants that define modern models

### MQA and GQA

At inference, decoding is dominated by reading the KV cache from memory (Chapter 07 quantifies this).
Multi-Query Attention (Shazeer, 2019) keeps h query heads but shares a single K and V head across all of them, shrinking the KV cache by a factor of h.
The quality cost is measurable: one shared key space limits retrieval diversity.
Grouped-Query Attention (Ainslie et al., 2023) interpolates: g KV heads shared among h query heads (for example Llama 3 70B uses 64 query heads and 8 KV heads).
GQA at small g recovers nearly all of full multi-head quality while keeping most of MQA's memory savings, and it is the default in serious open models as of early 2026.
This is a pure inference-economics feature; it exists because of the memory-bandwidth analysis in Chapter 07, and it is a good example of serving constraints flowing backward into architecture.

### Sliding-window and hybrid attention

Sliding-window attention restricts each position to attend to the previous w positions, making per-layer cost linear in sequence length and capping that layer's KV cache at w.
Stacked layers give an effective receptive field of w times depth, since information can hop window by window.
Mistral 7B (2023) mainstreamed it; Gemma 2 and 3 (2024-2025) interleave local sliding-window layers with periodic global layers, which is the current standard compromise: most layers cheap and local, a few layers providing direct long-range retrieval.
The trade-off is that multi-hop relay through windows is lossier than direct attention, so pure sliding-window models degrade on precise long-range recall, which is why the hybrid pattern won.

### Mixture-of-experts

MoE replaces each dense MLP with E expert MLPs and a learned router that sends each token to the top-k experts (k is typically 1 or 2 for classic designs, higher with many small experts in 2024-2025 designs like DeepSeek-V3).
Outputs of the chosen experts are combined weighted by router scores.
The point is to decouple parameter count from per-token FLOPs: an MoE can have the knowledge capacity of a huge dense model while spending the compute of a small one per token.
Landmarks: Switch Transformer (Google, 2021) at scale, Mixtral 8x7B (2023) proving open MoE quality, DeepSeek-V3 (December 2024) with fine-grained experts plus a shared expert, and GPT-4 widely reported (never confirmed by OpenAI) to be MoE.

The costs are operational and real.
All experts must sit in memory even though few activate, so MoE trades cheap FLOPs for expensive capacity, which suits high-throughput serving more than single-user local deployment.
Routing must be load-balanced (via auxiliary losses or bias adjustments) or experts collapse; batches must be shuffled across experts, complicating distributed training and inference; and per-token quality is more variance-prone than dense models at equal average loss.
For an agent engineer the visible consequence is economic: MoE is a large part of why frontier-quality tokens got cheap between 2023 and 2025.

## 8. Minimal implementation

The NumPy version states the math with nothing hidden.
Verified conceptually against the equations above; shapes are annotated so you can check every step.

```python
import numpy as np

def softmax(x, axis=-1):
    x = x - x.max(axis=axis, keepdims=True)  # stability: softmax is shift-invariant
    e = np.exp(x)
    return e / e.sum(axis=axis, keepdims=True)

def causal_self_attention(X, W_Q, W_K, W_V, W_O, n_heads):
    """X: (n, d_model). Weight matrices: (d_model, d_model). Returns (n, d_model)."""
    n, d_model = X.shape
    d_k = d_model // n_heads

    def split(M):  # (n, d_model) -> (n_heads, n, d_k)
        return M.reshape(n, n_heads, d_k).transpose(1, 0, 2)

    Q, K, V = split(X @ W_Q), split(X @ W_K), split(X @ W_V)

    scores = Q @ K.transpose(0, 2, 1) / np.sqrt(d_k)     # (n_heads, n, n)
    mask = np.triu(np.ones((n, n), dtype=bool), k=1)      # True above diagonal
    scores = np.where(mask, -1e30, scores)                # forbid attending to the future

    out = softmax(scores) @ V                             # (n_heads, n, d_k)
    out = out.transpose(1, 0, 2).reshape(n, d_model)      # concat heads
    return out @ W_O

rng = np.random.default_rng(0)
n, d_model, n_heads = 8, 64, 4
X = rng.standard_normal((n, d_model)) / np.sqrt(d_model)
Ws = [rng.standard_normal((d_model, d_model)) / np.sqrt(d_model) for _ in range(4)]
Y = causal_self_attention(X, *Ws, n_heads)
assert Y.shape == (n, d_model)
```

The PyTorch version adds the pieces a real block needs, in the pre-norm arrangement (PyTorch 2.x API, current as of early 2026).

```python
import torch
import torch.nn as nn
import torch.nn.functional as F

class CausalSelfAttention(nn.Module):
    def __init__(self, d_model: int, n_heads: int):
        super().__init__()
        assert d_model % n_heads == 0
        self.n_heads, self.d_k = n_heads, d_model // n_heads
        self.qkv = nn.Linear(d_model, 3 * d_model, bias=False)
        self.proj = nn.Linear(d_model, d_model, bias=False)

    def forward(self, x: torch.Tensor) -> torch.Tensor:  # x: (batch, n, d_model)
        B, n, d = x.shape
        q, k, v = self.qkv(x).chunk(3, dim=-1)
        # (B, n, d) -> (B, n_heads, n, d_k)
        shape = (B, n, self.n_heads, self.d_k)
        q, k, v = (t.view(shape).transpose(1, 2) for t in (q, k, v))
        # Fused kernel; is_causal applies the triangular mask internally.
        out = F.scaled_dot_product_attention(q, k, v, is_causal=True)
        out = out.transpose(1, 2).reshape(B, n, d)
        return self.proj(out)

class Block(nn.Module):
    def __init__(self, d_model: int, n_heads: int):
        super().__init__()
        self.norm1 = nn.RMSNorm(d_model)
        self.attn = CausalSelfAttention(d_model, n_heads)
        self.norm2 = nn.RMSNorm(d_model)
        hidden = int(8 * d_model / 3)  # SwiGLU sizing convention
        self.gate = nn.Linear(d_model, hidden, bias=False)
        self.up = nn.Linear(d_model, hidden, bias=False)
        self.down = nn.Linear(hidden, d_model, bias=False)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x = x + self.attn(self.norm1(x))                       # pre-norm residual
        h = self.norm2(x)
        return x + self.down(F.silu(self.gate(h)) * self.up(h))  # SwiGLU MLP
```

What is deliberately omitted so the skeleton stays readable: RoPE application to q and k, GQA (fewer KV heads than Q heads), the KV cache for incremental decoding, and dropout.
Exercise 5 has you add the first three; production attention additionally uses fused kernels in the FlashAttention family, which compute exact attention without materializing the n-by-n score matrix in memory.

## Exercises

1. Prove the variance claim behind sqrt(d_k) scaling: for independent zero-mean unit-variance components, show Var(q . k) = d_k, then verify empirically in NumPy at d_k in {16, 256, 4096} by plotting the pre-softmax score distribution with and without scaling.
2. Remove the causal mask from the NumPy implementation, train a two-block model on next-character prediction over any text file, and explain the loss curve you observe in terms of label leakage.
3. Implement RoPE in NumPy for d_k = 8 and verify numerically, to floating-point tolerance, that the attention score between positions (m, n) equals the score between positions (m + s, n + s) for several shifts s.
4. Show algebraically that two stacked sliding-window layers with window w allow position t to receive information from position t - 2w, and describe a retrieval task where this two-hop path loses information that a global attention layer would preserve.
5. Extend the PyTorch block with (a) RoPE on q and k, (b) GQA with n_kv_heads < n_heads using torch.repeat_interleave on k and v, and (c) a KV cache supporting one-token incremental decode; verify that incremental decoding reproduces the full-sequence forward pass logits within tolerance.
6. For a dense model with d_model = 4096 and 32 layers, compute the parameter count of attention (4 projections) versus SwiGLU MLPs (3 projections at 2.7x width) per layer, and confirm the roughly one-third versus two-thirds split claimed in Section 6.
7. Write one page on why GQA exists, arguing purely from inference economics; you will check your answer against Chapter 07.

## Godhood check

You are ready for Chapter 03 when you can do all of the following without notes.

- Write scaled dot-product attention from a blank page, including the mask, and derive the sqrt(d_k) factor from a variance argument.
- Explain Q, K, V as a differentiable key-value store and why the three projections must be separate.
- Explain why the causal mask makes every position a parallel training example, and why that property is economically decisive.
- Describe RoPE precisely: what is rotated, why scores depend only on relative position, and why frequency rescaling enables context extension.
- State the pre-LN vs post-LN trade-off (stability at depth versus final-loss ceiling) and what RMSNorm removes from LayerNorm.
- Diagram a modern decoder block from memory: pre-norm, RoPE attention with GQA, SwiGLU MLP, residual stream.
- For MQA, GQA, sliding-window, and MoE, state in one sentence each: the resource it saves, the mechanism, and the quality or operational cost.
