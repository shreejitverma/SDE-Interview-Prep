# Chapter 94: Safe Reclamation — Hazard Pointers and RCU

The data-structure logic of a lock-free stack or queue is the easy part; the hard part — the 80% — is answering *when is it safe to free a node another thread might still be reading?* A thread can load a pointer to a node a microsecond before another thread unlinks and frees it, then dereference freed memory. This chapter develops the two production answers to the reclamation problem, **hazard pointers** and **RCU**, plus epoch-based reclamation, with the cost model that decides between them. Get this wrong and your lock-free structure has a use-after-free; this is the chapter that makes Chapter 77's structures actually safe.

## Chapter Roadmap

- 94.1 The Reclamation Problem, Precisely
- 94.2 Why Reference Counting Isn't Enough
- 94.3 Hazard Pointers
- 94.4 RCU: Read-Copy-Update
- 94.5 Epoch-Based Reclamation
- 94.6 Choosing a Scheme

---

## 94.1 The Reclamation Problem, Precisely

In a lock-free structure, removal has two steps that cannot be made simultaneous with the readers: a node is **unlinked** (no longer reachable from the structure), then later **freed**. Between a reader loading a pointer and dereferencing it, another thread can unlink and free that node. The window is small but real, and under load it *will* occur.

```text
Reader:   p = head.load();        // got pointer to node X
                                  // <-- preempted here
Remover:  unlink X;  free(X);     // X is now freed memory
Reader:   use *p;                 // USE-AFTER-FREE: p points to freed X
```

> **Why this matters.** This is *the* defining hazard of pointer-based lock-free programming, and it is distinct from (though related to) the ABA problem of Chapter 93: ABA is a CAS succeeding wrongly; reclamation is a *dereference* of freed memory. With a mutex this never arises — the lock serialises removal against use. Lock-free code has no such serialisation, so it needs an explicit protocol that lets a remover know *no reader still holds a pointer* before it frees. Every safe lock-free structure embeds one of the schemes below; a lock-free structure without one is simply incorrect, regardless of how elegant its CAS logic looks.

---

## 94.2 Why Reference Counting Isn't Enough

The obvious fix — a per-node atomic reference count incremented before use, decremented after — has a fatal bootstrapping problem: to increment the count you must first dereference the node, but the node may be freed *before* you increment. You cannot safely touch the count of a node that may already be gone.

> **Why this matters / cost model.** This chicken-and-egg is why naive `shared_ptr`-per-node does not make a lock-free structure safe (and why `atomic<shared_ptr>`, which *does* solve it, must do so with a carefully-designed split reference count or a hidden lock, paying for it). Reference counting also puts a contended atomic RMW on *every traversal step* — catastrophic for a read-heavy structure where readers vastly outnumber writers. The schemes below are designed precisely to make the *read side* cheap (ideally no atomic RMW at all) while deferring the cost to the rare reclaimer.

---

## 94.3 Hazard Pointers

A **hazard pointer** is a single-writer, multi-reader slot in which a thread publishes the pointer it is *currently* dereferencing. Before freeing a node, a reclaimer scans all threads' hazard pointers; if any references the node, freeing is deferred.

```cpp
// Min standard: C++11. Conceptual sketch (production needs per-thread slots, retire lists).
#include <atomic>
#include <vector>

std::atomic<void*> hazard[MAX_THREADS];   // one hazard slot per thread (single writer each)

template <typename T>
T* protect(int tid, std::atomic<T*>& src) {
    T* p;
    do {
        p = src.load(std::memory_order_acquire);
        hazard[tid].store(p, std::memory_order_release);   // publish: "I'm using p"
    } while (p != src.load(std::memory_order_acquire));     // recheck: p still current?
    return p;                                               // safe to dereference p now
}

void retire(T* p, std::vector<T*>& retired) {
    retired.push_back(p);                                   // don't free yet
    if (retired.size() >= THRESHOLD) scan_and_free(retired);
}
void scan_and_free(std::vector<T*>& retired) {
    // Collect every thread's hazard pointer; free only retired nodes that NO hazard references.
    for (auto it = retired.begin(); it != retired.end(); /* ... */)
        if (!any_hazard_references(*it)) { delete *it; /* erase */ }
}
```
*Listing 94.1 — Hazard pointers: publish-then-recheck on the read side; scan-before-free on the reclaim side.*

> **Why this matters / cost model.** The read side is cheap-ish: a store (publish) and a recheck load — no atomic RMW, no contention with other readers. The reclaim side is O(retired × threads) per scan but amortised over the THRESHOLD batch. The genius is the **publish-then-recheck**: a reader publishes its pointer, then verifies the pointer is still installed; if a remover unlinked it in between, the recheck fails and the reader retries — guaranteeing that any node a reader will dereference is visible to the scanner before use. Hazard pointers bound memory usage (at most threads × hazards-per-thread nodes pending) and are the right choice when *unbounded* deferred reclamation (RCU's risk) is unacceptable. They are slated for standardisation (`std::hazard_pointer`). The cost: each protected pointer needs a hazard slot and a recheck, adding latency to every traversal step.

---

## 94.4 RCU: Read-Copy-Update

**RCU** inverts the cost: readers are nearly *free* (no atomic operations at all on the read side), at the cost of writers that must wait for a **grace period** before reclaiming. A reader brackets its access in a read-side critical section; a writer, after unlinking a node, waits until every reader that *could* have held a reference has finished (a grace period has elapsed), then frees.

```cpp
// Min standard: C++14. Conceptual (Linux kernel RCU / userspace liburcu semantics).
// READ side — extremely cheap, no atomics in classic RCU:
//   rcu_read_lock();
//   p = rcu_dereference(head);     // load with dependency ordering
//   use(*p);
//   rcu_read_unlock();
//
// WRITE side:
//   old = head;
//   rcu_assign_pointer(head, new_node);   // publish replacement (release)
//   synchronize_rcu();                    // BLOCK until all pre-existing readers finish
//   free(old);                            // now provably safe — no reader holds `old`
```
*Listing 94.2 — RCU: free read side, writer waits a grace period before reclaiming.*

> **Why this matters / cost model.** RCU's defining property is a **near-zero-cost read side** — in classic (quiescent-state) RCU, `rcu_read_lock` is a no-op or a cheap per-CPU counter bump, with no atomic RMW and no cache-line contention between readers. This makes it *the* technique for read-mostly data (routing tables, configuration, the Linux kernel's dcache) where readers outnumber writers by orders of magnitude: readers scale perfectly with cores. The cost is moved entirely to the writer, which must `synchronize_rcu()` — wait for a grace period (every CPU to pass through a quiescent state) — before freeing, which can take *milliseconds*. The grace period also means reclamation is **deferred and unbounded**: a stalled reader delays all reclamation, so memory can balloon under a slow reader. RCU is the right tool when reads dominate and writers can tolerate latency; it is wrong when memory must be bounded tightly or writes are frequent.

---

## 94.5 Epoch-Based Reclamation

**Epoch-based reclamation (EBR)** is a pragmatic middle ground widely used in user-space lock-free libraries (crossbeam, folly). A global **epoch** counter advances periodically; each thread announces the epoch it entered its critical section in. A retired node is tagged with the current epoch and freed only once all threads have advanced past it (typically two epochs later).

> **Why this matters / cost model.** EBR keeps the read side cheap (announce the epoch on entry — one relaxed store, no per-pointer publish like hazard pointers) while bounding reclamation lag to a couple of epochs rather than an open-ended grace period. It is simpler to implement than full RCU and gives better read-side performance than hazard pointers (no per-dereference recheck), which is why it dominates user-space lock-free libraries. Its weakness is the same as RCU's: a thread that enters a critical section and stalls *pins the epoch*, preventing reclamation globally and risking unbounded memory growth — so it assumes critical sections are short. Hazard pointers, by contrast, only pin the *specific* nodes a stalled thread holds.

---

## 94.6 Choosing a Scheme

| Scheme | Read-side cost | Reclamation bound | Best for |
|---|---|---|---|
| Reference counting (`atomic<shared_ptr>`) | Atomic RMW per step | Immediate | Simplicity; low traversal rates |
| Hazard pointers | Publish + recheck per pointer | Tightly bounded (threads × slots) | Bounded memory; mixed read/write |
| RCU | Near-zero (no atomics) | Unbounded (grace period) | Read-mostly; writers tolerate latency |
| Epoch-based (EBR) | One epoch store per section | Bounded to ~2 epochs | General lock-free libraries |

> **The discipline.** Reclamation is the part of lock-free programming that separates a working prototype from a correct system, and the part most engineers underestimate. The decision is driven by two questions: *How read-heavy is the workload?* (read-mostly → RCU/EBR; mixed → hazard pointers) and *How tightly must memory be bounded?* (tight → hazard pointers; tolerant → RCU/EBR). Above all, the practical recommendation stands: **use a vetted library** (folly, crossbeam-style EBR, liburcu, libcds) that has solved reclamation correctly, rather than hand-rolling it — the schemes here are subtle, their bugs are non-deterministic use-after-frees, and getting them right is genuinely hard. With reclamation handled, the volume turns from lock-free structures to the locks themselves — and when a well-designed lock beats going lock-free at all.
