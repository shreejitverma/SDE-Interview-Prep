---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L02b, L02c]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-02-extensibility: Safe extensibility today: bpftrace and eBPF, plus application-level paging with userfaultfd

> [!info] Goal
> Make L02b (SPIN) and L02c (Exokernel) concrete with real commands and modern Linux features.

See [setup](../setup/README.md) for the VM.

## Prerequisites

- The AOS VM running and configured.
- `bpftrace` installed (provided by `make install` in the setup directory).
- Root privileges (via `sudo`) to load eBPF programs and use `userfaultfd`.

## Run commands

```sh
make run
make test
```

## What you should see

The output demonstrates both an application-level page fault handler (Exokernel concept) and a safe, in-kernel extension (SPIN concept).

```
--- Running userfaultfd_demo (Exokernel analogy) ---
Mapped 16384 bytes at 0xf6b1e29c2000
[uffd] Thread started
Accessing memory at 0xf6b1e29c2000...
[uffd] poll returned 1, revents=0x1
[uffd] Page fault at address: 0xf6b1e29c2000
[uffd] Page mapped successfully.
Read value: 'Xello from Exokernel userfaultfd handler! (Address: 0xf6b1e29c2000)'
```

For the eBPF trace, you will see a process being traced dynamically without kernel recompilation:

```
--- Running bpftrace exec trace (SPIN analogy) ---
Tracing execve() to demonstrate safe extensibility...
Attaching 1 probe...
Process bpftrace (PID 3943) is executing /usr/bin/sleep
```

## How it works

### eBPF and bpftrace (SPIN analogy)
SPIN (L02b) advocated for downloading type-safe, logically in-kernel extensions to avoid context switch overhead. Today, **eBPF** provides this exact capability. Programs are written in a restricted subset of C, compiled to eBPF bytecode, verified by the kernel to ensure they cannot crash or loop infinitely, and JIT-compiled for native performance. `bpftrace` is a high-level tracing language that compiles down to eBPF. In `trace_exec.sh`, we attach a small eBPF program to the `sys_enter_execve` tracepoint to print arguments.

### userfaultfd (Exokernel analogy)
Exokernel (L02c) pushed for secure, application-level resource management, including letting applications handle their own page faults. Linux `userfaultfd` allows exactly this. An application registers a memory range and maps it. When a thread accesses an unmapped page in that range, the kernel pauses the thread and sends a message to the `userfaultfd` file descriptor. A background handler thread reads the message, fetches or creates the page content, and uses the `UFFDIO_COPY` ioctl to map the page and wake the faulting thread.

## Experiments

1. **Change the tracepoint:**
   Modify `trace_exec.sh` to trace `sys_enter_openat` instead of `execve`. Run it with `-c 'cat /etc/os-release'`.
   > *Prediction:* What files will `cat` open before it opens `/etc/os-release`?
2. **Trace a specific process:**
   Find the PID of a long-running process (like an SSH daemon). Modify the bpftrace script to only trace that PID using a predicate like `/pid == 1234/`.
   > *Prediction:* How often does an idle daemon invoke system calls?
3. **Change the memory access pattern:**
   In `userfaultfd_demo.c`, add a loop that triggers page faults sequentially across 10 pages.
   > *Prediction:* Will the fault handler thread keep up, and does the address logged increment by precisely 4096 bytes each time?

## Questions

<details>
<summary>Why must eBPF programs pass a kernel verifier, and how does this relate to SPIN's use of Modula-3?</summary>

The verifier ensures the program cannot crash the kernel, access unauthorized memory, or loop infinitely. SPIN achieved safety through the type-safe compiler of Modula-3 and dynamic linking restrictions. Both approaches aim for "safe extensibility," but eBPF relies on a strict bytecode verifier rather than a specific high-level language compiler.
</details>

<details>
<summary>Why does userfaultfd use a separate file descriptor and poll mechanism rather than a signal like SIGSEGV?</summary>

Signals are asynchronous, interrupt the current thread's execution context, and are difficult to handle safely, especially in multi-threaded programs. `userfaultfd` provides a synchronous, file-based API. A dedicated background thread can block on `poll` or `read` without disrupting the state of the faulting thread, which is simply paused by the kernel until the memory is provided.
</details>

<details>
<summary>In the userfaultfd demo, what happens to the faulting thread while the handler thread is reading the uffd message and mapping the page?</summary>

The kernel places the faulting thread in an unrunnable state (sleeping). It remains suspended inside the kernel's page fault handler. Once the user-space handler calls `ioctl(UFFDIO_COPY)` to map the page, the kernel wakes up the faulting thread, which then retries the instruction and succeeds.
</details>
