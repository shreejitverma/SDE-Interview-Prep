---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://flask.palletsprojects.com/en/stable/, https://flask.palletsprojects.com/en/stable/appcontext/, https://flask.palletsprojects.com/en/stable/reqcontext/, https://flask.palletsprojects.com/en/stable/patterns/appfactories/, https://flask.palletsprojects.com/en/stable/config/, https://flask.palletsprojects.com/en/stable/errorhandling/, https://flask.palletsprojects.com/en/stable/async-await/, https://flask.palletsprojects.com/en/stable/testing/, https://flask.palletsprojects.com/en/stable/deploying/gunicorn/, https://docs.gunicorn.org/en/stable/design.html, https://docs.gunicorn.org/en/stable/settings.html, https://flask-sqlalchemy.readthedocs.io/en/stable/, https://github.com/abersheeran/a2wsgi]
---

# Flask

Flask for a role titled "Python Developer (Flask & FastAPI)": what it is, how a request flows, contexts, the app factory, databases, errors, deployment, testing, and moving to FastAPI.
F1, F3, F4, F5, F7, F9, F10, and F13 are near-certain; F2 is the live-coding exercise most likely to be asked.
Every code sample here was run on 2026-09-29 under Python 3.14.7 with Flask 3.1.3, Werkzeug 3.1.9, SQLAlchemy 2.1.1, and Pydantic 2.13.5 (plus Flask-SQLAlchemy 3.1.1, Flask-CORS 6.0.5, asgiref 3.12.1, a2wsgi 1.10.10, and gunicorn 26.2.0 where a question uses them): 29 pytest tests passed, and the gunicorn config in F10 booted the reference app and served requests.

---

## F1. What is Flask, and when would you choose it over FastAPI or Django? (must know)

> "Flask is a WSGI micro-framework: routing, a request and response object, sessions, and templating, built on Werkzeug and Jinja2.
> It deliberately leaves out an ORM, validation, auth, and API docs, so you pick extensions for those.
> I choose it for synchronous services and existing Flask codebases, when the team knows it, or when I need server-rendered pages with Jinja.
> For a new JSON API with heavy I/O fan-out I would pick FastAPI, and for a data-heavy product with an admin site and auth out of the box, Django."

| | Flask | FastAPI | Django |
| --- | --- | --- | --- |
| Interface | WSGI (sync) | ASGI (async-first) | WSGI and ASGI |
| Validation | Bring your own (marshmallow, Pydantic) | Pydantic built in | Forms, DRF serializers |
| OpenAPI docs | Extension (Flask-Smorest, Flask-RESTX) | Generated from type hints | DRF plus drf-spectacular |
| ORM | Bring your own (SQLAlchemy) | Bring your own | Django ORM built in |
| Concurrency model | One request per worker thread or process | Event loop plus threadpool for `def` | Mostly sync |
| Best at | Small to mid services, legacy APIs, HTML apps | High-concurrency I/O-bound APIs | Full products with admin and auth |

**Building blocks to name:** Werkzeug (WSGI toolkit: routing, `Request`/`Response`, dev server, debugger), Jinja2 (templates), itsdangerous (signed session cookies), Click (the `flask` CLI), Blinker (signals).

**Follow-ups they ask:**

- "Is Flask slow?"
  The framework overhead is rarely the bottleneck; database round trips and blocking I/O are.
  What limits Flask is that each in-flight request holds a worker thread or process, so high-latency fan-out needs many workers.
- "Can Flask do async?"
  Yes, `async def` views work, but each request still occupies a worker; see F13.

---

## F2. Build it live: an items CRUD API with validation, 404/409/422, pagination, and a DB session (must know)

What to say while typing: "App factory, one blueprint, a session per request closed in teardown, Pydantic for validation, one JSON error shape, the unique constraint decides 409, and bounded pagination."
The full file and its tests are in [code/flask_app](code/README.md); `pytest` there runs 10 tests.

```python
"""Flask reference service: an items CRUD API you can build in about 20 minutes.

Shows the app factory, a blueprint, a per-request SQLAlchemy session with
teardown, Pydantic validation (422), 404 and 409 handling, consistent JSON
errors, and offset pagination.

Run it:  flask --app flask_items run      (dev server only)
Serve:   gunicorn -w 4 'flask_items:create_app()'
"""

from __future__ import annotations

import logging
from typing import Any

from flask import Blueprint, Flask, abort, current_app, g, request, url_for
from pydantic import BaseModel, ConfigDict, Field, ValidationError
from sqlalchemy import String, create_engine, func, select
from sqlalchemy.exc import IntegrityError
from sqlalchemy.orm import DeclarativeBase, Mapped, Session, mapped_column, sessionmaker
from sqlalchemy.pool import StaticPool
from werkzeug.exceptions import HTTPException

log = logging.getLogger(__name__)


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


# ---------- validation schemas (Pydantic v2 works fine inside Flask) ----------
class ItemCreate(BaseModel):
    model_config = ConfigDict(extra="forbid", str_strip_whitespace=True)

    sku: str = Field(min_length=1, max_length=32)
    name: str = Field(min_length=1, max_length=100)
    price_cents: int = Field(ge=0)
    quantity: int = Field(default=0, ge=0)


class ItemUpdate(BaseModel):
    model_config = ConfigDict(extra="forbid", str_strip_whitespace=True)

    name: str | None = Field(default=None, min_length=1, max_length=100)
    price_cents: int | None = Field(default=None, ge=0)
    quantity: int | None = Field(default=None, ge=0)


class ItemRead(BaseModel):
    model_config = ConfigDict(from_attributes=True)

    id: int
    sku: str
    name: str
    price_cents: int
    quantity: int


# ---------- per-request session ----------
def get_db() -> Session:
    """Open one session per request lazily and cache it on `g`."""
    if "db" not in g:
        g.db = current_app.extensions["items_sessionmaker"]()
    return g.db


def close_db(exc: BaseException | None) -> None:
    db = g.pop("db", None)
    if db is not None:
        if exc is not None:
            db.rollback()
        db.close()


# ---------- errors: one JSON shape for every failure ----------
def error_body(status: int, message: str, details: Any = None) -> tuple[dict, int]:
    body: dict[str, Any] = {"error": {"status": status, "message": message}}
    if details is not None:
        body["error"]["details"] = details
    return body, status


def register_error_handlers(app: Flask) -> None:
    @app.errorhandler(HTTPException)
    def handle_http(exc: HTTPException):
        return error_body(exc.code or 500, exc.description or exc.name)

    @app.errorhandler(ValidationError)
    def handle_validation(exc: ValidationError):
        return error_body(422, "validation failed", exc.errors(include_url=False, include_context=False))

    @app.errorhandler(Exception)
    def handle_unexpected(exc: Exception):
        log.exception("unhandled error")  # full traceback goes to logs, not to the client
        return error_body(500, "internal server error")


# ---------- routes ----------
bp = Blueprint("items", __name__, url_prefix="/items")


def _get_or_404(db: Session, item_id: int) -> Item:
    item = db.get(Item, item_id)
    if item is None:
        abort(404, description=f"item {item_id} not found")  # raises NotFound
    return item


@bp.post("")
def create_item():
    payload = ItemCreate.model_validate(request.get_json(silent=True) or {})
    db = get_db()
    item = Item(**payload.model_dump())
    db.add(item)
    try:
        db.commit()  # the unique constraint is the source of truth, not a pre-check
    except IntegrityError:
        db.rollback()
        abort(409, description=f"sku {payload.sku!r} already exists")
    body = ItemRead.model_validate(item).model_dump()
    return body, 201, {"Location": url_for("items.get_item", item_id=item.id)}


@bp.get("")
def list_items():
    try:
        limit = int(request.args.get("limit", 20))
        offset = int(request.args.get("offset", 0))
    except ValueError:
        abort(422, description="limit and offset must be integers")
    if not (1 <= limit <= 100) or offset < 0:
        abort(422, description="limit must be 1-100 and offset >= 0")
    db = get_db()
    total = db.scalar(select(func.count()).select_from(Item))
    rows = db.scalars(select(Item).order_by(Item.id).limit(limit).offset(offset)).all()
    return {
        "items": [ItemRead.model_validate(r).model_dump() for r in rows],
        "total": total,
        "limit": limit,
        "offset": offset,
    }


@bp.get("/<int:item_id>")
def get_item(item_id: int):
    item = _get_or_404(get_db(), item_id)
    return ItemRead.model_validate(item).model_dump()


@bp.patch("/<int:item_id>")
def update_item(item_id: int):
    payload = ItemUpdate.model_validate(request.get_json(silent=True) or {})
    db = get_db()
    item = _get_or_404(db, item_id)
    for field, value in payload.model_dump(exclude_unset=True, exclude_none=True).items():
        setattr(item, field, value)
    db.commit()
    return ItemRead.model_validate(item).model_dump()


@bp.delete("/<int:item_id>")
def delete_item(item_id: int):
    db = get_db()
    item = _get_or_404(db, item_id)
    db.delete(item)
    db.commit()
    return "", 204


# ---------- app factory ----------
def make_engine(url: str):
    if url in ("sqlite://", "sqlite+pysqlite://") or url.endswith(":memory:"):
        # One shared in-memory connection, usable from any thread (tests, dev).
        return create_engine(url, connect_args={"check_same_thread": False}, poolclass=StaticPool)
    return create_engine(url, pool_pre_ping=True)


def create_app(config: dict[str, Any] | None = None) -> Flask:
    app = Flask(__name__)
    app.config.from_mapping(DATABASE_URL="sqlite+pysqlite:///:memory:")
    app.config.from_prefixed_env()  # FLASK_DATABASE_URL=... overrides
    if config:
        app.config.update(config)

    engine = make_engine(app.config["DATABASE_URL"])
    Base.metadata.create_all(engine)  # demo only; use Alembic migrations in real services
    app.extensions["items_sessionmaker"] = sessionmaker(engine, expire_on_commit=False)

    app.teardown_appcontext(close_db)
    register_error_handlers(app)
    app.register_blueprint(bp)

    @app.get("/healthz")
    def healthz():
        return {"status": "ok"}

    return app
```

Tests that prove the contract (the fixture is the pattern interviewers look for):

```python
@pytest.fixture
def app():
    # A fresh in-memory database per test: the factory is the seam for test config.
    return create_app({"TESTING": True, "DATABASE_URL": "sqlite+pysqlite:///:memory:"})


@pytest.fixture
def client(app):
    return app.test_client()


def make_item(client, **overrides):
    body = {"sku": "SKU-1", "name": "Widget", "price_cents": 1999, "quantity": 5} | overrides
    return client.post("/items", json=body)
```

```python
def test_validation_error_returns_422_with_field_details(client):
    resp = make_item(client, price_cents=-5, name="")
    assert resp.status_code == 422
    error = resp.get_json()["error"]
    assert error["message"] == "validation failed"
    assert {tuple(d["loc"]) for d in error["details"]} == {("price_cents",), ("name",)}
```

```python
def test_duplicate_sku_returns_409(client):
    assert make_item(client).status_code == 201
    resp = make_item(client, name="Other")
    assert resp.status_code == 409
    assert "already exists" in resp.get_json()["error"]["message"]
```

```python
def test_override_session_factory_to_simulate_db_outage(app, client, caplog):
    # Flask has no Depends(); the seam is whatever the factory registered.
    def broken_sessionmaker():
        raise OperationalError("SELECT 1", {}, Exception("connection refused"))

    app.extensions["items_sessionmaker"] = broken_sessionmaker
    resp = client.get("/items/1")
    assert resp.status_code == 500
    assert resp.get_json() == {"error": {"status": 500, "message": "internal server error"}}
    assert "unhandled error" in caplog.text  # logged server-side, not leaked to the client
```

**Talking points:**

- `abort(404)` raises `NotFound`; the `HTTPException` handler turns every Werkzeug error, including routing 404 and 405, into the same JSON envelope.
- 409 comes from the database constraint, not from a `SELECT` before the `INSERT`; a pre-check races under concurrency.
- `expire_on_commit=False` lets the view serialize the object after `commit()` without another query.
- `StaticPool` plus `check_same_thread=False` makes one in-memory SQLite connection visible to every thread; without it each connection gets an empty database.
- Offset pagination is fine for small tables; for deep pages switch to keyset (`WHERE id > :last_id ORDER BY id LIMIT :n`), see [REST API Design](05-REST-API-Design.md).
- Flask has no `Depends()`; the test seam is the factory's config and whatever it stores in `app.extensions`.

---

## F3. Walk me through the lifecycle of a Flask request. (must know)

1. The WSGI server (gunicorn) calls `app(environ, start_response)`, which calls `Flask.wsgi_app`.
2. Flask builds a `RequestContext` and pushes it; that pushes an `AppContext` if none is active, opens the session from the cookie, and matches the URL to an endpoint.
3. `request_started` signal, then `url_value_preprocessor` and `before_request` functions (app-level, then blueprint).
   If a `before_request` returns a value, that becomes the response and the view is skipped.
4. The view runs; its return value (str, dict, list, tuple, `Response`) goes through `make_response`.
5. Exceptions raised in steps 3-4 go to the matching `errorhandler`; if none matches, a 500 is generated (or the exception propagates in debug and testing mode).
6. `after_request` functions run in reverse registration order, then the session cookie is saved.
7. The response is returned to the server and streamed to the client.
8. The contexts pop: `teardown_request` then `teardown_appcontext` run, always, with the exception if there was one.

The hook order, observed:

```python
def test_hook_order_on_success():
    events: list[str] = []
    app = make_hooks_app(events)
    resp = app.test_client().get("/ok", headers={"X-Api-Key": "secret"})
    assert resp.headers["X-Served-By"] == "items"
    assert events == [
        "before_request",
        "view",
        "after_request",
        "teardown_request(None)",
        "teardown_appcontext",
    ]
```

```python
def test_hook_order_on_unhandled_exception():
    events: list[str] = []
    app = make_hooks_app(events)
    app.config["PROPAGATE_EXCEPTIONS"] = False  # production behaviour: turn it into a 500
    resp = app.test_client().get("/boom", headers={"X-Api-Key": "secret"})
    assert resp.status_code == 500
    assert events == [
        "before_request",
        "after_request",  # runs on the generated 500 response
        "teardown_request(ValueError)",
        "teardown_appcontext",
    ]
```

**Trick question:** "Does `after_request` run when the view raises?"
It runs on the error response that Flask generates, as the test shows, but a failing `after_request` stops the remaining ones.
Put cleanup that must always happen (closing sessions, releasing locks) in `teardown_*`, never in `after_request`.

---

## F4. Application context vs request context: what are `current_app`, `g`, `request`, and `session`, and how do the context locals work? (must know)

- **Application context:** "which app is active"; exposes `current_app` and `g`.
  Pushed for every request, for CLI commands, and manually with `app.app_context()` in scripts, workers, and tests.
- **Request context:** "which request is active"; exposes `request` and `session`.
  Pushing it also pushes an app context.
- **`g`:** scratch space for one app context (so one request): the current user, a DB session, a request ID.
  It is not global and does not survive to the next request.
- **`session`:** the user's cookie-backed session dict, persisted across requests (F15).

**How the "globals" work:** in Flask 3.1 each proxy is a Werkzeug `LocalProxy` over a `contextvars.ContextVar` (`flask.app_ctx` and `flask.request_ctx` in `flask/globals.py`).
Every thread and every asyncio task sees its own value of a `ContextVar`, which is why `request` is safe to import at module level yet points at a different request in each concurrent worker thread.

```python
def test_context_locals_need_an_active_context():
    app = Flask(__name__)
    with pytest.raises(RuntimeError, match="application context"):
        current_app.name
    with app.app_context():
        assert current_app.name == app.name
        g.user_id = 7
    with app.app_context():
        assert "user_id" not in g  # g lives and dies with one app context
    with app.test_request_context("/items?limit=5"):
        assert request.args["limit"] == "5"
        assert current_app.name == app.name  # a request context pushes an app context too
```

**The production failure:** "RuntimeError: Working outside of application context" (or request context).
It happens in a Celery task, a thread you started, a script, or code at import time.
A new `threading.Thread` does not inherit the caller's context variables on the standard CPython build, so hand it data, not proxies, or wrap it with `copy_current_request_context`:

```python
def test_background_thread_does_not_inherit_request_context():
    app = Flask(__name__)

    @app.get("/report")
    def report():
        results: dict[str, str] = {}

        def worker_plain():
            try:
                results["plain"] = request.path
            except RuntimeError:
                results["plain"] = "no request context"

        @copy_current_request_context
        def worker_copied():
            results["copied"] = request.path

        for fn in (worker_plain, worker_copied):
            t = threading.Thread(target=fn)
            t.start()
            t.join()
        return results

    body = app.test_client().get("/report").get_json()
    assert body == {"plain": "no request context", "copied": "/report"}
```

**Follow-up:** "Why not pass `request` explicitly?"
Flask chose implicit context for ergonomics; the cost is hidden coupling and harder testing, which is one reason FastAPI injects the request as a parameter.

---

## F5. What is the application factory pattern and why use it? (must know)

> "Instead of a module-level `app = Flask(__name__)`, a `create_app(config)` function builds, configures, and returns the app.
> Tests can build a fresh app per test with test config, I can run several configured instances, and there are no circular imports because blueprints and extensions do not import a global app."

The shape (from F2):

```python
def create_app(config: dict[str, Any] | None = None) -> Flask:
    app = Flask(__name__)
    app.config.from_mapping(DATABASE_URL="sqlite+pysqlite:///:memory:")
    app.config.from_prefixed_env()  # FLASK_DATABASE_URL=... overrides
    if config:
        app.config.update(config)

    engine = make_engine(app.config["DATABASE_URL"])
    Base.metadata.create_all(engine)  # demo only; use Alembic migrations in real services
    app.extensions["items_sessionmaker"] = sessionmaker(engine, expire_on_commit=False)

    app.teardown_appcontext(close_db)
    register_error_handlers(app)
    app.register_blueprint(bp)

    @app.get("/healthz")
    def healthz():
        return {"status": "ok"}

    return app
```

- Extensions are created at module level unbound (`db = SQLAlchemy()`), then bound inside the factory with `db.init_app(app)` (F12).
- Blueprints and views reach the app through `current_app`, never by importing it.
- Gunicorn and the `flask` CLI both call factories: `gunicorn 'flask_items:create_app()'`, and `flask --app flask_items run` finds `create_app` automatically.

**Pitfall:** doing work at import time (opening DB connections, reading secrets) in a module the factory imports; do it inside the factory or lazily per request.

---

## F6. What are Blueprints and how do you structure a large Flask app?

- A Blueprint is a deferred set of routes, error handlers, hooks, templates, and static files, registered on an app with an optional `url_prefix` (`/api/v1/items`).
- Endpoint names are namespaced: `url_for("items.get_item", item_id=1)`.
- Blueprint-level `before_request` and `errorhandler` apply only to requests routed to that blueprint.
- Blueprints can be nested (`parent.register_blueprint(child)`) and registered more than once under different names and prefixes (API versioning).

Layout that scales:

```text
app/
  __init__.py        # create_app()
  extensions.py      # db = SQLAlchemy(), cors = CORS(), ... unbound
  config.py          # BaseConfig, DevConfig, ProdConfig
  items/
    routes.py        # bp = Blueprint("items", __name__, url_prefix="/items")
    schemas.py       # Pydantic or marshmallow
    service.py       # business logic, no Flask imports: easy to unit test
    repository.py    # SQLAlchemy queries
  orders/...
tests/
  conftest.py        # app and client fixtures
```

**Trick question:** "Why does my blueprint's 404 handler not fire for `/api/does-not-exist`?"
Routing fails before Flask knows which blueprint the URL belongs to, so only app-level handlers see routing 404 and 405.

```python
def make_blueprint_app(app_level_handler: bool) -> Flask:
    app = Flask(__name__)
    api = Blueprint("api", __name__, url_prefix="/api")

    @api.errorhandler(404)
    def api_not_found(exc):
        return {"error": "api resource not found"}, 404

    @api.get("/things/<int:thing_id>")
    def get_thing(thing_id: int):
        abort(404)

    if app_level_handler:
        # The fix: one app-level handler that branches on the path.
        @app.errorhandler(404)
        def not_found(exc):
            if request.path.startswith("/api/"):
                return {"error": "not found"}, 404
            return exc

    app.register_blueprint(api)
    return app


def test_blueprint_error_handler_does_not_see_routing_404():
    client = make_blueprint_app(app_level_handler=False).test_client()
    assert client.get("/api/things/1").get_json() == {"error": "api resource not found"}
    unmatched = client.get("/api/does-not-exist")
    assert unmatched.status_code == 404
    assert not unmatched.is_json  # routing failed before any blueprint was selected

    fixed = make_blueprint_app(app_level_handler=True).test_client()
    assert fixed.get("/api/does-not-exist").get_json() == {"error": "not found"}
    assert fixed.get("/api/things/1").get_json() == {"error": "api resource not found"}
```

---

## F7. How do you handle errors and return consistent JSON errors? (must know)

- Register handlers on the app for `HTTPException` (every Werkzeug 4xx and 5xx), your validation error type, your domain exceptions, and `Exception` as the last resort.
- Handlers are chosen by the exception's class hierarchy, most specific first, so a handler for `HTTPException` wins over one for `Exception` for a 404.
- The catch-all logs the traceback and returns a generic 500; never send stack traces or SQL to the client.
- Raise errors with `abort(404, description=...)` or custom exceptions from service code; keep status mapping in one place.

The handlers from F2:

```python
def error_body(status: int, message: str, details: Any = None) -> tuple[dict, int]:
    body: dict[str, Any] = {"error": {"status": status, "message": message}}
    if details is not None:
        body["error"]["details"] = details
    return body, status


def register_error_handlers(app: Flask) -> None:
    @app.errorhandler(HTTPException)
    def handle_http(exc: HTTPException):
        return error_body(exc.code or 500, exc.description or exc.name)

    @app.errorhandler(ValidationError)
    def handle_validation(exc: ValidationError):
        return error_body(422, "validation failed", exc.errors(include_url=False, include_context=False))

    @app.errorhandler(Exception)
    def handle_unexpected(exc: Exception):
        log.exception("unhandled error")  # full traceback goes to logs, not to the client
        return error_body(500, "internal server error")
```

**Pitfalls:**

- Without a `HTTPException` handler, a routing 404 returns HTML while your views return JSON.
- In `TESTING` or `DEBUG` mode `PROPAGATE_EXCEPTIONS` defaults to true, so unhandled exceptions raise in the test instead of returning 500; that is useful, but test the 500 path with an explicit handler as F2 does.
- A 500 handler that itself touches the broken dependency (database) fails again; keep it dependency-free.
- For RFC 9457 problem details (`application/problem+json`), change `error_body`; see [REST API Design](05-REST-API-Design.md).

---

## F8. What are `before_request`, `after_request`, and the teardown hooks used for?

| Hook | Runs | Typical use | Can change the response? |
| --- | --- | --- | --- |
| `before_request` | Before the view, after routing | Auth checks, request ID, rate limit | Yes: returning a value skips the view |
| `after_request` | After a response exists (including error responses) | Headers (CORS, security, request ID) | Yes: must return the response |
| `teardown_request` | When the request context pops, always | Cleanup tied to the request | No |
| `teardown_appcontext` | When the app context pops, always (requests, CLI, tests) | Close DB sessions and connections | No |

```python
def make_hooks_app(events: list[str]) -> Flask:
    app = Flask(__name__)

    @app.before_request
    def authenticate():
        events.append("before_request")
        if request.headers.get("X-Api-Key") != "secret":
            return {"error": "unauthorized"}, 401  # a return value short-circuits the view

    @app.after_request
    def add_header(resp):
        events.append("after_request")
        resp.headers["X-Served-By"] = "items"
        return resp

    @app.teardown_request
    def teardown_request(exc):
        events.append(f"teardown_request({type(exc).__name__ if exc else None})")

    @app.teardown_appcontext
    def teardown_appcontext(exc):
        events.append("teardown_appcontext")

    @app.get("/ok")
    def ok():
        events.append("view")
        return {"ok": True}

    @app.get("/boom")
    def boom():
        raise ValueError("boom")

    return app
```

- `after_request` and teardown functions run in reverse registration order.
- `@after_this_request` inside a view adds a one-off hook (for example, set a cookie only on this response).
- Prefer `teardown_appcontext` for DB cleanup because it also runs for CLI commands and manual `app.app_context()` blocks.

---

## F9. How do you manage SQLAlchemy sessions in Flask, and how do connections leak? (must know)

> "One session per request, created lazily, and always closed in `teardown_appcontext`, with a rollback when the request failed.
> Flask-SQLAlchemy does exactly this with a `scoped_session` keyed on the app context; without it I cache a session on `g` and close it in teardown, as in F2.
> The engine and its connection pool are per process and created once in the factory."

Flask-SQLAlchemy 3.1 with the SQLAlchemy 2.x declarative style:

```python
class FsaBase(DeclarativeBase):
    pass


db = SQLAlchemy(model_class=FsaBase)  # created once at import time, bound to no app


class User(db.Model):
    id: Mapped[int] = mapped_column(primary_key=True)
    email: Mapped[str] = mapped_column(String(255), unique=True)


def create_users_app() -> Flask:
    app = Flask(__name__)
    app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite://"
    db.init_app(app)  # per-app state goes in app.extensions["sqlalchemy"]
    with app.app_context():
        db.create_all()

    @app.post("/users")
    def create_user():
        db.session.add(User(email=request.get_json()["email"]))
        db.session.commit()
        return {"count": len(db.session.scalars(select(User)).all())}, 201

    return app


def test_flask_sqlalchemy_init_app_and_session_scope():
    app = create_users_app()
    assert app.test_client().post("/users", json={"email": "a@x.io"}).get_json() == {"count": 1}

    with pytest.raises(RuntimeError, match="application context"):
        db.session.get(User, 1)
    with app.app_context():
        first = db.session()
    with app.app_context():
        second = db.session()
    assert first is not second  # one session per app context, removed at teardown
```

**How connections leak, and the fix:**

| Symptom | Cause | Fix |
| --- | --- | --- |
| `QueuePool limit of size 5 overflow 10 reached, connection timed out` | Sessions opened outside requests (threads, scripts) and never closed | `with Session(engine) as s:` or `with app.app_context():` around the work |
| Same error under load | Pool smaller than worker threads | Pool size at least threads per process; count processes x pool vs `max_connections` |
| `idle in transaction` sessions in PostgreSQL | A read left a transaction open until teardown, or teardown never ran | Short transactions; `session.close()` in teardown; `idle_in_transaction_session_timeout` |
| Stale connection errors after a DB failover or idle timeout | Pooled connections killed by the server or a firewall | `pool_pre_ping=True`, `pool_recycle` below the server's idle timeout |
| Data from one request visible in another | A module-level `Session()` shared by all threads | Never share a session across threads; one per request |

- Total DB connections = processes x (pool_size + max_overflow); 8 pods x 5 workers x 15 exceeds PostgreSQL's default `max_connections` of 100, so put PgBouncer in front or shrink pools.
- After `fork` (gunicorn `preload_app`), call `engine.dispose(close=False)` in each child so processes do not share sockets.

More on sessions and pooling: [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md).

---

## F10. How do you run Flask in production? Workers, worker count, timeouts. (must know)

> "Never the development server: it is single-process, unhardened, and the debugger allows code execution.
> I run gunicorn behind a reverse proxy or load balancer: `gthread` workers for typical I/O-bound APIs, start at `2 x cores + 1` processes and tune by measurement, a 30-second worker timeout, and worker recycling with jitter.
> In Kubernetes I set the worker count explicitly per pod and scale with replicas."

```python
# gunicorn.conf.py - read by: gunicorn -c gunicorn.conf.py 'flask_items:create_app()'
import os

bind = os.environ.get("BIND", "0.0.0.0:8000")
# Set WEB_CONCURRENCY explicitly in containers: os.cpu_count() sees host cores, not the cgroup limit.
workers = int(os.environ.get("WEB_CONCURRENCY", 2 * (os.cpu_count() or 1) + 1))
worker_class = "gthread"  # threads help I/O-bound views; sync = 1 request per process
threads = int(os.environ.get("GUNICORN_THREADS", 4))
timeout = 30  # a worker silent this long is killed and restarted
graceful_timeout = 30  # time to finish in-flight requests on SIGTERM
keepalive = 75  # above the load balancer idle timeout, so the proxy closes idle connections first
max_requests = 1000  # recycle workers to contain slow memory leaks
max_requests_jitter = 100  # so all workers do not restart at once
accesslog = "-"  # stdout, let the platform collect logs
errorlog = "-"
```

This config was checked with `gunicorn --check-config` and then booted with `WEB_CONCURRENCY=2`; `/healthz` and `POST /items` returned 200 and 201.

| Worker class | Model | Use when | Watch out for |
| --- | --- | --- | --- |
| `sync` (default) | 1 request per process | CPU-bound or very simple apps | One slow call blocks the process; `WORKER TIMEOUT` kills it |
| `gthread` | N threads per process | Most Flask APIs (I/O-bound) | Thread-safety of globals; GIL limits CPU work |
| `gevent` | Green threads, monkey-patched I/O | Thousands of idle connections, long polling | Patch before other imports; C drivers (psycopg2) need green support; one CPU-bound call stalls all |

- **Worker count:** gunicorn's docs suggest `(2 x cores) + 1` as a starting point; memory usually caps it first because each process loads the whole app.
  `os.cpu_count()` returns host cores inside a container, not the CPU limit, so set `WEB_CONCURRENCY` explicitly.
- **Timeouts:** `timeout` kills a worker silent for that long (default 30 s).
  With `sync` a single slow request trips it; with `gthread` it measures whether the worker process is alive, not request length.
  Set gunicorn's `keepalive` above the load balancer's idle timeout (60 s by default on an AWS ALB) so the proxy closes idle connections first; the reverse order causes sporadic 502s.
- **Graceful shutdown:** Kubernetes sends SIGTERM; `graceful_timeout` must be below `terminationGracePeriodSeconds`.
- **Per-process state:** each worker has its own memory, so in-process caches, rate-limit counters, and (as the F2 demo shows) in-memory SQLite are not shared; use Redis or the database.
- Gunicorn 26 also ships an `asgi` worker class, but Flask is WSGI, so it does not apply here.

More on containers and probes: [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md).

---

## F11. How do you handle configuration and secrets in Flask?

- Layer config: code defaults (`from_object`), then an optional instance file that is not committed (`from_pyfile` from the instance folder), then environment variables (`from_prefixed_env`), then explicit test overrides passed to the factory.
- `from_prefixed_env()` reads `FLASK_*` variables, parses each value with `json.loads` (so `40` becomes an int and `true` a bool), and nests on double underscores.
- Secrets come from the environment, injected by the platform (Kubernetes Secrets, AWS Secrets Manager, Vault); never from the repo.
- Rotate `SECRET_KEY` with `SECRET_KEY_FALLBACKS` (new in Flask 3.1) so existing sessions stay valid during rollout (F15).

```python
class BaseConfig:
    DATABASE_URL = "sqlite://"
    POOL_SIZE = 5


class ProdConfig(BaseConfig):
    POOL_SIZE = 20


def test_config_layers(monkeypatch, tmp_path):
    (tmp_path / "config.py").write_text("POOL_SIZE = 30\n")
    monkeypatch.setenv("FLASK_SECRET_KEY", "from-env")
    monkeypatch.setenv("FLASK_POOL_SIZE", "40")  # values are parsed with json.loads
    monkeypatch.setenv("FLASK_FEATURES__NEW_PRICING", "true")  # __ nests into a dict

    app = Flask(__name__, instance_path=str(tmp_path), instance_relative_config=True)
    app.config.from_object(ProdConfig)  # 1. code defaults
    assert app.config["POOL_SIZE"] == 20
    app.config.from_pyfile("config.py", silent=True)  # 2. instance/config.py (not in git)
    assert app.config["POOL_SIZE"] == 30
    app.config.from_prefixed_env()  # 3. environment wins
    assert app.config["POOL_SIZE"] == 40
    assert app.config["SECRET_KEY"] == "from-env"
    assert app.config["FEATURES"] == {"NEW_PRICING": True}
```

**Pitfalls:** reading `os.environ` all over the code base instead of once in the factory; config that differs between workers because it was mutated at runtime; logging the config dict with secrets in it.

---

## F12. How does the Flask extension pattern (`init_app`) work?

- An extension object is created once at import time without an app, so other modules can import it without a circular import.
- `ext.init_app(app)` reads the app's config, registers hooks (for example `teardown_appcontext`), and stores per-app state in `app.extensions["name"]`, never on the extension object itself.
- At request time the extension finds its state through `current_app.extensions[...]`, so one extension instance can serve several apps (tests create many).

The F2 service follows the same convention by hand: the factory stores the session factory in `app.extensions["items_sessionmaker"]`, and `get_db()` reads it through `current_app`.

Common extensions to name: Flask-SQLAlchemy, Flask-Migrate (Alembic), Flask-Smorest or Flask-RESTX (API and OpenAPI), Flask-JWT-Extended, Flask-CORS, Flask-Limiter, Flask-Caching.

---

## F13. Does Flask support async? What does an `async def` view actually do? (must know)

> "Since Flask 2.0 you can write `async def` views, hooks, and error handlers if you install `flask[async]`.
> Flask is still WSGI: `Flask.ensure_sync` wraps the coroutine with asgiref's `async_to_sync`, which runs it on an event loop in a separate thread while the worker thread blocks until it finishes.
> So concurrency inside one request works, for example `asyncio.gather` over five HTTP calls, but the worker is occupied for the whole request, and there is no WebSocket or background-task support.
> If most endpoints need async I/O, that is the signal to use FastAPI or Quart."

```python
def test_async_view_runs_via_asgiref_on_a_wsgi_worker():
    app = Flask(__name__)

    @app.before_request
    def remember_thread():
        g.request_thread = threading.get_ident()

    @app.get("/fanout")
    async def fanout():
        async def fetch(n: int) -> int:
            await asyncio.sleep(0.1)  # stands in for an HTTP call with httpx.AsyncClient
            return n

        start = time.perf_counter()
        results = await asyncio.gather(*(fetch(i) for i in range(5)))
        return {
            "results": results,
            "elapsed": time.perf_counter() - start,
            "same_thread_as_request": threading.get_ident() == g.request_thread,
        }

    body = app.test_client().get("/fanout").get_json()
    assert body["results"] == [0, 1, 2, 3, 4]
    assert body["elapsed"] < 0.3  # concurrent inside the view, not 5 x 0.1
    # asgiref runs the coroutine on an event loop in a separate thread while
    # the worker thread blocks waiting for it.
    assert body["same_thread_as_request"] is False
```

- Without `asgiref` installed, the first async view raises `RuntimeError: Install Flask with the 'async' extra`.
- Throughput is not better than a threaded `def` view; it can be worse because each call pays for an event loop and a thread handoff.
- Do not start background tasks with `asyncio.create_task` in a view; the loop is torn down after the view returns and pending tasks are cancelled.
- Running Flask under an ASGI server does not change this; you would wrap it in a WSGI-to-ASGI adapter and each request still runs synchronously in a thread.

---

## F14. How do you test a Flask app?

- A pytest fixture calls the factory with test config; another returns `app.test_client()`.
- `client.get/post(..., json=...)` exercises routing, hooks, error handlers, and serialization without a server.
- `app.test_request_context()` for unit-testing code that reads `request`; `app.app_context()` for code that needs `current_app` or `g`.
- `app.test_cli_runner()` for Click commands.
- Swap dependencies through the factory (test DB URL) or by replacing what it registered (`app.extensions[...]`), as F2 does to simulate a database outage.
- Use a real PostgreSQL in CI (Testcontainers or a service container) for anything that depends on PostgreSQL semantics; SQLite hides type, locking, and constraint differences.

```python
def test_create_then_get_happy_path(client):
    created = make_item(client)
    assert created.status_code == 201
    item = created.get_json()
    assert item == {"id": 1, "sku": "SKU-1", "name": "Widget", "price_cents": 1999, "quantity": 5}
    assert created.headers["Location"] == "/items/1"

    fetched = client.get("/items/1")
    assert fetched.status_code == 200
    assert fetched.get_json() == item
```

```python
def test_pagination(client):
    for i in range(5):
        make_item(client, sku=f"SKU-{i}")
    page = client.get("/items?limit=2&offset=2").get_json()
    assert page["total"] == 5
    assert [it["sku"] for it in page["items"]] == ["SKU-2", "SKU-3"]
    assert client.get("/items?limit=0").status_code == 422
    assert client.get("/items?limit=abc").status_code == 422
```

Broader testing strategy and production debugging: [Testing, Debugging, Production](10-Testing-Debugging-Production.md).

---

## F15. How do Flask sessions work? Are they secure? What about CSRF?

- The default session is a **signed** cookie (itsdangerous, HMAC with `SECRET_KEY`): the client cannot forge or modify it, but it can **read** it, because it is base64-encoded JSON, not encrypted.
- Never put secrets or PII in it; store an ID and keep the data server-side (Flask-Session with Redis) if it is sensitive or large (cookies cap near 4 KB).
- Cookie flags for production: `SESSION_COOKIE_SECURE=True`, `SESSION_COOKIE_HTTPONLY=True` (the default), `SESSION_COOKIE_SAMESITE="Lax"`.
- Logout does not invalidate a stolen signed cookie; for revocation use server-side sessions or short-lived tokens.

```python
def test_session_cookie_is_signed_not_encrypted():
    client = make_session_app("k1").test_client()
    client.post("/login")
    payload = _session_cookie(client).split(".")[0]
    decoded = base64.urlsafe_b64decode(payload + "=" * (-len(payload) % 4))
    assert json.loads(decoded) == {"user_id": 42}  # anyone can read it; nobody can forge it

    client.set_cookie("session", _session_cookie(client)[:-2] + "xx")  # tamper with signature
    assert client.get("/me").get_json() == {"user_id": None}


def test_secret_key_rotation_with_fallbacks():
    old = make_session_app("old-key").test_client()
    old.post("/login")
    cookie = _session_cookie(old)

    rotated = make_session_app("new-key", fallbacks=["old-key"]).test_client()
    rotated.set_cookie("session", cookie)
    assert rotated.get("/me").get_json() == {"user_id": 42}

    no_fallback = make_session_app("new-key").test_client()
    no_fallback.set_cookie("session", cookie)
    assert no_fallback.get("/me").get_json() == {"user_id": None}
```

**CSRF:** any cookie-authenticated endpoint that changes state needs protection, because the browser attaches cookies to cross-site requests.
Use Flask-WTF's `CSRFProtect` for form apps, or `SameSite` cookies plus a custom header check for JSON APIs.
A pure bearer-token API (JWT in the `Authorization` header, no cookies) is not exposed to CSRF.
Tokens, OAuth2, and JWT validation: [Auth and Security](07-Auth-and-Security.md).

---

## F16. `jsonify` vs returning a dict: how does Flask serialize JSON?

- Returning a `dict` or (since Flask 2.2) a `list` from a view is serialized with `app.json`, the same as `jsonify`.
- `jsonify` is still needed for other top-level types (a dataclass, a string as JSON) and accepts keyword arguments.
- The default provider sorts keys, handles `dataclasses`, `Decimal` (as a string), `UUID`, and `datetime`, but emits datetimes as RFC 822 HTTP dates, which surprises clients expecting ISO 8601.
- Customize by subclassing `DefaultJSONProvider` and assigning `app.json`; the old `JSON_SORT_KEYS` and `JSON_AS_ASCII` config keys were removed in Flask 2.3.

```python
def test_json_return_values_and_default_provider():
    import dataclasses
    from datetime import datetime, timezone
    from decimal import Decimal

    from flask import jsonify
    from flask.json.provider import DefaultJSONProvider

    @dataclasses.dataclass
    class Fill:
        qty: int
        price: Decimal
        at: datetime

    class IsoJSONProvider(DefaultJSONProvider):
        sort_keys = False  # default is True

        @staticmethod
        def default(o):
            if isinstance(o, datetime):
                return o.isoformat()  # the default provider emits an RFC 822 HTTP date
            return DefaultJSONProvider.default(o)

    app = Flask(__name__)
    fill = Fill(qty=10, price=Decimal("101.25"), at=datetime(2026, 9, 29, 14, 30, tzinfo=timezone.utc))

    @app.get("/dict")
    def as_dict():
        return {"b": 1, "a": 2}  # dicts are serialized automatically

    @app.get("/list")
    def as_list():
        return [1, 2, 3]  # lists too, since Flask 2.2

    @app.get("/fill")
    def as_fill():
        return jsonify(fill)  # dataclass, Decimal, datetime handled by the provider

    client = app.test_client()
    assert client.get("/dict").get_data(as_text=True).replace(" ", "").strip() == '{"a":2,"b":1}'
    assert client.get("/list").get_json() == [1, 2, 3]
    assert client.get("/fill").get_json() == {"at": "Tue, 29 Sep 2026 14:30:00 GMT", "price": "101.25", "qty": 10}

    app2 = Flask(__name__)
    app2.json = IsoJSONProvider(app2)

    @app2.get("/fill")
    def as_fill_iso():
        return jsonify(fill)

    assert app2.test_client().get("/fill").get_json()["at"] == "2026-09-29T14:30:00+00:00"
```

---

## F17. How do you handle file uploads safely?

- Set `MAX_CONTENT_LENGTH`; Werkzeug raises 413 as soon as the body is read, without buffering the excess.
  Flask 3.1 also caps non-file form data with `MAX_FORM_MEMORY_SIZE` (default 500,000 bytes) and `MAX_FORM_PARTS` (default 1,000), and allows per-request overrides via `request.max_content_length`.
- Run the name through `secure_filename`, store under a generated name, and validate the type by extension and content, not by the client's `Content-Type`.
- Stream large files to object storage (S3 multipart or a presigned URL the client uploads to directly) instead of through the app.

```python
def make_upload_app(upload_dir) -> Flask:
    app = Flask(__name__)
    app.config["MAX_CONTENT_LENGTH"] = 1 * 1024 * 1024  # 1 MiB; Werkzeug raises 413 beyond it
    allowed = {".csv", ".json"}

    @app.errorhandler(413)
    def too_large(exc):
        return {"error": "file too large", "limit_bytes": app.config["MAX_CONTENT_LENGTH"]}, 413

    @app.post("/uploads")
    def upload():
        f = request.files.get("file")
        if f is None or not f.filename:
            abort(400, description="multipart field 'file' is required")
        name = secure_filename(f.filename)  # never trust the client's path
        if not any(name.endswith(ext) for ext in allowed):
            abort(415, description="only .csv and .json are accepted")
        f.save(upload_dir / f"{uuid.uuid4().hex}-{name}")
        return {"stored_as": name}, 201

    return app


def test_upload_happy_path_and_limits(tmp_path):
    import io

    client = make_upload_app(tmp_path).test_client()
    ok = client.post("/uploads", data={"file": (io.BytesIO(b"a,b\n1,2\n"), "../../trades.csv")})
    assert ok.status_code == 201
    assert ok.get_json() == {"stored_as": "trades.csv"}  # path traversal stripped

    big = client.post("/uploads", data={"file": (io.BytesIO(b"x" * (2 * 1024 * 1024)), "big.csv")})
    assert big.status_code == 413
    assert big.get_json()["error"] == "file too large"

    wrong = client.post("/uploads", data={"file": (io.BytesIO(b"MZ"), "tool.exe")})
    assert wrong.status_code == 415
```

---

## F18. How do you stream a large response from Flask?

- Return a `Response` wrapping a generator; Werkzeug sends chunks as they are produced, so memory stays flat for a large CSV export.
- The generator runs after the view returns, when the request context is gone; wrap it with `stream_with_context` if it reads `request`, `session`, or `g`.
- Stream from a server-side cursor (`yield_per`) so the database result is not loaded all at once.
- Each streaming client holds a worker thread for the whole download; with `sync` workers long downloads also hit `timeout`.

```python
def make_stream_app() -> Flask:
    app = Flask(__name__)

    @app.get("/export")
    def export():
        @stream_with_context  # keeps the request context alive while the generator runs
        def rows():
            prefix = request.args.get("prefix", "row")
            for i in range(3):
                yield f"{prefix}-{i}\n"

        return Response(rows(), mimetype="text/plain")

    @app.get("/export-broken")
    def export_broken():
        def rows():
            yield request.args.get("prefix", "row")  # runs after the view returned

        return Response(rows(), mimetype="text/plain")

    return app
```

```python
def test_streaming_without_context_fails():
    client = make_stream_app().test_client()
    with pytest.raises(RuntimeError, match="request context"):
        client.get("/export-broken").get_data()
```

---

## F19. How do you enable CORS in Flask?

- CORS is enforced by browsers: a cross-origin call with custom headers or a non-simple method triggers a preflight `OPTIONS` request, and the browser blocks the response unless `Access-Control-Allow-Origin` matches.
- Use Flask-CORS with an explicit origin allowlist per route prefix; never `*` together with credentials.
- CORS is not authentication: non-browser clients ignore it entirely.

```python
def test_cors_preflight():
    from flask_cors import CORS

    app = Flask(__name__)
    CORS(app, resources={r"/api/*": {"origins": ["https://app.example.com"]}})

    @app.post("/api/orders")
    def create_order():
        return {"id": 1}, 201

    client = app.test_client()
    preflight_headers = {"Access-Control-Request-Method": "POST"}
    allowed = client.options("/api/orders", headers={"Origin": "https://app.example.com", **preflight_headers})
    assert allowed.headers["Access-Control-Allow-Origin"] == "https://app.example.com"
    denied = client.options("/api/orders", headers={"Origin": "https://evil.example", **preflight_headers})
    assert "Access-Control-Allow-Origin" not in denied.headers
```

---

## F20. How do you set up logging in a Flask service?

- `app.logger` is a standard `logging` logger named after the app's import name; configure logging with `logging.config.dictConfig` before creating the app.
- Emit JSON logs to stdout; the platform ships them.
- Add a request ID in `before_request`, echo it in a response header, and inject it into every record with a `logging.Filter`, so one request can be traced across services.
- Log exceptions with `log.exception` in the 500 handler; add OpenTelemetry for traces and metrics.

```python
class RequestIdFilter(logging.Filter):
    def filter(self, record: logging.LogRecord) -> bool:
        record.request_id = g.get("request_id", "-") if has_request_context() else "-"
        return True


def test_request_id_logging(caplog):
    app = Flask(__name__)
    app.logger.addFilter(RequestIdFilter())

    @app.before_request
    def assign_request_id():
        g.request_id = request.headers.get("X-Request-ID") or uuid.uuid4().hex

    @app.after_request
    def echo_request_id(resp):
        resp.headers["X-Request-ID"] = g.request_id
        return resp

    @app.get("/work")
    def work():
        current_app.logger.info("doing work")
        return {"ok": True}

    with caplog.at_level(logging.INFO, logger=app.logger.name):
        resp = app.test_client().get("/work", headers={"X-Request-ID": "abc123"})
    assert resp.headers["X-Request-ID"] == "abc123"
    assert [r.request_id for r in caplog.records if r.getMessage() == "doing work"] == ["abc123"]
```

---

## F21. How do you get validation and OpenAPI docs in Flask?

| Option | Validation | OpenAPI | Notes |
| --- | --- | --- | --- |
| Flask-Smorest | marshmallow schemas | Yes (OpenAPI 3) | Blueprint-based, pagination and ETag helpers; my default for new Flask APIs |
| Flask-RESTX | Its own `fields` models | Swagger 2 | Fork of Flask-RESTPlus; common in older codebases |
| Pydantic by hand | Pydantic v2 | No, unless you add a plugin | What F2 does; same schemas can move to FastAPI later |

- marshmallow: `schema.load(data)` validates and deserializes, `schema.dump(obj)` serializes; `ValidationError.messages` is a field-to-errors dict.
- Pydantic: `Model.model_validate(data)`, `model.model_dump()`, `ValidationError.errors()`.
- Using Pydantic in Flask makes a later move to FastAPI cheaper because the schemas carry over.

---

## F22. What are the most common Flask production issues you have debugged?

| Symptom | Root cause | Fix |
| --- | --- | --- |
| `RuntimeError: Working outside of application context` | Code in a thread, Celery task, script, or import time used `current_app`/`g`/`db.session` | Push `app.app_context()` in the task; pass data, not proxies (F4) |
| `QueuePool limit ... reached` or `idle in transaction` | Sessions not closed, pool smaller than threads | Teardown closes sessions; size pools; `pool_pre_ping` (F9) |
| `[CRITICAL] WORKER TIMEOUT` then 502s | Slow downstream call in a `sync` worker | Client timeouts on every outbound call; `gthread`; move slow work to a queue |
| Wrong or mixed data between users | Mutable module-level state (a dict cache, a shared client with per-user state) mutated by concurrent threads | Keep request state in `g`; lock or use thread-safe structures; external cache |
| Counter or cache "resets" randomly | State lives in one of N worker processes | Redis or the database |
| Memory grows until OOM kill | Unbounded in-process caches, large responses built in memory | Bounded caches, streaming (F18), `max_requests` recycling as a stopgap |
| Works locally, 404 or wrong URLs behind the proxy | Missing `X-Forwarded-*` handling | `werkzeug.middleware.proxy_fix.ProxyFix` with the right hop count |
| HTML error pages from a JSON API | No `HTTPException` handler (F7) | One JSON error envelope |

**[fill in: one Flask production incident you handled: symptom, how you found it (logs, metrics, py-spy), root cause, fix, and what you changed to prevent it.]**

---

## F23. When and how would you migrate a Flask service to FastAPI?

> "Only with a reason: many I/O-bound endpoints that need async fan-out, a need for generated OpenAPI contracts, or a team already standardizing on FastAPI.
> I would not do a big-bang rewrite.
> I mount the existing Flask app inside a FastAPI app as a WSGI sub-application, write new endpoints in FastAPI, and move old ones route by route behind the same URL space, with contract tests guarding each move."

Incremental migration with `a2wsgi` (Starlette 1.7 still ships `starlette.middleware.wsgi`, but importing it emits a deprecation warning that points to `a2wsgi`):

```python
def test_mount_flask_inside_fastapi_with_a2wsgi():
    from a2wsgi import WSGIMiddleware
    from fastapi import FastAPI
    from fastapi.testclient import TestClient

    from flask_items import create_app

    legacy = create_app({"TESTING": True})
    api = FastAPI(title="Items API v2")

    @api.get("/v2/health")
    async def health_v2():
        return {"status": "ok", "served_by": "fastapi"}

    api.mount("/", WSGIMiddleware(legacy))  # declared last: catches every path not matched above

    client = TestClient(api)
    assert client.get("/v2/health").json() == {"status": "ok", "served_by": "fastapi"}
    created = client.post("/items", json={"sku": "S1", "name": "Widget", "price_cents": 100})
    assert created.status_code == 201  # handled by the Flask app, unchanged
    assert client.get("/items/1").json()["sku"] == "S1"
```

**Migration checklist:**

1. Move validation to Pydantic models first, inside Flask (as F2 does); they carry over unchanged.
2. Pull business logic out of views into plain functions or services with no Flask imports.
3. Replace `g`, `current_app`, and `request` access with explicit parameters, which become `Depends()` in FastAPI.
4. Map hooks: `before_request` auth becomes a dependency, `after_request` headers become middleware, teardown becomes a yield dependency.
5. Map error handlers to `app.exception_handler`, and decide whether to keep the Flask JSON envelope or FastAPI's `{"detail": ...}` shape; clients depend on it.
6. Keep sync SQLAlchemy and use `def` endpoints at first; switch to async drivers only where it pays off.
7. Serve with uvicorn workers; the Flask part still runs in a thread per request inside the adapter.

Risk: the mounted Flask app runs in a threadpool, so its throughput is bounded by that pool, and hooks do not cross the boundary; plan the move so it does not linger for years.
FastAPI details: [FastAPI](04-FastAPI.md).

---

## Go deeper

- [FastAPI](04-FastAPI.md) for the other half of the role, and [REST API Design](05-REST-API-Design.md), [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md), [Auth and Security](07-Auth-and-Security.md), [Testing, Debugging, Production](10-Testing-Debugging-Production.md) in this pack.
- Reference service and tests: [code/README.md](code/README.md).
- [Concurrency Mechanics: the Global Interpreter Lock](../Python_Zero_to_Godhood/Chapter_10_CONCURRENCY_MECHANICS__THE_GLOBAL_INTERPRETER_LOCK.md) for why threads help I/O-bound Flask and not CPU-bound work.
- [CPU and I/O Bound System Concurrency](../Python_Zero_to_Godhood/Chapter_26_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md).
- [Native Async/Await](../Python_Zero_to_Godhood/Volume_03_Generators_Iterators_and_Async_Inception/Chapter_11_Native_Async_Await_and_New_Operators/Chapter_11_Native_Async_Await_and_New_Operators.md) for the coroutine model behind F13.
- [Rate Limiter design](../../../04-System-Design/02-Case-Studies/02-Rate-Limiter/design.md) for per-process versus shared state.
- Official: [Flask docs](https://flask.palletsprojects.com/en/stable/), [Application context](https://flask.palletsprojects.com/en/stable/appcontext/), [Async in Flask](https://flask.palletsprojects.com/en/stable/async-await/), [Deploying with gunicorn](https://flask.palletsprojects.com/en/stable/deploying/gunicorn/), [gunicorn design](https://docs.gunicorn.org/en/stable/design.html), [Flask-SQLAlchemy](https://flask-sqlalchemy.readthedocs.io/en/stable/), [a2wsgi](https://github.com/abersheeran/a2wsgi).
