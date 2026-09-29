---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://fastapi.tiangolo.com/, https://fastapi.tiangolo.com/async/, https://fastapi.tiangolo.com/tutorial/dependencies/, https://fastapi.tiangolo.com/tutorial/dependencies/dependencies-with-yield/, https://fastapi.tiangolo.com/tutorial/handling-errors/, https://fastapi.tiangolo.com/advanced/events/, https://fastapi.tiangolo.com/advanced/testing-dependencies/, https://fastapi.tiangolo.com/advanced/async-tests/, https://fastapi.tiangolo.com/advanced/custom-response/, https://fastapi.tiangolo.com/deployment/server-workers/, https://www.starlette.io/middleware/, https://asgi.readthedocs.io/en/latest/, https://docs.pydantic.dev/latest/, https://docs.sqlalchemy.org/en/20/orm/extensions/asyncio.html, https://github.com/Kludex/uvicorn-worker]
---

# FastAPI

FastAPI for a role titled "Python Developer (Flask & FastAPI)": the async model, dependency injection, Pydantic v2, errors, testing, databases, deployment, and how it compares with Flask.
A1, A2, A3, A4, A5, A6, A9, and A10 are near-certain; A2 (`async def` versus `def`) is the single most asked FastAPI question, and A3 (Flask versus FastAPI) is almost guaranteed for this role.
Every code sample here was run on 2026-09-29 under Python 3.14.7 with FastAPI 0.142.0, Starlette 1.7.0, Pydantic 2.13.5, SQLAlchemy 2.1.1, httpx 0.28.1, PyJWT 2.15.1, and aiosqlite 0.22.1: 35 pytest tests passed, and the reference app booted under `uvicorn --workers 2`, gunicorn with `uvicorn_worker.UvicornWorker`, and gunicorn 26's `asgi` worker.

---

## A1. What is FastAPI, and why is it fast? (must know)

> "FastAPI is an ASGI web framework built on Starlette for the web layer and Pydantic for validation and serialization.
> You declare parameters and bodies with type hints; FastAPI validates requests, converts types, serializes responses, and generates the OpenAPI schema and Swagger UI from the same declarations.
> It is fast in two senses: at runtime, because it runs on an async event loop under uvicorn and Pydantic's core is written in Rust; and to develop with, because validation, docs, and editor completion come from the types."

- **Starlette:** routing, requests and responses, middleware, WebSockets, background tasks, `TestClient`.
- **Pydantic v2:** request parsing and validation, response filtering and serialization, JSON Schema for OpenAPI.
- **Dependency injection:** `Depends()` for sessions, auth, pagination, settings; the same graph appears in the OpenAPI security schemes.
- **Server:** uvicorn (or another ASGI server: Hypercorn, Granian, gunicorn 26's `asgi` worker).

**Honest caveat:** "fast" means high concurrency for I/O-bound work.
A CPU-bound endpoint is no faster than in Flask, and a blocking call in an `async def` endpoint makes it much slower (A2).

---

## A2. What is the difference between `async def` and `def` endpoints? What happens if you block inside `async def`? (must know)

> "An `async def` endpoint runs directly on the event loop, so it must only await non-blocking I/O.
> A plain `def` endpoint (and a plain `def` dependency) is run in Starlette's threadpool via `anyio.to_thread.run_sync`, so blocking calls there are fine; the default pool has 40 threads per process.
> If you call something blocking inside `async def` (`time.sleep`, `requests.get`, a sync DB driver, heavy CPU work), the whole event loop stalls and every other request in that process waits."

Measured with five concurrent requests to each endpoint:

```python
blocking_app = FastAPI()


@blocking_app.get("/async-blocking")
async def async_blocking():
    time.sleep(0.2)  # BUG: blocks the event loop; every other request waits
    return {"ok": True}


@blocking_app.get("/sync-blocking")
def sync_blocking():
    time.sleep(0.2)  # fine: plain def runs in the threadpool
    return {"ok": True}


@blocking_app.get("/async-nonblocking")
async def async_nonblocking():
    await asyncio.sleep(0.2)  # awaits yield control back to the loop
    return {"ok": True}


@blocking_app.get("/async-offloaded")
async def async_offloaded():
    await run_in_threadpool(time.sleep, 0.2)  # legacy blocking call pushed to a thread
    return {"ok": True}


async def elapsed_for_concurrent_calls(path: str, n: int = 5) -> float:
    transport = httpx.ASGITransport(app=blocking_app)
    async with httpx.AsyncClient(transport=transport, base_url="http://test") as client:
        start = time.perf_counter()
        await asyncio.gather(*(client.get(path) for _ in range(n)))
        return time.perf_counter() - start


@pytest.mark.anyio
async def test_blocking_call_in_async_def_serializes_requests():
    assert await elapsed_for_concurrent_calls("/async-blocking") >= 1.0  # 5 x 0.2 s, one at a time
    assert await elapsed_for_concurrent_calls("/sync-blocking") < 0.6
    assert await elapsed_for_concurrent_calls("/async-nonblocking") < 0.6
    assert await elapsed_for_concurrent_calls("/async-offloaded") < 0.6
```

**Rules I follow:**

| The endpoint calls | Declare it as | Why |
| --- | --- | --- |
| Async libraries only (`httpx.AsyncClient`, `asyncpg`, SQLAlchemy async, `aioboto3`) | `async def` | No thread overhead, high concurrency |
| Blocking libraries (`requests`, sync SQLAlchemy, `boto3`, file I/O) | `def` | Runs in the threadpool |
| Mostly async plus one blocking call | `async def` with `await run_in_threadpool(fn, ...)` or `asyncio.to_thread` | Offload only the blocking part |
| CPU-heavy work (pandas, crypto, image processing) | Offload to a process pool or a worker queue | Threads do not escape the GIL on the default build |

**Follow-ups they ask:**

- "Is `def` slower?"
  Slightly per request (a thread handoff), and concurrency is capped by the threadpool size; under load, requests queue for a thread.
  The limit can be raised through AnyIO's default thread limiter, but the database pool is usually the real cap.
- "How do you find a blocked loop in production?"
  Latency rises for every endpoint at once while CPU looks low; enable asyncio debug mode (`PYTHONASYNCIODEBUG=1` logs callbacks slower than 100 ms) or take a py-spy dump of the worker.
- "Does `async` make a single request faster?"
  No; it lets one process serve many waiting requests.
  Use `asyncio.gather` inside a request to make independent calls concurrent.

---

## A3. Flask vs FastAPI: compare them. (must know)

| Dimension | Flask 3.1 | FastAPI 0.142 |
| --- | --- | --- |
| Server interface | WSGI; one request per worker thread | ASGI; event loop plus threadpool for `def` |
| Async | `async def` views run via asgiref in a thread, the worker is still blocked (see [Flask F13](03-Flask.md)) | Native; thousands of concurrent waiting requests per process |
| Validation | Bring your own (marshmallow, Pydantic by hand) | Pydantic from type hints, automatic 422 |
| Serialization | `jsonify` and the app's JSON provider | `response_model` or return type, serialized by Pydantic |
| OpenAPI and docs | Extension (Flask-Smorest, Flask-RESTX) | Built in: `/docs`, `/redoc`, `/openapi.json` |
| Request data access | Context-local globals (`request`, `g`, `current_app`) | Explicit parameters and `Depends()` |
| Dependency injection | None built in; app factory plus extensions | `Depends`, cached per request, overridable in tests |
| Error format | Whatever your handlers return | `{"detail": ...}`, 422 lists Pydantic errors |
| Hooks | `before_request`, `after_request`, teardown | Middleware, dependencies, yield dependencies |
| WebSockets and SSE | Extensions (Flask-SocketIO, Flask-Sock) | Built in |
| Production server | gunicorn (`sync`, `gthread`, `gevent`) | uvicorn workers, `fastapi run`, gunicorn with `uvicorn_worker` |
| Testing | `app.test_client()` | `TestClient`, `httpx.AsyncClient` with `ASGITransport` |
| Ecosystem maturity | Since 2010, huge extension ecosystem | Since 2018, fast-moving, 0.x version numbers |
| Best fit | Sync CRUD, legacy services, server-rendered HTML | New JSON APIs, I/O fan-out, typed contracts |

How to say it: "Both are good; I choose by workload and team.
For I/O-bound microservices with typed contracts I default to FastAPI; for an existing Flask estate I keep Flask, add Pydantic, and migrate incrementally only when there is a concrete payoff ([Flask F23](03-Flask.md))."

**Trick follow-up:** "Is FastAPI always faster?"
Only for concurrent I/O-bound load written with async libraries.
A FastAPI app with blocking calls in `async def` endpoints can be slower than the same app in Flask with `gthread` workers.

---

## A4. Build it live: an items CRUD API with validation, 404/409/422, pagination, and a DB session dependency (must know)

What to say while typing: "Separate Create, Update, and Read schemas; a yield dependency owns the session; a dependency loads the item or raises 404; the unique constraint decides 409; `Query` bounds the page size; the engine lives in `lifespan`."
The full file and its tests are in [code/fastapi_app](code/README.md); `pytest` there runs 11 tests.

```python
"""FastAPI reference service: an items CRUD API you can build in about 20 minutes.

Shows separate Create/Update/Read schemas, a yield dependency that owns the
SQLAlchemy session, lifespan-managed engine, 404/409/422 handling, bounded
pagination with Query validation, and a router.

Run it:  uvicorn fastapi_items:app --reload      (dev)
Serve:   uvicorn fastapi_items:app --workers 4   (or: fastapi run, gunicorn -k uvicorn_worker.UvicornWorker)
"""

from collections.abc import AsyncIterator, Iterator
from contextlib import asynccontextmanager
from typing import Annotated

from fastapi import APIRouter, Depends, FastAPI, HTTPException, Query, Request, Response, status
from pydantic import BaseModel, ConfigDict, Field
from sqlalchemy import String, create_engine, func, select
from sqlalchemy.engine import Engine
from sqlalchemy.exc import IntegrityError
from sqlalchemy.orm import DeclarativeBase, Mapped, Session, mapped_column, sessionmaker
from sqlalchemy.pool import StaticPool


# ---------- persistence ----------
class Base(DeclarativeBase):
    pass


class Item(Base):
    __tablename__ = "items"

    id: Mapped[int] = mapped_column(primary_key=True)
    sku: Mapped[str] = mapped_column(String(32), unique=True)
    name: Mapped[str] = mapped_column(String(100))
    price_cents: Mapped[int]
    quantity: Mapped[int] = mapped_column(default=0)


def make_engine(url: str) -> Engine:
    if url in ("sqlite://", "sqlite+pysqlite://") or url.endswith(":memory:"):
        # One shared in-memory connection that any threadpool thread may use.
        return create_engine(url, connect_args={"check_same_thread": False}, poolclass=StaticPool)
    return create_engine(url, pool_pre_ping=True, pool_size=10, max_overflow=5)


# ---------- schemas: never expose the ORM model directly ----------
class ItemCreate(BaseModel):
    model_config = ConfigDict(extra="forbid", str_strip_whitespace=True)

    sku: str = Field(min_length=1, max_length=32, examples=["SKU-1"])
    name: str = Field(min_length=1, max_length=100)
    price_cents: int = Field(ge=0)
    quantity: int = Field(default=0, ge=0)


class ItemUpdate(BaseModel):
    model_config = ConfigDict(extra="forbid", str_strip_whitespace=True)

    name: str | None = Field(default=None, min_length=1, max_length=100)
    price_cents: int | None = Field(default=None, ge=0)
    quantity: int | None = Field(default=None, ge=0)


class ItemRead(BaseModel):
    model_config = ConfigDict(from_attributes=True)  # lets FastAPI read ORM attributes

    id: int
    sku: str
    name: str
    price_cents: int
    quantity: int


class ItemPage(BaseModel):
    items: list[ItemRead]
    total: int
    limit: int
    offset: int


# ---------- dependencies ----------
def get_session(request: Request) -> Iterator[Session]:
    """One session per request; closed after the response (yield dependency)."""
    with request.app.state.sessionmaker() as session:
        yield session


SessionDep = Annotated[Session, Depends(get_session)]


def get_item_or_404(item_id: int, session: SessionDep) -> Item:
    item = session.get(Item, item_id)
    if item is None:
        raise HTTPException(status_code=404, detail=f"item {item_id} not found")
    return item


ItemDep = Annotated[Item, Depends(get_item_or_404)]


# ---------- routes (sync def: SQLAlchemy sync Session blocks, so run in the threadpool) ----------
router = APIRouter(prefix="/items", tags=["items"])


@router.post("", status_code=status.HTTP_201_CREATED, response_model=ItemRead)
def create_item(payload: ItemCreate, session: SessionDep, response: Response) -> Item:
    item = Item(**payload.model_dump())
    session.add(item)
    try:
        session.commit()  # the unique constraint is the source of truth, not a pre-check
    except IntegrityError:
        session.rollback()
        raise HTTPException(status_code=409, detail=f"sku {payload.sku!r} already exists")
    response.headers["Location"] = f"/items/{item.id}"
    return item


@router.get("", response_model=ItemPage)
def list_items(
    session: SessionDep,
    limit: Annotated[int, Query(ge=1, le=100)] = 20,
    offset: Annotated[int, Query(ge=0)] = 0,
) -> dict:
    total = session.scalar(select(func.count()).select_from(Item))
    rows = session.scalars(select(Item).order_by(Item.id).limit(limit).offset(offset)).all()
    return {"items": rows, "total": total, "limit": limit, "offset": offset}


@router.get("/{item_id}", response_model=ItemRead)
def get_item(item: ItemDep) -> Item:
    return item


@router.patch("/{item_id}", response_model=ItemRead)
def update_item(payload: ItemUpdate, item: ItemDep, session: SessionDep) -> Item:
    # ItemDep and SessionDep resolve get_session once per request (dependency cache),
    # so `item` belongs to this same `session`.
    for field, value in payload.model_dump(exclude_unset=True, exclude_none=True).items():
        setattr(item, field, value)
    session.commit()
    return item


@router.delete("/{item_id}", status_code=status.HTTP_204_NO_CONTENT)
def delete_item(item: ItemDep, session: SessionDep) -> None:
    session.delete(item)
    session.commit()


# ---------- app factory with lifespan ----------
def create_app(database_url: str = "sqlite+pysqlite:///:memory:") -> FastAPI:
    @asynccontextmanager
    async def lifespan(app: FastAPI) -> AsyncIterator[None]:
        engine = make_engine(database_url)
        Base.metadata.create_all(engine)  # demo only; use Alembic migrations in real services
        app.state.sessionmaker = sessionmaker(engine, expire_on_commit=False)
        yield
        engine.dispose()  # return pooled connections on shutdown

    app = FastAPI(title="Items API", version="1.0.0", lifespan=lifespan)
    app.include_router(router)

    @app.get("/healthz")
    def healthz() -> dict[str, str]:
        return {"status": "ok"}

    return app


app = create_app()
```

Tests, including the dependency override interviewers ask about:

```python
@pytest.fixture
def client():
    app = create_app("sqlite+pysqlite:///:memory:")
    with TestClient(app) as c:  # the `with` block runs lifespan startup and shutdown
        yield c


def make_item(client, **overrides):
    body = {"sku": "SKU-1", "name": "Widget", "price_cents": 1999, "quantity": 5} | overrides
    return client.post("/items", json=body)
```

```python
def test_validation_error_is_422_with_pydantic_details(client):
    resp = make_item(client, price_cents=-5)
    assert resp.status_code == 422
    [error] = resp.json()["detail"]
    assert error["loc"] == ["body", "price_cents"]
    assert error["type"] == "greater_than_equal"
```

```python
def test_not_found(client):
    resp = client.get("/items/999")
    assert resp.status_code == 404
    assert resp.json() == {"detail": "item 999 not found"}


def test_duplicate_sku_is_409(client):
    assert make_item(client).status_code == 201
    resp = make_item(client, name="Other")
    assert resp.status_code == 409
    assert "already exists" in resp.json()["detail"]
```

```python
def test_dependency_override_with_seeded_database():
    # Swap the session dependency for one bound to a database we control.
    engine = make_engine("sqlite://")
    Base.metadata.create_all(engine)
    TestSession = sessionmaker(engine, expire_on_commit=False)
    with TestSession() as s:
        s.add(Item(sku="SEEDED", name="From fixture", price_cents=1))
        s.commit()

    def override_get_session():
        with TestSession() as session:
            yield session

    app = create_app()
    app.dependency_overrides[get_session] = override_get_session
    try:
        with TestClient(app) as c:
            assert c.get("/items/1").json()["sku"] == "SEEDED"
    finally:
        app.dependency_overrides.clear()  # overrides are global to the app object


@pytest.fixture
def anyio_backend():
    return "asyncio"


@pytest.mark.anyio
async def test_async_client_with_asgi_transport():
    # ASGITransport does NOT send lifespan events, so provide the session by override.
    engine = make_engine("sqlite://")
    Base.metadata.create_all(engine)
    TestSession = sessionmaker(engine, expire_on_commit=False)

    def override_get_session():
        with TestSession() as session:
            yield session

    app = create_app()
    app.dependency_overrides[get_session] = override_get_session
    transport = httpx.ASGITransport(app=app)
    async with httpx.AsyncClient(transport=transport, base_url="http://test") as ac:
        created = await ac.post("/items", json={"sku": "A1", "name": "Async", "price_cents": 5})
        assert created.status_code == 201
        assert (await ac.get("/items/1")).json()["name"] == "Async"
```

**Talking points:**

- Endpoints are `def` because the SQLAlchemy `Session` is synchronous; they run in the threadpool (A2).
  With an async driver and `AsyncSession` they would be `async def` (A18).
- `get_item_or_404` is a dependency that depends on `SessionDep`; the dependency cache (A5) gives it the same session as the endpoint, so PATCH updates an object that belongs to the session it commits.
- `response_model=ItemRead` with `from_attributes=True` converts the ORM object and drops anything not in the schema.
- `extra="forbid"` turns a typo like `"prce_cents"` into a 422 instead of silently ignoring it.
- 422 comes free for wrong types, bounds, and missing fields; 409 and 404 are explicit `HTTPException`s.
- `TestClient` must be used as a context manager (`with TestClient(app) as c`) for `lifespan` to run; `httpx.ASGITransport` never runs it, which is why the async test overrides the session.

---

## A5. How does dependency injection work in FastAPI? (must know)

- A dependency is any callable (function, async function, class, or callable instance) declared with `Depends()`; FastAPI inspects its signature, resolves its own parameters (query, header, body, other dependencies), calls it, and passes the result in.
- Use `Annotated[T, Depends(fn)]` and name the alias (`SessionDep`, `CurrentUser`) so the dependency is declared once and reused.
- **Caching:** within one request each dependency runs once and its result is shared by everything that needs it; `Depends(fn, use_cache=False)` opts out.
  The cache is per request, not global.
- **Scope of application:** per parameter, per path operation (`dependencies=[...]` when you need the side effect but not the value), per router (`APIRouter(dependencies=[...])`), or app-wide (`FastAPI(dependencies=[...])`).
- `def` dependencies run in the threadpool, `async def` ones on the loop; the A2 rules apply.

```python
def test_dependency_cached_once_per_request():
    calls = {"n": 0}

    def get_settings() -> dict:
        calls["n"] += 1
        return {"env": "test"}

    def get_repo(settings: Annotated[dict, Depends(get_settings)]) -> str:
        return f"repo[{settings['env']}]"

    app = FastAPI()

    @app.get("/cached")
    def cached(settings: Annotated[dict, Depends(get_settings)], repo: Annotated[str, Depends(get_repo)]):
        return {"repo": repo}

    @app.get("/uncached")
    def uncached(
        a: Annotated[dict, Depends(get_settings, use_cache=False)],
        b: Annotated[dict, Depends(get_settings, use_cache=False)],
    ):
        return {}

    client = TestClient(app)
    client.get("/cached")
    assert calls["n"] == 1  # shared by the endpoint and get_repo
    client.get("/cached")
    assert calls["n"] == 2  # cache is per request, not global
    client.get("/uncached")
    assert calls["n"] == 4
```

Parameterized dependency (configure once, called per request):

```python
class Paginator:
    """A parameterized dependency: configure once, FastAPI calls __call__ per request."""

    def __init__(self, max_limit: int):
        self.max_limit = max_limit

    def __call__(self, limit: int = 20, offset: int = 0) -> dict:
        if not 1 <= limit <= self.max_limit or offset < 0:
            raise HTTPException(status_code=422, detail=f"limit must be 1-{self.max_limit}, offset >= 0")
        return {"limit": limit, "offset": offset}


Page = Annotated[dict, Depends(Paginator(max_limit=50))]


def test_class_based_dependency():
    app = FastAPI()

    @app.get("/fills")
    def list_fills(page: Page):
        return page

    client = TestClient(app)
    assert client.get("/fills", params={"limit": 10}).json() == {"limit": 10, "offset": 0}
    assert client.get("/fills", params={"limit": 500}).status_code == 422
```

Router-level guard:

```python
def test_router_level_dependency_guards_every_route():
    def require_api_key(x_api_key: Annotated[str | None, Header()] = None) -> None:
        if x_api_key != "secret":
            raise HTTPException(status_code=401, detail="missing or bad API key")

    admin = APIRouter(prefix="/admin", dependencies=[Depends(require_api_key)])

    @admin.get("/stats")
    def stats():
        return {"items": 3}

    app = FastAPI()
    app.include_router(admin)
    client = TestClient(app)
    assert client.get("/admin/stats").status_code == 401
    assert client.get("/admin/stats", headers={"X-API-Key": "secret"}).json() == {"items": 3}
```

**Why it matters:** dependencies make cross-cutting concerns (auth, sessions, tenancy, pagination) explicit in the signature, visible in OpenAPI, and replaceable in tests with `app.dependency_overrides` (A10).

---

## A6. How do yield dependencies work for DB sessions, and when does the cleanup run? (must know)

> "Code before `yield` runs before the endpoint; the yielded value is injected; code after `yield` runs as cleanup.
> Exceptions raised in the endpoint are re-raised at the `yield`, so a `try/except/finally` around it can roll back and close.
> In FastAPI 0.142 a yield dependency's exit code runs after the response has been sent by default (`scope="request"`), and `Depends(..., scope="function")` makes it run right after the endpoint returns, before the response is sent."

```python
def test_yield_dependency_sees_endpoint_exceptions_for_rollback():
    events: list[str] = []

    class FakeSession:
        def commit(self):
            events.append("commit")

        def rollback(self):
            events.append("rollback")

        def close(self):
            events.append("close")

    def get_tx() -> Iterator[FakeSession]:
        session = FakeSession()
        try:
            yield session
            session.commit()
        except Exception:
            session.rollback()
            raise  # re-raise, or FastAPI cannot produce the right error response
        finally:
            session.close()

    app = FastAPI()

    @app.post("/transfer")
    def transfer(amount: int, tx: Annotated[FakeSession, Depends(get_tx)]):
        if amount <= 0:
            raise HTTPException(status_code=400, detail="amount must be positive")
        return {"ok": True}

    client = TestClient(app)
    assert client.post("/transfer", params={"amount": 5}).status_code == 200
    assert events == ["commit", "close"]
    events.clear()
    assert client.post("/transfer", params={"amount": -1}).status_code == 400
    assert events == ["rollback", "close"]
```

The two scopes, observed with a streaming response:

```python
def make_scope_app(events: list[str]) -> FastAPI:
    def get_resource() -> Iterator[str]:
        events.append("open")
        try:
            yield "resource"
        finally:
            events.append("close")

    app = FastAPI()

    def body():
        events.append("stream")
        yield b"data"

    @app.get("/request-scope")  # default for yield dependencies
    def request_scope(res: Annotated[str, Depends(get_resource)]):
        return StreamingResponse(body())

    @app.get("/function-scope")
    def function_scope(res: Annotated[str, Depends(get_resource, scope="function")]):
        return StreamingResponse(body())

    return app


def test_yield_dependency_default_scope_outlives_streaming_response():
    events: list[str] = []
    TestClient(make_scope_app(events)).get("/request-scope")
    assert events == ["open", "stream", "close"]


def test_yield_dependency_function_scope_closes_before_response():
    events: list[str] = []
    TestClient(make_scope_app(events)).get("/function-scope")
    assert events == ["open", "close", "stream"]
```

- Default (`"request"`): the session stays open while a `StreamingResponse` is still reading from it; the right choice for DB sessions used by streaming endpoints.
- `"function"`: releases a connection or lock as soon as the endpoint returns, before a slow client finishes downloading.
- A dependency with scope `"request"` cannot depend on one with scope `"function"`; FastAPI raises at startup.

**Trick question:** "What happens if the yield dependency catches the exception and does not re-raise?"
The response is lost: even an intended 400 becomes a 500, and `TestClient` raises a `FastAPIError` pointing at the bare `except`.

```python
def test_yield_dependency_that_swallows_exceptions_turns_400_into_500():
    def get_swallowing() -> Iterator[int]:
        try:
            yield 1
        except Exception:
            pass  # BUG: no re-raise

    app = FastAPI()

    @app.get("/check")
    def check(dep: Annotated[int, Depends(get_swallowing)]):
        raise HTTPException(status_code=400, detail="bad input")

    resp = TestClient(app, raise_server_exceptions=False).get("/check")
    assert resp.status_code == 500  # the intended 400 is lost
```

**Version note:** this behavior has changed more than once (0.106 moved the exit code before the response was sent; later releases moved the default back to after the response and added `scope`).
State the version you have verified, and check the release notes for the version the team runs.

---

## A7. How do you use Pydantic v2 models for request and response schemas?

- Separate schemas per direction: `ItemCreate` (client input, no `id`), `ItemUpdate` (all fields optional for PATCH), `ItemRead` (what you return, `from_attributes=True` to read ORM objects).
  One model for everything leaks internal fields and makes `id` writable.
- `model_config = ConfigDict(...)`: `extra="forbid"` (reject unknown fields), `str_strip_whitespace=True`, `from_attributes=True`, `frozen=True`, and `alias_generator=to_camel` with `validate_by_name=True` for camelCase APIs (`populate_by_name` is discouraged since Pydantic 2.11).
- Constraints with `Field(gt=0, max_length=12, pattern=...)`, or reusable `Annotated[int, Field(gt=0)]` aliases.
- `@field_validator` for one field (normalize or reject), `@model_validator(mode="after")` for cross-field rules.
- Partial update: `payload.model_dump(exclude_unset=True)` distinguishes "not sent" from "sent as null".
- v1 to v2 renames to know: `.dict()` to `.model_dump()`, `.json()` to `.model_dump_json()`, `parse_obj` to `model_validate`, `orm_mode` to `from_attributes`, `@validator` to `@field_validator`, `class Config` to `model_config`.

```python
class OrderIn(BaseModel):
    model_config = ConfigDict(extra="forbid", str_strip_whitespace=True)

    symbol: str = Field(min_length=1, max_length=12)
    side: str
    qty: int = Field(gt=0)
    limit_price: float | None = Field(default=None, gt=0)
    order_type: str = "market"

    @field_validator("symbol")
    @classmethod
    def upper_symbol(cls, v: str) -> str:
        return v.upper()

    @field_validator("side")
    @classmethod
    def known_side(cls, v: str) -> str:
        if v not in {"buy", "sell"}:
            raise ValueError("side must be 'buy' or 'sell'")
        return v

    @model_validator(mode="after")
    def limit_needs_price(self) -> "OrderIn":
        if self.order_type == "limit" and self.limit_price is None:
            raise ValueError("limit orders need limit_price")
        return self


def make_orders_app() -> FastAPI:
    app = FastAPI()

    @app.post("/orders", status_code=201)
    def create_order(order: OrderIn) -> OrderIn:
        return order

    return app
```

```python
def test_validators_normalise_and_cross_check():
    client = TestClient(make_orders_app())
    ok = client.post("/orders", json={"symbol": " aapl ", "side": "buy", "qty": 10})
    assert ok.status_code == 201
    assert ok.json()["symbol"] == "AAPL"
    bad_side = client.post("/orders", json={"symbol": "AAPL", "side": "hold", "qty": 1})
    assert bad_side.json()["detail"][0]["msg"] == "Value error, side must be 'buy' or 'sell'"
    no_price = client.post("/orders", json={"symbol": "AAPL", "side": "buy", "qty": 1, "order_type": "limit"})
    assert no_price.status_code == 422
    assert no_price.json()["detail"][0]["loc"] == ["body"]
```

**Pitfalls:** `float` for money (use `Decimal` or integer cents, as the reference service does); mutable defaults (use `Field(default_factory=list)`); validators that do I/O (keep them pure, check the database in the service layer).

---

## A8. ASGI vs WSGI: what is the difference?

| | WSGI (PEP 3333) | ASGI |
| --- | --- | --- |
| Call shape | `app(environ, start_response)` returns an iterable | `await app(scope, receive, send)` |
| Concurrency | One request per thread or process for its whole life | Many requests interleaved on one event loop |
| Protocols | HTTP request and response only | HTTP, WebSocket, lifespan events, streaming both ways |
| Long-lived connections | Tie up a worker each | Cheap while idle |
| Servers | gunicorn, uWSGI, mod_wsgi | uvicorn, Hypercorn, Granian, gunicorn 26 `asgi` worker |
| Frameworks | Flask, Django (classic) | FastAPI, Starlette, Django (async), Quart |

- ASGI's `lifespan` messages are what `FastAPI(lifespan=...)` hooks into (A14).
- Bridging: WSGI apps run inside ASGI through an adapter (`a2wsgi`), each request in a thread; see [Flask F23](03-Flask.md).
- ASGI middleware is a callable wrapping `(scope, receive, send)`; see A13.

---

## A9. How do you handle errors, and what does a 422 look like? (must know)

- Raise `HTTPException(status_code, detail, headers=...)` for expected HTTP errors; `detail` can be any JSON-serializable value.
- Register `@app.exception_handler(DomainError)` so service code raises domain exceptions and the status mapping lives in one place.
- Request validation failures raise `RequestValidationError`, answered with 422 and a list of Pydantic errors; override its handler to match your API's error envelope.
- To catch framework-level 404 and 405 as well as your own `HTTPException`s, register the handler for Starlette's `HTTPException`, which FastAPI's subclasses.
- Unhandled exceptions become a plain 500; log them in middleware or a catch-all handler with the request ID, and never return the traceback.

The default 422 body, exactly:

```python
def test_422_shape_is_a_list_of_pydantic_errors():
    client = TestClient(make_orders_app())
    resp = client.post("/orders", json={"side": "buy", "qty": 0})
    assert resp.status_code == 422
    assert resp.json() == {
        "detail": [
            {
                "type": "missing",
                "loc": ["body", "symbol"],
                "msg": "Field required",
                "input": {"side": "buy", "qty": 0},
            },
            {
                "type": "greater_than",
                "loc": ["body", "qty"],
                "msg": "Input should be greater than 0",
                "input": 0,
                "ctx": {"gt": 0},
            },
        ]
    }
```

Custom handlers:

```python
class InsufficientInventory(Exception):
    def __init__(self, sku: str, requested: int, available: int):
        self.sku, self.requested, self.available = sku, requested, available


def install_error_handlers(app: FastAPI) -> None:
    @app.exception_handler(InsufficientInventory)
    async def insufficient_inventory(request: Request, exc: InsufficientInventory):
        return JSONResponse(
            status_code=409,
            content={"error": {"code": "insufficient_inventory", "sku": exc.sku, "available": exc.available}},
        )

    @app.exception_handler(RequestValidationError)
    async def validation_envelope(request: Request, exc: RequestValidationError):
        fields = [{"field": ".".join(map(str, e["loc"][1:])), "message": e["msg"]} for e in exc.errors()]
        return JSONResponse(status_code=422, content={"error": {"code": "validation_failed", "fields": fields}})


def test_custom_exception_handlers():
    app = FastAPI()
    install_error_handlers(app)

    @app.post("/reserve")
    def reserve(sku: str, qty: int):
        raise InsufficientInventory(sku, qty, available=2)

    client = TestClient(app)
    assert client.post("/reserve", params={"sku": "A", "qty": 5}).json() == {
        "error": {"code": "insufficient_inventory", "sku": "A", "available": 2}
    }
    assert client.post("/reserve", params={"sku": "A", "qty": "five"}).json() == {
        "error": {"code": "validation_failed", "fields": [{"field": "qty", "message": "Input should be a valid integer, unable to parse string as an integer"}]}
    }
```

- `loc` tells the client where the problem is: `body`, `query`, `path`, `header`, or `cookie`, then the field path.
- Starlette 1.7 names the constant `status.HTTP_422_UNPROCESSABLE_CONTENT`; the old `HTTP_422_UNPROCESSABLE_ENTITY` still resolves but is deprecated.
- Returning data that does not match `response_model` is a server bug and raises `ResponseValidationError` (a 500), not a 422.

---

## A10. How do you test a FastAPI app, including overriding dependencies? (must know)

- `TestClient(app)` runs the app in-process through httpx; use it as a context manager so `lifespan` runs.
- `app.dependency_overrides[real_dependency] = fake` swaps any dependency (DB session, current user, external client) without patching; clear it afterwards because it is global to the app object.
- For async tests (an async fixture, or awaiting async code between calls), use `httpx.AsyncClient(transport=httpx.ASGITransport(app=app), base_url="http://test")` with `pytest.mark.anyio` or pytest-asyncio.
  `ASGITransport` does not send lifespan events; drive `lifespan` yourself or override the dependencies that need it.
- Database: one transaction per test rolled back at the end, or a fresh database per test (SQLite in memory with `StaticPool` for speed, PostgreSQL in a container for fidelity).

```python
def make_lifespan_app(created: list) -> FastAPI:
    @asynccontextmanager
    async def lifespan(app: FastAPI) -> AsyncIterator[dict]:
        pricing = FakePricingClient()  # e.g. httpx.AsyncClient, DB engine, Kafka producer
        created.append(pricing)
        yield {"pricing": pricing}  # lifespan state, copied into each request's state
        await pricing.aclose()

    app = FastAPI(lifespan=lifespan)

    @app.get("/quote/{symbol}")
    async def quote(symbol: str, request: Request):
        return {"symbol": symbol, "price": await request.state.pricing.quote(symbol)}

    return app


def test_lifespan_creates_and_closes_shared_resources():
    created: list = []
    app = make_lifespan_app(created)
    with TestClient(app) as client:  # startup runs here
        assert client.get("/quote/AAPL").json() == {"symbol": "AAPL", "price": 101.5}
        assert created[0].closed is False
    assert created[0].closed is True  # shutdown ran on exit


@pytest.mark.anyio
async def test_asgi_transport_skips_lifespan():
    created: list = []
    app = make_lifespan_app(created)
    transport = httpx.ASGITransport(app=app, raise_app_exceptions=False)
    async with httpx.AsyncClient(transport=transport, base_url="http://test") as ac:
        resp = await ac.get("/quote/AAPL")
    assert created == []  # lifespan never ran
    assert resp.status_code == 500
```

Override pattern from the reference service:

```python
def test_dependency_override_with_seeded_database():
    # Swap the session dependency for one bound to a database we control.
    engine = make_engine("sqlite://")
    Base.metadata.create_all(engine)
    TestSession = sessionmaker(engine, expire_on_commit=False)
    with TestSession() as s:
        s.add(Item(sku="SEEDED", name="From fixture", price_cents=1))
        s.commit()

    def override_get_session():
        with TestSession() as session:
            yield session

    app = create_app()
    app.dependency_overrides[get_session] = override_get_session
    try:
        with TestClient(app) as c:
            assert c.get("/items/1").json()["sku"] == "SEEDED"
    finally:
        app.dependency_overrides.clear()  # overrides are global to the app object
```

- Starlette 1.7's `TestClient` prefers the `httpx2` package and emits a `StarletteDeprecationWarning` when it falls back to `httpx`; install `httpx2` in the test dependencies to silence it.
- Override an auth dependency to return a fixed user instead of minting tokens in every test, and keep one test that goes through the real token path (A17).

More: [Testing, Debugging, Production](10-Testing-Debugging-Production.md).

---

## A11. What does `response_model` do?

- Validates the return value against the schema, filters it to the declared fields, serializes it, and documents it in OpenAPI.
- It is how you avoid leaking internal fields: return the internal object, declare the public model.
- If `response_model` is not set, the return type annotation is used the same way; set `response_model` explicitly when the function returns an ORM object or a different internal type.
- `response_model_exclude_unset=True` omits fields the object never set (useful for sparse PATCH responses).
- FastAPI 0.142 serializes to JSON bytes through Pydantic when a response model or return type is declared, which is why `ORJSONResponse` is now deprecated.

```python
class UserInDB(BaseModel):
    id: int
    email: str
    password_hash: str


class UserPublic(BaseModel):
    id: int
    email: str


def test_response_model_strips_private_fields():
    app = FastAPI()

    @app.get("/users/{user_id}", response_model=UserPublic)
    def get_user(user_id: int) -> UserInDB:
        return UserInDB(id=user_id, email="a@x.io", password_hash="argon2id$...")

    assert TestClient(app).get("/users/1").json() == {"id": 1, "email": "a@x.io"}
```

---

## A12. How do path, query, body, and header parameters work, and how are they validated?

- Declared in the path template (`/items/{item_id}`): path parameter.
- A scalar parameter not in the path: query parameter; `Annotated[int, Query(ge=1, le=100)] = 20` adds bounds and docs.
- A Pydantic model parameter: JSON body; several models become one body keyed by parameter name; `Body(embed=True)` wraps a single one.
- `Header()` converts `x_api_key` to the `X-API-Key` header; `Cookie()`, `Form()`, and `File()`/`UploadFile` (needs `python-multipart`) cover the rest.
- Wrong types, missing required values, and constraint violations all produce 422 with `loc` pointing at the source.
- A body is parsed as JSON only when the `Content-Type` is `application/json` or `+json`.
  FastAPI 0.142 also refuses to guess when the header is missing (`strict_content_type=True` by default, a CSRF defense), so the request gets a 422; pass `strict_content_type=False` to the app or router for clients that cannot be fixed.

```python
def test_json_body_requires_json_content_type():
    client = TestClient(make_orders_app())
    raw = json.dumps({"symbol": "AAPL", "side": "buy", "qty": 1})
    resp = client.post("/orders", content=raw)  # no Content-Type header
    assert resp.status_code == 422
    ok = client.post("/orders", content=raw, headers={"Content-Type": "application/json"})
    assert ok.status_code == 201
```

---

## A13. Middleware vs dependencies: when do you use each? How do you add CORS and a request-ID?

| Use middleware for | Use dependencies for |
| --- | --- |
| Everything, including 404s and static files | Specific routes or routers |
| Raw request and response: headers, timing, compression, CORS, request IDs | Values the endpoint needs: session, current user, pagination |
| Cross-cutting behavior that must not appear in OpenAPI | Auth that should appear in OpenAPI security schemes |

- CORS: `app.add_middleware(CORSMiddleware, allow_origins=[...], allow_methods=[...], allow_headers=[...], allow_credentials=True)`; never `allow_origins=["*"]` with credentials.
- Order: the middleware added last is the outermost and runs first on the way in.
- `@app.middleware("http")` (Starlette's `BaseHTTPMiddleware`) is convenient, but it wraps the response stream in an extra task and memory channel; for hot paths and streaming, write pure ASGI middleware.

Pure ASGI request-ID and timing middleware:

```python
class RequestIdMiddleware:
    """Pure ASGI middleware: no BaseHTTPMiddleware overhead, safe with streaming."""

    def __init__(self, app):
        self.app = app

    async def __call__(self, scope, receive, send):
        if scope["type"] != "http":
            return await self.app(scope, receive, send)
        headers = dict(scope["headers"])
        request_id = headers.get(b"x-request-id", uuid.uuid4().hex.encode())
        scope.setdefault("state", {})["request_id"] = request_id.decode()
        start = time.perf_counter()

        async def send_wrapper(message):
            if message["type"] == "http.response.start":
                elapsed_ms = (time.perf_counter() - start) * 1000
                message.setdefault("headers", [])
                message["headers"] += [
                    (b"x-request-id", request_id),
                    (b"server-timing", f"app;dur={elapsed_ms:.1f}".encode()),
                ]
            await send(message)

        await self.app(scope, receive, send_wrapper)


def test_pure_asgi_request_id_middleware():
    app = FastAPI()
    app.add_middleware(RequestIdMiddleware)

    @app.get("/ping")
    def ping(request: Request):
        return {"request_id": request.state.request_id}

    resp = TestClient(app).get("/ping", headers={"X-Request-ID": "req-42"})
    assert resp.headers["x-request-id"] == "req-42"
    assert resp.json() == {"request_id": "req-42"}
    assert resp.headers["server-timing"].startswith("app;dur=")
```

Decorator middleware and ordering:

```python
def test_decorator_middleware_and_ordering():
    order: list[str] = []
    app = FastAPI()

    @app.middleware("http")
    async def inner(request: Request, call_next):
        order.append("inner")
        return await call_next(request)

    @app.middleware("http")
    async def outer(request: Request, call_next):  # added last = runs first
        order.append("outer")
        response = await call_next(request)
        response.headers["X-Process-Time"] = "measured"
        return response

    @app.get("/")
    def root():
        return {}

    assert TestClient(app).get("/").headers["x-process-time"] == "measured"
    assert order == ["outer", "inner"]
```

---

## A14. What is `lifespan`, and what goes in it?

- An async context manager passed to `FastAPI(lifespan=...)`: code before `yield` runs once per worker process at startup, code after `yield` at shutdown.
- Put process-wide resources there: the DB engine and pool, an `httpx.AsyncClient`, a Kafka producer, ML models, caches.
- Yield a dict to expose them as lifespan state (`request.state.<name>`), or set `app.state`.
- It replaces the deprecated `@app.on_event("startup")` and `"shutdown"`.
- Each worker process runs its own lifespan; do not run migrations there when you have several workers (race), run them as a separate deploy step.
- If startup raises, the worker exits instead of serving broken requests; that is what you want behind a readiness probe.

The lifespan used by the tests in A10 (`make_lifespan_app`) shows the shape, and the reference service in A4 creates and disposes its engine there.

---

## A15. How do you structure a larger FastAPI project?

```text
app/
  main.py            # create_app(): FastAPI(lifespan=...), include routers, middleware, handlers
  core/config.py     # Settings(BaseSettings) from pydantic-settings, cached with lru_cache
  core/security.py   # token decode, get_current_user dependency
  db/session.py      # engine, sessionmaker, get_session dependency
  items/
    router.py        # APIRouter(prefix="/items", tags=["items"])
    schemas.py       # ItemCreate, ItemUpdate, ItemRead
    service.py       # business rules, no FastAPI imports
    repository.py    # SQLAlchemy queries
  orders/...
tests/
  conftest.py        # app, client, db fixtures, dependency overrides
```

- `APIRouter` groups routes with a prefix, tags, shared `dependencies`, and shared `responses`; `app.include_router(router, prefix="/api/v1")` composes them, and versioning is a second include.
- Keep business logic out of route functions so it can be tested without HTTP and reused from workers and CLIs.
- Settings: a `pydantic-settings` `BaseSettings` class read from environment variables, exposed through a cached dependency so tests can override it.

---

## A16. `BackgroundTasks` vs Celery or a message queue: when do you use each?

- `BackgroundTasks` runs a function after the response is sent, in the same process: fine for small fire-and-forget work (send an email, write an audit log).
- It has no retries, no persistence, and no visibility; if the process restarts or the task raises, the work is lost, and a slow task holds the worker.
- Use a queue (Celery or RQ on Redis or RabbitMQ, Kafka, SQS) for anything that must happen, needs retries, takes more than a few seconds, or should scale separately.
- Pattern: write the job to the database and the queue (or an outbox table) in the request, return `202 Accepted` with a status URL, let a worker process it.

```python
def test_background_task_runs_after_response_is_built():
    events: list[str] = []

    def send_confirmation(order_id: int) -> None:
        events.append(f"email:{order_id}")

    app = FastAPI()

    @app.post("/orders/{order_id}/confirm", status_code=202)
    def confirm(order_id: int, background: BackgroundTasks):
        background.add_task(send_confirmation, order_id)
        events.append("handler-returned")
        return {"status": "accepted"}

    assert TestClient(app).post("/orders/7/confirm").status_code == 202
    assert events == ["handler-returned", "email:7"]
```

Queues, outbox, and idempotency: [Microservices and Messaging](08-Microservices-and-Messaging.md).

---

## A17. How do you secure a FastAPI endpoint with OAuth2 and JWT scopes?

- `OAuth2PasswordBearer(tokenUrl=...)` extracts the bearer token (401 with `WWW-Authenticate: Bearer` if missing) and adds the security scheme to OpenAPI; it does not validate anything.
- A `get_current_user` dependency decodes and verifies the JWT: signature, `exp`, `aud`, `iss`, and an explicit `algorithms=[...]` list.
- `Security(dep, scopes=[...])` is `Depends` plus required scopes; the dependency receives them through `SecurityScopes` and returns 403 when the token lacks one.
- 401 means "who are you?" (missing or invalid credentials), 403 means "I know you, and you may not".

```python
oauth2_scheme = OAuth2PasswordBearer(
    tokenUrl="token", scopes={"orders:read": "Read orders", "orders:write": "Place orders"}
)


def get_current_user(security_scopes: SecurityScopes, token: Annotated[str, Depends(oauth2_scheme)]) -> dict:
    challenge = f'Bearer scope="{security_scopes.scope_str}"' if security_scopes.scopes else "Bearer"
    try:
        claims = jwt.decode(token, SECRET, algorithms=["HS256"], audience="items-api")
    except jwt.InvalidTokenError:
        raise HTTPException(status_code=401, detail="invalid token", headers={"WWW-Authenticate": challenge})
    granted = set(claims.get("scope", "").split())
    if missing := [s for s in security_scopes.scopes if s not in granted]:
        raise HTTPException(status_code=403, detail=f"missing scopes: {missing}", headers={"WWW-Authenticate": challenge})
    return claims


def make_secure_app() -> FastAPI:
    app = FastAPI()

    @app.get("/orders")
    def list_orders(user: Annotated[dict, Security(get_current_user, scopes=["orders:read"])]):
        return {"user": user["sub"], "orders": []}

    @app.post("/orders")
    def place_order(user: Annotated[dict, Security(get_current_user, scopes=["orders:write"])]):
        return {"placed_by": user["sub"]}

    return app


def token(scope: str, minutes: int = 5) -> str:
    now = datetime.now(UTC)
    claims = {"sub": "alice", "aud": "items-api", "scope": scope, "iat": now, "exp": now + timedelta(minutes=minutes)}
    return jwt.encode(claims, SECRET, algorithm="HS256")


def test_oauth2_bearer_scopes():
    client = TestClient(make_secure_app())
    missing = client.get("/orders")
    assert missing.status_code == 401
    assert missing.headers["www-authenticate"] == "Bearer"

    read = {"Authorization": f"Bearer {token('orders:read')}"}
    assert client.get("/orders", headers=read).json() == {"user": "alice", "orders": []}
    forbidden = client.post("/orders", headers=read)
    assert forbidden.status_code == 403

    expired = {"Authorization": f"Bearer {token('orders:read', minutes=-1)}"}
    assert client.get("/orders", headers=expired).status_code == 401
```

- In production verify RS256 or ES256 tokens against the identity provider's JWKS instead of a shared HS256 secret, and keep secrets out of code.
- Token lifetimes, refresh, OIDC, and API security: [Auth and Security](07-Auth-and-Security.md).

---

## A18. How do you use async SQLAlchemy with FastAPI, and what goes wrong?

- `create_async_engine("postgresql+asyncpg://...")`, `async_sessionmaker(engine, expire_on_commit=False)`, and an `async def` yield dependency that opens an `AsyncSession` per request.
- `expire_on_commit=False` matters: after commit, expired attributes would need a lazy refresh, which async sessions cannot do implicitly.
- Relationships must be loaded eagerly (`selectinload`, `joinedload`) or awaited explicitly through the `AsyncAttrs` mixin (`await order.awaitable_attrs.lines`); a lazy load during serialization fails with `MissingGreenlet`.
- An `AsyncSession` must not be shared between concurrent tasks (no `asyncio.gather` over one session).

```python
async def get_session(request: Request) -> AsyncIterator[AsyncSession]:
    async with request.app.state.sessionmaker() as session:
        yield session
```

```python
@app.get("/orders/{order_id}", response_model=OrderRead)
async def get_order(order_id: int, session: SessionDep) -> Order:
    stmt = select(Order).where(Order.id == order_id).options(selectinload(Order.lines))  # eager load
    order = await session.scalar(stmt)
    if order is None:
        raise HTTPException(status_code=404, detail="order not found")
    return order


@app.get("/orders-lazy/{order_id}", response_model=OrderRead)
async def get_order_lazy(order_id: int, session: SessionDep) -> Order:
    return await session.get(Order, order_id)  # BUG: lines will lazy-load during serialization


def test_eager_loaded_relationship_serializes():
    with TestClient(app) as client:
        body = client.get("/orders/1").json()
    assert body == {"id": 1, "customer": "acme", "lines": [{"sku": "A", "qty": 2}, {"sku": "B", "qty": 1}]}


def test_lazy_load_in_async_session_fails_as_response_validation_error():
    with TestClient(app) as client:
        with pytest.raises(ResponseValidationError) as info:
            client.get("/orders-lazy/1")
    # Serialization touched order.lines; the implicit lazy-load IO cannot run in an async session.
    assert "MissingGreenlet" in str(info.value)
```

- The lazy-load bug surfaces as a `ResponseValidationError` (500) whose message contains `MissingGreenlet`, because Pydantic touched `order.lines` while building the response.
- Sync SQLAlchemy in `def` endpoints is a legitimate choice: simpler, mature drivers, and the threadpool handles concurrency up to the pool size.
- Size the pool per process and multiply by workers and pods, as in [Flask F9](03-Flask.md); use PgBouncer in transaction mode carefully with asyncpg (disable its prepared statement cache).

More: [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md).

---

## A19. How do you run FastAPI in production?

> "Uvicorn is the server.
> On a VM I run several worker processes (`uvicorn app.main:app --workers 4` or `fastapi run --workers 4`), and uvicorn's supervisor restarts workers that die.
> In Kubernetes I usually run one worker per container and scale with replicas and the HPA, so the orchestrator handles restarts, memory limits, and rolling deploys.
> Gunicorn with uvicorn workers is still common when a team wants gunicorn's process management."

| Command | Notes |
| --- | --- |
| `uvicorn app.main:app --host 0.0.0.0 --port 8000 --workers 4` | Built-in multiprocess supervisor |
| `fastapi run app/main.py --workers 4` | Needs `pip install "fastapi[standard]"` (the CLI package); production defaults, wraps uvicorn |
| `gunicorn app.main:app -k uvicorn_worker.UvicornWorker -w 4` | The worker lives in the `uvicorn-worker` package; `uvicorn.workers.UvicornWorker` still imports but is deprecated |
| `gunicorn app.main:app -k asgi -w 4` | Gunicorn 26's own ASGI worker; newer, check the team's standard before using it |

- Worker count: CPU cores per container is a starting point for async apps (each process already multiplexes I/O); measure.
- Uvicorn trusts `X-Forwarded-For` and `X-Forwarded-Proto` only from `127.0.0.1` by default; set `--forwarded-allow-ips` to the load balancer's addresses so client IPs and the scheme are correct.
- `--timeout-graceful-shutdown` below Kubernetes' `terminationGracePeriodSeconds`; readiness probe on a cheap `/healthz`, liveness that does not depend on the database.
- Install `uvicorn[standard]` for uvloop and httptools.
- Disable or protect `/docs` and `/openapi.json` on internet-facing services if the schema is sensitive (`FastAPI(docs_url=None, openapi_url=None)`).

Containers and CI/CD: [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md).

---

## A20. How do you make a FastAPI service faster?

1. **Measure first:** p50, p99, and throughput per endpoint (Prometheus or OpenTelemetry), then profile (py-spy, `pyinstrument`).
2. **Never block the loop** (A2): async drivers in `async def`, or `def` endpoints for sync libraries.
3. **Connection reuse:** one `httpx.AsyncClient` and one engine per process, created in `lifespan`; a new client per request pays TCP and TLS setup every time.
4. **Database:** fix N+1 with eager loading, index the filter and sort columns, select only needed columns, keyset pagination for deep pages, bounded `limit`.
5. **Serialization:** declare `response_model` or a return type so Pydantic serializes directly to JSON bytes; `ORJSONResponse` and `UJSONResponse` are deprecated in 0.142 for that reason.
6. **Concurrency inside a request:** `asyncio.gather` independent downstream calls, with per-call timeouts.
7. **Caching:** HTTP caching headers and ETags, Redis for hot reads with explicit invalidation.
8. **Compression:** `GZipMiddleware` for large JSON, or let the proxy do it.
9. **Offload:** CPU-bound work to a process pool or a queue (A16).

**[fill in: one latency or throughput improvement you delivered: baseline, change, measured result, and how you measured it.]**

---

## A21. How do you stream responses, and how do WebSockets fit in?

- `StreamingResponse(iterator, media_type=...)` for large exports; a sync iterator runs in the threadpool, an async one on the loop.
- In FastAPI 0.142 a generator path operation with a declared item type streams JSON Lines (`application/jsonl`) automatically, and `response_class=EventSourceResponse` from `fastapi.sse` turns it into Server-Sent Events.
- WebSockets: `@app.websocket(path)`, `await ws.accept()`, receive and send loops, and `WebSocketDisconnect` handling; scale-out needs a shared pub/sub (Redis) because each connection lives in one process.
- Yield-dependency cleanup runs after the stream finishes by default (A6), so a DB session stays open for a streaming query.

```python
class Tick(BaseModel):
    symbol: str
    price: float


def test_generator_endpoint_streams_jsonl():
    app = FastAPI()

    @app.get("/ticks")
    async def ticks() -> AsyncIterator[Tick]:
        for i in range(3):
            yield Tick(symbol="AAPL", price=100 + i)

    resp = TestClient(app).get("/ticks")
    assert resp.headers["content-type"].startswith("application/jsonl")
    assert [json.loads(line)["price"] for line in resp.text.splitlines()] == [100, 101, 102]


def test_streaming_response_for_csv_export():
    app = FastAPI()

    @app.get("/export.csv")
    def export():
        def rows() -> Iterator[str]:
            yield "id,sku\n"
            for i in range(3):  # stream from a server-side cursor in real code
                yield f"{i},SKU-{i}\n"

        return StreamingResponse(rows(), media_type="text/csv")

    assert TestClient(app).get("/export.csv").text.count("\n") == 4


def test_websocket_echo():
    app = FastAPI()

    @app.websocket("/ws/prices")
    async def prices(ws: WebSocket):
        await ws.accept()
        symbol = await ws.receive_text()
        await ws.send_json({"symbol": symbol, "price": 101.5})
        await ws.close()

    with TestClient(app).websocket_connect("/ws/prices") as ws:
        ws.send_text("AAPL")
        assert ws.receive_json() == {"symbol": "AAPL", "price": 101.5}
```

---

## A22. What FastAPI production issues have you seen, and how did you fix them?

| Symptom | Root cause | Fix |
| --- | --- | --- |
| Every endpoint slow at once, CPU low | Blocking call in `async def` (sync SDK, `requests`, sync DB) | `def` endpoint, async library, or `run_in_threadpool` (A2) |
| Latency climbs under load with `def` endpoints | Threadpool (40) or DB pool exhausted; requests queue | Size pools together; async driver; more processes |
| `QueuePool limit ... reached` | Sessions not closed, or pool smaller than concurrency | Yield dependency with `finally: close()`; pool sizing; `pool_pre_ping` |
| Intended 4xx returned as 500 | Yield dependency swallowed the exception (A6) | Re-raise in `except` |
| 500 with `ResponseValidationError` | Response does not match `response_model`, or a lazy load in an async session (A18) | Fix the data or schema; eager load |
| Clients get 422 after a "harmless" change | Stricter validation, `extra="forbid"`, or the JSON content-type check (A12) | Contract tests; version the API |
| Lifespan resources missing in tests | `ASGITransport` or `TestClient` without `with` skipped lifespan | Context-manager `TestClient`, or overrides |
| Memory grows per worker | Per-request clients or unbounded caches | Create clients in `lifespan`; bounded caches |
| Wrong client IP or `http` URLs behind the proxy | Proxy headers not trusted from the load balancer's IP | `--forwarded-allow-ips=<LB addresses>` |

**[fill in: one FastAPI production incident you handled: symptom, how you found it, root cause, fix, and prevention.]**

---

## Go deeper

- [Flask](03-Flask.md) for the other half of the role, and [REST API Design](05-REST-API-Design.md), [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md), [Auth and Security](07-Auth-and-Security.md), [Microservices and Messaging](08-Microservices-and-Messaging.md), [Testing, Debugging, Production](10-Testing-Debugging-Production.md) in this pack.
- Reference service and tests: [code/README.md](code/README.md).
- [FastAPI and Pydantic: Type-Safe Web Development](../Python_Zero_to_Godhood/Chapter_67_FastAPI_and_Pydantic_Type-Safe_Web_Development.md).
- [Native Async/Await](../Python_Zero_to_Godhood/Volume_03_Generators_Iterators_and_Async_Inception/Chapter_11_Native_Async_Await_and_New_Operators/Chapter_11_Native_Async_Await_and_New_Operators.md) and [Asyncio Inception](../Python_Zero_to_Godhood/Volume_03_Generators_Iterators_and_Async_Inception/Chapter_10_Asyncio_Inception_Pathlib_and_Enum/Chapter_10_Asyncio_Inception_Pathlib_and_Enum.md) for the event loop behind A2.
- [Concurrency Mechanics: the Global Interpreter Lock](../Python_Zero_to_Godhood/Chapter_10_CONCURRENCY_MECHANICS__THE_GLOBAL_INTERPRETER_LOCK.md) and [CPU and I/O Bound System Concurrency](../Python_Zero_to_Godhood/Chapter_26_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md).
- [Rate Limiter design](../../../04-System-Design/02-Case-Studies/02-Rate-Limiter/design.md).
- Official: [FastAPI docs](https://fastapi.tiangolo.com/), [Concurrency and async/await](https://fastapi.tiangolo.com/async/), [Dependencies with yield](https://fastapi.tiangolo.com/tutorial/dependencies/dependencies-with-yield/), [Testing dependencies](https://fastapi.tiangolo.com/advanced/testing-dependencies/), [Server workers](https://fastapi.tiangolo.com/deployment/server-workers/), [Starlette middleware](https://www.starlette.io/middleware/), [ASGI spec](https://asgi.readthedocs.io/en/latest/), [Pydantic](https://docs.pydantic.dev/latest/), [SQLAlchemy asyncio](https://docs.sqlalchemy.org/en/20/orm/extensions/asyncio.html), [uvicorn-worker](https://github.com/Kludex/uvicorn-worker).
