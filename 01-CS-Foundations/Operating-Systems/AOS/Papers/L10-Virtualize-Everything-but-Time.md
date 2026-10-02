---
type: paper
track: [sde, distinguished]
level:
status: seed
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
> We propose a new timekeeping architecture for virtualized systems, in the context of Xen.
Built upon a feed-forward based RADclock synchronization algorithm, it ensures that the clocks in each OS sharing the hardware derive from a single central clock in a resource effective way, and that this clock is both accurate and robust.
A key advantage is simple, seamless VM migration with consistent time.
In contrast, the current Xen approach for timekeeping behaves very poorly under live migration, posing a major problem for applications such as financial transactions, gaming, and network measurement, which are critically dependent on reliable timekeeping.
We also provide a detailed examination of the HPET and Xen Clocksource counters.
Results are validated using a hardware-supported testbed.

## Problem

Timekeeping poses particular problems for virtualization because a tight connection to absolute time must be maintained across the operating systems sharing the hardware.
Virtualization adds an extra layer that creates additional resource contention and increased latencies.
The default Xen approach uses the Network Time Protocol (NTP) where each guest runs its own independent and stateful ntpd daemon.
This approach behaves poorly under live migration because ntpd cannot account for the time during which a virtual machine has been halted.
Upon resuming, the sudden change in status renders the state information of ntpd invalid, resulting in extreme oscillator rate estimates and massive clock errors.

## Key idea

The paper proposes a new dependent clock timekeeping architecture for para-virtualized systems using a feed-forward synchronization algorithm called RADclock.
Instead of each guest running its own stateful ntpd process, a single central clock in the host domain (Dom0) runs the synchronization algorithm and maintains the clock parameters.
Guest operating systems simply read raw hardware timestamps and convert them to absolute time using these centrally maintained, stateless parameters provided via the XenStore.
This architecture ensures consistent time across all guests and enables seamless live virtual machine migration since the timekeeping state never actually migrates.

## Design

The architecture relies on the RADclock algorithm, which strictly separates timestamping from synchronization.
It uses a dependent clock paradigm where Dom0 runs the full synchronization process and writes updated clock parameters to the XenStore.
Guest operating systems do not run any synchronization daemon.
Instead, they utilize a stateless clock-reading function that reads raw timestamps from a local hardware counter, such as the High Precision Event Timer (HPET) or Xen Clocksource.
The guest then fetches the current clock parameters from the XenStore to compute the absolute time from the raw timestamp.

## Evaluation

The system was evaluated using a hardware-supported testbed involving a Stratum-1 NTP server on a local area network and a DAG card for independent external timestamping to eliminate host noise.
They showed that the RADclock dependent architecture maintains sub-10 microsecond precision under normal conditions.
The proposed architecture seamlessly handles virtual machine migration with effectively zero convergence time.
In contrast, independent ntpd clocks suffer errors in the multiple millisecond range under load.
Furthermore, independent ntpd clocks experience errors on the order of seconds following live migration.

## Limitations and critiques

The read and write operation to the XenStore is not instantaneous, causing a measured delay of roughly 1.2 to 1.4 milliseconds.
This creates a small window where a guest might use slightly outdated clock parameters immediately following an update.
While the impact is minimal since parameters change slowly, it represents a theoretical window for time inconsistencies across virtual machines.
Additionally, the solution requires hypervisor modifications and guest para-virtualization to expose hardware counters and the shared parameters.

## What it led to

This approach provides a robust and scalable method for precise timekeeping in virtualized cloud computing environments.
Reliable timekeeping is essential for distributed databases, high-frequency trading, and online gaming servers.
The work highlights the inadequacy of feedback-based clocks like NTP in virtualized settings and advocates for feed-forward architectures.
This led to the integration of RADclock into Linux and FreeBSD virtualized kernels for better clock management.

## Exam angles

<details>
<summary>Why does the standard NTP approach fail during live virtual machine migration in Xen?</summary>
NTP is a feedback-based, stateful protocol that maintains a time-varying estimate of the hardware counter's drift. When a VM is halted for migration, the counter stops or its state changes, rendering NTP's state information invalid upon resume and causing extreme clock errors or instability.
</details>
<details>
<summary>How does the feed-forward nature of RADclock solve the dependent clock problem for virtual machines?</summary>
A feed-forward clock separates the raw hardware timestamping from the synchronization algorithm. This allows the guest VM to be completely stateless regarding timekeeping, merely reading raw counter values and applying a linear transformation using parameters safely managed and updated by the host machine.
</details>

## Related

- Lessons: [L10a](../Part-5-Internet-Scale-Real-Time-and-Security/L10a-TS-Linux.md), [L10b](../Part-5-Internet-Scale-Real-Time-and-Security/L10b-Persistent-Temporal-Streams.md)
