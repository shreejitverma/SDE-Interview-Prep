---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
tags: [cs6210, cs6210/cheatsheet]
---

# Part 3 Cheat Sheet: Distributed Systems

## Lamport Clocks and Total Ordering

Lamport logical clocks establish a partial ordering of events in a distributed system, which can be extended to a total order.

- **Happened-before relation ($\to$)**: If event $a$ happens before $b$, then $a \to b$.
- **Clock condition**: If $a \to b$, then $C(a) < C(b)$.
- The converse is not true; $C(a) < C(b)$ does not imply $a \to b$.
- **Send rule**: Process $i$ increments its clock $C_i$ and timestamps the outgoing message $T_m = C_i$.
- **Receive rule**: Process $j$ receives timestamp $T_m$ and updates its clock $C_j = \max(C_j, T_m) + 1$.
- **Total order ($\Rightarrow$)**: $a \Rightarrow b$ if $C(a) < C(b)$ or ($C(a) = C(b)$ and $P_a < P_b$) where $P$ is an agreed-upon process ID tie-breaker.

### Physical Clock Synchronization Bound

To prevent anomalous behavior where messages travel faster than physical clocks sync, Lamport defined a bound based on the minimum message delay $\mu$, maximum clock skew $\epsilon$, and drift rate $\kappa$.

- **Formula**: $\mu \ge \frac{\epsilon}{1 - \kappa}$

### Logical Clocks Comparison

| Clock Type | Respects Happened-Before | Detects Concurrency | Tracks Wall Time | Payload Size on Wire |
| :--- | :--- | :--- | :--- | :--- |
| **Lamport** | Yes (one direction) | No | No | 1 integer |
| **Vector** | Yes (both directions) | Yes | No | $N$ integers |
| **Hybrid Logical** | Yes (one direction) | No | Yes (within sync error) | 2 integers |
| **TrueTime** | Yes (via wait out) | No | Yes (with explicit error) | Interval + wait |

## Distributed Mutual Exclusion

Lamport proposed a distributed mutual exclusion algorithm using total ordering.

| Algorithm | Messages per entry | Key Mechanism | Best Used For |
| :--- | :--- | :--- | :--- |
| **Lamport (Unoptimized)** | $3(N - 1)$ | Broadcasts request, awaits all acks, broadcasts release. | Teaching the total order concepts. |
| **Lamport (Optimized)** | Approaches $2(N - 1)$ | Defer acknowledgments by sending a later request or release. | Optimized paths with heavy queuing. |
| **Ricart-Agrawala** | Exactly $2(N - 1)$ | Eliminates release wave by delaying the reply until it grants the lock. | Reliable networks where tighter bounds are needed. |
| **Central Server** | 2 or 3 | Single coordinator manages all requests, grants, and releases. | Large $N$ where a single point of failure is acceptable. |

## RPC and Latency Limits

Network latency in RPC is often dominated by software overhead and controller interaction, not raw network bandwidth.

- **Standard RPC Overheads**: Marshalling, unmarshalling, context switching, dynamic buffer allocation, and routing.
- **Direct Thread Awakening**: The network interrupt routine directly awakens the waiting server or client thread.
- This cuts wakeups in half compared to routing through a generic OS scheduler (e.g., Firefly RPC).
- **On-the-fly Buffer Recycling**: Shared memory buffers are tied to call table entries and swapped directly in the interrupt handler.
- This avoids costly dynamic memory allocation on the fast path.
- **Zero-copy**: Pre-allocating shared memory allows direct NIC DMA, avoiding copying between user and kernel spaces.

## Network Protocol Composition

Monolithic networking stacks limit flexibility.
Decoupling protocols enables dynamic execution and adaptability.

- **x-Kernel**: Provides an object-oriented framework where micro-protocols are composed as graphs.
- This standardizes the interface between protocol layers.
- **Active Networks (ANTS)**: Replaces passive packets with "capsules" that carry both data and the custom code to execute on intermediate routers.
- This allows dynamic deployment of new protocols.
- **Ensemble**: Uses micro-protocols to build robust distributed communication frameworks.
- It optimizes performance through event-driven execution and automated protocol graph heuristics.

## Object-Oriented Distributed Systems

Modern distributed systems abstract network communication behind object interfaces, managing state and security across address spaces.

### Spring Operating System

A strongly-typed, object-oriented microkernel OS separates interfaces from implementations using an Interface Definition Language (IDL).

- **Nucleus (Microkernel)**: Provides fundamental abstractions like domains (processes) and doors (IPC capabilities).
- **Doors**: Local, kernel-managed capabilities allow extremely fast cross-address-space communication.
- **Network Proxies**: User-level domains seamlessly extend local door invocations across the network.
- **Subcontracts**: A runtime mechanism completely decouples an object's IDL interface from its communication semantics (e.g., singleton, replicated, caching).
- **Memory Objects vs. Pager Objects**: Memory objects represent the abstraction of data, while pager objects handle the actual I/O.
- This decoupling separates data representation from physical storage management.

### Java RMI (Remote Method Invocation)

Brings distributed objects natively to the JVM, addressing distributed memory management and parameter passing.

- **Parameter Passing**: Local (non-remote) objects are passed by value (copied via serialization).
- Remote objects are passed by reference (a network stub).
- **Distributed Garbage Collection (DGC)**: Tracks remote references across JVMs using a leasing mechanism.
- This gracefully handles network partitions and client crashes without leaking server memory.
- **Equality Checks**: Using `.equals()` on a remote object only evaluates reference equality (whether the stubs point to the same remote backend).
- It never evaluates deep content equality.

### Enterprise JavaBeans (EJB)

A component architecture for building scalable enterprise distributed applications.
EJB design requires balancing concurrency, security, and communication costs.

| EJB Design Pattern | Concurrency | Security | Network Overhead | Best Use Case |
| :--- | :--- | :--- | :--- | :--- |
| **Coarse-grained Session Bean** | Low | High (logic in EJB tier) | Low (1 call per request) | Simple, non-parallelized business logic. |
| **DAO with Entity Beans** | High (parallel access) | Low (logic in Web tier) | High (many fine-grained calls) | High-concurrency needs where security is handled externally. |
| **Session Façade (Remote)** | High | High | High (internal RMI overhead) | Distributed deployments requiring strict security. |
| **Session Façade (Local Interfaces)** | High | High | Low (bypasses RMI internally) | Complex, high-performance applications on a single server. |
