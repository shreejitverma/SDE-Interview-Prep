# Cryptography and Hashing (`hashlib`, `hmac`)


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


