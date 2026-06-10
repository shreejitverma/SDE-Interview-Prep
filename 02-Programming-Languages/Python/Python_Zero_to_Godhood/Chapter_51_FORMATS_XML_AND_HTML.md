# Chapter 51: Delimited and Configuration Files (`csv`, `configparser`)

Handling structured data from diverse sources is a primary use case for Python. The `csv` and `configparser` modules offer standardized ways to interact with these common formats, with the former being highly optimized for performance.

### 51.1 `csv`: The C-Level Dialect Engine

The `csv` module is not a simple string-splitter. It uses a sophisticated **Dialect** system to handle the myriad ways CSV files are quoted, escaped, and delimited.

#### 1. The `_csv` C Extension
In CPython, the heavy lifting is done in `Modules/_csv.c`.
*   **Speed**: By performing the parsing in C, it avoids the overhead of creating millions of Python string objects for every field until they are actually needed.
*   **State Machine**: The C parser is a state machine that tracks whether it is currently inside a quoted field, whether the next character is an escape character, etc.

#### 2. Dialects and `Sniffer`
*   **`register_dialect()`**: Allows you to define custom formatting (e.g., pipe-delimited, tab-delimited with backslash escapes).
*   **`csv.Sniffer`**: Analyzes a sample of the text to guess the delimiter and quoting rules automatically.

### 51.2 `configparser`: INI File Mechanics

`configparser` handles configuration files in the Windows INI format.
*   **Mapping Interface**: `ConfigParser` objects behave like a dictionary of dictionaries.
*   **Interpolation**: Supports dynamic value substitution (e.g., `path = %(base_dir)s/logs`).
*   **Internals**: It uses regular expressions to parse sections and keys. While slower than the `csv` module's C parser, it offers much more flexibility for human-readable configuration.

---

# Chapter 52: XML Processing and Expat (`xml.etree`, `xml.sax`)

XML is a verbose but highly structured format. Python provides several ways to process it, balancing ease of use with memory efficiency.

### 52.1 `xml.etree.ElementTree`: The High-Level Engine

`ElementTree` is the recommended way to handle XML in Python.

#### 1. The C-Accelerator: `_elementtree`
Since Python 3.3, `ElementTree` is automatically backed by a C implementation (`_elementtree.c`).
*   **Memory Efficiency**: It uses a compact C representation for the element tree, significantly reducing memory usage compared to pure Python DOM implementations.
*   **XPath Support**: Provides a subset of XPath for searching elements.

#### 2. The Expat Parser
Under the hood, Python uses the **Expat** library (an stream-oriented XML parser written in C).
*   **Streaming**: Expat does not build a tree in memory; it calls callbacks as it encounters tags. `ElementTree` uses these callbacks to build its internal tree structure efficiently.

### 52.2 `xml.sax`: Event-Driven Parsing

SAX (Simple API for XML) is an event-driven alternative to the tree-based `ElementTree`.
*   **When to use?**: When you need to parse a multi-gigabyte XML file that won't fit in RAM.
*   **Internals**: It wraps the Expat parser directly, allowing you to define a `ContentHandler` that processes tags as they appear in the stream.

---

# Chapter 53: HTML Parsing and Internet Data (`html`, `email`)

Interacting with web and mail systems requires robust handling of semi-structured and often non-compliant data.

### 53.1 `html.parser`: The SGML-Style Parser

The `html.parser` module is a structured markup parser that is more forgiving than XML parsers.
*   **Internals**: It uses a state-driven approach to identify tags and entities. It can handle "broken" HTML (e.g., missing closing tags) by following standardized tag-balancing rules.
*   **Security**: Always use `html.escape()` when outputting user data to prevent Cross-Site Scripting (XSS) vulnerabilities.

### 53.2 `email`: The Recursive Object Tree

The `email` package is a massive framework for managing email messages, which are fundamentally recursive structures (a message can contain a multipart message, which contains an attachment, etc.).

#### 1. The `Message` Object
An `email.message.EmailMessage` object consists of:
*   **Headers**: A dictionary-like mapping of field names to values.
*   **Payload**: Either a string (for simple text) or a list of `Message` objects (for multipart).

#### 2. Policy and Content Management
Modern Python (3.6+) introduced the **Policy** system.
*   **`policy.default`**: Uses the modern "Godhood" approach—handling Unicode, binary attachments, and folded headers automatically according to the latest RFCs (5322, 6532).
*   **Lazy Loading**: The `BytesParser` can lazily parse attachments, only reading them from the disk when the content is actually requested.

---
