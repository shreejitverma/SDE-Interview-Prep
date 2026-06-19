# CAPSTONE PROJECT - HIGH-PERFORMANCE ORDER BOOK


# CAPSTONE PROJECT - HIGH-PERFORMANCE ORDER BOOK


This capstone project integrates C++20/23 features into a realistic high-frequency trading (HFT) component. It demonstrates Modules, Concepts, Ranges, Coroutines, and modern error handling.

### Project Structure
```text
order_book/
 src/
    types.cppm        (Module: Common types)
    order.cppm        (Module: Order definition)
    book.cppm         (Module: OrderBook logic)
    main.cpp          (Entry point)
 CMakeLists.txt
 README.md
```

### 1. Types Module (types.cppm)
```cpp
export module types;

import <cstdint>;
import <compare>;

export namespace hft {
    using Price = int64_t;
    using Quantity = uint32_t;
    using OrderId = uint64_t;

    enum class Side : uint8_t { Buy, Sell };
}
```

### 2. Order Module (order.cppm)
```cpp
export module order;

import types;
import <format>;
import <string>;

export namespace hft {
    struct Order {
        OrderId id;
        Side side;
        Price price;
        Quantity quantity;

        // C++20 Spaceship for easy comparison
        auto operator<=>(const Order&) const = default;
        
        // C++23 Deducing This for generic accessors (example)
        template<typename Self>
        auto&& get_price(this Self&& self) {
            return std::forward<Self>(self).price;
        }
    };
}

// C++20 Formatter specialization
template<>
struct std::formatter<hft::Order> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    auto format(const hft::Order& o, format_context& ctx) const {
        return std::format_to(ctx.out(), "[ID:{}] {} @ {}", 
            o.id, (o.side == hft::Side::Buy ? "BUY" : "SELL"), o.price);
    }
};
```

### 3. Order Book Module (book.cppm)
```cpp
export module book;

import types;
import order;
import <vector>;
import <map>;
import <ranges>;
import <algorithm>;
import <expected>;
import <print>;
import <coroutine>;

export namespace hft {

    // C++20 Concept for Order Container
    template<typename T>
    concept OrderContainer = requires(T c) {
        c.push_back(std::declval<Order>());
        c.size();
    };

    class OrderBook {
    private:
        // Use std::flat_map (C++23) for cache locality if available, 
        // else std::map. Simulated here as vector for simplicity + ranges
        std::vector<Order> bids;
        std::vector<Order> asks;

    public:
        // C++23 std::expected for error handling
        std::expected<void, std::string> add_order(Order o) {
            if (o.quantity == 0) return std::unexpected("Invalid quantity");
            
            auto& side_vec = (o.side == Side::Buy) ? bids : asks;
            side_vec.push_back(o);
            
            // Keep sorted (simplified)
            std::ranges::sort(side_vec, {}, &Order::price);
            if (o.side == Side::Buy) std::ranges::reverse(side_vec);
            
            return {};
        }

        // C++20 Coroutine Generator to stream top orders
        // Note: Requires <generator> (C++23) or custom implementation
        // Here we simulate a simple generator pattern or use ranges
        auto top_levels(Side side, int depth) const {
            const auto& vec = (side == Side::Buy) ? bids : asks;
            return vec | std::views::take(depth);
        }

        void print_book() const {
            std::println("--- Order Book ---");
            std::println("ASKS:");
            for (const auto& o : asks | std::views::reverse) std::println("  {}", o);
            std::println("BIDS:");
            for (const auto& o : bids) std::println("  {}", o);
            std::println("------------------");
        }
    };
}
```

### 4. Main Application (main.cpp)
```cpp
import book;
import order;
import types;
import <print>;

int main() {
    hft::OrderBook book;

    book.add_order({1, hft::Side::Buy, 100, 10});
    book.add_order({2, hft::Side::Buy, 99, 5});
    book.add_order({3, hft::Side::Sell, 101, 20});
    book.add_order({4, hft::Side::Sell, 102, 15});

    book.print_book();
    
    // Demonstrate Error Handling
    if (auto res = book.add_order({5, hft::Side::Buy, 100, 0}); !res) {
        std::println(stderr, "Error adding order: {}", res.error());
    }

    return 0;
}
```

---

---


# APPENDICES


---


# APPENDICES
