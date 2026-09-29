---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://www.rfc-editor.org/rfc/rfc9110, https://www.rfc-editor.org/rfc/rfc9111, https://www.rfc-editor.org/rfc/rfc9457, https://www.rfc-editor.org/rfc/rfc5789, https://www.rfc-editor.org/rfc/rfc7396, https://www.rfc-editor.org/rfc/rfc6902, https://www.rfc-editor.org/rfc/rfc6585, https://www.rfc-editor.org/rfc/rfc8594, https://www.rfc-editor.org/rfc/rfc9745, https://www.rfc-editor.org/rfc/rfc8288, https://datatracker.ietf.org/doc/draft-ietf-httpapi-ratelimit-headers/, https://datatracker.ietf.org/doc/draft-ietf-httpapi-idempotency-key-header/, https://spec.openapis.org/oas/latest.html, https://fastapi.tiangolo.com/advanced/additional-responses/, https://aws.amazon.com/builders-library/timeouts-retries-and-backoff-with-jitter/, https://www.sqlite.org/rowvalue.html]
---

# REST API design

REST design questions for a Flask and FastAPI backend role: resource modeling, HTTP semantics, pagination, versioning, idempotency, concurrency, caching, rate limiting, and integrating with third-party APIs.
R1 through R8 are near-certain in some form; R8 (idempotency keys) and R6 (pagination) are where senior candidates separate themselves.
Every code sample here was run on 2026-09-29 under Python 3.14.7 with FastAPI 0.142.0, Pydantic 2.13.5, httpx 0.28.1, and SQLite from the Python 3.14 stdlib: 34 pytest tests passed.

---

## R1. What makes an API RESTful? (must know)

> "REST is an architectural style, not a protocol.
> Its constraints are client-server, stateless requests, cacheable responses, a uniform interface, a layered system, and optionally code on demand.
> In practice 'RESTful' means resources identified by URLs, manipulated through standard HTTP methods with correct semantics, self-descriptive messages with proper status codes and media types, and no server-side session state between requests."

- **Uniform interface** is the core: resource identification in the URL, manipulation through representations (JSON), self-descriptive messages (method, status, headers), and hypermedia (HATEOAS, see R21).
- **Stateless** means each request carries everything needed to process it (auth token, cursor, idempotency key).
  State lives in the database, not in a sticky server session, which is what makes horizontal scaling behind a load balancer trivial.
- **Cacheable** means responses declare their cacheability (`Cache-Control`, `ETag`), see R12.
- **Layered** means clients cannot tell whether they talk to the origin, a gateway, a CDN, or a load balancer.

Richardson maturity model, a common follow-up:

| Level | What it means | Example |
| --- | --- | --- |
| 0 | One endpoint, one method, RPC over HTTP | `POST /api` with an action in the body |
| 1 | Resources | `POST /orders/42` |
| 2 | Resources plus HTTP verbs and status codes | `GET /orders/42`, `DELETE /orders/42` returns 204 |
| 3 | Plus hypermedia controls | Responses carry `links` to next actions |

Most production "REST" APIs are level 2, and that is fine to say out loud.

---

## R2. Which HTTP methods are safe and idempotent? (must know)

- **Safe**: no intended state change (read-only), so crawlers, caches, and prefetchers may call it freely.
- **Idempotent**: making the same request N times leaves the server in the same state as making it once.
  The response can differ (a second `DELETE` may return 404); idempotency is about server state.
- Idempotency is what makes a request **safe to retry** after a timeout, which is the real reason interviewers ask.

| Method | Safe | Idempotent | Typical use | Success codes |
| --- | --- | --- | --- | --- |
| GET | Yes | Yes | Read a resource or collection | 200, 304 |
| HEAD | Yes | Yes | Headers only (existence, size, ETag) | 200, 304 |
| OPTIONS | Yes | Yes | Capabilities, CORS preflight | 200, 204 |
| PUT | No | Yes | Replace the full resource at a known URL (or create it there) | 200, 201, 204 |
| DELETE | No | Yes | Remove a resource | 204, 200, 202 |
| POST | No | No | Create in a collection, or trigger a process | 201, 202, 200 |
| PATCH | No | Not necessarily | Partial update | 200, 204 |

**PATCH is not guaranteed idempotent**: a JSON Patch `add` to the end of an array (`"path": "/tags/-"`) appends again on every retry, and an "increment by 1" patch is obviously not idempotent.
A JSON Merge Patch that only sets fields is idempotent in practice, but the method itself makes no promise (RFC 5789).

**POST can be made idempotent** with an `Idempotency-Key` header (R8).

Follow-ups they ask:

- *Can GET have a body?* RFC 9110 says content in a GET request has no generally defined semantics and some intermediaries reject or drop it.
  For complex search, use `POST /trades/search` (and say that it is a safe operation despite the method), or watch the IETF `QUERY` method draft.
- *Is DELETE on a missing resource a 404 or a 204?* Either is defensible; pick one and document it.
  Returning 204 makes client retries simpler; returning 404 surfaces client bugs.

---

## R3. Which status codes do you use, and when? (must know)

> "I use a small, consistent set: 2xx for success with the right flavor, 4xx when the client must change something, 5xx when the server or an upstream failed and a retry might work.
> The two distinctions interviewers probe are 401 versus 403 and 400 versus 422 versus 409."

| Code | Meaning | When to use it |
| --- | --- | --- |
| 200 OK | Success with a body | GET, or PUT/PATCH that returns the updated resource |
| 201 Created | New resource | POST (or PUT) that created something; include `Location: /orders/42` |
| 202 Accepted | Queued, not done | Long-running work; return a status resource (R14) |
| 204 No Content | Success, empty body | DELETE, or PUT/PATCH when you return nothing |
| 301 / 308 | Permanent redirect | Moved URLs; 308 keeps the method and body, 301 lets clients turn POST into GET |
| 304 Not Modified | Cached copy is still valid | Conditional GET with `If-None-Match` (R12) |
| 400 Bad Request | Malformed request | Unparseable JSON, wrong types, missing required header |
| 401 Unauthorized | Not authenticated | Missing, expired, or invalid credentials; must send `WWW-Authenticate` |
| 403 Forbidden | Authenticated, not allowed | Valid token lacking the scope or role |
| 404 Not Found | No such resource | Also used deliberately to hide a resource the caller may not see |
| 405 Method Not Allowed | Wrong verb for this URL | Send an `Allow` header listing valid methods |
| 409 Conflict | Conflicts with current state | Duplicate unique key, invalid state transition (cancel a filled order) |
| 412 Precondition Failed | `If-Match` did not match | Optimistic concurrency failure (R13) |
| 415 Unsupported Media Type | Wrong `Content-Type` | Client sent XML or form data to a JSON endpoint |
| 422 Unprocessable Content | Well-formed but semantically invalid | Validation errors; FastAPI's default for request validation |
| 429 Too Many Requests | Rate limited | Include `Retry-After` (R11) |
| 500 Internal Server Error | Bug | Unhandled exception; never leak the stack trace |
| 502 Bad Gateway | Upstream sent a bad response | Gateway or your service calling a broken dependency |
| 503 Service Unavailable | Temporarily overloaded or down | Load shedding, maintenance; include `Retry-After` |
| 504 Gateway Timeout | Upstream did not answer in time | Your dependency call timed out |

Pitfalls:

- **401 vs 403**: 401 means "I do not know who you are" (re-authenticate); 403 means "I know who you are, and the answer is no" (re-authenticating will not help).
  The tested FastAPI dependency in [Auth and Security U9](07-Auth-and-Security.md) returns exactly these.
- **400 vs 422**: FastAPI returns 422 for body validation errors and Flask apps typically return 400; either is fine if consistent.
  Use 400 for syntax, 422 for semantics, 409 for conflicts with existing state.
- **Returning 200 with `{"error": ...}`** breaks client retry logic, caches, load-balancer health checks, and error-rate dashboards.
- Map upstream failures deliberately: a dependency timeout is a 504 from you, not a 500.

---

## R4. PUT versus PATCH: which do you use, and what goes in a PATCH body? (must know)

- **PUT** replaces the whole resource with the request body; omitted fields are reset or rejected.
  It is idempotent, so it is safe to retry.
- **PATCH** applies a partial change described by a patch document; the media type says how to interpret it.
- Two standard PATCH formats:

| | JSON Merge Patch (RFC 7396) | JSON Patch (RFC 6902) |
| --- | --- | --- |
| Content-Type | `application/merge-patch+json` | `application/json-patch+json` |
| Body | A partial object: `{"limit": 200, "nickname": null}` | A list of operations: `[{"op": "replace", "path": "/limit", "value": 200}]` |
| Delete a field | Set it to `null` | `{"op": "remove", "path": "/nickname"}` |
| Array edits | Replaces the whole array | Can add, remove, move items by index |
| Set a field to literal null | Impossible (null means delete) | Possible |
| Atomic test-and-set | No | `{"op": "test", ...}` fails the whole patch |
| Idempotent | Yes for field sets | No for array appends |

In FastAPI, a merge-patch endpoint uses a Pydantic model with all-optional fields and `model_dump(exclude_unset=True)`, so "not sent" and "sent as null" stay distinguishable.

```python
from pydantic import BaseModel


class AccountPatch(BaseModel):
    name: str | None = None
    limit: int | None = None


def apply_merge_patch(current: dict, patch: AccountPatch) -> dict:
    changes = patch.model_dump(exclude_unset=True)  # {"limit": 200} if only limit was sent
    return {**current, **changes}  # an explicit null survives, so the client can clear a field
```

Pitfall (tested): `model_dump(exclude_none=True)` loses the "set this to null" intent; `exclude_unset=True` is the one you want.

---

## R5. How do you name resources and structure URLs? (must know)

- Nouns, plural, lowercase, kebab-case: `/trade-allocations`, not `/getTradeAllocation`.
- Hierarchy only for true containment, at most one or two levels: `/accounts/{account_id}/positions`.
  Deeper nesting (`/desks/1/books/2/trades/3/fills/4`) couples clients to your data model; give sub-entities their own top-level route with a filter instead: `/fills?trade_id=3`.
- Filters, sorting, and pagination go in the query string, not the path (R16).
- Actions that do not map to CRUD become sub-resources or custom methods: `POST /orders/{id}/cancel` or Google AIP-style `POST /orders/{id}:cancel`.
  Cancel is a state transition, so a 409 is the natural answer when the order already filled.
- Use opaque identifiers (UUID, ULID) in public URLs rather than auto-increment integers, which leak volume and invite enumeration (OWASP API1, see [U18](07-Auth-and-Security.md)).
- Pick one JSON casing (snake_case is natural for Python services) and one date format (RFC 3339 / ISO 8601 in UTC) and never mix.
- Pick trailing-slash behavior once; redirect the other form with 308 or reject it consistently.

---

## R6. How do you paginate a large collection? Offset versus cursor. (must know)

> "Offset pagination is simple and supports jumping to page N, but the database still reads and discards every skipped row, so page 10,000 is slow, and concurrent inserts shift rows so clients see duplicates or skip items.
> Keyset (cursor) pagination remembers the sort key of the last row and asks for rows after it, which is an index seek, constant cost at any depth, and stable under inserts.
> I default to keyset for anything large or append-heavy and hide the key in an opaque cursor."

| | Offset (`?limit=50&offset=5000`) | Keyset / cursor (`?limit=50&cursor=...`) |
| --- | --- | --- |
| Cost at depth | O(offset + limit): rows are scanned and discarded | O(log n + limit) with a matching index |
| Concurrent inserts or deletes | Duplicates and skipped rows | Stable: each row seen at most once |
| Jump to page N | Yes | No (next and previous only) |
| Sort requirements | Any | A total order: sort key plus a unique tie-breaker |
| Good for | Admin UIs, small tables | Feeds, exports, sync, public APIs |

Tested implementation (plain Python plus SQLite; the SQL is the same in PostgreSQL):

```python
import base64
import json
import sqlite3


class InvalidCursor(ValueError):
    pass


def encode_cursor(created_at: str, row_id: int) -> str:
    raw = json.dumps([created_at, row_id], separators=(",", ":")).encode()
    return base64.urlsafe_b64encode(raw).decode().rstrip("=")


def decode_cursor(cursor: str) -> tuple[str, int]:
    try:
        padded = cursor + "=" * (-len(cursor) % 4)
        created_at, row_id = json.loads(base64.urlsafe_b64decode(padded))
        if not isinstance(created_at, str) or not isinstance(row_id, int):
            raise TypeError
        return created_at, row_id
    except (ValueError, TypeError) as exc:  # binascii.Error and JSONDecodeError are ValueErrors
        raise InvalidCursor("malformed cursor") from exc


def list_trades(conn: sqlite3.Connection, limit: int = 50, cursor: str | None = None) -> dict:
    """Newest first, ordered by (created_at DESC, id DESC); id breaks ties so order is total."""
    limit = max(1, min(limit, 200))  # clamp page size server side
    params: list = []
    where = ""
    if cursor is not None:
        created_at, row_id = decode_cursor(cursor)
        where = "WHERE (created_at, id) < (?, ?)"  # row-value comparison: SQLite 3.15+, PostgreSQL
        params = [created_at, row_id]
    rows = conn.execute(
        f"SELECT id, created_at, symbol FROM trades {where} "
        "ORDER BY created_at DESC, id DESC LIMIT ?",
        [*params, limit + 1],  # fetch one extra to learn whether another page exists
    ).fetchall()
    page, has_more = rows[:limit], len(rows) > limit
    next_cursor = encode_cursor(page[-1][1], page[-1][0]) if has_more else None
    return {
        "items": [{"id": r[0], "created_at": r[1], "symbol": r[2]} for r in page],
        "next_cursor": next_cursor,
    }
```

The index that makes it an index seek, matching the ORDER BY exactly:

```sql
CREATE INDEX ix_trades_created_id ON trades (created_at DESC, id DESC);
```

The tests walk every page at several page sizes and assert each row appears exactly once and in order, insert a newer row mid-walk and assert no duplicates, reject malformed cursors, and check `EXPLAIN QUERY PLAN` uses the index.

Pitfalls and follow-ups:

- **Tie-breaker**: ordering by `created_at` alone is not a total order; rows with equal timestamps get skipped or repeated across page boundaries.
  Always append the primary key.
- **Mixed directions**: the row-value comparison `(a, b) < (x, y)` only works when every sort column goes the same direction.
  For `ORDER BY a DESC, b ASC`, expand it: `a < :x OR (a = :x AND b > :y)`.
- **Opaque cursors**: base64 is encoding, not security.
  If a cursor must not be tampered with, sign it (HMAC) or encrypt it; always validate it and return 400 on garbage.
- **Total counts are expensive**: `COUNT(*)` in PostgreSQL scans every visible row because of MVCC.
  Return `has_more` or `next_cursor` instead, cap the count ("10,000+"), or use an estimate from the planner statistics.
- Clamp `limit` on the server; never trust `?limit=1000000`.
- Return links: `{"items": [...], "next_cursor": "..."}` or an RFC 8288 `Link: <...>; rel="next"` header.

---

## R7. How do you version an API? (must know)

> "I put the major version in the path, `/v1/`, and evolve additively inside a version so most changes never need a new one.
> A new major version is for genuinely breaking changes, runs side by side with the old one, and the old one gets Deprecation and Sunset headers plus usage monitoring before removal."

| Strategy | Example | Pros | Cons |
| --- | --- | --- | --- |
| URI path | `/v1/orders` | Visible, easy routing and gateway rules, cache-friendly, easy to test with curl | Version is not really part of the resource identity; coarse-grained |
| Custom header | `Api-Version: 2` | Clean URLs | Invisible in logs and browsers; caches need `Vary: Api-Version` |
| Media type | `Accept: application/vnd.acme.v2+json` | Most "RESTful"; per-resource versions | Awkward for clients and tooling; needs `Vary: Accept` |
| Query parameter | `/orders?version=2` | Easy | Pollutes caching and is easy to forget |
| Date-pinned | Account pinned to `2026-09-01`, overridable per request | Fine-grained, many small changes | Needs a compatibility layer per change; heavy to build |

In FastAPI, versioning by path is just two `APIRouter`s with `prefix="/v1"` and `prefix="/v2"` sharing the service layer; in Flask, two blueprints with `url_prefix`.
Keep version translation at the edge (request and response models), never scattered through business logic.

Follow-up: *what counts as breaking?* See R15.

---

## R8. How do you make a POST safe to retry? Implement idempotency keys. (must know)

> "The client generates a unique key per logical operation (a UUID) and sends it in an `Idempotency-Key` header.
> The server stores the key with a fingerprint of the request and the response it produced, in the same transaction as the business write, under a unique constraint.
> A retry with the same key and body gets the stored response replayed instead of a second order; the same key with a different body is a client bug and gets rejected; keys expire after a TTL such as 24 hours."

Tested FastAPI implementation using SQLite (swap in PostgreSQL and SQLAlchemy in production; the shape is identical):

```python
import hashlib
import json
import sqlite3
import time
from typing import Annotated

from fastapi import FastAPI, Header, HTTPException, Request
from fastapi.responses import JSONResponse
from pydantic import BaseModel

SCHEMA = """
CREATE TABLE IF NOT EXISTS orders (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    client_id TEXT NOT NULL,
    symbol TEXT NOT NULL,
    qty INTEGER NOT NULL
);
CREATE TABLE IF NOT EXISTS idempotency_keys (
    client_id     TEXT NOT NULL,
    key           TEXT NOT NULL,
    request_hash  TEXT NOT NULL,
    status_code   INTEGER NOT NULL,
    response_body TEXT NOT NULL,
    created_at    REAL NOT NULL,
    PRIMARY KEY (client_id, key)            -- the unique constraint does the dedupe
);
"""


class OrderIn(BaseModel):
    symbol: str
    qty: int


def request_fingerprint(method: str, path: str, body: dict) -> str:
    canonical = json.dumps(body, sort_keys=True, separators=(",", ":"))
    return hashlib.sha256(f"{method} {path} {canonical}".encode()).hexdigest()


def create_app(db_path: str) -> FastAPI:
    app = FastAPI()

    def connect() -> sqlite3.Connection:
        conn = sqlite3.connect(db_path, isolation_level=None)  # explicit BEGIN/COMMIT below
        conn.row_factory = sqlite3.Row
        return conn

    with connect() as conn:
        conn.executescript(SCHEMA)

    @app.post("/orders", status_code=201)
    def create_order(
        order: OrderIn,
        request: Request,
        idempotency_key: Annotated[str | None, Header()] = None,
        x_client_id: Annotated[str, Header()] = "anonymous",  # in real life: from the auth token
    ):
        if not idempotency_key:
            raise HTTPException(400, "Idempotency-Key header is required")
        fp = request_fingerprint("POST", request.url.path, order.model_dump())
        conn = connect()
        try:
            conn.execute("BEGIN IMMEDIATE")
            row = conn.execute(
                "SELECT request_hash, status_code, response_body FROM idempotency_keys"
                " WHERE client_id = ? AND key = ?",
                (x_client_id, idempotency_key),
            ).fetchone()
            if row is not None:
                conn.execute("ROLLBACK")
                if row["request_hash"] != fp:
                    raise HTTPException(422, "Idempotency-Key reused with a different request")
                return JSONResponse(
                    json.loads(row["response_body"]),
                    status_code=row["status_code"],
                    headers={"Idempotent-Replayed": "true"},
                )
            # Business write and idempotency record commit (or roll back) together.
            cur = conn.execute(
                "INSERT INTO orders (client_id, symbol, qty) VALUES (?, ?, ?)",
                (x_client_id, order.symbol, order.qty),
            )
            body = {"id": cur.lastrowid, "symbol": order.symbol, "qty": order.qty}
            conn.execute(
                "INSERT INTO idempotency_keys VALUES (?, ?, ?, ?, ?, ?)",
                (x_client_id, idempotency_key, fp, 201, json.dumps(body), time.time()),
            )
            conn.execute("COMMIT")
        except sqlite3.IntegrityError:
            # A concurrent request with the same key committed first; tell the client to retry.
            conn.execute("ROLLBACK")
            raise HTTPException(409, "A request with this Idempotency-Key is in flight")
        finally:
            conn.close()
        return JSONResponse(body, status_code=201, headers={"Location": f"/orders/{body['id']}"})

    return app


def purge_expired(db_path: str, ttl_seconds: float, now: float | None = None) -> int:
    now = time.time() if now is None else now
    with sqlite3.connect(db_path) as conn:
        cur = conn.execute("DELETE FROM idempotency_keys WHERE created_at < ?", (now - ttl_seconds,))
        return cur.rowcount
```

What the tests prove: the first call returns 201 with `Location`; a retry (even with keys in a different JSON order) replays the same body with `Idempotent-Replayed: true` and the orders table has exactly one row; the same key with a different body gets 422; a missing key gets 400; keys are scoped per client; the TTL purge deletes only expired keys.

Design points to say out loud:

- **Same transaction** as the business write, so there is never an order without a key record or a key record without an order.
- **Unique constraint on (client, key)** is the concurrency control.
  In PostgreSQL, `INSERT ... ON CONFLICT (client_id, key) DO NOTHING` as the first statement makes a concurrent duplicate block until the first transaction commits, then see the conflict and replay.
- **Scope keys per client** (from the auth token, not a header in real life), or two tenants who pick the same UUID collide.
- **Fingerprint** the method, path, and canonical body; reject a reused key with a different request.
- **TTL**: purge keys older than the retry window (24 hours is common) with a scheduled job or a partitioned table.
- **Only cache final outcomes**: store 2xx and deterministic 4xx; do not store 5xx, so the client can retry after a transient failure.
- **External side effects** (charging a card, sending to an exchange) cannot share your transaction.
  Record an `in_progress` state first, pass your key downstream if the provider supports its own idempotency key, and make recovery resume from the recorded step.
- Status of the standard: the IETF `Idempotency-Key` header draft (draft-ietf-httpapi-idempotency-key-header) expired without becoming an RFC, but it documents the common convention: 400 for a missing key, 409 while the original is still in flight, 422 for a reused key with a different payload.

[fill in: a place in your trading or platform services where a retried write could double-book, and how you deduplicated it]

---

## R9. What should an error response look like?

> "One error shape across every service, using RFC 9457 Problem Details: `application/problem+json` with `type`, `title`, `status`, `detail`, and `instance`, plus extension fields like a machine-readable error code, validation errors, and a trace id.
> Clients branch on `status` and `type`, humans read `detail`, and support searches logs by the trace id."

```json
{
  "type": "https://api.example.com/problems/insufficient-limit",
  "title": "Insufficient credit limit",
  "status": 409,
  "detail": "Order notional 12,000,000 exceeds remaining limit 10,000,000.",
  "instance": "/orders/7f3c",
  "error_code": "LIMIT_EXCEEDED",
  "trace_id": "4bf92f3577b34da6a3ce929d0e0e4736"
}
```

A FastAPI handler that turns every HTTP error, including router 404 and 405, into problem details (from the same tested module as R13):

```python
@app.exception_handler(StarletteHTTPException)  # also catches router 404/405
async def problem_details(request: Request, exc: StarletteHTTPException) -> JSONResponse:
    """RFC 9457 problem details for every HTTPException."""
    return JSONResponse(
        {
            "type": "about:blank",  # with about:blank, title should be the status phrase
            "title": HTTPStatus(exc.status_code).phrase,
            "status": exc.status_code,
            "detail": exc.detail,
            "instance": request.url.path,
        },
        status_code=exc.status_code,
        media_type="application/problem+json",
        headers=exc.headers,
    )
```

Pitfalls:

- Register the handler for Starlette's `HTTPException`, not only FastAPI's, or router-level 404 and 405 keep the default `{"detail": ...}` shape.
- Also override `RequestValidationError` so 422 validation errors share the shape (put the Pydantic error list in an `errors` extension field).
- RFC 9457 obsoletes RFC 7807; the format is the same, so either citation is understood.
- Never put stack traces, SQL, or internal hostnames in `detail`; log them with the trace id instead.

---

## R10. How should a client call a third-party API: timeouts, retries, backoff? (must know)

> "Every outbound call gets explicit timeouts, because the default in `requests` is to wait forever.
> I retry only idempotent calls, or POSTs protected by an idempotency key, and only on transient failures: connection errors, 408, 429, 502, 503, 504.
> Retries use exponential backoff with full jitter, honor `Retry-After`, are capped in attempts and total time, and sit behind a circuit breaker so a dead dependency is not hammered."

```python
import random
import time
from email.utils import parsedate_to_datetime

import httpx

RETRYABLE_STATUS = {408, 429, 502, 503, 504}
IDEMPOTENT_METHODS = {"GET", "HEAD", "OPTIONS", "PUT", "DELETE"}


def backoff_delay(attempt: int, base: float = 0.2, cap: float = 10.0) -> float:
    """Full jitter: uniform in [0, min(cap, base * 2**attempt)]."""
    return random.uniform(0, min(cap, base * 2**attempt))


def retry_after_seconds(value: str | None) -> float | None:
    if value is None:
        return None
    if value.isdigit():
        return float(value)
    try:  # Retry-After may also be an HTTP-date
        return max(0.0, parsedate_to_datetime(value).timestamp() - time.time())
    except (TypeError, ValueError):
        return None


def request_with_retries(
    client: httpx.Client,
    method: str,
    url: str,
    *,
    max_attempts: int = 4,
    sleep=time.sleep,
    **kwargs,
) -> httpx.Response:
    headers = kwargs.pop("headers", {})
    # POST is only safe to retry when the server dedupes it with an Idempotency-Key.
    retry_safe = method.upper() in IDEMPOTENT_METHODS or "Idempotency-Key" in headers
    for attempt in range(max_attempts):
        try:
            resp = client.request(method, url, headers=headers, **kwargs)
        except httpx.TransportError:  # connect errors, timeouts, resets
            if not retry_safe or attempt == max_attempts - 1:
                raise
            sleep(backoff_delay(attempt))
            continue
        if resp.status_code not in RETRYABLE_STATUS or not retry_safe or attempt == max_attempts - 1:
            return resp
        hinted = retry_after_seconds(resp.headers.get("Retry-After"))
        sleep(hinted if hinted is not None else backoff_delay(attempt))
    raise AssertionError("unreachable")
```

The tests use `httpx.MockTransport` to prove: GET retries through two 503s; a POST without a key is not retried; a POST with an `Idempotency-Key` is; `Retry-After` in seconds and as an HTTP-date is honored; 400 is never retried; the backoff never exceeds its cap.

Points interviewers look for:

- **Timeouts**: `requests` has no default timeout; `httpx` defaults to 5 seconds.
  Set connect and read timeouts separately and keep them below the caller's own deadline.
- **Full jitter** spreads retries so thousands of clients do not reconverge on the same instant (the AWS Builders' Library analysis).
- **Retry amplification**: three layers each retrying three times turn one user request into up to 64 calls to the bottom service.
  Retry at one layer, usually the one closest to the user or the one that owns the idempotency key.
- **Retry budgets** (for example, retries capped at 10 percent of traffic) and **circuit breakers** stop retry storms during an outage.
- Libraries: `tenacity` for decorators, `urllib3.util.Retry` mounted on a `requests.HTTPAdapter`; `httpx.HTTPTransport(retries=n)` retries only connection failures, not status codes.
- Deeper resilience patterns (bulkheads, circuit breakers, timeouts budgets) are in [Microservices and Messaging](08-Microservices-and-Messaging.md).

---

## R11. How do you rate-limit an API?

> "Rate limiting protects the service and enforces fairness per client.
> I prefer a token bucket, which allows short bursts up to the bucket size while enforcing a sustained rate, keyed by API key or user, enforced at the gateway where possible and in Redis when the limit must be shared across app instances.
> Over the limit, the answer is 429 with a `Retry-After` header."

| Algorithm | How it works | Pros | Cons |
| --- | --- | --- | --- |
| Token bucket | Tokens refill at rate r up to capacity b; each request takes one | Allows bursts, O(1) state | Burst size needs tuning |
| Leaky bucket | Queue drained at a constant rate | Smooth output | Adds latency; queue can fill |
| Fixed window | Counter per clock minute | Trivial (Redis `INCR` plus `EXPIRE`) | Up to 2x the limit across a window boundary |
| Sliding window log | Timestamp per request | Exact | Memory grows with the limit |
| Sliding window counter | Weighted current plus previous window | Close to exact, O(1) | Approximate |

A single-process token bucket with an injectable clock (tested: burst, rejection with a retry hint, refill, cap):

```python
import math
import time


class TokenBucket:
    """Capacity = burst size, rate = sustained requests per second. Single process only."""

    def __init__(self, capacity: float, rate: float, clock=time.monotonic) -> None:
        self.capacity, self.rate, self.clock = capacity, rate, clock
        self.tokens = capacity
        self.updated = clock()

    def _refill(self) -> None:
        now = self.clock()
        self.tokens = min(self.capacity, self.tokens + (now - self.updated) * self.rate)
        self.updated = now

    def try_acquire(self, cost: float = 1.0) -> tuple[bool, int]:
        """Returns (allowed, retry_after_seconds)."""
        self._refill()
        if self.tokens >= cost:
            self.tokens -= cost
            return True, 0
        return False, math.ceil((cost - self.tokens) / self.rate)
```

Production points:

- **Distributed**: state in Redis, updated atomically with a Lua script (or a single `INCR` for fixed windows), so N app instances share one limit.
  Decide fail-open versus fail-closed when Redis is down.
- **Keys**: per API key or user for fairness, per IP for anonymous traffic and login endpoints, per route for expensive endpoints; limit cost, not just count, for heavy queries.
- **Headers**: `429` plus `Retry-After`.
  The IETF `RateLimit` and `RateLimit-Policy` header fields are still an Internet-Draft (draft-ietf-httpapi-ratelimit-headers-11, in IESG evaluation as of September 2026, not an RFC); many APIs still send the non-standard `X-RateLimit-Limit`, `X-RateLimit-Remaining`, and `X-RateLimit-Reset`.
- **Where**: the API gateway or ingress (API Gateway usage plans, Kong, Envoy, NGINX `limit_req`) handles coarse limits cheaply; app-level limits handle business rules.
- Full design walkthrough: [Rate Limiter design](../../../04-System-Design/02-Case-Studies/02-Rate-Limiter/design.md).

---

## R12. How do you use HTTP caching for an API?

- **`Cache-Control`** tells clients and shared caches what they may store:
  `max-age=60` (fresh for 60 s), `s-maxage` (shared caches only), `no-cache` (may store, must revalidate every time), `no-store` (never store; use for sensitive data), `private` (browser only, never a CDN), `public`, `stale-while-revalidate=30`.
- **Validators** let a client revalidate cheaply: the server sends `ETag: "v7"` (or `Last-Modified`), the client sends `If-None-Match: "v7"`, and the server answers `304 Not Modified` with no body if nothing changed.
- **Strong vs weak ETags**: `"v7"` means byte-identical; `W/"v7"` means semantically equivalent.
  `If-Match` for writes requires strong comparison.
- **`Vary`** must list request headers that change the response (`Accept`, `Accept-Encoding`, `Authorization`), or a cache serves one user's response to another.
- **Authenticated responses**: a shared cache must not reuse a response to a request with `Authorization` unless the response explicitly allows it (`public`, `s-maxage`, or `must-revalidate`) per RFC 9111; still, mark per-user data `private` explicitly.
- What to cache: reference data (instruments, calendars), expensive aggregates; not per-user balances unless `private, no-cache` with an ETag.
- Server-side caching (Redis, `functools.lru_cache` for pure lookups) is a separate layer; name the invalidation strategy (TTL, write-through, event-driven) when you propose it.

The tested GET handler in R13 returns an `ETag` with `Cache-Control: private, no-cache` and answers a matching `If-None-Match` with an empty 304.

---

## R13. How do you prevent lost updates when two clients edit the same resource?

> "Optimistic concurrency with ETags.
> A GET returns the resource version as an ETag; the client sends it back in `If-Match` on the PUT or PATCH; the server applies the write only if the version still matches, atomically, and otherwise returns 412 Precondition Failed so the client re-reads and retries.
> If the client omits `If-Match` on a resource that requires it, the answer is 428 Precondition Required."

```python
from http import HTTPStatus
from typing import Annotated

from fastapi import FastAPI, Header, HTTPException, Request, Response
from fastapi.responses import JSONResponse
from pydantic import BaseModel
from starlette.exceptions import HTTPException as StarletteHTTPException

app = FastAPI()

# In-memory store: account id -> (version, data). A real service keeps `version` in a column.
ACCOUNTS: dict[int, tuple[int, dict]] = {1: (1, {"id": 1, "name": "Rates Desk", "limit": 100})}


class AccountUpdate(BaseModel):
    name: str
    limit: int


@app.exception_handler(StarletteHTTPException)  # also catches router 404/405
async def problem_details(request: Request, exc: StarletteHTTPException) -> JSONResponse:
    """RFC 9457 problem details for every HTTPException."""
    return JSONResponse(
        {
            "type": "about:blank",  # with about:blank, title should be the status phrase
            "title": HTTPStatus(exc.status_code).phrase,
            "status": exc.status_code,
            "detail": exc.detail,
            "instance": request.url.path,
        },
        status_code=exc.status_code,
        media_type="application/problem+json",
        headers=exc.headers,
    )


def etag_for(version: int) -> str:
    return f'"v{version}"'  # strong validator: quotes are part of the ETag syntax


@app.get("/accounts/{account_id}")
def get_account(
    account_id: int,
    response: Response,
    if_none_match: Annotated[str | None, Header()] = None,
):
    if account_id not in ACCOUNTS:
        raise HTTPException(404, "Account not found")
    version, data = ACCOUNTS[account_id]
    etag = etag_for(version)
    if if_none_match == etag:
        return Response(status_code=304, headers={"ETag": etag})
    response.headers["ETag"] = etag
    response.headers["Cache-Control"] = "private, no-cache"  # may store, must revalidate
    return data


@app.put("/accounts/{account_id}")
def put_account(
    account_id: int,
    body: AccountUpdate,
    response: Response,
    if_match: Annotated[str | None, Header()] = None,
):
    if account_id not in ACCOUNTS:
        raise HTTPException(404, "Account not found")
    if if_match is None:
        raise HTTPException(428, "If-Match header is required")  # RFC 6585
    version, data = ACCOUNTS[account_id]
    if if_match != etag_for(version):
        raise HTTPException(412, "Resource was modified; re-fetch and retry")
    # In SQL: UPDATE accounts SET ..., version = version + 1 WHERE id = :id AND version = :v
    new = {"id": account_id, **body.model_dump()}
    ACCOUNTS[account_id] = (version + 1, new)
    response.headers["ETag"] = etag_for(version + 1)
    return new
```

The tests prove: GET returns `ETag: "v1"` and a matching `If-None-Match` gets an empty 304; a PUT without `If-Match` gets 428 as problem JSON; a PUT with the current ETag succeeds and returns `"v2"`; a second PUT with the stale ETag gets 412 and the first writer's data survives; an unknown route (404) and a wrong method (405, with its `Allow` header preserved) return problem JSON too.

- The check and the write must be one atomic step in the database: `UPDATE ... SET version = version + 1 WHERE id = :id AND version = :expected`, then treat `rowcount == 0` as 412.
  Checking in Python and then writing is a race.
- SQLAlchemy supports this natively with `version_id_col` in the mapper, raising `StaleDataError` on a mismatch; see [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md).
- Pessimistic locking (`SELECT ... FOR UPDATE`) suits short, high-contention server-side transactions, not a human editing a form for minutes.

---

## R14. How do you design a long-running operation?

> "Do not hold the HTTP request open.
> Accept the job with 202 Accepted and a `Location` header pointing to a status resource; the client polls it (honoring `Retry-After`) or registers a webhook; when the job finishes, the status resource links to the result."

```text
POST /v1/reports               -> 202 Accepted
                                  Location: /v1/operations/op_123
                                  Retry-After: 5
GET  /v1/operations/op_123     -> 200 {"status": "running", "progress": 0.4}
GET  /v1/operations/op_123     -> 200 {"status": "succeeded", "result": "/v1/reports/rpt_9"}
                                  (or {"status": "failed", "error": {problem details}})
POST /v1/operations/op_123:cancel -> 202
```

- The job runs on a worker (Celery, RQ, a Kafka or RabbitMQ consumer, AWS Batch), never in the web process; FastAPI `BackgroundTasks` is fine for small fire-and-forget work but dies with the process.
- Make submission idempotent (R8), because clients will retry the POST.
- Persist the operation state in a database, not in worker memory, so any API instance can answer the status poll.
- For push notification, send a signed webhook (R17) and keep the status endpoint as the source of truth.
- Expire operation records after a retention window.

---

## R15. How do you evolve an API without breaking clients?

> "Additive changes only within a version: new endpoints, new optional request fields, new response fields.
> Never remove or rename a field, change its type or meaning, make an optional field required, or tighten validation.
> Clients must be tolerant readers that ignore unknown fields.
> When a breaking change is unavoidable, ship a new version side by side, announce deprecation with the `Deprecation` and `Sunset` headers, watch per-client usage of the old version, and only then remove it."

| Change | Safe? | Why |
| --- | --- | --- |
| Add a response field | Yes | Tolerant readers ignore it |
| Add an optional request field with a default | Yes | Old requests behave as before |
| Add an endpoint | Yes | Nobody calls it yet |
| Add an enum value | Risky | Strict clients (generated code, `match` without default) crash; document "expect unknown values" up front |
| Rename or remove a field | No | Clients read it |
| Repurpose a field (same name, new meaning) | Never | Silent data corruption, the worst kind of break |
| Change a type (`"42"` to `42`) or a format | No | Parsers fail |
| Make an optional field required, or tighten validation | No | Previously valid requests now fail |
| Change a default or an error code | No | Behavior changes silently |

Deprecation headers, both standardized:

```http
Deprecation: @1803859199
Sunset: Tue, 31 Aug 2027 23:59:59 GMT
Link: <https://api.example.com/docs/migrate-v2>; rel="deprecation"; type="text/html"
```

- `Deprecation` is RFC 9745 (March 2025); its value is a Structured Fields date (`@` plus a Unix timestamp) saying when the resource was or will be deprecated (here 2027-02-28 23:59:59 UTC).
- `Sunset` is RFC 8594; its value is an HTTP-date after which the resource may stop responding, and it should not be earlier than the deprecation date.
- Guard it in CI: diff the generated OpenAPI document against the last release with a breaking-change checker (for example `oasdiff`), and run consumer contract tests.

---

## R16. How do you design filtering, sorting, and field selection?

```text
GET /v1/trades?status=open&desk=rates&traded_after=2026-09-01T00:00:00Z
              &sort=-traded_at,id&fields=id,symbol,qty&limit=50&cursor=...
```

- Filters are query parameters named after fields, with explicit range suffixes (`_after`, `_before`, `_min`, `_max`) rather than a mini query language, unless you truly need one.
- **Sort** with a comma list and `-` for descending; **allowlist** the sortable fields, because `ORDER BY` columns cannot be bound as SQL parameters, and an unvalidated sort field is a SQL injection.
  Every allowed sort needs an index and a unique tie-breaker for keyset pagination (R6).
- **Field selection** (`fields=`) cuts payload size; implement it after fetching or with a column list built from an allowlist, never by string-formatting user input into SQL.
- In FastAPI, declare filters as a Pydantic model with `Annotated[Filters, Query()]` (query parameter models, tested here with a `Literal` status and a bounded `limit`) so they are validated and documented in OpenAPI; in Flask, parse `request.args` through a schema (Pydantic, marshmallow).
- Cap everything: page size, number of filter values, date range width, so one request cannot scan the whole table.
- If clients need arbitrary joins and projections across many resources, that is the signal to consider GraphQL (R19).

---

## R17. How do you integrate webhooks safely, sending and receiving?

> "A webhook is an unauthenticated POST from the internet until you verify it.
> The sender signs the raw body plus a timestamp with a shared secret using HMAC-SHA256; the receiver recomputes the signature over the exact raw bytes, compares in constant time, rejects stale timestamps to stop replays, deduplicates by event id because delivery is at least once, returns 2xx quickly, and processes asynchronously."

```python
import hashlib
import hmac
import time


def sign(secret: bytes, timestamp: int, body: bytes) -> str:
    msg = str(timestamp).encode() + b"." + body  # sign the exact raw bytes, never re-serialized JSON
    return hmac.new(secret, msg, hashlib.sha256).hexdigest()


def verify(
    secret: bytes,
    body: bytes,
    timestamp_header: str,
    signature_header: str,
    tolerance_s: int = 300,
    now: float | None = None,
) -> bool:
    try:
        ts = int(timestamp_header)
    except ValueError:
        return False
    now = time.time() if now is None else now
    if abs(now - ts) > tolerance_s:  # replay window
        return False
    expected = sign(secret, ts, body)
    return hmac.compare_digest(expected, signature_header)  # constant time
```

Tested: a valid signature passes, a modified body fails, a timestamp outside the 300-second window fails, a wrong secret fails, and a malformed timestamp fails.

Receiving checklist:

- Verify over the **raw bytes**: `await request.body()` in FastAPI, `request.get_data()` in Flask.
  Parsing and re-serializing JSON changes whitespace and key order and breaks the signature.
- `hmac.compare_digest`, never `==`, to avoid timing leaks.
- **Dedupe** with a unique constraint on the provider's event id; a duplicate is acknowledged with 2xx so the sender stops retrying.
- **Acknowledge fast** (enqueue to Kafka, RabbitMQ, or SQS, then return 200); slow handlers cause sender timeouts and redelivery storms.
- **Ordering** is not guaranteed: use event timestamps or versions, or re-fetch the current state from the provider's API (the "thin event, fetch-back" pattern), which also limits damage from a forged event.
- Support **secret rotation** by accepting signatures from the current and previous secret during a transition window.

Sending checklist: sign every delivery, include an event id and timestamp, retry with exponential backoff over hours or days, disable endpoints that fail persistently, offer a replay API, and never follow redirects or call private IP ranges (SSRF, see [Auth and Security](07-Auth-and-Security.md)).

Broader third-party integration questions (rate limits on their side, pagination of their API, sandbox versus production credentials, schema drift, circuit breakers) reuse R10 and R11.
[fill in: a third-party or cross-team integration you built, what failed in production, and what you changed]

---

## R18. REST versus GraphQL versus gRPC: when do you pick each?

| | REST (JSON over HTTP) | GraphQL | gRPC |
| --- | --- | --- | --- |
| Contract | OpenAPI (optional) | Schema (SDL), required | Protobuf `.proto`, required |
| Transport | HTTP/1.1 or HTTP/2 | Usually HTTP POST to one endpoint | HTTP/2 |
| Payload | JSON text | JSON text | Binary protobuf, compact and fast |
| Fetching | Fixed resource shapes; over- or under-fetching | Client picks fields; one round trip for nested data | Fixed messages per RPC |
| HTTP caching | Native (GET, ETag, CDN) | Hard (POST, one URL); needs persisted queries | None at the HTTP layer |
| Streaming | SSE, chunked, WebSockets separately | Subscriptions | Native client, server, and bidirectional streams |
| Browser support | Native | Native | Needs gRPC-Web and a proxy |
| Typical fit | Public and partner APIs, CRUD services | Aggregating many backends for varied UI clients | Internal service-to-service, low latency, polyglot |
| Main risks | Chatty clients, versioning discipline | N+1 resolvers, query cost attacks, authz per field | Tooling for debugging, load balancing long-lived HTTP/2 connections |

> "Default to REST for external and partner APIs, gRPC for high-throughput internal calls where both sides share the proto, and GraphQL when many clients need different shapes of the same graph and a team can own the schema and its performance."

In Python, gRPC is `grpcio` plus generated stubs; see [Microservices and gRPC in Python](../Python_Zero_to_Godhood/Chapter_63_Microservices_and_gRPC_in_Python.md).

---

## R19. OpenAPI-first or code-first? How do you document an API?

- **Code-first** (FastAPI's model): the OpenAPI document is generated from type hints and Pydantic models, so it cannot drift from the code; Swagger UI at `/docs` and ReDoc at `/redoc` come free.
  Flask needs an extension for this (apispec with flask-smorest, or APIFlask).
- **Design-first**: write `openapi.yaml`, review it with consumers before any code, lint it (Spectral), mock it (Prism), and generate clients and server stubs (openapi-generator).
  Best when several teams or external partners depend on the contract.
- A practical middle: design-first review of the contract for new public APIs, then implement code-first and diff the generated document against the approved one in CI.
- Good documentation beyond the schema: authentication how-to, error catalog with `type` URIs (R9), pagination and rate-limit rules, idempotency guidance, examples for every endpoint (`json_schema_extra` or `openapi_examples` in FastAPI), a changelog, and deprecation policy.
- FastAPI details (response models, `responses=` for documenting error codes, tags, `openapi_url=None` to hide docs in production) are in [FastAPI](04-FastAPI.md).

---

## R20. How do you design bulk operations?

- Endpoint: `POST /v1/trades:batchCreate` (or `/v1/trades/bulk`) with `{"items": [...]}`; cap the item count and body size (413 Content Too Large beyond it).
- Decide and document **atomic versus partial**:
  atomic means one transaction, all or nothing, simpler for clients;
  partial means per-item results, better for large imports.
- Partial success response: 200 with a per-item array (`{"index": 3, "status": 409, "error": {...}}`) or 207 Multi-Status (from WebDAV, RFC 4918); many teams avoid 207 because generic clients treat it as plain success.
- Per-item idempotency: accept a client id per item so retries of a half-applied batch do not duplicate the applied half.
- Large batches go asynchronous: 202 plus an operation resource (R14), or a file upload to object storage followed by a job.
- Database side: `executemany` or SQLAlchemy bulk inserts, chunks of a few thousand rows, and `COPY` in PostgreSQL for very large loads; see [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md).

---

## R21. What is HATEOAS, and why do most APIs skip it?

- **Hypermedia as the Engine of Application State**: responses include links to the actions and related resources available now, so clients follow links instead of constructing URLs.

```json
{"id": "ord_42", "status": "open",
 "links": {"self": "/v1/orders/ord_42", "cancel": "/v1/orders/ord_42:cancel", "fills": "/v1/fills?order_id=ord_42"}}
```

- Why most APIs skip it: clients are written against documentation and hard-code URLs anyway, there is no dominant hypermedia format or generic client, and links add payload and design effort.
- Where it still pays off, even at maturity level 2: `Location` on 201 and 202, `next` links or `Link` headers for pagination, and state-dependent action links (`cancel` present only when the order is cancellable) for UI-driven clients.
- A good answer admits the trade-off rather than claiming full REST compliance.

---

## R22. Rapid-fire trick questions

**Is PUT idempotent even if it creates the resource?**
Yes; the second identical PUT leaves the same state (201 the first time, 200 or 204 after).

**Should `POST /orders` return the created object?**
Return 201, a `Location` header, and usually the representation, which saves the client a GET.

**Is a 404 on the second DELETE a violation of idempotency?**
No; idempotency is about server state, not identical responses.

**Return 403 or 404 when a user asks for someone else's order?**
Usually 404, so the API does not confirm the resource exists (OWASP API1, BOLA); either way, check ownership on every object access.

**Why not use verbs in URLs?**
The HTTP method is the verb; `/getOrders` duplicates it and breaks caching and tooling conventions.
Non-CRUD actions are the exception (R5).

**Is `GET /delete-user?id=5` just ugly?**
It is dangerous: GET must be safe, and crawlers, link prefetchers, and caches will trigger it.

**What does 202 promise?**
Only that the request was accepted for processing, not that it will succeed.

**Where do you put the API key?**
In a header (`Authorization` or `X-API-Key`), never in the query string, which ends up in access logs, browser history, and `Referer` headers.

**Why does `requests.get(url)` without `timeout=` fail code review?**
It can block a worker forever; one slow dependency then exhausts the worker pool (R10).

---

## Go deeper

Vault notes:

- [Auth and Security](07-Auth-and-Security.md), [FastAPI](04-FastAPI.md), [Flask](03-Flask.md), [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md), [Microservices and Messaging](08-Microservices-and-Messaging.md), [System Design](12-System-Design.md).
- [Rate Limiter design](../../../04-System-Design/02-Case-Studies/02-Rate-Limiter/design.md) - token bucket, windows, distributed counters.
- [System design basics](../../../04-System-Design/00-Concepts/system_design_basics.md).
- [FastAPI and Pydantic: Type-Safe Web Development](../Python_Zero_to_Godhood/Chapter_67_FastAPI_and_Pydantic_Type-Safe_Web_Development.md).
- [Microservices and gRPC in Python](../Python_Zero_to_Godhood/Chapter_63_Microservices_and_gRPC_in_Python.md).
- [urllib and http from the standard library](../Python_Zero_to_Godhood/Chapter_43_High-Level_URL_and_HTTP_Handling_urllib_http.md).
- [OWASP Top 10 checklist](../../../11-Security-And-Cryptography/01-Common-Vulnerabilities/owasp_top_10.md).

Official references:

- [RFC 9110 HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110) (methods, status codes, conditional requests) and [RFC 9111 HTTP Caching](https://www.rfc-editor.org/rfc/rfc9111).
- [RFC 9457 Problem Details](https://www.rfc-editor.org/rfc/rfc9457), [RFC 5789 PATCH](https://www.rfc-editor.org/rfc/rfc5789), [RFC 7396 JSON Merge Patch](https://www.rfc-editor.org/rfc/rfc7396), [RFC 6902 JSON Patch](https://www.rfc-editor.org/rfc/rfc6902).
- [RFC 9745 Deprecation header](https://www.rfc-editor.org/rfc/rfc9745), [RFC 8594 Sunset header](https://www.rfc-editor.org/rfc/rfc8594), [RFC 8288 Web Linking](https://www.rfc-editor.org/rfc/rfc8288).
- [RateLimit header fields draft](https://datatracker.ietf.org/doc/draft-ietf-httpapi-ratelimit-headers/) and [Idempotency-Key header draft (expired)](https://datatracker.ietf.org/doc/draft-ietf-httpapi-idempotency-key-header/).
- [OpenAPI Specification](https://spec.openapis.org/oas/latest.html).
- [AWS Builders' Library: Timeouts, retries, and backoff with jitter](https://aws.amazon.com/builders-library/timeouts-retries-and-backoff-with-jitter/).
- [SQLite row values](https://www.sqlite.org/rowvalue.html) (the keyset comparison used in R6).
