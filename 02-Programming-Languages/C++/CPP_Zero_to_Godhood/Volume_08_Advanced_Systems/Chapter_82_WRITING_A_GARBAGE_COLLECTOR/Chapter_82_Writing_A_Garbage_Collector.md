# Chapter 82: Writing a Garbage Collector

C++ deliberately has no garbage collector — it gives you RAII and deterministic destruction instead — yet building one teaches exactly why that choice was made, and equips you to implement GC where it genuinely belongs (a scripting engine embedded in a C++ host, a managed runtime, a graph with cycles RAII cannot break). This chapter implements mark-and-sweep collection, surveys reference counting and tri-colour concurrent collection, and quantifies the cost model — pause times, throughput, memory overhead — that makes GC the wrong default for the latency-critical systems the rest of this volume targets.

## Chapter Roadmap

- 82.1 Why a GC in a Language That Has RAII
- 82.2 Reachability: Roots and the Object Graph
- 82.3 Mark-and-Sweep
- 82.4 Reference Counting and Its Cycle Problem
- 82.5 Tri-Colour Marking and Concurrent Collection
- 82.6 Conservative vs Precise Collection
- 82.7 The Cost Model: Why C++ Chose RAII

---

## 82.1 Why a GC in a Language That Has RAII

C++ manages memory through **RAII**: an object's destructor runs deterministically when it leaves scope, releasing whatever it owns. `std::unique_ptr` and `std::shared_ptr` extend this to the heap. This is the opposite philosophy to garbage collection, which reclaims memory *non-deterministically* at times the runtime chooses.

So why implement a GC? Three legitimate reasons:

1. **Embedding a managed language.** A Lua/JavaScript/Python interpreter written in C++ needs a GC for *its* objects, because scripts create cyclic, dynamically-typed object graphs that RAII cannot express.
2. **Cyclic data structures.** `shared_ptr` leaks on reference cycles (§82.4); a graph with back-edges may need a collector or manual cycle-breaking.
3. **Understanding the trade-off.** Building mark-and-sweep makes the costs of GC — pauses, throughput overhead, memory bloat — concrete, which is the best argument for why C++ defaults to RAII.

> **Why this matters.** "Manual memory management" in modern C++ is mostly automatic *and* deterministic via RAII — you get the convenience of automatic reclamation without the pause times of GC. The reason this volume's latency-critical chapters never reach for a GC is precisely the cost model developed in §82.7. But when you *are* writing a runtime that hosts a managed language, you need this chapter.

---

## 82.2 Reachability: Roots and the Object Graph

Garbage collection rests on one definition: an object is **live** if it is *reachable* from a **root**, and **garbage** otherwise. Roots are the pointers the program can access directly without going through the heap:

- Local variables and parameters on the **stack**.
- **Global** and static pointers.
- Pointers in **CPU registers**.

From the roots, the collector traverses the **object graph** — following every pointer field of every reachable object — to find the full set of live objects. Everything not reached is unreachable and may be freed.

> **Why this matters.** The hard part of GC in C++ is *finding the roots and the pointer fields*. A managed language's runtime knows its object layout exactly (which fields are pointers), so it can do **precise** collection. A collector bolted onto arbitrary C++ cannot know which stack words are pointers and which are integers that happen to look like addresses — hence **conservative** collection (§82.6). Reachability is simple; knowing where the pointers are is the engineering.

---

## 82.3 Mark-and-Sweep

The classic algorithm has two phases. **Mark:** starting from the roots, traverse the object graph and set a `marked` flag on every reachable object. **Sweep:** iterate the entire heap, free every unmarked object, and clear the marks for the next cycle.

```cpp
// Min standard: C++17. Illustrative managed-runtime GC (not for arbitrary C++).
#include <vector>
#include <algorithm>

struct GCObject {
    bool marked = false;
    virtual void trace(class VM&) {}   // override to mark children
    virtual ~GCObject() = default;
};

class VM {
    std::vector<GCObject*> heap_;     // every allocated object
    std::vector<GCObject*> roots_;    // pointers currently reachable directly
public:
    GCObject* track(GCObject* o) { heap_.push_back(o); return o; }
    void add_root(GCObject* o)   { roots_.push_back(o); }

    void mark() {
        for (auto* o : roots_) mark_object(o);
    }
    void mark_object(GCObject* o) {
        if (!o || o->marked) return;  // null or already visited (handles cycles)
        o->marked = true;
        o->trace(*this);              // recurse into children via overridden trace()
    }
    void sweep() {
        auto it = std::remove_if(heap_.begin(), heap_.end(), [](GCObject* o) {
            if (!o->marked) { delete o; return true; }  // unreachable -> free
            o->marked = false;                          // reachable -> reset for next cycle
            return false;
        });
        heap_.erase(it, heap_.end());
    }
    void collect() { mark(); sweep(); }
};
```
*Listing 82.1 — Mark-and-sweep. The `marked` check in `mark_object` is what terminates traversal of cyclic graphs.*

> **Why this matters / cost model.** Mark-and-sweep is **O(live)** to mark and **O(heap)** to sweep, and in its naive form it is **stop-the-world**: the program must be paused for the entire collection so the object graph does not change underneath the collector. That pause — potentially milliseconds for a large heap — is the defining cost of GC. Critically, mark-and-sweep *handles cycles correctly* (the `marked` flag breaks the recursion), which is its key advantage over reference counting. The trade-offs: pause time (mitigated by incremental/concurrent variants, §82.5), and the sweep cost over the whole heap (mitigated by generational collection, which collects young objects more often than old).

---

## 82.4 Reference Counting and Its Cycle Problem

**Reference counting** takes the opposite approach: each object carries a count of references to it; the count is incremented on copy and decremented on destruction, and the object is freed the instant the count hits zero. This is what `std::shared_ptr` does.

```cpp
// shared_ptr is reference counting. Min standard: C++11.
#include <memory>
struct Node { std::shared_ptr<Node> next; };
// Deterministic: the object is freed exactly when the last shared_ptr to it dies.
```

> **Why this matters / cost model.** Reference counting's appeal is **determinism** — reclamation happens immediately and predictably, with no stop-the-world pause — which is why `shared_ptr` fits C++'s philosophy. Its costs: every copy/destroy is an **atomic** increment/decrement (`shared_ptr`'s control block is thread-safe, so this is a contended atomic RMW — Chapter 76), and it **cannot collect cycles**. A `Node` whose `next` points back to it (directly or through a ring) keeps its own count above zero forever, leaking. The fix is `std::weak_ptr` for back-edges, or a tracing collector (§82.3) for genuinely cyclic graphs. Reference counting trades the pause for per-operation overhead and the cycle leak.

---

## 82.5 Tri-Colour Marking and Concurrent Collection

To shrink the stop-the-world pause, real collectors use **tri-colour marking**, which lets the program run *concurrently* with marking. Each object is one of three colours:

- **White** — not yet visited (candidate garbage).
- **Grey** — visited, but its children not yet scanned (the work frontier).
- **Black** — visited and all children scanned (definitely live).

The collector advances grey objects to black, greying their white children, until no grey remains; surviving white objects are garbage. The invariant that makes this safe concurrently: **no black object may point to a white object** without that white being greyed. A **write barrier** intercepts pointer writes the mutator makes during collection and re-greys as needed.

> **Why this matters / cost model.** Tri-colour concurrent collection is how modern GCs (Go, Java's G1/ZGC, .NET) keep pauses sub-millisecond even on multi-gigabyte heaps — they do most marking while the program runs, pausing only briefly to scan roots and finalise. The cost moves from one big pause to (1) the **write barrier** on every pointer store the program executes (a constant tax on mutator throughput) and (2) collector threads competing for CPU and cache. This is the fundamental GC trade-off in its sharpest form: you can have low pause times *or* zero per-operation overhead, but not both.

---

## 82.6 Conservative vs Precise Collection

- **Precise (exact) collection** knows the exact type and layout of every object and stack frame, so it follows only genuine pointers. Managed runtimes generate this metadata (stack maps); it is accurate and supports moving/compacting collection.
- **Conservative collection** (e.g. the Boehm–Demers–Weiser GC for C/C++) does *not* know which words are pointers, so it treats *any* word that looks like a heap address as a potential pointer. It works without language support but has two costs: an integer that coincidentally equals a heap address pins a dead object (a leak), and because a "pointer" might be an integer, objects **cannot be moved** (you cannot rewrite what might not be a pointer), forbidding compaction.

> **Why this matters.** This is the crux of why you cannot simply add a great GC to standard C++: the language does not record which stack/heap words are pointers, so a drop-in collector must be conservative, with its leaks and no compaction. The toy collector in §82.3 sidesteps this by being a *managed-object* GC (the runtime controls allocation and the `trace` method enumerates children precisely) — which is exactly the situation where GC in C++ makes sense: collecting *your runtime's* objects, whose layout you control, not arbitrary C++ objects.

---

## 82.7 The Cost Model: Why C++ Chose RAII

| Strategy | Reclamation timing | Per-op cost | Cycles | Pauses | Moving |
|---|---|---|---|---|---|
| RAII / `unique_ptr` | Deterministic (scope exit) | None | N/A (ownership tree) | None | N/A |
| Reference counting (`shared_ptr`) | Deterministic (count → 0) | Atomic inc/dec per copy | Leaks (need `weak_ptr`) | None | No |
| Mark-and-sweep | Non-deterministic | None on mutator | Handles | Stop-the-world | Optional |
| Tri-colour concurrent | Non-deterministic | Write barrier per store | Handles | Sub-ms | Yes (if precise) |

> **The discipline.** C++ chose RAII because, for the systems it targets, *deterministic* reclamation with *zero* steady-state overhead and *no* pauses beats GC's convenience. A GC pause — even a "small" one — is unacceptable in an HFT order path, an audio callback, or a kernel; this is the same determinism imperative that drives the allocator (Chapter 79), threading discipline (Chapter 96), and jitter-elimination (Chapter 106) chapters. Reach for tracing GC only when you are *implementing a managed runtime* whose object graph is cyclic and dynamically typed, and even then, isolate the collected heap from the latency-critical C++ host. The next chapter applies the same build-it-to-understand-it method to the standard library itself.
