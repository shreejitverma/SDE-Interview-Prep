---
tags: [trading/low-latency-cpp, type/moc]
aliases: [Low-Latency C++ MOC, Lock-Free C++ MOC]
status: seed
module: 08
created: 2026-08-22
---

# MOC — 08 Low-Latency Programming (C++/Rust)

Hardware-aligned systems programming: memory models, lock-free data structures, allocation-free paradigms, and compiler optimization.

---

## Core Concepts
- [[Notes/C++ Memory Model and Memory Orders]] — `memory_order_relaxed`, `acquire`, `release`, `acq_rel`, `seq_cst`; hardware barriers on x86 vs ARM.
- [[Notes/Lock-Free SPSC Ring Buffer Design]] — Single-Producer Single-Consumer cache-aligned circular buffer with zero atomic contention.
- [[Notes/Lock-Free MPMC Queue Mechanics]] — Multi-Producer Multi-Consumer topologies, CAS loops, ABA problem, hazard pointers vs epoch reclamation.
- [[Notes/Allocation-Free Steady State Patterns]] — Arena allocators, object pools, intrusive containers, eliminating `malloc`/`new` on hot paths.
- [[Notes/Cache-Conscious Data Layout]] — Structure of Arrays (SoA) vs Array of Structures (AoS), cache line packing, field ordering.
- [[Notes/Branchless Programming Idioms]] — Bit manipulation hacks, arithmetic multiplexing, eliminating data-dependent branches.
- [[Notes/Static vs Virtual Dispatch in Hot Paths]] — CRTP (Curiously Recurring Template Pattern), `std::variant` + `std::visit`, concept-based polymorphism.
- [[Notes/Compiler Optimizations and Code Placement]] — Profile-Guided Optimization (PGO), Link-Time Optimization (LTO), hot/cold code splitting (`[[likely]]`, `[[unlikely]]`).

## Labs & Implementations
- [[Labs/Lab - 08 Ultra-Low Latency SPSC Ring Buffer]] — Implement and benchmark a cache-aligned, wait-free SPSC queue sustaining >50M msgs/sec with <15ns latency.

## Drills & War Stories
- [[Drills/Drill - 08 Lock-Free Concurrency and Memory Ordering]] — Debug concurrency bugs and relax unnecessary `seq_cst` atomics down to `acquire`/`release`.
- [[Notes/War Story - The False Sharing Meltdown in Order Routing]] — Two threads updating adjacent atomic sequence numbers destroying throughput by 95%.

## Canonical Sources
- [[Sources/CppCon 2017 - When a Microsecond is an Eternity by Carl Cook]] — Canonical industry talk on low-latency C++ techniques.
- [[Sources/C++ Concurrency in Action by Anthony Williams]] — Reference text on the C++ memory model and lock-free implementations.
