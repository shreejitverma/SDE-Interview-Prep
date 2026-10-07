---
type: concept
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "Space/Time Trade-offs in Hash Coding with Allowable Errors, Burton Bloom, CACM 1970"
  - "An Improved Data Stream Summary: The Count-Min Sketch and its Applications, Cormode and Muthukrishnan, 2005"
  - "HyperLogLog: the analysis of a near-optimal cardinality estimation algorithm, Flajolet et al., 2007"
---

# Probabilistic Structures

## TL;DR

Some questions do not need an exact set.
A Bloom filter stores membership with false positives and no false negatives.
A count-min sketch stores frequencies with a one-sided error.
HyperLogLog stores a cardinality.
Use them at the edge of a crawler, a cache, or an analytics path, and keep the exact store behind a positive result that matters.

## Bloom filter

A Bloom filter is a bit array of m bits and k hash functions.
Insert sets k bits.
Lookup returns "maybe" if all k bits are set, and "no" if any bit is clear.
Deletes are not supported.
A counting Bloom filter replaces bits with counters if you truly need deletes.

The false-positive rate is about `(1 - e^{-kn/m})^k`.
The useful setting is `k = (m/n) ln 2`, which is about `0.693 m/n`.
At 10 bits per item and 7 hashes, the false-positive rate is under 1 percent.
That is the number to memorize, and then recompute for your n.

```python
import hashlib
import math

def bloom_false_positive(m: int, n: int, k: int) -> float:
    if m <= 0 or n <= 0 or k <= 0:
        raise ValueError("m, n, and k must be positive")
    return (1 - math.exp(-k * n / m)) ** k

def bit_index(item: bytes, seed: int, m: int) -> int:
    digest = hashlib.blake2b(item, person=seed.to_bytes(8, "little")).digest()
    return int.from_bytes(digest[:8], "little") % m
```

`bloom_false_positive(10 * 1_000_000, 1_000_000, 7)` is about 0.0082.
[[07-Distributed-Web-Crawler/design|The crawler]] uses this so the frontier does not ask the exact store for every URL it has never seen.
A false positive skips a page you could have fetched.
A false negative would refetch the web forever, which is why the structure is built not to have one.

## Count-min and HyperLogLog

Count-min keeps d rows of w counters.
Each event adds one to one counter in each row, chosen by that row's hash.
The estimate is the minimum of those d counters.
The estimate is never below the truth.
With `w = ceil(e / ε)` and `d = ceil(ln(1 / δ))`, the estimate is at most the truth plus `ε` times the stream length, with probability at least `1 - δ`.
Use it for heavy hitters and for "is this key hot", not for billing.

HyperLogLog estimates how many distinct items it has seen.
With m registers, the relative standard error is about `1.04 / sqrt(m)`.
At m = 16,384 the error is under 1 percent, in about 16 KB if each register is a byte.
Use it for unique visitors.
Do not use it to decide whether two specific users exist.

## Pitfalls

- A Bloom filter that is never resized silently passes 50 percent false positives after the insert count doubles.
- Count-min on a stream you also need to decrement requires a structure that supports it, or a windowed rebuild.
- Sharing one hash function across the k positions correlates the bits and ruins the formula.
- Using a probabilistic structure as the billing ledger.

## Interview questions

1. You have a billion URLs and 1 GB of RAM. Can you remember them exactly, and what do you store instead.
2. Why is a Bloom false positive acceptable in a crawler and unacceptable in a payments deduper.
3. How do you find the hottest ten keys without a counter per key.
4. What happens to a HyperLogLog estimate if you union two of them.

## Further reading

- Bloom, CACM 1970.
- Cormode and Muthukrishnan, 2005.
- Flajolet, Fusy, Gandouet, and Meunier, 2007.
- [[Caching-and-Invalidation]] for the singleflight that sits next to these structures, not inside them.
