#include <list>
#include <unordered_map>
#include <utility>

using namespace std;

/*
 * Problem: LeetCode 146 - LRU Cache
 * Difficulty: Medium
 * Concepts: Hash Table, Linked List, Design, Doubly-Linked List
 *
 * Time Complexity: O(1) for both get and put
 * Space Complexity: O(capacity)
 */

class LRUCache {
public:
    LRUCache(int capacity) : capacity_(capacity) {
    }

    int get(int key) {
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            return -1;
        }

        // Move the accessed item to the front of the list (most recently used)
        items_.splice(items_.begin(), items_, it->second);
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            // Key already exists: update value and move to front
            it->second->second = value;
            items_.splice(items_.begin(), items_, it->second);
            return;
        }

        // Evict least recently used item if at capacity
        if (static_cast<int>(cache_.size()) == capacity_) {
            int lru_key = items_.back().first;
            cache_.erase(lru_key);
            items_.pop_back();
        }

        // Insert new item at the front
        items_.emplace_front(key, value);
        cache_[key] = items_.begin();
    }

private:
    int capacity_;
    list<pair<int, int>> items_; // List of (key, value) pairs
    unordered_map<int, list<pair<int, int>>::iterator> cache_; // Map of key -> list iterator
};
