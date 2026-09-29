---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://github.com/confluentinc/librdkafka/blob/master/CONFIGURATION.md, https://kafka.apache.org/documentation/, https://kafka.apache.org/blog/2026/02/17/apache-kafka-4.2.0-release-announcement/, https://www.rabbitmq.com/docs/quorum-queues, https://www.rabbitmq.com/docs/consumers, https://docs.celeryq.dev/en/stable/userguide/configuration.html, https://docs.aws.amazon.com/AWSSimpleQueueService/latest/SQSDeveloperGuide/quotas-messages.html, https://aws.amazon.com/builders-library/timeouts-retries-and-backoff-with-jitter/, https://debezium.io/documentation/reference/stable/transformations/outbox-event-router.html]
---

# Microservices and messaging

Microservice architecture, resilience between services, distributed transactions, and the Kafka and RabbitMQ questions a Python backend role asks.
M1-M8 are near-certain for a "REST APIs and microservices" job description; the outbox (M7) and idempotent consumer (M8) answers are where senior candidates separate themselves.
If they use Kafka, expect M4, M9, and M12 in depth.

Every code sample here was run on 2026-09-29 under Python 3.14.7 with FastAPI 0.142.0 and confluent-kafka 2.15.1 (librdkafka 2.15.1, using its in-process mock cluster, so no real broker): 24 pytest tests passed.

---

## M1. Monolith or microservices: when would you choose each? (must know)

> "Microservices are an organizational scaling tool first and a technical one second.
> They buy independent deployment, independent scaling, and fault isolation, and they cost you network calls, distributed transactions, eventual consistency, and a lot of platform work.
> My default for a new product or a small team is a modular monolith with strict module boundaries; I split out a service when a boundary is stable and there is a concrete reason: a different scaling profile, a different release cadence, a separate team, or a compliance boundary."

| | Monolith | Modular monolith | Microservices |
| --- | --- | --- | --- |
| Deploy unit | One | One | One per service |
| Transactions | Local ACID | Local ACID | Sagas, outbox, eventual consistency |
| Calls between parts | Function call | Function call through a module's public API | Network call: latency, timeouts, partial failure |
| Scaling | Whole app | Whole app | Per service |
| Team autonomy | Low at scale | Medium | High |
| Operational cost | Low | Low | High: CI/CD per service, tracing, service discovery, on-call |
| Typical failure | Big ball of mud | Boundaries erode without enforcement | Distributed monolith |

- **Modular monolith:** one deployable, but packages own their tables and expose a narrow interface; enforce it with import rules (for example `import-linter` contracts in CI).
  It keeps the option to split later at low cost.
- **Distributed monolith** (the classic trap): services that must be deployed together, share a database, or call each other synchronously in long chains.
  You pay all the costs of microservices and get none of the benefits.
- **Signals it is time to split:** one module needs 10x the instances of the rest, a team is blocked by another team's release train, a component has a different availability or compliance requirement (a payments boundary in a bank), or build and test times have become the bottleneck.

**Follow-ups they ask:**

- "How big should a microservice be?"
  Size it by a business capability and a team (one team can own several services; a service should not need two teams), not by lines of code.
- "What would you do first when migrating?"
  See the strangler-fig walkthrough in [System Design X5](12-System-Design.md).
- Experience hook: **[fill in: a system you worked on that was split, or deliberately kept as a monolith, and why]**.

---

## M2. Synchronous (REST, gRPC) or asynchronous (events, queues) communication between services? (must know)

- **Synchronous** (REST over HTTP/JSON, gRPC over HTTP/2 with Protobuf) when the caller needs the answer now to continue: a query, a validation, an authorization check.
- **Asynchronous** (a broker: Kafka, RabbitMQ, SQS) when the caller only needs to know the work will happen: notifications, projections, downstream processing, anything that can lag.
- Sync couples availability: if B is down, A's request fails; a chain of five services at 99.9% each is about 99.5% end to end.
- Async decouples availability and absorbs bursts, at the cost of eventual consistency, harder debugging, and the need for idempotent consumers.

| | REST/JSON | gRPC | Events via broker |
| --- | --- | --- | --- |
| Coupling | Temporal and schema | Temporal and schema (strict contract) | Schema only |
| Latency | Low | Lowest (binary, multiplexed HTTP/2) | Higher, variable |
| Contract | OpenAPI | `.proto`, generated stubs | Avro/Protobuf/JSON Schema plus registry |
| Browser clients | Native | Needs gRPC-Web or a gateway | No |
| Streaming | SSE, WebSockets | Native bidirectional | Native (it is a stream) |
| Good for | Public and partner APIs, CRUD | Internal high-volume service-to-service | Fan-out, workflows, decoupling, audit trails |

- **Commands versus events:** a command ("ChargeCard") is addressed to one service and may be rejected; an event ("OrderPlaced") states a fact in the past tense and any number of consumers may react.
- **Request-reply over a queue** exists (RabbitMQ `reply_to` plus `correlation_id`), but it keeps the temporal coupling of sync calls; usually a smell.
- **Python detail:** in FastAPI, make outbound HTTP calls with one shared `httpx.AsyncClient` created in the `lifespan` (connection pooling), with explicit timeouts; never call `requests` from an `async def` endpoint because it blocks the event loop.
  See [FastAPI](04-FastAPI.md).

---

## M3. How do you make service-to-service calls resilient? (must know)

> "Every remote call gets a timeout, and the timeout is budgeted from the caller's deadline.
> Retries only on transient errors, only for idempotent operations, with capped exponential backoff and jitter, and a retry budget so we never multiply load during an outage.
> A circuit breaker fails fast when a dependency is clearly down, bulkheads keep one slow dependency from exhausting all workers, and there is a fallback or a clean error for when everything else fails."

| Pattern | Problem it solves | Python-side detail |
| --- | --- | --- |
| Timeout | A hung dependency ties up a worker forever | `httpx.Timeout(connect=1, read=2, write=2, pool=1)`; `requests` has **no default timeout** |
| Deadline propagation | Downstream keeps working after the caller gave up | Pass the remaining budget in a header; gRPC has deadlines built in |
| Retry with backoff and jitter | Transient blips (503, reset, timeout) | Only idempotent calls, or with an idempotency key; `tenacity` or the helper below |
| Retry budget | Retry storms: 3 layers x 3 retries = 27x load | Cap retries to a percentage of traffic; retry at one layer only |
| Circuit breaker | Hammering a dead dependency, slow failures | Fail fast, probe after a cool-down; see M10 |
| Bulkhead | One slow dependency exhausts the whole worker pool | Separate connection pools or `asyncio.Semaphore` per dependency |
| Fallback | Degrade instead of fail | Cached value, default, or partial response; never for money movement |
| Rate limiting and load shedding | Protect yourself from callers | 429 with `Retry-After`; see [System Design X2](12-System-Design.md) |

Retry with capped exponential backoff and full jitter:

```python
import random
import time
from collections.abc import Callable
from typing import TypeVar

T = TypeVar("T")


class TransientError(Exception):
    """Timeouts, connection resets, 429, 502/503/504: safe to retry if the call is idempotent."""


def retry(
    fn: Callable[[], T],
    *,
    attempts: int = 4,
    base: float = 0.1,
    cap: float = 2.0,
    retryable: tuple[type[BaseException], ...] = (TransientError,),
    sleep: Callable[[float], None] = time.sleep,
    rng: random.Random | None = None,
) -> T:
    """Call fn up to `attempts` times; only retryable errors are retried; the last error propagates."""
    rng = rng or random.Random()
    for attempt in range(attempts):
        try:
            return fn()
        except retryable:
            if attempt == attempts - 1:
                raise
            sleep(rng.uniform(0, min(cap, base * 2**attempt)))
    raise AssertionError("unreachable")
```

- **Why jitter:** without it, every client that failed at the same moment retries at the same moment, producing synchronized waves; full jitter spreads them out.
- **What not to retry:** 400, 401, 403, 404, 409, 422; a non-idempotent POST without an idempotency key; anything after the caller's deadline.
- **Timeouts must shrink going down the stack:** if the edge gives up at 3 s, the service behind it cannot wait 5 s on its own dependency.

**Follow-ups they ask:**

- "Retries caused an outage; what happened?"
  Retry amplification: each layer retried, load multiplied during a brownout, and the dependency never recovered.
  Fix with a retry budget, retries at a single layer, and a breaker.
- "Is a timeout an error or not?"
  For a write, a timeout means **unknown outcome**; retry with the same idempotency key or reconcile, never assume it failed.

---

## M4. Explain Kafka's core concepts. (must know)

- A **topic** is a named log split into **partitions**; each partition is an append-only, ordered sequence of records identified by **offsets**.
- **Ordering is guaranteed only within a partition.**
  The producer picks the partition by hashing the record **key**, so all events for one key (an account, an order) stay in order.
- A **consumer group** shares the partitions of a topic: each partition is assigned to exactly one consumer in the group, so parallelism is capped at the partition count.
  Different groups read the same topic independently (fan-out for free).
- Consumers **commit offsets** to the `__consumer_offsets` topic; after a restart or rebalance, a consumer resumes from the committed offset.
- **Retention** is time- or size-based (`retention.ms`, `retention.bytes`), independent of consumption: messages are not deleted when read, so you can replay.
- **Log compaction** (`cleanup.policy=compact`) keeps at least the latest record per key; a record with a null value (tombstone) deletes the key; good for changelog and "current state" topics.
- **Replication:** each partition has a leader and followers; the **ISR** (in-sync replicas) are the followers caught up with the leader.
  With `replication.factor=3`, `min.insync.replicas=2`, and producer `acks=all`, a write is acknowledged only when at least two replicas have it, so one broker can fail without data loss or write unavailability.
- **KRaft:** Kafka 4.0 removed ZooKeeper; the cluster metadata is managed by a Raft quorum of controllers.
- **Share groups (queues for Kafka, KIP-932):** production-ready in Kafka 4.2 (February 2026); consumers in a share group consume records cooperatively with per-record acknowledgement instead of exclusive partition ownership.
  Check whether the target's cluster and client versions support them before bringing it up.

```text
topic: payments (3 partitions)          consumer group "ledger"      consumer group "audit"
  p0: [0][1][2][3][4] <- key acct-7      consumer A: p0, p1           consumer X: p0, p1, p2
  p1: [0][1][2]       <- key acct-2      consumer B: p2
  p2: [0][1][2][3]    <- key acct-9      (a 4th consumer would sit idle)
```

**Follow-ups they ask:**

- "How many partitions?"
  Enough for peak consumer parallelism with headroom (target throughput divided by per-consumer throughput), because adding partitions later **changes the key-to-partition mapping** and breaks per-key ordering for existing keys.
- "Hot partition?"
  One key dominates (a large client account).
  Options: a composite key if strict per-key ordering is not required, or a dedicated topic for the whale.
- Experience hook: **[fill in: a Kafka topic you owned or consumed at work, its key, partition count, and consumer-group layout]**.

---

## M5. Kafka, RabbitMQ, or SQS/SNS: how do you choose? (must know)

> "Kafka is a durable, replayable, partitioned log: I pick it for event streams with several independent consumers, high throughput, ordering per key, and replay.
> RabbitMQ is a smart broker with flexible routing and per-message acknowledgement: I pick it for task queues and routing-heavy workflows.
> SQS and SNS are the managed AWS answer when I want zero broker operations and the volume and ordering needs are modest."

| | Kafka | RabbitMQ | SQS (+ SNS for fan-out) |
| --- | --- | --- | --- |
| Model | Partitioned, replicated log; consumers pull and track offsets | Exchanges route to queues; broker pushes to consumers; per-message ack | Managed queue; consumers poll; visibility timeout |
| Retention and replay | Yes, by time or size; replay by resetting offsets | No: a message is gone once acked (RabbitMQ streams add log semantics) | No replay; retention up to 14 days |
| Ordering | Per partition (key) | Per queue, with a single consumer | Standard: best effort; FIFO: per message group ID |
| Fan-out | Multiple consumer groups | Fanout or topic exchange to many queues | SNS topic to many SQS queues |
| Routing | By topic and key only | Rich: direct, topic (wildcards), fanout, headers | SNS filter policies |
| Throughput | Very high (millions per second per cluster) | High (tens of thousands per second per queue, lower for quorum queues) | Standard queues scale nearly without limit; FIFO is capped |
| Delivery | At-least-once by default; transactions for Kafka-to-Kafka exactly-once | At-least-once with acks and publisher confirms | At-least-once (standard); FIFO deduplicates within a 5-minute window |
| Poison messages | DIY: retry topics and DLQ topic | DLX; quorum queues have a delivery limit (default 20 since 4.0) | Redrive policy with `maxReceiveCount` to a DLQ |
| Operations | Heavy unless managed (MSK, Confluent Cloud, Event Hubs Kafka endpoint) | Medium; managed options exist | None |
| Python clients | `confluent-kafka` (librdkafka), `aiokafka` | `pika`, `aio-pika`, `kombu` (Celery) | `boto3`, `aioboto3` |

- **Choose Kafka** for event sourcing, CDC pipelines, audit trails, market or trade event streams, analytics feeds, and anything where a new consumer must be able to read history.
- **Choose RabbitMQ** for background jobs, RPC-style work distribution, priority queues, complex routing, and per-message TTLs and delays.
- **Choose SQS/SNS** on AWS when the team does not want to run a broker and does not need replay.
- Azure equivalents: **Service Bus** (queues and topics, sessions for ordering, native DLQ) and **Event Hubs** (a partitioned log with a Kafka-compatible endpoint).
  GCP: **Pub/Sub**.
- SQS and SNS payloads can now be up to 1 MiB (raised from 256 KiB in 2025); for anything larger, store the object in S3 and send a reference (the claim-check pattern).

---

## M6. How do you handle a business transaction that spans multiple services? (must know)

> "I avoid two-phase commit across services and use a saga: a sequence of local transactions, each publishing an event or replying to an orchestrator, with a compensating action for every step that can be undone.
> Each step is idempotent, the saga state is persisted, and the hard part, making the local write and the message atomic, is handled with a transactional outbox."

- **Why not 2PC (XA):** the coordinator is a blocking single point of failure (participants hold locks while in doubt), throughput suffers, many brokers and NoSQL stores do not support XA, and it couples services' availability.
- **Saga:** each step commits locally; failure triggers **compensations** in reverse order.
  A compensation is a semantic undo (refund, release reservation, cancel), not a rollback: the intermediate state was visible.

| | Orchestration | Choreography |
| --- | --- | --- |
| Control | A central orchestrator sends commands and tracks state | Services react to each other's events |
| Visibility | The whole flow is in one place | Flow is implicit, spread across services |
| Coupling | Orchestrator knows every participant | Services know only events |
| Good for | Complex flows, many steps, finance workflows that need an audit trail | Short flows with 2-4 steps |
| Risk | Orchestrator becomes a god service | Cyclic dependencies, hard to reason about "where is order 42?" |
| Tools | Temporal, AWS Step Functions, Camunda, a state machine table | Plain Kafka or RabbitMQ events |

A minimal orchestrator, to show the compensation logic:

```python
from collections.abc import Callable
from dataclasses import dataclass


@dataclass
class Step:
    name: str
    action: Callable[[dict], None]       # local transaction in one service
    compensate: Callable[[dict], None]   # semantic undo (refund, release), must be idempotent


class SagaFailed(Exception):
    def __init__(self, failed_step: str, compensated: list[str]):
        super().__init__(f"step {failed_step!r} failed; compensated {compensated}")
        self.failed_step = failed_step
        self.compensated = compensated


def run_saga(steps: list[Step], ctx: dict) -> None:
    """Orchestrated saga: run steps in order; on failure compensate completed steps in reverse."""
    done: list[Step] = []
    for step in steps:
        try:
            step.action(ctx)
        except Exception as exc:
            compensated = []
            for prev in reversed(done):
                prev.compensate(ctx)   # a real orchestrator persists state and retries these
                compensated.append(prev.name)
            raise SagaFailed(step.name, compensated) from exc
        done.append(step)
```

**Pitfalls interviewers probe:**

- **Isolation is lost:** another request can see the intermediate state (inventory reserved, payment not taken).
  Countermeasures: semantic locks (a `PENDING` status), commutative updates, reread before acting.
- **Compensations can fail too:** they must be retried until they succeed, so they must be idempotent; alert a human when they cannot complete.
- **Order the steps** so the hardest-to-compensate step (sending money out, sending an email) is last; this is the "pivot transaction."
- In-memory state as above is for illustration; a real orchestrator persists each transition so a crash resumes the saga.

---

## M7. What is the dual-write problem, and how does the transactional outbox solve it? (must know)

> "A handler that writes to the database and then publishes to Kafka can crash between the two, so either the event is lost or, if you publish first, an event goes out for a write that rolled back.
> The outbox pattern writes the event into an outbox table in the same local transaction as the business change; a separate relay reads the outbox and publishes it, and marks it sent.
> That turns the dual write into one atomic write plus at-least-once delivery, so consumers must be idempotent."

```text
  API handler                           relay (poller or CDC)            Kafka
  BEGIN
    INSERT INTO orders ...
    INSERT INTO outbox (event) ...
  COMMIT  ---------------------------->  SELECT ... WHERE unpublished
                                         publish(key=aggregate_id) ----> orders.events
                                         UPDATE outbox SET published_at
```

Writer and polling relay (sqlite for the test; the SQL is the same in PostgreSQL apart from `AUTOINCREMENT` becoming `GENERATED ALWAYS AS IDENTITY` and `datetime('now')` becoming `now()`):

```python
import json
import sqlite3
import uuid
from collections.abc import Callable

SCHEMA = """
CREATE TABLE IF NOT EXISTS orders (
    id TEXT PRIMARY KEY,
    account_id TEXT NOT NULL,
    amount_cents INTEGER NOT NULL,
    status TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS outbox (
    seq          INTEGER PRIMARY KEY AUTOINCREMENT,  -- relay order
    event_id     TEXT NOT NULL UNIQUE,               -- consumers dedupe on this
    aggregate_id TEXT NOT NULL,                      -- becomes the Kafka key
    topic        TEXT NOT NULL,
    payload      TEXT NOT NULL,
    published_at TEXT                                -- NULL until relayed
);
"""

Publisher = Callable[[str, str, str, str], None]  # (topic, key, event_id, payload); raises on failure


def init_db(conn: sqlite3.Connection) -> None:
    conn.executescript(SCHEMA)


def create_order(conn: sqlite3.Connection, account_id: str, amount_cents: int) -> str:
    """Business write and event write in ONE local transaction: no dual write."""
    order_id = str(uuid.uuid4())
    event = {"type": "OrderCreated", "order_id": order_id,
             "account_id": account_id, "amount_cents": amount_cents}
    with conn:
        conn.execute("INSERT INTO orders VALUES (?, ?, ?, 'NEW')", (order_id, account_id, amount_cents))
        conn.execute(
            "INSERT INTO outbox (event_id, aggregate_id, topic, payload) VALUES (?, ?, ?, ?)",
            (str(uuid.uuid4()), order_id, "orders.events", json.dumps(event)),
        )
    return order_id


def relay_once(conn: sqlite3.Connection, publish: Publisher, batch: int = 100) -> int:
    """Publish unpublished rows in order; stop at the first failure to keep per-key order.

    At-least-once: a crash after publish() but before the UPDATE re-sends that row,
    so consumers must dedupe on event_id.
    """
    rows = conn.execute(
        "SELECT seq, event_id, aggregate_id, topic, payload FROM outbox "
        "WHERE published_at IS NULL ORDER BY seq LIMIT ?",
        (batch,),
    ).fetchall()
    sent = 0
    for seq, event_id, key, topic, payload in rows:
        try:
            publish(topic, key, event_id, payload)   # must wait for the broker ack (acks=all)
        except Exception:
            break                                    # retry on the next poll
        with conn:
            conn.execute("UPDATE outbox SET published_at = datetime('now') WHERE seq = ?", (seq,))
        sent += 1
    return sent
```

The tests prove: the order and its event commit together or not at all, the relay publishes in order, a broker failure stops the batch and the next poll resumes, and a crash between publish and mark produces a duplicate that the idempotent consumer in M8 absorbs.

- **Polling relay versus CDC:** polling is simple and portable but adds latency and load on the table.
  **Change data capture** (Debezium reading the PostgreSQL WAL through logical replication) tails the outbox table with no polling; Debezium's outbox event router turns each row into a Kafka record keyed by the aggregate ID.
- **Multiple relay instances:** use `SELECT ... FOR UPDATE SKIP LOCKED` in PostgreSQL, or a single leader, or partition the outbox by key; otherwise two relays publish the same row.
- **Housekeeping:** delete or archive published rows on a schedule; an ever-growing outbox slows the relay query.
- **Why not "publish then write"?** The event can go out for a write that later fails; consumers then act on something that never happened.
- **The inbox pattern** is the consumer-side mirror: store received message IDs (M8) in the same transaction as the effect.

---

## M8. How do you make a message consumer idempotent? (must know)

> "Every broker I would use in production delivers at least once, so duplicates will happen: redelivery after a crash before the ack, a rebalance, a producer retry, or the outbox relay re-sending.
> I give every message a stable unique ID and record it in a processed-messages table in the same transaction as the side effect, keyed on consumer name plus message ID, so a duplicate hits the unique key and is skipped.
> Where possible I also make the effect itself naturally idempotent: upserts, set-to-value instead of increment, conditional updates on a version."

```python
import json
import sqlite3

SCHEMA = """
CREATE TABLE IF NOT EXISTS processed_messages (
    consumer   TEXT NOT NULL,
    message_id TEXT NOT NULL,
    PRIMARY KEY (consumer, message_id)
);
CREATE TABLE IF NOT EXISTS balances (
    account_id TEXT PRIMARY KEY,
    amount_cents INTEGER NOT NULL
);
"""


def init_db(conn: sqlite3.Connection) -> None:
    conn.executescript(SCHEMA)


def handle(conn: sqlite3.Connection, consumer: str, raw: bytes) -> bool:
    """Apply a 'credit' event exactly once per (consumer, message_id).

    The dedupe insert and the business write share ONE local transaction, so
    either both happen or neither does. Returns False for a duplicate.
    """
    event = json.loads(raw)
    with conn:  # BEGIN ... COMMIT, or ROLLBACK on exception
        cur = conn.execute(
            "INSERT INTO processed_messages (consumer, message_id) VALUES (?, ?) "
            "ON CONFLICT DO NOTHING",
            (consumer, event["event_id"]),
        )
        if cur.rowcount == 0:
            return False  # already processed: skip the effect, ack again
        conn.execute(
            "INSERT INTO balances (account_id, amount_cents) VALUES (?, ?) "
            "ON CONFLICT(account_id) DO UPDATE SET amount_cents = amount_cents + excluded.amount_cents",
            (event["account_id"], event["amount_cents"]),
        )
    return True
```

- `INSERT ... ON CONFLICT DO NOTHING` is valid in both SQLite and PostgreSQL; checking `rowcount` distinguishes a duplicate from a real failure.
- **Pitfall caught while testing this:** an earlier draft caught every `IntegrityError` and returned "duplicate," which silently swallowed a genuine NOT NULL violation in the business write.
  Do not treat every constraint error as a duplicate; test the failure path (the test for it is in the scratch suite).
- **The dedupe must be in the same transaction as the effect.**
  "Check Redis, then write to PostgreSQL" has a race and a crash window; a Redis `SET NX` dedupe with a TTL is acceptable only when the effect is itself idempotent or a rare duplicate is harmless.
- **Where the ID comes from:** a producer-assigned event ID (UUID from the outbox), a business key (payment instruction ID), or topic plus partition plus offset (only stable if the message is never re-published).
- **Side effects outside your database** (email, a partner API): pass the message ID as that system's idempotency key, or record "sent" before sending and accept at-most-once for that effect.
- **Retention of the dedupe table:** keep IDs at least as long as the broker can redeliver (topic retention, plus replay windows); purge older rows by time.

---

## M9. At-most-once, at-least-once, exactly-once: what does Kafka's "exactly once" actually cover? (must know)

| Semantics | How you get it | Risk |
| --- | --- | --- |
| At-most-once | Commit (ack) before processing | A crash loses the message |
| At-least-once | Process, then commit | A crash re-delivers: duplicates |
| Effectively-once | At-least-once plus idempotent processing (M8) | Needs a dedupe key and discipline |

- **Idempotent producer** (`enable.idempotence=true`): the broker deduplicates producer retries using a producer ID and per-partition sequence numbers, so a retry after a lost ack does not write twice and does not reorder.
  The Java client defaults it on since Kafka 3.0; **librdkafka, and therefore `confluent-kafka` in Python, defaults it to `false`**, so set it explicitly.
- **Transactions** (`transactional.id`, `send_offsets_to_transaction`): a consume-transform-produce loop can atomically write output records and commit the input offsets; downstream consumers with `isolation.level=read_committed` see only committed data.
- **What it does not cover:** any side effect outside Kafka.
  Writing to PostgreSQL, calling a REST API, or sending an email inside the loop is not covered; if the process dies after the database commit but before the offset commit, the message is processed again.
- The honest answer: **exactly-once processing end to end is at-least-once delivery plus idempotent effects**, or storing the consumer offset in the same database transaction as the effect.

**Follow-up:** "How would you get exactly-once into PostgreSQL?"
Either the dedupe table (M8), or store the partition offset in a table updated in the same transaction as the effect and, on startup or assignment, seek to the stored offset instead of the Kafka-committed one.

---

## M10. Walk me through a circuit breaker implementation.

- **Closed:** calls flow; count consecutive failures of the kinds that indicate dependency health (timeouts, 5xx, connection errors), not client errors.
- **Open:** after the threshold, fail immediately without calling the dependency, for a cool-down period.
- **Half-open:** after the cool-down, let one trial call through; success closes the breaker, failure re-opens it and restarts the timer.

```python
import threading
import time
from collections.abc import Callable
from enum import Enum
from typing import TypeVar

T = TypeVar("T")


class State(Enum):
    CLOSED = "closed"        # calls flow; failures are counted
    OPEN = "open"            # calls fail fast until the cool-down expires
    HALF_OPEN = "half_open"  # one trial call decides: close or re-open


class CircuitOpenError(Exception):
    """Raised instead of calling the dependency while the breaker is open."""


class CircuitBreaker:
    def __init__(
        self,
        failure_threshold: int = 5,
        reset_timeout: float = 30.0,
        counted: tuple[type[BaseException], ...] = (Exception,),
        clock: Callable[[], float] = time.monotonic,
    ) -> None:
        self.failure_threshold = failure_threshold
        self.reset_timeout = reset_timeout
        self.counted = counted          # e.g. timeouts and 5xx, not 4xx validation errors
        self.clock = clock              # injectable so tests need no sleep
        self._lock = threading.Lock()
        self._state = State.CLOSED
        self._failures = 0
        self._opened_at = 0.0
        self._trial_in_flight = False

    @property
    def state(self) -> State:
        with self._lock:
            return self._current_state()

    def _current_state(self) -> State:
        # Caller holds the lock. OPEN turns into HALF_OPEN lazily once the cool-down has passed.
        if self._state is State.OPEN and self.clock() - self._opened_at >= self.reset_timeout:
            self._state = State.HALF_OPEN
            self._trial_in_flight = False
        return self._state

    def call(self, fn: Callable[..., T], *args, **kwargs) -> T:
        with self._lock:
            state = self._current_state()
            if state is State.OPEN:
                raise CircuitOpenError("circuit open: failing fast")
            if state is State.HALF_OPEN:
                if self._trial_in_flight:
                    raise CircuitOpenError("circuit half-open: trial call already in flight")
                self._trial_in_flight = True
        try:
            result = fn(*args, **kwargs)   # never hold the lock while calling out
        except self.counted:
            self._on_failure()
            raise
        except BaseException:
            # Not a dependency-health signal (for example a 4xx); release a half-open trial slot.
            with self._lock:
                self._trial_in_flight = False
            raise
        self._on_success()
        return result

    def _on_success(self) -> None:
        with self._lock:
            self._state = State.CLOSED
            self._failures = 0
            self._trial_in_flight = False

    def _on_failure(self) -> None:
        with self._lock:
            self._trial_in_flight = False
            if self._state is State.HALF_OPEN:
                self._trip()
                return
            self._failures += 1
            if self._failures >= self.failure_threshold:
                self._trip()

    def _trip(self) -> None:
        self._state = State.OPEN
        self._opened_at = self.clock()
        self._failures = 0
```

Talking points while you write it:

- The clock is injected, so the tests move time instead of sleeping.
- The lock protects state transitions but is **never held during the remote call**.
- Only the `counted` exceptions trip the breaker; a 400 from bad input says nothing about the dependency's health.
- Half-open admits exactly one trial; concurrent callers fail fast until it resolves.
- This version is per process: with 8 gunicorn workers and 20 pods there are 160 independent breakers, which is usually fine (each learns fast); a shared breaker in Redis adds a dependency to protect a dependency.
- Production variants use a failure **rate** over a sliding window with a minimum call count, rather than consecutive failures.
- For an `async` service, the same state machine works with `asyncio.Lock`; libraries: `pybreaker`, `aiobreaker`, or a service mesh (Envoy outlier detection) doing it outside the code.

---

## M11. How do you draw service boundaries?

- Start from the domain: **bounded contexts** from domain-driven design, where a term has one meaning and one owner ("Account" in the ledger is not "Account" in CRM).
- Align services with business capabilities and team ownership (Conway's law works whether you plan for it or not).
- **Database per service:** each service owns its tables; others read through its API or its events, never its tables.
  A shared database is the strongest coupling there is: a schema change becomes a cross-team release.
- **High cohesion, low coupling test:** a typical feature change should touch one service; if most changes touch three services, the boundary is wrong.
- **Data that others need:** publish events and let consumers build their own read models (with the staleness that implies), rather than synchronous lookups on every request.
- **Red flags:** chatty calls (N+1 across the network), services that must deploy together, a "common" or "utils" service everyone calls, distributed joins in application code.
- Split late: boundaries are cheap to move inside a modular monolith and expensive to move between services.

---

## M12. Which Kafka producer and consumer settings matter for durability, and when do you commit offsets?

Producer durability:

- `acks=all`: wait for all in-sync replicas.
- `enable.idempotence=true`: no duplicates or reordering from producer retries (off by default in librdkafka).
- Broker or topic: `replication.factor=3`, `min.insync.replicas=2`, `unclean.leader.election.enable=false`.
- Always handle the delivery callback, and `flush()` on shutdown; `produce()` in `confluent-kafka` is asynchronous and only enqueues.

Consumer commit strategy:

- `enable.auto.commit=true` (the default) commits in the background every 5 s whatever you have polled, which can commit a message you have not finished processing (loss on crash) or reprocess a window (duplicates).
- For anything that matters: `enable.auto.commit=false`, process, then commit; synchronous commit per message is simplest, commit per batch is faster.
- Keep processing time under `max.poll.interval.ms` (5 minutes by default) or the consumer is kicked out of the group and its partitions are reassigned, causing duplicate processing.

The worker below was tested against librdkafka's in-process mock cluster: same key lands on one partition in order, the poison message goes to the DLQ with its source offset in a header, the committed offset is last processed plus one, and a restarted consumer in the same group resumes where the first one committed.

```python
from collections.abc import Callable

from confluent_kafka import Consumer, KafkaException, Message, Producer

PRODUCER_CONF = {
    "acks": "all",                 # wait for all in-sync replicas (librdkafka default is already -1)
    "enable.idempotence": True,    # librdkafka default is False: set it; no duplicates or reordering on retry
    "linger.ms": 5,
    "compression.type": "zstd",
}

CONSUMER_CONF = {
    "enable.auto.commit": False,   # commit only after the side effect is durable
    "auto.offset.reset": "earliest",
    "isolation.level": "read_committed",
}


def make_producer(bootstrap: str) -> Producer:
    return Producer({"bootstrap.servers": bootstrap, **PRODUCER_CONF})


def make_consumer(bootstrap: str, group_id: str, **overrides) -> Consumer:
    return Consumer({"bootstrap.servers": bootstrap, "group.id": group_id, **CONSUMER_CONF, **overrides})


def process_batch(
    consumer: Consumer,
    dlq: Producer,
    dlq_topic: str,
    handler: Callable[[Message], None],
    max_messages: int,
    timeout: float = 1.0,
    max_attempts: int = 3,
) -> int:
    """Poll up to max_messages; handle each, send poison messages to the DLQ, commit after each.

    Returns the number of messages consumed (handled or dead-lettered).
    """
    consumed = 0
    while consumed < max_messages:
        msg = consumer.poll(timeout)
        if msg is None:
            break
        if msg.error():
            raise KafkaException(msg.error())
        for attempt in range(1, max_attempts + 1):
            try:
                handler(msg)          # must be idempotent: a crash before commit re-delivers
                break
            except Exception as exc:
                if attempt == max_attempts:
                    dlq.produce(
                        dlq_topic, key=msg.key(), value=msg.value(),
                        headers=[("error", repr(exc).encode()),
                                 ("source", f"{msg.topic()}/{msg.partition()}/{msg.offset()}".encode())],
                    )
                    if dlq.flush(10) != 0:        # do not commit past a message we failed to park
                        raise RuntimeError("DLQ publish not acknowledged")
        consumer.commit(message=msg, asynchronous=False)   # commits offset + 1 for that partition
        consumed += 1
    return consumed
```

- The three in-place attempts are for a cheap, deterministic handler; for transient downstream failures, use retry topics (M15) so one slow message does not stall the partition.
- **Testing trick worth mentioning:** librdkafka accepts `test.mock.num.brokers` in the client config and starts an in-process mock cluster, so Kafka client code can be tested in CI without Docker.
  It is a test-only property; never set it in production config.

---

## M13. What happens during a consumer-group rebalance, and how do you keep it cheap?

- A rebalance reassigns partitions when a consumer joins, leaves, crashes (misses heartbeats for `session.timeout.ms`), or exceeds `max.poll.interval.ms`.
- **Eager (classic) protocol:** every consumer revokes all partitions, then the group gets a new assignment: a stop-the-world pause.
- **Cooperative incremental** (`partition.assignment.strategy=cooperative-sticky`): only the moving partitions are revoked.
- **The new consumer group protocol (KIP-848)**, general availability in Kafka 4.0, moves assignment to the broker; in librdkafka it is selected with `group.protocol=consumer` (the default is still `classic`).
- **Static membership** (`group.instance.id`, for example the pod name from a StatefulSet): a restarting consumer that comes back within the session timeout keeps its partitions, so rolling deploys do not cause rebalances.
- **Commit on revoke:** in the `on_revoke` callback, commit what you have processed so the next owner does not redo it.
- **Symptoms of rebalance storms:** consumer lag sawtooth, duplicate processing, "member was kicked out" logs; usually slow handlers exceeding `max.poll.interval.ms`, or pods restarted by a failing liveness probe.

---

## M14. Explain RabbitMQ's model: exchanges, queues, bindings, acks, prefetch.

- Producers publish to an **exchange** with a **routing key**; **bindings** route messages to **queues**; consumers read from queues.

| Exchange type | Routes by | Example |
| --- | --- | --- |
| direct | Exact routing key | `payments.settle` to the settlement queue |
| topic | Pattern: `*` one word, `#` zero or more words | `trade.*.nyse` or `trade.#` |
| fanout | Ignores the key: every bound queue | Broadcast cache invalidation |
| headers | Header values (`x-match: all` or `any`) | Route by `region` and `format` headers |

- **Acknowledgements:** with manual acks, the broker redelivers an unacked message if the channel or connection closes; `basic_nack(requeue=False)` or reject sends it to the dead-letter exchange.
- **Consumer timeout:** a delivery unacked for longer than `consumer_timeout` (30 minutes by default) closes the channel with `PRECONDITION_FAILED` and requeues its messages; long tasks with late acks hit this.
- **Prefetch** (`basic_qos(prefetch_count=N)`): the maximum number of unacked messages per consumer.
  Unlimited prefetch lets one consumer hoard the queue; a small value (tens) spreads work and bounds memory; start low for slow tasks, higher for fast ones, and measure.
- **Publisher confirms:** the producer-side equivalent of acks; without them a publish can be lost silently.
- **Durability:** a durable queue plus persistent messages (`delivery_mode=2`) plus publisher confirms.
- **Quorum queues:** replicated with Raft, the recommended type for data safety; default cluster group size 3; a delivery limit (default 20 since RabbitMQ 4.0) after which the message is dropped or dead-lettered.
  Classic mirrored queues were removed in RabbitMQ 4.0.
- **Streams:** an append-only, replayable log type inside RabbitMQ, for Kafka-like consumption.
- **DLX:** set `x-dead-letter-exchange` on a queue; rejected, expired (TTL), or over-limit messages go there.
  Delayed retry: a retry queue with a message TTL that dead-letters back to the work exchange.

---

## M15. How do you deal with poison messages, dead-letter queues, and retries?

- A **poison message** fails every time (bad schema, a bug, missing reference data); if you keep retrying it in place, it blocks the partition (Kafka) or loops forever (a queue with requeue).
- **Classify errors:** transient (timeout, 503, lock contention) gets retried with backoff; permanent (validation, deserialization) goes straight to the DLQ.
- **Kafka non-blocking retries:** on transient failure publish to `topic.retry.1m`, then `topic.retry.10m`, then `topic.DLQ`; a consumer on each retry topic waits until the message's due time.
  Trade-off: **per-key ordering is lost** for the retried message; if ordering matters (account balance events), you must block the partition or park all later events for that key.
- **RabbitMQ:** reject without requeue into a DLX; TTL retry queues for delays; quorum queue delivery limits as a backstop.
- **SQS:** redrive policy `maxReceiveCount` moves the message to a DLQ; redrive back to the source queue after a fix.
- **DLQ hygiene:** record the error, the original topic, partition, and offset, and the attempt count in headers; alert on DLQ depth, not just its existence; have a replay tool and a runbook; set an owner.
- **Never** commit past a message you failed to park; that is silent data loss (the worker in M12 raises instead).

---

## M16. What are an API gateway, service discovery, and a service mesh?

- **API gateway:** the single entry point for external clients: TLS termination, authentication (validate the JWT once at the edge), rate limiting, routing, request size limits, sometimes response aggregation (backend-for-frontend).
  Examples: AWS API Gateway, Kong, Apigee, Azure API Management, Envoy-based gateways.
  Services still authorize: the gateway checks "who are you," the service checks "may you do this to this resource."
- **Service discovery:** how a caller finds healthy instances.
  In Kubernetes it is DNS plus a Service's virtual IP (`http://payments.prod.svc.cluster.local`); outside it, Consul or a cloud registry.
- **Service mesh** (Istio, Linkerd): a proxy next to every service (sidecar, or node-level in ambient mode) that provides mTLS between services, retries, timeouts, circuit breaking (outlier detection), traffic splitting for canaries, and uniform metrics, without code changes.
  Cost: operational complexity, extra latency per hop, and a debugging layer; worth it at dozens of services, rarely at five.
- **Pitfall:** retries configured in both the mesh and the code multiply (M3).

---

## M17. How do you use Celery for background jobs, and what goes wrong?

- **Broker** carries task messages (RabbitMQ or Redis; SQS is supported); the **result backend** stores return values and states (Redis, a database), and is optional: disable results you never read.
- **Default acknowledgement is early:** the message is acked right before the task runs, so a worker crash loses the task.
  `task_acks_late=True` acks after completion, so a crash redelivers it: at-least-once, so **tasks must be idempotent**.
- `task_reject_on_worker_lost=True` (with late acks) requeues when the worker process dies; beware tasks that crash the worker every time (a poison loop).
- `worker_prefetch_multiplier` defaults to 4 per process; set it to 1 for long tasks so one worker does not hoard them.
- **Visibility timeout** (Redis and SQS brokers): if a task is not acked within it (1 hour by default for Redis), the message is delivered again to another worker and runs twice; set it above your longest task's ETA or runtime.
  On RabbitMQ the equivalent trap is `consumer_timeout`.
- **Time limits:** `task_soft_time_limit` raises `SoftTimeLimitExceeded` so the task can clean up; `task_time_limit` kills the worker process.
- **Pass IDs, not objects:** send `order_id` and reload in the task, so the task sees current state and the message stays small; never pass ORM instances.
- **Enqueue after commit:** calling `.delay()` inside a database transaction that later rolls back runs a task for data that does not exist (the dual write again); enqueue after commit, or use an outbox.
- Alternatives worth naming: RQ (simple, Redis only), Dramatiq, arq (asyncio), and a plain Kafka consumer for event-driven work; FastAPI `BackgroundTasks` runs in the same process after the response and is lost on restart, so it is only for best-effort work.

---

## M18. Which Python clients do you use for Kafka and RabbitMQ, and what are the pitfalls?

| Client | Style | Notes |
| --- | --- | --- |
| `confluent-kafka` | Sync API over librdkafka (C) | Fastest, most complete (transactions, admin, schema registry serializers); `poll()` blocks; `confluent_kafka.aio` adds `AIOProducer` and `AIOConsumer` (present in 2.15.1; check the version you run) |
| `aiokafka` | Native asyncio | Fits FastAPI and asyncio services; pure Python protocol, slower at very high throughput |
| `pika` | Sync, blocking connection | Not thread-safe; one connection per thread; heartbeats stall if you block the I/O loop |
| `aio-pika` | asyncio wrapper over aiormq | The usual choice inside async services; robust (auto-reconnecting) connections |
| `kombu` | Messaging abstraction | What Celery uses underneath |

The classic pitfall: a blocking consumer inside an async web app.
Calling `consumer.poll()` (or `pika`'s blocking consume) in `async def` code freezes the event loop, so every HTTP request stalls.
Run it in a thread from the FastAPI lifespan and stop it on shutdown (tested: the health endpoint answers while the consumer blocks, and shutdown closes the consumer):

```python
import asyncio
import threading
from contextlib import asynccontextmanager

from fastapi import FastAPI


def consume_loop(consumer, handle, stop: threading.Event) -> None:
    """Blocking poll loop: runs in a worker thread, never on the event loop."""
    try:
        while not stop.is_set():
            msg = consumer.poll(0.5)          # blocking C call; fine in a thread
            if msg is None or msg.error():
                continue
            handle(msg)
            consumer.commit(message=msg, asynchronous=False)
    finally:
        consumer.close()                      # leave the group cleanly: fast rebalance


def create_app(consumer_factory, handle) -> FastAPI:
    @asynccontextmanager
    async def lifespan(app: FastAPI):
        stop = threading.Event()
        task = asyncio.create_task(asyncio.to_thread(consume_loop, consumer_factory(), handle, stop))
        app.state.consumer_task = task
        yield
        stop.set()                            # SIGTERM -> lifespan shutdown -> drain and close
        await task

    app = FastAPI(lifespan=lifespan)

    @app.get("/healthz")
    async def healthz():
        return {"ok": True}                   # stays responsive while the consumer blocks

    return app
```

Other pitfalls:

- **Committing before processing** (auto-commit, or `ack` first) converts at-least-once into at-most-once.
- **Consumer in the web process at all:** scaling the API also scales consumers and triggers rebalances on every deploy; a separate worker Deployment is usually cleaner.
  Also, with gunicorn `--workers 4`, each worker process starts its own consumer.
- **Forking after creating a client:** librdkafka and `pika` connections do not survive `fork()`; create them in each worker after fork (gunicorn `post_fork`, or the app lifespan), never at import time with `--preload`.
- **Not flushing the producer on shutdown:** `produce()` only enqueues; exit without `flush()` and queued messages are lost.
- **Unbounded in-memory buffering:** `BufferError` from `produce()` when the local queue is full; call `poll(0)` regularly to serve delivery callbacks and apply backpressure.

---

## M19. How do you observe a request that crosses five services?

- **Correlation and trace IDs:** accept or create one at the edge, propagate it on every hop (W3C `traceparent` header for HTTP; message headers for Kafka and RabbitMQ), and put it in every log line.
- **Distributed tracing with OpenTelemetry:** auto-instrumentation for FastAPI, Flask, httpx, requests, SQLAlchemy, psycopg, confluent-kafka, and Celery produces spans; export via OTLP to a collector, then Jaeger, Tempo, Datadog, X-Ray, or Azure Monitor.
  Sample (head or tail-based) to control cost; always keep error traces.
- **Metrics:** **RED** for request-driven services (Rate, Errors, Duration as p50/p95/p99 histograms per route) and **USE** for resources (Utilization, Saturation, Errors: CPU, pool usage, queue depth).
  For consumers, **consumer lag** per partition is the key metric; alert on lag growth and age of the oldest message, not on a single spike.
- **Structured logs:** JSON with `trace_id`, `span_id`, service, version, and business IDs (order ID), never secrets or PII; `structlog` or `python-json-logger`, with `contextvars` so async tasks keep their request context.
- **SLOs and alerts** on user-visible symptoms (error rate, latency) rather than causes; dashboards per service with a dependency view.
- Experience hook: **[fill in: how tracing or logging was done in a service you ran, and one incident it helped you diagnose]**.

---

## M20. How do you evolve event schemas without breaking consumers?

- Use a schema format with explicit evolution rules (**Avro**, **Protobuf**, or JSON Schema) and a **schema registry** (Confluent Schema Registry, AWS Glue Schema Registry, Apicurio) that rejects incompatible changes at publish time.
- **Backward compatible** (new consumers can read old data): adding an optional field with a default, removing a field is allowed; upgrade consumers first.
- **Forward compatible** (old consumers can read new data): adding a field is allowed (old readers ignore it); upgrade producers first.
- **Full** means both; choose the mode per topic, usually backward or full for long-retention topics.
- **Never:** rename a field, change a type incompatibly, reuse a Protobuf field number, or make an optional field required.
- **Breaking change:** publish a new versioned topic (`orders.v2`), dual-publish during migration, move consumers, retire the old one.
- **Envelope:** event ID, type, version, occurred-at time, producer, trace context, then the payload; consumers ignore unknown fields (Pydantic's default `extra="ignore"` does this).

---

## M21. What consistency guarantees do you give across services: eventual consistency, read-your-writes, CQRS?

- With database per service and events, other services' views are **eventually consistent**: they converge after the events are processed; make the lag measurable (consumer lag, event age) and bounded.
- **Read-your-writes:** after a user changes something, they must see their change.
  Options: read from the primary (not a replica) for that user for a short window, return the new state in the write response, or pass a version or LSN token that the read path waits for.
- **Monotonic reads:** pin a session to one replica so data does not appear to go back in time.
- **CQRS:** separate the write model (normalized, transactional) from read models (denormalized projections built from events), each scaled and stored independently (PostgreSQL for writes, Elasticsearch or a reporting table for reads).
  Worth it when read and write shapes differ a lot; overkill for CRUD.
- **Event sourcing** stores the events as the source of truth and derives state; powerful for audit (finance), costly in complexity; do not propose it casually.
- **Where strong consistency is non-negotiable** (balances, limits), keep the invariant inside one service and one database transaction; do not spread it across services.

---

## M22. What goes wrong with microservices in production?

The "have you actually run this" question; answer with failure modes and their fixes.

| Failure | Symptom | Fix |
| --- | --- | --- |
| Cascading failure | One slow dependency takes down everything upstream | Timeouts, circuit breakers, bulkheads (M3) |
| Retry storm | Load multiplies during a brownout | Retry budget, one retry layer, jitter |
| Distributed monolith | Lockstep deploys, shared DB | Real boundaries, contracts, database per service |
| Chatty calls | p99 dominated by many sequential hops | Batch endpoints, events and local read models, parallel calls with `asyncio.gather` |
| Lost or duplicated events | Reconciliation breaks | Outbox (M7), idempotent consumers (M8) |
| Consumer lag | Stale data, missed SLAs | Scale consumers up to the partition count, speed the handler, fix rebalance storms |
| Version skew | New producer, old consumer | Schema registry compatibility, expand-and-contract |
| Clock skew | Ordering by timestamps is wrong | Order by sequence or offset, not wall-clock time |
| Debugging blind | No idea where time went | Tracing and correlation IDs (M19) |
| Config drift | Works in staging, fails in prod | Config as code, same image across environments |

Experience hook: **[fill in: one real production incident involving a service dependency or a queue, what you found, and what you changed]**.

---

## Go deeper

Vault notes:

- [Microservices and gRPC in Python](../Python_Zero_to_Godhood/Chapter_63_Microservices_and_gRPC_in_Python.md)
- [Message Brokers: Kafka and RabbitMQ](../Python_Zero_to_Godhood/Chapter_76_Message_Brokers_Kafka_and_RabbitMQ.md)
- [Distributed Systems](../Python_Zero_to_Godhood/Chapter_77_DISTRIBUTED_SYSTEMS.md)
- [Distributed Databases and the CAP Theorem](../Python_Zero_to_Godhood/Chapter_74_Distributed_Databases_Python_and_the_CAP_Theorem.md)
- [Microservices vs Monolith](../../../08-Distinguished-Engineering/04-Architecture-Patterns/microservices_vs_monolith.md)
- [Saga Pattern](../../../08-Distinguished-Engineering/05-Distributed-Transactions/saga_pattern.md)
- [Circuit breaker (Python)](../../../08-Distinguished-Engineering/04-Architecture-Patterns/circuit_breaker.py)
- [System Design Basics](../../../04-System-Design/00-Concepts/system_design_basics.md)

Pack siblings: [FastAPI](04-FastAPI.md), [REST API Design](05-REST-API-Design.md), [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md), [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md), [System Design](12-System-Design.md).

Official docs:

- [Apache Kafka documentation](https://kafka.apache.org/documentation/) and the [librdkafka configuration reference](https://github.com/confluentinc/librdkafka/blob/master/CONFIGURATION.md)
- [confluent-kafka-python](https://docs.confluent.io/platform/current/clients/confluent-kafka-python/html/index.html)
- [RabbitMQ quorum queues](https://www.rabbitmq.com/docs/quorum-queues), [consumers and acknowledgements](https://www.rabbitmq.com/docs/consumers)
- [Celery configuration](https://docs.celeryq.dev/en/stable/userguide/configuration.html)
- [Debezium outbox event router](https://debezium.io/documentation/reference/stable/transformations/outbox-event-router.html)
- [Timeouts, retries, and backoff with jitter (AWS Builders' Library)](https://aws.amazon.com/builders-library/timeouts-retries-and-backoff-with-jitter/)
- [OpenTelemetry Python](https://opentelemetry.io/docs/languages/python/)
