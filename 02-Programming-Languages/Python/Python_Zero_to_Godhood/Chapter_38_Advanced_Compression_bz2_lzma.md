# Advanced Compression (`bz2`, `lzma`)


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
