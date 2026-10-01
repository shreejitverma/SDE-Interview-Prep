---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: ["slides L07a; GMS paper"]
course: cs6210
part: 4
sub_lesson: L07a
lab: "[[labs/lab-14-global-memory/README|lab-14-global-memory]]"
papers: ["[[L07-GMS]]", "[[L07-TreadMarks]]", "[[L07-xFS-Serverless-NFS]]", "[[L07-Coda]]"]
tags: [cs6210, cs6210/lesson]
aliases: ["Global Memory Systems"]
---

# L07a Global Memory Systems

> [!summary] TL;DR
> To be written.

## Learning outcomes

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Motivation and the problem

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Core concepts

### Context for distributed subsystems

<!-- coverage: L07a-01 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Cluster memory as a paging device

<!-- coverage: L07a-02 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Local and global parts of memory

<!-- coverage: L07a-03 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Page fault case 1: page in global cache of another node

<!-- coverage: L07a-04 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Page fault case 2: no global part on the faulting node

<!-- coverage: L07a-05 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Page fault case 3: page on disk

<!-- coverage: L07a-06 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Page fault case 4: page actively shared

<!-- coverage: L07a-07 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Behavior with idle nodes

<!-- coverage: L07a-08 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Page age management and epochs

<!-- coverage: L07a-09 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Initiator selection and min-weight

<!-- coverage: L07a-10 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### GMS implementation in the OS

<!-- coverage: L07a-11 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Page frame directory, global cache directory, page ownership directory

<!-- coverage: L07a-12 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Putting a page into global memory

<!-- coverage: L07a-13 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Mechanisms step by step

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Worked examples

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Comparison

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Paper deep dives

- [Implementing Global Memory Management in a Workstation Cluster](../Papers/L07-GMS.md)
- [TreadMarks: Shared Memory Computing on Networks of Workstations](../Papers/L07-TreadMarks.md)
- [Serverless Network File Systems](../Papers/L07-xFS-Serverless-NFS.md)
- [Coda: A Highly Available File System for a Distributed Workstation Environment](../Papers/L07-Coda.md)

## Modern descendants

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Pitfalls and exam traps

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Practice

- [Practice L07](../Practice/Practice-L07.md)

## Lab

- [lab-14-global-memory](../labs/lab-14-global-memory/README.md): Global memory: cluster LRU with epochs and remote paging costs

## Further reading

> [!todo] Seed
> To be written; see the coverage matrix row for sources.
