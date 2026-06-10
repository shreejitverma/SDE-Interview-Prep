# Appendix K: The Complete Python Standard Library Reference Table


This appendix provides a definitive reference for the entire Python 3.13 Standard Library. For each module, we list its primary purpose, its underlying implementation (C vs. Python), and its thread-safety characteristics.

### K.1 Core Runtime and Text Processing
| Module | Implementation | Purpose | Thread Safe? |
| :--- | :--- | :--- | :--- |
| `builtins` | C | Core language objects and functions. | Yes |
| `sys` | C | Interpreter configuration and hooks. | Yes |
| `re` | C (SRE) | Regular expression engine. | Yes |
| `string` | Python/C | String formatting and constants. | Yes |
| `textwrap` | Python | Word wrapping and filling. | Yes |
| `unicodedata`| C | Unicode character database. | Yes |
| `stringprep` | Python | Internet string preparation. | Yes |

### K.2 Data Types and Collections
| Module | Implementation | Purpose | Thread Safe? |
| :--- | :--- | :--- | :--- |
| `collections` | C/Python | High-performance containers. | Partial |
| `heapq` | C | Min-priority queue. | No (Sync required) |
| `bisect` | C | Binary search on sorted lists. | No (Sync required) |
| `array` | C | Efficient arrays of numeric values. | No |
| `weakref` | C | Weak references and proxies. | Yes |
| `types` | Python | Dynamic type creation helpers. | Yes |
| `copy` | Python | Shallow and deep copy operations. | No |
| `enum` | Python | Support for enumerations. | Yes |

### K.3 Numeric and Mathematical
| Module | Implementation | Purpose | Thread Safe? |
| :--- | :--- | :--- | :--- |
| `math` | C | C-standard math functions. | Yes |
| `cmath` | C | Complex number math. | Yes |
| `decimal` | C (decNumber) | Correctly-rounded decimal math. | Yes (Context local)|
| `fractions` | Python | Rational number arithmetic. | Yes |
| `random` | Python/C | PRNG (Mersenne Twister/PCG64). | No |
| `statistics` | Python | Mathematical statistics functions. | Yes |

### K.4 File and Directory Handling
| Module | Implementation | Purpose | Thread Safe? |
| :--- | :--- | :--- | :--- |
| `os.path` | Python | Platform-independent path manipulation.| Yes |
| `pathlib` | Python | Object-oriented filesystem paths. | Yes |
| `tempfile` | Python | Generate temporary files and dirs. | Yes |
| `shutil` | Python | High-level file operations (copy/move).| No |
| `stat` | Python | Interpret `os.stat()` results. | Yes |

### K.5 Data Persistence and Compression
| Module | Implementation | Purpose | Thread Safe? |
| :--- | :--- | :--- | :--- |
| `pickle` | C | Object serialization. | No |
| `copyreg` | Python | Registry for `pickle`. | Yes |
| `sqlite3` | C | SQLite database engine. | Partial (Shared) |
| `zlib` | C | Deflate compression. | Yes (Releases GIL) |
| `gzip` | Python/C | Gzip file support. | Yes |
| `bz2` | C | Bzip2 compression. | Yes |
| `lzma` | C | LZMA/XZ compression. | Yes |
| `zipfile` | Python | ZIP archive handling. | No |
| `tarfile` | Python | TAR archive handling. | No |

### K.6 Networking and IPC
| Module | Implementation | Purpose | Thread Safe? |
| :--- | :--- | :--- | :--- |
| `socket` | C | Low-level networking. | Yes |
| `ssl` | C | TLS/SSL encryption. | Yes |
| `select` | C | Wait for I/O completion. | Yes |
| `selectors` | Python | High-level I/O multiplexing. | Yes |
| `asyncio` | Python/C | Asynchronous I/O framework. | Single-thread only |
| `mmap` | C | Memory-mapped file support. | Partial |

### K.7 Internet Protocols
| Module | Implementation | Purpose | Thread Safe? |
| :--- | :--- | :--- | :--- |
| `email` | Python | Email and MIME handling. | No |
| `json` | C/Python | JSON encoding and decoding. | Yes |
| `urllib` | Python | URL handling and requesting. | Yes |
| `http` | Python | HTTP server/client protocols. | No |
| `ftplib` | Python | FTP client. | No |
| `smtplib` | Python | SMTP client. | No |
| `xmlrpc` | Python | XML-RPC client and server. | No |

---

**This table provides the essential "Sovereign Map" for any architect navigating the Python Standard Library.**

---
