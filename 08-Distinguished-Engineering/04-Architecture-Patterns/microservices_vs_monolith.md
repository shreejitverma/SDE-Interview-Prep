# Microservices vs Monolith

**Topic:** Choosing the right architecture.

## 1. Monolithic Architecture
A single codebase, single executable, single database.

*   **Pros:**
    *   Simple to develop, test, and deploy (initially).
    *   No network latency between components.
    *   ACID transactions are easy.
*   **Cons:**
    *   **Scale:** Hard to scale specific parts (e.g., Image Processing needs GPU, but the rest doesn't).
    *   **Coupling:** A bug in one module can crash the whole app.
    *   **Technology Lock-in:** Hard to rewrite one module in a different language.

## 2. Microservices Architecture
Small, independent services communicating via APIs (HTTP/gRPC) or Events.

*   **Pros:**
    *   **Scalability:** Scale services independently.
    *   **Resilience:** Service A crashing doesn't kill Service B.
    *   **Flexibility:** Service A in Python, Service B in Go.
*   **Cons:**
    *   **Complexity:** Distributed transactions (SAGA), Service Discovery, Network failures.
    *   **Observability:** Need distributed tracing (Jaeger/Zipkin).

## 3. When to use what?
*   **Start with a Monolith.** (Don't over-engineer early).
*   **Move to Microservices when:**
    *   Team size grows (>20 engineers).
    *   Traffic scales significantly.
    *   Domains become clearly separated (e.g., Billing vs Search).
