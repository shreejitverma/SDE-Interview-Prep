# Chapter 113: C++ in the Cloud

C++ is often dismissed as a "systems language," not a cloud language — but that intuition is wrong: in cloud-native architectures where you pay per millisecond of compute and per gigabyte of memory, C++'s speed and tiny footprint translate *directly* into lower bills and better latency. This chapter covers C++ as a first-class cloud citizen: high-throughput microservices, serverless functions where its fast cold start is a decisive advantage, and the deployment realities (containers, observability) that make native services practical. The recurring theme is that the performance disciplines of this book become *cost* disciplines in the cloud.

## Chapter Roadmap

- 113.1 Why C++ Belongs in the Cloud
- 113.2 High-Throughput Microservices
- 113.3 Serverless C++ and Cold-Start Advantage
- 113.4 Containers and Static Linking
- 113.5 Observability and Operational Reality
- 113.6 The Cloud Cost Discipline

---

## 113.1 Why C++ Belongs in the Cloud

Cloud economics invert the usual "developer time over machine time" calculus at scale: when you run thousands of instances or bill per-millisecond on serverless, the *machine* cost dominates, and a service that uses half the CPU and a tenth the memory of its Java/Node equivalent costs proportionally less to run. Modern C++ is a first-class citizen in cloud-native architectures precisely because its zero-overhead abstractions produce small, fast binaries.

> **Why this matters.** Every performance property this book teaches — low latency, low memory, no GC pauses, predictable tails — becomes a *cost lever* in the cloud. Lower CPU per request means fewer instances for the same load (lower compute bill). Lower memory footprint means more instances per host or a cheaper instance class. Fast startup means serverless functions bill for less duration. No GC means no pause-induced latency that forces over-provisioning to hit an SLA. The cloud is where the determinism discipline of Volume 8 pays a literal dividend.

---

## 113.2 High-Throughput Microservices

Modern C++ web frameworks — **Drogon**, **Userver** (Yandex), **oat++**, and the Seastar-based stacks — deliver microservices that handle far more requests per core than interpreted or JVM frameworks.

```cpp
// Min standard: C++17 + Drogon (non-portable: requires the framework). An HTTP handler.
void Handler::get(const HttpRequestPtr& req,
                  std::function<void(const HttpResponsePtr&)>&& callback) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody("Hello from a high-performance microservice!");
    callback(resp);                                 // async completion via callback
}
```
*Listing 113.1 — A Drogon-style async HTTP handler. The callback model is the reactor pattern of Chapter 112.*

> **Why this matters / cost model.** These frameworks are built on the async event-loop architecture of Chapter 112 (an `epoll`/`io_uring` reactor) and the concurrency patterns of Chapter 78 — which is why they sustain hundreds of thousands of requests per second per core where a thread-per-request framework saturates far sooner. The handler is *asynchronous* (it completes via a callback or coroutine rather than blocking) so one event loop serves many concurrent requests without the thread-per-request overhead. The practical payoff is density: a C++ microservice often does the work of several JVM instances, directly reducing the instance count and thus the bill. The trade-off is ecosystem maturity — fewer batteries-included libraries than Spring or Express — so C++ microservices are chosen when throughput density justifies building more yourself.

---

## 113.3 Serverless C++ and Cold-Start Advantage

In **serverless** (AWS Lambda, Google Cloud Functions), you are billed for execution *duration* and the function is spun up on demand — so **cold-start time** (the latency before the first request can be served) is a critical metric, and it is exactly where C++ excels.

```cpp
// Min standard: C++11 + AWS Lambda C++ runtime (non-portable). A native Lambda handler.
#include <aws/lambda-runtime/runtime.h>
using namespace aws::lambda_runtime;

invocation_response handler(invocation_request const& req) {
    return invocation_response::success("Processed!", "application/json");
}
int main() { run_handler(handler); return 0; }
```
*Listing 113.2 — A native AWS Lambda handler. Cold start is sub-5ms vs 100ms+ for JVM runtimes. Non-portable.*

> **Why this matters / cost model.** A native C++ binary has *no runtime to initialise* — no JVM to warm up, no interpreter to start, no large dependency graph to load — so its cold start is typically **under 5 ms** versus 100ms+ for Java or .NET and tens of ms for Node. This matters twice: cold-start latency is user-visible (the first request after a scale-up is slow on heavy runtimes), and you *pay* for the cold-start duration on every cold invocation. Combined with C++'s faster per-request execution (you bill for less duration) and lower memory (serverless prices by memory tier), native functions can be dramatically cheaper for latency-sensitive or bursty workloads. The cost is developer ergonomics — the C++ Lambda toolchain is less polished than Python's — so it suits performance-critical functions, not glue code.

---

## 113.4 Containers and Static Linking

Cloud services ship as **container images**, and here C++'s linking model (Chapter 102) becomes a deployment lever. A **statically-linked** C++ binary has no external shared-library dependencies, so it runs in a minimal "distroless" or `scratch` container — a few megabytes versus hundreds for an image carrying a JVM or interpreter and OS libraries.

> **Why this matters / cost model.** Image size affects real costs: smaller images pull faster (lower scale-up latency, lower registry bandwidth), have a smaller attack surface (fewer libraries means fewer CVEs — Chapter 121), and start faster. A statically-linked C++ service in a `scratch` container can be 5–20 MB total; the equivalent JVM service image is often 200–500 MB. The trade-off is the one from Chapter 102: static linking produces larger *binaries* (no shared memory across processes) and forgoes runtime library updates — but in a container, where each service is isolated and rebuilt to update anyway, that trade-off favours static linking strongly. This is why Go (which static-links by default) dominated cloud-native, and why static C++ binaries get the same benefits.

---

## 113.5 Observability and Operational Reality

A cloud service is only as good as your ability to *operate* it: **observability** — structured logging, metrics, and distributed tracing — is non-negotiable, and C++ integrates with the standard stack (OpenTelemetry, Prometheus). The discipline from Volume 8 applies: logging and metrics must stay *off the hot path* (Chapter 106's hot/cold split), emitted via a ring buffer to a background thread, so observability does not become a latency source.

> **Why this matters.** The operational gap is the real reason teams hesitate to use C++ in the cloud — not performance, but the maturity of tooling for logging, tracing, config, and health checks. Modern C++ closes this: OpenTelemetry has a C++ SDK, Prometheus client libraries exist, and structured logging frameworks (spdlog) are excellent. The key engineering point is that the *same* hot-path discipline that makes the service fast (no allocation, no blocking, no syscalls inline) must be applied to its instrumentation — a synchronous log write or a metrics mutex on the request path reintroduces exactly the jitter you used C++ to avoid. Emit telemetry asynchronously, sample tracing, and keep the request path clean.

---

## 113.6 The Cloud Cost Discipline

| Property | C++ advantage | Cloud cost impact |
|---|---|---|
| Per-request CPU | Lower (no interpreter/JIT) | Fewer instances → lower compute bill |
| Memory footprint | Lower (no runtime heap) | Cheaper instance tier; higher density |
| Cold start | <5 ms (no runtime init) | Less serverless duration billed; faster scale-up |
| Image size | MBs (static link) | Faster pulls, smaller attack surface |
| GC pauses | None (RAII) | No over-provisioning for tail SLA |

> **The discipline.** C++ in the cloud is the recognition that, at scale and on serverless, *machine efficiency is money* — and C++'s speed, small footprint, fast startup, and absence of GC pauses translate directly into lower bills and better latency. Build cloud services on the async reactor architecture (Chapter 112), keep telemetry off the hot path (Chapter 106), static-link into minimal containers (Chapter 102), and treat the performance disciplines of this book as cost-optimisation disciplines. The same engineering that makes a trading system fast makes a cloud service cheap. The next chapters extend C++'s reach to the browser, mobile, and the desktop.
