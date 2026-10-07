---
type: concept
track: [sde]
level:
status: solid
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P3L5"
  - "Linux Device Drivers, 3rd Ed., Corbet et al."
  - "Understanding the Linux Kernel, Bovet & Cesati"
---

# P3L5: I/O Management

> **Module goal:** Master I/O device management - device drivers, block vs. character devices, I/O scheduling (NOOP, CFQ, Deadline, BFQ, mq-deadline), DMA, interrupt handling, and the Linux VFS layer.

## Table of Contents

- [1. I/O Device Types and Architecture](#1-io-device-types-and-architecture)
- [2. Device Drivers](#2-device-drivers)
- [3. I/O Communication Methods: PIO, MMIO, DMA](#3-io-communication-methods-pio-mmio-dma)
- [4. Interrupt Handling and Top/Bottom Halves](#4-interrupt-handling-and-topbottom-halves)
- [5. Block I/O Layer and I/O Scheduling](#5-block-io-layer-and-io-scheduling)
- [6. I/O Scheduling Algorithms](#6-io-scheduling-algorithms)
- [7. Virtual File System (VFS) Layer](#7-virtual-file-system-vfs-layer)
- [8. Linux I/O Subsystem: From Syscall to Disk](#8-linux-io-subsystem-from-syscall-to-disk)
- [9. I/O Multiplexing: select, poll, epoll, kqueue](#9-io-multiplexing-select-poll-epoll-kqueue)
- [10. Windows I/O Architecture](#10-windows-io-architecture)
- [11. Quizzes and Exercises](#11-quizzes-and-exercises)
- [12. Key Takeaways](#12-key-takeaways)
- [13. Linux Block Layer Internals](#13-linux-block-layer-internals)
- [14. Zero-Copy I/O Techniques](#14-zero-copy-io-techniques)
- [15. The Linux Page Cache Deep Dive](#15-the-linux-page-cache-deep-dive)
- [16. Production I/O Tuning Reference](#16-production-io-tuning-reference)
- [17. NVMe Protocol Internals](#17-nvme-protocol-internals)
- [18. Advanced io_uring Patterns](#18-advanced-io_uring-patterns)
- [19. Storage Performance Benchmarking](#19-storage-performance-benchmarking)
- [20. Windows I/O Completion Ports (IOCP)](#20-windows-io-completion-ports-iocp)
- [21. macOS Storage Architecture and Darwin I/O Internals](#21-macos-storage-architecture-and-darwin-io-internals)

---

## 1. I/O Device Types and Architecture

```
I/O Device Architecture:
+------------------+     +------------------+     +-----------+
|    CPU Core      |     |   Device         |     | Physical  |
|                  |     |   Controller     |     | Device    |
|  user program    |     |  +------------+  |     |           |
|       |          |     |  | Status reg |  |     | disk      |
|  syscall         |     |  | Command reg|  |     | NIC       |
|       |          |     |  | Data reg   |  |     | GPU       |
|  device driver   |<--->|  +------------+  |<--->|           |
|                  | bus |  | DMA engine |  |     |           |
+------------------+     +------------------+     +-----------+
```

### Device Categories

| Category | Examples | Access Pattern | Interface |
|----------|---------|----------------|-----------|
| **Block** | HDD, SSD, NVMe | Random access, fixed-size blocks | `/dev/sda`, `/dev/nvme0n1` |
| **Character** | Terminal, keyboard, serial port, mouse | Sequential byte stream | `/dev/tty`, `/dev/input/mice` |
| **Network** | Ethernet, WiFi | Packet-oriented | Socket API (not /dev) |
| **Pseudo** | `/dev/null`, `/dev/zero`, `/dev/random` | Special | Various |

```bash
# List block devices
lsblk

# List character devices
ls -la /dev/tty*

# See all devices with major/minor numbers
ls -la /dev/ | head -20
# crw-rw-rw- 1 root root 1, 3  /dev/null    (char, major 1, minor 3)
# brw-rw---- 1 root disk 8, 0  /dev/sda     (block, major 8, minor 0)
```

---

## 2. Device Drivers

A device driver is kernel code that mediates between the OS and a specific hardware device.

```
Kernel Architecture:
+----------------------------------------------------------+
|  User Space                                               |
|    Application                                            |
+----------------------------------------------------------+
|  System Call Interface                                     |
+----------------------------------------------------------+
|  VFS Layer    | Network Stack | Input Subsystem           |
+----------------------------------------------------------+
|  Block Layer  | Character     | net_device                |
|               | dev framework |                           |
+----------------------------------------------------------+
|  Device Drivers                                           |
|  +--------+ +--------+ +--------+ +--------+             |
|  | SCSI   | | NVMe   | | e1000  | | USB    |             |
|  | driver | | driver | | driver | | driver |             |
|  +--------+ +--------+ +--------+ +--------+             |
+----------------------------------------------------------+
|  Hardware                                                 |
+----------------------------------------------------------+
```

```bash
# List loaded kernel modules (drivers)
lsmod

# Info about a module
modinfo e1000

# Load/unload a module
sudo modprobe e1000      # load
sudo modprobe -r e1000   # unload

# See kernel messages from drivers
dmesg | tail -20
```

---

## 3. I/O Communication Methods: PIO, MMIO, DMA

### Programmed I/O (PIO)

```
CPU reads/writes device registers one byte/word at a time:

for each byte:
    write byte to device data register
    wait for device ready

Problem: CPU is busy during entire transfer
         (CPU-bound for large transfers)
```

### Memory-Mapped I/O (MMIO)

```
Device registers are mapped to physical memory addresses:

Physical Address Space:
0x00000000 - 0x3FFFFFFF: RAM
0x40000000 - 0x40000FFF: GPU registers (MMIO)
0x40001000 - 0x40001FFF: NIC registers (MMIO)

CPU accesses device registers via normal load/store instructions
to these special addresses.
No special I/O instructions needed.
```

### Direct Memory Access (DMA)

```
DMA: Device transfers data directly to/from memory without CPU involvement

1. CPU programs DMA controller:
   - Source address
   - Destination address
   - Transfer size
   - Direction (read/write)

2. DMA controller performs transfer:
   Device <--> Memory  (CPU is free to do other work)

3. DMA controller raises interrupt when done:
   CPU handles completion

CPU involvement: setup + interrupt handling only
Transfer: handled by DMA hardware
```

```bash
# See DMA allocations
cat /proc/buddyinfo | grep DMA
# Node 0, zone DMA     ...
# Node 0, zone DMA32   ...
```

---

## 4. Interrupt Handling and Top/Bottom Halves

### Interrupt Flow

```
Hardware event (e.g., disk I/O complete)
     |
     v
Interrupt controller (APIC) sends interrupt to CPU
     |
     v
CPU saves state, switches to kernel mode
     |
     v
Interrupt Descriptor Table (IDT) lookup -> handler address
     |
     v
Interrupt handler (ISR) runs
     |
     +-> Top half (hardirq): minimal, fast, interrupts disabled
     |     - Acknowledge interrupt
     |     - Copy urgent data
     |     - Schedule bottom half
     |
     +-> Bottom half (softirq/tasklet/workqueue): deferred, heavier
           - Process the data
           - Wake waiting processes
           - Can sleep (workqueue only)
```

### Top Half vs. Bottom Half

| Aspect | Top Half (hardirq) | Bottom Half |
|--------|-------------------|-------------|
| When | Immediately on interrupt | Deferred (kernel decides when) |
| Context | Interrupt context | Softirq, tasklet, or workqueue context |
| Can sleep? | NO | Softirq/tasklet: NO. Workqueue: YES |
| Preemptible? | NO | Softirq/tasklet: NO. Workqueue: YES |
| Duration | Must be very short (<100 us) | Can be longer |

```bash
# See interrupt counts per CPU
cat /proc/interrupts
#            CPU0    CPU1    CPU2    CPU3
#   0:       45      0       0       0   IO-APIC   2-edge    timer
#   1:       0       0       9       0   IO-APIC   1-edge    i8042
#   8:       0       0       0       1   IO-APIC   8-edge    rtc0
#  24:       0   15432       0       0   PCI-MSI    nvme0q1
#  25:       0       0   12890       0   PCI-MSI    nvme0q2

# See softirq counts
cat /proc/softirqs
# Monitor interrupt rate
watch -n 1 'cat /proc/interrupts | head -20'
```

---

## 5. Block I/O Layer and I/O Scheduling

```
Application:  read(fd, buf, 4096)
     |
     v
VFS Layer:    determines filesystem, inode, block numbers
     |
     v
Block Layer:  creates bio (block I/O) requests
     |         merges adjacent requests
     |         sorts requests (I/O scheduler)
     v
Device Driver: sends requests to hardware
     |
     v
Hardware:     disk controller, NVMe, etc.
```

### I/O Request Representation

```
struct bio {
    sector_t bi_sector;     // starting sector on disk
    unsigned int bi_size;   // total size in bytes
    struct bio_vec *bi_io_vec; // scatter-gather list
    // ...
};

I/O Scheduler merges and reorders bio requests:
  [Read sector 100] [Read sector 101] -> merge into [Read sectors 100-101]
  [Read sector 500] [Read sector 100] -> reorder to [Read 100] then [Read 500]
```

---

## 6. I/O Scheduling Algorithms

### For Rotational Disks (HDD)

Seek time is the dominant cost. Schedulers minimize head movement.

#### NOOP (No-op)

```
Simple FIFO. No reordering.
Best for: SSDs (no seek time), virtual machines, RAM disks

[R100] [R500] [R50] [R300] -> served in this order
```

#### CFQ (Completely Fair Queuing)

```
Allocates time slices to each process's I/O queue.
Each process gets a fair share of disk bandwidth.
Supports I/O priorities (ionice).

Process 1 queue: [R100] [R200]
Process 2 queue: [R500] [R600]
Process 3 queue: [R50]

Round-robin between queues with time slices.
```

#### Deadline

```
Two sorted queues (read/write) + two FIFO deadline queues.

Read deadline:  500 ms  (reads have tighter deadline)
Write deadline: 5000 ms

Normal operation: serve from sorted queues (minimize seek)
If a request's deadline expires: serve it immediately (prevent starvation)
```

### For SSDs and NVMe

#### mq-deadline (Multi-Queue Deadline)

```
Modern deadline scheduler for multi-queue block devices (NVMe).
One pair of sorted+FIFO queues per hardware queue.
Supports blk-mq (multi-queue block layer).

This is the default scheduler for NVMe devices on modern Linux.
```

#### BFQ (Budget Fair Queueing)

```
Advanced proportional-share scheduler.
Assigns each process a "budget" (number of sectors).
Low-latency guarantee for interactive I/O.
Best for: desktops with mixed interactive and background I/O.
```

```bash
# See current I/O scheduler for a device
cat /sys/block/sda/queue/scheduler
# [mq-deadline] kyber bfq none

# Change I/O scheduler
echo "bfq" | sudo tee /sys/block/sda/queue/scheduler

# Set I/O priority (Linux ionice)
ionice -c 2 -n 0 dd if=/dev/sda of=/dev/null bs=1M count=100
# -c 2 = best-effort class
# -n 0 = highest priority within class (0-7)

# Classes: 1=Realtime, 2=Best-effort, 3=Idle
ionice -c 3 ./background_backup.sh  # Idle: only runs when no other I/O
```

---

## 7. Virtual File System (VFS) Layer

VFS provides a **uniform interface** to all filesystem types.

```
Application:   open("/home/user/file.txt")
                     |
                     v
VFS Layer:     +---------------------------+
               | Common API:               |
               |  open, read, write, close |
               |  stat, mmap, ioctl        |
               +-----+------+------+------+
                     |      |      |
                ext4 |   XFS|   NFS|  tmpfs
                     |      |      |
               +-----v------v------v------+
               |  Filesystem Drivers       |
               +---------------------------+
               |  Block/Network Layer       |
               +---------------------------+
```

### VFS Key Objects

| Object | Purpose | Linux Structure |
|--------|---------|----------------|
| **Superblock** | Represents a mounted filesystem | `super_block` |
| **Inode** | Represents a file (metadata) | `inode` |
| **Dentry** | Directory entry (name -> inode mapping) | `dentry` |
| **File** | Represents an open file (per-process) | `file` |

```
Process opens /home/user/file.txt:

  file struct (per open fd)
  +--------------------+
  | f_pos = 0          |  (current read/write position)
  | f_flags = O_RDWR   |
  | f_op = &ext4_ops   |  (function pointers)
  | f_dentry --------->+  dentry
  +--------------------+  +---------------+
                          | d_name = "file.txt"
                          | d_inode -------->  inode
                          +---------------+    +-----------------+
                                               | i_size = 4096   |
                                               | i_mode = 0644   |
                                               | i_blocks = 8    |
                                               | i_sb (superblock)|
                                               +-----------------+
```

```bash
# VFS statistics
cat /proc/filesystems    # supported filesystem types
mount                     # currently mounted filesystems
cat /proc/mounts         # same, from kernel's view

# Inode info
stat /etc/passwd
# File: /etc/passwd
# Size: 2789  Blocks: 8  IO Block: 4096  regular file
# Inode: 1048618  Links: 1

# Dentry and inode cache stats
cat /proc/slabinfo | grep -E "dentry|inode"
```

---

## 8. Linux I/O Subsystem: From Syscall to Disk

```
read(fd, buf, 4096)
    |
    v
sys_read() [kernel]
    |
    v
VFS: file->f_op->read() dispatches to filesystem
    |
    v
Page Cache check: is this page already cached?
    YES -> copy from page cache to user buffer (fast!)
    NO  -> continue...
    |
    v
Filesystem (ext4): map file offset to disk block number
    |
    v
Block Layer: create bio request
    |
    v
I/O Scheduler: merge, sort, queue
    |
    v
Device Driver: submit to hardware
    |
    v
DMA: device reads disk block into page cache
    |
    v
Interrupt: transfer complete
    |
    v
Copy from page cache to user buffer
    |
    v
Return to user space
```

```bash
# Monitor I/O at each layer
# Block layer stats
iostat -x 1

# Per-process I/O
iotop

# Trace I/O requests through the stack
sudo perf trace -e 'block:*' dd if=/dev/sda of=/dev/null bs=4k count=10
```

---

## 9. I/O Multiplexing: select, poll, epoll, kqueue

### select (oldest, portable)

```c
fd_set read_fds;
FD_ZERO(&read_fds);
FD_SET(fd1, &read_fds);
FD_SET(fd2, &read_fds);

struct timeval tv = {5, 0};  // 5 second timeout
int ret = select(max_fd + 1, &read_fds, NULL, NULL, &tv);

if (FD_ISSET(fd1, &read_fds)) { /* fd1 is ready */ }
```

### poll (slightly better)

```c
struct pollfd fds[2];
fds[0] = (struct pollfd){fd1, POLLIN, 0};
fds[1] = (struct pollfd){fd2, POLLIN, 0};

int ret = poll(fds, 2, 5000);  // 5000 ms timeout

if (fds[0].revents & POLLIN) { /* fd1 ready */ }
```

### epoll (Linux, scalable)

```c
int epfd = epoll_create1(0);

struct epoll_event ev = {.events = EPOLLIN, .data.fd = fd1};
epoll_ctl(epfd, EPOLL_CTL_ADD, fd1, &ev);

struct epoll_event events[10];
int n = epoll_wait(epfd, events, 10, 5000);
for (int i = 0; i < n; i++) {
    if (events[i].data.fd == fd1) { /* fd1 ready */ }
}
```

### Comparison

| Feature | select | poll | epoll | kqueue (macOS/BSD) |
|---------|--------|------|-------|---------|
| Max FDs | 1024 (FD_SETSIZE) | No limit | No limit | No limit |
| Scaling | O(n) per call | O(n) per call | O(1) per event | O(1) per event |
| Edge-triggered | No | No | Yes (EPOLLET) | Yes |
| Thread-safe | Reentrant | Reentrant | Yes | Yes |
| Best for | <100 FDs | <1000 FDs | High-scale servers | macOS/BSD servers |

---

## 10. Windows I/O Architecture

```
Windows I/O Architecture:
+----------------------------------------------------------+
|  Application (Win32 API)                                  |
|    CreateFile, ReadFile, WriteFile                        |
+----------------------------------------------------------+
|  I/O Manager (NT Executive)                               |
|    Creates IRPs (I/O Request Packets)                     |
+----------------------------------------------------------+
|  Filter Drivers (antivirus, encryption)                   |
+----------------------------------------------------------+
|  Filesystem Driver (NTFS, ReFS)                           |
+----------------------------------------------------------+
|  Volume Manager / Storage Driver                          |
+----------------------------------------------------------+
|  Disk Driver                                              |
+----------------------------------------------------------+
|  Hardware (disk controller)                               |
+----------------------------------------------------------+
```

### Windows I/O Completion Ports (IOCP)

The Windows equivalent of epoll for high-performance I/O:

```c
// Create completion port
HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                                       NULL, 0, num_threads);

// Associate a socket with the IOCP
CreateIoCompletionPort((HANDLE)socket, hIOCP, (ULONG_PTR)context, 0);

// Worker thread loop
DWORD bytes;
ULONG_PTR key;
OVERLAPPED *ov;
while (GetQueuedCompletionStatus(hIOCP, &bytes, &key, &ov, INFINITE)) {
    // Process completed I/O operation
}
```

---

## 11. Quizzes and Exercises

### Quiz 1: I/O Device Classification (Clips 360-361)

> [!question]
> For each of the following hardware devices, classify whether it is typically used for **Input**, for **Output**, or for **Both**:
> 1. Keyboard
> 2. Speaker
> 3. Display monitor
> 4. Hard disk drive (HDD)
> 5. Microphone
> 6. Network Interface Card (NIC)
> 7. Flash memory card

> [!success]- Answer
> 1. **Keyboard:** Input
> 2. **Speaker:** Output
> 3. **Display monitor:** Output
> 4. **Hard disk drive (HDD):** Both (Read / Write)
> 5. **Microphone:** Input
> 6. **Network Interface Card (NIC):** Both (Receive / Transmit packets)
> 7. **Flash memory card:** Both (Read / Write)

---

### Quiz 2: I/O Devices as Special Files (Clips 366-367)

> [!question]
> Consider the following three Unix commands:
> 
> ```bash
> cat doc.txt > /dev/lp0
> echo "Hello, world" > /dev/lp0
> cp report.pdf /dev/lp0
> ```
> 
> What physical operation do all three commands perform?
> What does `lp0` represent?

> [!success]- Answer
> All three commands **print content to the system's first line printer**.
> In the Unix device abstraction, `/dev/lp0` represents the first parallel/USB line printer (`lp` = Line Printer, `0` = device index 0).
> Writing raw bytes to the special device file streams data directly to the printer controller via its device driver.

---

### Quiz 3: Pseudo and Virtual Devices (Clips 368-369)

> [!question]
> Name the full Unix path for the special pseudo-devices that provide the following kernel functions:
> 1. A device that accepts and discards all written data immediately without producing any output.
> 2. A device that produces an infinite, non-blocking stream of cryptographically secure pseudo-random bytes.

> [!success]- Answer
> 1. `/dev/null` (commonly used to suppress unwanted stdout or stderr streams).
> 2. `/dev/urandom` (produces pseudo-random bytes seeded by environmental kernel entropy; `/dev/random` historically blocked when entropy was depleted).

---

### Quiz 4: Exploring Device Nodes in `/dev` (Clips 370-371)

> [!question]
> When executing `ls -l /dev` on a POSIX system, what do the leading `b` and `c` file type characters signify?
> Give two examples of each.

> [!success]- Answer
> - `b` signifies a **Block Device**: Transmits data in fixed-size blocks (e.g., 512B or 4KB), supports random-access seeking, and leverages the kernel page cache (e.g., `/dev/sda`, `/dev/nvme0n1`).
> - `c` signifies a **Character Device**: Transmits data as an unbuffered, sequential stream of individual bytes without seeking (e.g., `/dev/tty`, `/dev/null`, `/dev/random`).

---

### Quiz 5: Programmed I/O (PIO) vs. Direct Memory Access (DMA) (Clips 375-376)

> [!question]
> Consider a hypothetical system where:
> - A CPU store instruction to a device register costs **1 cycle**.
> - Configuring the DMA controller requires **5 cycles**.
> - The PCI bus transfer width is 8 bytes.
> 
> Which device access method (PIO, DMA, or It Depends) is optimal for:
> 1. A keyboard?
> 2. A network interface card (NIC)?

> [!success]- Answer
> 1. **Keyboard: Programmed I/O (PIO).**
> Keyboards produce keystroke events consisting of only 1 or 2 bytes.
> Storing the bytes directly into the CPU registers requires 1 or 2 cycles, whereas configuring the DMA controller would consume 5 cycles of configuration overhead.
> 
> 2. **Network Interface Card (NIC): It depends.**
> For very small network packets (e.g., single-byte TCP acknowledgments requiring fewer than 5 register stores), PIO is faster than configuring the DMA controller.
> For standard payloads (such as 1500-byte MTU frames or 9000-byte jumbo frames), DMA is overwhelmingly superior: configuring the controller takes 5 cycles, after which the hardware transfers the entire buffer into RAM without burning CPU execution cycles.

---

### Quiz 6: Inode File Size Limits and Indirect Pointers (Clips 389-390)

> [!question]
> A Unix inode contains:
> - 12 direct block pointers
> - 1 single indirect pointer
> - 1 double indirect pointer
> - 1 triple indirect pointer
> 
> Each block pointer is 4 bytes wide.
> 1. If the disk block size is **1 KB**, what is the maximum supported file size (rounded to the nearest GB)?
> 2. If the disk block size is increased to **8 KB**, what is the maximum supported file size (rounded to the nearest TB)?

> [!success]- Answer
> **1. Disk block size = 1 KB ($1024$ bytes):**
> - Pointers per indirect block: $\frac{1024}{4} = 256 = 2^8$ pointers.
> - Direct blocks: 12 blocks.
> - Single indirect: $256$ blocks.
> - Double indirect: $256^2 = 65,536$ blocks.
> - Triple indirect: $256^3 = 16,777,216 = 2^{24}$ blocks.
> - Total blocks $\approx 2^{24}$ blocks.
> - Maximum file size: $2^{24} \times 1 \text{ KB} = 2^{34} \text{ bytes} =$ **16 GB**.
> 
> **2. Disk block size = 8 KB ($8192$ bytes):**
> - Pointers per indirect block: $\frac{8192}{4} = 2048 = 2^{11}$ pointers.
> - Triple indirect: $2048^3 = 2^{33}$ blocks.
> - Maximum file size: $2^{33} \times 8 \text{ KB} = 2^{33} \times 2^{13} \text{ bytes} = 2^{46} \text{ bytes} =$ **64 TB**.
> 
> Increasing the block size by an 8x factor yields a 4096x increase in addressable file capacity due to the cubic expansion of triple indirect blocks.

---

### Quiz 7: Modern NVMe I/O Schedulers

> [!question]
> Which I/O scheduler should be selected for high-performance NVMe Solid State Drives, and why are traditional elevator algorithms like CFQ obsolete for these devices?

> [!success]- Answer
> Use **`none`** or **`mq-deadline`**.
> Traditional disk schedulers (e.g., CFQ, Deadline, Anticipatory) were designed for spinning magnetic platters where rotational latency and mechanical seek times made sorting requests by sector number critical.
> NVMe SSDs have zero seek latency, support internal hardware parallelism across hundreds of flash channels, and expose up to 64,000 independent hardware queues.
> Running an elevator algorithm on NVMe adds CPU serialization overhead with zero mechanical benefit.
> Using `none` bypasses OS queuing and dispatches requests directly to hardware queues.

---

## 12. Key Takeaways

1. **Block devices** support random access (disks); **character devices** are sequential (terminals). **VFS** unifies their interface.
2. **DMA** offloads data transfer from the CPU; the CPU only sets up and handles the completion interrupt.
3. **Interrupt handling** is split into a fast top half (hardirq) and deferred bottom half (softirq/tasklet/workqueue).
4. **I/O schedulers** optimize disk access patterns: deadline for latency, BFQ for fairness, mq-deadline for SSDs/NVMe.
5. **VFS** abstracts filesystem differences through superblock/inode/dentry/file objects and function pointers.
6. **epoll** (Linux) and **kqueue** (macOS/BSD) provide O(1) I/O multiplexing for high-scale servers; **IOCP** is the Windows equivalent.
7. The **page cache** is critical for I/O performance; most reads are served from cached pages, not disk.

---

## 13. Linux Block Layer Internals

### The Multi-Queue Block Layer (blk-mq)

Modern Linux uses a two-level queue structure for high-throughput I/O:

```
Application
    |
    | write()/read() syscall
    v
+------------------+
|   VFS Layer      |  (file → inode → block address)
+------------------+
    |
    | bio (Block I/O) submission
    v
+------------------+
| Page Cache       |  (writeback: dirty pages → disk)
+------------------+
    |
    v
+---------------------------+
|   blk-mq: Block Layer    |
|                           |
|  Software Queues (per-CPU)|  ← One per CPU core
|  +----+----+----+----+   |
|  | Q0 | Q1 | Q2 | Q3 |   |
|  +----+----+----+----+   |
|          |               |
|  Hardware Queues (per-NCQ)|  ← One per device queue
|  +------+------+         |
|  | HQ 0 | HQ 1 |...     |
|  +------+------+         |
+---------------------------+
    |
    v
+------------------+
| Device Driver    |  (NVMe, SCSI, SATA)
+------------------+
    |
    v
  Physical Device
```

```bash
# View block device queue parameters
ls /sys/block/nvme0n1/queue/

# Queue depth (outstanding I/O requests)
cat /sys/block/nvme0n1/queue/nr_requests

# I/O scheduler
cat /sys/block/nvme0n1/queue/scheduler

# Number of hardware queues (NVMe)
cat /sys/block/nvme0n1/queue/nr_hw_queues

# Request merge behavior
cat /sys/block/nvme0n1/queue/nomerges     # 0=merge, 1=no merge

# Read-ahead (prefetch) size
cat /sys/block/nvme0n1/queue/read_ahead_kb

# Maximum sectors per request
cat /sys/block/nvme0n1/queue/max_sectors_kb

# Set I/O scheduler at runtime
echo mq-deadline > /sys/block/sda/queue/scheduler
echo none > /sys/block/nvme0n1/queue/scheduler  # Best for NVMe

# Tune read-ahead for sequential workloads
echo 4096 > /sys/block/sda/queue/read_ahead_kb  # 4MB read-ahead
```

### NVMe Queue Depth and Parallelism

NVMe supports up to 65,535 queues with 65,535 commands each.

```bash
# View NVMe device details
nvme list
nvme id-ctrl /dev/nvme0  | grep -E "sqes|cqes|mdts|aerl"

# Queue depth per namespace
cat /sys/class/nvme/nvme0/queue_count
cat /sys/class/nvme/nvme0n1/queue/nr_requests

# IO statistics per queue
cat /sys/block/nvme0n1/mq/*/cpu_list  # CPU → queue mapping
cat /sys/block/nvme0n1/mq/0/nr_tags  # Tags in use (= outstanding I/Os)

# Benchmark with various queue depths (fio)
fio --name=qd_test --filename=/dev/nvme0n1 \
    --rw=randread --bs=4k --direct=1 \
    --iodepth=1 --numjobs=1 --runtime=10 --time_based

fio --name=qd_test --filename=/dev/nvme0n1 \
    --rw=randread --bs=4k --direct=1 \
    --iodepth=128 --numjobs=8 --runtime=10 --time_based
```

### bio Structure: The Core I/O Unit

```c
/* Simplified view of struct bio (linux/bio.h) */
struct bio {
    struct block_device *bi_bdev;     /* Target block device */
    sector_t             bi_sector;   /* Starting sector */
    unsigned int         bi_size;     /* Total bytes remaining */
    unsigned short       bi_vcnt;     /* Number of bio_vecs */
    struct bio_vec      *bi_io_vec;   /* scatter-gather list */
    bio_end_io_t        *bi_end_io;   /* Completion callback */
    void                *bi_private;  /* Caller's private data */
    struct bvec_iter     bi_iter;     /* Current position */
};

struct bio_vec {
    struct page  *bv_page;    /* Physical page */
    unsigned int  bv_len;     /* Length in bytes */
    unsigned int  bv_offset;  /* Offset within page */
};

/* Submitting a bio (kernel space only) */
struct bio *bio = bio_alloc(GFP_KERNEL, nr_pages);
bio_set_dev(bio, bdev);
bio->bi_iter.bi_sector = sector;
bio->bi_end_io = my_completion_callback;
bio->bi_opf = REQ_OP_READ;
bio_add_page(bio, page, PAGE_SIZE, 0);
submit_bio(bio);
```

---

## 14. Zero-Copy I/O Techniques

### sendfile(2) - Kernel-Bypass File Transfer

Traditional approach (2 copies + 2 context switches per loop):
```
disk → kernel buffer → user buffer → kernel socket buffer → NIC
```

`sendfile()` approach (1 copy, stays in kernel):
```
disk → kernel buffer/page cache → NIC (DMA directly)
```

```c
#include <sys/sendfile.h>

/* Send file_fd contents to socket_fd - ZERO user-space copies! */
int file_fd  = open("/path/to/file", O_RDONLY);
int sock_fd  = /* connected socket */;
struct stat st;
fstat(file_fd, &st);

off_t offset = 0;
ssize_t sent = sendfile(sock_fd, file_fd, &offset, st.st_size);
/* offset is updated to position after last byte sent */
```

```bash
# Verify sendfile is being used (strace)
strace -e trace=sendfile64 nginx -g 'daemon off;'

# Check if sendfile is enabled in nginx
grep sendfile /etc/nginx/nginx.conf   # sendfile on;

# For large files, combine with tcp_nopush (cork then flush)
# nginx: sendfile on; tcp_nopush on; tcp_nodelay on;
```

### splice(2) - Pipe-Based Zero-Copy

`splice()` moves data between file descriptors using the pipe as an intermediate buffer, never touching user space.

```c
#include <fcntl.h>

/* Move data from file to socket via pipe - zero user copies */
int pipefd[2];
pipe(pipefd);

/* Splice file → pipe (kernel copy) */
ssize_t spliced = splice(file_fd, &offset,
                          pipefd[1], NULL,
                          65536, SPLICE_F_MOVE | SPLICE_F_MORE);

/* Splice pipe → socket (DMA from pipe buffer to NIC) */
splice(pipefd[0], NULL,
       sock_fd, NULL,
       spliced, SPLICE_F_MOVE);
```

### vmsplice(2) - User Buffer → Pipe (Donate Pages)

```c
/* Transfer ownership of user pages to kernel pipe buffer - avoids copy */
struct iovec iov = { .iov_base = user_buf, .iov_len = buf_size };
vmsplice(pipefd[1], &iov, 1, SPLICE_F_GIFT);
/* After GIFT: do NOT modify user_buf! Pages are owned by kernel. */
```

---

## 15. The Linux Page Cache Deep Dive

```bash
# View page cache usage
free -h
# Buffers = block device cache (metadata)
# Cache   = page cache (file data)

# Detailed memory breakdown
cat /proc/meminfo | grep -E "Cached|Buffers|Dirty|Writeback|SwapCached"

# Force write of dirty pages to disk
sync
echo 3 > /proc/sys/vm/drop_caches  # Drop all caches (TEST ONLY!)
# 1 = pagecache only, 2 = dentries+inodes, 3 = all

# View dirty ratio settings
cat /proc/sys/vm/dirty_ratio        # Max % of RAM that can be dirty
cat /proc/sys/vm/dirty_background_ratio  # % at which writeback starts
cat /proc/sys/vm/dirty_expire_centisecs  # How long pages stay dirty
cat /proc/sys/vm/dirty_writeback_centisecs  # Writeback thread interval

# Monitor writeback activity
cat /proc/vmstat | grep writeback
watch -n1 "cat /proc/vmstat | grep -E 'pgpg|pswp|dirty'"

# Per-file cache status (mincore syscall equivalent)
vmtouch /path/to/file   # Install: apt install vmtouch
vmtouch -l /path/to/file  # Lock pages in cache (like mlock)
vmtouch -e /path/to/file  # Evict pages from cache

# Understand page cache behavior with cachestat (BPF)
sudo bpftrace -e '
    kprobe:mark_page_accessed {
        @hits = count();
    }
    kprobe:add_to_page_cache_locked {
        @misses = count();
    }
    interval:s:1 {
        print(@hits);
        print(@misses);
        clear(@hits);
        clear(@misses);
    }'

# cachestat tool (Linux 6.5+ has built-in perf stat support)
perf stat -e page-faults,minor-faults,major-faults ./your_app
```

### Page Cache Write Modes

```c
/* Synchronous write - waits for disk acknowledgment */
int fd = open("file", O_WRONLY | O_SYNC);     /* Sync every write */
int fd = open("file", O_WRONLY | O_DSYNC);    /* Sync data only (not metadata) */
int fd = open("file", O_WRONLY | O_DIRECT);   /* Bypass page cache (O_DIRECT) */

/* fsync: flush buffered writes to disk */
write(fd, data, size);
fsync(fd);             /* Flushes data AND metadata (inode) */
fdatasync(fd);         /* Flushes data only (faster) */

/* msync: flush mmap'd region */
msync(mem, size, MS_SYNC);      /* Wait for completion */
msync(mem, size, MS_ASYNC);     /* Schedule, don't wait */
msync(mem, size, MS_INVALIDATE); /* Invalidate other mappings */

/* fadvise: hint kernel about access pattern */
posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);  /* Increase read-ahead */
posix_fadvise(fd, 0, 0, POSIX_FADV_RANDOM);       /* Disable read-ahead */
posix_fadvise(fd, 0, 0, POSIX_FADV_WILLNEED);     /* Prefetch now */
posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED);     /* Evict from cache */
posix_fadvise(fd, 0, 0, POSIX_FADV_NOREUSE);      /* Single-pass read */
```

---

## 16. Production I/O Tuning Reference

### Identify I/O Bottlenecks

```bash
# Real-time I/O monitoring
iostat -xz 1                    # Extended stats every 1 second
#   %util > 90% = device saturated
#   await > 10ms = high latency (HDD) or > 0.1ms (NVMe) = investigate

# Per-process I/O
iotop -o                        # Show only processes doing I/O
pidstat -d 1                    # I/O per process per second

# Block device statistics
cat /proc/diskstats             # Raw counters (fields: reads, merges, sectors, ms...)
# Or prettier:
cat /sys/block/sda/stat

# Trace I/O operations (BPF/eBPF)
sudo biolatency                 # Histogram of block I/O latencies (bcc-tools)
sudo biosnoop                   # Print every block I/O with PID, latency
sudo bitesize                   # Histogram of I/O sizes

# Detailed I/O trace
blktrace -d /dev/sda -o trace &
sleep 10
killall blktrace
blkparse trace.sda.blktrace.0 | head -50

# Analyze with blkparse and btt
blkparse -i trace -f "%D %2c %8s %5T.%9t %5p %2a %3d\n" | head -20
btt -i trace.sda.blktrace.0    # Shows: seek distance, merge rate, latency
```

### Filesystem-Level Tuning

```bash
# Mount options that affect I/O performance
mount -o noatime /dev/sda1 /mnt    # Don't update access time (huge win!)
mount -o nodiratime /dev/sda1 /mnt # Don't update dir access time
mount -o relatime /dev/sda1 /mnt   # Only update atime if older than mtime
mount -o data=writeback /dev/sda1 /mnt  # Ext4: best performance (less safe)
mount -o data=ordered /dev/sda1 /mnt    # Ext4: default (safe + fast)
mount -o barrier=0 /dev/sda1 /mnt      # Disable write barriers (DANGEROUS w/o UPS)

# View current mount options
cat /proc/mounts | grep " / "
findmnt /

# Ext4 tuning
tune2fs -l /dev/sda1 | grep -E "Block size|Journal|Features"
tune2fs -o journal_data_writeback /dev/sda1  # Change journal mode
e2fsck -f /dev/sda1              # Fsck before tuning

# XFS tuning (often preferred for large files)
xfs_info /dev/sda1
xfs_admin -l /dev/sda1

# BTRFS (copy-on-write; good for snapshots)
btrfs filesystem usage /
btrfs scrub start /              # Verify data integrity
btrfs balance start /            # Rebalance chunks
```

---

## 17. NVMe Protocol Internals

NVMe (Non-Volatile Memory Express) was designed from scratch for SSDs over PCIe - no SATA/SCSI legacy.

```
NVMe Architecture:

Host                        NVMe Controller
+------------------+        +------------------+
| Submission Queue | ──────► | SQ (ring buffer) |
| Completion Queue | ◄────── | CQ (ring buffer) |
+------------------+        +------------------+
                                    │
                            (PCIe DMA)
                                    │
                            +------------------+
                            | NAND Flash Array  |
                            | 4 channels × 8   |
                            | dies × 128 planes |
                            +------------------+

NVMe supports up to 65535 queues (vs. SATA's single queue)
Each queue: up to 65536 entries
Multiple queues → exploits parallelism in NAND flash

NVMe latencies:
  Sequential read:  ~100 μs (vs. HDD 5-10 ms = 50-100x faster)
  Random 4K read:   ~70-150 μs
  Write (consumer): ~100-200 μs
  Write (DC P4600): ~15-20 μs (enterprise SSD)

PCIe bandwidth:
  PCIe 3.0 x4:  3.5 GB/s
  PCIe 4.0 x4:  7.0 GB/s
  PCIe 5.0 x4: 14.0 GB/s
```

```bash
# NVMe management (nvme-cli)
sudo apt install nvme-cli

# List all NVMe drives
nvme list
# Node             SN                   Model                Namespace Usage                      Format           FW Rev
# /dev/nvme0n1     S4EUPNA...           Samsung 980 Pro      1         500.11 GB / 500.11 GB      512   B +  0 B   1B2QEXM7

# NVMe device info
nvme id-ctrl /dev/nvme0                  # Controller capabilities
nvme id-ns /dev/nvme0n1                  # Namespace info
nvme get-feature /dev/nvme0 -f 0x7      # Number of queues

# SMART health log
nvme smart-log /dev/nvme0
# critical_warning: 0       (0 = healthy)
# temperature: 35°C
# available_spare: 100%     (drops when flash wears out)
# percentage_used: 3%       (wear indicator)
# data_units_read: 5,234,567  (512B units)
# power_on_hours: 4,567

# NVMe namespace management
nvme list-ns /dev/nvme0        # List namespaces
nvme create-ns /dev/nvme0 --nsze=... --ncap=...  # Create namespace

# Queue depth and I/O scheduling
cat /sys/block/nvme0n1/queue/nr_requests   # Queue depth (default 1023)
echo 256 > /sys/block/nvme0n1/queue/nr_requests  # Reduce for latency

# I/O scheduler (nvme should use "none" - already has its own queueing)
cat /sys/block/nvme0n1/queue/scheduler
# [none] mq-deadline kyber
echo "none" > /sys/block/nvme0n1/queue/scheduler  # Best for NVMe

# NVMe write cache (volatile write cache)
nvme set-feature /dev/nvme0 -f 0x6 -v 1   # Enable volatile write cache
nvme get-feature /dev/nvme0 -f 0x6        # Query status
# fsync() flushes volatile write cache to flash

# NVMe-oF: NVMe over Fabrics (access remote NVMe via RDMA/TCP)
modprobe nvme-fabrics nvme-rdma nvme-tcp

# Connect to remote NVMe target
nvme connect -t tcp -a 192.168.1.100 -s 4420 -n nqn.2024.io:storage.nvme0

# NVMe perf benchmark
fio --name=randread --ioengine=libaio --iodepth=128 --rw=randread \
    --bs=4k --direct=1 --size=100G --numjobs=4 --filename=/dev/nvme0n1
# Expected: 500K-1M IOPS on modern NVMe
```

---

## 18. Advanced io_uring Patterns

io_uring (Linux 5.1+) is the most significant I/O innovation in Linux in decades - near-zero-syscall I/O.

```c
/*
 * io_uring: Shared ring buffer between kernel and userspace
 * Eliminates syscall overhead for I/O submission and completion
 *
 * Two ring buffers:
 *   SQ (Submission Queue): userspace writes I/O requests
 *   CQ (Completion Queue): kernel writes completions
 */
#include <liburing.h>

#define QUEUE_DEPTH 256

/* io_uring copy-file example */
int uring_copy_file(const char *src, const char *dst) {
    struct io_uring ring;
    struct io_uring_sqe *sqe;
    struct io_uring_cqe *cqe;

    /* Initialize ring with queue depth 256 */
    io_uring_queue_init(QUEUE_DEPTH, &ring, 0);

    int src_fd = open(src, O_RDONLY | O_DIRECT);
    int dst_fd = open(dst, O_WRONLY | O_CREAT | O_DIRECT, 0644);
    struct stat st;
    fstat(src_fd, &st);

    /* Allocate aligned buffer for O_DIRECT */
    void *buf;
    posix_memalign(&buf, 4096, 65536);  /* 64KB aligned buffer */

    /* Register buffers and files with kernel (avoid re-registration) */
    struct iovec iov = { .iov_base = buf, .iov_len = 65536 };
    io_uring_register_buffers(&ring, &iov, 1);

    int files[2] = {src_fd, dst_fd};
    io_uring_register_files(&ring, files, 2);

    off_t offset = 0;
    while (offset < st.st_size) {
        /* Queue READ */
        sqe = io_uring_get_sqe(&ring);
        io_uring_prep_read_fixed(sqe, 0 /* registered fd idx */, buf,
                                 65536, offset, 0 /* buf_idx */);
        sqe->user_data = 1;  /* Tag: read */
        io_uring_submit(&ring);

        /* Wait for read completion */
        io_uring_wait_cqe(&ring, &cqe);
        int bytes_read = cqe->res;
        io_uring_cqe_seen(&ring, cqe);

        /* Queue WRITE */
        sqe = io_uring_get_sqe(&ring);
        io_uring_prep_write_fixed(sqe, 1 /* dst fd idx */, buf,
                                  bytes_read, offset, 0);
        sqe->user_data = 2;  /* Tag: write */
        io_uring_submit(&ring);

        io_uring_wait_cqe(&ring, &cqe);
        io_uring_cqe_seen(&ring, cqe);

        offset += bytes_read;
    }

    io_uring_queue_exit(&ring);
    return 0;
}

/* Linked SQE: chain read → write atomically */
void uring_linked_rw(struct io_uring *ring, int src_fd, int dst_fd,
                     void *buf, size_t len, off_t offset) {
    /* SQE1: read */
    struct io_uring_sqe *sqe1 = io_uring_get_sqe(ring);
    io_uring_prep_read(sqe1, src_fd, buf, len, offset);
    sqe1->flags |= IOSQE_IO_LINK;  /* Link to next SQE */

    /* SQE2: write (runs after SQE1 completes) */
    struct io_uring_sqe *sqe2 = io_uring_get_sqe(ring);
    io_uring_prep_write(sqe2, dst_fd, buf, len, offset);
    /* No IOSQE_IO_LINK: end of chain */

    io_uring_submit(ring);  /* Submit both atomically */
}

/* FIXED FILES + BUFFERS: zero-copy file-to-file transfer */
void uring_splice(struct io_uring *ring, int pipe_fds[2],
                  int src_fd, int dst_fd, size_t len, off_t off) {
    /* src → pipe (zero-copy) */
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_splice(sqe, src_fd, off, pipe_fds[1], -1, len,
                         SPLICE_F_MOVE | SPLICE_F_MORE);
    sqe->flags |= IOSQE_IO_LINK;

    /* pipe → dst (zero-copy) */
    sqe = io_uring_get_sqe(ring);
    io_uring_prep_splice(sqe, pipe_fds[0], -1, dst_fd, off, len,
                         SPLICE_F_MOVE);

    io_uring_submit(ring);
}
```

```bash
# io_uring observability
# Check if io_uring is supported
cat /proc/sys/kernel/io_uring_disabled  # 0=enabled, 1=disabled for unpriv, 2=disabled

# Count io_uring operations per process
sudo bpftrace -e '
    tracepoint:io_uring:io_uring_submit_sqe {
        @ops[comm, args->opcode] = count();
    }
    interval:s:5 { print(@ops); clear(@ops); }'

# io_uring operation codes
# IORING_OP_READ=22, IORING_OP_WRITE=23
# IORING_OP_ACCEPT=13, IORING_OP_CONNECT=16
# IORING_OP_SEND=26, IORING_OP_RECV=27
# IORING_OP_TIMEOUT=11, IORING_OP_POLL_ADD=6

# Latency comparison: syscall vs io_uring
# On modern hardware (fio benchmark):
fio --name=randread --ioengine=io_uring --iodepth=64 \
    --rw=randread --bs=4k --direct=1 --size=10G
fio --name=randread --ioengine=libaio --iodepth=64 \
    --rw=randread --bs=4k --direct=1 --size=10G
# io_uring ~10-30% lower latency due to fewer syscalls
# At high IOPS: io_uring can eliminate syscalls entirely (SQPOLL mode)

# SQPOLL mode: kernel thread polls SQ (ZERO syscalls for submission!)
# Only use when submitting >100K IOPS (wastes a CPU core when idle)
fio --name=sqpoll --ioengine=io_uring --hipri=1 --sqthread_poll=1 \
    --iodepth=64 --rw=randread --bs=4k --direct=1 --size=10G
```

---

## 19. Storage Performance Benchmarking

```bash
# ── fio: The reference storage benchmarking tool ──

# Sequential read bandwidth (large files, streaming)
fio --name=seqread \
    --ioengine=libaio \
    --iodepth=64 \
    --rw=read \
    --bs=1M \
    --direct=1 \
    --size=10G \
    --numjobs=1 \
    --runtime=60 \
    --time_based

# Random 4K read IOPS (database workload)
fio --name=randread_4k \
    --ioengine=libaio \
    --iodepth=128 \
    --rw=randread \
    --bs=4k \
    --direct=1 \
    --size=10G \
    --numjobs=4 \
    --runtime=60 \
    --time_based \
    --lat_percentiles=1 \
    --percentile_list=50,90,99,99.9,99.99

# Mixed read/write (70/30 typical OLTP)
fio --name=oltp \
    --ioengine=libaio \
    --iodepth=32 \
    --rw=randrw \
    --rwmixread=70 \
    --bs=8k \
    --direct=1 \
    --size=10G \
    --numjobs=8

# Sync write latency (fsync cost - critical for databases)
fio --name=fsync_lat \
    --ioengine=sync \
    --rw=write \
    --bs=4k \
    --fdatasync=1 \     # fsync after each write
    --size=1G \
    --iodepth=1 \
    --numjobs=1
# Expected: 100-500μs NVMe, 5-10ms SSD, 5-15ms HDD

# ── dd: Quick bandwidth test ──
dd if=/dev/zero of=/tmp/testfile bs=1M count=1024 oflag=dsync
dd if=/tmp/testfile of=/dev/null bs=1M

# ── hdparm: Drive capabilities ──
sudo hdparm -I /dev/sda     # Drive info
sudo hdparm -t /dev/sda     # Read throughput (buffered)
sudo hdparm -T /dev/sda     # Read throughput (cached)
sudo hdparm -W /dev/sda     # Write cache status

# ── iostat: Runtime I/O monitoring ──
iostat -x 1                  # Extended stats every 1 second
# %util: 100% = device saturated
# await: average I/O wait time (ms) - should be <1ms NVMe, <10ms SSD
# r/s w/s: reads/writes per second
# rkB/s wkB/s: throughput

# ── blktrace: Block layer tracing ──
sudo blktrace -d /dev/sda -o trace &
# ... run workload ...
kill %1
blkparse trace.blktrace.0 | head -50

# ── ioping: I/O latency ping ──
ioping -R /dev/sda            # Random seek latency
ioping -RL /dev/sda           # Sequential read speed
ioping -c 100 /dev/sda        # 100 sequential reads

# ── bonnie++: Filesystem-level benchmark ──
bonnie++ -d /tmp -u nobody -r 512  # 512 MB file size
```

---

## 20. Windows I/O Completion Ports (IOCP)

IOCP is Windows' high-performance async I/O mechanism - equivalent to Linux epoll + thread pool combined.

```c
#include <windows.h>
#include <winsock2.h>

/*
 * IOCP Architecture:
 *  - Create completion port
 *  - Associate sockets/files with the port
 *  - Worker threads call GetQueuedCompletionStatus() to process completions
 *  - Kernel posts completed I/O to the port's queue
 */

/* Context for each async I/O operation */
typedef struct {
    OVERLAPPED overlapped;   /* MUST be first member */
    SOCKET     socket;
    char       buffer[4096];
    WSABUF     wsa_buf;
    DWORD      op_type;     /* OP_READ, OP_WRITE, OP_ACCEPT */
} IOContext;

/* Create IOCP: NumberOfConcurrentThreads = 0 → use CPU count */
HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

/* Worker thread: process completions */
DWORD WINAPI worker_thread(LPVOID param) {
    HANDLE port = (HANDLE)param;
    DWORD bytes_transferred;
    ULONG_PTR completion_key;
    OVERLAPPED *overlapped;

    while (1) {
        BOOL ok = GetQueuedCompletionStatus(
            port,
            &bytes_transferred,
            &completion_key,      /* Identifies which socket */
            &overlapped,          /* Identifies which I/O operation */
            INFINITE              /* Wait forever */
        );

        if (!ok) {
            if (!overlapped) break;  /* Shutdown signal */
            /* I/O error - overlapped is valid but operation failed */
            DWORD err = GetLastError();
            continue;
        }

        /* Cast back to our context */
        IOContext *ctx = CONTAINING_RECORD(overlapped, IOContext, overlapped);

        switch (ctx->op_type) {
        case OP_READ:
            /* Process received data */
            process_data(ctx->buffer, bytes_transferred);
            /* Post another async read */
            ZeroMemory(&ctx->overlapped, sizeof(OVERLAPPED));
            WSARecv(ctx->socket, &ctx->wsa_buf, 1, NULL, &flags,
                    &ctx->overlapped, NULL);
            break;
        case OP_WRITE:
            /* Write completed; free context or post next write */
            free(ctx);
            break;
        }
    }
    return 0;
}

/* Associate a socket with IOCP and start first async read */
void associate_socket(HANDLE iocp, SOCKET s) {
    /* Associate socket with port (completion_key = socket handle) */
    CreateIoCompletionPort((HANDLE)s, iocp, (ULONG_PTR)s, 0);

    IOContext *ctx = calloc(1, sizeof(*ctx));
    ctx->socket = s;
    ctx->wsa_buf.buf = ctx->buffer;
    ctx->wsa_buf.len = sizeof(ctx->buffer);
    ctx->op_type = OP_READ;

    DWORD flags = 0;
    WSARecv(s, &ctx->wsa_buf, 1, NULL, &flags, &ctx->overlapped, NULL);
    /* Returns ERROR_IO_PENDING: completion will be posted when data arrives */
}

/* Spin up worker threads (typically: CPU count or CPU count * 2) */
int nworkers = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
for (int i = 0; i < nworkers; i++) {
    HANDLE t = CreateThread(NULL, 0, worker_thread, iocp, 0, NULL);
    CloseHandle(t);
}
```

```powershell
# Windows I/O monitoring
# Perfmon: Disk I/O counters
Get-Counter '\PhysicalDisk(*)\Avg. Disk Queue Length'
Get-Counter '\PhysicalDisk(*)\Disk Transfers/sec'
Get-Counter '\PhysicalDisk(*)\Avg. Disk sec/Transfer'  # Latency (should be <10ms)

# WinSAT: Windows built-in storage benchmark
winsat disk                    # Runs built-in storage benchmark
winsat disk -drive c           # Specific drive
# Results: Sequential 64.0 KB   MB/s, Avg RT 0.10 ms

# CrystalDiskMark equivalent via PowerShell (using robocopy for throughput)
Measure-Command {
    Copy-Item C:\bigfile.iso D:\bigfile.iso
}

# DiskSpd (Microsoft's fio equivalent)
diskspd -b4K -d60 -o32 -t8 -r -w30 -L C:\testfile.dat
# -b: block size; -d: duration; -o: queue depth; -t: threads
# -r: random; -w30: 30% write

# Process I/O stats
Get-Process -Name myapp | Select-Object IO*
# IOReadBytes, IOWriteBytes, IOOtherBytes, IOReadOperationsCount

# File I/O monitoring with Process Monitor
# Filter: Include; Operation: ReadFile, WriteFile; Process: myapp
# Shows: path, result, bytes
```

---

## 21. macOS Storage Architecture and Darwin I/O Internals

### 21.1 The I/O Kit and DriverKit Architecture

The hardware abstraction and driver framework in macOS (Darwin / XNU) is the **I/O Kit**:
- Unlike the monolithic procedural C driver model of Linux, the I/O Kit is an object-oriented C++ framework embedded directly within the XNU kernel.
- The I/O Kit organizes all hardware controllers, buses, and logical partitions into a dynamic hierarchical graph called the **I/O Registry** (`IORegistry`).
- Core abstractions include:
  1. `IOService`: The base class representing any device, controller, or logical service.
  2. `IOMedia`: Represents an accessible storage medium (e.g., raw NVMe namespace or partition slice).
  3. `IOBlockStorageDriver`: Manages block request queues and translates generic block I/O requests into hardware bus commands.

To eliminate kernel panics caused by third-party drivers, modern Darwin replaces legacy Kernel Extensions (KEXTs) with **DriverKit**:
- Drivers run as sandboxed user-space processes called System Extensions.
- When an I/O interrupt occurs, the kernel routes events to the user-space DriverKit daemon via Mach messaging.
- If a DriverKit extension crashes, macOS restarts the driver without crashing the operating system.

```bash
# Inspect the live I/O Registry tree on Darwin
ioreg -l -w0 -p IOService | head -40

# Inspect active user-space DriverKit system extensions
systemextensionsctl list
```

### 21.2 Apple File System (APFS) Architecture

Since macOS 10.13 (High Sierra), the default storage layer is the **Apple File System (APFS)**, designed specifically for solid-state storage.
Key architectural features of APFS include:

1. **Space Sharing across Containers:**
Instead of partitioning physical disks into fixed-size block allocations, APFS creates an **APFS Container**.
Multiple independent volumes (e.g., System, Data, Recovery, Preboot) share the same underlying pool of free storage dynamically without repartitioning.

2. **Zero-Copy File Cloning (`clonefile`):**
APFS supports instantaneous Copy-on-Write cloning at the file system level:
- When a user duplicates a 50 GB virtual machine disk image, APFS does not copy data blocks.
- It creates a new directory entry referencing the existing block extents.
- Only when subsequent writes modify specific blocks does APFS allocate new blocks (Copy-on-Write).
- Programmers invoke this capability using the Darwin `clonefile()` system call:

```c
/* clonefile_demo.c - Zero-copy file cloning on APFS */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/attr.h>
#include <sys/clonefile.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <source_file> <clone_file>\n", argv[0]);
        return 1;
    }

    /* Perform instantaneous APFS extent cloning */
    if (clonefile(argv[1], argv[2], 0) != 0) {
        perror("clonefile failed");
        return 1;
    }

    printf("Successfully created zero-copy APFS clone: %s -> %s\n", argv[1], argv[2]);
    return 0;
}
```

```bash
# Compile and run clonefile on macOS
clang -Wall -Wextra clonefile_demo.c -o clonefile_demo
./clonefile_demo large_dataset.bin clone_dataset.bin
```

3. **Snapshots and Sealed System Volume (SSV):**
APFS supports instant, read-only point-in-time container snapshots.
macOS mounts the operating system from a cryptographically signed and sealed snapshot (`com.apple.os.update-...`), ensuring kernel binaries and system libraries cannot be modified by root malware.

### 21.3 macOS Storage and I/O Diagnostic CLI Tooling

Darwin provides specialized utilities to monitor disk performance, file system traps, and block latency:

```bash
# 1. diskutil: Comprehensive disk, partition, and APFS container management
diskutil list
diskutil apfs list

# 2. fs_usage: High-fidelity real-time file system call tracer
# Traces open, read, write, close, lookup, stat across all processes
sudo fs_usage -w -f filesys | grep -i "myapp"

# Filter only disk write operations
sudo fs_usage -w -f filesys | grep "WrData"

# 3. iostat: Disk transfer metrics per second
# Displays KB/transfer, transfers/sec (IOPS), and MB/s per disk slice
iostat -d -c 5 1

# 4. iosnoop: DTrace-based block I/O latency tracer
# Measures exact block request completion latency in microseconds
sudo iosnoop -d disk0

# 5. purge: Force disk cache flush and purge inactive disk cache frames
sudo purge
```

---

**Previous:** [P3L4: Synchronization Constructs](P3L4-Synchronization-Constructs.md)
**Next:** [P3L6: Virtualization](P3L6-Virtualization.md)



