---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
tags: [cs6210, cs6210/cheatsheet]
---

# Part 4 Cheat Sheet: Distributed Subsystems and Recovery

## Global Memory Systems (GMS)

GMS aggregates the idle memory of a workstation cluster to serve as a shared paging and file cache, avoiding slow mechanical disk I/O.

- **Age Information**: Pages are ranked globally by age (time since last access) to determine replacement candidates.
- **Epochs**: Time is divided into epochs; at each epoch boundary, nodes exchange age information to approximate the global state.
- **Initiator vs. Peer**: A node needing memory (initiator) pushes older pages to a peer node with idle memory instead of writing to disk.
- **Page Fault Handling**: On a fault, the initiator requests the page from the global directory; if found in a peer's memory, it is fetched over the network (faster than disk).
- **Epoch Arithmetic**: For $M$ nodes, a node receives forwards proportional to its weight fraction $\frac{w_i}{\sum w_k}$.
- **Hit vs Disk Latency**: A non-shared getpage hit avoids the disk bottleneck but incurs network routing, reply receipt, and target processing overheads.

## Distributed Shared Memory (DSM)

DSM provides the illusion of a shared address space across independent cluster nodes.

### Consistency Models

| Model | Rule | Network Overhead | Best Use Case |
| :--- | :--- | :--- | :--- |
| **Strict** | Absolute time ordering for all reads and writes. | Impossible | Theoretical baseline. |
| **Sequential** | Operations appear in some global sequential order consistent with program order. | Very High | Simple reasoning, high synchronization cost. |
| **Release (RC)** | Synchronization accesses are sequentially consistent; regular accesses are guaranteed visible only after an acquire or release. | Medium | Performance-critical DSMs (e.g., Munin). |
| **Lazy Release (LRC)** | Updates are pulled by the acquiring node rather than pushed by the releasing node. | Low | Reducing bandwidth in page-based DSMs (e.g., TreadMarks). |

### Twins, Diffs, and False Sharing

False sharing occurs when two nodes modify different variables on the same memory page.
A multiple-writer protocol solves this by allowing concurrent writes to the same page.

- **Twins**: Upon the first write, the DSM creates a pristine copy (the twin) of the page.
- **Diffs**: At synchronization time, the modified page is compared against the twin to generate a compact diff using Run-Length Encoding.
- **Network Cost**: Only the diffs are transmitted over the network, drastically minimizing bandwidth.
- **Diff Size Formula**: $\text{Diff Size} = \text{RLE Header} + \text{Payload}$.
- For example, an 8-byte header plus a 4-byte payload equals 12 bytes.
- **Diff Garbage Collection**: Periodically applies accumulated diffs to the master copy to reclaim memory.
- **GC Trigger Math**: $\text{Max Diffs Before GC} = \frac{\text{Space Limit}}{\text{Average Diff Size}}$.

## Distributed File Systems (DFS)

### xFS: Serverless Network File Systems

xFS completely decentralizes file system operations to eliminate single points of failure.

- **Log-Structured Striping**: Data and metadata are grouped into log segments and striped across multiple storage servers.
- **Stripe Groups**: The cluster is divided into smaller subsets of servers to prevent fragmentation and allow parallel operations.
- **Dynamic Manager Maps**: File metadata management is distributed dynamically among peers.
- **Cooperative Caching**: Clients check peer memory caches before fetching from disk.
- Consider $N$ servers in a stripe group with a segment size $S$.
- The data fragment size is $\frac{S}{N-1}$.
- The parity fragment size equals the data fragment size.
- Total data transferred over the network is $S + \text{Parity Size}$.
- The parity overhead fraction is $\frac{1}{N-1}$.

### Coda: Highly Available File Systems

Coda prioritizes high availability and mobility over strict consistency during network partitions.

- **Volume Storage Group (VSG)**: A replicated set of servers holding a file volume.
- **Accessible VSG (AVSG)**: The subset of servers currently reachable by the client.
- **Disconnected Operation**: When the AVSG is empty, the client operates entirely from its local cache.
- **Client Modification Log (CML)**: Writes during disconnected operation are logged locally.
- **Reintegration**: Upon reconnection, the CML is replayed to the servers.
- **Optimistic Replication**: Conflicting updates are allowed during partitions and resolved upon reintegration.

## Transactions and Recoverable Memory

### Lightweight Recoverable Virtual Memory (LRVM)

LRVM provides persistent data structures without the overhead of a full database management system.

- **User-Level Library**: Applications define transactional boundaries (begin_transaction, set_range, commit_transaction).
- **No Undo Logs**: Changes are staged in memory; aborted transactions are discarded before reaching the disk.
- **Redo Logs**: Committed changes are appended sequentially to an on-disk redo log to guarantee atomicity and durability.
- **Inter-Transaction Optimization**: Consecutive transactions modifying the same memory block coalesce their logs, keeping only the final payload.
- **Log Size Calculation**: $\text{Unoptimized Log Size} = N \times (\text{Tx Header} + \text{Range Header} + \text{Payload} + \text{Displacements})$.
- With optimization, the $N$ transactions coalesce into one payload.

### Rio Vista

Rio Vista eliminates synchronous disk writes for transaction commits by relying on reliable battery-backed memory.

- **Reliable Memory**: Volatile state survives power failures, rendering main memory persistent.
- **Free Transactions**: Transactions commit at the speed of memory access.
- **Performance**: LRVM incurs a massive disk penalty (e.g., 10,000 microseconds), RVM-Rio cuts out disk wait (500 microseconds), and Vista eliminates the log and syscalls entirely (5 microseconds).
- **Vista Overhead Math**: Base overhead (e.g., 5 microseconds) plus a linear scaling factor per KB beyond the base payload size.
- For example, the overhead is calculated as $5 + (\text{Payload KB} - 1) \times 17.9$ microseconds.

### Quicksilver

Quicksilver integrates distributed transaction recovery directly into the operating system IPC layer.

- **Unified Recovery**: The OS provides a single transaction manager that services files, memory, and IPC.
- **Hierarchical Commit Protocol**: Uses a transaction tree where nodes only communicate with their immediate superior and subordinates.
- **Two-Phase Commit (2PC)**: The coordinator requests votes down the tree, subordinates force logs and reply, and the coordinator broadcasts the commit decision.
- **Latency Calculation**: Total voting phase latency is the sum of all downward IPC/network hops, all local log forces at leaf nodes, and all upward reply hops.

### Comparison of Transactional Systems

| Feature | Quicksilver | TxOS | Percolator | LRVM |
| :--- | :--- | :--- | :--- | :--- |
| **Primary Goal** | Unified recovery in distributed OS | OS-level system call atomicity | Massive-scale incremental indexing | Lightweight persistence |
| **Transaction Scope** | Distributed across multiple nodes | Local OS resources and files | Distributed across Bigtable rows | Local application memory |
| **Isolation Level** | Serializable via locks | Strong isolation via buffers | Snapshot isolation | Application-managed |
