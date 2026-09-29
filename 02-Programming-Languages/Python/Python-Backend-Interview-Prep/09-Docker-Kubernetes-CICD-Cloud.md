---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://docs.docker.com/build/building/best-practices/, https://kubernetes.io/docs/concepts/configuration/liveness-readiness-startup-probes/, https://kubernetes.io/docs/concepts/containers/container-lifecycle-hooks/, https://kubernetes.io/docs/concepts/workloads/pods/pod-lifecycle/, https://www.kubernetes.dev/blog/2025/11/12/ingress-nginx-retirement/, https://gunicorn.org/reference/settings/, https://uvicorn.dev/deployment/, https://docs.github.com/en/actions, https://learn.microsoft.com/en-us/azure/devops/pipelines/yaml-schema/, https://www.jenkins.io/doc/book/pipeline/syntax/, https://helm.sh/docs/, https://github.com/Kludex/mangum, https://github.com/aquasecurity/trivy/security/advisories/GHSA-69fq-xp46-6x23]
---

# Docker, Kubernetes, CI/CD, and cloud

Shipping a Python API: the image, the container runtime contract, Kubernetes, the pipeline, Git, and the AWS, Azure, and GCP services around it.
D1-D8 are near-certain for this job description (Docker is required; Kubernetes, CI/CD, and cloud are preferred and will be probed); D4 (probes) and D8 (workers inside containers) are where Python-specific depth shows.

Every code sample here was run on 2026-09-29 under Python 3.14.7 with FastAPI 0.142.0, uvicorn 0.54.0, gunicorn 26.2.0, uvicorn-worker 0.4.0, and Mangum 0.22.0: 17 pytest tests passed, including SIGTERM sent to real uvicorn and gunicorn processes mid-request.
Non-Python artifacts were validated for syntax only: all YAML parsed with PyYAML, the Kubernetes manifests passed `kubernetes-validate --strict` against the 1.36 and 1.34 schemas, the Helm chart passed `helm lint` and `helm template` (Helm 4.3.0), the workflow passed `actionlint` 1.7.12, and `docker compose config` accepted the compose file.
The Docker daemon was not running, so the Dockerfile was **not built**; the Jenkinsfile and `azure-pipelines.yml` were not run on a server.

---

## D1. What is the difference between an image and a container, and how do layers and the build cache work? (must know)

- An **image** is an immutable, content-addressed stack of read-only filesystem layers plus metadata (entrypoint, env, user); a **container** is a running process (or process tree) started from an image, with its own writable layer, namespaces (PID, network, mount), and cgroup limits.
- It is not a VM: containers share the host kernel; isolation comes from namespaces and cgroups.
- Each Dockerfile instruction that changes the filesystem (`RUN`, `COPY`, `ADD`) creates a layer.
- **Build cache:** a step is reused if its instruction and inputs (for `COPY`, the file checksums) are unchanged **and every step before it was reused**; the first miss invalidates everything after it.
- **Ordering rule:** least frequently changing first.
  Copy `requirements.txt` and install dependencies before copying the source, so a code change rebuilds only the last layers.
- Deleting a file in a later layer does not shrink the image; the bytes stay in the earlier layer.
  Clean up in the same `RUN` (`apt-get install ... && rm -rf /var/lib/apt/lists/*`), or use a multi-stage build.
- **Tags are mutable, digests are not:** `orders-api:1.4.2` can be re-pushed; `orders-api@sha256:...` always means the same bytes.
  Deploy by digest (or an immutable tag policy in the registry).

**Follow-ups they ask:**

- "`COPY` versus `ADD`?"
  Use `COPY`; `ADD` also fetches URLs and auto-extracts local tar archives, which is surprising behavior.
- "`CMD` versus `ENTRYPOINT`?"
  `ENTRYPOINT` is the executable, `CMD` its default arguments (or the whole command if there is no entrypoint); `docker run image args` replaces `CMD` only.

---

## D2. Walk me through a production Dockerfile for a FastAPI or Flask app. (must know)

```dockerfile
# syntax=docker/dockerfile:1
ARG PYTHON_VERSION=3.14

# ---- build stage: compilers and caches stay here ----
FROM python:${PYTHON_VERSION}-slim AS builder
ENV PIP_DISABLE_PIP_VERSION_CHECK=1
RUN python -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"
WORKDIR /build
# Dependencies first: this layer is reused until requirements.txt changes.
COPY requirements.txt .
RUN --mount=type=cache,target=/root/.cache/pip \
    pip install --require-hashes -r requirements.txt

# ---- runtime stage: slim base, venv, code, non-root ----
FROM python:${PYTHON_VERSION}-slim AS runtime
ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1 \
    PATH="/opt/venv/bin:$PATH"
RUN groupadd --system --gid 10001 app \
 && useradd --system --uid 10001 --gid app --no-create-home --shell /usr/sbin/nologin app
WORKDIR /srv
COPY --from=builder /opt/venv /opt/venv
# Code stays root-owned and read-only to the app user.
COPY gunicorn.conf.py ./
COPY app/ ./app/
USER 10001:10001
EXPOSE 8000
# Exec form: gunicorn is PID 1 and receives SIGTERM directly.
CMD ["gunicorn", "-c", "gunicorn.conf.py", "app.main:app"]
```

```text
# .dockerignore
.git
.github
.venv
venv
__pycache__/
*.py[cod]
.pytest_cache/
.mypy_cache/
.ruff_cache/
.coverage
htmlcov/
.env
*.env
tests/
compose*.yaml
Dockerfile
```

What each choice buys, which is what the interviewer wants to hear:

| Choice | Why |
| --- | --- |
| Multi-stage | Compilers, headers, and pip caches stay in the builder; the runtime image has only the venv and code |
| `-slim` base | Debian with glibc, so manylinux wheels install; far smaller than the full image |
| Not Alpine | musl libc: many wheels do not apply, so numpy or psycopg compile from source, builds are slow, and behavior can differ |
| venv copied across | One directory to copy; no `--user` or site-packages path games |
| Pinned, hashed deps | `--require-hashes` with a lock file (`uv pip compile --generate-hashes` or `pip-compile --generate-hashes`) makes builds reproducible and resists tampered packages |
| `PYTHONUNBUFFERED=1` | Logs reach `docker logs` and the log shipper immediately instead of sitting in a buffer (and being lost on crash) |
| `PYTHONDONTWRITEBYTECODE=1` | No `.pyc` writes at runtime (and none needed on a read-only root filesystem) |
| Non-root numeric UID | Limits the blast radius; Kubernetes `runAsNonRoot` can verify a numeric UID |
| Code root-owned | A compromised process cannot rewrite its own code |
| Exec-form `CMD` | The server is PID 1 and gets signals (D9) |
| `.dockerignore` | Smaller context, faster builds, and no `.git` or `.env` secrets baked into a layer |

- **Build secrets** (a private package index token): `RUN --mount=type=secret,id=pip_token ...`, never `ARG` or `ENV`, which persist in the image history.
- **Flask** is the same image with a WSGI worker: `worker_class = "gthread"` (or `sync`) and `app.main:create_app()` as the target.
- **HEALTHCHECK** in a Dockerfile is used by Docker and Compose but **ignored by Kubernetes**, which uses probes (D4).
- **Alternatives to name:** `uv` in the builder for much faster installs (`COPY --from=ghcr.io/astral-sh/uv:<pinned> /uv /bin/uv`), distroless or Chainguard Python bases for a smaller attack surface (no shell, which also makes debugging harder).

---

## D3. Explain the Kubernetes objects you use to run a Python API. (must know)

| Object | What it is |
| --- | --- |
| Pod | Smallest deployable unit: one or more containers sharing a network namespace and volumes; ephemeral, gets a new IP when recreated |
| ReplicaSet | Keeps N identical pods running; you rarely touch it directly |
| Deployment | Declarative desired state for stateless apps; manages ReplicaSets for rolling updates and rollback |
| StatefulSet | Stable identity and storage per pod (databases, Kafka brokers, consumers with static membership) |
| Job, CronJob | Run to completion, or on a schedule (migrations, nightly exports) |
| Service | Stable virtual IP and DNS name in front of a pod selector |
| Ingress | HTTP routing from outside to Services (host and path rules), implemented by an ingress controller |
| ConfigMap | Non-secret configuration as env vars or files |
| Secret | Sensitive values; base64-encoded, not encrypted by default |
| Namespace | Scope for names, RBAC, quotas, and network policies (per team or per environment) |
| HorizontalPodAutoscaler | Scales replicas on CPU, memory, or custom metrics |
| PodDisruptionBudget | Limits voluntary evictions (node drains) so enough replicas stay up |

Service types:

- **ClusterIP** (default): internal virtual IP; how services call each other (`http://orders-api.orders.svc.cluster.local`).
- **NodePort:** opens a port on every node; rarely used directly.
- **LoadBalancer:** provisions a cloud load balancer (NLB on AWS).
- **ExternalName:** a DNS CNAME to something outside the cluster.
- **Headless** (`clusterIP: None`): DNS returns pod IPs directly (StatefulSets, client-side load balancing such as gRPC).

The manifests (Deployment, Service, HPA, PDB, Ingress, ConfigMap), schema-validated in strict mode:

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: orders-api
  namespace: orders
data:
  LOG_LEVEL: INFO
  WEB_CONCURRENCY: "2"
---
apiVersion: apps/v1
kind: Deployment
metadata:
  name: orders-api
  namespace: orders
  labels:
    app: orders-api
spec:
  replicas: 3
  revisionHistoryLimit: 5
  selector:
    matchLabels:
      app: orders-api
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 1          # one extra pod during the rollout
      maxUnavailable: 0    # never drop below the desired count
  template:
    metadata:
      labels:
        app: orders-api
    spec:
      serviceAccountName: orders-api            # bound to a cloud IAM role (IRSA / Workload Identity)
      terminationGracePeriodSeconds: 30
      securityContext:
        runAsNonRoot: true
        runAsUser: 10001
        seccompProfile:
          type: RuntimeDefault
      containers:
        - name: api
          image: registry.example.com/orders-api:1.4.2   # immutable tag or digest, never :latest
          ports:
            - name: http
              containerPort: 8000
          envFrom:
            - configMapRef:
                name: orders-api
          env:
            - name: DATABASE_URL
              valueFrom:
                secretKeyRef:
                  name: orders-api-db
                  key: url
          resources:
            requests:
              cpu: 500m
              memory: 512Mi
            limits:
              memory: 512Mi     # memory limit = request: predictable, no noisy-neighbour OOM
              # no CPU limit: avoids CFS throttling; the request guarantees the share
          startupProbe:
            httpGet:
              path: /livez
              port: http
            periodSeconds: 2
            failureThreshold: 30   # up to 60 s to boot before liveness starts counting
          livenessProbe:
            httpGet:
              path: /livez         # process health only, no dependency checks
              port: http
            periodSeconds: 10
            timeoutSeconds: 2
            failureThreshold: 3
          readinessProbe:
            httpGet:
              path: /readyz        # may check DB and downstreams
              port: http
            periodSeconds: 5
            timeoutSeconds: 2
            failureThreshold: 2
          lifecycle:
            preStop:
              sleep:
                seconds: 5         # let endpoints and load balancers stop routing first
          securityContext:
            allowPrivilegeEscalation: false
            readOnlyRootFilesystem: true
            capabilities:
              drop: ["ALL"]
          volumeMounts:
            - name: tmp
              mountPath: /tmp
      volumes:
        - name: tmp
          emptyDir: {}
---
apiVersion: v1
kind: Service
metadata:
  name: orders-api
  namespace: orders
spec:
  type: ClusterIP
  selector:
    app: orders-api
  ports:
    - name: http
      port: 80
      targetPort: http
---
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: orders-api
  namespace: orders
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: orders-api
  minReplicas: 3
  maxReplicas: 20
  metrics:
    - type: Resource
      resource:
        name: cpu
        target:
          type: Utilization
          averageUtilization: 70   # percent of the CPU *request*
---
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: orders-api
  namespace: orders
spec:
  minAvailable: 2
  selector:
    matchLabels:
      app: orders-api
---
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: orders-api
  namespace: orders
spec:
  ingressClassName: alb
  rules:
    - host: orders.example.com
      http:
        paths:
          - path: /
            pathType: Prefix
            backend:
              service:
                name: orders-api
                port:
                  name: http
```

- **HPA math:** it scales on utilization **relative to the request**, so a wrong request makes autoscaling wrong; it needs metrics-server (or a custom metrics adapter for queue depth or requests per second).
- **Ingress versus Gateway API:** the Ingress API is stable and feature-frozen; the community **ingress-nginx controller was retired in March 2026** (no further releases or security fixes), and Gateway API is the recommended successor.
  On EKS the AWS Load Balancer Controller (`ingressClassName: alb`) is common.
- The `preStop` `sleep` action is native since Kubernetes 1.34 (GA); on older clusters use `exec: {command: ["sleep", "5"]}`, which needs a `sleep` binary in the image.

---

## D4. Liveness, readiness, and startup probes: what is the difference, and why should liveness not check the database? (must know)

> "Readiness answers 'should this pod get traffic right now'; failing it removes the pod from the Service endpoints but does not restart it.
> Liveness answers 'is this process wedged beyond recovery'; failing it makes the kubelet restart the container.
> Startup protects slow boots: liveness and readiness do not run until it succeeds.
> Liveness must not check the database: if the database blips, every pod fails liveness at once, Kubernetes restarts the whole fleet, the restarts stampede the recovering database with new connections, and a partial outage becomes a total one."

| | Failure effect | Should check | Should not check |
| --- | --- | --- | --- |
| Startup | Container restarted after `failureThreshold x periodSeconds` | The app booted | Dependencies |
| Liveness | Container restarted | The process can serve a trivial request (event loop not deadlocked) | DB, cache, downstream services |
| Readiness | Removed from Service endpoints, no restart | Can it do useful work: DB pool, critical dependency, warm-up done | Non-critical dependencies (degrade instead) |

```python
import asyncio
import logging
import os
from collections.abc import Awaitable, Callable
from contextlib import asynccontextmanager
from dataclasses import dataclass

from fastapi import FastAPI, Response, status

log = logging.getLogger("app")


@dataclass(frozen=True)
class Settings:
    """12-factor config: everything from the environment, validated once at startup."""
    database_url: str
    log_level: str = "INFO"
    slow_ms: int = 0  # test hook for the graceful-shutdown test

    @classmethod
    def from_env(cls) -> "Settings":
        url = os.environ.get("DATABASE_URL")
        if not url:
            raise RuntimeError("DATABASE_URL is required")  # fail fast at boot, not on first request
        return cls(database_url=url,
                   log_level=os.environ.get("LOG_LEVEL", "INFO"),
                   slow_ms=int(os.environ.get("SLOW_MS", "0")))


async def default_db_ping() -> None:
    """Replace with `SELECT 1` on the engine; kept dependency-free here."""


def create_app(db_ping: Callable[[], Awaitable[None]] = default_db_ping) -> FastAPI:
    @asynccontextmanager
    async def lifespan(app: FastAPI):
        app.state.settings = Settings.from_env()
        app.state.ready = True
        yield
        app.state.ready = False            # shutdown: stop advertising readiness
        log.info("shutdown complete")

    app = FastAPI(lifespan=lifespan)

    @app.get("/livez")
    async def livez():
        # Liveness: "is this process wedged?" No dependency checks, or a DB outage restarts every pod.
        return {"status": "alive"}

    @app.get("/readyz")
    async def readyz(response: Response):
        # Readiness: "should traffic be sent here right now?" Dependencies are fair game.
        if not app.state.ready:
            response.status_code = status.HTTP_503_SERVICE_UNAVAILABLE
            return {"status": "not ready"}
        try:
            await asyncio.wait_for(db_ping(), timeout=1.0)
        except Exception:
            response.status_code = status.HTTP_503_SERVICE_UNAVAILABLE
            return {"status": "db unavailable"}
        return {"status": "ready"}

    @app.get("/work")
    async def work():
        await asyncio.sleep(app.state.settings.slow_ms / 1000)
        return {"done": True}

    return app


app = create_app()
```

The tests prove: with the database down, `/livez` stays 200 while `/readyz` returns 503; a hanging database makes `/readyz` fail within the 1 s timeout instead of hanging the probe; and missing `DATABASE_URL` fails at startup.

- **Readiness caveat:** if every pod's readiness depends on the same database, a database outage still empties the Service; that is usually correct (fail fast with 503 at the load balancer), but decide deliberately.
- **Probe endpoints must be cheap:** no auth, no logging at INFO on every hit, bounded timeouts, and `async` code that never blocks, because a blocked event loop fails liveness.
- **Classic incident:** a synchronous call inside an `async def` endpoint blocks the loop under load, liveness times out, pods restart in a loop, and the restarts look like the cause.
- Flask equivalents are the same two routes; with a sync worker, a long request can starve the probe unless there are spare threads or workers.

---

## D5. Describe a CI/CD pipeline for a Python service. (must know)

> "On every pull request: lint and format check with ruff, type-check with mypy, run pytest with coverage against a real PostgreSQL service container.
> On merge to main: build the image once, scan it, push it tagged with the commit SHA, deploy that exact digest to staging, run smoke tests, then promote the same digest to production behind an approval.
> Build once, promote the artifact; never rebuild per environment."

```yaml
name: ci-cd

on:
  pull_request:
  push:
    branches: [main]

permissions:
  contents: read            # least privilege by default; jobs widen it explicitly

concurrency:
  group: ${{ github.workflow }}-${{ github.ref }}
  cancel-in-progress: true

env:
  IMAGE: ghcr.io/example-org/orders-api   # GHCR names must be lowercase

jobs:
  test:
    runs-on: ubuntu-latest
    services:
      postgres:
        image: postgres:17
        env:
          POSTGRES_USER: app
          POSTGRES_PASSWORD: app
          POSTGRES_DB: app
        ports: ["5432:5432"]
        options: >-
          --health-cmd "pg_isready -U app -d app"
          --health-interval 5s --health-timeout 3s --health-retries 10
    env:
      DATABASE_URL: postgresql+psycopg://app:app@localhost:5432/app
    steps:
      - uses: actions/checkout@v7
      - uses: actions/setup-python@v7
        with:
          python-version: "3.14"
          cache: pip
      - run: pip install -r requirements.txt -r requirements-dev.txt
      - run: ruff check .
      - run: ruff format --check .
      - run: mypy app
      - run: pytest --cov=app --cov-report=xml --cov-fail-under=85

  build:
    needs: test
    if: github.event_name == 'push'
    runs-on: ubuntu-latest
    permissions:
      contents: read
      packages: write
    outputs:
      digest: ${{ steps.push.outputs.digest }}
    steps:
      - uses: actions/checkout@v7
      - uses: docker/setup-buildx-action@v4
      - uses: docker/login-action@v4
        with:
          registry: ghcr.io
          username: ${{ github.actor }}
          password: ${{ secrets.GITHUB_TOKEN }}
      - name: Build and load for scanning
        uses: docker/build-push-action@v7
        with:
          context: .
          load: true
          tags: ${{ env.IMAGE }}:${{ github.sha }}
          cache-from: type=gha
          cache-to: type=gha,mode=max
      - name: Scan image (fail on fixable HIGH/CRITICAL)
        uses: aquasecurity/trivy-action@ed142fd0673e97e23eac54620cfb913e5ce36c25  # v0.36.0, pinned by commit SHA
        with:
          image-ref: ${{ env.IMAGE }}:${{ github.sha }}
          severity: CRITICAL,HIGH
          ignore-unfixed: true
          exit-code: "1"
      - name: Push
        id: push
        uses: docker/build-push-action@v7
        with:
          context: .
          push: true
          tags: ${{ env.IMAGE }}:${{ github.sha }}
          cache-from: type=gha

  deploy-staging:
    needs: build
    runs-on: ubuntu-latest
    environment: staging
    permissions:
      contents: read
      id-token: write       # OIDC federation to AWS: no long-lived access keys in secrets
    steps:
      - uses: actions/checkout@v7
      - uses: aws-actions/configure-aws-credentials@v6
        with:
          role-to-assume: ${{ vars.AWS_DEPLOY_ROLE_ARN }}
          aws-region: us-east-1
      - uses: azure/setup-helm@v5
        with:
          version: v4.3.0
      - run: aws eks update-kubeconfig --name "${{ vars.EKS_CLUSTER }}"
      - name: Deploy by digest
        run: >-
          helm upgrade --install orders-api ./chart
          --namespace orders-staging
          --set image.repository=${{ env.IMAGE }}
          --set image.digest=${{ needs.build.outputs.digest }}
          --wait --rollback-on-failure --timeout 5m
      - name: Smoke test
        run: curl --fail --retry 5 --retry-delay 3 https://orders.staging.example.com/readyz

  deploy-prod:
    needs: [build, deploy-staging]
    runs-on: ubuntu-latest
    environment: production  # required reviewers on this environment gate the promotion
    permissions:
      contents: read
      id-token: write
    steps:
      - uses: actions/checkout@v7
      - uses: aws-actions/configure-aws-credentials@v6
        with:
          role-to-assume: ${{ vars.AWS_DEPLOY_ROLE_ARN }}
          aws-region: us-east-1
      - uses: azure/setup-helm@v5
        with:
          version: v4.3.0
      - run: aws eks update-kubeconfig --name "${{ vars.EKS_CLUSTER }}"
      - name: Promote the same digest
        run: >-
          helm upgrade --install orders-api ./chart
          --namespace orders
          -f chart/values-prod.yaml
          --set image.repository=${{ env.IMAGE }}
          --set image.digest=${{ needs.build.outputs.digest }}
          --wait --rollback-on-failure --timeout 10m
```

- **Security points that impress:** `permissions: contents: read` by default; OIDC federation (`id-token: write`) instead of stored cloud keys; environments with required reviewers for production; third-party actions **pinned to a full commit SHA**.
  The SHA pin is not theoretical: in March 2026 attackers force-pushed 75 of 76 version tags of `aquasecurity/trivy-action` to credential-stealing code, so every workflow using a tag ran it; SHA-pinned workflows were unaffected.
  Pin the first-party actions by SHA too in a real repository (Dependabot or Renovate keeps the pins current).
- **Helm 4** renamed `--atomic` to `--rollback-on-failure`; on Helm 3 use `--atomic`.
- **Database migrations** (Alembic) run as a separate step or a Kubernetes Job before the rollout, and must be backward compatible with the running version (expand, deploy, contract), because old and new pods run side by side during a rolling update.
- **What else goes in:** dependency audit (`pip-audit`), secret scanning, SBOM generation (Syft), image signing (cosign), and a coverage gate that blocks regressions rather than chasing 100%.
- Experience hook: **[fill in: the CI/CD system you used at work (Jenkins is on the resume), what the stages were, and one improvement you made]**.

---

## D6. Which AWS services does a typical Python API use, and why IAM roles instead of access keys? (must know)

| Need | AWS service | Notes |
| --- | --- | --- |
| Run containers | ECS on Fargate (simplest), EKS (Kubernetes) | ECS task role, EKS IRSA or Pod Identity for credentials |
| Run functions | Lambda (+ API Gateway or a function URL) | D19 |
| Relational DB | RDS or Aurora PostgreSQL/MySQL, RDS for SQL Server | RDS Proxy for connection pooling from Lambda or many pods |
| NoSQL | DynamoDB | Single-digit millisecond key-value at any scale; design by access pattern |
| Object storage | S3 | Presigned URLs for direct client upload and download (see [System Design X4](12-System-Design.md)) |
| Messaging | SQS, SNS, EventBridge, MSK (Kafka) | [Microservices and Messaging M5](08-Microservices-and-Messaging.md) |
| Cache | ElastiCache (Redis or Valkey) | Sessions, rate limits, caching |
| Secrets and config | Secrets Manager (rotation), SSM Parameter Store | Fetched at startup or injected via the Secrets Store CSI driver or External Secrets |
| Identity for code | IAM roles | Temporary credentials, no keys |
| Edge | ALB, API Gateway, CloudFront, WAF, Route 53 | TLS, routing, throttling |
| Images | ECR | Scan on push |
| Observability | CloudWatch Logs, Metrics, Alarms; X-Ray or OpenTelemetry (ADOT) | D22 |
| Encryption | KMS | Envelope encryption for data and secrets |

> "Code running in AWS should never hold long-lived access keys.
> An IAM role attached to the workload (ECS task role, EKS service account through IRSA or Pod Identity, Lambda execution role) gives the SDK temporary credentials that rotate automatically, are scoped by least-privilege policies, and are auditable in CloudTrail.
> A leaked access key works from anywhere until someone notices; a role's credentials are short-lived and bound to the workload.
> The same idea applies to CI: GitHub Actions assumes a role through OIDC instead of storing keys."

- `boto3` finds credentials through its default chain (environment, shared config, container and instance metadata, web identity token), so the application code does not change between a laptop profile and a pod with IRSA.
- `boto3` clients are thread-safe and should be created once and reused; `boto3.Session` objects and resources are not thread-safe.
- Experience hook: **[fill in: AWS or GCP services you used in production (both are on the resume), and how credentials were handled]**.

---

## D7. Trunk-based development or GitFlow? Rebase or merge? (must know)

- **Trunk-based:** short-lived branches (hours to a day or two) merged to `main` behind CI, feature flags hide unfinished work, `main` is always releasable.
  It fits continuous delivery and microservices; this is what I would default to.
- **GitFlow:** long-lived `develop` plus `feature/*`, `release/*`, `hotfix/*` branches.
  It fits scheduled releases and versioned products (on-prem software, mobile apps, regulated release trains), at the cost of merge pain and delayed integration.
- **Rebase versus merge:**
  - Rebase your own feature branch onto `main` to keep a linear history and resolve conflicts commit by commit.
  - **Never rebase shared history** that others have pulled; if you must update a pushed branch you own, use `git push --force-with-lease`, which refuses if someone else pushed.
  - Merge commits preserve the true history of integration; squash merge gives one commit per pull request (a clean `main`, easy revert), at the cost of losing intermediate commits.
- **Protected branches:** required pull request reviews, required status checks, no force pushes, linear history if the team wants it, CODEOWNERS for critical paths, and a merge queue on busy repos so every merge is tested against the latest `main`.

---

## D8. How do you run gunicorn or uvicorn inside a container, and how many workers? (must know)

> "In Kubernetes I lean towards fewer worker processes per pod and more pods, because Kubernetes already does process supervision, restarts, and load balancing across pods.
> I size workers from the pod's CPU request or limit, not from `os.cpu_count()`, which returns the node's cores.
> For FastAPI that is gunicorn with the uvicorn worker class, or plain uvicorn; for Flask, gunicorn with sync or threaded workers."

```python
import os
from pathlib import Path


def cgroup_cpu_limit(cpu_max: Path = Path("/sys/fs/cgroup/cpu.max")) -> float | None:
    """CPU limit from cgroup v2 ("max 100000" = unlimited, "150000 100000" = 1.5 CPUs)."""
    try:
        quota, period = cpu_max.read_text().split()
    except (OSError, ValueError):
        return None
    if quota == "max":
        return None
    return int(quota) / int(period)


def worker_count(cpu_max: Path = Path("/sys/fs/cgroup/cpu.max"), per_cpu: int = 1) -> int:
    """Size workers from the container's CPU limit, not the node's core count.

    os.cpu_count() reports the node's CPUs (for example 64) even when the pod is limited
    to 2, which spawns far too many workers and gets them throttled or OOMKilled.
    WEB_CONCURRENCY wins when set explicitly.
    """
    if env := os.environ.get("WEB_CONCURRENCY"):
        return max(1, int(env))
    limit = cgroup_cpu_limit(cpu_max)
    cpus = limit if limit is not None else (os.process_cpu_count() or 1)
    return max(1, int(cpus) * per_cpu)
```

```python
# gunicorn.conf.py - read by `gunicorn -c gunicorn.conf.py app.main:app`
from app.workers import worker_count

bind = "0.0.0.0:8000"
workers = worker_count()                  # from the cgroup CPU limit, or WEB_CONCURRENCY
worker_class = "uvicorn_worker.UvicornWorker"   # ASGI (FastAPI); use "gthread" or "sync" for Flask
timeout = 30                              # kill a worker stuck longer than this
graceful_timeout = 25                     # < terminationGracePeriodSeconds (30) minus preStop sleep
keepalive = 5
max_requests = 1000                       # recycle workers to contain slow memory leaks
max_requests_jitter = 100                 # so all workers do not restart at once
accesslog = "-"                           # stdout; the platform ships logs
errorlog = "-"
forwarded_allow_ips = "*"                 # trust X-Forwarded-* only because the ingress is the sole entry
```

- **Why several workers at all:** the GIL means one Python process uses about one core for Python code; an async worker handles many concurrent I/O-bound requests but not CPU-bound work.
  Workers are processes, so memory multiplies: 4 workers x 250 MB is 1 GB, which must fit the memory limit.
- **The old "2 x cores + 1" rule** is for sync workers on a VM; with async workers and pods, 1 per CPU (or even 1 per pod, scaling by replicas) is the usual start, then load-test.
- **Version details verified locally:** the `uvicorn.workers` module is deprecated in favor of the separate `uvicorn-worker` package (`uvicorn_worker.UvicornWorker`); gunicorn reads `WEB_CONCURRENCY` for its default worker count; gunicorn 26.2 also ships a native `asgi` worker class; Python 3.13+ has `os.process_cpu_count()` (respects CPU affinity, still not the cgroup quota) and `PYTHON_CPU_COUNT` to override it.
- **One process per container versus a process manager:** Kubernetes restarts the container, so running uvicorn directly (single process) is valid and simplest; gunicorn adds worker supervision, `max_requests` recycling, and multi-core use inside one pod.
- **Pitfalls:**
  - `--preload` loads the app before forking, which saves memory but shares anything created at import time (database pools, Kafka clients) across forks; create connections in the lifespan or `post_fork`, not at import.
  - Prometheus client metrics need multiprocess mode (`PROMETHEUS_MULTIPROC_DIR`) with several workers, or each scrape sees one random worker.
  - A gunicorn `timeout` shorter than your slowest legitimate request kills workers mid-request.

---

## D9. Why does PID 1 matter in a container, and why must `CMD` use exec form?

- The first process in the container is PID 1; `docker stop` and the kubelet send it `SIGTERM`, wait for the grace period (10 s for `docker stop`, 30 s default in Kubernetes), then `SIGKILL`.
- **Shell form** (`CMD gunicorn ...`) runs `/bin/sh -c "gunicorn ..."`; the shell can end up as PID 1 and does not forward `SIGTERM`, so the app never shuts down gracefully and is killed after the grace period (some shells exec a single simple command, but do not rely on it).
  **Exec form** (`CMD ["gunicorn", ...]`) makes the server PID 1.
- **PID 1 ignores signals it has no handler for:** the kernel does not apply the default "terminate" action to PID 1 in a PID namespace, so a plain Python script without a `SIGTERM` handler ignores `docker stop` and gets `SIGKILL`.
  gunicorn and uvicorn install handlers, so they are fine.
- **Zombie reaping:** PID 1 must reap orphaned children; if your app spawns subprocesses, run under an init (`tini`, `docker run --init`) or exec the server from an entrypoint script with `exec "$@"`.
- **Graceful shutdown, verified:** SIGTERM sent to a real uvicorn process during a 1.5 s request: the request completed, then the process exited.
  uvicorn 0.54 re-raises the captured signal after its graceful shutdown, so the exit status is "terminated by SIGTERM" (143 in a shell), not 0; gunicorn with the uvicorn worker exited 0.
  Do not alert on exit code 143 during deploys.
- `uvicorn --timeout-graceful-shutdown N` bounds how long it waits for in-flight requests; gunicorn's `graceful_timeout` does the same for workers on `SIGTERM` (`SIGQUIT` and `SIGINT` are immediate shutdowns in gunicorn).

---

## D10. What happens when Kubernetes terminates a pod, and how do you avoid dropped requests?

1. The pod is marked Terminating; **in parallel**, it is removed from Service EndpointSlices (so kube-proxy, ingress controllers, and cloud load balancers stop routing to it) and the kubelet starts shutdown.
2. The `preStop` hook runs first (for example sleep 5).
3. Then `SIGTERM` goes to PID 1 of each container.
4. The app stops accepting new connections, finishes in-flight requests, runs lifespan shutdown (close DB pools, flush Kafka producers, commit offsets).
5. When `terminationGracePeriodSeconds` (counted from the start, **including** the preStop time) runs out, `SIGKILL`.

- **Why the preStop sleep:** endpoint removal propagates asynchronously; without it the app can stop accepting connections while load balancers still send it traffic, producing 502s on every deploy.
- **Budget:** preStop sleep + app graceful timeout < `terminationGracePeriodSeconds` (5 + 25 <= 30 in the example; a test asserts this).
- **Long-running work** (a Celery task, a Kafka batch) needs a longer grace period or must be checkpointed and resumable.
- **Keep-alive connections:** a client holding a keep-alive connection can still send a request as the server closes; clients should retry idempotent requests on connection reset.

---

## D11. How do resource requests and limits work, and what are CPU throttling and OOMKilled?

- **Requests** are what the scheduler reserves (a pod lands only on a node with that much unreserved capacity); **limits** are enforced at runtime.
- **CPU limit** is enforced by the CFS quota: a pod limited to 1 CPU gets 100 ms of CPU per 100 ms period; a multi-threaded or multi-process burst that uses it up early is **throttled** for the rest of the period, which shows up as p99 latency spikes with low average CPU.
- **Memory limit** is hard: exceeding it gets the process killed by the kernel OOM killer: `OOMKilled`, exit code 137, and a restart.
- **QoS classes:** Guaranteed (requests equal limits for CPU and memory in every container), Burstable (some requests or limits), BestEffort (none, evicted first).
- **Common practice (and its trade-off):** set memory request equal to memory limit; set a CPU request and either no CPU limit (no throttling, relies on requests for fairness) or a generous one; some platforms mandate CPU limits through LimitRange or quotas, and the manifest above is Burstable rather than Guaranteed as a result.
- **Python specifics:** each gunicorn worker is a separate process with its own memory; memory grows with worker count; watch RSS growth over time for leaks (`max_requests` recycling is the pragmatic mitigation).

---

## D12. How do rolling updates and rollbacks work, and when would you use blue-green or canary?

- **Rolling update** (Deployment default): new ReplicaSet scaled up while the old one scales down, governed by `maxSurge` and `maxUnavailable` (25% each by default); readiness gates each step.
  `kubectl rollout status deploy/orders-api`, `kubectl rollout history`, `kubectl rollout undo deploy/orders-api [--to-revision=N]`.
- Old and new versions **run at the same time**, so APIs, events, and database schemas must be backward compatible across one version.
- **Blue-green:** a full second environment; switch traffic at once (Service selector, load balancer target group, DNS); instant rollback by switching back; costs double capacity and needs care with database migrations.
- **Canary:** send a small percentage of traffic (1%, 5%, 25%) to the new version, compare error rate and latency against the baseline, and promote or abort automatically.
  Tools: Argo Rollouts, Flagger, service mesh traffic splitting, or weighted ALB target groups.
- **Feature flags** decouple deploy from release: ship dark, enable per user or percentage, kill switch without a redeploy.
- For a financial-services firm, expect change windows and approvals; canary plus automated rollback on SLO breach is the strongest answer.

---

## D13. How do you manage configuration and secrets?

- **12-factor:** config that varies between environments comes from the environment (env vars or mounted files), the same image runs everywhere, and the app validates config at startup and fails fast (`Settings.from_env` above; `pydantic-settings` in real FastAPI apps).
- **Kubernetes Secrets are base64-encoded, not encrypted**; anyone with read access to Secrets in the namespace can read them, and encryption at rest in etcd depends on the cluster's configuration (managed clusters increasingly enable envelope encryption with a KMS key by default).
- **Better sources of truth:** AWS Secrets Manager, Azure Key Vault, GCP Secret Manager, or HashiCorp Vault, synchronized by External Secrets Operator or mounted by the Secrets Store CSI driver; the workload authenticates with its identity (IRSA, Azure Workload Identity, GKE Workload Identity), not a stored key.
- **Rotation:** prefer files over env vars for secrets that rotate (env vars are fixed at process start); database credentials can be short-lived (RDS IAM authentication, Vault dynamic secrets).
- **Never:** secrets in the image, in Git, in `ARG` or `ENV`, in logs, or in exception messages; scan repositories for committed secrets in CI.

---

## D14. How do you set up local development with Docker Compose and PostgreSQL?

```yaml
services:
  api:
    build: .
    ports:
      - "8000:8000"
    environment:
      DATABASE_URL: postgresql+psycopg://app:app@db:5432/app
      LOG_LEVEL: DEBUG
    # Dev only: autoreload with the source mounted over the image copy.
    command: ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000", "--reload"]
    volumes:
      - ./app:/srv/app
    depends_on:
      db:
        condition: service_healthy

  db:
    image: postgres:17
    environment:
      POSTGRES_USER: app
      POSTGRES_PASSWORD: app          # local only; never a real secret in compose files
      POSTGRES_DB: app
    ports:
      - "5432:5432"
    volumes:
      - pgdata:/var/lib/postgresql/data
    healthcheck:
      test: ["CMD-SHELL", "pg_isready -U app -d app"]
      interval: 5s
      timeout: 3s
      retries: 10

volumes:
  pgdata:
```

- Inside the Compose network the host name is the **service name** (`db`), not `localhost`; `localhost` inside the `api` container is the container itself (a very common interview trick question).
- `depends_on` with `condition: service_healthy` waits for the healthcheck; plain `depends_on` only orders container start, not readiness.
- The app should still retry its first database connection, because in Kubernetes there is no `depends_on`.
- Named volume `pgdata` survives `docker compose down`; `docker compose down -v` deletes it.
- Integration tests in CI can use the same PostgreSQL image as a service container (D5) or Testcontainers; see [Testing, Debugging, Production](10-Testing-Debugging-Production.md).

---

## D15. A pod is in CrashLoopBackOff (or Pending, or ImagePullBackOff). How do you debug it?

```sh
kubectl get pods -n orders -o wide
kubectl describe pod <pod> -n orders          # Events: scheduling, pulls, probe failures, OOMKilled
kubectl logs <pod> -n orders --previous       # logs of the crashed container, not the new one
kubectl get events -n orders --sort-by=.lastTimestamp
kubectl rollout history deploy/orders-api -n orders
kubectl debug -it <pod> -n orders --image=busybox:1.37 --target=api   # ephemeral debug container
```

| Status | Usual causes |
| --- | --- |
| CrashLoopBackOff | App exits at startup (missing env var, bad config, migration failed, import error), or liveness probe kills it |
| OOMKilled (exit 137) | Memory limit too low, too many workers, a leak, loading a large file into memory |
| Pending | No node with enough requested CPU or memory, taints and tolerations, unbound PersistentVolumeClaim, quota |
| ImagePullBackOff | Wrong tag, private registry without pull credentials, registry rate limit |
| Running but 0/1 Ready | Readiness failing: dependency down, wrong port or path, slow warm-up |
| CreateContainerConfigError | Referenced ConfigMap or Secret key does not exist |

- `--previous` is the flag people forget; the current container's logs are often empty after a restart.
- Distroless images have no shell, which is why `kubectl debug` with an ephemeral container matters.
- Experience hook: **[fill in: a production container or Kubernetes issue you debugged, and the root cause]**.

---

## D16. How would the same pipeline look in Jenkins and Azure DevOps?

Jenkins (declarative `Jenkinsfile`; not run on a Jenkins server here):

```groovy
pipeline {
  agent { label 'linux-docker' }
  options {
    timeout(time: 30, unit: 'MINUTES')
    disableConcurrentBuilds()
  }
  environment {
    IMAGE = 'registry.example.com/orders-api'
  }
  stages {
    stage('Lint, type-check, test') {
      steps {
        sh '''
          python3 -m venv .venv
          . .venv/bin/activate
          pip install -r requirements.txt -r requirements-dev.txt
          ruff check .
          ruff format --check .
          mypy app
          pytest --cov=app --junitxml=reports/junit.xml
        '''
      }
      post {
        always { junit 'reports/junit.xml' }
      }
    }
    stage('Build, scan, push') {
      when { branch 'main' }
      steps {
        withCredentials([usernamePassword(credentialsId: 'registry-creds',
                                          usernameVariable: 'REG_USER', passwordVariable: 'REG_PASS')]) {
          // Single quotes: the shell expands the secrets, Groovy never interpolates them into the log.
          sh '''
            echo "$REG_PASS" | docker login registry.example.com -u "$REG_USER" --password-stdin
            docker build -t "$IMAGE:$GIT_COMMIT" .
            trivy image --exit-code 1 --severity HIGH,CRITICAL --ignore-unfixed "$IMAGE:$GIT_COMMIT"
            docker push "$IMAGE:$GIT_COMMIT"
          '''
        }
      }
    }
    stage('Deploy staging') {
      when { branch 'main' }
      steps {
        sh '''
          helm upgrade --install orders-api ./chart --namespace orders-staging \
            --set image.repository="$IMAGE" --set image.tag="$GIT_COMMIT" \
            --wait --rollback-on-failure --timeout 5m
          curl --fail --retry 5 --retry-delay 3 https://orders.staging.example.com/readyz
        '''
      }
    }
    stage('Promote to production') {
      when { branch 'main' }
      input {
        message 'Deploy this build to production?'
        ok 'Deploy'
      }
      steps {
        sh '''
          helm upgrade --install orders-api ./chart --namespace orders -f chart/values-prod.yaml \
            --set image.repository="$IMAGE" --set image.tag="$GIT_COMMIT" \
            --wait --rollback-on-failure --timeout 10m
        '''
      }
    }
  }
}
```

Azure DevOps (`azure-pipelines.yml`; YAML syntax validated, not run):

```yaml
trigger:
  branches:
    include: [main]
pr:
  branches:
    include: [main]

variables:
  imageRepository: orders-api
  dockerRegistryServiceConnection: acr-connection   # service connection, not a password in YAML
  tag: $(Build.SourceVersion)

stages:
  - stage: Test
    jobs:
      - job: test
        pool:
          vmImage: ubuntu-latest
        steps:
          - task: UsePythonVersion@0
            inputs:
              versionSpec: "3.14"
          - script: pip install -r requirements.txt -r requirements-dev.txt
            displayName: Install
          - script: |
              ruff check .
              ruff format --check .
              mypy app
              pytest --cov=app --cov-report=xml --junitxml=test-results.xml
            displayName: Lint, type-check, test
          - task: PublishTestResults@2
            condition: succeededOrFailed()
            inputs:
              testResultsFiles: test-results.xml

  - stage: Build
    dependsOn: Test
    condition: and(succeeded(), eq(variables['Build.SourceBranch'], 'refs/heads/main'))
    jobs:
      - job: build
        pool:
          vmImage: ubuntu-latest
        steps:
          - task: Docker@2
            inputs:
              command: buildAndPush
              containerRegistry: $(dockerRegistryServiceConnection)
              repository: $(imageRepository)
              tags: $(tag)

  - stage: DeployStaging
    dependsOn: Build
    jobs:
      - deployment: staging
        environment: orders-staging        # approvals and checks are configured on the environment
        pool:
          vmImage: ubuntu-latest
        strategy:
          runOnce:
            deploy:
              steps:
                - checkout: self
                - task: AzureCLI@2
                  inputs:
                    azureSubscription: azure-deploy-connection
                    scriptType: bash
                    scriptLocation: inlineScript
                    inlineScript: |
                      az aks get-credentials --resource-group rg-orders --name aks-staging
                      helm upgrade --install orders-api ./chart --namespace orders-staging \
                        --set image.repository=myacr.azurecr.io/$(imageRepository) \
                        --set image.tag=$(tag) --wait --rollback-on-failure --timeout 5m
```

| Concept | GitHub Actions | Jenkins | Azure DevOps |
| --- | --- | --- | --- |
| Pipeline file | `.github/workflows/*.yml` | `Jenkinsfile` (Groovy DSL) | `azure-pipelines.yml` |
| Unit of work | job, step | stage, step | stage, job, step or task |
| Runner | GitHub-hosted or self-hosted runner | agent (node label) | Microsoft-hosted or self-hosted agent pool |
| Secrets | Secrets and variables, OIDC | Credentials plugin, `withCredentials` | Variable groups, Key Vault link, service connections |
| Approvals | Environment protection rules | `input` step or directive | Environment approvals and checks |
| Reuse | Reusable workflows, composite actions | Shared libraries | Templates |

- **Jenkins pitfalls:** Groovy string interpolation of secrets (`sh "echo ${PASSWORD}"`) leaks them into the build log and process list; use single-quoted `sh` so the shell expands them.
  A stage-level `input` directive waits without holding an executor; an `input` step inside `steps` holds the agent while a human decides.
- Experience hook: **[fill in: the Jenkins setup you used (the resume lists Jenkins): shared libraries, agents, how deploys were gated]**.

---

## D17. Walk me through resolving a merge conflict, cherry-picking a fix, and finding the commit that broke something.

- **Conflict:** `git fetch`, `git rebase origin/main` (or merge), open each conflicted file, understand **both** sides (read the other change's intent, not just the markers), edit, run the tests, `git add`, `git rebase --continue`.
  Talk to the other author when the intent is unclear; `git rerere` remembers resolutions for repeated rebases.
- **Cherry-pick a hotfix:** `git cherry-pick -x <sha>` onto the release branch; `-x` records the original SHA in the message for traceability.
  Prefer fixing on `main` first and cherry-picking to the release branch, so the fix is never lost.
- **Undo safely:** `git revert <sha>` creates an inverse commit (safe on shared branches); `git reset` rewrites history (only for local, unpushed work).
- **Find the breaking commit with bisect** (verified on a scratch repository with a planted regression; `bisect run` found the right commit):

```sh
git bisect start HEAD v1.0                 # bad commit first, then a known good one
git bisect run pytest -x -q tests/test_fees.py   # exit 0 = good, 1-127 = bad, 125 = skip
git bisect reset
```

- **Code review etiquette:** small pull requests (under about 400 lines), a description with the why and how it was tested, review for correctness and design before style (let ruff own style), comments phrased as questions or suggestions with a reason, label nits as nits, approve with minor comments rather than blocking, respond to every comment.

---

## D18. Map the AWS services to Azure and GCP.

| Need | AWS | Azure | GCP |
| --- | --- | --- | --- |
| Managed Kubernetes | EKS | AKS | GKE |
| Simple container hosting | ECS on Fargate | Azure Container Apps, App Service | Cloud Run |
| Functions | Lambda | Azure Functions | Cloud Run functions |
| PostgreSQL / MySQL | RDS, Aurora | Azure Database for PostgreSQL or MySQL (Flexible Server) | Cloud SQL, AlloyDB |
| SQL Server | RDS for SQL Server | Azure SQL Database, Managed Instance | Cloud SQL for SQL Server |
| NoSQL | DynamoDB | Cosmos DB | Firestore, Bigtable |
| Object storage | S3 | Blob Storage | Cloud Storage |
| Queue | SQS | Service Bus queues, Storage queues | Pub/Sub, Cloud Tasks |
| Pub/sub and events | SNS, EventBridge | Service Bus topics, Event Grid | Pub/Sub, Eventarc |
| Managed Kafka | MSK | Event Hubs (Kafka endpoint), Confluent on Azure | Managed Service for Apache Kafka |
| Cache | ElastiCache | Azure Cache for Redis / Azure Managed Redis | Memorystore |
| Secrets | Secrets Manager, SSM Parameter Store | Key Vault | Secret Manager |
| Identity for workloads | IAM roles (IRSA, Pod Identity, task roles) | Managed identities, Entra Workload ID | Service accounts, Workload Identity Federation |
| User identity / SSO | IAM Identity Center, Cognito | Microsoft Entra ID | Cloud Identity, Identity Platform |
| Registry | ECR | ACR | Artifact Registry |
| API gateway | API Gateway | API Management | API Gateway, Apigee |
| Observability | CloudWatch, X-Ray | Azure Monitor, Application Insights, Log Analytics | Cloud Monitoring, Cloud Logging, Cloud Trace |
| CI/CD (native) | CodePipeline, CodeBuild | Azure DevOps Pipelines, GitHub Actions | Cloud Build, Cloud Deploy |

- Azure Active Directory was renamed **Microsoft Entra ID**; use the new name.
- Google renamed Cloud Functions to **Cloud Run functions** in 2024.
- Product names move fast; verify the few the target company uses before the interview.

---

## D19. Serverless (Lambda plus API Gateway) or containers for a FastAPI service?

```python
from mangum import Mangum

from app.main import create_app

app = create_app()
# lifespan="auto" runs FastAPI's lifespan on cold start; the handler is reused across warm invocations.
handler = Mangum(app, lifespan="auto")
```

The test feeds an API Gateway HTTP API (payload version 2.0) event to `handler` and gets the FastAPI response back, so the same app runs in a container and on Lambda.
Mangum is maintained: 0.22.0 was released in August 2026.
The AWS Lambda Web Adapter is the other common route (run the unchanged uvicorn server inside Lambda).

| | Lambda + API Gateway | Containers (ECS, EKS, Cloud Run) |
| --- | --- | --- |
| Scaling | Per request, to zero, automatic | Replicas and HPA; Cloud Run and Fargate reduce ops |
| Cost | Pay per request and duration; cheap at low or spiky volume | Pay for provisioned capacity; cheaper at steady high volume |
| Cold starts | Yes (heavier with large dependencies; mitigations: provisioned concurrency, SnapStart for Python on recent runtimes, smaller packages) | No, once warm |
| Limits | 15-minute max duration, payload size limits, API Gateway integration timeout of about 30 s by default | Your own limits |
| Connections | Each concurrent invocation is its own process: database connection storms; use RDS Proxy | Normal pooling |
| Background work and WebSockets | Needs other services (SQS, Step Functions, API Gateway WebSockets) | Native |
| Local dev and debugging | Harder | Same image locally and in production |

- **When Lambda wins:** spiky or low traffic, event handlers (S3 upload triggers, SQS consumers, schedules), glue code, teams without platform engineers.
- **When containers win:** steady traffic, low-latency SLAs, long requests, WebSockets or streaming, heavy dependencies, and portability across clouds.

---

## D20. How do you keep images small and secure?

- Slim or distroless base, multi-stage build, no compilers or package caches in the runtime stage, `.dockerignore`.
- **Scan** images in CI and in the registry (Trivy, Grype, Docker Scout, ECR scanning with Inspector, Defender for Containers on Azure); fail on fixable HIGH and CRITICAL, triage the rest with an owner and an expiry.
- **Rebuild regularly** even without code changes, so base-image security patches land; pin the base by digest and let Dependabot or Renovate bump it.
- Non-root user, read-only root filesystem, drop all Linux capabilities, `allowPrivilegeEscalation: false`, seccomp `RuntimeDefault` (all in the manifest in D3).
- **Supply chain:** hashed lock files, `pip-audit`, SBOM (Syft, `docker buildx --sbom`), signing and verification (cosign), and an admission policy that only allows signed images from your registry.
- Size matters less for security than content: fewer packages means fewer CVEs and less for an attacker to use.

---

## D21. What is Helm, and how do you use it?

- Helm is a package manager for Kubernetes: a **chart** is a set of templated manifests plus `values.yaml`; a **release** is an installed instance with revision history.
- `helm upgrade --install` is idempotent (install or upgrade); `helm rollback <release> <revision>`; `helm template` renders locally (useful in CI to validate or diff); `helm lint` checks the chart.
- **Per-environment values:** `values.yaml` for defaults, `-f values-prod.yaml` for overrides, `--set image.digest=...` from CI.

```yaml
replicaCount: 3
image:
  repository: registry.example.com/orders-api
  tag: ""        # set one of tag or digest from CI; digest wins
  digest: ""
env:
  LOG_LEVEL: INFO
resources:
  requests:
    cpu: 500m
    memory: 512Mi
  limits:
    memory: 512Mi
```

```yaml
{{- define "orders-api.image" -}}
{{- if .Values.image.digest -}}
{{ .Values.image.repository }}@{{ .Values.image.digest }}
{{- else -}}
{{ .Values.image.repository }}:{{ required "image.tag or image.digest is required" .Values.image.tag }}
{{- end -}}
{{- end -}}
```

- The `required` helper fails `helm template` with a clear message when CI forgets to pass an image (verified).
- **Pitfalls:** giant charts with every field templated (unreadable), secrets in values files committed to Git (use External Secrets or SOPS), and drift from manual `kubectl edit`.
- **Alternatives:** Kustomize (overlays, no templating, built into `kubectl`), and GitOps controllers (Argo CD, Flux) that sync the cluster from a Git repository instead of CI pushing with `helm upgrade`.

---

## D22. What observability stack would you run, at a glance?

| Stack | Metrics | Logs | Traces | Notes |
| --- | --- | --- | --- | --- |
| Open source | Prometheus + Grafana | Loki, or ELK/EFK (Elasticsearch, Fluent Bit or Logstash, Kibana) | Tempo or Jaeger | OpenTelemetry Collector in front of all of it |
| AWS native | CloudWatch Metrics, Alarms | CloudWatch Logs (Logs Insights) | X-Ray, or ADOT to any backend | Container Insights for ECS and EKS |
| Azure native | Azure Monitor metrics | Log Analytics (KQL) | Application Insights | OpenTelemetry distro for Python |
| GCP native | Cloud Monitoring | Cloud Logging | Cloud Trace | |
| Commercial | Datadog, New Relic, Dynatrace, Splunk | same | same | Fastest to adopt, expensive at volume |

- **Instrument once with OpenTelemetry** (FastAPI, Flask, SQLAlchemy, httpx, Kafka instrumentations) and choose the backend by configuration; this keeps you portable.
- Log JSON to stdout; let the platform collect it; never write log files inside the container.
- Golden signals per service: latency (p50, p95, p99), traffic, errors, saturation (CPU throttling, memory, DB pool, queue lag); SLO-based alerts with runbooks.
- Details on tracing and correlation IDs: [Microservices and Messaging M19](08-Microservices-and-Messaging.md); on production debugging: [Testing, Debugging, Production](10-Testing-Debugging-Production.md).

---

## Go deeper

Vault notes:

- [Productionizing Python: Docker and Kubernetes](../Python_Zero_to_Godhood/Chapter_65_Productionizing_Python_Docker_and_Kubernetes.md)
- [Cloud and Distributed](../Python_Zero_to_Godhood/Chapter_99_CLOUD_AND_DISTRIBUTED.md)
- [Microservices and gRPC in Python](../Python_Zero_to_Godhood/Chapter_63_Microservices_and_gRPC_in_Python.md)
- [Development Practices](../../../10-Development-Practices/README.md), with its [CI pipeline example](../../../10-Development-Practices/02-CICD/ci_pipeline.yaml) and [Dockerfile example](../../../10-Development-Practices/03-Cloud-Native/Dockerfile) (older Python and action versions; compare them with D2 and D5)
- [Unit testing comparison](../../../10-Development-Practices/01-Testing/unit_testing_comparison.md)
- [System Design Basics](../../../04-System-Design/00-Concepts/system_design_basics.md)

Pack siblings: [Microservices and Messaging](08-Microservices-and-Messaging.md), [Testing, Debugging, Production](10-Testing-Debugging-Production.md), [System Design](12-System-Design.md), [Auth and Security](07-Auth-and-Security.md).

Official docs:

- [Docker build best practices](https://docs.docker.com/build/building/best-practices/)
- [Kubernetes probes](https://kubernetes.io/docs/concepts/configuration/liveness-readiness-startup-probes/), [pod lifecycle and termination](https://kubernetes.io/docs/concepts/workloads/pods/pod-lifecycle/), [container lifecycle hooks](https://kubernetes.io/docs/concepts/containers/container-lifecycle-hooks/)
- [gunicorn settings](https://gunicorn.org/reference/settings/), [uvicorn deployment](https://uvicorn.dev/deployment/)
- [GitHub Actions](https://docs.github.com/en/actions), [Jenkins pipeline syntax](https://www.jenkins.io/doc/book/pipeline/syntax/), [Azure Pipelines YAML schema](https://learn.microsoft.com/en-us/azure/devops/pipelines/yaml-schema/)
- [Helm docs](https://helm.sh/docs/), [Mangum](https://github.com/Kludex/mangum)
- [Trivy supply-chain advisory GHSA-69fq-xp46-6x23](https://github.com/aquasecurity/trivy/security/advisories/GHSA-69fq-xp46-6x23)
