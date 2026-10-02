---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://www.usenix.org/conference/osdi10/virtualize-everything-time"]
course: cs6210
lesson: L10
reading: required
venue: "OSDI 2010"
authors: ["Timothy Broomhead", "Laurence Cremean", "Julien Ridoux", "Darryl Veitch"]
tags: [cs6210, cs6210/paper]
aliases: ["Virtualize Everything but Time"]
---

# Virtualize Everything but Time

OSDI 2010. Reading status: required. [Link](https://www.usenix.org/conference/osdi10/virtualize-everything-time).

> [!abstract] One-line summary
> A new timekeeping architecture for virtualized systems uses a feed-forward synchronization algorithm to ensure that guest operating systems derive accurate and robust time from a single central clock, which naturally supports seamless virtual machine migration.

## Problem

Timekeeping poses particular problems for virtualization because a tight connection to absolute time must be maintained across the operating systems sharing the hardware.
Virtualization adds an extra layer that creates additional resource contention and increased latencies.
The default approach uses a standard network time protocol where each guest runs its own independent and stateful synchronization daemon.
This approach behaves poorly under live migration because the synchronization daemon cannot account for the time during which a virtual machine has been halted.
Upon resuming, the sudden change in status renders the state information invalid, resulting in extreme oscillator rate estimates and massive clock errors.

## Key idea

The core idea is to introduce a dependent clock timekeeping architecture for para-virtualized systems using a feed-forward synchronization algorithm where a single central clock in the host domain maintains stateless clock parameters in a shared store, thereby allowing guest operating systems to simply read raw hardware timestamps and convert them to absolute time without running their own synchronization daemons, which ensures consistent time across all guests and enables seamless live virtual machine migration.

## Design

The architecture relies on an algorithm that strictly separates timestamping from synchronization.
It uses a dependent clock paradigm where the host domain runs the full synchronization process and writes updated clock parameters to a shared storage area.
Guest operating systems do not run any synchronization daemon.
Instead, they utilize a stateless clock-reading function that reads raw timestamps from a local hardware counter.
The guest then fetches the current clock parameters from the shared store to compute the absolute time from the raw timestamp.

## Evaluation

The system was evaluated using a hardware-supported testbed involving a dedicated time server on a local area network and a specialized capture card for independent external timestamping to eliminate host noise.
The dependent architecture maintains precision under ten microseconds during normal conditions.
The proposed architecture seamlessly handles virtual machine migration with effectively zero convergence time.
In contrast, independent clocks suffer errors in the multiple millisecond range under load and experience errors on the order of seconds following live migration.

## Limitations and critiques

The read and write operation to the shared store is not instantaneous, causing a measured delay of roughly one millisecond.
This creates a small window where a guest might use slightly outdated clock parameters immediately following an update.
While the impact is minimal since parameters change slowly, it represents a theoretical window for time inconsistencies across virtual machines.
Additionally, the solution requires hypervisor modifications and guest para-virtualization to expose hardware counters and the shared parameters.

## What it led to

This approach provides a robust and scalable method for precise timekeeping in virtualized cloud computing environments.
Reliable timekeeping is essential for distributed databases, high-frequency trading, and online gaming servers.
The work highlights the inadequacy of feedback-based clocks in virtualized settings and advocates for feed-forward architectures.
This led to the integration of feed-forward clock synchronization into mainstream virtualized kernels for better clock management.

## Exam angles

<details>
<summary>Why does the standard feedback-based approach fail during live virtual machine migration?</summary>
A feedback-based protocol maintains a time-varying estimate of the hardware counter's drift, so when a virtual machine is halted for migration, the counter stops or its state changes, which renders the state information invalid upon resume and causes extreme clock errors.
</details>

<details>
<summary>How does the feed-forward nature of the system solve the dependent clock problem for virtual machines?</summary>
A feed-forward clock separates the raw hardware timestamping from the synchronization algorithm, which allows the guest to be completely stateless regarding timekeeping by merely reading raw counter values and applying a linear transformation using parameters safely managed by the host machine.
</details>

<details>
<summary>What hardware limitations affect the accuracy of the guest operating system's clock reading?</summary>
The guest operating system is limited by the latency of reading the shared parameter store and the latency of reading the underlying physical hardware counters.
</details>

## Related

- Lessons: [L10a](../Part-5-Internet-Scale-Real-Time-and-Security/L10a-TS-Linux.md), [L10b](../Part-5-Internet-Scale-Real-Time-and-Security/L10b-Persistent-Temporal-Streams.md)
