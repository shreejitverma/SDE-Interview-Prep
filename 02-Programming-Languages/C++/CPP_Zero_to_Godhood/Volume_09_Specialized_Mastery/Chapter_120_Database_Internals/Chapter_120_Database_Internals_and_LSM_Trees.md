# Chapter 120: Database Internals and LSM-Trees

A storage engine is one of the most demanding things you can build in C++: it must be durable (survive crashes), concurrent (serve many readers and writers), and fast against storage that is orders of magnitude slower than RAM. The central insight of modern write-optimised databases — RocksDB, LevelDB, Cassandra — is to turn slow *random* writes into fast *sequential* ones, via the **Log-Structured Merge-tree (LSM-tree)**. This chapter builds the LSM-tree from that insight: the in-memory and on-disk structures, the compaction that maintains them, and the Bloom filters and memory-mapping that make reads fast.

## Chapter Roadmap

- 120.1 The Storage Cost Model: Sequential Beats Random
- 120.2 The LSM-Tree Architecture
- 120.3 The Write Path: MemTable and WAL
- 120.4 SSTables and Compaction
- 120.5 The Read Path: Bloom Filters and mmap
- 120.6 LSM vs B-Tree and the Engine Discipline

---

## 120.1 The Storage Cost Model: Sequential Beats Random

The defining fact of storage is that **random access is far slower than sequential access** — dramatically so on spinning disks (a seek is ~10 ms versus sequential throughput of hundreds of MB/s), and still significantly on SSDs (random writes cause write amplification and wear). A traditional B-tree updates data *in place*, which means a write to a random key is a random write to disk — the storage cost model's worst case.

> **Why this matters.** This is the storage analogue of the cache cost model (Chapter 87): just as a CPU vastly prefers sequential, prefetchable memory access over random pointer chasing, a storage device vastly prefers sequential writes over random ones. The B-tree's in-place update is "random writes," which bottlenecks on the device's IOPS. The LSM-tree's entire design is a *transformation* of random writes into sequential ones — it never updates in place; it always *appends*. The lesson echoes the whole book: performance comes from matching the access pattern to what the hardware does well, and for storage that means *sequential*.

---

## 120.2 The LSM-Tree Architecture

The **LSM-tree** achieves millions of writes per second by buffering writes in memory and flushing them to disk in large *sequential* batches, never updating existing data in place. It has three tiers:

- A **MemTable** — an in-memory sorted structure (a skip list or balanced tree) that absorbs all incoming writes at RAM speed.
- A **Write-Ahead Log (WAL)** — an append-only on-disk log that makes writes durable *before* they reach disk in sorted form.
- **SSTables** (Sorted String Tables) — immutable, sorted on-disk files, written sequentially when a MemTable fills.

> **Why this matters.** The architecture is a memory-buffer-plus-sequential-flush design — the same shape as a CPU write buffer (Chapter 76) or a batching I/O scheme (Chapter 98): absorb many small operations in fast memory, then commit them to slow storage in one efficient sequential batch. Writes hit the MemTable (fast RAM) and the WAL (a sequential append, fast), so the user-visible write latency is tiny; the expensive sorting-and-flushing to SSTables happens in the background. The key structural choice is **immutability**: SSTables, once written, are never modified — updates and deletes are *new* entries that shadow old ones, and the cleanup happens during compaction (§120.4). Immutability is what makes the structure concurrent-friendly (readers never see partial writes) and crash-safe.

---

## 120.3 The Write Path: MemTable and WAL

A write goes to *two* places: the WAL (for durability) and the MemTable (for query-ability). When the MemTable fills, it becomes **immutable**, a new MemTable takes over incoming writes, and the immutable one is flushed to a new SSTable in the background.

```text
write(key, value):
  1. append (key, value) to the WAL            # durable: survives a crash, sequential I/O
  2. insert (key, value) into the MemTable     # query-able: in-memory sorted structure
  -> return success (both are fast: a sequential append + an in-memory insert)

MemTable full:
  3. freeze it (immutable), start a new MemTable for incoming writes
  4. background: flush the immutable MemTable to a new SSTable (sequential write), then drop the WAL segment
```
*Listing 120.1 — The LSM write path: append to the WAL, insert into the MemTable, flush sequentially when full.*

> **Why this matters / cost model.** The WAL is the durability mechanism: because the MemTable is in volatile RAM, a crash would lose its contents — so every write is *first* appended to the on-disk WAL (a cheap sequential write), and on restart the MemTable is *reconstructed* by replaying the WAL. This is the classic durability pattern (write-ahead logging) that underpins essentially every database, and it is why writes are both *fast* (a sequential append, not a random in-place update) and *durable* (recoverable from the log). The MemTable's choice of structure matters: a **skip list** is popular because it supports sorted iteration (needed to flush in sorted order) and is amenable to lock-free concurrent insertion (Chapter 77) — letting many writers proceed without a global lock. The cost is *write amplification* deferred to compaction.

---

## 120.4 SSTables and Compaction

Over time, flushing produces *many* SSTables, and a key may have entries in several (an old value in one, a newer value or a deletion in another). **Compaction** is the background process that merges multiple SSTables into fewer, discarding shadowed (overwritten or deleted) entries.

> **Why this matters / cost model.** Compaction is the LSM-tree's central trade-off and its hardest engineering. Without it, reads would have to check an ever-growing number of SSTables, and deleted/overwritten data would never be reclaimed. Compaction merges sorted SSTables (a sequential merge — cheap I/O) and keeps only the latest version of each key, bounding the number of files a read must consult. The cost is **write amplification**: data is rewritten multiple times as it moves through compaction levels (a value written once may be rewritten several times over its life), consuming I/O bandwidth and SSD endurance. The choice of compaction strategy — *leveled* (lower write amplification, higher read performance, used by RocksDB) vs *size-tiered* (lower write amplification on writes, used by Cassandra) — tunes the read/write/space trade-off for the workload. Compaction is the price of turning random writes into sequential ones: you pay it in background I/O, not in user-visible write latency.

---

## 120.5 The Read Path: Bloom Filters and mmap

A read must find the *latest* value for a key, which may be in the MemTable or any SSTable — potentially checking many files. Two techniques make this fast:

- A **Bloom filter** per SSTable — a compact probabilistic structure that answers "is this key *possibly* in this file?" with no false negatives. If the Bloom filter says "no," the key is *definitely* not in that SSTable, so the read skips it entirely without touching disk.
- **Memory-mapped I/O** (`mmap`, Chapter 99) — mapping SSTable files into the address space so reads access the OS page cache directly, with no explicit `read` syscall or user-space copy.

```cpp
// Min standard: C++11. Conceptual read path with a Bloom-filter skip.
// value get(key):
//   if MemTable has key -> return it (newest)
//   for each SSTable, newest to oldest:
//       if (!sstable.bloom_filter.might_contain(key)) continue;   // SKIP: definitely not here
//       if (auto v = sstable.lookup(key)) return v;               // found the newest on-disk value
//   return not_found;
```
*Listing 120.2 — The read path: the Bloom filter lets a read skip SSTables that cannot contain the key.*

> **Why this matters / cost model.** The Bloom filter is the read-path hero: without it, a read might touch every SSTable (many disk accesses); with it, a read skips the SSTables that *cannot* contain the key (the common case for a key that lives in only one file), turning a potential N-file scan into one or two lookups. Its probabilistic nature (it may say "maybe" for a key that's absent — a false positive, costing an unnecessary lookup — but *never* "no" for a key that's present) is exactly the right trade: a small chance of wasted work to avoid a guaranteed disk access. `mmap` (Chapter 99) further removes the syscall-and-copy cost of reading SSTable data — though with the page-fault-latency caveat of Chapter 88. Together they make the read path of a write-optimised structure acceptably fast, addressing the LSM-tree's inherent weakness (reads must consider multiple files, unlike a B-tree's single path).

---

## 120.6 LSM vs B-Tree and the Engine Discipline

| Aspect | B-tree (in-place) | LSM-tree (append-only) |
|---|---|---|
| Writes | Random, in-place — IOPS-bound | Sequential batches — fast |
| Reads | One path — fast | Multiple files — Bloom-filtered |
| Write amplification | Low | Higher (compaction) |
| Space amplification | Low | Higher (shadowed data until compaction) |
| Best for | Read-heavy, point queries | Write-heavy, ingest-heavy |

> **The discipline.** A storage engine is the storage cost model made architecture: the LSM-tree exists because *sequential I/O beats random I/O*, and its every feature follows — the MemTable buffers writes in RAM, the WAL makes them durable with a sequential append, SSTables are immutable and written sequentially, compaction merges them sequentially in the background, and Bloom filters plus `mmap` keep reads fast despite the multi-file structure. The trade-off is explicit: the LSM-tree optimises *writes* at the cost of write amplification and read complexity, while a B-tree optimises *reads* and *point queries* at the cost of random-write performance — so the choice follows the workload (write-heavy ingest → LSM; read-heavy with updates → B-tree). Building one exercises nearly every discipline in this book: durability and crash-safety, lock-free concurrency (Chapter 77), memory-mapped and batched I/O (Chapters 98–99), cache-conscious structures (Chapter 109), and a hard-nosed cost model. The next chapter turns to a concern that cuts across every domain — security.
