# Low-Level Networking and Sockets


Networking in Python is built upon the foundational Berkeley Sockets API. While high-level libraries like `requests` or `httpx` are common, systems engineering requires mastery of the low-level `socket` and `ssl` modules.

### 38.1 `socket`: The Berkeley Interface

A socket is an endpoint for communication. The `socket` module provides a C-like interface to the operating system's networking stack.

#### 1. Address Families and Socket Types
*   **AF_INET / AF_INET6**: IPv4 and IPv6 networking.
*   **AF_UNIX**: Unix Domain Sockets (local IPC, faster than network sockets as they skip the TCP/IP stack).
*   **SOCK_STREAM**: TCP (reliable, connection-oriented).
*   **SOCK_DGRAM**: UDP (unreliable, connectionless).

#### 2. The Lifecycle of a Server Socket
1.  **`socket()`**: Create the socket descriptor.
2.  **`bind()`**: Associate the socket with an address and port.
3.  **`listen()`**: Enable the socket to accept connections (sets the backlog size).
4.  **`accept()`**: Block until a client connects. Returns a **new** socket object specifically for that connection.

#### 3. Blocking vs. Non-blocking
By default, sockets are blocking. Setting `sock.setblocking(False)` makes `send` and `recv` return immediately, raising `BlockingIOError` if no data is available. This is the foundation for multiplexing (as seen in Chapter 10).

### 38.2 `ssl`: Secure Communication

The `ssl` module wraps OpenSSL to provide TLS/SSL encryption.

#### 1. `SSLContext`
This object stores configuration (certificates, cipher suites, protocol versions).
*   **Certificate Verification**: `context.verify_mode` ensures the server's identity is valid against a CA bundle.
*   **ALPN/SNI**: Support for modern TLS features like Application-Layer Protocol Negotiation (used for HTTP/2) and Server Name Indication.

#### 2. Wrapping Sockets
You don't create an SSL socket directly; you "wrap" an existing TCP socket:
```python
conn = context.wrap_socket(raw_sock, server_hostname="example.com")
```
This triggers the TLS handshake process.

### 38.3 `mmap`: Memory-Mapped Files

`mmap` allows you to map a file directly into the process's virtual memory space.

#### 1. Why use `mmap`?
*   **Performance**: Reading from an `mmap` object is often faster than standard `read()` calls because it avoids copying data from kernel space to user space (Zero-copy).
*   **IPC**: Multiple processes can map the same file. Changes made by one process are immediately visible to others, providing a high-speed shared memory mechanism.

#### 2. Interface
`mmap` objects behave like both a bytearray and a file. They support slicing, regex searching, and standard `read`/`write` methods.

### 38.4 Performance Optimizations: `sendfile`

For high-performance file serving, Python provides `os.sendfile`.
*   **Zero-Copy**: It instructs the kernel to copy data directly from a file descriptor (disk) to a socket descriptor (network) without the data ever entering the Python interpreter's memory. This drastically reduces CPU usage and memory bandwidth for static file delivery.

---


---

