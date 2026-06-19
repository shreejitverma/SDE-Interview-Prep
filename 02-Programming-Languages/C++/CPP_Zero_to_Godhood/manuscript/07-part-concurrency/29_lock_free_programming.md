# Chapter 29: Lock-Free Programming

> *Programming without mutexes — the ultimate performance unlock.*

If you use a `std::mutex`, you are at the mercy of the Operating System's scheduler. If a thread acquires a lock and is then immediately preempted (put to sleep by the OS to let another program run), every other thread waiting for that lock is now blocked. This is a disaster in high-performance or real-time systems.

**Lock-Free Programming** is a set of techniques that guarantee that *some* thread is always making progress, regardless of what the OS scheduler is doing.

---

## 29.1 What "Lock-Free" Actually Means

"Lock-Free" does **not** simply mean "I didn't use a `std::mutex`." It is a mathematical guarantee about system-wide progress.

1.  **Wait-Free**: Every thread guarantees it will complete its operation in a bounded number of steps. (The absolute holy grail, but extremely difficult to achieve).
2.  **Lock-Free**: At least *one* thread is guaranteed to complete its operation in a bounded number of steps. (If 10 threads clash, 9 might have to retry, but 1 definitely succeeds).
3.  **Obstruction-Free**: A thread guarantees it will complete its operation *only if* all other threads are paused.

If you write a spinlock (`while(atomic_flag.test_and_set());`), you are **not** lock-free. If the thread holding the "lock" is preempted, the spinning threads will loop forever, burning CPU, making no progress.

## 29.2 The CAS Loop

The engine of almost all lock-free programming is the **Compare-And-Swap (CAS)** loop, achieved via `compare_exchange_weak`.

The logic goes like this:
1.  Read the shared atomic variable into `expected`.
2.  Calculate the `desired` new value based on `expected`.
3.  Attempt the CAS. If the shared variable still equals `expected`, it atomically changes it to `desired` and returns `true`.
4.  If another thread snuck in and changed the shared variable, CAS returns `false` and updates `expected` with the new actual value.
5.  Loop back to step 2 and try again.

```cpp
std::atomic<int> shared_data{0};

void lock_free_multiply(int factor) {
    int expected = shared_data.load();
    int desired;
    do {
        desired = expected * factor;
        // If shared_data == expected, shared_data = desired. Return true.
        // Else, expected = shared_data. Return false.
    } while (!shared_data.compare_exchange_weak(expected, desired));
}
```
*Why `weak` instead of `strong`? On some architectures (like ARM), CAS can fail "spuriously" even if the value hasn't changed. Inside a loop, `weak` is faster. `strong` is better if you aren't looping.*

## 29.3 The Lock-Free Stack

Let's build a simple Lock-Free Stack. A stack operates on the `head` pointer.

```cpp
template<typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
        Node(const T& data) : data(data), next(nullptr) {}
    };
    std::atomic<Node*> head{nullptr};

public:
    void push(const T& data) {
        Node* new_node = new Node(data);
        
        // 1. Read current head
        new_node->next = head.load();
        
        // 2. Try to replace head with new_node
        // If head has changed since step 1, new_node->next is updated, and we try again.
        while (!head.compare_exchange_weak(new_node->next, new_node));
    }
};
```
This `push` is completely lock-free. Even if 100 threads try to push at once, one will always succeed on the first try.

## 29.4 The Dreaded ABA Problem

Let's look at `pop()`. You read `head` (let's call it Node A). You read `head->next` (Node B). You CAS `head` from A to B.

But what if, right after you read A and B, your thread gets put to sleep?
While you sleep:
1. Another thread pops A.
2. Another thread pops B.
3. Another thread pushes A back onto the stack!

You wake up. Your CAS asks: "Is `head` still A?" The answer is YES. So you update `head` to B.
**But B was deleted!** Your stack is now corrupted.

This is the **ABA Problem**. The variable changed from A to B, and back to A. Your code thought nothing changed.

## 29.5 Memory Reclamation Strategies

In a garbage-collected language (like Java or C#), the ABA problem is mitigated because Node B wouldn't be deleted while a thread still had a reference to it. In C++, we manage our own memory, making lock-free data structure deletion extremely dangerous.

How do we safely `delete` popped nodes?

### 1. Hazard Pointers (Coming to C++26)
A Hazard Pointer is a way for a thread to announce: *"I am currently looking at this memory address. Do not delete it!"*
When a thread pops a node, it checks the global list of Hazard Pointers. If anyone is looking at the node, it pushes the node to a "to-be-deleted-later" list. If no one is looking, it deletes it safely.

C++26 is introducing `std::hazard_pointer` natively.

### 2. Read-Copy-Update (RCU) (Coming to C++26)
Used extensively in the Linux Kernel. RCU allows incredibly fast reads with zero overhead. When writing, the writer creates a completely new copy of the data structure, updates the global pointer, and then waits for a "grace period" (until all current readers finish) before deleting the old copy.

C++26 is introducing `std::rcu`.

## 29.6 Lock-Free Queues

A lock-free Queue is much harder than a Stack because it has two ends (`head` and `tail`). Updating both atomically is nearly impossible with standard CAS.

The most famous algorithm is the **Michael-Scott Queue**. It relies on the tail pointer sometimes "lagging behind" the actual end of the queue, requiring other threads to "help" push the tail forward before they can do their own work. 

If you just need a pipeline between exactly two threads, you can use an **SPSC (Single-Producer, Single-Consumer)** Ring Buffer. This is incredibly fast and avoids the ABA problem entirely because only one thread ever touches the read index, and one thread touches the write index.

## 29.7 Priority Inversion

Why go through all this effort? Why not just use a Mutex?

In 1997, the Mars Pathfinder rover started randomly resetting on Mars. The cause was **Priority Inversion**.
1. A Low-Priority task grabbed a mutex to write to a data bus.
2. The OS preempted it to run a Medium-Priority long-running task.
3. A High-Priority task (the vital system watchdog) woke up and tried to grab the data bus mutex. It couldn't. It had to wait for the Low-Priority task.
4. But the Low-Priority task couldn't run because the Medium-Priority task was hogging the CPU!

The High-Priority task missed its deadline, and the rover crashed.

Lock-Free programming completely eliminates Priority Inversion, because there are no locks to hold. This is why it is mandatory in Real-Time Operating Systems (RTOS), aerospace, and high-frequency trading.

---

> [!WARNING]
> **Godhood Warning: Don't write it yourself.**
> Writing a bug-free MPMC (Multi-Producer Multi-Consumer) Lock-Free Queue is the subject of PhD theses. Unless you are doing it for educational purposes, do not write your own. Use proven, battle-tested libraries like `boost::lockfree` or Cameron Desrochers' `moodycamel::ConcurrentQueue`.

We have now covered the highest-performance bare-metal techniques in C++. Next, we will look at how to achieve massive data-parallelism using **OpenMP**.
