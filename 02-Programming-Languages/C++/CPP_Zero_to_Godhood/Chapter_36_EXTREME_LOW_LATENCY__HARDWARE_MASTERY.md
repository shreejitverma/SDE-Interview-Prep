# EXTREME LOW LATENCY & HARDWARE MASTERY


To achieve sub-microsecond latency, you must program the hardware, not just the language.

### 31.1 CPU Architecture & Cache Topology
*   **L1 Cache**: ~32KB, 3-4 cycles. Per core.
*   **L2 Cache**: ~256KB-1MB, 10-12 cycles. Per core.
*   **L3 Cache**: ~10MB+, 40-70 cycles. Shared across cores.
*   **RAM**: 100+ cycles.

**Optimization Goal**: Stay in L1/L2.
**Technique**: Minimize object size, use contiguous memory (arrays), align data to cache lines (64 bytes).

### 31.2 NUMA (Non-Uniform Memory Access)
On multi-socket servers, accessing RAM attached to another CPU socket is slow.
*   **Solution**: Pin threads to cores. Allocate memory on the local node.
*   **Tool**: `numactl --cpunodebind=0 --membind=0 ./app`

### 31.3 Compiler Optimizations (The "Free Lunch")
*   `-O3`: Aggressive optimization.
*   `-march=native`: Use instructions available on the build machine (AVX2, AVX-512).
*   `-flto` (Link Time Optimization): Optimize across translation units (inlining across .cpp files).
*   **PGO (Profile Guided Optimization)**:
    1.  Compile with `-fprofile-generate`.
    2.  Run the app (training run).
    3.  Recompile with `-fprofile-use`.

### 31.4 Lock-Free Stack Implementation (Wait-Free Push)
A classic interview and system component.

```cpp
template<typename T>
struct Node {
    T data;
    Node* next;
    Node(const T& d) : data(d), next(nullptr) {}
};

template<typename T>
class LockFreeStack {
    std::atomic<Node<T>*> head{nullptr};

public:
    void push(const T& data) {
        Node<T>* new_node = new Node<T>(data);
        new_node->next = head.load(std::memory_order_relaxed);
        
        // CAS Loop
        while (!head.compare_exchange_weak(
            new_node->next, 
            new_node,
            std::memory_order_release, 
            std::memory_order_relaxed));
    }

    bool pop(T& result) {
        Node<T>* old_head = head.load(std::memory_order_acquire);
        
        while (old_head && !head.compare_exchange_weak(
            old_head,
            old_head->next,
            std::memory_order_acquire,
            std::memory_order_relaxed));
            
        if (!old_head) return false;
        
        result = old_head->data;
        // Note: Deletion in lock-free requires Hazard Pointers or RCU!
        // Leaking here for simplicity of example.
        return true;
    }
};
```

### 31.5 Measurable Performance Targets
Define Service Level Objectives (SLOs) in percentiles.
*   **p50 (Median)**: Typical case.
*   **p99**: The "slow" case (1 in 100).
*   **p99.9**: The tail latency (1 in 1000). Crucial for HFT.

**Example Target**:
"Order processing must have p99 latency < 5 microseconds."

---
