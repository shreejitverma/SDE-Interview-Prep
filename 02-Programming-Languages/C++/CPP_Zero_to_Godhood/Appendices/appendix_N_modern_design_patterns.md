# Appendix N: MODERN DESIGN PATTERNS (C++20/23/26 Edition)

In this appendix, we revisit the classic Gang of Four (GoF) design patterns and see how modern C++ features like **Concepts, Lambdas, Variants, and Coroutines** allow us to implement them with more safety and far less boilerplate.

***

### 1. The Strategy Pattern (The Lambda Way)
Historically, the Strategy pattern required a virtual base class and multiple derived classes. In Modern C++, we can use `std::function` or C++23's `std::move_only_function` to swap behaviors at runtime without inheritance.

**Analogy**: Imagine a smartphone. You don't need a different phone to take a photo or send a text; you just "plug in" a different App (Strategy).

```cpp
#include <functional>
#include <print>

using Strategy = std::move_only_function<void()>;

class Robot {
    Strategy movement;
public:
    void set_movement(Strategy s) { movement = std::move(s); }
    void move() { movement(); }
};

int main() {
    Robot r;
    r.set_movement([]{ std::println("Flying..."); });
    r.move();
    r.set_movement([]{ std::println("Walking..."); });
    r.move();
}
```

***

### 2. The Visitor Pattern (The Variant Way)
The classic Visitor pattern is notoriously complex and "wordy." C++17's `std::variant` and `std::visit` turn this into a clean, type-safe pattern.

**Analogy**: A postman (The Visitor) delivering mail to different house types (The Variants). He doesn't need to know the architecture of the house; he just needs a specific rule for "Apartment" vs "Mansion."

```cpp
#include <variant>
#include <print>

struct Circle { double r; };
struct Square { double s; };

using Shape = std::variant<Circle, Square>;

void draw_shapes() {
    std::vector<Shape> shapes = { Circle{5.0}, Square{10.0} };

    for (const auto& s : shapes) {
        std::visit(overloaded {
            [](Circle c) { std::println("Circle area: {}", 3.14 * c.r * c.r); },
            [](Square s) { std::println("Square area: {}", s.s * s.s); }
        }, s);
    }
}
```

***

### 3. The Factory Pattern (The Metaprogramming Way)
Using `if constexpr` and variadic templates, we can build a factory that is resolved at compile-time, saving valuable nanoseconds in the hot path.

```cpp
enum class OrderType { Market, Limit };

template<OrderType T>
auto create_order() {
    if constexpr (T == OrderType::Market) return MarketOrder{};
    else return LimitOrder{};
}
```

***

