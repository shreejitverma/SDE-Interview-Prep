# Part VII: Concurrency and Parallelism

*Threading, atomics, lock-free programming, and the memory model.*

# Chapter 27: Threads and Synchronization

> *The kitchen analogy — multiple chefs, one knife.*

For the first 30 years of its existence, C++ had no concept of threads. Developers relied on OS-specific APIs like POSIX Threads (pthreads) on Linux or the Windows API. 

C++11 finally introduced a standardized multithreading library, allowing us to write cross-platform concurrent code. Over the next decade, C++14, 17, and 20 polished and expanded this library into an industrial-grade concurrency suite.

Think of a computer program like a kitchen. A single-threaded program has one chef. They chop the onions, *then* boil the water, *then* cook the pasta. Multithreading allows you to hire multiple chefs. But if two chefs try to grab the same knife at the exact same time, disaster strikes.

---

## 27.1 `std::thread` and the Lifecycle

To hire a new chef, you create a `std::thread` and pass it a function (or a lambda) to execute.

```cpp
#include <thread>
#include <iostream>

void boil_water() { std::cout << "Boiling water...\n"; }

int main() {
    std::thread chef2(boil_water); // Starts immediately!
    
    std::cout << "Chopping onions...\n";
    
    // We MUST wait for chef2 to finish before the kitchen closes.
    chef2.join(); 
}
```

### The Join/Detach Rule
Every `std::thread` object has a strict rule: before it is destroyed (goes out of scope), you **must** call either `.join()` or `.detach()`.
*   `join()`: Blocks the current thread until the child thread finishes.
*   `detach()`: Cuts the cord. The child thread runs freely in the background.

If a `std::thread` destructor runs and you haven't called either, the entire program instantly crashes via `std::terminate`.

### The C++20 Fix: `std::jthread`
Because forgetting to call `.join()` caused thousands of crashes worldwide, C++20 introduced `std::jthread` (Joining Thread). It automatically calls `.join()` in its destructor. Always use `std::jthread` if you have C++20.

## 27.2 Mutexes: Guarding the Knife

If two threads try to modify the same variable at the same time, you create a **Data Race** (Undefined Behavior). To prevent this, you use a `std::mutex` (Mutual Exclusion).

```cpp
#include <mutex>

int counter = 0;
std::mutex mtx;

void increment() {
    mtx.lock();   // Grab the knife
    counter++;    // Use the knife safely
    mtx.unlock(); // Put the knife back
}
```

### The RAII Guards (Never call `.lock()` manually)
What if `counter++` threw an exception? `mtx.unlock()` would never be called, the mutex would remain locked forever, and your program would freeze (Deadlock).

Instead, we use RAII guards.
*   **`std::lock_guard`**: Locks on creation, unlocks on destruction.
*   **`std::unique_lock`**: Like `lock_guard`, but allows manual locking/unlocking and moving.
*   **`std::scoped_lock` [C++17]**: Can lock *multiple* mutexes at the same time without deadlocking. (Replaces `lock_guard`).

```cpp
void safe_increment() {
    std::scoped_lock lock(mtx); // Locks safely!
    counter++;
} // Automatically unlocks here, even if an exception is thrown
```

### 27.3 Reader-Writer Locks [C++17]
Sometimes, 100 threads just want to *read* a variable, but only 1 thread wants to *write* to it. A standard mutex blocks everyone. C++17 introduced `std::shared_mutex`.

```cpp
#include <shared_mutex>

std::shared_mutex rw_mtx;
int data = 0;

void reader() {
    // Multiple threads can hold a shared_lock simultaneously
    std::shared_lock lock(rw_mtx); 
    std::cout << data;
}

void writer() {
    // Only ONE thread can hold a unique_lock. Blocks all readers!
    std::unique_lock lock(rw_mtx);
    data = 42;
}
```

## 27.4 Condition Variables: Waiting for the Bell

If Chef A is waiting for Chef B to finish boiling the water, Chef A shouldn't stand there checking the water every millisecond (a "spin lock" or "busy wait", which burns CPU). Chef A should go to sleep, and Chef B should ring a bell when it's done.

We achieve this with `std::condition_variable`.

```cpp
#include <condition_variable>

std::condition_variable cv;
std::mutex cv_m;
bool water_boiled = false;

// Chef A
void wait_for_water() {
    std::unique_lock lock(cv_m);
    // Go to sleep until water_boiled is true
    cv.wait(lock, [] { return water_boiled; }); 
    std::cout << "Finally, I can cook pasta!\n";
}

// Chef B
void boil_water() {
    {
        std::scoped_lock lock(cv_m);
        water_boiled = true;
    }
    cv.notify_one(); // Ring the bell! Wakes up Chef A
}
```
*Note: Always pass a lambda check to `.wait()`. Operating systems can sometimes wake up threads randomly (Spurious Wakeups). The lambda ensures the thread goes right back to sleep if the condition isn't actually true.*

## 27.5 `std::async` and Futures

Manually creating threads and mutexes is exhausting just to run a simple background math function. `std::async` allows you to launch a task and get a "ticket" (`std::future`) to retrieve the result later.

```cpp
#include <future>

int heavy_math() { return 42; }

int main() {
    // Launch on a background thread
    std::future<int> ticket = std::async(std::launch::async, heavy_math);
    
    std::cout << "Doing other work...\n";
    
    // Blocks until the math is done, then gets the result
    int result = ticket.get(); 
}
```

## 27.6 C++20 Synchronization Primitives

C++20 added specialized tools to replace messy Condition Variable setups:

*   **`std::latch`**: A single-use countdown. (e.g., Wait for 4 threads to finish initializing before the main loop starts).
*   **`std::barrier`**: Like a latch, but reusable in phases.
*   **`std::counting_semaphore`**: A tollbooth that only lets `N` threads through at a time. Perfect for limiting access to a database connection pool.

## 27.7 Cooperative Cancellation (`std::stop_token`)

Before C++20, if you wanted to tell an infinite-looping background thread to stop, you had to build a custom atomic boolean flag. C++20 built this into `std::jthread` via `std::stop_token`.

```cpp
void worker(std::stop_token stoken) {
    while (!stoken.stop_requested()) {
        // Do work...
    }
    std::cout << "Stopping gracefully!\n";
}

int main() {
    std::jthread t(worker);
    // ...
    t.request_stop(); // Politely asks the thread to stop
}
```

## 27.8 The Ultimate Solution: Thread Pools

Spawning a new `std::thread` every time you need to do work is wildly inefficient. The OS takes time to allocate the thread stack and register it.

In professional C++, you build or use a **Thread Pool**. A pool spins up $N$ threads at application startup (usually equal to your CPU core count). These threads sit in an infinite loop, sleeping on a condition variable. When you push a task (a lambda) into a queue, a thread wakes up, grabs the task, executes it, and goes back to sleep.

*(A full Thread Pool implementation utilizing `std::packaged_task`, Mutexes, and Condition Variables is a classic C++ interview question, showcasing mastery of everything covered in this chapter).*

---

Mutexes are safe, but they are incredibly slow. When a thread is blocked by a mutex, the OS puts it to sleep and switches context. For high-frequency trading engines or real-time audio processors, even a 1-millisecond mutex sleep is unacceptable. 

To achieve ultimate speed, we must bypass the OS entirely and talk directly to the CPU's caching system. We must enter the complex, dangerous world of **The Memory Model and Atomics**.
