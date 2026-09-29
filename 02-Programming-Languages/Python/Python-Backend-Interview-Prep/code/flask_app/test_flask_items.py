"""Tests for the Flask reference service. Run with: pytest"""

import pytest
from sqlalchemy.exc import OperationalError

from flask_items import create_app


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


def test_create_then_get_happy_path(client):
    created = make_item(client)
    assert created.status_code == 201
    item = created.get_json()
    assert item == {"id": 1, "sku": "SKU-1", "name": "Widget", "price_cents": 1999, "quantity": 5}
    assert created.headers["Location"] == "/items/1"

    fetched = client.get("/items/1")
    assert fetched.status_code == 200
    assert fetched.get_json() == item


def test_validation_error_returns_422_with_field_details(client):
    resp = make_item(client, price_cents=-5, name="")
    assert resp.status_code == 422
    error = resp.get_json()["error"]
    assert error["message"] == "validation failed"
    assert {tuple(d["loc"]) for d in error["details"]} == {("price_cents",), ("name",)}


def test_unknown_field_is_rejected(client):
    resp = make_item(client, colour="red")
    assert resp.status_code == 422


def test_missing_body_is_422_not_500(client):
    resp = client.post("/items", data="not json", content_type="text/plain")
    assert resp.status_code == 422


def test_not_found_is_json(client):
    resp = client.get("/items/999")
    assert resp.status_code == 404
    assert resp.get_json() == {"error": {"status": 404, "message": "item 999 not found"}}


def test_unknown_route_is_json_404(client):
    resp = client.get("/nope")
    assert resp.status_code == 404
    assert resp.is_json


def test_duplicate_sku_returns_409(client):
    assert make_item(client).status_code == 201
    resp = make_item(client, name="Other")
    assert resp.status_code == 409
    assert "already exists" in resp.get_json()["error"]["message"]


def test_pagination(client):
    for i in range(5):
        make_item(client, sku=f"SKU-{i}")
    page = client.get("/items?limit=2&offset=2").get_json()
    assert page["total"] == 5
    assert [it["sku"] for it in page["items"]] == ["SKU-2", "SKU-3"]
    assert client.get("/items?limit=0").status_code == 422
    assert client.get("/items?limit=abc").status_code == 422


def test_patch_is_partial_and_delete_is_204(client):
    make_item(client)
    patched = client.patch("/items/1", json={"quantity": 9})
    assert patched.status_code == 200
    assert patched.get_json()["quantity"] == 9
    assert patched.get_json()["name"] == "Widget"  # untouched field kept

    assert client.delete("/items/1").status_code == 204
    assert client.get("/items/1").status_code == 404


def test_override_session_factory_to_simulate_db_outage(app, client, caplog):
    # Flask has no Depends(); the seam is whatever the factory registered.
    def broken_sessionmaker():
        raise OperationalError("SELECT 1", {}, Exception("connection refused"))

    app.extensions["items_sessionmaker"] = broken_sessionmaker
    resp = client.get("/items/1")
    assert resp.status_code == 500
    assert resp.get_json() == {"error": {"status": 500, "message": "internal server error"}}
    assert "unhandled error" in caplog.text  # logged server-side, not leaked to the client
