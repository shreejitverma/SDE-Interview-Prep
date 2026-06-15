# CHAPTER 66: CAPSTONE PROJECT - HIGH-PERFORMANCE ORDER BOOK


# CAPSTONE: HIGH-PERFORMANCE HFT ORDER BOOK

In this final chapter, we synthesize everything from Volume 01 to Volume 08 to build a production-grade, low-latency Limit Order Book (LOB). This project demonstrates the "Godhood" level of C++ engineering: zero-allocation during the hot path, cache-friendly data structures, and hardware-sympathetic design.

### 1. Architectural Principles
*   **Zero Dynamic Allocation**: All memory for orders and levels is pre-allocated at startup using custom pool allocators or `std::pmr`.
*   **Cache Locality**: Using `std::vector` or fixed-size arrays for price levels to ensure contiguous memory access.
*   **Mechanical Sympathy**: Using `alignas(64)` to prevent **False Sharing** between threads (e.g., between the matching engine and the gateway).
*   **Lock-Free Hot Path**: Using SPSC (Single Producer Single Consumer) ring buffers for order entry to minimize synchronization overhead.

### 2. Implementation: The Matching Engine

```cpp
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <expected>
#include <print>

namespace hft {
    using OrderId = uint64_t;
    using Price = int64_t;     // Scaled integer (e.g., cents * 100)
    using Quantity = uint32_t;

    enum class Side { Buy, Sell };

    struct Order {
        OrderId id;
        Side side;
        Price price;
        Quantity quantity;

        // C++20 Spaceship for easy comparison
        auto operator<=>(const Order&) const = default;
    };

    class OrderBook {
    private:
        // Use map for simplicity in this example, but in production HFT:
        // Use a fixed-size array/vector for a tight price range or a B-Tree.
        std::map<Price, std::vector<Order>, std::greater<Price>> bids;
        std::map<Price, std::vector<Order>, std::less<Price>> asks;

    public:
        // C++23 return type using std::expected
        std::expected<void, std::string> add_order(const Order& order) {
            if (order.quantity == 0) return std::unexpected("Quantity must be > 0");

            if (order.side == Side::Buy) {
                match_order(order, asks, bids);
            } else {
                match_order(order, bids, asks);
            }
            return {};
        }

    private:
        template<typename MapType1, typename MapType2>
        void match_order(Order order, MapType1& counter_party_side, MapType2& own_side) {
            auto it = counter_party_side.begin();
            while (it != counter_party_side.end() && order.quantity > 0) {
                Price top_price = it->first;
                
                // Check if price matches
                if ((order.side == Side::Buy && order.price >= top_price) ||
                    (order.side == Side::Sell && order.price <= top_price)) {
                    
                    auto& orders_at_level = it->second;
                    auto o_it = orders_at_level.begin();
                    while (o_it != orders_at_level.end() && order.quantity > 0) {
                        uint32_t match_qty = std::min(order.quantity, o_it->quantity);
                        
                        // LOG MATCH (In HFT, this would be a zero-copy callback)
                        std::println("MATCH: Order {} matched with {} for {} @ {}", 
                                     order.id, o_it->id, match_qty, top_price);

                        order.quantity -= match_qty;
                        o_it->quantity -= match_qty;

                        if (o_it->quantity == 0) {
                            o_it = orders_at_level.erase(o_it);
                        } else {
                            ++o_it;
                        }
                    }

                    if (orders_at_level.empty()) {
                        it = counter_party_side.erase(it);
                    } else {
                        ++it;
                    }
                } else {
                    break;
                }
            }

            // If remaining quantity, add to book (Passive Liquidity)
            if (order.quantity > 0) {
                own_side[order.price].push_back(order);
            }
        }

    public:
        void print_book() const {
            std::println("--- ORDER BOOK ---");
            std::println("ASKS:");
            for (const auto& [price, orders] : asks) {
                Quantity level_qty = 0;
                for (const auto& o : orders) level_qty += o.quantity;
                std::println("  {} : {}", price, level_qty);
            }
            std::println("BIDS:");
            for (const auto& [price, orders] : bids) {
                Quantity level_qty = 0;
                for (const auto& o : orders) level_qty += o.quantity;
                std::println("  {} : {}", price, level_qty);
            }
        }
    };
}

int main() {
    hft::OrderBook book;

    // Add some liquidity
    book.add_order({1, hft::Side::Sell, 105, 10});
    book.add_order({2, hft::Side::Sell, 110, 20});
    book.add_order({3, hft::Side::Buy, 100, 15});

    // Aggressive Buy Order
    std::println("Adding Aggressive Buy...");
    book.add_order({4, hft::Side::Buy, 107, 15});

    book.print_book();
    
    // Demonstrate C++23 Expected
    if (auto res = book.add_order({5, hft::Side::Buy, 100, 0}); !res) {
        std::println(stderr, "Error adding order: {}", res.error());
    }

    return 0;
}
```

### 3. Professional Note: Line-Rate Processing
In ultra-low latency systems, the matching engine often runs on an **FPGA** or a **Solarflare Onload** kernel bypass stack. The C++ code is responsible for the complex business logic (Matching, Risk Checks) while the networking is offloaded. To achieve "Godhood" speed, ensure your matching engine uses **Branch Prediction Hints** (`[[likely]]`) for the "No Match" path and avoids all virtual calls in the hot path.

### 4. Godhood Summary: The Ultimate Synthesis
You have reached the end of the roadmap. You have mastered:
1.  **Foundations**: The raw metal, memory, and pointers.
2.  **Modernity**: Move semantics, smart pointers, and zero-overhead abstractions.
3.  **The Future**: Reflection, Contracts, and Deducing `this`.
4.  **Specialization**: HFT Order Books, Lock-Free Concurrency, and Compiler Theory.

**Final Rule of C++**: The fastest code is the code that doesn't run. The second fastest is the code that respects the hardware. Go forth and master the beast.

---

# APPENDICES

---

# APPENDICES
