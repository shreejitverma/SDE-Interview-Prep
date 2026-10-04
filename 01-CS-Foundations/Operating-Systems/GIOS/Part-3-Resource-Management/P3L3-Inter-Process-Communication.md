---
type: concept
track: [sde]
level: advanced
status: complete
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P3L3"
  - "The Linux Programming Interface, Kerrisk"
  - "Advanced Programming in the UNIX Environment, Stevens & Rago"
---

# P3L3: Inter-Process Communication

> **Module goal:** Master IPC mechanisms - pipes (named/unnamed), shared memory (POSIX and System V), memory-mapped files, message queues, sockets, signals, and understand their tradeoffs for different communication patterns.

## Table of Contents

- [1. IPC Overview and Taxonomy](#1-ipc-overview-and-taxonomy)
- [2. Pipes: Anonymous and Named (FIFOs)](#2-pipes-anonymous-and-named-fifos)
- [3. Shared Memory: POSIX](#3-shared-memory-posix)
- [4. Shared Memory: System V](#4-shared-memory-system-v)
- [5. Memory-Mapped Files (mmap)](#5-memory-mapped-files-mmap)
- [6. Message Queues](#6-message-queues)
- [7. Unix Domain Sockets](#7-unix-domain-sockets)
- [8. Signals as IPC](#8-signals-as-ipc)
- [9. Windows IPC Mechanisms](#9-windows-ipc-mechanisms)
- [10. IPC Comparison and Selection Guide](#10-ipc-comparison-and-selection-guide)
- [11. Quizzes and Exercises](#11-quizzes-and-exercises)
- [12. Key Takeaways](#12-key-takeaways)
- [13. Advanced IPC: eventfd, signalfd, io_uring](#13-advanced-ipc-eventfd-signalfd-io_uring)
- [14. Cross-Process Synchronization via Shared Memory](#14-cross-process-synchronization-via-shared-memory)
- [15. IPC Performance Deep Dive](#15-ipc-performance-deep-dive)
- [16. Inter-Host Communication: Socket Programming Patterns](#16-inter-host-communication-socket-programming-patterns)
- [17. macOS Mach Messaging and Darwin IPC Internals](#17-macos-mach-messaging-and-darwin-ipc-internals)

---

## 1. IPC Overview and Taxonomy

```
IPC Mechanisms by Data Transfer Method:

Message-Based                     Shared Memory-Based
(kernel copies data)              (direct access, user-managed)
+--------------------+            +--------------------+
| Pipes              |            | POSIX shm_open     |
| Message Queues     |            | System V shmget    |
| Sockets            |            | mmap (shared)      |
| Signals (limited)  |            +--------------------+
+--------------------+
       |                                   |
       v                                   v
  Synchronization                   Synchronization
  built-in (blocking               must be explicitly
  read/write)                      managed (semaphores,
                                   mutexes, etc.)
```

| IPC Type | Data Direction | Best For |
|----------|---------------|----------|
| **Pipe** | Unidirectional (or 2 pipes for bidirectional) | Parent-child, shell pipelines |
| **Named Pipe (FIFO)** | Unidirectional (filesystem path) | Unrelated processes |
| **Shared Memory** | Bidirectional (fastest) | High-throughput, low-latency |
| **Message Queue** | Bidirectional, typed messages | Decoupled producers/consumers |
| **Socket** | Bidirectional (network-capable) | Network communication, flexibility |
| **Signal** | Notification only (no data) | Asynchronous events |

---

## 2. Pipes: Anonymous and Named (FIFOs)

### Anonymous Pipes

Unidirectional byte stream between related processes (parent-child via fork).

```c
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];  // pipefd[0] = read end, pipefd[1] = write end

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child: reads from pipe
        close(pipefd[1]);  // Close write end
        char buf[256];
        ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1);
        buf[n] = '\0';
        printf("Child received: %s\n", buf);
        close(pipefd[0]);
    } else {
        // Parent: writes to pipe
        close(pipefd[0]);  // Close read end
        const char *msg = "Hello from parent!";
        write(pipefd[1], msg, strlen(msg));
        close(pipefd[1]);  // EOF signal to child
        wait(NULL);
    }

    return 0;
}
```

**Shell pipelines use anonymous pipes:**
```bash
# ls | grep ".c" | wc -l
# Shell creates two pipes:
#   ls -> pipe1 -> grep -> pipe2 -> wc
# Each | creates a pipe(pipefd), fork(), and dup2() to redirect stdin/stdout
```

### Named Pipes (FIFOs)

Persistent filesystem entry; unrelated processes can communicate.

```bash
# Create a named pipe
mkfifo /tmp/myfifo

# Terminal 1: writer
echo "Hello via FIFO" > /tmp/myfifo

# Terminal 2: reader (blocks until data available)
cat /tmp/myfifo
# Output: Hello via FIFO

# Cleanup
rm /tmp/myfifo
```

```c
// Writer process
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    mkfifo("/tmp/myfifo", 0666);
    int fd = open("/tmp/myfifo", O_WRONLY);
    const char *msg = "Data from writer process\n";
    write(fd, msg, strlen(msg));
    close(fd);
    return 0;
}

// Reader process
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    int fd = open("/tmp/myfifo", O_RDONLY);
    char buf[256];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    buf[n] = '\0';
    printf("Reader got: %s\n", buf);
    close(fd);
    return 0;
}
```

---

## 3. Shared Memory: POSIX

Fastest IPC - processes map the same physical pages into their address spaces. No kernel copy on read/write.

```c
// Writer process
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    const char *name = "/my_shm";
    const int SIZE = 4096;

    // Create shared memory object
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);

    // Map into address space
    void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0);
    close(fd);

    // Write data
    sprintf(ptr, "Hello from shared memory writer! PID=%d", getpid());
    printf("Writer: wrote to shared memory\n");

    // Don't unlink yet - reader needs it
    sleep(5);
    munmap(ptr, SIZE);
    shm_unlink(name);
    return 0;
}

// Reader process
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main(void) {
    const char *name = "/my_shm";
    const int SIZE = 4096;

    int fd = shm_open(name, O_RDONLY, 0666);
    void *ptr = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);

    printf("Reader: %s\n", (char*)ptr);

    munmap(ptr, SIZE);
    return 0;
}
```

```bash
# Compile (link with -lrt on Linux)
gcc -o shm_writer shm_writer.c -lrt
gcc -o shm_reader shm_reader.c -lrt

# List POSIX shared memory objects
ls -la /dev/shm/

# Remove shared memory object
rm /dev/shm/my_shm
```

---

## 4. Shared Memory: System V

Older API, still widely used.

```c
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

int main(void) {
    key_t key = ftok("/tmp", 'R');  // Generate unique key

    // Create shared memory segment
    int shmid = shmget(key, 4096, IPC_CREAT | 0666);

    // Attach to address space
    char *data = (char *)shmat(shmid, NULL, 0);

    // Write data
    strcpy(data, "Hello from System V shared memory!");
    printf("Wrote: %s\n", data);

    // Detach (don't destroy)
    shmdt(data);

    // To destroy: shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
```

```bash
# List System V IPC objects
ipcs -m   # shared memory
ipcs -q   # message queues
ipcs -s   # semaphores

# Remove a shared memory segment
ipcrm -m $SHMID
```

---

## 5. Memory-Mapped Files (mmap)

Map a file directly into virtual memory. Changes to the mapped region are reflected in the file (and vice versa).

```c
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    const char *filepath = "/tmp/mmap_test.txt";

    // Create and populate file
    int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, 0666);
    const char *initial = "Hello, mmap world! This is the initial content.\n";
    write(fd, initial, strlen(initial));

    // Get file size
    struct stat st;
    fstat(fd, &st);

    // Map file into memory
    char *mapped = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, 0);
    close(fd);

    // Read via mmap
    printf("Read via mmap: %s", mapped);

    // Modify via mmap (changes written to file!)
    memcpy(mapped, "MODIFIED", 8);

    // Force sync to disk
    msync(mapped, st.st_size, MS_SYNC);

    munmap(mapped, st.st_size);

    // Verify: read file normally
    fd = open(filepath, O_RDONLY);
    char buf[256];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    buf[n] = '\0';
    printf("Read via read(): %s", buf);
    close(fd);

    return 0;
}
// Output:
// Read via mmap: Hello, mmap world! This is the initial content.
// Read via read(): MODIFIED mmap world! This is the initial content.
```

### mmap Flags

| Flag | Meaning |
|------|---------|
| `MAP_SHARED` | Changes visible to other processes and written to file |
| `MAP_PRIVATE` | Copy-on-write; changes are private to this process |
| `MAP_ANONYMOUS` | No file backing; used for shared memory between parent/child |
| `MAP_FIXED` | Map at exact address (dangerous) |
| `MAP_HUGETLB` | Use huge pages |

---

## 6. Message Queues

### POSIX Message Queues

Typed, priority-ordered message passing.

```c
// Sender
#include <mqueue.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = 256,
        .mq_curmsgs = 0
    };

    mqd_t mq = mq_open("/my_queue", O_CREAT | O_WRONLY, 0666, &attr);

    const char *msg = "Priority message!";
    mq_send(mq, msg, strlen(msg) + 1, 5);  // priority 5

    const char *msg2 = "Normal message";
    mq_send(mq, msg2, strlen(msg2) + 1, 1);  // priority 1

    mq_close(mq);
    return 0;
}

// Receiver
#include <mqueue.h>
#include <stdio.h>

int main(void) {
    mqd_t mq = mq_open("/my_queue", O_RDONLY);

    char buf[256];
    unsigned int priority;

    // Receives highest priority message first
    mq_receive(mq, buf, sizeof(buf), &priority);
    printf("Received (priority %u): %s\n", priority, buf);

    mq_receive(mq, buf, sizeof(buf), &priority);
    printf("Received (priority %u): %s\n", priority, buf);

    mq_close(mq);
    mq_unlink("/my_queue");
    return 0;
}
```

```bash
# Compile with -lrt
gcc -o mq_send mq_send.c -lrt
gcc -o mq_recv mq_recv.c -lrt

# See message queue limits
cat /proc/sys/fs/mqueue/msg_max
cat /proc/sys/fs/mqueue/msgsize_max
```

---

## 7. Unix Domain Sockets

Local IPC with socket API semantics. Faster than TCP/IP (no network stack overhead).

```c
// Server (Unix Domain Socket)
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_PATH "/tmp/my_uds.sock"

int main(void) {
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("Server listening on %s\n", SOCKET_PATH);

    int client_fd = accept(server_fd, NULL, NULL);
    char buf[256];
    ssize_t n = read(client_fd, buf, sizeof(buf) - 1);
    buf[n] = '\0';
    printf("Server received: %s\n", buf);

    const char *reply = "ACK from server";
    write(client_fd, reply, strlen(reply));

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}

// Client (Unix Domain Socket)
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_PATH "/tmp/my_uds.sock"

int main(void) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    connect(fd, (struct sockaddr*)&addr, sizeof(addr));

    const char *msg = "Hello from client!";
    write(fd, msg, strlen(msg));

    char buf[256];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    buf[n] = '\0';
    printf("Client received: %s\n", buf);

    close(fd);
    return 0;
}
```

---

## 8. Signals as IPC

Signals are asynchronous notifications. Limited data transfer (signal number only, or `sigval` with `sigqueue`).

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void handler(int sig, siginfo_t *info, void *context) {
    printf("Received signal %d with value %d from PID %d\n",
           sig, info->si_value.sival_int, info->si_pid);
}

int main(void) {
    struct sigaction sa = {0};
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);

    printf("PID=%d, waiting for SIGUSR1...\n", getpid());
    pause();  // Wait for signal

    return 0;
}

// Sender (from another process):
// kill -USR1 $PID
// Or with data:
// union sigval val = { .sival_int = 42 };
// sigqueue(target_pid, SIGUSR1, val);
```

---

## 9. Windows IPC Mechanisms

| Linux IPC | Windows Equivalent | API |
|-----------|-------------------|-----|
| Anonymous pipe | Anonymous pipe | `CreatePipe()` |
| Named pipe (FIFO) | Named pipe | `CreateNamedPipe()` |
| POSIX shared memory | File mapping | `CreateFileMapping()` + `MapViewOfFile()` |
| Message queue | Mailslots | `CreateMailslot()` |
| Unix domain socket | Named pipe (bidirectional) | `CreateNamedPipe()` |
| Signal | Events / APC | `SetEvent()`, `QueueUserAPC()` |

### Windows Named Pipe (Bidirectional)

```c
// Server
HANDLE hPipe = CreateNamedPipe(
    TEXT("\\\\.\\pipe\\MyPipe"),
    PIPE_ACCESS_DUPLEX,
    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
    1, 4096, 4096, 0, NULL);

ConnectNamedPipe(hPipe, NULL);  // Wait for client

char buf[256];
DWORD bytesRead;
ReadFile(hPipe, buf, sizeof(buf), &bytesRead, NULL);
printf("Server received: %s\n", buf);

const char *reply = "ACK";
DWORD bytesWritten;
WriteFile(hPipe, reply, strlen(reply) + 1, &bytesWritten, NULL);

DisconnectNamedPipe(hPipe);
CloseHandle(hPipe);

// Client
HANDLE hPipe = CreateFile(
    TEXT("\\\\.\\pipe\\MyPipe"),
    GENERIC_READ | GENERIC_WRITE,
    0, NULL, OPEN_EXISTING, 0, NULL);

const char *msg = "Hello from client";
DWORD bytesWritten, bytesRead;
WriteFile(hPipe, msg, strlen(msg) + 1, &bytesWritten, NULL);

char buf[256];
ReadFile(hPipe, buf, sizeof(buf), &bytesRead, NULL);
printf("Client received: %s\n", buf);

CloseHandle(hPipe);
```

---

## 10. IPC Comparison and Selection Guide

| Mechanism | Speed | Ease | Persistence | Network | Data Size | Use Case |
|-----------|-------|------|-------------|---------|-----------|----------|
| Pipe | Fast | Easy | No | No | Stream | Shell, parent-child |
| Named Pipe | Fast | Easy | Filesystem | No | Stream | Unrelated processes |
| Shared Memory | Fastest | Hard | Kernel lifetime | No | Large | High-throughput |
| mmap | Fast | Moderate | File-backed | No | Large | File-based sharing |
| Message Queue | Moderate | Moderate | Kernel lifetime | No | Sized msgs | Typed messages |
| Unix Socket | Fast | Moderate | Filesystem | No | Stream/dgram | Client-server |
| TCP/UDP Socket | Slower | Moderate | No | Yes | Stream/dgram | Network comms |
| Signal | Fastest | Hard | No | No | Minimal | Notifications |

---

## 11. Quizzes and Exercises

### Quiz 1: IPC Mechanism Comparison (Clips 300-301)

> [!question]
> We saw two major paradigms for implementing IPC: message passing (e.g., pipes, message queues, sockets) and shared memory (e.g., POSIX `shm_open`, System V `shmget`).
> Which one of the two performs better?
> 
> 1. Message passing
> 2. Shared memory
> 3. It depends

> [!success]- Answer
> The correct answer is **3. It depends**.
> 
> **Rationale:**
> - **Message passing:** Requires multiple data copies between process user space and kernel space buffers (`user -> kernel -> user`), causing CPU and cache overhead.
> However, for small payloads, the setup cost is near zero, and synchronization is automatically handled by the kernel via blocking read/write calls.
> - **Shared memory:** Avoids all subsequent data copies because processes read and write directly to physical frames mapped into their virtual address spaces.
> However, establishing the mapping incurs significant initial overhead (allocating physical frames, modifying page tables, configuring TLB entries).
> Furthermore, processes must explicitly coordinate synchronization using semaphores or mutexes.
> - **Conclusion:** For small messages or single transfers, message passing is faster because the mapping overhead exceeds the copy cost.
> For large data transfers or continuous high-bandwidth communication, shared memory dominates because the one-time mapping cost is amortized over many zero-copy accesses.

---

### Quiz 2: Message Queue System Calls Treasure Hunt (Clips 309-310)

> [!question]
> What are the exact System V Linux system call names used for the following operations?
> 1. Send a message to a message queue
> 2. Receive a message from a message queue
> 3. Perform a control operation on a message queue
> 4. Obtain or create a message queue identifier

> [!success]- Answer
> 1. Send: `msgsnd`
> 2. Receive: `msgrcv`
> 3. Control: `msgctl`
> 4. Obtain identifier: `msgget`

---

### Quiz 3: Copy vs. Map Threshold (Lecture 302)

> [!question]
> Suppose a message-based IPC channel copies data twice ($2 \times \text{size}$ bytes) at a memory bandwidth of 10 GB/s.
> Establishing a shared memory mapping requires a round of page table modifications and TLB invalidations costing approximately 2.5 microseconds.
> Below what payload size does message copying outperform establishing shared memory?

> [!success]- Answer
> Let $S$ be the payload size in bytes.
> The copying time is:
> $$T_{\text{copy}} = \frac{2 \times S}{10 \times 10^9 \text{ bytes/sec}}$$
> Setting $T_{\text{copy}} < 2.5 \times 10^{-6} \text{ seconds}$:
> $$2 \times S < 2.5 \times 10^{-6} \times 10 \times 10^9 = 25,000 \text{ bytes}$$
> $$S < 12,500 \text{ bytes} \approx 12.2 \text{ KB}$$
> 
> For data sizes below approximately 12 KB, the CPU time required to copy data twice is less than the kernel overhead of modifying page tables and mapping virtual pages.
> This mathematical threshold is why production microkernels and IPC frameworks use message copying for small control payloads and switch to page remapping/shared memory for bulk transfers.

---

### Quiz 4: Cross-Process Pthreads Synchronization

> [!question]
> When two independent processes coordinate access to a shared memory region, can standard `pthread_mutex_t` and `pthread_cond_t` variables be used?
> If so, what special initialization steps are mandatory?

> [!success]- Answer
> **Yes**, standard Pthreads mutexes and condition variables can be placed inside a shared memory segment, provided:
> 1. The mutex/condvar is allocated directly inside the shared memory region so both processes access the same physical bytes.
> 2. The mutex attributes are initialized with `pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)`.
> 3. The condition variable attributes are initialized with `pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED)`.
> 
> By default, mutex attributes are set to `PTHREAD_PROCESS_PRIVATE`, which allows the pthread library to optimize under the assumption that only threads within the calling process will access the lock.
> Specifying `PTHREAD_PROCESS_SHARED` instructs the kernel and runtime to coordinate lock state across address space boundaries.

---

### Exercise: Runnable Shell Pipeline (`cmd1 | cmd2`)

```c
/* pipeline_demo.c - Implements a UNIX shell pipeline: "ls -l | wc -l" */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        /* Child 1: executes "ls -l", writes stdout into pipe */
        close(pipefd[0]); /* Close unused read end */
        dup2(pipefd[1], STDOUT_FILENO); /* Redirect stdout to write end */
        close(pipefd[1]);

        char *argv[] = {"ls", "-l", NULL};
        execvp(argv[0], argv);
        perror("execvp ls");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        /* Child 2: executes "wc -l", reads stdin from pipe */
        close(pipefd[1]); /* Close unused write end */
        dup2(pipefd[0], STDIN_FILENO); /* Redirect stdin to read end */
        close(pipefd[0]);

        char *argv[] = {"wc", "-l", NULL};
        execvp(argv[0], argv);
        perror("execvp wc");
        exit(EXIT_FAILURE);
    }

    /* Parent closes both pipe ends and waits for both children */
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
```

---

## 12. Key Takeaways

1. **Pipes** are the simplest IPC; anonymous for parent-child, named (FIFO) for unrelated processes.
2. **Shared memory** is the fastest IPC (no kernel copy) but requires explicit synchronization.
3. **mmap** provides file-backed shared memory and is used extensively by the kernel (page cache, shared libraries).
4. **Message queues** provide typed, priority-ordered messages; good for decoupled architectures.
5. **Unix domain sockets** provide full socket semantics without network overhead; widely used by system services (D-Bus, systemd).
6. **Signals** are for notifications only; use `sigqueue()` for minimal data transfer.
7. Choose based on: data volume (shared memory for large), coupling (message queue for loose), and whether network is needed (sockets).

---

## 13. Advanced IPC: eventfd, signalfd, io_uring

### eventfd - Lightweight Event Notification

`eventfd` is the modern Linux primitive for signaling between threads or processes without a full pipe overhead.
It uses a single 64-bit counter; write adds to counter, read blocks until counter > 0.

```c
#include <sys/eventfd.h>
#include <stdint.h>

// Create eventfd counter (EFD_NONBLOCK = non-blocking; EFD_SEMAPHORE = semaphore mode)
int efd = eventfd(0, EFD_NONBLOCK);

// Signal: increment counter by 1
uint64_t val = 1;
write(efd, &val, 8);

// Wait: read blocks until counter > 0, then atomically resets to 0
uint64_t count;
read(efd, &count, 8);  // count = how many events fired
printf("Got %lu events\n", count);
```

**Used by:** epoll/io_uring, futexes, Docker container events, gRPC async.

```bash
# Monitor eventfd with strace
strace -e trace=eventfd2,read,write -p <pid>

# eventfd in /proc
ls /proc/<pid>/fd        # Shows all open file descriptors including eventfd
cat /proc/<pid>/fdinfo/<fd_num>   # Shows current counter value
```

### timerfd - Kernel Timers as File Descriptors

```c
#include <sys/timerfd.h>
int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);

struct itimerspec ts = {
    .it_value    = { .tv_sec = 1, .tv_nsec = 0 },  /* First expiry: 1s */
    .it_interval = { .tv_sec = 1, .tv_nsec = 0 },  /* Repeat: every 1s */
};
timerfd_settime(tfd, 0, &ts, NULL);

// Poll/epoll for timer events - no signals needed!
uint64_t expirations;
read(tfd, &expirations, 8);
printf("Timer fired %lu times\n", expirations);
```

**Advantage:** Integrates with epoll for event-driven timer handling without SIGALRM.

### io_uring - Modern Async I/O (Linux 5.1+)

`io_uring` is the state-of-the-art Linux async I/O framework replacing epoll+aio.
Uses two shared ring buffers between kernel and user space for **zero-copy**, **zero-syscall** submission.

```
io_uring Architecture:

User Space:                           Kernel Space:
+------------------+                  +------------------+
| Submission Queue |  <-- SQ ring --> | io_uring core    |
| (SQE array)      |                  |                  |
|                  |  --> CQ ring --> | Worker threads   |
| Completion Queue |                  | (optional)       |
+------------------+                  +------------------+
       ↑
       | mmap'd shared memory - no syscall for submit/complete!
```

```c
#include <liburing.h>
/* Compile: gcc -o io_uring_demo io_uring_demo.c -luring */

struct io_uring ring;
io_uring_queue_init(256, &ring, 0);  /* 256-entry ring */

/* Submit a read operation */
struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
io_uring_prep_read(sqe, fd, buf, sizeof(buf), 0);
sqe->user_data = (uint64_t)buf;  /* Tag for correlation */
io_uring_submit(&ring);           /* One syscall for MANY ops! */

/* Wait for completion */
struct io_uring_cqe *cqe;
io_uring_wait_cqe(&ring, &cqe);
printf("Read %d bytes\n", cqe->res);
io_uring_cqe_seen(&ring, cqe);   /* Advance completion queue */

io_uring_queue_exit(&ring);
```

**io_uring vs. epoll vs. AIO:**

| Feature | epoll | AIO (libaio) | io_uring |
|---------|-------|-------------|----------|
| Supported ops | I/O ready events | Read/write only | 50+ ops (read, write, accept, sendmsg, fsync, ...) |
| Syscalls per op | 1 (epoll_wait) | 2 (submit+getevents) | ~0 (batch: 1 for many) |
| Zero-copy | No | No | Yes (fixed buffers) |
| Linked ops | No | No | Yes (chained SQEs) |
| Kernel threads | No | Sometimes | Optional (SQPOLL mode) |
| Latency | Low | Medium | Lowest |

```bash
# Check io_uring support
grep CONFIG_IO_URING /boot/config-$(uname -r)

# Monitor io_uring operations (kernel 5.15+)
cat /proc/<pid>/io_uring

# Using liburing high-level API
# Install: sudo apt install liburing-dev
# Docs: https://unixism.net/loti/
```

---

## 14. D-Bus: System Message Bus

D-Bus is the dominant IPC mechanism for Linux desktop and system services.
Used by: systemd, NetworkManager, BlueZ, PulseAudio, GNOME, KDE.

```
D-Bus Architecture:
+----------+     +-----------+     +----------+
| App A    |<--->| dbus-     |<--->| App B    |
| (client) |     | daemon    |     | (server) |
+----------+     | (message  |     +----------+
                 | router)   |
                 +-----------+
                       |
                  Two buses:
                  - system bus  (privileged services)
                  - session bus (per-user desktop apps)
```

```bash
# List all services on session bus
dbus-send --session --dest=org.freedesktop.DBus \
    --type=method_call --print-reply \
    /org/freedesktop/DBus \
    org.freedesktop.DBus.ListNames

# Query systemd unit state via D-Bus
dbus-send --system --dest=org.freedesktop.systemd1 \
    --type=method_call --print-reply \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager.ListUnits

# Watch all D-Bus messages (very verbose!)
dbus-monitor --system

# Introspect a service (list its methods/signals)
gdbus introspect --system \
    --dest=org.freedesktop.NetworkManager \
    --object-path=/org/freedesktop/NetworkManager

# Call a D-Bus method from shell
gdbus call --system \
    --dest=org.freedesktop.NetworkManager \
    --object-path=/org/freedesktop/NetworkManager \
    --method=org.freedesktop.NetworkManager.GetDevices
```

**D-Bus concepts:**

| Term | Description |
|------|-------------|
| Bus name | Service identifier (`org.freedesktop.NetworkManager`) |
| Object path | Resource within service (`/org/freedesktop/NetworkManager`) |
| Interface | Group of methods/signals (`org.freedesktop.DBus.Properties`) |
| Method call | Request/response (synchronous or async) |
| Signal | Broadcast notification (fire-and-forget) |
| Property | Named value (get/set via `org.freedesktop.DBus.Properties`) |

---

## 15. IPC Performance Deep Dive

### Latency Measurements (Same machine, 4KB payload)

```bash
# Install ipc-bench
git clone https://github.com/goldsborough/ipc-bench
cd ipc-bench && cmake . && make

# Benchmark pipe latency
./ipc-bench pipe --count 100000 --size 4096

# Benchmark Unix socket latency
./ipc-bench unix --count 100000 --size 4096

# Benchmark shared memory latency
./ipc-bench shm --count 100000 --size 4096
```

Typical results on modern Linux (single-socket Intel):

| Mechanism | Latency (p50) | Throughput | Notes |
|-----------|--------------|------------|-------|
| Shared memory + futex | 0.2 µs | 8 GB/s | Fastest; requires sync |
| eventfd | 0.5 µs | N/A | Signal only |
| Unix socket (stream) | 1.5 µs | 4 GB/s | Full socket API |
| Pipe | 2.0 µs | 3 GB/s | Byte stream |
| POSIX message queue | 3.0 µs | 1 GB/s | Typed messages |
| TCP loopback | 10–30 µs | 1 GB/s | Network stack overhead |

### Tuning Pipe and Socket Buffers

```bash
# View current pipe capacity (bytes)
ulimit -p                            # In 512-byte blocks
cat /proc/sys/fs/pipe-max-size       # Maximum per pipe

# Increase pipe buffer (requires CAP_SYS_RESOURCE or root)
echo 1048576 > /proc/sys/fs/pipe-max-size  # 1 MB max

# In code: set pipe capacity
int fd[2]; pipe(fd);
fcntl(fd[1], F_SETPIPE_SZ, 1024 * 1024);  /* Request 1MB buffer */
int actual = fcntl(fd[1], F_GETPIPE_SZ);

# Socket buffer sizes
sysctl net.core.rmem_max             # Max receive buffer
sysctl net.core.wmem_max             # Max send buffer
sysctl net.core.rmem_default         # Default receive buffer

# Set socket buffer per socket
setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
```

### Profiling IPC with perf and strace

```bash
# Count IPC-related syscalls
strace -c -e trace=read,write,recvmsg,sendmsg,mmap,munmap ./your_app

# Profile context switches caused by IPC
perf stat -e context-switches,migrations ./your_app

# Identify IPC hotspots
perf record -g ./your_app && perf report

# BPF: trace all pipe writes > 4KB
sudo bpftrace -e '
    tracepoint:syscalls:sys_enter_write
    /args->count > 4096/ {
        printf("PID %d: write %d bytes to fd %d\n",
               pid, args->count, args->fd);
    }'

# Watch shared memory usage
ipcs -m           # System V shared memory segments
ls /dev/shm/      # POSIX shared memory objects
cat /proc/<pid>/maps | grep shm    # mmap'd shared regions
```

---

## 16. Inter-Host Communication: Socket Programming Patterns

### TCP Echo Server (Complete, production-quality)

```c
/* tcp_echo_server.c
 * Features: SO_REUSEADDR, accept loop, graceful shutdown, error handling
 * Compile: gcc -Wall -O2 -o echo_server tcp_echo_server.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>   /* TCP_NODELAY */
#include <arpa/inet.h>
#include <fcntl.h>

static volatile int running = 1;

void handle_client(int cfd) {
    char buf[4096];
    ssize_t n;
    while ((n = recv(cfd, buf, sizeof(buf), 0)) > 0) {
        ssize_t sent = 0;
        while (sent < n)
            sent += send(cfd, buf + sent, n - sent, MSG_NOSIGNAL);
    }
    close(cfd);
}

int main(int argc, char *argv[]) {
    int port = argc > 1 ? atoi(argv[1]) : 8080;

    /* Create socket */
    int lfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (lfd < 0) { perror("socket"); exit(1); }

    /* Allow both IPv4 and IPv6 */
    int off = 0;
    setsockopt(lfd, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off));

    /* Allow fast restart (avoid TIME_WAIT bind failure) */
    int yes = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));

    /* Disable Nagle (for low-latency applications) */
    setsockopt(lfd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));

    struct sockaddr_in6 addr = {
        .sin6_family = AF_INET6,
        .sin6_port   = htons(port),
        .sin6_addr   = in6addr_any,
    };
    if (bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); exit(1);
    }
    listen(lfd, SOMAXCONN);
    printf("Listening on [::]:%d\n", port);

    while (running) {
        struct sockaddr_in6 client;
        socklen_t len = sizeof(client);
        int cfd = accept(lfd, (struct sockaddr *)&client, &len);
        if (cfd < 0) {
            if (errno == EINTR) continue;
            perror("accept"); break;
        }

        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &client.sin6_addr, ipstr, sizeof(ipstr));
        printf("Connection from %s:%d\n", ipstr, ntohs(client.sin6_port));

        handle_client(cfd);
    }
    close(lfd);
    return 0;
}
```

### epoll-based Event Loop

```c
/* Handles thousands of simultaneous connections with O(1) event detection */
#include <sys/epoll.h>

int epfd = epoll_create1(EPOLL_CLOEXEC);

/* Register listening socket */
struct epoll_event ev = {
    .events  = EPOLLIN,
    .data.fd = lfd
};
epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

/* Event loop */
struct epoll_event events[64];
while (1) {
    int n = epoll_wait(epfd, events, 64, -1);  /* -1 = wait forever */
    for (int i = 0; i < n; i++) {
        int fd = events[i].data.fd;
        if (fd == lfd) {
            /* New connection */
            int cfd = accept4(lfd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
            ev.events  = EPOLLIN | EPOLLET;  /* Edge-triggered! */
            ev.data.fd = cfd;
            epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
        } else {
            /* Data available */
            char buf[4096];
            ssize_t n;
            while ((n = read(fd, buf, sizeof(buf))) > 0)
                write(fd, buf, n);  /* Echo */
            if (n == 0 || (n < 0 && errno != EAGAIN)) {
                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
            }
        }
    }
}
```

**Level-triggered vs. Edge-triggered epoll:**

```
Level-triggered (default, like select/poll):
  epoll_wait returns as long as the fd is readable.
  Safe: works with partial reads.

Edge-triggered (EPOLLET):
  epoll_wait returns ONCE when state changes (empty→data available).
  Requires: drain the fd completely in a loop until EAGAIN.
  Faster: fewer wakeups; used by Nginx, Redis.
```

---

## 17. macOS Mach Messaging and Darwin IPC Internals

### 17.1 Mach Ports and Port Rights Architecture

While Linux relies primarily on pipes, POSIX queues, and Unix domain sockets, the foundation of inter-process communication on macOS (Darwin / XNU) is the **Mach Messaging Subsystem**.
Almost all system services, GUI frameworks, window servers (`WindowServer`), and system daemons (`launchd`) communicate via Mach messages.

The fundamental communication endpoint in Darwin is a **Mach Port** (`mach_port_t`):
- A Mach port is a kernel-protected message queue referenced by an integer capability handle within each task port table.
- Tasks do not hold the port itself; rather, they hold specific **Port Rights**:
  1. **Receive Right (`MACH_PORT_RIGHT_RECEIVE`):** Exactly one task in the entire operating system holds the receive right for a given port. This task owns the message queue and dequeues incoming messages.
  2. **Send Right (`MACH_PORT_RIGHT_SEND`):** One or more tasks hold send rights to transmit messages into the port queue.
  3. **Send-Once Right (`MACH_PORT_RIGHT_SEND_ONCE`):** A transient right granted to allow a recipient to send exactly one reply message back to the sender. Once the reply is sent or the right destroyed, the kernel invalidates the capability.
  4. **Port Set (`MACH_PORT_RIGHT_PORT_SET`):** A collection of receive rights aggregated into a single entity, allowing a server thread to wait on messages across hundreds of distinct ports simultaneously with a single system trap.

### 17.2 The `mach_msg()` System Trap and In-Line vs. Out-of-Line Memory

All Mach communication converges on a single system trap: `mach_msg_trap()`, invoked through the user-space wrapper `mach_msg()`:

```c
/* mach_ipc_demo.c - Mach Message IPC between threads on Darwin */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach/mach.h>
#include <pthread.h>

#define MSG_ID_GREETING 1001

/* Custom Mach message definition */
typedef struct {
    mach_msg_header_t header;
    char text[64];
} TextMessage;

static mach_port_t server_port;

void *server_thread(void *arg) {
    (void)arg;
    TextMessage msg;
    memset(&msg, 0, sizeof(msg));

    printf("[Server] Waiting for Mach message on port %u...\n", server_port);

    /* Block until a message arrives */
    kern_return_t kr = mach_msg(
        &msg.header,
        MACH_RCV_MSG,
        0,
        sizeof(msg),
        server_port,
        MACH_MSG_TIMEOUT_NONE,
        MACH_PORT_NULL
    );

    if (kr == MACH_MSG_SUCCESS) {
        printf("[Server] Received message ID %d: '%s'\n", msg.header.msgh_id, msg.text);
    } else {
        fprintf(stderr, "[Server] mach_msg receive failed: %d\n", kr);
    }

    return NULL;
}

int main(void) {
    mach_port_t task = mach_task_self();

    /* 1. Allocate a port with a receive right */
    kern_return_t kr = mach_port_allocate(task, MACH_PORT_RIGHT_RECEIVE, &server_port);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to allocate Mach port: %d\n", kr);
        return 1;
    }

    /* 2. Insert a send right for the allocated port */
    kr = mach_port_insert_right(task, server_port, server_port, MACH_MSG_TYPE_MAKE_SEND);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to insert send right: %d\n", kr);
        return 1;
    }

    /* 3. Launch server listener thread */
    pthread_t thread;
    pthread_create(&thread, NULL, server_thread, NULL);

    /* Let the server thread enter receive state */
    usleep(100000);

    /* 4. Construct and send message */
    TextMessage send_msg;
    memset(&send_msg, 0, sizeof(send_msg));
    send_msg.header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
    send_msg.header.msgh_size = sizeof(send_msg);
    send_msg.header.msgh_remote_port = server_port;
    send_msg.header.msgh_local_port = MACH_PORT_NULL;
    send_msg.header.msgh_id = MSG_ID_GREETING;
    strncpy(send_msg.text, "Hello from Mach messaging on macOS!", sizeof(send_msg.text) - 1);

    printf("[Client] Sending message to server port %u...\n", server_port);
    kr = mach_msg(
        &send_msg.header,
        MACH_SEND_MSG,
        sizeof(send_msg),
        0,
        MACH_PORT_NULL,
        MACH_MSG_TIMEOUT_NONE,
        MACH_PORT_NULL
    );

    if (kr != MACH_MSG_SUCCESS) {
        fprintf(stderr, "mach_msg send failed: %d\n", kr);
    }

    pthread_join(thread, NULL);

    /* Clean up Mach port */
    mach_port_destroy(task, server_port);
    printf("Mach IPC demo completed successfully\n");

    return 0;
}
```

```bash
# Compile and run Mach IPC demo on Darwin
clang -Wall -Wextra mach_ipc_demo.c -o mach_ipc_demo
./mach_ipc_demo
```

### 17.3 Out-of-Line (OOL) Data and Virtual Memory Remapping

For payloads larger than a single page (16 KB on Apple Silicon, 4 KB on Intel), copying bytes through the kernel message buffer degrades cache locality and memory throughput.
Mach provides **Out-of-Line (OOL) descriptors** (`mach_msg_ool_descriptor_t`):
- The sender specifies a pointer and length in its address space.
- The kernel handles the transfer by updating virtual memory mappings.
- The recipient receives a newly allocated virtual address range backed by the same physical pages with **Copy-On-Write (COW)** protection.
- If neither sender nor receiver modifies the buffer, zero physical copies occur, allowing gigabytes of IPC data to be transferred in constant time $O(1)$.

### 17.4 File Descriptor Passing via Unix Domain Sockets on Darwin

In addition to Mach ports, Darwin supports standard BSD Unix domain sockets (`AF_UNIX`).
Using ancillary control messages (`struct cmsghdr`) with type `SCM_RIGHTS`, one process can duplicate an open file descriptor into another process's file descriptor table:

```c
/* Passing an open file descriptor over a UNIX domain socket */
#include <sys/socket.h>
#include <sys/uio.h>

int send_fd(int socket, int fd_to_send) {
    struct msghdr msg = {0};
    char buf[CMSG_SPACE(sizeof(int))];
    memset(buf, 0, sizeof(buf));

    struct iovec io = { .iov_base = "FD", .iov_len = 2 };
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    *((int *)CMSG_DATA(cmsg)) = fd_to_send;

    return sendmsg(socket, &msg, 0);
}
```

### 17.5 macOS XPC Services and IPC Diagnostics

Apple layered **XPC Services** (`libxpc`) above Mach messaging to provide a modern, secure RPC and serialization mechanism:
- **Sandbox integration:** Applications split privileged operations into unprivileged child XPC services managed directly by `launchd`.
- **Structured messages:** XPC passes dictionaries (`xpc_object_t`) supporting integers, strings, arrays, UUIDs, shared memory regions (`xpc_shmem`), and file descriptors.

```bash
# Inspect registered XPC services on Darwin
launchctl list | head -25

# Trace Mach messaging activity in real time on macOS
sudo sc_usage -c | grep -i mach

# Trace specific process Mach traps using DTrace
sudo dtruss -f -t mach_msg_trap -p <pid>
```

---

**Previous:** [P3L2: Memory Management](P3L2-Memory-Management.md)
**Next:** [P3L4: Synchronization Constructs](P3L4-Synchronization-Constructs.md)


