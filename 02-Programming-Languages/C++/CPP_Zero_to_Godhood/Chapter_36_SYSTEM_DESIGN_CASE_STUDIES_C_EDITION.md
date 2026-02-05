# SYSTEM DESIGN CASE STUDIES (C++ EDITION)


Solving common interview system design problems using C++ primitives.

### 10.5.1 LRU Cache
**Problem**: Design a Least Recently Used cache with O(1) get and put.
**Solution**: Combine `std::list` (ordering) and `std::unordered_map` (lookup).

```cpp
#include <list>
#include <unordered_map>
#include <iostream>

template<typename Key, typename Value>
class LRUCache {
    size_t capacity;
    std::list<std::pair<Key, Value>> items;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> lookup;

public:
    LRUCache(size_t cap) : capacity(cap) {}

    void put(Key key, Value val) {
        if (lookup.find(key) != lookup.end()) {
            // Update: Move to front, update value
            items.splice(items.begin(), items, lookup[key]);
            lookup[key]->second = val;
            return;
        }

        if (items.size() == capacity) {
            // Evict: Remove back
            lookup.erase(items.back().first);
            items.pop_back();
        }

        // Insert: Push front
        items.emplace_front(key, val);
        lookup[key] = items.begin();
    }

    std::optional<Value> get(Key key) {
        if (lookup.find(key) == lookup.end()) return std::nullopt;
        // Access: Move to front
        items.splice(items.begin(), items, lookup[key]);
        return lookup[key]->second;
    }
};
```

### 10.5.2 Token Bucket Rate Limiter
**Problem**: Limit requests to N per second.
**Solution**: Refill tokens based on time elapsed.

```cpp
#include <chrono>
#include <mutex>

class TokenBucket {
    const long long capacity;
    const long long rate_per_sec;
    
    double tokens;
    std::chrono::steady_clock::time_point last_refill;
    std::mutex mtx;

public:
    TokenBucket(long long cap, long long rate) 
        : capacity(cap), rate_per_sec(rate), tokens(cap), 
          last_refill(std::chrono::steady_clock::now()) {}

    bool allow_request(int cost = 1) {
        std::lock_guard<std::mutex> lock(mtx);
        refill();
        
        if (tokens >= cost) {
            tokens -= cost;
            return true;
        }
        return false;
    }

private:
    void refill() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - last_refill).count();
        
        double new_tokens = (duration * rate_per_sec) / 1000000.0;
        tokens = std::min((double)capacity, tokens + new_tokens);
        last_refill = now;
    }
};
```
