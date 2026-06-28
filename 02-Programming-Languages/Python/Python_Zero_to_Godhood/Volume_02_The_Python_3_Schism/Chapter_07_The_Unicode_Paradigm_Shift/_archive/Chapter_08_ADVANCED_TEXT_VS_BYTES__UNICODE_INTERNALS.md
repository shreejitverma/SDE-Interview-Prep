# ADVANCED TEXT VS BYTES & UNICODE INTERNALS


### 8.1 The Pre-PEP 393 Memory Overhead
Historically, Python 3 represented all Unicode strings using either **UCS-2** (2 bytes per character) or **UCS-4** (4 bytes per character) wide character arrays, configured at compilation time.
*   **UCS-2 Pitfall**: Limited to characters under 65535. Characters beyond that required surrogate pairs, making indexing ($O(1)$ lookup by offset) complex.
*   **UCS-4 Pitfall**: Consumed 4 bytes per character regardless of contents. An ASCII string like `"hello"` (which requires only 5 bytes) consumed 20 bytes on the heap, wasting memory.

### 8.2 PEP 393: Flexible String Representation
To solve memory overhead, Python 3.3 introduced **PEP 393**. CPython now selects the most compact memory layout based on the string's characters:

```
PEP 393 String Header and Struct Formats:
1. PyASCIIObject:
   +-------------------------------------------------------------+
   | ob_refcnt | ob_type | length | state (ASCII=1, compact=1)  | -> Raw ASCII data (1 byte/char)
   +-------------------------------------------------------------+

2. Compact 1-Byte (Latin-1):
   +-------------------------------------------------------------+
   | ob_refcnt | ob_type | length | state (ASCII=0, compact=1)  | -> Latin-1 data (1 byte/char)
   +-------------------------------------------------------------+

3. Compact 2-Byte (UCS-2):
   +-------------------------------------------------------------+
   | ob_refcnt | ob_type | length | state (UCS-2)               | -> UCS-2 data (2 bytes/char)
   +-------------------------------------------------------------+

4. Compact 4-Byte (UCS-4):
   +-------------------------------------------------------------+
   | ob_refcnt | ob_type | length | state (UCS-4)               | -> UCS-4 data (4 bytes/char)
   +-------------------------------------------------------------+
```

*   **Runtime Memory Analysis**: We can verify this dynamic memory layout using `sys.getsizeof()`:

```python
import sys

# ASCII String: 1 byte per character + 49 bytes header
str_ascii = "hello"
print("ASCII size:", sys.getsizeof(str_ascii), "bytes") # 49 + 5 = 54

# Latin-1 String (Unicode character <= 255): 1 byte per character + 73 bytes header (non-ASCII compact)
str_latin = "hell"
print("Latin-1 size:", sys.getsizeof(str_latin), "bytes") # 73 + 5 = 78

# UCS-2 String (Unicode character <= 65535): 2 bytes per character + 73 bytes header
str_ucs2 = "hell"
print("UCS-2 size:", sys.getsizeof(str_ucs2), "bytes") # 73 + (6 * 2) = 85

# UCS-4 String (Unicode character > 65535, e.g. emoji): 4 bytes per character + 73 bytes header
str_ucs4 = "hell"
print("UCS-4 size:", sys.getsizeof(str_ucs4), "bytes") # 73 + (7 * 4) = 101
```

### 8.3 Encoding, Decoding, and Binary Buffers
*   **The Boundary**: Python strictly enforces the boundary between text (`str`, Unicode code points) and binary data (`bytes`, raw 8-bit octets).
    - `str.encode(encoding)` translates Unicode code points into raw byte sequences.
    - `bytes.decode(encoding)` decodes raw bytes back into Unicode strings.
*   **Zero-Copy Slicing via memoryview**: 
    `memoryview` objects allow sharing access to binary buffers (like `bytearray` or file buffers) without copying the underlying bytes. This is critical for high-performance networking and I/O tasks.

```python
# Zero-copy slicing demo
data = bytearray(b"system_payload_data")
view = memoryview(data)

# Slice without copying
sub_view = view[7:14]
print("Slice contents:", sub_view.tobytes())

# Modify buffer in-place
sub_view[0] = ord(b"P")
print("Modified original array:", data) # data becomes bytearray(b"system_Payload_data")
```

---

