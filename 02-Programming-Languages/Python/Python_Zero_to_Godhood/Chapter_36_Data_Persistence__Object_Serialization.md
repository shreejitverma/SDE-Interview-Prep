# Data Persistence & Object Serialization


Data persistence allows Python objects to survive the termination of the process. This involves serialization (converting an object to a byte stream) and storage.

### 36.1 `pickle`: The Virtual Stack Machine

The `pickle` module is Python's native serialization format. Unlike JSON, it can serialize almost any Python object (including classes, functions, and complex circular references).

#### 1. The Pickle Protocol
`pickle` doesn't just store data; it stores a **program** that, when executed by the pickle virtual machine, reconstructs the object.
*   **Opcodes**: A pickle stream consists of a series of opcodes (e.g., `PROTO`, `EMPTY_DICT`, `SETITEM`, `STOP`).
*   **The Stack**: The pickle VM uses a stack to build objects. For example, to create a list, it might push several items and then call an opcode that pops them into a new list object.

#### 2. Security Warning: `__reduce__`
When an object is unpickled, the VM may execute arbitrary code. The `__reduce__` method allows an object to define exactly how it should be reconstructed, which can be exploited to execute shell commands. **Never unpickle data from an untrusted source.**

### 36.2 `json`: The Universal Exchange Format

The `json` module provides a standard way to serialize basic Python types (dicts, lists, strings, numbers) into a format readable by almost any language.

#### 1. C Optimization: `_json`
In CPython, the `json` module is backed by a C extension (`_json.c`).
*   **Encoding**: Iterates through Python objects and builds a C string buffer.
*   **Decoding**: Uses a fast scan-based parser to identify JSON tokens and convert them to Python objects.

#### 2. Limitations
`json` cannot handle complex Python objects, circular references, or non-string keys in dictionaries. For these, custom `JSONEncoder` and `JSONDecoder` subclasses are required.

### 36.3 `sqlite3`: The Embedded Database

Python comes with a complete SQL database engine: SQLite.

#### 1. The C Extension Architecture
The `sqlite3` module is a wrapper around the SQLite C library.
*   **Connection and Cursor**: These are C-level objects that manage the database file and the result set pointers.
*   **GIL Management**: The `sqlite3` module releases the GIL during long-running SQL queries, allowing other Python threads to run while the database is processing I/O or complex joins.

#### 2. Type Mapping
The module automatically maps Python types to SQL types (e.g., `int` to `INTEGER`, `str` to `TEXT`). You can register custom adapters and converters to handle complex types like `datetime` or even `pickle` objects.

### 36.4 `dbm` and `shelve`: Simple Key-Value Stores

*   **`dbm`**: Provides an interface to Unix "database manager" libraries (like GDBM or Berkeley DB). It stores string keys and values in a disk-based hash table.
*   **`shelve`**: A wrapper around `dbm` that uses `pickle` to serialize the values. This allows you to treat a disk file as a persistent Python dictionary.

---


---