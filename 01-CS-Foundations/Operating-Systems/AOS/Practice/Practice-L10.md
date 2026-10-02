---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: L10
tags: [cs6210, cs6210/practice]
---

# Practice L10

Original exam-style questions for [L10a](../Part-5-Internet-Scale-Real-Time-and-Security/L10a-TS-Linux.md), [L10b](../Part-5-Internet-Scale-Real-Time-and-Security/L10b-Persistent-Temporal-Streams.md).
Each question names the coverage ids it exercises; answers are folded so this page works as a self-test.

> [!question]- Q1. How do firm timers differ from both pure soft timers and traditional high-resolution hardware timers? (concepts: L10a-03, L10a-04)
> Firm timers combine the benefits of soft timers and high-resolution hardware timers while mitigating their individual drawbacks.
> Traditional high-resolution hardware timers interrupt the CPU to enforce deadlines, causing cache thrashing and harming throughput.
> Pure soft timers avoid hardware interrupts by checking deadlines opportunistically during transitions like system calls.
> They fail if the system is completely idle because no transitions occur.
> Firm timers use opportunistic soft polling during normal operation to dispatch deadlines efficiently.
> They also arm a one-shot APIC hardware timer with an overshoot value.
> If the system goes idle and the soft timer check is missed, the hardware interrupt fires as a fallback to guarantee the deadline is met within the specified overshoot tolerance.

> [!question]- Q2. You install a high-resolution timer in a standard monolithic Linux kernel to improve responsiveness for a time-sensitive audio application, but latency bounds are still frequently missed. What are the primary sources of latency, and how does Time-Sensitive Linux address this issue? (concepts: L10a-01, L10a-02, L10a-05, P-Time-Sensitive-Commodity-OS)
> Time-sensitive applications require bounded latency, whereas standard commodity operating systems are throughput-oriented.
> The primary sources of latency are the timer resolution, the scheduler itself, and the non-preemptible nature of the kernel.
> Installing a high-resolution timer only addresses the timer resolution.
> If a high-priority multimedia task becomes runnable while a background task is executing a long system call in the kernel, the background task cannot be preempted until it returns to user space.
> Time-Sensitive Linux addresses this by introducing fine-grained preemptibility.
> It converts standard spinlocks into mutexes or inserts explicit preemption points.
> This allows the kernel to switch context and immediately schedule the time-sensitive task even if it was previously executing kernel code.

> [!question]- Q3. A soft modem task requires 4 milliseconds of computation every 16 milliseconds to function correctly. A background video encoding task occasionally holds a shared resource that blocks the soft modem for 2 milliseconds. How does a proportion-period scheduler ensure the soft modem meets its deadlines, and how is the priority inversion handled? (concepts: L10a-06, L10a-07, L10a-08)
> The soft modem is a time-sensitive task that requires a steady proportion of CPU time alongside throughput tasks.
> A proportion-period scheduler guarantees the soft modem an allocation of 25 percent (4/16) of the CPU within its 16 millisecond period.
> When the background task holds a lock needed by the soft modem, a priority inversion occurs.
> The scheduler handles this by applying priority inheritance or calculating a worst-case blocking factor.
> The admission control test evaluates if the sum of all proportion requirements plus the blocking factor is less than a safe utilization threshold.
> Because the remaining unreserved CPU time is sufficient to absorb the 2 millisecond blocking delay, the scheduler still guarantees the soft modem will meet its deadline.

> [!question]- Q4. Why do traditional stateful clock synchronization protocols like NTP fail catastrophically during live virtual machine migration, and what approach is recommended instead? (concepts: L10a-09, P-Virtualize-Everything-but-Time)
> Virtualizing the passage of time is challenging because hardware oscillators change when a virtual machine moves to a new physical host.
> Live migration pauses the virtual machine during the final memory copy phase, creating a sudden gap in the guest's perception of time.
> NTP uses a stateful feedback loop that expects a stable, continuous hardware clock.
> When it encounters a sudden time jump and a changed oscillator frequency after migration, NTP miscalculates violently, causing massive multi-second timing shocks.
> The paper argues for exposing true physical time to guest operating systems using a feed-forward synchronization architecture.
> A privileged domain calculates clock parameters on stable hardware, and guest domains use a stateless read function to compute time.
> This dependent clock paradigm avoids stateful feedback loops and eliminates the massive timing shocks associated with live migration.

> [!question]- Q5. Why do traditional socket streams fail to adequately support situation awareness applications, and how does the PTS programming model address these challenges? (concepts: L10b-01, L10b-02, P-Persistent-Temporal-Streams)
> Situation awareness applications involve aggregating and analyzing continuous data streams from multiple sensors.
> Traditional socket streams provide sequential, unindexed data delivery without built-in persistence or time awareness.
> This forces application developers to implement complex buffering and alignment logic to correlate events across different sensors.
> Persistent Temporal Streams shifts this burden to the middleware.
> The PTS programming model provides time-indexed channels, allowing producers to tag items with explicit timestamps.
> It introduces built-in windowed persistence and garbage collection, enabling consumers to request data by time index rather than sequential order.

> [!question]- Q6. A sensor system uses PTS to capture video and audio. The channels have a garbage collection window of 60 seconds. At absolute time T=200, a consumer requests a bundled stream of video and audio at time index 130. Describe the execution of the time-based get operation, considering persistence and temporal correlation. (concepts: L10b-03, L10b-04, L10b-05, L10b-06)
> In the PTS model, producers use the time-based put operation to insert time-indexed items into channels.
> Since the garbage collection window is 60 seconds and the current time is 200, any data with a timestamp older than 140 is eligible for garbage collection.
> The consumer issues a time-based get operation for index 130.
> Because 130 is outside the persistence window, the middleware will return an error or a null result, as the data has already been purged.
> If the request had been for index 150, the bundling feature would automatically correlate the video and audio channels.
> The middleware handles the buffering and alignment internally, ensuring that the retrieved items from both streams are perfectly synchronized at the requested time index before returning them to the consumer.

> [!question]- Q7. How does Yima achieve high scalability and fault tolerance for continuous media delivery, and why is its pseudorandom data placement algorithm superior to round-robin striping when adding nodes? (concepts: L10b-07, P-Yima)
> Yima is a continuous media server designed for the delivery of isochronous streams using a bipartite architecture that separates client interactions from data storage nodes.
> It achieves scalability by eliminating the master node bottleneck.
> Clients connect to any server node, which acts as a controller, and all nodes stream data directly to the client.
> Yima uses the SCADDAR pseudorandom block placement algorithm to distribute data across multiple independent disks.
> This ensures probabilistically even load distribution without requiring a central metadata directory.
> When a new disk is added, round-robin striping forces nearly all blocks to change their absolute index locations, resulting in massive data movement.
> SCADDAR uses a predictable seed to compute block locations mathematically.
> This approach ensures that only the mathematically necessary fraction of blocks move to the new disk to maintain perfect load balancing, drastically minimizing the I/O cost of online scaling.
