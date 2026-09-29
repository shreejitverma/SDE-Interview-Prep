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
