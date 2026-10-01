---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed: 2026-10-01
sources: []
course: cs6210
lessons: [L06a, L06b, L06c]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-13-distributed-objects: Distributed objects: Java RMI, subcontract-style invocation, and a generic gRPC call

> [!info] Goal
> Make L06a, L06b, L06c concrete with real commands and measurements, demonstrating Java RMI, the Spring Subcontract pattern, and gRPC.

> [!warning] Honor code guard
> This lab deliberately does not implement a course project: generic greeter only; no gRPC store or vendor service (Project 3).

## Prerequisites
- The `aos` Lima VM is running. See `AOS/labs/setup/README.md` for details.
- Java 21, Python 3.12, and the `aos-venv` virtual environment are available in the VM.

## How it works

This lab explores distributed object invocation across three different models:
1.  **Spring Subcontract (L06b)**: A Python simulation of pluggable invocation. It separates the application logic from the communication mechanism. We measure the overhead of a Singleton subcontract versus a Replicated subcontract.
2.  **Java RMI (L06a)**: The classic Remote Method Invocation framework in Java. We define a `Hello` interface, implement it in a `Server`, and invoke it from a `Client`. A local `rmiregistry` tracks the object references.
3.  **gRPC (L06c)**: A modern RPC framework using Protocol Buffers. We define a `Greeter` service, generate Python stubs, and compare synchronous versus asynchronous invocation using Python's `asyncio`.

All measurements use the `CLOCK_MONOTONIC` clock (via `System.nanoTime()` in Java and `time.monotonic_ns()` in Python). We perform 10 runs and report the median invocation time to account for initialization jitter. NUMA effects are absent as the `aos` VM presents a single NUMA node.

## Run commands

Run the lab directly inside the VM using the setup script:

```bash
# From the repository root
01-CS-Foundations/Operating-Systems/AOS/labs/setup/run-in-vm.sh 01-CS-Foundations/Operating-Systems/AOS/labs/lab-13-distributed-objects run
```

To run tests:
```bash
01-CS-Foundations/Operating-Systems/AOS/labs/setup/run-in-vm.sh 01-CS-Foundations/Operating-Systems/AOS/labs/lab-13-distributed-objects test
```

## What you should see

Expected output shows the median invocation times for each model:

```text
--- Running Subcontract (Python) ---
[Singleton] invocation median time (10 runs): 15007577 ns
Singleton output: Worker A processed: test data
[Replicated] invoked 3 replicas, invocation median time (10 runs): 39529160 ns
Replicated output: Worker A processed: test data

--- Running Java RMI ---
Server ready
response: Hello, world from Java RMI!
RMI invocation median time (10 runs): 262585 ns

--- Running gRPC ---
Greeter client received: Hello, sync client!
gRPC sync invocation median time (10 runs): 476836 ns
Greeter client received: Hello, async client!
gRPC async invocation median time (10 runs): 519002 ns
```

*Note: Python subcontract times include an artificial `time.sleep(0.01)` inside the worker to simulate real work.*

## Experiments to try

1.  **Prediction**: What happens to the gRPC async invocation time if the number of concurrent asynchronous requests scales to 1,000?
    *   *Action*: Modify `greeter_client_async.py` to use `asyncio.gather` for 1,000 concurrent requests.
2.  **Prediction**: How does the Java RMI performance change if the client and server reside on different machines with 10ms network latency?
    *   *Action*: Use `sudo tc qdisc add dev lo root netem delay 10ms` to inject artificial network delay on the loopback interface, then observe the RMI median time.
3.  **Prediction**: If a replica in the Replicated subcontract fails (e.g., throws an exception), how does the subcontract handle it?
    *   *Action*: Modify `subcontract.py` so that `Worker B` raises a `RuntimeError`, and adapt the `ReplicatedSubcontract` to catch it and still return a valid result from the remaining workers.

## Questions

<details>
<summary>Why does the Replicated subcontract take longer than the Singleton subcontract?</summary>

The Replicated subcontract synchronously invokes the method on all three worker objects in a loop before returning the result. Because our simulated worker sleeps for 10ms, three sequential calls take roughly 30ms, compared to 10ms for a single call. In a production environment, you might dispatch these concurrently or wait only for a quorum.
</details>

<details>
<summary>How does Java RMI locate the server object?</summary>

The `Server` registers its exported object stub with the local `rmiregistry` listening on port 1099, binding it to the name "Hello". The `Client` queries the registry for the name "Hello" and receives the stub, which it then uses to marshal method arguments and forward the call over TCP to the actual server object.
</details>

<details>
<summary>Why use gRPC asynchronous stubs instead of synchronous ones?</summary>

Asynchronous stubs allow a single thread to manage multiple concurrent RPCs without blocking. When a client waits for the network response, Python's `asyncio` event loop can schedule other coroutines. This is critical for building highly scalable microservices that interact with multiple backends simultaneously.
</details>
