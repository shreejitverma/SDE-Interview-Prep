/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Smart Pointers (std::unique_ptr, std::shared_ptr)
 * Description: Demonstrates strict ownership vs shared ownership, critical for resource management in Quant systems.
 */

#include <iostream>
#include <memory>
#include <vector>

class MarketOrder {
public:
    int id;
    double price;

    MarketOrder(int i, double p) : id(i), price(p) {
        std::cout << "Order " << id << " created.\n";
    }

    ~MarketOrder() {
        std::cout << "Order " << id << " destroyed.\n";
    }
};

void process_order(std::unique_ptr<MarketOrder> order) {
    std::cout << "Processing unique order: " << order->id << " at $" << order->price << "\n";
    // Order is destroyed here as unique_ptr goes out of scope
}

int main() {
    // 1. Unique Pointer (Exclusive Ownership)
    // Fast, lightweight, zero overhead over raw pointer.
    std::unique_ptr<MarketOrder> order1 = std::make_unique<MarketOrder>(101, 150.50);
    
    // process_order(order1); // Error: Cannot copy unique_ptr
    process_order(std::move(order1)); // OK: Ownership transferred
    
    if (!order1) {
        std::cout << "Order1 pointer is now empty.\n";
    }

    // 2. Shared Pointer (Reference Counted)
    // Thread-safe reference counting. Slightly slower due to atomic operations.
    std::shared_ptr<MarketOrder> order2 = std::make_shared<MarketOrder>(102, 200.00);
    {
        std::shared_ptr<MarketOrder> order2_copy = order2;
        std::cout << "Order2 ref count: " << order2.use_count() << "\n"; // Should be 2
    } // order2_copy destroyed, ref count drops to 1
    
    std::cout << "Order2 ref count after scope: " << order2.use_count() << "\n";

    return 0;
}
