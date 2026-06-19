# Chapter 62: Interoperability

# INTEROPERABILITY

C++ is the dark matter of the software universe: it binds everything together.

### 35.1 Python Bindings (pybind11)

Bridging the gap between Python's ease of use and C++'s raw power.
*   **The Problem:** Python objects are ref-counted (`PyObject*`). C++ has RAII. Who owns the pointer?
*   **Return Value Policies:**
    *   `py::return_value_policy::copy`: Python gets a copy (Safe).
    *   `py::return_value_policy::reference`: Python references C++ memory (Dangous if C++ deletes it).
    *   `py::return_value_policy::take_ownership`: Python takes over `delete`.

```cpp
#include <pybind11/pybind11.h>

namespace py = pybind11;

struct Pet {
    std::string name;
    Pet(const std::string &name) : name(name) { }
};

PYBIND11_MODULE(example, m) {
    py::class_<Pet>(m, "Pet")
        .def(py::init<const std::string &>())
        .def_readwrite("name", &Pet::name);
}
```

### 35.2 Java Native Interface (JNI)

The bridge to the Enterprise (and Android).
*   **Cost:** Crossing the JVM boundary is expensive (pointer chasing, pinning objects).
*   **Pitfall:** `JNIEnv*` is thread-local. Do not share it between threads.
*   **Local References:** JNI creates local refs for every object returned. If you don't `DeleteLocalRef` in a loop, the JVM OOMs.

```cpp
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_MyClass_nativeMethod(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF("Hello from C++");
}
```

### 35.3 Stable C ABI

To speak to Rust, C#, or Go, use the lingua franca: C.
*   **`extern "C"`**: Disables C++ name mangling (e.g., `_Z3foov` becomes `foo`).
*   **Struct Layout:** Use `StandardLayoutType` structs (no virtuals, all public).

