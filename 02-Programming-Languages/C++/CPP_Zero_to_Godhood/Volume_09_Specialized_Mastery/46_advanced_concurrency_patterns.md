# Chapter 46: Advanced Concurrency Patterns

In high-performance systems engineering, standard concurrent primitives like raw threads and coarse-grained mutexes are often too slow, resource-heavy, or error-prone. This chapter details advanced design patterns used to achieve maximum throughput, low latency, and structured concurrency in modern C++ (C++20/C++23).

***

## 46.1 Production-Grade Thread Pool

Spawning a thread involves expensive operating system syscalls, stack memory allocation (typically 8MB on Linux), and kernel context-switching overhead. A thread pool mitigates this by maintaining a fixed set of reusable worker threads that process tasks from a concurrent queue.

Below is a modern, thread-safe, compile-ready implementation of a C++20 thread pool that supports arbitrary callables, perfect forwarding, and return value retrieval via `std::future`.

```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <memory>
#include <type_traits>

class ThreadPool {
public:
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type>;
        
    ~ThreadPool();

private:
    // Need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // The task queue
    std::queue<std::function<void()>> tasks;
    
    // Synchronization
    std::mutex queue_mutex;
    std::condition_variable cv;
    bool stop = false;
};

// Constructor launches worker threads
inline ThreadPool::ThreadPool(size_t threads) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->cv.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    });
                    
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task(); // Execute the task outside the lock
            }
        });
    }
}

// Add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::invoke_result<F, Args...>::type> 
{
    using return_type = typename std::invoke_result<F, Args...>::type;

    // Use packaged_task to wrap our callable and capture its return value
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // Don't allow enqueueing after stopping the pool
        if (stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        tasks.emplace([task]() { (*task)(); });
    }
    cv.notify_one();
    return res;
}

// Destructor joins all threads
inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    cv.notify_all();
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}
```

### 🔍 Architectural Highlights:

1. **Perfect Forwarding & Binding:** The `enqueue` function uses `std::forward` and `std::bind` to capture any callable object along with its arguments without unnecessary copying.
2. **`std::packaged_task`:** This wraps the callable so its execution writes to a shared state, which is read asynchronously by the caller via a `std::future`.
3. **RAII-Compliant Destructor:** The pool guarantees that all pending tasks are finished and all worker threads are joined on destruction, preventing deadlocks or abandoned threads.

***

## 46.2 The Actor Model (Active Objects)

The Actor Model is a design paradigm that eliminates shared state synchronization issues by banning raw memory sharing. Instead, the system is modeled as isolated entities called **Actors**.

### Core Tenets:

1. **Isolation:** An actor contains private data that cannot be accessed or mutated directly by other actors.
2. **Asynchronous Messages:** Actors communicate solely by passing immutable messages to each other's mailboxes.
3. **Single-Threaded Execution:** Each actor processes messages from its mailbox sequentially. This completely eliminates the need for internal locking.

```cpp
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

class Message {
public:
    enum class Type { PrintPayload, Shutdown };
    Type type;
    std::string payload;
};

class Actor {
private:
    std::queue<Message> mailbox;
    std::mutex mtx;
    std::condition_variable cv;
    std::thread worker;
    bool done = false;

    void process_messages() {
        while (true) {
            Message msg;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this] { return !mailbox.empty(); });
                msg = std::move(mailbox.front());
                mailbox.pop();
            }

            if (msg.type == Message::Type::Shutdown) {
                return;
            }
            
            // Handle message logic
            std::cout << "Actor " << std::this_thread::get_id() 
                      << " processing: " << msg.payload << "\n";
        }
    }

public:
    Actor() {
        worker = std::thread(&Actor::process_messages, this);
    }

    ~Actor() {
        send({Message::Type::Shutdown, ""});
        if (worker.joinable()) {
            worker.join();
        }
    }

    void send(Message msg) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            mailbox.push(std::move(msg));
        }
        cv.notify_one();
    }
};
```

> [!TIP]
> In production-grade C++ codebases, avoid rolling your own Actor framework. Use mature, lock-free implementations like the **CAF (C++ Actor Framework)**, which handles distributed networking and scheduling dynamically.

***

## 46.3 The Disruptor Pattern (LMAX Architecture)

Originally designed by LMAX, the Disruptor is an extremely high-throughput, low-latency inter-thread messaging library. It is widely used in high-frequency trading (HFT) systems to replace standard producer-consumer queues.

### Why standard queues are slow in HFT:

1. **Dynamic Memory Allocation:** Pushing to a standard queue allocates heap nodes, triggering allocator locks and memory latency.
2. **Lock Contention:** Mutex locks force OS kernel context switches.
3. **False Sharing:** The head and tail pointers of a queue often sit on the same cache line, causing cache-line bouncing between producer and consumer cores.

### Disruptor Solutions:

1. **Pre-allocated Ring Buffer:** A circular array of pre-allocated data structures. No allocations happen during runtime.
2. **Cache-Aligned Sequences:** Producer and consumer indices are aligned to cache lines (`alignas(64)`) to completely avoid false sharing.
3. **Lock-Free Busy-Spinning:** Uses atomic CAS operations and memory fences instead of OS mutexes.

```cpp
#include <atomic>
#include <vector>
#include <new>

template<typename T, size_t Size>
class DisruptorRingBuffer {
    static_assert((Size & (Size - 1)) == 0, "Disruptor size must be a power of 2");

private:
    struct Slot {
        T data;
    };

    // Pre-allocated array
    std::vector<Slot> ring_buffer;
    
    // Separate cache lines to prevent false sharing
    alignas(64) std::atomic<uint64_t> write_sequence{0};
    alignas(64) std::atomic<uint64_t> read_sequence{0};

public:
    DisruptorRingBuffer() : ring_buffer(Size) {}

    // Lock-Free Write
    template<typename F>
    void publish(F&& populate_func) {
        uint64_t current_write = write_sequence.load(std::memory_order_relaxed);
        
        // Busy spin if buffer is full
        while (current_write - read_sequence.load(std::memory_order_acquire) >= Size) {
            #if defined(__x86_64__)

            __asm__ __volatile__("pause" ::: "memory"); // Emit PAUSE to save power and improve spin loops
            #endif

        }

        // Write directly to pre-allocated slot
        populate_func(ring_buffer[current_write & (Size - 1)].data);

        // Advance sequence and publish to consumers
        write_sequence.store(current_write + 1, std::memory_order_release);
    }

    // Lock-Free Read
    bool consume(T& out_data) {
        uint64_t current_read = read_sequence.load(std::memory_order_relaxed);
        
        // Spin/wait if there is no new data
        if (current_read >= write_sequence.load(std::memory_order_acquire)) {
            return false; // Non-blocking read attempt
        }

        out_data = ring_buffer[current_read & (Size - 1)].data;
        read_sequence.store(current_read + 1, std::memory_order_release);
        return true;
    }
};
```

***

## 46.4 Coroutines for Concurrency

C++20 introduced stackless coroutines, allowing functions to suspend execution and resume later without blocking their OS worker threads. This enables writing highly concurrent asynchronous code (e.g., handling 100k network connections on a single thread) while maintaining clean, sequential code formatting.

### Under the Hood: The State Machine

When a compiler compiles a coroutine containing `co_await`, it transforms the function into a heap-allocated state machine object. The local variables are moved to this **Coroutine Frame**, and the function body is split into resume points.

```cpp
#include <coroutine>
#include <iostream>
#include <future>

// A custom task type that models execution suspension
struct Task {
    struct promise_type {
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        void return_void() {}
    };

    std::coroutine_handle<promise_type> handle;
    
    Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Task() { if (handle) handle.destroy(); }
    
    void resume() {
        if (handle && !handle.done()) {
            handle.resume();
        }
    }
};

// A simple awaitable utility
struct YieldAwaiter {
    bool await_ready() noexcept { return false; } // Forces suspension
    void await_suspend(std::coroutine_handle<>) noexcept {
        std::cout << "  (Coroutine suspended and returned control to caller)\n";
    }
    void await_resume() noexcept {}
};

Task run_async_flow() {
    std::cout << "Step 1: Starting async flow\n";
    co_await YieldAwaiter{};
    std::cout << "Step 2: Resumed async flow\n";
}

int main() {
    Task t = run_async_flow();
    std::cout << "Main: Resuming coroutine\n";
    t.resume();
    std::cout << "Main: Resuming coroutine again\n";
    t.resume();
    return 0;
}
```

### Output:

```text
Main: Resuming coroutine
Step 1: Starting async flow
  (Coroutine suspended and returned control to caller)
Main: Resuming coroutine again
Step 2: Resumed async flow
```

***

## 46.5 False Sharing and Cache Alignment

The CPU fetches memory in **64-byte Cache Lines**. When two cores write to different variables that sit on the same cache line, they force hardware cache coherency protocols (MESI) to constantly invalidate each other's L1/L2 caches. This is **False Sharing**, which can degrade multi-threaded performance by orders of magnitude.

```cpp
#include <new>
#include <atomic>

// BAD: Causes cache-line bouncing (False Sharing)
struct BadLayout {
    std::atomic<uint64_t> counterA; // 8 bytes
    std::atomic<uint64_t> counterB; // 8 bytes
    // Total size is 16 bytes. They sit on the same 64-byte cache line.
};

// GOOD: Eliminated False Sharing
struct GoodLayout {
    alignas(64) std::atomic<uint64_t> counterA; // Cache line 1
    alignas(64) std::atomic<uint64_t> counterB; // Cache line 2
};
```

> [!IMPORTANT]
> Since C++17, you should use `std::hardware_destructive_interference_size` (defined in `<new>`) instead of hardcoding `64`, as some modern server CPUs (like Intel Xeon or Apple M-series) have 128-byte or 256-byte cache line boundaries.
