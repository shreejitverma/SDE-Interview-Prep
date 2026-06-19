# Chapter 32: Memory Allocators

> *Taking absolute control over `new` and `delete`.*

In most C++ applications, when you need memory on the heap, you call `new` (or the underlying `malloc`), and when you are done, you call `delete` (or `free`). 

For 95% of applications, the default OS allocator is fantastic. But for the remaining 5%—game engines, embedded systems, high-frequency trading, and database engines—`malloc` is a major bottleneck.

---

## 32.1 The Problem with `malloc`

`malloc` is a general-purpose allocator. It has to handle everything from 1-byte requests to 1-gigabyte requests. 
When you call `malloc`:
1.  It checks its internal "Free List" to see if it has a block of memory that fits your request.
2.  If it doesn't, it has to make an expensive System Call (like `sbrk` or `mmap` on Linux) to ask the Operating System for more physical RAM.
3.  Because multiple threads can call `malloc` at the same time, it relies on locks/mutexes internally, causing contention.
4.  If you allocate and free small, random chunks of memory, it causes **Memory Fragmentation**, where your RAM looks like Swiss cheese. You might have 100MB of free RAM, but if it's broken up into 10-byte chunks, allocating a 1MB block will fail.

Custom Memory Allocators bypass the OS completely. You ask the OS for a massive chunk of RAM *once* at startup, and then you divide that memory up yourself using specialized, lightning-fast algorithms.

## 32.2 Linear (Arena) Allocators

The fastest allocator in the world is the Linear Allocator (also called an Arena or Stack Allocator). 

It maintains a pointer to the start of a large block of memory, and a `current` pointer. When you allocate, it just bumps the pointer forward. It is exactly as fast as allocating on the stack: $O(1)$.

```cpp
class LinearAllocator {
    char* start;
    char* current;
    size_t total_size;

public:
    LinearAllocator(size_t size) : total_size(size) {
        start = new char[size]; // Ask OS for one giant chunk
        current = start;
    }

    void* allocate(size_t bytes) {
        if (current + bytes > start + total_size) return nullptr; // Out of memory
        void* ptr = current;
        current += bytes; // Just bump the pointer!
        return ptr;
    }

    // You cannot free individual objects.
    // You can only wipe the entire Arena at once.
    void reset() { current = start; } 
    
    ~LinearAllocator() { delete[] start; }
};
```
*Use case: Game development. Create an arena for a "Level". Allocate thousands of enemies, bullets, and textures. When the player beats the level, don't call `delete` on thousands of objects. Just call `reset()` on the Arena. Instant cleanup.*

## 32.3 Pool Allocators

A Linear Allocator doesn't allow freeing individual objects. If you need to allocate and free objects rapidly over time, you use a **Pool Allocator**.

A Pool Allocator is designed to allocate objects of exactly *one specific size*. 
If you are writing a Particle System, and every `Particle` is 32 bytes, you create a Pool Allocator where the memory chunk is divided into thousands of 32-byte blocks.

Because every block is the same size, there is **zero memory fragmentation**. When you free a block, it is simply pushed onto a lock-free "Free List" (a linked list of empty blocks). Allocation is an $O(1)$ pop from the list.

## 32.4 Placement `new`

If you are using a custom allocator to get raw memory, how do you actually construct a C++ object inside that memory? You can't use standard `new`, because standard `new` calls `malloc`!

You must use **Placement `new`**. This syntax tells the compiler: *"Do not allocate memory. Just run the constructor at this specific memory address."*

```cpp
// 1. Get raw memory from our custom allocator
void* raw_memory = my_arena.allocate(sizeof(Player));

// 2. Use Placement New to construct the object in that memory
Player* p = new(raw_memory) Player("Godhood");

// 3. Since we didn't use standard new, we CANNOT use standard delete.
// We must call the destructor manually!
p->~Player(); 

// 4. (The memory is reclaimed when the Arena resets)
```

## 32.5 Polymorphic Memory Resources (`std::pmr`) [C++17]

Before C++17, if you wanted a `std::vector` to use your custom allocator, you had to pass the allocator type in as a template argument: `std::vector<int, MyAllocator>`. 
This created a massive problem: A function expecting a `std::vector<int>` would reject your custom vector because the *types* were completely different!

C++17 introduced `<memory_resource>` and `std::pmr` (Polymorphic Memory Resources). This uses virtual functions under the hood to allow you to swap allocators at runtime without changing the type of the container.

```cpp
#include <vector>
#include <memory_resource>

// Allocate a 1KB buffer on the stack!
char buffer[1024];

// Create a PMR allocator that uses our stack buffer
std::pmr::monotonic_buffer_resource pool(buffer, 1024);

// This vector will never call malloc. It will live entirely on the stack.
std::pmr::vector<int> v(&pool); 

v.push_back(10);
v.push_back(20);
```

## 32.6 Alignment

CPU hardware is picky. If you try to read a 4-byte `int`, the CPU prefers the memory address to be a multiple of 4. If you try to read 32 bytes into an AVX SIMD register, the CPU *demands* the memory address be a multiple of 32. 

If memory is unaligned, the CPU either takes a massive performance penalty, or it outright crashes the program (a `SIGBUS` error, common on ARM architectures).

When writing custom allocators, you must respect `alignof(T)`.

```cpp
struct alignas(32) SIMD_Data {
    float values[8];
};

// Alignment requirement
std::cout << alignof(SIMD_Data); // Prints 32
```
Standard `malloc` and standard `new` guarantee that the memory they return is suitably aligned for any standard type (usually 16 bytes).

---

By taking control of memory allocation, you eliminate the OS bottleneck, prevent fragmentation, and guarantee that your data is perfectly packed into the CPU cache.

But what if we could optimize our code before the program even starts running? What if we could force the compiler to do the math for us? We enter the realm of **Compile-Time Programming**.
