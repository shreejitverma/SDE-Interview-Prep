---
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Quant Dev Common Patterns

---

## C++ Interview Patterns

### 1. Rule of Five
When you define any of: destructor, copy constructor, copy assignment, move constructor, move assignment - you should define ALL five.

### 2. RAII (Resource Acquisition Is Initialization)
Tie resource lifetime to object lifetime. Use smart pointers. Never use raw `new`/`delete` in production code.

### 3. CRTP (Curiously Recurring Template Pattern)
Static polymorphism. `class Derived : public Base<Derived>`. Zero virtual dispatch overhead.

### 4. Template Metaprogramming Patterns
- **SFINAE:** `enable_if`, `is_integral`, concept-like constraints pre-C++20
- **Tag dispatch:** Overload resolution via empty tag types
- **Compile-time computation:** `constexpr`, `if constexpr`

### 5. Memory Ordering Ladder
`relaxed` → `acquire/release` → `seq_cst`
- **relaxed:** Counters, statistics (no ordering guarantees)
- **acquire/release:** Producer-consumer (publish pattern)
- **seq_cst:** When in doubt (but slower)

---

## Trading System Design Patterns

### Order Book Architecture
```
Price Level (map/sorted container)
  └── Order Queue (FIFO linked list per price)
       └── Order (id, qty, timestamp)
```
- **Key:** O(1) best bid/ask, O(1) add to existing price, O(log n) new price level

### Market Data Pipeline
```
Exchange → NIC (kernel bypass) → Parser → Normalizer → Order Book → Strategy
```
- **Critical path:** Every nanosecond counts
- **Design for:** Zero allocation, zero syscall on hot path

### Risk Checks Pipeline
```
Order → Pre-trade Risk → OMS → Exchange
                ↑
        Position Manager ← Fill Notifications
```

---

## Probability Problem-Solving Framework

1. **Define the sample space** - What are all possible outcomes?
2. **Identify the event** - What are we computing the probability of?
3. **Choose technique:**
   - Direct counting / combinatorics
   - Conditional probability / Bayes
   - Indicator random variables (for expectations)
   - Recursion / Markov chains
   - Symmetry arguments
4. **Sanity check:** Does the answer make sense at boundaries?
