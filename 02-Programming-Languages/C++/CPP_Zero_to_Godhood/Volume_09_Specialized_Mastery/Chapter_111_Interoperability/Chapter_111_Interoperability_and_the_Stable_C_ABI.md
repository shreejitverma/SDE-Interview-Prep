# Chapter 111: Interoperability and the Stable C ABI

C++ is the dark matter of software: it sits beneath Python data-science stacks, Java enterprise systems, game engines scripted in Lua, and services written in Rust, Go, and C#, binding them to the hardware. But every language boundary is a *contract* — about memory ownership, name mangling, object layout, and threading — and getting it wrong produces crashes, leaks, and corruption at the seam. This chapter covers crossing those boundaries safely: the linkage rules that underlie them, the stable C ABI as the universal lingua franca, and the specific hazards of Python (pybind11) and Java (JNI) interop.

## Chapter Roadmap

- 111.1 Why Interoperability Is Hard
- 111.2 Linkage and `extern "C"`
- 111.3 The Stable C ABI as Lingua Franca
- 111.4 C Incompatibilities to Watch
- 111.5 Python Bindings with pybind11
- 111.6 The Java Native Interface (JNI)
- 111.7 C++ Attributes and the Interop Discipline

---

## 111.1 Why Interoperability Is Hard

Two languages can call each other only if they agree on a **binary contract**: how functions are named in the object file, how arguments are passed, how objects are laid out in memory, who owns and frees allocations, and how errors and threads are handled. C++'s rich features — name mangling, exceptions, RAII, templates, virtual layout — are exactly what *other* languages cannot see, so the boundary must be reduced to something they can.

> **Why this matters.** Every interop bug is a *contract mismatch* at the boundary: a pointer freed on one side and used on the other (Chapter 97's use-after-free crossing a language line), a C++ exception thrown across a C boundary (undefined behaviour), a mangled name the other language cannot find, or a class layout one side assumes and the other doesn't (Chapter 102's ABI). Safe interop is the discipline of *narrowing* the boundary to a contract both sides understand — almost always the C ABI — and being explicit about ownership at every crossing.

---

## 111.2 Linkage and `extern "C"`

**Linkage** determines whether a name refers to the same entity across scopes and translation units:

- **External linkage** — referable from other TUs (non-`static` globals, non-inline functions). The default for namespace-scope functions and variables.
- **Internal linkage** — visible only within its own TU (`static` globals, anything in an anonymous namespace).
- **No linkage** — local to its scope (local variables).

**`extern "C"`** tells the C++ compiler to give a function **C-style linkage** — no name mangling — so the symbol is the plain function name (`foo`, not `_Z3foov`) and a C program (or any language speaking the C ABI) can find it.

```cpp
// Min standard: C++98. extern "C" exposes a C++ function under its plain, unmangled name.
extern "C" {
    int compute(int x);          // symbol is literally "compute" — callable from C/Rust/Go/Python ctypes
}
// Internal linkage keeps a helper private to this TU:
namespace { void helper(); }     // anonymous namespace -> internal linkage
```
*Listing 111.1 — `extern "C"` disables mangling; anonymous namespaces give internal linkage.*

> **Why this matters.** `extern "C"` is the single most important interop keyword: it is how you expose a C++ implementation under a name *other languages can link to*, because every FFI mechanism (Python `ctypes`, Rust `extern "C"`, Go cgo, C# P/Invoke) speaks the C ABI and cannot demangle C++ names. The constraint it imposes is that an `extern "C"` function cannot be overloaded (C has no overloading, so there is only one symbol per name) and should not let C++ exceptions escape it (Chapter 104). Internal linkage (anonymous namespaces) is the complement — it *hides* implementation symbols from the boundary, shrinking the exported surface (Chapter 102's visibility).

---

## 111.3 The Stable C ABI as Lingua Franca

To speak to Rust, C#, Go, Swift, or Python, expose a **stable C ABI** — the lowest-common-denominator contract every language can call. The rules:

- **`extern "C"`** on every exported function (no mangling).
- **Standard-layout types** at the boundary — plain structs with no virtual functions, no private/public mixing, no base classes — so their memory layout is predictable and matches what the other language declares.
- **Explicit ownership** — pass raw pointers and document who frees them; provide paired `create`/`destroy` functions rather than expecting the other language to call C++ `delete`.
- **No exceptions across the boundary** — catch everything at the `extern "C"` layer and convert to error codes or out-parameters.
- **Opaque handles** — hide C++ objects behind an opaque pointer (`typedef struct Foo Foo;`) so the other language never sees the layout.

```cpp
// Min standard: C++11. A stable C ABI wrapping a C++ class with opaque handles + paired lifetime.
extern "C" {
    typedef struct Engine Engine;                 // opaque to callers
    Engine* engine_create();                       // factory
    int     engine_process(Engine* e, const char* in, char* out, int out_len);  // error code, no exceptions
    void    engine_destroy(Engine* e);             // paired destructor
}
// Implementation (C++ side) catches all exceptions and returns error codes.
```
*Listing 111.2 — A stable C ABI: opaque handle, paired create/destroy, error codes instead of exceptions.*

> **Why this matters.** The C ABI is the *only* contract every systems language agrees on, which is why it is the universal interop layer — a C++ library wrapped in a clean C ABI can be called from virtually anything. The opaque-handle + paired-lifetime pattern is what makes it *safe*: the caller never sees the C++ object's layout (so it survives ABI changes, Chapter 102) and never tries to free it with the wrong allocator (the C++ side owns construction *and* destruction). This same pattern is what Pimpl (Chapter 107) does internally; here it crosses a *language* boundary instead of a *compilation* boundary. Designing this thin, stable C surface is the foundational interop skill — everything else (pybind11, JNI) is a higher-level convenience over it.

---

## 111.4 C Incompatibilities to Watch

C++ is *mostly* a superset of C, but several differences bite when mixing them:

- **`void*` conversions** — C allows implicit `void* → T*`; C++ requires an explicit cast. C headers that rely on implicit conversion may not compile as C++.
- **Struct names** — in C, `struct Foo` requires the `struct` keyword on use (unless `typedef`'d); in C++ the name alone is a type.
- **Empty parameter lists** — `int f()` in C means "unspecified arguments"; in C++ it means "no arguments" (`int f(void)`). A C function declared `f()` and called with arguments is legal C but wrong in C++.
- **Stricter type rules** — C++ forbids many implicit narrowing conversions and enum-to-int laxity that C permits.

> **Why this matters.** These differences are why a C header may not compile cleanly in C++ and why the same source can mean different things in each language. When consuming a C library, wrap its header in `extern "C" { #include <clib.h> }` so its functions get C linkage, and be aware that the C header's idioms (implicit `void*` casts, `f()` meaning variadic) follow C rules. When *writing* a header meant for both, restrict yourself to the common subset. These are small traps individually, but at a language boundary a single one — `f()` interpreted as variadic, a struct layout assumed identical — corrupts the call.

---

## 111.5 Python Bindings with pybind11

**pybind11** is the modern way to expose C++ to Python — a header-only library that maps C++ classes and functions to Python objects with minimal boilerplate.

```cpp
// Min standard: C++14 + pybind11 (non-portable: requires the library and Python).
#include <pybind11/pybind11.h>
namespace py = pybind11;
struct Pet {
    std::string name;
    explicit Pet(std::string n) : name(std::move(n)) {}
};
PYBIND11_MODULE(example, m) {                       // defines the Python module `example`
    py::class_<Pet>(m, "Pet")
        .def(py::init<std::string>())
        .def_readwrite("name", &Pet::name);
}
```
*Listing 111.3 — pybind11: a C++ class exposed to Python. Non-portable (requires pybind11 + Python).*

> **Why this matters / cost model.** The central interop problem here is **ownership across the GC boundary**: Python objects are reference-counted (`PyObject*`), C++ uses RAII, and the question "who frees this pointer" has no default answer. pybind11's **return-value policies** make it explicit: `copy` (Python gets its own copy — safe), `reference` (Python references C++-owned memory — dangling if C++ frees it), `take_ownership` (Python's GC will `delete` it). Choosing wrong is a use-after-free or a double-free across the language line. The cost model also matters: every crossing of the Python/C++ boundary has overhead (argument conversion, GIL acquisition), so the idiom is to do *bulk* work in C++ per call (pass a whole array, not one element at a time) rather than chatty fine-grained crossings — the same "amortise the boundary" principle as syscalls (Chapter 98). This is exactly how NumPy, PyTorch, and TensorFlow work: a thin Python API over heavy C++ engines (Chapter 117).

---

## 111.6 The Java Native Interface (JNI)

**JNI** is the bridge from C++ to the JVM (enterprise systems and Android). It is powerful but unforgiving.

```cpp
// Min standard: C++11 + JNI (non-portable). A native method callable from Java.
#include <jni.h>
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_MyClass_nativeMethod(JNIEnv* env, jobject thiz) {
    return env->NewStringUTF("Hello from C++");      // creates a LOCAL reference
}
```
*Listing 111.4 — A JNI native method. Note `extern "C"` (no mangling) and the `JNIEnv*` parameter. Non-portable.*

> **Why this matters / cost model.** JNI's hazards are specific and severe. **Boundary cost:** crossing into the JVM is expensive (object pinning, reference bookkeeping), so — again — batch work per call rather than crossing per element. **Thread-locality:** `JNIEnv*` is *thread-local*; sharing it between threads is undefined behaviour, so each native thread must attach to the JVM to get its own. **Local-reference leaks:** JNI creates a *local reference* for every object returned (like `NewStringUTF` above); in a loop that creates many references, failing to `DeleteLocalRef` exhausts the local reference table and the JVM throws or crashes. These are not C++ memory bugs — they are *JVM* resource bugs that manifest in C++ code, and they are why JNI code must be written with constant awareness of the JVM's object model. Note the `extern "C"` and the mangled-into-the-name `Java_com_example_MyClass_nativeMethod` convention: JNI finds native methods by an unmangled, fully-qualified C name.

---

## 111.7 C++ Attributes and the Interop Discipline

C++ **attributes** (`[[...]]`) give the compiler standardized hints, several relevant to robust boundary code:

- `[[nodiscard]]` — warn if a return value (e.g. an error code from a C-ABI function) is ignored.
- `[[maybe_unused]]` — suppress unused-variable warnings (common with platform-conditional code).
- `[[deprecated("reason")]]` — mark an obsolete API at a stable boundary.
- `[[noreturn]]`, `[[fallthrough]]`, `[[likely]]`/`[[unlikely]]` (C++20) — control-flow and optimization hints.

```cpp
// Min standard: C++17. Attributes that harden a boundary API.
[[nodiscard]] int engine_process(Engine*, const char* in, char* out, int len);  // ignoring the error code warns
[[deprecated("use engine_process_v2")]] int engine_process_old(Engine*);
```
*Listing 111.5 — `[[nodiscard]]` forces callers to check an error code; `[[deprecated]]` manages API evolution.*

> **The discipline.** Interoperability is the discipline of reducing C++'s rich, mangled, exception-throwing, RAII-managed world to a *narrow, explicit contract* the other side can honour — and the C ABI is that contract. The rules compose into one practice: expose `extern "C"` functions over standard-layout types and opaque handles; make ownership explicit with paired create/destroy and documented return-value policies; never let exceptions cross the boundary; batch work per crossing because every boundary call has overhead; and respect the *other* runtime's rules (Python's refcounting, the JVM's thread-local environment and local references). `[[nodiscard]]` and friends harden the contract against careless callers. Master this and C++ becomes what it already is in practice — the engine beneath every other language. The next chapters explore the domains those engines power, starting with networking.
