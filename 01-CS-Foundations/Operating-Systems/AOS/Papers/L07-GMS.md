---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/224056.224072"]
course: cs6210
lesson: L07
reading: required
venue: "SOSP 1995"
authors: ["Michael J. Feeley", "William E. Morgan", "Frederic H. Pighin", "Anna R. Karlin", "Henry M. Levy", "Chandramohan A. Thekkath"]
tags: [cs6210, cs6210/paper]
aliases: ["Implementing Global Memory Management in a Workstation Cluster", "GMS"]
---

# Implementing Global Memory Management in a Workstation Cluster

SOSP 1995. Reading status: required. [Link](https://doi.org/10.1145/224056.224072).

> [!abstract] One-line summary
> Put one cluster-wide page replacement policy under the OS, and use idle nodes' DRAM as a cache of clean pages so busy nodes stop rereading the disk.

## Problem

Workstation clusters in 1995 already had fast CPUs, large DRAMs, and a low-latency LAN, but each OS still treated its machine as an independent timesharing system.
A node under memory pressure wrote pages to disk while a neighbor sat idle with free frames.
Disk was the widening gap: a local hit is more than three orders of magnitude faster than a disk access, and a hit in another node's memory is only about two to ten times faster than disk.
Earlier systems either dedicated memory-server machines or let each client act alone (N-chance forwarding picks a random peer and recirculates singlets).
Neither made a replacement choice that was good for the faulting node and for the cluster at the same time, and neither covered anonymous VM pages, mapped files, and the file cache together.
The paper also wants nodes to join and leave, and it wants a crash to lose no unique data.

## Key idea

GMS splits each node's DRAM into local pages, used by that node's own processes, and global pages, clean private pages stored for someone else.
On a fault, the node looks in cluster memory before disk.
Time is divided into epochs of a few seconds.
An initiator collects age summaries, publishes the minimum age of the M oldest pages, and publishes a weight per node equal to how many of those pages that node holds.
A victim older than that minimum age is discarded.
A younger victim is forwarded to a node chosen with probability proportional to its weight, which is how the algorithm approximates global LRU without a synchronous cluster-wide clock.
The node with the largest weight, the one sitting on the most soon-to-be-dead pages, becomes the next initiator, and every node can compute that choice from the same message.
Only clean pages are placed in global memory, so the disk remains the stable copy.

## Design

Pages are private or shared, and local or global.
A shared page may be local on several nodes.
A global page is always private, and there is one global copy because a remote hit costs the same wherever it lives.
The four fault cases are the design.

1. X is global on Q and P still has a global frame: swap X with any global page on P. Q's counts do not change. No disk I/O.
2. X is global on Q and P is entirely local: exchange P's LRU local page with X. Both boundaries stay put. No disk I/O.
3. X is on disk: read it into P as a local page, and replace the oldest page in the cluster, writing that page out if it is dirty. If P has a global page, forward one. If not, forward P's LRU local page.
4. X is a shared page in Q's local memory: copy it, leave Q's copy, and replace the oldest cluster page the same way, because the extra copy consumed a frame.

The implementation on OSF/1 does not perform that swap inside the fault.
The fault allocates a free frame and does getpage.
The pageout daemon later does putpage.
Age of anonymous pages comes from a PALcode TLB handler that sets a referenced bit, sampled by a kernel thread, with a full TLB flush once a minute.
A 128-bit UID names a page by backing-node IP, disk partition, inode, and offset.
The page frame directory maps a UID to a local frame.
The global cache directory is a partitioned hash from UID to the node that caches the page.
The page ownership directory, replicated everywhere and updated only on membership changes, maps a UID to the node that holds the relevant GCD partition.
A private page's GCD entry lives on the node using it, so the common fault is one remote hop.
Nodes are assumed to trust each other.
The network is treated as reliable AN2 with flow control.
Global pages are clean, so a dead holder is handled by reading disk.
A master node redistributes the POD and GCD when membership changes.
That master is a single point of failure for joins, not for page contents.

## Evaluation

What they measured, on what hardware, and the two or three numbers worth remembering.

Hardware for the microbenchmarks is eight 225 MHz DEC 3000-700 Alphas running OSF/1 V3.2, on a 155 Mb/s DEC AN2 ATM network, with an 8 KB page.
Application runs use up to twenty machines, mixing 225 MHz DEC 3000 Model 700s and a 233 MHz AlphaStation.

- A non-shared getpage hit is 1440 microseconds. A miss that only proves the page is not cached is 15 microseconds, which is 0.1 to 0.4 percent of a 3.6 to 14.3 ms disk read.
- Random non-shared reads drop from 14.3 ms to 2.1 ms (about 6.8x). Sequential reads drop from 3.6 ms to 2.1 ms (41 percent).
- Memory-intensive applications (007, compilation, Render, a web query server) speed up by 1.5x to 3.5x once idle cluster memory passes about 200 MB, and the speedup stays roughly flat from 5 to 20 nodes when two of every five nodes are idle.
- Shifting which nodes are idle every 1 second still leaves the 007 benchmark at 1.9x, despite moving 70 MB. Seven clients on one idle node push 2880 transfers/s and 56 percent of that CPU (194 microseconds each).

Check the hit total: 61 + 156 + 8 + 1135 + 80 = 1440.
Check the idle CPU: 2880 x 194e-6 = 0.559, which is the reported 56 percent.
The shared-page hit column in the extracted text does not re-add cleanly to the printed total, so this note does not quote that column.

## Limitations and critiques

The machines trust one another.
A corrupt or malicious peer can be asked to hold pages, and the paper's answer is a single administrative domain, not a security mechanism.
Dirty pages are not placed in global memory until they have been written to disk, so GMS does not remove the write path.
The authors list replication of dirty pages as future work and note the data-loss risk.
The initiator and the membership master are not themselves recovered in the implementation, though the paper says an election is the obvious extension.
Age information is an approximation.
A short epoch or a sudden load shift can forward pages to a node that is no longer idle, which is why the 1-second shift experiment is in the paper.
The idle node's CPU becomes the bottleneck before the algorithm "runs out of ideas": 56 percent of a CPU at seven clients.
Global-page ages are boosted so they lose to local pages of similar age, which means the policy is not pure LRU.
It is LRU biased by the cost of a mistake.
N-chance forwarding matches GMS when idle memory is plentiful and uniform, so the global summary is not free complexity in every workload.
The network assumption (no dropped packets) is tied to AN2 flow control.

## What it led to

The idea that idle DRAM is a cache level survived the ATM cluster.
Cooperative caching in the xFS work (Dahlin and others are coauthors of both lines) is the file-system cousin.
User-level and in-kernel remote paging came back with RDMA: Infiniswap (NSDI 2017) harvests unused memory across machines, and Fastswap (EuroSys 2020) is a Linux swap backend with remote hits under 5 microseconds.
CXL pooling, as in Pond (ASPLOS 2023), changes the mechanism from getpage and putpage to load and store into a small pool, but the placement problem is the same one GMS posed: who is idle, and what does it cost to be wrong.
Linux zswap is the single-node version of "insert a level between DRAM and disk".
The paper's negative result also lasted: if there is no idle memory, turn the feature off.
MinAge of 0 is that switch.

## Exam angles

> [!question]- Q1. Node P has no global frames and faults on a page that is global on Q. What moves, and does anyone touch the disk?
> This is case 2.
> P's LRU local page is exchanged with Q's global page.
> Both nodes keep the same local and global counts, so both boundaries stay put.
> The disk is not read, because the page was already in cluster memory, and it is not written, because the victim becomes a clean global page on Q rather than being discarded as dirty.
> Case 1 would have required P to donate a global frame.

> [!question]- Q2. M = 1000. The oldest 1000 pages sit 100 on A, 200 on B, 700 on C. A fault on a busy node must evict a page younger than MinAge. Where does it go, and who is the next initiator?
> Weights are 100, 200, and 700, and they sum to M.
> The victim is forwarded, not discarded.
> The probability is 0.1 to A, 0.2 to B, and 0.7 to C.
> C has the largest weight, so C is the next initiator.
> C may also end the epoch after it has accepted 700 forwarded pages.
> If the summary had shown essentially no old pages, MinAge would be 0 and the victim would be discarded or written to disk instead.

> [!question]- Q3. A node holding global pages crashes. What data is lost, and what is only disrupted?
> No unique page data is lost, because every global page is clean and can be reread from the disk or the NFS server that backs its UID.
> What is disrupted is the age summary if the initiator died, and membership updates if the master died.
> A lookup during redistribution may miss and fall through to disk.
> That is the intended safe failure, not a coherence bug.
> Losing dirty data would require the extension the paper does not implement: forwarding a page before its disk write completes.

> [!question]- Q4. Why is a shared page never stored as a global page, and what does case 4 do instead?
> A global page is defined to be a single private copy, since any node can fetch it at the same remote cost.
> A shared page already has a live local copy on the node using it, and coherence belongs to the file system.
> Case 4 copies the page onto the faulting node, leaves the original, and pays for the extra frame by replacing the oldest page in the cluster.
> A later putpage that finds a duplicate already exists just drops the extra copy.

## Related

- Lesson note: [L07a Global Memory Systems](../Part-4-Distributed-Subsystems-and-Recovery/L07a-Global-Memory-Systems.md)
- Lab: [lab-14-global-memory](../labs/lab-14-global-memory/README.md)
- Sibling papers: [TreadMarks](L07-TreadMarks.md), [xFS](L07-xFS-Serverless-NFS.md), [Coda](L07-Coda.md)
