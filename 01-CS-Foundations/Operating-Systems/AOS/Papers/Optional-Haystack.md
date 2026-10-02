---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://www.usenix.org/conference/osdi10/finding-needle-haystack-facebooks-photo-storage"]
course: cs6210
lesson: optional
reading: optional
venue: "OSDI 2010"
authors: ["Doug Beaver", "Sanjeev Kumar", "Harry C. Li", "Jason Sobel", "Peter Vajgel"]
tags: [cs6210, cs6210/paper]
aliases: ["Finding a Needle in Haystack: Facebook's Photo Storage"]
---

# Finding a Needle in Haystack: Facebook's Photo Storage

OSDI 2010. Reading status: optional. [Link](https://www.usenix.org/conference/osdi10/finding-needle-haystack-facebooks-photo-storage).

> [!abstract] One-line summary
> Haystack is an object storage system optimized for Facebook's massive photo collection that eliminates disk reads for metadata by packing multiple photos into large files, allowing all metadata to fit in main memory.

## Problem

Traditional POSIX filesystems perform poorly for Facebook's photo workload because storing each photo in a separate file requires multiple disk operations just to read the filesystem metadata before reading the actual image data.
The massive number of photos meant this metadata could not fit in the appliance's cache, making disk operations for metadata the throughput bottleneck.

## Key idea

The key idea is to dramatically reduce the per-photo filesystem metadata by storing multiple photos sequentially in very large volume files.
By keeping the metadata small (about 10 bytes per photo), the storage machines can cache all metadata in main memory, ensuring that reading a photo requires at most one disk operation.
This approach avoids the metadata bottleneck of POSIX filesystems where reading an inode requires a separate disk seek, optimizing for a write-once, read-often workload.

## Design

The Haystack architecture consists of a Directory, a Cache, and a Store.
The Directory maintains a mapping from logical volumes to physical volumes and handles load balancing.
The Cache acts as an internal content delivery network to absorb requests for highly popular, newly uploaded photos.
The Store manages the physical storage by organizing photos into large physical volumes.
Each physical volume is a large file composed of a superblock followed by a sequence of needles, where each needle contains the actual photo data and its minimal metadata.
An in-memory mapping allows the Store machine to calculate the exact offset and size of a needle without hitting the disk.

## Evaluation

Haystack provided a usable terabyte of storage for approximately 28 percent less cost than an equivalent terabyte on a traditional NAS appliance.
Furthermore, a Haystack terabyte processed about 4 times more reads per second than a NAS terabyte.

## Limitations and critiques

The design enforces an append-only write model, meaning photos cannot be overwritten in place.
Modifications require appending a new needle and updating mappings.
The system reclaims space from deleted photos only via a compaction process, which can temporarily consume significant I/O and disk space.

## What it led to

Haystack demonstrated that custom application-specific storage systems can significantly outperform generic POSIX filesystems for workloads like write-once-read-often media.
It influenced the design of subsequent large-scale blob storage systems in industry.

## Exam angles

<details>
<summary>Why did traditional network attached storage using NFS fail to scale for Facebook's photo serving workload?</summary>
NAS systems required multiple disk operations to read a single photo because they had to read directory metadata and the file's inode from disk before reading the photo itself.
The massive number of photos meant this metadata could not fit in the appliance's cache, making disk operations for metadata the throughput bottleneck.
</details>

<details>
<summary>How does Haystack guarantee that retrieving a photo requires at most one disk operation?</summary>
Haystack packs many photos into massive volume files, significantly reducing the metadata footprint per photo to about 10 bytes.
This allows the system to keep the entire index mapping photo IDs to volume offsets and sizes in main memory, enabling the system to read the photo data directly from disk in a single operation.
</details>

<details>
<summary>Explain the purpose of the index file in a Haystack Store machine.</summary>
The index file serves as a checkpoint of the in-memory data structures needed to locate needles in the volume file.
It allows a Store machine to quickly reconstruct its in-memory mappings upon restarting without having to scan the entire terabyte-sized volume file.
</details>

## Related

- Lessons: not covered in lectures (optional reading)
