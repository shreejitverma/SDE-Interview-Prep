# Chapter 26: Filesystem and Polymorphic Memory

> *C++17 brought two long-missing capabilities into the standard library: a portable filesystem API — paths, directory traversal, and file operations, distilled from `boost::filesystem` — and a polymorphic memory model (`std::pmr`) that decouples a container's type from where it allocates. The first lets you write file-manipulating code once and run it on any platform; the second lets you redirect a `std::pmr::vector`'s allocations to a stack buffer, an arena, or a pool without changing its type.*

These two features answer questions every systems programmer had been solving by hand. "How do I list a directory, copy a file, or read a path's extension portably?" previously meant `#ifdef`-laden POSIX/Win32 code or a third-party dependency; `<filesystem>` makes it standard. "How do I make this container allocate from *my* memory, not the global heap?" previously meant writing a custom `Allocator` and threading it through every template instantiation; `<memory_resource>` makes the allocation strategy a *runtime* parameter behind a single type. For low-latency code the second is the headline: a `std::pmr::vector<int>` backed by a `monotonic_buffer_resource` over a stack array performs zero heap allocations until that buffer is exhausted.

---

## Table of Contents

- [26.1 `std::filesystem`: Overview](#261-stdfilesystem-overview)
- [26.2 Paths and Their Decomposition](#262-paths-and-their-decomposition)
- [26.3 Iterating Directories](#263-iterating-directories)
- [26.4 Filesystem Operations and Queries](#264-filesystem-operations-and-queries)
- [26.5 Error Handling: Exceptions vs `error_code`](#265-error-handling-exceptions-vs-error_code)
- [26.6 Polymorphic Memory Resources (`std::pmr`)](#266-polymorphic-memory-resources-stdpmr)
- [26.7 The `pmr` Container Aliases and `polymorphic_allocator`](#267-the-pmr-container-aliases-and-polymorphic_allocator)
- [26.8 The Standard Memory Resources](#268-the-standard-memory-resources)
- [26.9 Professional Insights](#269-professional-insights)

---

## 26.1 `std::filesystem`: Overview

`std::filesystem` (header `<filesystem>`, namespace conventionally aliased `fs`) standardizes file-system operations — path manipulation, directory traversal, and file operations such as copy, rename, and remove. It is based directly on `boost::filesystem`, so the design is mature and the migration from Boost is largely mechanical.

```cpp
// Listing 26.1: the conventional namespace alias
#include <filesystem>
namespace fs = std::filesystem;
```

The library divides into three layers: the **`path`** value type (a portable representation of a filesystem path), **directory iterators** (range-`for`-friendly traversal), and **free-function operations** (the verbs: `create_directory`, `copy`, `remove`, `exists`, and so on). Every operation comes in two forms — one that throws `fs::filesystem_error` and one that reports through a `std::error_code` (Section 26.5).

---

## 26.2 Paths and Their Decomposition

`fs::path` is a value type wrapping a filesystem path in the platform's native format. It is constructible from a string and offers decomposition accessors that return the path's components without any string parsing on your part.

```cpp
// Listing 26.2: constructing and decomposing a path
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

fs::path p = "/home/user/data.txt";

std::cout << p.filename()    << "\n";   // "data.txt"
std::cout << p.extension()   << "\n";   // ".txt"
std::cout << p.parent_path() << "\n";   // "/home/user"
std::cout << p.stem()        << "\n";   // "data"  (filename without extension)
```

Paths compose with `operator/`, which appends a component using the platform's preferred separator — so you never hand-concatenate `"/"` or `"\\"`:

```cpp
// Listing 26.3: building paths portably with operator/
fs::path dir  = "/home/user";
fs::path full = dir / "logs" / "today.txt";   // "/home/user/logs/today.txt"
```

Because `path` is a first-class value type, it converts cleanly to and from `std::string`/`std::wstring`, compares, and is hashable — making it usable as a map key or set element.

---

## 26.3 Iterating Directories

`fs::directory_iterator` yields the entries of a single directory; `fs::recursive_directory_iterator` descends into subdirectories. Both model an input range, so the idiomatic traversal is a range-`for` over `directory_entry` objects, each of which carries the entry's `path()` (and cached status):

```cpp
// Listing 26.4: shallow and recursive directory traversal
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// One directory level:
for (const auto& entry : fs::directory_iterator("/home/user")) {
    std::cout << entry.path() << "\n";
}

// Every level beneath the root:
for (const auto& entry : fs::recursive_directory_iterator("/home/user")) {
    if (entry.is_regular_file())
        std::cout << entry.path() << "  " << entry.file_size() << " bytes\n";
}
```

`directory_entry` exposes status queries (`is_regular_file()`, `is_directory()`, `file_size()`, `last_write_time()`) that are cached from the directory read where the OS supports it — avoiding a second stat call per entry, which matters when walking large trees.

---

## 26.4 Filesystem Operations and Queries

The free-function operations are the verbs of the library. The common ones map directly onto familiar shell commands:

```cpp
// Listing 26.5: core filesystem operations
#include <filesystem>

namespace fs = std::filesystem;

fs::create_directory("sandbox");       // mkdir
fs::copy("a.txt", "b.txt");            // cp
fs::rename("b.txt", "c.txt");          // mv
bool removed = fs::remove("c.txt");    // rm — returns true if a file was removed

bool exists       = fs::exists("sandbox");      // does the path exist?
std::uintmax_t sz = fs::file_size("a.txt");     // size in bytes
```

| Function | Purpose |
|----------|---------|
| `create_directory` / `create_directories` | make one directory / a full chain of directories |
| `copy` / `copy_file` | copy a tree / a single file (with `copy_options`) |
| `rename` | move or rename |
| `remove` / `remove_all` | delete a file / a directory tree, returns count removed |
| `exists` / `is_directory` / `is_regular_file` | existence and type queries |
| `file_size` | size of a regular file in bytes |
| `current_path` | get or set the working directory |
| `space` | free/available/total capacity of the filesystem |

`remove` returns a `bool` (whether something was deleted); `remove_all` returns the **count** of entries removed, so it doubles as a recursive delete.

---

## 26.5 Error Handling: Exceptions vs `error_code`

Filesystem operations interact with the OS and therefore fail for reasons outside the program's control (permissions, races, missing files). Each operation is overloaded two ways:

- **Throwing form** — on failure throws `fs::filesystem_error`, which carries the failing paths and an `error_code`. Use it when a failure is exceptional and should unwind.
- **`error_code` form** — takes a trailing `std::error_code&` out-parameter, sets it on failure, and does **not** throw. Use it in hot paths, in `noexcept` contexts, or when failure is an expected, handled outcome.

```cpp
// Listing 26.6: the non-throwing overload for expected failures
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

std::error_code ec;
std::uintmax_t sz = fs::file_size("maybe_missing.txt", ec);
if (ec) {
    // handle the error without an exception — ec.message() describes it
} else {
    // use sz
}
```

Choosing the `error_code` form is the disciplined default for systems and low-latency code: it makes the failure path explicit and avoids exception-unwinding cost where file absence is a normal condition rather than a bug.

---

## 26.6 Polymorphic Memory Resources (`std::pmr`)

The C++11 allocator model bakes the allocator into the container's *type*: `std::vector<int, MyAlloc>` and `std::vector<int>` are different, incompatible types, and propagating a custom allocator through a codebase means propagating template parameters everywhere. C++17's `<memory_resource>` breaks this coupling. A **memory resource** is a runtime object — derived from the abstract base `std::pmr::memory_resource` — that knows how to `allocate` and `deallocate` bytes. Containers hold a *pointer* to one, so the allocation **strategy is a runtime parameter, not a type parameter**.

```cpp
// Listing 26.7: a container that allocates from a stack buffer
#include <memory_resource>
#include <vector>

char buffer[1024];                                     // storage on the stack
std::pmr::monotonic_buffer_resource pool(buffer, sizeof(buffer));
std::pmr::vector<int> v(&pool);                        // allocates from 'pool'

v.push_back(1);
v.push_back(2);
// No heap allocation occurs until the 1024-byte buffer is exhausted;
// only then does the resource fall back to its upstream (the heap).
```

The decisive properties: every `std::pmr` container is the **same type** regardless of which resource backs it (they all use `std::pmr::polymorphic_allocator`), so a function taking `std::pmr::vector<int>&` accepts a stack-backed, pool-backed, or heap-backed vector interchangeably; and the resource is chosen at the point of *construction*, at runtime, with no template plumbing.

---

## 26.7 The `pmr` Container Aliases and `polymorphic_allocator`

For every standard container, the `std::pmr` namespace provides an alias that fixes the allocator to **`std::pmr::polymorphic_allocator<T>`** — an allocator that simply forwards to whatever `memory_resource` it was given.

```cpp
// Listing 26.8: the pmr container aliases all share one allocator type
namespace pmr = std::pmr;

pmr::vector<int>                      v;     // == std::vector<int, polymorphic_allocator<int>>
pmr::string                           s;
pmr::map<int, pmr::string>            m;
pmr::unordered_map<int, int>          um;
```

Because the allocator type is fixed, these aliases interoperate freely; only the *resource* differs at runtime. A `polymorphic_allocator` constructed from a `memory_resource*` routes all allocations through that resource:

```cpp
// Listing 26.9: passing a resource explicitly via polymorphic_allocator
#include <memory_resource>

std::pmr::monotonic_buffer_resource arena(64 * 1024);
std::pmr::polymorphic_allocator<int> alloc(&arena);

std::pmr::vector<int> v(alloc);   // every (re)allocation comes from 'arena'
```

If no resource is supplied, a `pmr` container uses the process-wide **default resource** (`std::pmr::get_default_resource()`, initially the heap via `new_delete_resource()`), which `std::pmr::set_default_resource()` can override globally.

---

## 26.8 The Standard Memory Resources

`<memory_resource>` ships several ready-made resources, each tuned for a different allocation pattern. All take an optional **upstream** resource to fall back on when their own storage is exhausted.

- **`std::pmr::monotonic_buffer_resource`** — allocates by simply bumping a pointer through a buffer; **never frees individual allocations**, releasing everything at once on destruction. Fastest possible allocation; ideal for a phase that allocates a lot and then discards it all (parsing a request, building a frame). Construct it over a stack array or with an initial size.

- **`std::pmr::unsynchronized_pool_resource`** and **`std::pmr::synchronized_pool_resource`** — pool allocators that group allocations into size classes to reduce fragmentation and reuse freed blocks. The `synchronized` variant is thread-safe; the `unsynchronized` variant is faster but single-threaded. Use these for many small, individually-freed objects of varying lifetimes.

- **`std::pmr::new_delete_resource()`** — the global resource that forwards to `::operator new`/`::operator delete`; the default upstream and default resource.

- **`std::pmr::null_memory_resource()`** — a resource whose `allocate` always throws `std::bad_alloc`. Used as an *upstream* to **guarantee** that a `monotonic_buffer_resource` over a fixed buffer never silently falls back to the heap — turning buffer exhaustion into a hard, detectable error.

```cpp
// Listing 26.10: a strictly heap-free arena via null_memory_resource upstream
#include <memory_resource>
#include <vector>

char buf[4096];
// If 'buf' is exhausted, the null upstream throws bad_alloc instead of heap-allocating:
std::pmr::monotonic_buffer_resource arena(buf, sizeof(buf),
                                          std::pmr::null_memory_resource());
std::pmr::vector<int> v(&arena);   // provably allocates ONLY from buf
```

This composition — a monotonic arena bounded by a null upstream — is the canonical low-latency idiom: it gives a container fast, contiguous, allocation-free storage and *proves* at runtime that it never touches the global heap.

---

## 26.9 Professional Insights

**Prefer the `error_code` overloads of filesystem operations in systems code.** File operations fail for environmental reasons that are not program bugs — a missing file, a permission change, a race with another process. The non-throwing overload makes that failure path explicit and avoids exception-unwinding cost on a path that is often expected, not exceptional. Reserve the throwing form for genuinely unrecoverable I/O failures.

**Use `directory_entry`'s cached status when walking large trees.** Querying `is_regular_file()` / `file_size()` through the `directory_entry` reuses information gathered during the directory read where the OS allows it, avoiding a second `stat` per entry. On a tree with millions of files that is the difference between one syscall per entry and two.

**Reach for `std::pmr` whenever allocation pattern, not element type, is the performance lever.** A `std::pmr::vector<int>` over a `monotonic_buffer_resource` is the same type as any other `pmr::vector<int>`, so you can localize an arena to a request, a frame, or a scope without rewriting interfaces. This is the standard, non-intrusive way to eliminate heap traffic from a hot path that the old `Allocator` template parameter made painful.

**Bound a `monotonic_buffer_resource` with `null_memory_resource()` when "no heap" is a hard requirement.** In latency-critical code, silent fallback to the heap when a buffer fills is exactly the surprise you are trying to prevent. Giving the arena a null upstream converts exhaustion into a thrown `bad_alloc` you can detect in testing, guaranteeing the allocation-free property rather than hoping for it.

**Match the resource to the lifetime pattern.** Monotonic for allocate-much-then-discard-all phases; a pool resource for many small objects freed individually; `new_delete_resource` as the general fallback. The wrong resource (a monotonic arena for long-lived, individually-freed objects) leaks until destruction; the right one turns the allocator into a tuned, measurable component of the design.
