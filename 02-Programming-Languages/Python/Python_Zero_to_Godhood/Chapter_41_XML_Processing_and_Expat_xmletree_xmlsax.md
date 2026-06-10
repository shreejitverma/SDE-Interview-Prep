# XML Processing and Expat (`xml.etree`, `xml.sax`)


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
