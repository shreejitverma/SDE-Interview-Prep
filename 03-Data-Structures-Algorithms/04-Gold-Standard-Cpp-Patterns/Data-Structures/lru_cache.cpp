/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: LRU (Least Recently Used) Cache
 * Standard: Modern C++20
 * Description: High-performance, template-specialized LRU Cache using std::list
 *              and std::unordered_map with iterator splicing for O(1) eviction and access.
 * 
 * Complexity:
 * - get(): O(1) average time, O(1) space
 * - put(): O(1) average time, O(1) space
 * - Space: O(Capacity)
 */

#include <iostream>
#include <list>
#include <unordered_map>
#include <optional>
#include <string>
#include <cassert>

template <typename Key, typename Value>
class LRUCache {
public:
    using KeyValuePair = std::pair<Key, Value>;
    using ListIterator = typename std::list<KeyValuePair>::iterator;

    explicit LRUCache(size_t capacity) : capacity_(capacity) {
        assert(capacity > 0 && "Capacity must be strictly positive");
    }

    // Returns the value associated with key, moving the element to the front (most recently used).
    std::optional<Value> get(const Key& key) {
        auto it = map_.find(key);
        if (it == map_.end()) {
            return std::nullopt;
        }
        // Splice existing node to front in O(1) time without reallocation
        items_.splice(items_.begin(), items_, it->second);
        return it->second->second;
    }

    // Inserts or updates a key-value pair, evicting the least recently used element if capacity exceeded.
    void put(const Key& key, Value value) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            // Update existing value and move to front
            it->second->second = std::move(value);
            items_.splice(items_.begin(), items_, it->second);
            return;
        }

        // Evict LRU element from back if at capacity
        if (items_.size() >= capacity_) {
            const Key& lru_key = items_.back().first;
            map_.erase(lru_key);
            items_.pop_back();
        }

        // Insert new element at front
        items_.emplace_front(key, std::move(value));
        map_[key] = items_.begin();
    }

    [[nodiscard]] size_t size() const noexcept {
        return items_.size();
    }

    [[nodiscard]] size_t capacity() const noexcept {
        return capacity_;
    }

    void clear() noexcept {
        items_.clear();
        map_.clear();
    }

private:
    size_t capacity_;
    std::list<KeyValuePair> items_;
    std::unordered_map<Key, ListIterator> map_;
};

int main() {
    LRUCache<int, std::string> cache(2);

    cache.put(1, "Alpha");
    cache.put(2, "Beta");

    assert(cache.get(1).value() == "Alpha"); // 1 becomes MRU, 2 is LRU

    cache.put(3, "Gamma"); // Evicts 2

    assert(!cache.get(2).has_value());       // 2 was evicted
    assert(cache.get(3).value() == "Gamma");
    assert(cache.get(1).value() == "Alpha");

    std::cout << "All LRU Cache tests passed successfully.\n";
    return 0;
}
