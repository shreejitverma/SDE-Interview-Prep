/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Memory Pool (Custom Allocator)
 * Description: A fixed-size block allocator.
 *           Standard 'new/malloc' is slow and non-deterministic (syscalls, fragmentation).
 *           In Low Latency C++, we pre-allocate a large chunk of memory and manage it manually.
 */

#include <cstddef>
#include <iostream>
#include <memory>
#include <new>
#include <utility>
#include <vector>

// Arena semantics: objects are never freed individually. Every object is destroyed
// (in reverse construction order) when the pool itself is destroyed, e.g. at end of day.
// Individual reuse would need a free list threaded through the unused slots.
template <typename T, std::size_t BlockSize = 1024>
class MemoryPool {
    // Raw, correctly aligned storage: no T is constructed until allocate() is called,
    // so T does not need a default constructor.
    struct Block {
        alignas(T) std::byte storage[sizeof(T) * BlockSize];
    };

    std::vector<std::unique_ptr<Block>> blocks;
    std::size_t current_slot = BlockSize; // Index of the next free slot in blocks.back()

public:
    MemoryPool() = default;
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    ~MemoryPool() {
        // Destroy every constructed object; the blocks are released by unique_ptr.
        for (std::size_t b = blocks.size(); b-- > 0;) {
            const std::size_t used = (b + 1 == blocks.size()) ? current_slot : BlockSize;
            for (std::size_t i = used; i-- > 0;) {
                slot(*blocks[b], i)->~T();
            }
        }
    }

    // Allocate and construct one object in O(1); allocates a new block only when full.
    template <typename... Args>
    T* allocate(Args&&... args) {
        if (current_slot >= BlockSize) {
            blocks.push_back(std::make_unique<Block>());
            current_slot = 0;
        }
        // Placement new: construct the object at a pre-allocated address.
        T* obj = ::new (static_cast<void*>(blocks.back()->storage + current_slot * sizeof(T)))
            T(std::forward<Args>(args)...);
        ++current_slot;
        return obj;
    }

private:
    static T* slot(Block& block, std::size_t i) {
        return std::launder(reinterpret_cast<T*>(block.storage + i * sizeof(T)));
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
    std::cout << "Order 2 Price: " << o2->price << "\n";
    
    return 0;
}
