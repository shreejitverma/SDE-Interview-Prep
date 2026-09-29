---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://docs.sqlalchemy.org/en/21/orm/session_basics.html, https://docs.sqlalchemy.org/en/21/orm/queryguide/relationships.html, https://docs.sqlalchemy.org/en/21/core/pooling.html, https://docs.sqlalchemy.org/en/21/orm/extensions/asyncio.html, https://docs.sqlalchemy.org/en/21/orm/versioning.html, https://docs.sqlalchemy.org/en/21/dialects/postgresql.html, https://alembic.sqlalchemy.org/en/latest/ops.html, https://www.postgresql.org/docs/current/transaction-iso.html, https://www.postgresql.org/docs/current/explicit-locking.html, https://www.postgresql.org/docs/current/sql-createindex.html, https://www.postgresql.org/docs/current/using-explain.html, https://dev.mysql.com/doc/refman/8.4/en/innodb-transaction-isolation-levels.html, https://learn.microsoft.com/en-us/sql/t-sql/statements/set-transaction-isolation-level-transact-sql, https://www.pgbouncer.org/features.html, https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/bp-partition-key-design.html, https://www.sqlite.org/eqp.html]
---

# SQL and SQLAlchemy

SQL fundamentals, PostgreSQL behavior, SQLAlchemy 2.x, Alembic, and the NoSQL and Redis touchpoints a Python backend interview reaches for.
S1-S3 (joins, second highest, top N per group), S7 (indexes), S9 (isolation), S14 (Session), S16 (N+1), S18 (pooling), and S22 (zero-downtime migrations) are near-certain; know them cold.
Every code sample here was run on 2026-09-29 under Python 3.14.7 with SQLAlchemy 2.1.1, aiosqlite 0.22.1, Alembic 1.20.0, FastAPI 0.142.0, Flask 3.1.3, and SQLite 3.53.4: 41 pytest tests passed.
PostgreSQL-only statements were compiled with SQLAlchemy's PostgreSQL dialect or rendered by Alembic offline mode and checked as text, not executed against a live PostgreSQL server.

The SQL questions use this fixture (runnable as is with `sqlite3`):

```sql
CREATE TABLE departments (id INTEGER PRIMARY KEY, name TEXT NOT NULL);
CREATE TABLE employees (
    id         INTEGER PRIMARY KEY,
    name       TEXT    NOT NULL,
    email      TEXT    NOT NULL,
    dept_id    INTEGER REFERENCES departments(id),
    manager_id INTEGER REFERENCES employees(id),
    salary     INTEGER NOT NULL,
    hired_on   TEXT    NOT NULL
);
INSERT INTO departments VALUES (1, 'Eng'), (2, 'Sales'), (3, 'Legal');
INSERT INTO employees VALUES
    (1, 'Ada',  'ada@x.com', 1,    NULL, 200, '2020-01-01'),
    (2, 'Bob',  'bob@x.com', 1,    1,    150, '2021-03-01'),
    (3, 'Cy',   'cy@x.com',  1,    1,    150, '2021-06-01'),
    (4, 'Di',   'di@x.com',  2,    1,    120, '2022-01-01'),
    (5, 'Ed',   'ed@x.com',  2,    4,     90, '2023-01-01'),
    (6, 'Bob2', 'bob@x.com', NULL, 2,     80, '2024-01-01');
```

---

## S1. Explain the join types. How do you find rows with no match? (must know)

- **INNER JOIN** keeps only matching pairs; **LEFT JOIN** keeps every left row and fills the right side with NULL; **FULL OUTER JOIN** keeps unmatched rows from both sides; **CROSS JOIN** is the Cartesian product.
- An **anti-join** ("departments with no employees") is `NOT EXISTS` or `LEFT JOIN ... WHERE right.id IS NULL`.
- Never write the anti-join as `NOT IN (subquery)` when the subquery column is nullable: one NULL makes the whole predicate unknown and the query returns nothing.

```sql
-- Anti-join, the safe forms: both return 'Legal'
SELECT d.name FROM departments d
WHERE NOT EXISTS (SELECT 1 FROM employees e WHERE e.dept_id = d.id);

SELECT d.name FROM departments d
LEFT JOIN employees e ON e.dept_id = d.id
WHERE e.id IS NULL;

-- The trap: Bob2 has dept_id NULL, so this returns zero rows
SELECT d.name FROM departments d
WHERE d.id NOT IN (SELECT e.dept_id FROM employees e);
```

On the fixture: inner join 5 rows, left join 6 (Bob2 with a NULL department), full outer join 7 (plus Legal with no employee).

Pitfalls they probe:

- A filter on the right table in `WHERE` silently turns a LEFT JOIN into an INNER JOIN; put it in the `ON` clause instead.
- Joining two one-to-many children from the same parent multiplies rows (fan-out); `SUM` then double counts.
  Aggregate each child in a subquery or CTE first, then join.
- MySQL has no `FULL OUTER JOIN`; emulate it with `LEFT JOIN ... UNION ... RIGHT JOIN`.
  PostgreSQL, SQL Server, and SQLite 3.39+ support it.

---

## S2. Find the second highest salary. Then the Nth highest. (must know)

```sql
-- 1. OFFSET, wrapped in a scalar subquery so "no second salary" returns NULL, not zero rows
SELECT (SELECT DISTINCT salary FROM employees
        ORDER BY salary DESC LIMIT 1 OFFSET 1) AS second_highest;      -- 150

-- 2. MAX below MAX: portable to every engine
SELECT MAX(salary) AS second_highest FROM employees
WHERE salary < (SELECT MAX(salary) FROM employees);                    -- 150

-- 3. Nth highest with DENSE_RANK (ties share a rank; :n = 3 gives 120)
SELECT DISTINCT salary FROM (
    SELECT salary, DENSE_RANK() OVER (ORDER BY salary DESC) AS rnk FROM employees
) AS ranked WHERE rnk = :n;
```

- Say out loud: `DISTINCT` matters because Bob and Cy both earn 150; without it `OFFSET 1` returns 150 for the wrong reason and `OFFSET 2` returns 150 again.
- Dialects: SQL Server uses `ORDER BY salary DESC OFFSET 1 ROWS FETCH NEXT 1 ROWS ONLY` (or `TOP`); PostgreSQL and MySQL accept `LIMIT 1 OFFSET 1`.
- Follow-up: "per department" turns this into S3.

---

## S3. Return the top N earners per department. Explain ROW_NUMBER vs RANK vs DENSE_RANK. (must know)

```sql
WITH ranked AS (
    SELECT e.*, DENSE_RANK() OVER (PARTITION BY dept_id ORDER BY salary DESC) AS rnk
    FROM employees e WHERE dept_id IS NOT NULL
)
SELECT dept_id, name, salary FROM ranked
WHERE rnk <= :n
ORDER BY dept_id, salary DESC, id;
-- :n = 2 -> Eng: Ada, Bob, Cy (tie at 150); Sales: Di, Ed
```

For salaries 200, 150, 150, 120, 90:

| Function | Result | Use when |
| --- | --- | --- |
| `ROW_NUMBER()` | 1, 2, 3, 4, 5 | Exactly N rows per group; add a tie-breaker (`ORDER BY salary DESC, id`) or the result is nondeterministic |
| `RANK()` | 1, 2, 2, 4, 5 | Olympic ranking; gaps after ties |
| `DENSE_RANK()` | 1, 2, 2, 3, 4 | "Top N distinct values", no gaps |

- You cannot filter on a window function in `WHERE` of the same query, because windows are computed after `WHERE`; wrap it in a CTE or subquery.
- PostgreSQL shortcut for top 1 per group: `SELECT DISTINCT ON (dept_id) * FROM employees ORDER BY dept_id, salary DESC`.
- At scale, an index on `(dept_id, salary DESC)` lets the database read each group in order instead of sorting the table.

---

## S4. Find duplicate rows, then delete all but one.

```sql
-- Find
SELECT email, COUNT(*) AS n FROM employees
GROUP BY email HAVING COUNT(*) > 1;                   -- ('bob@x.com', 2)

-- Delete, keeping the lowest id per email
DELETE FROM employees
WHERE id IN (
    SELECT id FROM (
        SELECT id, ROW_NUMBER() OVER (PARTITION BY email ORDER BY id) AS rn
        FROM employees
    ) AS t WHERE rn > 1
);                                                    -- deletes id 6
```

- The same statement runs on PostgreSQL and SQLite.
  MySQL rejects a subquery that reads the table being deleted from (error 1093) unless it is wrapped in a derived table, as above, or written as a self-join `DELETE e1 FROM employees e1 JOIN employees e2 ON e1.email = e2.email AND e1.id > e2.id`.
- PostgreSQL alternative: `DELETE FROM employees a USING employees b WHERE a.email = b.email AND a.id > b.id`.
- Production angle: delete in batches on a big table, then add the `UNIQUE` constraint so duplicates cannot come back.
  The fix without the constraint is only a cleanup.
- For case or whitespace duplicates, partition by `lower(trim(email))`.

---

## S5. WHERE vs HAVING, and what else can window functions do?

- `WHERE` filters rows before grouping; `HAVING` filters groups after aggregation, so it can reference `COUNT(*)` or `AVG(...)`.
- Logical order of evaluation: `FROM`/`JOIN` -> `WHERE` -> `GROUP BY` -> `HAVING` -> window functions -> `SELECT` -> `DISTINCT` -> `ORDER BY` -> `LIMIT`.
  That order explains why a `SELECT` alias cannot be used in `WHERE` (PostgreSQL allows it in `ORDER BY`, and MySQL also in `HAVING`).
- Window functions compute across related rows without collapsing them: running totals, moving averages, previous-row deltas.

```sql
SELECT d.name, COUNT(*) AS headcount, AVG(e.salary) AS avg_salary
FROM employees e JOIN departments d ON d.id = e.dept_id
GROUP BY d.name
HAVING COUNT(*) >= 2 AND AVG(e.salary) > 100;          -- Eng (3, 166.67), Sales (2, 105)

-- Running total of payroll by hire date: 200, 350, 500, 620, 710, 790
SELECT name, hired_on, salary,
       SUM(salary) OVER (ORDER BY hired_on
                         ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) AS payroll_to_date
FROM employees ORDER BY hired_on;

-- Salary change vs the previous hire in the same department
SELECT name, dept_id, salary,
       salary - LAG(salary) OVER (PARTITION BY dept_id ORDER BY hired_on) AS diff_vs_prev_hire
FROM employees WHERE dept_id IS NOT NULL;
```

- Trick question: with `ORDER BY` but no frame clause, the default frame is `RANGE BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW`, which lumps ties together; spell out `ROWS` when rows share an order key.
- `COUNT(col)` skips NULLs; `COUNT(*)` does not.

---

## S6. What is a CTE? Write a recursive one.

- A CTE (`WITH name AS (...)`) names a subquery for readability and reuse inside one statement.
- PostgreSQL 12+ inlines a non-recursive CTE referenced once; `AS MATERIALIZED` forces the old optimization-fence behavior.
- A recursive CTE has an anchor member, `UNION ALL`, and a recursive member that joins back to the CTE; use it for org charts, category trees, and bill-of-materials.

```sql
WITH RECURSIVE chain(id, name, depth, path) AS (
    SELECT id, name, 0, name FROM employees WHERE manager_id IS NULL
    UNION ALL
    SELECT e.id, e.name, c.depth + 1, c.path || ' > ' || e.name
    FROM employees e JOIN chain c ON e.manager_id = c.id
)
SELECT name, depth, path FROM chain ORDER BY path;
-- Ada 0 'Ada'; Bob 1 'Ada > Bob'; Bob2 2 'Ada > Bob > Bob2'; ...; Ed 2 'Ada > Di > Ed'
```

- Guard against cycles in dirty data: carry a path array and stop when the next id is already in it (PostgreSQL 14+ also has a `CYCLE` clause), or cap `depth`.
- SQL Server spells it `WITH chain AS (...)` without `RECURSIVE` and concatenates with `+`.

---

## S7. How do indexes work? How do you choose composite column order, and when is an index not used? (must know)

- The default index is a **B-tree**: a balanced, sorted tree giving O(log n) lookup, range scans, and ordered reads.
- A composite index on `(a, b)` is sorted by `a`, then `b` within each `a`: it serves `a = ?`, `a = ? AND b > ?`, and `ORDER BY a, b`, but not `b = ?` alone (**leftmost prefix** rule).
- Order columns as **equality first, then range, then sort**; after the first range condition, later columns cannot narrow the seek.
- A **covering** index contains every column the query needs, so the table is never read (PostgreSQL "Index Only Scan", `INCLUDE (...)` in PostgreSQL 11+ and SQL Server).
- A **partial** index (`WHERE status = 'queued'`) indexes only hot rows; SQL Server calls it a filtered index; MySQL has no equivalent.
- Every index costs write amplification, storage, and cache; index for real query patterns, not every column.

Verified with SQLite's `EXPLAIN QUERY PLAN` after `CREATE INDEX ix_emp_dept_salary ON employees (dept_id, salary)`:

| Query | Plan |
| --- | --- |
| `WHERE dept_id = ? AND salary > ?` | `SEARCH employees USING INDEX ix_emp_dept_salary (dept_id=? AND salary>?)` |
| `WHERE salary > ?` (second column only) | `SCAN employees` |
| `SELECT salary ... WHERE dept_id = ?` | `SEARCH employees USING COVERING INDEX ix_emp_dept_salary (dept_id=?)` |
| `WHERE lower(email) = ?` with an index on `email` | `SCAN employees` |
| same, after `CREATE INDEX ... ON employees (lower(email))` | `SEARCH employees USING INDEX ix_emp_email_lower (<expr>=?)` |

Why an index is not used:

1. A function or expression on the column (`lower(email)`, `created_at::date`, `amount + 0`): index the expression instead.
2. Implicit type conversion, for example comparing a MySQL `VARCHAR` column to a number.
3. A leading wildcard `LIKE '%abc'` (use trigram `pg_trgm` or full-text search); in PostgreSQL, `LIKE 'abc%'` needs `text_pattern_ops` unless the collation is `C`.
4. Low selectivity: fetching a large fraction of the table is cheaper as a sequential scan, and the planner is right to choose it.
5. The query does not use the leftmost column; some engines can skip-scan when the leading column has few distinct values, but do not design for it.
6. Stale statistics make the planner misestimate row counts; run `ANALYZE`.
7. `OR` across different columns (PostgreSQL may combine two indexes with a BitmapOr; MySQL may use index merge; often a `UNION` is clearer).

Follow-ups: hash indexes (equality only), GIN (JSONB, arrays, full text), GiST (geometry: PostGIS), BRIN (huge append-only time series, tiny index).
[fill in: an index you added or removed at LogiNext or Bank of America and what it changed]

---

## S8. A query is slow. How do you read EXPLAIN and EXPLAIN ANALYZE?

- `EXPLAIN` shows the planner's chosen plan with **estimated** cost and rows; `EXPLAIN ANALYZE` **runs** the query and adds actual time, actual rows, and loops.
- `EXPLAIN (ANALYZE, BUFFERS)` in PostgreSQL adds shared buffer hits and reads, which shows whether the time is I/O.
- Because `EXPLAIN ANALYZE` executes the statement, wrap an `UPDATE` or `DELETE` in `BEGIN; ... ROLLBACK;`.

What to look for, bottom-up (the innermost node runs first):

| Symptom in the plan | Usual meaning | Fix |
| --- | --- | --- |
| `Seq Scan` on a large table with a selective filter | Missing or unusable index | Add or fix the index (S7) |
| Estimated rows far from actual rows | Stale or insufficient statistics, correlated columns | `ANALYZE`; extended statistics (`CREATE STATISTICS`) |
| `Nested Loop` with a huge `loops=` count on the inner side | Planner expected few outer rows | Fix the estimate, index the join key |
| `Sort Method: external merge  Disk: ...` | Sort spilled to disk | Index for the `ORDER BY`, or raise `work_mem` for that query |
| `Rows Removed by Filter` large | Index found too many rows, the filter did the work | Better composite index |
| `Index Scan` then many heap fetches | Not covering | `INCLUDE` columns or narrow the select list |

A plan skeleton to recognize (numbers elided on purpose):

```text
Limit  (cost=... rows=10) (actual time=... rows=10 loops=1)
  ->  Index Scan using ix_orders_customer_created on orders  (actual time=... rows=10 loops=1)
        Index Cond: (customer_id = 42)
Planning Time: ...
Execution Time: ...
```

Workflow: find the slow query with `pg_stat_statements` (total time, not just mean), reproduce with production-like data and parameters, read the plan, change one thing, re-measure.
SQLite equivalent is `EXPLAIN QUERY PLAN`, MySQL has `EXPLAIN ANALYZE` since 8.0.18, SQL Server shows the actual execution plan.

---

## S9. Explain ACID, isolation levels, and the anomalies each one allows. What are the defaults? (must know)

- **Atomicity** (all or nothing, via the log), **Consistency** (constraints hold), **Isolation** (concurrent transactions do not see each other's partial work), **Durability** (committed data survives a crash, via the write-ahead log fsync).
- Isolation is a dial between correctness and concurrency; the defaults are **not** serializable.
- Defaults: PostgreSQL **Read Committed**; MySQL InnoDB **Repeatable Read**; SQL Server **Read Committed** (lock-based unless `READ_COMMITTED_SNAPSHOT` is ON; it is ON by default in Azure SQL Database); Oracle Read Committed.

Anomalies:

- **Dirty read:** you see another transaction's uncommitted write.
- **Non-repeatable read:** re-reading a row returns a different committed value.
- **Phantom:** re-running a range query returns new rows.
- **Lost update:** two read-modify-write cycles overwrite each other.
- **Write skew:** two transactions read the same data, each updates a different row, and together they break an invariant ("at least one doctor on call").

PostgreSQL behavior, which is what you should quote:

| Level | Dirty | Non-repeatable | Phantom | Lost update | Write skew |
| --- | --- | --- | --- | --- | --- |
| Read Uncommitted | No (treated as Read Committed) | Yes | Yes | Yes | Yes |
| Read Committed (default) | No | Yes | Yes | Yes | Yes |
| Repeatable Read (snapshot) | No | No | No | No: the second writer gets a serialization error | Yes |
| Serializable (SSI) | No | No | No | No | No |

- In PostgreSQL, Repeatable Read and Serializable raise `SQLSTATE 40001` (serialization failure); the application **must retry** the whole transaction.
- MySQL Repeatable Read: plain `SELECT` reads a snapshot, but `UPDATE` and locking reads act on the latest committed row with next-key (gap) locks, so a read-then-write computed in Python can still lose an update.
- SQL Server `SNAPSHOT` isolation is separate from RCSI and needs `ALLOW_SNAPSHOT_ISOLATION ON`.

Fixes for lost update, cheapest first:

1. Make it one atomic statement: `UPDATE accounts SET balance = balance - :amt WHERE id = :id AND balance >= :amt` and check the row count.
2. Pessimistic: `SELECT ... FOR UPDATE` then update (S10).
3. Optimistic: a version column (S10).
4. Raise isolation to Repeatable Read or Serializable and retry on 40001.

Setting it in SQLAlchemy: `create_engine(url, isolation_level="REPEATABLE READ")` or per connection `conn.execution_options(isolation_level="SERIALIZABLE")`.

---

## S10. Pessimistic vs optimistic locking. How do you implement each with SQLAlchemy?

- **Pessimistic:** lock the row when you read it (`SELECT ... FOR UPDATE`), so others block until you commit; right when conflicts are frequent and the critical section is short.
- **Optimistic:** read without locks, then update `WHERE version = :seen` and fail if zero rows matched; right when conflicts are rare or the "transaction" spans user think time (edit form, HTTP round trips).
- In SQLAlchemy, `select(...).with_for_update()` gives pessimistic locking, and `__mapper_args__ = {"version_id_col": version}` gives optimistic locking with a `StaleDataError` on conflict.

```python
from sqlalchemy.orm import DeclarativeBase, Mapped, mapped_column


class Base(DeclarativeBase):
    pass


class Account(Base):
    """Optimistic locking: every UPDATE adds `AND version = :old` and bumps it."""

    __tablename__ = "accounts"

    id: Mapped[int] = mapped_column(primary_key=True)
    balance: Mapped[int]
    version: Mapped[int] = mapped_column(nullable=False)

    __mapper_args__ = {"version_id_col": version}
```

```python
from sqlalchemy.orm import Session
from sqlalchemy.orm.exc import StaleDataError


def test_version_id_col_detects_lost_update(engine):
    with Session(engine) as s, s.begin():
        s.add(Account(id=1, balance=100))
    s1, s2 = Session(engine), Session(engine)
    a1, a2 = s1.get(Account, 1), s2.get(Account, 1)
    assert a1.version == a2.version == 1
    a1.balance += 50
    s1.commit()                                      # version -> 2
    a2.balance -= 30
    with pytest.raises(StaleDataError):
        s2.commit()                                  # UPDATE ... WHERE version = 1 matched 0 rows
    s2.rollback()
```

- In an API, map `StaleDataError` to **409 Conflict**, or expose the version as an `ETag` and require `If-Match` (412 Precondition Failed on mismatch).
- Pessimistic variants: `with_for_update(nowait=True)` fails immediately instead of waiting; `with_for_update(read=True)` is `FOR SHARE`; `of=Table` limits which tables in a join are locked.
- Never hold a `FOR UPDATE` lock across a network call to a third party; that is how a slow dependency becomes a database outage.

---

## S11. How do you build a job queue on PostgreSQL? How do you avoid and handle deadlocks?

- `FOR UPDATE SKIP LOCKED` lets many workers each claim different rows without blocking on each other; it is the standard "queue in PostgreSQL" pattern (MySQL 8.0 and Oracle support it too; SQL Server uses `WITH (READPAST, UPDLOCK)`).
- Claim, mark as running, and commit in one short transaction; do the actual work outside it; record the result in another transaction.

```python
from sqlalchemy import select

from models import Job   # id, status (indexed), payload


def claim_jobs_stmt(batch: int = 10):
    return (
        select(Job)
        .where(Job.status == "queued")
        .order_by(Job.id)
        .limit(batch)
        .with_for_update(skip_locked=True)
    )


# Compiled for PostgreSQL, the statement ends with "FOR UPDATE SKIP LOCKED".
# The claim loop below also ran on SQLite, which ignores FOR UPDATE.
with SessionLocal() as session, session.begin():
    for job in session.scalars(claim_jobs_stmt()):
        job.status = "running"
```

The same thing in one PostgreSQL statement (not executed here):

```sql
WITH next AS (
    SELECT id FROM jobs WHERE status = 'queued'
    ORDER BY id LIMIT 10
    FOR UPDATE SKIP LOCKED
)
UPDATE jobs SET status = 'running', started_at = now()
FROM next WHERE jobs.id = next.id
RETURNING jobs.id;
```

Deadlocks:

- A deadlock is a cycle: T1 locks row A then wants B, while T2 holds B and wants A.
  PostgreSQL detects it after `deadlock_timeout` (default 1s) and aborts one transaction with `SQLSTATE 40P01`.
- Avoid them by locking in a consistent order (sort ids before `FOR UPDATE`), keeping transactions short, never doing network I/O inside a transaction, and updating parent and child tables in the same order everywhere.
- Handle them by retrying the whole transaction with jittered backoff on 40P01 and 40001, and set `lock_timeout` and `statement_timeout` so a stuck lock fails fast instead of piling up connections.
- Diagnose with the server log (PostgreSQL logs both queries in the deadlock report), `pg_locks` joined to `pg_stat_activity`, and `log_lock_waits = on`.
- Production angle: jobs claimed by a worker that then crashes stay "running" forever; add a `locked_until` lease and a reaper, or use a real broker (see [Microservices and Messaging](08-Microservices-and-Messaging.md)).

---

## S12. Normalization vs denormalization: when do you break the rules?

- **1NF:** atomic values, no repeating groups; **2NF:** no attribute depends on part of a composite key; **3NF:** no attribute depends on another non-key attribute (no transitive dependency); **BCNF:** every determinant is a candidate key.
- Normalize for write correctness: one fact in one place, so no update anomalies.
- Denormalize deliberately for read performance, with a named mechanism that keeps the copy correct.

| Technique | Keeps it correct by | Cost |
| --- | --- | --- |
| Materialized view | `REFRESH MATERIALIZED VIEW CONCURRENTLY` on a schedule | Staleness window |
| Counter or summary column | Same transaction or trigger | Hot-row contention |
| JSONB snapshot (order line keeps the price at purchase time) | It is a historical fact, not a copy | None; this is correct modeling |
| Read model fed by events (CQRS) | Consumer of change events | Eventual consistency, replay tooling |
| Cache (S24) | TTL and invalidation | Staleness, stampedes |

- Trading and payments flavor: store the executed price and quantity on the trade row even if a reference table has them; that is history, not duplication.
  [fill in: a denormalization you did for FICC trade queries or LogiNext tracking and how you kept it consistent]

---

## S13. SQLAlchemy Core vs ORM. What are Engine, Connection, and Session?

- **Core** is the SQL expression language (`Table`, `select`, `insert`) returning `Row` tuples; **ORM** maps classes to tables and adds the `Session`, unit of work, identity map, and relationships.
- In 2.x both use the same `select()`; `session.execute(select(User))` and `connection.execute(select(users_table))` look alike.
- **Engine:** one per database per process; owns the dialect and the connection pool; thread-safe; create it once at startup.
- **Connection:** a checked-out DBAPI connection with transaction control (`engine.begin()` gives commit-on-success, rollback-on-error).
- **Session:** the ORM unit of work; not thread-safe; one per request, task, or job; acquires a connection per transaction and returns it to the pool on commit, rollback, or close.

A sketch (PostgreSQL URL, not executed here):

```python
from sqlalchemy import create_engine, select, text
from sqlalchemy.orm import sessionmaker

engine = create_engine("postgresql+psycopg://app@db/app", pool_pre_ping=True)
SessionLocal = sessionmaker(engine, expire_on_commit=False)

with engine.begin() as conn:                                  # Core
    conn.execute(text("UPDATE counters SET n = n + 1 WHERE id = :id"), {"id": 1})

with SessionLocal() as session:                                # ORM
    user = session.scalars(select(User).where(User.email == email)).one_or_none()
```

- Use Core for set-based bulk operations, reporting queries, and hot paths; use the ORM for domain logic with relationships and change tracking.
- Trick question: "Is `Session` thread-safe?" No; neither is `AsyncSession` safe to share between concurrent tasks (`asyncio.gather` over one session fails).
- 2.x changes interviewers check: `Session.query()` is legacy (still works); `select()` + `session.scalars()` is the style; there is no autocommit mode; `future=True` is gone because 2.0 behavior is the only behavior.

---

## S14. Walk through the Session lifecycle: unit of work, identity map, flush vs commit, expire_on_commit, DetachedInstanceError. (must know)

- **Unit of work:** the Session tracks new, dirty, and deleted objects and writes them in dependency order at **flush**.
- **Flush** sends pending INSERT, UPDATE, and DELETE statements inside the open transaction (autoflush runs before each query); **commit** flushes, then commits, then by default **expires** every loaded attribute.
- **Identity map:** within one Session, one database row is one Python object; `session.get(User, 1)` twice returns the same object and the second call emits no SQL.
- **States:** transient (new object) -> pending (`add`) -> persistent (flushed) -> detached (session closed) or deleted.
- **`DetachedInstanceError`:** you touched an expired or unloaded attribute on an object whose Session is closed, typically in a template, a serializer, or a background task.

All verified in tests:

```python
def test_identity_map_returns_same_object_without_sql(engine):
    with Session(engine) as s:
        a1 = s.get(Author, 1)
        with count_queries(engine) as stmts:
            a2 = s.get(Author, 1)                    # served from the identity map
        assert a1 is a2 and stmts == []
        a3 = s.scalars(select(Author).where(Author.id == 1)).one()
        assert a3 is a1                              # a query still returns the same instance


def test_flush_vs_commit(engine):
    with Session(engine) as s:
        a = Author(name="flushed")
        s.add(a)
        assert a.id is None
        s.flush()                                    # INSERT sent, transaction still open
        assert a.id is not None
        s.rollback()
    with Session(engine) as s:
        assert s.scalars(select(Author).where(Author.name == "flushed")).first() is None


def test_expire_on_commit_and_detached_instance(engine):
    with Session(engine) as s:
        a = s.scalars(select(Author)).first()
        s.commit()                                   # expires every loaded attribute
    with pytest.raises(DetachedInstanceError):
        a.name                                       # needs a refresh, but no session

    SessionLocal = sessionmaker(engine, expire_on_commit=False)
    with SessionLocal() as s:
        a = s.scalars(select(Author)).first()
        s.commit()
    assert a.name == "author0"                       # loaded value survives
    with pytest.raises(DetachedInstanceError):
        a.books                                      # an unloaded lazy relationship still fails
```

- Web apps usually set `expire_on_commit=False` so the handler can serialize the object after commit; the cost is that you may return values that another transaction has since changed.
- The identity map is **not** a cache across requests; it lives and dies with the Session.
- A query returning an object already in the identity map does not overwrite your unflushed changes; `session.refresh(obj)` or `populate_existing()` forces a reload.
- Fix for `DetachedInstanceError`: load what you need before closing (`selectinload`), convert to a Pydantic model inside the session, or use `expire_on_commit=False`; do not reattach objects across threads with `merge()` as a habit.

---

## S15. How do you manage sessions per request in Flask and in FastAPI?

- One Session per request, created lazily, always closed at the end, with commit explicit in the handler or service layer.
- **Flask:** store the Session on `g` and close it in `@app.teardown_appcontext` (Flask-SQLAlchemy does the same with a `scoped_session` under `db.session`).
- **FastAPI:** a `yield` dependency; the code after `yield` is the cleanup.
- Commit **inside** the handler, not after `yield`: with the default dependency scope, code after `yield` runs after the response has been sent, so a failed commit there cannot turn into a 500 (verified below); `Depends(..., scope="function")` moves the cleanup before the response.

```python
from collections.abc import Iterator
from typing import Annotated

from fastapi import Depends, FastAPI, HTTPException
from flask import Flask, current_app, g
from sqlalchemy import create_engine, select
from sqlalchemy.exc import IntegrityError
from sqlalchemy.orm import Session, sessionmaker


# ---------------- Flask: app factory + g + teardown ----------------

def create_flask_app(test_config: dict | None = None) -> Flask:
    app = Flask(__name__)
    app.config.from_mapping(DATABASE_URL="sqlite:///app.db")
    if test_config:
        app.config.update(test_config)

    engine = create_engine(app.config["DATABASE_URL"], pool_pre_ping=True)
    app.extensions["db_sessionmaker"] = sessionmaker(engine, expire_on_commit=False)

    @app.teardown_appcontext
    def close_session(exc: BaseException | None) -> None:
        session = g.pop("db_session", None)
        if session is not None:
            session.close()                          # rolls back anything uncommitted

    @app.post("/authors/<name>")
    def create_author(name: str):
        db = get_flask_session()
        db.add(Author(name=name))
        try:
            db.commit()
        except IntegrityError:
            db.rollback()
            return {"error": "exists"}, 409
        return {"name": name}, 201

    return app


def get_flask_session() -> Session:
    if "db_session" not in g:
        g.db_session = current_app.extensions["db_sessionmaker"]()
    return g.db_session


# ---------------- FastAPI: yield dependency ----------------

def create_fastapi_app(database_url: str) -> FastAPI:
    engine = create_engine(database_url, pool_pre_ping=True)
    SessionLocal = sessionmaker(engine, expire_on_commit=False)

    def get_session() -> Iterator[Session]:
        with SessionLocal() as session:              # closed after the request
            yield session

    SessionDep = Annotated[Session, Depends(get_session)]
    app = FastAPI()

    @app.post("/authors/{name}", status_code=201)
    def create_author(name: str, session: SessionDep) -> dict:
        session.add(Author(name=name))
        try:
            session.commit()                         # commit inside the handler, not after yield
        except IntegrityError:
            session.rollback()
            raise HTTPException(status_code=409, detail="exists")
        return {"name": name}

    return app
```

The timing test (FastAPI 0.142):

```python
def failing_after_yield():
    yield
    raise RuntimeError("commit failed")

# GET /default with Depends(failing_after_yield)                   -> 200, error happens after the response
# GET /function with Depends(failing_after_yield, scope="function") -> 500
```

- Sync `def` endpoints run in a thread pool, so a sync Session is fine there; in an `async def` endpoint use `AsyncSession` (S20), never a sync Session, or you block the event loop.
- Tests swap the dependency with `app.dependency_overrides[get_session]`; see [Testing, Debugging, Production](10-Testing-Debugging-Production.md).
- Framework specifics are in [Flask](03-Flask.md) and [FastAPI](04-FastAPI.md).

---

## S16. What is the N+1 problem, and how do SQLAlchemy loading strategies fix it? (must know)

- N+1: one query loads N parents, then touching a lazy relationship on each parent fires one more query per parent.
- It hides in serializers and templates, passes tests on 3 rows, and melts production at 3,000.
- Fix by choosing the loader per query: `selectinload` for collections, `joinedload` for many-to-one, and `raiseload` to make any accidental lazy load an error.
- Prove it with a query counter in tests, not by reading SQL logs.

```python
from collections.abc import Iterator
from contextlib import contextmanager

from sqlalchemy import Engine, event


@contextmanager
def count_queries(engine: Engine) -> Iterator[list[str]]:
    """Collect every SQL statement the engine sends to the DBAPI cursor."""
    statements: list[str] = []

    def before_cursor_execute(conn, cursor, statement, parameters, context, executemany):
        statements.append(statement)

    event.listen(engine, "before_cursor_execute", before_cursor_execute)
    try:
        yield statements
    finally:
        event.remove(engine, "before_cursor_execute", before_cursor_execute)
```

With 3 authors and 2 books each:

```python
def test_n_plus_one_lazy(engine):
    with Session(engine) as s, count_queries(engine) as stmts:
        assert sum(len(a.books) for a in s.scalars(select(Author))) == 6
    assert len(stmts) == 1 + 3                       # 1 for authors + 1 per author


def test_selectinload_is_two_queries(engine):
    with Session(engine) as s, count_queries(engine) as stmts:
        authors = s.scalars(select(Author).options(selectinload(Author.books))).all()
        assert sum(len(a.books) for a in authors) == 6
    assert len(stmts) == 2
    assert " IN (" in stmts[1]


def test_joinedload_is_one_query_and_needs_unique(engine):
    stmt = select(Author).options(joinedload(Author.books))
    with Session(engine) as s:
        with pytest.raises(InvalidRequestError, match="unique"):
            s.scalars(stmt).all()                    # collection joinedload requires .unique()
    with Session(engine) as s, count_queries(engine) as stmts:
        authors = s.scalars(stmt).unique().all()
        assert len(authors) == 3 and sum(len(a.books) for a in authors) == 6
    assert len(stmts) == 1
    assert "LEFT OUTER JOIN" in stmts[0]


def test_raiseload_turns_n_plus_one_into_an_error(engine):
    with Session(engine) as s:
        a = s.scalars(select(Author).options(raiseload("*"))).first()
        with pytest.raises(InvalidRequestError, match="raise"):
            a.books
```

| Strategy | Queries | Best for | Watch out |
| --- | --- | --- | --- |
| `lazy="select"` (default) | 1 + N | Rarely touched relationships | N+1; fails when detached or under asyncio |
| `selectinload` | 1 + 1 per relationship (IN batches of 500 keys) | One-to-many collections | Very large parent sets mean many IN batches |
| `joinedload` | 1 | Many-to-one, small collections | Row explosion with several collections; collections need `.unique()` |
| `subqueryload` | 1 + 1 | Legacy | Prefer `selectinload` |
| `raiseload` / `lazy="raise"` | Error | Enforcing explicit loading in APIs | Must opt in per query or per relationship |
| `lazy="write_only"` | Explicit query per access | Huge collections | You query it yourself, no `len()` |

- Put `lazy="raise"` on relationships in API services and load explicitly per endpoint; the N+1 then fails in tests, not in production.
- Row explosion example: `joinedload(Author.books), joinedload(Author.awards)` returns books x awards rows per author.

---

## S17. How do relationships and cascades work? What does delete-orphan do?

- `relationship()` with `back_populates` on both sides keeps both sides of the Python object graph in sync; the foreign key lives on the many side.
- `cascade="all, delete-orphan"` means: operations on the parent (add, merge, delete) propagate to children, and a child removed from the collection is deleted, not just un-linked.
- ORM cascades run in Python and only for loaded objects; `ForeignKey(..., ondelete="CASCADE")` plus `passive_deletes=True` lets the database do it in one statement.

```python
class Author(Base):
    __tablename__ = "authors"

    id: Mapped[int] = mapped_column(primary_key=True)
    name: Mapped[str] = mapped_column(String(100), unique=True)
    books: Mapped[list["Book"]] = relationship(
        back_populates="author", cascade="all, delete-orphan"
    )


class Book(Base):
    __tablename__ = "books"

    id: Mapped[int] = mapped_column(primary_key=True)
    title: Mapped[str] = mapped_column(String(200))
    author_id: Mapped[int] = mapped_column(ForeignKey("authors.id"), index=True)
    author: Mapped[Author] = relationship(back_populates="books")


def test_cascade_delete_orphan(engine):
    with Session(engine) as s, s.begin():
        a = s.get(Author, 1)
        a.books.pop()                                # orphan -> DELETE
    with Session(engine) as s, s.begin():
        assert len(s.get(Author, 1).books) == 1
        s.delete(s.get(Author, 1))                   # cascades to remaining books
    with Session(engine) as s:
        assert s.scalars(select(Book).where(Book.author_id == 1)).all() == []
```

- Index foreign-key columns yourself: PostgreSQL does not index the referencing side automatically, and unindexed FKs make parent deletes scan the child table.
- Many-to-many uses `secondary=association_table`; if the link carries data (role, created_at), promote it to an association object.
- Delete-orphan on a many-to-one or many-to-many side is usually a bug; SQLAlchemy requires `single_parent=True` for it.

---

## S18. How does connection pooling work, and how do you size pools across workers and Kubernetes replicas? (must know)

- The SQLAlchemy Engine owns a `QueuePool`: `pool_size=5` persistent connections, up to `max_overflow=10` extra, and callers wait `pool_timeout=30` seconds before `TimeoutError: QueuePool limit of size 5 overflow 10 reached, connection timed out, timeout 30.00`.
- `pool_pre_ping=True` tests a connection on checkout (a cheap round trip) so connections killed by a failover, a firewall, or a proxy idle timeout are replaced instead of failing a request.
- `pool_recycle=1800` closes connections older than N seconds, to stay under server or load balancer idle limits (MySQL `wait_timeout`, cloud NAT, PgBouncer `server_idle_timeout`).
- The pool is **per process**: every gunicorn or uvicorn worker has its own Engine and pool.
- Async engines use `AsyncAdaptedQueuePool` (verified); SQLite `:memory:` uses `SingletonThreadPool`.

The math interviewers want:

```text
max connections from the app = replicas x workers_per_pod x (pool_size + max_overflow)
must stay below: max_connections - superuser_reserved_connections - (migrations, cron, admin, replicas)
```

| Setup | Formula | Total |
| --- | --- | --- |
| 8 pods, 4 gunicorn workers, defaults | 8 x 4 x (5 + 10) | 480 |
| PostgreSQL default `max_connections` | | 100 |
| Same fleet, `pool_size=2, max_overflow=1` | 8 x 4 x 3 | 96: fits only if nothing else connects (100 - 3 reserved = 97) |
| Same fleet behind PgBouncer (transaction mode, `default_pool_size=20`) | 480 client connections to PgBouncer; PostgreSQL sees the pooler's server connections | about 20 per database/user pair |

- Size for the **HPA maximum replica count plus rolling-update surge**, not today's replica count.
- Match pool size to real concurrency: a sync gunicorn worker handles one request at a time (1-2 connections); `gthread` with 8 threads needs up to 8; an async worker is limited only by the pool, which then acts as the backpressure.
- A bigger pool is not faster: PostgreSQL throughput peaks at a small multiple of CPU cores; past that you add context switching and lock contention.

PgBouncer:

- **Session mode:** a server connection per client connection; safe but saves little.
- **Transaction mode:** a server connection only during a transaction; the big win, with caveats: session state does not survive between transactions (`SET` without `LOCAL`, session advisory locks, `LISTEN/NOTIFY`, temp tables, `WITH HOLD` cursors).
- Prepared statements under transaction mode: PgBouncer 1.21+ can track protocol-level prepared statements (`max_prepared_statements`); otherwise disable driver-side caching (asyncpg: `prepared_statement_cache_size=0`, and SQLAlchemy documents a unique `prepared_statement_name_func` plus `NullPool` for asyncpg behind PgBouncer).
- With an external pooler, keep the app-side pool small or use `NullPool` so you do not pool twice.

Fork safety: with gunicorn `--preload`, an Engine created before fork shares sockets across workers; create the Engine after fork, or call `engine.dispose(close=False)` in the child (for example in gunicorn's `post_fork` hook).
Kubernetes and gunicorn settings are in [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md).

---

## S19. How do you do bulk inserts, bulk updates, and upserts efficiently?

- Never loop `session.add()` + `commit()` per row: that is one round trip and one transaction per row.
- SQLAlchemy 2.x ORM bulk: `session.execute(insert(Model), list_of_dicts)` uses `executemany` or batched multi-row `VALUES` ("insertmanyvalues"); `session.execute(update(Model), list_of_dicts_with_pk)` is a bulk update by primary key.
- Upsert is dialect-specific: PostgreSQL and SQLite `INSERT ... ON CONFLICT DO UPDATE`, MySQL `INSERT ... ON DUPLICATE KEY UPDATE`, SQL Server `MERGE` (with care).
- For millions of rows in PostgreSQL, `COPY` (psycopg `cursor.copy()`) into a staging table then one `INSERT ... SELECT ... ON CONFLICT` beats everything else.

```python
from sqlalchemy import insert, update
from sqlalchemy.dialects.sqlite import insert as sqlite_insert
from sqlalchemy.dialects.postgresql import insert as pg_insert


def test_orm_bulk_insert_and_bulk_update(engine):
    rows = [{"title": f"bulk{i}", "author_id": 1} for i in range(1000)]
    with Session(engine) as s, count_queries(engine) as stmts:
        s.execute(insert(Book), rows)
        s.execute(update(Book), [{"id": 1, "title": "renamed"}, {"id": 2, "title": "renamed2"}])
        s.commit()
    assert len(stmts) == 2                           # one executemany each, not 1002 statements


def upsert_prices_sqlite(session: Session, rows: list[dict]) -> None:
    stmt = sqlite_insert(Price).values(rows)
    stmt = stmt.on_conflict_do_update(
        index_elements=[Price.sku], set_={"price": stmt.excluded.price}
    )
    session.execute(stmt)


# PostgreSQL: identical API from the postgresql dialect
stmt = pg_insert(Price).values(sku="A", price=10)
stmt = stmt.on_conflict_do_update(index_elements=[Price.sku], set_={"price": stmt.excluded.price})
# compiles to: ... ON CONFLICT (sku) DO UPDATE SET price = excluded.price
```

- Upserting `[{"sku": "A", "price": 1}, {"sku": "B", "price": 2}]` then `[{"sku": "A", "price": 10}, {"sku": "C", "price": 3}]` leaves `{"A": 10, "B": 2, "C": 3}` (tested).
- PostgreSQL rejects one `ON CONFLICT DO UPDATE` statement that touches the same key twice ("cannot affect row a second time"); deduplicate the batch first.
- Chunk huge batches (1,000-10,000 rows per statement) so one failure does not roll back an hour of work and locks stay short.
- Bulk paths skip ORM events and relationship cascades; that is the point, and also the trap.

---

## S20. How does async SQLAlchemy work, and what breaks under asyncio?

- `create_async_engine("postgresql+asyncpg://...")` plus `async_sessionmaker(engine, expire_on_commit=False)`; every I/O call is awaited (`await session.execute(...)`, `await session.commit()`).
- Under the hood SQLAlchemy runs its sync core inside greenlets; SQLAlchemy 2.1 does not install `greenlet` by default, so install `sqlalchemy[asyncio]`.
- **Implicit I/O is forbidden:** touching an unloaded lazy relationship raises `MissingGreenlet` (wrapped in a `StatementError`) because attribute access cannot `await`.
- Fixes: eager load (`selectinload`), `await obj.awaitable_attrs.rel` with the `AsyncAttrs` mixin, `expire_on_commit=False` so attributes are not expired after commit, or `lazy="raise"` to catch it early.

```python
from sqlalchemy.ext.asyncio import AsyncAttrs, async_sessionmaker, create_async_engine
from sqlalchemy.orm import DeclarativeBase


class Base(AsyncAttrs, DeclarativeBase):
    pass

# User.orders is a one-to-many relationship, lazy by default.


@pytest.fixture
async def session_factory(tmp_path):
    engine = create_async_engine(f"sqlite+aiosqlite:///{tmp_path}/app.db")
    async with engine.begin() as conn:
        await conn.run_sync(Base.metadata.create_all)   # DDL helpers are sync; run them in a greenlet
    factory = async_sessionmaker(engine, expire_on_commit=False)
    async with factory() as s, s.begin():
        s.add(User(name="ada", orders=[Order(), Order()]))
    yield factory
    await engine.dispose()


async def test_lazy_load_under_async_raises(session_factory):
    async with session_factory() as s:
        user = await s.scalar(select(User))
        with pytest.raises(StatementError, match="greenlet_spawn has not been called") as exc:
            user.orders                                  # implicit I/O is not allowed under asyncio
        assert isinstance(exc.value.orig, MissingGreenlet)


async def test_eager_load_fixes_it(session_factory):
    async with session_factory() as s:
        user = await s.scalar(select(User).options(selectinload(User.orders)))
        assert len(user.orders) == 2


async def test_awaitable_attrs(session_factory):
    async with session_factory() as s:
        user = await s.scalar(select(User))
        orders = await user.awaitable_attrs.orders       # explicit, awaited lazy load
        assert len(orders) == 2
```

- One `AsyncSession` per task; never share it across `asyncio.gather` branches.
- Async helps when the service waits on many concurrent I/O calls; it does not make one query faster, and CPU-heavy ORM hydration still blocks the loop.
- Call `await engine.dispose()` in the FastAPI `lifespan` shutdown.
- Drivers: `asyncpg` (fast, PostgreSQL only), `psycopg` 3 (sync and async in one driver), `aiomysql`/`asyncmy`, `aiosqlite` for tests.

---

## S21. How do you run raw SQL safely? When do you choose raw SQL over the ORM?

- Use `text()` with **bound parameters** (`:name`) and pass values separately; the driver sends them as parameters, so input is never parsed as SQL.
- Never build SQL with f-strings or `%` from user input; identifiers (table or column names, sort direction) cannot be bound, so map them through an allowlist.
- For `IN` lists, use an expanding bind parameter.

```python
from sqlalchemy import bindparam, text


def test_text_bound_params_block_injection(engine):
    evil = "x' OR '1'='1"
    with Session(engine) as s:
        unsafe = s.execute(text(f"SELECT id FROM authors WHERE name = '{evil}'")).all()
        safe = s.execute(text("SELECT id FROM authors WHERE name = :name"), {"name": evil}).all()
        stmt = text("SELECT name FROM authors WHERE id IN :ids ORDER BY id").bindparams(
            bindparam("ids", expanding=True)
        )
        names = s.scalars(stmt, {"ids": [1, 3]}).all()
    assert len(unsafe) == 3                          # injection returned every row
    assert safe == []
    assert names == ["author0", "author2"]
```

| Choose | When | Cost |
| --- | --- | --- |
| ORM | CRUD with relationships, change tracking, domain logic | Hidden queries (N+1), hydration overhead |
| Core `select()` | Dynamic filters, reporting, bulk set-based updates | No identity map or unit of work |
| `text()` | Vendor features (recursive CTEs with `SKIP LOCKED`, `COPY`, window-heavy reports), tuned hot queries | No composition, schema drift is invisible until runtime |

- Sort by user input safely: `ORDER_COLUMNS = {"created": Order.created_at, "total": Order.total}` then `order_by(ORDER_COLUMNS[key].desc())`, rejecting unknown keys with 422.
- Least privilege helps too: the app's database role should not own the schema or be able to `DROP`.

---

## S22. How do Alembic migrations work, and how do you run them with zero downtime? (must know)

- Alembic keeps a chain of revision scripts (`upgrade()`/`downgrade()`) and records the current head in `alembic_version`.
- `alembic revision --autogenerate` diffs models against the live schema; it detects tables, columns, nullability, indexes, unique constraints, and foreign keys, but not renames (it sees drop + add), not CHECK constraints or server-side defaults reliably, not enum value changes on every backend, and not data migrations.
  Always read and edit the generated script.
- Zero downtime means old and new code run at the same time during a rolling deploy, so every migration must be compatible with both: **expand, migrate, contract**.
  1. **Expand:** add nullable columns, new tables, new indexes; old code ignores them.
  2. **Deploy** code that writes both old and new shapes, then backfill in batches.
  3. **Deploy** code that reads the new shape only.
  4. **Contract:** add constraints, drop old columns, in a later release.
- Renaming a column is add new + dual write + backfill + switch reads + drop old, never `ALTER TABLE ... RENAME` under live traffic.

Adding a NOT NULL column to a large PostgreSQL table safely (rendered with Alembic offline mode for PostgreSQL and checked in tests):

```python
import sqlalchemy as sa
from alembic import op


def expand_add_nullable_column() -> None:
    # Step 1 (deploy N): nullable, no default -> metadata-only, no table rewrite.
    op.add_column("orders", sa.Column("currency", sa.String(3), nullable=True))


def backfill_in_batches() -> None:
    # Step 2 (job, not the migration in a big table): batch by primary key to keep locks short.
    op.execute(
        sa.text(
            "UPDATE orders SET currency = 'USD' "
            "WHERE id IN (SELECT id FROM orders WHERE currency IS NULL LIMIT 5000)"
        )
    )


def contract_set_not_null() -> None:
    # Step 3 (after every writer sets the column): validate without a long exclusive lock.
    op.create_check_constraint(
        "orders_currency_not_null", "orders", "currency IS NOT NULL", postgresql_not_valid=True
    )
    op.execute("ALTER TABLE orders VALIDATE CONSTRAINT orders_currency_not_null")
    op.alter_column("orders", "currency", nullable=False)   # PG 12+ skips the scan
    op.drop_constraint("orders_currency_not_null", "orders", type_="check")


def create_index_concurrently() -> None:
    # CREATE INDEX CONCURRENTLY cannot run inside a transaction block.
    with op.get_context().autocommit_block():
        op.create_index(
            "ix_orders_customer_id", "orders", ["customer_id"],
            postgresql_concurrently=True, if_not_exists=True,
        )
```

Rendered SQL for the last two:

```sql
ALTER TABLE orders ADD CONSTRAINT orders_currency_not_null CHECK (currency IS NOT NULL) NOT VALID;
ALTER TABLE orders VALIDATE CONSTRAINT orders_currency_not_null;
ALTER TABLE orders ALTER COLUMN currency SET NOT NULL;
ALTER TABLE orders DROP CONSTRAINT orders_currency_not_null;

COMMIT;
CREATE INDEX CONCURRENTLY IF NOT EXISTS ix_orders_customer_id ON orders (customer_id);
BEGIN;
```

- Why: `SET NOT NULL` alone scans the whole table under an `ACCESS EXCLUSIVE` lock; `NOT VALID` + `VALIDATE` takes a weaker lock, and PostgreSQL 12+ uses the validated CHECK to skip the scan.
- PostgreSQL 11+ adds a column with a constant default without rewriting the table, so `ADD COLUMN ... DEFAULT 'USD' NOT NULL` is also fast there; a volatile default (`now()`, `random()`) still rewrites.
- A failed `CREATE INDEX CONCURRENTLY` leaves an `INVALID` index; drop it and retry.
- Set `lock_timeout` (for example `SET lock_timeout = '5s'`) in migrations so a DDL statement waiting behind a long transaction fails fast instead of blocking every query queued behind it.
- Run migrations as a separate step (a Kubernetes Job or CI stage), not from every app replica on startup.
- Multiple developers create divergent heads; `alembic merge heads` joins them, and CI should fail on more than one head.

---

## S23. SQL vs NoSQL: when would you pick MongoDB or DynamoDB? How do you design DynamoDB keys?

- Default to PostgreSQL for transactional business data: joins, constraints, transactions, and ad hoc queries.
- Choose **MongoDB** for document-shaped data read and written as a unit, flexible or evolving schemas, and when embedding avoids joins (documents up to 16 MB; multi-document transactions exist since 4.0 but are the exception, not the design).
- Choose **DynamoDB** for known, key-based access patterns at any scale with single-digit-millisecond latency and no servers to run, accepting that you design the table around queries and cannot ad hoc query.

| Need | Relational (PostgreSQL) | MongoDB | DynamoDB |
| --- | --- | --- | --- |
| Ad hoc queries, reporting | Strong | Moderate (aggregation pipeline) | Weak (export to analytics) |
| Multi-entity transactions | Native | Supported, costlier | `TransactWriteItems`, up to 100 items |
| Schema evolution | Migrations | Flexible | Flexible |
| Scale-out writes | Hard (partitioning, Citus) | Sharding | Automatic by partition key |
| Access pattern known up front | Nice to have | Helpful | Mandatory |
| Geospatial | PostGIS (best in class) | 2dsphere indexes | Not native |

DynamoDB key design:

- The **partition key** is hashed to choose a partition; each partition serves up to about 3,000 read units and 1,000 write units per second, so a skewed key (a date, a status, one giant tenant) creates a **hot partition** and throttling even when table capacity looks fine.
- High-cardinality, evenly accessed partition keys (`customer_id`, `order_id`); add a write-sharding suffix (`key#0..9`) for known hot keys.
- The **sort key** enables range queries and hierarchies within one partition: `PK = CUSTOMER#42`, `SK = ORDER#2026-09-29#991`.
- **Single-table design:** several entity types in one table with generic `PK`/`SK` so one `Query` returns a customer and their orders; fast and cheap, but harder to evolve and to read.
- **GSI:** a different key over the same data, eventually consistent only, with its own capacity (a throttled GSI throttles writes to the base table); **LSI:** same partition key, different sort key, must be created with the table, and limits each partition key's item collection to 10 GB.
- Items are limited to 400 KB; `Scan` in a request path is a design smell.

[fill in: why LogiNext used MongoDB next to PostGIS, and what you would keep in each]

---

## S24. How do you cache with Redis? How do you prevent a cache stampede?

- **Cache-aside:** read the cache, on a miss load from the database and populate with a TTL; on write, update the database then **delete** the key.
- Delete instead of set on write, because two racing writers that each "set" can leave the older value in cache.
- Every key needs a TTL, plus random jitter so keys written together do not expire together.
- Cache misses too (negative caching, short TTL) so a flood of lookups for a nonexistent id does not hit the database.
- **Stampede** (thundering herd): a hot key expires and hundreds of requests rebuild it at once; fix with a rebuild lock (`SET key:lock 1 NX EX 10`), stale-while-revalidate, or probabilistic early refresh.

```python
import json
import random
import time
from collections.abc import Callable
from typing import Any

MISSING = "__missing__"          # negative-cache marker for ids that do not exist


def get_cached(
    cache: Any,                    # anything with redis-py's get / set(ex=, nx=) / delete
    key: str,
    load: Callable[[], dict | None],
    ttl: int = 300,
    lock_ttl: int = 10,
    wait_s: float = 0.05,
    max_waits: int = 40,
) -> dict | None:
    raw = cache.get(key)
    if raw is not None:
        value = json.loads(raw)
        return None if value == MISSING else value

    if cache.set(f"{key}:lock", "1", ex=lock_ttl, nx=True):     # only one caller rebuilds
        try:
            value = load()
            jittered = ttl + random.randint(0, ttl // 10)        # spread expiries apart
            cache.set(key, json.dumps(MISSING if value is None else value),
                      ex=jittered if value is not None else 30)
            return value
        finally:
            cache.delete(f"{key}:lock")

    for _ in range(max_waits):                                    # everyone else waits briefly
        time.sleep(wait_s)
        raw = cache.get(key)
        if raw is not None:
            value = json.loads(raw)
            return None if value == MISSING else value
    return load()                                                 # lock holder died: degrade to DB


def update_product(db_write: Callable[[], None], cache: Any, key: str) -> None:
    db_write()            # write the source of truth first
    cache.delete(key)     # then invalidate; never "set" here (racing writers leave stale data)
```

- Tested with a thread-safe fake of the three redis-py methods: 20 concurrent threads on a cold key triggered exactly one `load()`, and a nonexistent id hit the loader once.
- The lock release above is a plain `DELETE`; in production release it only if you still own it (store a token and compare-and-delete in a Lua script), or a slow holder can delete someone else's lock.
- In async FastAPI use `redis.asyncio` and `await` the same calls.
- Other patterns: write-through (write cache and DB together), write-behind (risky), read-through (the cache library loads).
- What goes wrong: unbounded keys without TTL fill memory and trigger eviction (`maxmemory-policy`), a cache outage becomes a database outage (cap fallback concurrency), and a cached authorization decision outlives a revoked permission.

---

## Go deeper

Vault notes:

- [DBMS notes](../../../01-CS-Foundations/DBMS/README.md): fundamentals, [transactions and indexes](../../../01-CS-Foundations/DBMS/notes/04-transactions-indexes.md), [normalization and ACID](../../../01-CS-Foundations/DBMS/notes/03-normalisation-acid.md).
- [SQL joins and aggregation](../../../01-CS-Foundations/DBMS/notes/06-sql-joins-aggregation.md), [subqueries and views](../../../01-CS-Foundations/DBMS/notes/09-subqueries-views.md), [window functions and query optimization](../../../01-CS-Foundations/DBMS/notes/08-window-function-query-optimisation.md).
- [DBMS complete reference](../../../01-CS-Foundations/DBMS/dbms_complete_reference.md), [distributed databases, Python, and CAP](../Python_Zero_to_Godhood/Chapter_74_Distributed_Databases_Python_and_the_CAP_Theorem.md).
- Database internals code: [write-ahead log](../../../08-Distinguished-Engineering/03-Database-Internals/wal.cpp), [LSM tree](../../../08-Distinguished-Engineering/03-Database-Internals/lsm_tree.cpp); [saga pattern](../../../08-Distinguished-Engineering/05-Distributed-Transactions/saga_pattern.md) for transactions across services.
- In this pack: [FastAPI](04-FastAPI.md), [Flask](03-Flask.md), [Testing, Debugging, Production](10-Testing-Debugging-Production.md), [System Design](12-System-Design.md).

Official docs:

- SQLAlchemy 2.1: [Session basics](https://docs.sqlalchemy.org/en/21/orm/session_basics.html), [relationship loading](https://docs.sqlalchemy.org/en/21/orm/queryguide/relationships.html), [pooling](https://docs.sqlalchemy.org/en/21/core/pooling.html), [asyncio](https://docs.sqlalchemy.org/en/21/orm/extensions/asyncio.html), [versioning](https://docs.sqlalchemy.org/en/21/orm/versioning.html).
- [Alembic operations](https://alembic.sqlalchemy.org/en/latest/ops.html).
- PostgreSQL: [transaction isolation](https://www.postgresql.org/docs/current/transaction-iso.html), [explicit locking](https://www.postgresql.org/docs/current/explicit-locking.html), [CREATE INDEX](https://www.postgresql.org/docs/current/sql-createindex.html), [using EXPLAIN](https://www.postgresql.org/docs/current/using-explain.html).
- [MySQL InnoDB isolation levels](https://dev.mysql.com/doc/refman/8.4/en/innodb-transaction-isolation-levels.html), [SQL Server isolation levels](https://learn.microsoft.com/en-us/sql/t-sql/statements/set-transaction-isolation-level-transact-sql).
- [PgBouncer features](https://www.pgbouncer.org/features.html), [DynamoDB partition key design](https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/bp-partition-key-design.html).
