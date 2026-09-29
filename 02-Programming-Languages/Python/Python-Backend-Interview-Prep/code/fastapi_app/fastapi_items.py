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
