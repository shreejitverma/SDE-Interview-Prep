# Chapter 50: The Capstone Project

> *Knowledge is only potential power. Execution is actual power.*

You have traversed the entire landscape of C++. You understand the hardware, the operating system, the compiler, and the standard library. You know how to write templates that calculate primes at compile time, and you know how to write lock-free queues that transfer data in nanoseconds.

But reading a book does not make you a programmer. Writing code does.

This final chapter is not a tutorial. It is a specification for your Capstone Project. If you can build this from scratch, without copying code from tutorials, you have achieved C++ Godhood.

---

## 50.1 The Goal: "GodKV"

**Your Task:** Build a high-performance, multithreaded, in-memory Key-Value database (similar to Redis).

### Requirements:
1.  **Networked:** It must run as a server listening on a TCP port. Clients connect via Telnet or netcat and send text commands.
2.  **Multithreaded:** It must handle thousands of concurrent client connections using a Thread Pool and Asynchronous I/O.
3.  **Thread-Safe:** Multiple clients must be able to read and write to the same keys simultaneously without corrupting memory or crashing.
4.  **Persistent:** Every 5 minutes, it must serialize its current state and save it to disk. If the server crashes, it must load the disk file on startup to restore the data.

### Supported Commands:
*   `SET <key> <value>`: Store a string value.
*   `GET <key>`: Retrieve a value.
*   `DEL <key>`: Delete a key.
*   `EXPIRE <key> <seconds>`: Automatically delete the key after N seconds.

---

## 50.2 Architecture Guide

Do not write everything in `main.cpp`. Architect your system using the modern C++ principles we have discussed.

### 1. The Core Data Structure
At its heart, your database is a `std::unordered_map<std::string, std::string>`.
However, `std::unordered_map` is not thread-safe. If Thread A inserts a key while Thread B is reading a key, the map might rehash its internal buckets, causing Thread B to read garbage memory (Undefined Behavior).

**The Solution:** Use a `std::shared_mutex` (C++17).
*   When a client calls `GET`, lock the mutex with `std::shared_lock`. This allows thousands of readers to read simultaneously.
*   When a client calls `SET` or `DEL`, lock the mutex with `std::unique_lock`. This blocks all readers and writers until the mutation is complete.

*Godhood Challenge:* If your database grows to millions of keys, a single global mutex will become a massive bottleneck. Can you implement **Lock Striping**? Create an array of 16 different mutexes and 16 different maps. Hash the key to determine which map (and which mutex) it belongs to.

### 2. Expiration (The TTL Thread)
How do you implement the `EXPIRE` command?
You need a background `std::thread`. 
Do not use `sleep()` or a spin-lock. Use a `std::priority_queue` that stores pairs of `<TimePoint, Key>`, sorted so the earliest expiration time is at the top. The background thread uses a `std::condition_variable` with `wait_until()` to sleep exactly until the moment the top key needs to be deleted.

### 3. Networking (Asynchronous I/O)
Do not spawn a new `std::thread` for every client that connects. If 10,000 clients connect, you will exhaust OS resources.
Use `boost::asio` or `epoll`/`kqueue`. Implement an Event Loop that detects when a socket has data ready to read, and dispatches that socket to a pre-allocated **Thread Pool**.

### 4. Persistence (Serialization)
Do not write the data to disk in plain text. It is too slow to parse on startup.
Write a binary serializer (like we discussed in Chapter 44). Write the length of the string as a 4-byte integer, followed by the raw `char` bytes. 

To ensure you don't block the server while saving to disk, take a snapshot of the map. Wait, how do you take a snapshot without locking the database for seconds?
*Godhood Challenge:* Use `fork()` on Linux. The OS uses Copy-on-Write memory. The child process will inherit a perfect, frozen snapshot of the memory, write it to disk, and exit, while the parent process continues serving clients seamlessly.

---

## 50.3 The Path Forward

If you complete GodKV, there is nothing left to teach you in a book. The rest of your journey relies on experience.

*   Read the C++ Core Guidelines.
*   Watch CppCon talks on YouTube.
*   Read the source code of massive open-source projects like LLVM, the Unreal Engine, or Google's Abseil library.

### Epilogue

C++ is not an elegant language. It is massive, historically burdened, and terrifyingly complex. It hands you a chainsaw without a safety guard.

But in exchange for demanding your utmost discipline, it gives you absolute power over the machine. It allows you to build software that changes the world. Software that lands rovers on Mars, renders blockbuster movies, processes billions of financial transactions a second, and powers the internet itself.

Welcome to the inner circle.

**You have achieved Godhood.**

---
*End of the Journey.*
