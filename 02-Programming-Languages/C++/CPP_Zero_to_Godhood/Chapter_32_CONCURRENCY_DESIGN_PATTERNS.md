# CONCURRENCY DESIGN PATTERNS


### 10.6.1 Active Object Pattern
Decouples method execution from invocation. The object owns a thread and a message queue.

```cpp
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class ActiveObject {
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    std::thread worker;
    bool done = false;

public:
    ActiveObject() {
        worker = std::thread([this] { run(); });
    }

    ~ActiveObject() {
        { std::lock_guard lock(mtx); done = true; }
        cv.notify_one();
        worker.join();
    }

    void invoke(std::function<void()> task) {
        std::lock_guard lock(mtx);
        tasks.push(std::move(task));
        cv.notify_one();
    }

private:
    void run() {
        while (true) {
            std::unique_lock lock(mtx);
            cv.wait(lock, [this] { return !tasks.empty() || done; });
            
            if (done && tasks.empty()) return;
            
            auto task = std::move(tasks.front());
            tasks.pop();
            lock.unlock();
            
            task(); // Execute
        }
    }
};
```

### 10.6.2 Monitor Object (Thread-Safe Interface)
Ensure thread safety by locking in public methods and calling private implementation methods.

```cpp
class Monitor {
    mutable std::mutex mtx;
    int state = 0;

public:
    void update(int val) {
        std::lock_guard lock(mtx); // Lock here
        update_impl(val);
    }

private:
    // Expects lock to be held
    void update_impl(int val) {
        state = val;
    }
};
```

---
