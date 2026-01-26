/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Memory Pool (Custom Allocator)
 * Description: A fixed-size block allocator.
 *           Standard 'new/malloc' is slow and non-deterministic (syscalls, fragmentation).
 *           In Low Latency C++, we pre-allocate a large chunk of memory and manage it manually.
 */

#include <iostream>
#include <vector>
#include <cassert>

template <typename T, size_t BlockSize = 1024>
class MemoryPool {
    struct Block {
        T data[BlockSize];
    };

    std::vector<Block*> pools;
    T* free_ptr = nullptr;     // Pointer to the next free slot
    size_t current_slot = 0;   // Index in the current block

public:
    MemoryPool() {
        allocateBlock();
    }

    ~MemoryPool() {
        for (auto ptr : pools) {
            delete ptr; // Cleanup
        }
    }

    // Allocate 1 object (O(1) time)
    template <typename... Args>
    T* allocate(Args&&... args) {
        if (current_slot >= BlockSize) {
            allocateBlock();
        }
        
        // Placement New: Construct object at specific memory address
        T* obj = new (&pools.back()->data[current_slot]) T(std::forward<Args>(args)...);
        current_slot++;
        return obj;
    }

    void deallocate(T* ptr) {
        // In a simple Linear/Arena allocator, we usually don't support individual deallocation.
        // We free the entire pool at the end (e.g., end of a trading day).
        // For individual deallocation, we'd need a "Free List".
    }

private:
    void allocateBlock() {
        pools.push_back(new Block());
        current_slot = 0;
    }
};

struct Order {
    int id;
    double price;
    Order(int i, double p) : id(i), price(p) {
        std::cout << "Order " << id << " Constructed.\n";
    }
};

int main() {
    MemoryPool<Order> pool;

    // Fast Allocation (No malloc syscalls after initial setup)
    Order* o1 = pool.allocate(1, 100.50);
    Order* o2 = pool.allocate(2, 101.00);

    std::cout << "Order 1 Price: " << o1->price << "\n";
    
    return 0;
}
