---
tags: [trading/ipc-messaging, type/concept]
aliases: [Shared Memory IPC, POSIX SHM, shm_open, mmap IPC, Zero-Copy IPC, Inter-Process Communication]
status: evergreen
module: 09
created: 2026-08-22
---

> [!summary]
> Shared Memory (SHM) IPC enables independent Linux processes to communicate across CPU cores with zero kernel involvement, zero socket syscall overhead, and zero memory copies. By mapping the same physical RAM into the virtual address spaces of both processes via `shm_open` and `mmap`, cross-process message transfers execute at the physical speed of the CPU L3/L2 cache hierarchy in under 25 nanoseconds.

---

## Why it matters
In high-frequency trading architectures, separate specialized processes run on the same server host (e.g., Feed Handler Process $\to$ Strategy Engine Process $\to$ Pre-Trade Risk Process $\to$ Order Gateway Process).

Communicating between processes via standard Linux IPC mechanisms introduces catastrophic latency penalties:
- **Unix Domain Sockets (`AF_UNIX`)**: Involves kernel transitions, socket buffers (`sk_buff`), and context switches (**1,200–3,500 ns**).
- **Linux Pipes / FIFOs**: Involves pipe buffer synchronization and kernel mutexes (**800–2,000 ns**).
- **Shared Memory (SHM)**: Once mapped, reads and writes are **pure CPU memory loads and stores** (**15–30 ns**).

```mermaid
flowchart LR
    subgraph ProcessA ["Process A: Feed Handler (Core 2)"]
        PA_VA["Virtual Memory: 0x7fff0000"]
        PA_LOOP["User-Space Write Loop"]
        PA_LOOP --> PA_VA
    end

    subgraph PhysicalRAM ["HugePage-Backed Physical RAM (Locked in DRAM/LLC)"]
        PHYS["Physical Memory Page (Page Frame #84920)"]
    end

    subgraph ProcessB ["Process B: Matching Engine (Core 4)"]
        PB_VA["Virtual Memory: 0x7fee1000"]
        PB_LOOP["User-Space Polling Loop"]
        PB_LOOP <--> PB_VA
    end

    PA_VA <==>|mmap(MAP_SHARED) - Zero Syscalls| PHYS
    PB_VA <==>|mmap(MAP_SHARED) - Zero Syscalls| PHYS
```

---

## Mechanism

### 1. The POSIX Shared Memory Lifecycle
1. **Creation / Opening (`shm_open`)**: Allocates a named memory object in the `tmpfs` virtual filesystem (typically mounted under `/dev/shm`).
2. **Sizing (`ftruncate`)**: Sets the physical byte capacity of the memory segment.
3. **Address Mapping (`mmap`)**: Maps the physical pages into the calling process's virtual address space with `MAP_SHARED`.
4. **Memory Locking (`mlock`)**: Locks the mapped pages into physical RAM to eliminate page faults.

### 2. Cache Coherence Across Processes
Because both processes map the **exact same physical DRAM pages**, the underlying hardware cache coherence protocol (MESI/MOESI) manages synchronization automatically:
- When Process A writes to the shared memory ring buffer, the cache line in Process B's L1/L2 cache is invalidated via standard hardware snooping.
- When Process B reads the updated index, it fetches the line directly across the intra-socket mesh/ring bus (**~12–25 ns**).
- **Zero OS kernel intervention occurs during steady-state messaging.**

---

## In Practice

### Production-Grade POSIX Shared Memory Segment Wrapper in C++20

```cpp
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>

template <typename HeaderStruct>
class SharedMemoryChannel {
private:
    std::string shm_name_;
    size_t size_bytes_;
    int shm_fd_{-1};
    HeaderStruct* mapped_header_{nullptr};
    bool is_creator_{false};

public:
    // Open or create shared memory channel
    SharedMemoryChannel(const std::string& name, size_t size_bytes, bool create_master)
        : shm_name_("/" + name), size_bytes_(size_bytes), is_creator_(create_master) {
        
        int flags = O_RDWR;
        if (is_creator_) {
            flags |= O_CREAT | O_EXCL;
            // Clean up stale SHM file if it exists
            shm_unlink(shm_name_.c_str());
        }

        shm_fd_ = shm_open(shm_name_.c_str(), flags, 0666);
        if (shm_fd_ < 0) {
            throw std::runtime_error("shm_open failed for " + shm_name_ + ": " + std::strerror(errno));
        }

        if (is_creator_) {
            if (ftruncate(shm_fd_, size_bytes_) != 0) {
                close(shm_fd_);
                shm_unlink(shm_name_.c_str());
                throw std::runtime_error("ftruncate failed: " + std::string(std::strerror(errno)));
            }
        }

        // Map into virtual address space
        void* ptr = mmap(nullptr, size_bytes_, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, shm_fd_, 0);
        if (ptr == MAP_FAILED) {
            close(shm_fd_);
            if (is_creator_) shm_unlink(shm_name_.c_str());
            throw std::runtime_error("mmap failed: " + std::string(std::strerror(errno)));
        }

        // Lock memory to guarantee zero page faults during live execution
        if (mlock(ptr, size_bytes_) != 0) {
            std::cerr << "Warning: mlock failed on shared memory segment\n";
        }

        mapped_header_ = static_cast<HeaderStruct*>(ptr);

        if (is_creator_) {
            // Zero-initialize memory on creation
            std::memset(mapped_header_, 0, size_bytes_);
        }
    }

    [[nodiscard]] HeaderStruct* get() noexcept { return mapped_header_; }

    ~SharedMemoryChannel() {
        if (mapped_header_ && mapped_header_ != MAP_FAILED) {
            munlock(mapped_header_, size_bytes_);
            munmap(mapped_header_, size_bytes_);
        }
        if (shm_fd_ >= 0) close(shm_fd_);
        if (is_creator_) {
            shm_unlink(shm_name_.c_str());
        }
    }

    SharedMemoryChannel(const SharedMemoryChannel&) = delete;
    SharedMemoryChannel& operator=(const SharedMemoryChannel&) = delete;
};
```

---

## Numbers

*Hardware Baseline: Intel Xeon Sapphire Rapids @ 4.0 GHz, Intra-Socket Cores.*

| Inter-Process Mechanism | Transit Latency ($p50$) | Transit Latency ($p99.9$) | Max Throughput | OS Context Switches |
| :--- | :--- | :--- | :--- | :--- |
| **TCP Loopback (`127.0.0.1`)** | **3,500–6,500 ns** | **18.0–45.0 µs** | ~1.5M msgs/sec | Yes (Syscalls + ksoftirqd) |
| **Unix Domain Socket (`AF_UNIX`)**| **1,400–2,800 ns** | **8.0–22.0 µs** | ~3.8M msgs/sec | Yes (Syscalls + copy) |
| **Linux Named Pipe (FIFO)** | **900–1,800 ns** | **5.0–15.0 µs** | ~5.2M msgs/sec | Yes (Kernel pipe lock) |
| **POSIX Shared Memory (SHM)** | **18–28 ns** | **<45 ns** | **>55M msgs/sec** | **ZERO (Pure L3/Cache)** |

---

## Trade-offs

| IPC Topology | Advantages | Disadvantages / Operational Hazards |
| :--- | :--- | :--- |
| **Shared Memory (SHM)** | Absolute lowest latency; zero kernel overhead; zero memory copy. | Complex crash recovery; memory corruption in one process can corrupt shared segment. |
| **Unix Domain Sockets** | OS enforces process isolation; automatic buffering and OS backpressure. | 100x slower than SHM; high context-switch jitter. |
| **HugeTLBFS Backed SHM** | Eliminates TLB misses on large historical ring buffers (>64MB). | Requires pre-allocated hugepage pools in Linux kernel. |

---

> [!warning] Gotchas
> 1. **Process Crash Stale Locks / Dirty State**: If Process A crashes halfway through writing a message into the SHM ring, Process B may spin forever waiting for the sequence number or read corrupted memory. *Remedy: Use atomic commit sequence flags and include heartbeat timestamps in the SHM header.*
> 2. **The `/dev/shm` Size Limit**: By default, Linux mounts `/dev/shm` as half of total physical RAM. If multiple processes allocate large ring buffers, `/dev/shm` fills up, causing subsequent `ftruncate` calls to fail with `ENOSPC` (No space left on device).

---

## Lab
**Objective**: Build a bidirectional, cross-process shared memory IPC channel between two independent processes, measuring round-trip transit time with `rdtsc`.

**Success Criteria**:
1. Run Producer process pinned to Core 2 and Consumer process pinned to Core 4.
2. Transfer 10,000,000 messages across the SHM channel.
3. Verify that one-way transit latency is **under 25 nanoseconds** in steady state.

---

> [!question]- Self-test
> 1. **Why does communicating over POSIX Shared Memory (`shm_open` + `mmap`) execute with zero system calls during steady-state trading?**
>    *Answer*: The system call (`mmap`) is executed only once during process startup to map the physical memory pages into both processes' virtual address spaces. Once mapped, reading and writing to the shared memory region are performed via native CPU load and store assembly instructions (`MOV`), completely bypassing the operating system kernel and eliminating all system call overhead.
> 2. **What happens to a POSIX shared memory object if the creator process crashes without calling `shm_unlink()`?**
>    *Answer*: POSIX shared memory objects have kernel persistence: the memory segment remains allocated in the `tmpfs` virtual filesystem (`/dev/shm`) even after all processes terminate. When a restarted process calls `shm_open` with `O_CREAT | O_EXCL`, the call will fail with `EEXIST` unless the stale object is explicitly unlinked via `shm_unlink()`.
> 3. **How does NUMA node placement affect Shared Memory IPC performance between two processes?**
>    *Answer*: If Process A (Core 2, Socket 0) creates and touches the shared memory segment first, Linux allocates the physical pages on Socket 0 (first-touch policy). If Process B runs on Core 18 (Socket 1), all of Process B's reads and writes must traverse the high-latency UPI/Infinity Fabric interconnect, injecting an extra 80–150 ns penalty. Both processes should be pinned to cores on the **same NUMA socket**.

---

## Related
- [[Notes/The LMAX Disruptor Architecture]]
- [[Notes/Aeron Messaging Transport]]
- [[Notes/Lock-Free SPSC Ring Buffer Design]]
- [[Notes/NUMA Topologies and Inter-Socket Jitter]]
- [[MOC - 09 Messaging & IPC]]

## Sources
- [[Sources/The LMAX Architecture by Martin Fowler]]
- [[Sources/Systems Performance by Brendan Gregg]]
- [[Sources/Linux Programmer's Manual - shm_overview(7)]]
