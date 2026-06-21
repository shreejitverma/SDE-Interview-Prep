# Chapter 50: Writing a Garbage Collector

C++ is built on the philosophy of RAII (Resource Acquisition Is Initialization) and deterministic object lifetimes. However, building a garbage collector (GC) from scratch is a profound exercise in systems architecture. It teaches you how the runtime interacts with stack layouts, memory alignment, and object reference graphs.

This chapter implements a fully functional, minimal **Mark-and-Sweep Garbage Collector** in C++ to demonstrate these systems-level mechanics.

***

## 50.1 Mark-and-Sweep Theory

A Mark-and-Sweep collector works in two distinct phases:

1.  **Mark Phase:** Start from a set of known pointers (the **Roots**), traverse the directed object reference graph, and mark every reachable object as `marked = true`.
2.  **Sweep Phase:** Iterate through all memory blocks allocated on the heap. If an object is not marked, it is unreachable and is immediately freed (`delete`). If it *is* marked, clear the mark flag (`marked = false`) to reset it for the next collection cycle.

### 🔍 Finding the "Roots"

In a real runtime (like Java's JVM or .NET's CLR), the root set consists of:
*   Pointers stored in CPU registers.
*   Pointers stored in global/static memory.
*   Pointers sitting on the active call stack (local variables of functions currently running).
To find these, a real GC must parse the execution stack frame-by-frame, checking memory boundaries for valid heap pointer values.

***

## 50.2 Compile-Ready C++ Garbage Collector Implementation

Below is a complete, simulated implementation. We model stack-based root management explicitly using a `VM` context.

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>

// Forward declaration
class VM;

// Base class for all garbage-collected objects
struct GCObject {
    bool marked = false;
    
    // Each object must be able to enumerate its children (pointers to other GCObjects)
    // for the mark-phase traversal.
    std::vector<GCObject*> children;

    virtual ~GCObject() = default;
};

// Example concrete GC object types
struct GCInt : public GCObject {
    int value;
    explicit GCInt(int val) : value(val) {}
};

struct GCPair : public GCObject {
    GCPair(GCObject* left, GCObject* right) {
        children.push_back(left);
        children.push_back(right);
    }
};

// The Virtual Machine / Execution Context managing GC
class VM {
private:
    // Tracking list of all allocated objects on the heap
    std::list<GCObject*> heap;

    // Simulated Execution Stack containing roots
    std::vector<GCObject*> stack;

    size_t next_gc_threshold = 8; // Collect after this many allocations

public:
    VM() = default;

    ~VM() {
        // Collect everything on shutdown
        stack.clear();
        collect();
    }

    // Push an object onto the simulated execution stack (marking it as a root)
    void push(GCObject* obj) {
        stack.push_back(obj);
    }

    // Pop an object off the stack (it is no longer a root)
    GCObject* pop() {
        if (stack.empty()) throw std::underflow_error("Stack underflow");
        GCObject* obj = stack.back();
        stack.pop_back();
        return obj;
    }

    // Factory methods for allocations
    GCInt* new_int(int val) {
        if (heap.size() >= next_gc_threshold) {
            collect();
        }
        auto* obj = new GCInt(val);
        heap.push_back(obj);
        return obj;
    }

    GCPair* new_pair(GCObject* left, GCObject* right) {
        if (heap.size() >= next_gc_threshold) {
            collect();
        }
        auto* obj = new GCPair(left, right);
        heap.push_back(obj);
        return obj;
    }

    // Phase 1: Mark
    void mark() {
        for (auto* obj : stack) {
            mark_object(obj);
        }
    }

    void mark_object(GCObject* obj) {
        if (!obj || obj->marked) return;

        obj->marked = true;

        // Traverse children recursively (Depth-First Search)
        for (auto* child : obj->children) {
            mark_object(child);
        }
    }

    // Phase 2: Sweep
    void sweep() {
        size_t before = heap.size();
        
        auto it = heap.begin();
        while (it != heap.end()) {
            GCObject* obj = *it;
            if (!obj->marked) {
                // Object is unreachable: delete it
                it = heap.erase(it);
                delete obj;
            } else {
                // Object is reachable: keep it, reset mark for next GC
                obj->marked = false;
                ++it;
            }
        }
        
        size_t freed = before - heap.size();
        std::cout << "[GC] Collected " << freed << " objects. Heap size: " << heap.size() << "\n";
        
        // Dynamic threshold adjustments
        next_gc_threshold = heap.size() + 8;
    }

    // Trigger Garbage Collection
    void collect() {
        std::cout << "[GC] Starting collection cycle...\n";
        mark();
        sweep();
    }

    size_t heap_size() const { return heap.size(); }
};
```

***

## 50.3 Verification & Cycle Handling

A major benefit of Mark-and-Sweep over simple Reference Counting (like `std::shared_ptr`) is its ability to handle **circular dependencies** without memory leaks. 

If Object A points to Object B, and Object B points to Object A, their reference counts will never drop to zero. However, if neither is reachable from the stack roots, our Mark-and-Sweep algorithm will collect both.

Below is a verification script demonstrating cycle collection:

```cpp
int main() {
    VM vm;

    std::cout << "--- 1. Allocate isolated objects ---\n";
    GCObject* obj1 = vm.new_int(42);
    GCObject* obj2 = vm.new_int(100);
    
    vm.push(obj1); // obj1 is now a stack root
    
    std::cout << "Triggering GC (obj2 should be collected, obj1 kept):\n";
    vm.collect(); // heap size should drop to 1

    std::cout << "\n--- 2. Create circular reference ---\n";
    GCObject* a = vm.new_int(1);
    GCObject* b = vm.new_int(2);
    
    // Create cycle: a points to b, b points to a
    a->children.push_back(b);
    b->children.push_back(a);

    vm.push(a); // Push 'a' to stack: both 'a' and 'b' are reachable
    vm.collect(); // Keep both 'a' and 'b'
    
    std::cout << "\n--- 3. Drop root to cycle ---\n";
    vm.pop(); // Pop 'a' off stack: cycle is now unreachable from roots
    vm.collect(); // Both 'a' and 'b' are collected

    return 0;
}
```

### 🧠 Performance Tuning Considerations:

1.  **Stop-The-World (STW):** The implementation above pauses execution completely to run GC. In latency-sensitive systems, this causes unacceptable spikes in execution time (jitter).
2.  **Incremental & Generational GC:** Production GCs run concurrently alongside the main threads and group objects by age (Generations), under the heuristic that "most objects die young", significantly reducing collection sweep scopes.
