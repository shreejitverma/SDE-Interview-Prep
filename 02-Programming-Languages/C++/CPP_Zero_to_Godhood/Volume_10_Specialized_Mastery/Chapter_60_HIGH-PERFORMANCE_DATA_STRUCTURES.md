# HIGH-PERFORMANCE DATA STRUCTURES


# HIGH-PERFORMANCE DATA STRUCTURES


When `std::unordered_map` is too slow, we descend into the hardware.

### 1. The Disruptor (Ring Buffer on Steroids)
A lock-free ring buffer designed for high-throughput messaging (LMAX Trading).
*   **Key Concept:** Pre-allocated memory, sequence numbers, and "barriers".
*   **False Sharing Prevention:** Padding sequence counters to 64 bytes (cache line).
*   **Batching:** Consumers process up to the known "published" sequence.

### 2. Swiss Table (Open Addressing + Metadata)
Used in `absl::flat_hash_map`.
*   **Structure:** Arrays of control bytes (metadata) and data slots.
*   **Control Byte:** 7 bits of hash + 1 bit for empty/deleted.
*   **SIMD Probing:** Load 16 control bytes into a vector register (SSE/AVX). Compare all 16 tags in parallel to find the slot.
*   **Result:** Drastically fewer cache misses than chaining.

### 3. Burst Tries / Judy Arrays
Cache-efficient digital trees (tries) for integer keys.
*   **Idea:** Nodes dynamically change type based on population (Linear list -> Bitmap -> Sub-trie).

### 4. Slot Map
O(1) insertion, deletion, and access with stable "handles" (indices) instead of pointers.
*   **Generational Indices:** Handle = `[Index | Generation]`. Prevents "Dangling Reference" equivalent (accessing a slot that was re-used for a new object).

---


---
