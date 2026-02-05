# CONCURRENCY & MULTITHREADING

C++11 brought a standard threading model.

## 1. Threads (`std::thread`)

```cpp
#include <thread>

void task() { /*...*/ }

int main() {
    std::thread t(task);
    t.join(); // Wait for finish
    // t.detach(); // Or let it run freely
}
```

## 2. Mutexes & Locking

Avoid data races.

*   `std::mutex`: Basic lock.
*   `std::lock_guard`: RAII wrapper (locks on construction, unlocks on destruction).
*   `std::unique_lock`: Flexible RAII wrapper (can unlock manually).

```cpp
std::mutex mtx;
void safe() {
    std::lock_guard<std::mutex> lock(mtx);
    // critical section
}
```

## 3. Condition Variables

Wait for a condition to be true.

```cpp
std::condition_variable cv;
std::mutex mtx;
bool ready = false;

// Waiter
std::unique_lock<std::mutex> lk(mtx);
cv.wait(lk, []{ return ready; });

// Notifier
{
    std::lock_guard<std::mutex> lk(mtx);
    ready = true;
}
cv.notify_one();
```

## 4. Futures & Promises

Asynchronous result retrieval.

*   `std::async`: Runs a function asynchronously.
*   `std::future`: Holds the result.

```cpp
auto f = std::async(std::launch::async, []{ return 42; });
int result = f.get(); // Blocks until ready
```

## 5. Atomics (`std::atomic`)

Lock-free operations for basic types.

```cpp
std::atomic<int> counter(0);
counter++; // Thread-safe increment
```
