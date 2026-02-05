# CUSTOM MEMORY ALLOCATORS


`malloc` and `new` are general-purpose and slow (locks, fragmentation). Real-time systems use custom allocators.

### 33.1 Linear Allocator (Arena)
The absolute fastest allocator. O(1). Zero overhead.

```cpp
class LinearAllocator {
    char* start;
    char* current;
    size_t size;
public:
    LinearAllocator(size_t s) : size(s) {
        start = new char[s];
        current = start;
    }
    
    void* allocate(size_t n) {
        if (current + n > start + size) return nullptr;
        void* ptr = current;
        current += n;
        return ptr;
    }
    
    void reset() { current = start; } // Free ALL at once
};
```
*   **Use Case**: Per-frame game memory, Request-scoped web server memory.

### 33.2 Pool Allocator
Fixed-size blocks. No external fragmentation. O(1) malloc/free.

```cpp
struct Chunk { Chunk* next; };

class PoolAllocator {
    Chunk* head = nullptr;
public:
    void* allocate() {
        if (!head) return ::operator new(sizeof(Chunk)); // Or expand pool
        Chunk* ptr = head;
        head = head->next;
        return ptr;
    }
    
    void deallocate(void* ptr) {
        Chunk* chunk = static_cast<Chunk*>(ptr);
        chunk->next = head;
        head = chunk;
    }
};
```

---

# APPENDICES
