# LOW-LATENCY SYSTEM ARCHITECTURE


Designing systems where microseconds matter (Trading, Real-time AdTech).

### 24.1 The Disruptor Pattern (C++ Implementation)
A high-performance inter-thread messaging library. Key concept: **Single-Writer Ring Buffer** with no locks.

```cpp
template<typename T, size_t Size>
class Disruptor {
    std::array<T, Size> ring_buffer;
    alignas(64) std::atomic<int64_t> cursor{-1}; // Cache line padded
    
public:
    template<typename F>
    void publish(F&& factory) {
        int64_t current = cursor.load(std::memory_order_relaxed);
        int64_t next = current + 1;
        
        // Write data (no contention for single writer)
        factory(ring_buffer[next & (Size - 1)]);
        
        // Commit
        cursor.store(next, std::memory_order_release);
    }
    
    // Consumer tracks its own sequence...
};
```

### 24.2 Kernel Bypass Networking (Concept)
Standard OS networking (interrupts, context switches) adds 10-50us latency.
**Solution**: Map the NIC (Network Interface Card) directly to user-space memory (DPDK, Solarflare OpenOnload).

*   **Zero Copy**: Packet data goes from NIC -> CPU L3 Cache -> User Buffer.
*   **Polling**: Instead of interrupts, one CPU core spins (`while(true)`) checking the NIC ring.

### 24.3 OS Tuning for C++
Your code is only as fast as the OS allows.

1.  **CPU Isolation (`isolcpus`)**: Isolate cores from the OS scheduler so your thread never gets preempted.
2.  **Huge Pages**: Use 2MB or 1GB pages to reduce TLB (Translation Lookaside Buffer) misses.
    ```cpp
    void* ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, 
                     MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB, -1, 0);
    ```
3.  **Disable C-States**: Prevent CPU from going to sleep (power save) which causes wake-up latency.

### 24.4 Zero-Copy Serialization (Cap'n Proto / FlatBuffers)
Avoid parsing JSON/XML. Access data directly from the binary buffer.

```cpp
// FlatBuffers schema compiled to C++ header
// No parsing step! Pointers just point to the right offsets.
auto monster = GetMonster(buffer_pointer);
auto hp = monster->hp(); // Immediate access
auto pos = monster->pos();
```

### 24.5 LMAX Disruptor Internals

The key to Disruptor's speed is the **Sequence Barrier**.

1.  **Cursor**: Monotonically increasing number (atomic).
2.  **Barrier**: Consumers wait until `cursor >= my_sequence`.
3.  **Wait Strategy**:
    *   `BusySpinWaitStrategy`: Loops `while(cursor < seq)`. 100% CPU, 0ns latency.
    *   `YieldingWaitStrategy`: Loops but calls `std::this_thread::yield()`.
    *   `BlockingWaitStrategy`: Uses `std::condition_variable` (slowest).

---
