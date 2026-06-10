# Chapter 42: Regular Expressions Engine Internals (`re`, `sre_compile`)

Regular expressions are a language within a language. While most developers use them as black boxes, the CPython `re` module is a sophisticated engine that translates patterns into a custom bytecode executed by a specialized virtual machine.

### 42.1 The `sre` Engine Architecture

CPython's regex engine is called **sre** (Secret Rabbit Engine). It is a **backtracking-based NFA** (Non-deterministic Finite Automaton) engine.

#### 1. Compilation to `sre` Bytecode
When you call `re.compile(pattern)`, the following happens:
1.  **Parsing**: The `re` module (in Python) parses the pattern string into a tree of tokens.
2.  **Optimization**: It performs optimizations like merging adjacent literal characters into single "string" match commands.
3.  **Code Generation**: The `sre_compile` module translates this tree into a sequence of integer-based opcodes.

You can actually see these opcodes using the undocumented `re.purge()` and looking at the compiled object's `.code` attribute, or by setting `re.DEBUG` during compilation:
```python
import re
re.compile("a(b|c)d", re.DEBUG)
```
*Output (Simplified):*
```
literal 97
subpattern 1
    branch
        literal 98
    or
        literal 99
literal 100
```

#### 2. The Matcher VM (`sre_lib.h`)
The actual matching happens in C (`Modules/_sre/sre.c`). It is a recursive function that takes the bytecode and the input string.
*   **Backtracking**: When a branch fails (e.g., in `(b|c)`), the engine "backtracks" to the last save point and tries the next alternative.
*   **Performance Trap**: Because it uses backtracking, "catastrophic backtracking" (exponential time complexity) is possible with certain nested quantifiers.

### 42.2 Modern Enhancements and Python 3.11+
In Python 3.11, the `re` engine received significant performance boosts. The "atomic grouping" (`(?>...)`) and possessive quantifiers (`*+`, `++`) were added, allowing developers to explicitly disable backtracking for specific subpatterns, protecting against ReDoS (Regular Expression Denial of Service) attacks.

---

# Chapter 43: Advanced Text Processing (`string`, `textwrap`)

While `str` methods cover basic needs, the `string` and `textwrap` modules handle complex formatting and layout logic, often interacting with terminal dimensions and internationalization.

### 43.1 `string.Formatter`: The Engine of `.format()`

The `f-string` (Chapter 12) is the fastest, but `string.Formatter` is the most extensible.
*   **`parse(format_string)`**: This method returns an iterator of `(literal_text, field_name, format_spec, conversion)`.
*   **`get_value(key, args, kwargs)`**: This is the hook for custom lookup logic.
*   **Internals**: F-strings are compiled to specialized bytecode (`FORMAT_VALUE`), whereas `.format()` calls into the `string` module's C-accelerated formatting logic.

### 43.2 `textwrap`: Dynamic Layout Management

`textwrap` is essential for CLI tools that must adapt to varying terminal widths.
*   **`TextWrapper` Object**: Maintains state for `width`, `indent`, and `break_long_words`.
*   **`wrap()` vs. `fill()`**: `wrap` returns a list of strings; `fill` returns a single newline-joined string.
*   **Algorithms**: It uses a greedy algorithm to fit words into the specified width, handling edge cases like hyphenated words and double-width Unicode characters correctly.

---

# Chapter 44: Binary Data Packing (`struct`, `binascii`)

Interfacing with C libraries or binary network protocols requires precise control over memory layout, endianness, and padding.

### 44.1 `struct`: C-Structs in Python

The `struct` module converts between Python values and C structs represented as Python `bytes` objects.

#### 1. Format Strings and Alignment
*   **`i`**: 4-byte integer.
*   **`f`**: 4-byte float.
*   **`d`**: 8-byte double.
*   **Endianness**: `<` (Little-endian), `>` (Big-endian), `!` (Network/Big-endian).

#### 2. The `Struct` Class Optimization
Using `struct.pack()` repeatedly is slow because it re-parses the format string every time. The `Struct` class pre-compiles the format into a C-level object:
```python
import struct
packer = struct.Struct(">I 2s f")  # Pre-compiled
data = packer.pack(1, b"ab", 3.14)
```

### 44.2 `binascii` and `base64`: Encoding Transmissions

*   **`binascii`**: Low-level C functions for hex, base64, and CRC32/Adler32 checksums.
*   **`base64`**: High-level wrapper for RFC 4648 encodings.
*   **Godhood Detail**: `base64` in Python is extremely fast because it uses vectorized (SIMD) instructions in the underlying C library where available to process 6-bit to 8-bit conversions.

---

# Chapter 45: Cryptography and Hashing (`hashlib`, `hmac`)

Security-sensitive hashing and Message Authentication Codes (MACs) are handled by `hashlib` and `hmac`, which act as bridges to the system's OpenSSL library.

### 45.1 `hashlib`: The OpenSSL Bridge

`hashlib` provides a common interface to many different secure hash and message digest algorithms.

#### 1. Static vs. Dynamic Algorithms
*   **Guaranteed**: `sha256`, `sha512`, `md5` are always available.
*   **OpenSSL-dependent**: Algorithms like `blake2b` or `sha3` are available only if the linked OpenSSL library supports them.

#### 2. Releasing the GIL
Hashing large files can be CPU-intensive. CPython's `hashlib` implementations **release the GIL** during the `update()` call if the data is large enough. This allows true parallelism when hashing multiple files in separate threads.

### 45.2 `hmac`: Keyed-Hashing for Message Authentication

`hmac` implements the HMAC algorithm as defined by RFC 2104.
*   **Why not just `hash(key + message)`?**: Simple concatenation is vulnerable to "length-extension attacks" in certain hash functions (like MD5 and SHA-1). `hmac` uses a double-hashing nested structure to prevent this.
*   **`compare_digest(a, b)`**: Always use this function for comparing hashes/tokens. It is a **constant-time** comparison, preventing "timing attacks" where an attacker can deduce the correct token by measuring how long the comparison takes to fail.

---
