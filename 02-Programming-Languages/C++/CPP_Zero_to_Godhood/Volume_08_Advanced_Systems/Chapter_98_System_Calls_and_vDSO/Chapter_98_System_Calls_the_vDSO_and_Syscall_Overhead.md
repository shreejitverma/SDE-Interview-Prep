# Chapter 98: System Calls, the vDSO, and Syscall Overhead

A system call is the only way a program can do anything the hardware protects — read a file, send a packet, get the time, allocate a page — and it is one of the most expensive routine operations in a program's life, because it is a controlled transition into the kernel with a full privilege-level change. This chapter quantifies that cost, explains the mechanism and why modern security mitigations made it worse, introduces the **vDSO** that eliminates it for a few hot calls, and establishes the discipline of *minimising kernel crossings* that underlies the high-performance I/O and clock chapters that follow.

## Chapter Roadmap

- 98.1 What a System Call Actually Costs
- 98.2 The Mechanism: Crossing the Privilege Boundary
- 98.3 Why Mitigations Made Syscalls Slower
- 98.4 The vDSO: Syscalls Without the Crossing
- 98.5 Reducing Syscall Overhead
- 98.6 The Discipline

---

## 98.1 What a System Call Actually Costs

A **system call (syscall)** is a request from a user-space program for the kernel to perform a privileged operation. Unlike a function call (a few cycles), a syscall costs **hundreds to thousands of cycles** even for trivial work, because the CPU must switch privilege level, save and restore state, and — post-2018 — flush microarchitectural buffers for security.

| Operation | Approx cost |
|---|---|
| Plain function call | ~1–5 cycles |
| `getpid()` (minimal syscall, no vDSO) | ~100–300 ns (hundreds of cycles) |
| `read()`/`write()` (small) | ~0.5–2 μs including kernel work |
| Syscall with mitigations enabled | +hundreds of cycles over the above |
| `clock_gettime()` via vDSO | ~15–30 ns (no kernel crossing) |

> **Why this matters.** The syscall is the boundary between the two halves of the cost model (Chapter 85): everything below it is the OS, and crossing it is expensive enough that *how often you cross* often dominates I/O-bound performance more than the work on either side. A loop that makes one syscall per item (one `write` per log line, one `recv` per packet) is frequently bottlenecked on the crossings, not the data. This is why the entire high-performance I/O story (Chapter 99) is about *amortising* or *eliminating* syscalls — batching many operations per crossing (`io_uring`, `writev`) or avoiding the kernel entirely (kernel bypass, Chapter 100).

---

## 98.2 The Mechanism: Crossing the Privilege Boundary

User code runs in an unprivileged CPU mode (ring 3 on x86); the kernel runs privileged (ring 0). A syscall transitions between them via a dedicated instruction (`syscall` on x86-64, `svc` on ARM) that:

1. Switches to kernel privilege and the kernel stack.
2. Saves the user register state.
3. Dispatches to the kernel handler via the syscall number (in a register), with arguments in registers.
4. Executes the kernel code (which may itself sleep, schedule, or do I/O).
5. Restores user state and returns to ring 3.

> **Why this matters / cost model.** Even with the fast `syscall`/`sysret` instructions (which avoid the old slow interrupt-based `int 0x80` path), the *fixed* cost of saving/restoring state and switching stacks and privilege is unavoidable — it is the price of the protection boundary that keeps user code from corrupting the kernel. On top of that fixed entry/exit cost sits the *variable* cost of the actual kernel work, which for a blocking call (`read` from a socket with no data) can include descheduling the thread entirely (a context switch, Chapter 96). The fixed cost is why even a "do-nothing" syscall is hundreds of cycles, and why reducing the *count* of syscalls is a first-order optimization independent of what each does.

---

## 98.3 Why Mitigations Made Syscalls Slower

The 2018 **Meltdown** and **Spectre** speculative-execution vulnerabilities forced kernels to add mitigations at the user/kernel boundary: **KPTI** (kernel page-table isolation) unmaps kernel memory from the user page tables, so every syscall now also swaps page tables and flushes TLB entries; **retpolines** and buffer-clearing instructions (`IBRS`, `MDS` clears) run on kernel entry/exit.

> **Why this matters / cost model.** These mitigations *multiplied* syscall cost — a bare syscall that was ~100 ns pre-2018 can be 2–3× that with full mitigations, because the TLB flush from page-table switching turns subsequent user accesses into page walks (Chapter 88) too. This made syscall-heavy code measurably slower across an entire industry overnight and sharpened the incentive to avoid the kernel. It is also why benchmarks must specify their mitigation status, and why latency-critical deployments sometimes disable specific mitigations on dedicated, trusted hardware (a security trade-off, not a casual one). The lesson: the syscall boundary is not just expensive but *got more expensive*, and the architectural response — batch and bypass — became more valuable.

---

## 98.4 The vDSO: Syscalls Without the Crossing

The **vDSO** (virtual dynamic shared object) is a small shared library the kernel maps into every process's address space, containing user-space implementations of a few read-only "syscalls" whose answer the kernel can safely expose without a privilege transition. The prime examples are `clock_gettime`, `gettimeofday`, and `getcpu`.

```cpp
// Min standard: C++11 + POSIX. The vDSO makes this NOT a real syscall on Linux.
#include <time.h>
struct timespec ts;
clock_gettime(CLOCK_MONOTONIC, &ts);   // resolved in user space via the vDSO: ~15-30 ns, no kernel entry
```
*Listing 98.1 — `clock_gettime` is serviced by the vDSO with no privilege transition. Linux-specific mechanism.*

> **Why this matters / cost model.** Time queries are extraordinarily frequent in latency-measuring and timestamping code (every event in a trading system is timestamped), and if each one were a real syscall at hundreds of nanoseconds, timestamping would dominate. The vDSO solves this by having the kernel publish the timekeeping data (a counter and scaling factors) into a page the user code reads directly, computing the time *in user space* — turning a ~hundreds-of-ns syscall into a ~tens-of-ns function call with no kernel crossing (Chapter 101 builds on this). The vDSO only works for operations whose data the kernel can safely expose read-only; it is not a general syscall-elimination mechanism, but for the handful of hot read-only calls it covers, it removes the boundary cost entirely. You get it for free — `clock_gettime` already uses it — but knowing it exists explains why time queries are cheap and other syscalls are not.

---

## 98.5 Reducing Syscall Overhead

The techniques to minimise the boundary cost:

- **Batch.** Combine many operations into one syscall: `writev`/`readv` (scatter-gather one syscall for multiple buffers), `sendmmsg`/`recvmmsg` (multiple datagrams per call), and `io_uring` (Chapter 99) which submits *and completes* many I/Os per crossing.
- **Buffer.** User-space buffering (`std::ofstream`, a ring buffer) coalesces many small writes into few large syscalls — the reason unbuffered per-character I/O is catastrophically slow.
- **Avoid blocking syscalls on the hot path.** A blocking `read` may context-switch; use readiness (`epoll`) or completion (`io_uring`) models so the hot thread never blocks in a syscall.
- **Use the vDSO** for time/CPU queries (automatic).
- **Bypass the kernel entirely** for the highest-rate I/O (DPDK/RDMA, Chapter 100).

> **Why this matters.** The unifying principle is **amortise the fixed crossing cost over more work per crossing**. One `writev` of 100 buffers pays the syscall cost once instead of 100 times; one `io_uring` submission can carry dozens of reads. Buffering is the everyday form of this — the standard library buffers `iostream`/`stdio` precisely because per-call syscalls would dominate. For the hot path the principle escalates to readiness/completion I/O models (so you never block) and, at the extreme, to kernel bypass (so you never cross at all).

---

## 98.6 The Discipline

| Pattern | Crossings | When |
|---|---|---|
| One syscall per item | Worst — N crossings | Avoid on hot paths |
| User-space buffering | Few large crossings | Default for I/O |
| Scatter-gather (`writev`, `sendmmsg`) | One per batch | Known multi-buffer I/O |
| `io_uring` | Amortised, async | High-rate I/O (Chapter 99) |
| vDSO | Zero (user space) | Time/CPU queries (automatic) |
| Kernel bypass | Zero | Extreme packet rates (Chapter 100) |

> **The discipline.** Treat every syscall as a hundreds-of-cycles event that may also block and context-switch, and design the hot path to cross the boundary as rarely as possible: buffer small operations into large ones, batch with scatter-gather and `io_uring`, lean on the vDSO for the time queries you make constantly, and bypass the kernel for the highest packet rates. Counting syscalls (`strace -c` reports the count and time per syscall) is the diagnostic — an I/O-bound program with a surprising syscall count usually has its answer there. The next two chapters are the concrete realisation of this discipline: high-performance I/O models that amortise the crossing, and kernel bypass that removes it.
