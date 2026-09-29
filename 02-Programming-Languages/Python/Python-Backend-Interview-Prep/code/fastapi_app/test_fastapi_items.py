"""Tests for the FastAPI reference service. Run with: pytest"""

import httpx
import pytest
from fastapi.testclient import TestClient
from sqlalchemy.orm import Session, sessionmaker

from fastapi_items import Base, Item, create_app, get_session, make_engine


@pytest.fixture
def client():
    app = create_app("sqlite+pysqlite:///:memory:")
    with TestClient(app) as c:  # the `with` block runs lifespan startup and shutdown
        yield c


def make_item(client, **overrides):
    body = {"sku": "SKU-1", "name": "Widget", "price_cents": 1999, "quantity": 5} | overrides
    return client.post("/items", json=body)


def test_create_then_get_happy_path(client):
    created = make_item(client)
    assert created.status_code == 201
    item = created.json()
    assert item == {"id": 1, "sku": "SKU-1", "name": "Widget", "price_cents": 1999, "quantity": 5}
    assert created.headers["location"] == "/items/1"
    assert client.get("/items/1").json() == item


def test_validation_error_is_422_with_pydantic_details(client):
    resp = make_item(client, price_cents=-5)
    assert resp.status_code == 422
    [error] = resp.json()["detail"]
    assert error["loc"] == ["body", "price_cents"]
    assert error["type"] == "greater_than_equal"


def test_path_param_type_error_is_422(client):
    resp = client.get("/items/abc")
    assert resp.status_code == 422
    assert resp.json()["detail"][0]["loc"] == ["path", "item_id"]


def test_not_found(client):
    resp = client.get("/items/999")
    assert resp.status_code == 404
    assert resp.json() == {"detail": "item 999 not found"}


def test_duplicate_sku_is_409(client):
    assert make_item(client).status_code == 201
    resp = make_item(client, name="Other")
    assert resp.status_code == 409
    assert "already exists" in resp.json()["detail"]


def test_pagination_and_query_bounds(client):
    for i in range(5):
        make_item(client, sku=f"SKU-{i}")
    page = client.get("/items", params={"limit": 2, "offset": 2}).json()
    assert page["total"] == 5
    assert [it["sku"] for it in page["items"]] == ["SKU-2", "SKU-3"]
    assert client.get("/items", params={"limit": 101}).status_code == 422
    assert client.get("/items", params={"offset": -1}).status_code == 422


def test_patch_is_partial_and_delete_is_204(client):
    make_item(client)
    patched = client.patch("/items/1", json={"quantity": 9})
    assert patched.status_code == 200
    assert patched.json()["quantity"] == 9
    assert patched.json()["name"] == "Widget"
    assert client.delete("/items/1").status_code == 204
    assert client.get("/items/1").status_code == 404


def test_openapi_lists_the_routes(client):
    paths = client.get("/openapi.json").json()["paths"]
    assert set(paths) == {"/items", "/items/{item_id}", "/healthz"}


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


def test_session_is_same_object_across_dependencies():
    # Dependency caching: get_session runs once per request even though two dependencies need it.
    seen: list[Session] = []
    app = create_app()

    def spy_session():
        engine = make_engine("sqlite://")
        Base.metadata.create_all(engine)
        with Session(engine) as s:
            s.add(Item(sku="X", name="x", price_cents=1))
            s.commit()
            seen.append(s)
            yield s

    app.dependency_overrides[get_session] = spy_session
    with TestClient(app) as c:
        assert c.patch("/items/1", json={"quantity": 3}).json()["quantity"] == 3
    assert len(seen) == 1
