---
tags: [operating-systems, kernel/internals, unix, linux, windows-nt, xv6, type/moc]
aliases: [Operating Systems and Kernels MOC, Kernel Architecture Canon]
status: evergreen
created: 2026-09-17
---

# Operating Systems & Kernel Internals

> [!summary]
> This domain explores the architectural design, abstraction boundaries, and scheduling mechanics of modern operating systems. Spanning historic treatises from Unix pioneers Dennis Ritchie and Ken Thompson to MIT's xv6 teaching kernel, Linux scheduler failure modes, and the internal architecture of the Windows NT Executive, this collection provides core systems literacy for principal engineers.

---

## Pillar Guides

1. **[[Unix-and-Linux-Kernel-Foundations|UNIX & Linux Kernel Foundations]]**
   - *The UNIX Time-Sharing System (Dennis M. Ritchie & Ken Thompson, 1974)*: The seminal paper establishing the hierarchical file system, files-as-byte-streams, and the process model.
   - *The Linux Kernel Hackers' Guide (Michael K. Johnson, 1995)*: Early architectural blueprint of the monolithic Linux kernel.
   - *Unreliable Guide To Hacking The Linux Kernel (Paul Rusty Russell, 2000)*: Concurrency rules, spinlocks, interrupt contexts, and usercopy mechanics.
   - *The Linux Scheduler: a Decade of Wasted Cores (Lozi et al., 2016)*: Seminal EuroSys paper documenting four major bugs in the Linux CFS multi-core scheduler causing up to $138\times$ slowdowns.
2. **[[OS-From-Scratch-and-Teaching-Kernels|Teaching Operating Systems & OS From Scratch]]**
   - *xv6: a simple, Unix-like teaching operating system (Russ Cox, Frans Kaashoek, Robert Morris, MIT, 2012)*: Re-implementation of Dennis Ritchie's Version 6 Unix in ANSI C for multi-core x86.
   - *Writing a Simple Operating System — from Scratch (Nick Blundell, 2010)*: Bootloaders, 16-bit real mode, switching to 32-bit protected mode, GDT setup, and basic paging.
3. **[[Windows-NT-Internals-Architecture|Windows NT Internals & Architecture]]**
   - *Windows NT Alerts Design Note (David N. Cutler, 1989)*: Asynchronous Procedure Calls (APCs) and interrupt mechanisms by NT's chief architect.
   - *Windows: A Software Engineering Odyssey (Mark Lucovsky)*: Architectural retrospective on building NT.
   - *Windows Kernel Internals 10-Part Curriculum (David B. Probert, Ph.D.)*: In-depth analysis of Traps/Interrupts, Virtual Memory, Cache Manager, I/O Architecture, LPC, NTFS, Registry, Object Manager, and Synchronization.
   - *WinDbg: From A to Z! (Robert Kuster, 2007)* & *Kernel Debugging with WinDbg (2005)*: The definitive guide to Windows kernel crash dump and live debugging.
   - *The Linux Kernel Hidden Inside Windows 10 (Alex Ionescu, 2016)*: The reverse-engineering breakdown of Windows Subsystem for Linux (WSL1) and Pico processes.

---

## Operating System Architectures Compared

| Architectural Dimension | UNIX / Linux | Windows NT | MIT xv6 |
| :--- | :--- | :--- | :--- |
| **Kernel Model** | Monolithic (Loadable Kernel Modules) | Hybrid / Modified Microkernel | Minimal Monolithic |
| **Hardware Abstraction** | Architecture directory (`arch/x86`) | Hardware Abstraction Layer (`HAL.dll`)| Direct hardware drivers |
| **Object Model** | Everything is a file descriptor (`vfs`) | Everything is an Executive Object (`ObCreateObject`)| File descriptors & Inodes |
| **IPC Mechanism** | Pipes, UNIX Sockets, Shared Memory | Lightweight / Advanced LPC (ALPC) | Pipes |
| **Scheduler** | Completely Fair Scheduler (CFS) | Priority-driven Preemptive (32 Priority Levels) | Round-Robin across processes |

---

## Related Notes
- [[../README|Technical Whitepapers Master MOC]]
- [[../01-Systems-Performance-and-Tracing/README|Systems Performance and Tracing]]
- [[../03-Memory-Architecture-and-Concurrency/README|Memory Architecture and Concurrency]]
- [[../../01-CS-Foundations/Operating-Systems/README|CS Foundations: Operating Systems]]
