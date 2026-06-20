# CUSTOM MEMORY ALLOCATORS

## 1. Why Custom Allocators?

*   **Performance:** `malloc` is general purpose. Specialized allocators are faster.
*   **Locality:** Keep related objects close in memory (cache friendly).
*   **Fragmentation:** Reduce memory fragmentation.

## 2. Linear / Stack Allocator

Moves a pointer forward. O(1) allocation. Deallocation only possible by resetting the whole buffer (LIFO).

```cpp
class StackAllocator {
    char* start;
    char* current;
    size_t size;
public:
    void* allocate(size_t n) {
        if (current + n > start + size) return nullptr;
        void* ptr = current;
        current += n;
        return ptr;
    }
    void reset() { current = start; }
};
```

## 3. Pool Allocator

Allocates chunks of fixed size. No fragmentation. O(1) alloc/dealloc (using a free list).

## 4. `std::pmr` (Polymorphic Memory Resources)

C++17 feature. Allows changing allocators at runtime without changing the type (e.g., `std::pmr::vector`).

```cpp
#include <memory_resource>

char buffer[1024];
std::pmr::monotonic_buffer_resource pool(buffer, 1024);
std::pmr::vector<int> v(&pool); // Uses stack buffer
```

## 5. Alignment

Understanding `alignof`, `alignas`, and `std::max_align_t`.

*   Unaligned access can be slow or cause crashes (SIGBUS on ARM).
*   SIMD often requires 16, 32, or 64-byte alignment.
