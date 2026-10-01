---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/151244.151247"]
course: cs6210
lesson: L05
reading: required
venue: "TOCS 1993"
authors: ["Chandramohan A. Thekkath", "Henry M. Levy"]
tags: [cs6210, cs6210/paper]
aliases: ["Limits to Low-Latency Communication on High-Speed Networks"]
---

# Limits to Low-Latency Communication on High-Speed Networks

TOCS 1993. Reading status: required. [Link](https://doi.org/10.1145/151244.151247).

> [!abstract] One-line summary
> Evaluates the system-level effects of high-speed networks, showing that software and controller design, not just network bandwidth, dictate remote communication latency.

## Problem

While local area network throughput is increasing dramatically (e.g., moving from Ethernet to ATM and FDDI), the latency of cross-machine communication is not improving at the same rate.
Distributed systems rely heavily on low-latency communication for operations like remote procedure calls (RPC), but existing network controllers and software architectures fail to translate high bandwidth into correspondingly low latency.

## Key idea

The authors design and implement a new, highly optimized RPC system to isolate and measure the fundamental costs of small-packet communication on various network technologies (Ethernet, FDDI, ATM).
They demonstrate that as processor speeds and network bandwidths increase, the bottleneck for low-latency communication shifts to the network controller's design and its interface with the host architecture (e.g., DMA vs. PIO, interrupt handling).
They found that simple FIFO-based controllers can outperform complex DMA controllers for small messages by minimizing setup overhead.

## Design

- Minimalist, low-latency RPC system designed to bypass traditional OS overheads.
- Tested across different platforms (DECstation, SPARCstation) and networks (10 Mbps Ethernet, 100 Mbps FDDI, 140 Mbps ATM).
- Isolated component costs: time on the wire, controller latency, data transfer across host bus, interrupt vectoring, and interrupt service.
- The RPC software eliminates context switches where possible and optimizes stubs and marshaling for small packets.

## Evaluation

- Achieved a round-trip RPC time of 170 microseconds on an ATM network using DECstation 5000/200 hosts.
- Hardware-level packet exchange for small packets (60 bytes): Ethernet took 253 microseconds, FDDI took 263 microseconds, ATM took 73 microseconds.
- Controller latency and data transfer overheads dominate: FDDI latency (97 us) was worse than Ethernet (51 us) despite higher bandwidth, largely due to complex controller interactions.

## Limitations and critiques

- The experimental testbed eliminates typical operating system overheads (like multi-threading and protection), representing a best-case lower bound rather than realistic system performance.
- The ATM measurements bypassed segmentation and reassembly (SAR) costs for multi-cell packets, presenting an optimistic view of ATM's practical latency.
- Focused primarily on small packets, which skews the preference towards Programmed I/O (PIO) over DMA.

## What it led to

- Motivated user-level networking architectures (like U-Net, VIA, and eventually RDMA) that bypass the OS kernel to reduce latency.
- Highlighted the trade-offs between Programmed I/O (better for latency/small packets) and DMA (better for throughput/large packets).
- Influenced the design of network interface cards (NICs) to simplify host-controller interactions and minimize latency.

## Exam angles

<details>
<summary>Why did the 100 Mbps FDDI network perform slightly worse than the 10 Mbps Ethernet for small packet round-trips?</summary>
The FDDI controller had significantly higher controller latency and interrupt service overhead compared to the Ethernet controller.
For small packets, the time on the wire is extremely short, making the host-controller interaction overhead the dominant factor in overall latency.
</details>

<details>
<summary>What are the relative advantages of Programmed I/O (PIO) versus DMA for network data transfer, according to the paper?</summary>
PIO is advantageous for small packets because it avoids the high setup costs and cache invalidation overheads associated with DMA, resulting in lower latency.
DMA is better for large packets and high throughput because it offloads the data transfer from the CPU, freeing the CPU for other tasks while the transfer occurs.
</details>

<details>
<summary>How does processor architecture impact the overhead of network communication in this study?</summary>
Processor architecture affects the cost of vectoring interrupts, servicing interrupts, and moving data across the host bus.
The study shows that differences in how architectures handle cache flushes during DMA or how efficiently they read/write to controller memory significantly influence the lower-bound latency of network operations.
</details>

## Related

- Lessons: [L05a](../Part-3-Distributed-Systems/L05a-Distributed-Systems-Definitions.md), [L05b](../Part-3-Distributed-Systems/L05b-Lamport-Clocks.md), [L05c](../Part-3-Distributed-Systems/L05c-Latency-Limits.md), [L05d](../Part-3-Distributed-Systems/L05d-Active-Networks.md), [L05e](../Part-3-Distributed-Systems/L05e-Systems-from-Components.md)

