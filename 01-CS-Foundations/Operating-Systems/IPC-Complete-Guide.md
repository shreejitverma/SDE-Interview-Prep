# Inter-Process Communication (IPC): Complete In-Depth Learning Guide

## Table of Contents
1. [Fundamentals](#fundamentals)
2. [Basic IPC Mechanisms](#basic-ipc-mechanisms)
3. [Advanced IPC Mechanisms](#advanced-ipc-mechanisms)
4. [Synchronization Primitives](#synchronization-primitives)
5. [Performance Patterns](#performance-patterns)
6. [Trading-Specific Applications](#trading-specific-applications)
7. [Debugging & Monitoring](#debugging--monitoring)
8. [Best Practices](#best-practices)

---

## Fundamentals

### Why IPC Exists: Process Isolation

#### The Operating System's Design Decision

Modern operating systems isolate processes into separate virtual address spaces for safety and security:

```
Virtual Memory Architecture:
┌─────────────────────────────┐
│ Process A                   │      ┌─────────────────────────────┐
├─────────────────────────────┤      │ Process B                   │
│ Kernel Space (0xFFFF)       │      ├─────────────────────────────┤
│ (shared, protected)         │      │ Kernel Space (0xFFFF)       │
├─────────────────────────────┤      │ (shared, protected)         │
│ Stack (grows down)          │      ├─────────────────────────────┤
│ local variables             │      │ Stack (grows down)          │
├─────────────────────────────┤      │ local variables             │
│ Heap (grows up)             │      ├─────────────────────────────┤
│ malloc/new allocations      │      │ Heap (grows up)             │
├─────────────────────────────┤      │ malloc/new allocations      │
│ Data Segment                │      ├─────────────────────────────┤
│ global variables            │      │ Data Segment                │
├─────────────────────────────┤      │ global variables            │
│ Code Segment (text)         │      ├─────────────────────────────┤
│ read-only, executable       │      │ Code Segment (text)         │
└─────────────────────────────┘      │ read-only, executable       │
         0x0 (user start)            └─────────────────────────────┘
```

#### Why This Isolation?

**Advantages:**
- **Safety**: Process crash doesn't corrupt other processes
- **Security**: Process A can't read Process B's passwords or secrets
- **Reliability**: OS can kill problematic process without affecting others
- **Modularity**: Easier to develop, test, deploy independent services

**The Problem:**
Legitimate processes often need to communicate:
- Database writes data; application reads it
- Market data server distributes quotes; strategies consume them
- Risk manager monitors all traders' positions
- Compliance logs trades; auditor reads them

**Solution: Inter-Process Communication (IPC)**

IPC allows secure, controlled communication between isolated processes while maintaining OS-enforced isolation.

---

### Core IPC Concepts

#### What Processes Need to Exchange

```
1. DATA (Messages, Buffers)
   ├─ Small: Signal (just an integer)
   ├─ Medium: Message (100 bytes - 1 MB)
   └─ Large: Shared region (GB of data)

2. SYNCHRONIZATION (Coordination)
   ├─ "Wait until producer has data"
   ├─ "Prevent two processes modifying simultaneously"
   └─ "Wake me when event occurs"

3. SIGNALING (Events)
   ├─ "Order executed!"
   ├─ "Risk limit exceeded"
   └─ "Market data updated"
```

#### Communication Patterns

**Request-Reply:**
```
Client (Request)  ──→  Server
                        ├─ Process request
                        └─ Send response
        ←──────────────
```

**Producer-Consumer:**
```
Producer  ──→  Buffer  ←──  Consumer
        (writes)      (reads)
```

**Broadcast:**
```
Publisher  ──→  Topic/Channel  ←──  Subscriber 1
                                  ←──  Subscriber 2
                                  ←──  Subscriber N
```

#### IPC Mechanisms Trade-Off Matrix

| Mechanism | Latency | Throughput | Data Size | Ease | Persistence |
|-----------|---------|-----------|-----------|------|-------------|
| **Signals** | < 1 μs | Very low | 32 bits | Easy | No |
| **Pipes** | 1-10 ms | Medium | KB | Easy | No |
| **Sockets** | 1-10 ms | High | MB | Medium | No |
| **Message Queues** | 1-10 ms | Medium | KB-MB | Medium | Yes |
| **Shared Memory** | 10-100 ns | Very high | GB | Hard | No |
| **Memory-mapped Files** | 10-100 ns* | Very high | GB | Hard | Yes |
| **Ring Buffer** | 50-100 ns | Very high | KB | Hard | No |

*\*If page cached; page fault = 10 ms*

---

## Basic IPC Mechanisms

### Signals

#### What They Are

Signals are software interrupts—a way to asynchronously notify a process that an event has occurred.

```
Signal Definition:
├─ Signal number (SIGTERM = 15, SIGUSR1 = 10, etc.)
├─ Handler function (what to do when signal arrives)
└─ Delivery mechanism (OS interrupts running code)

Key Property:
├─ Asynchronous (event-driven)
├─ Cannot transmit large data (only signal number)
└─ Signal may be lost if process not ready (unreliable)
```

#### Standard Signals in Linux

```
SIGTERM (15)    : Terminate gracefully (kill command default)
SIGKILL (9)     : Force terminate (can't be caught)
SIGUSR1 (10)    : User-defined signal 1
SIGUSR2 (12)    : User-defined signal 2
SIGALRM (14)    : Timer expired (from alarm())
SIGCHLD (17)    : Child process terminated
SIGSEGV (11)    : Segmentation fault (memory error)
SIGINT  (2)     : Interrupt (Ctrl+C)
SIGSTOP (19)    : Stop process (can't be caught)
SIGCONT (18)    : Continue stopped process
```

#### C++ Signal Handling Example

```cpp
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <cstring>

// Global flag (must be volatile sig_atomic_t for thread-safety)
volatile sig_atomic_t signal_received = 0;
volatile sig_atomic_t sigusr1_count = 0;

// Signal handler (called when signal is received)
void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        sigusr1_count++;
        signal_received = 1;
        
        // Note: Not safe to do printf or complex operations here
        // Only signal-safe functions allowed
    }
}

int main() {
    // Method 1: Using signal() (older, less reliable)
    signal(SIGUSR1, signal_handler);
    
    // Method 2: Using sigaction() (recommended, more reliable)
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);          // Don't block any signals
    sa.sa_flags = SA_RESTART;          // Restart interrupted syscalls
    
    sigaction(SIGUSR1, &sa, nullptr);
    
    // Print PID so other process can send signal
    std::cout << "Process ID: " << getpid() << std::endl;
    std::cout << "Waiting for SIGUSR1..." << std::endl;
    
    // Wait for signals
    int count = 0;
    while (count < 5) {
        if (signal_received) {
            std::cout << "Received signal " << sigusr1_count << std::endl;
            signal_received = 0;
            count++;
        }
        sleep(1);
    }
    
    std::cout << "Got 5 signals, exiting" << std::endl;
    return 0;
}
```

**Compile and test:**
```bash
g++ -o signal_receiver signal_receiver.cpp
./signal_receiver &
PID=$!

# Send signals from another terminal
for i in {1..5}; do
    sleep 1
    kill -USR1 $PID
done

wait
```

#### Sending Signals from C++

```cpp
#include <signal.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <target_pid>" << std::endl;
        return 1;
    }
    
    pid_t target = atoi(argv[1]);
    
    std::cout << "Sending SIGUSR1 to process " << target << std::endl;
    
    if (kill(target, SIGUSR1) == -1) {
        perror("kill");
        return 1;
    }
    
    std::cout << "Signal sent successfully" << std::endl;
    return 0;
}
```

#### Limitations of Signals

```
✗ No data transmission (only signal number, maybe one value)
✗ Unreliable (signal may be lost if process not ready)
✗ Limited number (~64 on Linux)
✗ Race conditions (handler can interrupt any instruction)
✗ Not suitable for high-frequency communication
```

#### When to Use Signals

- Graceful shutdown (SIGTERM handler closes connections)
- Timeout handling (SIGALRM)
- Child process monitoring (SIGCHLD)
- Lightweight notifications (not data transfer)

---

### Pipes (Unnamed & Named)

#### Unnamed Pipes (Anonymous Pipes)

**Concept:**
Unnamed pipes exist only in memory, between parent and child processes. They're used with shell redirection (`|`).

```
Command Line:
./producer | ./consumer

Kernel Implementation:
┌──────────────┐      ┌─────────────┐      ┌──────────────┐
│   producer   │ ──→  │  Pipe Buffer  │  ──→  │  consumer    │
└──────────────┘      │ (64 KB max)   │      └──────────────┘
  writes to fd[1]     └─────────────┘      reads from fd[0]
```

#### Creating and Using Unnamed Pipes

```cpp
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <cstring>

void parent_child_pipe_example() {
    int fd[2];  // fd[0] = read end, fd[1] = write end
    
    // Create pipe
    if (pipe(fd) == -1) {
        perror("pipe");
        return;
    }
    
    // Fork child process
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        // CHILD PROCESS: Reader
        close(fd[1]);  // Close write end (not used in child)
        
        char buffer[256];
        ssize_t bytes = read(fd[0], buffer, sizeof(buffer) - 1);
        
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::cout << "[Child] Received: " << buffer << std::endl;
        }
        
        close(fd[0]);
        exit(0);
    } else {
        // PARENT PROCESS: Writer
        close(fd[0]);  // Close read end (not used in parent)
        
        const char* message = "Hello from parent!";
        write(fd[1], message, strlen(message));
        
        std::cout << "[Parent] Sent: " << message << std::endl;
        
        close(fd[1]);
        
        // Wait for child to finish
        int status;
        waitpid(pid, &status, 0);
    }
}
```

**Output:**
```
[Parent] Sent: Hello from parent!
[Child] Received: Hello from parent!
```

#### Named Pipes (FIFOs)

**Concept:**
Named pipes are actual files on the filesystem, allowing unrelated processes to communicate.

```cpp
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

// Writer Process
void named_pipe_writer() {
    const char* fifo_path = "/tmp/trading_fifo";
    
    // Create FIFO (first time only)
    mkfifo(fifo_path, 0666);
    
    std::cout << "[Writer] Opening FIFO for writing..." << std::endl;
    
    // Open for writing (blocks until reader opens)
    int fd = open(fifo_path, O_WRONLY);
    
    if (fd == -1) {
        perror("open");
        return;
    }
    
    std::cout << "[Writer] FIFO opened, sending quotes..." << std::endl;
    
    // Send multiple quotes
    for (int i = 0; i < 10; ++i) {
        char message[256];
        sprintf(message, "Quote %d: AAPL 175.%02d", i, i);
        
        write(fd, message, strlen(message));
        std::cout << "[Writer] Sent: " << message << std::endl;
        
        usleep(100000);  // 100ms delay
    }
    
    close(fd);
    unlink(fifo_path);  // Clean up
}

// Reader Process
void named_pipe_reader() {
    const char* fifo_path = "/tmp/trading_fifo";
    
    std::cout << "[Reader] Opening FIFO for reading..." << std::endl;
    
    // Open for reading (blocks until writer opens)
    int fd = open(fifo_path, O_RDONLY);
    
    if (fd == -1) {
        perror("open");
        return;
    }
    
    std::cout << "[Reader] FIFO opened, receiving quotes..." << std::endl;
    
    char buffer[256];
    ssize_t bytes;
    
    // Read until EOF
    while ((bytes = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        std::cout << "[Reader] Received: " << buffer << std::endl;
    }
    
    close(fd);
}
```

**Run in separate terminals:**
```bash
Terminal 1: ./program reader
Terminal 2: ./program writer
```

#### Pipe Characteristics

```
Advantages:
✓ Simple to use: Just read() and write()
✓ Sequential: FIFO order guaranteed
✓ Buffered: OS manages buffer (64 KB default)
✓ Easy to redirect: Shell redirection (`|`)

Disadvantages:
✗ One-way: Data flows in one direction only
✗ Slow: Context switches + buffering = 1-10 ms latency
✗ Limited capacity: Buffer full → writer blocks
✗ Not reliable: Data lost if reader crashes
✗ Unnamed pipes only between parent-child

Latency Profile:
├─ Write small data: < 1 μs (memcpy to kernel buffer)
├─ Context switch to reader: ~100 μs
├─ Reader reads from buffer: < 1 μs
├─ Total: ~1-10 milliseconds
```

---

### Sockets

#### TCP/IP Sockets (Network IPC)

**Concept:**
Sockets provide bidirectional communication, locally or over network. TCP ensures reliable, ordered delivery.

#### TCP Socket Server

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

void tcp_server() {
    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_fd == -1) {
        perror("socket");
        return;
    }
    
    // Allow reusing address (important for testing)
    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    // Bind to port 5000
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return;
    }
    
    // Listen for connections (backlog = 5)
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        return;
    }
    
    std::cout << "[Server] Listening on port 5000..." << std::endl;
    
    // Accept connections
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    
    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        return;
    }
    
    std::cout << "[Server] Client connected from " 
              << inet_ntoa(client_addr.sin_addr) << std::endl;
    
    // Receive message
    char buffer[256] = {0};
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes > 0) {
        std::cout << "[Server] Received: " << buffer << std::endl;
    }
    
    // Send response
    const char* response = "Quote received and processed";
    send(client_fd, response, strlen(response), 0);
    
    close(client_fd);
    close(server_fd);
}
```

#### TCP Socket Client

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

void tcp_client() {
    // Create socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (client_fd == -1) {
        perror("socket");
        return;
    }
    
    // Connect to server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    std::cout << "[Client] Connecting to server..." << std::endl;
    
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_fd);
        return;
    }
    
    std::cout << "[Client] Connected to server" << std::endl;
    
    // Send quote
    const char* quote = "BUY 1000 AAPL @ 175.50";
    send(client_fd, quote, strlen(quote), 0);
    
    std::cout << "[Client] Sent: " << quote << std::endl;
    
    // Receive response
    char buffer[256] = {0};
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes > 0) {
        std::cout << "[Client] Received: " << buffer << std::endl;
    }
    
    close(client_fd);
}
```

**Compile and run:**
```bash
g++ -o socket_server socket_server.cpp
g++ -o socket_client socket_client.cpp

# Terminal 1
./socket_server

# Terminal 2
./socket_client
```

#### Unix Domain Sockets (Local IPC)

**Concept:**
Unix domain sockets provide fast local communication (faster than TCP/IP, no network stack overhead).

```cpp
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

// Server
void unix_socket_server() {
    const char* socket_path = "/tmp/trading.sock";
    
    // Create socket
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, socket_path);
    
    // Remove old socket file
    unlink(socket_path);
    
    // Bind
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    // Listen
    listen(server_fd, 5);
    
    std::cout << "[Server] Listening on " << socket_path << std::endl;
    
    // Accept
    int client_fd = accept(server_fd, nullptr, nullptr);
    
    // Receive and send
    char buffer[256] = {0};
    recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    std::cout << "[Server] Received: " << buffer << std::endl;
    
    const char* response = "Processed";
    send(client_fd, response, strlen(response), 0);
    
    close(client_fd);
    close(server_fd);
}

// Client
void unix_socket_client() {
    const char* socket_path = "/tmp/trading.sock";
    
    // Create socket
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, socket_path);
    
    // Connect
    connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    // Send and receive
    const char* message = "Quote: AAPL 175.50";
    send(client_fd, message, strlen(message), 0);
    
    char buffer[256] = {0};
    recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    std::cout << "[Client] Received: " << buffer << std::endl;
    
    close(client_fd);
}
```

#### Socket Characteristics

```
Advantages:
✓ Bidirectional: Full-duplex communication
✓ Scalable: Handle many concurrent connections
✓ Reliable: TCP guarantees delivery and order
✓ Network-capable: Can communicate over network

Disadvantages:
✗ Latency: 1-10 ms (TCP overhead, context switches)
✗ Overhead: Need serialization/deserialization
✗ Connection overhead: Setup/teardown complexity
✗ Not ideal for sub-microsecond trading

Latency Profile (loopback):
├─ Send: Kernel queues data (~1 μs)
├─ Context switch to receiver: ~100 μs
├─ TCP processing: ~100 μs
├─ Receive: Kernel copies to buffer (~1 μs)
├─ Total: ~1-10 milliseconds
```

---

## Advanced IPC Mechanisms

### Shared Memory

#### Concept

Multiple processes map the same physical memory region. No copying—processes read/write directly.

```
Virtual Address Spaces:

Process A (0x7fff00000)          Physical RAM (0x1000000)
[Shared Memory]  ┐               [Quote Data]
                 └─────maps to──→ AAPL: 175.50
                                  BID: 175.49
                                  ASK: 175.51

Process B (0x60000000)
[Shared Memory]  ┐
                 └─────maps to──→ Same Physical RAM (different virtual address)

Both processes see same data!
No copying required (except in CPU cache).
```

#### POSIX Shared Memory API

```cpp
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <cstring>

// Shared structure
struct Quote {
    char symbol[10];
    double bid;
    double ask;
    int bid_size;
    int ask_size;
};

// Writer Process
void shm_writer() {
    // Generate unique key
    key_t key = ftok("/tmp", 'Q');
    
    // Create shared memory (4096 bytes)
    int shmid = shmget(key, sizeof(Quote), IPC_CREAT | 0666);
    
    // Attach to address space
    Quote* quote = (Quote*)shmat(shmid, nullptr, 0);
    
    // Write data (NO copying!)
    strcpy(quote->symbol, "AAPL");
    quote->bid = 175.49;
    quote->ask = 175.50;
    quote->bid_size = 1000;
    quote->ask_size = 1000;
    
    std::cout << "[Writer] Wrote quote to shared memory" << std::endl;
    
    sleep(10);  // Keep running so reader can access
    
    // Detach
    shmdt(quote);
    
    // Remove (only after all processes detach)
    shmctl(shmid, IPC_RMID, nullptr);
}

// Reader Process
void shm_reader() {
    sleep(1);  // Wait for writer
    
    // Open existing shared memory
    key_t key = ftok("/tmp", 'Q');
    int shmid = shmget(key, sizeof(Quote), 0666);
    
    // Attach to address space (may be different virtual address than writer)
    Quote* quote = (Quote*)shmat(shmid, nullptr, 0);
    
    // Read data (NO copying!)
    std::cout << "[Reader] Symbol: " << quote->symbol << std::endl;
    std::cout << "[Reader] Bid: " << quote->bid << std::endl;
    std::cout << "[Reader] Ask: " << quote->ask << std::endl;
    
    shmdt(quote);
}
```

**Compile and test:**
```bash
g++ -o shm_writer shm_writer.cpp
g++ -o shm_reader shm_reader.cpp

# Terminal 1
./shm_writer

# Terminal 2
./shm_reader
```

#### Modern C++: Boost.Interprocess

```cpp
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <iostream>

using namespace boost::interprocess;

struct Quote {
    char symbol[10];
    double bid;
    double ask;
};

// Writer
void boost_shm_writer() {
    // Create managed shared memory (1 MB)
    managed_shared_memory segment(
        create_only,           // Create new
        "trading_quotes",      // Name
        1024 * 1024            // Size
    );
    
    // Construct Quote in shared memory
    Quote* quote = segment.construct<Quote>("current_quote")();
    
    strcpy(quote->symbol, "AAPL");
    quote->bid = 175.49;
    quote->ask = 175.50;
    
    std::cout << "[Writer] Wrote quote" << std::endl;
    
    sleep(10);
}

// Reader
void boost_shm_reader() {
    sleep(1);
    
    // Open existing shared memory
    managed_shared_memory segment(open_only, "trading_quotes");
    
    // Find Quote
    Quote* quote = segment.find<Quote>("current_quote").first;
    
    if (quote) {
        std::cout << "[Reader] Symbol: " << quote->symbol << std::endl;
        std::cout << "[Reader] Bid: " << quote->bid << std::endl;
        std::cout << "[Reader] Ask: " << quote->ask << std::endl;
    }
}
```

#### Shared Memory with Mutex (Safe Concurrent Access)

```cpp
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <iostream>

using namespace boost::interprocess;

struct SharedData {
    int counter;
    char message[256];
};

void safe_writer() {
    managed_shared_memory segment(create_only, "shared_data", 1024);
    named_mutex mtx(create_only, "data_mutex");
    
    SharedData* data = segment.construct<SharedData>("data")();
    
    for (int i = 0; i < 5; ++i) {
        mtx.lock();  // Acquire lock
        {
            data->counter = i;
            strcpy(data->message, "Writer data");
            std::cout << "[Writer] Updated counter to " << i << std::endl;
        }
        mtx.unlock();  // Release lock
        
        sleep(1);
    }
}

void safe_reader() {
    sleep(1);
    
    managed_shared_memory segment(open_only, "shared_data");
    named_mutex mtx(open_only, "data_mutex");
    
    SharedData* data = segment.find<SharedData>("data").first;
    
    for (int i = 0; i < 5; ++i) {
        mtx.lock();
        {
            std::cout << "[Reader] Counter: " << data->counter 
                      << ", Message: " << data->message << std::endl;
        }
        mtx.unlock();
        
        sleep(1);
    }
}
```

#### Shared Memory Characteristics

```
Advantages:
✓ Ultra-fast: < 100 nanoseconds per read/write
✓ No copying: Direct memory access
✓ Large capacity: Can share GB of data
✓ Persistent: Data survives process crash (until cleanup)

Disadvantages:
✗ Synchronization complexity: Must use mutexes/semaphores
✗ Race conditions: Multiple writers → corruption
✗ Hard debugging: Memory corruption hard to trace
✗ Cleanup issues: Manual cleanup required
✗ Platform-specific: Different APIs on Windows

Latency Profile:
├─ Read (L1 cache): ~1 nanosecond
├─ Read (L3 cache): ~40 nanoseconds
├─ Read (main memory): ~200 nanoseconds
├─ Write (L1 cache): ~1 nanosecond
├─ Context switch: ~100 nanoseconds
├─ Total per read-write cycle: ~200-500 nanoseconds
```

---

### Memory-Mapped Files

#### Concept

Map a file into virtual address space. Multiple processes can map the same file. Changes persist to disk.

```
Virtual Address Space          Disk File
[Mapped File]  ┐              [file.dat]
                └─────maps─→
(changes auto-synced to disk)
```

#### Basic Implementation

```cpp
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

struct TradeRecord {
    uint64_t timestamp;
    char symbol[10];
    double price;
    int quantity;
};

void create_trade_file() {
    const char* filename = "/tmp/trades.dat";
    
    // Create file
    int fd = open(filename, O_CREAT | O_RDWR, 0666);
    
    // Pre-allocate file (100 MB)
    lseek(fd, 100 * 1024 * 1024 - 1, SEEK_SET);
    write(fd, "", 1);
    
    // Map file to memory
    TradeRecord* trades = (TradeRecord*)mmap(
        nullptr,                          // Let OS choose address
        100 * 1024 * 1024,                // Size
        PROT_READ | PROT_WRITE,          // Read + write
        MAP_SHARED,                       // Share between processes
        fd,                               // File descriptor
        0                                 // Offset
    );
    
    // Write trades (persisted to disk!)
    for (int i = 0; i < 10; ++i) {
        trades[i].timestamp = 1000 + i;
        sprintf(trades[i].symbol, "AAPL%d", i);
        trades[i].price = 175.50 + i * 0.01;
        trades[i].quantity = 1000 * (i + 1);
    }
    
    // Sync to disk (important!)
    msync(trades, 100 * 1024 * 1024, MS_SYNC);
    
    std::cout << "[Writer] Wrote trades to file" << std::endl;
    
    sleep(10);
    
    munmap(trades, 100 * 1024 * 1024);
    close(fd);
}

void read_trade_file() {
    sleep(1);
    
    const char* filename = "/tmp/trades.dat";
    
    int fd = open(filename, O_RDONLY);
    
    // Map file for reading
    TradeRecord* trades = (TradeRecord*)mmap(
        nullptr,
        100 * 1024 * 1024,
        PROT_READ,
        MAP_SHARED,
        fd,
        0
    );
    
    // Read trades
    std::cout << "[Reader] Trades:" << std::endl;
    for (int i = 0; i < 10; ++i) {
        std::cout << "  Trade " << i << ": " << trades[i].symbol 
                  << " @ " << trades[i].price 
                  << " qty=" << trades[i].quantity << std::endl;
    }
    
    munmap(trades, 100 * 1024 * 1024);
    close(fd);
}
```

#### Memory-Mapped File Characteristics

```
Advantages:
✓ Ultra-fast: < 100 nanoseconds (if page in cache)
✓ Persistent: Changes written to disk
✓ Large files: Can map multi-GB files
✓ Automatic cleanup: File persists after process exit

Disadvantages:
✗ Page faults: First access to unmapped page = 10-100 ms (disk I/O)
✗ I/O sync cost: msync() forces disk write (10-100 ms)
✗ Fragmentation: Large files may be fragmented on disk

Latency Profile:
├─ Read (page in cache): ~50 nanoseconds
├─ Read (page fault - disk I/O): ~10-100 milliseconds
├─ Write (page in cache): ~50 nanoseconds
├─ msync (force to disk): ~1-100 milliseconds
```

---

## Synchronization Primitives

### Semaphores

#### Binary Semaphore (0 or 1)

```cpp
#include <semaphore.h>
#include <iostream>
#include <unistd.h>

void binary_semaphore_example() {
    // Create binary semaphore (value=0, unavailable)
    sem_t* sem = sem_open("/market_data_ready", O_CREAT, 0666, 0);
    
    // Producer thread: waits on semaphore
    // sem_wait(sem);  // Blocks until semaphore > 0
    // Read market data
    
    // Consumer thread: signals when data ready
    // sem_post(sem);  // Increment semaphore, wake blocked thread
}
```

#### Counting Semaphore (N slots)

```cpp
#include <semaphore.h>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <iostream>

using namespace boost::interprocess;

// Producer-consumer with counting semaphore
const int BUFFER_SIZE = 10;

void producer_with_semaphore() {
    // Create semaphores
    sem_t* empty = sem_open("/empty", O_CREAT, 0666, BUFFER_SIZE);  // Empty slots
    sem_t* full = sem_open("/full", O_CREAT, 0666, 0);              // Full slots
    
    // Create shared buffer
    managed_shared_memory segment(create_only, "buffer", 1024);
    
    for (int i = 0; i < 20; ++i) {
        // Wait for empty slot
        sem_wait(empty);
        
        std::cout << "[Producer] Produced item " << i << std::endl;
        
        // Signal consumer: item available
        sem_post(full);
        
        usleep(100000);  // 100ms
    }
}

void consumer_with_semaphore() {
    sleep(1);
    
    sem_t* empty = sem_open("/empty", 0);
    sem_t* full = sem_open("/full", 0);
    
    managed_shared_memory segment(open_only, "buffer");
    
    for (int i = 0; i < 20; ++i) {
        // Wait for full slot
        sem_wait(full);
        
        std::cout << "[Consumer] Consumed item " << i << std::endl;
        
        // Signal producer: slot empty
        sem_post(empty);
        
        usleep(200000);  // 200ms
    }
}
```

#### Semaphore Characteristics

```
Advantages:
✓ Simple: sem_wait() and sem_post()
✓ No busy-waiting: OS handles blocking
✓ Efficient: Minimal CPU usage when blocked

Disadvantages:
✗ Deadlock risk: Circular dependencies
✗ Priority inversion: Low-priority task holds semaphore for high-priority
✗ No fairness: No guarantee which waiter wakes first

Latency Profile:
├─ sem_post (semaphore available): ~1 microsecond
├─ sem_wait (must block): ~100 microseconds (context switch)
```

---

### Condition Variables

#### Concept

Condition variables allow threads/processes to wait for specific conditions.

```cpp
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <iostream>

using namespace boost::interprocess;

struct Data {
    int value;
    bool ready;
};

void notifier() {
    managed_shared_memory segment(create_only, "data_segment", 1024);
    named_mutex mtx(create_only, "data_mutex");
    named_condition cond(create_only, "data_condition");
    
    Data* data = segment.construct<Data>("data")();
    data->value = 0;
    data->ready = false;
    
    for (int i = 0; i < 5; ++i) {
        sleep(1);
        
        mtx.lock();
        {
            data->value = i;
            data->ready = true;
            std::cout << "[Notifier] Updated value to " << i << std::endl;
        }
        mtx.unlock();
        
        // Notify waiting process
        cond.notify_all();
    }
}

void waiter() {
    sleep(1);
    
    managed_shared_memory segment(open_only, "data_segment");
    named_mutex mtx(open_only, "data_mutex");
    named_condition cond(open_only, "data_condition");
    
    Data* data = segment.find<Data>("data").first;
    
    for (int i = 0; i < 5; ++i) {
        scoped_lock<named_mutex> lock(mtx);
        
        // Wait until ready
        while (!data->ready) {
            cond.wait(lock);
        }
        
        std::cout << "[Waiter] Received value: " << data->value << std::endl;
        
        data->ready = false;
    }
}
```

---

## Performance Patterns

### Ring Buffer (Lock-Free Queue)

#### Concept

A circular buffer with two pointers (read/write). Ultra-high performance using atomic operations instead of locks.

```
Ring Buffer (size = 8):
┌─────────────────────────────────────────┐
│ [0] [1] [2] [3] [4] [5] [6] [7]        │
└─────────────────────────────────────────┘
  ↑ read_pos              ↑ write_pos
  (consumer)              (producer)

Producer writes to write_pos, advances pointer.
Consumer reads from read_pos, advances pointer.

Empty: read_pos == write_pos
Full: (write_pos + 1) % SIZE == read_pos
```

#### C++ Implementation

```cpp
#include <atomic>
#include <cstring>

template<typename T, size_t SIZE = 1024>
class LockFreeRingBuffer {
    T buffer[SIZE];
    std::atomic<size_t> write_pos{0};
    std::atomic<size_t> read_pos{0};
    
public:
    bool push(const T& item) {
        size_t write = write_pos.load(std::memory_order_acquire);
        size_t next_write = (write + 1) % SIZE;
        
        // Check if buffer full
        if (next_write == read_pos.load(std::memory_order_acquire)) {
            return false;  // Buffer full
        }
        
        // Write item
        buffer[write] = item;
        
        // Advance write pointer
        write_pos.store(next_write, std::memory_order_release);
        
        return true;
    }
    
    bool pop(T& item) {
        size_t read = read_pos.load(std::memory_order_acquire);
        
        // Check if buffer empty
        if (read == write_pos.load(std::memory_order_acquire)) {
            return false;  // Buffer empty
        }
        
        // Read item
        item = buffer[read];
        
        // Advance read pointer
        read_pos.store((read + 1) % SIZE, std::memory_order_release);
        
        return true;
    }
    
    size_t size() const {
        size_t w = write_pos.load(std::memory_order_relaxed);
        size_t r = read_pos.load(std::memory_order_relaxed);
        return (w - r + SIZE) % SIZE;
    }
};

// Trading example
struct Quote {
    char symbol[10];
    double price;
    uint64_t timestamp;
};

LockFreeRingBuffer<Quote> quote_buffer;

void market_data_thread() {
    while (true) {
        Quote q = receive_quote_from_exchange();
        
        // Push to ring buffer (< 100 nanoseconds)
        if (!quote_buffer.push(q)) {
            std::cerr << "Quote buffer full (dropped)" << std::endl;
        }
    }
}

void strategy_thread() {
    Quote q;
    
    while (true) {
        // Pop from ring buffer (< 100 nanoseconds)
        if (quote_buffer.pop(q)) {
            if (q.price < fair_value) {
                submit_buy_order(q);
            }
        }
    }
}
```

#### Ring Buffer Characteristics

```
Advantages:
✓ Ultra-fast: < 100 nanoseconds per push/pop
✓ Lock-free: No mutexes (no context switches)
✓ Predictable: Constant latency
✓ Scalable: Multiple consumers reading independently

Disadvantages:
✗ Fixed size: Must pre-allocate
✗ Data loss: Overwrite if producer faster than consumer
✗ Complex: Memory ordering (acquire/release) required
✗ No data persistence

Latency Profile:
├─ Push: ~50 nanoseconds (atomic operations)
├─ Pop: ~50 nanoseconds
├─ Total: ~100 nanoseconds (1000x faster than pipes!)
```

---

### LMAX Disruptor Pattern

#### Concept

Production-grade ring buffer used by high-frequency trading firms. Multiple producers, multiple consumers, cache-aligned, mechanical sympathy.

```cpp
#include <atomic>
#include <thread>
#include <iostream>

struct TradeEvent {
    uint64_t sequence;
    uint64_t timestamp;
    char symbol[10];
    double price;
    int quantity;
    char side;  // 'B' or 'S'
    
    // Padding to ensure each entry is on separate cache line (64 bytes)
    char padding[64 - sizeof(uint64_t) * 2 - 10 - sizeof(double) - sizeof(int) - 1];
};

class Disruptor {
    static const size_t BUFFER_SIZE = 65536;
    
    std::array<TradeEvent, BUFFER_SIZE> ring_buffer;
    alignas(64) std::atomic<uint64_t> next_sequence{0};
    
public:
    // Publisher (single threaded for best performance)
    bool publish(const TradeEvent& event) {
        uint64_t seq = next_sequence.fetch_add(1, std::memory_order_acq_rel);
        
        size_t index = seq % BUFFER_SIZE;
        ring_buffer[index] = event;
        ring_buffer[index].sequence = seq;
        
        return true;
    }
    
    // Subscriber waits for specific sequence
    bool get_event(uint64_t expected_seq, TradeEvent& out_event) {
        size_t index = expected_seq % BUFFER_SIZE;
        
        // Busy-wait with pause (reduces power consumption)
        while (ring_buffer[index].sequence != expected_seq) {
            _mm_pause();  // x86 PAUSE instruction
        }
        
        out_event = ring_buffer[index];
        return true;
    }
    
    bool get_event_timeout(uint64_t expected_seq, TradeEvent& out_event, 
                          int timeout_ms) {
        size_t index = expected_seq % BUFFER_SIZE;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        while (ring_buffer[index].sequence != expected_seq) {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - start
            ).count();
            
            if (elapsed > timeout_ms) {
                return false;  // Timeout
            }
            
            _mm_pause();
        }
        
        out_event = ring_buffer[index];
        return true;
    }
};

// Usage
void disruptor_example() {
    Disruptor disruptor;
    
    // Publisher thread
    std::thread publisher([&disruptor]() {
        for (int i = 0; i < 1000000; ++i) {
            TradeEvent event;
            event.sequence = i;
            event.timestamp = get_timestamp_ns();
            event.price = 175.50 + (rand() % 100) * 0.01;
            event.quantity = 1000;
            event.side = 'B';
            strcpy(event.symbol, "AAPL");
            
            disruptor.publish(event);
        }
    });
    
    // Subscriber thread (e.g., risk monitor)
    std::thread subscriber([&disruptor]() {
        TradeEvent event;
        
        for (uint64_t seq = 0; seq < 1000000; ++seq) {
            if (disruptor.get_event_timeout(seq, event, 1000)) {
                std::cout << "Trade " << seq << ": " << event.symbol 
                          << " @ " << event.price << std::endl;
            }
        }
    });
    
    publisher.join();
    subscriber.join();
}
```

#### Disruptor Characteristics

```
Advantages:
✓ Ultra-fast: ~50-100 nanoseconds per operation
✓ Lock-free: No mutexes or semaphores
✓ Scalable: Multiple consumers without contention
✓ Deterministic: Constant latency (no garbage collection pauses)
✓ Industry-proven: Used by trading firms globally

Disadvantages:
✗ Complex: Difficult to implement correctly
✗ CPU-intensive: Busy-waiting consumes CPU cycles
✗ Memory overhead: Fixed ring buffer size
✗ Learning curve: Requires understanding of memory ordering

Latency Profile:
├─ Publish: ~50 nanoseconds
├─ Consume (event ready): ~50 nanoseconds
├─ Consume (event not ready, busy-wait): ~100 nanoseconds
```

---

## Trading-Specific Applications

### Architecture: Market Data Distribution

```
Market Data Source (exchange feed)
├─ 100K quotes/second
└─ Requirements: <100 μs latency, multiple subscribers

Solution using Ring Buffer:

┌─────────────────┐
│ Market Data     │ Write PTP timestamp
│ Receiver        │ Decode quote
│ (Real-time)     │ Push to ring buffer (< 100 ns)
└────────┬────────┘
         │
         ↓
    ┌─────────────────────────────┐
    │  Disruptor Ring Buffer      │
    │  (shared memory)            │
    │  Size: 65K quotes           │
    │  Throughput: 1M quotes/sec  │
    │  Latency: < 100 ns          │
    └──────────┬────────┬────────┬┘
               │        │        │
         ┌─────▼─┐ ┌────▼──┐ ┌───▼────┐
         │Strategy├─Strategy├─Strategy│
         │1       │ 2      │ 3      │
         │(Thread)│(Thread)│(Thread)│
         └────────┘ └───────┘ └────────┘
```

**Implementation:**

```cpp
#include <boost/interprocess/managed_shared_memory.hpp>
#include <atomic>

using namespace boost::interprocess;

struct Quote {
    uint64_t sequence;
    uint64_t ptp_timestamp;
    char symbol[10];
    double bid;
    double ask;
    int bid_size;
    int ask_size;
};

class QuoteDisruptor {
    static const size_t BUFFER_SIZE = 65536;
    std::array<Quote, BUFFER_SIZE> buffer;
    std::atomic<uint64_t> next_seq{0};
    
public:
    void publish(const Quote& q) {
        uint64_t seq = next_seq.fetch_add(1, std::memory_order_acq_rel);
        buffer[seq % BUFFER_SIZE] = q;
    }
    
    bool get_quote(uint64_t seq, Quote& out_q) {
        size_t index = seq % BUFFER_SIZE;
        
        // Busy-wait for sequence
        while (buffer[index].sequence != seq) {
            _mm_pause();
        }
        
        out_q = buffer[index];
        return true;
    }
};

// Market data receiver
void market_data_receiver(QuoteDisruptor& disruptor) {
    uint64_t seq = 0;
    
    while (true) {
        Quote q = receive_quote_from_exchange();
        q.sequence = seq;
        q.ptp_timestamp = get_ptp_timestamp();
        
        disruptor.publish(q);
        seq++;
    }
}

// Trading strategy (consumer)
void trading_strategy(QuoteDisruptor& disruptor, int strategy_id) {
    uint64_t next_seq = 0;
    Quote q;
    
    while (true) {
        if (disruptor.get_quote(next_seq, q)) {
            // Process quote
            if (q.ask < calculate_fair_value()) {
                submit_buy_order(q.ask, 1000);
            }
            next_seq++;
        }
    }
}
```

---

### Position Tracking with Shared Memory

```cpp
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

using namespace boost::interprocess;

struct Position {
    char symbol[10];
    int quantity;
    double avg_cost;
    double current_price;
    double unrealized_pnl;
    uint64_t last_updated;
};

class PositionManager {
    managed_shared_memory& segment;
    named_mutex& mutex;
    
public:
    PositionManager(managed_shared_memory& seg, named_mutex& mtx) 
        : segment(seg), mutex(mtx) {}
    
    void update_position(const std::string& symbol, int qty, double price) {
        scoped_lock<named_mutex> lock(mutex);
        
        // Find or create position
        typedef std::pair<const char, Position> MapType;
        typedef allocator<MapType, managed_shared_memory::segment_manager> Alloc;
        typedef std::map<std::string, Position, std::less<std::string>, Alloc> PosMap;
        
        // Get or create position
        auto& positions = *segment.find_or_construct<PosMap>("positions")(
            std::less<std::string>(),
            segment.get_segment_manager()
        );
        
        // Update position
        Position& pos = positions[symbol];
        if (pos.quantity == 0) {
            pos.avg_cost = price;
        } else {
            pos.avg_cost = (pos.avg_cost * pos.quantity + price * qty) / 
                          (pos.quantity + qty);
        }
        
        pos.quantity += qty;
        pos.current_price = price;
        pos.unrealized_pnl = pos.quantity * (price - pos.avg_cost);
        pos.last_updated = get_timestamp_ns();
    }
    
    double get_total_pnl() {
        scoped_lock<named_mutex> lock(mutex);
        
        auto positions = segment.find<std::map<std::string, Position>>("positions").first;
        
        double total = 0.0;
        for (const auto& [symbol, pos] : *positions) {
            total += pos.unrealized_pnl;
        }
        
        return total;
    }
};
```

---

## Debugging & Monitoring

### Tools for IPC Debugging

#### ipcs: List IPC Resources

```bash
# List all shared memory segments
ipcs -m

# Output:
# ------ Message Queues --------
# key        msqid      owner      perms      used-bytes   messages
# 0x00000000 0          root       666        0            0
#
# ------ Shared Memory Segments --------
# key        shmid      owner      perms      bytes        nattch     status
# 0x51234567 262144     user       666        4096         2          dest
```

#### Remove IPC Resources

```bash
# Remove shared memory
ipcrm -m <shmid>

# Remove all
ipcrm -a
```

#### strace: Trace System Calls

```bash
# Trace all system calls
strace -o trace.log ./program

# Trace specific syscalls
strace -e trace=write,read -o trace.log ./program

# See which processes access shared memory
strace -e trace=shmat,shmget ./program
```

#### perf: Performance Profiling

```bash
# Record CPU cycles (see where time is spent)
perf record -e cycles ./program

# Show report
perf report

# Check for cache misses
perf stat -e LLC-loads,LLC-load-misses ./program
```

### Latency Monitoring

```cpp
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>

class LatencyTracker {
    std::vector<uint64_t> latencies_ns;
    
public:
    void record(uint64_t latency_ns) {
        latencies_ns.push_back(latency_ns);
    }
    
    void print_stats() {
        if (latencies_ns.empty()) return;
        
        std::sort(latencies_ns.begin(), latencies_ns.end());
        
        size_t n = latencies_ns.size();
        
        double min = latencies_ns[0];
        double max = latencies_ns[n-1];
        double mean = std::accumulate(latencies_ns.begin(), latencies_ns.end(), 0.0) / n;
        
        double p50 = latencies_ns[n * 50 / 100];
        double p95 = latencies_ns[n * 95 / 100];
        double p99 = latencies_ns[n * 99 / 100];
        double p999 = latencies_ns[n * 999 / 1000];
        
        std::cout << "Latency (ns):\n"
                  << "  Min:  " << (long long)min << "\n"
                  << "  P50:  " << (long long)p50 << "\n"
                  << "  P95:  " << (long long)p95 << "\n"
                  << "  P99:  " << (long long)p99 << "\n"
                  << "  P99.9: " << (long long)p999 << "\n"
                  << "  Mean: " << (long long)mean << "\n"
                  << "  Max:  " << (long long)max << "\n";
    }
};

// Usage
LatencyTracker tracker;

void test_ring_buffer_latency() {
    LockFreeRingBuffer<Quote> buffer;
    
    for (int i = 0; i < 1000000; ++i) {
        Quote q;
        q.price = 175.50 + (rand() % 100) * 0.01;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        buffer.push(q);
        
        Quote out;
        buffer.pop(out);
        
        auto end = std::chrono::high_resolution_clock::now();
        
        auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(
            end - start
        ).count();
        
        tracker.record(latency);
    }
    
    tracker.print_stats();
}
```

---

## Best Practices

### 1. Choose the Right IPC for Your Use Case

```
Real-time trading:
├─ Market data: Ring buffer (Disruptor)
├─ Position updates: Shared memory + mutex
├─ Risk checks: Shared memory + atomic
└─ Audit trail: Memory-mapped file

Periodic reporting:
├─ Reports: Message queues or pipes
├─ Notifications: Signals or condition variables
└─ Logging: Memory-mapped file

Cross-network:
├─ Communication: TCP sockets
├─ Streaming: gRPC or custom protocol
└─ Distributed: Message brokers (Kafka, RabbitMQ)
```

### 2. Avoid Common Pitfalls

```
✗ Forgetting to cleanup IPC resources
  → Use ipcrm -a before testing new code

✗ Deadlock in mutex-protected sections
  → Keep critical section small
  → Use timeouts: scoped_lock with timeout

✗ Data corruption from race conditions
  → Use proper synchronization (mutex, atomic)
  → Test with multiple processes simultaneously

✗ Page faults in memory-mapped files
  → Pre-allocate file to avoid on-demand paging
  → Use madvise(MADV_SEQUENTIAL) for streaming

✗ Buffer overflows in shared memory
  → Check buffer bounds before writing
  → Use ASLR (Address Space Layout Randomization) for security

✗ Cache line false sharing
  → Align data structures to 64-byte cache lines
  → Separate read-only from read-write data
```

### 3. Performance Optimization Checklist

```
☐ Profile first (identify actual bottleneck)
☐ Use lock-free (atomics) instead of mutexes
☐ Align data to cache lines (avoid false sharing)
☐ Pre-allocate memory (avoid allocation latency)
☐ Use ring buffers for streaming data
☐ Batch operations (reduce context switches)
☐ Pin threads to CPU cores (isolation)
☐ Use memory-mapped files for persistence
☐ Monitor latency percentiles (P99 is important)
☐ Test under realistic load (peak traffic)
```

### 4. Debugging Checklist

```
☐ Check strace output for unexpected syscalls
☐ Monitor context switches (vmstat)
☐ Profile cache misses (perf stat)
☐ Verify thread affinity (taskset)
☐ Check for memory leaks (valgrind)
☐ Trace latency outliers (perf record)
☐ Test cleanup (ipcs -m should be empty)
☐ Verify NUMA locality (numastat)
```

---

## Summary: IPC Mechanisms Comparison

| Mechanism | Latency | Use Case | Pros | Cons |
|-----------|---------|----------|------|------|
| **Signals** | N/A | Notifications | Simple | No data |
| **Pipes** | 1-10 ms | Filtering | Easy | One-way, slow |
| **Sockets** | 1-10 ms | Network | Reliable | Overhead |
| **Shared Memory** | 100 ns | Fast coordination | Ultra-fast | Complex sync |
| **Memory-mapped File** | 100 ns* | Persistence | Fast+persistent | Page faults |
| **Ring Buffer** | 100 ns | Streaming | Ultra-fast | Fixed size |
| **Disruptor** | 100 ns | HFT | Production-grade | Complex |

*If page cached

---

## References & Further Reading

- POSIX IPC: `man 7 ipc`
- Boost.Interprocess: https://www.boost.org/doc/libs/release/libs/interprocess/
- LMAX Disruptor: https://github.com/LMAX-Exchange/disruptor
- Linux man pages: `man 2 shmat`, `man 2 mmap`, `man 7 semaphore`
- Mechanical Sympathy: https://mechanical-sympathy.blogspot.com/

---

**Created for comprehensive IPC understanding from basics to production-grade trading systems.**
