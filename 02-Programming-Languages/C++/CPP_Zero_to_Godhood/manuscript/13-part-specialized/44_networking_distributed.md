# Part XIII: Specialized Domains

*The Final Frontier. Networking, Finance, Embedded, and Graphics.*

# Chapter 44: Networking and Distributed Systems

> *There is no cloud, just someone else's computer.*

Up until now, our C++ programs have lived in isolation. They start, they use the RAM on the local machine, and they exit. 

But in the modern world, a single computer is rarely enough. Whether you are building a multiplayer game server, a microservice in a cloud cluster, or a high-frequency trading node, your C++ program must talk to the outside world.

---

## 44.1 The Socket Abstraction

At the lowest level (provided by the OS), computers communicate over the network using **Sockets**. A socket is essentially a file descriptor. Just like you can open a text file and `write()` to it, you can open a socket and `write()` to it, and those bytes are sent over the Ethernet cable to another IP address.

*   **TCP (Transmission Control Protocol):** Guarantees delivery and order. Used for web browsing, chat apps, and database connections.
*   **UDP (User Datagram Protocol):** Fire-and-forget. Faster, but packets can be lost or arrive out of order. Used for multiplayer games, VoIP, and live video streaming.

In C++, using raw POSIX sockets requires tedious boilerplate with `sockaddr_in`, `bind()`, `listen()`, and `accept()`.

```cpp
// A massive oversimplification of a TCP server setup:
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
bind(server_fd, (struct sockaddr*)&address, sizeof(address));
listen(server_fd, 3);
int client_socket = accept(server_fd, ...);
send(client_socket, "Hello", 5, 0);
```

## 44.2 Serialization: The "Box and Label" Problem

When you send a `std::string` or a custom `User` class over a socket, you cannot just send the memory address. The address `0x1A42` on your computer means absolutely nothing to a server in Japan.

You must **Serialize** the data. Serialization is like taking a LEGO castle, breaking it down into individual bricks, putting them in a numbered box with instructions, and shipping it. The receiver then **Deserializes** it—rebuilding the castle brick-by-brick.

### A Simple Binary Serializer

```cpp
#include <vector>
#include <string>
#include <cstdint>

class Buffer {
    std::vector<uint8_t> data;
public:
    // Write primitive types (int, float, etc.)
    template<typename T>
    void write(const T& val) {
        static_assert(std::is_trivially_copyable_v<T>, "Must be trivial!");
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&val);
        data.insert(data.end(), ptr, ptr + sizeof(T));
    }

    // Write complex types like std::string
    void write_string(const std::string& s) {
        write<uint32_t>(s.size()); // First write the length
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(s.data());
        data.insert(data.end(), ptr, ptr + s.size()); // Then write the characters
    }
    
    const uint8_t* get_bytes() const { return data.data(); }
    size_t size() const { return data.size(); }
};
```

In the real world, you do not write this yourself. You use industry-standard serialization formats:
1.  **JSON**: Human-readable, but bloated and very slow to parse.
2.  **Protocol Buffers (Protobuf)**: Created by Google. Binary, extremely fast, strongly typed, and backwards-compatible. *This is the C++ Godhood standard.*

## 44.3 RPC (Remote Procedure Calls) and gRPC

Once you can serialize data, you want to build **RPC**. 

An RPC framework hides the network completely. It makes calling a function on a server in Tokyo look exactly like calling a local C++ function.

```cpp
// Local Code:
int result = add(5, 3); 

// RPC Code:
int result = server.add(5, 3); // Looks identical!
```

Under the hood, the `server.add()` function is a **Stub**. It intercepts the call, serializes `5` and `3` into a Protobuf message, opens a socket, sends the message over TCP, waits for the server to calculate `8`, receives the response, deserializes it, and returns `8` to your local program.

The most popular framework for this is **gRPC**, which uses HTTP/2 and Protobuf.

## 44.4 The Problem of Consensus

When you have one server, truth is absolute. When you have 5 servers handling a distributed database, how do they agree on the state of the data if the network between Server 3 and Server 4 goes down? This is the fundamental problem of Distributed Systems.

To solve this, systems use **Consensus Algorithms** like **Paxos** or **Raft**.

In Raft, servers elect a "Leader". Only the Leader can accept writes from clients. The Leader then replicates the log to the "Followers". If the Leader crashes, the Followers detect a timeout and automatically hold a new election.

```cpp
enum class State { Follower, Candidate, Leader };

struct RaftNode {
    State state = State::Follower;
    int current_term = 0;
    int voted_for = -1;

    void on_timeout() {
        if (state == State::Follower) {
            // I haven't heard from the Leader! I'm running for office!
            state = State::Candidate;
            current_term++;
            voted_for = my_id;
            request_votes_from_peers();
        }
    }
};
```

## 44.5 Modern Asynchronous I/O (`boost::asio`)

Handling 10 network connections by spinning up 10 `std::thread`s works. Handling 10,000 connections with 10,000 threads will crash your OS due to context-switching overhead.

To build massively scalable C++ servers, you must use **Asynchronous I/O** (like `epoll` on Linux or `kqueue` on macOS). Instead of blocking while waiting for a socket to receive data, you register a callback or a C++20 Coroutine.

The undisputed king of C++ networking is **Boost.Asio**. 

```cpp
// A glimpse of asynchronous networking with Boost.Asio
boost::asio::async_read(socket_, boost::asio::buffer(data_, max_length),
    [this](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
            std::cout << "Received " << length << " bytes asynchronously!\n";
        }
    });
```

*(Note: There is an ongoing effort to standardize networking into the C++ standard library as `<net>`, but until then, `boost::asio` is the professional choice).*

---

Networking is about distance. Our next chapter goes in the opposite direction. What happens when you have practically zero memory, zero operating system, and zero room for error? We move to **Chapter 45: Embedded Systems and Real-Time C++**.
