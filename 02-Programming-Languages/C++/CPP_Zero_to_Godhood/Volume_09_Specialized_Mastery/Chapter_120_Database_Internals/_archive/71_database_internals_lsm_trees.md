# Chapter 71: Database Internals (LSM Trees)

# DATABASE INTERNALS (LSM TREES)

How RocksDB and LevelDB achieve millions of writes per second.

### 1. The Write Problem
Random writes to disk (B-Tree update) are slow (IOPS bottleneck). Sequential writes are fast.

### 2. LSM Tree (Log-Structured Merge Tree)
*   **MemTable:** In-memory sorted structure (SkipList or Red-Black Tree).
    *   Writes go here first (fast RAM access).
    *   WAL (Write Ahead Log) on disk for durability.
*   **Immutable MemTable:** When MemTable is full, it becomes immutable and is flushed to disk.
*   **SSTable (Sorted String Table):** The flushed file on disk. Key-Value pairs sorted by Key.
*   **Compaction:** Background process merges multiple SSTables, discarding overwritten/deleted keys (Leveled Compaction).

### 3. Bloom Filters
To read a key, we might have to check *all* SSTables. Slow!
*   **Optimization:** Each SSTable has a Bloom Filter.
*   **Check:** If Bloom says "No", key is definitely not in this file. Skip it.

### 4. Memory Mapped I/O (`mmap`)
Mapping the SSTable file directly into virtual address space. The OS manages paging.
*   **Benefits:** Zero-copy from disk cache to user space.
*   **Risks:** `SIGBUS` if file is truncated; lack of control over eviction.

