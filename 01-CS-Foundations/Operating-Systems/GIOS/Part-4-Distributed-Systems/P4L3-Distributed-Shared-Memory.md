---
type: concept
track: [sde]
level:
status: solid
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P4L3"
  - "Memory Coherence in Shared Virtual Memory Systems, Li & Hudak (1989)"
  - "Distributed Shared Memory: Concepts and Systems, Nitzberg & Lo (1991)"
  - "The IVY Distributed Shared Memory System"
  - "TreadMarks: Shared Memory Computing on Networks of Workstations"
---

# P4L3: Distributed Shared Memory

## Table of Contents
1. [DSM Motivation and Concept](#1-dsm-motivation-and-concept)
2. [Hardware vs Software DSM](#2-hardware-vs-software-dsm)
3. [Consistency Models](#3-consistency-models)
4. [DSM Design Decisions](#4-dsm-design-decisions)
5. [Page-Based DSM: IVY](#5-page-based-dsm-ivy)
6. [TreadMarks: Lazy Release Consistency](#6-treadmarks-lazy-release-consistency)
7. [Modern DSM: RDMA-Based Approaches](#7-modern-dsm-rdma-based-approaches)
8. [Distributed Shared Memory in Practice](#8-distributed-shared-memory-in-practice)
9. [Message Passing vs Shared Memory](#9-message-passing-vs-shared-memory)
10. [Case Study: OpenSHMEM](#10-case-study-openshmem)
11. [Case Study: PGAS Languages](#11-case-study-pgas-languages)
12. [Performance and False Sharing](#12-performance-and-false-sharing)
13. [Quizzes and Exercises](#13-quizzes-and-exercises)
14. [Cross-Platform Distributed and Hardware Shared Memory: Linux, Windows, and Apple Silicon](#14-cross-platform-distributed-and-hardware-shared-memory-linux-windows-and-apple-silicon)

---

## 1. DSM Motivation and Concept

**Distributed Shared Memory (DSM)** gives processes on different machines the *illusion* of a single shared address space, enabling shared-memory programming models over a cluster of commodity machines.

```
WITHOUT DSM (Message Passing):
┌──────────────┐               ┌──────────────┐
│  Node A      │               │  Node B      │
│  int x = 5;  │               │ // no x here │
│              │  ──send(x)──► │  int x_copy; │
│              │               │  x_copy = 5; │
└──────────────┘               └──────────────┘
Programmer must explicitly move data

WITH DSM:
┌──────────────┐               ┌──────────────┐
│  Node A      │               │  Node B      │
│  x = 5;      │   ──────────► │  y = x + 1;  │
│              │  (automatic)  │              │
└──────────────┘               └──────────────┘
DSM runtime moves data transparently
```

### Why DSM?

1. **Programming ease**: Shared memory programs are easier than message-passing (no explicit sends/receives)
2. **Code reuse**: Existing SMP code can run on distributed systems with minimal changes
3. **Cost**: Build large "virtual SMP" from cheap commodity machines instead of expensive shared-memory machines

### Why is DSM Hard?

```
On a real SMP (shared memory machine):
  Coherence is maintained in hardware by the cache coherence protocol (MESI etc.)
  Latency: ~5ns for L1 cache, ~100ns for main memory

On a DSM cluster:
  Coherence must be maintained in software by the DSM runtime
  Latency: ~10,000ns (10µs) for local memory, ~100,000ns (100µs) network RTT

The gap is 3-4 orders of magnitude!
→ Frequent sharing is catastrophically expensive
→ DSM must minimize network communication
```

---

## 2. Hardware vs Software DSM

### Hardware DSM (NUMA machines)

High-end servers implement hardware DSM via interconnects like InfiniBand, HyperTransport, or Intel QPI/UPI:

```
┌─────────────────────────────────────────────────────────┐
│          NUMA MACHINE (Hardware DSM)                    │
│                                                         │
│  ┌──────────────────┐     QPI/UPI     ┌──────────────────┐
│  │  Socket 0        │◄──────────────►│  Socket 1        │
│  │  CPU0 CPU1       │                │  CPU2 CPU3       │
│  │  LLC (L3)        │                │  LLC (L3)        │
│  │  Local DRAM 32GB │                │  Local DRAM 32GB │
│  └──────────────────┘                └──────────────────┘
│
│  Accessing local DRAM: ~60ns
│  Accessing remote DRAM (via QPI): ~120ns (2x slower)
└─────────────────────────────────────────────────────────┘
```

```bash
# Linux NUMA awareness commands
numactl --hardware
# available: 2 nodes (0-1)
# node 0 cpus: 0 1 2 3 4 5 6 7
# node 0 size: 32768 MB
# node 0 free: 28196 MB
# node 1 cpus: 8 9 10 11 12 13 14 15
# node 1 size: 32768 MB
# node 1 free: 27500 MB
# node distances:
# node   0   1
#   0:  10  21
#   1:  21  10

# Run process on specific NUMA node
numactl --cpunodebind=0 --membind=0 ./myprogram

# Run with interleaved memory (for uniform access)
numactl --interleave=all ./myprogram

# Show NUMA memory statistics
numastat
numastat -m  # per-node memory info
numastat -p $(pgrep myprogram)  # per-process NUMA stats

# Prevent NUMA imbalance in kernel scheduler
echo 1 > /proc/sys/kernel/numa_balancing  # enable automatic NUMA balancing

# Check NUMA topology
lstopo  # graphical (requires hwloc)
lstopo --no-io --of ascii  # ASCII art topology diagram
```

### Software DSM

Software DSM runs entirely in user space, intercepting memory accesses via:
1. **OS page faults** (mprotect + SIGSEGV handler)
2. **Compiler instrumentation** (instrument every load/store)
3. **Hardware memory protection** (RDMA with remote memory registration)

```
Software DSM via Page Faults:

Node A                           Node B
┌────────────────────────┐       ┌────────────────────────┐
│  Virtual Address Space │       │  Virtual Address Space │
│  ┌──────────────────┐  │       │  ┌──────────────────┐  │
│  │  Page 0 (local)  │  │       │  │  Page 0 (remote) │  │
│  │  [data here]     │  │       │  │  [PROT_NONE]     │  │
│  └──────────────────┘  │       │  └────────┬─────────┘  │
│                        │       │           │ SIGSEGV!    │
│  Page 1 [PROT_NONE]    │       │  DSM handler fetches   │
│     │ SIGSEGV!         │       │  page from Node A      │
│  DSM handler:          │       └────────────────────────┘
│   1. Find owner        │◄──────────────────────────────
│   2. Request page      │  network: "give me page 1"
│   3. Update table      │──────────────────────────────►
│   4. mprotect read     │  "here is page 1 data"
│   5. Resume app        │
└────────────────────────┘
```

---

## 3. Consistency Models

### Sequential Consistency (SC)

The result of execution is as if all operations of all processors were executed in some sequential order, and the operations of each individual processor appear in this sequence in the order specified by its program.

```
Sequentially consistent VALID execution:
  P1: write x=1, write y=1
  P2: read y=1, read x=1

Sequentially consistent INVALID execution:
  P1: write x=1, write y=1
  P2: read y=1, read x=0   ← cannot see y=1 but not x=1!
```

### Release Consistency (RC)

Weaker model: guarantees only that all writes before a `release` are visible to any process that subsequently does an `acquire`.

```
Process 1:                    Process 2:
  x = 1                         acquire(lock)  ← must see all writes
  y = 1                             z = x + y  ← guaranteed: x=1, y=1
  release(lock) ──────────────►  release(lock)
```

### Lazy Release Consistency (LRC) - TreadMarks

LRC is even weaker: a process only receives updates from processes it **causally depends on**.

```
Process 1: write x=1, release L1
Process 2: write y=2, release L2
Process 3: acquire L1, acquire L2
           → only now gets x=1 AND y=2

If Process 3 only acquires L1:
           → only guaranteed to see x=1 (not y=2)
```

### Entry Consistency (EC)

Each shared variable is associated with a specific lock. You must acquire the lock to see the protected variable's updates.

```
lock_A protects: x, y
lock_B protects: z

P1: acquire A; x=1; y=2; release A
P2: acquire B; z=3;       release B
P3: acquire A;             ← sees x=1, y=2 (but not necessarily z=3)
```

### Consistency Model Comparison

```
STRONGEST                                              WEAKEST
     │                                                    │
     ▼                                                    ▼
Strict  →  Sequential  →  Causal  →  Release  →  Lazy RC  →  Eventual
 
Performance cost: HIGH                                      LOW
Programming ease: HIGH (most intuitive)                     LOW
Network traffic:  HIGH                                      LOW
```

---

## 4. DSM Design Decisions

### Page Size

```
Small pages (e.g., 4KB):
  PRO: Less false sharing (unrelated vars less likely on same page)
  CON: More page faults, higher page table overhead

Large pages (e.g., 4MB):
  PRO: Fewer page transfers, better spatial locality
  CON: More false sharing, wasted bandwidth

False Sharing:
  Thread 1 writes: x (at address 0x1000)
  Thread 2 writes: y (at address 0x1008)
  x and y are on the SAME 4KB page!
  → Each write invalidates the other thread's copy
  → Constant page ping-pong even with no logical sharing
```

### Replication vs Migration

```
MIGRATION (single copy):
  Only one node has the page at a time
  Read fault: fetch from owner, become new owner
  Write fault: same
  PRO: Simple, no consistency overhead
  CON: No read parallelism, reader-reader false sharing

REPLICATION (multiple copies):
  Multiple nodes can have read-only copies
  Write fault: invalidate all copies, become sole writer
  PRO: Read parallelism (multiple readers)
  CON: Invalidation protocol complexity

Most DSM systems use replication with invalidation:
  Multiple readers → parallel reads (no network)
  First writer     → invalidate all, get write access
```

### Granularity

| Granularity | Example | False Sharing | Overhead |
|-------------|---------|---------------|----------|
| Page (4KB) | IVY, TreadMarks | High | Low (OS-managed) |
| Object | Java DSM, CORBA | Medium | Medium |
| Variable | Compiler-instrumented | None | Very High |

---

## 5. Page-Based DSM: IVY

**IVY** (Integrated shared Virtual memory at Yale, Li & Hudak 1989) is the seminal page-based DSM system.

### IVY Protocol

Every page has:
- **Owner**: the node currently responsible for maintaining the page
- **Access rights**: None (no access), Read (read-only), Write (read-write)

**Read fault** (node N accesses page P, has no access):
```
Node N (no access)              Page Manager           Owner of P
     │                              │                      │
     │──── read fault ─────────────►│                      │
     │                              │── who owns page P? ──│
     │                              │                      │
     │                              │◄─ "I own page P" ────│
     │                              │── send read copy ───►│
     │                              │                      │◄─ send page ─│
     │◄─ page data ─────────────────│                      │
     │   (now has read access)      │                      │
```

**Write fault** (node N wants to write page P, has only read access):
```
Node N (read only)              Page Manager           All readers
     │                              │                      │
     │── write fault ──────────────►│                      │
     │                              │── invalidate all ───►│
     │                              │◄─ ack ───────────────│
     │                              │                      │
     │◄─ write access granted ──────│                      │
     │   (N is now owner/writer)    │                      │
```

### IVY Implementation Simulation

```c
/* ivy_dsm_sim.c - Simplified IVY protocol simulation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#define NUM_NODES 4
#define NUM_PAGES 16
#define PAGE_SIZE 4096

typedef enum { NONE, READ_ONLY, READ_WRITE } AccessRight;

typedef struct {
    int         owner;           /* node that currently owns the page */
    AccessRight rights[NUM_NODES]; /* access rights per node */
    int         copy_set[NUM_NODES]; /* nodes with read copies */
    int         copy_count;
    char        data[PAGE_SIZE]; /* simulated page data */
    pthread_mutex_t lock;
} PageEntry;

static PageEntry page_table[NUM_PAGES];

void dsm_init(void) {
    for (int i = 0; i < NUM_PAGES; i++) {
        page_table[i].owner = 0;  /* Node 0 initially owns all pages */
        page_table[i].copy_count = 0;
        memset(page_table[i].rights, NONE, sizeof(page_table[i].rights));
        page_table[i].rights[0] = READ_WRITE;  /* Owner has write access */
        pthread_mutex_init(&page_table[i].lock, NULL);
    }
}

/* Request read access to page */
int dsm_read_fault(int requesting_node, int page_id) {
    PageEntry *p = &page_table[page_id];
    pthread_mutex_lock(&p->lock);

    if (p->rights[requesting_node] >= READ_ONLY) {
        pthread_mutex_unlock(&p->lock);
        return 0; /* Already have access */
    }

    int owner = p->owner;
    printf("[READ FAULT] Node %d → Page %d (owned by Node %d)\n",
           requesting_node, page_id, owner);

    /* Fetch page from owner */
    /* In real system: network RPC to owner to get page data */
    printf("  Node %d fetches page %d data from Node %d\n",
           requesting_node, page_id, owner);

    /* Grant read access */
    p->rights[requesting_node] = READ_ONLY;
    p->copy_set[p->copy_count++] = requesting_node;

    pthread_mutex_unlock(&p->lock);
    return 0;
}

/* Request write access to page */
int dsm_write_fault(int requesting_node, int page_id) {
    PageEntry *p = &page_table[page_id];
    pthread_mutex_lock(&p->lock);

    if (p->rights[requesting_node] == READ_WRITE) {
        pthread_mutex_unlock(&p->lock);
        return 0; /* Already have write access */
    }

    int old_owner = p->owner;
    printf("[WRITE FAULT] Node %d → Page %d (owned by Node %d)\n",
           requesting_node, page_id, old_owner);

    /* Invalidate all read copies */
    for (int i = 0; i < p->copy_count; i++) {
        int reader = p->copy_set[i];
        if (reader != requesting_node) {
            printf("  Invalidating copy on Node %d\n", reader);
            p->rights[reader] = NONE;
        }
    }
    p->copy_count = 0;

    /* Transfer ownership */
    p->rights[old_owner] = NONE;  /* Old owner loses write access */
    p->owner = requesting_node;
    p->rights[requesting_node] = READ_WRITE;
    printf("  Node %d is now owner of page %d\n", requesting_node, page_id);

    pthread_mutex_unlock(&p->lock);
    return 0;
}

void print_page_state(int page_id) {
    PageEntry *p = &page_table[page_id];
    printf("Page %d: owner=Node%d, rights=[", page_id, p->owner);
    const char *names[] = {"NONE", "RO", "RW"};
    for (int i = 0; i < NUM_NODES; i++) {
        printf("N%d:%s%s", i, names[p->rights[i]],
               i < NUM_NODES-1 ? " " : "");
    }
    printf("]\n");
}

int main(void) {
    dsm_init();
    printf("=== IVY Protocol Simulation ===\n\n");

    print_page_state(0);

    printf("\n--- Node 1 reads Page 0 ---\n");
    dsm_read_fault(1, 0);
    print_page_state(0);

    printf("\n--- Node 2 reads Page 0 ---\n");
    dsm_read_fault(2, 0);
    print_page_state(0);

    printf("\n--- Node 3 writes Page 0 (triggers invalidation!) ---\n");
    dsm_write_fault(3, 0);
    print_page_state(0);

    printf("\n--- Node 1 reads Page 0 again (must re-fetch!) ---\n");
    dsm_read_fault(1, 0);
    print_page_state(0);

    return 0;
}
```

```bash
gcc -o ivy_sim ivy_dsm_sim.c -lpthread
./ivy_sim
# === IVY Protocol Simulation ===
# Page 0: owner=Node0, rights=[N0:RW N1:NONE N2:NONE N3:NONE]
# 
# --- Node 1 reads Page 0 ---
# [READ FAULT] Node 1 → Page 0 (owned by Node 0)
#   Node 1 fetches page 0 data from Node 0
# Page 0: owner=Node0, rights=[N0:RW N1:RO N2:NONE N3:NONE]
# ...
```

---

## 6. TreadMarks: Lazy Release Consistency

TreadMarks (Rice University, 1994) improved on IVY with:
1. **Multiple writers protocol**: multiple nodes can write the same page simultaneously (using diffs/write notices)
2. **Lazy Release Consistency**: updates only propagated at synchronization points
3. **Interval-based timestamps**: track causal dependencies without global time

### TreadMarks Key Concepts

```
INTERVAL: time between consecutive synchronization events on a process
VECTOR TIMESTAMP: N-dimensional vector tracking latest interval seen from each process
WRITE NOTICE: record that page was modified in an interval (sent lazily)

Example: Processes P1, P2, P3

P1: [interval 1]  write x=1  ──release──  [interval 2]
P2:                                 [acquire] ──→ P2 receives write notice for x
                                              P2's vector clock shows P1.interval=1

P3: [interval 1]  write y=2  ──release──  ...
    (P2 didn't synchronize with P3, so P2 does NOT get write notice for y)

Lazy: P2 only asks for diffs from processes whose write notices it hasn't seen yet.
```

### Twin & Diff Mechanism (Multiple Writers)

```
Before write:
  Page 0: [AAAA....AAAA]  (original content)
  DSM creates TWIN (copy): [AAAA....AAAA]

Node 1 writes to offsets 0-99:
  Page 0 now: [BBBB....AAAA]

At release, compute diff:
  diff = XOR(page, twin) = changes made by this process
  Send diff to other nodes at next sync point

Node 2 writes to offsets 100-199:
  Page 0: [BBBB....CCCC]
  diff = XOR(page, twin) = changes at 100-199

At synchronization, apply both diffs:
  Result: [BBBB....CCCC]  (both writes merged!)

This allows MULTIPLE CONCURRENT WRITERS to the same page,
unlike IVY which allows only one writer at a time.
```

---

## 7. Modern DSM: RDMA-Based Approaches

Modern DSM uses **RDMA (Remote Direct Memory Access)** to read/write remote memory without involving the remote CPU, achieving ~1-2µs latency.

### RDMA Overview

```
Traditional network read (software DSM):
  Local CPU → kernel → NIC → network → remote NIC → remote kernel → remote CPU
                                                      remote CPU reads memory
                                                      → reverse path to send data
  Latency: ~100µs, uses both CPUs

RDMA read:
  Local CPU → NIC (RDMA GET) → network → remote NIC → remote DRAM (direct!)
                                ← data ←
  Latency: ~1-2µs, remote CPU NOT involved!
```

```bash
# ── RDMA SETUP ON LINUX ────────────────────────────────────────────

# Install RDMA tools (InfiniBand)
sudo apt-get install rdma-core libibverbs-dev librdmacm-dev infiniband-diags

# Check RDMA devices
ibv_devinfo
# hca_id:	mlx5_0
#   transport:          InfiniBand (0)
#   fw_ver:             14.28.2006
#   node_guid:          e41d:2d03:004b:7960
#   phys_port_cnt:      1
#   port:               1
#     state:            PORT_ACTIVE (4)
#     max_mtu:          4096 (5)
#     active_mtu:       1024 (3)
#     link_layer:       InfiniBand

# Show port status
ibstat mlx5_0

# Basic RDMA bandwidth test
# Server:
ib_send_bw -d mlx5_0
# Client:
ib_send_bw -d mlx5_0 server-hostname

# RDMA latency test
ib_send_lat -d mlx5_0               # server
ib_send_lat -d mlx5_0 server-host   # client

# RDMA read/write bandwidth
ib_read_bw -d mlx5_0  # server
ib_read_bw -d mlx5_0 server-host  # client
```

```c
/* rdma_dsm_demo.c - RDMA-based DSM read/write demo */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rdma/rdma_cma.h>
#include <infiniband/verbs.h>

#define PORT 7471
#define BUFFER_SIZE (1 << 20)  /* 1 MB */

/* Server: register memory region and expose to clients */
typedef struct {
    struct rdma_cm_id       *cm_id;
    struct ibv_pd           *pd;
    struct ibv_mr           *mr;
    char                    *buf;
    uint64_t                 addr;   /* remote address */
    uint32_t                 rkey;   /* remote key for RDMA access */
} DSMContext;

int server_register_memory(DSMContext *ctx) {
    /* Allocate and register 1MB as remotely accessible */
    ctx->buf = malloc(BUFFER_SIZE);
    if (!ctx->buf) return -1;

    /* IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE: allow RDMA */
    ctx->mr = ibv_reg_mr(ctx->pd, ctx->buf, BUFFER_SIZE,
                          IBV_ACCESS_LOCAL_WRITE |
                          IBV_ACCESS_REMOTE_READ |
                          IBV_ACCESS_REMOTE_WRITE);
    if (!ctx->mr) {
        fprintf(stderr, "Failed to register MR\n");
        free(ctx->buf);
        return -1;
    }

    ctx->addr = (uint64_t)(uintptr_t)ctx->buf;
    ctx->rkey = ctx->mr->rkey;

    /* Exchange addr and rkey with client (via control channel) */
    printf("Memory region registered:\n");
    printf("  Address: 0x%lx\n", ctx->addr);
    printf("  Rkey:    0x%x\n", ctx->rkey);
    printf("  Size:    %d bytes\n", BUFFER_SIZE);

    /* Initialize with known data */
    memset(ctx->buf, 'A', BUFFER_SIZE);
    strncpy(ctx->buf, "Hello from server memory!", 25);

    return 0;
}

/* Client: RDMA READ remote memory without involving server CPU */
int client_rdma_read(struct ibv_qp *qp, struct ibv_mr *local_mr,
                     void *local_buf, uint64_t remote_addr, uint32_t rkey,
                     size_t length) {
    struct ibv_sge sge = {
        .addr   = (uint64_t)(uintptr_t)local_buf,
        .length = length,
        .lkey   = local_mr->lkey,
    };

    struct ibv_send_wr wr = {
        .opcode      = IBV_WR_RDMA_READ,  /* RDMA READ operation */
        .sg_list     = &sge,
        .num_sge     = 1,
        .send_flags  = IBV_SEND_SIGNALED,
        .wr.rdma = {
            .remote_addr = remote_addr,
            .rkey        = rkey,
        },
    };

    struct ibv_send_wr *bad_wr;
    int ret = ibv_post_send(qp, &wr, &bad_wr);
    if (ret) {
        fprintf(stderr, "RDMA READ post failed: %d\n", ret);
        return ret;
    }

    /* Poll completion queue for result */
    struct ibv_wc wc;
    while (ibv_poll_cq(qp->send_cq, 1, &wc) == 0);
    if (wc.status != IBV_WC_SUCCESS) {
        fprintf(stderr, "RDMA READ failed: %s\n", ibv_wc_status_str(wc.status));
        return -1;
    }

    printf("RDMA READ complete! Got: '%s'\n", (char *)local_buf);
    return 0;
}
```

### Disaggregated Memory (Modern Trend)

```
Traditional cluster:           Disaggregated Memory Cluster:
┌────────────────────┐         ┌──────────┐  ┌──────────┐
│  Node = CPU + RAM  │         │ Compute  │  │ Compute  │
│  Node = CPU + RAM  │    VS   │  Node A  │  │  Node B  │
│  Node = CPU + RAM  │         └────┬─────┘  └────┬─────┘
└────────────────────┘              │              │
                                    └─────┬────────┘
                                          │ RDMA fabric
                               ┌──────────▼──────────┐
                               │   Memory Pool        │
                               │   (CPU-less servers  │
                               │    with DRAM/PMEM)   │
                               └─────────────────────┘
Benefit: Memory is shared/allocated dynamically across all compute nodes
Systems: Liqid, Fungible, CXL (PCIe 5.0 Compute Express Link)
```

---

## 8. Distributed Shared Memory in Practice

### Memcached (Distributed Cache as DSM)

```bash
# Install memcached
sudo apt-get install memcached libmemcached-dev

# Configure
sudo tee /etc/memcached.conf << 'EOF'
-p 11211              # port
-l 0.0.0.0            # listen on all interfaces
-m 512                # max memory (MB)
-c 1024               # max connections
-t 4                  # threads
-I 10m                # max item size (10MB)
EOF

sudo systemctl start memcached

# Use with telnet (simple test)
telnet localhost 11211
set mykey 0 0 5
hello
get mykey
# VALUE mykey 0 5
# hello
delete mykey
quit

# Python client
pip install pymemcache

python3 << 'EOF'
from pymemcache.client.hash import HashClient
import time

# Connect to multiple memcached servers (distributed)
servers = [
    ('server1', 11211),
    ('server2', 11211),
    ('server3', 11211),
]
client = HashClient(servers)

# Set/get (like shared variable)
client.set('counter', 0, expire=3600)
val = client.get('counter')
print(f"counter = {val}")

# Atomic increment (like += on shared variable)
client.set('counter', 0)
for _ in range(1000):
    client.incr('counter', 1)

print(f"final counter = {client.get('counter')}")

# Benchmark throughput
start = time.perf_counter()
for i in range(10000):
    client.set(f'key_{i}', f'value_{i}')
for i in range(10000):
    client.get(f'key_{i}')
elapsed = time.perf_counter() - start
print(f"10K set + 10K get: {elapsed:.2f}s = {20000/elapsed:.0f} ops/sec")
EOF
```

### Redis as Distributed Shared Memory

```bash
# Install Redis
sudo apt-get install redis-server redis-tools

# Configure Redis cluster
sudo tee /etc/redis/redis.conf << 'EOF'
port 6379
bind 0.0.0.0
maxmemory 1gb
maxmemory-policy allkeys-lru
appendonly yes
appendfsync everysec
cluster-enabled no  # set to yes for cluster mode
EOF

sudo systemctl start redis-server

# Basic DSM operations
redis-cli

# String (simple shared variable)
SET x 42
GET x
INCR x         # Atomic increment
INCRBY x 10    # Add 10 atomically

# Hash (shared struct)
HSET myobj field1 val1 field2 val2
HGET myobj field1
HGETALL myobj

# List (shared queue)
RPUSH queue "task1" "task2" "task3"
LLEN queue
LPOP queue     # Pop from front (like dequeue)
RPOP queue     # Pop from back

# Pub/Sub (broadcast shared state changes)
# Terminal 1 (subscriber):
redis-cli SUBSCRIBE state_updates

# Terminal 2 (publisher):
redis-cli PUBLISH state_updates "x changed to 100"

# Atomic transactions (like critical section)
redis-cli MULTI
redis-cli SET x 1
redis-cli SET y 2
redis-cli EXEC  # Execute all atomically

# Lua scripting (complex atomic ops)
redis-cli EVAL "
    local x = redis.call('GET', KEYS[1])
    if tonumber(x) > tonumber(ARGV[1]) then
        redis.call('SET', KEYS[1], ARGV[1])
        return 'updated'
    end
    return 'no change'
" 1 mykey 100
```

```python
# redis_dsm_demo.py - Using Redis as distributed shared memory
import redis
import threading
import time

r = redis.Redis(host='localhost', port=6379, decode_responses=True)

def shared_counter_demo():
    """Demonstrate atomic operations on shared counter."""
    r.set('counter', 0)

    def increment_worker(worker_id, n):
        for _ in range(n):
            r.incr('counter')  # Atomic!

    threads = [threading.Thread(target=increment_worker, args=(i, 1000))
               for i in range(10)]
    for t in threads: t.start()
    for t in threads: t.join()

    final = int(r.get('counter'))
    print(f"10 threads × 1000 increments = {final} (expected 10000)")

def publish_subscribe_demo():
    """Simulate DSM cache invalidation via pub/sub."""
    local_cache = {}

    def cache_update_listener():
        pubsub = r.pubsub()
        pubsub.subscribe('cache_invalidations')
        for message in pubsub.listen():
            if message['type'] == 'message':
                key = message['data']
                if key in local_cache:
                    del local_cache[key]
                    print(f"Cache invalidated: {key}")

    # Start listener thread
    listener = threading.Thread(target=cache_update_listener, daemon=True)
    listener.start()

    # Simulate writes
    r.set('shared_x', 42)
    local_cache['shared_x'] = 42

    # Another "node" updates the value
    r.set('shared_x', 100)
    r.publish('cache_invalidations', 'shared_x')

    time.sleep(0.1)  # Allow listener to process
    print(f"local_cache has 'shared_x': {'shared_x' in local_cache}")  # False

shared_counter_demo()
publish_subscribe_demo()
```

---

## 9. Message Passing vs Shared Memory

### MPI (Message Passing Interface) - The Alternative to DSM

```bash
# Install MPI
sudo apt-get install openmpi-bin libopenmpi-dev

# MPI Hello World
cat > mpi_hello.c << 'EOF'
#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Hello from rank %d of %d\n", rank, size);
    MPI_Finalize();
    return 0;
}
EOF

mpicc -o mpi_hello mpi_hello.c
mpirun -np 4 ./mpi_hello
# Hello from rank 0 of 4
# Hello from rank 2 of 4
# Hello from rank 1 of 4
# Hello from rank 3 of 4
```

```c
/* mpi_vs_dsm.c - MPI point-to-point vs shared memory */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MPI equivalent of: shared_x = 42; (on rank 0 → rank 1) */
void mpi_shared_write_demo(int rank) {
    int shared_x = 0;

    if (rank == 0) {
        shared_x = 42;
        /* Explicit send (vs DSM: just assign x=42) */
        MPI_Send(&shared_x, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Rank 0: sent x=%d\n", shared_x);
    } else if (rank == 1) {
        /* Explicit receive (vs DSM: just read x) */
        MPI_Recv(&shared_x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank 1: received x=%d\n", shared_x);
    }
}

/* MPI collective: broadcast (like DSM write seen by all) */
void mpi_broadcast_demo(int rank) {
    int value = (rank == 0) ? 100 : 0;

    /* Root broadcasts to all */
    MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf("Rank %d: value = %d\n", rank, value);
}

/* MPI reduce: sum across all processes */
void mpi_reduce_demo(int rank) {
    int local_sum = rank * 10;  /* Each process has different value */
    int global_sum;

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Global sum = %d\n", global_sum);
    }
}

/* Bandwidth benchmark: MPI ping-pong */
void mpi_bandwidth_test(int rank) {
    const int N = 1024 * 1024;  /* 1MB */
    char *buf = malloc(N);
    memset(buf, 'X', N);

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    if (rank == 0) {
        for (int i = 0; i < 100; i++) {
            MPI_Send(buf, N, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buf, N, MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else if (rank == 1) {
        for (int i = 0; i < 100; i++) {
            MPI_Recv(buf, N, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(buf, N, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        }
    }

    double elapsed = MPI_Wtime() - t0;
    if (rank == 0) {
        double bw = (100.0 * 2 * N) / elapsed / (1024*1024);
        printf("Bandwidth: %.1f MB/s (RTT: %.3f ms)\n",
               bw, elapsed / 100 * 1000);
    }

    free(buf);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) printf("=== MPI Demos ===\n");

    printf("--- Point-to-Point ---\n");
    mpi_shared_write_demo(rank);
    MPI_Barrier(MPI_COMM_WORLD);

    printf("--- Broadcast ---\n");
    mpi_broadcast_demo(rank);
    MPI_Barrier(MPI_COMM_WORLD);

    printf("--- Reduce ---\n");
    mpi_reduce_demo(rank);
    MPI_Barrier(MPI_COMM_WORLD);

    printf("--- Bandwidth ---\n");
    if (rank < 2) mpi_bandwidth_test(rank);

    MPI_Finalize();
    return 0;
}
```

```bash
mpicc -O2 -o mpi_vs_dsm mpi_vs_dsm.c
mpirun -np 4 ./mpi_vs_dsm
```

---

## 10. Case Study: OpenSHMEM

OpenSHMEM is a standard API for Partitioned Global Address Space (PGAS) programming on clusters.

```bash
# Install OpenSHMEM (via OpenMPI)
sudo apt-get install libopenmpi-dev

# Or SOS (Sandia OpenSHMEM)
git clone https://github.com/Sandia-OpenSHMEM/SOS.git
cd SOS && ./autogen.sh && ./configure && make -j4 && sudo make install
```

```c
/* shmem_demo.c - OpenSHMEM distributed shared memory example */
#include <stdio.h>
#include <shmem.h>
#include <string.h>

int main(void) {
    shmem_init();

    int mype = shmem_my_pe();    /* My PE (process element) number */
    int npes = shmem_n_pes();    /* Total number of PEs */

    /* Allocate SYMMETRIC memory (accessible from all PEs via SHMEM) */
    long *shared_data = shmem_malloc(npes * sizeof(long));
    long *psum        = shmem_malloc(sizeof(long));

    /* Each PE writes its own value */
    shared_data[mype] = mype * 100;

    /* Barrier: wait for all PEs to write */
    shmem_barrier_all();

    /* PE 0 reads all values via SHMEM GET (no explicit message send!) */
    if (mype == 0) {
        long total = 0;
        for (int i = 0; i < npes; i++) {
            long val;
            /* Remote GET: reads PE i's shared_data[i] without PE i's involvement */
            shmem_long_g(&val, &shared_data[i], i);
            printf("PE 0 read from PE %d: %ld\n", i, val);
            total += val;
        }
        printf("Total: %ld\n", total);
    }

    shmem_barrier_all();

    /* Atomic operations (like hardware atomic on shared memory) */
    *psum = 0;
    shmem_barrier_all();

    /* All PEs atomically add to PE 0's psum */
    long my_contribution = mype + 1;
    shmem_long_atomic_add(psum, my_contribution, 0);

    shmem_barrier_all();

    if (mype == 0) {
        printf("Atomic sum: %ld (expected %ld)\n", *psum,
               (long)npes * (npes + 1) / 2);
    }

    /* Collective: all-to-all broadcast */
    long my_val = mype * 7;
    long *all_vals = shmem_malloc(npes * sizeof(long));
    shmem_collect32(all_vals, &my_val, 1, 0, 0, npes, shmem_team_world);

    shmem_free(shared_data);
    shmem_free(psum);
    shmem_free(all_vals);
    shmem_finalize();
    return 0;
}
```

```bash
oshcc -o shmem_demo shmem_demo.c
oshrun -np 4 ./shmem_demo
```

---

## 11. Case Study: PGAS Languages

```
PGAS (Partitioned Global Address Space) Languages:
- UPC (Unified Parallel C): C extension for PGAS
- Co-array Fortran: Fortran extension for distributed arrays
- Chapel: Modern PGAS language from Cray/HPE
- X10: IBM's PGAS language
```

### UPC (Unified Parallel C)

```c
/* upc_hello.upc - PGAS programming with UPC */
#include <upc.h>
#include <stdio.h>
#include <bupc_util.h>

/* Shared array: each element lives on one thread's memory,
   but ALL threads can access ALL elements */
shared int shared_arr[THREADS * 10];  /* THREADS = number of UPC threads */

/* Private to each thread */
int private_val;

int main(void) {
    /* THREADS: number of UPC threads (set at compile time or runtime) */
    /* MYTHREAD: this thread's index (0 to THREADS-1) */

    printf("Thread %d of %d\n", MYTHREAD, THREADS);

    /* Each thread writes to its own "partition" of shared_arr */
    for (int i = 0; i < 10; i++) {
        shared_arr[MYTHREAD * 10 + i] = MYTHREAD * 100 + i;
    }

    /* Barrier: all threads must reach here */
    upc_barrier;

    /* Thread 0 reads ALL values (remote accesses are transparent) */
    if (MYTHREAD == 0) {
        int sum = 0;
        for (int i = 0; i < THREADS * 10; i++) {
            sum += shared_arr[i];  /* May be a remote access! */
        }
        printf("Sum of all shared_arr = %d\n", sum);
    }

    /* Parallel loop: split work among threads */
    upc_forall(int i = 0; i < THREADS * 10; i++; &shared_arr[i]) {
        /* Each thread processes elements it "owns" (affinity) */
        shared_arr[i] *= 2;  /* Local access - fast! */
    }

    upc_barrier;
    return 0;
}
```

```bash
# Compile with Berkeley UPC
upc_cc -o upc_hello upc_hello.upc -DTHREADS=4

# Run
upcrun -n 4 ./upc_hello
```

---

## 12. Performance and False Sharing

### False Sharing Demo

```c
/* false_sharing.c - Demonstrate false sharing performance impact */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N_THREADS 4
#define ITERATIONS 100000000L

/* BAD: all counters on same cache line */
struct BadCounters {
    long c[N_THREADS];  /* 4 longs = 32 bytes = same cache line! */
} __attribute__((packed));

/* GOOD: each counter on its own cache line */
struct GoodCounter {
    long c;
    char padding[64 - sizeof(long)];  /* pad to 64 bytes = 1 cache line */
};
struct GoodCounters {
    struct GoodCounter cnt[N_THREADS];
};

static struct BadCounters  bad;
static struct GoodCounters good;

void *bad_worker(void *arg) {
    int id = *(int *)arg;
    for (long i = 0; i < ITERATIONS; i++)
        bad.c[id]++;  /* Writes to shared cache line → ping-pong! */
    return NULL;
}

void *good_worker(void *arg) {
    int id = *(int *)arg;
    for (long i = 0; i < ITERATIONS; i++)
        good.cnt[id].c++;  /* Each thread has its own cache line */
    return NULL;
}

double bench(void *(*worker)(void *)) {
    pthread_t tids[N_THREADS];
    int ids[N_THREADS];
    struct timespec t0, t1;

    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < N_THREADS; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, worker, &ids[i]);
    }
    for (int i = 0; i < N_THREADS; i++)
        pthread_join(tids[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    return (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
}

int main(void) {
    printf("=== False Sharing Benchmark ===\n");
    printf("Threads: %d, Iterations per thread: %ld\n\n", N_THREADS, ITERATIONS);

    double bad_time  = bench(bad_worker);
    double good_time = bench(good_worker);

    printf("BAD  (false sharing):   %.3f seconds\n", bad_time);
    printf("GOOD (padded):          %.3f seconds\n", good_time);
    printf("Speedup: %.1fx\n", bad_time / good_time);

    /* Verify results */
    long bad_sum = 0, good_sum = 0;
    for (int i = 0; i < N_THREADS; i++) {
        bad_sum  += bad.c[i];
        good_sum += good.cnt[i].c;
    }
    printf("Bad  total: %ld\n", bad_sum);
    printf("Good total: %ld\n", good_sum);
    return 0;
}
```

```bash
gcc -O2 -o false_sharing false_sharing.c -lpthread
./false_sharing
# === False Sharing Benchmark ===
# Threads: 4, Iterations per thread: 100000000
# 
# BAD  (false sharing):   3.847 seconds
# GOOD (padded):          0.412 seconds
# Speedup: 9.3x   ← 9x slower due to false sharing!

# Verify with perf
perf stat -e cache-misses,cache-references,LLC-load-misses ./false_sharing
```

---

## 13. Quizzes and Exercises

> [!question]
> **Quiz 1: Software vs. Hardware Responsibilities in Hybrid DSM (Clips 507-508)**
> In the classic survey paper *Distributed Shared Memory: Concepts and Systems* (Nitzberg and Lo), implementations are categorized as hardware-only, software-only, or hybrid hardware-software.
> In hybrid DSM systems, which of the following operations is predominantly implemented in software rather than specialized hardware?
> 1. Prefetching pages
> 2. Virtual-to-physical address translation
> 3. Triggering page/cache invalidations

> [!success]- Answer
> **Predominantly implemented in software: Option 1 (Prefetching pages).**
> 
> **Architectural Rationale:**
> - **Address Translation (Option 2):** Handled directly by the hardware Memory Management Unit (MMU) and Translation Lookaside Buffer (TLB).
> Incurring software traps on every address lookup would impose unacceptable execution overhead.
> - **Triggering Invalidations (Option 3):** Hardware directory controllers and snooping buses have well-defined, rigid states that can execute cache line invalidations at wire speed.
> - **Prefetching Pages (Option 1):** Determining *whether* and *what* to prefetch depends intimately on application-level access patterns, data structures, and algorithmic strides.
> Software runtimes and compilers have semantic awareness of application behavior that rigid hardware state machines lack.
> Therefore, hybrid DSM designs delegate heuristic page prefetching to software while relying on hardware for low-level memory protection and fast invalidation broadcasts.

---

> [!question]
> **Quiz 2: DSM Performance Metrics and Data Management Techniques (Clips 512-513)**
> If memory access latency is the primary performance optimization metric in a Distributed Shared Memory architecture, which of the following data management techniques are well-suited for your design?
> Select all that apply:
> 1. Migration
> 2. Caching
> 3. Replication

> [!success]- Answer
> **Well-suited techniques: Options 2 (Caching) and 3 (Replication).**
> 
> **Detailed Analysis:**
> - **Migration (Option 1):** In migration, when a node accesses a page residing on another node, the entire page is moved exclusively to the requesting node.
> While acceptable for strictly sequential single-reader/single-writer workloads, migration causes severe **ping-pong thrashing** in general multi-reader/multi-writer programs.
> Pages are repeatedly shuttled across the interconnect, drastically increasing latency.
> - **Caching (Option 2):** Caching retains copies of recently accessed pages in local RAM.
> Subsequent reads hit local physical memory at sub-microsecond speeds rather than incurring millisecond network round trips.
> - **Replication (Option 3):** Replication creates multiple concurrent read-only copies across different nodes.
> Any node possessing a replica reads locally without network delay, scaling aggregate read throughput across the cluster.
> 
> *Caveat:* When concurrent writes occur, caching and replication incur invalidation overhead.
> Just as Sprite DFS disabled caching during concurrent write-sharing, DSM protocols must trade off write-invalidation cost against read-caching benefits.

---

> [!question]
> **Quiz 3: Sequential Consistency Execution Analysis (Clips 524-525)**
> Consider an execution trace across two processors, $P_1$ and $P_2$, accessing shared memory locations $m_1$ and $m_3$ (initially all 0):
> 
> ```
> P1:  W(m1)x  ────────►  W(m3)y
> P2:               R(m1)x  ────────►  R(m3)y
> ```
> 
> Is this execution sequentially consistent? (Yes / No)
> Explain the formal condition.

> [!success]- Answer
> **Answer: Yes.**
> 
> **Formal Justification:**
> Lamport defined Sequential Consistency: the result of any execution is the same as if the operations of all processors were executed in some sequential order, and the operations of each individual processor appear in this sequence in the order specified by its program.
> In this trace:
> - $P_1$ program order: $W(m_1)x \rightarrow W(m_3)y$.
> - $P_2$ program order: $R(m_1)x \rightarrow R(m_3)y$.
> - Interleaved global sequence: $W(m_1)x \rightarrow R(m_1)x \rightarrow W(m_3)y \rightarrow R(m_3)y$.
> 
> Because all operations respect each processor's program order and $P_2$ observes the write to $m_3$ only after observing the write to $m_1$, the execution satisfies sequential consistency.
> If $P_2$ had read $R(m_3)y$ while earlier or concurrent reads to $m_1$ returned 0, sequential consistency would have been violated.

---

> [!question]
> **Quiz 4: Sequential vs. Causal Consistency Execution Analysis (Clips 526-527)**
> Consider an execution across four processors ($P_1, P_2, P_3, P_4$) accessing locations $m_1$ and $m_2$ (initially 0):
> 
> ```
> P1:  W(m1)x
> P2:  W(m2)y
> P3:               R(m1)x  ───────►  R(m2)y
> P4:               R(m2)y  ───────►  R(m1)x
> ```
> 
> 1. Is this execution sequentially consistent?
> 2. Is this execution causally consistent?

> [!success]- Answer
> **1. Sequentially Consistent: NO.**
> **2. Causally Consistent: YES.**
> 
> **Detailed Architectural Explanation:**
> - **Why it fails Sequential Consistency:**
> Sequential consistency mandates a single, globally agreed-upon total order of all writes visible to every processor.
> $P_3$ observes that $W(m_1)x$ occurred before $W(m_2)y$.
> However, $P_4$ observes that $W(m_2)y$ occurred before $W(m_1)x$.
> Because $P_3$ and $P_4$ observe mutually contradictory write orders, no valid single sequential interleaving exists.
> - **Why it satisfies Causal Consistency:**
> Causal consistency requires only that writes that are *causally related* must be seen in the same order by all processors.
> Writes that are concurrent (not causally related) may be observed in different orders by different processors.
> Here, $P_1$'s write to $m_1$ and $P_2$'s write to $m_2$ are independent and concurrent ($P_2$ did not read $m_1$ before writing to $m_2$).
> Because there is no causal relationship between $W(m_1)x$ and $W(m_2)y$, it is completely legal under causal consistency for $P_3$ and $P_4$ to observe them in opposite orders.

---

> [!question]
> **Quiz 5: Causally Dependent Writes Across Multiple Processors (Clips 528-529)**
> Consider the following sequence of operations across four processors:
> 
> ```
> P2:  W(m2)y
> P1:               R(m2)y  ───────►  W(m3)z
> P3:                                             R(m2)y  ───────►  R(m3)z
> P4:                                             R(m3)z  ───────►  R(m2)0
> ```
> 
> 1. Is this execution sequentially consistent?
> 2. Is this execution causally consistent?

> [!success]- Answer
> **1. Sequentially Consistent: NO.**
> **2. Causally Consistent: NO.**
> 
> **Detailed Architectural Explanation:**
> - **Causal Chain Formation:**
> $P_2$ writes $y$ to $m_2$.
> Next, $P_1$ reads $m_2$ and observes $y$.
> Subsequently, $P_1$ writes $z$ to $m_3$.
> Because $P_1$ observed $P_2$'s write before issuing its own write, $W(m_3)z$ is **causally dependent** on $W(m_2)y$ via Lamport's happens-before relation:
> $$W(m_2)y \longrightarrow R(m_2)y \longrightarrow W(m_3)z \implies W(m_2)y \longrightarrow W(m_3)z$$
> - **Evaluation on $P_3$:** $P_3$ reads $m_2=y$ and then $m_3=z$, which matches the causal ordering.
> - **Evaluation on $P_4$:** $P_4$ reads $m_3=z$ (observing the effect), but its subsequent read of $m_2$ returns 0 (it has not yet observed the cause).
> $P_4$ observes the consequence of an action without observing the action that caused it.
> This violates the fundamental definition of causal consistency.
> Because it violates causal consistency, it also automatically violates sequential consistency.

---

> [!question]
> **Quiz 6: Weak Consistency and Explicit Synchronization (Clips 530-533)**
> In a weak consistency model, memory accesses are categorized into ordinary read/write operations and synchronization operations (`Sync` / `Barrier` / `Lock`).
> 
> Scenario A: Process $P_1$ executes writes $W(m_1)x$ and $W(m_2)y$.
> Processes $P_2$ and $P_3$ read $m_1$ and $m_2$ in arbitrary, conflicting orders.
> Neither $P_1$, $P_2$, nor $P_3$ executes any synchronization operations.
> Is Scenario A weakly consistent?
> 
> Scenario B: If we remove the synchronization primitives entirely and evaluate Scenario A against causal consistency, is it causally consistent?

> [!success]- Answer
> **Scenario A (Weak Consistency): YES.**
> Weak consistency makes zero guarantees about the ordering or visibility of regular data reads and writes until an explicit synchronization operation is executed.
> Because no processor invoked `Sync`, the memory system is permitted to reorder, delay, or interleave reads and writes arbitrarily.
> 
> **Scenario B (Causal Consistency): NO.**
> Under causal consistency, all writes executed by the *same processor* are causally ordered by program order:
> $$W(m_1)x \longrightarrow W(m_2)y$$
> If $P_3$ observes $W(m_2)y$ while reading old data for $m_1$, it violates causal consistency because it observed the later write before the earlier causally ordered write from that same thread.

---

> [!question]
> **Quiz 7: IVY Page Fault Handling and Invalidation Protocol (Li and Hudak)**
> Describe how the IVY distributed shared memory system handles a **Write Fault** under the Dynamic Distributed Manager with Broadcast scheme.
> What happens to the page copy set, page ownership, and page access permissions across nodes?

> [!success]- Answer
> **IVY Write Fault Sequence:**
> 1. **Fault Interception:** Node $k$ attempts to write to a page currently marked read-only or invalid in its local page table.
> The CPU hardware MMU triggers a page protection fault, which is trapped by the IVY kernel signal handler (`SIGSEGV`).
> 2. **Manager Inquiry:** Node $k$ sends a message to the page manager querying the current page owner.
> 3. **Ownership Transfer and Invalidation:**
> - The manager sends an invalidation message to all nodes in the page's **copy set** (the list of all nodes holding read-only copies).
> - Every node in the copy set marks its local page table entry as invalid (`PROT_NONE`) and acknowledges invalidation.
> - The current owner transfers the latest page contents to Node $k$.
> 4. **New Owner Established:** Node $k$ becomes the exclusive owner of the page.
> The copy set is cleared to contain only Node $k$.
> 5. **Protection Upgrade:** Node $k$ updates its local page table permissions to read-write (`PROT_READ | PROT_WRITE`) and resumes the faulting instruction.

---

> [!question]
> **Quiz 8: False Sharing Mitigation in Page-Based DSM**
> Why is false sharing orders of magnitude more damaging in page-based DSM systems than in single-node symmetric multiprocessing (SMP) cache coherence?
> What programming technique eliminates false sharing in DSM data structures?

> [!success]- Answer
> **Damage Disparity:**
> - **In SMP Hardware:** Cache lines are small (typically 64 bytes).
> When false sharing occurs, cache line invalidations bounce across high-speed on-die interconnects or UPI/QPI links with latencies measured in nanoseconds ($~30-80\text{ ns}$).
> - **In Page-Based DSM:** Granularity is governed by the OS page size ($4\text{ KB}$ or $64\text{ KB}$).
> Two threads modifying independent variables that happen to reside within the same 4 KB virtual page cause the DSM subsystem to interpret the access as a write collision.
> The entire 4 KB page must be serialized, copied, transmitted across an IP network, and marked invalid via OS kernel page table modifications.
> Network latency ($100\ \mu\text{s} - 10\text{ ms}$) is $10^4$ to $10^5$ times slower than an SMP cache bus, causing catastrophic throughput collapse.
> 
> **Mitigation Technique:**
> - **Structure Padding:** Align and pad data structures so that variables modified by distinct nodes reside on separate virtual memory pages.
> - **Fine-Grained DSM / Diffing:** Transition from strict page invalidation (IVY) to twin-and-diff lazy release consistency (TreadMarks), where multiple nodes write to the same page concurrently, and diffs are merged at synchronization points.

---

## 14. Cross-Platform Distributed and Hardware Shared Memory: Linux, Windows, and Apple Silicon

### Linux: Kernel RDMA Subsystem and NUMA Memory Architecture

#### 1. Remote Direct Memory Access (RDMA) over Converged Ethernet (RoCE) and InfiniBand
Modern enterprise distributed shared memory relies on RDMA to bypass the OS kernel and remote CPU entirely.
Network Interface Cards (NICs) read and write host RAM directly using DMA over the network in under 1 microsecond.

```bash
# Verify RDMA hardware devices and active InfiniBand / RoCE ports
ibstat
ibv_devinfo -v

# Query registered RDMA link layers (InfiniBand vs Ethernet/RoCE)
rdma link show

# Check max locked memory limit (essential for RDMA memory registration ibv_reg_mr)
ulimit -l
# Production requirement in /etc/security/limits.conf:
# * soft memlock unlimited
# * hard memlock unlimited
```

#### 2. Linux NUMA (Non-Uniform Memory Access) Control and Diagnostics
On multi-socket Linux servers, physical memory is partitioned into NUMA nodes attached directly to specific CPU sockets.
Accessing remote socket memory incurs higher latency and lower bandwidth.

```bash
# Display hardware NUMA topology, socket assignments, and node distances
numactl --hardware

# Inspect per-node memory allocation hit/miss ratios
# numa_miss: allocated on another node because preferred node was full
# numa_foreign: intended for another node but allocated here
numastat

# Run an application bound strictly to NUMA Node 0 (CPUs and memory)
numactl --cpunodebind=0 --membind=0 ./memory_intensive_app

# Run application with interleaved memory allocation across all NUMA nodes
# (Evenly distributes memory bandwidth across all memory controllers)
numactl --interleave=all ./high_bandwidth_app
```

#### 3. Linux Kernel C NUMA Allocation (`mbind` and `set_mempolicy`)
```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <numa.h>
#include <numaif.h>
#include <sys/mman.h>

int main(void) {
    if (numa_available() < 0) {
        fprintf(stderr, "NUMA not available on this platform\n");
        return 1;
    }

    size_t size = 64 * 1024 * 1024; // 64 MB
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, 
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Bind memory region explicitly to NUMA Node 1
    unsigned long nodemask = (1UL << 1);
    if (mbind(ptr, size, MPOL_BIND, &nodemask, sizeof(nodemask) * 8, MPOL_MF_MOVE) < 0) {
        perror("mbind failed");
        return 1;
    }

    printf("Successfully bound 64MB buffer to NUMA Node 1\n");
    munmap(ptr, size);
    return 0;
}
```

---

### Windows: Windows RDMA (SMB Direct) and Win32 NUMA Architecture

#### 1. Windows RDMA Verification via PowerShell
Windows Server and Windows 11 Enterprise support SMB Direct over RDMA (iWARP and RoCE).

```powershell
# Query RDMA capability and operational status on all physical network adapters
Get-NetAdapterRdma

# Inspect SMB Direct network interfaces connected to storage fabric
Get-SmbServerNetworkInterface

# Monitor real-time RDMA performance counters
Get-Counter -Counter "\RDMA Activity(*)\*" -Continuous
```

#### 2. Win32 NUMA Node Memory Allocation (`VirtualAllocExNuma`)
Windows exposes explicit NUMA node scheduling and memory allocation APIs.

```c
#include <windows.h>
#include <stdio.h>

int main(void) {
    ULONG highestNodeNumber;
    if (!GetNumaHighestNodeNumber(&highestNodeNumber)) {
        printf("GetNumaHighestNodeNumber failed (%lu)\n", GetLastError());
        return 1;
    }
    printf("Highest NUMA Node Number: %lu\n", highestNodeNumber);

    // Allocate 16MB of physical RAM directly on NUMA Node 0
    SIZE_T allocationSize = 16 * 1024 * 1024;
    LPVOID pMemory = VirtualAllocExNuma(
        GetCurrentProcess(),
        NULL,
        allocationSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE,
        0 // Target NUMA Node 0
    );

    if (pMemory == NULL) {
        printf("VirtualAllocExNuma failed (%lu)\n", GetLastError());
        return 1;
    }

    printf("Allocated 16MB on NUMA Node 0 at address %p\n", pMemory);
    VirtualFree(pMemory, 0, MEM_RELEASE);
    return 0;
}
```

---

### macOS and Apple Silicon: Unified Memory Architecture (UMA)

#### 1. UMA Architectural Distinction vs. Distributed / NUMA Systems
Traditional high-performance systems use distributed memory or NUMA topologies:
- CPUs have local DDR memory.
- GPUs have discrete VRAM connected via PCIe buses.
- Moving data between CPU and GPU requires explicit DMA transfers over PCIe (bandwidth limited to $32-64\text{ GB/s}$).

Apple Silicon (M1/M2/M3/M4 series) rejects both NUMA and discrete GPU memory in favor of a **Unified Memory Architecture (UMA)**:
- A single physical pool of wide, high-frequency LPDDR5/LPDDR5X memory (up to $800+\text{ GB/s}$ bandwidth on M-series Ultra chips).
- The CPU Performance cores, CPU Efficiency cores, GPU execution cores, and the Apple Neural Engine (ANE) share direct physical access to the same memory addresses.
- Hardware cache coherency is maintained across CPU and GPU cores by the Apple Silicon system-level cache (SLC) and fabric arbiter.

#### 2. Zero-Copy Shared Memory in macOS Metal
In Apple Silicon, applications achieve true zero-copy processing between CPU threads and GPU shaders.
Data written by a CPU thread is immediately accessible by GPU kernels without memory copies or PCIe bus transit.

```metal
// Metal Shading Language (MSL) Kernel
#include <metal_stdlib>
using namespace metal;

kernel void process_shared_buffer(device float* data [[buffer(0)]],
                                  uint id [[thread_position_in_grid]]) {
    data[id] = data[id] * 2.0f;
}
```

```objc
// Objective-C / Darwin CPU Setup for Zero-Copy UMA
#import <Metal/Metal.h>
#include <stdio.h>

void execute_uma_pipeline() {
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    
    size_t count = 1000000;
    size_t size = count * sizeof(float);
    
    // MTLResourceStorageModeShared: Buffer is shared between CPU and GPU with no copies!
    id<MTLBuffer> sharedBuffer = [device newBufferWithLength:size 
                                                     options:MTLResourceStorageModeShared];
    
    // Direct CPU pointer access to the shared buffer
    float* cpuPtr = (float*)[sharedBuffer contents];
    for (size_t i = 0; i < count; i++) {
        cpuPtr[i] = (float)i;
    }
    
    printf("CPU initialized %zu elements directly in unified physical RAM.\n", count);
    // When GPU executes kernel, it reads the identical physical RAM addresses!
}
```

#### 3. macOS Memory and Hardware Topology Inspection
```zsh
# Inspect CPU core topology and cache levels on Apple Silicon
sysctl -a | grep -E "hw.perflevel|hw.l[1-3]|hw.memsize"

# Monitor real-time unified memory bandwidth and SoC power consumption
# Displays DRAM bandwidth consumption split between CPU, GPU, and Neural Engine
sudo powermetrics --samplers cpu_power,gpu_power,bandwidth -n 1

# Display macOS virtual memory page statistics and compression metrics
vm_stat
```

---

*Cross-links: [[P3L3-Inter-Process-Communication]] (shared memory on single node) | [[P3L2-Memory-Management]] (virtual memory, page faults) | [[P4L2-Distributed-File-Systems]] (distributed storage) | [[P2L2-Threads-and-Concurrency]] (synchronization)*

