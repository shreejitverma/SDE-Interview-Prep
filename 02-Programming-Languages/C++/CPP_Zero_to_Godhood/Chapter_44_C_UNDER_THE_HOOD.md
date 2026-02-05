# C++ UNDER THE HOOD


To truly master C++, you must understand what the compiler generates.

### 14.1 Object Layout & ABI (Itanium C++ ABI)
How does `virtual` work?

```cpp
class Base {
    int64_t id;
public:
    virtual void func() {}
};

class Derived : public Base {
    int64_t data;
public:
    void func() override {}
};
```

**Memory Layout (64-bit system):**
```text
[ vptr (8 bytes) ] -> [ vtable for Base ]
[ id   (8 bytes) ]
```
For `Derived`:
```text
[ vptr (8 bytes) ] -> [ vtable for Derived ]
[ id   (8 bytes) ]
[ data (8 bytes) ]
```
*   **vptr**: Hidden pointer added to classes with virtual functions.
*   **vtable**: Static table of function pointers.
*   **Alignment**: Data is padded to align with word boundaries.

### 14.2 Small String Optimization (SSO)
`std::string` doesn't always allocate heap memory.

```cpp
std::string s = "Hello"; // 5 chars
// Layout typically (24-32 bytes):
// [ size (8) ] [ capacity (8) ] [ pointer (8) ]  <-- Normal mode
// [ size (1) ] [ ... chars 22 bytes ...     ]  <-- SSO mode (Union)
```
Strings shorter than 15-22 chars (depending on libc++) live entirely on the stack.

### 14.3 Return Value Optimization (RVO)
Copy elision is mandatory in C++17.

```cpp
struct BigObject { int data[1000]; };

BigObject create() {
    BigObject obj;
    // ... fill obj ...
    return obj; // No copy, no move. Constructed directly in caller's stack frame.
}

BigObject x = create();
```

---
