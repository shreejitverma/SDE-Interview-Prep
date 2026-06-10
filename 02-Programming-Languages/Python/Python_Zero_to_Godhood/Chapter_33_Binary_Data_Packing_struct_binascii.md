# Binary Data Packing (`struct`, `binascii`)


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
