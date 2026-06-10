# High-Level URL and HTTP Handling (`urllib`, `http`)


While low-level sockets (Chapter 38) are for systems plumbing, most application-level networking uses HTTP. Python provides a layered suite of modules to handle URLs and the HTTP protocol state machine.

### 54.1 `urllib.parse`: The URL State Machine

A URL is not just a string; it is a complex address with hierarchy and parameters (RFC 3986).
*   **`urlparse()`**: Breaks a string into a 6-item named tuple (`scheme`, `netloc`, `path`, `params`, `query`, `fragment`).
*   **Safety**: Modern CPython has hardened `urllib.parse` to prevent "domain name splitting" attacks where special Unicode characters (like `\uff01`) are used to trick servers into misrouting requests.

### 54.2 `http.client`: The Protocol Engine

`http.client` is the lowest level of HTTP handling before raw sockets.
*   **Persistence**: It supports HTTP/1.1 persistent connections (`Connection: keep-alive`).
*   **Streaming**: You can send and receive request bodies in chunks using the `chunked` transfer encoding, which is essential for uploading large files without consuming all system memory.

### 54.3 `urllib.request`: The Opener Pipeline

`urllib.request` provides a high-level API built on an extensible "Handler" architecture.
1.  **Handlers**: Objects that handle specific schemes (`HTTPHandler`, `FTPHandler`, `FileHandler`).
2.  **Opener**: The `OpenerDirector` manages a list of handlers. When you call `urlopen()`, it iterates through handlers until one accepts the request.
3.  **Hooks**: You can write custom handlers to implement caching, authentication, or automatic retry logic.

---
