# Chapter 63: Diagnostics and Lifetime Utilities

> This chapter collects the C++23 additions that operate at the boundary between your program and the machine: capturing a call stack for diagnostics with **`std::stacktrace`**, bridging modern smart pointers to legacy `T**` C APIs with **`std::out_ptr`** and **`std::inout_ptr`**, formally blessing the "I have bytes, treat them as an object" pattern with **`std::start_lifetime_as`**, and doing allocation-free stream I/O over a fixed buffer with **`std::spanstream`**. Individually small, together they remove a cluster of long-standing workarounds in systems and interop code.

## Table of Contents

1. [`std::stacktrace` — Standard Call-Stack Capture](#631-stdstacktrace--standard-call-stack-capture)
2. [Inspecting Frames with `stacktrace_entry`](#632-inspecting-frames-with-stacktrace_entry)
3. [`std::out_ptr` and `std::inout_ptr` — Smart Pointers Meet C APIs](#633-stdout_ptr-and-stdinout_ptr--smart-pointers-meet-c-apis)
4. [`std::start_lifetime_as` — Legitimizing Byte-to-Object Reinterpretation](#634-stdstart_lifetime_as--legitimizing-byte-to-object-reinterpretation)
5. [`std::spanstream` — Allocation-Free Stream I/O](#635-stdspanstream--allocation-free-stream-io)
6. [Performance and Safety Notes](#636-performance-and-safety-notes)
7. [Professional Insights](#637-professional-insights)

---

## 63.1 `std::stacktrace` — Standard Call-Stack Capture

For decades, capturing a call stack in portable C++ was impossible: you reached for platform APIs (`backtrace` on glibc, `CaptureStackBackTrace` on Windows, `libunwind`) and a symbol-resolution library, and wrote per-platform glue. C++23's `<stacktrace>` standardizes it. `std::stacktrace::current()` captures the stack at the point of call as an ordinary, copyable, iterable container of frames.

**Listing 63.1: Capturing and printing the current stack.**

```cpp
#include <stacktrace>
#include <print>

void crash_handler() {
    std::println("Crash! Stacktrace:\n{}", std::stacktrace::current());
}

void inner()  { crash_handler(); }
void outer()  { inner(); }

int main() { outer(); }
```

`std::stacktrace` is a regular container: it has `size()`, `begin()`/`end()`, and `operator[]`, and it is directly formattable (and streamable), so `std::println("{}", trace)` produces a human-readable, symbolized backtrace — function names, and where debug info is available, file and line. The capture is a value you can store, copy, compare, and attach to a log record or an exception, rather than a one-shot print.

A common pattern is to embed a captured trace in a custom exception or an error-logging path so that when something fails, the log shows *where* it failed without a debugger.

---

## 63.2 Inspecting Frames with `stacktrace_entry`

Each element of a `std::stacktrace` is a `std::stacktrace_entry` — a handle to a single frame. You can interrogate it programmatically rather than only printing the whole trace:

- **`entry.description()`** — the function name / description of the frame.
- **`entry.source_file()`** — the source file, if available.
- **`entry.source_line()`** — the line number, if available.
- **`explicit operator bool`** — whether the entry refers to a real frame.

**Listing 63.2: Walking individual frames.**

```cpp
#include <stacktrace>
#include <print>

void report() {
    auto trace = std::stacktrace::current();
    for (const auto& frame : trace)
        std::println("{} at {}:{}", frame.description(),
                     frame.source_file(), frame.source_line());
}

int main() { report(); }
```

This frame-level access lets you build structured diagnostics — filtering out library frames, formatting your own way, or correlating frames with application data — rather than being limited to the default rendering.

---

## 63.3 `std::out_ptr` and `std::inout_ptr` — Smart Pointers Meet C APIs

A pervasive interop friction: a C API initializes a resource through an *output parameter* of type `T**` (it writes a freshly-allocated pointer into `*pp`), but you want to own that resource in a `std::unique_ptr` or `std::shared_ptr`. Before C++23 you had to declare a raw `T*`, pass `&raw`, then manually construct the smart pointer from `raw` — a two-step dance that is easy to get wrong and leaks if an exception intervenes.

`std::out_ptr` and `std::inout_ptr` (header `<memory>`) are adaptors that present a smart pointer to such an API as if it were a `T**`, and on destruction of the adaptor, *reset the smart pointer to own whatever the C function wrote*.

- **`std::out_ptr(sp)`** — for pure *output* parameters: the function is expected to overwrite the slot. The smart pointer's previous value is released first. Pass it where a `T**` is expected.
- **`std::inout_ptr(sp)`** — for *in-out* parameters: the function reads the existing pointer (perhaps to `realloc` or free it) and writes back a new one. It hands the current raw pointer to the API and adopts the result.

**Listing 63.3: Adopting a C API's output into a `unique_ptr`.**

```cpp
#include <memory>
#include <cstdio>

// Legacy C-style API: allocates and writes through a T**.
struct Widget;
extern "C" int widget_create(Widget** out);   // writes *out, returns status
extern "C" void widget_destroy(Widget*);

int main() {
    std::unique_ptr<Widget, decltype(&widget_destroy)> w(nullptr, &widget_destroy);

    // out_ptr presents 'w' as a Widget**; on scope exit it adopts the result.
    if (widget_create(std::out_ptr(w)) == 0) {
        // 'w' now owns the Widget the C function allocated; auto-freed via deleter.
    }
}
```

This turns a leak-prone manual handshake into a single, exception-safe expression, and it is the standard, allocator-aware way to bridge owning smart pointers with the enormous body of C APIs that use `T**` outputs.

---

## 63.4 `std::start_lifetime_as` — Legitimizing Byte-to-Object Reinterpretation

Systems code constantly receives raw bytes — from a socket, a memory-mapped file, a DMA buffer — and wants to interpret them as a trivially-copyable struct: `auto* p = reinterpret_cast<Header*>(buffer);`. The dirty secret is that, by the strict letter of the object model, this is **undefined behavior**: no `Header` object's *lifetime* has begun in that storage, so accessing `p->field` is UB even though it "works" everywhere. C++23's `std::start_lifetime_as<T>(ptr)` (header `<memory>`) fixes this precisely: it *starts the lifetime* of a `T` (or array of `T`) in the given storage, using the bytes already present as the object's representation, and returns a usable, well-defined pointer — with no copying and no code generated.

**Listing 63.4: Well-defined reinterpretation of a received buffer.**

```cpp
#include <memory>
#include <cstdint>
#include <print>

struct PacketHeader {        // trivially copyable
    std::uint16_t type;
    std::uint16_t length;
    std::uint32_t seq;
};

void on_bytes(std::byte* buffer, std::size_t n) {
    // Begin a PacketHeader's lifetime in 'buffer' using the bytes there.
    PacketHeader* h = std::start_lifetime_as<PacketHeader>(buffer);
    std::println("type={} len={} seq={}", h->type, h->length, h->seq);  // defined behavior
}
```

The distinction from `reinterpret_cast` is purely about the object model and therefore about *what the optimizer is allowed to assume*: `start_lifetime_as` makes the access defined, so the compiler cannot treat it as the unreachable UB that `reinterpret_cast`-then-access technically is. There is a companion `std::start_lifetime_as_array<T>` for buffers holding many objects. (Note the contrast with `std::bit_cast`, from Volume 5, which *copies* bytes into a fresh object; `start_lifetime_as` begins an object's lifetime *in place*, with no copy.)

---

## 63.5 `std::spanstream` — Allocation-Free Stream I/O

`std::stringstream` is convenient for formatting into and parsing out of memory, but it *owns and grows a heap buffer*. In latency-sensitive or memory-constrained code you often have a fixed buffer already (on the stack, or a slice of a larger arena) and want stream semantics over it with **zero allocation**. The deprecated `std::strstream` tried to fill this role but was unsafe. C++23's `std::spanstream` (header `<spanstream>`) is the proper replacement: an `iostream` that operates over a caller-provided `std::span<char>`.

**Listing 63.5: Formatting into and parsing from a fixed buffer.**

```cpp
#include <spanstream>
#include <print>
#include <string>

int main() {
    char buffer[128];
    std::spanstream ss(buffer);     // stream over the fixed buffer, no heap

    ss << "Hello " << 123;          // write into 'buffer'
    std::string_view written = ss.span();   // the bytes actually written
    std::println("wrote: {}", written);     // Hello 123

    // Parsing back out, also allocation-free:
    std::spanstream in(buffer);
    std::string word; int n;
    in >> word >> n;                // word="Hello", n=123
    std::println("parsed: {} {}", word, n);
}
```

Because the buffer is supplied by the caller and never reallocated, `spanstream` is suitable for hot paths where `stringstream`'s allocation would be unacceptable, and for embedded contexts where dynamic allocation is forbidden outright. (`spanstream` was introduced in the same C++23 wave as `std::print`; it is discussed here alongside the other lifetime/buffer utilities, and also mentioned in Chapter 61's output context.)

> **Version-trap flag:** `std::stacktrace`/`stacktrace_entry`, `std::out_ptr`/`std::inout_ptr`, `std::start_lifetime_as`/`start_lifetime_as_array`, and `std::spanstream` are all **C++23**. `std::stacktrace` in particular depends on the platform's unwinder and symbol info — capture works broadly, but symbolized output quality varies, and some libraries require linking a stacktrace backend. Check `__cpp_lib_stacktrace`, `__cpp_lib_out_ptr`, `__cpp_lib_start_lifetime_as`, and `__cpp_lib_spanstream`.

---

## 63.6 Performance and Safety Notes

- **`std::stacktrace` capture is not free.** Walking and especially *symbolizing* the stack reads debug information and can cost microseconds to milliseconds — fine for an error path or a crash handler, far too expensive for a hot loop. Capture lazily (store the trace, symbolize only when you actually render it) and never on a fast path.
- **`out_ptr`/`inout_ptr` adaptors are zero-overhead and exception-safe.** They compile to the same raw-pointer handshake you would write by hand, but with guaranteed cleanup; there is no reason to keep doing it manually.
- **`start_lifetime_as` generates no code.** It is purely a signal to the abstract machine and the optimizer; the resulting pointer access is exactly as fast as the (technically-UB) `reinterpret_cast` version, but now defined. Its precondition is real, though: the storage must be suitably sized and aligned for `T`, and `T` must be an implicit-lifetime type (trivially copyable, essentially) — get the alignment wrong and you are back in UB territory.
- **`spanstream` never allocates**, but it also never grows: writing past the span's capacity sets the stream's fail bit rather than expanding, so you must size the buffer for the worst case and check stream state.

---

## 63.7 Professional Insights

**Attach a captured `std::stacktrace` to your errors, but symbolize lazily.** The highest-value use is embedding `std::stacktrace::current()` into an exception type or structured log record at the moment of failure, so post-mortem diagnosis does not require reproducing the bug under a debugger. The cost discipline that makes this viable is to *capture* cheaply and *symbolize* (the expensive part) only when the trace is actually printed — never decorate a hot path with stack capture, and never symbolize traces you discard.

**Use `out_ptr`/`inout_ptr` everywhere you currently launder a raw pointer into a smart pointer across a C API.** The manual "declare `T* raw`, pass `&raw`, construct the smart pointer" pattern is a recurring source of leaks on the error path and is simply obsolete. The adaptors are zero-overhead and exception-safe, so adopting them is a pure correctness upgrade with no performance cost — exactly the kind of change to make wholesale when modernizing interop code.

**Replace `reinterpret_cast`-onto-bytes with `std::start_lifetime_as` in serialization and wire-protocol code.** The old pattern is undefined behavior that happens to work until an optimizer decides otherwise; `start_lifetime_as` makes the same in-place, no-copy reinterpretation *defined*, closing a real (if usually latent) miscompilation risk in exactly the parsing and DMA code where it matters most. It costs nothing at runtime, so there is no reason to keep relying on the UB version — provided you honor the alignment and implicit-lifetime preconditions.

**Reach for `spanstream` when you want `stringstream` ergonomics without the allocation.** Fixed-buffer formatting and parsing on a hot path, or in an allocation-prohibited embedded context, is precisely its niche. Size the buffer for the worst case and check the fail bit, and you get the familiar `<<`/`>>` interface with deterministic, zero-allocation behavior — the safe modern successor to the dangerous `strstream` that this finally lets you delete.
