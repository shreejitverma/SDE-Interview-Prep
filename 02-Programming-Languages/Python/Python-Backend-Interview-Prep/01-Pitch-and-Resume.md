---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: []
---

# Pitch and resume deep dives

The first 10 minutes of every round, and the place where a contract screen is usually decided.
For a contract role the interviewer is checking three things fast: can you do this exact job from day one, can you explain your own work crisply, and do your claims survive follow-up.
P1, P2, P3, and P4 are near-certain; rehearse them out loud with a timer.

## Ground rules

- **The JD title is "Python Developer (Flask & FastAPI)".**
  Lead every answer with Python backend work, not C++ or quant research.
  The finance background is a strength here (the employer is most likely a large financial-services firm), but it is the second sentence, not the first.
- **Be exact about Flask versus FastAPI.**
  The strongest evidence you have is FastAPI (the Sovereign AI Command Center codebase).
  If your hands-on Flask work is thin, say so plainly and bridge (P3); an interviewer who catches an inflated claim discounts everything else.
- **Say who built what.**
  The Sovereign AI Command Center is a solo project built with an AI-assisted engineering harness; say that if asked how one person produced it, and be ready to explain any file in it.
- **Only quote numbers you can defend.**
  Every number needs a baseline, a method, and a time window; anything else is a **[fill in]**.
- **Keep the years consistent.**
  The JD asks for 5+ years of Python; the resume covers professional work from June 2018 to May 2026 with gaps for graduate study.
  **[fill in: the one number you will use everywhere, for example "about six years of professional Python and C++"]**
- **Answer shape for any project:** problem, architecture, hard parts, results, what I would change.

---

## P1. Tell me about yourself. (must know)

Aim for about 75 seconds: past, present, why this role.

> "I'm a Python and C++ engineer with about six years of professional experience, most of it in financial services.
> At Bank of America I worked on Python trading services on the Quartz platform in the FICC business, and I led the migration of more than a million lines of that Python to Python 3.8.
> Before that, at Bank of America, I built an ML platform for deploying predictive models.
> At LogiNext I led a 12-engineer team building a geospatial platform on PostgreSQL with PostGIS, MongoDB, and S3.
> Most recently I was at BNP Paribas on the market-making stack.
> On the web-service side, my most complete recent work is a FastAPI application I built and run: about fifty feature routers, SQLAlchemy persistence, cookie and bearer-token auth, OAuth integrations with Google and Microsoft, streaming endpoints, Docker Compose deployment, and a CI pipeline with ruff, mypy, and a pytest coverage gate.
> What I bring to this role is production discipline from trading systems (testing, performance, and debugging under pressure) applied to Python APIs and microservices.
> I can start immediately, and the hybrid schedule in Whippany or New York works for me."

**30-second version** (recruiter screen or "quick intro"):

> "Python and C++ engineer, about six years, mostly in financial services: Bank of America, Versor, LogiNext, and BNP Paribas.
> I've built Python trading services, migrated a million-line Python codebase to Python 3.8, and recently built a FastAPI application with about fifty routers, SQLAlchemy, OAuth, Docker, and CI.
> I'm available immediately for the hybrid schedule."

**Delivery notes.**

- Stop after the pitch; let them pick the thread.
- If they ask "why Python backend after C++ trading work?", see P9.
- Do not mention rate expectations in a technical round; if asked, defer to the agency conversation.

---

## P2. Walk me through a Python API or backend you built. (must know)

Use the Sovereign AI Command Center (resume project; the codebase is private).
Facts below were checked against the repository on 2026-09-29.

**Problem.**
A self-hosted AI workspace (chat, agents with tools, document search, mail, calendar, contacts) that runs on hardware the user controls, for individuals and for small teams behind their own reverse proxy.

**Architecture (say it in this order).**

- **Framework:** FastAPI application served by uvicorn; one `APIRouter` module per feature (about fifty routers), wired in the app module and mounted under stable `/api/...` prefixes.
- **Cross-cutting concerns in middleware:** security headers, a request-timeout middleware, an auth middleware, and a middleware that keeps deprecated route prefixes working through an alias table.
- **Auth:** multi-user accounts with bcrypt password hashing; opaque random session tokens in httpOnly SameSite cookies for the browser; separate API bearer tokens for programmatic clients; delegated OAuth to Google and Microsoft for mail and calendar, with refresh tokens stored and revoked when a user is deleted.
- **Persistence:** SQLAlchemy over SQLite for relational data, ChromaDB for vector search.
- **Streaming:** `StreamingResponse` for LLM token streaming; `BackgroundTasks` and a scheduler for background work; `lifespan` for startup and shutdown resources.
- **Deployment:** Docker image on `python:3.12-slim` running uvicorn, Docker Compose with optional sidecars, plus native launchers for macOS, Linux, and Windows.
- **Quality gate:** GitHub Actions running ruff, mypy, and pytest with a coverage gate, plus packaging smoke tests per OS.

**Hard parts worth telling.**

1. **Renaming public routes without breaking clients.**
   Eleven route prefixes were renamed; the old ones still resolve through an alias middleware until a stated major version, and OAuth redirect URIs were handled separately because the identity provider validates them on its side.
   This is a ready answer for "how do you version or evolve an API without breaking consumers?" ([REST API Design](05-REST-API-Design.md)).
2. **Revocation that actually revokes.**
   Deleting a user purges their session tokens, their API tokens, and their delegated OAuth grants in one operation; otherwise an old cookie or refresh token keeps working.
   Ready answer for "how do you revoke access?" ([Auth and Security](07-Auth-and-Security.md)).
3. **Streaming plus timeouts.**
   Long LLM streams and a request-timeout middleware conflict unless streaming routes are handled deliberately.
   **[fill in: exactly how you exempted or bounded streaming routes]**

**Results.** **[fill in: number of tests, coverage threshold, users or deployments, one latency or reliability number you can defend]**

**What I would change.**
SQLite is right for a single-host product but a multi-tenant hosted version would move to PostgreSQL with Alembic migrations and connection pooling; the session store would move from a file to the database or Redis so the app can run more than one replica.
**[fill in: anything else you would redo, for example the container running as root if it does]**

**Follow-ups they ask, and where the answer lives.**

| Follow-up | Short answer | Note |
| --- | --- | --- |
| Why FastAPI? | Type-driven validation, automatic OpenAPI, async for streaming and many concurrent I/O-bound calls to model servers | [FastAPI](04-FastAPI.md) |
| Why opaque session tokens, not JWT, for the browser? | Instant revocation and no token contents to leak; JWT fits stateless service-to-service calls | [Auth and Security](07-Auth-and-Security.md) |
| How do you test it? | pytest with `TestClient`, dependency overrides, coverage gate in CI | [Testing, Debugging, Production](10-Testing-Debugging-Production.md) |
| How would you scale it? | Stateless app tier, shared session store, PostgreSQL, horizontal replicas behind a load balancer | [System Design](12-System-Design.md) |

---

## P3. How much hands-on Flask experience do you have? (must know)

This question will come, because Flask is in the title.
Decide the honest answer before the interview.
**[fill in: where you used Flask and for how long; for example internal tools, a model-serving endpoint on the BofA ML platform, or a LogiNext service]**

**If the experience is real, answer with specifics:** app factory, Blueprints, how you managed the SQLAlchemy session, how you deployed it (gunicorn worker type and count), one production problem you debugged.

**If it is thinner than FastAPI, bridge honestly:**

> "Most of my recent web-service work is FastAPI.
> Flask and FastAPI share the same shape: routers or Blueprints, a request-scoped database session, middleware or hooks, and a WSGI or ASGI server in front.
> The differences I watch for in Flask are the application and request context, the WSGI worker model under gunicorn, and the fact that validation and OpenAPI come from extensions like marshmallow rather than being built in.
> I've built the same CRUD service in both to be sure of the differences."

Then prove it: the [Flask](03-Flask.md) note and the reference service in [code](code/README.md) are what you should be able to write from memory.

---

## P4. Tell me about a production issue you debugged. (must know)

Pick one real incident and tell it as: detection, mitigation first, root cause second, the systemic fix that prevents recurrence.
**[fill in: one incident from Bank of America (trade processing on Quartz), LogiNext (geospatial platform), or the FastAPI application]**

If you need a structure for the technical middle, the scenarios in [Testing, Debugging, Production](10-Testing-Debugging-Production.md) cover the common API failures: latency spike after deploy, 500s only under load from connection-pool exhaustion, memory growth in workers, a blocked event loop, database deadlocks, and 502 or 504 errors behind a load balancer.

---

## P5. Tell me about the Python 3.8 migration at Bank of America.

Resume line: led the migration of 1M+ lines of code to Python 3.8, improving scalability and execution efficiency by 40%.
Expect a senior interviewer to probe every word of that sentence.

- **Scope:** **[fill in: what "led" meant: how many engineers, how many teams, how long]**
- **Approach to name:** inventory and ownership map; automated rewriting (for example `2to3`, `modernize`, or `pyupgrade`-style fixers) for mechanical changes; a compatibility layer while both versions ran; test coverage added before changing risky modules; migration in dependency order; shadow runs comparing outputs.
  **[fill in: which of these you actually used]**
- **Classic breakages to mention** (they show you did it): `str` versus `bytes` at I/O boundaries, integer division, dict ordering assumptions, `print` and `exec`, removed `iteritems` and `has_key`, changed `sort` semantics (no `cmp`), pickled data compatibility, C extensions.
- **The 40% number:** **[fill in: 40% of what, measured how, against which baseline]**
  If you cannot answer that crisply, drop the number and describe the qualitative outcome.

---

## P6. Tell me about the Python trading services on Quartz.

Resume line: engineered Python-based trading services enhancing the storage, processing, matching, and execution of trades on Quartz.

- Quartz is Bank of America's large internal Python platform with its own object database (Sandra); interviewers from other banks may know it by reputation.
- **[fill in: one service you built end to end, its inputs and outputs, volume, and how it was tested and deployed]**
- Tie it to the JD: integration with databases and downstream services, performance, and production support.

---

## P7. Tell me about the LogiNext platform and leading 12 engineers.

- **Stack:** PostgreSQL with PostGIS, MongoDB, S3; routing and map-construction algorithms.
- **API angle:** **[fill in: which services exposed the platform to clients, which framework, and how they were deployed]**
- **Leadership angle:** one technical decision with a real trade-off, one disagreement and how it resolved, and how you ran code review.
- **The LLM debugging tool** (cut mean bug-resolution time by 80%): good for "a time you improved developer productivity"; **[fill in: baseline, method, window for the 80%]**.

---

## P8. Why a contract role, and what is your availability?

- **Availability:** can interview within the stated three-day window and start immediately. **[fill in: confirm]**
- **Location:** hybrid, three days onsite in Whippany, NJ or New York. **[fill in: confirm commute plan]**
- **Work authorization:** **[fill in: exact status and any sponsorship requirement, stated the same way every time]**
- **Why contract:** "I want to contribute to a production Python platform immediately, and a contract lets both sides confirm the fit quickly."
  Ask about extension and conversion in the questions section, not here.

---

## P9. Your resume is C++ and quant. Why a Python backend role?

> "Python has been half of my work throughout: trading services and a large migration at Bank of America, ML pipelines at Versor, the platform at LogiNext, and my recent FastAPI work.
> The C++ and trading background is what makes me useful on a backend team: I care about latency, correctness, testing, and what happens under load.
> A Python API team at a financial firm needs exactly that."

Do not apologize for the C++ focus; frame it as depth.

---

## P10. What would your last manager say about you?

Two strengths with evidence, one real growth area with what you are doing about it.
**[fill in]**

---

## JD mapping

Use this table the night before: every requirement, your strongest evidence, and where to revise.
Rows marked "gap" need the prepared bridge, not bluffing.

| JD requirement | My evidence | Risk | Revise |
| --- | --- | --- | --- |
| 5+ years Python | Bank of America Quartz services and migration; Versor ML pipelines; FastAPI application | Low | [Python Core](02-Python-Core.md) |
| Strong Flask | **[fill in]** | Gap unless filled | [Flask](03-Flask.md), [code](code/README.md) |
| Strong FastAPI | FastAPI application, about fifty routers | Low | [FastAPI](04-FastAPI.md) |
| REST APIs and microservices | FastAPI routers and route versioning; **[fill in: a multi-service system]** | Medium | [REST API Design](05-REST-API-Design.md), [Microservices and Messaging](08-Microservices-and-Messaging.md) |
| PostgreSQL, MySQL, SQL Server | PostgreSQL and PostGIS at LogiNext | Low for PostgreSQL | [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md) |
| SQLAlchemy | SQLAlchemy in the FastAPI application | Low | [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md) |
| Git | Daily; worktree-based workflow | Low | [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md) |
| JWT and OAuth2 | Session and bearer tokens; delegated OAuth with Google and Microsoft | Medium: know JWT validation cold | [Auth and Security](07-Auth-and-Security.md) |
| Docker | Dockerfile and Compose for the FastAPI application | Low | [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md) |
| Debugging and problem solving | Trading-system production work | Low | [Testing, Debugging, Production](10-Testing-Debugging-Production.md) |
| Cloud (AWS, Azure, GCP) | S3 at LogiNext; resume lists AWS and GCP | Medium | [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md) |
| Kubernetes | Resume lists Kubernetes and Helm; **[fill in: where]** | Medium | [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md) |
| Kafka or RabbitMQ | Resume lists Kafka; **[fill in: where]** | Medium | [Microservices and Messaging](08-Microservices-and-Messaging.md) |
| CI/CD (Jenkins, GitHub Actions, Azure DevOps) | GitHub Actions CI with lint, types, coverage gate; Jenkins on resume | Low | [Docker, Kubernetes, CI/CD, Cloud](09-Docker-Kubernetes-CICD-Cloud.md) |
| MongoDB or DynamoDB | MongoDB at LogiNext | Low for MongoDB | [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md) |
| Agile and Scrum | Team lead at LogiNext | Low | [Behavioral and Rapid Fire](13-Behavioral-and-Rapid-Fire.md) |
| Stakeholder communication | Worked with traders and business users at banks | Low | [Behavioral and Rapid Fire](13-Behavioral-and-Rapid-Fire.md) |

---

## Resume deep-dive checklist

For every resume line, be ready for four follow-ups: what exactly did you do, how did you measure it, what went wrong, and what would you do differently.

- [ ] Bank of America: Quartz trading services (P6)
- [ ] Bank of America: 1M+ line Python 3.8 migration and the 40% (P5)
- [ ] Bank of America: 50% trade-processing latency reduction with C++ pipelines into Sandra
- [ ] Bank of America: ML platform, 67% decision accuracy, 36 FTE effort saved
- [ ] Versor: ML pipelines for order and execution management, 29% execution efficiency
- [ ] LogiNext: 12-engineer team, PostGIS/MongoDB/S3 platform, LLM debugging tool with 80% (P7)
- [ ] BNP Paribas: market-making hot path, on-prem LLM tooling with Git, Jira, Confluence
- [ ] Sovereign AI Command Center: the FastAPI application (P2)

## Go deeper

- [Agentic AI pitch and resume deep dives](../../../13-Agentic-AI/Agentic-AI-Interview-Prep/01-Pitch-and-Resume-Deep-Dives.md): the same resume told for AI roles; keep the facts consistent across both.
- [STAR method](../../../06-Interview-Prep/01-Behavioral/star_method.md)
