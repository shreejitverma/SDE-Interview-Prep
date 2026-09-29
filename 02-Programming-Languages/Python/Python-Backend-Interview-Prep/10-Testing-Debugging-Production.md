---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://docs.pytest.org/en/stable/, https://docs.python.org/3/library/unittest.mock.html, https://fastapi.tiangolo.com/advanced/testing-dependencies/, https://fastapi.tiangolo.com/advanced/async-tests/, https://fastapi.tiangolo.com/advanced/testing-events/, https://flask.palletsprojects.com/en/stable/testing/, https://docs.sqlalchemy.org/en/21/orm/session_transaction.html, https://docs.sqlalchemy.org/en/21/dialects/sqlite.html, https://docs.python.org/3/library/asyncio-dev.html, https://docs.python.org/3/library/tracemalloc.html, https://docs.python.org/3/library/profile.html, https://docs.python.org/3/library/pdb.html, https://hypothesis.readthedocs.io/en/latest/, https://docs.locust.io/en/stable/, https://opentelemetry.io/docs/languages/python/, https://docs.gunicorn.org/en/stable/settings.html, https://www.uvicorn.org/settings/]
---

# Testing, debugging, and production troubleshooting

How to test Flask and FastAPI services, and how to debug them when production misbehaves.
T1-T4 (pyramid, pytest, mocking, FastAPI tests), T6 (database tests), T7 (logging with request IDs), and the scenarios T8, T9, and T10 are near-certain for a role that lists "debugging and production troubleshooting"; know them cold.
Every code sample here was run on 2026-09-29 under Python 3.14.7 with FastAPI 0.142.0, Starlette 1.7.0, Flask 3.1.3, SQLAlchemy 2.1.1, pytest 9.1.1, pytest-asyncio 1.4.0, hypothesis 6.168.3, and opentelemetry-sdk 1.45.0: 33 pytest tests passed.
The locustfile in T19 was run headless for 8 seconds against the same app under uvicorn to prove it works; the testcontainers snippet in T6 was not run (it needs Docker).

---

## T1. How do you structure tests for a backend service? (must know)

- **Test pyramid:** many fast unit tests, fewer integration tests, a handful of end-to-end tests.
- **Unit:** pure logic (pricing rules, validation, mappers) with no I/O; milliseconds each; run on every save.
- **Integration:** the API through its HTTP interface with a real database in a rolled-back transaction, external HTTP faked at the boundary; this layer catches the most real bugs in a CRUD service.
- **Contract:** the JSON shapes other services depend on (T22).
- **End-to-end / smoke:** a few critical journeys against a deployed environment after each deploy.
- **Non-functional:** load (T19), security scanning, migration tests on a copy of production schema.

| Layer | Speed | What it catches | What it misses |
| --- | --- | --- | --- |
| Unit | ms | Logic and edge cases | Wiring, SQL, serialization |
| API + real DB | 10-100 ms | Routing, validation, SQL, transactions, status codes | Other services' behavior |
| Contract | ms | Breaking changes between services | Runtime behavior |
| E2E | seconds-minutes | Deploy and config problems | Most edge cases (too slow to cover) |

- Senior framing: test behavior through public interfaces, not private methods, so refactors do not break tests.
- The anti-pattern to name is the "ice-cream cone": mostly slow UI or E2E tests, flaky and ignored.
- [fill in: how testing was layered on the Bank of America Python 3.8 migration (1M+ LOC) and what caught regressions]

---

## T2. Walk through pytest essentials: fixtures and scopes, conftest, parametrize, markers, built-in fixtures. (must know)

- **Fixtures** provide setup through arguments; `yield` splits setup from teardown, and teardown runs even if the test fails.
- **Scopes:** `function` (default), `class`, `module`, `package`, `session`; expensive resources (engine, container) are session-scoped, mutable state (a DB transaction) is function-scoped.
  A fixture can only depend on fixtures of the same or wider scope.
- **`conftest.py`** shares fixtures with every test in its directory and below, with no import needed.
- **`@pytest.mark.parametrize`** runs one test over many inputs, each reported separately.
- **Markers** tag tests (`@pytest.mark.integration`) so CI can select them with `-m "not integration"`; register them in config or `--strict-markers` fails.
- Built-ins to name: `tmp_path` (a fresh `pathlib.Path` per test), `monkeypatch` (set env vars, attributes, `sys.path`, auto-undone), `capsys` (captured stdout and stderr), `caplog` (captured log records), `request` (introspect the calling test).

```ini
# pytest.ini used for every test in this note
[pytest]
asyncio_mode = auto
asyncio_default_fixture_loop_scope = function
markers =
    integration: needs real infrastructure (PostgreSQL, Redis)
    slow: takes more than a second
filterwarnings =
    error
    ignore:Using .httpx. with .starlette.testclient. is deprecated
```

```python
import pytest

from flask_app import create_app


@pytest.fixture
def app():
    return create_app({"TESTING": True, "FEATURE_NEW_PRICING": True})


@pytest.fixture
def client(app):
    return app.test_client()


@pytest.mark.parametrize("payload", [None, {}, {"qty": 1}])
def test_quote_requires_notional(client, payload):
    r = client.post("/quote", json=payload)
    assert r.status_code == 400
    assert r.get_json()["error"] == "notional is required"


def test_reads_prefixed_env(monkeypatch):
    monkeypatch.setenv("FLASK_RATE_LIMIT_PER_MIN", "5")      # undone after the test
    app = create_app()                                        # no test_config: reads the env
    assert app.config["RATE_LIMIT_PER_MIN"] == 5              # parsed as JSON, so an int
```

- `filterwarnings = error` turns deprecation warnings into failures, so upgrades surface in CI instead of production; the one ignore above is Starlette 1.x asking for `httpx2` in its `TestClient`.
- Useful flags: `-x` (stop at first failure), `--lf` (rerun last failures), `-k expr` (select by name), `--durations=10` (slowest tests), `-p no:cacheprovider` in read-only CI.
- Fixture `autouse=True` is convenient and invisible; use it only for true global hygiene (resetting a registry), never for data setup.

---

## T3. How do you mock correctly? Patch where it is looked up, autospec, AsyncMock, and when not to mock. (must know)

- **Patch where the name is looked up, not where it is defined.**
  If `pricing.py` does `from fx_client import fetch_rate`, the test must patch `pricing.fetch_rate`; patching `fx_client.fetch_rate` leaves `pricing`'s reference untouched.
- **`autospec=True`** makes the mock enforce the real signature, so a call with wrong arguments fails the test instead of passing silently.
- **`AsyncMock`** for coroutines: `patch()` on an `async def` target creates one automatically (Python 3.8+); assert with `assert_awaited_once_with`.
- **Do not mock** what you own and can run cheaply (your ORM models, your database in a transaction, pure functions); mock at the **process boundary** (third-party HTTP, clock, randomness, email).

```python
# pricing.py
import logging

from fx_client import fetch_quote, fetch_rate      # names are bound into THIS module

log = logging.getLogger("app.pricing")


def price_in(amount_usd: float, currency: str) -> float:
    if currency == "USD":
        return amount_usd
    return round(amount_usd * fetch_rate("USD", currency), 2)


async def mid_price(symbol: str) -> float:
    q = await fetch_quote(symbol)
    return (q["bid"] + q["ask"]) / 2


def price_with_fallback(amount_usd: float, currency: str, fallback_rate: float) -> float:
    try:
        return price_in(amount_usd, currency)
    except Exception:
        log.warning("fx lookup failed, using fallback", extra={"currency": currency}, exc_info=True)
        return round(amount_usd * fallback_rate, 2)
```

```python
# test_mocking.py
import logging
from unittest.mock import AsyncMock, patch

import pytest

import pricing


def test_patch_where_it_is_looked_up():
    with patch("pricing.fetch_rate", autospec=True, return_value=0.9) as fake:
        assert pricing.price_in(100, "EUR") == 90.0
    fake.assert_called_once_with("USD", "EUR")


def test_autospec_catches_signature_drift():
    with patch("pricing.fetch_rate", autospec=True, return_value=1.0) as fake:
        with pytest.raises(TypeError):
            fake("USD")                               # missing 'quote': real signature enforced
    with patch("pricing.fetch_rate", return_value=1.0) as loose:
        loose("USD")                                  # a bare MagicMock accepts anything


async def test_patch_detects_async_functions():
    # patch() on an "async def" target creates an AsyncMock automatically (Python 3.8+).
    with patch("pricing.fetch_quote") as fake:
        assert isinstance(fake, AsyncMock)
        fake.return_value = {"bid": 99.0, "ask": 101.0}
        assert await pricing.mid_price("UST10Y") == 100.0
    fake.assert_awaited_once_with("UST10Y")
    # With autospec=True you get a signature-checked coroutine function backed by an AsyncMock.
    with patch("pricing.fetch_quote", autospec=True) as spec_fake:
        assert isinstance(spec_fake.mock, AsyncMock)


def test_caplog_on_fallback(caplog):
    with patch("pricing.fetch_rate", autospec=True, side_effect=TimeoutError("fx down")):
        with caplog.at_level(logging.WARNING, logger="app.pricing"):
            assert pricing.price_with_fallback(100, "EUR", fallback_rate=0.5) == 50.0
    [record] = caplog.records
    assert record.levelno == logging.WARNING
    assert record.getMessage() == "fx lookup failed, using fallback"
    assert record.currency == "EUR"
    assert isinstance(record.exc_info[1], TimeoutError)
```

- `side_effect` raises an exception, returns successive values from an iterable, or calls a function; use it for "fail twice then succeed" retry tests.
- Classic bug: `mock.assert_called_once()` misspelled as `mock.assert_called_ince()` used to pass silently; modern `unittest.mock` raises `AttributeError` for unknown `assert*` names, and `autospec` prevents it entirely.
- Over-mocking smell: a test that mocks the session, the query, and the result only proves the code calls what the test says it calls.
  Prefer a fake at the HTTP boundary (`httpx.MockTransport`, `respx`, `responses`) or a real database.
- Freeze time with an injected clock or `time-machine`/`freezegun`; never `sleep` in tests to wait for time to pass.

---

## T4. How do you test a FastAPI app: TestClient, dependency_overrides, async tests, lifespan? (must know)

- **`TestClient`** calls the ASGI app in-process through an httpx-style client; no server, no network.
- **`app.dependency_overrides[real_dep] = fake_dep`** swaps any `Depends()` (session, current user, settings) for the test; clear it afterwards.
- **Lifespan:** `with TestClient(app) as client:` runs startup and shutdown; a bare `TestClient(app)` does not.
- **Async tests:** `httpx.AsyncClient(transport=httpx.ASGITransport(app=app), base_url="http://test")` inside an `async def` test; `ASGITransport` does **not** run the lifespan, so enter `app.router.lifespan_context(app)` yourself or use `asgi-lifespan`.

The conftest (a transactional database fixture plus a dependency override):

```python
# conftest.py
from collections.abc import Iterator

import pytest
from fastapi.testclient import TestClient
from sqlalchemy import Engine, create_engine
from sqlalchemy.orm import Session
from sqlalchemy.pool import StaticPool

from app.db import Base, get_session
from app.main import app


@pytest.fixture(scope="session")
def engine() -> Iterator[Engine]:
    # One in-memory database for the whole run; StaticPool = one shared connection.
    # check_same_thread=False: TestClient runs sync endpoints in a worker thread.
    # autocommit=False (Python 3.12+): PEP 249 transactions, so SAVEPOINTs nest correctly.
    engine = create_engine(
        "sqlite://",
        connect_args={"check_same_thread": False, "autocommit": False},
        poolclass=StaticPool,
    )
    Base.metadata.create_all(engine)
    yield engine
    engine.dispose()


@pytest.fixture
def db_session(engine: Engine) -> Iterator[Session]:
    """Each test runs inside a transaction that is rolled back afterwards.

    The app's own commit() only releases a SAVEPOINT, so nothing leaks between tests.
    """
    with engine.connect() as conn:
        outer = conn.begin()
        session = Session(bind=conn, join_transaction_mode="create_savepoint")
        try:
            yield session
        finally:
            session.close()
            outer.rollback()


@pytest.fixture
def client(db_session: Session) -> Iterator[TestClient]:
    app.dependency_overrides[get_session] = lambda: db_session
    with TestClient(app) as c:          # "with" runs the lifespan startup and shutdown
        yield c
    app.dependency_overrides.clear()
```

The endpoint under test commits for real (`session.commit()`, with an `IntegrityError` mapped to 409), and the tests prove the rollback isolates them:

```python
# test_api.py
from sqlalchemy import func, select

from app.db import Item


def test_create_and_list(client):
    r = client.post("/items", json={"name": "bond", "price_cents": 9950})
    assert r.status_code == 201
    assert r.json() == {"id": 1, "name": "bond", "price_cents": 9950}
    assert [i["name"] for i in client.get("/items").json()] == ["bond"]


def test_rollback_isolates_tests(client, db_session):
    # The previous test committed "bond" through the API; it must be gone now.
    assert db_session.scalar(select(func.count()).select_from(Item)) == 0
    assert client.get("/items").json() == []


def test_duplicate_is_409_and_session_still_usable(client):
    assert client.post("/items", json={"name": "note", "price_cents": 1}).status_code == 201
    assert client.post("/items", json={"name": "note", "price_cents": 2}).status_code == 409
    assert len(client.get("/items").json()) == 1


def test_validation_error_is_422(client):
    r = client.post("/items", json={"name": "", "price_cents": -5})
    assert r.status_code == 422
    assert {e["loc"][-1] for e in r.json()["detail"]} == {"name", "price_cents"}
```

Lifespan behavior, verified:

```python
import httpx
from fastapi.testclient import TestClient

from app.main import app   # lifespan sets app.state.ready = True, then False on shutdown


def test_testclient_without_with_skips_lifespan():
    app.state.ready = False
    assert TestClient(app).get("/health/ready").json() == {"ready": False}


def test_testclient_with_block_runs_lifespan():
    with TestClient(app) as client:
        assert client.get("/health/ready").json() == {"ready": True}
    assert app.state.ready is False                   # shutdown ran too


async def test_asgi_transport_does_not_run_lifespan_unless_you_do():
    app.state.ready = False
    transport = httpx.ASGITransport(app=app)
    async with httpx.AsyncClient(transport=transport, base_url="http://test") as client:
        assert (await client.get("/health/ready")).json() == {"ready": False}
        async with app.router.lifespan_context(app):  # drive the lifespan yourself
            assert (await client.get("/health/ready")).json() == {"ready": True}
```

- Without `autocommit=False` (sqlite3 legacy transaction control), the rollback test above **failed**: the SAVEPOINT did not join the outer transaction, so committed rows leaked into the next test.
  On PostgreSQL the same fixture works without that flag.
- Override auth the same way: `app.dependency_overrides[get_current_user] = lambda: User(id=1, roles={"admin"})`, and keep one test that exercises the real auth dependency with a real token.
- Use `raise_server_exceptions=False` on `TestClient` to assert on a 500 instead of receiving the raised exception.
- More FastAPI specifics are in [FastAPI](04-FastAPI.md).

---

## T5. How do you test a Flask app?

- Build the app with an **app factory** (`create_app(test_config)`) so each test gets a fresh app and config; never import a module-level `app` configured for production.
- `app.test_client()` issues requests in-process; `app.test_request_context()` or `app.app_context()` lets you call code that uses `current_app`, `g`, or `request`.
- `TESTING=True` propagates exceptions to the test instead of returning a generic 500.

```python
from flask import Flask, current_app, jsonify, request


def create_app(test_config: dict | None = None) -> Flask:
    app = Flask(__name__)
    app.config.from_mapping(RATE_LIMIT_PER_MIN=60, FEATURE_NEW_PRICING=False)
    if test_config is None:
        app.config.from_prefixed_env()               # FLASK_RATE_LIMIT_PER_MIN=... in prod
    else:
        app.config.update(test_config)

    @app.post("/quote")
    def quote():
        body = request.get_json(silent=True) or {}
        if "notional" not in body:
            return jsonify(error="notional is required"), 400
        factor = 0.99 if current_app.config["FEATURE_NEW_PRICING"] else 1.0
        return jsonify(price=round(body["notional"] * factor, 2))

    return app


def test_quote_uses_test_config(client):       # fixtures from T2
    r = client.post("/quote", json={"notional": 1000})
    assert r.status_code == 200 and r.get_json() == {"price": 990.0}
```

- Database tests use the same transactional fixture as T4, with the Flask session factory pointed at the test connection.
- `app.test_cli_runner()` tests Click commands (`flask db upgrade`, custom admin commands).
- More in [Flask](03-Flask.md).

---

## T6. How do you test database code: SQLite vs real PostgreSQL, transaction rollback, factories? (must know)

- **Use the production engine in integration tests.**
  SQLite differs from PostgreSQL in types, constraint enforcement, locking, JSON operators, `ON CONFLICT` details, and isolation, so a green SQLite suite can hide real bugs.
- SQLite in memory is fine for fast unit tests of query-building logic; run the API suite against PostgreSQL in CI.
- **Testcontainers** starts a throwaway PostgreSQL per test session in Docker; migrations run once; each test runs in a transaction that is rolled back (T4's `db_session` fixture).
- **Factories** (`factory_boy`, or plain builder functions) create valid objects with overridable defaults, so tests state only what matters.

Not run here (needs Docker); the fixture shape:

```python
import pytest
from sqlalchemy import create_engine
from testcontainers.postgres import PostgresContainer


@pytest.fixture(scope="session")
def engine():
    with PostgresContainer("postgres:17") as pg:
        engine = create_engine(pg.get_connection_url())
        run_migrations(engine)            # alembic upgrade head, same as production
        yield engine
        engine.dispose()
```

A plain factory:

```python
def make_item(session, **overrides) -> Item:
    fields = {"name": f"item-{uuid.uuid4().hex[:8]}", "price_cents": 100} | overrides
    item = Item(**fields)
    session.add(item)
    session.flush()                       # get the id without committing
    return item
```

| Strategy | Speed | Fidelity | Trap |
| --- | --- | --- | --- |
| Transaction rollback per test | Fastest | High | Code that opens its own connection or commits on another thread escapes the transaction |
| Truncate tables after each test | Medium | High | Slow with many tables; order matters with foreign keys |
| New database per test (template DB) | Slow | Highest | Only for migration tests |
| SQLite stand-in | Fast | Low | False confidence |

- Test migrations too: `alembic upgrade head`, then `downgrade -1`, then `upgrade head` again on an empty database in CI, and check that `alembic check` reports no drift between models and migrations.
- With `pytest-xdist`, give each worker its own database (`worker_id` fixture) or schema.

---

## T7. How do you configure logging for a service, and how do you correlate logs across one request? (must know)

- Log to **stdout** in **JSON** (one object per line) and let the platform ship it; never log to local files in containers.
- Configure logging **once** at startup (`dictConfig` or a function like below); libraries only call `logging.getLogger(__name__)`.
- Put a **request ID** in a `contextvars.ContextVar` in middleware, attach it to every record with a `logging.Filter`, and echo it in the `X-Request-ID` response header; propagate it (or the W3C `traceparent`) to downstream calls.
- Log events with fields (`extra={"item_id": ...}`), not interpolated strings, so they are queryable; never log secrets, tokens, or full card or account numbers.

```python
import json
import logging
from contextvars import ContextVar

request_id_var: ContextVar[str | None] = ContextVar("request_id", default=None)


class RequestIdFilter(logging.Filter):
    """Stamp every record with the current request id (None outside a request)."""

    def filter(self, record: logging.LogRecord) -> bool:
        record.request_id = request_id_var.get()
        return True


class JsonFormatter(logging.Formatter):
    EXTRA_KEYS = ("item_id", "duration_ms", "status_code", "path")

    def format(self, record: logging.LogRecord) -> str:
        payload = {
            "ts": self.formatTime(record, "%Y-%m-%dT%H:%M:%S%z"),
            "level": record.levelname,
            "logger": record.name,
            "msg": record.getMessage(),
            "request_id": getattr(record, "request_id", None),
        }
        payload |= {k: getattr(record, k) for k in self.EXTRA_KEYS if hasattr(record, k)}
        if record.exc_info:
            payload["exc"] = self.formatException(record.exc_info)
        return json.dumps(payload)


def configure_logging(level: str = "INFO") -> None:
    handler = logging.StreamHandler()
    handler.addFilter(RequestIdFilter())
    handler.setFormatter(JsonFormatter())
    root = logging.getLogger()
    root.handlers[:] = [handler]
    root.setLevel(level)
```

```python
# FastAPI middleware (app/main.py)
@app.middleware("http")
async def request_id_and_timing(request: Request, call_next):
    request_id = request.headers.get("X-Request-ID") or uuid.uuid4().hex
    token = request_id_var.set(request_id)
    start = time.perf_counter()
    try:
        response = await call_next(request)
        log.info("request done", extra={
            "path": request.url.path,
            "status_code": response.status_code,
            "duration_ms": round((time.perf_counter() - start) * 1000, 1),
        })
    finally:
        request_id_var.reset(token)
    response.headers["X-Request-ID"] = request_id
    return response
```

The caplog test that proves the ID reaches a log line written inside a sync endpoint (which runs in a worker thread):

```python
def test_request_id_reaches_endpoint_logs(client, caplog):
    caplog.handler.addFilter(RequestIdFilter())
    with caplog.at_level(logging.INFO, logger="app"):
        r = client.post("/items", json={"name": "fx", "price_cents": 5},
                        headers={"X-Request-ID": "req-42"})
    assert r.status_code == 201
    created = [rec for rec in caplog.records if rec.getMessage() == "item created"]
    done = [rec for rec in caplog.records if rec.getMessage() == "request done"]
    # The sync endpoint ran in a worker thread and still saw the contextvar.
    assert created[0].request_id == "req-42" and created[0].item_id == 1
    assert done[0].request_id == "req-42" and done[0].status_code == 201
```

- Why a filter on the **handler**: filters on a logger apply only to records created by that exact logger, not to records propagated from its children.
- Use a `ContextVar`, not `threading.local`: it follows asyncio tasks, and Starlette copies the context into the thread pool for sync endpoints (proven by the test).
- In Flask, set the ID in `@app.before_request` (on `g` or the same `ContextVar`) and add the header in `@app.after_request`.
- Log levels: `ERROR` means someone should look; `WARNING` means degraded but handled (the fallback above); `INFO` is one line per request and business event; `DEBUG` is off in production.
- Uvicorn and gunicorn have their own loggers (`uvicorn.error`, `uvicorn.access`, `gunicorn.error`); route them through the same JSON handler or you get mixed formats.

---

## T8. Scenario: API latency spiked right after a deploy. Walk me through it. (must know)

1. **Mitigate first:** if the SLO is burning, roll back (or shift traffic off the canary) before root-causing; you can debug the bad version in staging or a canary afterwards.
2. **Scope:** which endpoints, p50 or only p99, all pods or some, all customers or one; line it up against deploy markers on the dashboards.
3. **Diff the change:** code, dependency versions (lockfile diff), config (pool size, worker count, timeouts, log level), migrations, infrastructure (instance type, CPU limits).
4. **Find where the time goes:** traces for a slow request (T18) split time into app, database, and downstream calls.
5. **Confirm and fix**, then add a guard so it cannot recur.

Usual culprits after a deploy:

| Evidence | Likely cause | Confirm with |
| --- | --- | --- |
| More DB queries per request | N+1 from a new serializer field or relationship | Query count per request, `pg_stat_statements` calls |
| One query much slower | New filter on an unindexed column, or a migration that dropped or invalidated an index | `EXPLAIN (ANALYZE, BUFFERS)` |
| Only first minutes are slow | Cold caches, JIT or import-time work, connection pool warm-up | Latency by pod age |
| All pods slow, CPU pegged | Debug logging on, expensive validation, sync JSON of huge payloads | `py-spy top` on a live pod |
| Slow, CPU idle | New synchronous downstream call, smaller pool, lock contention | Traces; pool checked-out metrics |
| One pod slow | Noisy neighbor, bad node, CPU throttling | Per-pod dashboards, `container_cpu_cfs_throttled_periods_total` |

- Prevent: canary deploys with automatic rollback on latency and error-rate SLOs, query-count assertions in API tests ([SQLAlchemy N+1](06-SQL-and-SQLAlchemy.md)), and a small load test on hot endpoints in CI.
- [fill in: a real latency regression you chased, what the evidence was, and the fix]

---

## T9. Scenario: the service returns 500s only under load. What do you suspect? (must know)

- First suspect: **connection pool exhaustion.**
  Requests wait `pool_timeout` (30 s by default) for a database connection, then fail with `sqlalchemy.exc.TimeoutError: QueuePool limit of size 5 overflow 10 reached, connection timed out, timeout 30.00`.
  Latency climbs toward 30 s just before the 500s: that shape is the tell.
- Why the pool runs dry:
  1. **Leaked sessions:** a code path that never closes its Session (no context manager, an early return, a background task holding one).
  2. **Long transactions:** a connection held while calling a slow third-party API or doing CPU work; `pg_stat_activity` shows `idle in transaction`.
  3. **Pool smaller than concurrency:** async workers accept hundreds of concurrent requests but the pool holds 15.
  4. **Server limit:** replicas x workers x pool exceeds PostgreSQL `max_connections` ("sorry, too many clients already").
- Other load-only failures: the sync thread pool (AnyIO's default limiter is 40 threads for FastAPI `def` endpoints), file descriptor limits, downstream rate limits (429 surfaced as 500), memory limits (OOM kill surfaces as 502 at the LB), and races that only appear with concurrency.

Confirm:

```sql
-- PostgreSQL: who holds connections and in what state
SELECT state, count(*), max(now() - state_change) AS oldest
FROM pg_stat_activity WHERE datname = current_database()
GROUP BY state;
```

```python
# App side: expose pool gauges as metrics or log them periodically
engine.pool.status()          # "Pool size: 5  Connections in pool: 0 Current Overflow: 10 Current Checked out connections: 15"
```

Fix:

- Always scope sessions (`with SessionLocal() as s`, a `yield` dependency), and never do network I/O inside a transaction.
- Set `idle_in_transaction_session_timeout` and `statement_timeout` in PostgreSQL so a stuck transaction cannot hold a connection forever.
- Size pools with the math in [SQL and SQLAlchemy S18](06-SQL-and-SQLAlchemy.md), or add PgBouncer.
- Fail fast: a lower `pool_timeout` (a few seconds) plus a 503 with `Retry-After` beats 30 s timeouts that pile up; add concurrency limits so the service sheds load before the database falls over.
- Reproduce with a load test (T19) at production concurrency; unit tests will never show it.

---

## T10. Scenario: a FastAPI service's latency jumps for every request at once, and health checks time out. (must know)

- Classic cause: **the event loop is blocked.**
  One worker runs one event loop; a synchronous call inside `async def` (`requests.get`, `time.sleep`, a sync DB driver, `boto3`, bcrypt hashing, pandas on a big frame) freezes every request on that worker, including health checks, so Kubernetes may restart healthy pods.
- Detect it: asyncio **debug mode** (`PYTHONASYNCIODEBUG=1` or `python -X dev`) logs `Executing <Task ...> took 0.300 seconds` for any step longer than `loop.slow_callback_duration` (0.1 s); a **loop-lag monitor** exported as a metric; `py-spy dump --pid` shows the main thread stuck in a sync call; on Python 3.14, `python -m asyncio ps <pid>` lists a live process's tasks.
- Fix: declare the endpoint `def` (FastAPI runs it in the thread pool), offload with `await asyncio.to_thread(...)` or `run_in_threadpool`, use async clients (`httpx.AsyncClient`, `asyncpg`, `redis.asyncio`), and push CPU-heavy work to a process pool or a worker queue.

A tiny demo that detects the blocking call:

```python
# blocking_app.py
import asyncio
import time

from fastapi import FastAPI

app = FastAPI()


@app.get("/blocking")
async def blocking() -> dict:
    time.sleep(0.3)                    # BUG: sync call inside async def freezes the event loop
    return {"ok": True}


@app.get("/fixed")
async def fixed() -> dict:
    await asyncio.to_thread(time.sleep, 0.3)   # or make the endpoint "def", or use an async client
    return {"ok": True}


class LoopLagMonitor:
    """Measures how late a periodic timer fires; lag means something blocked the loop."""

    def __init__(self, interval: float = 0.01):
        self.interval = interval
        self.max_lag = 0.0
        self._task: asyncio.Task | None = None

    async def _run(self) -> None:
        loop = asyncio.get_running_loop()
        while True:
            start = loop.time()
            await asyncio.sleep(self.interval)
            self.max_lag = max(self.max_lag, loop.time() - start - self.interval)

    def __enter__(self) -> "LoopLagMonitor":
        self._task = asyncio.get_running_loop().create_task(self._run())
        return self

    def __exit__(self, *exc) -> None:
        self._task.cancel()
```

```python
# test_blocking.py
import asyncio
import logging
import time

import httpx

from blocking_app import LoopLagMonitor, app


async def two_concurrent_requests(path: str) -> float:
    transport = httpx.ASGITransport(app=app)
    async with httpx.AsyncClient(transport=transport, base_url="http://test") as client:
        start = time.perf_counter()
        responses = await asyncio.gather(client.get(path), client.get(path))
        assert all(r.status_code == 200 for r in responses)
        return time.perf_counter() - start


async def test_blocking_call_serializes_requests_and_lags_the_loop():
    with LoopLagMonitor() as monitor:
        elapsed = await two_concurrent_requests("/blocking")
        await asyncio.sleep(0.02)                     # let the monitor observe the last stall
    assert elapsed >= 0.55                            # 2 x 0.3 s: requests ran one after another
    assert monitor.max_lag >= 0.25                    # the loop could not run anything else


async def test_offloaded_call_runs_concurrently():
    with LoopLagMonitor() as monitor:
        elapsed = await two_concurrent_requests("/fixed")
    assert elapsed < 0.5                              # both sleeps overlapped
    assert monitor.max_lag < 0.1


async def test_asyncio_debug_mode_names_the_slow_callback(caplog):
    loop = asyncio.get_running_loop()
    loop.set_debug(True)                              # same as PYTHONASYNCIODEBUG=1
    loop.slow_callback_duration = 0.1                 # default is 0.1 s
    try:
        with caplog.at_level(logging.WARNING, logger="asyncio"):
            await two_concurrent_requests("/blocking")
    finally:
        loop.set_debug(False)
    slow = [r.getMessage() for r in caplog.records if "took" in r.getMessage()]
    assert slow and slow[0].startswith("Executing <Task")
```

- Trick question: "Is `async def` always faster?" No; a `def` endpoint with a sync driver is correct and often simpler, and a mostly-CPU endpoint gains nothing from async.
- Watch the thread pool too: 40 default threads means the 41st concurrent sync call queues.
- Make health checks cheap and separate liveness (process alive) from readiness (dependencies reachable), so a blocked loop does not cascade into restarts of every pod.

---

## T11. Scenario: memory keeps growing in the workers until they are OOM-killed.

- Symptoms: RSS climbs steadily per worker; pods restart with `OOMKilled` (exit code 137); latency degrades before each restart.
- Common causes: an unbounded module-level cache or list, `functools.cache` (unbounded) on a function with high-cardinality arguments, per-request objects stored in a global registry, reading entire files or result sets into memory, a leaking C extension, and allocator fragmentation.
- Find it with **`tracemalloc` snapshot diffs** (which line allocated what is still alive), `memray` for native allocations and flame graphs, and `gc.get_objects()` type counts for object-level leaks.
- Mitigate while you look: gunicorn `max_requests` with `max_requests_jitter` recycles each worker after N requests (jitter stops them all restarting at once); it is a band-aid, not a fix.

```python
# leaky.py
_seen_requests: list[bytes] = []          # module-level "cache" with no bound: a classic leak


def handle(payload: bytes) -> int:
    _seen_requests.append(payload * 10)
    return len(payload)
```

```python
import tracemalloc

import leaky


def test_snapshot_diff_points_at_the_leak():
    tracemalloc.start(10)
    before = tracemalloc.take_snapshot()
    for _ in range(2000):
        leaky.handle(b"x" * 512)
    after = tracemalloc.take_snapshot()
    tracemalloc.stop()

    top = after.compare_to(before, "lineno")[0]
    frame = top.traceback[0]
    assert frame.filename.endswith("leaky.py") and frame.lineno == 5
    assert top.size_diff > 2000 * 5000                 # about 10 MB retained
```

- In production, expose a guarded debug endpoint or signal handler that takes a snapshot, or reproduce locally with a replay of production traffic; `tracemalloc` costs CPU and memory, so do not leave it on.
- Fix patterns: bound caches (`lru_cache(maxsize=...)`, TTL caches, Redis), stream large responses and query results (`yield_per`, `StreamingResponse`), and close clients and sessions.
- Kubernetes: set a memory request close to real usage and a limit with headroom; alert on growth rate, not only on the limit.

```python
# gunicorn.conf.py (the band-aid, conventional values)
max_requests = 1000
max_requests_jitter = 100
```

---

## T12. Scenario: intermittent 502 and 504 errors behind a load balancer.

- **502 Bad Gateway:** the load balancer got a broken or closed connection from the app.
  **504 Gateway Timeout:** the app did not answer within the load balancer's timeout.
- **Keep-alive mismatch (the classic intermittent 502):** the LB keeps idle upstream connections open longer than the app server does (AWS ALB idle timeout defaults to 60 s; gunicorn `keepalive` defaults to 2 s; uvicorn `--timeout-keep-alive` defaults to 5 s).
  The app closes an idle connection just as the LB sends a request on it, and that request fails with 502.
  Fix: the app's keep-alive timeout must be **longer** than the LB idle timeout (for example uvicorn `--timeout-keep-alive 65` behind a 60 s ALB).
- **Worker timeout:** a sync gunicorn worker that exceeds `timeout` (30 s default) is killed (`WORKER TIMEOUT` in the log) and the LB returns 502; a request slower than the LB or nginx `proxy_read_timeout` (60 s default) returns 504.
  Fix the slow path (move it to a background job with 202 + polling), rather than raising every timeout.
- **Deploys:** pods terminated while still receiving traffic return 502s during rollouts.
  Fix with a readiness probe, a `preStop` sleep of a few seconds so endpoints deregister first, graceful SIGTERM handling, and `terminationGracePeriodSeconds` longer than the longest request.
- **OOM kills and crashes** also appear as 502 at the LB; check pod restarts and exit codes.
- Evidence: LB access logs (target status code, target processing time), app logs at the same timestamp (no log line at all means the request never reached the app), and restart counts.

```text
# Timeouts should nest from the outside in: client > LB idle/read > app worker > DB statement
client 90s > ALB idle 60s > uvicorn keep-alive 65s (idle) / request budget 30s > statement_timeout 5s
```

---

## T13. Scenario: sporadic deadlock errors from the database.

- Symptom: occasional 500s with `SQLSTATE 40P01` (`deadlock detected` in PostgreSQL, `psycopg.errors.DeadlockDetected`); PostgreSQL aborted one transaction to break a lock cycle.
- Read the PostgreSQL log: the deadlock report names both processes and both statements, which usually shows two code paths locking the same rows in opposite orders.
- Fix: lock in a consistent order (sort ids; parent before child), keep transactions short, avoid mixing bulk updates with row-by-row updates on the same table, and retry the whole transaction with jittered backoff on 40P01 and 40001.
- Distinguish from **lock waits** (no cycle, just a long blocker such as a migration holding `ACCESS EXCLUSIVE`): find the blocker with `pg_blocking_pids()` and set `lock_timeout`.
- Reproduce in a test with two connections and two threads, each locking rows in the opposite order, to prove the fix.
- Details and the `SKIP LOCKED` job-queue pattern: [SQL and SQLAlchemy S11](06-SQL-and-SQLAlchemy.md).

---

## T14. Scenario: it works locally but fails in the container.

Check these in order; each one is a real, common incident:

1. **Bind address:** the server listens on `127.0.0.1` inside the container, so nothing outside can reach it; bind `0.0.0.0`.
2. **`localhost` means the container itself:** a `DATABASE_URL` pointing at `localhost` works on a laptop and fails in Docker; use the service name (Compose) or the Kubernetes Service DNS name.
3. **Configuration and secrets:** an env var set in your shell or `.env` file is missing in the image or the manifest; fail fast at startup with a settings model that validates required values.
4. **Architecture and native wheels:** an Apple Silicon build produces `arm64` images and the cluster runs `amd64` (`exec format error`); Alpine's musl libc breaks or slows manylinux wheels; `psycopg` needs `libpq` unless you use the binary package.
5. **Permissions and filesystem:** a non-root user cannot write to the app directory, or the root filesystem is read-only; write to a mounted volume or `/tmp`.
6. **Resource limits:** a memory limit lower than local RAM means OOM kills (exit 137); a CPU limit on a large node makes `workers = os.cpu_count() * 2 + 1` spawn dozens of workers, because `os.cpu_count()` reports the node's CPUs, not the container's quota.
   Set the worker count explicitly from config.
7. **Signals and PID 1:** shell-form `CMD python app.py` runs under `/bin/sh`, which does not forward SIGTERM, so shutdown is never graceful; use exec-form `CMD ["gunicorn", ...]` or an init such as `tini`.
8. **Time zone, locale, and file paths:** relative paths resolve against a different working directory; use `pathlib` relative to the package, UTC everywhere.

Tools: `docker run --rm -it --entrypoint sh image`, `docker inspect`, `kubectl logs --previous`, `kubectl describe pod` (last state, exit code, OOMKilled), `kubectl debug` with an ephemeral container.
Container specifics are in [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md).

---

## T15. What is on your code review checklist for a Python API pull request?

Say the top five, then offer the rest:

1. **Correctness and contracts:** does it do what the ticket says; status codes, error shapes, idempotency of retried writes, and backward compatibility of the API and the database schema (expand/contract).
2. **Data access:** N+1 queries, missing indexes for new filters, transactions scoped correctly, no network I/O inside a transaction, sessions always closed.
3. **Security:** input validated with Pydantic, no SQL built from strings, authorization checked on every object (not just authentication), secrets not logged or committed, dependencies pinned.
4. **Failure handling:** timeouts on every outbound call, retries only for idempotent operations with backoff, no bare `except:` or swallowed exceptions, errors logged once with context.
5. **Tests:** they fail without the change, cover the edge cases and the error paths, and do not over-mock.

Then:

- Async correctness: no blocking calls in `async def`, no shared `AsyncSession` across tasks.
- Observability: structured logs with request IDs, metrics or spans for new external calls.
- Performance: pagination on list endpoints, bounded caches, streaming for large payloads.
- Readability: clear names, type hints on public functions, small functions, no dead code or stale comments; docs and OpenAPI descriptions updated.
- Operability: config via environment, feature flag for risky changes, migration safe under rolling deploy, rollback plan.
- How to review: run it or read the tests first, comment on the design before nits, label nits as optional, and approve with follow-ups rather than blocking on style that a linter (`ruff`) should own.

---

## T16. How do you profile a slow Python API? cProfile, py-spy, pyinstrument, line_profiler.

| Tool | Type | Use when | Overhead |
| --- | --- | --- | --- |
| `cProfile` + `pstats` | Deterministic, stdlib | Reproducible slow function locally or in a test | High; distorts very small functions |
| `py-spy` | Sampling, attaches to a running PID | Production: `py-spy top --pid`, `py-spy dump --pid` (stack of every thread), `py-spy record -o flame.svg --pid` | Very low; needs `SYS_PTRACE` in containers |
| `pyinstrument` | Sampling, call-tree output, understands async | One slow request end to end; has an async mode | Low |
| `line_profiler` (`@profile`, `kernprof -l -v`) | Per line | After you know the hot function | High, targeted |
| `memray` / `tracemalloc` | Memory | Leaks and allocation spikes (T11) | Medium |

```python
import cProfile
import io
import pstats


def slow_serialize(rows):
    out = ""
    for r in rows:
        out += f"{r},"                        # quadratic string building
    return out


def handler():
    rows = list(range(20_000))
    total = sum(rows)
    return total, slow_serialize(rows)


def test_cprofile_finds_the_hot_function():
    profiler = cProfile.Profile()
    profiler.enable()
    handler()
    profiler.disable()
    buf = io.StringIO()
    stats = pstats.Stats(profiler, stream=buf).sort_stats("cumulative")
    stats.print_stats(5)
    assert "slow_serialize" in buf.getvalue()
    # the hottest own-time entry is the serializer, not sum()
    own = sorted(stats.stats.items(), key=lambda kv: kv[1][2], reverse=True)
    assert own[0][0][2] == "slow_serialize"
```

- `cumulative` time finds the expensive call path; `tottime` (own time) finds the function doing the work.
- Profile with production-like data sizes; many problems (quadratic loops, N+1) are invisible on 10 rows.
- For a latency problem, check a trace first: if 90% of the request is waiting on the database or a downstream service, a CPU profiler shows nothing useful.
- Python 3.14 also lets `python -m pdb -p <pid>` attach a debugger to a running process (PEP 768); treat that as a staging tool, not a production habit.

Go deeper: [profiling guide](../../../12-Performance-Engineering/02-Profiling/profiling_guide.md), [Python profiling and diagnostics](../Python_Zero_to_Godhood/Chapter_29_PROFILING_BENCHMARKING_AND_DIAGNOSTICS.md).

---

## T17. How do you make a Python API faster?

Measure first (T16, T18), then work down this list; most wins are in I/O, not Python:

1. **Database:** remove N+1 (eager loading), add the right index, select only needed columns, push aggregation into SQL, batch writes.
2. **Caching:** Redis cache-aside for hot reads, HTTP caching (`ETag`, `Cache-Control`) for public GETs; see [SQL and SQLAlchemy S24](06-SQL-and-SQLAlchemy.md).
3. **Pagination:** never return unbounded lists; keyset (cursor) pagination beats `OFFSET` on deep pages.
4. **Concurrency for I/O:** fan out independent downstream calls with `asyncio.gather` (with a timeout and a concurrency limit) instead of calling them one after another.
5. **Connection reuse:** one long-lived `httpx.Client` or `AsyncClient` per process (created in the lifespan), not a new client per request, so TCP and TLS handshakes are reused; same for DB pools.
6. **Serialization:** declare a `response_model` or return type in FastAPI so Pydantic serializes straight to JSON bytes in Rust; FastAPI 0.142 marks `ORJSONResponse` and `UJSONResponse` deprecated for that reason.
   In Flask, a custom JSON provider backed by `orjson` helps for large payloads.
7. **Compression:** `GZipMiddleware(minimum_size=1000)` (or at the proxy) for large JSON; skip it for tiny responses.
8. **Background work:** send emails, webhooks, and reports through a queue; return 202.
9. **Workers:** tune gunicorn/uvicorn worker count to CPU and memory, not to a formula from a blog.

- Report the result as p50/p95/p99 and throughput before and after, measured the same way; "it feels faster" is not a result.
- [fill in: an optimization you shipped, with the before and after numbers you actually measured]

---

## T18. How does distributed tracing with OpenTelemetry work?

- A **trace** is one request's journey across services; it is a tree of **spans** (timed operations with attributes); context propagates between services in the W3C `traceparent` header.
- Auto-instrumentation packages (`opentelemetry-instrumentation-fastapi`, `-flask`, `-sqlalchemy`, `-httpx`, `-requests`) create spans for inbound requests, SQL queries, and outbound calls with no code changes; export over OTLP to a collector, then Jaeger, Tempo, Datadog, or X-Ray.
- Add manual spans around business steps, and attach the trace ID to log records so logs and traces link.

```python
from opentelemetry import trace
from opentelemetry.sdk.trace import TracerProvider
from opentelemetry.sdk.trace.export import SimpleSpanProcessor
from opentelemetry.sdk.trace.export.in_memory_span_exporter import InMemorySpanExporter

exporter = InMemorySpanExporter()
provider = TracerProvider()
provider.add_span_processor(SimpleSpanProcessor(exporter))   # BatchSpanProcessor + OTLP in prod
trace.set_tracer_provider(provider)
tracer = trace.get_tracer("app.pricing")


def price_order(order_id: str, qty: int) -> float:
    with tracer.start_as_current_span("price_order") as span:
        span.set_attribute("order.id", order_id)
        span.set_attribute("order.qty", qty)
        with tracer.start_as_current_span("fetch_rate"):
            rate = 0.9                                          # an outbound call in real code
        return qty * rate


def test_spans_are_nested_and_tagged():
    exporter.clear()
    assert price_order("o-1", 10) == 9.0
    child, parent = exporter.get_finished_spans()               # children finish first
    assert parent.name == "price_order" and parent.attributes["order.id"] == "o-1"
    assert child.parent.span_id == parent.context.span_id
    assert child.context.trace_id == parent.context.trace_id
```

- Sampling: head sampling (keep N% at the start) is cheap; tail sampling in the collector keeps all errors and slow traces, which is what you want during incidents.
- The three signals: **logs** (what happened), **metrics** (how much and how often; alert on these), **traces** (where the time went).
- RED metrics per endpoint: **R**ate, **E**rrors, **D**uration (as a histogram, so you get percentiles).

---

## T19. How do you load test an API, and what do you measure?

- Tools: **Locust** (Python, scenarios as code), **k6** (JavaScript scripts, low overhead), `wrk`/`hey` for quick single-endpoint checks.
- Measure **latency percentiles** (p50, p95, p99; averages hide tails), **throughput** (requests per second), **error rate**, and **saturation** on the server (CPU, memory, pool checked-out, DB connections, event-loop lag).
- Test shapes: **load** (expected peak), **stress** (ramp until it breaks, find the knee), **soak** (hours at normal load, finds leaks as in T11), **spike** (sudden burst, tests autoscaling and pool limits).
- Run against a production-like environment with production-like data; a load test against an empty database proves nothing about queries.

```python
# locustfile.py: run with
#   locust -f locustfile.py --headless -u 20 -r 10 -t 8s --host http://127.0.0.1:8765
import random
import uuid

from locust import HttpUser, between, task


class ApiUser(HttpUser):
    wait_time = between(0.05, 0.2)             # think time per simulated user

    @task(5)
    def list_items(self):
        self.client.get("/items", name="GET /items")

    @task(1)
    def create_item(self):
        body = {"name": f"item-{uuid.uuid4().hex[:12]}", "price_cents": random.randint(1, 10_000)}
        with self.client.post("/items", json=body, name="POST /items", catch_response=True) as r:
            if r.status_code != 201:
                r.failure(f"unexpected {r.status_code}")
```

- Closed-loop tools (a fixed number of users who wait for each response) under-report tail latency when the server slows down (**coordinated omission**); for latency SLO checks prefer an open-loop, constant-arrival-rate model (k6 `constant-arrival-rate`).
- Watch the client machine: a saturated load generator reports server slowness that is not there.
- Put the p95/p99 and error-rate thresholds in CI for the hot endpoints so regressions fail the build.

---

## T20. How do you wire tests into CI? Fail fast, parallelism, flaky tests.

- Stage order: lint and type check (`ruff`, `mypy`) -> unit tests -> integration tests with PostgreSQL as a service container -> build image -> smoke test the image -> deploy.
- **Fail fast:** cheapest checks first; `pytest -x` or `--maxfail=1` for quick feedback on branches, full run on main.
- **Parallel:** `pytest-xdist` (`pytest -n auto`, `--dist loadscope` to keep a module's tests on one worker); give each worker its own database or schema.
- Cache dependencies (keyed on the lockfile), pin versions, and run the same commands locally and in CI.
- **Flaky tests** are bugs: quarantine with a marker and a ticket, then fix the cause (shared state between tests, time, ordering, real network, `sleep`-based waits); never auto-retry the whole suite and call it green.
- Publish JUnit XML and coverage reports; gate merges on required checks.
- Pipeline specifics (Jenkins, GitHub Actions, Azure DevOps) are in [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md).

---

## T21. What does code coverage tell you, and what does it not?

- Coverage tells you which lines (and with `--cov-branch`, which branches) ran during tests: `pytest --cov=app --cov-branch --cov-report=term-missing --cov-fail-under=85` with `pytest-cov`.
- It does **not** tell you the tests assert anything, that edge cases are covered, or that behavior is correct; 100% coverage with no assertions is possible.
- Use it to find untested code (the `term-missing` report), not as a target to game; a sensible floor for a service is 80-90% with review of what is excluded.
- Better signals: tests that fail when the code is broken (mutation testing with `mutmut` or `cosmic-ray` measures exactly that), bug escape rate, and whether every bug fix came with a regression test.

---

## T22. What are contract tests, and why do microservices need them?

- A **contract** is the set of requests and responses a consumer relies on; a contract test fails the provider's build when a change would break a consumer, before deploy.
- **Consumer-driven contracts (Pact):** consumers publish expectations; the provider verifies them in CI against its real app.
- A lighter version for an internal API: generate the OpenAPI schema from the app and assert the fields consumers rely on are still present with the same types, or diff the schema against the last released one.

```python
from app.main import app

# What the consumers (order service, UI) rely on. Owned jointly; breaking it fails CI.
CONSUMER_EXPECTATIONS = {
    "ItemOut": {"required": {"id", "name", "price_cents"}, "types": {"id": "integer", "price_cents": "integer"}},
}


def test_provider_still_satisfies_consumer_contract():
    schemas = app.openapi()["components"]["schemas"]
    for name, expected in CONSUMER_EXPECTATIONS.items():
        schema = schemas[name]
        assert expected["required"] <= set(schema["required"]), f"{name} dropped a required field"
        for field, typ in expected["types"].items():
            assert schema["properties"][field]["type"] == typ, f"{name}.{field} changed type"
```

- For Kafka or RabbitMQ messages, the contract is the message schema: a schema registry (Avro or Protobuf) with compatibility rules (backward, forward) does the same job; see [Microservices and Messaging](08-Microservices-and-Messaging.md).
- Adding optional fields is safe; removing or renaming fields, changing types, or tightening validation is breaking and needs versioning (see [REST API Design](05-REST-API-Design.md)).

---

## T23. What is property-based testing? When is it worth it?

- Instead of hand-picked examples, you state a **property** that must hold for all inputs, and Hypothesis generates many inputs, then **shrinks** a failure to the smallest counterexample.
- Worth it for parsers and serializers (round trips), pagination and batching, money and rounding rules, state machines, and anything with tricky edge cases (empty, unicode, huge integers).

```python
from hypothesis import given
from hypothesis import strategies as st

from cursor import decode_cursor, encode_cursor, paginate


@given(st.text(), st.integers(min_value=0, max_value=2**63 - 1))
def test_cursor_round_trips(created_at, id_):
    assert decode_cursor(encode_cursor(created_at, id_)) == (created_at, id_)


@given(st.sets(st.integers(min_value=0, max_value=10_000)), st.integers(min_value=1, max_value=50))
def test_pagination_covers_every_row_exactly_once(ids, limit):
    rows = sorted(ids)
    pages = paginate(rows, limit)
    assert [r for page in pages for r in page] == rows
    assert all(1 <= len(p) <= limit for p in pages)
```

- `encode_cursor` is URL-safe base64 of a compact JSON pair with the padding stripped; `paginate` is keyset pagination over sorted ids (both in the scratch module that these tests import).
- Pin a failing example with `@example(...)` so it stays in the suite as a regression test.
- Keep example-based tests for the business cases people read; property tests complement them.

---

## T24. How do you debug a Python service: pdb, breakpoint(), post-mortem?

- `breakpoint()` (Python 3.7+) drops into `pdb` at that line; `PYTHONBREAKPOINT=0` disables every call, so a forgotten one cannot hang production, and `PYTHONBREAKPOINT=ipdb.set_trace` swaps the debugger.
- **Post-mortem:** `python -m pdb -c continue app.py` stops at an uncaught exception; `pytest --pdb` opens the debugger at the failing assertion; `pdb.pm()` inspects the last traceback in a REPL.
- Commands to know: `n` (next), `s` (step into), `c` (continue), `l`/`ll` (list), `p expr`, `pp`, `w` (where), `u`/`d` (up and down the stack), `b file:line, condition`, `interact`.
- In async code, `breakpoint()` pauses the whole event loop, so every request on that worker freezes; debug with a single worker locally.
- Hung process: `py-spy dump --pid` (no restart), `faulthandler.dump_traceback_later(...)` or `python -X faulthandler`, and on Python 3.14 `python -m pdb -p <pid>` and `python -m asyncio pstree <pid>`.
- In production, prefer logs, traces, and metrics over debuggers; reproduce with the same inputs locally, then write the failing test before the fix.
- Debug with the IDE (VS Code or PyCharm `debugpy` attach) for multi-file stepping; remote-attach to a container only in non-production environments.

---

## Go deeper

Vault notes:

- [Unit testing frameworks compared](../../../10-Development-Practices/01-Testing/unit_testing_comparison.md).
- [pytest examples: fixtures, parametrize, custom markers](../unittesting_pytest) (runnable code, no entry note), for example [the fixture test](../unittesting_pytest/fixtures/test_mydb.py).
- [Testing, debugging, and quality assurance](../Python_Zero_to_Godhood/Chapter_41_Testing_Debugging_and_Quality_Assurance.md).
- [Profiling, benchmarking, and diagnostics](../Python_Zero_to_Godhood/Chapter_29_PROFILING_BENCHMARKING_AND_DIAGNOSTICS.md), [CPU- and I/O-bound concurrency](../Python_Zero_to_Godhood/Chapter_26_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md), [profiling guide](../../../12-Performance-Engineering/02-Profiling/profiling_guide.md).
- [Tracing and observability](../../../13-Agentic-AI/Agentic_AI_Zero_to_Godhood/Volume_10_Evaluation_and_Observability/Chapter_06_Tracing_and_Observability.md).
- In this pack: [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md), [FastAPI](04-FastAPI.md), [Flask](03-Flask.md), [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md).

Official docs:

- [pytest](https://docs.pytest.org/en/stable/), [unittest.mock](https://docs.python.org/3/library/unittest.mock.html), [Hypothesis](https://hypothesis.readthedocs.io/en/latest/).
- FastAPI: [testing dependencies](https://fastapi.tiangolo.com/advanced/testing-dependencies/), [async tests](https://fastapi.tiangolo.com/advanced/async-tests/), [testing lifespan events](https://fastapi.tiangolo.com/advanced/testing-events/); [Flask testing](https://flask.palletsprojects.com/en/stable/testing/).
- [SQLAlchemy: joining a Session into an external transaction](https://docs.sqlalchemy.org/en/21/orm/session_transaction.html), [SQLite transaction control](https://docs.sqlalchemy.org/en/21/dialects/sqlite.html).
- Python: [developing with asyncio](https://docs.python.org/3/library/asyncio-dev.html), [tracemalloc](https://docs.python.org/3/library/tracemalloc.html), [profilers](https://docs.python.org/3/library/profile.html), [pdb](https://docs.python.org/3/library/pdb.html).
- [Locust](https://docs.locust.io/en/stable/), [OpenTelemetry Python](https://opentelemetry.io/docs/languages/python/), [gunicorn settings](https://docs.gunicorn.org/en/stable/settings.html), [uvicorn settings](https://www.uvicorn.org/settings/).
