# C++11 CONCURRENCY


Before C++11, multithreading was platform-specific (pthreads, Windows threads). C++11 added a standard memory model and threading library.

---

## 1. THREADS

`std::thread` represents a single thread of execution.

```cpp
#include <thread>
#include <iostream>

void task(int id) {
    std::cout << "Thread " << id << " running\n";
}

int main() {
    std::thread t1(task, 1);
    std::thread t2(task, 2);

    // Must join or detach before destructor
    t1.join(); // Wait for finish
    t2.join();
    return 0;
}
```

---

## 2. MUTEX AND LOCKS

Protect shared data with `std::mutex`.

```cpp
#include <mutex>

std::mutex mtx;
int count = 0;

void safe_increment() {
    // RAII Lock: locks on construction, unlocks on destruction
    std::lock_guard<std::mutex> lock(mtx);
    count++;
}
```

---

## 3. ATOMICS

`std::atomic<T>` provides lock-free thread safety for simple types.

```cpp
#include <atomic>

std::atomic<int> counter(0);

void fast_increment() {
    counter++; // Atomic increment (hardware supported)
}
```

This avoids the overhead of mutexes for simple counters and flags.

---

## 4. ASYNC AND FUTURE

`std::async` runs a function asynchronously and returns a `std::future` that holds the result.

```cpp
#include <future>

int calculate() { return 42; }

int main() {
    // Launch async task
    std::future<int> result = std::async(std::launch::async, calculate);
    
    // Do other work...    
    // Get result (blocks if not ready)
    std::cout << result.get(); 
}
```

---

## 5. CONDITION VARIABLES

Used for thread synchronization (waiting for an event).

```cpp
std::condition_variable cv;
std::mutex cv_m;
bool ready = false;

void worker() {
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk, []{ return ready; }); // Wait until ready is true
    // process...
}

void signal() {
    {
        std::lock_guard<std::mutex> lk(cv_m);
        ready = true;
    }
    cv.notify_one();
}
```

