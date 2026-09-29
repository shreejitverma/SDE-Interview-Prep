---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://docs.python.org/3/library/collections.html, https://docs.python.org/3/library/heapq.html, https://docs.python.org/3/library/functools.html, https://docs.python.org/3/library/itertools.html, https://docs.python.org/3/library/asyncio-sync.html, https://docs.python.org/3/library/asyncio-task.html, https://docs.pydantic.dev/latest/concepts/models/, https://docs.pydantic.dev/latest/concepts/validators/, https://www.python-httpx.org/advanced/transports/, https://aws.amazon.com/builders-library/timeouts-retries-and-backoff-with-jitter/]
---

# Coding round

The problems a Python developer contract screen gives in a 30-45 minute live coding pad, most-asked first.
Expect one Python-idiom problem (K1-K4, K10-K14) and one classic data-structures problem (K5-K9); K1 (LRU cache), K2 (flatten a dict), and K3 (log aggregation) are near-certain in some form.
The "build a small API" exercise lives in [FastAPI](04-FastAPI.md) and [Flask](03-Flask.md).
Every solution here was run on 2026-09-29 under Python 3.14.7 with pytest 9.1.1, Pydantic 2.13.5, and httpx 0.28.1: 66 pytest tests passed against the exact code in this note (each module is extracted from its block byte for byte and every usage block is run), and mypy 2.3.1 reports no issues on the 15 solution modules.

---

## How to run a live coding round

The interviewer grades the process as much as the code; narrate each step.

1. **Clarify (2-3 minutes).** Restate the problem, then ask about input size, types, empty and invalid input, duplicates, ordering of the output, and whether to raise or return a sentinel.
2. **Examples.** Write two or three concrete input and output pairs, including one edge case, before any code; they become your tests.
3. **Brute force first, out loud.** State it and its complexity in one or two sentences ("nested loops, O(n^2)"), then say how you will beat it ("a dict of seen values makes it O(n)").
4. **Code the clean version.** Small functions, real names, type hints, a docstring line, standard library first (`collections`, `heapq`, `itertools`, `functools`, `bisect`).
   Do not import third-party packages unless they say you can.
5. **Test out loud.** Walk one example through the code line by line, then run the edge cases: empty, one element, duplicates, very large, invalid.
   If the pad runs code, write `assert` lines or a tiny pytest function.
6. **Complexity.** State time and space, and name the bottleneck.
7. **Production follow-ups.** Volunteer one or two: thread safety, memory for large inputs, logging, retries, what you would test next.

**Python idioms that read as senior:** `collections.Counter`, `defaultdict`, `deque`, `heapq.nlargest`, `enumerate`, `zip`, comprehensions, generators for streaming, `dataclass`, `with` for resources, `functools.wraps`, and type hints.
**Things that read as junior:** `range(len(xs))` when you need items, `list.pop(0)` in a loop, `x in some_list` inside a loop, bare `except:`, and mutable default arguments (see [Python core](02-Python-Core.md)).

---

## K1. Implement an LRU cache with O(1) `get` and `put`. (must know)

Clarify: capacity is positive, `get` on a missing key returns a default (LeetCode 146 uses -1), and `put` on an existing key updates the value and makes it most recent.

**Version 1: `OrderedDict`**, the answer most interviewers accept first.

```python
# lru_cache.py
from collections import OrderedDict
from collections.abc import Hashable
from typing import Any


class LRUCache:
    """Least-recently-used cache; O(1) get and put."""

    def __init__(self, capacity: int) -> None:
        if capacity <= 0:
            raise ValueError("capacity must be positive")
        self.capacity = capacity
        self._data: OrderedDict[Hashable, Any] = OrderedDict()

    def get(self, key: Hashable, default: Any = None) -> Any:
        if key not in self._data:
            return default
        self._data.move_to_end(key)             # mark as most recently used
        return self._data[key]

    def put(self, key: Hashable, value: Any) -> None:
        if key in self._data:
            self._data.move_to_end(key)
        self._data[key] = value
        if len(self._data) > self.capacity:
            self._data.popitem(last=False)      # evict the least recently used

    def __len__(self) -> int:
        return len(self._data)
```

**Version 2: dict plus a doubly linked list**, for "now do it without `OrderedDict`".
Sentinel head and tail nodes remove every `None` check.

```python
# lru_cache_manual.py
from collections.abc import Hashable
from typing import Any


class _Node:
    __slots__ = ("key", "value", "prev", "next")

    def __init__(self, key: Hashable = None, value: Any = None) -> None:
        self.key = key
        self.value = value
        self.prev: _Node = self                 # self-links until inserted: no None checks
        self.next: _Node = self


class LRUCacheManual:
    """dict for O(1) lookup; linked list for O(1) recency updates and eviction."""

    def __init__(self, capacity: int) -> None:
        if capacity <= 0:
            raise ValueError("capacity must be positive")
        self.capacity = capacity
        self._map: dict[Hashable, _Node] = {}
        self._head = _Node()                    # head.next is the most recent
        self._tail = _Node()                    # tail.prev is the least recent
        self._head.next = self._tail
        self._tail.prev = self._head

    def _unlink(self, node: _Node) -> None:
        node.prev.next = node.next
        node.next.prev = node.prev

    def _push_front(self, node: _Node) -> None:
        node.prev = self._head
        node.next = self._head.next
        self._head.next.prev = node
        self._head.next = node

    def get(self, key: Hashable, default: Any = None) -> Any:
        node = self._map.get(key)
        if node is None:
            return default
        self._unlink(node)
        self._push_front(node)
        return node.value

    def put(self, key: Hashable, value: Any) -> None:
        node = self._map.get(key)
        if node is not None:
            node.value = value
            self._unlink(node)
            self._push_front(node)
            return
        if len(self._map) == self.capacity:
            lru = self._tail.prev
            self._unlink(lru)
            del self._map[lru.key]
        node = _Node(key, value)
        self._map[key] = node
        self._push_front(node)

    def keys_mru_first(self) -> list[Hashable]:
        out, node = [], self._head.next
        while node is not self._tail:
            out.append(node.key)
            node = node.next
        return out

    def __len__(self) -> int:
        return len(self._map)
```

Testing it out loud, as a pytest file:

```python
# test_lru_from_note.py
import pytest

from lru_cache import LRUCache
from lru_cache_manual import LRUCacheManual


@pytest.mark.parametrize("cls", [LRUCache, LRUCacheManual])
def test_evicts_least_recently_used(cls):
    cache = cls(2)
    cache.put("a", 1)
    cache.put("b", 2)
    assert cache.get("a") == 1          # "a" is now most recent
    cache.put("c", 3)                   # evicts "b"
    assert cache.get("b") is None
    assert cache.get("a") == 1 and cache.get("c") == 3
    cache.put("a", 10)                  # update keeps size
    assert len(cache) == 2 and cache.get("a") == 10


@pytest.mark.parametrize("cls", [LRUCache, LRUCacheManual])
def test_rejects_bad_capacity(cls):
    with pytest.raises(ValueError):
        cls(0)
```

**Complexity:** O(1) for `get` and `put`, O(capacity) space.

**Follow-ups they ask:**

- "Make it thread-safe."
  Wrap `get` and `put` in one `threading.Lock`; `get` mutates order, so even reads need the lock.
- "Add a TTL."
  Store `(value, expires_at)` using `time.monotonic()`, treat expired entries as misses on `get`, and optionally sweep in the background.
- "What would you use in production?"
  `functools.lru_cache` or `functools.cache` for pure functions in one process, `cachetools` for TTL, and Redis with `maxmemory-policy allkeys-lru` when the cache must be shared across workers or pods.

---

## K2. Flatten a nested dict or JSON document into dotted keys. (must know)

Clarify: lists get index keys (`items.0.sku`), empty containers are kept as leaf values, the separator is configurable, and colliding keys (`{"a.b": 1, "a": {"b": 2}}`) raise instead of silently overwriting.

```python
# flatten.py
from collections.abc import Iterator
from typing import Any


def _walk(obj: Any, prefix: str, sep: str) -> Iterator[tuple[str, Any]]:
    if isinstance(obj, dict) and obj:
        for key, value in obj.items():
            yield from _walk(value, f"{prefix}{sep}{key}" if prefix else str(key), sep)
    elif isinstance(obj, list) and obj:
        for index, value in enumerate(obj):
            yield from _walk(value, f"{prefix}{sep}{index}" if prefix else str(index), sep)
    else:
        yield prefix, obj                       # a scalar or an empty container is a leaf


def flatten(obj: Any, sep: str = ".") -> dict[str, Any]:
    """{"a": {"b": [1, {"c": 2}]}} -> {"a.b.0": 1, "a.b.1.c": 2}"""
    out: dict[str, Any] = {}
    for key, value in _walk(obj, "", sep):
        if key in out:
            raise ValueError(f"key collision on {key!r}")
        out[key] = value
    return out


def unflatten(flat: dict[str, Any], sep: str = ".") -> dict[str, Any]:
    """Inverse for dict-only nesting (list indexes come back as string keys)."""
    root: dict[str, Any] = {}
    for dotted, value in flat.items():
        node = root
        *parents, leaf = dotted.split(sep)
        for part in parents:
            node = node.setdefault(part, {})
        node[leaf] = value
    return root
```

```python
from flatten import flatten, unflatten

doc = {"order": {"id": 7, "items": [{"sku": "A", "qty": 2}, {"sku": "B", "qty": 1}], "meta": {}}}
assert flatten(doc) == {
    "order.id": 7,
    "order.items.0.sku": "A",
    "order.items.0.qty": 2,
    "order.items.1.sku": "B",
    "order.items.1.qty": 1,
    "order.meta": {},
}
assert unflatten(flatten({"a": {"b": 1, "c": {"d": 2}}})) == {"a": {"b": 1, "c": {"d": 2}}}
```

**Complexity:** O(total number of leaves times key length) time and space.

**Follow-ups they ask:**

- "What about very deep nesting?"
  The recursion is bounded by `sys.getrecursionlimit()` (1000 by default); for untrusted input, cap the depth or switch to an explicit stack of `(prefix, node)` pairs.
- "Why not `pandas.json_normalize`?"
  Fine in a data job; in a service or an interview, show you can write the recursion and control the edge cases.

---

## K3. Aggregate a web-server log: requests, error rate, and p95 latency per endpoint. (must know)

Input: an iterable of lines such as `2026-09-29T10:00:01Z GET /api/orders/123 200 45ms`, possibly a multi-GB file.
Clarify: skip and count malformed lines, group `/orders/123` and `/orders/456` together, and define an error as status 500 or above.

```python
# log_stats.py
import math
import re
from collections import Counter, defaultdict
from collections.abc import Iterable
from dataclasses import dataclass, field

LINE_RE = re.compile(
    r"^(?P<ts>\S+) (?P<method>[A-Z]+) (?P<path>/\S*) (?P<status>\d{3}) (?P<ms>\d+)ms$"
)
ID_RE = re.compile(r"/\d+(?=/|$)")


def normalize(path: str) -> str:
    """/api/orders/123/items -> /api/orders/{id}/items"""
    return ID_RE.sub("/{id}", path)


@dataclass
class Report:
    total: int = 0
    malformed: int = 0
    hits: Counter[str] = field(default_factory=Counter)
    errors: Counter[str] = field(default_factory=Counter)
    status_classes: Counter[str] = field(default_factory=Counter)
    latencies: defaultdict[str, list[int]] = field(default_factory=lambda: defaultdict(list))

    def error_rate(self, endpoint: str) -> float:
        hits = self.hits[endpoint]
        return self.errors[endpoint] / hits if hits else 0.0

    def p95(self, endpoint: str) -> int | None:
        values = sorted(self.latencies.get(endpoint, []))
        if not values:
            return None
        return values[math.ceil(0.95 * len(values)) - 1]      # nearest-rank percentile

    def top(self, n: int) -> list[tuple[str, int]]:
        return self.hits.most_common(n)


def analyze(lines: Iterable[str]) -> Report:
    report = Report()
    for line in lines:                                        # streams: one line in memory
        match = LINE_RE.match(line.strip())
        if match is None:
            report.malformed += 1
            continue
        endpoint = f"{match['method']} {normalize(match['path'])}"
        status = int(match["status"])
        report.total += 1
        report.hits[endpoint] += 1
        report.status_classes[f"{status // 100}xx"] += 1
        if status >= 500:
            report.errors[endpoint] += 1
        report.latencies[endpoint].append(int(match["ms"]))
    return report
```

```python
from log_stats import analyze

lines = [
    "2026-09-29T10:00:01Z GET /api/orders/123 200 40ms",
    "2026-09-29T10:00:02Z GET /api/orders/456 503 900ms",
    "2026-09-29T10:00:03Z POST /api/orders 201 120ms",
    "garbage line",
    "2026-09-29T10:00:04Z GET /api/orders/789 200 60ms",
]
r = analyze(lines)
assert r.total == 4 and r.malformed == 1
assert r.top(1) == [("GET /api/orders/{id}", 3)]
assert round(r.error_rate("GET /api/orders/{id}"), 2) == 0.33
assert r.p95("GET /api/orders/{id}") == 900
assert r.status_classes == {"2xx": 3, "5xx": 1}
```

Run it on a real file with `with open(path, encoding="utf-8", errors="replace") as f: report = analyze(f)`, which streams line by line.

**Complexity:** O(n) time to scan; O(endpoints) for the counters, but O(n) for the stored latencies, plus O(k log k) per endpoint to sort for p95.

**Follow-ups they ask:**

- "The file is 50 GB."
  Streaming already keeps lines out of memory; replace the latency lists with a fixed-bucket histogram or a t-digest, and split the file by byte ranges across processes, then merge the `Counter`s (they add with `+`).
- "Why `Counter` and `defaultdict`?"
  They remove the `if key not in d` boilerplate, and `Counter.most_common(n)` uses a heap when `n` is given.
- "Make it a CLI."
  `argparse` with a path argument, JSON output, and a non-zero exit code when the error rate crosses a threshold.

---

## K4. Write a retry decorator with exponential backoff and jitter. (must know)

Clarify: which exceptions are retryable, the maximum number of attempts, the delay cap, and whether the function is sync or async.
Only retry **idempotent** operations and **transient** failures (timeouts, connection resets, 429, 503).

```python
# retry.py
import asyncio
import functools
import logging
import random
import time
from collections.abc import Awaitable, Callable
from typing import ParamSpec, TypeVar

P = ParamSpec("P")
R = TypeVar("R")
log = logging.getLogger(__name__)


def backoff_delay(attempt: int, base: float, cap: float, rng: Callable[[], float]) -> float:
    """Full jitter: uniform in [0, min(cap, base * 2**(attempt - 1))]."""
    return rng() * min(cap, base * 2 ** (attempt - 1))


def retry(
    *,
    attempts: int = 3,
    base_delay: float = 0.1,
    max_delay: float = 5.0,
    exceptions: tuple[type[BaseException], ...] = (ConnectionError, TimeoutError),
    sleep: Callable[[float], None] = time.sleep,
    rng: Callable[[], float] = random.random,
) -> Callable[[Callable[P, R]], Callable[P, R]]:
    if attempts < 1:
        raise ValueError("attempts must be >= 1")

    def decorator(func: Callable[P, R]) -> Callable[P, R]:
        @functools.wraps(func)
        def wrapper(*args: P.args, **kwargs: P.kwargs) -> R:
            for attempt in range(1, attempts + 1):
                try:
                    return func(*args, **kwargs)
                except exceptions as exc:
                    if attempt == attempts:
                        raise                               # give up: original traceback
                    delay = backoff_delay(attempt, base_delay, max_delay, rng)
                    log.warning("%s failed (%r), attempt %d/%d, retrying in %.3fs",
                                func.__qualname__, exc, attempt, attempts, delay)
                    sleep(delay)
            raise AssertionError("unreachable")
        return wrapper
    return decorator


def async_retry(
    *,
    attempts: int = 3,
    base_delay: float = 0.1,
    max_delay: float = 5.0,
    exceptions: tuple[type[BaseException], ...] = (ConnectionError, TimeoutError),
    rng: Callable[[], float] = random.random,
) -> Callable[[Callable[P, Awaitable[R]]], Callable[P, Awaitable[R]]]:
    if attempts < 1:
        raise ValueError("attempts must be >= 1")

    def decorator(func: Callable[P, Awaitable[R]]) -> Callable[P, Awaitable[R]]:
        @functools.wraps(func)
        async def wrapper(*args: P.args, **kwargs: P.kwargs) -> R:
            for attempt in range(1, attempts + 1):
                try:
                    return await func(*args, **kwargs)
                except exceptions:
                    if attempt == attempts:
                        raise
                    await asyncio.sleep(backoff_delay(attempt, base_delay, max_delay, rng))
            raise AssertionError("unreachable")
        return wrapper
    return decorator
```

```python
from retry import retry

delays, calls = [], []


@retry(attempts=4, base_delay=0.1, max_delay=0.3, sleep=delays.append, rng=lambda: 1.0)
def flaky() -> str:
    calls.append(1)
    if len(calls) < 4:
        raise ConnectionError("reset")
    return "ok"


assert flaky() == "ok"
assert delays == [0.1, 0.2, 0.3]            # 0.1, 0.2, then capped at 0.3 (0.4 without the cap)
assert flaky.__name__ == "flaky"
```

**Talking points:**

- Why jitter: without it, every client that failed together retries together, and the synchronized waves keep the recovering service down (the thundering herd).
  "Full jitter" (a uniform random delay up to the exponential cap) spreads them out.
- Injecting `sleep` and `rng` makes the decorator deterministic in tests with no real waiting.
- Non-retryable exceptions (a 400, a validation error) propagate on the first attempt.
- In production also cap the **total** time with a deadline, honor a `Retry-After` header, keep a retry budget so retries cannot multiply load across layers, and pair with a circuit breaker (see [Microservices and messaging](08-Microservices-and-Messaging.md)).
- Libraries: `tenacity` for general use, `urllib3.util.Retry` for `requests`, and `httpx.HTTPTransport(retries=...)`, which retries only failed connection attempts.

---

## K5. Two sum and group anagrams: the hash-map warm-ups. (must know)

```python
# hashing.py
from collections import defaultdict


def two_sum(nums: list[int], target: int) -> tuple[int, int] | None:
    """Indexes of two different elements summing to target; O(n) time, O(n) space."""
    seen: dict[int, int] = {}                    # value -> index
    for i, n in enumerate(nums):
        j = seen.get(target - n)
        if j is not None:
            return j, i
        seen[n] = i
    return None


def group_anagrams(words: list[str]) -> list[list[str]]:
    """O(n * k log k) for n words of length k; groups in first-seen order."""
    groups: defaultdict[tuple[str, ...], list[str]] = defaultdict(list)
    for word in words:
        groups[tuple(sorted(word))].append(word)
    return list(groups.values())
```

```python
from hashing import group_anagrams, two_sum

assert two_sum([2, 7, 11, 15], 9) == (0, 1)
assert two_sum([3, 3], 6) == (0, 1)             # same value, different indexes
assert two_sum([1, 2], 7) is None
assert group_anagrams(["eat", "tea", "tan", "ate", "nat", "bat"]) == [
    ["eat", "tea", "ate"], ["tan", "nat"], ["bat"]
]
```

**Brute force to say first:** two sum by nested loops is O(n^2); the dict turns "have I seen the complement?" into O(1).
For anagrams, a 26-slot count tuple as the key makes it O(n * k) for lowercase ASCII input.
If the input array is sorted, two pointers give O(1) extra space.

---

## K6. Valid parentheses.

```python
# brackets.py
PAIRS = {")": "(", "]": "[", "}": "{"}
OPENERS = set(PAIRS.values())


def is_valid(s: str) -> bool:
    """Every closer matches the most recent unmatched opener; other characters are ignored."""
    stack: list[str] = []
    for ch in s:
        if ch in OPENERS:
            stack.append(ch)
        elif ch in PAIRS:
            if not stack or stack.pop() != PAIRS[ch]:
                return False
    return not stack
```

```python
from brackets import is_valid

assert is_valid("()[]{}") and is_valid("{[()]}") and is_valid("")
assert not is_valid("(]") and not is_valid("([)]") and not is_valid("((") and not is_valid(")")
assert is_valid('{"a": [1, (2)]}')              # ignores non-bracket characters
```

**Complexity:** O(n) time, O(n) space for the stack.
The common bugs are forgetting the empty-stack check before `pop` and forgetting `return not stack` for leftover openers.

---

## K7. Top k most frequent elements.

```python
# top_k.py
import heapq
from collections import Counter
from collections.abc import Hashable, Iterable
from operator import itemgetter


def top_k_frequent(items: Iterable[Hashable], k: int) -> list[Hashable]:
    """O(n log k) with a size-k heap; ties keep first-seen order."""
    counts = Counter(items)
    return [item for item, _ in heapq.nlargest(k, counts.items(), key=itemgetter(1))]


def top_k_bucket(items: Iterable[Hashable], k: int) -> list[Hashable]:
    """O(n) bucket sort: index = frequency."""
    counts = Counter(items)
    buckets: list[list[Hashable]] = [[] for _ in range(max(counts.values(), default=0) + 1)]
    for item, freq in counts.items():
        buckets[freq].append(item)
    out: list[Hashable] = []
    for freq in range(len(buckets) - 1, 0, -1):
        for item in buckets[freq]:
            out.append(item)
            if len(out) == k:
                return out
    return out
```

```python
from top_k import top_k_bucket, top_k_frequent

data = [1, 1, 1, 2, 2, 3, 4, 4, 4, 4]
assert top_k_frequent(data, 2) == [4, 1] == top_k_bucket(data, 2)
assert top_k_frequent("abracadabra", 2) == ["a", "b"]    # b and r tie at 2; b seen first
assert top_k_frequent([], 3) == [] == top_k_bucket([], 3)
```

**Talking points:** `Counter(items).most_common(k)` is the one-liner and uses `heapq.nlargest` internally; say so, then show the heap.
The heap keeps only k entries, which matters when the number of distinct items is huge; for a stream, keep the `Counter` and query on demand, or use a count-min sketch when exact counts are too big.

---

## K8. Merge overlapping intervals.

Backend framing: merge overlapping maintenance windows, bookings, or trading-halt periods.

```python
# intervals.py
def merge(intervals: list[tuple[int, int]]) -> list[tuple[int, int]]:
    """Sort by start, then extend the last merged interval while they overlap."""
    merged: list[list[int]] = []
    for start, end in sorted(intervals):
        if merged and start <= merged[-1][1]:          # touching counts as overlapping
            merged[-1][1] = max(merged[-1][1], end)
        else:
            merged.append([start, end])
    return [(s, e) for s, e in merged]
```

```python
from intervals import merge

assert merge([(1, 3), (2, 6), (8, 10), (15, 18)]) == [(1, 6), (8, 10), (15, 18)]
assert merge([(1, 4), (4, 5)]) == [(1, 5)]
assert merge([(5, 7), (1, 10)]) == [(1, 10)]           # unsorted input, containment
assert merge([]) == []
```

**Complexity:** O(n log n) for the sort, O(n) for the output.
Clarify whether touching intervals (`[1, 4]` and `[4, 5]`) merge; flip `<=` to `<` if they do not.

---

## K9. Longest substring without repeating characters (sliding window).

```python
# sliding_window.py
def longest_unique_substring(s: str) -> int:
    """Window [start, i] holds no repeats; jump start past the previous occurrence."""
    last_seen: dict[str, int] = {}
    start = best = 0
    for i, ch in enumerate(s):
        if last_seen.get(ch, -1) >= start:
            start = last_seen[ch] + 1
        last_seen[ch] = i
        best = max(best, i - start + 1)
    return best
```

```python
from sliding_window import longest_unique_substring

assert longest_unique_substring("abcabcbb") == 3        # "abc"
assert longest_unique_substring("bbbbb") == 1
assert longest_unique_substring("pwwkew") == 3          # "wke"
assert longest_unique_substring("abba") == 2            # start must never move left
assert longest_unique_substring("") == 0
```

**Complexity:** O(n) time, O(alphabet) space.
The `>= start` check is the bug magnet: without it, `"abba"` moves `start` backward and returns 3.

---

## K10. Write a timing decorator and a memoize decorator.

```python
# decorators.py
import functools
import inspect
import logging
import time
from collections.abc import Callable
from typing import Any

log = logging.getLogger(__name__)


def timed(func: Callable[..., Any]) -> Callable[..., Any]:
    """Log wall time for sync and async functions alike."""
    if inspect.iscoroutinefunction(func):
        @functools.wraps(func)
        async def async_wrapper(*args, **kwargs):
            start = time.perf_counter()
            try:
                return await func(*args, **kwargs)
            finally:
                log.info("%s took %.3f ms", func.__qualname__, (time.perf_counter() - start) * 1e3)
        return async_wrapper

    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        start = time.perf_counter()
        try:
            return func(*args, **kwargs)
        finally:
            log.info("%s took %.3f ms", func.__qualname__, (time.perf_counter() - start) * 1e3)
    return wrapper


def memoize(func: Callable[..., Any]) -> Callable[..., Any]:
    """Unbounded cache keyed on the arguments; they must be hashable."""
    cache: dict[Any, Any] = {}
    missing = object()

    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        key = (args, tuple(sorted(kwargs.items())))
        result = cache.get(key, missing)          # a sentinel, so a cached None is a hit
        if result is missing:
            result = cache[key] = func(*args, **kwargs)
        return result

    wrapper.cache = cache                         # type: ignore[attr-defined]
    wrapper.cache_clear = cache.clear             # type: ignore[attr-defined]
    return wrapper
```

```python
import asyncio
import logging

from decorators import memoize, timed

logging.basicConfig(level=logging.INFO)
calls = []


@memoize
def fib(n: int) -> int:
    calls.append(n)
    return n if n < 2 else fib(n - 1) + fib(n - 2)


assert fib(80) == 23416728348467685
assert len(calls) == 81                        # each n computed once: O(n), not O(2^n)


@timed
async def handler() -> str:
    await asyncio.sleep(0.01)
    return "done"


assert asyncio.run(handler()) == "done"        # awaited inside the wrapper, so timing is real
assert handler.__name__ == "handler"
```

**Talking points:**

- Use `time.perf_counter()` for durations, never `time.time()` (wall clock can jump).
- In production, prefer `functools.cache` or `functools.lru_cache(maxsize=...)`: they are C-implemented, their bookkeeping is thread-safe (though two threads can both compute the same missing key), and a bounded size prevents a memory leak.
- `lru_cache` on an instance method keeps every `self` alive; see [Python core Y19](02-Python-Core.md).
- A sync wrapper around an `async def` would time only the creation of the coroutine; that is why `timed` branches on `inspect.iscoroutinefunction`.

---

## K11. Implement a thread-safe token-bucket rate limiter.

Clarify: rate in tokens per second, a burst capacity, non-blocking `try_acquire` versus blocking `acquire` with a timeout, and per-process versus distributed.

```python
# rate_limiter.py
import threading
import time
from collections.abc import Callable


class TokenBucket:
    """Refills continuously at `rate` tokens/s up to `capacity`; thread-safe."""

    def __init__(self, rate: float, capacity: float,
                 clock: Callable[[], float] = time.monotonic) -> None:
        if rate <= 0 or capacity <= 0:
            raise ValueError("rate and capacity must be positive")
        self.rate = rate
        self.capacity = capacity
        self._clock = clock
        self._tokens = capacity                 # start full: allow an initial burst
        self._last = clock()
        self._lock = threading.Lock()

    def _refill(self) -> None:                  # caller holds the lock
        now = self._clock()
        self._tokens = min(self.capacity, self._tokens + (now - self._last) * self.rate)
        self._last = now

    def try_acquire(self, tokens: float = 1.0) -> bool:
        if tokens > self.capacity:
            raise ValueError("request exceeds bucket capacity")
        with self._lock:
            self._refill()
            if self._tokens >= tokens:
                self._tokens -= tokens
                return True
            return False

    def wait_time(self, tokens: float = 1.0) -> float:
        """Seconds until `tokens` would be available (0 if available now)."""
        with self._lock:
            self._refill()
            return max(0.0, (tokens - self._tokens) / self.rate)

    def acquire(self, tokens: float = 1.0, timeout: float | None = None) -> bool:
        """Block until acquired or the timeout expires; never sleeps holding the lock."""
        deadline = None if timeout is None else self._clock() + timeout
        while not self.try_acquire(tokens):
            wait = self.wait_time(tokens)
            if deadline is not None and self._clock() + wait > deadline:
                return False
            time.sleep(wait)
        return True
```

```python
import threading

from rate_limiter import TokenBucket

now = [0.0]
bucket = TokenBucket(rate=10, capacity=5, clock=lambda: now[0])     # fake clock
assert [bucket.try_acquire() for _ in range(6)] == [True] * 5 + [False]
now[0] += 0.25                                                    # 2.5 tokens refilled
assert bucket.try_acquire() and bucket.try_acquire() and not bucket.try_acquire()
assert abs(bucket.wait_time() - 0.05) < 1e-9

frozen = TokenBucket(rate=1, capacity=50, clock=lambda: 0.0)      # no refill during the race
granted = []
lock = threading.Lock()


def worker():
    for _ in range(100):
        if frozen.try_acquire():
            with lock:
                granted.append(1)


threads = [threading.Thread(target=worker) for _ in range(8)]
for t in threads:
    t.start()
for t in threads:
    t.join()
assert len(granted) == 50                                         # never over-granted
```

**Complexity:** O(1) per call, O(1) space per bucket.

**Talking points:**

- Refill lazily from elapsed time instead of a background thread; use `time.monotonic()` so clock adjustments cannot mint tokens.
- The lock covers refill plus decrement, which is a check-then-act sequence that races without it even under the GIL.
- Per-client limits: a dict of buckets keyed by API key or IP, created under a lock, with idle-bucket eviction.
- Across pods, an in-process bucket does not work; use Redis (an atomic Lua script or a fixed or sliding window with `INCR` and `EXPIRE`) or the API gateway, and return `429` with `Retry-After` (see [REST API design](05-REST-API-Design.md)).
- Alternatives: fixed window (simple, bursty at the boundary), sliding log (exact, O(requests) memory), sliding window counter (a good approximation), leaky bucket (smooth output rate).

---

## K12. Call an API for many items concurrently with a concurrency limit and a timeout.

Clarify: how many items, the per-call and overall timeouts, whether one failure fails the batch, and whether the output order must match the input.

```python
# fanout.py
import asyncio
from collections.abc import Awaitable, Callable, Iterable
from typing import TypeVar

T = TypeVar("T")
R = TypeVar("R")


async def fan_out(
    items: Iterable[T],
    worker: Callable[[T], Awaitable[R]],
    *,
    limit: int = 10,
    timeout: float = 5.0,
) -> list[R | BaseException]:
    """Run worker(item) for every item, at most `limit` at a time.

    Results are in input order; a failure or a per-item timeout is returned in its slot
    as the exception instead of cancelling the whole batch.
    """
    if limit < 1:
        raise ValueError("limit must be >= 1")
    semaphore = asyncio.Semaphore(limit)

    async def run_one(item: T) -> R:
        async with semaphore:                       # waits here while `limit` calls are in flight
            async with asyncio.timeout(timeout):    # the clock starts after acquiring a slot
                return await worker(item)

    return await asyncio.gather(*(run_one(item) for item in items), return_exceptions=True)
```

Tested with a real `httpx.AsyncClient` over a mock transport, so no network is needed:

```python
import asyncio

import httpx

from fanout import fan_out

in_flight = peak = 0


async def handler(request: httpx.Request) -> httpx.Response:
    global in_flight, peak
    in_flight += 1
    peak = max(peak, in_flight)
    try:
        order_id = int(request.url.path.rsplit("/", 1)[1])
        await asyncio.sleep(1.0 if order_id == 13 else 0.02)       # 13 is the slow one
        if order_id == 7:
            return httpx.Response(503)
        return httpx.Response(200, json={"id": order_id})
    finally:
        in_flight -= 1


async def main() -> list:
    transport = httpx.MockTransport(handler)
    async with httpx.AsyncClient(transport=transport, base_url="https://api.test") as client:
        async def get_order(order_id: int) -> dict:
            resp = await client.get(f"/orders/{order_id}")
            resp.raise_for_status()
            return resp.json()

        return await fan_out(range(20), get_order, limit=5, timeout=0.2)


results = asyncio.run(main())
assert peak <= 5
assert results[0] == {"id": 0} and results[19] == {"id": 19}
assert isinstance(results[7], httpx.HTTPStatusError)
assert isinstance(results[13], TimeoutError)
assert sum(isinstance(r, dict) for r in results) == 18
```

**Talking points:**

- The semaphore bounds sockets and upstream load; unbounded `gather` over 10,000 items opens 10,000 connections and gets you rate-limited or blocked.
- Share one `httpx.AsyncClient` for connection pooling; creating a client per request defeats keep-alive.
- `return_exceptions=True` keeps partial results; if one failure should cancel the rest, use `asyncio.TaskGroup` instead.
- For millions of items, `gather` still creates one coroutine per item up front; use a fixed pool of N worker tasks reading from an `asyncio.Queue`, which also gives back-pressure.
- Add retries with jitter per item (K4), and an overall deadline with an outer `asyncio.timeout`.

---

## K13. Parse and validate a batch of input records with Pydantic, reporting per-row errors.

Clarify: reject the whole batch or keep the good rows, which fields are required, and how to report errors to the caller.

```python
# trades.py
from collections.abc import Iterable
from decimal import Decimal
from enum import StrEnum
from typing import Any

from pydantic import (AwareDatetime, BaseModel, ConfigDict, Field, TypeAdapter,
                      ValidationError, field_validator, model_validator)

MAX_NOTIONAL = Decimal("10000000")


class Side(StrEnum):
    BUY = "buy"
    SELL = "sell"


class Trade(BaseModel):
    model_config = ConfigDict(extra="forbid", frozen=True, str_strip_whitespace=True)

    trade_id: str = Field(pattern=r"^T\d+$")
    symbol: str = Field(min_length=1, max_length=12)
    side: Side
    qty: int = Field(gt=0)
    price: Decimal = Field(gt=0, max_digits=12, decimal_places=4)
    executed_at: AwareDatetime                     # naive timestamps are rejected

    @field_validator("side", mode="before")
    @classmethod
    def lower_side(cls, v: Any) -> Any:
        return v.lower() if isinstance(v, str) else v

    @field_validator("symbol")
    @classmethod
    def upper_symbol(cls, v: str) -> str:
        return v.upper()

    @model_validator(mode="after")
    def check_notional(self) -> "Trade":
        if self.qty * self.price > MAX_NOTIONAL:
            raise ValueError(f"notional exceeds {MAX_NOTIONAL}")
        return self


def parse_trades(rows: Iterable[dict[str, Any]]) -> tuple[list[Trade], list[dict[str, Any]]]:
    """Keep valid rows; collect structured errors for the rest."""
    good: list[Trade] = []
    bad: list[dict[str, Any]] = []
    for index, row in enumerate(rows):
        try:
            good.append(Trade.model_validate(row))
        except ValidationError as exc:
            bad.append({
                "row": index,
                "errors": [{"loc": ".".join(map(str, e["loc"])), "msg": e["msg"]}
                           for e in exc.errors(include_url=False)],
            })
    return good, bad


TRADES = TypeAdapter(list[Trade])                  # all-or-nothing, straight from JSON bytes
```

```python
from decimal import Decimal

from pydantic import ValidationError

from trades import TRADES, Side, parse_trades

rows = [
    {"trade_id": "T1", "symbol": " ibm ", "side": "BUY", "qty": "100",
     "price": "101.25", "executed_at": "2026-09-29T14:30:00Z"},
    {"trade_id": "X2", "symbol": "MSFT", "side": "sell", "qty": 0,
     "price": "10", "executed_at": "2026-09-29T14:31:00Z"},
    {"trade_id": "T3", "symbol": "AAPL", "side": "buy", "qty": 1,
     "price": "1", "executed_at": "2026-09-29T14:32:00"},          # naive timestamp
]
good, bad = parse_trades(rows)
assert len(good) == 1
assert good[0].symbol == "IBM" and good[0].side is Side.BUY and good[0].price == Decimal("101.25")
assert [b["row"] for b in bad] == [1, 2]
assert {e["loc"] for e in bad[0]["errors"]} == {"trade_id", "qty"}
assert bad[1]["errors"][0]["loc"] == "executed_at"

try:
    TRADES.validate_json(b'[{"trade_id": "T9"}]')
except ValidationError as exc:
    assert exc.error_count() == 5                                     # five missing fields
```

**Talking points:**

- Validate once at the boundary, then pass typed objects inward; `extra="forbid"` catches misspelled fields that would otherwise be silently dropped.
- `Decimal` for money, never `float`; `AwareDatetime` rejects naive timestamps, which cause timezone bugs in trading systems.
- `mode="before"` validators normalize raw input; `mode="after"` and `model_validator` enforce invariants across fields.
- `exc.errors()` is structured, so the API can return per-row, per-field 422 details; FastAPI does this automatically for request bodies (see [FastAPI](04-FastAPI.md)).
- Lax mode coerces `"100"` to `100`; set `strict=True` on a field or model when coercion would hide upstream bugs.

---

## K14. Read a large file in batches with a generator.

Clarify: text lines or binary chunks, batch size, and what consumes the batches (a bulk DB insert, a Kafka producer, an HTTP bulk API).

```python
# batching.py
from collections.abc import Iterable, Iterator
from itertools import islice
from typing import TypeVar

T = TypeVar("T")


def batched(iterable: Iterable[T], size: int) -> Iterator[tuple[T, ...]]:
    """Lazy fixed-size batches; the last one may be shorter (itertools.batched in 3.12+)."""
    if size < 1:
        raise ValueError("size must be >= 1")
    it = iter(iterable)
    while batch := tuple(islice(it, size)):
        yield batch


def read_line_batches(path: str, size: int, encoding: str = "utf-8") -> Iterator[tuple[str, ...]]:
    """Stream a text file in batches of lines; memory is O(size), not O(file)."""
    with open(path, encoding=encoding) as f:
        yield from batched((line.rstrip("\n") for line in f), size)


def read_chunks(path: str, chunk_size: int = 1 << 20) -> Iterator[bytes]:
    """Stream a binary file in fixed-size chunks (for hashing, uploads, parsing)."""
    with open(path, "rb") as f:
        while chunk := f.read(chunk_size):
            yield chunk
```

```python
import hashlib
import itertools
import os
import tempfile

from batching import batched, read_chunks, read_line_batches

assert list(batched(range(7), 3)) == [(0, 1, 2), (3, 4, 5), (6,)]
assert list(batched(range(7), 3)) == list(itertools.batched(range(7), 3))
assert next(batched(itertools.count(), 2)) == (0, 1)            # lazy: works on an infinite stream

with tempfile.TemporaryDirectory() as tmp:
    path = os.path.join(tmp, "orders.csv")
    with open(path, "w", encoding="utf-8") as f:
        f.writelines(f"order-{i}\n" for i in range(10_005))
    sizes = [len(b) for b in read_line_batches(path, 1_000)]
    assert sizes == [1_000] * 10 + [5]
    digest = hashlib.sha256()
    for chunk in read_chunks(path, chunk_size=4096):
        digest.update(chunk)
    with open(path, "rb") as f:
        assert digest.hexdigest() == hashlib.sha256(f.read()).hexdigest()
```

**Talking points:**

- `with` inside the generator closes the file when the generator is exhausted or closed; a half-consumed generator keeps the file open until it is garbage collected, so consume fully or call `.close()`.
- Typical use is `for batch in batched(csv.DictReader(f), 1000): session.execute(insert(Order), list(batch))`, one transaction per batch (see [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md)).
- Batch size trades memory and transaction length against round trips; 500 to 5,000 rows is a common starting point, then measure.
- Since 3.13, `itertools.batched(it, n, strict=True)` raises if the final batch is short.

---

## Go deeper

- DSA practice: [NeetCode 150 in Python](../NeetCode-150-Python.md), [Blind 75 in Python](../Blind-75-LeetCode-Python.md), [LeetCode quant set](../LeetCode-Quant-Complete-Python-Full.md).
- Python depth: [Python core](02-Python-Core.md) for decorators, generators, and asyncio, [Ultimate Python Advanced Guide](../Ultimate-Python-Advanced-Guide.md), [Python Advanced Guide](../Python-Advanced-Guide.md), [Anti-patterns and pitfalls](../Python_Zero_to_Godhood/Chapter_70_Python_Anti-Patterns_and_Common_Pitfalls.md), [CPU- and I/O-bound concurrency](../Python_Zero_to_Godhood/Chapter_26_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md).
- Pack: [FastAPI](04-FastAPI.md) and [Flask](03-Flask.md) for the build-a-mini-API exercise, [Testing, debugging, production](10-Testing-Debugging-Production.md) for pytest patterns, [System design](12-System-Design.md) for rate limiting and caching at scale.
- Official: [collections](https://docs.python.org/3/library/collections.html), [heapq](https://docs.python.org/3/library/heapq.html), [functools](https://docs.python.org/3/library/functools.html), [itertools](https://docs.python.org/3/library/itertools.html), [asyncio synchronization](https://docs.python.org/3/library/asyncio-sync.html), [Pydantic validators](https://docs.pydantic.dev/latest/concepts/validators/), [httpx transports](https://www.python-httpx.org/advanced/transports/), [Timeouts, retries, and backoff with jitter](https://aws.amazon.com/builders-library/timeouts-retries-and-backoff-with-jitter/).
