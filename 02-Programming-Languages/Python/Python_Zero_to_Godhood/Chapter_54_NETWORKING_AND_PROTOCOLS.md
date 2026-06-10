# Chapter 54: High-Level URL and HTTP Handling (`urllib`, `http`)

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

# Chapter 55: Legacy and Specialized Protocols (`ftplib`, `smtplib`, `imaplib`)

Python's strength is its "batteries included" philosophy, providing clients for nearly every major internet protocol.

### 55.1 `smtplib`: The SMTP State Machine

SMTP (Simple Mail Transfer Protocol) is a conversational protocol.
*   **The Conversation**: `EHLO` $\rightarrow$ `STARTTLS` $\rightarrow$ `AUTH` $\rightarrow$ `MAIL FROM` $\rightarrow$ `RCPT TO` $\rightarrow$ `DATA` $\rightarrow$ `QUIT`.
*   **Internals**: `smtplib` manages the socket and parses the numeric status codes (e.g., 250 OK, 550 Failure) returned by the server. It handles the transition from a plaintext connection to a secure TLS connection via the `ssl` module.

### 55.2 `ftplib`: Active vs. Passive Mode

FTP is unique because it uses two separate socket connections: one for commands (Control) and one for data.
*   **Passive Mode (Recommended)**: The client initiates the data connection to the server.
*   **Active Mode**: The server attempts to connect back to the client (often blocked by modern firewalls/NAT).
*   **Internals**: `ftplib` handles the complex choreography of listening on a temporary port and coordinating with the control socket to transfer file data.

### 55.3 `imaplib`: Mailbox Synchronization

IMAP (Internet Message Access Protocol) is much more complex than SMTP or POP3 because it is stateful and supports partial downloads.
*   **Literal Handling**: `imaplib` implements the "IMAP Literal" protocol, allowing for the transfer of large binary message parts without crashing the interpreter's string allocation system.

---

# Chapter 56: IP Address Manipulation and RPC (`ipaddress`, `xmlrpc`)

### 56.1 `ipaddress`: Vectorized Network Math

Manipulating IP ranges with regex is a recipe for security vulnerabilities. `ipaddress` provides objects for IPv4 and IPv6 addresses and networks.

#### 1. Internal Representations
*   **IPv4**: Stored as a 32-bit Python `int`.
*   **IPv6**: Stored as a 128-bit Python `int`.
*   **Performance**: Operations like `addr in network` are implemented using fast bitwise mask operations (`(addr_int & mask) == network_int`), making them extremely efficient for high-speed firewall log analysis.

### 56.2 `xmlrpc`: Simple Remote Procedure Calls

XML-RPC is a legacy but still widely used protocol for calling functions across the network.
*   **`ServerProxy`**: Uses Python's `__getattr__` dunder method to dynamically map local method calls to remote network requests.
*   **Serialization**: It uses the `xml.etree` module to convert Python types (ints, dicts, lists) into the XML format required by the protocol.

---
