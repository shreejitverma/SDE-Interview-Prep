---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: L05
tags: [cs6210, cs6210/practice]
---

# Practice L05

Original exam-style questions for [L05a](../Part-3-Distributed-Systems/L05a-Distributed-Systems-Definitions.md), [L05b](../Part-3-Distributed-Systems/L05b-Lamport-Clocks.md), [L05c](../Part-3-Distributed-Systems/L05c-Latency-Limits.md), [L05d](../Part-3-Distributed-Systems/L05d-Active-Networks.md), [L05e](../Part-3-Distributed-Systems/L05e-Systems-from-Components.md).
Each question names the coverage ids it exercises; answers are folded so this page works as a self-test.

## Distributed Systems Definitions

> [!question]- Q1. What defines a distributed system in terms of spatial separation and time? (concepts: L05a-01)
> A distributed system is a collection of distinct processes that are spatially separated and communicate via messages.
> The spatial separation implies that communication latency is non-negligible compared to local event execution time.
> There is no shared memory and no global clock, meaning processes must rely on message passing to coordinate and establish any notion of time.

> [!question]- Q2. What are the three types of events in a distributed system, and how do they establish the happened-before relation? (concepts: L05a-02, L05a-03)
> The three types of events are local computation, message send, and message receive.
> The happened-before relation is defined by two fundamental rules.
> First, if two events occur in the same process, the one that happens first in local sequence happened-before the other.
> Second, if one event is the sending of a message and another is the receipt of that same message, the send event happened-before the receive event.

> [!question]- Q3. If event A happened-before event B, and B happened-before C, how does transitivity apply, and when are two events considered concurrent? (concepts: L05a-04, L05a-05)
> Transitivity means that if A happened-before B, and B happened-before C, then A happened-before C.
> This allows us to establish a causal ordering across multiple processes in the system.
> Two events are considered concurrent if neither can causally affect the other.
> Specifically, A and B are concurrent if it is not true that A happened-before B, nor that B happened-before A.

## Lamport Clocks

> [!question]- Q4. Explain the conditions that a Lamport logical clock must satisfy to be consistent with the happened-before relation. (concepts: L05b-01, L05b-02, P-Time-Clocks-Ordering)
> A Lamport logical clock assigns a monotonic integer timestamp to each event.
> To be consistent with the happened-before relation, if event A happened-before event B, then the clock value of A must be strictly less than the clock value of B.
> This requires each process to increment its local clock between consecutive events.
> Additionally, upon receiving a message, a process must update its clock to be greater than both its current local time and the timestamp attached to the incoming message.

> [!question]- Q5. Why is the partial order provided by logical clocks insufficient for resource allocation, and how does Lamport use process IDs to create a total order? (concepts: L05b-03, L05b-04)
> The partial order is insufficient because concurrent events can have the same logical timestamp, leaving the system unable to decide which request should be granted the resource first.
> To resolve ties and create a total order, Lamport uses the unique process ID associated with each event.
> If two events have the same logical timestamp, the event from the process with the smaller process ID is ordered before the other.
> This guarantees that all processes observe exactly the same totally ordered sequence of events.

> [!question]- Q6. Trace the message complexity for Lamport's distributed mutual exclusion algorithm when a single node requests and releases the lock among N nodes. (concepts: L05b-05, L05b-06)
> To request the lock, a node sends a request message to all N-1 other nodes.
> Each of the N-1 nodes replies with an acknowledgment message, totaling N-1 replies.
> To release the lock, the node sends a release message to all N-1 other nodes.
> Therefore, the total message complexity for a single critical section execution is 3*(N-1) messages.

> [!question]- Q7. What are the conditions for Lamport physical clocks to avoid anomalous behavior, and how do IPC time and clock drift rate affect these conditions? (concepts: L05b-07, L05b-08)
> Anomalous behavior occurs when a causal event external to the system violates the clock ordering.
> To avoid this, physical clocks must be synchronized such that the clock drift between any two processes is less than the minimum inter-process communication (IPC) time.
> The drift rate limits how fast clocks diverge over time, requiring periodic resynchronization messages.
> The bound ensures that a message cannot travel between processes faster than their clocks drift apart.

> [!question]- Q8. How do vector clocks overcome the limitation of Lamport clocks where a smaller clock value does not imply happened-before? (concepts: L05b-09)
> With Lamport clocks, if timestamp A is less than timestamp B, we cannot determine if A happened-before B or if they are concurrent.
> Vector clocks solve this by maintaining an array of timestamps, one for each process in the system.
> A process increments its own index upon a local event and merges incoming vector clocks by taking the pairwise maximum.
> This allows the system to determine exactly whether two events are causally related or concurrent by comparing the vectors.
> Hybrid logical clocks also build on these ideas by combining physical time with logical increments for better scalability.

## Latency Limits

> [!question]- Q9. Compare and contrast latency and throughput in the context of network communication. (concepts: L05c-01)
> Latency is the time it takes for a single message to travel from the sender to the receiver.
> Throughput is the total amount of data that can be transmitted over the network in a given period of time.
> While throughput can be improved by adding parallel links or larger buffers, latency is fundamentally limited by the speed of light, protocol overhead, and processing delays.
> Distributed systems often struggle more with high latency than low throughput for small, frequent control messages.

> [!question]- Q10. What are the primary components that contribute to RPC latency, and how do marshaling and data copying impact the overall overhead? (concepts: L05c-02, L05c-03, P-Limits-Low-Latency)
> RPC latency includes network transmission time, control transfer overhead, context switches, and protocol processing.
> Marshaling transforms complex data structures into a flat byte stream, and unmarshaling reverses this process, both adding CPU overhead.
> Data copying moves the marshaled data between user space buffers, kernel network buffers, and the network interface card.
> These memory copies often dominate the total latency because memory bandwidth is limited and copying requires CPU intervention.

> [!question]- Q11. Explain how sharing descriptors between the driver and the OS can reduce data copies during an RPC call. (concepts: L05c-04)
> Traditional network stacks require copying data from user space to a kernel buffer, and then to the network card.
> To reduce this, the OS and the network interface driver can share memory descriptors that point directly to the user space buffers.
> Instead of copying the payload, the OS simply passes the descriptor to the network adapter.
> The network card then uses direct memory access (DMA) to read the data directly from user memory, eliminating an intermediate copy.

> [!question]- Q12. How does the Firefly RPC implementation optimize control transfer, context switches, and protocol processing on a reliable LAN? (concepts: L05c-05, L05c-06, L05c-07, P-Firefly-RPC)
> Firefly RPC minimizes context switches by executing the RPC protocol code within the context of the calling thread.
> It streamlines protocol processing by assuming a reliable LAN, removing complex sequence numbering and heavy error-correction logic from the fast path.
> Control transfer is optimized by using a direct upcall mechanism from the network driver to the waiting thread.
> A key lesson is that optimizing the common case of successful, single-packet RPCs drastically improves overall system performance.

## Active Networks

> [!question]- Q13. What motivated the proposal of active networks, and what are the main implementation challenges they introduce? (concepts: L05d-01, L05d-02, P-Active-Networks-ANTS)
> Active networks were proposed to allow network routers to dynamically execute custom code on passing packets, enabling rapid deployment of new protocols without upgrading router firmware globally.
> The primary implementation challenges involve safety and security, as malicious code could compromise the router.
> Performance is another major challenge, because executing arbitrary code on every packet is much slower than fast-path hardware routing.
> Finally, resource management is difficult when multiple custom protocols compete for CPU and memory on a single router.

> [!question]- Q14. Describe the ANTS toolkit, specifically how capsules differ from traditional packets and what information is included in an ANTS header. (concepts: L05d-03, L05d-04)
> The ANTS toolkit implements an active network by replacing standard packets with capsules.
> Capsules contain both a data payload and a reference to the code required to process them.
> The ANTS header includes a type identifier that specifies which code module to execute at each active node.
> It also contains resource limits, such as a time-to-live counter, to prevent capsules from consuming infinite router resources.

> [!question]- Q15. How does the ANTS API support capsule execution, and how is code caching used to improve the performance of capsule implementation? (concepts: L05d-05, L05d-06)
> The ANTS API provides primitives for capsules to manipulate their routing, access node state, and generate new capsules.
> To improve performance, routers cache the capsule code after fetching it for the first time.
> When subsequent capsules of the same type arrive, the router uses the cached code instead of downloading it again.
> This code caching significantly reduces the latency overhead introduced by the active network model.

> [!question]- Q16. What are the potential applications for active networks, and what are the primary arguments against adopting them globally? (concepts: L05d-07, L05d-08)
> Potential applications include application-specific congestion control, customized multicast routing, and transparent data compression or encryption at the edge.
> The main argument against global adoption is the severe performance penalty of software-based packet processing compared to specialized hardware switches.
> Additionally, network operators are highly risk-averse and fear the security implications of executing unverified user code on backbone routers.
> Finally, standardizing end-to-end protocols proved more practical for most applications.

> [!question]- Q17. Explain the uniform protocol architecture introduced by the x-kernel and how it compares to modern descendants like SDN or eBPF. (concepts: L05d-09, L05d-10, P-x-Kernel)
> The x-kernel provides a uniform architecture where all protocols implement a standard interface to pass messages up and down the stack.
> This modularity allows developers to easily compose custom protocol graphs without modifying the core OS kernel.
> Modern descendants like Software-Defined Networking (SDN) shift the custom logic to a centralized controller rather than embedding code in every packet.
> Technologies like eBPF and XDP allow safe injection of custom packet-processing logic into the kernel, achieving the flexibility of active networks with much higher performance.

## Systems from Components

> [!question]- Q18. What is the design cycle for building systems from components, and how does IOA fit into specifying both abstract and concrete behaviors? (concepts: L05e-01, L05e-02, L05e-03, P-Ensemble-Systems-from-Components)
> The design cycle involves specifying the system, implementing it using small composable modules, and then applying formal optimization tools to collapse the stack for performance.
> The Input/Output Automaton (IOA) formalism is used to mathematically specify the system's behavior.
> An abstract IOA specification defines the high-level properties and correctness guarantees of the system.
> Concrete IOA specifications model the actual implementation details of the individual micro-protocols and their interactions.

> [!question]- Q19. How did the researchers use OCaml for implementation and NuPrl for optimization in the Ensemble system? (concepts: L05e-04, L05e-05)
> OCaml was chosen for implementation because its strong typing and functional nature map closely to the formal IOA specifications.
> This makes it easier to verify that the micro-protocol code matches the mathematical model.
> The NuPrl theorem prover was then used to analyze the composed OCaml code and mechanically prove equivalence for optimized paths.
> NuPrl automatically applies optimizations based on these proofs, safely bypassing intermediate protocol layers in the common case.

> [!question]- Q20. Describe the process of synthesizing a TCP/IP stack from micro-protocols and explain the primary sources of optimization such as header compression and delayed processing. (concepts: L05e-06, L05e-07)
> Synthesizing a TCP/IP stack involves chaining together many tiny micro-protocols, each handling a single feature like sliding windows or checksums.
> The primary source of optimization is identifying the common path, where most packets flow without errors or retransmissions.
> On this common path, header compression reduces overhead by storing predicted header fields in the connection state.
> Delayed processing allows the system to acknowledge packets immediately and defer complex state updates until the CPU is idle.
