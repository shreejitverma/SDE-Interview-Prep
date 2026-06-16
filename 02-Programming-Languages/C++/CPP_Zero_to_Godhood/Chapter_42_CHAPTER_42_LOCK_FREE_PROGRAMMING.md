# CHAPTER 42: LOCK FREE PROGRAMMING


# LOCK-FREE PROGRAMMING

Welcome to the most dangerous and rewarding part of C++. Lock-free programming is like performing open-heart surgery while the patient is running a marathon.

### The Atomic Coffee Shop Analogy

Imagine a busy coffee shop with many customers (threads) and one barista (the data).

1.  **Mutex (The Locked Door)**: To talk to the barista, you have to lock the front door of the shop. No one else can even enter until you are done. This is safe, but if you take 10 minutes to order, theres a giant line outside.
2.  **Lock-Free (The Ticket System)**: Everyone is in the shop at once. The barista has a "Current Ticket" number. You look at your ticket, and if it matches the current number, you swap it for your coffee in one instant motion. If someone else gets there first, your ticket is "out of date," and you have to go to the back of the line and try again.

#### Why do we care?
In high-frequency trading (HFT), waiting for a Mutex is like waiting for a slow elevator. Lock-free code is like a high-speed conveyor belt.

---

### The "Voucher Exchange" (Compare-And-Swap)

The heart of lock-free is **CAS (Compare-And-Swap)**. Think of it as an "Honest Exchange":

1.  You show the Barista a photo of the counter as it looked 10 seconds ago (**Old Value**).
2.  You say: "If the counter still looks exactly like this photo, put this coffee on it (**New Value**)."
3.  The Barista looks. If it matches, the swap happens instantly. If it *doesn't* match (someone else moved a cup), the Barista says "Transaction Denied," and hands you a *new* photo of the counter.

---

### The ABA Problem: The Water Cooler Analogy

The biggest trap in lock-free is the **ABA Problem**.

Imagine you see a full bottle on the water cooler (Value A). You leave to get a cup.
While you are gone:
1.  Friend 1 drinks all the water (Value B).
2.  Friend 2 refills the bottle with swamp water (Value A again).

You come back, see the bottle is "full" (Value A), and drink it. You think nothing changed, but everything changed! 

> **Godhood Tip**: To solve this, we use "Tagged Pointers" or "Hazard Pointers" to track not just the value, but *how many times* it has changed.

---

## 1. The Concept

Programming without Mutexes. Guarantees system-wide progress.

*   **Lock-Free:** At least one thread always makes progress.
*   **Wait-Free:** Every thread makes progress in finite steps.

## 2. Compare-And-Swap (CAS)

The primitive of lock-free. `compare_exchange_weak` vs `compare_exchange_strong`.

```cpp
std::atomic<int> head;

void push(int new_val) {
    int old_head = head.load();
    // Loop until we successfully swap head with new_val
    while (!head.compare_exchange_weak(old_head, new_val)) {
        // old_head is updated to current head value automatically
    }
}
```

## 3. The ABA Problem

1.  Thread 1 reads A.
2.  Thread 2 changes A to B, then back to A.
3.  Thread 1 CAS(A, new) succeeds, thinking nothing changed.

**Solutions:**
*   **Versioned Pointers:** Store `{ptr, count}`. `std::atomic<uint128_t>` (if supported).
*   **Hazard Pointers:** Protect pointers currently being read.
*   **RCU (Read-Copy-Update):** Wait for all readers to finish before reclaiming memory.

## 4. Lock-Free Data Structures

*   **Lock-Free Stack:** Easy (CAS on head).
*   **Lock-Free Queue:** Harder (Head and Tail). Use Michael-Scott Queue algorithm.
*   **Lock-Free Hash Map:** Very hard (Split-Ordered Lists).
