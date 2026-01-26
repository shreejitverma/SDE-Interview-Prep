/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Lock-Free Stack (Treiber Stack)
 * Description: A thread-safe stack implementation using std::atomic and CAS (Compare-And-Swap).
 *           This avoids the overhead of std::mutex and prevents deadlocks.
 *           Used in: High-performance memory allocators, job schedulers.
 */

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

template <typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
        Node(const T& data) : data(data), next(nullptr) {}
    };

    std::atomic<Node*> head;

public:
    LockFreeStack() : head(nullptr) {}

    void push(const T& data) {
        Node* new_node = new Node(data);
        
        // 1. Prepare the new node
        new_node->next = head.load(std::memory_order_relaxed);

        // 2. CAS Loop (Compare-And-Swap)
        // If 'head' is still what we read (new_node->next), replace it with 'new_node'.
        // If 'head' changed (another thread pushed), update new_node->next and retry.
        while (!head.compare_exchange_weak(new_node->next, new_node, 
                                           std::memory_order_release, 
                                           std::memory_order_relaxed));
    }

    bool pop(T& result) {
        Node* old_head = head.load(std::memory_order_relaxed);

        // CAS Loop
        while (old_head && 
               !head.compare_exchange_weak(old_head, old_head->next, 
                                           std::memory_order_acquire, 
                                           std::memory_order_relaxed));
        
        if (old_head == nullptr) return false; // Empty stack

        result = old_head->data;
        // Note: In a real system, we need "Hazard Pointers" or RCU here to safely delete old_head.
        // delete old_head; 
        return true;
    }
};

void worker(LockFreeStack<int>& stack, int id) {
    for (int i = 0; i < 1000; ++i) {
        stack.push(id * 1000 + i);
        int val;
        stack.pop(val);
    }
}

int main() {
    LockFreeStack<int> stack;
    std::vector<std::thread> threads;

    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(worker, std::ref(stack), i);
    }

    for (auto& t : threads) t.join();

    std::cout << "Lock-Free Stack operations completed successfully.\n";
    return 0;
}
