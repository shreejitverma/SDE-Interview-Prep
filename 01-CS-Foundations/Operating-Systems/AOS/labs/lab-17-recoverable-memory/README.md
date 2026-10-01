---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L08a, L08b]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-17-recoverable-memory: Recoverable virtual memory: undo and redo logs, fsync, crash injection

> [!info] Goal
> Build a simple Lightweight Recoverable Virtual Memory (LRVM) system in C to demonstrate undo/redo logging, aborts, log truncation, and crash recovery. Run a storage benchmark to observe the fsync overhead that motivated systems like Rio Vista.

## Concepts Exercised

- **L08a (LRVM):** `set_range` undo logging, in-memory updates, redo log formatting, `fsync` boundaries, log truncation.
- **L08b (Rio Vista):** Storage latency boundaries and the cost of flushing synchronous updates to disk.

## Honor Code Guard

There is no honor code restriction for this lab. LRVM is an architectural paradigm from a research paper, not a direct graded assignment for CS 6210. 

## Prerequisites

- The `aos` Lima VM is running (see [setup](../setup/README.md)).
- Standard build tools (`gcc`, `make`) and `fio` for latency measurement.

## Run Commands

Run the testing suite inside the VM:
```bash
../setup/run-in-vm.sh lab-17-recoverable-memory test
```

## What You Should See

You should see successful commits, aborts, log truncations, and a successful recovery from an injected `kill -9` crash. Finally, an `fio` benchmark highlights `fsync` latencies.

```text
--- Running test_abort ---
PASS: test_abort
--- Running test_commit ---
PASS: test_commit
--- Running test_truncate ---
PASS: test_truncate
--- Running test_crash_recovery ---
PASS: test_crash_recovery (uncommitted tx rolled back)
--- fio fsync latency test ---
Motivating RioVista: standard RVM pays high fsync overhead.
  lat (usec)   : 2=26.05%, 4=2.84%, 10=1.60%, 20=0.05%, 50=4.22%
  lat (usec)   : 100=19.03%, 250=0.23%, 500=0.03%, 750=0.03%
  lat (msec)   : 2=0.01%
PASS: fio_fsync
```

*Note: Your `fio` latencies may vary based on your host SSD and virtualization overhead, but they typically reach hundreds of microseconds or milliseconds, showing the cost of `fsync` compared to simple memory operations.*

## How It Works

1. **Initialization (`rvm_init`)**: RVM applies any existing redo log records to the backing file (crash recovery). It then memory-maps the backing file privately (`MAP_PRIVATE`). This ensures modifications stay in RAM and do not flush to the disk unless explicitly copied.
2. **Pre-modification (`rvm_set_range`)**: Before writing to memory, the library creates an undo record in memory with the original contents. 
3. **Commit (`rvm_end_transaction`)**: RVM takes the modified memory and appends a "redo record" to the log file on disk, followed by a commit marker and an `fsync()`.
4. **Abort (`rvm_abort_transaction`)**: RVM simply restores the memory using the in-memory undo records.
5. **Crash Injection (`test_crash_recovery`)**: A child process forks, modifies the data, and crashes via `kill -9` before calling `rvm_end_transaction`. When the parent re-initializes RVM, it sees the old state because the uncommitted changes were never flushed to the log.
6. **Truncation (`rvm_truncate`)**: Log records are systematically applied to the permanent backing file. Once complete, the redo log is emptied.

## Experiments to Try

1. **Remove `fsync` from commit**:
   - *Prompt:* What happens if you remove the `fsync()` call from `rvm_end_transaction`? Can you cause `test_crash_recovery` to fail?
   - *Hypothesis:* Yes. Without `fsync()`, the redo log might remain in the OS page cache. If the kernel crashes or loses power before the cache is flushed, committed data will be lost. (Note: A standard user-space `kill -9` might still preserve the cache, so you would need a kernel panic to observe data loss).
2. **Comment out `rvm_set_range`**:
   - *Prompt:* Comment out `rvm_set_range` in `test_abort`. Predict the output of `make test`.
   - *Hypothesis:* The `test_abort` assertion will fail. The memory will hold the "aborted!!" string because the system didn't create an undo record to roll it back.
3. **Adjust the Fio block size**:
   - *Prompt:* Change `bs=1k` to `bs=128k` in `fio-fsync.sh`. How will the fsync latency profile change?
   - *Hypothesis:* Latencies will increase significantly since the disk must write much more data per sync operation.

## Questions

<details>
<summary>Why does the implementation use <code>MAP_PRIVATE</code> instead of <code>MAP_SHARED</code>?</summary>

Using `MAP_SHARED` would allow the OS to lazily write back dirty pages directly to the backing file behind the library's back. This breaks atomicity: uncommitted changes could leak into the permanent file before a transaction completes. `MAP_PRIVATE` ensures the original backing file remains untouched by direct memory accesses, letting the RVM library meticulously control disk updates via the redo log.
</details>

<details>
<summary>How does Rio Vista improve upon this traditional LRVM design?</summary>

Rio Vista eliminates the redo log entirely by running on battery-backed RAM (the Rio file cache). Once an application stores data into the memory map, it is guaranteed to survive power outages and OS crashes. Vista only needs an in-memory undo log to handle software aborts. This drops transaction overhead by avoiding the expensive `fsync` system calls you saw in the `fio` test.
</details>

<details>
<summary>During crash recovery, how does the system know which records are safe to apply?</summary>

The log strictly relies on a commit marker. As RVM reads the redo log during initialization, it queues up pending updates. If it reaches the end of the file without seeing an `END_TX` commit marker, it assumes the system crashed mid-commit. The partial records are discarded, ensuring only fully atomic transactions are recovered.
</details>

<details>
<summary>Why isn't serializability enforced by the library?</summary>

LRVM was designed to be lightweight. By decoupling serializability from atomicity and permanence, LRVM leaves concurrency control (like locking) up to the application. This allows developers to use granular application-specific locks, improving throughput and avoiding the heavy IPC overhead seen in full database systems like Camelot.
</details>
