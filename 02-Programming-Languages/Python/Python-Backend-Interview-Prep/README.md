---
type: moc
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://flask.palletsprojects.com/, https://fastapi.tiangolo.com/, https://docs.sqlalchemy.org/en/20/, https://docs.pydantic.dev/latest/]
---

# Python Backend Interview Prep (Flask and FastAPI)

A question bank for Python backend developer interviews centered on Flask and FastAPI: REST APIs, microservices, SQL and SQLAlchemy, JWT and OAuth2, Docker, Kubernetes, messaging, CI/CD, and production debugging.
It was built for a contract "Python Developer (Flask & FastAPI)" role at a financial-services employer (hybrid, New Jersey and New York), so the ordering and examples lean toward what that kind of screen asks: framework depth first, then data, security, and delivery.
Answers are tied to my own projects wherever possible.

## How to use this pack

- Every question has a short spoken answer first, then depth for follow-ups.
- Questions marked **(must know)** are the ones to know cold.
- **[fill in]** marks a detail only I know (a project fact, a measured number).
  Replace it before the interview or do not raise the topic.
- The [code](code/README.md) folder holds a small reference CRUD service in both Flask and FastAPI with pytest tests; building it from memory is the best single rehearsal for a live "build an endpoint" round.
- Versions were checked on 2026-09-29: Python 3.14, Flask 3.1, FastAPI 0.142, Starlette 1.7, Pydantic 2.13, SQLAlchemy 2.1, PyJWT 2.15.
  Each note states what was run.

## The notes

| # | Note | Questions | Why it matters |
| --- | --- | --- | --- |
| 1 | [Pitch and Resume](01-Pitch-and-Resume.md) | P1-P10, JD mapping | Opens every round; decides a contract screen |
| 2 | [Python Core](02-Python-Core.md) | Y | Language questions for a 5+ year developer, plus predict-the-output |
| 3 | [Flask](03-Flask.md) | F | In the job title; contexts, factories, Blueprints, deployment |
| 4 | [FastAPI](04-FastAPI.md) | A | In the job title; async, dependencies, Pydantic, testing |
| 5 | [REST API Design](05-REST-API-Design.md) | R | Status codes, idempotency, pagination, versioning |
| 6 | [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md) | S | Required: SQL databases and ORM, plus NoSQL touchpoints |
| 7 | [Auth and Security](07-Auth-and-Security.md) | U | Required: JWT and OAuth2 |
| 8 | [Microservices and Messaging](08-Microservices-and-Messaging.md) | M | Microservices required; Kafka and RabbitMQ preferred |
| 9 | [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md) | D | Docker required; Kubernetes, CI/CD, and cloud preferred |
| 10 | [Testing, Debugging, Production](10-Testing-Debugging-Production.md) | T | Automated testing and production troubleshooting |
| 11 | [Coding Round](11-Coding-Round.md) | K | Tested solutions to write from memory |
| 12 | [System Design](12-System-Design.md) | X | One 30-45 minute backend design round |
| 13 | [Behavioral and Rapid Fire](13-Behavioral-and-Rapid-Fire.md) | B, definitions | Stories, one-liners, questions to ask, day-of checklist |
| 14 | [Concurrency Fundamentals and Threading](14-Concurrency-Fundamentals-and-Threading.md) | CT | Deep dive: GIL, free-threading, primitives, races, deadlocks |
| 15 | [Multiprocessing and Parallelism](15-Multiprocessing-and-Parallelism.md) | MP | Deep dive: start methods, pools, IPC, shared memory, subinterpreters |
| 16 | [Asyncio Deep Dive](16-Asyncio-Deep-Dive.md) | AS | Deep dive: event loop, tasks, cancellation, timeouts, sync and async bridging |
| 17 | [Concurrency in Web Services and Coding](17-Concurrency-in-Web-Services-and-Coding.md) | CW | Workers, threadpools, cross-process races, and the concurrency coding round |

## Concurrency is the focus

The interviewers have said they will examine multithreading, multiprocessing, synchronous versus asynchronous execution, and concurrency heavily.
Notes 14-17 are the deep dive; the short answers in [Python Core](02-Python-Core.md) (Y9, Y10) and [FastAPI](04-FastAPI.md) (A2) are the opening lines, and the deep-dive notes are the follow-up chain.
Read them in order 14, 16, 15, 17: threading and the GIL set up the vocabulary, asyncio is what FastAPI runs on, multiprocessing covers CPU-bound work, and 17 ties it all to Flask, FastAPI, gunicorn, and the database.

## Priority plan when time is short

1. Fill the **[fill in]** markers in [Pitch and Resume](01-Pitch-and-Resume.md), then rehearse **P1-P4 out loud** with a timer.
   Settle the honest Flask answer (P3) before anything else.
2. Know every **(must know)** question in [FastAPI](04-FastAPI.md) and [Flask](03-Flask.md).
   The single most asked question is `def` versus `async def` in FastAPI and what blocking the event loop does.
3. Every **(must know)** question in notes 14-17, then write three problems from the concurrency coding round in [note 17](17-Concurrency-in-Web-Services-and-Coding.md) from memory (a bounded blocking queue, print in order, and a rate-limited async fetcher).
4. Build the CRUD service in [code](code/README.md) from memory in 20 minutes, in FastAPI and then in Flask.
5. Must-know questions in [REST API Design](05-REST-API-Design.md), [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md), and [Auth and Security](07-Auth-and-Security.md).
6. One system design out loud with a 35-minute timer (X1 or X5 in [System Design](12-System-Design.md)).
7. Skim [Python Core](02-Python-Core.md) predict-the-output snippets and the [Coding Round](11-Coding-Round.md) solutions.
8. Read [Behavioral and Rapid Fire](13-Behavioral-and-Rapid-Fire.md) last.

## Three-day plan

| Day | Morning | Afternoon | Evening |
| --- | --- | --- | --- |
| 1 | Note 1, rehearse pitches; notes 3-4 | Notes 14 and 16 (threading, asyncio) | Build the CRUD service twice; predict-the-output in notes 2, 14, 16 |
| 2 | Notes 15 and 17 (multiprocessing, web concurrency) | Notes 5-7 | Concurrency coding round in note 17, then note 11 |
| 3 | Notes 8-10, note 12 with one timed design | Note 13, record yourself on three stories | Must-know questions in 14-17 again, then rest |

## Six ideas that carry most answers

1. **Never block the event loop.**
   In FastAPI, `async def` plus a blocking call serializes the whole worker; use `def`, a threadpool, or an async client.
2. **Validate at the edge, keep the core plain.**
   Pydantic or marshmallow schemas at the boundary, separate create, read, and update models, and plain domain code behind them.
3. **Every write must be safe to retry.**
   Idempotency keys, unique constraints, idempotent consumers, and the transactional outbox make retries harmless.
4. **The database is usually the bottleneck.**
   N+1 queries, missing indexes, unbounded pagination, and a pool sized without counting workers and replicas cause most API latency incidents.
5. **Pick the concurrency model from the bottleneck.**
   I/O-bound with many connections: asyncio; I/O-bound with blocking libraries: threads; CPU-bound: processes (or free-threaded Python where supported); and any shared state across workers or replicas belongs in the database or Redis, not in memory.
6. **Security lives in code you can test.**
   Validate tokens fully (signature, algorithm, expiry, audience, issuer), authorize per resource, parameterize every query, and keep secrets out of images and git.

## Go deeper

- [FastAPI and Pydantic](../Python_Zero_to_Godhood/Chapter_67_FastAPI_and_Pydantic_Type-Safe_Web_Development.md)
- [Microservices and gRPC in Python](../Python_Zero_to_Godhood/Chapter_63_Microservices_and_gRPC_in_Python.md)
- [Productionizing Python: Docker and Kubernetes](../Python_Zero_to_Godhood/Chapter_65_Productionizing_Python_Docker_and_Kubernetes.md)
- [Message Brokers: Kafka and RabbitMQ](../Python_Zero_to_Godhood/Chapter_76_Message_Brokers_Kafka_and_RabbitMQ.md)
- [Agentic AI Interview Prep](../../../13-Agentic-AI/Agentic-AI-Interview-Prep/README.md): the sister pack for AI engineering roles.
