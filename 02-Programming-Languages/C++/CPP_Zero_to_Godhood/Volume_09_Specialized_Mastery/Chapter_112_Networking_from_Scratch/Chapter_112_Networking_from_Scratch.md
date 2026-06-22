# Chapter 112: Networking from Scratch

Every C++ networking library — Asio, gRPC, Seastar — is ultimately a wrapper over the operating system's **Berkeley sockets** API, and you cannot reason about a network library's performance or debug its failures without understanding the syscalls underneath. This chapter builds networking from the sockets API up: the blocking server, the readiness model (`epoll`) that lets one thread serve tens of thousands of connections, and the path from there to the kernel-bypass techniques of Volume 8. The throughline is the cost model: networking performance is dominated by syscall crossings and copies, exactly as Chapter 98–99 established.

## Chapter Roadmap

- 112.1 Why Learn the Sockets API
- 112.2 The Berkeley Sockets API
- 112.3 The Blocking Server and Its Limits
- 112.4 Non-Blocking I/O and `epoll`
- 112.5 The Event Loop and the Reactor Pattern
- 112.6 From `epoll` to the Modern Stack

---

## 112.1 Why Learn the Sockets API

The **Berkeley (BSD) sockets** API is the foundation of the Internet — the set of system calls (`socket`, `bind`, `listen`, `accept`, `read`, `write`) through which every program sends and receives data over a network. High-level libraries hide it, but understanding it is what lets you reason about their behaviour.

> **Why this matters.** Asio's "asynchronous read completed" and gRPC's "stream backpressure" are abstractions over `epoll`/`io_uring` readiness and socket buffer fullness; when they misbehave — a connection hangs, throughput plateaus, latency spikes — the diagnosis lives in the sockets layer (a full send buffer, a blocking accept, a syscall per message). Every networking cost model in this book reduces to the sockets API: a `read`/`write` is a syscall (Chapter 98) plus a kernel↔user copy (Chapter 99), and the entire art of high-performance networking is amortising or eliminating those.

---

## 112.2 The Berkeley Sockets API

A TCP server follows a fixed sequence: create a socket, bind it to an address/port, listen for connections, accept each one, then read/write on the accepted connection.

```cpp
// Min standard: C++11 + POSIX (non-portable: Linux/Unix). A minimal blocking TCP echo server.
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);   // TCP socket

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);                     // host-to-network byte order

    bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    listen(server_fd, 16);                              // backlog of pending connections

    int conn = accept(server_fd, nullptr, nullptr);     // BLOCKS until a client connects
    char buffer[1024] = {};
    read(conn, buffer, sizeof(buffer));                 // BLOCKS until data arrives

    const char* resp = "HTTP/1.1 200 OK\r\nContent-Length: 6\r\n\r\nHello!";
    write(conn, resp, std::strlen(resp));

    close(conn);
    close(server_fd);
    return 0;
}
```
*Listing 112.1 — A blocking TCP server. Each step is a syscall; `htons` handles network byte order. POSIX-specific.*

> **Why this matters.** Two details are easy to get wrong and instructive. **Byte order:** `htons`/`htonl` convert host byte order to network (big-endian) order — forgetting them is a classic bug where the port or address is interpreted wrong (the serialization concern of Chapter 84). **The backlog** in `listen` bounds how many connections can queue before being accepted; too small and connections are refused under load. Each call here — `socket`, `bind`, `listen`, `accept`, `read`, `write`, `close` — is a syscall crossing the kernel boundary (Chapter 98), which is why naive per-message networking is syscall-bound.

---

## 112.3 The Blocking Server and Its Limits

The server in Listing 112.1 handles *one* connection because `accept` and `read` **block** — they sleep the thread until something happens. The naive fix is one thread per connection, but that does not scale: each thread is an ~8 MB stack and a scheduling entity, so a few thousand connections exhaust memory and the scheduler thrashes (Chapter 78).

> **Why this matters / cost model.** Blocking I/O is simple and fine for a handful of connections, but the **C10K problem** (handling 10,000 concurrent connections) cannot be solved by thread-per-connection — the per-thread overhead and context-switching cost dominate. The two scalable answers diverge here: keep blocking but use *coroutines* (cheap user-space "threads," Chapter 78) so each connection is a small frame not an OS thread, or go *non-blocking* and multiplex many connections on one thread with a readiness API. Both eliminate the one-OS-thread-per-connection cost; the readiness model (`epoll`) is the classic foundation.

---

## 112.4 Non-Blocking I/O and `epoll`

The scalable approach marks sockets **non-blocking** (so `read`/`accept` return immediately rather than sleeping) and asks the kernel which sockets are *ready* for I/O. The Linux mechanism is **`epoll`**: register the file descriptors once, then a single `epoll_wait` returns only the descriptors that became ready — O(ready), not O(total connections).

```cpp
// Min standard: C++11 + Linux (non-portable). epoll readiness loop.
#include <sys/epoll.h>
int epoll_fd = epoll_create1(0);

epoll_event ev{};
ev.events = EPOLLIN;                     // notify when readable
ev.data.fd = server_fd;
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);   // register the listening socket once

epoll_event events[64];
while (true) {
    int n = epoll_wait(epoll_fd, events, 64, -1);     // ONE syscall returns all ready fds
    for (int i = 0; i < n; ++i) {
        if (events[i].data.fd == server_fd) {
            // accept() a new connection, set it non-blocking, register it with epoll
        } else {
            // read() available data from a ready connection, process it
        }
    }
}
```
*Listing 112.2 — `epoll`: one `epoll_wait` returns all ready descriptors. Linux-specific (`kqueue` on BSD/macOS, IOCP on Windows).*

> **Why this matters / cost model.** `epoll` is how nginx, Redis, and Node.js serve tens of thousands of connections from a *single* thread (or one per core). Its key property over the older `select`/`poll` is scalability: `select` re-scans *all* registered fds on every call (O(n)), while `epoll` registers fds once and returns only the ready ones (O(ready)) — so a server with 50,000 mostly-idle connections pays only for the few that are active. It remains a *readiness* model (the kernel says "this socket can be read," then you issue the `read` — one syscall per ready operation plus the copy), which is why `io_uring`'s *completion* model (Chapter 99) is the next step: it submits the read itself and amortises even the readiness syscall.

---

## 112.5 The Event Loop and the Reactor Pattern

The structure in Listing 112.2 is the **reactor pattern**: an *event loop* waits for I/O readiness and *dispatches* each ready event to a handler. This is the architecture of every asynchronous networking library — Asio's `io_context`, libuv (Node.js), and the C++ networking TS all implement a reactor (or the **proactor** variant for completion-based I/O like `io_uring`/IOCP).

> **Why this matters.** The reactor decouples *what* to do on an event (the handler) from *how* events are detected (`epoll`/`kqueue`/`io_uring`), so the same application logic runs over different OS mechanisms. This is why understanding `epoll` transfers directly to understanding Asio: Asio is a portable reactor/proactor over `epoll`, `kqueue`, `io_uring`, and IOCP, with handlers expressed as callbacks or coroutines (Chapter 78). The reactor's hazard is that a *blocking* operation inside a handler stalls the *entire* event loop (and thus all connections it serves) — so the cardinal rule of event-loop programming is that handlers must never block; long work is offloaded to a thread pool, and all I/O goes through the loop.

---

## 112.6 From `epoll` to the Modern Stack

The progression of network I/O models, each removing a cost (Chapters 98–100):

| Model | Connections/thread | Cost removed |
|---|---|---|
| Blocking, thread-per-conn | ~thousands (thread-limited) | — (simple) |
| Coroutine-per-connection | tens of thousands | OS-thread overhead (Ch 78) |
| `epoll` reactor | tens of thousands | thread-per-connection |
| `io_uring` proactor | hundreds of thousands | per-operation readiness syscall (Ch 99) |
| Kernel bypass (DPDK/Onload) | line rate | the kernel entirely (Ch 100) |

> **The discipline.** Networking from scratch teaches that every networking library is a structured way of issuing the same sockets syscalls, and its performance is governed by the same cost model: minimise syscall crossings and data copies. Reach up the stack only as your scale demands — blocking or coroutines for modest concurrency, an `epoll`/Asio reactor for C10K, `io_uring` when the per-operation syscall cost dominates, and kernel bypass (Chapter 100) only at the extreme. Whatever the library, the rules from the sockets layer hold: handle byte order, never block the event loop, size buffers and backlogs for load, and remember that a `read`/`write` is a kernel crossing to be amortised. The next chapter takes these services into the cloud.
