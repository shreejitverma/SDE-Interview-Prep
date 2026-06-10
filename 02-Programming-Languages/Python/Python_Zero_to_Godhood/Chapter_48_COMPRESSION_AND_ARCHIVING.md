# Chapter 48: The DEFLATE Algorithm and Zlib (`zlib`, `gzip`)

Data compression is a cornerstone of modern systems engineering, reducing storage costs and network latency. Python's `zlib` and `gzip` modules provide the foundational tools for the DEFLATE algorithm.

### 48.1 `zlib`: The C-Level Compression Engine

The `zlib` module is a direct wrapper around the widely-used zlib C library. It implements the **DEFLATE** algorithm, which combines Huffman coding with LZ77 compression.

#### 1. Compression Objects and Flushing
For streaming data, you use `compressobj()` and `decompressobj()`.
*   **`flush()`**: This is critical for network protocols. It forces the compressor to output all pending data, potentially starting a new Huffman block.
*   **GIL Management**: The `zlib` module **releases the GIL** during compression and decompression. This allows multiple threads to compress separate data streams in parallel, making it highly effective for multi-core web servers.

#### 2. Adler-32 vs. CRC32
`zlib` uses Adler-32 checksums for integrity checks, which are faster to calculate than CRC32 but slightly less robust.

### 48.2 `gzip`: The File Format Wrapper

`gzip` provides a file-like interface for reading and writing `.gz` files.
*   **Internals**: It adds a 10-byte header (including timestamp and OS metadata) and an 8-byte trailer (CRC32 and original size) around a raw `zlib` DEFLATE stream.
*   **Random Access**: Standard `gzip` files do not support efficient random access (seeking). To seek to the end, the entire file must be decompressed.

---

# Chapter 49: Advanced Compression (`bz2`, `lzma`)

For higher compression ratios at the cost of CPU time and memory, Python provides modules for Bzip2 and LZMA.

### 49.1 `bz2`: Burrows-Wheeler Transform

The `bz2` module implements the Bzip2 algorithm.
1.  **Run-Length Encoding (RLE)**: Collapses repeated characters.
2.  **Burrows-Wheeler Transform (BWT)**: A block-sorting algorithm that groups similar characters together, making them easier to compress with move-to-front and Huffman coding.
3.  **Memory Usage**: Unlike `zlib`, `bz2` requires significant memory (up to 7.5 MB for the 900k block size) during compression.

### 49.2 `lzma`: High-Ratio Compression (7-Zip)

The `lzma` module (added in Python 3.3) provides support for the LZMA (Lempel-Ziv-Markov chain algorithm) and XZ formats.
*   **Compression Ratio**: LZMA typically achieves much better compression than Gzip or Bzip2.
*   **Complexity**: The algorithm is extremely CPU-intensive and requires significant memory for its "dictionaries" (often hundreds of megabytes).
*   **Godhood Detail**: Use `lzma.PRESET_EXTREME` only if you have plenty of RAM and time. For most high-performance systems, the default preset or `zlib` is a better trade-off between speed and size.

---

# Chapter 50: Archive Formats (`zipfile`, `tarfile`)

Archives group multiple files into a single container, often with compression.

### 50.1 `zipfile`: The Directory-at-the-End Architecture

The ZIP format stores its **Central Directory** at the *end* of the file.
*   **Random Access**: This design allows a program to read the directory once and then jump (seek) to any file within the archive without reading the whole file.
*   **Encryption**: `zipfile` supports password-protected archives (Legacy ZIP encryption), but it is computationally weak. For modern security, use an external library like `pycryptodome` for AES-256.

### 50.2 `tarfile`: The Tape Archive Heritage

Originally designed for magnetic tapes, the TAR format is a simple concatenation of files with a 512-byte header for each.
*   **No Central Directory**: To find a file at the end of a `.tar` archive, you must read all previous headers.
*   **Compression**: `.tar.gz` or `.tar.xz` are created by piping the output of the TAR stream into a compressor. `tarfile` handles this transparently via its `mode` argument (e.g., `'w:gz'`).
*   **Sparse Files**: `tarfile` can handle "sparse files" (files with large holes of zeros) efficiently, preserving their structure on disk without allocating physical space for the zeros.

---
