---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: L07
tags: [cs6210, cs6210/practice]
---

# Practice L07

Original exam-style questions for [L07a](../Part-4-Distributed-Subsystems-and-Recovery/L07a-Global-Memory-Systems.md), [L07b](../Part-4-Distributed-Subsystems-and-Recovery/L07b-Distributed-Shared-Memory.md), [L07c](../Part-4-Distributed-Subsystems-and-Recovery/L07c-Distributed-File-Systems.md).
Each question names the coverage ids it exercises; answers are folded so this page works as a self-test.

## Global Memory Systems (L07a)

> [!question]- Q1. GMS uses cluster memory as a paging device for clean private pages. Why are shared pages excluded from global memory, and how does the boundary between local and global memory change when a node becomes idle? (concepts: L07a-01, L07a-02, L07a-03, P-GMS)
> GMS avoids running a coherence protocol, leaving the management of shared pages to the file system.
> Global memory stores only clean private pages.
> When a node becomes idle, its local pages age out and lose the global replacement contest.
> The local portion shrinks, and the global portion grows as the node accepts clean pages from other active nodes in the cluster.

> [!question]- Q2. Trace the movement of pages when node P faults on page X, which is currently in the global cache of node Q. Describe both Case 1 (P has at least one global frame) and Case 2 (P has no global frames). (concepts: L07a-04, L07a-05, L07a-06, L07a-07, L07a-13)
> In Case 1, P pulls page X into local memory and swaps any of its global pages to Q to take X's old frame.
> The local boundary on P grows by one, but Q's boundaries remain unchanged.
> In Case 2, P has no global frames to surrender.
> P exchanges its least-recently-used local page with X on Q.
> The boundaries on both P and Q remain unchanged.
> Neither case requires disk I/O, because the exchange is a logical swap implemented via getpage and putpage operations.

> [!question]- Q3. How does the initiator select the `MinAge` and the weight vector in GMS, and why is the node with the largest weight chosen as the next initiator? (concepts: L07a-08, L07a-09, L07a-10)
> At the start of an epoch, nodes send age summaries of their pages to the initiator.
> The initiator finds the M oldest pages globally, sets `MinAge` to the age of the youngest page in that set, and assigns each node a weight proportional to how many of those M pages it holds.
> The node with the largest weight holds the most pages destined for replacement.
> This node is chosen as the next initiator because it is the least active node and can use the arrival of forwarded pages to statistically determine when the epoch is over.

> [!question]- Q4. What are the roles of the Page Frame Directory (PFD), Global Cache Directory (GCD), and Page Ownership Directory (POD) when finding a page in GMS? (concepts: L07a-11, L07a-12)
> The PFD tracks the mapping from a global unique identifier (UID) to a physical frame on a single node.
> The GCD is a cluster-wide partitioned hash table mapping a UID to the IP address of the node caching the page.
> The POD is replicated on every node and maps the UID to the node that stores the GCD partition for that page.
> The OS implementation populates the referenced bit by patching the PALcode TLB miss handler, avoiding massive overhead.

## Distributed Shared Memory (L07b)

> [!question]- Q5. Compare the programming models of implicitly parallel programs, explicit message passing, and Distributed Shared Memory (DSM). Why does strict Sequential Consistency (SC) fail to scale well on clusters? (concepts: L07b-01, L07b-02, L07b-03, L07b-04)
> Implicitly parallel programs rely on compilers to distribute work, while explicit message passing requires programmers to manually send and receive data.
> DSM provides the familiar shared-state programming model using threads and locks without manual data movement.
> SC fails to scale because it generates a cache coherence network message on almost every shared memory access.
> Network latency is much higher than a local bus, so forcing immediate global visibility stalls processors and cripples scalability.

> [!question]- Q6. How does Release Consistency (RC) improve upon SC, and what distinguishes Lazy Release Consistency (LRC) from Eager Release Consistency? (concepts: L07b-05, L07b-06, P-TreadMarks)
> RC defers propagating updates to shared memory until a synchronization release operation occurs.
> This batches communication and overlaps it with computation.
> Eager RC pushes updates to all nodes caching the data as soon as a lock is released.
> LRC, as implemented in TreadMarks, pulls updates only when another node explicitly acquires the lock.
> LRC reduces network traffic further by avoiding broadcasts to nodes that never read the data again.

> [!question]- Q7. DSM typically uses page granularity, which can lead to false sharing. How does the multiple-writer coherence protocol use twins and diffs to prevent the ping-pong effect? (concepts: L07b-07, L07b-08, L07b-09)
> False sharing happens when two nodes modify independent variables on the same virtual memory page.
> A multiple-writer protocol allows concurrent writes without invalidating other copies.
> When a node first writes to a shared page, it creates a pristine copy called a twin.
> At synchronization time, the node compares the modified page against the twin to produce a compact diff.
> The system discards the twin and sends only the small diff over the network, perfectly isolating the concurrent changes.

> [!question]- Q8. What triggers garbage collection of diffs in TreadMarks, and how does structured DSM bypass the false sharing problem entirely? (concepts: L07b-10, L07b-11, L07b-12)
> Garbage collection in TreadMarks is triggered by a space metric, when pending diffs consume too much memory, or a time metric, when applying diffs takes too long.
> During garbage collection, accumulated diffs are applied to the master page, and cached copies are invalidated.
> Structured DSM manages coherence at the level of language constructs, such as objects or tuples, rather than OS pages.
> This avoids false sharing because the coherence unit exactly matches the logical data unit, keeping the computation-to-communication ratio favorable for scalability.

## Distributed File Systems (L07c)

> [!question]- Q9. Contrast the centralized architecture of NFS with the decentralized architecture of Coda. How does Coda address the single point of failure in NFS? (concepts: L07c-01, L07c-02, L07c-13, P-Coda)
> NFS routes all metadata lookups, cache misses, and disk writes through one central server, creating a severe bottleneck.
> Coda distributes data and control logic to provide high availability.
> Coda replicates servers into a Volume Storage Group.
> During network partitions, clients enter disconnected operation, working out of a local cache and logging modifications to a client modification log.
> Updates are optimistically reintegrated upon reconnection, eliminating the single point of failure.

> [!question]- Q10. How do Log-Structured File Systems (LFS) and software RAID combine to solve the small write problem, how does LFS differ from Journaling file systems, and why does xFS use stripe groups instead of Zebra's whole-cluster striping? (concepts: L07c-03, L07c-04, L07c-05, L07c-06, L07c-07)
> LFS buffers modifications in memory and writes them sequentially as a large contiguous log segment.
> Software RAID computes parity over this large buffer in software before striping it across the network.
> This avoids the expensive read-modify-write cycle typical of hardware RAID for small updates.
> Unlike Journaling file systems which maintain traditional in-place structures alongside a separate structural log, LFS writes all data only to the log and requires garbage collection.
> Zebra striped segments across all available servers, causing fragmentation on large clusters.
> xFS limits striping to specific subsets of servers called stripe groups to match client bandwidth and improve fault tolerance.

> [!question]- Q11. In the serverless network file system xFS, what are the roles of the Manager Map, File Directory, Imap, and Stripe Group Map? (concepts: L07c-08, L07c-09, L07c-10, L07c-11, P-xFS-Serverless-NFS)
> The Manager Map directs clients to the specific node managing metadata for a file index number.
> The File Directory maps human-readable file names to index numbers.
> The Imap translates the index number to the disk log address of its index node.
> The Stripe Group Map translates the segment identifier to the physical storage servers holding the data.
> Distributed log cleaning operates without a central server by having stripe group leaders coordinate garbage collection for their specific servers.

> [!question]- Q12. Trace a client read operation in xFS. If the local cache misses, what is the sequence of node communications required to fetch the data using cooperative client caching? (concepts: L07c-12)
> When a local cache miss occurs, the client consults the Manager Map and queries the file's manager.
> If the manager knows another client holds the data, the manager forwards the request to that peer.
> The peer then serves the data directly to the requesting client over the network.
> If no peer has the data, the manager traverses the Imap and Stripe Group Map to direct the client to read from the storage servers.
