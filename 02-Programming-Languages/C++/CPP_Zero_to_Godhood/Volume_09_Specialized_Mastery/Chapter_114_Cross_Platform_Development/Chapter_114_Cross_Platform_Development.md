# Chapter 114: Cross-Platform Development

C++'s reach is unmatched: the *same* C++ core can run on a server, compile to run in a browser via WebAssembly, link into an Android app through the NDK, and ship inside an iOS app — which is why C++ is the natural choice for the performance-critical *engine* shared across every platform a product targets. This chapter covers the portability disciplines and the major targets — WebAssembly, mobile (NDK/JNI) — along with the architecture pattern that makes it work: a portable C++ core behind thin platform-specific shells.

## Chapter Roadmap

- 114.1 The Portable-Core Architecture
- 114.2 WebAssembly with Emscripten
- 114.3 Mobile C++: Android NDK and iOS
- 114.4 Portability Disciplines
- 114.5 Conditional Compilation and Its Hazards

---

## 114.1 The Portable-Core Architecture

The dominant pattern for cross-platform C++ is a **portable core**: the performance-critical, platform-independent logic (the engine, the codec, the business rules) is written once in standard C++, and each platform gets a *thin shell* that adapts the core to that platform's UI, lifecycle, and APIs.

> **Why this matters.** Writing the core once and sharing it across web, mobile, and desktop is the entire economic argument for C++ in cross-platform products: the hard, valuable logic (a video codec, a database engine, a game's simulation, a crypto library) is implemented and optimised *once*, and only the comparatively small platform glue is rewritten per target. This is how products like Figma (C++ rendering core compiled to WebAssembly), Google's mobile apps (shared C++ via the NDK), and most game engines work. The discipline is to keep the core *free of platform dependencies* — no direct OS, UI, or filesystem calls in the core; those go behind abstract interfaces the shell implements — so the same source compiles everywhere.

---

## 114.2 WebAssembly with Emscripten

**WebAssembly (Wasm)** lets C++ run in the browser at near-native speed, compiled by **Emscripten** (an LLVM-based toolchain) to a `.wasm` binary callable from JavaScript.

```cpp
// Min standard: C++11 + Emscripten (non-portable toolchain). A function exported to JavaScript.
#include <emscripten/emscripten.h>
extern "C" {
    EMSCRIPTEN_KEEPALIVE                 // prevent the linker from stripping it (it's "unused" in C++)
    int add(int a, int b) { return a + b; }   // callable from JS as Module._add(a, b)
}
```
```bash
# emcc main.cpp -o index.html -s WASM=1     # compile C++ to a .wasm + JS glue + HTML harness
```
*Listing 114.1 — A C++ function exported to JavaScript via Emscripten. `extern "C"` + `EMSCRIPTEN_KEEPALIVE` expose it.*

> **Why this matters / cost model.** WebAssembly brings C++'s performance to the browser, where JavaScript's dynamic typing and GC previously imposed a ceiling — enabling in-browser video editing, CAD, games, and scientific tools that would be too slow in JS. The mechanism reuses concepts from Chapter 111: `extern "C"` exposes functions under unmangled names, and the JS↔Wasm boundary has crossing overhead (and only passes numeric types directly — strings/objects need marshalling through linear memory), so the idiom is again to do *bulk* work per call. Wasm's constraints: it runs in a sandbox (no direct OS/filesystem — Emscripten emulates a virtual FS), has a linear memory model, and historically lacked threads (now available via SharedArrayBuffer). The cost is a larger download than equivalent JS and the marshalling overhead — worth it when the compute justifies native speed.

---

## 114.3 Mobile C++: Android NDK and iOS

On **Android**, the **NDK** (Native Development Kit) lets you write performance-critical code in C++ and call it from Java/Kotlin via **JNI** (Chapter 111). On **iOS**, C++ integrates directly through **Objective-C++** (`.mm` files mix C++ and Objective-C) or a C interface consumed by Swift.

```cpp
// Min standard: C++11 + Android NDK/JNI (non-portable). A native method called from Kotlin/Java.
#include <jni.h>
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapp_MainActivity_stringFromJNI(JNIEnv* env, jobject /* this */) {
    return env->NewStringUTF("Hello from C++");
}
```
*Listing 114.2 — An Android NDK native method exposed to Kotlin via JNI. Non-portable.*

> **Why this matters / cost model.** Mobile is where the portable-core pattern delivers most: a shared C++ engine (often the *same* one compiled to Wasm for web) powers both the Android and iOS apps, with platform-specific UI in Kotlin/Compose and Swift/SwiftUI calling into it. The boundary hazards are exactly those of Chapter 111 — JNI's thread-local `JNIEnv*` and local-reference leaks on Android, and ownership/lifetime across the Objective-C ARC boundary on iOS. The cost model again favours coarse-grained crossings (do meaningful work per native call, not chatty per-element calls) because each language transition has overhead. The payoff is one optimised codebase for the core logic across both mobile platforms plus the web.

---

## 114.4 Portability Disciplines

Writing genuinely portable C++ requires avoiding the assumptions that differ across platforms:

- **Sizes and layout** — `int` is not always 32-bit, `long` differs between Windows (32-bit) and Linux (64-bit); use fixed-width types (`int32_t`, `int64_t`) at boundaries and in serialization.
- **Endianness** — x86/ARM are little-endian, but network protocols and some hardware are big-endian; convert explicitly (Chapters 84, 112).
- **Alignment and padding** — struct layout varies; do not rely on it across platforms without `#pragma pack` or explicit serialization.
- **Filesystem and paths** — `/` vs `\`, case sensitivity; use `std::filesystem` (C++17) for portability.
- **Standard library completeness** — not every platform (especially embedded/Wasm) provides the full standard library, threads, or exceptions.

> **Why this matters.** Portability bugs are *latent*: code that works perfectly on the development platform fails on another because of an implicit assumption — a `long` that changed size, a struct laid out differently, an endianness mismatch in a serialized format (Chapter 84). The discipline is to *make the assumptions explicit*: fixed-width integer types in any data that crosses a platform boundary or the wire, explicit endianness conversion, `std::filesystem` for paths, and feature-testing (`__has_include`, `__cpp_*` macros) rather than assuming library availability. Test on *all* target platforms in CI — a cross-platform codebase that only builds on one platform's CI is cross-platform in aspiration only.

---

## 114.5 Conditional Compilation and Its Hazards

Platform-specific code is selected with the preprocessor, but conditional compilation is a hazard if overused.

```cpp
// Min standard: C++17. Isolating platform code behind a clean interface.
#if defined(_WIN32)
    // Windows-specific implementation
#elif defined(__linux__)
    // Linux-specific implementation
#elif defined(__APPLE__)
    // macOS/iOS-specific implementation
#endif
// Better: define a portable interface, and put each #if in ONE small .cpp per platform.
```
*Listing 114.2b — Conditional compilation. Concentrate it behind an interface rather than scattering `#if` everywhere.*

> **Why this matters.** Scattered `#ifdef`s are a maintenance disaster: the code becomes unreadable, each platform's path is only compiled (and tested) on that platform so the others rot, and a change must be made in many places. The discipline mirrors the portable-core pattern at the file level: define a *platform-neutral interface* (an abstract class or a set of function declarations) and implement it in *one small file per platform* (`platform_win.cpp`, `platform_linux.cpp`), selected by the build system (Chapter 110) rather than by `#if` inside shared logic. This keeps the bulk of the code platform-free and confines the unavoidable platform differences to thin, separately-tested files.

> **The discipline.** Cross-platform C++ is the portable-core architecture applied rigorously: write the performance-critical logic once in standard, platform-free C++, and adapt it per target through thin shells — JNI/Objective-C++ for mobile, Emscripten for the web, native UI on desktop. Make every platform assumption explicit (fixed-width types, endianness, `std::filesystem`), confine platform code to per-platform files behind clean interfaces, and test on every target in CI. The reward is C++'s defining superpower: one optimised engine running everywhere from a browser tab to a phone to a server. The next chapter covers one of those targets in depth — the desktop GUI.
