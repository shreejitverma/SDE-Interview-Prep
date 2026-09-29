---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://scrumguides.org/scrum-guide.html, https://12factor.net/]
---

# Behavioral and rapid fire

Behavioral questions for a contract backend role are short and practical: can you work with business analysts and product owners, handle production issues calmly, take code review well, and deliver in sprints.
The rapid-fire section is for the last hour before the interview, and the closing section covers questions to ask and day-of logistics.

## Rules for every story

- 60-90 seconds spoken; Situation and Task in two sentences, most of the time on Action.
- Say "I," not "we," for the actions you took; name what the team did separately.
- End with a result and a lesson; if there is no measured number, say what you observed.
- For a contract role, lean toward stories that show fast onboarding, ownership without authority, and clear communication with non-engineers.

## Story map

| Theme | Primary story | Backup |
| --- | --- | --- |
| Production incident | **[fill in: Bank of America trade processing]** | **[fill in: LogiNext platform]** |
| Worked with business analysts or product owners | **[fill in: requirement clarified with traders or ops at Bank of America]** | LogiNext customer-driven routing requirement **[fill in]** |
| Disagreement in code review | **[fill in]** | Route-rename versus alias decision in the FastAPI application (P2 in [Pitch and Resume](01-Pitch-and-Resume.md)) |
| Large, risky change delivered safely | Python 3.8 migration (P5) | Route prefix rename with aliases (P2) |
| Led or mentored | 12 engineers at LogiNext | **[fill in]** |
| Improved developer productivity | LogiNext LLM debugging tool | CI gate with lint, types, and coverage on the FastAPI application |
| Tight deadline | **[fill in]** | **[fill in]** |
| Learned a new technology fast | **[fill in: for example FastAPI or FPGA work]** | BNP Paribas onboarding onto the market-making stack |

---

## B1. Tell me about a production issue you handled. (must know)

- **Situation and Task:** what broke, who was affected, how you found out.
- **Action:** mitigate first (roll back, feature flag, scale out, shed load), communicate status on a fixed cadence, then find root cause with evidence (logs, metrics, traces, a reproduction).
- **Result:** time to mitigate, time to root cause, and the systemic fix: a test, an alert, a guard, or a runbook.
- **Lesson:** one sentence.
- **[fill in]**

---

## B2. Tell me about working with business analysts, product owners, or non-technical stakeholders. (must know)

- Show that you turned a vague request into a written, testable acceptance criterion before coding.
- Show that you pushed back with options and trade-offs, not a flat no.
- Show how you kept them informed (demo, sprint review, short written update).
- **[fill in]**

---

## B3. Tell me about a disagreement in code review.

- Disagree with evidence (a benchmark, a failing test, a documented standard), not preference.
- Separate blocking issues (correctness, security, data loss) from style; let style go.
- Commit once decided, and follow up on the outcome.
- **[fill in]**

---

## B4. Tell me about a time you delivered under a tight deadline.

- Protect what cannot be undone (data correctness, security, backward compatibility) and trade what can (scope, polish).
- Say what you cut, who agreed, and what you delivered afterward.
- **[fill in]**

---

## B5. Tell me about a time you were wrong.

- A real technical misjudgment; spend most of the time on how you found out and what you changed in your process.
- **[fill in]**

---

## B6. How do you onboard onto an unfamiliar codebase quickly?

Strong answer for a contract role, because they need you productive in the first week.

- Run it locally and run the tests on day one; if either is hard, fix the docs as you go.
- Read the entry points (app factory or `main`, routers, settings, database session setup) and trace one real request end to end.
- Read the last month of merged pull requests and recent incidents to learn the conventions and the fragile parts.
- Ship a small, low-risk change in the first days to learn the review and deploy path.
- Ask the team what "done" means: tests, docs, review, deployment, monitoring.

---

## B7. How do you work in Agile and Scrum?

- Sprint planning with estimates, daily stand-up (yesterday, today, blockers), sprint review demos, retrospectives.
- Stories carry acceptance criteria; the definition of done includes tests, review, docs, and deployment.
- Surface blockers the same day instead of at the next stand-up.
- Prefer small pull requests that merge daily over long-lived branches.

---

## B8. How do you prioritize when several things are urgent?

Rank by irreversibility and blast radius first (production down, data loss, security), then by value per effort; write the ranking down, share it with the lead, and re-rank when new information arrives.

---

## B9. How do you keep code quality high?

- Automated gates: formatter and linter (ruff), type checks (mypy), tests with coverage in CI, dependency and image scanning.
- Small pull requests with a clear description and a test that proves the change.
- Reviews focus on correctness, failure handling, security, and readability.
- Documentation updated in the same pull request as the behavior change.

---

## B10. Why this role?

Tie it to the JD: Python services with Flask and FastAPI, microservices, and production work at a financial firm, where your banking background and production discipline apply directly.
**[fill in: one specific thing about the team or platform, once the recruiter tells you]**

---

## Rapid fire

One-line answers to say without hesitation.

### Python

| Term | One line |
| --- | --- |
| GIL | Lock that lets one thread execute Python bytecode at a time in CPython; threads help I/O-bound work, processes help CPU-bound work |
| Generator | Function with `yield` that produces values lazily and keeps its state between calls |
| Decorator | Callable that takes a function and returns a wrapped one; use `functools.wraps` |
| Context manager | Object with `__enter__` and `__exit__` that guarantees cleanup; `with` statement |
| Mutable default argument | Default evaluated once at definition time; use `None` and create inside |
| `is` versus `==` | Identity versus equality |
| `*args, **kwargs` | Extra positional arguments as a tuple, extra keyword arguments as a dict |
| MRO | Method resolution order, C3 linearization; what `super()` follows |
| `__slots__` | Fixed attribute set, no per-instance `__dict__`, less memory |
| Dataclass | Generates `__init__`, `__repr__`, `__eq__` from annotated fields; no validation |
| Pydantic model | Validates and coerces input from type hints; serializes to dict or JSON |
| asyncio | Single-threaded event loop running coroutines cooperatively; any blocking call stalls everything |
| Shallow versus deep copy | New outer container sharing inner objects versus a fully recursive copy |

### Flask and FastAPI

| Term | One line |
| --- | --- |
| WSGI | Synchronous Python server-to-app interface; one request per worker thread or process at a time |
| ASGI | Asynchronous successor supporting async, WebSockets, and lifespan events |
| Flask application context | Makes `current_app` and `g` available; pushed for each request and for CLI or background work |
| Flask request context | Makes `request` and `session` available for the current request |
| Blueprint | Group of Flask routes and handlers registered on an app, often with a URL prefix |
| App factory | `create_app(config)` function that builds the app, so tests and environments get separate instances |
| FastAPI `Depends` | Declares a dependency that FastAPI resolves per request; the basis for DB sessions and auth |
| `def` versus `async def` endpoint | `def` runs in a threadpool; `async def` runs on the event loop and must not block |
| `response_model` | Output schema that filters and documents the response |
| 422 | FastAPI's status for request validation errors |
| `lifespan` | Async context manager for startup and shutdown resources in FastAPI |
| gunicorn | Pre-fork process manager; runs Flask with sync or threaded workers, or FastAPI with uvicorn workers |
| uvicorn | ASGI server for FastAPI |

### REST, data, security

| Term | One line |
| --- | --- |
| Idempotent | Repeating the request has the same effect as doing it once: GET, PUT, DELETE |
| 401 versus 403 | Not authenticated versus authenticated but not allowed |
| 409 | Conflict with current state, for example a duplicate unique key |
| 429 | Rate limited; send `Retry-After` |
| Keyset pagination | Page by the last seen sort key instead of `OFFSET`; stable and fast on large tables |
| N+1 | One query for a list plus one per row; fix with `selectinload` or `joinedload` |
| ACID | Atomicity, consistency, isolation, durability |
| Isolation default | PostgreSQL and SQL Server: Read Committed; MySQL InnoDB: Repeatable Read |
| Connection pool | Reused database connections; size it across workers and replicas |
| Alembic | SQLAlchemy's migration tool |
| JWT | Signed, base64url-encoded claims; not encrypted by default |
| OAuth2 | Delegated authorization framework; issues access tokens |
| OIDC | Identity layer on OAuth2; adds the ID token |
| Authorization code with PKCE | The OAuth2 flow for user-facing apps |
| Client credentials | The OAuth2 flow for service-to-service calls |
| CORS | Browser rule for cross-origin reads; not an authorization mechanism |
| Parameterized query | Values sent separately from SQL text; prevents SQL injection |

### Microservices, delivery, cloud

| Term | One line |
| --- | --- |
| Circuit breaker | Stops calling a failing dependency for a cool-down period, then probes |
| Transactional outbox | Write the event to an outbox table in the same transaction as the data, relay it later |
| Saga | Sequence of local transactions with compensating actions instead of a distributed transaction |
| Idempotent consumer | Records processed message IDs so redelivery has no extra effect |
| Kafka partition | Ordered, append-only log; ordering holds only within a partition |
| Consumer group | Consumers that share a topic's partitions; each partition goes to one consumer |
| RabbitMQ exchange | Routes messages to queues by type: direct, topic, fanout, headers |
| Dead letter queue | Where messages go after repeated failure, for inspection and replay |
| Readiness probe | Kubernetes stops sending traffic when it fails |
| Liveness probe | Kubernetes restarts the container when it fails; do not check dependencies here |
| Multi-stage Docker build | Build in one stage, copy only artifacts into a slim runtime image |
| Blue-green versus canary | Switch all traffic between two environments versus shift a small percentage first |
| 12-factor | Config in the environment, stateless processes, logs as streams, disposable processes |

---

## Questions to ask them

Pick three; the first two are the most useful for a contract role.

1. "What would a successful first 30 and 90 days look like in this role?"
2. "Which services would I work on first, and what is the split between Flask and FastAPI today? Is there a migration plan between them?"
3. "How does code get to production here: CI tool, review process, environments, and how often you deploy?"
4. "Where do the services run (which cloud or on-premises Kubernetes), and which database and messaging systems do they use?"
5. "How is the team organized, and who are the business analysts and product owners I would work with?"
6. "What does production support look like: on-call rotation, incident process, and monitoring stack?"
7. "What is the expected contract length, and how have extensions or conversions worked for this team?"

Avoid asking about rate in a technical round; that conversation goes through the agency.

## Day-of checklist

- [ ] Rehearse P1 and P2 out loud once, with a timer.
- [ ] Re-read the must-know questions in [FastAPI](04-FastAPI.md) and [Flask](03-Flask.md).
- [ ] Write the reference CRUD service from memory once; compare with [code](code/README.md).
- [ ] Have the job description, your resume, and this pack's JD mapping open.
- [ ] Test camera, microphone, and screen sharing; have a local Python 3 environment and an editor ready for live coding.
- [ ] For onsite: route and travel time to Whippany or the New York office, photo ID.
- [ ] Prepare a one-line answer for availability and work authorization (P8 in [Pitch and Resume](01-Pitch-and-Resume.md)).

## Go deeper

- [STAR method](../../../06-Interview-Prep/01-Behavioral/star_method.md)
- [Agentic AI behavioral stories](../../../13-Agentic-AI/Agentic-AI-Interview-Prep/10-Behavioral.md): the same career told for AI roles; keep facts consistent.
- [The Scrum Guide](https://scrumguides.org/scrum-guide.html)
- [The Twelve-Factor App](https://12factor.net/)
