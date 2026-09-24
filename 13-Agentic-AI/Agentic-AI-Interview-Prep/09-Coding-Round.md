---
type: playbook
track: [ai-eng, sde]
level:
status: draft
last_reviewed:
sources: [https://github.com/modelcontextprotocol/python-sdk, https://docs.pydantic.dev/latest/, https://plg.uwaterloo.ca/~gvcormac/cormacksigir09-rrf.pdf, https://aws.amazon.com/builders-library/timeouts-retries-and-backoff-with-jitter/]
---

# Coding round

Practice these by writing them from memory, then compare with the versions below.
Every solution here was run on 2026-09-23 under Python 3.14 with pydantic 2.13, numpy 2.5, and the MCP Python SDK 2.2 (plus 1.30 for the legacy server form): 28 unit tests passed, including a real stdio round trip against the MCP server in Q61.

**What interviewers look for in AI coding rounds:** input validation, errors fed back to the model instead of crashing, bounded loops, timeouts, deterministic tests with a fake model, and saying the complexity out loud.

---

## Q55. Implement a minimal agent loop with tool calling.

Provider-agnostic sketch you should be able to write from memory.
The `Block` and `Response` types stand in for a provider SDK so the loop runs and tests without network access.

```python
import json
from dataclasses import dataclass, field
from typing import Any, Callable

from pydantic import BaseModel, ValidationError


# --- Minimal message types so the loop runs without a real provider SDK ---
@dataclass
class Block:
    type: str                      # "text" or "tool_use"
    text: str = ""
    id: str = ""
    name: str = ""
    input: dict = field(default_factory=dict)


@dataclass
class Response:
    content: list[Block]


# --- Tools: a function plus a Pydantic model that validates its arguments ---
class WeatherArgs(BaseModel):
    city: str


def get_weather(city: str) -> str:
    return f"Sunny, 22C in {city}"


TOOLS: dict[str, tuple[Callable[..., Any], type[BaseModel]]] = {
    "get_weather": (get_weather, WeatherArgs),
}
TOOL_SCHEMAS = [{
    "name": "get_weather",
    "description": "Get current weather for a city. Use only for weather questions.",
    "input_schema": WeatherArgs.model_json_schema(),
}]

MAX_RESULT_CHARS = 4000


def execute(call: Block, seen: set) -> tuple[str, bool]:
    """Run one tool call; return (output, is_error). Never raises."""
    key = (call.name, json.dumps(call.input, sort_keys=True))
    if key in seen:                                   # loop detection
        return "Duplicate call with identical arguments; try a different approach.", True
    seen.add(key)
    if call.name not in TOOLS:
        return f"Unknown tool {call.name!r}. Available: {sorted(TOOLS)}", True
    fn, schema = TOOLS[call.name]
    try:
        args = schema(**call.input)                   # validate before executing
    except ValidationError as e:
        return f"Invalid arguments: {e.errors(include_url=False)}", True
    try:
        return str(fn(**args.model_dump())), False
    except Exception as e:                            # tool bug: report, do not crash the loop
        return f"Tool error: {type(e).__name__}: {e}", True


def run_agent(llm: Callable[..., Response], user_msg: str, max_steps: int = 10) -> str:
    messages: list[dict] = [{"role": "user", "content": user_msg}]
    seen: set = set()
    for _ in range(max_steps):
        resp = llm(messages=messages, tools=TOOL_SCHEMAS)
        messages.append({"role": "assistant", "content": resp.content})
        calls = [b for b in resp.content if b.type == "tool_use"]
        if not calls:
            return "".join(b.text for b in resp.content if b.type == "text")
        results = []
        for c in calls:
            out, is_error = execute(c, seen)
            results.append({"type": "tool_result", "tool_use_id": c.id,
                            "content": out[:MAX_RESULT_CHARS], "is_error": is_error})
        messages.append({"role": "user", "content": results})  # all results in one turn
    return "Stopped: step limit reached."
```

**Talking points:** validation before execution, errors returned to the model as `is_error` results so it can self-correct, loop detection, output truncation, a step limit, and all parallel results returned in one turn.

**What I would add next:** per-tool timeouts and async parallel execution (Q62), tracing spans per call, retries with backoff for transient provider errors (Q59), a token budget, and a checkpoint after each step.

**Testing it deterministically** with a scripted fake model:

```python
import unittest

from q55_agent_loop import Block, Response, run_agent


def scripted(responses):
    it = iter(responses)
    calls = []

    def llm(messages, tools):
        calls.append([m for m in messages])
        return next(it)
    llm.calls = calls
    return llm


class TestAgentLoop(unittest.TestCase):
    def test_tool_then_answer(self):
        llm = scripted([
            Response([Block("tool_use", id="t1", name="get_weather", input={"city": "Paris"})]),
            Response([Block("text", text="It is sunny.")]),
        ])
        self.assertEqual(run_agent(llm, "weather?"), "It is sunny.")
        last = llm.calls[1][-1]["content"][0]
        self.assertEqual(last["tool_use_id"], "t1")
        self.assertFalse(last["is_error"])
        self.assertIn("Paris", last["content"])
```

Go deeper: [The Agent Loop From Scratch](../Agentic_AI_Zero_to_Godhood/Volume_03_Tool_Use_and_the_Agent_Loop/Chapter_03_The_Agent_Loop_From_Scratch.md), [Error Handling and Recovery](../Agentic_AI_Zero_to_Godhood/Volume_03_Tool_Use_and_the_Agent_Loop/Chapter_05_Error_Handling_and_Recovery.md).

---

## Q56. Cosine similarity and top-k retrieval with NumPy.

Normalize, take the dot product, then `argpartition` for an O(n) partial selection and sort only the k winners.

```python
import numpy as np


def top_k_cosine(query: np.ndarray, docs: np.ndarray, k: int) -> tuple[np.ndarray, np.ndarray]:
    """Return (indices, scores) of the k docs most similar to query, best first.

    query: shape (d,), docs: shape (n, d). O(n*d) for the scores, O(n) for argpartition.
    """
    if docs.ndim != 2 or query.shape != (docs.shape[1],):
        raise ValueError("query must be (d,) and docs (n, d)")
    k = min(k, len(docs))
    if k <= 0:
        return np.empty(0, dtype=int), np.empty(0)
    eps = 1e-12                                          # avoid divide-by-zero on zero vectors
    q = query / (np.linalg.norm(query) + eps)
    d = docs / (np.linalg.norm(docs, axis=1, keepdims=True) + eps)
    scores = d @ q
    idx = np.argpartition(-scores, k - 1)[:k]            # unordered top k in O(n)
    idx = idx[np.argsort(-scores[idx])]                  # sort only those k
    return idx, scores[idx]
```

Complexity: O(n d) for the scores, O(n + k log k) for the selection.
Edge cases handled: zero vectors, k larger than n, k of zero, and shape mismatches.

---

## Q57. Text chunker with overlap.

```python
def chunk_tokens(tokens: list[str], size: int, overlap: int) -> list[list[str]]:
    """Fixed-size windows of `size` tokens, each sharing `overlap` tokens with the previous one."""
    if size <= 0 or not 0 <= overlap < size:
        raise ValueError("need size > 0 and 0 <= overlap < size")
    step = size - overlap
    chunks = []
    for start in range(0, len(tokens), step):
        chunks.append(tokens[start:start + size])
        if start + size >= len(tokens):                  # last window reached the end
            break
    return chunks
```

For "10 tokens, size 4, overlap 1" the windows are `abcd`, `defg`, `ghij`.
In an interview, mention that production chunkers split on structure first (headings, paragraphs) and fall back to token windows.

---

## Q58. Reciprocal Rank Fusion.

```python
from collections import defaultdict


def rrf(rankings: list[list[str]], k: int = 60) -> list[tuple[str, float]]:
    """Fuse ranked lists of doc ids. score(d) = sum over lists of 1 / (k + rank), rank from 1."""
    scores: dict[str, float] = defaultdict(float)
    for ranking in rankings:
        for rank, doc_id in enumerate(ranking, start=1):
            scores[doc_id] += 1.0 / (k + rank)
    return sorted(scores.items(), key=lambda kv: (-kv[1], kv[0]))   # tie-break by id for determinism
```

Ranks start at 1; k = 60 by convention damps the advantage of the very top ranks.

---

## Q59. Retry decorator with exponential backoff and jitter.

```python
import functools
import random
import time


def retry(max_attempts: int = 5, base: float = 0.5, cap: float = 30.0,
          retry_on: tuple[type[BaseException], ...] = (TimeoutError, ConnectionError),
          sleep=time.sleep):
    """Retry transient failures. Full jitter: sleep uniform(0, min(cap, base * 2**attempt))."""
    if max_attempts < 1:
        raise ValueError("max_attempts must be >= 1")

    def decorator(fn):
        @functools.wraps(fn)
        def wrapper(*args, **kwargs):
            for attempt in range(max_attempts):
                try:
                    return fn(*args, **kwargs)
                except retry_on:
                    if attempt == max_attempts - 1:
                        raise                                    # budget spent: surface the real error
                    sleep(random.uniform(0, min(cap, base * 2 ** attempt)))
        return wrapper
    return decorator
```

**Talking points:** retry only transient errors (timeouts, 429, 5xx), never validation errors; full jitter avoids synchronized retry storms; cap the delay; re-raise the real error when the budget is spent; only retry idempotent operations, or pass an idempotency key.
The `sleep` parameter is injected so tests run instantly.

---

## Q60. Parse and validate structured LLM output, then repair and retry.

```python
import json
from typing import Callable, Literal

from pydantic import BaseModel, Field, ValidationError


class Ticket(BaseModel):
    category: Literal["bug", "feature", "question"]
    priority: int = Field(ge=1, le=4)
    summary: str = Field(min_length=1, max_length=200)


def extract_json(text: str) -> str:
    """Models often wrap JSON in prose or code fences; take the outermost {...} span."""
    start, end = text.find("{"), text.rfind("}")
    if start == -1 or end < start:
        raise ValueError("no JSON object found")
    return text[start:end + 1]


def parse_with_repair(llm: Callable[[str], str], prompt: str, max_attempts: int = 3) -> Ticket:
    """Ask, validate, and on failure feed the exact error back so the model can fix it."""
    attempt_prompt = prompt
    last_error = ""
    for _ in range(max_attempts):
        raw = llm(attempt_prompt)
        try:
            return Ticket.model_validate_json(extract_json(raw))
        except (ValueError, ValidationError) as e:        # ValidationError subclasses ValueError
            last_error = str(e)
            attempt_prompt = (f"{prompt}\n\nYour previous reply was invalid.\n"
                              f"Reply:\n{raw}\nError:\n{last_error}\n"
                              f"Return only a JSON object matching this schema:\n"
                              f"{json.dumps(Ticket.model_json_schema())}")
    raise ValueError(f"no valid output after {max_attempts} attempts: {last_error}")
```

**Talking points:** validation errors are fed back verbatim, which is the cheapest repair signal; the retry count is bounded; the final failure raises with the last error.
When the provider supports schema-constrained output or strict tools, use it first and keep this as the safety net.

---

## Q61. Write a tiny MCP server exposing one tool.

The Python SDK's high-level server class was renamed between major versions; know both forms.

**MCP Python SDK 2.x:**

```python
from mcp.server.mcpserver import MCPServer
from mcp.server.mcpserver.exceptions import ToolError

mcp = MCPServer("orders")


@mcp.tool()
def lookup_order(order_id: str) -> dict:
    """Look up one order by id (format ORD-<digits>). Read-only. Use before answering any refund question."""
    if not order_id.startswith("ORD-") or not order_id[4:].isdigit():
        # ToolError text reaches the model; any other exception is masked as "Error executing tool"
        raise ToolError("order_id must look like ORD-12345")
    return {"order_id": order_id, "status": "shipped", "total_usd": 42.50}


if __name__ == "__main__":
    mcp.run()          # stdio by default; mcp.run(transport="streamable-http") for a remote server
```

**MCP Python SDK 1.x** (still common in tutorials and existing code):

```python
from mcp.server.fastmcp import FastMCP

mcp = FastMCP("orders")

@mcp.tool()
def lookup_order(order_id: str) -> dict:
    """Look up one order by id (format ORD-<digits>). Read-only."""
    return {"order_id": order_id, "status": "shipped", "total_usd": 42.50}

if __name__ == "__main__":
    mcp.run()
```

**Details that show hands-on use:**

- The docstring becomes the tool description and the type hints become the input schema, so write both for the model.
- In 2.x, raise `ToolError` for failures the model should read; any other exception reaches the client only as "Error executing tool lookup_order".
- In the 2.x client, the result flag is `result.is_error` (1.x: `result.isError`).
- Test it with a client over stdio (`StdioServerParameters`, `stdio_client`, `ClientSession.initialize`, `list_tools`, `call_tool`) or with the MCP Inspector.

Go deeper: [Building and Testing Servers](../Agentic_AI_Zero_to_Godhood/Volume_09_Model_Context_Protocol/Chapter_06_Building_and_Testing_Servers.md).

---

## Q62. Run tool calls concurrently with per-call timeouts.

```python
import asyncio
from typing import Any, Awaitable, Callable


async def run_tools_concurrently(calls: list[tuple[str, Callable[[], Awaitable[Any]]]],
                                 timeout_s: float) -> list[dict]:
    """Run independent tool calls in parallel. One slow or failing call never sinks the others."""
    async def one(call_id: str, make_coro) -> dict:
        try:
            result = await asyncio.wait_for(make_coro(), timeout=timeout_s)
            return {"id": call_id, "ok": True, "content": result}
        except asyncio.TimeoutError:
            return {"id": call_id, "ok": False, "content": f"timed out after {timeout_s}s"}
        except Exception as e:
            return {"id": call_id, "ok": False, "content": f"{type(e).__name__}: {e}"}

    # gather preserves input order, so results line up with call ids for the tool_result blocks
    return await asyncio.gather(*(one(cid, mk) for cid, mk in calls))
```

**Talking points:** `asyncio.gather` keeps input order so results match call IDs; each call has its own timeout, so one slow tool does not block the turn; errors become results, not exceptions.
Mention that CPU-bound or blocking tools need `asyncio.to_thread` or a process pool, and that the concurrency level should be capped with a semaphore for rate-limited APIs.

---

## Q63. Token-budgeted history trimmer.

```python
from typing import Callable


def count_tokens(text: str) -> int:
    """Stand-in tokenizer: about 4 characters per token for English. Use the provider's counter in production."""
    return max(1, len(text) // 4)


def trim_history(messages: list[dict], budget: int,
                 summarize: Callable[[list[dict]], str] | None = None) -> list[dict]:
    """Keep the system message and the newest turns that fit in `budget` tokens.

    Dropped middle turns are replaced by one summary message when `summarize` is given.
    Tool-use and tool-result messages must stay paired; callers should group them into one
    message (as the provider APIs do) so this function never splits a pair.
    """
    system = [m for m in messages if m["role"] == "system"]
    rest = [m for m in messages if m["role"] != "system"]
    used = sum(count_tokens(m["content"]) for m in system)
    if used > budget:
        raise ValueError("system prompt alone exceeds the budget")
    kept: list[dict] = []
    for m in reversed(rest):                             # newest first
        cost = count_tokens(m["content"])
        if used + cost > budget:
            break
        kept.append(m)
        used += cost
    kept.reverse()
    dropped = rest[:len(rest) - len(kept)]
    if dropped and summarize is not None:
        # make room for the summary by evicting the oldest kept turns if needed
        while True:
            note = {"role": "user", "content": "Summary of earlier conversation: " + summarize(dropped)}
            if used + count_tokens(note["content"]) <= budget:
                kept.insert(0, note)
                break
            if not kept:
                break
            evicted = kept.pop(0)
            used -= count_tokens(evicted["content"])
            dropped.append(evicted)
    return system + kept
```

**Talking points:** the system prompt is always kept; the newest turns are kept first; a summary of dropped turns is optional; tool-use and tool-result pairs must never be split, because providers reject an orphaned result.
Use the provider's token counter in production; the character heuristic is only for the interview.

---

## Expansion problems (K1-K11)

### K1. Implement BM25.

```python
import math
from collections import Counter


class BM25:
    """Okapi BM25 over pre-tokenized documents. k1 controls term saturation, b length normalization."""

    def __init__(self, docs: list[list[str]], k1: float = 1.5, b: float = 0.75):
        self.docs, self.k1, self.b = docs, k1, b
        self.n = len(docs)
        self.avgdl = sum(map(len, docs)) / self.n if self.n else 0.0
        self.tf = [Counter(d) for d in docs]
        df = Counter(term for d in docs for term in set(d))
        # BM25+ style idf that stays positive for very common terms
        self.idf = {t: math.log(1 + (self.n - f + 0.5) / (f + 0.5)) for t, f in df.items()}

    def score(self, query: list[str], i: int) -> float:
        tf, dl = self.tf[i], len(self.docs[i])
        s = 0.0
        for t in query:
            if t in tf:
                f = tf[t]
                s += self.idf[t] * f * (self.k1 + 1) / (f + self.k1 * (1 - self.b + self.b * dl / self.avgdl))
        return s

    def top_k(self, query: list[str], k: int) -> list[tuple[int, float]]:
        scored = [(i, self.score(query, i)) for i in range(self.n)]
        return sorted(scored, key=lambda x: -x[1])[:k]
```

`k1` (about 1.2-2.0) sets how fast repeated terms saturate; `b` (about 0.75) sets how much long documents are penalized.

### K2. Implement a token-bucket rate limiter.

```python
import threading
import time


class TokenBucket:
    """Allow `rate` requests per second with bursts up to `capacity`. Thread-safe."""

    def __init__(self, rate: float, capacity: float, clock=time.monotonic):
        if rate <= 0 or capacity <= 0:
            raise ValueError("rate and capacity must be positive")
        self.rate, self.capacity, self.clock = rate, capacity, clock
        self.tokens, self.last = capacity, clock()
        self.lock = threading.Lock()

    def try_acquire(self, cost: float = 1.0) -> bool:
        with self.lock:
            now = self.clock()
            self.tokens = min(self.capacity, self.tokens + (now - self.last) * self.rate)
            self.last = now
            if self.tokens >= cost:
                self.tokens -= cost
                return True
            return False
```

Use it in front of a provider API or per tenant in a gateway; the injected clock makes it testable.

### K3. Implement a TTL plus LRU cache for tool results.

```python
import time
from collections import OrderedDict


class TTLCache:
    """Cache deterministic tool results: evict least recently used past `maxsize`, expire after `ttl` seconds."""

    def __init__(self, maxsize: int, ttl: float, clock=time.monotonic):
        self.maxsize, self.ttl, self.clock = maxsize, ttl, clock
        self.data: OrderedDict = OrderedDict()          # key -> (expires_at, value)

    def get(self, key):
        item = self.data.get(key)
        if item is None:
            return None
        expires_at, value = item
        if self.clock() >= expires_at:
            del self.data[key]
            return None
        self.data.move_to_end(key)                       # mark as recently used
        return value

    def put(self, key, value) -> None:
        self.data[key] = (self.clock() + self.ttl, value)
        self.data.move_to_end(key)
        while len(self.data) > self.maxsize:
            self.data.popitem(last=False)                # evict least recently used
```

Only cache deterministic, read-only tools, and key on the tool name plus canonical JSON of the arguments.

### K4. Implement a model cascade.

```python
from typing import Callable


def cascade(prompt: str, models: list[tuple[str, Callable[[str], str]]],
            accept: Callable[[str], bool]) -> tuple[str, str]:
    """Try models cheapest first; return (model_name, answer) from the first answer that passes `accept`.

    `accept` is a verifier (schema check, tests, a judge), not the model's own confidence.
    """
    last_error = "no models configured"
    for name, call in models:
        try:
            answer = call(prompt)
        except Exception as e:                           # provider error: escalate to the next model
            last_error = f"{name}: {type(e).__name__}: {e}"
            continue
        if accept(answer):
            return name, answer
        last_error = f"{name}: answer rejected by verifier"
    raise RuntimeError(f"all models failed; last: {last_error}")
```

The acceptance check must be an independent verifier (schema, tests, judge); a model's self-reported confidence is poorly calibrated.

### K5. Implement scaled dot-product attention with a causal mask in NumPy.

Asked often in AI-engineer loops, especially at model-adjacent companies.

```python
import numpy as np


def softmax(x: np.ndarray, axis: int = -1) -> np.ndarray:
    x = x - x.max(axis=axis, keepdims=True)           # subtract the max for numerical stability
    e = np.exp(x)
    return e / e.sum(axis=axis, keepdims=True)


def attention(q: np.ndarray, k: np.ndarray, v: np.ndarray, causal: bool = False) -> np.ndarray:
    """softmax(Q K^T / sqrt(d)) V for shapes q: (n, d), k: (m, d), v: (m, d_v)."""
    d = q.shape[-1]
    scores = q @ k.T / np.sqrt(d)                       # (n, m)
    if causal:
        n, m = scores.shape
        mask = np.triu(np.ones((n, m), dtype=bool), k=m - n + 1)   # hide future positions
        scores = np.where(mask, -np.inf, scores)
    return softmax(scores) @ v
```

**Talking points:** subtract the row max before `exp` so large logits do not overflow; dividing by `sqrt(d)` keeps the softmax from saturating as the dimension grows; the causal mask sets future positions to negative infinity.
The mask offset `m - n + 1` also handles a decode step, where one new query attends to every cached key.
Cost is O(n m d) time and O(n m) memory, which is what FlashAttention avoids materializing.

### K6. Execute tool calls that depend on each other (a DAG), in parallel where possible.

The LLMCompiler idea: the planner emits a dependency graph and the executor runs each call as soon as its inputs exist.

```python
import asyncio
from typing import Any, Awaitable, Callable


async def run_dag(tasks: dict[str, tuple[list[str], Callable[..., Awaitable[Any]]]]) -> dict[str, Any]:
    """Run tasks as soon as their dependencies finish; independent tasks run concurrently.

    tasks maps name -> (dependency names, async fn taking the dependency results as keyword args).
    """
    for name, (deps, _) in tasks.items():
        missing = [d for d in deps if d not in tasks]
        if missing:
            raise ValueError(f"{name} depends on unknown tasks {missing}")
    # Kahn's algorithm up front, so a cycle fails fast instead of deadlocking
    indegree = {n: len(deps) for n, (deps, _) in tasks.items()}
    children: dict[str, list[str]] = {n: [] for n in tasks}
    for n, (deps, _) in tasks.items():
        for d in deps:
            children[d].append(n)
    ready = [n for n, deg in indegree.items() if deg == 0]
    seen = 0
    while ready:
        n = ready.pop()
        seen += 1
        for c in children[n]:
            indegree[c] -= 1
            if indegree[c] == 0:
                ready.append(c)
    if seen != len(tasks):
        raise ValueError("dependency cycle detected")

    futures: dict[str, asyncio.Task] = {}

    async def run(name: str) -> Any:
        deps, fn = tasks[name]
        results = await asyncio.gather(*(futures[d] for d in deps))
        return await fn(**dict(zip(deps, results)))

    for name in tasks:                                   # create all tasks first, then await
        futures[name] = asyncio.ensure_future(run(name))
    return {name: await fut for name, fut in futures.items()}
```

**Talking points:** validate the graph and detect cycles before starting anything; start independent calls concurrently; pass dependency results by name.
Next steps: per-task timeouts (Q62), cancelling dependents when a task fails, and a concurrency cap.

### K7. Implement a semantic cache.

```python
from typing import Callable

import numpy as np


class SemanticCache:
    """Return a cached answer when a new query is close enough in embedding space.

    The threshold trades hit rate against wrong answers; tune it on labeled query pairs.
    """

    def __init__(self, embed: Callable[[str], np.ndarray], threshold: float = 0.92):
        self.embed, self.threshold = embed, threshold
        self.keys: list[np.ndarray] = []
        self.values: list[str] = []

    def _unit(self, text: str) -> np.ndarray:
        v = np.asarray(self.embed(text), dtype=float)
        return v / (np.linalg.norm(v) + 1e-12)

    def get(self, query: str) -> str | None:
        if not self.keys:
            return None
        sims = np.stack(self.keys) @ self._unit(query)
        best = int(np.argmax(sims))
        return self.values[best] if sims[best] >= self.threshold else None

    def put(self, query: str, answer: str) -> None:
        self.keys.append(self._unit(query))
        self.values.append(answer)
```

**Talking points:** exact-match caches miss paraphrases; a semantic cache hits on meaning but can return a wrong answer for a question that is close but different ("refund policy for EU" versus "for US").
Tune the threshold on labeled query pairs, scope entries by tenant and by anything that changes the answer (user, locale, date), and add a TTL.
At scale, replace the linear scan with an ANN index.

### K8. Implement a small vector store with upsert, delete, and a metadata filter.

```python
from typing import Callable

import numpy as np


class VectorStore:
    """Exact-search store with ids, deletes, and a metadata filter applied before ranking (for ACLs)."""

    def __init__(self, dim: int):
        self.dim = dim
        self.ids: list[str] = []
        self.vecs = np.empty((0, dim))
        self.meta: list[dict] = []

    def upsert(self, doc_id: str, vec, meta: dict) -> None:
        v = np.asarray(vec, dtype=float)
        if v.shape != (self.dim,):
            raise ValueError(f"expected dim {self.dim}")
        v = v / (np.linalg.norm(v) + 1e-12)
        if doc_id in self.ids:                          # update in place
            i = self.ids.index(doc_id)
            self.vecs[i], self.meta[i] = v, meta
        else:
            self.ids.append(doc_id)
            self.vecs = np.vstack([self.vecs, v])
            self.meta.append(meta)

    def delete(self, doc_id: str) -> bool:
        if doc_id not in self.ids:
            return False
        i = self.ids.index(doc_id)
        del self.ids[i], self.meta[i]
        self.vecs = np.delete(self.vecs, i, axis=0)
        return True

    def search(self, vec, k: int, where: Callable[[dict], bool] = lambda m: True) -> list[tuple[str, float]]:
        allowed = [i for i, m in enumerate(self.meta) if where(m)]   # pre-filter: never rank forbidden docs
        if not allowed or k <= 0:
            return []
        q = np.asarray(vec, dtype=float)
        q = q / (np.linalg.norm(q) + 1e-12)
        scores = self.vecs[allowed] @ q
        order = np.argsort(-scores)[:k]
        return [(self.ids[allowed[j]], float(scores[j])) for j in order]
```

**Talking points:** the filter runs before ranking, so a forbidden document can never be returned (ACLs, G2 in [RAG](03-RAG-and-Knowledge-Systems.md)); vectors are normalized on write so search is a dot product; upsert keeps ids unique.
`np.vstack` per insert is O(n); a real store preallocates or batches, and uses an ANN index.

### K9. Implement pass@k and pass^k.

```python
from math import comb


def pass_at_k(n: int, c: int, k: int) -> float:
    """Unbiased estimate of P(at least one of k samples passes), given c passes in n samples."""
    if not 0 <= c <= n or not 1 <= k <= n:
        raise ValueError("need 0 <= c <= n and 1 <= k <= n")
    if n - c < k:
        return 1.0
    return 1.0 - comb(n - c, k) / comb(n, k)


def pass_hat_k(n: int, c: int, k: int) -> float:
    """Estimate of P(all k samples pass): the reliability metric from tau-bench."""
    if not 0 <= c <= n or not 1 <= k <= n:
        raise ValueError("need 0 <= c <= n and 1 <= k <= n")
    return comb(c, k) / comb(n, k)
```

With 7 passes in 10 trials, pass@3 is 0.99 and pass^3 is 0.29 (see Q28 in [Evals](04-Evals-Reliability-Observability.md)).
Use the combinatorial form, not `1 - (1 - c/n)^k`, which is biased when n is small.

### K10. Rebuild tool calls from a streamed response.

Streaming APIs send a tool call's arguments as text fragments; this is the bug-prone part of every streaming UI.

```python
import json


def accumulate_tool_calls(deltas: list[dict]) -> list[dict]:
    """Rebuild complete tool calls from streamed chunks.

    Each delta looks like {"index": 0, "id": "call_1", "name": "search", "arguments": '{"q": "ab'}.
    id and name arrive once; argument text arrives in fragments keyed by index.
    Parse the JSON only after the stream ends; fragments are not valid JSON on their own.
    """
    calls: dict[int, dict] = {}
    for d in deltas:
        call = calls.setdefault(d["index"], {"id": None, "name": None, "arguments": ""})
        if d.get("id"):
            call["id"] = d["id"]
        if d.get("name"):
            call["name"] = d["name"]
        call["arguments"] += d.get("arguments") or ""
    out = []
    for i in sorted(calls):
        c = calls[i]
        try:
            args = json.loads(c["arguments"] or "{}")
        except json.JSONDecodeError as e:
            raise ValueError(f"tool call {i} has malformed arguments: {e}") from e
        out.append({"id": c["id"], "name": c["name"], "arguments": args})
    return out
```

**Talking points:** fragments are keyed by the call's index, several calls can interleave, and the JSON is parsed only when the stream ends.
Show the user progress while streaming, but never execute a tool from partial arguments.

### K11. Parse a text-based ReAct step for a model without native tool calling.

Common with small local models and older prompts.

```python
import json
import re

ACTION_RE = re.compile(r"^Action:\s*(\w+)\s*\n+Action Input:\s*(.+?)\s*$", re.M | re.S)
FINAL_RE = re.compile(r"^Final Answer:\s*(.+)$", re.M | re.S)


def parse_react(text: str) -> tuple[str, str | dict]:
    """Parse one step of a text-based ReAct model (no native tool calling).

    Returns ("final", answer) or ("action", {"tool": name, "input": parsed_input}).
    Raises ValueError with a message that can be fed back to the model.
    """
    final = FINAL_RE.search(text)
    action = ACTION_RE.search(text)
    if final and action:
        raise ValueError("Output contained both an Action and a Final Answer; give exactly one.")
    if final:
        return "final", final.group(1).strip()
    if not action:
        raise ValueError("Could not parse. Use 'Action: <tool>' then 'Action Input: <json>', or 'Final Answer: <text>'.")
    raw = action.group(2).strip()
    try:
        tool_input = json.loads(raw)
    except json.JSONDecodeError:
        tool_input = raw                                   # tolerate a bare string input
    return "action", {"tool": action.group(1), "input": tool_input}
```

**Talking points:** parse errors return a message the model can act on; output with both an action and a final answer is rejected; stop generation at "Observation:" with a stop sequence so the model cannot invent the tool result.
Prefer native tool calling or constrained decoding when available.

Go deeper: [Build Your Own Coding Agent](../Agentic_AI_Zero_to_Godhood/Volume_13_Coding_Agents_and_Computer_Use/Chapter_07_Build_Your_Own_Coding_Agent.md).
