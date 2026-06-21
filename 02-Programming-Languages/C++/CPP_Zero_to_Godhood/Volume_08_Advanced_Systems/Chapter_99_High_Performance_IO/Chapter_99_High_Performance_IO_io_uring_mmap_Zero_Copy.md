# Chapter 99: High-Performance I/O — io_uring, mmap, and Zero-Copy

I/O performance is dominated by two costs that have nothing to do with the device: the **syscall crossings** to initiate each operation (Chapter 98) and the **memory copies** between kernel and user buffers. This chapter develops the I/O models that attack both — the evolution from blocking to readiness (`epoll`) to completion (`io_uring`), memory-mapped I/O, and the zero-copy techniques (`sendfile`, `splice`, `MSG_ZEROCOPY`) that move data without copying it through user space. The throughline is the same as the previous chapter: amortise the crossings, and eliminate the copies.

## Chapter Roadmap

- 99.1 The Two Costs of I/O
- 99.2 Blocking, Non-Blocking, and Readiness (`epoll`)
- 99.3 The Completion Model: `io_uring`
- 99.4 Memory-Mapped I/O
- 99.5 Zero-Copy Techniques
- 99.6 Choosing an I/O Model

---

## 99.1 The Two Costs of I/O

Reading a megabyte from a socket involves far more than the network: the kernel copies the data from its socket buffer into your user buffer (a **copy**), and you made a syscall to ask for it (a **crossing**). For high-throughput or low-latency I/O these two costs dominate:

- **Syscall crossings** — one `read`/`write` per operation, hundreds of ns each (Chapter 98), multiplied by the operation rate.
- **Data copies** — the kernel↔user buffer copy consumes memory bandwidth and cache, and for large transfers the copy itself can exceed the device time.

> **Why this matters.** Traditional I/O (`read` into a buffer, process, `write` out) pays *both* costs on *every* operation: a crossing to initiate and a copy to move the bytes. At high packet or request rates, a server can spend most of its CPU on syscall entry/exit and `memcpy` rather than on application logic. The two families of techniques in this chapter target the two costs independently: **completion-based models** (`io_uring`) amortise the *crossings*, and **zero-copy** techniques eliminate the *copies*. The fastest I/O does both.

---

## 99.2 Blocking, Non-Blocking, and Readiness (`epoll`)

The historical progression of Linux I/O models:

- **Blocking I/O:** `read` sleeps the thread until data arrives. Simple, but one thread per connection — does not scale past a few thousand connections (each thread is an 8 MB stack and a scheduling entity).
- **Non-blocking + readiness:** mark sockets non-blocking and ask the kernel which are *ready*. `select`/`poll` are O(n) per call (they re-scan all fds); **`epoll`** is O(ready) — you register fds once and the kernel returns only those that became ready.

```cpp
// Min standard: C++11 + Linux. epoll readiness loop (non-portable).
// int ep = epoll_create1(0);
// epoll_ctl(ep, EPOLL_CTL_ADD, fd, &ev);          // register once
// for (;;) {
//   int n = epoll_wait(ep, events, MAX, -1);      // ONE syscall returns all ready fds
//   for (int i = 0; i < n; ++i)
//     handle(events[i].data.fd);                  // then a read()/write() PER ready fd
// }
```
*Listing 99.1 — `epoll`: one wait returns all ready fds, but each still needs a `read`/`write` syscall.*

> **Why this matters / cost model.** `epoll` is the foundation of every scalable Linux server (nginx, Redis, libevent) because it decouples the *number of connections* from the *cost of finding ready ones* — O(ready), not O(total). It is a **readiness** model: the kernel tells you a socket *can* be read, then *you* issue the `read`. That is still one syscall per ready operation plus the kernel↔user copy — so `epoll` solves the C10K scaling problem but not the per-operation crossing/copy cost. That residual is what the completion model removes.

---

## 99.3 The Completion Model: `io_uring`

**`io_uring`** (Linux 5.1+) is a fundamentally different model: two shared ring buffers — a **submission queue (SQ)** and a **completion queue (CQ)** — mapped between user space and kernel. You write I/O requests into the SQ and read results from the CQ *without a syscall per operation*; one `io_uring_enter` (or none, in polled mode) submits and reaps *many* operations at once.

```cpp
// Min standard: C++11 + liburing (Linux 5.1+). Conceptual; non-portable.
// io_uring ring; io_uring_queue_init(QD, &ring, 0);
// // Submit many reads with NO syscall each:
// for (each request) {
//   io_uring_sqe* sqe = io_uring_get_sqe(&ring);
//   io_uring_prep_read(sqe, fd, buf, len, offset);   // queue into the SQ ring
// }
// io_uring_submit(&ring);                              // ONE syscall submits the whole batch
// io_uring_cqe* cqe;
// io_uring_wait_cqe(&ring, &cqe);                      // reap completions from the CQ ring
```
*Listing 99.2 — `io_uring`: batch-submit via a shared ring, reap completions asynchronously. Linux-specific.*

> **Why this matters / cost model.** `io_uring` attacks the crossing cost directly: instead of one syscall *per operation*, you batch dozens into one `submit`, and in **SQPOLL** mode a kernel thread polls the SQ ring so you make *zero* syscalls on the submission path. It is a **completion** model — you submit the *whole* operation ("read these bytes into this buffer") and are notified when it is *done*, not merely when the fd is ready — which also eliminates the second syscall (`epoll` tells you ready, then you `read`; `io_uring` just reads). It supports nearly every I/O operation (network, file, `fsync`, even `openat`), registered buffers/fds to skip per-call setup, and chained operations. The cost is complexity (asynchronous completion handling, buffer lifetime management) and recency (kernel 5.1+, evolving API, some security restrictions in hardened environments). For the highest-throughput I/O, `io_uring` is the modern Linux answer.

---

## 99.4 Memory-Mapped I/O

**`mmap`** maps a file (or device) directly into the process's address space, so reading the file becomes ordinary memory access — the kernel pages content in on demand (a minor/major fault, Chapter 88) rather than via explicit `read` syscalls and copies.

```cpp
// Min standard: C++11 + POSIX. Memory-map a file for direct access (non-portable specifics).
#include <sys/mman.h>
#include <fcntl.h>
// int fd = open(path, O_RDONLY);
// void* p = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
// // Access p[i] like memory; the kernel faults pages in transparently. No read() syscalls.
// munmap(p, size);
```
*Listing 99.3 — `mmap` turns file access into memory access, paging on demand. POSIX/Linux.*

> **Why this matters / cost model.** `mmap` eliminates the explicit-`read`-plus-copy model for file I/O: there is no user buffer to copy into — your code reads the page cache directly, and the OS shares physical pages between the file cache and your mapping (one copy from disk, none to user space). This is ideal for random access to large files (databases, indexes) and for sharing read-only data between processes. The trade-offs: access *latency is now a page fault* (a major fault stalls milliseconds — Chapter 88's hazard returns), faults are unpredictable (bad for the hot path unless pre-faulted with `MAP_POPULATE`/`madvise`), and writes have subtle flush/durability semantics (`msync`). `mmap` shines for read-mostly random access to large files; for streaming sequential I/O, `io_uring` or buffered reads are usually better.

---

## 99.5 Zero-Copy Techniques

Even with batched syscalls, the *copy* between kernel and user buffers remains. **Zero-copy** techniques move data between file descriptors (or to the NIC) without routing it through a user buffer at all:

- **`sendfile(out, in, ...)`** — copy a file directly to a socket inside the kernel (the classic static-file-server optimization); the bytes never enter user space.
- **`splice`/`vmsplice`** — move data between an fd and a pipe without copying, enabling kernel-side plumbing of data flows.
- **`MSG_ZEROCOPY`** (send) and **registered buffers** (`io_uring`) — the NIC DMAs directly from user pages; the kernel pins the pages and notifies on completion, avoiding the copy into the socket buffer.

> **Why this matters / cost model.** For large transfers the kernel↔user copy is not free — it consumes memory bandwidth, pollutes the cache with data you may only forward, and at 10–100 Gbps line rates the copy can become *the* bottleneck. `sendfile` serving a static file does *zero* user-space copies: disk→page-cache→NIC entirely in the kernel, which is why it transformed web-server throughput. `MSG_ZEROCOPY` and `io_uring` registered buffers let the NIC DMA straight from your buffer. The costs: zero-copy send requires the buffer stay stable until the async completion (you cannot reuse it immediately), `MSG_ZEROCOPY` only pays off above a size threshold (the page-pinning overhead exceeds a small copy), and it complicates buffer management. Zero-copy is for *large* or *high-rate* transfers where the copy genuinely dominates — for small messages a plain copy is cheaper than the bookkeeping.

---

## 99.6 Choosing an I/O Model

| Model | Crossings | Copies | Best for |
|---|---|---|---|
| Blocking (thread-per-conn) | One per op | One per op | Low connection counts, simplicity |
| `epoll` (readiness) | One per ready op | One per op | Scalable servers (C10K+) |
| `io_uring` (completion) | Amortised / zero (SQPOLL) | One (or zero with registered buffers) | Highest-throughput modern I/O |
| `mmap` | Faults, not syscalls | Zero to user (shared pages) | Random access to large files |
| `sendfile`/zero-copy | Few | Zero (kernel-internal) | Large/streaming transfers |
| Kernel bypass (Ch. 100) | Zero | Zero | Extreme packet rates |

> **The discipline.** High-performance I/O is the systematic removal of the two non-device costs. Climb from the simple model to the one your rate demands: blocking for a handful of connections; `epoll` for scalable connection counts; `io_uring` when per-operation syscall cost dominates; `mmap` for random large-file access; zero-copy (`sendfile`, registered buffers) when the data copy itself is the bottleneck. Each step trades simplicity for the elimination of a crossing or a copy, and the right stopping point is dictated by *measurement* (`strace -c` for crossings, `perf` for copy/`memcpy` time), not ambition. When even the kernel's involvement is too much — at millions of packets per second — the next chapter removes it entirely.
