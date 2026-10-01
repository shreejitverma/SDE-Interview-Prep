---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: L01
tags: [cs6210, cs6210/practice]
---

# Practice L01

Original exam-style questions for [L01](../Part-1-OS-Structure-and-Virtualization/L01-Introduction-to-AOS.md).
Each question names the coverage ids it exercises; answers are folded so this page works as a self-test.

> [!question]- Q1. True or false: the OS hides the complexity of underlying hardware from applications through abstractions, which means the OS itself no longer needs to be aware of the specific hardware organization. (concepts: L01-01, L01-02)
> False.
> While the OS provides simplified abstractions like files and processes to user-level applications, the OS kernel itself must intimately understand the hardware organization to function effectively.
> It relies on specific hardware hooks, such as interrupts, memory management units, and bus architectures, to physically manage data movement and enforce security boundaries.

> [!question]- Q2. A system performs a context switch that flushes the Translation Lookaside Buffer (TLB). If the system immediately executes 5000 memory accesses with a cold TLB hit rate of 40%, and a TLB miss costs 120 cycles, what is the total penalty in cycles incurred by the context switch due to TLB misses? (concepts: L01-03, L01-05)
> The context switch causes 5000 memory accesses to experience a 60% miss rate due to the cold TLB.
> The number of missed accesses is 5000 multiplied by 0.60, which equals 3000 misses.
> Multiplying 3000 misses by the 120-cycle penalty per miss results in a total penalty of 360,000 cycles.
> This demonstrates the indirect cache and memory hierarchy costs of CPU management through context switching.

> [!question]- Q3. Consider a scenario where a monolithic kernel is being redesigned to improve fault isolation. How would moving to a microkernel architecture change the boundary between policy and mechanism, and what is the primary performance tradeoff? (concepts: L01-04, L01-06)
> In a microkernel, only minimal mechanisms like basic inter-process communication and low-level scheduling remain in the privileged protection domain.
> High-level OS functionalities and policies, such as file system management and complex scheduling algorithms, are moved into unprivileged user-space servers.
> The primary tradeoff is reduced performance due to the overhead of frequent context switches and system calls required to communicate between these isolated user-space servers.
> However, this provides much stronger fault isolation, as a crashed user-space service can be restarted without bringing down the entire kernel.

> [!question]- Q4. The operating system multiplexes execution units to manage the CPU across multiple ready threads. Why is a timer interrupt an essential hardware mechanism for this CPU management? (concepts: L01-05, L01-06)
> A timer interrupt guarantees that the CPU control is periodically returned from a user-level application back to the operating system kernel.
> Without this mechanism, a long-running or malicious process operating in its own protection domain could enter an infinite loop and permanently monopolize the CPU.
> The timer interrupt enables preemptive scheduling policies, allowing the OS to forcibly pause the current process, save its state, and schedule a different thread to ensure system responsiveness and fairness.

> [!question]- Q5. Outline the sequence of hardware and OS events that occur when a user application needs to read a file, transitioning across protection domains. (concepts: L01-01, L01-04, L01-06)
> The application invokes the file read abstraction by making a system call, which executes a hardware trap instruction.
> The hardware switches from unprivileged user mode to the privileged kernel protection domain and saves the user registers.
> The OS kernel validates the request arguments to ensure the application has the necessary permissions.
> The OS then performs the requested file system management service by interacting with the storage device.
> Finally, the kernel executes a return-from-trap instruction, which restores the user registers and switches the hardware back to user mode to resume application execution.
