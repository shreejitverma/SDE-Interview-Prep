# Archive Formats (`zipfile`, `tarfile`)


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


