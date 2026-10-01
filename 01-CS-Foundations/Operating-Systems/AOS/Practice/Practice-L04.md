---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: L04
tags: [cs6210, cs6210/practice]
---

# Practice L04

Original exam-style questions for [L04a](../Part-2-Parallel-Systems/L04a-Shared-Memory-Machines.md), [L04b](../Part-2-Parallel-Systems/L04b-Synchronization.md), [L04c](../Part-2-Parallel-Systems/L04c-Barrier-Synchronization.md), [L04d](../Part-2-Parallel-Systems/L04d-Lightweight-RPC.md), [L04e](../Part-2-Parallel-Systems/L04e-Scheduling.md), [L04f](../Part-2-Parallel-Systems/L04f-Shared-Memory-Multiprocessor-OS.md).
Each question names the coverage ids it exercises; answers are folded so this page works as a self-test.

## Shared Memory Machines

> [!question]- Q1. How do the topologies of dance hall architecture, symmetric multiprocessor (SMP) architecture, and distributed shared memory (DSM) architecture fundamentally differ, and how do these differences dictate the scalability of shared memory machines? (concepts: L04a-01, L04a-02, L04a-03, L04a-10)
> Dance hall architectures place processors on one side of an interconnection network and memory on the other.
> SMP architectures connect processors to a single shared bus and memory, which limits scalability due to bus contention.
> DSM architectures distribute memory physically among nodes while maintaining a logically shared address space, allowing much greater scalability.
> The interconnection network and physical distribution of memory are the primary factors that determine scalability across these designs.

> [!question]- Q2. What is the distinction between a memory consistency model and cache coherence, and how does sequential consistency illustrate this difference? (concepts: L04a-04, L04a-05, L04a-06)
> Cache coherence ensures that all processors see a uniform view of a single memory location.
> A memory consistency model defines the ordering of memory operations across all memory locations as perceived by different processors.
> Sequential consistency is a strict memory consistency model that requires all memory operations to appear as if executed in some sequential order, regardless of the underlying cache coherence protocol.
> Therefore, coherence is about a single variable, while consistency is about the global ordering of all variables.

> [!question]- Q3. Under what conditions would a write-update protocol be preferable to a write-invalidate protocol in cache-coherent multiprocessors? (concepts: L04a-08, L04a-09)
> A write-update protocol is preferable when shared data is frequently read by multiple processors after it is written.
> Write-invalidate is better for bursty writes by a single processor, as it avoids broadcasting every individual write.
> Write-update broadcasts the new value to all caches holding the line, reducing read latency for subsequent readers at the cost of higher bus traffic.
> The choice depends heavily on the read-to-write ratio and the sharing patterns of the workload.

> [!question]- Q4. How do non-cache-coherent multiprocessors handle shared data, and how do modern descendants with weak memory models and C11 atomics provide synchronization without strict hardware coherence? (concepts: L04a-07, L04a-11)
> Non-cache-coherent multiprocessors rely on software to explicitly flush or invalidate caches when sharing data.
> Modern descendants utilize weak memory models where hardware provides minimal ordering guarantees to maximize performance.
> Programmers use C11 atomics and memory barriers to enforce ordering only when necessary for synchronization.
> This shifts the burden of maintaining consistency from hardware protocols to software primitives.

## Synchronization

> [!question]- Q5. How does the caching spinlock (spin on read) improve upon the naive spinlock (spin on test-and-set) regarding atomic read-modify-write instructions and scalability issues like latency? (concepts: L04b-02, L04b-03, L04b-04, L04b-05)
> The naive spinlock repeatedly executes atomic read-modify-write instructions, generating intense interconnect traffic and increasing latency.
> The caching spinlock spins on a cached copy of the lock value using standard reads.
> It only attempts an atomic instruction when the cached value changes, significantly reducing bus contention.
> This reduction in interconnect traffic directly mitigates scalability issues related to latency.

> [!question]- Q6. In the context of the associated paper, compare the Anderson array-based queuing lock with the MCS linked-list queuing lock. (concepts: L04b-08, L04b-09, P-MCS-Scalable-Synchronization)
> The Anderson lock uses a circular array where each processor spins on its own designated slot, eliminating global contention but requiring statically allocated space proportional to the number of processors.
> The MCS lock uses a distributed linked list where each processor spins on a locally allocated node.
> The MCS lock achieves minimal space per waiting processor and constant network traffic per lock acquisition.
> The algorithm demonstrates that the MCS lock scales better because it does not require statically sizing an array.

> [!question]- Q7. Why does the MCS lock need compare-and-swap on release, and how is the algorithm modified if the hardware provides an MCS lock with only fetch-and-store? (concepts: L04b-10, L04b-11)
> The MCS lock uses compare-and-swap on release to safely transition an empty queue to an unlocked state without race conditions if a new requester arrives concurrently.
> If only fetch-and-store is available, the releaser must hand off the lock differently.
> The releaser sets its own node's next pointer to a special value if it thinks there are no successors.
> This creates a small window where a new requester might have to wait for the releaser to finish, slightly complicating the protocol compared to using compare-and-swap.

> [!question]- Q8. How does the ticket lock address fairness among lock and barrier primitives, and how does a spinlock with delay and exponential backoff attempt to achieve similar performance goals? (concepts: L04b-01, L04b-06, L04b-07, L04b-12)
> The ticket lock guarantees strict FIFO fairness by assigning sequential numbers to requesters.
> However, it still causes global bus traffic when the shared counter is updated.
> A spinlock with delay and exponential backoff reduces bus traffic by having processors wait for increasing durations before retrying, though it sacrifices fairness.
> Modern descendants like the Linux qspinlock combine queuing structures to achieve both fairness and high performance without the severe global contention of simple ticket locks.

## Barrier Synchronization

> [!question]- Q9. What is the primary bottleneck of a centralized counting barrier, and how does a sense-reversing barrier solve this specific issue? (concepts: L04c-01, L04c-02)
> A centralized counting barrier uses a single shared counter, which becomes a severe bottleneck due to contention as all processors attempt to update and read it simultaneously.
> Additionally, processors must wait for the barrier to reset before it can be reused, risking race conditions on consecutive barrier calls.
> A sense-reversing barrier toggles a boolean flag for each barrier episode.
> This eliminates the need for a separate reset phase, preventing processors from accidentally advancing into the next phase prematurely.

> [!question]- Q10. Compare the structure of a combining tree barrier with the MCS tree barrier featuring 4-ary arrival. (concepts: L04c-03, L04c-04)
> A combining tree barrier organizes processors into a tree topology where arrivals are propagated up the tree and wakeups are broadcast down the tree.
> This distributes the contention across multiple nodes rather than a single counter.
> The MCS tree barrier specifically uses a 4-ary arrival tree to optimize the depth of the tree.
> A 4-ary tree balances the number of children per node against the height of the tree, minimizing network traffic and latency during the barrier operation.

> [!question]- Q11. When performing a barrier algorithm comparison regarding space and network traffic, how does the tournament barrier differ from the dissemination barrier? (concepts: L04c-05, L04c-06, L04c-07)
> The tournament barrier organizes processors in a binary tree where winners of each round advance to the next, requiring a logarithmic number of rounds and a corresponding amount of space for flags.
> The dissemination barrier operates in a logarithmic number of rounds but uses a specific communication pattern where each processor signals another processor at an exponentially increasing distance.
> The dissemination barrier requires more space per processor for synchronization flags compared to the tournament barrier.
> However, the dissemination barrier often achieves lower latency because it does not have a strict hierarchical critical path.

## Lightweight RPC

> [!question]- Q12. According to the foundational paper on the topic, why is RPC on the same machine so common? (concepts: L04d-01, P-LRPC)
> Microkernel architectures place many OS services in separate user-level processes rather than a monolithic kernel.
> Consequently, client applications must frequently communicate with these local service processes to perform routine tasks.
> This makes RPC on the same machine highly common, as it is the primary mechanism for inter-process communication in these systems.
> The paper addresses the performance bottleneck this creates by optimizing local calls.

> [!question]- Q13. What are the primary costs of RPC concerning copies and control transfer in a traditional system, and how does making RPC cheaper through binding and the A-stack help? (concepts: L04d-02, L04d-03)
> Traditional RPC involves multiple data copies between the client, kernel, and server, as well as expensive context switches for control transfer.
> The system reduces these costs by performing early binding, where the client and server set up shared state during initialization.
> This shared state includes the A-stack, which is mapped into both address spaces.
> Using the A-stack allows arguments to be passed directly without being copied through the kernel.

> [!question]- Q14. How does the system utilize a shared argument stack and copy reduction to improve performance during domain switching and handoff scheduling, particularly for RPC on SMP? (concepts: L04d-04, L04d-05, L04d-06)
> The shared argument stack eliminates the need to copy data between client and server buffers, achieving significant copy reduction.
> During a call, the kernel performs a domain switch to change the address space but uses handoff scheduling to directly yield the CPU to the server thread.
> This bypasses the standard scheduler, reducing latency.
> For RPC on SMP, caching domains on idle processors allows the system to execute the server routine on an idle core, further eliminating the context switch on the calling core.

## Scheduling

> [!question]- Q15. How do scheduling goals and cache affinity influence the choice between first come first served and fixed processor scheduling? (concepts: L04e-01, L04e-02, L04e-03)
> First come first served scheduling ignores where a thread previously ran, which can destroy cache affinity and degrade performance.
> Fixed processor scheduling binds a thread to a specific processor, guaranteeing maximal cache affinity.
> However, fixed processor scheduling can lead to severe load imbalance if some processors have long-running threads while others sit idle.
> Scheduling goals must balance the benefit of cache affinity against the need for load balancing.

> [!question]- Q16. Compare last processor scheduling with the minimum intervening policy and minimum intervening with limited queue policies as described in the relevant paper. (concepts: L04e-04, L04e-05, L04e-06, P-Cache-Affinity-Scheduling)
> Last processor scheduling attempts to schedule a thread on the processor it last executed on to reuse cached data.
> The minimum intervening policy refines this by quantifying cache disruption based on the number of other threads that have run on that processor.
> Minimum intervening with limited queue prevents excessive waiting by allowing a thread to migrate if the queue for its preferred processor is too long.
> The paper shows that these policies provide a structured trade-off between affinity and load balance.

> [!question]- Q17. How do implementation issues regarding global versus per-processor queues affect performance metrics like throughput, response time, and variance? (concepts: L04e-07, L04e-08)
> Global queues provide perfect load balancing but suffer from severe lock contention, reducing throughput on large systems.
> Per-processor queues eliminate contention but can cause high variance in response time due to load imbalances.
> Workloads on lightly loaded processors will have much better response times than those on heavily loaded processors.
> Hybrid approaches often use per-processor queues combined with periodic balancing mechanisms.

> [!question]- Q18. How do load balancing and work stealing mechanisms integrate with cache-aware scheduling on multicore and multithreaded processors, and how do modern descendants like Linux CFS and EEVDF apply these concepts? (concepts: L04e-09, L04e-10, L04e-11, P-Multithreaded-Chip-Multiprocessors)
> Work stealing allows idle processors to take tasks from busy processors, dynamically achieving load balancing.
> The paper demonstrates that cache-aware scheduling on chip multiprocessors must account for shared caches between cores.
> Modern descendants like Linux CFS and EEVDF use scheduling domains to represent the cache hierarchy.
> These domains guide work stealing so that threads migrate between processors sharing a cache before migrating to distant processors.

## Shared Memory Multiprocessor OS

> [!question]- Q19. What are the primary challenges of parallel systems when dealing with a NUMA architecture, and how does false sharing impact OS design principles for scalability? (concepts: L04f-01, L04f-02, L04f-03, L04f-04)
> In a NUMA architecture, remote memory accesses are significantly slower than local accesses, making data placement critical.
> The main challenges of parallel systems include avoiding bottlenecks from shared data structures and minimizing interconnect traffic.
> False sharing occurs when independent variables reside on the same cache line, causing unnecessary invalidations when modified by different processors.
> OS design principles for scalability dictate that data should be partitioned or aligned to cache lines to prevent false sharing and contention.

> [!question]- Q20. According to the relevant paper, how does the system handle page fault service in a parallel OS using Tornado clustered objects? (concepts: L04f-05, L04f-06, P-Tornado)
> The paper redesigns the OS by representing every resource as a Tornado clustered object.
> During page fault service in a parallel OS, multiple threads can concurrently fault on different pages of the same memory region.
> By using clustered objects, the region object can be distributed across processors.
> This prevents the page fault handler from becoming a serialized bottleneck.

> [!question]- Q21. How does the degree of clustering regarding replicated, partitioned, and true shared strategies influence the design of object references, translation tables, and miss handling? (concepts: L04f-07, L04f-08)
> The degree of clustering allows an object to be replicated per processor, partitioned across nodes, or maintained as a true shared structure.
> Object references are essentially local pointers that go through translation tables on each processor.
> If a local representation does not exist, miss handling logic is invoked to instantiate the appropriate local component of the clustered object.
> This indirection allows the implementation strategy to vary without changing the object's interface.

> [!question]- Q22. How does the objectization of memory management and the use of a protected procedure call facilitate dynamic memory allocation in a parallel OS? (concepts: L04f-09, L04f-10, L04f-12)
> The objectization of memory management encapsulates memory structures like page tables and region maps into localized objects.
> A protected procedure call allows clients to invoke operations on these objects efficiently without traditional heavy kernel traps.
> Dynamic memory allocation in a parallel OS uses these mechanisms to provide per-processor memory pools.
> This ensures that memory allocation can occur concurrently without central locks.

> [!question]- Q23. How does the system ensure safe object destruction using hierarchical locking and existence guarantees, and how does this relate to modern descendants that use per-CPU data and RCU in Linux? (concepts: L04f-11, L04f-15)
> The system uses existence guarantees, which delay the destruction of an object until it is guaranteed that no processor holds a reference to it.
> This is coupled with hierarchical locking to allow safe concurrent access without grabbing a global lock first.
> Modern descendants use Read-Copy-Update in Linux to achieve similar existence guarantees.
> This allows readers to access per-CPU data without locking, deferring reclamation until all readers have finished their grace periods.

> [!question]- Q24. How do address ranges, shares, and dedicated cores in Corey improve scalability, and how does Cellular Disco provide virtualization for scalability and fault containment? (concepts: L04f-13, L04f-14, P-Corey, P-Cellular-Disco)
> Corey introduces address ranges and shares to give applications explicit control over which OS resources are shared versus private.
> It also introduces dedicated cores to isolate specific OS functions, reducing cache pollution and lock contention.
> Cellular Disco runs a virtual machine monitor across the multiprocessor.
> It provides virtualization for scalability and fault containment by isolating faults within virtual machines and dynamically migrating workloads to balance resources.
