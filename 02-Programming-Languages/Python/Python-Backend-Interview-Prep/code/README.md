---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: []
---

# Reference services: Flask and FastAPI items CRUD

Two small, tested implementations of the same items CRUD API, one per framework.
They back the "Build it live" answers in [Flask F2](../03-Flask.md) and [FastAPI A2](../04-FastAPI.md).
Both were run on 2026-09-29 under Python 3.14.7 with Flask 3.1.3, FastAPI 0.142.0, Starlette 1.7.0, Pydantic 2.13.5, and SQLAlchemy 2.1.1: 21 pytest tests passed (10 Flask, 11 FastAPI).

## What each service shows

| Concern | `flask_app/flask_items.py` | `fastapi_app/fastapi_items.py` |
| --- | --- | --- |
| App construction | `create_app(config)` factory plus a Blueprint | `create_app(database_url)` plus an `APIRouter` and `lifespan` |
| Validation | Pydantic v2 models called by hand, errors mapped to 422 | Pydantic v2 models in the signature, automatic 422 |
| DB session | One session per request cached on `g`, closed in `teardown_appcontext` | `get_session` yield dependency, cached per request |
| Errors | One JSON envelope for every `HTTPException`, 422, and 500 | `HTTPException` for 404 and 409, default `{"detail": ...}` shape |
| 409 | Catch `IntegrityError` on commit, roll back | Same |
| Pagination | `limit`/`offset` parsed and bounded by hand | `Query(ge=1, le=100)` |
| Test seam | Factory config plus replacing `app.extensions[...]` | `app.dependency_overrides[get_session]` |

Both use SQLite in memory with `StaticPool`, so every request and every thread shares one connection and no database file is written.

## Run the tests

```sh
python3 -m venv .venv && source .venv/bin/activate
pip install flask fastapi sqlalchemy httpx pytest
pytest            # from this folder: runs both suites (21 tests)
cd flask_app && pytest     # or one suite at a time
cd ../fastapi_app && pytest
```

- The module names differ (`flask_items`, `fastapi_items`, and their test files), so both suites run from this folder without an `__init__.py` or `conftest.py`.
- Pydantic comes with FastAPI; the Flask app uses it too.
- Starlette 1.7 prefers the `httpx2` package for `TestClient` and emits a `StarletteDeprecationWarning` when only `httpx` is installed; the tests still pass.
  `pip install httpx2` silences it.
- The FastAPI async test uses the AnyIO pytest plugin (`@pytest.mark.anyio`), which ships with `anyio`, a FastAPI dependency.

## Run the services

```sh
cd flask_app && flask --app flask_items run                    # dev server only
cd flask_app && gunicorn -w 4 -k gthread --threads 4 'flask_items:create_app()'
cd fastapi_app && uvicorn fastapi_items:app --reload          # dev
cd fastapi_app && uvicorn fastapi_items:app --workers 4       # multi-process
```

Each worker process gets its own in-memory database, so data written through one worker is invisible to the others.
Point `FLASK_DATABASE_URL` (Flask) or the `create_app(database_url=...)` argument (FastAPI) at PostgreSQL for anything real.
